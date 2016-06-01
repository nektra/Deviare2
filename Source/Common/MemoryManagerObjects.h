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

#include <windows.h>
#include <stdio.h>
#include <new.h>
#include "MemoryManager.h"
#include "WaitableObjects.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #define NKT_MEMMGR_NEW new(THIS_FILE, __LINE__)
#else //NKT_ENABLE_MEMORY_TRACKING
  #define NKT_MEMMGR_NEW new
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

class __declspec(novtable) CNktMemMgrObj
{
public:
  void* __cdecl operator new(__in size_t nSize)
    {
    return nktMemMalloc(nSize);
    };
  void* __cdecl operator new[](__in size_t nSize)
    {
    return nktMemMalloc(nSize);
    };
  void* __cdecl operator new(__in size_t nSize, __inout void* lpInPlace)
    {
    return lpInPlace;
    };
  void* __cdecl operator new(__in size_t nSize, __in const std::nothrow_t&) throw()
    {
    return nktMemMalloc(nSize);
    };

  void __cdecl operator delete(__inout void* p)
    {
    nktMemFree(p);
    return;
    };
  void __cdecl operator delete[](__inout void* p)
    {
    nktMemFree(p);
    return;
    };
  void __cdecl operator delete(__inout void* p, __inout void* lpPlace)
    {
    return;
    };
  void __cdecl operator delete(__inout void *p, __in const std::nothrow_t&) throw()
    {
    nktMemFree(p);
    return;
    };

#ifdef NKT_ENABLE_MEMORY_TRACKING
  //for file name/line number tracking using SL_ALLOC_NEW
  void* __cdecl operator new(__in size_t nSize, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    return _nktMemMallocWithTrack(nSize, szFileName, nLineNumber);
    };
  void* __cdecl operator new[](__in size_t nSize, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    return _nktMemMallocWithTrack(nSize, szFileName, nLineNumber);
    };
#if _MSC_VER >= 1200
  void __cdecl operator delete(__inout void* p, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    _nktMemFreeWithTrack(p);
    return;
    };
  void __cdecl operator delete[](__inout void* p, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    _nktMemFreeWithTrack(p);
    return;
    };
#endif //_MSC_VER >= 1200
#endif //NKT_ENABLE_MEMORY_TRACKING
};

//-----------------------------------------------------------

template <class T>
class __declspec(novtable) TNktMemMgrObjPool
{
public:
  void* __cdecl operator new(__in size_t nSize)
    {
    return IntMemAlloc(nSize, NULL, 0);
    };
  
  void* __cdecl operator new(__in size_t nSize, __inout void* lpInPlace)
    {
    return lpInPlace;
    };

  void __cdecl operator delete(__inout void* p)
    {
    IntMemFree(p);
    return;
    };
#if _MSC_VER >= 1200
  void __cdecl operator delete(__inout void* p, __inout void* lpPlace)
    {
    return;
    };
#endif //_MSC_VER >= 1200

#ifdef NKT_ENABLE_MEMORY_TRACKING
  //for file name/line number tracking using SL_ALLOC_NEW
  void* __cdecl operator new(__in size_t nSize, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    return IntMemAlloc(nSize, szFileName, nLineNumber);
    };

#if _MSC_VER >= 1200
  void __cdecl operator delete(__inout void* p, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    _nktMemFreeWithTrack(p);
    return;
    };
#endif //_MSC_VER >= 1200
#endif //NKT_ENABLE_MEMORY_TRACKING

  static VOID MemSetMaxAge(__in DWORD _dwMaxAge)
    {
    dwMaxAge = _dwMaxAge;
    return;
    };

  static VOID MemSetCheckFrequency(__in DWORD _dwCheckFreq)
    {
    dwCheckFreq = _dwCheckFreq;
    return;
    };

private:
  static LPVOID IntMemAlloc(__in size_t nSize, __in_z_opt const char *szFileName, __in int nLineNumber)
    {
    CNktAutoFastMutex cLock(&cListMtx);
    LPITEM lpItem;

    NKT_ASSERT(nSize <= sizeof(T));
    if (sFreeList.lpHead != NULL)
    {
      //remove from the free list
      lpItem = sFreeList.lpHead;
      if (lpItem == sFreeList.lpTail)
      {
        sFreeList.lpTail = NULL;
        NKT_ASSERT(lpItem->lpNext = NULL);
      }
      sFreeList.lpHead = lpItem->lpNext;
    }
    else
    {
#ifdef NKT_ENABLE_MEMORY_TRACKING
      lpItem = (LPITEM)_nktMemMallocWithTrack(sizeof(ITEM), szFileName, nLineNumber);
#else //NKT_ENABLE_MEMORY_TRACKING
      lpItem = (LPITEM)nktMemMalloc(sizeof(ITEM));
#endif //NKT_ENABLE_MEMORY_TRACKING
      if (lpItem == NULL)
        return NULL;
    }
    return &(lpItem->aBuffer);
    };

  static VOID IntMemFree(__in LPVOID lpPtr)
    {
    if (lpPtr != NULL)
    {
      CNktAutoFastMutex cLock(&cListMtx);
      LPITEM lpItem;

      lpItem = (LPITEM)((LPBYTE)lpPtr - FIELD_OFFSET(ITEM, aBuffer));
      //add this item to the free list
      lpItem->lpPrev = NULL;
      lpItem->lpNext = sFreeList.lpHead;
      if (sFreeList.lpHead != NULL)
        sFreeList.lpHead->lpPrev = lpItem;
      if (sFreeList.lpTail == NULL)
        sFreeList.lpTail = lpItem;
      sFreeList.lpHead = lpItem;
      lpItem->dwLastInUseTime = ::GetTickCount();
      if ((++dwCheckFreqCounter) >= dwCheckFreq)
      {
        dwCheckFreqCounter = 0;
        IntMemTrimSpace((lpItem->dwLastInUseTime != 0) ? (lpItem->dwLastInUseTime) : 0);
      }
    }
    return;
    };

public:
  static VOID IntMemCleanup()
    {
    LPITEM lpItem, lpNextItem;

    for (lpItem=sFreeList.lpHead; lpItem!=NULL; lpItem=lpNextItem)
    {
      lpNextItem = lpItem->lpNext;
#ifdef NKT_ENABLE_MEMORY_TRACKING
      _nktMemFreeWithTrack(lpItem);
#else //NKT_ENABLE_MEMORY_TRACKING
      nktMemFree(lpItem);
#endif //NKT_ENABLE_MEMORY_TRACKING
    }
    sFreeList.lpHead = sFreeList.lpTail = NULL;
    return;
    };

private:
  static VOID IntMemTrimSpace(__in DWORD dwTime)
    {
    LPITEM lpPrevItem;
    DWORD dw;

    while (sFreeList.lpTail != NULL)
    {
      lpPrevItem = sFreeList.lpTail->lpPrev;
      if (dwTime >= sFreeList.lpTail->dwLastInUseTime)
        dw = dwTime - sFreeList.lpTail->dwLastInUseTime;
      else
        dw = (~(sFreeList.lpTail->dwLastInUseTime))+dwTime+1UL;
      if (dw < dwMaxAge)
        break;
      //remove this tail item
      if (lpPrevItem != NULL)
        lpPrevItem->lpNext = NULL;
      if (sFreeList.lpTail == sFreeList.lpHead)
      {
        sFreeList.lpHead = NULL;
        NKT_ASSERT(lpPrevItem == NULL);
      }
#ifdef NKT_ENABLE_MEMORY_TRACKING
      _nktMemFreeWithTrack(sFreeList.lpTail);
#else //NKT_ENABLE_MEMORY_TRACKING
      nktMemFree(sFreeList.lpTail);
#endif //NKT_ENABLE_MEMORY_TRACKING
      sFreeList.lpTail = lpPrevItem;
    }
    return;
    };

private:
  typedef struct tagITEM {
    struct tagITEM *lpNext, *lpPrev;
    union {
      DWORD dwLastInUseTime;
      ULONGLONG _pad0;
    };
    BYTE aBuffer[sizeof(T)];
  } ITEM, *LPITEM;

  typedef struct tagLIST {
    LPITEM lpHead, lpTail;
  } LIST;

  static DWORD dwMaxAge; //10secs default
  static DWORD dwCheckFreq; //128 rounds default
  static DWORD dwCheckFreqCounter;
  static CNktFastMutex cListMtx;
  static LIST sFreeList;
};

#define DECLARE_TNKTMEMMGROBJPOOL_VARS(T)                                                    \
template <class T>                                                                           \
DWORD TNktMemMgrObjPool<T>::dwMaxAge = 10000;                                                \
template <class T>                                                                           \
DWORD TNktMemMgrObjPool<T>::dwCheckFreq = 128;                                               \
template <class T>                                                                           \
DWORD TNktMemMgrObjPool<T>::dwCheckFreqCounter = 0;                                          \
template <class T>                                                                           \
CNktFastMutex TNktMemMgrObjPool<T>::cListMtx;                                                \
template <class T>                                                                           \
typename TNktMemMgrObjPool<T>::LIST TNktMemMgrObjPool<T>::sFreeList = { NULL, NULL };        \
static void __cdecl Pool_Finalize_##T(void)                                                  \
  { TNktMemMgrObjPool<T>::IntMemCleanup(); };                                                \
NKTMEMMGR_PRAGMASECTION(".CRT$XTV600");                                                      \
__pragma(data_seg(".CRT$XTV600"))                                                            \
static __declspec(allocate(".CRT$XTV600")) void (__cdecl *a##T##_Pool_Finalizer[])(void) = { \
  &(Pool_Finalize_##T) };                                                                    \
__pragma(data_seg())

//-----------------------------------------------------------

