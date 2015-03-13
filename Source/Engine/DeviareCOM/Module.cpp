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
#include "Module.h"
#include "ExportedFunctionsEnum.h"
#include "ExportedFunction.h"

//-----------------------------------------------------------
// CNktModuleImpl

HRESULT CNktModuleImpl::get_ProcessId(__deref_out LONG *pVal)
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
  //get proc id
  *pVal = (LONG)(lpIntObj->GetProcessId());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktModuleImpl::get_PlatformBits(__deref_out LONG *pVal)
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

HRESULT CNktModuleImpl::get_BaseAddress(__deref_out my_ssize_t *pVal)
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
  *pVal = (my_ssize_t)(lpIntObj->GetBaseAddress());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktModuleImpl::get_Size(__deref_out my_ssize_t *pVal)
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
  //get size
  *pVal = (my_ssize_t)(lpIntObj->GetSize());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktModuleImpl::get_Name(__deref_out BSTR *pVal)
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
    //get base name
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

HRESULT CNktModuleImpl::get_Path(__deref_out BSTR *pVal)
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
    //get dll full path
    sW = lpIntObj->GetPath();
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

HRESULT CNktModuleImpl::get_FileName(__deref_out BSTR *pVal)
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
    //get dll file name
    sW = lpIntObj->GetFileName();
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

HRESULT CNktModuleImpl::get_Handle(__deref_out my_ssize_t *pVal)
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
  //get module handle
  *pVal = (my_ssize_t)(lpIntObj->GetHandle());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktModuleImpl::ExportedFunctions(__deref_out INktExportedFunctionsEnum **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncsEnum;
  TNktComPtr<INktExportedFunctionsEnum> cIExpFuncsEnum;
  HRESULT hRes;

  if (ppExpFunc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get exported functions enumerator
    hRes = lpIntObj->GetExportedFunctionsEnumerator(&cExpFuncsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionsEnumImpl,INktExportedFunctionsEnum,
                            CNktDvExportedFunctionsEnumerator>(&cIExpFuncsEnum, cExpFuncsEnum,
                                                               InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = cIExpFuncsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::FunctionByOrdinal(__in LONG ordinal,
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
    hRes = lpIntObj->FindExportedFunctionByOrdinal(&cExpFunc, (SIZE_T)(SSIZE_T)ordinal);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = cIExpFunc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::FunctionByName(__in BSTR name, __deref_out INktExportedFunction **ppExpFunc)
{
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<INktExportedFunction> cIExpFunc;
  HRESULT hRes;

  if (ppExpFunc == NULL || name == NULL)
  {
    if (ppExpFunc != NULL)
      *ppExpFunc = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find export
    hRes = lpIntObj->FindExportedFunctionByName(&cExpFunc, name);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                            CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = cIExpFunc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::FunctionByAddress(__in my_ssize_t addr, __in VARIANT_BOOL searchNearest,
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
    hRes = lpIntObj->FindExportedFunctionByAddress(&cExpFunc, (LPVOID)addr,
                                                  Bool_From_VariantBool(searchNearest));
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktExportedFunctionImpl,INktExportedFunction,
                                   CNktDvExportedFunction>(&cIExpFunc, cExpFunc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppExpFunc = cIExpFunc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::FileHeader(__deref_out INktStructPEFileHeader **ppVal)
{
  CComObject<CNktStructPEFileHeaderImpl> *lpFileHdrImpl;
  NKT_DV_IMAGE_NT_HEADER sNtHdr;
  HRESULT hRes;

  if (ppVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppVal = NULL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //create new object
  hRes = CComObject<CNktStructPEFileHeaderImpl>::CreateInstance(&lpFileHdrImpl);
  //initialize object
  if (SUCCEEDED(hRes))
  {
    lpFileHdrImpl->AddRef(); //CComObject::CreateInstance returns with zero refcount
    hRes = CNktDvTools::GetImageNtHeaders(&sNtHdr, lpIntObj->GetProcessId(),
                                          lpIntObj->GetBaseAddress());
    if (SUCCEEDED(hRes))
      hRes = lpFileHdrImpl->Initialize(&sNtHdr, (SIZE_T)hRes);
  }
  //query interface
  if (SUCCEEDED(hRes))
    hRes = lpFileHdrImpl->QueryInterface<INktStructPEFileHeader>(ppVal);
  //decrement extra ref
  if (lpFileHdrImpl != NULL)
    lpFileHdrImpl->Release();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::OptionalHeader(__deref_out INktStructPEOptionalHeader **ppVal)
{
  CComObject<CNktStructPEOptionalHeaderImpl> *lpOptHdrImpl;
  NKT_DV_IMAGE_NT_HEADER sNtHdr;
  HRESULT hRes;

  if (ppVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppVal = NULL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //create new object
  hRes = CComObject<CNktStructPEOptionalHeaderImpl>::CreateInstance(&lpOptHdrImpl);
  //initialize object
  if (SUCCEEDED(hRes))
  {
    lpOptHdrImpl->AddRef(); //CComObject::CreateInstance returns with zero refcount
    hRes = CNktDvTools::GetImageNtHeaders(&sNtHdr, lpIntObj->GetProcessId(),
                                          lpIntObj->GetBaseAddress());
    if (SUCCEEDED(hRes))
      hRes = lpOptHdrImpl->Initialize(&sNtHdr, (SIZE_T)hRes);
  }
  //query interface
  if (SUCCEEDED(hRes))
    hRes = lpOptHdrImpl->QueryInterface<INktStructPEOptionalHeader>(ppVal);
  //decrement extra ref
  if (lpOptHdrImpl != NULL)
    lpOptHdrImpl->Release();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktModuleImpl::Sections(__deref_out INktStructPESections **ppVal)
{
  CComObject<CNktStructPESectionsImpl> *lpPESections;
  TNktAutoFreePtr<CNktDvTools::PROCESSSECTION> cSections;
  HRESULT hRes;

  if (ppVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppVal = NULL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //create new object
  hRes = CComObject<CNktStructPESectionsImpl>::CreateInstance(&lpPESections);
  //initialize object
  if (SUCCEEDED(hRes))
  {
    lpPESections->AddRef(); //CComObject::CreateInstance returns with zero refcount
    hRes = CNktDvTools::GetImageSections(&cSections, lpIntObj->GetProcessId(),
                                         lpIntObj->GetBaseAddress());
    if (SUCCEEDED(hRes))
      hRes = lpPESections->Initialize(cSections.Get(), (SIZE_T)hRes);
  }
  //query interface
  if (SUCCEEDED(hRes))
    hRes = lpPESections->QueryInterface<INktStructPESections>(ppVal);
  //decrement extra ref
  if (lpPESections != NULL)
    lpPESections->Release();
  return SetupErrorInfoAndReturn(hRes);
}
