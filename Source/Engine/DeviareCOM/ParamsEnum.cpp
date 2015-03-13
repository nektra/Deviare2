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
#include "ParamsEnum.h"
#include "Param.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktParamsEnumImpl

HRESULT CNktParamsEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktParamsEnumImpl::GetAt(__in LONG index, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cParam)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, hRes);
  }
  //done
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::First(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::Last(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                        InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::Next(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                        InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::Previous(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                        InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::GetByDeviareId(__in my_ssize_t devId, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (devId == 0)
  {
    *ppParam = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cParam.Release();
      hRes = lpIntObj->GetAt(i, &cParam);
      if (FAILED(hRes))
        break;
      if (cParam->GetDeviareId() == (SIZE_T)devId)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = (SUCCEEDED(hRes)) ? cIParam.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamsEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
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
      cParam.Release();
      hRes = lpIntObj->GetAt(i, &cParam);
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIParam.Release();
        hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                     InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIParam);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
