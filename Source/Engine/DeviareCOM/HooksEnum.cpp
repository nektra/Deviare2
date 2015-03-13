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
#include "HooksEnum.h"
#include "Hook.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktHooksEnumImpl

HRESULT CNktHooksEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktHooksEnumImpl::GetAt(__in LONG index, __deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cHook)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::First(__deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cHook);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Last(__deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cHook);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Next(__deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cHook);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Previous(__deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cHook);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::GetByDeviareId(__in my_ssize_t devId, __deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (devId == 0)
  {
    *ppHook = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cHook.Release();
      hRes = lpIntObj->GetAt(i, &cHook);
      if (FAILED(hRes))
        break;
      if (cHook->GetDeviareId() == (SIZE_T)devId)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::GetById(__in my_ssize_t _id, __deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (_id == 0)
  {
    *ppHook = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cHook.Release();
      hRes = lpIntObj->GetAt(i, &cHook);
      if (FAILED(hRes))
        break;
      if (cHook->GetId() == (SIZE_T)_id)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = (SUCCEEDED(hRes)) ? cIHook.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Add(__in INktHook *pHook)
{
  TNktComPtr<CNktDvHook> cTempHook;
  CNktHookImpl *lpHookImpl;
  HRESULT hRes;

  if (pHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    lpHookImpl = static_cast<CNktHookImpl*>(pHook);
    lpHookImpl->get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      hRes = lpIntObj->Add(lpHookImpl->lpIntObj);
    }
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Attach(__in VARIANT procOrId, __in VARIANT_BOOL sync)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //add process
    if (dwPid != 0)
      hRes = lpIntObj->AddProcess(dwPid, Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->AddProcess(cProc, Bool_From_VariantBool(sync));
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Detach(__in VARIANT procOrId, __in VARIANT_BOOL sync)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //remove process
    if (dwPid != 0)
      hRes = lpIntObj->RemoveProcess(dwPid, Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->RemoveProcess(cProc, Bool_From_VariantBool(sync));
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Enable(__in VARIANT procOrId, __in VARIANT_BOOL enable, __in VARIANT_BOOL sync)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //enable process
    if (dwPid != 0)
      hRes = lpIntObj->EnableProcess(dwPid, Bool_From_VariantBool(enable), Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->EnableProcess(cProc, Bool_From_VariantBool(enable), Bool_From_VariantBool(sync));
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Hook(__in VARIANT_BOOL sync)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //activate hooks
  hRes = lpIntObj->Hook(Bool_From_VariantBool(sync));
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::Unhook(__in VARIANT_BOOL sync)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //deactivate hooks
  hRes = lpIntObj->Unhook(Bool_From_VariantBool(sync));
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::QueryOverwrite(__in VARIANT procOrId, __out VARIANT *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktArrayList<BYTE> cResults;
  DWORD dwPid;
  HRESULT hRes;

  if (pVal == NULL)
    return E_POINTER;
  ::VariantInit(pVal);
  pVal->vt = VT_EMPTY;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //query overwrites
    if (dwPid != 0)
      hRes = lpIntObj->QueryOverwrite(dwPid, cResults);
    else
      hRes = lpIntObj->QueryOverwrite(cProc, cResults);
  }
  if (SUCCEEDED(hRes))
  {
    SAFEARRAY *lpArray;
    VARIANT_BOOL *lpData;
    SIZE_T i, nCount;
    LPBYTE s;

    nCount = cResults.GetCount();
    lpArray = ::SafeArrayCreateVector(VT_BOOL, 0, (ULONG)nCount);
    if (lpArray != NULL)
    {
      hRes = ::SafeArrayAccessData(lpArray, (LPVOID*)&lpData);
      if (SUCCEEDED(hRes))
      {
        s = cResults.GetBuffer();
        for (i=0; i<nCount; i++)
          lpData[i] = (s[i] != 0) ? VARIANT_TRUE : VARIANT_FALSE;
        ::SafeArrayUnaccessData(lpArray);
      }
      else
      {
        ::SafeArrayDestroy(lpArray);
      }
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    if (SUCCEEDED(hRes))
    {
      pVal->vt = VT_ARRAY|VT_BOOL;
      pVal->parray = lpArray;
    }
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHooksEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
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
      cHook.Release();
      hRes = lpIntObj->GetAt(i, &cHook);
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIHook.Release();
        hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook,
                                                                  InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIHook);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
