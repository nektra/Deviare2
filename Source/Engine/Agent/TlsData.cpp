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

#include "TlsData.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static DWORD dwTlsDataIndex = TLS_OUT_OF_INDEXES;
static TNktLnkLst<CNktDvTlsData> cTlsDataList;
static LONG volatile nListLock = 0;
static LONG volatile nAccessLock = 0;

//-----------------------------------------------------------

__inline static VOID AddToList(CNktDvTlsData *lpTlsData)
{
  CNktSimpleLockNonReentrant cListLock(&nListLock);

  cTlsDataList.PushTail(lpTlsData);
  return;
}

__inline static VOID RemoveFromList(CNktDvTlsData *lpTlsData)
{
  CNktSimpleLockNonReentrant cListLock(&nListLock);

  lpTlsData->RemoveNode();
  return;
}

//-----------------------------------------------------------

HRESULT nktDvTlsData_Initialize()
{
  CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);

  dwTlsDataIndex = ::TlsAlloc();
  return (dwTlsDataIndex != TLS_OUT_OF_INDEXES) ? S_OK : E_OUTOFMEMORY;
}

VOID nktDvTlsData_Finalize()
{
  CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);
  CNktDvTlsData *lpTlsData;

  if (dwTlsDataIndex != TLS_OUT_OF_INDEXES)
  {
    while ((lpTlsData = cTlsDataList.PopHead()) != NULL)
    {
      lpTlsData->Release();
    }
    ::TlsFree(dwTlsDataIndex);
    dwTlsDataIndex = TLS_OUT_OF_INDEXES;
  }
  return;
}

HRESULT nktDvTlsData_Get(__out CNktDvTlsData **lplpTlsData, __in BOOL bCreate)
{
  CNktDvTlsData *lpTlsData;

  NKT_ASSERT(lplpTlsData != NULL);
  *lplpTlsData = NULL;
  if (dwTlsDataIndex == TLS_OUT_OF_INDEXES)
    return NKT_DVERR_Cancelled;
  lpTlsData = (CNktDvTlsData*)::TlsGetValue(dwTlsDataIndex);
  if (lpTlsData == (CNktDvTlsData*)1)
    return NKT_DVERR_Cancelled;
  if (lpTlsData == NULL)
  {
    TNktComPtr<CNktDvTlsData> cNewTlsData;
    HRESULT hRes;

    if (bCreate == FALSE)
      return NKT_DVERR_NotFound;
    ::TlsSetValue(dwTlsDataIndex, (CNktDvTlsData*)1); //avoid recursion
    cNewTlsData.Attach(NKT_MEMMGR_NEW CNktDvTlsData);
    if (cNewTlsData == NULL)
    {
      ::TlsSetValue(dwTlsDataIndex, NULL);
      return E_OUTOFMEMORY;
    }
    hRes = cNewTlsData->Initialize();
    if (FAILED(hRes))
    {
      ::TlsSetValue(dwTlsDataIndex, NULL);
      return hRes;
    }
    {
      CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);

      AddToList((CNktDvTlsData*)cNewTlsData);
      ::TlsSetValue(dwTlsDataIndex, (CNktDvTlsData*)cNewTlsData);
      lpTlsData = cNewTlsData.Detach();
    }
  }
  lpTlsData->AddRef();
  *lplpTlsData = lpTlsData;
  return S_OK;
}

VOID nktDvTlsData_OnThreadExit()
{
  CNktDvTlsData *lpTlsData = NULL;

  {
    CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);

    if (dwTlsDataIndex != TLS_OUT_OF_INDEXES)
    {
      lpTlsData = (CNktDvTlsData*)::TlsGetValue(dwTlsDataIndex);
      ::TlsSetValue(dwTlsDataIndex, NULL);
    }
  }
  if (lpTlsData != NULL && lpTlsData != (CNktDvTlsData*)1)
    lpTlsData->Release(); //final release
  return;
}

BOOL nktDvTlsData_CheckInUseStack(__in DWORD dwTid)
{
  CNktSimpleLockNonReentrant cAccessLock(&nAccessLock);

  if (dwTlsDataIndex != TLS_OUT_OF_INDEXES)
  {
    TNktLnkLst<CNktDvTlsData>::Iterator it;
    CNktDvTlsData *lpTlsData;

    for (lpTlsData=it.Begin(cTlsDataList); lpTlsData!=NULL; lpTlsData=it.Next())
    {
      if (lpTlsData->GetTid() == dwTid)
      {
        if (lpTlsData->sHookCallData.cInUseStack.IsEmpty() == FALSE)
          return TRUE;
        break;
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------

CNktDvTlsData::CNktDvTlsData() : CNktDvObject(), TNktLnkLstNode<CNktDvTlsData>()
{
  dwTid = ::GetCurrentThreadId();
  bInCallLock = bSendLoadedUnloadedDllAsync = FALSE;
  return;
}

CNktDvTlsData::~CNktDvTlsData()
{
  TNktLnkLst<CNktDvHookCallDataBase>::Iterator it;
  CNktDvHookCallDataBase *lpItem;

  for (lpItem=it.Begin(sHookCallData.cFreeList); lpItem!=NULL; lpItem=it.Next())
    delete lpItem;
  for (lpItem=it.Begin(sHookCallData.cInUseStack); lpItem!=NULL; lpItem=it.Next())
    delete lpItem;
  RemoveFromList(this);
  return;
}

HRESULT CNktDvTlsData::Initialize()
{
  nktMemSet(&sThreadTimes, 0, sizeof(sThreadTimes));
  return S_OK;
}

VOID CNktDvTlsData::ThreadTimesAccumulate(__in FILETIME &ftCreationTime, __in double nCurrentKernelTimeMs,
                                          __in double nCurrentUserTimeMs,
                                          __in ULONG64 nCurrentCpuClockCycles)
{
  if ((sThreadTimes.nInitFlags & 1) == 0)
  {
    ::FileTimeToSystemTime(&ftCreationTime, &(sThreadTimes.sCreationTime));
    sThreadTimes.nInitFlags |= 1;
  }
  //----
  if ((sThreadTimes.nInitFlags & 2) != 0)
  {
    //accumulate
    if (nCurrentKernelTimeMs > sThreadTimes.sLast.nKernelTimeMs)
      sThreadTimes.nKernelTimeMs += (nCurrentKernelTimeMs - sThreadTimes.sLast.nKernelTimeMs);
    if (nCurrentUserTimeMs > sThreadTimes.sLast.nUserTimeMs)
      sThreadTimes.nUserTimeMs += (nCurrentUserTimeMs - sThreadTimes.sLast.nUserTimeMs);
    if (nCurrentCpuClockCycles > sThreadTimes.sLast.nCpuClockCycles)
      sThreadTimes.nCpuClockCycles += (nCurrentCpuClockCycles - sThreadTimes.sLast.nCpuClockCycles);
  }
  else
  {
    //setup initial times
    sThreadTimes.nKernelTimeMs = nCurrentKernelTimeMs;
    sThreadTimes.nUserTimeMs = nCurrentUserTimeMs;
    sThreadTimes.nCpuClockCycles = nCurrentCpuClockCycles;
    sThreadTimes.nInitFlags |= 2;
  }
  sThreadTimes.sLast.nKernelTimeMs = nCurrentKernelTimeMs;
  sThreadTimes.sLast.nUserTimeMs = nCurrentUserTimeMs;
  sThreadTimes.sLast.nCpuClockCycles = nCurrentCpuClockCycles;
  return;
}

VOID CNktDvTlsData::ThreadTimesSetLast(__in double nCurrentKernelTimeMs, __in double nCurrentUserTimeMs,
                                       __in ULONG64 nCurrentCpuClockCycles)
{
  sThreadTimes.sLast.nKernelTimeMs = nCurrentKernelTimeMs;
  sThreadTimes.sLast.nUserTimeMs = nCurrentUserTimeMs;
  sThreadTimes.sLast.nCpuClockCycles = nCurrentCpuClockCycles;
  return;
}
