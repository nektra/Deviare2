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
#include "PdbFunctionSymbol.h"
#include "Param.h"

//-----------------------------------------------------------
// CNktPdbFunctionSymbolImpl

HRESULT CNktPdbFunctionSymbolImpl::get_AddrOffset(__out my_ssize_t *pVal)
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
  //get base address
  *pVal = (my_ssize_t)(lpIntObj->GetAddressOffset());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktPdbFunctionSymbolImpl::get_PlatformBits(__out LONG *pVal)
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
  //get platform bits
  *pVal = (LONG)(lpIntObj->GetPlatformBits());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktPdbFunctionSymbolImpl::get_Name(__out BSTR *pVal)
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

HRESULT CNktPdbFunctionSymbolImpl::get_ModuleName(__out BSTR *pVal)
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

HRESULT CNktPdbFunctionSymbolImpl::get_CompleteName(__out BSTR *pVal)
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
