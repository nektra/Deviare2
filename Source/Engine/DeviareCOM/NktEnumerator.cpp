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
#include "NktEnumerator.h"

//-----------------------------------------------------------
// CNktEnumeratorImpl

HRESULT CNktEnumeratorImpl::AddItem(__in IUnknown *lpUnk)
{
  TNktComPtr<IDispatch> cDisp;
  HRESULT hRes;

  if (lpUnk == NULL)
    return E_POINTER;
  hRes = lpUnk->QueryInterface(IID_IDispatch, (void**)&cDisp);
  if (FAILED(hRes))
    return hRes;
  if (aItemsList.AddElement(cDisp) == FALSE)
    return E_OUTOFMEMORY;
  cDisp.Detach();
  return S_OK;
}

HRESULT CNktEnumeratorImpl::Next(__in ULONG celt, __out VARIANT *rgVar, __out_opt ULONG *pCeltFetched)
{
  ULONG i, fetched;

  if (pCeltFetched != NULL)
    *pCeltFetched = 0;
  if (rgVar == NULL)
    return E_POINTER;
  //----
  for (i=0; i<celt; i++)
    ::VariantInit(&rgVar[i]);
  fetched = 0;
  while (nIndex < aItemsList.GetCount() && fetched < celt)
  {
    rgVar[fetched].vt = VT_DISPATCH;
    rgVar[fetched].pdispVal = aItemsList[nIndex];
    rgVar[fetched].pdispVal->AddRef();
    nIndex++;
    fetched++;
  }
  if (pCeltFetched != NULL)
    *pCeltFetched = fetched;
  return (celt == fetched) ? S_OK : S_FALSE;
}

HRESULT CNktEnumeratorImpl::Skip(__in ULONG celt)
{
  while (nIndex < aItemsList.GetCount() && celt > 0)
  {
    nIndex++;
    celt--;
  }
  return (celt == 0) ? S_OK : S_FALSE;
}

HRESULT CNktEnumeratorImpl::Reset()
{
  nIndex = 0;
  return S_OK;
}

HRESULT CNktEnumeratorImpl::Clone(__deref_out IEnumVARIANT **ppEnum)
{
  CComObject<CNktEnumeratorImpl> *lpPtr = NULL;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (ppEnum == NULL)
    return E_POINTER;
  *ppEnum = NULL;
  hRes = CComObject<CNktEnumeratorImpl>::CreateInstance(&lpPtr);
  if (SUCCEEDED(hRes))
  {
    lpPtr->AddRef(); //CComObject::CreateInstance returns with zero refcount
    nCount = aItemsList.GetCount();
    if (nCount > 0)
    {
      if (lpPtr->aItemsList.SetCount(nCount) != FALSE)
      {
        for (i=0; i<nCount; i++)
        {
          lpPtr->aItemsList[i] = aItemsList[i];
          aItemsList[i]->AddRef();
        }
      }
      else
      {
        hRes = E_OUTOFMEMORY;
      }
    }
  }
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IEnumVARIANT>(ppEnum);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  return hRes;
}
