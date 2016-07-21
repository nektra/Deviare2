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

#include "MemoryManager.h"

#if (defined(NKT_ENABLE_MEMORY_TRACKING)) &&          \
    (!defined(NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS))

#include "WaitableObjects.h"
#pragma warning(disable : 4091) //disable VS2015 warnings because of SDK v7.1
#include <imagehlp.h>
#pragma warning(default : 4091)

//-----------------------------------------------------------

#define NKT_MEMORY_TRACKING_STACK_ENTRIES                 16
#define NKT_MEMORY_TRACKING_HASH_SIZE                     97
#define NKT_MEMORY_TRACKING_BLOCK_SIZE                     \
                 (sizeof(CNktMemMgrTracker::MEMTRACE_BLOCK))

#define nktMemTracker() CNktMemMgrTracker::GetAccessor()
#define nktMemTrackerNoCreate() CNktMemMgrTracker::GetAccessor(TRUE)

//-----------------------------------------------------------

class CNktMemMgrTracker
{
public:
  typedef struct tagMEMTRACE_BLOCK {
    struct tagMEMTRACE_BLOCK *lpNext;
    SIZE_T nTag;
    char *szFileName;
    union {
      int nLineNumber;
      SIZE_T _dummy0;
    };
    SIZE_T nBlockSize;
    SIZE_T nStackValues[NKT_MEMORY_TRACKING_STACK_ENTRIES];
    SIZE_T _pad0;
  } MEMTRACE_BLOCK, *LPMEMTRACE_BLOCK;

  static CNktMemMgrTracker* GetAccessor(__in BOOL bDontCreate=FALSE);

  CNktMemMgrTracker();
  virtual ~CNktMemMgrTracker();

  virtual void AddTrace(__in LPBYTE lpPtr, __in SIZE_T nBlockSize, __in_z_opt const char *szFileName,
                __in int nLineNumber);
  virtual void RemoveTrace(__in LPBYTE lpPtr);

  virtual void DumpLeaks();

  void* __cdecl operator new(__in size_t nSize, __inout void* lpInPlace);
#if _MSC_VER >= 1200
  void __cdecl operator delete(__inout void* p, __inout void* lpPlace);
#endif //_MSC_VER >= 1200

private:
  typedef USHORT (WINAPI *lpfnRtlCaptureStackBackTrace)(ULONG, ULONG, PVOID*, PULONG);
  typedef BOOL (WINAPI *lpfnSymInitializeW)(__in HANDLE hProcess, __in_opt PCWSTR UserSearchPath,
                                            __in BOOL fInvadeProcess);
  typedef BOOL (WINAPI *lpfnSymCleanup)(__in HANDLE hProcess);
  typedef BOOL (WINAPI *lpfnSymGetSymFromAddr64)(__in HANDLE hProcess, __in DWORD64 qwAddr,
                                                __out_opt PDWORD64 pdwDisplacement,
                                                __inout PIMAGEHLP_SYMBOL64 Symbol);
  typedef LPAPI_VERSION (WINAPI *lpfnImagehlpApiVersion)(VOID);

  HINSTANCE hDbgHelpDLL;
  lpfnSymInitializeW fnSymInitializeW;
  lpfnSymCleanup fnSymCleanup;
  lpfnSymGetSymFromAddr64 fnSymGetSymFromAddr64;
  lpfnImagehlpApiVersion fnImagehlpApiVersion;
  //----
  CNktFastMutex cMutex[NKT_MEMORY_TRACKING_HASH_SIZE];
  LPMEMTRACE_BLOCK volatile lpMemoryBlocks[NKT_MEMORY_TRACKING_HASH_SIZE];
  SIZE_T nMemMgrFlags;
  lpfnRtlCaptureStackBackTrace fnRtlCaptureStackBackTrace;
};

//-----------------------------------------------------------

#endif //NKT_ENABLE_MEMORY_TRACKING && !NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS

