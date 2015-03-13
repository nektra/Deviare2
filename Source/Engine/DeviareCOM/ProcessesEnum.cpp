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
#include "ProcessesEnum.h"
#include "Process.h"
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktProcessesEnumImpl

HRESULT CNktProcessesEnumImpl::get_Count(__deref_out LONG *pVal)
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

HRESULT CNktProcessesEnumImpl::GetAt(__in LONG index, __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = (index >= 0) ? (lpIntObj->GetAt((SIZE_T)index, &cProc)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::First(__deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->First(&cProc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::Last(__deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Last(&cProc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::Next(__deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Next(&cProc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::Previous(__deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->Previous(&cProc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                              InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::GetByDeviareId(__in my_ssize_t devId, __deref_out INktProcess** ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (devId == 0)
  {
    *ppProc = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cProc.Release();
      hRes = lpIntObj->GetAt(i, &cProc);
      if (FAILED(hRes))
        break;
      if (cProc->GetDeviareId() == (SIZE_T)devId)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::GetById(__in LONG _id, __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (_id == 0)
  {
    *ppProc = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cProc.Release();
      hRes = lpIntObj->GetAt(i, &cProc);
      if (FAILED(hRes))
        break;
      if (cProc->GetProcessId() == (DWORD)_id)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::GetByName(__in BSTR name, __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  SIZE_T i, nCount;
  BOOL bFullPathSearch;
  HRESULT hRes;

  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (name == NULL)
  {
    *ppProc = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  if (name[0] == 0)
  {
    *ppProc = NULL;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    bFullPathSearch = (wcschr(name, L'\\') != 0) ? TRUE : FALSE;
    nCount = lpIntObj->GetCount();
    for (i=0; i<nCount; i++)
    {
      cProc.Release();
      hRes = lpIntObj->GetAt(i, &cProc);
      if (FAILED(hRes))
        break;
      if (_wcsicmp((bFullPathSearch != FALSE) ? cProc->GetPath() : cProc->GetFileName(), name) == 0)
        break;
    }
    if (i >= nCount)
      hRes = E_FAIL;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = (SUCCEEDED(hRes)) ? cIProc.Detach() : NULL;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessesEnumImpl::get__NewEnum(__deref_out IUnknown** ppUnk)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
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
      cProc.Release();
      hRes = lpIntObj->GetAt(i, &cProc);
      if (SUCCEEDED(hRes))
      {
        //get outer interface
        cIProc.Release();
        hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,
                                CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
      }
      if (SUCCEEDED(hRes))
        hRes = lpPtr->AddItem(cIProc);
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IUnknown>(ppUnk);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
