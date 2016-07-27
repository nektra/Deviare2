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

//-----------------------------------------------------------

template <class T, typename _uuid>
class ATL_NO_VTABLE IDynamicConnectionPoint : public _ICPLocator<&__uuidof(_uuid)>
{
  typedef CComEnum<IEnumConnections, &__uuidof(IEnumConnections), CONNECTDATA,
                   _Copy<CONNECTDATA>> CComEnumConnections;

public:
  IDynamicConnectionPoint() : _ICPLocator<&__uuidof(_uuid)>()
    {
    return;
    };

  ~IDynamicConnectionPoint()
    {
    SIZE_T i, nCount;

    nCount = aList.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (aList[i].lpStream != NULL)
      {
        ::CoReleaseMarshalData(aList[i].lpStream);
        aList[i].lpStream->Release();
      }
      if (aList[i].lpObj != NULL)
        aList[i].lpObj->Release();
    }
    aList.RemoveAllElements();
    return;
    };

  STDMETHOD(_LocCPQueryInterface)(__in REFIID riid, __deref_out void **ppvObject)
    {
    ATLASSERT(ppvObject != NULL);
    if (ppvObject == NULL)
      return E_POINTER;
    *ppvObject = NULL;
    if (InlineIsEqualGUID(riid, __uuidof(IConnectionPoint)) || InlineIsEqualUnknown(riid))
    {
      *ppvObject = this;
      AddRef();
      return S_OK;
    }
    return E_NOINTERFACE;
    };

  STDMETHOD(GetConnectionInterface)(__out IID* piid2)
    {
    if (piid2 == NULL)
      return E_POINTER;
    *piid2 = __uuidof(_uuid);
    return S_OK;
    };

  STDMETHOD(GetConnectionPointContainer)(__in IConnectionPointContainer** ppCPC)
    {
    T* pT = static_cast<T*>(this);

    // No need to check ppCPC for NULL since QI will do that for us
    return pT->QueryInterface(__uuidof(IConnectionPointContainer), (void**)ppCPC);
    };

  STDMETHOD(Advise)(__in IUnknown *pUnkSink, __out DWORD *pdwCookie)
    {
    T* pT = static_cast<T*>(this);
    ITEM *lpItem;
    TNktComPtr<IStream> cStream;
    TNktComPtr<IDispatch> p;
    HRESULT hRes;

    if (pdwCookie != NULL)
      *pdwCookie = 0;
    if (pUnkSink == NULL || pdwCookie == NULL)
      return E_POINTER;
    hRes = pUnkSink->QueryInterface(IID_IDispatch, (void**)&p);
    if (FAILED(hRes))
      return (hRes == E_NOINTERFACE) ? CONNECT_E_CANNOTCONNECT : hRes;
    //create marshaller
    hRes = ::CreateStreamOnHGlobal(NULL, TRUE, &cStream);
    if (SUCCEEDED(hRes))
      hRes = ::CoMarshalInterface(cStream, IID_IDispatch, p, MSHCTX_INPROC, NULL, MSHLFLAGS_TABLESTRONG);
    if (SUCCEEDED(hRes))
    {
      CNktAutoFastMutex cLock(&cMtx);
      SIZE_T i, nCount;

      //find a free slot and add to list
      nCount = aList.GetCount();
      for (i=0; i<nCount; i++)
      {
        if (aList[i].lpObj == NULL)
          break;
      }
      if (i >= nCount)
      {
        lpItem = aList.ReserveBlock(1);
        if (lpItem != NULL)
        {
          lpItem->lpObj = p.Detach();
          lpItem->lpStream = cStream.Detach();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        aList[i].lpObj = p.Detach();
        aList[i].lpStream = cStream.Detach();
      }
      *pdwCookie = (DWORD)(i+1);
    }
    if (FAILED(hRes))
      ::CoReleaseMarshalData(cStream);
    return hRes;
    };

  STDMETHOD(Unadvise)(__in DWORD dwCookie)
    {
    T* pT = static_cast<T*>(this);
    TNktComPtr<IDispatch> p;
    TNktComPtr<IStream> cStream;
    LARGE_INTEGER li;

    {
      CNktAutoFastMutex cLock(&cMtx);

      if (dwCookie < 1 || (SIZE_T)dwCookie > aList.GetCount())
      {
        NKT_ASSERT(FALSE);
        return CONNECT_E_NOCONNECTION;
      }
      dwCookie--;
      p.Attach(aList[dwCookie].lpObj);
      cStream.Attach(aList[dwCookie].lpStream);
      aList[dwCookie].lpStream = NULL;
      aList[dwCookie].lpObj = NULL;
    }
    if (cStream != NULL)
    {
      li.QuadPart = 0;
      cStream->Seek(li, SEEK_SET, NULL);
      ::CoReleaseMarshalData(cStream);
    }
    return S_OK;
    };

  STDMETHOD(EnumConnections)(__deref_out IEnumConnections** ppEnum)
    {
    T* pT = static_cast<T*>(this);
    CComObject<CComEnumConnections>* pEnum;
    TNktComPtr<IStream> cStream;
    TNktComPtr<IUnknown> cUnk;
    TNktComPtr<IDispatch> p;
    LARGE_INTEGER li;
    CONNECTDATA *lpCd;
    SIZE_T i, k, nItems, nCount;
    HRESULT hRes;

    if (ppEnum == NULL)
      return E_POINTER;
    *ppEnum = NULL;
    pEnum = NULL;
    ATLTRY(pEnum = new CComObject<CComEnumConnections>)
    if (pEnum == NULL)
      return E_OUTOFMEMORY;
    {
      CNktAutoFastMutex cLock(&cMtx);

      nCount = aList.GetCount();
      lpCd = new CONNECTDATA[(nCount > 0) ? nCount : 1];
      if (lpCd == NULL)
      {
        delete pEnum;
        return E_OUTOFMEMORY;
      }
      //duplicate streams
      hRes = S_OK;
      for (i=nItems=0; i<nCount && SUCCEEDED(hRes); i++)
      {
        if (aList[i].lpObj != NULL)
        {
          hRes = aList[i].lpStream->Clone((IStream**)&(lpCd[nItems].pUnk));
          if (FAILED(hRes))
            break;
          lpCd[nItems].dwCookie = (DWORD)(i+1);
          nItems++;
        }
      }
      if (FAILED(hRes))
      {
        for (i=0; i<nItems; i++)
          ((IStream*)(lpCd[nItems].pUnk))->Release();
        delete lpCd;
        delete pEnum;
        return hRes;
      }
    }
    for (i=0; i<nItems; i++)
    {
      cStream.Release();
      cStream.Attach((IStream*)(lpCd[i].pUnk));
      li.QuadPart = 0;
      hRes = cStream->Seek(li, SEEK_SET, NULL);
      if (SUCCEEDED(hRes))
      {
        p.Release();
        hRes = ::CoUnmarshalInterface(cStream, IID_IDispatch, (void**)&p);
      }
      if (SUCCEEDED(hRes))
        hRes = p->QueryInterface<IUnknown>(&(lpCd[i].pUnk));
      if (FAILED(hRes))
      {
        for (k=0; k<nItems; k++)
        {
          if (k < i)
          {
            lpCd[k].pUnk->Release(); //pUnk are real IUnknown interfaces
          }
          else if (k > i)
          {
            ((IStream*)(lpCd[k].pUnk))->Release(); //pUnk are IStream interfaces
          }
        }
        return hRes;
      }
    }
    //don't copy the data, but transfer ownership to it
    pEnum->Init(lpCd, lpCd+nItems, NULL, AtlFlagTakeOwnership);
    hRes = pEnum->_InternalQueryInterface(__uuidof(IEnumConnections), (void**)ppEnum);
    if (FAILED(hRes))
      delete pEnum;
    return hRes;
    };

  HRESULT GetConnAt(__deref_out IDispatch **lplpDisp, __in int nIndex)
    {
    T* pT = static_cast<T*>(this);
    TNktComPtr<IStream> cStream;
    LARGE_INTEGER li;
    HRESULT hRes;

    NKT_ASSERT(lplpDisp != NULL);
    *lplpDisp = NULL;
    {
      CNktAutoFastMutex cLock(&cMtx);

      if (nIndex >= 0 && (SIZE_T)nIndex < aList.GetCount())
      {
        if (aList[nIndex].lpObj != NULL)
        {
          hRes = aList[nIndex].lpStream->Clone(&cStream);
          if (SUCCEEDED(hRes))
          {
            li.QuadPart = 0;
            hRes = cStream->Seek(li, SEEK_SET, NULL);
          }
        }
        else
        {
          hRes = E_FAIL;
        }
      }
      else
      {
        hRes = E_INVALIDARG;
      }
    }
    if (SUCCEEDED(hRes))
      hRes = ::CoUnmarshalInterface(cStream, IID_IDispatch, (void**)lplpDisp);
    if (FAILED(hRes))
      return (hRes == E_INVALIDARG) ? E_INVALIDARG : E_FAIL;
    return S_OK;
    };

  //NOTE: May return DISP_E_MEMBERNOTFOUND
  HRESULT FireCommon(__inout IDispatch *lpConnDisp, __in DISPID nDispId,
                     __in CComVariant aVarParams[], __in int aVarParamsCount)
    {
    DISPPARAMS sParams = { aVarParams, NULL, (UINT)aVarParamsCount, 0 };
    CComVariant cVarResult;
    HRESULT hRes;
    UINT nArgErr;

    try
    {
      hRes = lpConnDisp->Invoke(nDispId, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &sParams,
                                &cVarResult, NULL, &nArgErr);
    }
    catch (...)
    {
      hRes = E_FAIL;
    }
    return hRes;
    };

private:
  typedef struct {
    IDispatch *lpObj;
    IStream *lpStream;
  } ITEM;

  CNktFastMutex cMtx;
  TNktArrayList4Structs<ITEM> aList;
};
