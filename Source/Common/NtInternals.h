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
#include "DynamicAPIs.h"
#include <winternl.h>
#pragma warning(disable : 4091) //disable VS2015 warnings because of SDK v7.1
#include <imagehlp.h>
#pragma warning(default : 4091)
#include "StringLiteW.h"

//-----------------------------------------------------------

#define NKT_DV_ProcessBasicInformation                     0
#define NKT_DV_ProcessSessionInformation                  24
#define NKT_DV_ProcessWow64Information                    26
#define NKT_DV_ProcessImageFileName                       27

#define NKT_DV_ObjectNameInformation                       1
#define NKT_DV_FileNameInformation                         9

#define NKT_DV_MemoryBasicInformation                      0
#define NKT_DV_MemorySectionName                           2

#define NKT_DV_ThreadBasicInformation                      0
#define NKT_DV_ThreadBasePriority                          3

#if defined _M_X64
  #define NKT_DV_UNW_FLAG_NHANDLER                         0

  #define NKT_DV_UNWIND_HISTORY_TABLE_SIZE                12
  #define NKT_DV_UNWIND_HISTORY_TABLE_NONE                 0
  #define NKT_DV_UNWIND_HISTORY_TABLE_GLOBAL               1
  #define NKT_DV_UNWIND_HISTORY_TABLE_LOCAL                2
#endif //_M_X64

#define NKT_DV_SystemProcessInformation                    5
#define NKT_DV_SystemSessionProcessInformation            53
#define NKT_DV_SystemExtendedProcessInformation           57

//-----------------------------------------------------------

typedef LONG KPRIORITY;
typedef LONG KWAIT_REASON;

#pragma pack(8)
typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  ULONG Buffer;
} NKT_DV_UNICODE_STRING32;

typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  ULONGLONG Buffer;
} NKT_DV_UNICODE_STRING64;

typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  LPSTR Buffer;
} NKT_DV_ANSI_STRING;

typedef struct {
  USHORT Length;
  USHORT MaximumLength;
  LPWSTR Buffer;
} NKT_DV_UNICODE_STRING;

typedef struct {
  ULONG Flink;
  ULONG Blink;
} NKT_DV_LIST_ENTRY32;

typedef struct {
  ULONGLONG Flink;
  ULONGLONG Blink;
} NKT_DV_LIST_ENTRY64;

typedef struct {
  NKT_DV_LIST_ENTRY32 InLoadOrderLinks;
  NKT_DV_LIST_ENTRY32 InMemoryOrderLinks;
  NKT_DV_LIST_ENTRY32 InInitializationOrderLinks;
  ULONG DllBase;
  ULONG EntryPoint;
  ULONG SizeOfImage;
  NKT_DV_UNICODE_STRING32 FullDllName;
  NKT_DV_UNICODE_STRING32 BaseDllName;
  ULONG  Flags;
  USHORT LoadCount;
  //following fields may be wrong although not used
  union {
    NKT_DV_LIST_ENTRY32 HashLinks;
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
} NKT_DV_LDR_DATA_TABLE_ENTRY32;

typedef struct {
  NKT_DV_LIST_ENTRY64 InLoadOrderLinks;
  NKT_DV_LIST_ENTRY64 InMemoryOrderLinks;
  NKT_DV_LIST_ENTRY64 InInitializationOrderLinks;
  ULONGLONG DllBase;
  ULONGLONG EntryPoint;
  ULONGLONG SizeOfImage;
  NKT_DV_UNICODE_STRING64 FullDllName;
  NKT_DV_UNICODE_STRING64 BaseDllName;
  ULONG  Flags;
  USHORT LoadCount;
  //following fields may be wrong although not used
  union {
    NKT_DV_LIST_ENTRY64 HashLinks;
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
} NKT_DV_LDR_DATA_TABLE_ENTRY64;

#if defined _M_IX86
  #define NKT_DV_LDR_DATA_TABLE_ENTRY NKT_DV_LDR_DATA_TABLE_ENTRY32
#elif defined _M_X64
  #define NKT_DV_LDR_DATA_TABLE_ENTRY NKT_DV_LDR_DATA_TABLE_ENTRY64
#endif

typedef struct {
  LONG ExitStatus;
  ULONG PebBaseAddress;
  ULONG AffinityMask;
  LONG BasePriority;
  ULONG UniqueProcessId;
  ULONG InheritedFromUniqueProcessId;
} NKT_DV_PROCESS_BASIC_INFORMATION32;

typedef struct {
  LONG ExitStatus;
  ULONG _dummy1;
  ULONGLONG PebBaseAddress;
  ULONGLONG AffinityMask;
  LONG BasePriority;
  ULONG _dummy2;
  ULONGLONG UniqueProcessId;
  ULONGLONG InheritedFromUniqueProcessId;
} NKT_DV_PROCESS_BASIC_INFORMATION64;

typedef struct {
  DWORD dwVersion; //usually 2 or 4
  DWORD dwEntriesCount;
} NKT_DV_APIMAPSET_HEADER_V2;

typedef struct {
  DWORD dwNameOffset;
  DWORD dwNameLength;
  DWORD dwHostModulesOffset;
} NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V2;

typedef struct {
  DWORD dwCount;
} NKT_DV_APIMAPSET_HOST_HEADER_V2;

typedef struct {
  DWORD dwNameOffsetRealName;     //used only when dwCount > 1 and compared with dll name 
  DWORD dwLengthRealName;         //from LDR_DATA_TABLE_ENTRY
  DWORD dwNameOffset;
  DWORD dwLength;
} NKT_DV_APIMAPSET_HOST_ENTRY_V2;

typedef struct {
  DWORD dwVersion; //usually 2 or 4
  DWORD dwUnknown[2];
  DWORD dwEntriesCount;
} NKT_DV_APIMAPSET_HEADER_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwNameOffset1;
  DWORD dwNameLength1;
  DWORD dwNameOffset2;
  DWORD dwNameLength2;
  DWORD dwHostModulesOffset;
} NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwCount;
} NKT_DV_APIMAPSET_HOST_HEADER_V4;

typedef struct {
  DWORD dwUnknown;
  DWORD dwNameOffsetRealName;     //used only when dwCount > 1 and compared with dll name 
  DWORD dwLengthRealName;         //from LDR_DATA_TABLE_ENTRY
  DWORD dwNameOffset;
  DWORD dwLength;
} NKT_DV_APIMAPSET_HOST_ENTRY_V4;

typedef union {
  IMAGE_NT_HEADERS32 u32;
  IMAGE_NT_HEADERS64 u64;
} NKT_DV_IMAGE_NT_HEADER;

typedef struct {
  NKT_DV_LDR_DATA_TABLE_ENTRY32 sEntry;
  //internal usage, don't touch
  HANDLE hProcess;
  ULONG nFirstLink, nCurrLink;
} NKT_DV_LDRENTRY32;

typedef struct {
  NKT_DV_LDR_DATA_TABLE_ENTRY64 sEntry;
  //internal usage, don't touch
  HANDLE hProcess;
  ULONGLONG nFirstLink, nCurrLink;
} NKT_DV_LDRENTRY64;

typedef struct {
  NTSTATUS Status;
  PVOID Pointer;
  ULONG_PTR Information;
} NKT_DV_IO_STATUS_BLOCK;

typedef struct {
  ULONG FileNameLength;
  WCHAR FileName[1];
} NKT_DV_FILE_NAME_INFORMATION;

typedef struct {
  struct {
    USHORT Length;
    USHORT MaximumLength;
    LPWSTR Buffer;
  } Name;
  WCHAR Buffer[1];
} NKT_DV_OBJECT_NAME_INFORMATION;

typedef struct {
  ULONG UniqueProcess;
  ULONG UniqueThread;
} NKT_DV_CLIENT_ID32;

typedef struct {
  ULONGLONG UniqueProcess;
  ULONGLONG UniqueThread;
} NKT_DV_CLIENT_ID64;

#if defined _M_IX86
  #define NKT_DV_CLIENT_ID NKT_DV_CLIENT_ID32
#elif defined _M_X64
  #define NKT_DV_CLIENT_ID NKT_DV_CLIENT_ID64
#endif

typedef struct {
  NTSTATUS ExitStatus;
  LPVOID TebBaseAddress;
  NKT_DV_CLIENT_ID ClientId;
  ULONG_PTR AffinityMask;
  KPRIORITY Priority;
  LONG BasePriority;
} NKT_DV_THREAD_BASIC_INFORMATION;

#if defined _M_X64

#define UNWIND_HISTORY_TABLE_NONE   0
#define UNWIND_HISTORY_TABLE_GLOBAL 1
#define UNWIND_HISTORY_TABLE_LOCAL  2

//from http://technet.microsoft.com/en-us/query/ms680617
//     http://msdn.microsoft.com/en-us/library/windows/desktop/ms680597(v=vs.85).aspx
//     http://www.nynaeve.net/?p=101
typedef struct _NKT_DV_KNONVOLATILE_CONTEXT_POINTERS {
  union {
    PM128A FloatingContext[16];
    struct {
      PM128A Xmm0;
      PM128A Xmm1;
      PM128A Xmm2;
      PM128A Xmm3;
      PM128A Xmm4;
      PM128A Xmm5;
      PM128A Xmm6;
      PM128A Xmm7;
      PM128A Xmm8;
      PM128A Xmm9;
      PM128A Xmm10;
      PM128A Xmm11;
      PM128A Xmm12;
      PM128A Xmm13;
      PM128A Xmm14;
      PM128A Xmm15;
    };
  };
  union {
    PULONG64 IntegerContext[16];
    struct {
      PULONG64 Rax;
      PULONG64 Rcx;
      PULONG64 Rdx;
      PULONG64 Rbx;
      PULONG64 Rsp;
      PULONG64 Rbp;
      PULONG64 Rsi;
      PULONG64 Rdi;
      PULONG64 R8;
      PULONG64 R9;
      PULONG64 R10;
      PULONG64 R11;
      PULONG64 R12;
      PULONG64 R13;
      PULONG64 R14;
      PULONG64 R15;
    };
  };
} NKT_DV_KNONVOLATILE_CONTEXT_POINTERS, *PNKT_DV_KNONVOLATILE_CONTEXT_POINTERS;

typedef struct _NKT_DV_UNWIND_HISTORY_TABLE_ENTRY {
  ULONG64 ImageBase;
  PRUNTIME_FUNCTION FunctionEntry;
} NKT_DV_UNWIND_HISTORY_TABLE_ENTRY, *PNKT_DV_UNWIND_HISTORY_TABLE_ENTRY;

typedef struct _NKT_DV_UNWIND_HISTORY_TABLE {
  ULONG Count;
  UCHAR Search;
  DWORD64 LowAddress;
  DWORD64 HighAddress;
  NKT_DV_UNWIND_HISTORY_TABLE_ENTRY Entry[NKT_DV_UNWIND_HISTORY_TABLE_SIZE];
} NKT_DV_UNWIND_HISTORY_TABLE, *PNKT_DV_UNWIND_HISTORY_TABLE;
#endif //_M_X64

typedef struct {
  LARGE_INTEGER KernelTime;
  LARGE_INTEGER UserTime;
  LARGE_INTEGER CreateTime;
  ULONG WaitTime;
  PVOID StartAddress;
#ifdef _M_X64
  //ULONG Reserved1;
#endif //_M_X64
  NKT_DV_CLIENT_ID ClientId;
  KPRIORITY Priority;
  LONG BasePriority;
  ULONG ContextSwitches;
  ULONG ThreadState;
  KWAIT_REASON WaitReason;
  //ULONG PadPadAlignment;
} NKT_DV_SYSTEM_THREAD_INFORMATION, *PNKT_DV_SYSTEM_THREAD_INFORMATION;

typedef struct {
  NKT_DV_SYSTEM_THREAD_INFORMATION ThreadInfo;
  PVOID StackBase;
  PVOID StackLimit;
  PVOID Win32StartAddress;
  PVOID TebAddress; //Vista or later
  ULONG Reserved1;
  ULONG Reserved2;
  ULONG Reserved3;
} NKT_DV_SYSTEM_EXTENDED_THREAD_INFORMATION, *PNKT_DV_SYSTEM_EXTENDED_THREAD_INFORMATION;

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
  NKT_DV_UNICODE_STRING ImageName;
  KPRIORITY BasePriority;
  HANDLE UniqueProcessId;
  HANDLE InheritedFromUniqueProcessId;
  ULONG HandleCount;
  ULONG SessionId;
  ULONG_PTR PageDirectoryBase;
  //----
  SIZE_T PeakVirtualSize;
  SIZE_T VirtualSize;
  ULONG PageFaultCount;
#ifdef _M_X64
  ULONG Reserved1;
#endif //_M_X64
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
    NKT_DV_SYSTEM_THREAD_INFORMATION Threads[1];
    NKT_DV_SYSTEM_EXTENDED_THREAD_INFORMATION ExtThreads[1];
  };
} NKT_DV_SYSTEM_PROCESS_INFORMATION, *PNKT_DV_SYSTEM_PROCESS_INFORMATION;

typedef struct {
  ULONG SessionId;
  ULONG SizeOfBuf;
  PVOID Buffer;
} NKT_DV_SYSTEM_SESSION_PROCESS_INFORMATION, *PNKT_DV_SYSTEM_SESSION_PROCESS_INFORMATION;

typedef struct {
  ULONG SessionId;
} NKT_DV_PROCESS_SESSION_INFORMATION, *PNKT_DV_PROCESS_SESSION_INFORMATION;
#pragma pack()

//-----------------------------------------------------------

HRESULT nktDvNtGetPeb(__out LPBYTE *lplpPeb, __in HANDLE hProcess, __in SIZE_T nPlatformBits);

HRESULT nktDvNtCheckImageType(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in HANDLE hProcess,
                              __in LPBYTE lpBaseAddr, __out_opt LPBYTE *lplpNtHdrAddr=NULL);

HRESULT nktDvNtGetBaseAddress32(__out LPBYTE *lplpBaseAddress, __in LPBYTE lpPeb, __in HANDLE hProcess);

HRESULT nktDvNtGetFirstLdrEntry32(__out NKT_DV_LDRENTRY32 *lpEntry32, __in LPBYTE lpPeb,
                                  __in HANDLE hProcess);
HRESULT nktDvNtGetNextLdrEntry32(__inout NKT_DV_LDRENTRY32 *lpEntry32);

#if defined _M_X64
HRESULT nktDvNtGetBaseAddress64(__out LPBYTE *lplpBaseAddress, __in LPBYTE lpPeb, __in HANDLE hProcess);

HRESULT nktDvNtGetFirstLdrEntry64(__out NKT_DV_LDRENTRY64 *lpEntry64, __in LPBYTE lpPeb,
                                  __in HANDLE hProcess);
HRESULT nktDvNtGetNextLdrEntry64(__inout NKT_DV_LDRENTRY64 *lpEntry64);
#endif //_M_X64

HRESULT nktDvNtFindApiMapSet(__out CNktStringW &cStrReplDllW, __in HANDLE hProcess,
                             __in SIZE_T nPlatformBits, __in LPCWSTR szDllToFindW,
                             __in LPCWSTR szImportingDllW);

HRESULT nktDvNtGetFileNameFromHandle(__out CNktStringW &cStrNameW, __in HANDLE hFile);
HRESULT nktDvNtGetKeyNameFromHandle(__out CNktStringW &cStrNameW, __in HKEY hKey);

HRESULT nktDvNtLockLoader(__out PVOID *lplpCookie, __in BOOL bOnlyTry=FALSE);
HRESULT nktDvNtUnlockLoader(__in PVOID lpCookie);
PRTL_CRITICAL_SECTION nktDvNtGetLoaderLock();
BOOL nktDvNtIsLoaderLoaderLockHeld(__in BOOL bAnyThread);

//-----------------------------------------------------------

