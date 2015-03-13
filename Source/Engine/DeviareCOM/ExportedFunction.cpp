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
#include "ExportedFunction.h"
#include "DbObject.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktExportedFunctionImpl

HRESULT CNktExportedFunctionImpl::get_Ordinal(__deref_out LONG *pVal)
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
  //get ordinal
  *pVal = (LONG)(lpIntObj->GetOrdinal());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::get_Name(__deref_out BSTR *pVal)
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
    //get name
    sW = lpIntObj->GetName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_ModuleName(__deref_out BSTR *pVal)
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
    //get module name
    sW = lpIntObj->GetModuleName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_CompleteName(__deref_out BSTR *pVal)
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
    //get complete name
    sW = lpIntObj->GetCompleteName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_IsForwarded(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is forwarded"
  *pVal = VariantBool_From_Bool(lpIntObj->IsForwarded());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::get_ForwardedApiName(__deref_out BSTR *pVal)
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
    //get forwarded api name
    sW = lpIntObj->GetForwardedApiName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_ForwardedModuleName(__deref_out BSTR *pVal)
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
    //get forwarded module name
    sW = lpIntObj->GetForwardedModuleName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_ForwardedCompleteName(__deref_out BSTR *pVal)
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
    //get forwarded complete name
    sW = lpIntObj->GetForwardedCompleteName();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktExportedFunctionImpl::get_AddrBase(__deref_out my_ssize_t *pVal)
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
  //get address base
  *pVal = (my_ssize_t)(lpIntObj->GetAddrBase());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::get_AddrOffset(__deref_out my_ssize_t *pVal)
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
  //get address offset
  *pVal = (my_ssize_t)(lpIntObj->GetAddrOffset());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::get_Addr(__deref_out my_ssize_t *pVal)
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
  *pVal = (my_ssize_t)(lpIntObj->GetAddr());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::get_CallingConv(__deref_out eNktDboCallingConvention *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = ccNA;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get calling convention
  *pVal = (eNktDboCallingConvention)(lpIntObj->GetCallingConvention());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktExportedFunctionImpl::DbObject(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  CNktDvDbObjectNoRef *lpDbObjOrig;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get dbo function entry
    lpDbObjOrig = lpIntObj->GetFunctionEntry();
    if (lpDbObjOrig != NULL)
    {
      cDbObj.Release();
      cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjOrig));
      if (cDbObj != NULL)
        cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
    else
    {
      hRes = E_FAIL;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = cIDbObj.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionImpl::get_HasDeclaration(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;
  BOOL bHasDecl;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  bHasDecl = FALSE;
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //has declaration?
    bHasDecl = lpIntObj->HasDeclaration();
    hRes = S_OK;
  }
  //done
  *pVal = VariantBool_From_Bool(bHasDecl);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktExportedFunctionImpl::get_Declaration(__deref_out BSTR *pVal)
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
    //get declaration
    sW = lpIntObj->GetDeclaration();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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
