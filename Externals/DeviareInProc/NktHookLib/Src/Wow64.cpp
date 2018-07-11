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
#include "Wow64.h"
#include "WaitableObjects.h"

//-----------------------------------------------------------

#define Handle2Ull(_param) (ULONGLONG)((LONGLONG)((LONG)(_param)))
#define Ptr2Ull(_param)    (ULONGLONG)((SIZE_T)(_param))
#define Ul2Ull(_param)     (ULONGLONG)((ULONG)(_param))

//-----------------------------------------------------------

#if defined(_M_IX86)
extern "C" {
extern ULONGLONG NktHookLib_fnNtQueryInformationProcess64;
extern ULONGLONG NktHookLib_fnNtQueryInformationThread64;
extern ULONGLONG NktHookLib_fnNtSetInformationThread64;
extern ULONGLONG NktHookLib_fnNtCreateThreadEx64;
extern ULONGLONG NktHookLib_fnNtGetContextThread64;
extern ULONGLONG NktHookLib_fnNtSetContextThread64;
extern ULONGLONG NktHookLib_fnNtAllocateVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtFreeVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtFlushVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtReadVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtWriteVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtProtectVirtualMemory64;
extern ULONGLONG NktHookLib_fnNtFlushInstructionCache64;
extern ULONGLONG NktHookLib_fnNtQueueApcThread64;
}; //extern "C"
#endif //_M_IX86

#if defined(_M_IX86)
static LONG volatile nMutex = 0;
static LONG volatile nInitialized = 0;
#endif //_M_IX86

//-----------------------------------------------------------

#if defined(_M_IX86)
extern "C" {

int __stdcall NktHookLib_InitializeWow64();
NTSTATUS __stdcall NktHookLib_CallWow64(__in ULONGLONG nApiAddr, __in ULONGLONG nParam1, __in ULONGLONG nParam2,
                                        __in ULONGLONG nParam3, __in ULONGLONG nParam4, __in ULONGLONG nParam5,
                                        __in ULONGLONG nParam6, __in ULONGLONG nParam7, __in ULONGLONG nParam8,
                                        __in ULONGLONG nParam9, __in ULONGLONG nParam10, __in ULONGLONG nParam11);

}; //extern "C"

static VOID Initialize();
#endif //_M_IX86

//-----------------------------------------------------------

namespace NktHookLib {
namespace Internals {

NTSTATUS NtQueryInformationProcess64(__in HANDLE Process, __in PROCESSINFOCLASS ProcessInfoClass,
                                     __out_opt PVOID ProcessInfo, __in ULONG ProcessInfoLength,
                                     __out_opt PULONG ReturnLength)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtQueryInformationProcess64 == 0ui64)
  {
    if (ProcessInfo != NULL)
      NktHookLibHelpers::MemSet(ProcessInfo, 0, ProcessInfoLength);
    if (ReturnLength != NULL)
      *ReturnLength = 0;
    return STATUS_NOT_IMPLEMENTED;
  }
  return NktHookLib_CallWow64(NktHookLib_fnNtQueryInformationProcess64, Handle2Ull(Process), Ul2Ull(ProcessInfoClass),
                              Ptr2Ull(ProcessInfo), Ul2Ull(ProcessInfoLength), Ptr2Ull(ReturnLength),
                              0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtQueryInformationProcess(Process, ProcessInfoClass, ProcessInfo, ProcessInfoLength, ReturnLength);
#endif
}

NTSTATUS NtQueryInformationThread64(__in HANDLE ThreadHandle, __in THREADINFOCLASS ThreadInfoClass,
                                    __out_opt PVOID ThreadInfo, __in ULONG ThreadInfoLength,
                                    __out_opt PULONG ReturnLength)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtQueryInformationThread64 == 0ui64)
  {
    if (ThreadInfo != NULL)
      NktHookLibHelpers::MemSet(ThreadInfo, 0, ThreadInfoLength);
    if (ReturnLength != NULL)
      *ReturnLength = 0;
    return STATUS_NOT_IMPLEMENTED;
  }
  return NktHookLib_CallWow64(NktHookLib_fnNtQueryInformationThread64, Handle2Ull(ThreadHandle),
                              Ul2Ull(ThreadInfoClass), Ptr2Ull(ThreadInfo), Ul2Ull(ThreadInfoLength),
                              Ptr2Ull(ReturnLength), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtQueryInformationThread(ThreadHandle, ThreadInfoClass, ThreadInfo, ThreadInfoLength, ReturnLength);
#endif
}

NTSTATUS NtSetInformationThread64(__in HANDLE ThreadHandle, __in THREADINFOCLASS ThreadInfoClass,
                                  __in PVOID ThreadInfo, __in ULONG ThreadInfoLength)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtSetInformationThread64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  return NktHookLib_CallWow64(NktHookLib_fnNtSetInformationThread64, Handle2Ull(ThreadHandle), Ul2Ull(ThreadInfoClass),
                              Ptr2Ull(ThreadInfo), Ul2Ull(ThreadInfoLength), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64,
                              0ui64);
#elif defined(_M_X64)
  return NktNtSetInformationThread(ThreadHandle, ThreadInfoClass, ThreadInfo, ThreadInfoLength);
#endif
}


NTSTATUS NtCreateThreadEx64(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess, __in LPVOID ObjectAttributes,
                            __in HANDLE ProcessHandle, __in ULONGLONG lpStartAddress, __in ULONGLONG lpParameter,
                            __in ULONG CreateFlags, __in ULONGLONG ZeroBits, __in ULONGLONG SizeOfStackCommit,
                            __in ULONGLONG SizeOfStackReserve, __in ULONGLONG lpBytesBuffer)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtCreateThreadEx64 == 0ui64)
  {
    if (ThreadHandle != NULL)
      NktHookLibHelpers::MemSet(ThreadHandle, 0, sizeof(HANDLE));
    return STATUS_NOT_IMPLEMENTED;
  }
  return NktHookLib_CallWow64(NktHookLib_fnNtCreateThreadEx64, Ptr2Ull(ThreadHandle), Ul2Ull(DesiredAccess),
                              Ptr2Ull(ObjectAttributes), Handle2Ull(ProcessHandle), lpStartAddress, lpParameter,
                              Ul2Ull(CreateFlags), ZeroBits, SizeOfStackCommit, SizeOfStackReserve, lpBytesBuffer);
#elif defined(_M_X64)
  typedef NTSTATUS (WINAPI *lpfnNtCreateThreadEx)(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess,
                                                  __in LPVOID ObjectAttributes, __in HANDLE ProcessHandle,
                                                  __in LPTHREAD_START_ROUTINE lpStartAddress, __in LPVOID lpParameter,
                                                  __in ULONG CreateFlags, __in SIZE_T ZeroBits,
                                                  __in SIZE_T SizeOfStackCommit, __in SIZE_T SizeOfStackReserve,
                                                  __in LPVOID lpBytesBuffer);

  static lpfnNtCreateThreadEx fnNtCreateThreadEx = NULL;

  if (fnNtCreateThreadEx == NULL)
  {
    HINSTANCE hNtDll = NktHookLibHelpers::GetModuleBaseAddress(L"ntdll.dll");
    if (hNtDll != NULL)
    {
      lpfnNtCreateThreadEx _fnNtCreateThreadEx = (lpfnNtCreateThreadEx)NktHookLibHelpers::GetProcedureAddress(hNtDll,
                                                                                             "NtCreateThreadEx");
      fnNtCreateThreadEx = (_fnNtCreateThreadEx != NULL) ? _fnNtCreateThreadEx : (lpfnNtCreateThreadEx)1;
    }
  }
  if (fnNtCreateThreadEx == (lpfnNtCreateThreadEx)1)
  {
    if (ThreadHandle != NULL)
      NktHookLibHelpers::MemSet(ThreadHandle, 0, sizeof(HANDLE));
    return STATUS_NOT_IMPLEMENTED;
  }
  return fnNtCreateThreadEx(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle,
                            (LPTHREAD_START_ROUTINE)lpStartAddress, (LPVOID)lpParameter, CreateFlags, ZeroBits,
                            SizeOfStackCommit, SizeOfStackReserve, (LPVOID)lpBytesBuffer);
#endif
}

NTSTATUS NtGetContextThread64(__in HANDLE ThreadHandle, __out PCONTEXT64 Context)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtGetContextThread64 == 0ui64)
  {
    if (Context != NULL)
      NktHookLibHelpers::MemSet(Context, 0, sizeof(CONTEXT64));
    return STATUS_NOT_IMPLEMENTED;
  }
  return NktHookLib_CallWow64(NktHookLib_fnNtGetContextThread64, Handle2Ull(ThreadHandle), Ptr2Ull(Context),
                              0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtGetContextThread(ThreadHandle, (PCONTEXT)Context);
#endif
}

NTSTATUS NtSetContextThread64(__in HANDLE ThreadHandle, __in PCONTEXT64 Context)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtSetContextThread64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  return NktHookLib_CallWow64(NktHookLib_fnNtSetContextThread64, Handle2Ull(ThreadHandle), Ptr2Ull(Context),
                              0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtSetContextThread(ThreadHandle, (PCONTEXT)Context);
#endif
}

NTSTATUS NtAllocateVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __in ULONGLONG ZeroBits,
                                   __inout PSIZE_T RegionSize, __in ULONG AllocationType, __in ULONG Protect)
{
#if defined(_M_IX86)
  ULONGLONG nRegionSize;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtAllocateVirtualMemory64 == 0ui64)
  {
    if (BaseAddress != NULL)
      *BaseAddress = 0;
    if (RegionSize != NULL)
      *RegionSize = 0;
    return STATUS_NOT_IMPLEMENTED;
  }
  nRegionSize = (ULONGLONG)(*RegionSize);
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtAllocateVirtualMemory64, Handle2Ull(ProcessHandle),
                                   Ptr2Ull(BaseAddress), ZeroBits, Ptr2Ull(&nRegionSize), Ul2Ull(AllocationType),
                                   Ul2Ull(Protect), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
  *RegionSize = (SIZE_T)nRegionSize;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtAllocateVirtualMemory(ProcessHandle, (PVOID*)BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
#endif
}

NTSTATUS NtFreeVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __inout PSIZE_T RegionSize,
                               __in ULONG FreeType)
{
#if defined(_M_IX86)
  ULONGLONG nRegionSize;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtFreeVirtualMemory64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  nRegionSize = (ULONGLONG)(*RegionSize);
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtFreeVirtualMemory64, Handle2Ull(ProcessHandle), Ptr2Ull(BaseAddress),
                                   Ptr2Ull(&nRegionSize), Ul2Ull(FreeType), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64,
                                   0ui64);
  *RegionSize = (SIZE_T)nRegionSize;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtFreeVirtualMemory(ProcessHandle, (PVOID*)BaseAddress, RegionSize, FreeType);
#endif
}

NTSTATUS NtFlushVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __inout PSIZE_T RegionSize,
                                __out PIO_STATUS_BLOCK64 IoStatus)
{
#if defined(_M_IX86)
  ULONGLONG nRegionSize;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtFlushVirtualMemory64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  nRegionSize = (ULONGLONG)(*RegionSize);
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtFlushVirtualMemory64, Handle2Ull(ProcessHandle), Ptr2Ull(BaseAddress),
                                   Ptr2Ull(&nRegionSize), Ptr2Ull(IoStatus), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64,
                                   0ui64);
  *RegionSize = (SIZE_T)nRegionSize;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtFlushVirtualMemory(ProcessHandle, (PVOID*)BaseAddress, RegionSize, (PIO_STATUS_BLOCK)IoStatus);
#endif
}

NTSTATUS NtReadVirtualMemory64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress, __out PVOID Buffer,
                               __in SIZE_T NumberOfBytesToRead, __out_opt PSIZE_T NumberOfBytesRead)
{
#if defined(_M_IX86)
  ULONGLONG nReaded;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtReadVirtualMemory64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtReadVirtualMemory64, Handle2Ull(ProcessHandle), BaseAddress,
                                   Ptr2Ull(Buffer), Ul2Ull(NumberOfBytesToRead), Ptr2Ull(&nReaded), 0ui64, 0ui64,
                                   0ui64, 0ui64, 0ui64, 0ui64);
  if (NumberOfBytesRead != NULL)
    *NumberOfBytesRead = NT_SUCCESS(nNtStatus) ? (SIZE_T)nReaded : 0;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtReadVirtualMemory(ProcessHandle, (PVOID)BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesRead);
#endif
}

NTSTATUS NtWriteVirtualMemory64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress, __in PVOID Buffer,
                                __in SIZE_T NumberOfBytesToWrite, __out_opt PSIZE_T NumberOfBytesWritten)
{
#if defined(_M_IX86)
  ULONGLONG nWritten;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtWriteVirtualMemory64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtWriteVirtualMemory64, Handle2Ull(ProcessHandle), BaseAddress,
                                   Ptr2Ull(Buffer), Ul2Ull(NumberOfBytesToWrite), Ptr2Ull(&nWritten), 0ui64, 0ui64,
                                   0ui64, 0ui64, 0ui64, 0ui64);
  if (NumberOfBytesWritten != NULL)
    *NumberOfBytesWritten = NT_SUCCESS(nNtStatus) ? (SIZE_T)nWritten : 0;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtWriteVirtualMemory(ProcessHandle, (PVOID)BaseAddress, Buffer, NumberOfBytesToWrite, NumberOfBytesWritten);
#endif
}

NTSTATUS NtProtectVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *UnsafeBaseAddress,
                                  __inout PSIZE_T UnsafeNumberOfBytesToProtect, __in ULONG NewAccessProtection,
                                  __out PULONG UnsafeOldAccessProtection)
{
#if defined(_M_IX86)
  ULONGLONG nBytesToProtect;
  NTSTATUS nNtStatus;

  Initialize();
  //----
  if (NktHookLib_fnNtProtectVirtualMemory64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  nBytesToProtect = (ULONGLONG)(*UnsafeNumberOfBytesToProtect);
  nNtStatus = NktHookLib_CallWow64(NktHookLib_fnNtProtectVirtualMemory64, Handle2Ull(ProcessHandle),
                                   Ptr2Ull(UnsafeBaseAddress), Ptr2Ull(nBytesToProtect), Ul2Ull(NewAccessProtection),
                                   Ptr2Ull(UnsafeOldAccessProtection), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
  *UnsafeNumberOfBytesToProtect = (SIZE_T)nBytesToProtect;
  return nNtStatus;
#elif defined(_M_X64)
  return NktNtProtectVirtualMemory(ProcessHandle, (PVOID*)UnsafeBaseAddress, UnsafeNumberOfBytesToProtect,
                                   NewAccessProtection, UnsafeOldAccessProtection);
#endif
}

NTSTATUS NtFlushInstructionCache64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress,
                                   __in SIZE_T NumberOfBytesToFlush)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtFlushInstructionCache64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  return NktHookLib_CallWow64(NktHookLib_fnNtFlushInstructionCache64, Handle2Ull(ProcessHandle), BaseAddress,
                              Ul2Ull(NumberOfBytesToFlush), 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtFlushInstructionCache(ProcessHandle, (PVOID)BaseAddress, NumberOfBytesToFlush);
#endif
}

NTSTATUS NtQueueApcThread64(__in HANDLE ThreadHandle, __in ULONGLONG ApcRoutine, __in ULONGLONG ApcArgument1,
                            __in ULONGLONG ApcArgument2, __in ULONGLONG ApcArgument3)
{
#if defined(_M_IX86)
  Initialize();
  //----
  if (NktHookLib_fnNtQueueApcThread64 == 0ui64)
    return STATUS_NOT_IMPLEMENTED;
  return NktHookLib_CallWow64(NktHookLib_fnNtQueueApcThread64, Handle2Ull(ThreadHandle), ApcRoutine,
                              ApcArgument1, ApcArgument2, ApcArgument3, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64, 0ui64);
#elif defined(_M_X64)
  return NktNtQueueApcThread(ThreadHandle, (PVOID)ApcRoutine, (PVOID)ApcArgument1, (PVOID)ApcArgument2,
                             (PVOID)ApcArgument3);
#endif
}

//-----------------------------------------------------------

} //Internals
} //NktHookLib

//-----------------------------------------------------------

#if defined(_M_IX86)
static VOID Initialize()
{
  if (nInitialized == 0)
  {
    NktHookLib::Internals::CNktSimpleLockNonReentrant cLock(&nMutex);

    if (nInitialized == 0)
    {
      LONG nResult = -1;

      //are we running in a X64 processor?
      if (NktHookLibHelpers::GetProcessorArchitecture() == PROCESSOR_ARCHITECTURE_AMD64)
      {
        if (NktHookLib_InitializeWow64() != 0)
          nResult = 1;
      }
      _InterlockedExchange(&nInitialized, nResult);
    }
  }
  return;
}
#endif //_M_IX86
/*
typedef VOID(NTAPI * PKNORMAL_ROUTINE)(IN PVOID NormalContext OPTIONAL, IN PVOID SystemArgument1 OPTIONAL, IN PVOID SystemArgument2 OPTIONAL)


Status = NtQueueApcThread(hThread,
  959                               (PKNORMAL_ROUTINE)BaseDispatchApc,
  960                               pfnAPC,
  961                               (PVOID)dwData,
  962                               (ActCtxInfo.dwFlags & 1) ?
  963                               INVALID_ACTIVATION_CONTEXT : ActCtxInfo.hActCtx);

  */