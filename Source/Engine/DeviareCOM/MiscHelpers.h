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

#pragma once

#include "resource.h"
#ifdef _WIN64
  #include "DeviareCOM64_i.h"
#else //_WIN64
  #include "DeviareCOM_i.h"
#endif //_WIN64

//-----------------------------------------------------------

BOOL IsFatalHResult(__in HRESULT hRes);
HRESULT SetupErrorInfoAndReturn(__in REFGUID rIid, __in HRESULT hErrorCodeRes,
                                __in BOOL bForceFatal=FALSE);

//-----------------------------------------------------------

HRESULT ProcId_From_Variant(__deref_out_opt CNktDvProcess **lplpProc, __out LPDWORD lpdwPid,
                            __in VARIANT *lpVar);

__inline VARIANT* RealVariant_From_Variant(__in VARIANT *lpVar)
{
  while (lpVar != NULL && lpVar->vt == (VT_BYREF|VT_VARIANT))
    lpVar = lpVar->pvarVal;
  return lpVar;
}

__inline BOOL Bool_From_VariantBool(__in VARIANT_BOOL vBool)
{
  return (vBool != VARIANT_FALSE) ? TRUE : FALSE;
}

__inline BOOL VariantBool_From_Bool(__in BOOL bBool)
{
  return (bBool != FALSE) ? VARIANT_TRUE : VARIANT_FALSE;
}

BOOL Numeric_From_Variant(__in VARIANT *lpVar, __out PULONGLONG lpnValue,
                          __out_opt LPBOOL lpbIsSigned=NULL);

HRESULT IsMultiThreadApartment();

//-----------------------------------------------------------

template <class IntObj, class BaseInterface, class CheckInterface>
HRESULT Impl_From_Interface(__deref_out_opt IntObj **lplpIntObj, __in BaseInterface *lpUnk)
{
  TNktComPtr<CheckInterface> cNktObject;
  //TNktComPtr<INktObject> cNktObject;
  my_ssize_t nDevId;
  IntObj *lpIntObj;
  HRESULT hRes;

  if (lpUnk == NULL || lplpIntObj == NULL)
    return E_POINTER;
  //get interface
  hRes = lpUnk->QueryInterface(__uuidof(CheckInterface), (LPVOID*)&cNktObject);
  if (SUCCEEDED(hRes))
    hRes = cNktObject->get_DeviareId(&nDevId);
  if (SUCCEEDED(hRes))
    hRes = CNktDvEngBaseObj::GetObjectFromDeviareId((CNktDvEngBaseObj**)&lpIntObj, (SIZE_T)nDevId);
  if (FAILED(hRes))
    return (hRes != E_OUTOFMEMORY) ? E_INVALIDARG : hRes;
  *lplpIntObj = lpIntObj;
  return S_OK;
}

//-----------------------------------------------------------

template <class Impl, class Interface, class IntObj>
HRESULT GetDummyInterfaceHelper(__deref_out Interface **lplpObj, __in HRESULT hLastError)
{
  TNktComObject<Impl,IntObj> *lpPtr;
  HRESULT hRes;

  NKT_ASSERT(FAILED(hLastError));
  if (lplpObj == NULL)
    return E_POINTER;
  //if we have no "OuterInterface" stored, create a new one
  hRes = TNktComObject<Impl,IntObj>::CreateComObject(&lpPtr, NULL, 0);
  if (SUCCEEDED(hRes))
  {
    hRes = lpPtr->QueryInterface<Interface>(lplpObj);
    if (SUCCEEDED(hRes))
    {
      lpPtr->SetPermanentError(hLastError);
      lpPtr->Release(); //remove the extra reference added by QI
    }
    else
    {
      delete lpPtr; //delete if QI failed
      //lpPtr->Release(); //TODO: Confirm this
    }
  }
  if (FAILED(hRes))
    *lplpObj = NULL;
  return hRes;
}

//-----------------------------------------------------------

HRESULT GetIDispatchFromBaseObj(__in CNktDvEngBaseObj *lpObj, __in DWORD dwCookie,
                                __deref_out IDispatch **ppDisp);
