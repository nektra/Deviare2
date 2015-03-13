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
#include "MiscHelpers.h"
#include <typeinfo.h>
#include "SpyMgr.h"
#include "Process.h"
#include "ProcessesEnum.h"
#include "Module.h"
#include "ModulesEnum.h"
#include "ExportedFunction.h"
#include "ExportedFunctionsEnum.h"
#include "Hook.h"
#include "HooksEnum.h"
#include "HookCallInfo.h"
#include "StackTrace.h"
#include "Param.h"
#include "ParamsEnum.h"
#include "ProcessMemory.h"
#include "DbModule.h"
#include "DbModulesEnum.h"
#include "DbObject.h"
#include "DbObjectsEnum.h"

//-----------------------------------------------------------

LONG volatile nOuterInterfaceLock = 0;

//-----------------------------------------------------------

template <class ImplementationClass2, class InterfaceClass2, class InternalObject2>
static HRESULT GetIDispatchFromBaseObj_Helper(__in CNktDvEngBaseObj *lpObj, __in DWORD dwCookie,
                                              __deref_out IDispatch **ppDisp);

//-----------------------------------------------------------

BOOL IsFatalHResult(__in HRESULT hRes)
{
  switch (hRes)
  {
    case E_POINTER:
    case E_INVALIDARG:
    case E_OUTOFMEMORY:
    //case E_FAIL: //This is commented on purpose. E_FAIL is NOT a fatal error
    case E_NOTIMPL:
      return TRUE;
  }
  return FALSE;
}

HRESULT SetupErrorInfoAndReturn(__in REFGUID rIid, __in HRESULT hErrorCodeRes, __in BOOL bForceFatal)
{
  //Checked in calls that uses RPC threads for marshalling an interface to its respective
  //apartment, the IErrorInfo interface is useful for setting the error of the last method
  //call.
  //
  //To inform an error to the client, we set up an IErrorInfo interface and put the string-ized
  //error code into the 'description' field. COM internals propagates the value store with
  //SetErrorInfo to the thread that did the method/property call also if we return S_OK
  //
  //We also export a function named 'STDAPI GetLastErrorCode()' in DerviareCOM.cpp as a helper
  //to retrieve the error code of the last function call
  if (SUCCEEDED(hErrorCodeRes))
  {
    ::SetErrorInfo(0, NULL);
  }
  else
  {
    CComPtr<ICreateErrorInfo> cCreateErrorInfo;
    CComPtr<IErrorInfo> cErrorInfo;
    CNktComBStr cTempBStr;
    WCHAR szBufW[32];
    HRESULT hRes;

    //create an error info object
    hRes = ::CreateErrorInfo(&cCreateErrorInfo);
    if (SUCCEEDED(hRes))
    {
      hRes = cCreateErrorInfo->SetGUID(rIid);
      if (SUCCEEDED(hRes))
        hRes = cTempBStr.Set(L"DeviareCOM");
      if (SUCCEEDED(hRes))
        hRes = cCreateErrorInfo->SetSource((BSTR)cTempBStr);
      if (SUCCEEDED(hRes))
      {
        swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"%ld", hErrorCodeRes);
        hRes = cTempBStr.Set(szBufW);
      }
      if (SUCCEEDED(hRes))
        hRes = cCreateErrorInfo->SetDescription(szBufW);
      if (SUCCEEDED(hRes))
      {
        hRes = cCreateErrorInfo.QueryInterface<IErrorInfo>(&cErrorInfo);
        if (SUCCEEDED(hRes) && cErrorInfo != NULL)
          hRes = ::SetErrorInfo(0, cErrorInfo);
      }
      if (FAILED(hRes))
        ::SetErrorInfo(0, NULL);
    }
  }
  return (IsFatalHResult(hErrorCodeRes) == FALSE && bForceFatal == FALSE) ? S_OK : hErrorCodeRes;
}

//-----------------------------------------------------------

HRESULT ProcId_From_Variant(__deref_out_opt CNktDvProcess **lplpProc, __out LPDWORD lpdwPid,
                            __in VARIANT *lpVar)
{
  CComVariant vTemp;

  NKT_ASSERT(lplpProc != NULL);
  NKT_ASSERT(lpdwPid != NULL);
  if (lpVar == NULL)
    return E_POINTER;
  *lpdwPid = 0;
  *lplpProc = NULL;
  switch (lpVar->vt)
  {
    case VT_I1:
    case VT_I2:
    case VT_I4:
    case VT_I8:
    case VT_UI1:
    case VT_UI2:
    case VT_UI4:
    case VT_UI8:
      if (FAILED(::VariantChangeType(&vTemp, lpVar, 0, VT_I4)))
        return E_INVALIDARG;
      *lpdwPid = (DWORD)(vTemp.lVal);
      return S_OK;

    case VT_DISPATCH:
      //get inner object
      return Impl_From_Interface<CNktDvProcess, IDispatch, INktProcess>(lplpProc, lpVar->pdispVal);

    case VT_UNKNOWN:
      //get inner object
      return Impl_From_Interface<CNktDvProcess, IUnknown, INktProcess>(lplpProc, lpVar->punkVal);
  }
  return E_INVALIDARG;
}

BOOL Numeric_From_Variant(__in VARIANT *lpVar, __out PULONGLONG lpnValue, __out_opt LPBOOL lpbIsSigned)
{
  NKT_ASSERT(lpnValue != NULL);
  if (lpVar == NULL)
    return FALSE;
  switch (lpVar->vt)
  {
    case VT_I1:
      *lpnValue = (ULONGLONG)(LONGLONG)(lpVar->cVal);
      break;
    case VT_I2:
      *lpnValue = (ULONGLONG)(LONGLONG)(lpVar->iVal);
      break;
    case VT_I4:
      *lpnValue = (ULONGLONG)(LONGLONG)(lpVar->lVal);
      break;
    case VT_I8:
      *lpnValue = (ULONGLONG)(lpVar->llVal);
      break;
    case VT_INT:
      *lpnValue = (ULONGLONG)(LONGLONG)(lpVar->intVal);
      break;
    case VT_UI1:
      *lpnValue = (ULONGLONG)(lpVar->bVal);
      break;
    case VT_UI2:
      *lpnValue = (ULONGLONG)(lpVar->uiVal);
      break;
    case VT_UI4:
      *lpnValue = (ULONGLONG)(lpVar->ulVal);
      break;
    case VT_UI8:
      *lpnValue = (lpVar->ullVal);
      break;
    case VT_UINT:
      *lpnValue = (ULONGLONG)(lpVar->uintVal);
      break;
    default:
      return FALSE;
  }
  //check if signed
  if (lpbIsSigned != NULL)
  {
    switch (lpVar->vt)
    {
      case VT_I1:
      case VT_I2:
      case VT_I4:
      case VT_I8:
      case VT_INT:
        *lpbIsSigned = TRUE;
        break;
      case VT_UI1:
      case VT_UI2:
      case VT_UI4:
      case VT_UI8:
      case VT_UINT:
        *lpbIsSigned = FALSE;
        break;
    }
  }
  return TRUE;
}

//-----------------------------------------------------------

HRESULT IsMultiThreadApartment()
{
  //code from: http://msdn.microsoft.com/en-us/library/windows/desktop/dd542641(v=vs.85).aspx
  //surrounded with a lock and a try/catch block to avoid failures
  ULONG_PTR nToken;
  IUnknown *lpUnk = NULL;
  IComThreadingInfo *lpComTI = NULL;
  APTTYPE nAptType; 
  HRESULT hRes;

  try
  {
    hRes = ::CoGetContextToken(&nToken);
    if (SUCCEEDED(hRes))
    {
      lpUnk = reinterpret_cast<IUnknown*>(nToken);
      if (lpUnk == NULL)
        hRes = E_FAIL;
    }
    if (SUCCEEDED(hRes))
      hRes = lpUnk->QueryInterface(__uuidof(IComThreadingInfo), (void**)&lpComTI);
    if (SUCCEEDED(hRes))
    {
      hRes = lpComTI->GetCurrentApartmentType(&nAptType);
      if (SUCCEEDED(hRes))
      {
        if (nAptType == APTTYPE_STA || nAptType == APTTYPE_MAINSTA)
          hRes = S_FALSE;
      }
    }
  }
  catch (...)
  {
    hRes = E_FAIL;
  }
  try
  {
    //lpUnk retrieved by CoGetContextToken should NOT be released
    if (lpComTI != NULL)
      lpComTI->Release();
  }
  catch (...)
  {
  }
  return hRes;
}

HRESULT GetIDispatchFromBaseObj(__in CNktDvEngBaseObj *lpObj, __in DWORD dwCookie,
                                __deref_out IDispatch **ppDisp)
{
  TNktComPtr<INktDvEngBaseComLink> cComLink;

  if (ppDisp != NULL)
    *ppDisp = NULL;
  if (lpObj == NULL || ppDisp == NULL)
    return E_POINTER;
  //create the com object based on the base one
#define IDISP4BASEOBJ_CHECK_TYPE(ImplementationClass2, InternalObject2)                              \
  if (typeid(*lpObj) == typeid(InternalObject2))                                                     \
    return TNktComObject<ImplementationClass2,InternalObject2>::GetComLink<IDispatch>(ppDisp,        \
                                                                        (InternalObject2*)lpObj, 0);
  //IDISP4BASEOBJ_CHECK_TYPE(CNktDbObjectsEnumImpl,CNktDvDbObjectsEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktSpyMgrImpl, CNktDvSpyMgr)
  IDISP4BASEOBJ_CHECK_TYPE(CNktProcessImpl, CNktDvProcess)
  IDISP4BASEOBJ_CHECK_TYPE(CNktProcessesEnumImpl, CNktDvProcessesEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktModuleImpl, CNktDvModule)
  IDISP4BASEOBJ_CHECK_TYPE(CNktModulesEnumImpl, CNktDvModulesEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktExportedFunctionImpl, CNktDvExportedFunction)
  IDISP4BASEOBJ_CHECK_TYPE(CNktExportedFunctionsEnumImpl, CNktDvExportedFunctionsEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktHookImpl, CNktDvHook)
  IDISP4BASEOBJ_CHECK_TYPE(CNktHooksEnumImpl, CNktDvHooksEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktParamImpl, CNktDvParam)
  IDISP4BASEOBJ_CHECK_TYPE(CNktParamsEnumImpl, CNktDvParamsEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktHookCallInfoImpl, CNktDvHookCallInfo)
  IDISP4BASEOBJ_CHECK_TYPE(CNktProcessMemoryImpl, CNktDvProcessMemory)
  IDISP4BASEOBJ_CHECK_TYPE(CNktStackTraceImpl, CNktDvStackTrace)
  //The following are special cases checked against wrappers
  IDISP4BASEOBJ_CHECK_TYPE(CNktDbObjectImpl, CNktDvDbObjectWrapper)
  IDISP4BASEOBJ_CHECK_TYPE(CNktDbObjectsEnumImpl, CNktDvDbObjectsEnumerator)
  IDISP4BASEOBJ_CHECK_TYPE(CNktDbModuleImpl, CNktDvDbModuleWrapper)
  IDISP4BASEOBJ_CHECK_TYPE(CNktDbModulesEnumImpl, CNktDvDbModulesEnumerator)
  //The following are not INktObjects so ignore them:
  //    INktStructPEFileHeader, INktStructPEOptionalHeader, INktStructPESections
  //    INktHookCallInfoPlugin, INktTools
#undef IDISP4BASEOBJ_CHECK_TYPE
  //else return not found
  return NKT_DVERR_NotFound;
}
