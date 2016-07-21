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

#include "MemoryManagerTracker.h"

#if (defined(NKT_ENABLE_MEMORY_TRACKING)) &&          \
    (!defined(NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS))

#include <crtdbg.h>
#include "Debug.h"
#include "NtInternals.h"

//-----------------------------------------------------------

//#define MEMTRACE_DONTLOAD_DBGHELP

#define MEMTRACE_TAG                              0x544D454D
#define MEMTRACE_TAG_END                          0x34BF45C3

#define TAGENDPTR(x,blksize)                               \
                    ((SIZE_T*)(((LPBYTE)(x)) + (blksize) + \
                    NKT_MEMORY_TRACKING_BLOCK_SIZE))

//-----------------------------------------------------------

static BOOL bLeaksFound = FALSE;
static BYTE _MemTrackerContainer[sizeof(CNktMemMgrTracker)] = {0};
static CNktMemMgrTracker *lpMemTrackerObj = NULL;
static LONG volatile nMemTrackerSingletonLock = 0;

//-----------------------------------------------------------

static void __cdecl MEMTRACKER_Finalizer();

typedef void (__cdecl *_PVFV)(void);

#if _MSC_FULL_VER >= 140050214
  #pragma section(".CRT$XTU",long,read)
#else //_MSC_FULL_VER >= 140050214
  #if defined _M_IX86
    #pragma section(".CRT$XTU",long,read,write)
  #elif defined _M_X64
    #pragma section(".CRT$XTU",long,read)
  #else
    #error Unsupported platform
  #endif
#endif //_MSC_FULL_VER >= 140050214
#pragma data_seg(".CRT$XTU")
static __declspec(allocate(".CRT$XTU")) _PVFV aMEMTRACKER_Finalizers[] = { &MEMTRACKER_Finalizer };
#pragma data_seg()

//-----------------------------------------------------------

CNktMemMgrTracker::CNktMemMgrTracker()
{
  HMODULE hNtDll;
  DWORD dwOsVer;
  SIZE_T i;

  nMemMgrFlags = 0;
  for (i=0; i<NKT_MEMORY_TRACKING_HASH_SIZE; i++)
    lpMemoryBlocks[i] = NULL;
  fnRtlCaptureStackBackTrace = NULL;
  hNtDll = ::GetModuleHandleW(L"ntdll.dll");
  if (hNtDll != NULL)
  {
    dwOsVer = ::GetVersion();
    i = (SIZE_T)(LOBYTE(LOWORD(dwOsVer)));
    if (i > 5 ||
        (i == 5 && HIBYTE(LOWORD(dwOsVer)) >= 1))
    {
      //use RtlCaptureStackBackTrace only on xp or later
      fnRtlCaptureStackBackTrace = (lpfnRtlCaptureStackBackTrace)::GetProcAddress(hNtDll,
                                        "RtlCaptureStackBackTrace");
    }
  }
  //----
  fnSymInitializeW = NULL;
  fnSymCleanup = NULL;
  fnSymGetSymFromAddr64 = NULL;
  fnImagehlpApiVersion = NULL;
#ifdef MEMTRACE_DONTLOAD_DBGHELP
  hDbgHelpDLL = NULL;
#else //MEMTRACE_DONTLOAD_DBGHELP
  i = 0;
  hDbgHelpDLL = ::LoadLibraryW(L"dbghelp.dll");
  if (hDbgHelpDLL != NULL)
  {
    fnSymInitializeW = (lpfnSymInitializeW)::GetProcAddress(hDbgHelpDLL, "SymInitializeW");
    fnSymCleanup = (lpfnSymCleanup)::GetProcAddress(hDbgHelpDLL, "SymCleanup");
    fnSymGetSymFromAddr64 = (lpfnSymGetSymFromAddr64)::GetProcAddress(hDbgHelpDLL, "SymGetSymFromAddr64");
    fnImagehlpApiVersion = (lpfnImagehlpApiVersion)::GetProcAddress(hDbgHelpDLL, "ImagehlpApiVersion");
    if (fnSymInitializeW != NULL && fnSymCleanup != NULL &&
        fnSymGetSymFromAddr64 != NULL && fnImagehlpApiVersion != NULL)
    {
      LPAPI_VERSION lpApiVer;

      try
      {
        lpApiVer = fnImagehlpApiVersion();
        if (lpApiVer->MajorVersion > 5 ||
            (lpApiVer->MajorVersion == 5 && lpApiVer->MinorVersion >= 1))
          i = 1;
        if (i != 0 &&
            fnSymInitializeW(::GetCurrentProcess(), NULL, FALSE) == FALSE)
          i = 0;
      }
      catch (...)
      {
        i = 0;
      }
    }
  }
  if (i == 0)
  {
    fnSymInitializeW = NULL;
    fnSymCleanup = NULL;
    fnSymGetSymFromAddr64 = NULL;
    fnImagehlpApiVersion = NULL;
    if (hDbgHelpDLL != NULL)
    {
      ::FreeLibrary(hDbgHelpDLL);
      hDbgHelpDLL = NULL;
    }
  }
#endif //MEMTRACE_DONTLOAD_DBGHELP
  return;
}

CNktMemMgrTracker::~CNktMemMgrTracker()
{
  SIZE_T i;
  BOOL bShutdownInProgress;

  nktDvDynApis_RtlDllShutdownInProgress(&bShutdownInProgress);
  if (bShutdownInProgress == FALSE)
    DumpLeaks();
  for (i=0; i<NKT_MEMORY_TRACKING_HASH_SIZE; i++)
    lpMemoryBlocks[i] = NULL;
  //----
  if (hDbgHelpDLL != NULL)
  {
    fnSymCleanup(::GetCurrentProcess());
    fnSymInitializeW = NULL;
    fnSymCleanup = NULL;
    fnSymGetSymFromAddr64 = NULL;
    fnImagehlpApiVersion = NULL;
    ::FreeLibrary(hDbgHelpDLL);
    hDbgHelpDLL = NULL;
  }
  return;
}

CNktMemMgrTracker* CNktMemMgrTracker::GetAccessor(__in BOOL bDontCreate)
{
  _ReadWriteBarrier();
  if (lpMemTrackerObj == NULL && bDontCreate == FALSE)
  {
    CNktMemMgrTracker *lpNewObject;

    while (NktInterlockedExchange(&nMemTrackerSingletonLock, 1) != 0)
      ::Sleep(1);
    if (lpMemTrackerObj == NULL)
    {
      //if object does not exists, first create it
      lpNewObject = NULL;
      try
      {
        lpNewObject = new (_MemTrackerContainer) CNktMemMgrTracker;
      }
      catch (...)
      {
        lpNewObject = NULL;
      }
      if (lpNewObject == NULL)
        return NULL;
      _ReadWriteBarrier();
      lpMemTrackerObj = lpNewObject;
    }
    NktInterlockedExchange(&nMemTrackerSingletonLock, 0);
  }
  return lpMemTrackerObj;
}

void CNktMemMgrTracker::AddTrace(__in LPBYTE lpPtr, __in SIZE_T nBlockSize,
                                 __in_z_opt const char *szFileName, __in int nLineNumber)
{
  LPMEMTRACE_BLOCK lpCurrBlock, lpNewBlock;
  SIZE_T nHashIndex;

  lpNewBlock = (LPMEMTRACE_BLOCK)lpPtr;
  nHashIndex = ((SIZE_T)lpPtr >> 8) % NKT_MEMORY_TRACKING_HASH_SIZE;
  {
    CNktAutoFastMutex cLock(&cMutex[nHashIndex]);
    ULONG nHash;

    for (lpCurrBlock=lpMemoryBlocks[nHashIndex]; lpCurrBlock!=NULL; lpCurrBlock=lpCurrBlock->lpNext)
    {
      NKT_ASSERT(lpCurrBlock->nTag == MEMTRACE_TAG);
      NKT_ASSERT(*TAGENDPTR(lpCurrBlock, lpCurrBlock->nBlockSize) == MEMTRACE_TAG_END);
      if (lpCurrBlock == lpNewBlock)
        break;
    }
    if (lpCurrBlock != NULL)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlMemoryTracker, ("MemoryTracker: Duplicate traced pointer @ %IXh",
                        (SIZE_T)lpPtr + NKT_MEMORY_TRACKING_BLOCK_SIZE));
    }
    else
    {
      lpNewBlock->nTag = MEMTRACE_TAG;
      lpNewBlock->lpNext = lpMemoryBlocks[nHashIndex];
      lpNewBlock->szFileName = (char*)szFileName;
      lpNewBlock->nLineNumber = nLineNumber;
      lpNewBlock->nBlockSize = nBlockSize;
      nktMemSet(lpNewBlock->nStackValues, 0, sizeof(lpNewBlock->nStackValues));
      if (fnRtlCaptureStackBackTrace != NULL)
        fnRtlCaptureStackBackTrace(2, NKT_MEMORY_TRACKING_STACK_ENTRIES, //W2K: last param cant be null
                                   (LPVOID*)(lpNewBlock->nStackValues), &nHash);
      lpMemoryBlocks[nHashIndex] = lpNewBlock;
      *TAGENDPTR(lpNewBlock, nBlockSize) = MEMTRACE_TAG_END;
    }
  }
  NKT_DEBUGPRINTLNA(Nektra::dlMemoryTrackerLevel2, ("%s(%d): Adding memory trace @ %IXh(%Iu) "
                        "[%IX,%IX,%IX,%IX,%IX]", szFileName, nLineNumber,
                        (SIZE_T)lpPtr + NKT_MEMORY_TRACKING_BLOCK_SIZE, nBlockSize,
                        lpNewBlock->nStackValues[0], lpNewBlock->nStackValues[1],
                        lpNewBlock->nStackValues[2], lpNewBlock->nStackValues[3],
                        lpNewBlock->nStackValues[4]));
  return;
}

void CNktMemMgrTracker::RemoveTrace(__in LPBYTE lpPtr)
{
  LPMEMTRACE_BLOCK lpCurrBlock, lpToRemoveBlock, lpPrevBlock;
  SIZE_T i, j, nHashIndex;
  int k;
  char szStackA[NKT_MEMORY_TRACKING_STACK_ENTRIES*(sizeof(SIZE_T)*2+4)];

  lpToRemoveBlock = (LPMEMTRACE_BLOCK)lpPtr;
  NKT_ASSERT(lpToRemoveBlock->nTag == MEMTRACE_TAG);
  NKT_ASSERT(*TAGENDPTR(lpToRemoveBlock, lpToRemoveBlock->nBlockSize) == MEMTRACE_TAG_END);
  nHashIndex = ((SIZE_T)lpPtr >> 8) % NKT_MEMORY_TRACKING_HASH_SIZE;
  {
    CNktAutoFastMutex cLock(&cMutex[nHashIndex]);

    lpPrevBlock = NULL;
    for (lpCurrBlock=lpMemoryBlocks[nHashIndex]; lpCurrBlock!=NULL; lpCurrBlock=lpCurrBlock->lpNext)
    {
      NKT_ASSERT(lpCurrBlock->nTag == MEMTRACE_TAG);
      if (lpCurrBlock == lpToRemoveBlock)
        break;
      lpPrevBlock = lpCurrBlock;
    }
    if (lpCurrBlock == NULL)
    {
      for (i=j=0; i<NKT_MEMORY_TRACKING_STACK_ENTRIES; i++)
      {
        szStackA[j++] = (i == 0) ? ' ' : ',';
        k = sprintf_s(szStackA+j, sizeof(szStackA)-j, "%IXh", lpToRemoveBlock->nStackValues[i]);
        if (k > 0)
          j += (SIZE_T)k;
      }
      szStackA[j] = 0;
      NKT_DEBUGPRINTLNA(Nektra::dlMemoryTracker, ("Free on untraced pointer @ %IXh [Stack:%s]",
                         (SIZE_T)lpPtr + NKT_MEMORY_TRACKING_BLOCK_SIZE, szStackA));
    }
    else
    {
      NKT_DEBUGPRINTLNA(Nektra::dlMemoryTrackerLevel2, ("%s(%d): Removing memory trace @ %IXh(%Iu)",
                         lpCurrBlock->szFileName, lpCurrBlock->nLineNumber,
                         (SIZE_T)lpPtr + NKT_MEMORY_TRACKING_BLOCK_SIZE, lpCurrBlock->nBlockSize));
      if (lpPrevBlock == NULL)
        lpMemoryBlocks[nHashIndex] = lpCurrBlock->lpNext;
      else
        lpPrevBlock->lpNext = lpCurrBlock->lpNext;
    }
  }
  return;
}

void CNktMemMgrTracker::DumpLeaks()
{
  LPMEMTRACE_BLOCK lpCurrBlock;
  SIZE_T i, j;
  BYTE sSymBuf[sizeof(IMAGEHLP_SYMBOL64)+512];
  IMAGEHLP_SYMBOL64* lpSymBuf = (IMAGEHLP_SYMBOL64*)sSymBuf;
  DWORD64 qwDisp;

  //for (i=0; i<NKT_MEMORY_TRACKING_HASH_SIZE; i++)
  //  cs[i].Lock();
  for (i=0; i<NKT_MEMORY_TRACKING_HASH_SIZE; i++)
  {
    for (lpCurrBlock=lpMemoryBlocks[i]; lpCurrBlock!=NULL; lpCurrBlock=lpCurrBlock->lpNext)
    {
      NKT_ASSERT(lpCurrBlock->nTag == MEMTRACE_TAG);
      NKT_DEBUGPRINTA(Nektra::dlMemoryTracker, ("%s(%d): MemoryTracker: Leak found @ %IXh (%Iu bytes)",
                       lpCurrBlock->szFileName, lpCurrBlock->nLineNumber,
                       (SIZE_T)lpCurrBlock + NKT_MEMORY_TRACKING_BLOCK_SIZE, lpCurrBlock->nBlockSize));
      if (hDbgHelpDLL != NULL)
      {
        NKT_DEBUGPRINTA(Nektra::dlMemoryTracker, ("\n"));
        for (j=0; j<NKT_MEMORY_TRACKING_STACK_ENTRIES; j++)
        {
          if (lpCurrBlock->nStackValues[j] == 0)
            break;
          memset(lpSymBuf, 0, sizeof(IMAGEHLP_SYMBOL64));
          lpSymBuf->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
          lpSymBuf->MaxNameLength = 512;
          if (fnSymGetSymFromAddr64(::GetCurrentProcess(), (DWORD64)(lpCurrBlock->nStackValues[j]),
                                    &qwDisp, lpSymBuf) == FALSE)
          {
            lpSymBuf->Name[0] = lpSymBuf->Name[1] = lpSymBuf->Name[2] = '?';
            lpSymBuf->Name[3] = 0;
          }
          NKT_DEBUGPRINTLNA(Nektra::dlMemoryTracker, ("      %IXh => %s", lpCurrBlock->nStackValues[j],
                             lpSymBuf->Name));
        }
      }
      else
      {
        NKT_DEBUGPRINTA(Nektra::dlMemoryTracker, (" bytes) [Stack:"));
        for (j=0; j<NKT_MEMORY_TRACKING_STACK_ENTRIES; j++)
        {
          NKT_DEBUGPRINTA(Nektra::dlMemoryTracker, ("%c%IXh", ((j == 0) ? ' ' : ','),
                           lpCurrBlock->nStackValues[j]));
        }
        NKT_DEBUGPRINTLNA(Nektra::dlMemoryTracker, ("]"));
      }
      bLeaksFound = TRUE;
    }
  }
  //for (i=0; i<NKT_MEMORY_TRACKING_HASH_SIZE; i++)
  //  cs[i].Unlock();
  return;
}

void* __cdecl CNktMemMgrTracker::operator new(__in size_t nSize, __inout void* lpInPlace)
{
  return lpInPlace;
}

#if _MSC_VER >= 1200
void __cdecl CNktMemMgrTracker::operator delete(__inout void* p, __inout void* lpPlace)
{
  return;
}
#endif //_MSC_VER >= 1200

//-----------------------------------------------------------

static void __cdecl MEMTRACKER_Finalizer()
{
  while (NktInterlockedExchange(&nMemTrackerSingletonLock, 1) != 0)
    ::Sleep(1);
  if (lpMemTrackerObj != NULL)
  {
    lpMemTrackerObj->~CNktMemMgrTracker();
    lpMemTrackerObj = NULL;
  }
  NktInterlockedExchange(&nMemTrackerSingletonLock, 0);
  return;
}

//-----------------------------------------------------------

#endif //NKT_ENABLE_MEMORY_TRACKING && !NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
