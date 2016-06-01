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

#pragma once

#include "EngDefines.h"

//-----------------------------------------------------------

#define NKT_DV_NTSTATUS LONG

#define NKT_DV_NTSTATUS_UNSUCCESSFUL             0xC0000001L
#define NKT_DV_NTSTATUS_INFO_LENGTH_MISMATCH     0xC0000004L
#define NKT_DV_NTSTATUS_BUFFER_TOO_SMALL         0xC0000023L

//-----------------------------------------------------------

HRESULT nktDvDynApis_IsWow64Process(__in HANDLE hProcess, __out LPBOOL lpbIsWow64Process);
BOOL nktDvDynApis_Is64BitProcess(__in HANDLE hProcess);
HRESULT nktDvDynApis_Wow64GetThreadContext(__in HANDLE hThread, __inout PWOW64_CONTEXT lpContext);
HRESULT nktDvDynApis_Wow64SetThreadContext(__in HANDLE hThread, __in CONST WOW64_CONTEXT *lpContext);
HRESULT nktDvDynApis_GetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo);
HRESULT nktDvDynApis_QueryFullProcessImageName(__in HANDLE hProcess,
                                               __out_ecount(dwSize) LPWSTR lpExeNameW, __in DWORD dwSize);
HRESULT nktDvDynApis_GetProcessImageFileNameW(__in HANDLE hProcess, __out_ecount(dwSize) LPWSTR lpExeNameW,
                                              __in DWORD dwSize);
HRESULT nktDvDynApis_NtQueryInformationProcess(__in HANDLE ProcessHandle, __in int ProcessInformationClass,
                                               __out LPVOID ProcessInformation,
                                               __in ULONG ProcessInformationLength,
                                               __out_opt PULONG ReturnLength);
HRESULT nktDvDynApis_GetModuleFileNameW(__in_opt HMODULE hModule, __out_ecount(nSize) LPWSTR lpFilename,
                                        __in DWORD nSize);
HRESULT nktDvDynApis_GetModuleFileNameExW(__in HANDLE hProcess, __in_opt HMODULE hModule,
                                          __out_ecount(nSize) LPWSTR lpFilename, __in DWORD nSize);
HRESULT nktDvDynApis_NtQueryInformationThread(__in HANDLE ThreadHandle, __in int ThreadInformationClass,
                          __out_bcount(ThreadInformationLength) PVOID ThreadInformation,
                          __in ULONG ThreadInformationLength, __out_opt PULONG ReturnLength);
HRESULT nktDvDynApis_NtQueryInformationFile(__in HANDLE FileHandle, __out PVOID IoStatusBlock,
                                            __out LPVOID FileInformation, __in ULONG Length,
                                            __in int FileInformationClass);
HRESULT nktDvDynApis_NtQueryObject(__in_opt HANDLE Handle, __in int ObjectInformationClass,
                                   __out_opt LPVOID ObjectInformation, __in ULONG ObjectInformationLength,
                                   __out_opt PULONG ReturnLength);
HRESULT nktDvDynApis_NtSuspendThread(__in HANDLE ThreadHandle);
HRESULT nktDvDynApis_NtResumeThread(__in HANDLE ThreadHandle);
HRESULT nktDvDynApis_NtQuerySystemInformation(__in int sic, __inout PVOID lpSI, __in ULONG uSysInfLength,
                                              __out_opt PULONG ReturnLength);

HRESULT nktDvDynApis_EnumProcessModules(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                        __in DWORD cb, __out LPDWORD lpcbNeeded);
HRESULT nktDvDynApis_EnumProcessModulesEx(__in HANDLE hProcess, __out_bcount(cb) HMODULE *lphModule,
                                          __in DWORD cb, __out LPDWORD lpcbNeeded,
                                          __in DWORD dwFilterFlag);

HRESULT nktDvDynApis_GetProcessId(__in HANDLE hProcess, __out LPDWORD lpdwPid);

HRESULT nktDvDynApis_NtQueryVirtualMemory(__in HANDLE ProcessHandle, __in PVOID BaseAddress,
                                          __in int MemoryInformationClass, __out PVOID MemoryInformation,
                                          __in SIZE_T MemoryInformationLength,
                                          __out_opt PSIZE_T ReturnLength);

HRESULT nktDvDynApis_LdrLockLoaderLock(__in ULONG Flags, __out_opt ULONG *Disposition,
                                       __out PVOID *Cookie);
HRESULT nktDvDynApis_LdrUnlockLoaderLock(__in ULONG Flags, __inout PVOID Cookie);

HRESULT nktDvDynApis_LdrFindEntryForAddress(__in PVOID Address, __out PVOID *lplpEntry);

HRESULT nktDvDynApis_RtlEnterCriticalSection(__inout PVOID lpRtlCritSection, __in BOOL bOnlyTry=FALSE);
HRESULT nktDvDynApis_RtlLeaveCriticalSection(__inout PVOID lpRtlCritSection);

HRESULT nktDvDynApis_LdrGetProcedureAddress(__in HMODULE ModuleHandle, __in LPCSTR szAddressA,
                                            __out PVOID *FunctionAddress);
HRESULT nktDvDynApis_RtlDllShutdownInProgress(__out BOOL *ShutdownInProgress);

HRESULT nktDvDynApis_QueryThreadCycleTime(__in HANDLE ThreadHandle, __out PULONG64 CycleTime);

#if defined _M_X64
HRESULT nktDvDynApis_RtlLookupFunctionEntry(__out PRUNTIME_FUNCTION *lplpRet, __in ULONG64 ControlPc,
                                            __out PULONG64 ImageBase, __inout_opt LPVOID HistoryTable);
HRESULT nktDvDynApis_RtlVirtualUnwind(__out_opt PEXCEPTION_ROUTINE *lplpRet, __in DWORD HandlerType,
                                      __in DWORD64 ImageBase, __in DWORD64 ControlPc,
                                      __in PRUNTIME_FUNCTION FunctionEntry, __inout PCONTEXT ContextRecord,
                                      __out PVOID *HandlerData, __out PDWORD64 EstablisherFrame,
                                      __inout_opt LPVOID ContextPointers);
HRESULT nktDvDynApis_RtlCaptureContext(__out PCONTEXT ContextRecord);

HRESULT nktDvDynApis_AddVectoredExceptionHandler(__out PVOID *lplpRet, __in ULONG FirstHandler,
                                                 __in PVECTORED_EXCEPTION_HANDLER VectoredHandler);
HRESULT nktDvDynApis_RemoveVectoredExceptionHandler(__in PVOID Handler);
#endif //_M_X64

HRESULT nktDvDynApis_AllocateVirtualMemory(__out PVOID *BaseAddress, __in SIZE_T RegionSize, __in ULONG Protect);
HRESULT nktDvDynApis_NtFreeVirtualMemory(__in PVOID BaseAddress);
HRESULT nktDvDynApis_NtProtectVirtualMemory(__in PVOID BaseAddress, __in SIZE_T BytesToProtect,
                                            __in ULONG NewAccessProtection, __out PULONG UnsafeOldAccessProtection);
HRESULT nktDvDynApis_NtFlushInstructionCache(__in PVOID BaseAddress, __in ULONG NumberOfBytesToFlush);
HRESULT nktDvDynApis_NtOpenThread(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess, __in DWORD dwTid);
HRESULT nktDvDynApis_NtClose(__in HANDLE Handle);
HRESULT nktDvDynApis_NtCreateEvent(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess, __in LPCWSTR szNameW,
                                   __in BOOLEAN ManualReset, __in BOOLEAN InitialState);
HRESULT nktDvDynApis_NtOpenEvent(__out PHANDLE EventHandle, __in ACCESS_MASK DesiredAccess, __in LPCWSTR szNameW);
HRESULT nktDvDynApis_NtResetEvent(__in HANDLE EventHandle);
HRESULT nktDvDynApis_NtSetEvent(__in HANDLE EventHandle);
HRESULT nktDvDynApis_NtGetContextThread(__in HANDLE ThreadHandle, __out PCONTEXT Context);
HRESULT nktDvDynApis_NtDelayExecution(__in DWORD dwMilliseconds);
HRESULT nktDvDynApis_SetThreadPriority(__in HANDLE hThread, __in int _nPriority);
HRESULT nktDvDynApis_GetThreadPriority(__in HANDLE hThread, __out int *lpnPriority);

HRESULT nktDvDynApis_DuplicateHandle(__in HANDLE hSourceProcessHandle, __in HANDLE hSourceHandle,
                                     __in HANDLE hTargetProcessHandle, __deref_out LPHANDLE lpTargetHandle,
                                     __in DWORD dwDesiredAccess, __in BOOL bInheritHandle, __in DWORD dwOptions);

//-----------------------------------------------------------

