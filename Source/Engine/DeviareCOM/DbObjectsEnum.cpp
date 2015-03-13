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
#include "DbObjectsEnum.h"
#include "DbObject.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktDbObjectsEnumImpl

HRESULT CNktDbObjectsEnumImpl::get_Count(__deref_out LONG *pVal)
{
  SIZE_T nCount;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get count
  nCount = lpIntObj->GetCount();
  *pVal = (nCount <= LONG_MAX) ? (LONG)nCount : LONG_MAX;
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectsEnumImpl::GetAt(__in LONG index, __deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cDbObj)) : E_INVALIDARG;
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,
                            CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::First(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cDbObj);
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::Last(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cDbObj);
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::Next(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cDbObj);
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::Previous(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cDbObj);
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::GetById(__in LONG _id, __deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (_id >= 0) ? (lpIntObj->GetItemById(&cDbObj, (SIZE_T)_id)) : E_INVALIDARG;
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::GetByName(__in BSTR name, __deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByName(&cDbObj, name);
    if (SUCCEEDED(hRes))
    {
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbObj.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectsEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppUnk == NULL)
    return E_POINTER;
  *ppUnk = NULL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  hRes = CComObject<CNktEnumeratorImpl>::CreateInstance(&lpPtr);
  if (SUCCEEDED(hRes))
  {
    lpPtr->AddRef(); //CComObject::CreateInstance returns with zero refcount
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount && SUCCEEDED(hRes); i++)
    {
      cDbObj.Release();
      hRes = lpIntObj->GetAt(i, &cDbObj);
      if (SUCCEEDED(hRes))
      {
        cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
        hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
      }
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIDbObj.Release();
        hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,
                                       CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                              InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIDbObj);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
