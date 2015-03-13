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
#include "HookCallInfo.h"
#include "StackTrace.h"
#include "ParamsEnum.h"
#include "Param.h"
#include "Process.h"
#include "Hook.h"

//-----------------------------------------------------------
// CNktHookCallInfoImpl

HRESULT CNktHookCallInfoImpl::get_IsPreCall(__deref_out VARIANT_BOOL *pVal)
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
  //get "is precall"
  *pVal = VariantBool_From_Bool(lpIntObj->IsPreCall());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_Cookie(__deref_out LONG *pVal)
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
  //get "cookie"
  *pVal = lpIntObj->GetCookie();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_LastError(__deref_out LONG *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = NO_ERROR;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get last error
  *pVal = (LONG)(lpIntObj->GetLastError());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::put_LastError(__in LONG newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //set last error
    hRes = lpIntObj->SetLastError((DWORD)newValue);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::get_CurrentTimeMs(__deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get current time
  *pVal = lpIntObj->GetCurrentTimeMs();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_ElapsedTimeMs(__deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get elapsed time
  *pVal = lpIntObj->GetElapsedTimeMs();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_ChildsElapsedTimeMs(__deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get childs elapsed time
  *pVal = lpIntObj->GetChildsElapsedTimeMs();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_KernelModeTimeMs(__deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get kernel mode time
  *pVal = lpIntObj->GetKernelModeTimeMs();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_UserModeTimeMs(__deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get user mode time
  *pVal = lpIntObj->GetUserModeTimeMs();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_CpuClockCycles(__deref_out unsigned __int64 *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0ui64;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get cpu clock cycles
  *pVal = lpIntObj->GetCpuClockCycles();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::Hook(__deref_out INktHook **ppHook)
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
    //get hook
    hRes = lpIntObj->GetHook(&cHook);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHook = cIHook.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::Process(__deref_out INktProcess **ppProc)
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
    //get process
    hRes = lpIntObj->GetProcess(&cProc);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = cIProc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::get_ThreadId(__deref_out LONG *pVal)
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
  //get the thread id
  *pVal = (LONG)(lpIntObj->GetThreadId());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::get_ChainDepth(__deref_out LONG *pVal)
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
  //get the thread id
  *pVal = (LONG)(lpIntObj->GetChainDepth());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::StackTrace(__deref_out INktStackTrace **ppStackTrace)
{
  TNktComPtr<CNktDvStackTrace> cStackTrace;
  TNktComPtr<INktStackTrace> cIStackTrace;
  HRESULT hRes;

  if (ppStackTrace == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get stack trace
    hRes = lpIntObj->GetStackTrace(&cStackTrace);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktStackTraceImpl,INktStackTrace, CNktDvStackTrace>(&cIStackTrace, cStackTrace,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppStackTrace = cIStackTrace.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::Params(__deref_out INktParamsEnum **ppParamsEnum)
{
  TNktComPtr<CNktDvParamsEnumerator> cParamsEnum;
  TNktComPtr<INktParamsEnum> cIParamsEnum;
  HRESULT hRes;

  if (ppParamsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get parameters
    hRes = lpIntObj->GetFunctionParameters(&cParamsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamsEnumImpl,INktParamsEnum,
                            CNktDvParamsEnumerator>(&cIParamsEnum, cParamsEnum, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParamsEnum = cIParamsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::Result(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get result
    hRes = lpIntObj->GetFunctionResult(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                        InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::CustomParams(__deref_out INktParamsEnum **ppParamsEnum)
{
  TNktComPtr<CNktDvParamsEnumerator> cParamsEnum;
  TNktComPtr<INktParamsEnum> cIParamsEnum;
  HRESULT hRes;

  if (ppParamsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get custom parameters
    hRes = lpIntObj->GetCustomParameters(&cParamsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamsEnumImpl,INktParamsEnum,
                            CNktDvParamsEnumerator>(&cIParamsEnum, cParamsEnum, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParamsEnum = cIParamsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::get_Register(__in eNktRegister asmReg, __deref_out my_ssize_t *pVal)
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
  //get register value
  *pVal = (my_ssize_t)(lpIntObj->GetRegisterValue((ENktDvMiscEnums::eNktDvRegister)asmReg));
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::put_Register(__in eNktRegister asmReg, __in my_ssize_t newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //set register value
    hRes = lpIntObj->SetRegisterValue((ENktDvMiscEnums::eNktDvRegister)asmReg, (SIZE_T)newValue);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::get_FloatingRegister(__in eNktRegister asmReg, __deref_out double *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0.0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get floating register value
  *pVal = lpIntObj->GetFloatingRegisterValue((ENktDvMiscEnums::eNktDvRegister)asmReg);
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktHookCallInfoImpl::put_FloatingRegister(__in eNktRegister asmReg, __in double newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //set floating register value
    hRes = lpIntObj->SetFloatingRegisterValue((ENktDvMiscEnums::eNktDvRegister)asmReg, newValue);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::SkipCall(void)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //skip call
    hRes = lpIntObj->SkipCall();
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::ReadIntercallData(__in my_ssize_t destAddr, __in my_ssize_t offset,
                                                __in my_ssize_t bytes)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //read intercall data
    if (offset >= 0 && bytes >= 0)
      hRes = lpIntObj->ReadIntercallData((LPVOID)destAddr, (SIZE_T)offset, (SIZE_T)bytes);
    else
      hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktHookCallInfoImpl::WriteIntercallData(__in my_ssize_t srcAddr, __in my_ssize_t offset,
                                                 __in my_ssize_t bytes)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //write intercall data
    if (offset >= 0 && bytes >= 0)
      hRes = lpIntObj->WriteIntercallData((LPVOID)srcAddr, (SIZE_T)offset, (SIZE_T)bytes);
    else
      hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}
