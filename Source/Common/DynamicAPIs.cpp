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

#include "EngBaseObj.h"
#include "DynamicAPIs.h"
#include "NtInternals.h"
#ifdef AGENTDLL
  #include "..\SupportLibs\UDis86\GetInstructionLength.h"
#endif //AGENTDLL

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#ifdef AGENTDLL
  #if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
    #define NKT_UNALIGNED __unaligned
  #else
    #define NKT_UNALIGNED
  #endif
#endif //AGENTDLL

//-----------------------------------------------------------

#ifdef AGENTDLL
static HRESULT BuildOriginalNtCalls();
static LPVOID BuildOriginalNtCall(__in LPBYTE lpFileFuncAddr);
static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount);
#endif //AGENTDLL

//-----------------------------------------------------------

typedef BOOL (WINAPI *lpfnQueryFullProcessImageNameW)(__in HANDLE hProcess, __in DWORD dwFlags,
                                                      __out LPWSTR lpExeNameW, __inout PDWORD lpdwSize);
typedef DWORD (WINAPI *lpfnGetProcessImageFileNameW)(__in HANDLE hProcess,
                                                     __out_ecount(nSize) LPWSTR lpImageFileName,
                                                     __in DWORD nSize);
typedef BOOL (WINAPI *lpfnIsWow64Process)(__in HANDLE, __in PBOOL);
typedef VOID (WINAPI *lpfnGetNativeSystemInfo)(__out LPSYSTEM_INFO lpSystemInfo);
typedef LONG (WINAPI *lpfnNtQueryInformationProcess)(__in HANDLE ProcessHandle,
                                                     __in int ProcessInformationClass,
                                                     __out LPVOID ProcessInformation,
                                                     __in ULONG ProcessInformationLength,
                                                     __out_opt PULONG ReturnLength);
//NOTE: Wow64GetThreadContext is available on Vista x64 or later. for xp one can implement a code
//      similar to this: http://www.nynaeve.net/Code/GetThreadWow64Context.cpp
typedef BOOL (WINAPI *lpfnWow64GetThreadContext)(__in HANDLE hThread, __inout PWOW64_CONTEXT lpContext);
typedef BOOL (WINAPI *lpfnWow64SetThreadContext)(__in HANDLE hThread, __in CONST WOW64_CONTEXT *lpContext);
typedef DWORD (WINAPI *lpfnGetModuleFileNameExW)(__in HANDLE hProcess, __in_opt HMODULE hModule,
                                                 __out_ecount(nSize) LPWSTR lpFilename, __in DWORD nSize);
typedef LONG (WINAPI *lpfnNtQueryInformationThread)(__in HANDLE ThreadHandle, __in int ThreadInformationClass,
                                                    __out_bcount(ThreadInformationLength) PVOID ThreadInformation,
                                                    __in ULONG ThreadInformationLength, __out_opt PULONG ReturnLength);
typedef LONG (WINAPI *lpfnNtSetInformationThread)(__in HANDLE ThreadHandle, __in ULONG ThreadInformationClass,
                                                  __in PVOID ThreadInformation, __in ULONG ThreadInformationLength);
typedef LONG (WINAPI *lpfnNtQueryInformationFile)(__in HANDLE FileHandle, __out LPVOID IoStatusBlock,
                                                  __out PVOID FileInformation, __in ULONG Length,
                                                  __in int FileInformationClass);
typedef LONG (WINAPI *lpfnNtQueryObject)(__in_opt HANDLE Handle, __in int ObjectInformationClass,
                                         __out_opt LPVOID ObjectInformation,
                                         __in ULONG ObjectInformationLength,
                                         __out_opt PULONG ReturnLength);
typedef BOOL (WINAPI *lpfnEnumProcessModules)(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                              __in DWORD cb, __out LPDWORD lpcbNeeded);
typedef BOOL (WINAPI *lpfnEnumProcessModulesEx)(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                                __in DWORD cb, __out LPDWORD lpcbNeeded,
                                                __in DWORD dwFilterFlag);
typedef DWORD (WINAPI *lpfnGetProcessId)(__in HANDLE Process);
typedef LONG (WINAPI *lpfnNtQueryVirtualMemory)(__in HANDLE ProcessHandle, __in PVOID BaseAddress,
                          __in int MemoryInformationClass,
                          __out_bcount(MemoryInformationLength) PVOID MemoryInformation,
                          __in SIZE_T MemoryInformationLength, __out_opt PSIZE_T ReturnLength);
typedef LONG (__stdcall *lpfnNtSuspendThread)(__in HANDLE ThreadHandle, __out_opt PULONG PreviousSuspendCount);
typedef LONG (__stdcall *lpfnNtResumeThread)(__in HANDLE ThreadHandle, __out_opt PULONG PreviousSuspendCount);
typedef LONG (__stdcall *lpfnNtQuerySystemInformation)(__in int sic, __inout PVOID lpSI, __in ULONG uSysInfLength,
                                                       __out_opt PULONG ReturnLength);
typedef LONG (NTAPI *lpfnLdrLockLoaderLock)(__in ULONG Flags, __out_opt ULONG *Disposition,
                                            __out PVOID *Cookie);
typedef LONG (NTAPI *lpfnLdrUnlockLoaderLock)(__in ULONG Flags, __inout PVOID Cookie);
typedef LONG (NTAPI *lpfnLdrFindEntryForAddress)(__in PVOID Address, __out PVOID *lplpEntry);
typedef LONG (NTAPI *lpfnRtlEnterCriticalSection)(__inout PVOID lpRtlCritSection);
typedef BOOLEAN (NTAPI *lpfnRtlTryEnterCriticalSection)(__inout PVOID lpRtlCritSection);
typedef LONG (NTAPI *lpfnRtlLeaveCriticalSection)(__inout PVOID lpRtlCritSection);
typedef LONG (NTAPI *lpfnLdrGetProcedureAddress)(__in HMODULE ModuleHandle,
                                                 __in NKT_DV_ANSI_STRING *FunctionName, __in WORD Oridinal,
                                                 __deref_out PVOID *FunctionAddress);
typedef BOOLEAN (NTAPI *lpfnRtlDllShutdownInProgress)();

typedef BOOL (WINAPI *lpfnQueryThreadCycleTime)(__in HANDLE ThreadHandle, __out PULONG64 CycleTime);

#ifdef _M_X64
typedef PRUNTIME_FUNCTION (NTAPI *lpfnRtlLookupFunctionEntry)(__in ULONG64 ControlPc, __out PULONG64 ImageBase,
                                                              __inout_opt PNKT_DV_UNWIND_HISTORY_TABLE HistoryTable);
typedef PEXCEPTION_ROUTINE (NTAPI *lpfnRtlVirtualUnwind)(__in DWORD HandlerType, __in DWORD64 ImageBase,
                                                 __in DWORD64 ControlPc, __in PRUNTIME_FUNCTION FunctionEntry,
                                                 __inout PCONTEXT ContextRecord, __out PVOID *HandlerData,
                                                 __out PDWORD64 EstablisherFrame,
                                                 __inout_opt PNKT_DV_KNONVOLATILE_CONTEXT_POINTERS ContextPointers);
typedef VOID (NTAPI *lpfnRtlCaptureContext)(__out PCONTEXT ContextRecord);

typedef PVOID (WINAPI *lpfnAddVectoredExceptionHandler)(__in ULONG FirstHandler,
                                                        __in PVECTORED_EXCEPTION_HANDLER VectoredHandler);
typedef ULONG (WINAPI *lpfnRemoveVectoredExceptionHandler)(__in PVOID Handler);

#endif //_M_X64

typedef NTSTATUS (NTAPI *lpfnNtAllocateVirtualMemory)(__in HANDLE ProcessHandle, __inout PVOID *BaseAddress,
                                                      __in ULONG_PTR ZeroBits, __inout PSIZE_T RegionSize,
                                                      __in ULONG AllocationType, __in ULONG Protect);
typedef NTSTATUS (NTAPI *lpfnNtFreeVirtualMemory)(__in HANDLE ProcessHandle, __inout PVOID *BaseAddress,
                                                  __inout PSIZE_T RegionSize, __in ULONG FreeType);
typedef NTSTATUS (NTAPI *lpfnNtProtectVirtualMemory)(__in HANDLE ProcessHandle, __inout PVOID *UnsafeBaseAddress,
                                                     __inout SIZE_T *UnsafeNumberOfBytesToProtect,
                                                     __in ULONG NewAccessProtection,
                                                     __out PULONG UnsafeOldAccessProtection);

typedef NTSTATUS (NTAPI *lpfnNtFlushInstructionCache)(__in HANDLE ProcessHandle, __in PVOID BaseAddress,
                                                      __in ULONG NumberOfBytesToFlush);
typedef NTSTATUS (NTAPI *lpfnNtClose)(__in HANDLE Handle);
typedef NTSTATUS (NTAPI *lpfnNtCreateEvent)(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess,
                                            __in_opt POBJECT_ATTRIBUTES ObjectAttributes, __in ULONG EventType,
                                            __in BOOLEAN InitialState);
typedef NTSTATUS (NTAPI *lpfnNtOpenEvent)(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess,
                                          __in POBJECT_ATTRIBUTES ObjectAttributes);
typedef NTSTATUS (NTAPI *lpfnNtResetEvent)(__in HANDLE EventHandle, __out_opt PLONG NumberOfWaitingThreads);
typedef NTSTATUS (NTAPI *lpfnNtSetEvent)(__in HANDLE EventHandle, __out_opt PLONG NumberOfWaitingThreads);

typedef NTSTATUS (NTAPI *lpfnNtOpenThread)(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess,
                                           __in POBJECT_ATTRIBUTES ObjectAttributes, __in_opt PVOID ClientId);
typedef NTSTATUS (NTAPI *lpfnNtGetContextThread)(__in HANDLE ThreadHandle, __out PCONTEXT Context);

typedef NTSTATUS (NTAPI *lpfnNtDelayExecution)(__in BOOLEAN Alertable, __in PLARGE_INTEGER DelayInterval);

typedef NTSTATUS (NTAPI *lpfnNtDuplicateObject)(__in HANDLE SourceProcessHandle, __in HANDLE SourceHandle,
                                                __in HANDLE TargetProcessHandle, __out PHANDLE TargetHandle,
                                                __in ACCESS_MASK DesiredAccess, __in ULONG HandleAttributes,
                                                __in ULONG Options);

//-----------------------------------------------------------

static LONG volatile nInitialized = 0;
static LONG volatile nInitializedMutex = 0;
static HINSTANCE hNtDll = NULL;
static HINSTANCE hkernel32Dll = NULL;
static HINSTANCE hPsApiDll = NULL;
static lpfnQueryFullProcessImageNameW fnQueryFullProcessImageNameW = NULL;
static lpfnGetProcessImageFileNameW fnGetProcessImageFileNameW = NULL;
static lpfnIsWow64Process fnIsWow64Process = NULL;
static lpfnGetNativeSystemInfo fnGetNativeSystemInfo = NULL;
static lpfnNtQueryInformationProcess fnNtQueryInformationProcess = NULL;
static lpfnWow64GetThreadContext fnWow64GetThreadContext = NULL;
static lpfnWow64SetThreadContext fnWow64SetThreadContext = NULL;
static lpfnGetModuleFileNameExW fnGetModuleFileNameExW = NULL;
static lpfnNtQueryInformationThread fnNtQueryInformationThread = NULL;
static lpfnNtSetInformationThread fnNtSetInformationThread = NULL;
static lpfnNtQueryInformationFile fnNtQueryInformationFile = NULL;
static lpfnNtQueryObject fnNtQueryObject = NULL;
static lpfnEnumProcessModules fnEnumProcessModules = NULL;
static lpfnEnumProcessModulesEx fnEnumProcessModulesEx = NULL;
static lpfnGetProcessId fnGetProcessId = NULL;
static lpfnNtQueryVirtualMemory fnNtQueryVirtualMemory = NULL;
static lpfnNtSuspendThread fnNtSuspendThread = NULL;
static lpfnNtResumeThread fnNtResumeThread = NULL;
static lpfnNtQuerySystemInformation fnNtQuerySystemInformation = NULL;
static lpfnLdrLockLoaderLock fnLdrLockLoaderLock = NULL;
static lpfnLdrUnlockLoaderLock fnLdrUnlockLoaderLock = NULL;
static lpfnLdrFindEntryForAddress fnLdrFindEntryForAddress = NULL;
static lpfnRtlEnterCriticalSection fnRtlEnterCriticalSection = NULL;
static lpfnRtlTryEnterCriticalSection fnRtlTryEnterCriticalSection = NULL;
static lpfnRtlLeaveCriticalSection fnRtlLeaveCriticalSection = NULL;
static lpfnLdrGetProcedureAddress fnLdrGetProcedureAddress = NULL;
static lpfnRtlDllShutdownInProgress fnRtlDllShutdownInProgress = NULL;
static lpfnQueryThreadCycleTime fnQueryThreadCycleTime = NULL;
#ifdef _M_X64
static lpfnRtlLookupFunctionEntry fnRtlLookupFunctionEntry = NULL;
static lpfnRtlVirtualUnwind fnRtlVirtualUnwind = NULL;
static lpfnRtlCaptureContext fnRtlCaptureContext = NULL;
static lpfnAddVectoredExceptionHandler fnAddVectoredExceptionHandler = NULL;
static lpfnRemoveVectoredExceptionHandler fnRemoveVectoredExceptionHandler = NULL;
#endif //_M_X64
static lpfnNtAllocateVirtualMemory fnNtAllocateVirtualMemory = NULL;
static lpfnNtFreeVirtualMemory fnNtFreeVirtualMemory = NULL;
static lpfnNtProtectVirtualMemory fnNtProtectVirtualMemory = NULL;
static lpfnNtFlushInstructionCache fnNtFlushInstructionCache = NULL;
static lpfnNtClose fnNtClose = NULL;
static lpfnNtCreateEvent fnNtCreateEvent = NULL;
static lpfnNtOpenEvent fnNtOpenEvent = NULL;
static lpfnNtResetEvent fnNtResetEvent = NULL;
static lpfnNtSetEvent fnNtSetEvent = NULL;
static lpfnNtOpenThread fnNtOpenThread = NULL;
static lpfnNtGetContextThread fnNtGetContextThread = NULL;
static lpfnNtDelayExecution fnNtDelayExecution = NULL;
static lpfnNtDuplicateObject fnNtDuplicateObject = NULL;
static SYSTEM_INFO sSi = { 0 };

//-----------------------------------------------------------

static HRESULT InitializeInternals();

//-----------------------------------------------------------

HRESULT nktDvDynApis_IsWow64Process(__in HANDLE hProcess, __out LPBOOL lpbIsWow64Process)
{
  if (lpbIsWow64Process == NULL)
    return E_POINTER;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnIsWow64Process == NULL)
    return E_NOTIMPL;
  return (fnIsWow64Process(hProcess, lpbIsWow64Process) != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
}

BOOL nktDvDynApis_Is64BitProcess(__in HANDLE hProcess)
{
  BOOL bIsWow64, bRet;
  HRESULT hRes;

#if defined _M_IX86
  bRet = FALSE;
#elif defined _M_X64
  bRet = TRUE;
#endif
  if (nInitialized == 0)
  {
    hRes = InitializeInternals();
    if (FAILED(hRes))
      return bRet;
  }
  //----
  switch (sSi.wProcessorArchitecture)
  {
    case PROCESSOR_ARCHITECTURE_AMD64:
    case PROCESSOR_ARCHITECTURE_IA64:
    case PROCESSOR_ARCHITECTURE_ALPHA64:
      //check on 64-bit platforms
      if (fnIsWow64Process != NULL &&
          fnIsWow64Process(hProcess, &bIsWow64) != FALSE)
      {
#if defined _M_IX86
        if (bIsWow64 == FALSE)
          bRet = TRUE;
#elif defined _M_X64
        if (bIsWow64 != FALSE)
          bRet = FALSE;
#endif
      }
      break;
  }
  return bRet;
}

HRESULT nktDvDynApis_Wow64GetThreadContext(__in HANDLE hThread, __inout PWOW64_CONTEXT lpContext)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnWow64GetThreadContext == NULL)
    return E_NOTIMPL;
  ::SetLastError(NOERROR);
  return (fnWow64GetThreadContext(hThread, lpContext) != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
}

HRESULT nktDvDynApis_Wow64SetThreadContext(__in HANDLE hThread, __in CONST WOW64_CONTEXT *lpContext)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnWow64SetThreadContext == NULL)
    return E_NOTIMPL;
  ::SetLastError(NOERROR);
  return (fnWow64SetThreadContext(hThread, lpContext) != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
}

HRESULT nktDvDynApis_GetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnGetNativeSystemInfo == NULL)
    return E_NOTIMPL;
  fnGetNativeSystemInfo(lpSystemInfo);
  return S_OK;
}

HRESULT nktDvDynApis_QueryFullProcessImageName(__in HANDLE hProcess,
                                               __out_ecount(dwSize) LPWSTR lpExeNameW, __in DWORD dwSize)
{
  DWORD dw;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnQueryFullProcessImageNameW == NULL)
    return E_NOTIMPL;
  dw = dwSize;
  ::SetLastError(NOERROR);
  if (fnQueryFullProcessImageNameW(hProcess, 0, lpExeNameW, &dw) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  if (dw >= dwSize)
    return NKT_HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  lpExeNameW[dw] = 0;
  return S_OK;
}

HRESULT nktDvDynApis_GetProcessImageFileNameW(__in HANDLE hProcess, __out_ecount(dwSize) LPWSTR lpExeNameW,
                                              __in DWORD dwSize)
{
  DWORD dwLen;
  HRESULT hRes;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnGetProcessImageFileNameW == NULL)
    return E_NOTIMPL;
  ::SetLastError(NOERROR);
  dwLen = fnGetProcessImageFileNameW(hProcess, lpExeNameW, dwSize);
  hRes = NKT_HRESULT_FROM_LASTERROR();
  if (dwLen >= dwSize && SUCCEEDED(hRes))
    hRes = NKT_HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  if (FAILED(hRes))
    return hRes;
  lpExeNameW[dwLen] = 0;
  return S_OK;
}

HRESULT nktDvDynApis_NtQueryInformationProcess(__in HANDLE ProcessHandle, __in int ProcessInformationClass,
                                               __out LPVOID ProcessInformation, __in ULONG ProcessInformationLength,
                                               __out_opt PULONG ReturnLength)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryInformationProcess == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation,
                                          ProcessInformationLength, ReturnLength);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_GetModuleFileNameExW(__in HANDLE hProcess, __in_opt HMODULE hModule,
                                          __out_ecount(dwBufSize) LPWSTR lpFilename, __in DWORD dwBufSize)
{
  DWORD dwLen;
  HRESULT hRes;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnGetModuleFileNameExW == NULL)
    return E_NOTIMPL;
  ::SetLastError(NOERROR);
  dwLen = fnGetModuleFileNameExW(hProcess, hModule, lpFilename, dwBufSize);
  hRes = NKT_HRESULT_FROM_LASTERROR(); //may return E_HANDLE if hModule is unloaded
  if (dwLen >= dwBufSize && SUCCEEDED(hRes))
    hRes = NKT_HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
  if (FAILED(hRes))
    return hRes;
  lpFilename[dwLen] = 0;
  return S_OK;
}

HRESULT nktDvDynApis_NtQueryInformationThread(__in HANDLE ThreadHandle, __in int ThreadInformationClass,
                          __out_bcount(ThreadInformationLength) PVOID ThreadInformation,
                          __in ULONG ThreadInformationLength, __out_opt PULONG ReturnLength)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryInformationThread == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation,
                                         ThreadInformationLength, ReturnLength);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtQueryInformationFile(__in HANDLE FileHandle, __out PVOID IoStatusBlock,
                                            __out LPVOID FileInformation, __in ULONG Length,
                                            __in int FileInformationClass)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryInformationFile == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryInformationFile(FileHandle, IoStatusBlock, FileInformation, Length,
                                       FileInformationClass);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtQueryObject(__in_opt HANDLE Handle, __in int ObjectInformationClass,
                                   __out_opt LPVOID ObjectInformation, __in ULONG ObjectInformationLength,
                                   __out_opt PULONG ReturnLength)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryObject == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryObject(Handle, ObjectInformationClass, ObjectInformation, ObjectInformationLength,
                              ReturnLength);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtSuspendThread(__in HANDLE ThreadHandle)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtSuspendThread == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtSuspendThread(ThreadHandle, NULL);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtResumeThread(__in HANDLE ThreadHandle)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtResumeThread == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtResumeThread(ThreadHandle, NULL);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtQuerySystemInformation(__in int sic, __inout PVOID lpSI, __in ULONG uSysInfLength,
                                              __out_opt PULONG ReturnLength)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQuerySystemInformation == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQuerySystemInformation(sic, lpSI, uSysInfLength, ReturnLength);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_EnumProcessModules(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                        __in DWORD cb, __out LPDWORD lpcbNeeded)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnEnumProcessModules == NULL)
    return E_NOTIMPL;
  if (fnEnumProcessModules(hProcess, lphModule, cb, lpcbNeeded) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  return S_OK;
}

HRESULT nktDvDynApis_EnumProcessModulesEx(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                          __in DWORD cb, __out LPDWORD lpcbNeeded, __in DWORD dwFilterFlag)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnEnumProcessModulesEx == NULL)
    return E_NOTIMPL;
  if (fnEnumProcessModulesEx(hProcess, lphModule, cb, lpcbNeeded, dwFilterFlag) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  return S_OK;
}

HRESULT nktDvDynApis_GetProcessId(__in HANDLE hProcess, __out LPDWORD lpdwPid)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (lpdwPid == NULL)
    return E_POINTER;
  *lpdwPid = 0;
  if (fnGetProcessId != NULL)
  {
    *lpdwPid = fnGetProcessId(hProcess);
  }
  else
  {
    SIZE_T nPlatformBits;
    NKT_DV_PROCESS_BASIC_INFORMATION32 sPbi32;
#if defined _M_X64
    NKT_DV_PROCESS_BASIC_INFORMATION64 sPbi64;
#endif
    HRESULT hRes;
    ULONG k;

    nPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
    switch (nPlatformBits)
    {
      case 32:
        hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessBasicInformation, &sPbi32,
                                                      sizeof(sPbi32), &k);
        if (FAILED(hRes))
          return hRes;
        *lpdwPid = (DWORD)(sPbi32.UniqueProcessId);
        break;

#if defined _M_X64
      case 64:
        hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessBasicInformation, &sPbi64,
                                                      sizeof(sPbi64), &k);
        if (FAILED(hRes))
          return hRes;
        *lpdwPid = (DWORD)(sPbi64.UniqueProcessId);
        break;
#endif

      default:
        return E_INVALIDARG;
    }
  }
  return S_OK;
}

HRESULT nktDvDynApis_NtQueryVirtualMemory(__in HANDLE ProcessHandle, __in PVOID BaseAddress,
                                    __in int MemoryInformationClass, __out PVOID MemoryInformation,
                                    __in SIZE_T MemoryInformationLength, __out_opt PSIZE_T ReturnLength)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryVirtualMemory == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryVirtualMemory(ProcessHandle, BaseAddress, MemoryInformationClass,
                                     MemoryInformation, MemoryInformationLength, ReturnLength);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_LdrLockLoaderLock(__in ULONG Flags, __out_opt ULONG *Disposition,
                                       __out PVOID *Cookie)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnLdrLockLoaderLock == NULL)
    return E_NOTIMPL;
  nNtStatus = fnLdrLockLoaderLock(Flags, Disposition, Cookie);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_LdrUnlockLoaderLock(__in ULONG Flags, __inout PVOID Cookie)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnLdrUnlockLoaderLock == NULL)
    return E_NOTIMPL;
  nNtStatus = fnLdrUnlockLoaderLock(Flags, Cookie);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_LdrFindEntryForAddress(__in PVOID Address, __out PVOID *lplpEntry)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnLdrFindEntryForAddress == NULL)
    return E_NOTIMPL;
  __try
  {
    nNtStatus = fnLdrFindEntryForAddress(Address, lplpEntry);
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    nNtStatus = NKT_DV_NTSTATUS_UNSUCCESSFUL;
  }
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_RtlEnterCriticalSection(__inout PVOID lpRtlCritSection, __in BOOL bOnlyTry)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (bOnlyTry == FALSE)
  {
    if (fnRtlEnterCriticalSection == NULL)
      return E_NOTIMPL;
    nNtStatus = fnRtlEnterCriticalSection(lpRtlCritSection);
    return (nNtStatus >= 0) ? S_OK : HRESULT_FROM_NT(nNtStatus);
  }
  if (fnRtlTryEnterCriticalSection == NULL)
    return E_NOTIMPL;
  return (fnRtlTryEnterCriticalSection(lpRtlCritSection) != FALSE) ? S_OK : S_FALSE;
}

HRESULT nktDvDynApis_RtlLeaveCriticalSection(__inout PVOID lpRtlCritSection)
{
  LONG nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRtlLeaveCriticalSection == NULL)
    return E_NOTIMPL;
  nNtStatus = fnRtlLeaveCriticalSection(lpRtlCritSection);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_LdrGetProcedureAddress(__in HMODULE ModuleHandle, __in LPCSTR szAddressA,
                                            __out PVOID *FunctionAddress)
{
  NKT_DV_ANSI_STRING sAnsiStr, *lpAnsiStr;
  WORD wOrdinal;
  SIZE_T nLen;
  LONG nNtStatus;

  if (FunctionAddress != NULL)
    *FunctionAddress = NULL;
  if (szAddressA == NULL || FunctionAddress == NULL)
    return E_POINTER;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnLdrGetProcedureAddress == NULL)
    return E_NOTIMPL;
  if (HIWORD(szAddressA) != 0)
  {
    nLen = strlen(szAddressA) * sizeof(CHAR);
    if (nLen > 0xFFFF - sizeof(CHAR))
      nLen = 0xFFFF - sizeof(CHAR);
    sAnsiStr.Length = sAnsiStr.MaximumLength = (USHORT)nLen;
    sAnsiStr.Buffer = (LPSTR)szAddressA;
    lpAnsiStr = &sAnsiStr;
    wOrdinal = 0;
  }
  else
  {
    wOrdinal = LOWORD(szAddressA);
    lpAnsiStr = NULL;
  }
  nNtStatus = fnLdrGetProcedureAddress(ModuleHandle, lpAnsiStr, wOrdinal, FunctionAddress);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_RtlDllShutdownInProgress(__out BOOL *ShutdownInProgress)
{
  if (ShutdownInProgress == NULL)
    return E_POINTER;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRtlDllShutdownInProgress == NULL)
  {
    *ShutdownInProgress = FALSE;
    return E_NOTIMPL;
  }
  *ShutdownInProgress = (fnRtlDllShutdownInProgress() != 0) ? TRUE : FALSE;
  return S_OK;
}

HRESULT nktDvDynApis_QueryThreadCycleTime(__in HANDLE ThreadHandle, __out PULONG64 CycleTime)
{
  if (CycleTime == NULL)
    return E_POINTER;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnQueryThreadCycleTime == NULL)
  {
    *CycleTime = 0;
    return E_NOTIMPL;
  }
  if (fnQueryThreadCycleTime(ThreadHandle, CycleTime) == FALSE)
  {
    *CycleTime = 0;
    return NKT_HRESULT_FROM_LASTERROR();
  }
  return S_OK;
}

#if defined _M_X64
HRESULT nktDvDynApis_RtlLookupFunctionEntry(__out PRUNTIME_FUNCTION *lplpRet, __in ULONG64 ControlPc,
                                            __out PULONG64 ImageBase, __inout_opt LPVOID HistoryTable)
{
  PRUNTIME_FUNCTION lpRet;

  if (lplpRet == NULL)
    return E_POINTER;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRtlLookupFunctionEntry == NULL)
  {
    if (lplpRet != NULL)
      *lplpRet = NULL;
    if (ImageBase != NULL)
      *ImageBase = 0;
    return E_NOTIMPL;
  }
  lpRet = fnRtlLookupFunctionEntry(ControlPc, ImageBase, (PNKT_DV_UNWIND_HISTORY_TABLE)HistoryTable);
  if (lplpRet != NULL)
    *lplpRet = lpRet;
  return S_OK;
}

HRESULT nktDvDynApis_RtlVirtualUnwind(__out_opt PEXCEPTION_ROUTINE *lplpRet, __in DWORD HandlerType,
                                      __in DWORD64 ImageBase, __in DWORD64 ControlPc,
                                      __in PRUNTIME_FUNCTION FunctionEntry, __inout PCONTEXT ContextRecord,
                                      __out PVOID *HandlerData, __out PDWORD64 EstablisherFrame,
                                      __inout_opt LPVOID ContextPointers)
{
  PEXCEPTION_ROUTINE lpRet;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRtlVirtualUnwind == NULL)
  {
    if (lplpRet != NULL)
      *lplpRet = NULL;
    if (HandlerData != NULL)
      *HandlerData = NULL;
    if (EstablisherFrame != NULL)
      *EstablisherFrame = 0;
    return E_NOTIMPL;
  }
  lpRet = fnRtlVirtualUnwind(HandlerType, ImageBase, ControlPc, FunctionEntry, ContextRecord, HandlerData,
                             EstablisherFrame, (PNKT_DV_KNONVOLATILE_CONTEXT_POINTERS)ContextPointers);
  if (lplpRet != NULL)
    *lplpRet = lpRet;
  return S_OK;
}

HRESULT nktDvDynApis_RtlCaptureContext(__out PCONTEXT ContextRecord)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRtlCaptureContext == NULL)
    return E_NOTIMPL;
  fnRtlCaptureContext(ContextRecord);
  return S_OK;
}

HRESULT nktDvDynApis_AddVectoredExceptionHandler(__out PVOID *lplpRet, __in ULONG FirstHandler,
                                                 __in PVECTORED_EXCEPTION_HANDLER VectoredHandler)
{
  if (lplpRet == NULL)
    return E_POINTER;
  *lplpRet = NULL;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnAddVectoredExceptionHandler == NULL)
    return E_NOTIMPL;
  *lplpRet = fnAddVectoredExceptionHandler(FirstHandler, VectoredHandler);
  return S_OK;
}

HRESULT nktDvDynApis_RemoveVectoredExceptionHandler(__in PVOID Handler)
{
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnRemoveVectoredExceptionHandler == NULL)
    return E_NOTIMPL;
  fnRemoveVectoredExceptionHandler(Handler);
  return S_OK;
}
#endif //_M_X64

HRESULT nktDvDynApis_AllocateVirtualMemory(__out PVOID *BaseAddress, __in SIZE_T RegionSize, __in ULONG Protect)
{
  SIZE_T RegionSize_Temp;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtAllocateVirtualMemory == NULL)
    return E_NOTIMPL;
  *BaseAddress = NULL;
  RegionSize_Temp = RegionSize;
  nNtStatus = fnNtAllocateVirtualMemory((HANDLE)-1, BaseAddress, 0, &RegionSize_Temp, MEM_COMMIT, Protect);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtFreeVirtualMemory(__in PVOID BaseAddress)
{
  SIZE_T RegionSize_Temp;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtFreeVirtualMemory == NULL)
    return E_NOTIMPL;
  RegionSize_Temp = 0;
  nNtStatus = fnNtFreeVirtualMemory((HANDLE)-1, &BaseAddress, &RegionSize_Temp, MEM_RELEASE);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtProtectVirtualMemory(__in PVOID BaseAddress, __in SIZE_T BytesToProtect,
                                            __in ULONG NewAccessProtection, __out PULONG UnsafeOldAccessProtection)
{
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtProtectVirtualMemory == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtProtectVirtualMemory((HANDLE)-1, &BaseAddress, &BytesToProtect, NewAccessProtection,
                                       UnsafeOldAccessProtection);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtFlushInstructionCache(__in PVOID BaseAddress, __in ULONG NumberOfBytesToFlush)
{
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtFlushInstructionCache == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtFlushInstructionCache((HANDLE)-1, BaseAddress, NumberOfBytesToFlush);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtOpenThread(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess, __in DWORD dwTid)
{
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_DV_CLIENT_ID sClientId;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtOpenThread == NULL)
    return E_NOTIMPL;
  sClientId.UniqueThread = (SIZE_T)(ULONG_PTR)(dwTid);
  sClientId.UniqueProcess = 0;
  InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
  nNtStatus = fnNtOpenThread(ThreadHandle, DesiredAccess, &sObjAttr, &sClientId);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtClose(__in HANDLE Handle)
{
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtClose == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtClose(Handle);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtCreateEvent(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess, __in LPCWSTR szNameW,
                                   __in BOOLEAN ManualReset, __in BOOLEAN InitialState)
{
  OBJECT_ATTRIBUTES sObjAttr;
  UNICODE_STRING sName;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtCreateEvent == NULL)
    return E_NOTIMPL;
  if (szNameW != NULL)
  {
    sName.Buffer = (PWSTR)szNameW;
    for (sName.Length=0; szNameW[sName.Length]!=0; sName.Length++);
    sName.Length *= (USHORT)sizeof(WCHAR);
    sName.MaximumLength = sName.Length;
  }
  InitializeObjectAttributes(&sObjAttr, (szNameW != NULL) ? &sName : NULL, 0, NULL, NULL);
  nNtStatus = fnNtCreateEvent(EventHandle, EVENT_ALL_ACCESS, &sObjAttr, (ManualReset != FALSE) ? 0 : 1, InitialState);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtOpenEvent(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess, __in LPCWSTR szNameW)
{
  OBJECT_ATTRIBUTES sObjAttr;
  UNICODE_STRING sName;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtOpenEvent == NULL)
    return E_NOTIMPL;
  if (szNameW != NULL)
  {
    sName.Buffer = (PWSTR)szNameW;
    for (sName.Length=0; szNameW[sName.Length]!=0; sName.Length++);
    sName.Length *= (USHORT)sizeof(WCHAR);
    sName.MaximumLength = sName.Length;
  }
  InitializeObjectAttributes(&sObjAttr, (szNameW != NULL) ? &sName : NULL, 0, NULL, NULL);
  nNtStatus = fnNtOpenEvent(EventHandle, EVENT_ALL_ACCESS, &sObjAttr);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtResetEvent(__in HANDLE EventHandle)
{
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtResetEvent == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtResetEvent(EventHandle, NULL);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtSetEvent(__in HANDLE EventHandle)
{
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtSetEvent == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtSetEvent(EventHandle, NULL);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtGetContextThread(__in HANDLE ThreadHandle, __out PCONTEXT Context)
{
  //15 5177 6866
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtGetContextThread == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtGetContextThread(ThreadHandle, Context);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_NtDelayExecution(__in DWORD dwMilliseconds)
{
  LARGE_INTEGER sLI;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtDelayExecution == NULL)
    return E_NOTIMPL;
  sLI.QuadPart = -(LONGLONG)((ULONGLONG)dwMilliseconds * 10000ui64);
  nNtStatus = fnNtDelayExecution(FALSE, &sLI);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_GetThreadPriority(__in HANDLE hThread, __out int *lpnPriority)
{
  NKT_DV_THREAD_BASIC_INFORMATION sTbi;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtQueryInformationThread == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtQueryInformationThread(hThread, NKT_DV_ThreadBasicInformation, &sTbi, sizeof(sTbi), NULL);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  if (sTbi.BasePriority == THREAD_BASE_PRIORITY_LOWRT+1)
    *lpnPriority = (int)THREAD_PRIORITY_TIME_CRITICAL;
  else if (sTbi.BasePriority == THREAD_BASE_PRIORITY_IDLE-1)
    *lpnPriority = (int)THREAD_PRIORITY_IDLE;
  else
    *lpnPriority = (int)(sTbi.BasePriority);
  return S_OK;
}

HRESULT nktDvDynApis_SetThreadPriority(__in HANDLE hThread, __in int _nPriority)
{
  LONG nPriority = _nPriority;
  NTSTATUS nNtStatus;

  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtSetInformationThread == NULL)
    return E_NOTIMPL;
  if (nPriority == THREAD_PRIORITY_TIME_CRITICAL)
    nPriority = THREAD_BASE_PRIORITY_LOWRT + 1;
  else if (nPriority == THREAD_PRIORITY_IDLE)
    nPriority = THREAD_BASE_PRIORITY_IDLE - 1;
  nNtStatus = fnNtSetInformationThread(hThread, NKT_DV_ThreadBasePriority, &nPriority, sizeof(nPriority));
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

HRESULT nktDvDynApis_DuplicateHandle(__in HANDLE hSourceProcessHandle, __in HANDLE hSourceHandle,
                                     __in HANDLE hTargetProcessHandle, __deref_out LPHANDLE lpTargetHandle,
                                     __in DWORD dwDesiredAccess, __in BOOL bInheritHandle, __in DWORD dwOptions)
{
  NTSTATUS nNtStatus;

  if (lpTargetHandle != NULL)
    *lpTargetHandle = NULL;
  if (nInitialized == 0)
  {
    HRESULT hRes = InitializeInternals();
    if (FAILED(hRes))
      return hRes;
  }
  //----
  if (fnNtDuplicateObject == NULL)
    return E_NOTIMPL;
  nNtStatus = fnNtDuplicateObject(hSourceProcessHandle, hSourceHandle, hTargetProcessHandle, lpTargetHandle,
                                  dwDesiredAccess, (bInheritHandle != FALSE) ? OBJ_INHERIT : 0, dwOptions);
  if (nNtStatus < 0)
    return HRESULT_FROM_NT(nNtStatus);
  return S_OK;
}

//-----------------------------------------------------------

static HRESULT InitializeInternals()
{
  CNktSimpleLockNonReentrant cLock(&nInitializedMutex);

  if (nInitialized == 0)
  {
    hNtDll = ::LoadLibraryW(L"ntdll.dll");
    if (hNtDll != NULL)
    {
#ifdef AGENTDLL
      BuildOriginalNtCalls();
#endif //AGENTDLL
      if (fnNtQueryInformationProcess == NULL)
      {
        fnNtQueryInformationProcess = (lpfnNtQueryInformationProcess)::GetProcAddress(hNtDll,
                                                                                      "NtQueryInformationProcess");
      }
      if (fnNtQueryInformationThread == NULL)
        fnNtQueryInformationThread = (lpfnNtQueryInformationThread)::GetProcAddress(hNtDll, "NtQueryInformationThread");
      if (fnNtSetInformationThread == NULL)
        fnNtSetInformationThread = (lpfnNtSetInformationThread)::GetProcAddress(hNtDll, "NtSetInformationThread");
      if (fnNtQueryInformationFile == NULL)
        fnNtQueryInformationFile = (lpfnNtQueryInformationFile)::GetProcAddress(hNtDll, "NtQueryInformationFile");
      if (fnNtQueryObject == NULL)
        fnNtQueryObject = (lpfnNtQueryObject)::GetProcAddress(hNtDll, "NtQueryObject");
      if (fnNtQueryVirtualMemory == NULL)
        fnNtQueryVirtualMemory = (lpfnNtQueryVirtualMemory)::GetProcAddress(hNtDll, "NtQueryVirtualMemory");
      if (fnNtSuspendThread == NULL)
        fnNtSuspendThread = (lpfnNtSuspendThread)::GetProcAddress(hNtDll, "NtSuspendThread");
      if (fnNtResumeThread == NULL)
        fnNtResumeThread = (lpfnNtResumeThread)::GetProcAddress(hNtDll, "NtResumeThread");
      if (fnNtQuerySystemInformation == NULL)
        fnNtQuerySystemInformation = (lpfnNtQuerySystemInformation)::GetProcAddress(hNtDll, "NtQuerySystemInformation");
      fnLdrLockLoaderLock = (lpfnLdrLockLoaderLock)::GetProcAddress(hNtDll, "LdrLockLoaderLock");
      fnLdrUnlockLoaderLock = (lpfnLdrUnlockLoaderLock)::GetProcAddress(hNtDll, "LdrUnlockLoaderLock");
      if (fnLdrLockLoaderLock == NULL || fnLdrUnlockLoaderLock == NULL)
      {
        fnLdrLockLoaderLock = NULL;
        fnLdrUnlockLoaderLock = NULL;
      }
      fnLdrFindEntryForAddress = (lpfnLdrFindEntryForAddress)::GetProcAddress(hNtDll, "LdrFindEntryForAddress");
      fnRtlEnterCriticalSection = (lpfnRtlEnterCriticalSection)::GetProcAddress(hNtDll, "RtlEnterCriticalSection");
      fnRtlTryEnterCriticalSection = (lpfnRtlTryEnterCriticalSection)::GetProcAddress(hNtDll,
                                                                                      "RtlTryEnterCriticalSection");
      fnRtlLeaveCriticalSection = (lpfnRtlLeaveCriticalSection)::GetProcAddress(hNtDll, "RtlLeaveCriticalSection");
      if (fnRtlEnterCriticalSection == NULL || fnRtlLeaveCriticalSection == NULL)
      {
        fnRtlEnterCriticalSection = NULL;
        fnRtlLeaveCriticalSection = NULL;
      }
      fnLdrGetProcedureAddress = (lpfnLdrGetProcedureAddress)::GetProcAddress(hNtDll, "LdrGetProcedureAddress");
      fnRtlDllShutdownInProgress = (lpfnRtlDllShutdownInProgress)::GetProcAddress(hNtDll, "RtlDllShutdownInProgress");
#ifdef _M_X64
      fnRtlLookupFunctionEntry = (lpfnRtlLookupFunctionEntry)::GetProcAddress(hNtDll, "RtlLookupFunctionEntry");
      fnRtlVirtualUnwind = (lpfnRtlVirtualUnwind)::GetProcAddress(hNtDll, "RtlVirtualUnwind");
      fnRtlCaptureContext = (lpfnRtlCaptureContext)::GetProcAddress(hNtDll, "RtlCaptureContext");
      if (fnRtlLookupFunctionEntry == NULL || fnRtlVirtualUnwind == NULL || fnRtlCaptureContext == NULL)
      {
        fnRtlLookupFunctionEntry = NULL;
        fnRtlVirtualUnwind = NULL;
        fnRtlCaptureContext = NULL;
      }
#endif //_M_X64
      if (fnNtAllocateVirtualMemory == NULL)
        fnNtAllocateVirtualMemory = (lpfnNtAllocateVirtualMemory)::GetProcAddress(hNtDll, "NtAllocateVirtualMemory");
      if (fnNtFreeVirtualMemory == NULL)
        fnNtFreeVirtualMemory = (lpfnNtFreeVirtualMemory)::GetProcAddress(hNtDll, "NtFreeVirtualMemory");
      if (fnNtProtectVirtualMemory == NULL)
        fnNtProtectVirtualMemory = (lpfnNtProtectVirtualMemory)::GetProcAddress(hNtDll, "NtProtectVirtualMemory");
      if (fnNtFlushInstructionCache == NULL)
        fnNtFlushInstructionCache = (lpfnNtFlushInstructionCache)::GetProcAddress(hNtDll, "NtFlushInstructionCache");
      if (fnNtClose == NULL)
        fnNtClose = (lpfnNtClose)::GetProcAddress(hNtDll, "NtClose");
      if (fnNtCreateEvent == NULL)
        fnNtCreateEvent = (lpfnNtCreateEvent)::GetProcAddress(hNtDll, "NtCreateEvent");
      if (fnNtOpenEvent == NULL)
        fnNtOpenEvent = (lpfnNtOpenEvent)::GetProcAddress(hNtDll, "NtOpenEvent");
      if (fnNtResetEvent == NULL)
        fnNtResetEvent = (lpfnNtResetEvent)::GetProcAddress(hNtDll, "NtResetEvent");
      if (fnNtSetEvent == NULL)
        fnNtSetEvent = (lpfnNtSetEvent)::GetProcAddress(hNtDll, "NtSetEvent");
      if (fnNtOpenThread == NULL)
        fnNtOpenThread = (lpfnNtOpenThread)::GetProcAddress(hNtDll, "NtOpenThread");
      if (fnNtGetContextThread == NULL)
        fnNtGetContextThread = (lpfnNtGetContextThread)::GetProcAddress(hNtDll, "NtGetContextThread");
      if (fnNtDelayExecution == NULL)
        fnNtDelayExecution = (lpfnNtDelayExecution)::GetProcAddress(hNtDll, "NtDelayExecution");
      if (fnNtDuplicateObject == NULL)
        fnNtDuplicateObject = (lpfnNtDuplicateObject)::GetProcAddress(hNtDll, "NtDuplicateObject");
    }
    hkernel32Dll = ::LoadLibraryW(L"kernel32.dll");
    if (hkernel32Dll != NULL)
    {
      fnQueryFullProcessImageNameW = (lpfnQueryFullProcessImageNameW)::GetProcAddress(hkernel32Dll,
                                          "QueryFullProcessImageNameW");
      fnGetNativeSystemInfo = (lpfnGetNativeSystemInfo)::GetProcAddress(hkernel32Dll, "GetNativeSystemInfo");
      fnIsWow64Process = (lpfnIsWow64Process)::GetProcAddress(hkernel32Dll, "IsWow64Process");
      fnWow64GetThreadContext = (lpfnWow64GetThreadContext)::GetProcAddress(hkernel32Dll, "Wow64GetThreadContext");
      fnWow64SetThreadContext = (lpfnWow64SetThreadContext)::GetProcAddress(hkernel32Dll, "Wow64SetThreadContext");
      fnGetProcessId = (lpfnGetProcessId)::GetProcAddress(hkernel32Dll, "GetProcessId");
      fnQueryThreadCycleTime = (lpfnQueryThreadCycleTime)::GetProcAddress(hkernel32Dll, "QueryThreadCycleTime");
#ifdef _M_X64
      fnAddVectoredExceptionHandler = (lpfnAddVectoredExceptionHandler)::GetProcAddress(hkernel32Dll,
                                                                                        "AddVectoredExceptionHandler");
      fnRemoveVectoredExceptionHandler = (lpfnRemoveVectoredExceptionHandler)::GetProcAddress(hkernel32Dll,
                                                                                  "RemoveVectoredExceptionHandler");
      if (fnAddVectoredExceptionHandler == NULL || fnRemoveVectoredExceptionHandler == NULL)
      {
        fnAddVectoredExceptionHandler = NULL;
        fnRemoveVectoredExceptionHandler = NULL;
      }
#endif //_M_X64
    }
    hPsApiDll = ::LoadLibraryW(L"psapi.dll");
    if (hPsApiDll != NULL)
    {
      fnGetProcessImageFileNameW = (lpfnGetProcessImageFileNameW)::GetProcAddress(hPsApiDll,
                                        "GetProcessImageFileNameW");
      fnGetModuleFileNameExW = (lpfnGetModuleFileNameExW)::GetProcAddress(hPsApiDll,
                                        "GetModuleFileNameExW");
      fnEnumProcessModules = (lpfnEnumProcessModules)::GetProcAddress(hPsApiDll, "EnumProcessModules");
      fnEnumProcessModulesEx = (lpfnEnumProcessModulesEx)::GetProcAddress(hPsApiDll,
                                        "EnumProcessModulesEx");
    }
    //----
    if (sSi.wProcessorArchitecture == 0)
    {
      if (fnGetNativeSystemInfo != NULL)
        fnGetNativeSystemInfo(&sSi);
      else
        ::GetSystemInfo(&sSi);
    }
    //----
    NktInterlockedExchange(&nInitialized, 1);
  }
  return S_OK;
}

//-----------------------------------------------------------

#ifdef AGENTDLL

static HRESULT BuildOriginalNtCalls()
{
  static struct {
    LPSTR szApiNameA;
    LPVOID *lpAddr;
  } aToCheckApis[] = {
    { "NtQueryInformationProcess", (LPVOID*)&fnNtQueryInformationProcess },
    { "NtQueryInformationThread",  (LPVOID*)&fnNtQueryInformationThread },
    { "NtSetInformationThread",    (LPVOID*)&fnNtSetInformationThread },
    { "NtQueryInformationFile",    (LPVOID*)&fnNtQueryInformationFile },
//    { "NtQueryObject",             (LPVOID*)&fnNtQueryObject }, //affects ThinApp virtual handles
    { "NtQueryVirtualMemory",      (LPVOID*)&fnNtQueryVirtualMemory },
    { "NtSuspendThread",           (LPVOID*)&fnNtSuspendThread },
    { "NtResumeThread",            (LPVOID*)&fnNtResumeThread },
    { "NtQuerySystemInformation",  (LPVOID*)&fnNtQuerySystemInformation },
    { "NtAllocateVirtualMemory",   (LPVOID*)&fnNtAllocateVirtualMemory },
    { "NtFreeVirtualMemory",       (LPVOID*)&fnNtFreeVirtualMemory },
    { "NtProtectVirtualMemory",    (LPVOID*)&fnNtProtectVirtualMemory },
    { "NtFlushInstructionCache",   (LPVOID*)&fnNtFlushInstructionCache },
    { "NtClose",                   (LPVOID*)&fnNtClose },
    { "NtCreateEvent",             (LPVOID*)&fnNtCreateEvent },
    { "NtOpenEvent",               (LPVOID*)&fnNtOpenEvent },
    { "NtResetEvent",              (LPVOID*)&fnNtResetEvent },
    { "NtSetEvent",                (LPVOID*)&fnNtSetEvent },
    { "NtOpenThread",              (LPVOID*)&fnNtOpenThread },
    { "NtGetContextThread",        (LPVOID*)&fnNtGetContextThread },
    { "NtDelayExecution",          (LPVOID*)&fnNtDelayExecution },
    { "NtDuplicateObject",         (LPVOID*)&fnNtDuplicateObject },
    { NULL, NULL }
  };
  WCHAR szBufW[4096];
  HANDLE hFile, hFileMap;
  NKT_DV_IMAGE_NT_HEADER sFileNtHdr;
  LPBYTE lpData, lpFileNtHdrAddr, lpFileFuncAddr;
  SIZE_T nSecCount;
  IMAGE_SECTION_HEADER *lpFileImgSect;
  IMAGE_DATA_DIRECTORY *lpFileExportsDir;
  IMAGE_EXPORT_DIRECTORY *lpExpDir;
  DWORD i, j, dwRawAddr, *lpdwAddressOfFunctions, *lpdwAddressOfNames;
  WORD wOrd, *lpwAddressOfNameOrdinals;
  LPSTR sA;
  HRESULT hRes;

  hFile = hFileMap = NULL;
  lpData = NULL;
  ::GetModuleFileNameW(hNtDll, szBufW, (DWORD)NKT_DV_ARRAYLEN(szBufW));
  hFile = ::CreateFileW(szBufW, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
  hRes = (hFile != NULL && hFile != INVALID_HANDLE_VALUE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  if (SUCCEEDED(hRes))
  {
    hFileMap = ::CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMap == NULL || hFileMap == INVALID_HANDLE_VALUE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (SUCCEEDED(hRes))
  {
    lpData = (LPBYTE)::MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
    if (lpData == NULL)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (SUCCEEDED(hRes))
  {
    switch (nktDvNtCheckImageType(&sFileNtHdr, ::GetCurrentProcess(), lpData, &lpFileNtHdrAddr))
    {
#if defined _M_IX86
      case 32:
        nSecCount = (SIZE_T)(sFileNtHdr.u32.FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpFileNtHdrAddr + sizeof(sFileNtHdr.u32));
        lpFileExportsDir = &(sFileNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#elif defined _M_X64
      case 64:
        nSecCount = (SIZE_T)(sFileNtHdr.u64.FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpFileNtHdrAddr + sizeof(sFileNtHdr.u64));
        lpFileExportsDir = &(sFileNtHdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#endif
      default:
        hRes = E_FAIL;
        break;
    }
    //check exports
    if (SUCCEEDED(hRes) && lpFileExportsDir->VirtualAddress != 0 && lpFileExportsDir->Size > 0)
    {
      dwRawAddr = HelperConvertVAToRaw(lpFileExportsDir->VirtualAddress, lpFileImgSect, nSecCount);
      if (dwRawAddr != 0)
      {
        lpExpDir = (IMAGE_EXPORT_DIRECTORY*)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfFunctions, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpdwAddressOfFunctions = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNames, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpdwAddressOfNames = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNameOrdinals, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpwAddressOfNameOrdinals = (LPWORD)(lpData + (SIZE_T)dwRawAddr);
        for (i=0; i<lpExpDir->NumberOfNames; i++)
        {
          dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfNames[i], lpFileImgSect, nSecCount);
          if (dwRawAddr == 0)
            continue;
          sA = (LPSTR)(lpData + (SIZE_T)dwRawAddr);
          for (j=0; aToCheckApis[j].szApiNameA!=NULL; j++)
          {
            if (strcmp(sA, aToCheckApis[j].szApiNameA) == 0)
              break;
          }
          if (aToCheckApis[j].szApiNameA != NULL)
          {
            wOrd = lpwAddressOfNameOrdinals[i];
            dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfFunctions[wOrd], lpFileImgSect, nSecCount);
            lpFileFuncAddr = lpData + (SIZE_T)dwRawAddr;
            if (lpFileFuncAddr >= (LPBYTE)lpExpDir &&
                lpFileFuncAddr < (LPBYTE)lpExpDir+(SIZE_T)(lpFileExportsDir->Size))
              continue;//a forwarded function => ignore
            //create new stub
            *(aToCheckApis[j].lpAddr) = BuildOriginalNtCall(lpFileFuncAddr);
          }
        }
      }
    }
  }
  if (lpData != NULL)
    ::UnmapViewOfFile(lpData);
  if (hFileMap != NULL && hFileMap != INVALID_HANDLE_VALUE)
    ::CloseHandle(hFileMap);
  if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    ::CloseHandle(hFile);
  return hRes;
}

static LPVOID BuildOriginalNtCall(__in LPBYTE lpFileFuncAddr)
{
  SIZE_T i, k, nInstrLen, nCurrSize, nExtraSize;
  LPBYTE lpSrc, lpDest, lpStub;

  //stage 1: scan for a return
  nCurrSize = nExtraSize = 0;
  while (nCurrSize < 128)
  {
    if (lpFileFuncAddr[nCurrSize] == 0xC2)
    {
      nCurrSize += 3;
      break;
    }
    if (lpFileFuncAddr[nCurrSize] == 0xC3)
    {
      nCurrSize++;
      break;
    }
    if (lpFileFuncAddr[nCurrSize] == 0xE8)
    {
      //near call found, calculate size too
      lpSrc = lpFileFuncAddr+nCurrSize+5 + (SSIZE_T) *((LONG NKT_UNALIGNED*)(lpFileFuncAddr+nCurrSize+1));
      nCurrSize += 5;
      k = 0;
      while (k < 128)
      {
        if (lpSrc[k] == 0xC2)
        {
          k += 3;
          break;
        }
        if (lpSrc[k] == 0xC3)
        {
          k++;
          break;
        }
        nInstrLen = GetInstructionLength(lpSrc+k, 128, sizeof(SIZE_T)<<3, NULL);
        k += nInstrLen;
      }
      //add to extra size
      if (k >= 128)
        return NULL; //unsupported size
      nExtraSize += k;
    }
    else
    {
      nInstrLen = GetInstructionLength(lpFileFuncAddr+nCurrSize, 128, sizeof(SIZE_T)<<3, NULL);
      nCurrSize += nInstrLen;
    }
  }
  if (nCurrSize >= 128)
    return NULL; //unsupported size
  //allocate memory
  lpStub = (LPBYTE)::VirtualAlloc(NULL, nCurrSize+nExtraSize, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (lpStub != NULL)
  {
    //stage 2: copy
    i = nExtraSize = 0;
    while (i < 128)
    {
      if (lpFileFuncAddr[i] == 0xC2)
      {
        memcpy(lpStub+i, lpFileFuncAddr+i, 3);
        i += 3;
        break;
      }
      if (lpFileFuncAddr[i] == 0xC3)
      {
        lpStub[i] = lpFileFuncAddr[i];
        i++;
        break;
      }
      if (lpFileFuncAddr[i] == 0xE8)
      {
        //near call found, relocate
        lpStub[i] = 0xE8;
        *((ULONG NKT_UNALIGNED*)(lpStub+i+1)) = (ULONG)(nCurrSize+nExtraSize - (i+5));
        lpSrc = lpFileFuncAddr+i+5 + (SSIZE_T) *((LONG NKT_UNALIGNED*)(lpFileFuncAddr+i+1));
        i += 5;
        lpDest = lpStub+nCurrSize+nExtraSize;
        k = 0;
        while (k < 128)
        {
          if (lpSrc[k] == 0xC2)
          {
            memcpy(lpDest+k, lpSrc+k, 3);
            k += 3;
            break;
          }
          if (lpSrc[k] == 0xC3)
          {
            lpDest[k] = lpSrc[k];
            k++;
            break;
          }
          nInstrLen = GetInstructionLength(lpSrc+k, 128, sizeof(SIZE_T)<<3, NULL);
          memcpy(lpDest+k, lpSrc+k, nInstrLen);
          k += nInstrLen;
        }
        //add to extra size
        NKT_ASSERT(k < 128);
        nExtraSize += k;
      }
      else
      {
        nInstrLen = GetInstructionLength(lpFileFuncAddr+i, 128, sizeof(SIZE_T)<<3, NULL);
        memcpy(lpStub+i, lpFileFuncAddr+i, nInstrLen);
        i += nInstrLen;
      }
    }
    NKT_ASSERT(i == nCurrSize);
  }
  return lpStub;
}

static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount)
{
  SIZE_T i;

  for (i=0; i<nSecCount; i++)
  {
    if (dwVirtAddr >= lpImgSect[i].VirtualAddress &&
        dwVirtAddr < lpImgSect[i].VirtualAddress + lpImgSect[i].Misc.VirtualSize)
      return dwVirtAddr - lpImgSect[i].VirtualAddress + lpImgSect[i].PointerToRawData;
  }
  return 0;
}

#endif //AGENTDLL
