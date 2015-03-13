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

#include "DynamicConnectionPoint.h"

//-----------------------------------------------------------

#define IDDCP_EVENT_Exit                                   0
#define IDDCP_EVENT_ItemAvailable                          1

//-----------------------------------------------------------

class CDelayableDynamicConnectionPointWorkerThread : public CNktDvObject, protected CNktThread,
                                                     protected CNktFastMutex
{
protected:
  CDelayableDynamicConnectionPointWorkerThread();
public:
  virtual ~CDelayableDynamicConnectionPointWorkerThread();

  static CDelayableDynamicConnectionPointWorkerThread* Get();
  ULONG __stdcall Release();

  HRESULT FireWithDelay(__in IDispatch *lpDisp, __in DISPID nDispId, __in CComVariant *lpVarParams,
                        __in int nVarParamsCount);

private:
  class ITEM;

  VOID ThreadProc();
  HRESULT ProcessDelayedEvent(__in ITEM *lpItem);

  class ITEM : public CNktMemMgrObj, public TNktLnkLstNode<ITEM>
  {
  public:
    ITEM();
    ~ITEM();

  public:
    TNktComPtr<IStream> cIStream;
    DISPID nDispId;
    VARIANT *lpVarParams;
    LPBYTE lpnVarParamsReleaseMarshaled;
    int nVarParamsCount;
    BOOL bReleaseMarshaled;
  };

  CNktEvent cItemAvailEv;
  TNktLnkLst<ITEM> cDelayedEventsList;
};

//-----------------------------------------------------------

template <class T, typename _uuid>
class ATL_NO_VTABLE IDelayableDynamicConnectionPoint : public IDynamicConnectionPoint<T, _uuid>
{
public:
  IDelayableDynamicConnectionPoint() : IDynamicConnectionPoint()
    {
    NktInterlockedExchange(&nMutex, 0);
    return;
    };

  HRESULT FireWithDelay(__in IDispatch *lpDisp, __in DISPID nDispId, __in CComVariant *lpVarParams,
                        __in int nVarParamsCount)
    {
    if (cWorker == NULL)
    {
      CNktSimpleLockNonReentrant cLock(&nMutex);

      if (cWorker == NULL)
      {
        cWorker.Attach(CDelayableDynamicConnectionPointWorkerThread::Get());
        if (cWorker == NULL)
          return E_OUTOFMEMORY;
      }
    }
    return cWorker->FireWithDelay(lpDisp, nDispId, lpVarParams, nVarParamsCount);
    };

private:
  LONG volatile nMutex;
  TNktComPtr<CDelayableDynamicConnectionPointWorkerThread> cWorker;
};
