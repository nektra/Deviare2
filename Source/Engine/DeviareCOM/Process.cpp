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
#include "Process.h"
#include "ModulesEnum.h"
#include "Module.h"
#include "ExportedFunction.h"
#include "ProcessMemory.h"

//-----------------------------------------------------------
// CNktProcessImpl

HRESULT CNktProcessImpl::get_Id(__deref_out LONG *pVal)
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

HRESULT CNktProcessImpl::get_ParentId(__deref_out LONG *pVal)
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
  //get parent id
  *pVal = (LONG)(lpIntObj->GetParentProcessId());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktProcessImpl::get_PlatformBits(__deref_out LONG *pVal)
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

HRESULT CNktProcessImpl::get_ThreadsCount(__deref_out LONG *pVal)
{
  HRESULT hRes;
  SIZE_T nCount;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get threads count
  nCount = lpIntObj->GetThreadsCount();
  *pVal = (nCount <= LONG_MAX) ? (LONG)nCount : LONG_MAX;
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktProcessImpl::get_PriorityBaseClass(__deref_out LONG *pVal)
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
  //get parent id
  *pVal = lpIntObj->GetPriorityBaseClass();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktProcessImpl::get_Path(__deref_out BSTR *pVal)
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
    //get exe full path
    sW = lpIntObj->GetPath();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes) && IsFatalHResult(hRes) == FALSE)
    hRes = cComBStr.Set(sW);
  if (FAILED(hRes))
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::get_Name(__deref_out BSTR *pVal)
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
    //get exe name
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

HRESULT CNktProcessImpl::get_UserName(__deref_out BSTR *pVal)
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
    //get user name
    sW = lpIntObj->GetUserName();
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

HRESULT CNktProcessImpl::get_DomainName(__deref_out BSTR *pVal)
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
    //get domain name
    sW = lpIntObj->GetDomainName();
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

HRESULT CNktProcessImpl::get_LogonSID(__deref_out BSTR *pVal)
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
    //get logon sid
    sW = lpIntObj->GetLogonSid();
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

HRESULT CNktProcessImpl::get_Icon(__deref_out IPicture **ppIcon)
{
  HRESULT hRes;

  if (ppIcon == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get icon
    hRes = CNktDvTools::_ExtractIcon(ppIcon, lpIntObj->GetPath(), 0);
  }
  else
  {
    *ppIcon = NULL;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::get_DefaultIcon(__deref_out IPicture **ppIcon)
{
  CNktStringW cStrTempW;
  HRESULT hRes;

  if (ppIcon == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  hRes = CNktDvTools::GetSystem32Path(cStrTempW);
  if (SUCCEEDED(hRes))
  {
    if (cStrTempW.Concat(L"shell32.dll") == FALSE)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    hRes = CNktDvTools::_ExtractIcon(ppIcon, cStrTempW, 2);
  }
  else
  {
    *ppIcon = NULL;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::get_IsActive(__in LONG waitTimeMs, __deref_out VARIANT_BOOL *pVal)
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
  if (waitTimeMs >= 0)
  {
    //get "is active"
    hRes = lpIntObj->IsActive((DWORD)waitTimeMs);
    if (hRes == S_OK || hRes == S_FALSE)
    {
      *pVal = (hRes == S_OK) ? VARIANT_TRUE : VARIANT_FALSE;
      hRes = S_OK;
    }
    else
    {
      *pVal = VARIANT_FALSE;
    }
  }
  else
  {
    *pVal = VARIANT_FALSE;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::Terminate(__in LONG exitCode)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //terminate
  lpIntObj->Terminate((DWORD)exitCode);
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktProcessImpl::Modules(__deref_out INktModulesEnum **ppModEnum)
{
  TNktComPtr<CNktDvModulesEnumerator> cModsEnum;
  TNktComPtr<INktModulesEnum> cIModsEnum;
  HRESULT hRes;

  if (ppModEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get modules enumerator
    hRes = lpIntObj->GetModulesEnumeratorV(&cModsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModulesEnumImpl,INktModulesEnum,
                            CNktDvModulesEnumerator>(&cIModsEnum, cModsEnum, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppModEnum = cIModsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::ModuleByName(__in BSTR name, __deref_out INktModule **ppMod)
{
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<INktModule> cIMod;
  HRESULT hRes;

  if (ppMod == NULL || name == NULL)
  {
    if (ppMod != NULL)
      *ppMod = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //find module
    hRes = lpIntObj->FindModuleByName(&cMod, name);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = cIMod.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::ModuleByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                                          __deref_out INktModule **ppMod)
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
    hRes = lpIntObj->FindModuleByAddress(&cMod, (LPVOID)addr, (CNktDvModule::eSearchMode)searchMode);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktModuleImpl,INktModule,CNktDvModule>(&cIMod, cMod,
                                                                    InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppMod = cIMod.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::FunctionByName(__in BSTR name, __deref_out INktExportedFunction **ppExpFunc)
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

HRESULT CNktProcessImpl::FunctionByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
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
                                                   (CNktDvModule::eSearchMode)searchMode);
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

HRESULT CNktProcessImpl::Handle(__in LONG desiredAccess, __deref_out my_ssize_t *pVal)
{
  HANDLE hProc;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = 0;
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get process handle
    hRes = lpIntObj->GetHandle(&hProc, (DWORD)desiredAccess);
  }
  if (SUCCEEDED(hRes))
  {
    *pVal = (my_ssize_t)hProc;
  }
  //done
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::FileHeader(__deref_out INktStructPEFileHeader **ppVal)
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
    hRes = CNktDvTools::GetImageNtHeaders(&sNtHdr, lpIntObj->GetProcessId(), NULL);
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

HRESULT CNktProcessImpl::OptionalHeader(__deref_out INktStructPEOptionalHeader **ppVal)
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
    hRes = CNktDvTools::GetImageNtHeaders(&sNtHdr, lpIntObj->GetProcessId(), NULL);
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

HRESULT CNktProcessImpl::Sections(__deref_out INktStructPESections **ppVal)
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
    hRes = CNktDvTools::GetImageSections(&cSections, lpIntObj->GetProcessId(), NULL);
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

HRESULT CNktProcessImpl::get_Privilege(__in BSTR privilegeName, __deref_out VARIANT_BOOL *pVal)
{
  BOOL bEnabled;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = VARIANT_FALSE;
  if (privilegeName == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (privilegeName[0] == 0)
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get privilege
  hRes = CNktDvTools::GetProcessPrivilege(&bEnabled, lpIntObj->GetProcessId(), privilegeName);
  if (SUCCEEDED(hRes))
    *pVal = VariantBool_From_Bool(bEnabled);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::put_Privilege(__in BSTR privilegeName, __in VARIANT_BOOL newValue)
{
  HRESULT hRes;

  if (privilegeName == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (privilegeName[0] == 0)
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //set privilege
  hRes = CNktDvTools::SetProcessPrivilege(lpIntObj->GetProcessId(), privilegeName,
                                          Bool_From_VariantBool(newValue));
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::Memory(__deref_out INktProcessMemory **ppProcMem)
{
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<INktProcessMemory> cIProcMem;
  HRESULT hRes;

  if (ppProcMem == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get memory object
    hRes = lpIntObj->GetProcessMemory(&cProcMem);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessMemoryImpl,INktProcessMemory,
                            CNktDvProcessMemory>(&cIProcMem, cProcMem, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProcMem = cIProcMem.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessImpl::InvalidateCache(__in my_ssize_t hDll)
{
  HRESULT hRes;

  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //invalidate cache
    lpIntObj->MarkModulesEnumeratorAsDirtyV((HINSTANCE)hDll);
    CNktDvProcess::MarkModulesEnumeratorAsDirty(lpIntObj->GetProcessId(), (HINSTANCE)hDll);
  }
  return SetupErrorInfoAndReturn(hRes);
}
