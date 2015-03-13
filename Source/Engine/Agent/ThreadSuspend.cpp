/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare licenses may use this file
 * in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/. Use the contact form
 * at http://www.nektra.com/contact/ for further information.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 3.0 as published by the Free Software Foundation
 * and appearing in the file LICENSE.GPL included in the packaging of this
 * file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
 * information to ensure the GNU General Public License version 3.0
 * requirements will be met.
 *
 **/

#include "ThreadSuspend.h"
#include "..\..\Common\DynamicAPIs.h"
#include "..\..\Common\NtInternals.h"

//-----------------------------------------------------------

#define X_ALLOCATION_SIZE                                256
#define X_CHECKTHREADS_ALLOCATION_SIZE                 65536

//-----------------------------------------------------------

#define NEXT_PROCESS(_currProc)                                                                   \
     (PNKT_DV_SYSTEM_PROCESS_INFORMATION)((LPBYTE)_currProc + (SIZE_T)(_currProc->NextEntryOffset))

//-----------------------------------------------------------

static const int aGatherMethods[3] = {
  NKT_DV_SystemProcessInformation, NKT_DV_SystemSessionProcessInformation, NKT_DV_SystemExtendedProcessInformation
};

//-----------------------------------------------------------

static __inline LPVOID MemAlloc(__in SIZE_T nSize)
{
  return nktMemMalloc(nSize);
  //PVOID lpPtr;

  //lpPtr = NULL;
  //if (FAILED(nktDvDynApis_AllocateVirtualMemory(&lpPtr, nSize, PAGE_READWRITE)))
  //  return NULL;
  //return lpPtr;
}

static __inline VOID MemFree(__in LPVOID lpPtr)
{
  nktMemFree(lpPtr);
  //nktDvDynApis_NtFreeVirtualMemory(lpPtr);
  return;
}


CNktDvThreadSuspend::CNktDvThreadSuspend() : CNktMemMgrObj()
{
  nktMemSet(&sSuspendedTids, 0, sizeof(sSuspendedTids));
  //NOTE: A deadlock may arise in the second call to EnumProcesses (the one that checks if new threads
  //      where created while suspending) if a suspended thread was in the middle of a memory allocation
  //      routine (malloc, etc.). So we use a special memory handling in this class.
  nktMemSet(&sCheckThreads, 0, sizeof(sCheckThreads));
  return;
}

CNktDvThreadSuspend::~CNktDvThreadSuspend()
{
  ResumeAll();
  if (sCheckThreads.lpMem != NULL)
    MemFree(sCheckThreads.lpMem);
  return;
}

HRESULT CNktDvThreadSuspend::SuspendAll(__in IP_RANGE *lpRanges, __in SIZE_T nRangesCount)
{
  CONTEXT sThreadCtx;
  DWORD dwCurrTid, dwCurrSessionId;
  SIZE_T i, k, nSuspendTries, nCurrIP, nEnumMethod;
  LPBYTE lpPtr;
  BOOL bGrowCheckProcessThreadsMem;
#if defined _M_IX86
  BOOL b;
#endif //_M_IX86
  HRESULT hRes;
  int nOrigPriority;

  bGrowCheckProcessThreadsMem = (sCheckThreads.nSize == 0) ? TRUE : FALSE;
#if defined _M_IX86
  lpPtr = (LPBYTE)__readfsdword(0x18); //get TEB
  dwCurrTid = *((DWORD*)(lpPtr+0x24)); //TEB.ClientId.UniqueThread
#elif defined _M_X64
  lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
  dwCurrTid = *((DWORD*)(lpPtr+0x48)); //TEB.ClientId.UniqueThread
#endif
  nktDvDynApis_GetThreadPriority((HANDLE)-2, &nOrigPriority);
  nktDvDynApis_SetThreadPriority((HANDLE)-2, THREAD_PRIORITY_TIME_CRITICAL);
  //main loop
  for (;;)
  {
    if (bGrowCheckProcessThreadsMem != FALSE)
    {
      if (GrowCheckProcessThreadsMem() == FALSE)
        return E_OUTOFMEMORY;
      bGrowCheckProcessThreadsMem = FALSE;
    }
    //----
    hRes = EnumProcessThreads(&nEnumMethod, &dwCurrSessionId, NULL, NULL);
    if (FAILED(hRes))
      break;
    //suspend all threads in the list
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].dwTid == dwCurrTid)
        continue; //skip myself
      //open thread for context and suspend
      if (FAILED(nktDvDynApis_NtOpenThread(&(sSuspendedTids.lpList[i].hThread), THREAD_GET_CONTEXT|THREAD_SUSPEND_RESUME,
                                           sSuspendedTids.lpList[i].dwTid)))
        sSuspendedTids.lpList[i].hThread = NULL;
      if (sSuspendedTids.lpList[i].hThread != NULL)
      {
        //suspend the thread
        for (nSuspendTries=20; nSuspendTries>0; nSuspendTries--)
        {
          hRes = nktDvDynApis_NtSuspendThread(sSuspendedTids.lpList[i].hThread);
          if (FAILED(hRes))
            break;
          //check thread position until it leaves our segment
          nktMemSet(&sThreadCtx, 0, sizeof(sThreadCtx));
          sThreadCtx.ContextFlags = CONTEXT_FULL;
          hRes = nktDvDynApis_NtGetContextThread(sSuspendedTids.lpList[i].hThread, &sThreadCtx);
          if (FAILED(hRes))
            break;
#if defined _M_X64
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
          nktDvDynApis_NtResumeThread(sSuspendedTids.lpList[i].hThread);
          nktDvDynApis_NtDelayExecution(1);
        }
        if (nSuspendTries == 0)
          break; //may this thread is waiting for something in another thread
      }
    }
    if (i < sSuspendedTids.nCount)
    {
      //could not send to sleep, all threads, wait a little time and try again
      if (FAILED(hRes))
        break;
      hRes = S_FALSE;
      goto sa_retry;
    }
#if defined _M_IX86
    //SuspendThread suspends the threads when leaving kernel mode by executing an APC
    //In WOW64, when the thread exits kernel mode, it usually jumps to code in wow64###.dll, which is
    //user-mode but some internal locks may be held by that dll's
    //The problem is that "NtQuerySystemInformation" may deadlock if called on this situation so,
    //although unsafe, we must skip the "detect recently new thread created code" below
    hRes = nktDvDynApis_IsWow64Process((HANDLE)-1, &b);
    if (SUCCEEDED(hRes) && b != FALSE)
      break; //running on Wow64... skip check
#endif //_M_IX86
    //if we are here, we must confirm that no new threads where created in the middle
    //of the process' suspension
    hRes = CheckProcessThreads(nEnumMethod, dwCurrSessionId);
    if (hRes == S_OK)
      break;
    if (hRes == E_FAIL)
    {
      ResumeAll();
      break;
    }
    //if threads count mismatch or no memory, retry
    NKT_ASSERT(hRes == S_FALSE || hRes == E_OUTOFMEMORY);
sa_retry:
    ResumeAll();
    nktDvDynApis_SetThreadPriority((HANDLE)-2, nOrigPriority);
    nktDvDynApis_NtDelayExecution(10); //sleep for a moment
    nktDvDynApis_SetThreadPriority((HANDLE)-2, THREAD_PRIORITY_TIME_CRITICAL);
    //----
    if (hRes == E_OUTOFMEMORY)
      bGrowCheckProcessThreadsMem = TRUE;
  }
  nktDvDynApis_SetThreadPriority((HANDLE)-2, nOrigPriority);
  return hRes;
}

VOID CNktDvThreadSuspend::ResumeAll()
{
  SIZE_T i;

  if (sSuspendedTids.lpList != NULL)
  {
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].hThread != NULL)
      {
        nktDvDynApis_NtResumeThread(sSuspendedTids.lpList[i].hThread);
        nktDvDynApis_NtClose(sSuspendedTids.lpList[i].hThread);
      }
    }
    MemFree(sSuspendedTids.lpList);
    sSuspendedTids.lpList = NULL;
  }
  sSuspendedTids.nCount = 0;
  return;
}

BOOL CNktDvThreadSuspend::CheckIfThreadIsInRange(__in SIZE_T nStart, __in SIZE_T nEnd)
{
  SIZE_T i;

  if (sSuspendedTids.lpList != NULL)
  {
    for (i=0; i<sSuspendedTids.nCount; i++)
    {
      if (sSuspendedTids.lpList[i].hThread != NULL &&
          sSuspendedTids.lpList[i].nCurrIP >= nStart &&
          sSuspendedTids.lpList[i].nCurrIP < nEnd)
        return TRUE;
    }
  }
  return FALSE;
}

HRESULT CNktDvThreadSuspend::GetProcessThreads(__out LPDWORD *lplpdwTids, __out SIZE_T *lpnTidsCount)
{
  SIZE_T nEnumMethod;
  DWORD dwSessionId;

  return EnumProcessThreads(&nEnumMethod, &dwSessionId, lplpdwTids, lpnTidsCount);
}

HRESULT CNktDvThreadSuspend::EnumProcessThreads(__out SIZE_T *lpnEnumMethod, __out LPDWORD lpdwSessionId,
                                                __out LPDWORD *lplpdwTids, __out SIZE_T *lpnTidsCount)
{
  PNKT_DV_SYSTEM_PROCESS_INFORMATION lpSysProcInfo, lpCurrProc;
  NKT_DV_SYSTEM_SESSION_PROCESS_INFORMATION sSpi;
  ULONG k, kNeeded, nSysProcInfoLen;
  SIZE_T nRealSize;
  DWORD dwCurrPid;
  PVOID lpPtr;
  HRESULT hRes;

  NKT_ASSERT(lpnEnumMethod != NULL && lpdwSessionId != NULL);
  if (lplpdwTids != NULL)
  {
    _ASSERT(lpnTidsCount != NULL);
    *lplpdwTids = NULL;
    *lpnTidsCount = 0;
  }
  else
  {
    sSuspendedTids.lpList = NULL;
    sSuspendedTids.nCount = 0;
  }
  //get current process id
#if defined _M_IX86
  lpPtr = (PVOID)__readfsdword(0x18);     //get TEB
  dwCurrPid = (DWORD)*((ULONGLONG*)((LPBYTE)lpPtr+0x20)); //TEB.ClientId.UniqueProcess
#elif defined _M_X64
  lpPtr = (PVOID)__readgsqword(0x30);     //get TEB
  dwCurrPid = (DWORD)*((ULONGLONG*)((LPBYTE)lpPtr+0x40)); //TEB.ClientId.UniqueProcess
#endif
  nRealSize = 0;
  //gather information using three methods
  lpSysProcInfo = NULL;
  for ((*lpnEnumMethod)=0; (*lpnEnumMethod)<3; (*lpnEnumMethod)++)
  {
    if ((*lpnEnumMethod) == 1)
    {
      if (GetCurrentProcessSessionId(lpdwSessionId) == FALSE)
        continue; //skip this method if cannot get session id
      sSpi.SessionId = *lpdwSessionId;
    }
    nSysProcInfoLen = 128000;
    for (;;)
    {
      lpSysProcInfo = (PNKT_DV_SYSTEM_PROCESS_INFORMATION)MemAlloc(nSysProcInfoLen);
      if (lpSysProcInfo == NULL)
        return E_OUTOFMEMORY;
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
      hRes = nktDvDynApis_NtQuerySystemInformation(aGatherMethods[*lpnEnumMethod], lpPtr, k, &kNeeded);
      if (SUCCEEDED(hRes))
        break;
      if (hRes != HRESULT_FROM_NT(NKT_DV_NTSTATUS_INFO_LENGTH_MISMATCH) &&
          hRes != HRESULT_FROM_NT(NKT_DV_NTSTATUS_BUFFER_TOO_SMALL))
      {
        MemFree(lpSysProcInfo);
        return E_FAIL;
      }
      if (kNeeded == 0)
      {
        //ThinApp may return STATUS_INFO_LENGTH_MISMATCH and needed equal to 0!!!!
        nktMemSet(lpSysProcInfo, 0, sizeof(NKT_DV_SYSTEM_PROCESS_INFORMATION));
        break;
      }
      kNeeded += 4096; //check if buffer small error and needed size less than provided (should not happen)
      if (kNeeded < nSysProcInfoLen)
        kNeeded = nSysProcInfoLen+4096;
      nSysProcInfoLen = kNeeded;
      MemFree(lpSysProcInfo);
    }
    //find the current process
    lpCurrProc = lpSysProcInfo;
    while (lpCurrProc != NULL && (DWORD)(lpCurrProc->UniqueProcessId) != dwCurrPid)
    {
      lpCurrProc = (lpCurrProc->NextEntryOffset != 0) ? NEXT_PROCESS(lpCurrProc) : NULL;
    }
    if (lpCurrProc != NULL)
      break;
    MemFree(lpSysProcInfo);
  }
  //current process not found (?), may be thin app hook of NtQuerySystemInformation
  if (lpCurrProc == NULL)
    return E_FAIL;
  if (lpCurrProc != NULL && lpCurrProc->NumberOfThreads > 0)
  {
    if (lplpdwTids != NULL)
    {
      *lpnTidsCount = (SIZE_T)(lpCurrProc->NumberOfThreads);
      //NOTE: Using nktMemMalloc here because list is not internal
      *lplpdwTids = (LPDWORD)nktMemMalloc((*lpnTidsCount)*sizeof(DWORD));
      if ((*lplpdwTids) == NULL)
      {
        MemFree(lpSysProcInfo);
        return E_OUTOFMEMORY;
      }
      //scan the threads of the process
      if ((*lpnEnumMethod) != 2)
      {
        for (k=0; k<lpCurrProc->NumberOfThreads; k++)
          (*lplpdwTids)[k] = (DWORD)(lpCurrProc->Threads[k].ClientId.UniqueThread);
      }
      else
      {
        for (k=0; k<lpCurrProc->NumberOfThreads; k++)
          (*lplpdwTids)[k] = (DWORD)(lpCurrProc->ExtThreads[k].ThreadInfo.ClientId.UniqueThread);
      }
    }
    else
    {
      sSuspendedTids.nCount = (SIZE_T)(lpCurrProc->NumberOfThreads);
      sSuspendedTids.lpList = (LPTHREAD_ITEM)MemAlloc(sSuspendedTids.nCount*sizeof(THREAD_ITEM));
      if (sSuspendedTids.lpList == NULL)
      {
        MemFree(lpSysProcInfo);
        return E_OUTOFMEMORY;
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
  }
  MemFree(lpSysProcInfo);
  return S_OK;
}

BOOL CNktDvThreadSuspend::GrowCheckProcessThreadsMem()
{
  LPBYTE lpPtr;

  lpPtr = (LPBYTE)MemAlloc(sCheckThreads.nSize+X_CHECKTHREADS_ALLOCATION_SIZE);
  if (lpPtr == NULL)
    return FALSE;
  if (sCheckThreads.lpMem != NULL)
    MemFree(sCheckThreads.lpMem);
  sCheckThreads.lpMem = lpPtr;
  sCheckThreads.nSize += X_CHECKTHREADS_ALLOCATION_SIZE;
  return TRUE;
}

HRESULT CNktDvThreadSuspend::CheckProcessThreads(__in SIZE_T nEnumMethod, __in DWORD dwSessionId)
{
  DWORD dwCurrPid;
  PNKT_DV_SYSTEM_PROCESS_INFORMATION lpSysProcInfo, lpCurrProc;
  NKT_DV_SYSTEM_SESSION_PROCESS_INFORMATION sSpi;
  SIZE_T i;
  ULONG k, kNeeded;
  PVOID lpPtr;
  HRESULT hRes;

  NKT_ASSERT(nEnumMethod >= 0 && nEnumMethod <= 2);
  //get current process id
#if defined _M_IX86
  lpPtr = (PVOID)__readfsdword(0x18);     //get TEB
  dwCurrPid = (DWORD)*((ULONGLONG*)((LPBYTE)lpPtr+0x20)); //TEB.ClientId.UniqueProcess
#elif defined _M_X64
  lpPtr = (PVOID)__readgsqword(0x30);     //get TEB
  dwCurrPid = (DWORD)*((ULONGLONG*)((LPBYTE)lpPtr+0x40)); //TEB.ClientId.UniqueProcess
#endif
  //gather information
  lpSysProcInfo = (PNKT_DV_SYSTEM_PROCESS_INFORMATION)(sCheckThreads.lpMem);
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
  hRes = nktDvDynApis_NtQuerySystemInformation(aGatherMethods[nEnumMethod], lpPtr, k, &kNeeded);
  if (FAILED(hRes))
  {
    return (hRes == HRESULT_FROM_NT(NKT_DV_NTSTATUS_INFO_LENGTH_MISMATCH) ||
            hRes == HRESULT_FROM_NT(NKT_DV_NTSTATUS_BUFFER_TOO_SMALL)) ? E_OUTOFMEMORY : E_FAIL;
  }
  //find the current process
  lpCurrProc = lpSysProcInfo;
  while (lpCurrProc != NULL && (DWORD)(lpCurrProc->UniqueProcessId) != dwCurrPid)
  {
    lpCurrProc = (lpCurrProc->NextEntryOffset != 0) ? NEXT_PROCESS(lpCurrProc) : NULL;
  }
  //current process not found (?) grrrr
  if (lpCurrProc == NULL)
    return E_FAIL;
  if (sSuspendedTids.nCount != (SIZE_T)(lpCurrProc->NumberOfThreads))
    return S_FALSE;
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
      return S_FALSE;
  }
  //done
  return S_OK;
}

BOOL CNktDvThreadSuspend::GetCurrentProcessSessionId(__out LPDWORD lpdwSessionId)
{
  NKT_DV_PROCESS_SESSION_INFORMATION sSi;
  HRESULT hRes;

  if (lpdwSessionId != NULL)
  {
    hRes = nktDvDynApis_NtQueryInformationProcess((HANDLE)-1, NKT_DV_ProcessSessionInformation, &sSi, sizeof(sSi),
                                                  NULL);
    if (SUCCEEDED(hRes))
    {
      *lpdwSessionId = sSi.SessionId;
      return TRUE;
    }
  }
  return FALSE;
}
