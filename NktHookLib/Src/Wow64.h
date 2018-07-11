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

#ifndef _NKT_HOOKLIB_WOW64_H
#define _NKT_HOOKLIB_WOW64_H

#include <NktHookLib.h>
#include "DynamicNtApi.h"
#include "NtHeapBaseObj.h"

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#define CONTEXT_X64_FULL 0x10000B

//-----------------------------------------------------------

#pragma pack(8)
typedef struct DECLSPEC_ALIGN(16) _XSAVE_FORMAT64 {
  WORD   ControlWord;
  WORD   StatusWord;
  BYTE  TagWord;
  BYTE  Reserved1;
  WORD   ErrorOpcode;
  DWORD ErrorOffset;
  WORD   ErrorSelector;
  WORD   Reserved2;
  DWORD DataOffset;
  WORD   DataSelector;
  WORD   Reserved3;
  DWORD MxCsr;
  DWORD MxCsr_Mask;
  M128A FloatRegisters[8];
  M128A XmmRegisters[16];
  BYTE  Reserved4[96];
} XSAVE_FORMAT64, *PXSAVE_FORMAT64;

typedef struct DECLSPEC_ALIGN(16) _CONTEXT64 {
  DWORD64 P1Home;
  DWORD64 P2Home;
  DWORD64 P3Home;
  DWORD64 P4Home;
  DWORD64 P5Home;
  DWORD64 P6Home;
  // Control flags.
  DWORD ContextFlags;
  DWORD MxCsr;
  // Segment Registers and processor flags.
  WORD   SegCs;
  WORD   SegDs;
  WORD   SegEs;
  WORD   SegFs;
  WORD   SegGs;
  WORD   SegSs;
  DWORD EFlags;
  // Debug registers
  DWORD64 Dr0;
  DWORD64 Dr1;
  DWORD64 Dr2;
  DWORD64 Dr3;
  DWORD64 Dr6;
  DWORD64 Dr7;
  // Integer registers.
  DWORD64 Rax;
  DWORD64 Rcx;
  DWORD64 Rdx;
  DWORD64 Rbx;
  DWORD64 Rsp;
  DWORD64 Rbp;
  DWORD64 Rsi;
  DWORD64 Rdi;
  DWORD64 R8;
  DWORD64 R9;
  DWORD64 R10;
  DWORD64 R11;
  DWORD64 R12;
  DWORD64 R13;
  DWORD64 R14;
  DWORD64 R15;
  // Program counter.
  DWORD64 Rip;
  // Floating point state.
  union {
    XSAVE_FORMAT64 FltSave;
    struct {
      M128A Header[2];
      M128A Legacy[8];
      M128A Xmm0;
      M128A Xmm1;
      M128A Xmm2;
      M128A Xmm3;
      M128A Xmm4;
      M128A Xmm5;
      M128A Xmm6;
      M128A Xmm7;
      M128A Xmm8;
      M128A Xmm9;
      M128A Xmm10;
      M128A Xmm11;
      M128A Xmm12;
      M128A Xmm13;
      M128A Xmm14;
      M128A Xmm15;
    } DUMMYSTRUCTNAME;
  } DUMMYUNIONNAME;
  // Vector registers.
  M128A VectorRegister[26];
  DWORD64 VectorControl;
  // Special debug control registers.
  DWORD64 DebugControl;
  DWORD64 LastBranchToRip;
  DWORD64 LastBranchFromRip;
  DWORD64 LastExceptionToRip;
  DWORD64 LastExceptionFromRip;
} CONTEXT64, *PCONTEXT64;

typedef struct _PROCESS_BASIC_INFORMATION64 {
  DWORD ExitStatus;
  ULONGLONG PebBaseAddress;
  ULONGLONG AffinityMask;
  DWORD BasePriority;
  ULONGLONG UniqueProcessId;
  ULONGLONG InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64, *PPROCESS_BASIC_INFORMATION64;

typedef struct _IO_STATUS_BLOCK64 {
  union {
    NTSTATUS Status;
    ULONGLONG Pointer;
  } DUMMYUNIONNAME;
  ULONGLONG Information;
} IO_STATUS_BLOCK64, *PIO_STATUS_BLOCK64;
#pragma pack()

//-----------------------------------------------------------

NTSTATUS NtQueryInformationProcess64(__in HANDLE Process, __in PROCESSINFOCLASS ProcessInfoClass,
                                     __out_opt PVOID ProcessInfo, __in ULONG ProcessInfoLength,
                                     __out_opt PULONG ReturnLength);
NTSTATUS NtQueryInformationThread64(__in HANDLE ThreadHandle, __in THREADINFOCLASS ThreadInfoClass,
                                    __out_opt PVOID ThreadInfo, __in ULONG ThreadInfoLength,
                                    __out_opt PULONG ReturnLength);

NTSTATUS NtSetInformationThread64(__in HANDLE ThreadHandle, __in THREADINFOCLASS ThreadInfoClass,
                                  __in PVOID ThreadInfo, __in ULONG ThreadInfoLength);

NTSTATUS NtCreateThreadEx64(__out PHANDLE ThreadHandle, __in ACCESS_MASK DesiredAccess, __in LPVOID ObjectAttributes,
                            __in HANDLE ProcessHandle, __in ULONGLONG lpStartAddress, __in ULONGLONG lpParameter,
                            __in ULONG CreateFlags, __in ULONGLONG ZeroBits, __in ULONGLONG SizeOfStackCommit,
                            __in ULONGLONG SizeOfStackReserve, __in ULONGLONG lpBytesBuffer);

NTSTATUS NtGetContextThread64(__in HANDLE ThreadHandle, __out PCONTEXT64 Context);
NTSTATUS NtSetContextThread64(__in HANDLE ThreadHandle, __in PCONTEXT64 Context);

NTSTATUS NtAllocateVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __in ULONGLONG ZeroBits,
                                   __inout PSIZE_T RegionSize, __in ULONG AllocationType, __in ULONG Protect);

NTSTATUS NtFreeVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __inout PSIZE_T RegionSize,
                               __in ULONG FreeType);

NTSTATUS NtFlushVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *BaseAddress, __inout PSIZE_T RegionSize,
                                __out PIO_STATUS_BLOCK64 IoStatus);

NTSTATUS NtReadVirtualMemory64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress, __out PVOID Buffer,
                               __in SIZE_T NumberOfBytesToRead, __out_opt PSIZE_T NumberOfBytesRead);

NTSTATUS NtWriteVirtualMemory64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress, __in PVOID Buffer,
                                __in SIZE_T NumberOfBytesToWrite, __out_opt PSIZE_T NumberOfBytesWritten);

NTSTATUS NtProtectVirtualMemory64(__in HANDLE ProcessHandle, __inout ULONGLONG *UnsafeBaseAddress,
                                  __inout PSIZE_T UnsafeNumberOfBytesToProtect, __in ULONG NewAccessProtection,
                                  __out PULONG UnsafeOldAccessProtection);

NTSTATUS NtFlushInstructionCache64(__in HANDLE ProcessHandle, __in ULONGLONG BaseAddress,
                                   __in SIZE_T NumberOfBytesToFlush);

NTSTATUS NtQueueApcThread64(__in HANDLE ThreadHandle, __in ULONGLONG ApcRoutine, __in ULONGLONG ApcArgument1,
                            __in ULONGLONG ApcArgument2, __in ULONGLONG ApcArgument3);

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_HOOKLIB_WOW64_H
