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
#include "ModulesEnum.h"
#include "Module.h"
#include "ExportedFunction.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktModulesEnumImpl

HRESULT CNktModulesEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktModulesEnumImpl::GetAt(__in LONG index, __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cMod)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::First(__deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cMod);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::Last(__deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cMod);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::Next(__deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cMod);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::Previous(__deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cMod);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::GetByDeviareId(__in my_ssize_t devId, __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (devId == 0)
  {
    *ppMod = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cMod.Release();
      hRes = lpIntObj->GetAt(i, &cMod);
      if (FAILED(hRes))
        break;
      if (cMod->GetDeviareId() == (SIZE_T)devId)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::GetByName(__in BSTR dllName, __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL || dllName == NULL)
  {
    if (ppMod != NULL)
      *ppMod = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByName(&cMod, dllName);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::GetByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                                          __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetItemByAddress(&cMod, (LPVOID)addr, (CNktDvModule::eSearchMode)searchMode);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModulesEnumImpl::FunctionByName(__in BSTR dllFunc,
                                            __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL || dllFunc == NULL)
  {
    if (ppExpFunc != NULL)
      *ppExpFunc = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->FindExportedFunctionByName(&cExpFunc, dllFunc);
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

HRESULT CNktModulesEnumImpl::FunctionByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
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
    hRes = lpIntObj->FindExportedFunctionByAddress(&cExpFunc, (LPVOID)addr,
                                                   (CNktDvModule::eSearchMode)searchMode);
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

HRESULT CNktModulesEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
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
      cMod.Release();
      hRes = lpIntObj->GetAt(i, &cMod);
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIMod.Release();
        hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                               InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIMod);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
