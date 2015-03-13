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
#include "Hook.h"
#include "Process.h"

//-----------------------------------------------------------
// CNktHookImpl

HRESULT CNktHookImpl::get_Id(__out my_ssize_t *pVal)
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
  //get id
  *pVal = (my_ssize_t)(SIZE_T)(lpIntObj->GetId());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookImpl::Destroy()
{
  HRESULT hRes;

  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  lpIntObj->Destroy();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookImpl::Attach(__in VARIANT procOrId, __in VARIANT_BOOL sync)
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

HRESULT CNktHookImpl::Detach(__in VARIANT procOrId, __in VARIANT_BOOL sync)
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

HRESULT CNktHookImpl::Enable(__in VARIANT procOrId, __in VARIANT_BOOL enable, __in VARIANT_BOOL sync)
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

HRESULT CNktHookImpl::State(__in VARIANT procOrId, __out eNktHookState *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  CNktDvHook::eState nState;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = stInactive;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get process state
    if (dwPid != 0)
      nState = lpIntObj->GetProcessState(dwPid);
    else
      nState = lpIntObj->GetProcessState(cProc);
  }
  *pVal = (eNktHookState)nState; //ensure enumeration values matches
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookImpl::QueryOverwrite(__in VARIANT procOrId, __out VARIANT_BOOL *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = VARIANT_FALSE;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get process state
    if (dwPid != 0)
      hRes = lpIntObj->QueryOverwrite(dwPid);
    else
      hRes = lpIntObj->QueryOverwrite(cProc);
    if (hRes == S_OK)
      *pVal = VARIANT_TRUE;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookImpl::LastError(__in VARIANT procOrId, __out LONG *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = (LONG)E_FAIL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get process state
    if (dwPid != 0)
      hRes = lpIntObj->GetLastErrorAtProc(dwPid);
    else
      hRes = lpIntObj->GetLastErrorAtProc(cProc);
  }
  *pVal = (LONG)hRes;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookImpl::Hook(__in VARIANT_BOOL sync)
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

HRESULT CNktHookImpl::Unhook(__in VARIANT_BOOL sync)
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

HRESULT CNktHookImpl::AddCustomHandler(__in BSTR handlerDll, __in LONG handlerFlags, __in BSTR parameters)
{
  HRESULT hRes;

  if (handlerDll == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //add handler
  hRes = lpIntObj->AddCustomHandler(handlerDll, handlerFlags, parameters);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookImpl::RemoveAllHandlers()
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn();
  //remove all handlers
  lpIntObj->RemoveAllHandlers();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookImpl::get_FunctionName(__out BSTR *pVal)
{
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get name
    hRes = cComBStr.Set(lpIntObj->GetFunctionName());
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookImpl::get_Flags(__out LONG *pVal)
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
  //get flags
  *pVal = lpIntObj->GetFlags();
  return SetupErrorInfoAndReturn(S_OK);
}
