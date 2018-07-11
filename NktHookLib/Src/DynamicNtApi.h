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

#ifndef _NKT_HOOKLIB_DYNAMIC_NTAPI_H
#define _NKT_HOOKLIB_DYNAMIC_NTAPI_H

#include <windows.h>
#include <winternl.h>

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#ifndef STATUS_SUCCESS
  #define STATUS_SUCCESS                 ((NTSTATUS)0x00000000L)
#endif //!STATUS_SUCCESS
#ifndef STATUS_UNSUCCESSFUL
  #define STATUS_UNSUCCESSFUL            ((NTSTATUS)0xC0000001L)
#endif //!STATUS_UNSUCCESSFUL
#ifndef STATUS_NOT_IMPLEMENTED
  #define STATUS_NOT_IMPLEMENTED         ((NTSTATUS)0xC0000002L)
#endif //!STATUS_NOT_IMPLEMENTED
#ifndef STATUS_INFO_LENGTH_MISMATCH
  #define STATUS_INFO_LENGTH_MISMATCH    ((NTSTATUS)0xC0000004L)
#endif //!STATUS_INFO_LENGTH_MISMATCH
#ifndef STATUS_NO_MEMORY
  #define STATUS_NO_MEMORY               ((NTSTATUS)0xC0000017L)
#endif //!STATUS_NO_MEMORY
#ifndef STATUS_ACCESS_DENIED
  #define STATUS_ACCESS_DENIED           ((NTSTATUS)0xC0000022L)
#endif //!STATUS_ACCESS_DENIED
#ifndef STATUS_BUFFER_TOO_SMALL
  #define STATUS_BUFFER_TOO_SMALL        ((NTSTATUS)0xC0000023L)
#endif //!STATUS_BUFFER_TOO_SMALL
#ifndef STATUS_PROCEDURE_NOT_FOUND
  #define STATUS_PROCEDURE_NOT_FOUND     ((NTSTATUS)0xC000007AL)
#endif //!STATUS_PROCEDURE_NOT_FOUND
#ifndef STATUS_NOT_SUPPORTED
  #define STATUS_NOT_SUPPORTED           ((NTSTATUS)0xC00000BBL)
#endif //!STATUS_NOT_SUPPORTED
#ifndef STATUS_NOT_FOUND
  #define STATUS_NOT_FOUND               ((NTSTATUS)0xC0000225L)
#endif //!STATUS_NOT_FOUND
#ifndef STATUS_PARTIAL_COPY
  #define STATUS_PARTIAL_COPY            ((NTSTATUS)0x8000000DL)
#endif //!STATUS_PARTIAL_COPY

#ifndef NKT_ASSERT
  #ifdef _DEBUG
    #define NKT_ASSERT(x) if (!(x)) __debugbreak();
  #else
    #define NKT_ASSERT(x)
  #endif //_DEBUG
#endif //!NKT_ASSERT

//-----------------------------------------------------------

typedef LONG NKT_HK_KPRIORITY;
typedef LONG NKT_HK_KWAIT_REASON;

typedef enum {
  MySystemProcessInformation = 5,
  MySystemSessionProcessInformation = 53,
  MySystemExtendedProcessInformation = 57
} NKT_HK_SYSTEM_INFORMATION_CLASS;

typedef enum {
  MyMemoryBasicInformation = 0,
  MyMemorySectionName = 2
} NKT_HK_VIRTUALMEMORYINFORMATIONCLASS;

typedef enum {
  MyProcessBasicInformation = 0,
  MyProcessSessionInformation = 24,
  MyProcessWow64Information = 26
} NKT_HK_PROCESSINFOCLASS;

#pragma pack(8)
typedef struct {
  SIZE_T UniqueProcess;
  SIZE_T UniqueThread;
} NKT_HK_CLIENT_ID;

typedef struct {
  LARGE_INTEGER KernelTime;
  LARGE_INTEGER UserTime;
  LARGE_INTEGER CreateTime;
  ULONG WaitTime;
  PVOID StartAddress;
  NKT_HK_CLIENT_ID ClientId;
  NKT_HK_KPRIORITY Priority;
  LONG BasePriority;
  ULONG ContextSwitches;
  ULONG ThreadState;
  NKT_HK_KWAIT_REASON WaitReason;
} NKT_HK_SYSTEM_THREAD_INFORMATION, *LPNKT_HK_SYSTEM_THREAD_INFORMATION;

typedef struct {
  NKT_HK_SYSTEM_THREAD_INFORMATION ThreadInfo;
  PVOID StackBase;
  PVOID StackLimit;
  PVOID Win32StartAddress;
  PVOID TebAddress; //Vista or later
  ULONG Reserved1;
  ULONG Reserved2;
  ULONG Reserved3;
} NKT_HK_SYSTEM_EXTENDED_THREAD_INFORMATION, *LPNKT_HK_SYSTEM_EXTENDED_THREAD_INFORMATION;

typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  LPWSTR Buffer;
} NKT_HK_UNICODE_STRING;

typedef struct {
  ULONG NextEntryOffset;
  ULONG NumberOfThreads;
  LARGE_INTEGER WorkingSetPrivateSize; //VISTA
  ULONG HardFaultCount; //WIN7
  ULONG NumberOfThreadsHighWatermark; //WIN7
  ULONGLONG CycleTime; //WIN7
  LARGE_INTEGER CreateTime;
  LARGE_INTEGER UserTime;
  LARGE_INTEGER KernelTime;
  NKT_HK_UNICODE_STRING ImageName;
  NKT_HK_KPRIORITY BasePriority;
  HANDLE UniqueProcessId;
  HANDLE InheritedFromUniqueProcessId;
  ULONG HandleCount;
  ULONG SessionId;
  ULONG_PTR PageDirectoryBase;
  //----
  SIZE_T PeakVirtualSize;
  SIZE_T VirtualSize;
  ULONG PageFaultCount;
  SIZE_T PeakWorkingSetSize;
  SIZE_T WorkingSetSize;
  SIZE_T QuotaPeakPagedPoolUsage;
  SIZE_T QuotaPagedPoolUsage;
  SIZE_T QuotaPeakNonPagedPoolUsage;
  SIZE_T QuotaNonPagedPoolUsage;
  SIZE_T PagefileUsage;
  SIZE_T PeakPagefileUsage;
  SIZE_T PrivatePageCount;
  //IO_COUNTERS
  LARGE_INTEGER ReadOperationCount;
  LARGE_INTEGER WriteOperationCount;
  LARGE_INTEGER OtherOperationCount;
  LARGE_INTEGER ReadTransferCount;
  LARGE_INTEGER WriteTransferCount;
  LARGE_INTEGER OtherTransferCount;
  union {
    NKT_HK_SYSTEM_THREAD_INFORMATION Threads[1];
    NKT_HK_SYSTEM_EXTENDED_THREAD_INFORMATION ExtThreads[1];
  };
} NKT_HK_SYSTEM_PROCESS_INFORMATION, *LPNKT_HK_SYSTEM_PROCESS_INFORMATION;

typedef struct {
  ULONG SessionId;
  ULONG SizeOfBuf;
  PVOID Buffer;
} NKT_HK_SYSTEM_SESSION_PROCESS_INFORMATION, *PNKT_HK_SYSTEM_SESSION_PROCESS_INFORMATION;

typedef struct {
  ULONG SessionId;
} NKT_HK_PROCESS_SESSION_INFORMATION, *LPNKT_HK_PROCESS_SESSION_INFORMATION;

typedef struct {
  DWORD ExitStatus;
  PTEB TebBaseAddress;
  NKT_HK_CLIENT_ID ClientId;
  ULONG_PTR AffinityMask;
  NKT_HK_KPRIORITY Priority;
  LONG BasePriority;
} NKT_HK_THREAD_BASIC_INFORMATION, *LPNKT_HK_THREAD_BASIC_INFORMATION;

typedef union {
  IMAGE_NT_HEADERS32 u32;
  IMAGE_NT_HEADERS64 u64;
} NKT_HK_IMAGE_NT_HEADER;

typedef struct {
  LONG ExitStatus;
  ULONG PebBaseAddress;
  ULONG AffinityMask;
  LONG BasePriority;
  ULONG UniqueProcessId;
  ULONG InheritedFromUniqueProcessId;
} NKT_HK_PROCESS_BASIC_INFORMATION32;

#if defined(_M_X64)
typedef struct {
  LONG ExitStatus;
  ULONG _dummy1;
  ULONGLONG PebBaseAddress;
  ULONGLONG AffinityMask;
  LONG BasePriority;
  ULONG _dummy2;
  ULONGLONG UniqueProcessId;
  ULONGLONG InheritedFromUniqueProcessId;
} NKT_HK_PROCESS_BASIC_INFORMATION64;
#endif //_M_X64

typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  ULONG Buffer;
} NKT_HK_UNICODE_STRING32;

#if defined(_M_X64)
typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  ULONGLONG Buffer;
} NKT_HK_UNICODE_STRING64;
#endif //_M_X64

typedef struct {
  ULONG Flink;
  ULONG Blink;
} NKT_HK_LIST_ENTRY32;

#if defined(_M_X64)
typedef struct {
  ULONGLONG Flink;
  ULONGLONG Blink;
} NKT_HK_LIST_ENTRY64;
#endif //_M_X64

typedef struct {
  NKT_HK_LIST_ENTRY32 InLoadOrderLinks;
  NKT_HK_LIST_ENTRY32 InMemoryOrderLinks;
  NKT_HK_LIST_ENTRY32 InInitializationOrderLinks;
  ULONG DllBase;
  ULONG EntryPoint;
  ULONG SizeOfImage;
  NKT_HK_UNICODE_STRING32 FullDllName;
  NKT_HK_UNICODE_STRING32 BaseDllName;
  ULONG  Flags;
  USHORT LoadCount;
  //following fields may be wrong although not used
  union {
    NKT_HK_LIST_ENTRY32 HashLinks;
    struct {
      ULONG SectionPointer;
      ULONG CheckSum;
    };
  };
  union {
    struct {
      ULONG TimeDateStamp;
    };
    struct {
      ULONG LoadedImports;
    };
  };
  ULONG EntryPointActivationContext;
  ULONG PatchInformation;
  //structure continues but it is not needed
} NKT_HK_LDR_DATA_TABLE_ENTRY32;

#if defined(_M_X64)
typedef struct {
  NKT_HK_LIST_ENTRY64 InLoadOrderLinks;
  NKT_HK_LIST_ENTRY64 InMemoryOrderLinks;
  NKT_HK_LIST_ENTRY64 InInitializationOrderLinks;
  ULONGLONG DllBase;
  ULONGLONG EntryPoint;
  ULONGLONG SizeOfImage;
  NKT_HK_UNICODE_STRING64 FullDllName;
  NKT_HK_UNICODE_STRING64 BaseDllName;
  ULONG  Flags;
  USHORT LoadCount;
  //following fields may be wrong although not used
  union {
    NKT_HK_LIST_ENTRY64 HashLinks;
    struct {
      ULONGLONG SectionPointer;
      ULONG CheckSum;
    };
  };
  union {
    struct {
      ULONG TimeDateStamp;
    };
    struct {
      ULONGLONG LoadedImports;
    };
  };
  ULONGLONG EntryPointActivationContext;
  ULONGLONG PatchInformation;
  //structure continues but it is not needed
} NKT_HK_LDR_DATA_TABLE_ENTRY64;
#endif //_M_X64

typedef struct {
  NKT_HK_LDR_DATA_TABLE_ENTRY32 sEntry;
  //internal usage, don't touch
  HANDLE hProcess;
  ULONG nFirstLink, nCurrLink;
} NKT_HK_LDRENTRY32;

#if defined(_M_X64)
typedef struct {
  NKT_HK_LDR_DATA_TABLE_ENTRY64 sEntry;
  //internal usage, don't touch
  HANDLE hProcess;
  ULONGLONG nFirstLink, nCurrLink;
} NKT_HK_LDRENTRY64;
#endif //_M_X64

typedef struct {
  DWORD dwVersion; //usually 2 or 4
  DWORD dwEntriesCount;
} NKT_HK_APIMAPSET_HEADER_V2;

typedef struct {
  DWORD dwNameOffset;
  DWORD dwNameLength;
  DWORD dwHostModulesOffset;
} NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V2;

typedef struct {
  DWORD dwCount;
} NKT_HK_APIMAPSET_HOST_HEADER_V2;

typedef struct {
  DWORD dwNameOffsetRealName;     //used only when dwCount > 1 and compared with dll name 
  DWORD dwLengthRealName;         //from LDR_DATA_TABLE_ENTRY
  DWORD dwNameOffset;
  DWORD dwLength;
} NKT_HK_APIMAPSET_HOST_ENTRY_V2;

typedef struct {
  DWORD dwVersion; //usually 2 or 4
  DWORD dwUnknown[2];
  DWORD dwEntriesCount;
} NKT_HK_APIMAPSET_HEADER_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwNameOffset1;
  DWORD dwNameLength1;
  DWORD dwNameOffset2;
  DWORD dwNameLength2;
  DWORD dwHostModulesOffset;
} NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwCount;
} NKT_HK_APIMAPSET_HOST_HEADER_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwNameOffsetRealName;     //used only when dwCount > 1 and compared with dll name 
  DWORD dwLengthRealName;         //from LDR_DATA_TABLE_ENTRY
  DWORD dwNameOffset;
  DWORD dwLength;
} NKT_HK_APIMAPSET_HOST_ENTRY_V4;
#pragma pack()

//-----------------------------------------------------------

#define NKT_PARSE_NTAPI_NTSTATUS(name, parameters, _notused)  \
  NTSTATUS __stdcall Nkt##name parameters;
#define NKT_PARSE_NTAPI_VOID(name, parameters, _notused)      \
  VOID __stdcall Nkt##name parameters;
#define NKT_PARSE_NTAPI_PVOID(name, parameters, _notused)     \
  PVOID __stdcall Nkt##name parameters;
#define NKT_PARSE_NTAPI_BOOLEAN(name, parameters, _notused)   \
  BOOLEAN __stdcall Nkt##name parameters;
#define NKT_PARSE_NTAPI_ULONG(name, parameters, _notused)     \
  ULONG __stdcall Nkt##name parameters;
#include "NtApiDeclarations.h"
#undef NKT_PARSE_NTAPI_NTSTATUS
#undef NKT_PARSE_NTAPI_VOID
#undef NKT_PARSE_NTAPI_PVOID
#undef NKT_PARSE_NTAPI_BOOLEAN
#undef NKT_PARSE_NTAPI_ULONG

HINSTANCE GetRemoteModuleBaseAddress(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in BOOL bScanMappedImages);
LPVOID GetRemoteProcedureAddress(__in HANDLE hProcess, __in LPVOID lpDllBase, __in_z LPCSTR szFuncNameA);

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_HOOKLIB_DYNAMIC_NTAPI_H
