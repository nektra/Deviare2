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

#include "StdAfx.h"
#include "DelayableDynamicConnectionPoint.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static CDelayableDynamicConnectionPointWorkerThread *lpThread = NULL;
static LONG volatile nMutex = 0;

//-----------------------------------------------------------

CDelayableDynamicConnectionPointWorkerThread::CDelayableDynamicConnectionPointWorkerThread() :
                                                  CNktDvObject(), CNktThread(), CNktFastMutex()
{
  return;
}

CDelayableDynamicConnectionPointWorkerThread::~CDelayableDynamicConnectionPointWorkerThread()
{
  TNktLnkLst<ITEM>::Iterator it;
  ITEM *lpItem;

  Stop();
  cItemAvailEv.Destroy();
  //free unprocessed events
  for (lpItem=it.Begin(cDelayedEventsList); lpItem!=NULL; lpItem=it.Next())
  {
    lpItem->RemoveNode();
    delete lpItem;
  }
  return;
}

CDelayableDynamicConnectionPointWorkerThread* CDelayableDynamicConnectionPointWorkerThread::Get()
{
  TNktComPtr<CDelayableDynamicConnectionPointWorkerThread> cThread;

  {
    CNktSimpleLockNonReentrant cLock(&nMutex);

    if (lpThread != NULL)
    {
      lpThread->AddRef();
      return lpThread;
    }
  }
  //create new worker
  cThread.Attach(NKT_MEMMGR_NEW CDelayableDynamicConnectionPointWorkerThread);
  if (cThread == NULL)
    return NULL;
  if (cThread->cItemAvailEv.Create(TRUE, FALSE) == FALSE ||
      cThread->Start() == FALSE)
    return NULL;
  //check again if worker thread already existed
  {
    CNktSimpleLockNonReentrant cLock(&nMutex);

    if (lpThread == NULL)
      lpThread = cThread.Detach();
    else
      lpThread->AddRef();
  }
  return lpThread;
}

ULONG CDelayableDynamicConnectionPointWorkerThread::Release()
{
  CNktSimpleLockNonReentrant cLock(&nMutex);

  return CNktDvObject::Release();
}

HRESULT CDelayableDynamicConnectionPointWorkerThread::FireWithDelay(__in IDispatch *lpDisp,
                         __in DISPID nDispId, __in CComVariant *lpVarParams, __in int nVarParamsCount)
{
  CNktAutoFastMutex cAutoLock(this);
  TNktComPtr<IStream> cIStream;
  TNktAutoDeletePtr<ITEM> cNewItem;
  int i;
  HRESULT hRes;

  if (lpDisp == NULL)
    return E_POINTER;
  if (nVarParamsCount > 0 && lpVarParams == NULL)
    return E_POINTER;
  //create new item
  cNewItem.Attach(NKT_MEMMGR_NEW ITEM);
  if (cNewItem == NULL)
    return E_OUTOFMEMORY;
  hRes = ::CreateStreamOnHGlobal(NULL, FALSE, &(cNewItem->cIStream));
  if (SUCCEEDED(hRes))
  {
    hRes = ::CoMarshalInterface(cNewItem->cIStream, IID_IDispatch, lpDisp, MSHCTX_INPROC, NULL,
                                MSHLFLAGS_NORMAL);
  }
  if (FAILED(hRes))
    return hRes;
  cNewItem->nDispId = nDispId;
  if (nVarParamsCount > 0)
  {
    //allocate for variant parameters and "lpnVarParamsReleaseMarshaled"
    cNewItem->lpVarParams = (VARIANT*)nktMemMalloc((SIZE_T)nVarParamsCount*sizeof(VARIANT));
    cNewItem->lpnVarParamsReleaseMarshaled = (LPBYTE)nktMemMalloc((SIZE_T)nVarParamsCount*sizeof(BYTE));
    if (cNewItem->lpVarParams == NULL || cNewItem->lpnVarParamsReleaseMarshaled == NULL)
      return E_OUTOFMEMORY;
    nktMemSet(cNewItem->lpVarParams, 0, (SIZE_T)nVarParamsCount*sizeof(VARIANT));
    nktMemSet(cNewItem->lpnVarParamsReleaseMarshaled, 1, (SIZE_T)nVarParamsCount*sizeof(BYTE));
    cNewItem->nVarParamsCount = nVarParamsCount;
    //process parameters
    for (i=0; i<nVarParamsCount; i++)
    {
      //marshasl IUnknown & IDispatch interfaces and replace the "punkVal" with the IStream pointer
      if ((lpVarParams[i].vt == VT_UNKNOWN && lpVarParams[i].punkVal != NULL) ||
          (lpVarParams[i].vt == VT_DISPATCH && lpVarParams[i].pdispVal != NULL))
      {
        cIStream.Release();
        hRes = ::CreateStreamOnHGlobal(NULL, FALSE, &cIStream);
        if (SUCCEEDED(hRes))
        {
          if (lpVarParams[i].vt == VT_UNKNOWN)
          {
            hRes = ::CoMarshalInterface(cIStream, IID_IUnknown, lpVarParams[i].punkVal, MSHCTX_INPROC,
                                        NULL, MSHLFLAGS_NORMAL);
          }
          else
          {
            hRes = ::CoMarshalInterface(cIStream, IID_IDispatch, lpVarParams[i].pdispVal, MSHCTX_INPROC,
                                        NULL, MSHLFLAGS_NORMAL);
          }
        }
        if (FAILED(hRes))
          return hRes;
        cNewItem->lpVarParams[i].vt = lpVarParams[i].vt;
        cNewItem->lpVarParams[i].punkVal = cIStream.Detach();
      }
      else
      {
        ::VariantCopy(&(cNewItem->lpVarParams[i]), &(lpVarParams[i]));
      }
    }
  }
  //add to list
  cDelayedEventsList.PushTail(cNewItem.Detach());
  cItemAvailEv.Set();
  return S_OK;
}

VOID CDelayableDynamicConnectionPointWorkerThread::ThreadProc()
{
  DWORD dwCheckCounter, dwHitEv;
  HANDLE hEvent;
  ITEM *lpItem;
  HRESULT hRes;

  hEvent = cItemAvailEv.GetEventHandle();
  while (CheckForAbort(INFINITE, 1, &hEvent, &dwHitEv) == FALSE)
  {
    if (dwHitEv == 1)
    {
      cItemAvailEv.Reset();
      dwCheckCounter = 0;
      do
      {
        if (((dwCheckCounter++) & 3) == 3 && CheckForAbort(0) != FALSE)
          break;
        //get first item and remove from list
        {
          CNktAutoFastMutex cAutoLock(this);

          lpItem = cDelayedEventsList.PopHead();
        }
        if (lpItem != NULL)
        {
          hRes = ProcessDelayedEvent(lpItem);
          //delete item
          delete lpItem;
        }
      }
      while (lpItem != NULL);
    }
  }
  return;
}

//NOTE: May return DISP_E_MEMBERNOTFOUND
HRESULT CDelayableDynamicConnectionPointWorkerThread::ProcessDelayedEvent(__in ITEM *lpItem)
{
  TNktComPtr<IDispatch> cConnDisp;
  TNktComPtr<IStream> cIStream;
  DISPPARAMS sParams = { 0 };
  CComVariant cVarResult;
  LARGE_INTEGER li = { 0 };
  UINT nArgErr;
  int i;
  HRESULT hRes;

  //unmarshal interfaces
  hRes = lpItem->cIStream->Seek(li, STREAM_SEEK_SET, NULL);
  if (SUCCEEDED(hRes))
    hRes = ::CoUnmarshalInterface(lpItem->cIStream, IID_IDispatch, (LPVOID*)&cConnDisp);
  if (FAILED(hRes))
    return hRes;
  lpItem->bReleaseMarshaled = FALSE;
  //unmarshal parameters
  if (lpItem->nVarParamsCount > 0)
  {
    for (i=0; i<lpItem->nVarParamsCount; i++)
    {
      //check if this parameters is a IUnknown/IDispach stream-Marshaled interface
      if ((lpItem->lpVarParams[i].vt == VT_UNKNOWN || lpItem->lpVarParams[i].vt == VT_DISPATCH) &&
          lpItem->lpVarParams[i].punkVal != NULL)
      {
        //punkVal has the IStream
        cIStream.Release();
        cIStream.Attach((IStream*)(lpItem->lpVarParams[i].punkVal));
        hRes = cIStream->Seek(li, STREAM_SEEK_SET, NULL);
        if (lpItem->lpVarParams[i].vt == VT_UNKNOWN)
        {
          hRes = ::CoUnmarshalInterface(cIStream, IID_IUnknown,
                                        (LPVOID*)&(lpItem->lpVarParams[i].punkVal));
        }
        else
        {
          hRes = ::CoUnmarshalInterface(cIStream, IID_IDispatch,
                                        (LPVOID*)&(lpItem->lpVarParams[i].pdispVal));
        }
        if (FAILED(hRes))
        {
          //on fail, detach stream leaving it on the variant
          lpItem->lpVarParams[i].punkVal = (IUnknown*)(cIStream.Detach());
          return hRes;
        }
        //on succeed, mark not to release marshaled
        lpItem->lpnVarParamsReleaseMarshaled[i] = 0;
      }
    }
  }
  //do fire
  sParams.rgvarg = lpItem->lpVarParams;
  sParams.cArgs = lpItem->nVarParamsCount;
  try
  {
    hRes = cConnDisp->Invoke(lpItem->nDispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &sParams,
                             &cVarResult, NULL, &nArgErr);
  }
  catch (...)
  {
    hRes = E_FAIL;
  }
  return hRes;
}

//-----------------------------------------------------------

CDelayableDynamicConnectionPointWorkerThread::ITEM::ITEM() : CNktMemMgrObj(), TNktLnkLstNode<ITEM>()
{
  lpVarParams = NULL;
  nVarParamsCount = 0;
  lpnVarParamsReleaseMarshaled = NULL;
  bReleaseMarshaled = TRUE;
  nDispId = 0;
  return;
}

CDelayableDynamicConnectionPointWorkerThread::ITEM::~ITEM()
{
  IStream *lpStream;
  int i;

  //free variants
  if (lpVarParams != NULL && lpnVarParamsReleaseMarshaled != NULL)
  {
    for (i=0; i<nVarParamsCount; i++)
    {
      //check if this parameters is a IUnknown/IDispach
      if ((lpVarParams[i].vt == VT_UNKNOWN || lpVarParams[i].vt == VT_DISPATCH) &&
          lpVarParams[i].punkVal != NULL)
      {
        if (lpnVarParamsReleaseMarshaled[i] != 0)
        {
          //if we reach here, we still have a stream-marshaled interface so free it
          lpStream = reinterpret_cast<IStream*>(lpVarParams[i].punkVal);
          ::CoReleaseMarshalData(lpStream);
          lpStream->Release();
        }
        else
        {
          //if this parameter was processed, we have a real variant
          ::VariantClear(&(lpVarParams[i]));
        }
      }
    }
  }
  if (lpVarParams != NULL)
    nktMemFree(lpVarParams);
  if (lpnVarParamsReleaseMarshaled != NULL)
    nktMemFree(lpnVarParamsReleaseMarshaled);
  //check if "lpDisp" marshaled data should be released
  if (cIStream != NULL && bReleaseMarshaled != FALSE)
    ::CoReleaseMarshalData(cIStream);
  return;
}
