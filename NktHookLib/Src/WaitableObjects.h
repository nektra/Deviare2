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

#ifndef _NKT_WAITABLEOBJECTS_H
#define _NKT_WAITABLEOBJECTS_H

#include <windows.h>
#include <intrin.h>
#include "DynamicNtApi.h"
#include "NtHeapBaseObj.h"

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedCompareExchange)

_inline LONG NktInterlockedAnd(__inout LONG volatile *lpnDest, __in LONG nSet)
{
  LONG i, j;

  j = *lpnDest;
  do {
    i = j;
    j = _InterlockedCompareExchange(lpnDest, i & nSet, i);
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
    j = _InterlockedCompareExchange(lpnDest, i | nSet, i);
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
    j = _InterlockedCompareExchange(lpnDest, i ^ nSet, i);
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

BOOL NktIsMultiProcessor();
VOID NktYieldProcessor();

//-----------------------------------------------------------

class CNktFastMutex : public CNktNtHeapBaseObj
{
public:
  CNktFastMutex() : CNktNtHeapBaseObj()
    {
    NktRtlInitializeCriticalSection(&cs);
    return;
    };

  ~CNktFastMutex()
    {
    NktRtlDeleteCriticalSection(&cs);
    return;
    };

  _inline VOID Lock()
    {
    NktRtlEnterCriticalSection(&cs);
    return;
    };

  _inline VOID Unlock()
    {
    NktRtlLeaveCriticalSection(&cs);
    return;
    };

private:
  CRITICAL_SECTION cs;
};

//-----------------------------------------------------------

class CNktAutoFastMutex : public CNktNtHeapBaseObj
{
public:
  CNktAutoFastMutex(__inout CNktFastMutex *lpFastMutex) : CNktNtHeapBaseObj()
    {
    lpFM = lpFastMutex;
    NKT_ASSERT(lpFM != NULL);
    lpFM->Lock();
    return;
    };

  virtual ~CNktAutoFastMutex()
    {
    lpFM->Unlock();
    return;
    };

private:
  CNktFastMutex *lpFM;
};

class CNktAutoFastMutexReverse : public CNktNtHeapBaseObj
{
public:
  CNktAutoFastMutexReverse(__inout CNktFastMutex *lpFastMutex) : CNktNtHeapBaseObj()
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

class CNktEvent : public CNktNtHeapBaseObj
{
public:
  CNktEvent();
  ~CNktEvent();

  BOOL Create(__in BOOL bManualReset, __in BOOL bInitialState, __in_z_opt LPCWSTR szName=NULL);
  BOOL Open(__in_z_opt LPCWSTR szName=NULL);
  VOID Destroy();

  HANDLE GetEventHandle()
    {
    return hEvent;
    };

  //The assert on operator& usually indicates a bug.  If this is really
  //what is needed, however, take the address of the lpPtr member explicitly.
  HANDLE* operator&()
    {
    NKT_ASSERT(hEvent == NULL);
    return &hEvent;
    };

  BOOL Wait(__in DWORD dwTimeout);

  BOOL Reset();
  BOOL Set();
  BOOL Pulse();

  VOID Attach(__in HANDLE hEvent);
  HANDLE Detach();

private:
  HANDLE hEvent;
};

//-----------------------------------------------------------

class CNktSimpleLockNonReentrant : public CNktNtHeapBaseObj
{
public:
  CNktSimpleLockNonReentrant(__inout LONG volatile *_lpnLock) : CNktNtHeapBaseObj()
  {
    lpnLock = _lpnLock;
    while (_InterlockedCompareExchange(lpnLock, 1, 0) != 0)
      NktYieldProcessor();
    return;
  };

  ~CNktSimpleLockNonReentrant()
  {
    _InterlockedExchange(lpnLock, 0);
    return;
  };

private:
  LONG volatile *lpnLock;
};

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_WAITABLEOBJECTS_H
