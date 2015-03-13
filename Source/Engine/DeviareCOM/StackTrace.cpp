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
#include "StackTrace.h"
#include "Module.h"
#include "ExportedFunction.h"
#include "ParamsEnum.h"
#include "Param.h"

//-----------------------------------------------------------
// CNktStackTraceImpl

HRESULT CNktStackTraceImpl::NearestFunction(__in LONG depth,
                                            __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find export
    hRes = (depth >= 0) ? (lpIntObj->GetNearestFunction(&cExpFunc, (SIZE_T)depth)) : E_INVALIDARG;
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

HRESULT CNktStackTraceImpl::Module(__in LONG depth, __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find module
    hRes = (depth >= 0) ? (lpIntObj->GetModule(&cMod, (SIZE_T)depth)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                           InterfaceItemGetCookies());
    /*
    NKT_DEBUGPRINTLNA((Nektra::dlNone, "GetComLinkHelper(MODULE) hRes=%08X [%X/%X]",
                     hRes, (SIZE_T)((CNktDvModule*)cMod), (SIZE_T)((INktModule*)cIMod)));
    */
  }
  //done
  *ppMod = (SUCCEEDED(hRes)) ? cIMod.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktStackTraceImpl::Offset(__in LONG depth, __deref_out my_ssize_t *pVal)
{
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
  //get offset
  if (depth >= 0)
  {
    *pVal = (my_ssize_t)(lpIntObj->GetOffset((SIZE_T)depth));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktStackTraceImpl::Address(__in LONG depth, __deref_out my_ssize_t *pVal)
{
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
  //get address
  if (depth >= 0)
  {
    *pVal = (my_ssize_t)(lpIntObj->GetAddress((SIZE_T)depth));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktStackTraceImpl::NearestSymbol(__in LONG depth, __deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  LPWSTR sW;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get nearest symbol
    if (depth >= 0)
    {
      sW = lpIntObj->GetNearestSymbol((SIZE_T)depth);
      if (sW == NULL)
        sW = L"";
      hRes = S_OK;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
  }
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set(sW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktStackTraceImpl::Params(__in LONG depth, __deref_out INktParamsEnum **ppParamsEnum)
{
  TNktComPtr<CNktDvParamsEnumerator> cParamsEnum;
  TNktComPtr<INktParamsEnum> cIParamsEnum;
  HRESULT hRes;

  if (ppParamsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find parameters
    hRes = (depth >= 0) ? (lpIntObj->GetParams(&cParamsEnum, NULL, (SIZE_T)depth)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamsEnumImpl,INktParamsEnum,
                                   CNktDvParamsEnumerator>(&cIParamsEnum, cParamsEnum,
                                                           InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParamsEnum = (SUCCEEDED(hRes)) ? cIParamsEnum.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktStackTraceImpl::Result(__in LONG depth, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find parameters
    hRes = (depth >= 0) ? (lpIntObj->GetParams(NULL, &cParam, (SIZE_T)depth)) : E_INVALIDARG;
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
