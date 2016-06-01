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
#include <intrin.h>
#include "Debug.h"

//-----------------------------------------------------------

#define NKT_FASTMUTEX_USES_CRITICAL_SECTIONS

//-----------------------------------------------------------

#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedCompareExchange)

#define NktInterlockedExchange        _InterlockedExchange
#define NktInterlockedIncrement       _InterlockedIncrement
#define NktInterlockedDecrement       _InterlockedDecrement
#define NktInterlockedCompareExchange _InterlockedCompareExchange

_inline LONG NktInterlockedAnd(__inout LONG volatile *lpnDest, __in LONG nSet)
{
  LONG i, j;

  j = *lpnDest;
  do {
    i = j;
    j = NktInterlockedCompareExchange(lpnDest, i & nSet, i);
  }
  while (i != j);
  return j;
}

_inline LONG NktInterlockedOr(__inout LONG volatile *lpnDest, __in LONG nSet)
{
  LONG i, j;

  j = *lpnDest;
  do {
    i = j;
    j = NktInterlockedCompareExchange(lpnDest, i | nSet, i);
  }
  while (i != j);
  return j;
}

_inline LONG NktInterlockedXor(__inout LONG volatile *lpnDest, __in LONG nSet)
{
  LONG i, j;

  j = *lpnDest;
  do {
    i = j;
    j = NktInterlockedCompareExchange(lpnDest, i ^ nSet, i);
  }
  while (i != j);
  return j;
}

_inline VOID NktInterlockedBitSet(__inout LONG volatile *lpnDest, __in LONG nBit)
{
  LONG nValue = (LONG)(1UL << (nBit & 31));
  NktInterlockedOr(lpnDest+(nBit>>5), nValue);
  return;
}

_inline VOID NktInterlockedBitReset(__inout LONG volatile *lpnDest, __in LONG nBit)
{
  LONG nValue = (LONG)(1UL << (nBit & 31));
  NktInterlockedAnd(lpnDest+(nBit>>5), (~nValue));
  return;
}

_inline BOOL NktInterlockedBitTestAndSet(__inout LONG volatile *lpnDest, __in LONG nBit)
{
  LONG nValue = (LONG)(1UL << (nBit & 31));
  return ((NktInterlockedOr(lpnDest+(nBit>>5), nValue) & nValue) != 0) ? TRUE : FALSE;
}

_inline BOOL NktInterlockedBitTestAndReset(__inout LONG volatile *lpnDest, __in LONG nBit)
{
  LONG nValue = (LONG)(1UL << (nBit & 31));
  return ((NktInterlockedAnd(lpnDest+(nBit>>5), (~nValue)) & nValue) != 0) ? TRUE : FALSE;
}

_inline BOOL NktInterlockedBitTestAndComplement(__inout LONG volatile *lpnDest, __in LONG nBit)
{
  LONG nValue = (LONG)(1UL << (nBit & 31));
  return ((NktInterlockedXor(lpnDest+(nBit>>5), nValue) & nValue) != 0) ? TRUE : FALSE;
}

//-----------------------------------------------------------

class CNktFastMutex
{
public:
  CNktFastMutex()
    {
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    ::InitializeCriticalSection(&cs);
#else //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    NktInterlockedExchange(&nLock, 0);
    NktInterlockedExchange(&nCount, 0);
    bIsMultiProcessor = IsMultiProcessor();
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    return;
    };

  ~CNktFastMutex()
    {
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    ::DeleteCriticalSection(&cs);
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    return;
    };

  _inline VOID Lock()
    {
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    ::EnterCriticalSection(&cs);
#else //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    LONG nTID, nRes;

    nTID = (LONG)::GetCurrentThreadId();
    //take ownership if the lock is free or owned by the calling thread
    for (;;)
    {
      nRes = NktInterlockedCompareExchange(&nLock, nTID, 0);
      if (nRes == 0 || nRes == nTID)
        break;
      if (bIsMultiProcessor == FALSE)
        ::SwitchToThread();
      else
        ::Sleep(1);
    }
    NktInterlockedIncrement(&nCount);
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    return;
    };

  _inline BOOL TryLock()
    {
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    return ::TryEnterCriticalSection(&cs);
#else //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    LONG nTID, nRes;

    nTID = (LONG)::GetCurrentThreadId();
    nRes = NktInterlockedCompareExchange(&nLock, nTID, 0);
    if (nRes == 0 || nRes == nTID)
    {
      NktInterlockedIncrement(&nCount);
      return TRUE;
    }
    return FALSE;
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    };

  _inline VOID Unlock()
    {
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    ::LeaveCriticalSection(&cs);
#else //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    NKT_ASSERT(nLock == (LONG)::GetCurrentThreadId());
    NKT_ASSERT(nCount > 0);
    if (NktInterlockedDecrement(&nCount) == 0)
      NktInterlockedExchange(&nLock, 0);
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
    return;
    };

  static BOOL IsMultiProcessor();

private:
#ifdef NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
  CRITICAL_SECTION cs;
#else //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
  _declspec(align(8)) LONG volatile nLock;
  _declspec(align(8)) LONG volatile nCount;
  BOOL bIsMultiProcessor;
#endif //NKT_FASTMUTEX_USES_CRITICAL_SECTIONS
};

//-----------------------------------------------------------

class CNktAutoFastMutex
{
public:
  CNktAutoFastMutex(__inout CNktFastMutex *lpFastMutex, __in BOOL bTry=FALSE)
    {
    lpFM = lpFastMutex;
    NKT_ASSERT(lpFM != NULL);
    if (bTry == FALSE)
    {
      lpFM->Lock();
      bLockHeld = TRUE;
    }
    else
    {
      bLockHeld = lpFM->TryLock();
    }
    return;
    };

  virtual ~CNktAutoFastMutex()
    {
    if (bLockHeld != FALSE)
      lpFM->Unlock();
    return;
    };

  virtual BOOL IsLockHeld() const
    {
    return bLockHeld;
    };

private:
  CNktFastMutex *lpFM;
  BOOL bLockHeld;
};

class CNktAutoFastMutexReverse
{
public:
  CNktAutoFastMutexReverse(__inout CNktFastMutex *lpFastMutex)
    {
    lpFM = lpFastMutex;
    NKT_ASSERT(lpFM != NULL);
    lpFM->Unlock();
    return;
    };

  virtual ~CNktAutoFastMutexReverse()
    {
    lpFM->Lock();
    return;
    };

private:
  CNktFastMutex *lpFM;
};

//-----------------------------------------------------------

class CNktEvent
{
public:
  CNktEvent();
  virtual ~CNktEvent();

  virtual BOOL Create(__in BOOL bManualReset, __in BOOL bInitialState, __in_z_opt LPCWSTR szName=NULL);
  virtual BOOL Open(__in_z_opt LPCWSTR szName=NULL);
  virtual VOID Destroy();

  virtual HANDLE GetEventHandle()
    {
    return hEvent;
    };

  virtual BOOL Wait(__in DWORD dwTimeout);

  virtual BOOL Reset();
  virtual BOOL Set();
  virtual BOOL Pulse();

  virtual HANDLE Detach();

private:
  HANDLE hEvent;
};

//-----------------------------------------------------------

class CNktMutex
{
public:
  CNktMutex();
  virtual ~CNktMutex();

  virtual BOOL Create(__in_z_opt LPCWSTR szName=NULL, __out_opt LPBOOL lpbAlreadyExists=NULL);
  virtual BOOL Open(__in_z_opt LPCWSTR szName=NULL, __in BOOL bQueryOnly=FALSE);
  virtual VOID Destroy();

  virtual HANDLE GetMutexHandle()
    {
    return hMutex;
    };

  virtual BOOL Lock(__in DWORD dwTimeout=INFINITE);
  virtual BOOL Unlock();

private:
  HANDLE hMutex;
  LONG volatile nHeldCounter;
};

//-----------------------------------------------------------

class CNktAutoMutex
{
public:
  CNktAutoMutex(__in CNktMutex *_lpMtx, __in DWORD dwTimeOut=INFINITE)
    {
    lpMtx = _lpMtx;
    NKT_ASSERT(lpMtx != NULL);
    bLockHeld = lpMtx->Lock(dwTimeOut);
    return;
    };

  virtual ~CNktAutoMutex()
    {
    if (bLockHeld != FALSE)
      lpMtx->Unlock();
    return;
    };

  virtual BOOL IsLockHeld() const
    {
    return bLockHeld;
    };

private:
  CNktMutex *lpMtx;
  BOOL bLockHeld;
};

//-----------------------------------------------------------

class CNktSimpleLockNonReentrant
{
public:
  CNktSimpleLockNonReentrant(__inout LONG volatile *_lpnLock)
    {
    BOOL bIsMultiProcessor;

    lpnLock = _lpnLock;
    bIsMultiProcessor = CNktFastMutex::IsMultiProcessor();
    while (NktInterlockedCompareExchange(lpnLock, 1, 0) != 0)
    {
      if (bIsMultiProcessor == FALSE)
        ::SwitchToThread();
      else
        ::Sleep(1);
    }
    return;
    };

  virtual ~CNktSimpleLockNonReentrant()
    {
    NktInterlockedExchange(lpnLock, 0);
    return;
    };

private:
  LONG volatile *lpnLock;
};

//-----------------------------------------------------------

class CNktFastReadWriteLock
{
public:
  CNktFastReadWriteLock(__in BOOL _bWriterPreferred)
    {
    bWriterPreferred = _bWriterPreferred;
    NktInterlockedExchange(&nReadWriteLock, 0);
    NktInterlockedExchange(&nReaderCount, 0);
    NktInterlockedExchange(&nWriterWait, 0);
    return;
    };

  virtual ~CNktFastReadWriteLock()
    {
    NKT_ASSERT(nReaderCount==0 && nReadWriteLock==0);
    return;
    };

  virtual VOID ReaderLock()
    {
    CNktAutoFastMutex cLock(&cInternalMutex);

    //wait until there are no writers
    while (nWriterWait>0 || (nReadWriteLock>0 && bWriterPreferred!=FALSE))
    {
      CNktAutoFastMutexReverse cRevLock(&cInternalMutex);

      ::Sleep(1);
    }
    //add a reader
    NktInterlockedIncrement(&nReaderCount);
    return;
    };

  virtual BOOL TryReaderLock()
    {
    CNktAutoFastMutex cLock(&cInternalMutex);

    //wait until there are no writers
    if (nWriterWait>0 || (nReadWriteLock>0 && bWriterPreferred!=FALSE))
      return FALSE;
    //add a reader
    NktInterlockedIncrement(&nReaderCount);
    return TRUE;
    };

  virtual VOID WriterLock()
    {
    CNktAutoFastMutex cLock(&cInternalMutex);

    //wait until there are no readers nor writers
    while (nReaderCount>0 || nReadWriteLock>0)
    {
      NktInterlockedIncrement(&nWriterWait);
      {
        CNktAutoFastMutexReverse cRevLock(&cInternalMutex);

        ::Sleep(1);
      }
      NktInterlockedDecrement(&nWriterWait);
    }
    //writer active
    NktInterlockedExchange(&nReadWriteLock, 1);
    return;
    };

  virtual BOOL TryWriterLock()
    {
    CNktAutoFastMutex cLock(&cInternalMutex);

    //check if no readers nor writers
    if (nReaderCount>0 || nReadWriteLock>0)
      return FALSE;
    //writer active
    NktInterlockedExchange(&nReadWriteLock, 1);
    return TRUE;
    };

  virtual VOID Unlock()
    {
    CNktAutoFastMutex cLock(&cInternalMutex);

    //if there are readers, then we are read-locked
    if (nReaderCount > 0)
    {
      //decrement reader count
      NktInterlockedDecrement(&nReaderCount);
    }
    else
    {
      //writer inactive
      NktInterlockedExchange(&nReadWriteLock, 0);
    }
    return;
    };

private:
#pragma pack(push, 8)
  CNktFastMutex cInternalMutex;
  LONG volatile nReadWriteLock;
  LONG volatile nReaderCount;
  LONG volatile nWriterWait;
  BOOL bWriterPreferred;
#pragma pack(pop)
};

//-----------------------------------------------------------

class CNktAutoFastReadWriteLock
{
public:
  CNktAutoFastReadWriteLock(__in CNktFastReadWriteLock *lpRwLock, __in BOOL bWrite, __in BOOL bTry=FALSE)
  {
    lpLock = lpRwLock;
    NKT_ASSERT(lpLock != NULL);
    if (bWrite == FALSE)
    {
      if (bTry == FALSE)
      {
        lpLock->ReaderLock();
        bLockHeld = TRUE;
      }
      else
        bLockHeld = lpLock->TryReaderLock();
    }
    else
    {
      if (bTry == FALSE)
      {
        lpLock->WriterLock();
        bLockHeld = TRUE;
      }
      else
      {
        bLockHeld = lpLock->TryWriterLock();
      }
    }
    return;
    };

  virtual ~CNktAutoFastReadWriteLock()
    {
    if (bLockHeld != FALSE)
      lpLock->Unlock();
    return;
    };

  virtual BOOL IsLockHeld() const
    {
    return bLockHeld;
    };

private:
  CNktFastReadWriteLock *lpLock;
  BOOL bLockHeld;
};

//-----------------------------------------------------------

