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
#include <intrin.h>
#include "WaitableObjects.h"
#include "RelocatableCode.h"
#include "AutoPtr.h"
#include "ThreadSuspend.h"
#include "ProcessEntry.h"
#include "Wow64.h"

#pragma intrinsic (_InterlockedIncrement)

using namespace NktHookLib::Internals;

//-----------------------------------------------------------

#define _DOALIGN(x, _align)   ((   (SIZE_T)(x) + (_align-1)) & (~(_align-1    )))

#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

#define SystemProcessorInformation                         1

#define ThreadBasicInformation                             0
#define ThreadBasePriority                                 3
#define ThreadImpersonationToken                           5
#define ThreadQuerySetWin32StartAddress                    9

#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE                    3
#define SE_IMPERSONATE_PRIVILEGE                          29

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED      0x00000001

#define MAX_INIT_PARAMS_SIZE                        10485760

#define Handle2Ull(_param) (ULONGLONG)((LONGLONG)((LONG)(_param)))
#define Ptr2Ull(_param)    (ULONGLONG)((SIZE_T)(_param))
#define Ul2Ull(_param)     (ULONGLONG)((ULONG)(_param))

//-----------------------------------------------------------

typedef struct {
  USHORT ProcessorArchitecture;
  USHORT ProcessorLevel;
  USHORT ProcessorRevision;
  USHORT Reserved;
  ULONG ProcessorFeatureBits;
} NKT_SYSTEM_PROCESSOR_INFORMATION;

typedef struct {
  BYTE Revision;
  BYTE SubAuthorityCount;
  SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
  DWORD SubAuthority[SID_MAX_SUB_AUTHORITIES];
} NKT_SID;

typedef struct {
  UCHAR AceType;
  UCHAR AceFlags;
  USHORT AceSize;
} NKT_ACE_HEADER;

typedef struct {
  NKT_ACE_HEADER Header;
  ACCESS_MASK AccessMask;
} NKT_ACE;

typedef struct {
  ULONG Size;
  ULONG Unknown1;
  ULONG Unknown2;
  PULONG Unknown3;
  ULONG Unknown4;
  ULONG Unknown5;
  ULONG Unknown6;
  PULONG Unknown7;
  ULONG Unknown8;
} NKT_NTCREATETHREADEXBUFFER;

typedef struct {
  SIZE_T UniqueProcess;
  SIZE_T UniqueThread;
} NKT_CLIENT_ID;

//-----------------------------------------------------------

static DWORD CreateProcessWithDll_Common(__inout LPPROCESS_INFORMATION lpPI, __in DWORD dwCreationFlags,
                                         __in_z LPCWSTR szDllNameW, __in_opt HANDLE hSignalCompleted,
                                         __in_z_opt LPCSTR szInitFunctionA, __in_opt LPVOID lpInitFuncParams,
                                         __in_opt ULONG nInitFuncParamsSize);

static DWORD WaitForProcessInitialization(__in HANDLE hProcess, __in LONG nProcPlatform, __in DWORD dwTimeoutMs);

static DWORD InjectDllInRunningProcess(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in_z_opt LPCSTR szInitFuncA,
                                       __in DWORD dwProcessInitWaitTimeoutMs, __out_opt LPHANDLE lphInjectorThread,
                                       __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize);
static DWORD InjectDllInNewProcess(__in HANDLE hProcess, __in HANDLE hMainThread, __in_z LPCWSTR szDllNameW,
                                   __in_z_opt LPCSTR szInitFunctionA, __in_opt HANDLE hCheckPointEvent,
                                   __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize);

static DWORD CreateThreadInRunningProcess(__in HANDLE hProcess, __in LPVOID lpCodeStart, __in LPVOID lpThreadParam,
                                          __out LPHANDLE lphNewThread);
#if defined(_M_IX86)
static DWORD CreateThreadInRunningProcess64(__in HANDLE hProcess, __in ULONGLONG lpCodeStart,
                                            __in ULONGLONG lpThreadParam, __out LPHANDLE lphNewThread);
#endif //_M_IX86
static NTSTATUS GenerateRestrictedThreadToken(__in HANDLE hProcess, __out HANDLE *lphThreadToken);

static NTSTATUS MyCreateRestrictedToken(__in HANDLE hToken, __out HANDLE *lphRestrictedToken);
static NTSTATUS SetTokenIntegrityLevel(__in HANDLE hToken, __in MANDATORY_LEVEL nLevel);
static NTSTATUS GetTokenIntegrityLevel(__in HANDLE hToken, __out MANDATORY_LEVEL *lpnLevel);
static NTSTATUS QueryTokenInfo(__in HANDLE hToken, __in TOKEN_INFORMATION_CLASS nClass, __out LPVOID *lplpInfo);

static NTSTATUS GetPrimaryThread(__in HANDLE hProcess, __out HANDLE *lphThread);

#if defined(_M_IX86)
static SIZE_T ReadMem64(__in HANDLE hProcess, __in LPVOID lpDest, __in ULONGLONG lpSrc, __in SIZE_T nBytesCount);
static BOOL WriteMem64(__in HANDLE hProcess, __in ULONGLONG lpDest, __in LPVOID lpSrc, __in SIZE_T nBytesCount);
#endif //_M_IX86

//-----------------------------------------------------------

namespace NktHookLibHelpers {

DWORD CreateProcessWithDllW(__in_z_opt LPCWSTR lpApplicationName, __inout_z_opt LPWSTR lpCommandLine,
                            __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
                            __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes, __in BOOL bInheritHandles,
                            __in DWORD dwCreationFlags, __in_z_opt LPCWSTR lpEnvironment,
                            __in_z_opt LPCWSTR lpCurrentDirectory, __in LPSTARTUPINFOW lpStartupInfo,
                            __out LPPROCESS_INFORMATION lpProcessInformation, __in_z LPCWSTR szDllNameW,
                            __in_opt HANDLE hSignalCompleted, __in_z_opt LPCSTR szInitFunctionA,
                            __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize)
{
  typedef BOOL (WINAPI *lpfnCreateProcessW)(__in_z_opt LPCWSTR lpApplicationName, __inout_z_opt LPWSTR lpCommandLine,
                                            __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                            __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                            __in BOOL bInheritHandles, __in DWORD dwCreationFlags,
                                            __in_opt LPVOID lpEnvironment, __in_z_opt LPCWSTR lpCurrentDirectory,
                                            __in LPSTARTUPINFOW lpStartupInfo,
                                            __out LPPROCESS_INFORMATION lpProcessInformation);
  HINSTANCE hKernel32Dll;
  lpfnCreateProcessW fnCreateProcessW;

  if (lpProcessInformation != NULL)
    MemSet(lpProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  //check parameters
  if (szDllNameW == NULL || szDllNameW[0] == 0 || lpProcessInformation == NULL)
    return ERROR_INVALID_PARAMETER;
  if ((lpInitFuncParams == NULL && nInitFuncParamsSize > 0) || nInitFuncParamsSize > MAX_INIT_PARAMS_SIZE)
    return ERROR_INVALID_PARAMETER;
  //get needed api from kernel32
  hKernel32Dll = GetModuleBaseAddress(L"kernel32.dll");
  if (hKernel32Dll == NULL)
    return ERROR_PROC_NOT_FOUND;
  fnCreateProcessW = (lpfnCreateProcessW)GetProcedureAddress(hKernel32Dll, "CreateProcessW");
  if (fnCreateProcessW == NULL)
    return ERROR_PROC_NOT_FOUND;
  //create process
  if (lpApplicationName != NULL && lpApplicationName[0] == 0)
    lpApplicationName = NULL;
  if (lpCommandLine != NULL && lpCommandLine[0] == 0 && lpApplicationName == NULL)
    lpCommandLine = NULL;
  if (lpEnvironment != NULL)
  {
    if (lpEnvironment[0] != 0)
      dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    else
      lpEnvironment = NULL;
  }
  if (lpCurrentDirectory != NULL && lpCurrentDirectory[0] == 0)
    lpCurrentDirectory = NULL;
  if (fnCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
                       dwCreationFlags|CREATE_SUSPENDED, (LPVOID)lpEnvironment, lpCurrentDirectory, lpStartupInfo,
                       lpProcessInformation) == FALSE)
    return GetWin32LastError();
  //inject dll load at entrypoint
  return CreateProcessWithDll_Common(lpProcessInformation, dwCreationFlags, szDllNameW, hSignalCompleted,
                                     szInitFunctionA, lpInitFuncParams, nInitFuncParamsSize);
}

DWORD CreateProcessWithLogonAndDllW(__in_z LPCWSTR lpUsername, __in_z_opt LPCWSTR lpDomain, __in_z LPCWSTR lpPassword,
                                    __in DWORD dwLogonFlags, __in_opt LPCWSTR lpApplicationName,
                                    __inout_opt LPWSTR lpCommandLine, __in DWORD dwCreationFlags,
                                    __in_z_opt LPCWSTR lpEnvironment, __in_z_opt LPCWSTR lpCurrentDirectory,
                                    __in LPSTARTUPINFOW lpStartupInfo, __out LPPROCESS_INFORMATION lpProcessInformation,
                                    __in_z LPCWSTR szDllNameW, __in_opt HANDLE hSignalCompleted,
                                    __in_z_opt LPCSTR szInitFunctionA, __in_opt LPVOID lpInitFuncParams,
                                    __in_opt ULONG nInitFuncParamsSize)
{
  typedef HMODULE (WINAPI *lpfnLoadLibraryW)(__in_z LPCWSTR lpFileNameW);
  typedef HMODULE (WINAPI *lpfnFreeLibrary)(__in HMODULE hLibModule);
  typedef BOOL (WINAPI *lpfnCreateProcessWithLogonW)(__in_z LPCWSTR lpUsername, __in_z_opt LPCWSTR lpDomain,
                                         __in_z LPCWSTR lpPassword, __in DWORD dwLogonFlags,
                                         __in_opt LPCWSTR lpApplicationName, __inout_opt LPWSTR lpCommandLine,
                                         __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment,
                                         __in_z_opt LPCWSTR lpCurrentDirectory, __in LPSTARTUPINFOW lpStartupInfo,
                                         __out LPPROCESS_INFORMATION lpProcessInformation);
  HINSTANCE hKernel32Dll, hAdvApi32Dll;
  lpfnLoadLibraryW fnLoadLibraryW;
  lpfnFreeLibrary fnFreeLibrary;
  lpfnCreateProcessWithLogonW fnCreateProcessWithLogonW;
  LPCWSTR sW;
  DWORD dwOsErr;

  if (lpProcessInformation != NULL)
    MemSet(lpProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  //check parameters
  if (szDllNameW == NULL || szDllNameW[0] == 0 || lpProcessInformation == NULL)
    return ERROR_INVALID_PARAMETER;
  if ((lpInitFuncParams == NULL && nInitFuncParamsSize > 0) || nInitFuncParamsSize > MAX_INIT_PARAMS_SIZE)
    return ERROR_INVALID_PARAMETER;
  //get needed api from kernel32
  hKernel32Dll = GetModuleBaseAddress(L"kernel32.dll");
  if (hKernel32Dll == NULL)
    return ERROR_PROC_NOT_FOUND;
  fnLoadLibraryW = (lpfnLoadLibraryW)GetProcedureAddress(hKernel32Dll, "LoadLibraryW");
  fnFreeLibrary = (lpfnFreeLibrary)GetProcedureAddress(hKernel32Dll, "FreeLibrary");
  if (fnLoadLibraryW == NULL || fnFreeLibrary == NULL)
    return ERROR_PROC_NOT_FOUND;
  //load advapi32.dll
  hAdvApi32Dll = fnLoadLibraryW(L"advapi32.dll");
  if (hAdvApi32Dll == NULL)
    return ERROR_PROC_NOT_FOUND;
  fnCreateProcessWithLogonW = (lpfnCreateProcessWithLogonW)GetProcedureAddress(hAdvApi32Dll, "CreateProcessWithLogonW");
  if (fnCreateProcessWithLogonW == NULL)
  {
    fnFreeLibrary(hAdvApi32Dll);
    return ERROR_PROC_NOT_FOUND;
  }
  //create process
  if (lpUsername == NULL)
    lpUsername = L"";
  if (lpDomain == NULL)
  {
    for (sW=lpUsername; *sW != 0 && *sW != L'@'; sW++);
    if (*sW == 0)
      lpDomain = L"";
  }
  if (lpPassword == NULL)
    lpPassword = L"";
  if (lpApplicationName != NULL && lpApplicationName[0] == 0)
    lpApplicationName = NULL;
  if (lpCommandLine != NULL && lpCommandLine[0] == 0 && lpApplicationName == NULL)
    lpCommandLine = NULL;
  if (lpEnvironment != NULL)
  {
    if (lpEnvironment[0] != 0)
      dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    else
      lpEnvironment = NULL;
  }
  if (lpCurrentDirectory != NULL && lpCurrentDirectory[0] == 0)
    lpCurrentDirectory = NULL;
  if (fnCreateProcessWithLogonW(lpUsername, lpDomain, lpPassword, dwLogonFlags, lpApplicationName, lpCommandLine,
                                dwCreationFlags|CREATE_SUSPENDED, (LPVOID)lpEnvironment, lpCurrentDirectory,
                                lpStartupInfo, lpProcessInformation) == FALSE)
  {
    dwOsErr = GetWin32LastError();
    fnFreeLibrary(hAdvApi32Dll);
    return dwOsErr;
  }
  fnFreeLibrary(hAdvApi32Dll);
  //inject dll load at entrypoint
  return CreateProcessWithDll_Common(lpProcessInformation, dwCreationFlags, szDllNameW, hSignalCompleted,
                                     szInitFunctionA, lpInitFuncParams, nInitFuncParamsSize);
}

DWORD CreateProcessWithTokenAndDllW(__in HANDLE hToken, __in DWORD dwLogonFlags, __in_z_opt LPCWSTR lpApplicationName,
                                    __inout_opt LPWSTR lpCommandLine, __in DWORD dwCreationFlags,
                                    __in_z_opt LPCWSTR lpEnvironment, __in_z_opt LPCWSTR lpCurrentDirectory,
                                    __in LPSTARTUPINFOW lpStartupInfo, __out LPPROCESS_INFORMATION lpProcessInformation,
                                    __in_z LPCWSTR szDllNameW, __in_opt HANDLE hSignalCompleted,
                                    __in_z_opt LPCSTR szInitFunctionA, __in_opt LPVOID lpInitFuncParams,
                                    __in_opt ULONG nInitFuncParamsSize)
{
  typedef HMODULE (WINAPI *lpfnLoadLibraryW)(__in_z LPCWSTR lpFileNameW);
  typedef HMODULE (WINAPI *lpfnFreeLibrary)(__in HMODULE hLibModule);
  typedef BOOL (WINAPI *lpfnCreateProcessWithTokenW)(__in HANDLE hToken, __in DWORD dwLogonFlags,
                                         __in_z_opt LPCWSTR lpApplicationName, __inout_opt LPWSTR lpCommandLine,
                                         __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment,
                                         __in_z_opt LPCWSTR lpCurrentDirectory, __in LPSTARTUPINFOW lpStartupInfo,
                                         __out LPPROCESS_INFORMATION lpProcessInformation);
  HINSTANCE hKernel32Dll, hAdvApi32Dll;
  lpfnLoadLibraryW fnLoadLibraryW;
  lpfnFreeLibrary fnFreeLibrary;
  lpfnCreateProcessWithTokenW fnCreateProcessWithTokenW;
  DWORD dwOsErr;

  if (lpProcessInformation != NULL)
    MemSet(lpProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  //check parameters
  if (szDllNameW == NULL || szDllNameW[0] == 0 || lpProcessInformation == NULL)
    return ERROR_INVALID_PARAMETER;
  if ((lpInitFuncParams == NULL && nInitFuncParamsSize > 0) || nInitFuncParamsSize > MAX_INIT_PARAMS_SIZE)
    return ERROR_INVALID_PARAMETER;
  //get needed api from kernel32
  hKernel32Dll = GetModuleBaseAddress(L"kernel32.dll");
  if (hKernel32Dll == NULL)
    return ERROR_PROC_NOT_FOUND;
  fnLoadLibraryW = (lpfnLoadLibraryW)GetProcedureAddress(hKernel32Dll, "LoadLibraryW");
  fnFreeLibrary = (lpfnFreeLibrary)GetProcedureAddress(hKernel32Dll, "FreeLibrary");
  if (fnLoadLibraryW == NULL || fnFreeLibrary == NULL)
    return ERROR_PROC_NOT_FOUND;
  //load advapi32.dll
  hAdvApi32Dll = fnLoadLibraryW(L"advapi32.dll");
  if (hAdvApi32Dll == NULL)
    return ERROR_PROC_NOT_FOUND;
  fnCreateProcessWithTokenW = (lpfnCreateProcessWithTokenW)GetProcedureAddress(hAdvApi32Dll,
                                                                               "CreateProcessWithTokenW");
  if (fnCreateProcessWithTokenW == NULL)
  {
    fnFreeLibrary(hAdvApi32Dll);
    return ERROR_PROC_NOT_FOUND;
  }
  //create process
  if (lpApplicationName != NULL && lpApplicationName[0] == 0)
    lpApplicationName = NULL;
  if (lpCommandLine != NULL && lpCommandLine[0] == 0 && lpApplicationName == NULL)
    lpCommandLine = NULL;
  if (lpEnvironment != NULL)
  {
    if (lpEnvironment[0] != 0)
      dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
    else
      lpEnvironment = NULL;
  }
  if (lpCurrentDirectory != NULL && lpCurrentDirectory[0] == 0)
    lpCurrentDirectory = NULL;
  if (fnCreateProcessWithTokenW(hToken, dwLogonFlags, lpApplicationName, lpCommandLine,
                                dwCreationFlags|CREATE_SUSPENDED, (LPVOID)lpEnvironment, lpCurrentDirectory,
                                lpStartupInfo, lpProcessInformation) == FALSE)
  {
    dwOsErr = GetWin32LastError();
    fnFreeLibrary(hAdvApi32Dll);
    return dwOsErr;
  }
  fnFreeLibrary(hAdvApi32Dll);
  //inject dll load at entrypoint
  return CreateProcessWithDll_Common(lpProcessInformation, dwCreationFlags, szDllNameW, hSignalCompleted,
                                     szInitFunctionA, lpInitFuncParams, nInitFuncParamsSize);
}

DWORD InjectDllByPidW(__in DWORD dwPid, __in_z LPCWSTR szDllNameW, __in_z_opt LPCSTR szInitFunctionA,
                      __in_opt DWORD dwProcessInitWaitTimeoutMs, __out_opt LPHANDLE lphInjectorThread,
                      __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize)
{
  HANDLE hProc = NULL;
  DWORD dwOsErr;
  NTSTATUS nNtStatus;

  if (lphInjectorThread != NULL)
    *lphInjectorThread = NULL;
  if ((lpInitFuncParams == NULL && nInitFuncParamsSize > 0) || nInitFuncParamsSize > MAX_INIT_PARAMS_SIZE)
    return ERROR_INVALID_PARAMETER;
  nNtStatus = CProcessesHandles::CreateHandle(dwPid, STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | PROCESS_CREATE_THREAD |
                                                     PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE |
                                                     PROCESS_DUP_HANDLE | PROCESS_SET_INFORMATION |
                                                     PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME, &hProc);
  if (NT_SUCCESS(nNtStatus))
  {
    dwOsErr = InjectDllByHandleW(hProc, szDllNameW, szInitFunctionA, dwProcessInitWaitTimeoutMs, lphInjectorThread,
                                 lpInitFuncParams, nInitFuncParamsSize);
    NktNtClose(hProc);
  }
  else
  {
    dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  }
  return dwOsErr;
}

DWORD InjectDllByHandleW(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in_z_opt LPCSTR szInitFunctionA,
                         __in_opt DWORD dwProcessInitWaitTimeoutMs, __out_opt LPHANDLE lphInjectorThread,
                         __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize)
{
  if (lphInjectorThread != NULL)
    *lphInjectorThread = NULL;
  if ((lpInitFuncParams == NULL && nInitFuncParamsSize > 0) || nInitFuncParamsSize > MAX_INIT_PARAMS_SIZE)
    return ERROR_INVALID_PARAMETER;
  return InjectDllInRunningProcess(hProcess, szDllNameW, szInitFunctionA, dwProcessInitWaitTimeoutMs,
                                   lphInjectorThread, lpInitFuncParams, nInitFuncParamsSize);
}

} //namespace NktHookLibHelpers

//-----------------------------------------------------------

static DWORD CreateProcessWithDll_Common(__inout LPPROCESS_INFORMATION lpPI, __in DWORD dwCreationFlags,
                                         __in_z LPCWSTR szDllNameW, __in_opt HANDLE hSignalCompleted,
                                         __in_z_opt LPCSTR szInitFunctionA, __in_opt LPVOID lpInitFuncParams,
                                         __in_opt ULONG nInitFuncParamsSize)
{
  DWORD dwOsErr;

  dwOsErr = InjectDllInNewProcess(lpPI->hProcess, lpPI->hThread, szDllNameW, szInitFunctionA, hSignalCompleted,
                                  lpInitFuncParams, nInitFuncParamsSize);
  if (dwOsErr == ERROR_SUCCESS)
  {
    if ((dwCreationFlags & CREATE_SUSPENDED) == 0)
      NktNtResumeThread(lpPI->hThread, NULL);
  }
  else
  {
    typedef NTSTATUS (NTAPI *lpfnNtTerminateProcess)(__in_opt HANDLE ProcessHandle, __in NTSTATUS ExitStatus);
    LPVOID fnNtTerminateProcess;
    HINSTANCE hNtDll;

    hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
    if (hNtDll != NULL)
    {
      fnNtTerminateProcess = NktHookLibHelpers::GetProcedureAddress(hNtDll, "NtTerminateProcess");
      if (fnNtTerminateProcess != NULL)
        ((lpfnNtTerminateProcess)fnNtTerminateProcess)(lpPI->hProcess, STATUS_UNSUCCESSFUL);
    }
    NktNtClose(lpPI->hProcess);
    NktNtClose(lpPI->hThread);
    NktHookLibHelpers::MemSet(lpPI, 0, sizeof(PROCESS_INFORMATION));
  }
  return dwOsErr;
}

static DWORD WaitForProcessInitialization(__in HANDLE hProcess, __in LONG nProcPlatform, __in DWORD dwTimeoutMs)
{
  PROCESS_BASIC_INFORMATION sPbi;
#if defined(_M_IX86)
  NktHookLib::Internals::PROCESS_BASIC_INFORMATION64 sPbi64;
#endif
  LPBYTE lpPeb, lpPtr;
  DWORD dw;
  ULONGLONG ull;
  LONG nNtStatus;

  //get remote process' PEB
  switch (nProcPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      {
#if defined(_M_IX86)
      nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessBasicInformation, &sPbi, (ULONG)sizeof(sPbi), NULL);
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
      lpPeb = (LPBYTE)(sPbi.PebBaseAddress);
#elif defined(_M_X64)
      ULONG_PTR nWow64;

      nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessWow64Information, &nWow64, sizeof(nWow64), NULL);
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
      if (nWow64 == 0)
        return ERROR_INVALID_DATA;
      lpPeb = (LPBYTE)nWow64;
#endif
      }
      break;

    case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
      nNtStatus = NktHookLib::Internals::NtQueryInformationProcess64(hProcess, ProcessBasicInformation, &sPbi64,
                                                                     (ULONG)sizeof(sPbi64), NULL);
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
#elif defined(_M_X64)
      nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessBasicInformation, &sPbi, (ULONG)sizeof(sPbi), NULL);
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
      lpPeb = (LPBYTE)(sPbi.PebBaseAddress);
#endif
      break;
  }
  //now check if process is really initialized
  do
  {
    //is the loader lock field initialized?
    switch (nProcPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
        //read loader lock pointer
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPeb + 0xA0, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw == 0)
          break; //loader data is not initialized
        //read loader lock's recursion count
        lpPtr = (LPBYTE)((SIZE_T)dw);
        dw = 0;
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPtr + 0x08, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw != 0)
          break; //loader lock is held

        //read PEB_LDR_DATA pointer
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPeb + 0x0C, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw == 0)
          break; //PEB_LDR_DATA not initialized
        //read PEB_LDR_DATA.Initialize value
        lpPtr = (LPBYTE)((SIZE_T)dw);
        dw = 0;
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPtr + 0x04, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw != 0)
          return ERROR_SUCCESS;
        break;

      case NKTHOOKLIB_ProcessPlatformX64:
        //read loader lock pointer
#if defined(_M_IX86)
        if (ReadMem64(hProcess, &ull, sPbi64.PebBaseAddress + 0x110ui64, sizeof(ull)) != sizeof(ull))
          return ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
        if (NktHookLibHelpers::ReadMem(hProcess, &ull, lpPeb + 0x110, sizeof(ull)) != sizeof(ull))
          return ERROR_ACCESS_DENIED;
#endif
        if (ull == 0ui64)
          break;
        //read loader lock's recursion count
        lpPtr = (LPBYTE)ull;
        dw = 0;
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPtr + 0x0C, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw != 0)
          break; //loader lock is held

        //read PEB_LDR_DATA pointer
#if defined(_M_IX86)
        if (ReadMem64(hProcess, &ull, sPbi64.PebBaseAddress + 0x18ui64, sizeof(ull)) != sizeof(ull))
          return ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
        if (NktHookLibHelpers::ReadMem(hProcess, &ull, lpPeb + 0x18, sizeof(ull)) != sizeof(ull))
          return ERROR_ACCESS_DENIED;
#endif
        if (ull == 0ui64)
          break; //PEB_LDR_DATA not initialized
        //read PEB_LDR_DATA.Initialize value
        lpPtr = (LPBYTE)ull;
        dw = 0;
        if (NktHookLibHelpers::ReadMem(hProcess, &dw, lpPtr + 0x04, sizeof(dw)) != sizeof(dw))
          return ERROR_ACCESS_DENIED;
        if (dw != 0)
          return ERROR_SUCCESS;
        break;
    }
    if (dwTimeoutMs > 0)
    {
      LARGE_INTEGER sLI;

      if (dwTimeoutMs > 10)
      {
        dwTimeoutMs -= 10;
        sLI.QuadPart = -100000;
      }
      else
      {
        sLI.QuadPart = -10000 * (LONGLONG)dwTimeoutMs;
        dwTimeoutMs = 0;
      }
      NktNtDelayExecution(FALSE, &sLI);
    }
  }
  while (dwTimeoutMs > 0);
  return ERROR_TIMEOUT;
}

static DWORD IsProcessInitialized(__in HANDLE hProcess, __in LONG nProcPlatform, __out LPBOOL lpbIsInitialized)
{
  HANDLE hMainThread;
  PVOID lpThreadStartAddr = NULL;
#if defined(_M_IX86)
  ULONGLONG nThreadStartAddr = 0ui64;
#endif //_M_IX86
  NTSTATUS nNtStatus;

  *lpbIsInitialized = FALSE;

  nNtStatus = GetPrimaryThread(hProcess, &hMainThread);
  if (NT_SUCCESS(nNtStatus))
  {
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      nNtStatus = NktHookLib::Internals::NtQueryInformationThread64(hMainThread,
                                         (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &nThreadStartAddr,
                                         (ULONG)sizeof(nThreadStartAddr), NULL);
      if (NT_SUCCESS(nNtStatus))
        *lpbIsInitialized = (nThreadStartAddr != 0ui64) ? TRUE : FALSE;
    }
    else
    {
#endif //_M_IX86
      nNtStatus = NktNtQueryInformationThread(hMainThread, (THREADINFOCLASS)ThreadQuerySetWin32StartAddress,
                                              &lpThreadStartAddr, (ULONG)sizeof(lpThreadStartAddr), NULL);
      if (NT_SUCCESS(nNtStatus))
        *lpbIsInitialized = (lpThreadStartAddr != NULL) ? TRUE : FALSE;
#if defined(_M_IX86)
    }
#endif //_M_IX86
    NktNtClose(hMainThread);
  }
  if (!NT_SUCCESS(nNtStatus))
    return NktRtlNtStatusToDosError(nNtStatus);
  return ERROR_SUCCESS;
}

static DWORD InjectDllInRunningProcess(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in_z_opt LPCSTR szInitFuncA,
                                       __in DWORD dwProcessInitWaitTimeoutMs, __out_opt LPHANDLE lphInjectorThread,
                                       __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize)
{
  CNktThreadSuspend cProcSusp;
  PROCESS_BASIC_INFORMATION sPbi;
  DWORD dwOsErr;
  SIZE_T k, nRemCodeSize, nDllNameLen, nInitFuncNameLen;
  LPBYTE lpRemCode = NULL;
#if defined(_M_IX86)
  ULONGLONG nRemCode64 = 0ui64;
#endif //_M_IX86
  LONG nProcPlatform;
  HANDLE hNewThread = NULL, hRemoteReadyEvent = NULL, hRemoteContinueEvent = NULL;
  ULONGLONG nRemoteThreadHandleAddr = 0ui64;
  RelocatableCode::GETMODULEANDPROCADDR_DATA gmpa_data = { 0 };
  NTSTATUS nNtStatus;

  //calculate dll name length
  if (szDllNameW == NULL)
    return ERROR_INVALID_PARAMETER;
  for (nDllNameLen=0; nDllNameLen<16384 && szDllNameW[nDllNameLen]!=0; nDllNameLen++);
  nDllNameLen *= sizeof(WCHAR);
  if (nDllNameLen == 0 || nDllNameLen >= 32768)
    return ERROR_INVALID_PARAMETER;
  //calculate init function name length if provided
  nInitFuncNameLen = 0;
  if (szInitFuncA != NULL)
  {
    for (nInitFuncNameLen=0; nInitFuncNameLen<16384 && szInitFuncA[nInitFuncNameLen]!=0; nInitFuncNameLen++);
    if (nInitFuncNameLen >= 32768)
      return ERROR_INVALID_PARAMETER;
  }
  //check process platform
  nProcPlatform = NktHookLibHelpers::GetProcessPlatform(hProcess);
  if (nProcPlatform != NKTHOOKLIB_ProcessPlatformX86 && nProcPlatform != NKTHOOKLIB_ProcessPlatformX64)
    return ERROR_CALL_NOT_IMPLEMENTED;
  //wait until process is initialized
  dwOsErr = WaitForProcessInitialization(hProcess, nProcPlatform, dwProcessInitWaitTimeoutMs);
  if (dwOsErr != ERROR_SUCCESS && dwOsErr != ERROR_TIMEOUT) //ignore timeouts right now
    return dwOsErr;
  //get process id and suspend
  nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessBasicInformation, &sPbi, sizeof(sPbi), NULL);
  if (NT_SUCCESS(nNtStatus))
    dwOsErr = cProcSusp.SuspendAll((DWORD)(sPbi.UniqueProcessId), NULL, 0);
  else
    dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  //allocate memory in remote process
  if (dwOsErr == ERROR_SUCCESS)
  {
    nRemCodeSize = _DOALIGN(RelocatableCode::InjectDllInRunningProcess_GetSize(nProcPlatform), 8);
    nRemCodeSize += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
    nRemCodeSize += _DOALIGN(nDllNameLen+2, 8);
    nRemCodeSize += _DOALIGN(nInitFuncNameLen+1, 8);
    if (nInitFuncNameLen > 0)
      nRemCodeSize += _DOALIGN(nInitFuncParamsSize, 8);
    nRemCodeSize = _DOALIGN(nRemCodeSize, 4096);
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      nNtStatus = NktHookLib::Internals::NtAllocateVirtualMemory64(hProcess, &nRemCode64, 0, &nRemCodeSize,
                                                                   MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    }
    else
    {
#endif //_M_IX86
      nNtStatus = NktNtAllocateVirtualMemory(hProcess, (PVOID*)&lpRemCode, 0, &nRemCodeSize, MEM_RESERVE | MEM_COMMIT,
                                             PAGE_EXECUTE_READWRITE);
#if defined(_M_IX86)
    }
#endif //_M_IX86
    if (!NT_SUCCESS(nNtStatus))
      dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  }
  //write remote code
  if (dwOsErr == ERROR_SUCCESS)
  {
    static const BYTE aZeroes[2] = { 0 };
    LPBYTE d, s;
#if defined(_M_IX86)
    ULONGLONG d64;
#endif //_M_IX86
    BOOL b;

    //assume error
    dwOsErr = ERROR_ACCESS_DENIED;
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
      d64 = nRemCode64;
    else
#endif //_M_IX86
      d = lpRemCode;
    //write new startup code
    k = RelocatableCode::InjectDllInRunningProcess_GetSize(nProcPlatform);
    s = RelocatableCode::InjectDllInRunningProcess_GetCode(nProcPlatform);
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      b = WriteMem64(hProcess, d64, s, k);
      d64 += (ULONGLONG)_DOALIGN(k, 8);
    }
    else
    {
#endif //_M_IX86
      b = NktHookLibHelpers::WriteMem(hProcess, d, s, k);
      d += _DOALIGN(k, 8);
#if defined(_M_IX86)
    }
#endif //_M_IX86
    if (b != FALSE)
    {
      //write get module address and get procedure address code
      k = RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform);
      s = RelocatableCode::GetModuleAndProcAddr_GetCode(nProcPlatform, gmpa_data);
#if defined(_M_IX86)
      if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
      {
        b = WriteMem64(hProcess, d64, s, k);
        d64 += (ULONGLONG)_DOALIGN(k, 8);
      }
      else
      {
#endif //_M_IX86
        b = NktHookLibHelpers::WriteMem(hProcess, d, s, k);
        d += _DOALIGN(k, 8);
#if defined(_M_IX86)
      }
#endif //_M_IX86
      if (b != FALSE)
      {
        //write dll name
#if defined(_M_IX86)
        if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
        {
          b = (WriteMem64(hProcess, d64, (LPVOID)szDllNameW, nDllNameLen) != FALSE &&
               WriteMem64(hProcess, d64 + (ULONGLONG)nDllNameLen, (LPVOID)aZeroes, 2) != FALSE);
        }
        else
        {
#endif //_M_IX86
          b = (NktHookLibHelpers::WriteMem(hProcess, d, (LPVOID)szDllNameW, nDllNameLen) != FALSE &&
               NktHookLibHelpers::WriteMem(hProcess, d + nDllNameLen, (LPVOID)aZeroes, 2) != FALSE);
#if defined(_M_IX86)
        }
#endif //_M_IX86
        if (b != FALSE)
        {
          dwOsErr = ERROR_SUCCESS;
          //if dll name ends with x86.dll, x64.dll, 32.dll or 64.dll, change the number to reflect the correct platform
          k = nDllNameLen / sizeof(WCHAR);
          if (k >= 4 && szDllNameW[k - 4] == L'.' &&
              (szDllNameW[k - 3] == L'd' || szDllNameW[k - 3] == L'D') &&
              (szDllNameW[k - 2] == L'l' || szDllNameW[k - 2] == L'L') &&
              (szDllNameW[k - 1] == L'l' || szDllNameW[k - 1] == L'L'))
          {
            switch (nProcPlatform)
            {
              case NKTHOOKLIB_ProcessPlatformX86:
                if (k >= 7 && (szDllNameW[k - 7] == L'x' || szDllNameW[k - 7] == L'X') &&
                    szDllNameW[k - 6] == L'6' && szDllNameW[k - 5] == L'4')
                {
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"86", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
                }
                else if (k >= 6 && szDllNameW[k - 6] == L'6' && szDllNameW[k - 5] == L'4')
                {
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"32", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
                }
                break;

              case NKTHOOKLIB_ProcessPlatformX64:
                if (k >= 7 && (szDllNameW[k - 7] == L'x' || szDllNameW[k - 7] == L'X') &&
                    szDllNameW[k - 6] == L'8' && szDllNameW[k - 5] == L'6')
                {
#if defined(_M_IX86)
                  if (WriteMem64(hProcess, d64 + (ULONGLONG)(k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#endif
                }
                else if (k >= 6 && szDllNameW[k - 6] == L'3' && szDllNameW[k - 5] == L'2')
                {
#if defined(_M_IX86)
                  if (WriteMem64(hProcess, d64 + (ULONGLONG)(k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#endif
                }
                break;
            }
          }
        }
        //write init function name, if provided
        if (dwOsErr == ERROR_SUCCESS && nInitFuncNameLen > 0)
        {
#if defined(_M_IX86)
          if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
          {
            d64 += (ULONGLONG)_DOALIGN(nDllNameLen + 2, 8);
            if (WriteMem64(hProcess, d64, (LPVOID)szInitFuncA, nInitFuncNameLen) == FALSE ||
                WriteMem64(hProcess, d64 + (ULONGLONG)nInitFuncNameLen, (LPVOID)aZeroes, 1) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
          }
          else
          {
#endif //_M_IX86
            d += _DOALIGN(nDllNameLen+2, 8);
            if (NktHookLibHelpers::WriteMem(hProcess, d, (LPVOID)szInitFuncA, nInitFuncNameLen) == FALSE ||
                NktHookLibHelpers::WriteMem(hProcess, d+nInitFuncNameLen, (LPVOID)aZeroes, 1) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
#if defined(_M_IX86)
          }
#endif //_M_IX86
        }
        //write init function parameters, if provided
        if (dwOsErr == ERROR_SUCCESS && nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
#if defined(_M_IX86)
          if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
          {
            d64 += (ULONGLONG)_DOALIGN(nInitFuncNameLen+1, 8);
            if (WriteMem64(hProcess, d64, lpInitFuncParams, nInitFuncParamsSize) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
          }
          else
          {
#endif //_M_IX86
            d += _DOALIGN(nInitFuncNameLen+1, 8);
            if (NktHookLibHelpers::WriteMem(hProcess, d, lpInitFuncParams, nInitFuncParamsSize) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
#if defined(_M_IX86)
          }
#endif //_M_IX86
        }
      }
    }
  }
  //write new startup data
  if (dwOsErr == ERROR_SUCCESS)
  {
    union {
      DWORD dw[7];
      ULONGLONG ull[7];
    };

    k = _DOALIGN(RelocatableCode::InjectDllInRunningProcess_GetSize(nProcPlatform), 8);
    switch (nProcPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
        //GetProcedureAddress & GetModuleBaseAddress
        dw[0] = (DWORD)((ULONG_PTR)(lpRemCode + k + gmpa_data.nOffset_GetProcedureAddress));
        dw[1] = (DWORD)((ULONG_PTR)(lpRemCode + k + gmpa_data.nOffset_GetModuleBaseAddress));
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        dw[2] = (DWORD)((ULONG_PTR)(lpRemCode + k));
        k += _DOALIGN(nDllNameLen+2, 8);
        //pointer to init function name
        dw[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          dw[3] = (DWORD)((ULONG_PTR)(lpRemCode + k));
          k += _DOALIGN(nInitFuncNameLen+1, 8);
        }
        //initialize 'continue' event to NULL
        dw[4] = NULL;
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          dw[5] = (DWORD)((ULONG_PTR)(lpRemCode + k));
          dw[6] = (DWORD)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          dw[5] = dw[6] = 0;
        }
        //write values
        if (NktHookLibHelpers::WriteMem(hProcess, lpRemCode, dw, 7 * sizeof(DWORD)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
        break;

      case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
        //GetProcedureAddress & GetModuleBaseAddress
        ull[0] = nRemCode64 + (ULONGLONG)k + (ULONGLONG)(gmpa_data.nOffset_GetProcedureAddress);
        ull[1] = nRemCode64 + (ULONGLONG)k + (ULONGLONG)(gmpa_data.nOffset_GetModuleBaseAddress);
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        ull[2] = nRemCode64 + (ULONGLONG)k;
        k += _DOALIGN(nDllNameLen + 2, 8);
        //pointer to init function name
        ull[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          ull[3] = nRemCode64 + (ULONGLONG)k;
          k += _DOALIGN(nInitFuncNameLen + 1, 8);
        }
        //initialize 'continue' event to NULL
        ull[4] = NULL;
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          ull[5] = nRemCode64 + (ULONGLONG)k;
          ull[6] = (ULONGLONG)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          ull[5] = ull[6] = 0;
        }
        //write values
        if (WriteMem64(hProcess, nRemCode64, ull, 7 * sizeof(ULONGLONG)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
        //GetProcedureAddress & GetModuleBaseAddress
        ull[0] = (ULONGLONG)(lpRemCode + k + gmpa_data.nOffset_GetProcedureAddress);
        ull[1] = (ULONGLONG)(lpRemCode + k + gmpa_data.nOffset_GetModuleBaseAddress);
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        ull[2] = (ULONGLONG)(lpRemCode + k);
        k += _DOALIGN(nDllNameLen+2, 8);
        //pointer to init function name
        ull[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          ull[3] = (ULONGLONG)(lpRemCode + k);
          k += _DOALIGN(nInitFuncNameLen+1, 8);
        }
        //initialize 'continue' event to NULL
        ull[4] = NULL;
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          ull[5] = (ULONGLONG)(lpRemCode + k);
          ull[6] = (ULONGLONG)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          ull[5] = ull[6] = 0;
        }
        //write values
        if (NktHookLibHelpers::WriteMem(hProcess, lpRemCode, ull, 7 * sizeof(ULONGLONG)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
#endif
        break;
    }
  }
  //create dll loader thread suspended
  if (dwOsErr == ERROR_SUCCESS)
  {
    switch (nProcPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
        dwOsErr = CreateThreadInRunningProcess(hProcess, lpRemCode + 7 * sizeof(DWORD), NULL, &hNewThread);
        break;

      case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
        dwOsErr = CreateThreadInRunningProcess64(hProcess, nRemCode64 + (ULONGLONG)(7 * sizeof(ULONGLONG)), NULL,
                                                 &hNewThread);
#elif defined(_M_X64)
        dwOsErr = CreateThreadInRunningProcess(hProcess, lpRemCode + 7 * sizeof(ULONGLONG), NULL, &hNewThread);
#endif
        break;
    }
  }
  //flush instruction cache
  if (dwOsErr == ERROR_SUCCESS)
  {
    ULONG ulOldProt;

#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      NktHookLib::Internals::NtProtectVirtualMemory64(hProcess, &nRemCode64, &nRemCodeSize, PAGE_EXECUTE_READ,
                                                      &ulOldProt);
      NktHookLib::Internals::NtFlushInstructionCache64(hProcess, nRemCode64, (ULONG)nRemCodeSize);
    }
    else
    {
#endif //_M_IX86
      NktNtProtectVirtualMemory(hProcess, (PVOID*)&lpRemCode, &nRemCodeSize, PAGE_EXECUTE_READ, &ulOldProt);
      NktNtFlushInstructionCache(hProcess, lpRemCode, (ULONG)nRemCodeSize);
#if defined(_M_IX86)
    }
#endif //_M_IX86
  }
  //done
  if (dwOsErr == ERROR_SUCCESS)
  {
    cProcSusp.ResumeAll();
    NktNtResumeThread(hNewThread, NULL);
    //store new thread handle
    if (lphInjectorThread != NULL)
      *lphInjectorThread = hNewThread;
    else
      NktNtClose(hNewThread);
  }
  else
  {
    //cleanup on error
    if (hNewThread != NULL)
    {
      NktNtClose(hNewThread);
    }
    if (hRemoteContinueEvent != NULL)
    {
      NktNtDuplicateObject(hProcess, hRemoteContinueEvent, hProcess, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
    }
    if (hRemoteReadyEvent != NULL)
    {
      NktNtDuplicateObject(hProcess, hRemoteReadyEvent, hProcess, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
    }
    if (lpRemCode != NULL)
    {
      SIZE_T nSize = 0;

#if defined(_M_IX86)
      if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
        NktHookLib::Internals::NtFreeVirtualMemory64(hProcess, &nRemCode64, &nSize, MEM_RELEASE);
      else
#endif //_M_IX86
        NktNtFreeVirtualMemory(hProcess, (PVOID*)&lpRemCode, &nSize, MEM_RELEASE);
    }
  }
  return dwOsErr;
}

static DWORD InjectDllInNewProcess(__in HANDLE hProcess, __in HANDLE hMainThread, __in_z LPCWSTR szDllNameW,
                                   __in_z_opt LPCSTR szInitFunctionA, __in_opt HANDLE hCheckPointEvent,
                                   __in_opt LPVOID lpInitFuncParams, __in_opt ULONG nInitFuncParamsSize)
{
#if defined(_M_IX86)
  NktHookLib::Internals::CONTEXT64 sCtx64;
  ULONGLONG nRemCode64 = 0ui64;
#elif defined(_M_X64)
  typedef NTSTATUS (NTAPI *lpfnRtlWow64GetThreadContext)(__in HANDLE hThread, __inout PWOW64_CONTEXT lpContext);
  typedef NTSTATUS (NTAPI *lpfnRtlWow64SetThreadContext)(__in HANDLE hThread, __in CONST PWOW64_CONTEXT lpContext);
  HINSTANCE hNtDll;
  lpfnRtlWow64GetThreadContext fnRtlWow64GetThreadContext;
  lpfnRtlWow64SetThreadContext fnRtlWow64SetThreadContext;
  WOW64_CONTEXT sWow64Ctx, *lpWow64Ctx = NULL;
#endif //_M_X64
  LPBYTE lpRemCode = NULL;
  LONG nProcPlatform;
  SIZE_T k, nRemCodeSize, nDllNameLen, nInitFuncNameLen;
  HANDLE hRemoteCheckPointEvent = NULL;
  CONTEXT sCtx;
  RelocatableCode::GETMODULEANDPROCADDR_DATA gmpa_data = { 0 };
  NTSTATUS nNtStatus;
  DWORD dwOsErr;

  //calculate dll name length
  if (szDllNameW == NULL)
    return ERROR_INVALID_PARAMETER;
  for (nDllNameLen=0; nDllNameLen<16384 && szDllNameW[nDllNameLen]!=0; nDllNameLen++);
  nDllNameLen *= sizeof(WCHAR);
  if (nDllNameLen == 0 || nDllNameLen >= 32768)
    return ERROR_INVALID_PARAMETER;
  //calculate init function name length if provided
  nInitFuncNameLen = 0;
  if (szInitFunctionA != NULL)
  {
    for (nInitFuncNameLen=0; nInitFuncNameLen<16384 && szInitFunctionA[nInitFuncNameLen]!=0; nInitFuncNameLen++);
    if (nInitFuncNameLen >= 32768)
      return ERROR_INVALID_PARAMETER;
  }
  //locate needed functions
#if defined(_M_X64)
  hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
  if (hNtDll == NULL)
    return ERROR_MOD_NOT_FOUND;
  fnRtlWow64GetThreadContext = (lpfnRtlWow64GetThreadContext)NktHookLibHelpers::GetProcedureAddress(hNtDll,
                                                                                    "RtlWow64GetThreadContext");
  fnRtlWow64SetThreadContext = (lpfnRtlWow64SetThreadContext)NktHookLibHelpers::GetProcedureAddress(hNtDll,
                                                                                    "RtlWow64SetThreadContext");
#endif //_M_X64
  //check process platform and retrieve main thread's entrypoint
  switch (nProcPlatform = NktHookLibHelpers::GetProcessPlatform(hProcess))
  {
    case NKTHOOKLIB_ProcessPlatformX86:
#if defined(_M_IX86)
      sCtx.ContextFlags = CONTEXT_FULL;
      nNtStatus = NktNtGetContextThread(hMainThread, &sCtx);

#elif defined(_M_X64)
      if (fnRtlWow64GetThreadContext != NULL && fnRtlWow64SetThreadContext != NULL)
      {
        sWow64Ctx.ContextFlags = CONTEXT_FULL;
        nNtStatus = fnRtlWow64GetThreadContext(hMainThread, &sWow64Ctx);
      }
      else
      {
        //try to locate the pointer to the WOW64_CONTEXT data by reading the thread's TLS slot 1
        NKT_HK_THREAD_BASIC_INFORMATION sTbi;
        LPBYTE lpTlsSlot;

        nNtStatus = NktNtQueryInformationThread(hMainThread, (THREADINFOCLASS)ThreadBasicInformation,
                                                &sTbi, sizeof(sTbi), NULL);
        if (NT_SUCCESS(nNtStatus))
        {
          lpTlsSlot = (LPBYTE)(sTbi.TebBaseAddress) + 0x0E10 + 1*sizeof(DWORD);
          if (NktHookLibHelpers::ReadMem(hProcess, &lpWow64Ctx, lpTlsSlot,
                                          sizeof(lpWow64Ctx)) != sizeof(lpWow64Ctx) ||
              NktHookLibHelpers::ReadMem(hProcess, &sWow64Ctx, lpWow64Ctx, sizeof(sWow64Ctx)) != sizeof(sWow64Ctx))
          {
            nNtStatus = STATUS_UNSUCCESSFUL;
          }
        }
      }
#endif
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
      break;

    case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
      sCtx64.ContextFlags = CONTEXT_X64_FULL;
      nNtStatus = NktHookLib::Internals::NtGetContextThread64(hMainThread, &sCtx64);

#elif defined(_M_X64)
      sCtx.ContextFlags = CONTEXT_FULL;
      nNtStatus = NktNtGetContextThread(hMainThread, &sCtx);
#endif
      if (!NT_SUCCESS(nNtStatus))
        return NktRtlNtStatusToDosError(nNtStatus);
      break;

    default:
      return ERROR_CALL_NOT_IMPLEMENTED;
  }
  //calculate memory size and allocate in remote process
  nRemCodeSize = _DOALIGN(RelocatableCode::InjectDllInNewProcess_GetSize(nProcPlatform), 8);
  nRemCodeSize += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
  nRemCodeSize += _DOALIGN(nDllNameLen+2, 8);
  nRemCodeSize += _DOALIGN(nInitFuncNameLen+1, 8);
  if (nInitFuncNameLen > 0)
    nRemCodeSize += _DOALIGN(nInitFuncParamsSize, 8);
  nRemCodeSize = _DOALIGN(nRemCodeSize, 4096);
#if defined(_M_IX86)
  if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
  {
    nNtStatus = NktHookLib::Internals::NtAllocateVirtualMemory64(hProcess, &nRemCode64, 0, &nRemCodeSize,
                                                                 MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  }
  else
  {
#endif //_M_IX86
    nNtStatus = NktNtAllocateVirtualMemory(hProcess, (PVOID*)&lpRemCode, 0, &nRemCodeSize, MEM_RESERVE|MEM_COMMIT,
                                           PAGE_EXECUTE_READWRITE);
#if defined(_M_IX86)
  }
#endif //_M_IX86
  dwOsErr = (NT_SUCCESS(nNtStatus)) ? ERROR_SUCCESS : NktRtlNtStatusToDosError(nNtStatus);
  //write remote code
  if (dwOsErr == ERROR_SUCCESS)
  {
    static const BYTE aZeroes[2] = { 0 };
    LPBYTE d, s;
#if defined(_M_IX86)
    ULONGLONG d64;
#endif //_M_IX86
    BOOL b;

    //assume error
    dwOsErr = ERROR_ACCESS_DENIED;
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
      d64 = nRemCode64;
    else
#endif //_M_IX86
      d = lpRemCode;
    //write new startup code
    k = RelocatableCode::InjectDllInNewProcess_GetSize(nProcPlatform);
    s = RelocatableCode::InjectDllInNewProcess_GetCode(nProcPlatform);
#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      b = WriteMem64(hProcess, d64, s, k);
      d64 += (ULONGLONG)_DOALIGN(k, 8);
    }
    else
    {
#endif //_M_IX86
      b = NktHookLibHelpers::WriteMem(hProcess, d, s, k);
      d += _DOALIGN(k, 8);
#if defined(_M_IX86)
    }
#endif //_M_IX86
    if (b != FALSE)
    {
      //write get module address and get procedure address code
      k = RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform);
      s = RelocatableCode::GetModuleAndProcAddr_GetCode(nProcPlatform, gmpa_data);
#if defined(_M_IX86)
      if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
      {
        b = WriteMem64(hProcess, d64, s, k);
        d64 += (ULONGLONG)_DOALIGN(k, 8);
      }
      else
      {
#endif //_M_IX86
        b = NktHookLibHelpers::WriteMem(hProcess, d, s, k);
        d += _DOALIGN(k, 8);
#if defined(_M_IX86)
      }
#endif //_M_IX86
      if (b != FALSE)
      {
        //write dll name
#if defined(_M_IX86)
        if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
        {
          b = (WriteMem64(hProcess, d64, (LPVOID)szDllNameW, nDllNameLen) != FALSE &&
               WriteMem64(hProcess, d64 + (ULONGLONG)nDllNameLen, (LPVOID)aZeroes, 2) != FALSE);
        }
        else
        {
#endif //_M_IX86
          b = (NktHookLibHelpers::WriteMem(hProcess, d, (LPVOID)szDllNameW, nDllNameLen) != FALSE &&
               NktHookLibHelpers::WriteMem(hProcess, d + nDllNameLen, (LPVOID)aZeroes, 2) != FALSE);
#if defined(_M_IX86)
        }
#endif //_M_IX86
        if (b != FALSE)
        {
          dwOsErr = ERROR_SUCCESS;
          //if dll name ends with x86.dll, x64.dll, 32.dll or 64.dll, change the number to reflect the correct platform
          k = nDllNameLen / sizeof(WCHAR);
          if (k >= 4 && szDllNameW[k - 4] == L'.' &&
              (szDllNameW[k - 3] == L'd' || szDllNameW[k - 3] == L'D') &&
              (szDllNameW[k - 2] == L'l' || szDllNameW[k - 2] == L'L') &&
              (szDllNameW[k - 1] == L'l' || szDllNameW[k - 1] == L'L'))
          {
            switch (nProcPlatform)
            {
              case NKTHOOKLIB_ProcessPlatformX86:
                if (k >= 7 && (szDllNameW[k - 7] == L'x' || szDllNameW[k - 7] == L'X') &&
                    szDllNameW[k - 6] == L'6' && szDllNameW[k - 5] == L'4')
                {
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"86", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
                }
                else if (k >= 6 && szDllNameW[k - 6] == L'6' && szDllNameW[k - 5] == L'4')
                {
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"32", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
                }
                break;

              case NKTHOOKLIB_ProcessPlatformX64:
                if (k >= 7 && (szDllNameW[k - 7] == L'x' || szDllNameW[k - 7] == L'X') &&
                    szDllNameW[k - 6] == L'8' && szDllNameW[k - 5] == L'6')
                {
#if defined(_M_IX86)
                  if (WriteMem64(hProcess, d64 + (ULONGLONG)(k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#endif
                }
                else if (k >= 6 && szDllNameW[k - 6] == L'3' && szDllNameW[k - 5] == L'2')
                {
#if defined(_M_IX86)
                  if (WriteMem64(hProcess, d64 + (ULONGLONG)(k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
                  if (NktHookLibHelpers::WriteMem(hProcess, d + (k - 6) * sizeof(WCHAR), L"64", 4) == FALSE)
                    dwOsErr = ERROR_ACCESS_DENIED;
#endif
                }
                break;
            }
          }
        }
        //write init function name, if provided
        if (dwOsErr == ERROR_SUCCESS && nInitFuncNameLen > 0)
        {
#if defined(_M_IX86)
          if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
          {
            d64 += (ULONGLONG)_DOALIGN(nDllNameLen + 2, 8);
            if (WriteMem64(hProcess, d64, (LPVOID)szInitFunctionA, nInitFuncNameLen) == FALSE ||
                WriteMem64(hProcess, d64 + (ULONGLONG)nInitFuncNameLen, (LPVOID)aZeroes, 1) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
          }
          else
          {
#endif //_M_IX86
            d += _DOALIGN(nDllNameLen+2, 8);
            if (NktHookLibHelpers::WriteMem(hProcess, d, (LPVOID)szInitFunctionA, nInitFuncNameLen) == FALSE ||
                NktHookLibHelpers::WriteMem(hProcess, d+nInitFuncNameLen, (LPVOID)aZeroes, 1) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
#if defined(_M_IX86)
          }
#endif //_M_IX86
        }
        //write init function parameters, if provided
        if (dwOsErr == ERROR_SUCCESS && nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
#if defined(_M_IX86)
          if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
          {
            d64 += (ULONGLONG)_DOALIGN(nInitFuncNameLen+1, 8);
            if (WriteMem64(hProcess, d64, lpInitFuncParams, nInitFuncParamsSize) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
          }
          else
          {
#endif //_M_IX86
            d += _DOALIGN(nInitFuncNameLen+1, 8);
            if (NktHookLibHelpers::WriteMem(hProcess, d, lpInitFuncParams, nInitFuncParamsSize) == FALSE)
            {
              dwOsErr = ERROR_ACCESS_DENIED;
            }
#if defined(_M_IX86)
          }
#endif //_M_IX86
        }
      }
    }
  }
  //duplicate checkpoint event on target process
  if (dwOsErr == ERROR_SUCCESS && hCheckPointEvent != NULL)
  {
    nNtStatus = NktNtDuplicateObject(NKTHOOKLIB_CurrentProcess, hCheckPointEvent, hProcess, &hRemoteCheckPointEvent,
                                     0, 0, DUPLICATE_SAME_ACCESS);
    if (!NT_SUCCESS(nNtStatus))
      dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  }
  //write new startup data
  if (dwOsErr == ERROR_SUCCESS)
  {
    union {
      DWORD dw[8];
      ULONGLONG ull[8];
    };

    k = _DOALIGN(RelocatableCode::InjectDllInNewProcess_GetSize(nProcPlatform), 8);
    switch (nProcPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
        //GetProcedureAddress & GetModuleBaseAddress
        dw[0] = (DWORD)((ULONG_PTR)(lpRemCode + k + gmpa_data.nOffset_GetProcedureAddress));
        dw[1] = (DWORD)((ULONG_PTR)(lpRemCode + k + gmpa_data.nOffset_GetModuleBaseAddress));
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        dw[2] = (DWORD)((ULONG_PTR)(lpRemCode + k));
        k += _DOALIGN(nDllNameLen+2, 8);
        //pointer to init function name
        dw[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          dw[3] = (DWORD)((ULONG_PTR)(lpRemCode + k));
          k += _DOALIGN(nInitFuncNameLen+1, 8);
        }
        //original entrypoint
#if defined(_M_IX86)
        dw[4] = (DWORD)sCtx.Eip;
#elif defined(_M_X64)
        dw[4] = (DWORD)sWow64Ctx.Eip;
#endif
        //checkpoint event
        dw[5] = (DWORD)((ULONG_PTR)hRemoteCheckPointEvent);
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          dw[6] = (DWORD)((ULONG_PTR)(lpRemCode + k));
          dw[7] = (DWORD)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          dw[6] = dw[7] = 0;
        }
        //write values
        if (NktHookLibHelpers::WriteMem(hProcess, lpRemCode, dw, 8 * sizeof(DWORD)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
        break;

      case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
        //GetProcedureAddress & GetModuleBaseAddress
        ull[0] = nRemCode64 + (ULONGLONG)k + (ULONGLONG)(gmpa_data.nOffset_GetProcedureAddress);
        ull[1] = nRemCode64 + (ULONGLONG)k + (ULONGLONG)(gmpa_data.nOffset_GetModuleBaseAddress);
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        ull[2] = nRemCode64 + (ULONGLONG)k;
        k += _DOALIGN(nDllNameLen + 2, 8);
        //pointer to init function name
        ull[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          ull[3] = nRemCode64 + (ULONGLONG)k;
          k += _DOALIGN(nInitFuncNameLen + 1, 8);
        }
        //original entrypoint
        ull[4] = (ULONGLONG)sCtx64.Rip;
        //checkpoint event
        ull[5] = Handle2Ull(hRemoteCheckPointEvent);
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          ull[6] = nRemCode64 + (ULONGLONG)k;
          ull[7] = (ULONGLONG)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          ull[6] = ull[7] = 0;
        }
        //write values
        if (WriteMem64(hProcess, nRemCode64, ull, 8 * sizeof(ULONGLONG)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
#elif defined(_M_X64)
        //GetProcedureAddress & GetModuleBaseAddress
        ull[0] = (ULONGLONG)(lpRemCode + k + gmpa_data.nOffset_GetProcedureAddress);
        ull[1] = (ULONGLONG)(lpRemCode + k + gmpa_data.nOffset_GetModuleBaseAddress);
        k += _DOALIGN(RelocatableCode::GetModuleAndProcAddr_GetSize(nProcPlatform), 8);
        //pointer to dll name
        ull[2] = (ULONGLONG)(lpRemCode + k);
        k += _DOALIGN(nDllNameLen+2, 8);
        //pointer to init function name
        ull[3] = 0;
        if (nInitFuncNameLen > 0)
        {
          ull[3] = (ULONGLONG)(lpRemCode + k);
          k += _DOALIGN(nInitFuncNameLen+1, 8);
        }
        //original entrypoint
        ull[4] = (ULONGLONG)sCtx.Rip;
        //checkpoint event
        ull[5] = (ULONGLONG)hRemoteCheckPointEvent;
        //initialize init function parameters
        if (nInitFuncNameLen > 0 && nInitFuncParamsSize > 0)
        {
          ull[6] = (ULONGLONG)(lpRemCode + k);
          ull[7] = (ULONGLONG)nInitFuncParamsSize;
          k += _DOALIGN(nInitFuncParamsSize, 8);
        }
        else
        {
          ull[6] = ull[7] = 0;
        }
        //write values
        if (NktHookLibHelpers::WriteMem(hProcess, lpRemCode, ull, 8 * sizeof(ULONGLONG)) == FALSE)
          dwOsErr = ERROR_ACCESS_DENIED;
#endif
        break;
    }
  }
  //change page protection and flush instruction cache
  if (dwOsErr == ERROR_SUCCESS)
  {
    ULONG ulOldProt;

#if defined(_M_IX86)
    if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
    {
      NktHookLib::Internals::NtProtectVirtualMemory64(hProcess, &nRemCode64, &nRemCodeSize, PAGE_EXECUTE_READ,
                                                      &ulOldProt);
      NktHookLib::Internals::NtFlushInstructionCache64(hProcess, nRemCode64, (ULONG)nRemCodeSize);
    }
    else
    {
#endif //_M_IX86
      NktNtProtectVirtualMemory(hProcess, (PVOID*)&lpRemCode, &nRemCodeSize, PAGE_EXECUTE_READ, &ulOldProt);
      NktNtFlushInstructionCache(hProcess, lpRemCode, (ULONG)nRemCodeSize);
#if defined(_M_IX86)
    }
#endif //_M_IX86
  }
  //change main thread's entrypoint
  if (dwOsErr == ERROR_SUCCESS)
  {
    switch (nProcPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
#if defined(_M_IX86)
        sCtx.Eip = (DWORD)(lpRemCode + 8 * sizeof(DWORD));
        nNtStatus = NktNtSetContextThread(hMainThread, &sCtx);
#elif defined(_M_X64)
        sWow64Ctx.Eip = (DWORD)((ULONG_PTR)(lpRemCode + 8 * sizeof(DWORD)));
        if (fnRtlWow64GetThreadContext != NULL && fnRtlWow64SetThreadContext != NULL)
        {
          nNtStatus = fnRtlWow64SetThreadContext(hMainThread, &sWow64Ctx);
        }
        else
        {
          if (NktHookLibHelpers::WriteMem(hMainThread, lpWow64Ctx, &sWow64Ctx, sizeof(sWow64Ctx)) != FALSE)
            nNtStatus = STATUS_ACCESS_DENIED;
        }
#endif
        if (!NT_SUCCESS(nNtStatus))
          dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
        break;

      case NKTHOOKLIB_ProcessPlatformX64:
#if defined(_M_IX86)
        sCtx64.Rip = (DWORD64)(nRemCode64 + (ULONGLONG)(8 * sizeof(ULONGLONG)));
        nNtStatus = NktHookLib::Internals::NtSetContextThread64(hMainThread, &sCtx64);
#elif defined(_M_X64)
        sCtx.Rip = (DWORD64)(lpRemCode + 8 * sizeof(ULONGLONG));
        nNtStatus = NktNtSetContextThread(hMainThread, &sCtx);
#endif
        if (!NT_SUCCESS(nNtStatus))
          dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
        break;
    }
  }
  //done
  if (dwOsErr != ERROR_SUCCESS)
  {
    //cleanup on error
    if (hRemoteCheckPointEvent != NULL)
    {
      NktNtDuplicateObject(hProcess, hRemoteCheckPointEvent, hProcess, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
    }
    if (lpRemCode != NULL)
    {
      SIZE_T nSize = 0;

#if defined(_M_IX86)
      if (nProcPlatform == NKTHOOKLIB_ProcessPlatformX64)
        NktHookLib::Internals::NtFreeVirtualMemory64(hProcess, &nRemCode64, &nSize, MEM_RELEASE);
      else
#endif //_M_IX86
        NktNtFreeVirtualMemory(hProcess, (PVOID*)&lpRemCode, &nSize, MEM_RELEASE);
    }
  }
  return dwOsErr;
}

static DWORD CreateThreadInRunningProcess(__in HANDLE hProcess, __in LPVOID lpCodeStart, __in LPVOID lpThreadParam,
                                          __out LPHANDLE lphNewThread)
{
  typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
  } CLIENT_ID;
  typedef HANDLE (WINAPI *lpfnCreateRemoteThread)(__in HANDLE hProcess, __in LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                                  __in SIZE_T dwStackSize, __in LPTHREAD_START_ROUTINE lpStartAddress,
                                                  __in LPVOID lpParameter, __in DWORD dwCreationFlags,
                                                  __out LPDWORD lpThreadId);
  typedef NTSTATUS (WINAPI *lpfnNtCreateThreadEx)(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess,
                                                  __in LPVOID ObjectAttributes, __in HANDLE ProcessHandle,
                                                  __in LPTHREAD_START_ROUTINE lpStartAddress, __in LPVOID lpParameter,
                                                  __in ULONG CreateFlags, __in SIZE_T ZeroBits,
                                                  __in SIZE_T SizeOfStackCommit, __in SIZE_T SizeOfStackReserve,
                                                  __in LPVOID lpBytesBuffer);
  lpfnNtCreateThreadEx fnNtCreateThreadEx;
  lpfnCreateRemoteThread fnCreateRemoteThread;
  HINSTANCE hNtDll, hKernel32Dll;
  DWORD dwOsErr;
  HANDLE hThreadToken;
  NTSTATUS nNtStatus;

  NKT_ASSERT(lphNewThread != NULL);
  *lphNewThread = NULL;
  fnNtCreateThreadEx = NULL;
  fnCreateRemoteThread = NULL;
  //locate needed functions
  hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
  if (hNtDll != NULL)
  {
    fnNtCreateThreadEx = (lpfnNtCreateThreadEx)NktHookLibHelpers::GetProcedureAddress(hNtDll, "NtCreateThreadEx");
  }
  hKernel32Dll = NktHookLibHelpers::GetModuleBaseAddress(L"kernel32.dll");
  if (hKernel32Dll != NULL)
  {
    fnCreateRemoteThread = (lpfnCreateRemoteThread)NktHookLibHelpers::GetProcedureAddress(hKernel32Dll,
                                                                                          "CreateRemoteThread");
  }
  if (fnNtCreateThreadEx == NULL && fnCreateRemoteThread == NULL)
    return ERROR_PROC_NOT_FOUND;
  //create remote thread using 'NtCreateThreadEx' if available
  dwOsErr = 0xFFFFFFFFUL;
  if (fnNtCreateThreadEx != NULL)
  {
    nNtStatus = fnNtCreateThreadEx(lphNewThread, 0x001FFFFFUL, NULL, hProcess, (LPTHREAD_START_ROUTINE)lpCodeStart,
                                    lpThreadParam, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, 0, NULL, NULL, NULL);
    dwOsErr = (NT_SUCCESS(nNtStatus)) ? ERROR_SUCCESS : NktRtlNtStatusToDosError(nNtStatus);
  }
  //on error, create remote thread using 'CreateRemoteThread' if available
  if (dwOsErr != ERROR_SUCCESS)
  {
    DWORD dwTid;

    *lphNewThread = fnCreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpCodeStart, lpThreadParam,
                                         CREATE_SUSPENDED, &dwTid);
    dwOsErr = (*lphNewThread != NULL) ? ERROR_SUCCESS : NktHookLibHelpers::GetWin32LastError();
  }
  //on success, set restricted token if needed
  if (dwOsErr == ERROR_SUCCESS)
  {
    nNtStatus = GenerateRestrictedThreadToken(hProcess, &hThreadToken);
    //set thread's restricted token if needed
    if (NT_SUCCESS(nNtStatus) && hThreadToken != NULL)
    {
      nNtStatus = NktNtSetInformationThread(*lphNewThread, (THREADINFOCLASS)ThreadImpersonationToken, &hThreadToken,
                                            sizeof(HANDLE));
    }
    if (!NT_SUCCESS(nNtStatus))
      dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  }
  //done
  return dwOsErr;
}

#if defined(_M_IX86)
static DWORD CreateThreadInRunningProcess64(__in HANDLE hProcess, __in ULONGLONG lpCodeStart,
                                            __in ULONGLONG lpThreadParam, __out LPHANDLE lphNewThread)
{
  typedef struct _CLIENT_ID {
    ULONGLONG UniqueProcess;
    ULONGLONG UniqueThread;
  } CLIENT_ID64;
  DWORD dwOsErr;
  HANDLE hThreadToken;
  NTSTATUS nNtStatus;

  NKT_ASSERT(lphNewThread != NULL);
  *lphNewThread = NULL;
  //create remote thread using 'NtCreateThreadEx' if available
  dwOsErr = 0xFFFFFFFFUL;
  nNtStatus = NktHookLib::Internals::NtCreateThreadEx64(lphNewThread, 0x001FFFFFUL, NULL, hProcess, lpCodeStart,
                                                        lpThreadParam, THREAD_CREATE_FLAGS_CREATE_SUSPENDED, 0, NULL,
                                                        NULL, NULL);
  dwOsErr = (NT_SUCCESS(nNtStatus)) ? ERROR_SUCCESS : NktRtlNtStatusToDosError(nNtStatus);
  //on success, set restricted token if needed
  if (dwOsErr == ERROR_SUCCESS)
  {
    nNtStatus = GenerateRestrictedThreadToken(hProcess, &hThreadToken);
    //set thread's restricted token if needed
    if (NT_SUCCESS(nNtStatus) && hThreadToken != NULL)
    {
      ULONGLONG nThreadToken64 = (ULONGLONG)((LONGLONG)((LONG)hThreadToken));
      nNtStatus = NktHookLib::Internals::NtSetInformationThread64(*lphNewThread,
                                                                  (THREADINFOCLASS)ThreadImpersonationToken,
                                                                  &nThreadToken64, sizeof(nThreadToken64));
    }
    if (!NT_SUCCESS(nNtStatus))
      dwOsErr = NktRtlNtStatusToDosError(nNtStatus);
  }
  //done
  return dwOsErr;

}
#endif //_M_IX86

static NTSTATUS GenerateRestrictedThreadToken(__in HANDLE hProcess, __out HANDLE *lphThreadToken)
{
  typedef NTSTATUS (NTAPI *lpfnRtlCreateAcl)(__out PACL Acl, __in ULONG AclLength, __in ULONG AceRevision);
  typedef NTSTATUS (NTAPI *lpfnRtlAddAccessAllowedAce)(__in PACL Acl, __in ULONG Revision, __in ACCESS_MASK AccessMask,
                                                       __in PSID Sid);
  lpfnRtlCreateAcl fnRtlCreateAcl;
  lpfnRtlAddAccessAllowedAce fnRtlAddAccessAllowedAce;
  HANDLE hToken, hNewToken;
  TOKEN_DEFAULT_DACL sNewDefDacl;
  MANDATORY_LEVEL nLevel;
  TNktAutoFreePtr<TOKEN_USER> cUser;
  TNktAutoFreePtr<TOKEN_GROUPS> cGroups;
  TNktAutoFreePtr<ACL> cAcl;
  OBJECT_ATTRIBUTES sObjAttr;
  SECURITY_QUALITY_OF_SERVICE sSqos;
  NKT_SID sSid;
  ULONG nSize;
  DWORD dwOsVerMajor, dwOsVerMinor;
  HINSTANCE hNtDll;
  NTSTATUS nNtStatus;

  *lphThreadToken = NULL;
  //check OS version
  if (NktHookLibHelpers::GetOsVersion(&dwOsVerMajor, &dwOsVerMinor) == FALSE)
    return STATUS_SUCCESS;
  if (dwOsVerMajor < 5 || (dwOsVerMajor == 5 && dwOsVerMinor < 1))
    return STATUS_SUCCESS; //only on XP or later
  //open process token
  nNtStatus = NktNtOpenProcessToken(hProcess, TOKEN_QUERY, &hToken);
  //query for restricted sids
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = QueryTokenInfo(hToken, TokenRestrictedSids, (LPVOID*)&cGroups);
  //errors?
  if (!NT_SUCCESS(nNtStatus))
  {
    NktNtClose(hToken);
    return nNtStatus;
  }
  //has none?
  if (cGroups->GroupCount == 0)
  {
    NktNtClose(hToken);
    return STATUS_SUCCESS;
  }
  //we need to create a restricted token, first get needed apis
  hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
  if (hNtDll == NULL)
  {
    NktNtClose(hToken);
    return STATUS_PROCEDURE_NOT_FOUND;
  }
  fnRtlCreateAcl = (lpfnRtlCreateAcl)NktHookLibHelpers::GetProcedureAddress(hNtDll, "RtlCreateAcl");
  fnRtlAddAccessAllowedAce = (lpfnRtlAddAccessAllowedAce)NktHookLibHelpers::GetProcedureAddress(hNtDll,
                                                                "RtlAddAccessAllowedAce");
  if (fnRtlCreateAcl == NULL || fnRtlAddAccessAllowedAce == NULL)
  {
    NktNtClose(hToken);
    return STATUS_PROCEDURE_NOT_FOUND;
  }
  //get integrity level
  if (dwOsVerMajor >= 6)
  {
    nNtStatus = GetTokenIntegrityLevel(hToken, &nLevel);
    if (!NT_SUCCESS(nNtStatus))
    {
      NktNtClose(hToken);
      return nNtStatus;
    }
  }
  NktNtClose(hToken);
  //get process token
  nNtStatus = NktNtOpenProcessToken(NKTHOOKLIB_CurrentProcess, TOKEN_ALL_ACCESS_P, &hToken);
  if (NT_SUCCESS(nNtStatus))
  {
    nNtStatus = MyCreateRestrictedToken(hToken, &hNewToken);
    if (NT_SUCCESS(nNtStatus))
    {
      NktNtClose(hToken);
      hToken = hNewToken;
    }
  }
  //get user
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = QueryTokenInfo(hToken, TokenUser, (LPVOID*)&cUser);
  //create wellknown restricted SID and new ACEs
  if (NT_SUCCESS(nNtStatus))
  {
    NktHookLibHelpers::MemSet(&sSid, 0, sizeof(sSid));
    sSid.Revision = SID_REVISION;
    sSid.IdentifierAuthority.Value[5] = 5; //SECURITY_NT_AUTHORITY
    sSid.SubAuthorityCount = 1;
    sSid.SubAuthority[0] = SECURITY_RESTRICTED_CODE_RID;
    //----
    nSize = (ULONG)(sizeof(ACL) + 2 * sizeof(NKT_ACE));
    nSize += (ULONG)FIELD_OFFSET(SID, SubAuthority[sSid.SubAuthorityCount]);
    nSize += (ULONG)FIELD_OFFSET(SID, SubAuthority[((NKT_SID*)(cUser->User.Sid))->SubAuthorityCount]);
    //----
    cAcl.Attach((ACL*)NktHookLibHelpers::MemAlloc(nSize));
    if (cAcl == NULL)
      nNtStatus = STATUS_NO_MEMORY;
  }
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = fnRtlCreateAcl(cAcl, (ULONG)nSize, ACL_REVISION);
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = fnRtlAddAccessAllowedAce(cAcl, ACL_REVISION, GENERIC_ALL, (PSID)&sSid);
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = fnRtlAddAccessAllowedAce(cAcl, ACL_REVISION, GENERIC_ALL, cUser->User.Sid);
  if (NT_SUCCESS(nNtStatus))
  {
    sNewDefDacl.DefaultDacl = cAcl;
    nNtStatus = NktNtSetInformationToken(hToken, TokenDefaultDacl, &sNewDefDacl, sizeof(sNewDefDacl));
  }
  //set token's integrity level
  if (NT_SUCCESS(nNtStatus) && dwOsVerMajor >= 6)
    nNtStatus = SetTokenIntegrityLevel(hToken, nLevel);
  //duplicate token for impersonation
  if (NT_SUCCESS(nNtStatus))
  {
    NktHookLibHelpers::MemSet(&sSqos, 0, sizeof(sSqos));
    sSqos.Length = (DWORD)sizeof(SECURITY_QUALITY_OF_SERVICE);
    sSqos.ImpersonationLevel = SecurityImpersonation;
    NktHookLibHelpers::MemSet(&sObjAttr, 0, sizeof(sObjAttr));
    sObjAttr.Length = (ULONG)sizeof(sObjAttr);
    sObjAttr.SecurityQualityOfService = &sSqos;
    nNtStatus = NktNtDuplicateToken(hToken, TOKEN_IMPERSONATE|TOKEN_QUERY, &sObjAttr, FALSE, TokenImpersonation,
                                    &hNewToken);
    if (NT_SUCCESS(nNtStatus))
    {
      NktNtClose(hToken);
      hToken = hNewToken;
    }
  }
  //done
  if (NT_SUCCESS(nNtStatus))
    *lphThreadToken = hToken;
  else
    NktNtClose(hToken);
  return nNtStatus;
}

static NTSTATUS MyCreateRestrictedToken(__in HANDLE hToken, __out HANDLE *lphRestrictedToken)
{
  typedef NTSTATUS (NTAPI *lpfnNtFilterToken)(__in HANDLE ExistingTokenHandle, __in ULONG Flags,
                                              __in_opt PTOKEN_GROUPS SidsToDisable,
                                              __in_opt PTOKEN_PRIVILEGES PrivilegesToDelete,
                                              __in_opt PTOKEN_GROUPS RestrictedSids, __out PHANDLE NewTokenHandle);
  HINSTANCE hNtDll;
  lpfnNtFilterToken fnNtFilterToken;
  TNktAutoFreePtr<TOKEN_USER> cUser;
  TNktAutoFreePtr<TOKEN_GROUPS> cGroups, cRestricted;
  DWORD i, k;
  NTSTATUS nNtStatus;

  *lphRestrictedToken = NULL;
  //get needed apis
  hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
  if (hNtDll == NULL)
    return STATUS_PROCEDURE_NOT_FOUND;
  fnNtFilterToken = (lpfnNtFilterToken)NktHookLibHelpers::GetProcedureAddress(hNtDll, "NtFilterToken");
  if (fnNtFilterToken == NULL)
    return STATUS_PROCEDURE_NOT_FOUND;
  //first get the restricting SIDs
  //get token user
  nNtStatus = QueryTokenInfo(hToken, TokenUser, (LPVOID*)&cUser);
  //get token groups
  if (NT_SUCCESS(nNtStatus))
    nNtStatus = QueryTokenInfo(hToken, TokenGroups, (LPVOID*)&cGroups);
  //count and allocate for sid & attributes
  if (NT_SUCCESS(nNtStatus))
  {
    for (i=k=0; i<cGroups->GroupCount; i++)
    {
      if ((cGroups->Groups[i].Attributes & SE_GROUP_INTEGRITY) == 0)
        k++;
    }
    cRestricted.Attach((PTOKEN_GROUPS)NktHookLibHelpers::MemAlloc(sizeof(DWORD)+(k+1)*sizeof(SID_AND_ATTRIBUTES)));
    if (cRestricted == NULL)
      return STATUS_NO_MEMORY;
  }
  if (NT_SUCCESS(nNtStatus))
  {
    cRestricted->GroupCount = k + 1;
    cRestricted->Groups[0].Attributes = 0;
    cRestricted->Groups[0].Sid = cUser->User.Sid;
    for (i=k=0; i<cGroups->GroupCount; i++)
    {
      if ((cGroups->Groups[i].Attributes & SE_GROUP_INTEGRITY) == 0)
      {
        k++;
        cRestricted->Groups[k].Attributes = 0;
        cRestricted->Groups[k].Sid = cGroups->Groups[i].Sid;
      }
    }
    //----
    nNtStatus = fnNtFilterToken(hToken, SANDBOX_INERT, NULL, NULL, cRestricted.Get(), lphRestrictedToken);
  }
  return nNtStatus;
}

static NTSTATUS SetTokenIntegrityLevel(__in HANDLE hToken, __in MANDATORY_LEVEL nLevel)
{
  NKT_SID sSid;
  TOKEN_MANDATORY_LABEL sLabel;
  ULONG nSize;

  NktHookLibHelpers::MemSet(&sLabel, 0, sizeof(sLabel));
  sLabel.Label.Attributes = SE_GROUP_INTEGRITY;
  sLabel.Label.Sid = (SID*)&sSid;
  NktHookLibHelpers::MemSet(&sSid, 0, sizeof(sSid));
  sSid.Revision = SID_REVISION;
  sSid.IdentifierAuthority.Value[5] = 16; //SECURITY_MANDATORY_LABEL_AUTHORITY
  sSid.SubAuthorityCount = 1;
  switch (nLevel)
  {
    case MandatoryLevelSecureProcess:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_SYSTEM_RID;
      break;
    case MandatoryLevelSystem:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_SYSTEM_RID;
      break;
    case MandatoryLevelHigh:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_HIGH_RID;
      break;
    case MandatoryLevelMedium:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_MEDIUM_RID;
      break;
    case MandatoryLevelLow:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_LOW_RID;
      break;
    case MandatoryLevelUntrusted:
      sSid.SubAuthority[0] = SECURITY_MANDATORY_UNTRUSTED_RID;
      break;
    default:
      return STATUS_INVALID_PARAMETER;
  }
  nSize = (ULONG)sizeof(TOKEN_MANDATORY_LABEL) + (ULONG)FIELD_OFFSET(SID, SubAuthority[sSid.SubAuthorityCount]);
  return NktNtSetInformationToken(hToken, TokenIntegrityLevel, &sLabel, nSize);
}

static NTSTATUS GetTokenIntegrityLevel(__in HANDLE hToken, __out MANDATORY_LEVEL *lpnLevel)
{
  TNktAutoFreePtr<TOKEN_MANDATORY_LABEL> cIntegrityLevel;
  NKT_SID *lpSid;
  DWORD dwIntegrityLevel;
  NTSTATUS nNtStatus;

  *lpnLevel = MandatoryLevelUntrusted;
  //query for restricted sids
  nNtStatus = QueryTokenInfo(hToken, TokenIntegrityLevel, (LPVOID*)&cIntegrityLevel);
  if (NT_SUCCESS(nNtStatus))
  {
    lpSid = (NKT_SID*)(cIntegrityLevel->Label.Sid);
    dwIntegrityLevel = lpSid->SubAuthority[lpSid->SubAuthorityCount - 1];
    if (dwIntegrityLevel < SECURITY_MANDATORY_LOW_RID)
      *lpnLevel = MandatoryLevelUntrusted;
    else if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
      *lpnLevel = MandatoryLevelLow;
    else if (dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
      *lpnLevel = MandatoryLevelMedium;
    else if (dwIntegrityLevel < SECURITY_MANDATORY_SYSTEM_RID)
      *lpnLevel = MandatoryLevelHigh;
    else if (dwIntegrityLevel < SECURITY_MANDATORY_PROTECTED_PROCESS_RID)
      *lpnLevel = MandatoryLevelSystem;
    else
      *lpnLevel = MandatoryLevelSecureProcess;
  }
  return nNtStatus;
}

static NTSTATUS QueryTokenInfo(__in HANDLE hToken, __in TOKEN_INFORMATION_CLASS nClass, __out LPVOID *lplpInfo)
{
  TNktAutoFreePtr<BYTE> cBuf;
  ULONG nRetLength;
  NTSTATUS nNtStatus;

  *lplpInfo = NULL;
  nNtStatus = NktNtQueryInformationToken(hToken, nClass, NULL, 0, &nRetLength);
  while (nNtStatus == STATUS_BUFFER_TOO_SMALL)
  {
    cBuf.Attach((LPBYTE)NktHookLibHelpers::MemAlloc((SIZE_T)nRetLength));
    if (cBuf == NULL)
      return STATUS_NO_MEMORY;
    nNtStatus = NktNtQueryInformationToken(hToken, nClass, cBuf.Get(), nRetLength, &nRetLength);
  }
  if (NT_SUCCESS(nNtStatus))
    *lplpInfo = cBuf.Detach();
  return nNtStatus;
}

static NTSTATUS GetPrimaryThread(__in HANDLE hProcess, __out HANDLE *lphThread)
{
  TNktAutoFreePtr<NKT_HK_SYSTEM_PROCESS_INFORMATION> cBuf;
  PROCESS_BASIC_INFORMATION sPbi;
  SIZE_T nSize, nMethod;
  LPNKT_HK_SYSTEM_PROCESS_INFORMATION lpCurrProc;
  ULONG nRetLength;
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  NTSTATUS nNtStatus;

  nNtStatus = NktNtQueryInformationProcess(hProcess, ProcessBasicInformation, &sPbi, (ULONG)sizeof(sPbi), NULL);
  if (!NT_SUCCESS(nNtStatus))
    return NktRtlNtStatusToDosError(nNtStatus);
  nNtStatus = STATUS_NOT_FOUND;
  for (nMethod=0; nMethod<2; nMethod++)
  {
    nNtStatus = NktNtQuerySystemInformation((nMethod == 0) ? (SYSTEM_INFORMATION_CLASS)MySystemProcessInformation :
                                            (SYSTEM_INFORMATION_CLASS)MySystemExtendedProcessInformation,
                                            NULL, 0, &nRetLength);
    while (nNtStatus == STATUS_INFO_LENGTH_MISMATCH || nNtStatus == STATUS_BUFFER_TOO_SMALL)
    {
      cBuf.Attach((LPNKT_HK_SYSTEM_PROCESS_INFORMATION)NktHookLibHelpers::MemAlloc((SIZE_T)nRetLength));
      if (cBuf == NULL)
        return STATUS_NO_MEMORY;
      nNtStatus = NktNtQuerySystemInformation((nMethod == 0) ? (SYSTEM_INFORMATION_CLASS)MySystemProcessInformation :
                                              (SYSTEM_INFORMATION_CLASS)MySystemExtendedProcessInformation,
                                              cBuf.Get(), nRetLength, &nRetLength);
      if ((nNtStatus == STATUS_INFO_LENGTH_MISMATCH || nNtStatus == STATUS_BUFFER_TOO_SMALL) &&
          nRetLength == 0)
        nNtStatus = STATUS_NOT_FOUND;
    }
    if (NT_SUCCESS(nNtStatus))
      break;
  }
  if (nMethod >= 2)
    return nNtStatus;
  //find process
  lpCurrProc = cBuf.Get();
  nSize = (SIZE_T)FIELD_OFFSET(NKT_HK_SYSTEM_PROCESS_INFORMATION, Threads);
  nSize += (nMethod == 0) ? sizeof(NKT_HK_SYSTEM_THREAD_INFORMATION) :
                            sizeof(NKT_HK_SYSTEM_EXTENDED_THREAD_INFORMATION);
  while ((SIZE_T)lpCurrProc + nSize - (SIZE_T)cBuf.Get() <= (SIZE_T)nRetLength)
  {
    if ((DWORD)((ULONG_PTR)(lpCurrProc->UniqueProcessId)) == (DWORD)((ULONG_PTR)(sPbi.UniqueProcessId)))
    {
      if (lpCurrProc->NumberOfThreads == 0)
        break;
      sClientId.UniqueProcess = 0;
      sClientId.UniqueThread = (nMethod == 0) ? (lpCurrProc->Threads[0].ClientId.UniqueThread) :
                                                (lpCurrProc->ExtThreads[0].ThreadInfo.ClientId.UniqueThread);
      InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
      nNtStatus = NktNtOpenThread(lphThread, STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | THREAD_TERMINATE |
                                             THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_SET_CONTEXT |
                                             THREAD_QUERY_INFORMATION | THREAD_SET_INFORMATION |
                                             THREAD_SET_THREAD_TOKEN | THREAD_IMPERSONATE |
                                             THREAD_DIRECT_IMPERSONATION, &sObjAttr, &sClientId);
      return nNtStatus;
    }
    if (lpCurrProc->NextEntryOffset == 0)
      break;
    lpCurrProc = (LPNKT_HK_SYSTEM_PROCESS_INFORMATION)((LPBYTE)lpCurrProc + (SIZE_T)(lpCurrProc->NextEntryOffset));
  }
  //process not found
  return STATUS_NOT_FOUND;
}

#if defined(_M_IX86)
static SIZE_T ReadMem64(__in HANDLE hProcess, __in LPVOID lpDest, __in ULONGLONG lpSrc, __in SIZE_T nBytesCount)
{
  NTSTATUS nStatus;
  SIZE_T nReaded;

  if (nBytesCount == 0)
    return 0;
  nStatus = NktHookLib::Internals::NtReadVirtualMemory64(hProcess, lpSrc, lpDest, nBytesCount, &nReaded);
  if (nStatus == STATUS_PARTIAL_COPY)
    return nReaded;
  return (NT_SUCCESS(nStatus)) ? nBytesCount : 0;
}

static BOOL WriteMem64(__in HANDLE hProcess, __in ULONGLONG lpDest, __in LPVOID lpSrc, __in SIZE_T nBytesCount)
{
  NTSTATUS nStatus;
  SIZE_T nWritten;

  if (nBytesCount == 0)
    return TRUE;
  nStatus = NktHookLib::Internals::NtWriteVirtualMemory64(hProcess, lpDest, lpSrc, nBytesCount, &nWritten);
  return (NT_SUCCESS(nStatus) ||
          (nStatus == STATUS_PARTIAL_COPY && nWritten == nBytesCount)) ? TRUE : FALSE;
}
#endif //_M_IX86
