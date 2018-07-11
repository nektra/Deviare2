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

#include <NktHookLib.h>
#include "DynamicNtApi.h"
#include "WaitableObjects.h"
#include <intrin.h>

#pragma intrinsic (_InterlockedIncrement)

using namespace NktHookLib::Internals;

//-----------------------------------------------------------

#define XISALIGNED(x)  ((((SIZE_T)(x)) & (sizeof(SIZE_T)-1)) == 0)

#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

#define SystemProcessorInformation                         1

#define ThreadBasicInformation                             0
#define ThreadBasePriority                                 3

#define ProcessWow64Information         (PROCESSINFOCLASS)26

//-----------------------------------------------------------

typedef struct {
  USHORT ProcessorArchitecture;
  USHORT ProcessorLevel;
  USHORT ProcessorRevision;
  USHORT Reserved;
  ULONG ProcessorFeatureBits;
} NKT_SYSTEM_PROCESSOR_INFORMATION;

#pragma pack(8)
typedef struct {
  LONG ExitStatus;
  ULONG PebBaseAddress;
  ULONG AffinityMask;
  LONG BasePriority;
  ULONG UniqueProcessId;
  ULONG InheritedFromUniqueProcessId;
} __PROCESS_BASIC_INFORMATION32;

typedef struct {
  LONG ExitStatus;
  ULONG _dummy1;
  ULONGLONG PebBaseAddress;
  ULONGLONG AffinityMask;
  LONG BasePriority;
  ULONG _dummy2;
  ULONGLONG UniqueProcessId;
  ULONGLONG InheritedFromUniqueProcessId;
} __PROCESS_BASIC_INFORMATION64;
#pragma pack()

//-----------------------------------------------------------

namespace NktHookLibHelpers {

lpfnInternalApiResolver fnInternalApiResolver = NULL;
LPVOID lpUserParam = NULL;

} //namespace NktHookLibHelpers

extern "C" {
  SIZE_T __stdcall NktHookLib_TryMemCopy(__out const void *lpDest, __in const void *lpSrc, __in size_t nCount);
  SIZE_T __stdcall NktHookLib_TryCallOneParam(__in LPVOID lpFunc, __in SIZE_T nParam1, __in BOOL bIsCDecl);
  int NktHookLib_vsnprintf(__out_z char *lpDest, __in size_t nMaxCount, __in_z const char *szFormatA,
                           __in va_list lpArgList);
  int NktHookLib_sprintf(__out_z char *lpDest, __in_z const char *szFormatA, ...);
  extern void* volatile NktHookLib_fn_vsnwprintf;
  extern void* volatile NktHookLib_fn_DbgPrint;
};

//-----------------------------------------------------------

namespace NktHookLibHelpers {

HINSTANCE GetModuleBaseAddress(__in_z LPCWSTR szDllNameW)
{
  return (HINSTANCE)GetRemoteModuleBaseAddress(NKTHOOKLIB_CurrentProcess, szDllNameW, FALSE);
}

LPVOID GetProcedureAddress(__in HINSTANCE hDll, __in LPCSTR szProcNameA)
{
  return GetRemoteProcedureAddress(NKTHOOKLIB_CurrentProcess, hDll, szProcNameA);
}

HINSTANCE GetRemoteModuleBaseAddress(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in BOOL bScanMappedImages)
{
  return (HINSTANCE)::NktHookLib::Internals::GetRemoteModuleBaseAddress(hProcess, szDllNameW, bScanMappedImages);
}

LPVOID GetRemoteProcedureAddress(__in HANDLE hProcess, __in HINSTANCE hDll, __in_z LPCSTR szProcNameA)
{
  return ::NktHookLib::Internals::GetRemoteProcedureAddress(hProcess, (LPVOID)hDll, szProcNameA);
}

//--------------------------------

int sprintf_s(__out_z char *lpDest, __in size_t nMaxCount, __in_z const char *szFormatA, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, szFormatA);
  ret = vsnprintf(lpDest, nMaxCount, szFormatA, argptr);
  va_end(argptr);
  return ret;
}

int vsnprintf(__out_z char *lpDest, __in size_t nMaxCount, __in_z const char *szFormatA, __in va_list lpArgList)
{
  //NOTE: To simplify C <-> C++ jumping (because C usage of vsnprintf), we do a lightweight call to
  //      RtlNtStatusToDosError to ensure 'vsnprintf' routine was loaded.
  NktRtlNtStatusToDosError(0);
  return NktHookLib_vsnprintf(lpDest, nMaxCount, szFormatA, lpArgList);
}

int swprintf_s(__out_z wchar_t *lpDest, __in size_t nMaxCount, __in_z const wchar_t *szFormatW, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, szFormatW);
  ret = vsnwprintf(lpDest, nMaxCount, szFormatW, argptr);
  va_end(argptr);
  return ret;
}

int vsnwprintf(__out_z wchar_t *lpDest, __in size_t nMaxCount, __in_z const wchar_t *szFormatW, __in va_list lpArgList)
{
  typedef int (__cdecl *lpfn_vsnwprintf)(_Out_cap_(_MaxCount) wchar_t * _DstBuf, _In_ size_t _MaxCount,
                                         _In_z_ _Printf_format_string_ const wchar_t * _Format, va_list _ArgList);

  //NOTE: To simplify C <-> C++ jumping (because C usage of vsnprintf), we do a lightweight call to
  //      RtlNtStatusToDosError to ensure 'vsnwprintf' routine was loaded.
  NktRtlNtStatusToDosError(0);
  if (lpDest != NULL && nMaxCount > 0)
    *lpDest = 0;
  if (NktHookLib_fn_vsnwprintf == NULL)
    return 0;
  return ((lpfn_vsnwprintf)NktHookLib_fn_vsnwprintf)(lpDest, nMaxCount, szFormatW, lpArgList);
}

//--------------------------------

LONG GetProcessorArchitecture()
{
  static LONG volatile nProcessorArchitecture = -1;

  if (nProcessorArchitecture == -1)
  {
    NKT_SYSTEM_PROCESSOR_INFORMATION sProcInfo;
    NTSTATUS nNtStatus;

    nNtStatus = NktRtlGetNativeSystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessorInformation, &sProcInfo,
                                                  sizeof(sProcInfo), NULL);
    if (nNtStatus == STATUS_NOT_IMPLEMENTED)
    {
      nNtStatus = NktNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemProcessorInformation, &sProcInfo,
                                              sizeof(sProcInfo), NULL);
    }
    if (NT_SUCCESS(nNtStatus))
      _InterlockedExchange(&nProcessorArchitecture, (LONG)(sProcInfo.ProcessorArchitecture));
  }
  return nProcessorArchitecture;
}

HANDLE OpenProcess(__in DWORD dwDesiredAccess, __in BOOL bInheritHandle, __in DWORD dwProcessId)
{
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  HANDLE hProc;
  NTSTATUS nNtStatus;

  sClientId.UniqueProcess = (SIZE_T)(ULONG_PTR)(dwProcessId);
  sClientId.UniqueThread = 0;
  InitializeObjectAttributes(&sObjAttr, NULL, ((bInheritHandle != FALSE) ? OBJ_INHERIT : 0), NULL, NULL);
  nNtStatus = NktNtOpenProcess(&hProc, dwDesiredAccess, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
    return NULL;
  return hProc;
}

HANDLE OpenThread(__in DWORD dwDesiredAccess, __in BOOL bInheritHandle, __in DWORD dwThreadId)
{
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  HANDLE hThread;
  NTSTATUS nNtStatus;

  sClientId.UniqueProcess = 0;
  sClientId.UniqueThread = (SIZE_T)(ULONG_PTR)(dwThreadId);
  InitializeObjectAttributes(&sObjAttr, NULL, ((bInheritHandle != FALSE) ? OBJ_INHERIT : 0), NULL, NULL);
  nNtStatus = NktNtOpenThread(&hThread, dwDesiredAccess, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
    return NULL;
  return hThread;
}

NTSTATUS GetProcessPlatform(__in HANDLE hProcess)
{
  ULONG_PTR nWow64;
  NTSTATUS nNtStatus;

  if (hProcess == NKTHOOKLIB_CurrentProcess)
  {
#if defined(_M_IX86)
    return NKTHOOKLIB_ProcessPlatformX86;
#elif defined(_M_X64)
    return NKTHOOKLIB_ProcessPlatformX64;
#endif
  }
  switch (GetProcessorArchitecture())
  {
    case PROCESSOR_ARCHITECTURE_INTEL:
      return NKTHOOKLIB_ProcessPlatformX86;

    case PROCESSOR_ARCHITECTURE_AMD64:
      //check on 64-bit platforms
      nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessWow64Information, &nWow64, sizeof(nWow64), NULL);
      if (NT_SUCCESS(nNtStatus))
      {
#if defined(_M_IX86)
        return (nWow64 == 0) ? NKTHOOKLIB_ProcessPlatformX64 : NKTHOOKLIB_ProcessPlatformX86;
#elif defined(_M_X64)
        return (nWow64 != 0) ? NKTHOOKLIB_ProcessPlatformX86 : NKTHOOKLIB_ProcessPlatformX64;
#endif
      }
#if defined(_M_IX86)
      return NKTHOOKLIB_ProcessPlatformX86;
#elif defined(_M_X64)
      return NKTHOOKLIB_ProcessPlatformX64;
#endif
      break;
    //case PROCESSOR_ARCHITECTURE_IA64:
    //case PROCESSOR_ARCHITECTURE_ALPHA64:
  }
  return STATUS_NOT_SUPPORTED;
}

SIZE_T ReadMem(__in HANDLE hProcess, __in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nBytesCount)
{
  NTSTATUS nStatus;
  SIZE_T nReaded;

  if (nBytesCount == 0)
    return 0;
  if (hProcess == NKTHOOKLIB_CurrentProcess)
    return TryMemCopy(lpDest, lpSrc, nBytesCount);
  nStatus = NktNtReadVirtualMemory(hProcess, lpSrc, lpDest, nBytesCount, &nReaded);
  if (nStatus == STATUS_PARTIAL_COPY)
    return nReaded;
  return (NT_SUCCESS(nStatus)) ? nBytesCount : 0;
}

BOOL WriteMem(__in HANDLE hProcess, __in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nBytesCount)
{
  NTSTATUS nStatus;
  SIZE_T nWritten;

  if (nBytesCount == 0)
    return TRUE;
  if (hProcess == NKTHOOKLIB_CurrentProcess)
    return (TryMemCopy(lpDest, lpSrc, nBytesCount) == nBytesCount) ? TRUE : FALSE;
  nStatus = NktNtWriteVirtualMemory(hProcess, lpDest, lpSrc, nBytesCount, &nWritten);
  return (NT_SUCCESS(nStatus) ||
          (nStatus == STATUS_PARTIAL_COPY && nWritten == nBytesCount)) ? TRUE : FALSE;
}

NTSTATUS GetThreadPriority(__in HANDLE hThread, __out int *lpnPriority)
{
  NKT_HK_THREAD_BASIC_INFORMATION sTbi;
  NTSTATUS nNtStatus;

  nNtStatus = NktNtQueryInformationThread(hThread, (THREADINFOCLASS)ThreadBasicInformation, &sTbi, sizeof(sTbi), NULL);
  if (NT_SUCCESS(nNtStatus))
  {
    if (sTbi.BasePriority == THREAD_BASE_PRIORITY_LOWRT+1)
      *lpnPriority = (int)THREAD_PRIORITY_TIME_CRITICAL;
    else if (sTbi.BasePriority == THREAD_BASE_PRIORITY_IDLE-1)
      *lpnPriority = (int)THREAD_PRIORITY_IDLE;
    else
      *lpnPriority = (int)(sTbi.BasePriority);
  }
  return nNtStatus;
}

NTSTATUS SetThreadPriority(__in HANDLE hThread, __in int _nPriority)
{
  LONG nPriority = _nPriority;
 
  if (nPriority == THREAD_PRIORITY_TIME_CRITICAL)
    nPriority = THREAD_BASE_PRIORITY_LOWRT + 1;
  else if (nPriority == THREAD_PRIORITY_IDLE)
    nPriority = THREAD_BASE_PRIORITY_IDLE - 1;
  return NktNtSetInformationThread(hThread, (THREADINFOCLASS)ThreadBasePriority, &nPriority, sizeof(nPriority));
}

DWORD GetCurrentThreadId()
{
  LPBYTE lpPtr;
  DWORD dw;

#if defined(_M_IX86)
  lpPtr = (LPBYTE)__readfsdword(0x18); //get TEB
  dw = *((DWORD*)(lpPtr+0x24));        //TEB.ClientId.UniqueThread
#elif defined(_M_X64)
  lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
  dw = *((DWORD*)(lpPtr+0x48));        //TEB.ClientId.UniqueThread
#endif
  return dw;
}

DWORD GetCurrentProcessId()
{
  LPBYTE lpPtr;
  DWORD dw;

#if defined(_M_IX86)
  lpPtr = (LPBYTE)__readfsdword(0x18);     //get TEB
  dw = (DWORD)*((ULONGLONG*)(lpPtr+0x20)); //TEB.ClientId.UniqueProcess
#elif defined(_M_X64)
  lpPtr = (LPBYTE)__readgsqword(0x30);     //get TEB
  dw = (DWORD)*((ULONGLONG*)(lpPtr+0x40)); //TEB.ClientId.UniqueProcess
#endif
  return dw;
}

HANDLE GetProcessHeap()
{
  LPBYTE lpPtr;
  HANDLE h;

#if defined(_M_IX86)
  lpPtr = (LPBYTE)__readfsdword(0x18); //get TEB
  lpPtr = *((LPBYTE*)(lpPtr+0x30));    //TEB.Peb
  h = *((HANDLE*)(lpPtr+0x18));        //PEB.ProcessHeap
#elif defined(_M_X64)
  lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
  lpPtr = *((LPBYTE*)(lpPtr+0x60));    //TEB.Peb
  h = *((HANDLE*)(lpPtr+0x30));        //PEB.ProcessHeap
#endif
  return h;
}

LPVOID MemAlloc(__in SIZE_T nSize)
{
  if (nSize == 0)
    nSize = 1;
  return NktRtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, nSize);
}

VOID MemFree(__in LPVOID lpPtr)
{
  if (lpPtr != NULL)
    NktRtlFreeHeap(GetProcessHeap(), 0, lpPtr);
  return;
}

VOID MemSet(__out void *lpDest, __in int nVal, __in SIZE_T nCount)
{
  LPBYTE d;
  SIZE_T n;

  d = (LPBYTE)lpDest;
  nVal &= 0xFF;
  if (XISALIGNED(d))
  {
    n = ((SIZE_T)nVal) | (((SIZE_T)nVal) << 8);
    n = n | (n << 16);
#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
    n = n | (n << 32);
#endif //_M_X64 || _M_IA64 || _M_AMD64
    while (nCount >= sizeof(SIZE_T))
    {
      *((SIZE_T*)d) = n;
      d += sizeof(SIZE_T);
      nCount -= sizeof(SIZE_T);
    }
  }
  //the following code is not fully optimized on purpose just but avoid VC compiler to insert undesired "_memset" calls
  if (nCount > 0)
  {
    do
    {
      *d++ = (BYTE)nVal;
    }
    while (--nCount > 0);
  }
  return;
}

VOID MemCopy(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  LPBYTE s, d;

  s = (LPBYTE)lpSrc;
  d = (LPBYTE)lpDest;
  if (XISALIGNED(s) && XISALIGNED(d))
  {
    while (nCount >= sizeof(SIZE_T))
    {
      *((SIZE_T*)d) = *((SIZE_T*)s);
      s += sizeof(SIZE_T);
      d += sizeof(SIZE_T);
      nCount -= sizeof(SIZE_T);
    }
  }
  while (nCount > 0)
  {
    *d++ = *s++;
    nCount--;
  }
  return;
}

VOID MemMove(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  LPBYTE s, d;

  s = (LPBYTE)lpSrc;
  d = (LPBYTE)lpDest;
  if (d <= s || d >= (s+nCount))
  {
    //dest is before source or non-overlapping buffers
    //copy from lower to higher addresses
    if (d+sizeof(SIZE_T) <= s && XISALIGNED(s) && XISALIGNED(d))
    {
      while (nCount >= sizeof(SIZE_T))
      {
        *((SIZE_T*)d) = *((SIZE_T*)s);
        s += sizeof(SIZE_T);
        d += sizeof(SIZE_T);
        nCount -= sizeof(SIZE_T);
      }
    }
    while ((nCount--) > 0)
      *d++ = *s++;
  }
  else
  {
    //dest is past source or overlapping buffers
    //copy from higher to lower addresses
    if (nCount >= sizeof(SIZE_T) && XISALIGNED(s) && XISALIGNED(d))
    {
      s += nCount;
      d += nCount;
      while (nCount>0 && (!XISALIGNED(nCount))) {
        --s;
        --d;
        *d = *s;
        nCount--;
      }
      while (nCount > 0)
      {
        s -= sizeof(SIZE_T);
        d -= sizeof(SIZE_T);
        *((SIZE_T*)d) = *((SIZE_T*)s);
        nCount -= sizeof(SIZE_T);
      }
    }
    else
    {
      s += nCount;
      d += nCount;
      while (nCount > 0)
      {
        --s;
        --d;
        *d = *s;
        nCount--;
      }
    }
  }
  return;
}

SIZE_T TryMemCopy(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  return NktHookLib_TryMemCopy(lpDest, lpSrc, nCount);
}

int MemCompare(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount)
{
  LPBYTE b1, b2;

  if (nCount == 0)
    return 0;
  b1 = (LPBYTE)lpBuf1;
  b2 = (LPBYTE)lpBuf2;
  while ((--nCount) > 0 && b1[0] == b2[0])
  {
    b1++;
    b2++;
  }
  return (int)(b1[0] - b2[0]);
}

//--------------------------------

VOID DebugPrint(__in LPCSTR szFormatA, ...)
{
  va_list argptr;

  va_start(argptr, szFormatA);
  DebugVPrint(szFormatA, argptr);
  va_end(argptr);
  return;
}

VOID DebugVPrint(__in LPCSTR szFormatA, __in va_list argptr)
{
  typedef int (__cdecl *lpfnDbgPrint)(char *Format, ...);
  CHAR szTempA[2048];
  EXCEPTION_RECORD sExcRec;
  SIZE_T i;

  i = (SIZE_T)vsnprintf(szTempA, 2047, szFormatA, argptr);
  szTempA[2047] = 0;
  if (i > 2047)
    i = 2047;
  if (NktHookLib_fn_DbgPrint != NULL)
  {
    ((lpfnDbgPrint)NktHookLib_fn_DbgPrint)("%s", szTempA);
  }
  else
  {
    MemSet(&sExcRec, 0, sizeof(sExcRec));
    sExcRec.ExceptionCode = DBG_PRINTEXCEPTION_C;
    sExcRec.NumberParameters = 2;
    sExcRec.ExceptionInformation[0] = (ULONG_PTR)(i+1); //include end of string
    sExcRec.ExceptionInformation[1] = (ULONG_PTR)szTempA;
    sExcRec.ExceptionAddress = (PVOID)NktRtlRaiseException;
    //avoid compiler stuff for try/except blocks
    NktHookLib_TryCallOneParam(NktRtlRaiseException, (SIZE_T)&sExcRec, FALSE);
  }
  return;
}

//--------------------------------

VOID SetInternalApiResolverCallback(__in lpfnInternalApiResolver _fnInternalApiResolver, __in LPVOID _lpUserParam)
{
  fnInternalApiResolver = _fnInternalApiResolver;
  lpUserParam = _lpUserParam;
  return;
}

//--------------------------------

DWORD GetWin32LastError(__in_opt HANDLE hThread)
{
  NKT_HK_THREAD_BASIC_INFORMATION sTbi;
  LPBYTE lpPtr;
  DWORD dwErr;
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  HANDLE hProc = NULL;
  NTSTATUS nNtStatus;

  if (hThread == NULL || hThread == NKTHOOKLIB_CurrentThread)
  {
#if defined(_M_IX86)
    lpPtr = (LPBYTE)__readfsdword(0x18); //get TEB
    return *((DWORD*)(lpPtr+0x34));        //TEB.LastErrorValue
#elif defined(_M_X64)
    lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
    return *((DWORD*)(lpPtr+0x68));        //TEB.LastErrorValue
#endif
  }
  //not current thread
  nNtStatus = NktNtQueryInformationThread(hThread, (THREADINFOCLASS)ThreadBasicInformation, &sTbi, sizeof(sTbi), NULL);
  if (!NT_SUCCESS(nNtStatus))
    return 0xFFFFFFFFUL;
  sClientId.UniqueProcess = sTbi.ClientId.UniqueProcess;
  sClientId.UniqueThread = 0;
  InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
  nNtStatus = NktNtOpenProcess(&hProc, PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
    nNtStatus = NktNtOpenProcess(&hProc, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
    return 0xFFFFFFFFUL;
  switch (GetProcessPlatform(hProc))
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      if (ReadMem(hProc, &dwErr, (LPBYTE)sTbi.TebBaseAddress + 0x34, sizeof(DWORD)) != sizeof(DWORD))
        dwErr = 0xFFFFFFFFUL;
      break;
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
      if (ReadMem(hProc, &dwErr, (LPBYTE)sTbi.TebBaseAddress + 0x68, sizeof(DWORD)) != sizeof(DWORD))
        dwErr = 0xFFFFFFFFUL;
      break;
#endif //_M_X64
    default:
      dwErr = 0xFFFFFFFFUL;
      break;
  }
  NktNtClose(hProc);
  return dwErr;
}

BOOL SetWin32LastError(__in DWORD dwErrorCode, __in_opt HANDLE hThread)
{
  NKT_HK_THREAD_BASIC_INFORMATION sTbi;
  LPBYTE lpPtr;
  NTSTATUS nNtStatus;
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  HANDLE hProc = NULL;
  BOOL b;

  if (hThread == NULL || hThread == NKTHOOKLIB_CurrentThread)
  {
#if defined(_M_IX86)
    lpPtr = (LPBYTE)__readfsdword(0x18);        //get TEB
    *((DWORD*)(lpPtr+0x34)) = dwErrorCode; //TEB.LastErrorValue
#elif defined(_M_X64)
    lpPtr = (LPBYTE)__readgsqword(0x30);        //get TEB
    *((DWORD*)(lpPtr+0x68)) = dwErrorCode; //TEB.LastErrorValue
#endif
    return TRUE;
  }
  //not current thread
  nNtStatus = NktNtQueryInformationThread(hThread, (THREADINFOCLASS)ThreadBasicInformation, &sTbi, sizeof(sTbi), NULL);
  if (!NT_SUCCESS(nNtStatus))
  {
    SetWin32LastError(NktRtlNtStatusToDosError(nNtStatus));
    return FALSE;
  }
  sClientId.UniqueProcess = sTbi.ClientId.UniqueProcess;
  sClientId.UniqueThread = 0;
  InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
  nNtStatus = NktNtOpenProcess(&hProc, PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_WRITE, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
    nNtStatus = NktNtOpenProcess(&hProc, PROCESS_QUERY_INFORMATION | PROCESS_VM_WRITE, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
  {
    SetWin32LastError(NktRtlNtStatusToDosError(nNtStatus));
    return FALSE;
  }
  switch (GetProcessPlatform(hProc))
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      b = WriteMem(hProc, (LPBYTE)sTbi.TebBaseAddress + 0x34, &dwErrorCode, sizeof(DWORD));
      break;
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
      b = WriteMem(hProc, (LPBYTE)sTbi.TebBaseAddress + 0x68, &dwErrorCode, sizeof(DWORD));
      break;
#endif //_M_X64
  }
  NktNtClose(hProc);
  SetWin32LastError(NktRtlNtStatusToDosError((b != FALSE) ? STATUS_SUCCESS : STATUS_ACCESS_DENIED));
  return b;
}

//--------------------------------

BOOL GetOsVersion(__out_opt LPDWORD lpdwVerMajor, __out_opt LPDWORD lpdwVerMinor, __out_opt LPDWORD lpdwBuildNumber)
{
  static DWORD dwVersion[3] = { 0xFFFFFFFFUL, 0, 0 };
  static LONG volatile nMutex = 0;

  if (dwVersion[0] == 0xFFFFFFFFUL)
  {
    CNktSimpleLockNonReentrant cLock(&nMutex);
    RTL_OSVERSIONINFOW sOviW;

    if (dwVersion[0] == 0xFFFFFFFFUL)
    {
      //get OS version
      MemSet(&sOviW, 0, sizeof(sOviW));
      sOviW.dwOSVersionInfoSize = sizeof(sOviW);
      if (!NT_SUCCESS(NktRtlGetVersion(&sOviW)))
      {
        if (lpdwVerMajor != NULL)
          *lpdwVerMajor = 0;
        if (lpdwVerMinor != NULL)
          *lpdwVerMinor = 0;
        if (lpdwBuildNumber != NULL)
          *lpdwBuildNumber = 0;
        return FALSE;
      }
      if (sOviW.dwPlatformId != VER_PLATFORM_WIN32_NT)
        return FALSE;
      dwVersion[2] = sOviW.dwBuildNumber;
      dwVersion[1] = sOviW.dwMinorVersion;
      dwVersion[0] = sOviW.dwMajorVersion;
    }
  }
  if (lpdwVerMajor != NULL)
    *lpdwVerMajor = dwVersion[0];
  if (lpdwVerMinor != NULL)
    *lpdwVerMinor = dwVersion[1];
  if (lpdwBuildNumber != NULL)
    *lpdwBuildNumber = dwVersion[2];
  return TRUE;
}

} //namespace NktHookLibHelpers
