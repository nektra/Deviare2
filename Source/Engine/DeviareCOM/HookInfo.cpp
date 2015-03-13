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
#include "HookInfo.h"
#include "DbObject.h"
#include "Process.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktHookInfoImpl

HRESULT CNktHookInfoImpl::get_Id(__deref_out my_ssize_t *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_FAIL);
  }
  //get "id"
  *pVal = (my_ssize_t)(SIZE_T)dwId;
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, S_OK);
}

HRESULT CNktHookInfoImpl::get_FunctionName(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_POINTER);
  //check internal object
  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else
  {
    //get base name
    hRes = cComBStr.Set((szFunctionNameW != NULL) ? szFunctionNameW : L"");
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::get_Address(__deref_out my_ssize_t *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_FAIL);
  }
  //get "address"
  *pVal = (my_ssize_t)nAddress;
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, S_OK);
}

HRESULT CNktHookInfoImpl::DbObject(__deref_out INktDbObject **ppDbObj)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  TNktComPtr<CNktDvDbObject> cDbObj;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_POINTER);
  //check internal object
  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else
  {
    //get dbo function entry
    if (lpDbFunc != NULL)
    {
      cDbObj.Release();
      cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbFunc));
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
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::SendCustomMessage(__in my_ssize_t msgCode, __in my_ssize_t msgParam, __in VARIANT_BOOL sync,
                                            __out my_ssize_t *pRetVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else if (pRetVal == NULL)
  {
    hRes = E_POINTER;
  }
  else if (sCallTrampoline.fnSendCustomMessage == NULL)
  {
    *pRetVal = 0;
    hRes = E_FAIL;
  }
  else
  {
    NKT_ASSERT(sCallTrampoline.lpInternal != NULL);
    hRes = sCallTrampoline.fnSendCustomMessage(sCallTrampoline.lpInternal, (SIZE_T*)pRetVal, (SIZE_T)msgCode,
                                               (SIZE_T)msgParam, Bool_From_VariantBool(sync));
  }
  if (FAILED(hRes) && pRetVal != NULL)
    *pRetVal = 0;
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::ThreadIgnore(__in LONG theadId, __in VARIANT_BOOL ignore)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else if (sCallTrampoline.fnThreadIgnore == NULL)
  {
    hRes = E_FAIL;
  }
  else
  {
    NKT_ASSERT(sCallTrampoline.lpInternal != NULL);
    hRes = sCallTrampoline.fnThreadIgnore(sCallTrampoline.lpInternal, (DWORD)theadId, Bool_From_VariantBool(ignore));
  }
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::InvalidateCache(__in my_ssize_t hDll)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else if (sCallTrampoline.fnInvalidateCache == NULL)
  {
    hRes = E_FAIL;
  }
  else
  {
    NKT_ASSERT(sCallTrampoline.lpInternal != NULL);
    hRes = sCallTrampoline.fnInvalidateCache(sCallTrampoline.lpInternal, (SIZE_T)hDll);
  }
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::CurrentProcess(__deref_out INktProcess **ppProc)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_POINTER);
  *ppProc = NULL;
  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookInfo, E_FAIL);
  //get outer interface
  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
  //done (don't create dummy interface on error)
  *ppProc = cIProc.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktHookInfo, hRes);
}

HRESULT CNktHookInfoImpl::InvalidateInterface()
{
  NktInterlockedExchange(&nValid, 0);
  return S_OK;
}

HRESULT CNktHookInfoImpl::InternalCreate(__in DV_AGENTCOM_HOOKINFO_CREATE_DATA *lpData,
                                         __deref_out IDispatch **lplpDisp)
{
  CComObject<CNktHookInfoImpl> *lpPtr = NULL;
  HRESULT hRes;

  if (lplpDisp != NULL)
    *lplpDisp = NULL;
  if (lpData == NULL || lplpDisp == NULL)
    return E_POINTER;
  //if we have no "OuterInterface" stored, create a new one
  hRes = CComObject<CNktHookInfoImpl>::CreateInstance(&lpPtr);
  //initialize object
  if (SUCCEEDED(hRes))
  {
    lpPtr->AddRef(); //CComObject::CreateInstance returns with zero refcount
    lpPtr->dwId = lpData->dwId;
    lpPtr->nAddress = lpData->nAddress;
    lpPtr->lpDbFunc = lpData->lpDbFunc;
    lpPtr->szFunctionNameW = lpData->szFunctionNameW;
    lpPtr->sCallTrampoline = lpData->sCallTrampoline;
    lpPtr->cProc = lpData->lpProc;
    lpPtr->InterfaceItemSetCookies(lpData->dwCookie);
  }
  //final initialization
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IDispatch>(lplpDisp);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  //done
  if (FAILED(hRes))
    *lplpDisp = NULL;
  return hRes;
}
