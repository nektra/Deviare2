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
#include "ExportedFunctionsEnum.h"
#include "ExportedFunction.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktExportedFunctionsEnumImpl

HRESULT CNktExportedFunctionsEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktExportedFunctionsEnumImpl::GetAt(__in LONG index, __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cExpFunc)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::First(__deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cExpFunc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::Last(__deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cExpFunc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::Next(__deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cExpFunc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::Previous(__deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cExpFunc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}


HRESULT CNktExportedFunctionsEnumImpl::GetByDeviareId(__in my_ssize_t devId,
                                                      __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (devId == 0)
  {
    *ppExpFunc = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cExpFunc.Release();
      hRes = lpIntObj->GetAt(i, &cExpFunc);
      if (FAILED(hRes))
        break;
      if (cExpFunc->GetDeviareId() == (SIZE_T)devId)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::GetByOrdinal(__in LONG ordinal,
                                                    __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByOrdinal(&cExpFunc, (SIZE_T)(SSIZE_T)ordinal);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::GetByName(__in BSTR funcName,
                                                 __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL || funcName == NULL)
  {
    if (ppExpFunc == NULL)
      *ppExpFunc = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByName(&cExpFunc, funcName);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::GetByAddress(__in my_ssize_t addr, __in VARIANT_BOOL searchNearest,
                                                    __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByAddress(&cExpFunc, (LPVOID)addr, Bool_From_VariantBool(searchNearest));
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = (SUCCEEDED(hRes)) ? cIExpFunc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionsEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
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
      cExpFunc.Release();
      hRes = lpIntObj->GetAt(i, &cExpFunc);
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIExpFunc.Release();
        hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                                       CNktDvExportedFunction>(&cIExpFunc, cExpFunc,
                                                               InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIExpFunc);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
