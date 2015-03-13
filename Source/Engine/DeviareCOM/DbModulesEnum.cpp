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
#include "DbModulesEnum.h"
#include "DbModule.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktDbModulesEnumImpl

HRESULT CNktDbModulesEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktDbModulesEnumImpl::GetAt(__in LONG index, __deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cDbMod)) : E_INVALIDARG;
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::First(__deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cDbMod);
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,
                                   CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::Last(__deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cDbMod);
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::Next(__deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cDbMod);
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::Previous(__deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cDbMod);
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::GetById(__in LONG _id, __deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (_id >= 0) ? (lpIntObj->GetItemById(&cDbMod, (SIZE_T)_id)) : E_INVALIDARG;
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::GetByName(__in BSTR name, __deref_out INktDbModule **ppDbObj)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByName(&cDbMod, name);
    if (SUCCEEDED(hRes))
    {
      cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
      hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = (SUCCEEDED(hRes)) ? cIDbMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbModulesEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvDbModule> cDbMod;
  TNktComPtr<CNktDvDbModuleWrapper> cDbModWrp;
  TNktComPtr<INktDbModule> cIDbMod;
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
      cDbMod.Release();
      hRes = lpIntObj->GetAt(i, &cDbMod);
      if (SUCCEEDED(hRes))
      {
        cDbModWrp.Attach(CNktDvDbModuleWrapper::Create(cDbMod));
        hRes = (cDbModWrp != NULL) ? S_OK : E_OUTOFMEMORY;
      }
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIDbMod.Release();
        hRes = GetComLinkHelper<CNktDbModuleImpl,INktDbModule,
                                CNktDvDbModuleWrapper>(&cIDbMod, cDbModWrp, InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIDbMod);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
