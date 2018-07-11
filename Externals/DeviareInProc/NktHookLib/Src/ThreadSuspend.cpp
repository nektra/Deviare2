/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#include "ThreadSuspend.h"
#include "ProcessEntry.h"
#include "DynamicNtApi.h"
#include "AutoPtr.h"

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#define X_ALLOCATION_SIZE                                256
#define X_CHECKTHREADS_ALLOCATION_SIZE                 65536

//-----------------------------------------------------------

#define NEXT_PROCESS(_currProc)                            \
          (LPNKT_HK_SYSTEM_PROCESS_INFORMATION)((LPBYTE)_currProc +(SIZE_T)(_currProc->NextEntryOffset))

//-----------------------------------------------------------

typedef struct{
  BYTE Revision;
  BYTE SubAuthorityCount;
  SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
  DWORD SubAuthority[SID_MAX_SUB_AUTHORITIES];
} NKT_SID;

//-----------------------------------------------------------

static const NKT_HK_SYSTEM_INFORMATION_CLASS aGatherMethods[3] = {
  MySystemProcessInformation, MySystemSessionProcessInformation, MySystemExtendedProcessInformation
};

//-----------------------------------------------------------

CNktThreadSuspend::CNktThreadSuspend()
{
  NktHookLibHelpers::MemSet(&sSuspendedTids, 0, sizeof(sSuspendedTids));
  //NOTE: A deadlock may arise in the second call to EnumProcesses (the one that checks if new threads
  //      where created while suspending) if a suspended thread was in the middle of a memory allocation
  //      routine (malloc, etc.). So we use a special memory handling in this class.
  NktHookLibHelpers::MemSet(&sCheckThreads, 0, sizeof(sCheckThreads));
  return;
}

CNktThreadSuspend::~CNktThreadSuspend()
{
  SIZE_T nSize;

  ResumeAll();
  //----
  if (sCheckThreads.lpMem != NULL)
  {
    nSize = 0;
    NktNtFreeVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&(sCheckThreads.lpMem), &nSize, MEM_RELEASE);
  }
  return;
}

DWORD CNktThreadSuspend::SuspendAll(__in DWORD dwPid, __in IP_RANGE *lpRanges, __in SIZE_T nRangesCount)
{
  CONTEXT sThreadCtx;
  DWORD dwCurrTid, dwCurrSessionId;
  HANDLE hThread, hProcess;
  SIZE_T i, k, nSuspendTries, nCurrIP, nEnumMethod;
  LONG nProcessorArchitecture;
  LARGE_INTEGER sLI;
  BOOL bGrowCheckProcessThreadsMem, bIsLowIlProcess;
  DWORD dwOsErr;
  int nOrigPriority;
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  NTSTATUS nNtStatus;

  if (dwPid == NktHookLibHelpers::GetCurrentProcessId())
  {
    hProcess = NKTHOOKLIB_CurrentProcess;
  }
  else
  {
    nNtStatus = CProcessesHandles::CreateHandle(dwPid, PROCESS_SUSPEND_RESUME | PROCESS_QUERY_INFORMATION, &hProcess);
    if (!NT_SUCCESS(nNtStatus))
      return NktRtlNtStatusToDosError(nNtStatus);
  }
  dwOsErr = IsCurrentProcessLowIntegrity(&bIsLowIlProcess);
  if (dwOsErr != ERROR_SUCCESS)
    return dwOsErr;
  nProcessorArchitecture = NktHookLibHelpers::GetProcessorArchitecture();
  if (nProcessorArchitecture < 0)
    return ERROR_NOT_SUPPORTED;
  bGrowCheckProcessThreadsMem = (sCheckThreads.nSize == 0) ? TRUE : FALSE;
  dwCurrTid = NktHookLibHelpers::GetCurrentThreadId();
  NktHookLibHelpers::GetThreadPriority(NKTHOOKLIB_CurrentThread, &nOrigPriority);
  NktHookLibHelpers::SetThreadPriority(NKTHOOKLIB_CurrentThread, THREAD_PRIORITY_TIME_CRITICAL);
  //main loop
  for (;;)
  {
    if (bGrowCheckProcessThreadsMem != FALSE)
    {
      if (GrowCheckProcessThreadsMem() == FALSE)
      {
        if (hProcess != NKTHOOKLIB_CurrentProcess)
          NktNtClose(hProcess);
        return ERROR_NOT_ENOUGH_MEMORY;
      }
      bGrowCheckProcessThreadsMem = FALSE;
    }
    //----
    dwOsErr = EnumProcessThreads(dwPid, hProcess, &nEnumMethod, &dwCurrSessionId);
    if (dwOsErr != ERROR_SUCCESS)
    {
      //if the current process is has a low mandatory integrity level, then it may not be capable of enumerating
      //threads so assume zero threads although might be unsafe
      if (dwOsErr == ERROR_ACCESS_DENIED && bIsLowIlProcess != FALSE)
        dwOsErr = ERROR_SUCCESS;
      break;
    }
    //if no threads were found, exit loop (should never happen)
    if (sSuspendedTids.nCount == 0)
      break;
    //suspend all threads in the list
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].dwTid == dwCurrTid)
        continue; //skip myself
      //open thread for context and suspend
      sClientId.UniqueProcess = 0;
      sClientId.UniqueThread = (SIZE_T)(ULONG_PTR)(sSuspendedTids.lpList[i].dwTid);
      InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
      nNtStatus = NktNtOpenThread(&hThread, THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME, &sObjAttr, &sClientId);
      if (NT_SUCCESS(nNtStatus))
      {
        sSuspendedTids.lpList[i].hThread = hThread;
        //suspend the thread
        for (nSuspendTries=20; nSuspendTries>0; nSuspendTries--)
        {
          NktNtSuspendThread(hThread, NULL);
          //check thread position until it leaves our segment
          NktHookLibHelpers::MemSet(&sThreadCtx, 0, sizeof(sThreadCtx));
          sThreadCtx.ContextFlags = CONTEXT_FULL;
          nNtStatus = NktNtGetContextThread(hThread, &sThreadCtx);
          if (!NT_SUCCESS(nNtStatus))
            break;
#if defined(_M_X64)
          nCurrIP = (SIZE_T)sThreadCtx.Rip;
#else //_M_X64
          nCurrIP = (SIZE_T)sThreadCtx.Eip;
#endif //_M_X64
          sSuspendedTids.lpList[i].nCurrIP = nCurrIP;
          for (k=0; k<nRangesCount; k++)
          {
            if (nCurrIP >= lpRanges[k].nStart && nCurrIP < lpRanges[k].nEnd)
              break;
          }
          if (k >= nRangesCount)
            break; //not in our range => exit loop
          //resume thread, sleep a moment and try again
          NktNtResumeThread(hThread, NULL);
          sLI.QuadPart = -10000;
          NktNtDelayExecution(FALSE, &sLI);
        }
        if (nSuspendTries == 0)
          break; //may this thread is waiting for something in another thread
      }
    }
    if (i < sSuspendedTids.nCount)
    {
      //could not send to sleep, all threads, wait a little time and try again
      dwOsErr = ERROR_NOT_READY;
      goto sa_retry;
    }
    //SuspendThread suspends the threads when leaving kernel mode by executing an APC
    //In WOW64, when the thread exits kernel mode, it usually jumps to code in wow64###.dll, which is
    //user-mode but some internal locks may be held by that dll's
    //The problem is that "NtQuerySystemInformation" may deadlock if called on this situation so,
    //although unsafe, we must skip the "detect recently new thread created code" below
    if (nProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
        nProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 ||
        nProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64)
    {
      if (NktHookLibHelpers::GetProcessPlatform(hProcess) == NKTHOOKLIB_ProcessPlatformX86)
        break; //process is running on Wow64... skip check
    }
    //if we are here, we must confirm that no new threads where created in the middle
    //of the process' suspension
    dwOsErr = CheckProcessThreads(dwPid, nEnumMethod, dwCurrSessionId);
    if (dwOsErr == ERROR_SUCCESS)
      break;
    if (dwOsErr == ERROR_ACCESS_DENIED)
    {
      ResumeAll();
      break;
    }
    //if threads count mismatch or no memory, retry
    NKT_ASSERT(dwOsErr == ERROR_NOT_READY || dwOsErr == ERROR_NOT_ENOUGH_MEMORY);
sa_retry:
    ResumeAll();
    NktHookLibHelpers::SetThreadPriority(NKTHOOKLIB_CurrentThread, nOrigPriority);
    sLI.QuadPart = -10000 * 10;
    NktNtDelayExecution(FALSE, &sLI); //sleep for a moment
    NktHookLibHelpers::SetThreadPriority(NKTHOOKLIB_CurrentThread, THREAD_PRIORITY_TIME_CRITICAL);
    //----
    if (dwOsErr == ERROR_NOT_ENOUGH_MEMORY)
      bGrowCheckProcessThreadsMem = TRUE;
  }
  NktHookLibHelpers::SetThreadPriority(NKTHOOKLIB_CurrentThread, nOrigPriority);
  if (hProcess != NKTHOOKLIB_CurrentProcess)
    NktNtClose(hProcess);
  return dwOsErr;
}

VOID CNktThreadSuspend::ResumeAll()
{
  SIZE_T i, nSize;

  if (sSuspendedTids.lpList != NULL)
  {
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].hThread != NULL)
      {
        NktNtResumeThread(sSuspendedTids.lpList[i].hThread, NULL);
        NktNtClose(sSuspendedTids.lpList[i].hThread);
      }
    }
    nSize = 0;
    NktNtFreeVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&(sSuspendedTids.lpList), &nSize, MEM_RELEASE);
    sSuspendedTids.lpList = NULL;
  }
  sSuspendedTids.nCount = 0;
  return;
}

BOOL CNktThreadSuspend::CheckIfThreadIsInRange(__in SIZE_T nStart, __in SIZE_T nEnd)
{
  SIZE_T i;

  if (sSuspendedTids.lpList != NULL)
  {
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].hThread != NULL &&
          sSuspendedTids.lpList[i].nCurrIP >= nStart && sSuspendedTids.lpList[i].nCurrIP < nEnd)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

DWORD CNktThreadSuspend::EnumProcessThreads(__in DWORD dwPid, __in HANDLE hProcess, __out SIZE_T *lpnEnumMethod,
                                            __out LPDWORD lpdwSessionId)
{
  SIZE_T nSize, nRealSize;
  LPNKT_HK_SYSTEM_PROCESS_INFORMATION lpSysProcInfo = NULL, lpCurrProc;
  NKT_HK_SYSTEM_SESSION_PROCESS_INFORMATION sSpi;
  ULONG k, kNeeded, nSysProcInfoLen;
  PVOID lpPtr;
  NTSTATUS nNtStatus;
  DWORD dwOsErr;

  NKT_ASSERT(lpnEnumMethod != NULL && lpdwSessionId != NULL);
  sSuspendedTids.lpList = NULL;
  sSuspendedTids.nCount = 0;
  nRealSize = 0;
  //gather information using three methods
  for ((*lpnEnumMethod)=0; (*lpnEnumMethod)<3; (*lpnEnumMethod)++)
  {
    if ((*lpnEnumMethod) == 1)
    {
      if (GetProcessSessionId(hProcess, lpdwSessionId) == FALSE)
        continue; //skip this method if cannot get session id
      sSpi.SessionId = *lpdwSessionId;
    }
    nSysProcInfoLen = 128000;
    NKT_ASSERT(nSysProcInfoLen >= sizeof(NKT_HK_SYSTEM_PROCESS_INFORMATION));
    for (;;)
    {
      if (lpSysProcInfo != NULL)
      {
        nSize = 0;
        NktNtFreeVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&lpSysProcInfo, &nSize, MEM_RELEASE);
        lpSysProcInfo = NULL;
      }
      nSize = (SIZE_T)nSysProcInfoLen;
      nNtStatus = NktNtAllocateVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&lpSysProcInfo, 0, &nSize,
                                             MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
      if (!NT_SUCCESS(nNtStatus))
      {
        dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
        goto done;
      }
      switch (*lpnEnumMethod)
      {
        case 1:
          sSpi.SizeOfBuf = nSysProcInfoLen;
          sSpi.Buffer = lpSysProcInfo;
          k = (ULONG)sizeof(sSpi);
          lpPtr = &sSpi;
          break;
        default:
          lpPtr = lpSysProcInfo;
          k = nSysProcInfoLen;
          break;
      }
      kNeeded = 0; //NtQuerySystemInformation seems dislike using the same var for needed and input size
      nNtStatus = NktNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)aGatherMethods[*lpnEnumMethod], lpPtr,
                                              k, &kNeeded);
      if (NT_SUCCESS(nNtStatus))
        break;
      if (nNtStatus != STATUS_INFO_LENGTH_MISMATCH && nNtStatus != STATUS_BUFFER_TOO_SMALL)
      {
        dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
        goto done;
      }
      if (kNeeded == 0)
      {
        //ThinApp may return STATUS_INFO_LENGTH_MISMATCH and needed equal to 0!!!!
        NktHookLibHelpers::MemSet(lpSysProcInfo, 0, sizeof(NKT_HK_SYSTEM_PROCESS_INFORMATION));
        break;
      }
      kNeeded += 4096; //check if buffer small error and needed size less than provided (should not happen)
      if (kNeeded < nSysProcInfoLen)
        kNeeded = nSysProcInfoLen+4096;
      nSysProcInfoLen = kNeeded;
    }
    //find the target process
    lpCurrProc = lpSysProcInfo;
    while (lpCurrProc != NULL && (DWORD)((ULONG_PTR)(lpCurrProc->UniqueProcessId)) != dwPid)
    {
      lpCurrProc = (lpCurrProc->NextEntryOffset != 0) ? NEXT_PROCESS(lpCurrProc) : NULL;
    }
    if (lpCurrProc != NULL)
      break;
  }
  //current process not found (?), may be, for e.g., ThinApp hooking apis
  if (lpCurrProc == NULL)
  {
    dwOsErr = ERROR_ACCESS_DENIED;
    goto done;
  }
  if (lpCurrProc->NumberOfThreads > 0)
  {
    sSuspendedTids.lpList = NULL;
    sSuspendedTids.nCount = (SIZE_T)(lpCurrProc->NumberOfThreads);
    nSize = sSuspendedTids.nCount * sizeof(THREAD_ITEM);
    nNtStatus = NktNtAllocateVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&(sSuspendedTids.lpList), 0, &nSize,
                                           MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (!NT_SUCCESS(nNtStatus))
    {
      dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
      goto done;
    }
    //scan the threads of the process
    if ((*lpnEnumMethod) != 2)
    {
      for (k=0; k<lpCurrProc->NumberOfThreads; k++)
      {
        sSuspendedTids.lpList[k].dwTid = (DWORD)(lpCurrProc->Threads[k].ClientId.UniqueThread);
        sSuspendedTids.lpList[k].hThread = NULL;
      }
    }
    else
    {
      for (k=0; k<lpCurrProc->NumberOfThreads; k++)
      {
        sSuspendedTids.lpList[k].dwTid = (DWORD)(lpCurrProc->ExtThreads[k].ThreadInfo.ClientId.UniqueThread);
        sSuspendedTids.lpList[k].hThread = NULL;
      }
    }
  }
  dwOsErr = ERROR_SUCCESS;

done:
  if (lpSysProcInfo != NULL)
  {
    nSize = 0;
    NktNtFreeVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&lpSysProcInfo, &nSize, MEM_RELEASE);
  }
  return dwOsErr;
}

BOOL CNktThreadSuspend::GrowCheckProcessThreadsMem()
{
  LPBYTE lpPtr;
  SIZE_T nSize;
  NTSTATUS nNtStatus;

  lpPtr = NULL;
  nSize = sCheckThreads.nSize + X_CHECKTHREADS_ALLOCATION_SIZE;
  nNtStatus = NktNtAllocateVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&lpPtr, 0, &nSize,
                                         MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (!NT_SUCCESS(nNtStatus))
    return FALSE;
  if (sCheckThreads.lpMem != NULL)
  {
    nSize = 0;
    NktNtFreeVirtualMemory(NKTHOOKLIB_CurrentProcess, (PVOID*)&(sCheckThreads.lpMem), &nSize, MEM_RELEASE);
  }
  sCheckThreads.lpMem = lpPtr;
  sCheckThreads.nSize += X_CHECKTHREADS_ALLOCATION_SIZE;
  return TRUE;
}

DWORD CNktThreadSuspend::CheckProcessThreads(__in DWORD dwPid, __in SIZE_T nEnumMethod, __in DWORD dwSessionId)
{
  LPNKT_HK_SYSTEM_PROCESS_INFORMATION lpSysProcInfo, lpCurrProc;
  NKT_HK_SYSTEM_SESSION_PROCESS_INFORMATION sSpi;
  SIZE_T i;
  ULONG k, kNeeded;
  PVOID lpPtr;
  NTSTATUS nNtStatus;

  NKT_ASSERT(nEnumMethod >= 0 && nEnumMethod <= 2);
  //gather information
  lpSysProcInfo = (LPNKT_HK_SYSTEM_PROCESS_INFORMATION)(sCheckThreads.lpMem);
  NKT_ASSERT(lpSysProcInfo != NULL);
  NKT_ASSERT(sCheckThreads.nSize != 0);
  switch (nEnumMethod)
  {
    case 1:
      sSpi.SessionId = dwSessionId;
      sSpi.SizeOfBuf = (ULONG)(sCheckThreads.nSize);
      sSpi.Buffer = lpSysProcInfo;
      k = (ULONG)sizeof(sSpi);
      lpPtr = &sSpi;
      break;
    default:
      lpPtr = lpSysProcInfo;
      k = (ULONG)(sCheckThreads.nSize);
      break;
  }
  kNeeded = 0; //NtQuerySystemInformation seems dislike using the same var for needed and input size
  nNtStatus = NktNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)aGatherMethods[nEnumMethod], lpPtr, k, &kNeeded);
  if (!NT_SUCCESS(nNtStatus))
  {
    switch (nNtStatus)
    {
      case STATUS_NOT_IMPLEMENTED:
        return ERROR_CALL_NOT_IMPLEMENTED;
      case STATUS_INFO_LENGTH_MISMATCH:
      case STATUS_BUFFER_TOO_SMALL:
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    return ERROR_ACCESS_DENIED;
  }
  //find the current process
  lpCurrProc = lpSysProcInfo;
  while (lpCurrProc != NULL && (DWORD)((ULONG_PTR)(lpCurrProc->UniqueProcessId)) != dwPid)
  {
    lpCurrProc = (lpCurrProc->NextEntryOffset != 0) ? NEXT_PROCESS(lpCurrProc) : NULL;
  }
  //current process not found (?) grrrr
  if (lpCurrProc == NULL)
    return ERROR_ACCESS_DENIED;
  if (sSuspendedTids.nCount != (SIZE_T)(lpCurrProc->NumberOfThreads))
    return ERROR_NOT_READY;
  for (i=0; i<sSuspendedTids.nCount; i++)
  {
    if (nEnumMethod != 2)
    {
      for (k=0; k<lpCurrProc->NumberOfThreads; k++)
      {
        if (sSuspendedTids.lpList[i].dwTid == (DWORD)(lpCurrProc->Threads[k].ClientId.UniqueThread))
          break;
      }
    }
    else
    {
      for (k=0; k<lpCurrProc->NumberOfThreads; k++)
      {
        if (sSuspendedTids.lpList[i].dwTid == (DWORD)(lpCurrProc->ExtThreads[k].ThreadInfo.ClientId.UniqueThread))
          break;
      }
    }
    if (k >= lpCurrProc->NumberOfThreads)
      return ERROR_NOT_READY;
  }
  //done
  return ERROR_SUCCESS;
}

BOOL CNktThreadSuspend::GetProcessSessionId(__in HANDLE hProcess, __out LPDWORD lpdwSessionId)
{
  NKT_HK_PROCESS_SESSION_INFORMATION sSi;
  NTSTATUS nNtStatus;
  ULONG k;

  NKT_ASSERT(lpdwSessionId != NULL);
  k = 0;
  nNtStatus = NktNtQueryInformationProcess(hProcess, (PROCESSINFOCLASS)MyProcessSessionInformation, &sSi,
                                           (ULONG)sizeof(sSi), &k);
  if (NT_SUCCESS(nNtStatus))
  {
    *lpdwSessionId = sSi.SessionId;
    return TRUE;
  }
  return FALSE;
}

DWORD CNktThreadSuspend::IsCurrentProcessLowIntegrity(__out BOOL *lpbProcessIsLow)
{
  TNktAutoFreePtr<TOKEN_MANDATORY_LABEL> cIntegrityLevel;
  NKT_SID *lpSid;
  DWORD dwOsVerMajor, dwIntegrityLevel;
  ULONG nRetLength;
  HANDLE hToken;
  NTSTATUS nNtStatus;

  *lpbProcessIsLow = FALSE;
  //check OS version
  if (NktHookLibHelpers::GetOsVersion(&dwOsVerMajor) == FALSE)
    return ERROR_SUCCESS;
  if (dwOsVerMajor < 6)
    return ERROR_SUCCESS; //only on Vista or later
  //open process token
  nNtStatus = NktNtOpenProcessToken(NKTHOOKLIB_CurrentProcess, TOKEN_QUERY, &hToken);
  //query for restricted sids
  if (NT_SUCCESS(nNtStatus))
  {
    //query for restricted sids
    nNtStatus = NktNtQueryInformationToken(hToken, TokenIntegrityLevel, NULL, 0, &nRetLength);
    while (nNtStatus == STATUS_BUFFER_TOO_SMALL)
    {
      cIntegrityLevel.Attach((TOKEN_MANDATORY_LABEL*)NktHookLibHelpers::MemAlloc((SIZE_T)nRetLength));
      if (cIntegrityLevel != NULL)
      {
        nNtStatus = NktNtQueryInformationToken(hToken, TokenIntegrityLevel, cIntegrityLevel.Get(), nRetLength,
                                               &nRetLength);
      }
      else
      {
        nNtStatus = STATUS_NO_MEMORY;
      }
    }
    if (NT_SUCCESS(nNtStatus))
    {
      lpSid = (NKT_SID*)(cIntegrityLevel->Label.Sid);
      dwIntegrityLevel = lpSid->SubAuthority[lpSid->SubAuthorityCount - 1];
      if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
        *lpbProcessIsLow = TRUE;
    }
    NktNtClose(hToken);
  }
  return (NT_SUCCESS(nNtStatus)) ? ERROR_SUCCESS : NktRtlNtStatusToDosError(nNtStatus);
}

//-----------------------------------------------------------

} //Internals
} //NktHookLib
