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
#include "SpyMgr.h"
#include "Process.h"
#include "ProcessesEnum.h"
#include "ExportedFunction.h"
#include "Hook.h"
#include "HooksEnum.h"
#include "HookCallInfo.h"
#include "ProcessMemory.h"
#include "DbObjectsEnum.h"
#include "DbModulesEnum.h"
#include "PdbFunctionSymbol.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#define INTERFACE_FROM_SPYMGR(_sprmgr)                     \
     static_cast<CNktSpyMgrImpl*>(_sprmgr->GetComLinkNoAddRef())

//-----------------------------------------------------------

VOID CNktSpyMgrEngineImpl::OnCustomDllLoad(__in CNktDvProcess *lpProc, __in LONG nActionId,
                                           __in HRESULT hActionResult)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnCustomDllLoad(cIProc, nActionId, (LONG)hActionResult);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnCustomDllLoad(cIProc, nActionId, (LONG)hActionResult);
  return;
}

VOID CNktSpyMgrEngineImpl::OnCustomDllUnload(__in CNktDvProcess *lpProc, __in LONG nActionId,
                                             __in HRESULT hActionResult)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnCustomDllUnload(cIProc, nActionId, (LONG)hActionResult);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnCustomDllUnload(cIProc, nActionId, (LONG)hActionResult);
  return;
}

VOID CNktSpyMgrEngineImpl::OnCustomApiCall(__in CNktDvProcess *lpProc, __in LONG nActionId,
                                           __in HRESULT hActionResult, __in LONG nApiResult)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnCustomApiCall(cIProc, nActionId, (LONG)hActionResult, nApiResult);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnCustomApiCall(cIProc, nActionId, (LONG)hActionResult, nApiResult);
  return;
}

VOID CNktSpyMgrEngineImpl::OnHookStateChanged(__in CNktDvHook *lpHook, __in CNktDvProcess *lpProc,
                                              __in CNktDvHook::eState nNewState,
                                              __in CNktDvHook::eState nOldState)
{
  TNktComPtr<INktHook> cIHook;
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktHookImpl *lpHookImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, lpHook, 0);
  if (FAILED(hRes))
    return;
  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnHookStateChanged(cIHook, cIProc, (eNktHookState)nNewState,
                                        (eNktHookState)nOldState);
  lpHookImpl = static_cast<CNktHookImpl*>((INktHook*)cIHook);
  lpHookImpl->Fire_OnStateChanged(cIHook, cIProc, (eNktHookState)nNewState, (eNktHookState)nOldState);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnHookStateChanged(cIHook, cIProc, (eNktHookState)nNewState, (eNktHookState)nOldState);
  return;
}

VOID CNktSpyMgrEngineImpl::OnHookCalled(__in CNktDvHook *lpHook, __in CNktDvProcess *lpProc,
                                        __in CNktDvHookCallInfo *lpCallInfo)
{
  TNktComPtr<INktHook> cIHook;
  TNktComPtr<INktProcess> cIProc;
  TNktComPtr<INktHookCallInfo> cIHookCallInfo;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktHookImpl *lpHookImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, lpHook, 0);
  if (FAILED(hRes))
    return;
  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  hRes = GetComLinkHelper<CNktHookCallInfoImpl,INktHookCallInfo,CNktDvHookCallInfo>(&cIHookCallInfo,
                                                                                           lpCallInfo, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnFunctionCalled(cIHook, cIProc, cIHookCallInfo);
  lpHookImpl = static_cast<CNktHookImpl*>((INktHook*)cIHook);
  lpHookImpl->Fire_OnFunctionCalled(cIHook, cIProc, cIHookCallInfo);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnFunctionCalled(cIHook, cIProc, cIHookCallInfo);
  return;
}

VOID CNktSpyMgrEngineImpl::OnCreateProcessCall(__in CNktDvProcess *lpProc, __in DWORD dwPid,
                                               __in DWORD dwMainThreadId, __in BOOL bIs64BitProcess,
                                               __in BOOL bCanHookNow)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnCreateProcessCall(cIProc, (LONG)dwPid, (LONG)dwMainThreadId, bIs64BitProcess,
                                         bCanHookNow);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnCreateProcessCall(cIProc, (LONG)dwPid, (LONG)dwMainThreadId, bIs64BitProcess,
                                       bCanHookNow);
  return;
}

VOID CNktSpyMgrEngineImpl::OnLoadLibraryCall(__in CNktDvProcess *lpProc, __in LPCWSTR szLibraryNameW,
                                             __in HMODULE hModule)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  CComVariant vtModule;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
#ifdef _WIN64
  vtModule = (LONGLONG)hModule;
#else //_WIN64
  vtModule = (LONG)hModule;
#endif //_WIN64
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnLoadLibraryCall(cIProc, szLibraryNameW, vtModule);
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpSpyMgrImpl->Fire_OnLoadLibraryCall2(cIProc, szLibraryNameW, (my_ssize_t)hModule);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnLoadLibraryCall(cIProc, szLibraryNameW, vtModule);
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpProcImpl->Fire_OnLoadLibraryCall2(cIProc, szLibraryNameW, (my_ssize_t)hModule);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  return;
}

VOID CNktSpyMgrEngineImpl::OnFreeLibraryCall(__in CNktDvProcess *lpProc, __in HMODULE hModule)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  CComVariant vtModule;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
#ifdef _WIN64
  vtModule = (LONGLONG)hModule;
#else //_WIN64
  vtModule = (LONG)hModule;
#endif //_WIN64
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnFreeLibraryCall(cIProc, vtModule);
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpSpyMgrImpl->Fire_OnFreeLibraryCall2(cIProc, (my_ssize_t)hModule);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnFreeLibraryCall(cIProc, vtModule);
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpProcImpl->Fire_OnFreeLibraryCall2(cIProc, (my_ssize_t)hModule);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  return;
}

VOID CNktSpyMgrEngineImpl::OnProcessAdded(__in CNktDvProcess *lpProc)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnProcessStarted(cIProc);
  return;
}

VOID CNktSpyMgrEngineImpl::OnProcessRemoved(__in CNktDvProcess *lpProc)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnProcessTerminated(cIProc);
  return;
}

VOID CNktSpyMgrEngineImpl::OnAgentLoad(__inout CNktDvProcess *lpProc, __in HRESULT hErrorCode)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnAgentLoad(cIProc, (LONG)hErrorCode);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnAgentLoad(cIProc, (LONG)hErrorCode);
  return;
}

VOID CNktSpyMgrEngineImpl::OnAgentUnload(__inout CNktDvProcess *lpProc)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnAgentUnload(cIProc);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnAgentUnload(cIProc);
  return;
}

SIZE_T CNktSpyMgrEngineImpl::OnCustomMessage(__inout CNktDvProcess *lpProc, __in SIZE_T nMsgCode,
                                             __in SIZE_T nMsgParam)
{
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktProcessImpl *lpProcImpl;
  CComVariant vtMsgCode, vtMsgParam, vtRetVal;
  SIZE_T nRetVal;
  ULONGLONG nRetVal_Returned;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return 0;
  //----
  nRetVal = 0;
#ifdef _WIN64
  vtMsgCode = (LONGLONG)(ULONGLONG)nMsgCode;
  vtMsgParam = (LONGLONG)(ULONGLONG)nMsgParam;
  vtRetVal = (LONGLONG)(ULONGLONG)nRetVal; //IDL has this param as [in,out] so vtRetVal will be modified
#else //_WIN64
  vtMsgCode = (LONG)(ULONG)nMsgCode;
  vtMsgParam = (LONG)(ULONG)nMsgParam;
  vtRetVal = (LONG)(ULONG)nRetVal; //IDL has this param as [in,out] so vtRetVal will be modified
#endif //_WIN64
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnCustomMessage(cIProc, vtMsgCode, vtMsgParam, &vtRetVal);
  //check for a valid 'vtRetVal'
  if (Numeric_From_Variant(&vtRetVal, &nRetVal_Returned) != FALSE)
  {
#ifdef _WIN64
    nRetVal = (SIZE_T)nRetVal_Returned;
#else //_WIN64
    if (nRetVal_Returned <= 0xFFFFFFFFui64)
      nRetVal = (SIZE_T)nRetVal_Returned;
#endif //_WIN64
  }
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpSpyMgrImpl->Fire_OnCustomMessage2(cIProc, (my_ssize_t)nMsgCode, (my_ssize_t)nMsgParam,
                                      (my_ssize_t*)&nRetVal);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  //----
#ifdef _WIN64
  vtRetVal = (LONGLONG*)(ULONGLONG)nRetVal; //IDL has this param as [in,out] so vtRetVal will be modified
#else //_WIN64
  vtRetVal = (LONG*)(ULONG)nRetVal; //IDL has this param as [in,out] so vtRetVal will be modified
#endif //_WIN64
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnCustomMessage(cIProc, vtMsgCode, vtMsgParam, &vtRetVal);
  //check for a valid 'vtRetVal'
  if (Numeric_From_Variant(&vtRetVal, &nRetVal_Returned) != FALSE)
  {
#ifdef _WIN64
    nRetVal = (SIZE_T)nRetVal_Returned;
#else //_WIN64
    if (nRetVal_Returned <= 0xFFFFFFFFui64)
      nRetVal = (SIZE_T)nRetVal_Returned;
#endif //_WIN64
  }
#ifdef IDL_CSHARP_EVENTS_SUPPORTED
  lpProcImpl->Fire_OnCustomMessage2(cIProc, (my_ssize_t)nMsgCode, (my_ssize_t)nMsgParam,
                                    (my_ssize_t*)&nRetVal);
#endif //IDL_CSHARP_EVENTS_SUPPORTED
  return nRetVal;
}

VOID CNktSpyMgrEngineImpl::OnHookOverwritten(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc)
{
  TNktComPtr<INktHook> cIHook;
  TNktComPtr<INktProcess> cIProc;
  CNktSpyMgrImpl *lpSpyMgrImpl;
  CNktHookImpl *lpHookImpl;
  CNktProcessImpl *lpProcImpl;
  HRESULT hRes;

  hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, lpHook, 0);
  if (FAILED(hRes))
    return;
  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, lpProc, 0);
  if (FAILED(hRes))
    return;
  //----
  lpSpyMgrImpl = INTERFACE_FROM_SPYMGR(this);
  lpSpyMgrImpl->Fire_OnHookOverwritten(cIHook, cIProc);
  lpHookImpl = static_cast<CNktHookImpl*>((INktHook*)cIHook);
  lpHookImpl->Fire_OnHookOverwritten(cIHook, cIProc);
  lpProcImpl = static_cast<CNktProcessImpl*>((INktProcess*)cIProc);
  lpProcImpl->Fire_OnHookOverwritten(cIHook, cIProc);
  return;
}

VOID CNktSpyMgrEngineImpl::OnThreadStarted()
{
  ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  return;
}

VOID CNktSpyMgrEngineImpl::OnThreadEnded()
{
  ::CoUninitialize();
  return;
}

//-----------------------------------------------------------
// CNktSpyMgrImpl

HRESULT WINAPI CNktSpyMgrImpl::CreateInstance(void* pv, REFIID riid, LPVOID* ppv)
{
  TNktComPtr<CNktSpyMgrEngineImpl> cTempIntObj;
  TNktComPtr<INktSpyMgr> cISpyMgr;
  HRESULT hRes;

  if (ppv == NULL)
    return E_POINTER;
  *ppv = NULL;
  if (pv != NULL)
    return CLASS_E_NOAGGREGATION;
  /*
  if (::IsEqualIID(riid, IID_INktSpyMgr) == FALSE)
    return CLASS_E_CLASSNOTAVAILABLE;
  */
  //check thread apartment
  hRes = IsMultiThreadApartment();
  if (hRes == S_FALSE)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("WARNING: Single threaded apartment detected. "
                                          "Performance may be degraded."));
    NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("         Try to switch to multi-threaded apartment "
                                          "model when possible."));
  }
  else if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("WARNING: Cannot retrieve current apartment type (hRes=%08X)",
                       hRes));
  }
  //create new object
  cTempIntObj.Attach(NKT_MEMMGR_NEW CNktSpyMgrEngineImpl);
  if (cTempIntObj == NULL)
    return E_OUTOFMEMORY;
  hRes = GetComLinkHelper<CNktSpyMgrImpl,INktSpyMgr,CNktSpyMgrEngineImpl>(&cISpyMgr, cTempIntObj, 0);
  //done
  *ppv = (SUCCEEDED(hRes)) ? cISpyMgr.Detach() : NULL;
  return hRes;
}

HRESULT CNktSpyMgrImpl::SettingOverride(__in BSTR setting, __in LONG newValue)
{
  ObjectLock lock(this);
  HRESULT hRes;

  hRes = lpIntObj->SettingOverride(setting, newValue);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::get_DatabasePath(__out BSTR *pVal)
{
  ObjectLock lock(this);
  HRESULT hRes;
  CNktComBStr cComBStr;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (FAILED(cComBStr.Set(cStrDbPathW)))
  {
    *pVal = NULL;
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY);
  }
  //done
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktSpyMgrImpl::put_DatabasePath(__in BSTR newValue)
{
  ObjectLock lock(this);
  HRESULT hRes;

  if (newValue == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized != 0)
    return SetupErrorInfoAndReturn(NKT_DVERR_AlreadyInitialized);
  //copy path
  if (cStrDbPathW.Copy(newValue) == FALSE)
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY);
  return SetupErrorInfoAndReturn(CNktDvTools::FixFolderStr(cStrDbPathW));
}

HRESULT CNktSpyMgrImpl::get_AgentPath(__out BSTR *pVal)
{
  ObjectLock lock(this);
  HRESULT hRes;
  CNktComBStr cComBStr;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (FAILED(cComBStr.Set(cStrAgentPathW)))
  {
    *pVal = NULL;
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY);
  }
  //done
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktSpyMgrImpl::put_AgentPath(__in BSTR newValue)
{
  ObjectLock lock(this);
  HRESULT hRes;

  if (newValue == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized != 0)
    return SetupErrorInfoAndReturn(NKT_DVERR_AlreadyInitialized);
  //copy path
  if (cStrAgentPathW.Copy(newValue) == FALSE)
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY);
  return SetupErrorInfoAndReturn(CNktDvTools::FixFolderStr(cStrAgentPathW));
}

HRESULT CNktSpyMgrImpl::Initialize(__out LONG *pVal)
{
  ObjectLock lock(this);
  CNktStringW cStrTempW[2];
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  hRes = (nInitialized != 0) ? NKT_DVERR_AlreadyInitialized : S_OK;
  //initialize internal spymgr engine object
  if (SUCCEEDED(hRes))
  {
    if (cStrDbPathW.GetLength() != 0)
    {
      if (cStrTempW[0].Copy(cStrDbPathW) == FALSE)
        hRes = E_OUTOFMEMORY;
    }
    else
    {
      hRes = CNktDvTools::GetModulePath(cStrTempW[0], GetDllInstanceHandle());
    }
  }
  if (SUCCEEDED(hRes))
  {
    if (cStrAgentPathW.GetLength() != 0)
    {
      if (cStrTempW[1].Copy(cStrAgentPathW) == FALSE)
        hRes = E_OUTOFMEMORY;
    }
    else
    {
      hRes = CNktDvTools::GetModulePath(cStrTempW[1], GetDllInstanceHandle());
    }
  }
  //initialize engine
  if (SUCCEEDED(hRes))
    hRes = lpIntObj->Initialize(cStrTempW[0], cStrTempW[1], 0);
  //done
  *pVal = hRes;
  if (SUCCEEDED(hRes))
    NktInterlockedExchange(&nInitialized, 1);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::Processes(__deref_out INktProcessesEnum **ppProcsEnum)
{
  TNktComPtr<CNktDvProcessesEnumerator> cProcsEnum;
  TNktComPtr<INktProcessesEnum> cIProcsEnum;
  HRESULT hRes;

  if (ppProcsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppProcsEnum = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppProcsEnum = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->GetProcessesEnumerator(&cProcsEnum);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessesEnumImpl,INktProcessesEnum,
                            CNktDvProcessesEnumerator>(&cIProcsEnum, cProcsEnum, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProcsEnum = cIProcsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::Hooks(__deref_out INktHooksEnum **ppHooksEnum)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<INktHooksEnum> cIHooksEnum;
  HRESULT hRes;

  if (ppHooksEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppHooksEnum = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppHooksEnum = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->GetHooksEnumerator(&cHooksEnum);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHooksEnumImpl,INktHooksEnum,
                            CNktDvHooksEnumerator>(&cIHooksEnum, cHooksEnum, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHooksEnum = cIHooksEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::CreateHooksCollection(__deref_out INktHooksEnum **ppHooksColl)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksColl;
  TNktComPtr<INktHooksEnum> cIHooksColl;
  HRESULT hRes;

  if (ppHooksColl == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppHooksColl = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppHooksColl = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->CreateHooksCollection(&cHooksColl);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHooksEnumImpl,INktHooksEnum,
                            CNktDvHooksEnumerator>(&cIHooksColl, cHooksColl, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppHooksColl = cIHooksColl.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::CreateHook(__in VARIANT func, __in LONG hookFlags, __deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<CNktDvPdbFunctionSymbol> cFuncSymbol;
  VARIANT *lpVar;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppHook = NULL;
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  lpVar = RealVariant_From_Variant(&func);
  if (lpVar == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  switch (lpVar->vt)
  {
    case VT_BSTR:
      if (lpVar->bstrVal == NULL)
        return SetupErrorInfoAndReturn(E_POINTER);
      //create hook
      hRes = lpIntObj->CreateHook(&cHook, lpVar->bstrVal, hookFlags);
      break;

    case VT_DISPATCH:
      if (lpVar->pdispVal == NULL)
        return SetupErrorInfoAndReturn(E_POINTER);
      //get inner object
      hRes = Impl_From_Interface<CNktDvExportedFunction, IDispatch,
                                 INktExportedFunction>(&cExpFunc, lpVar->pdispVal);
      if (SUCCEEDED(hRes))
      {
        //create hook
        hRes = lpIntObj->CreateHook(&cHook, cExpFunc, hookFlags);
      }
      else
      {
        //get inner object
        hRes = Impl_From_Interface<CNktDvPdbFunctionSymbol, IDispatch,
                                   INktPdbFunctionSymbol>(&cFuncSymbol, lpVar->pdispVal);
        if (SUCCEEDED(hRes))
        {
          //create hook
          hRes = lpIntObj->CreateHook(&cHook, cFuncSymbol, hookFlags);
        }
      }
      break;

    case VT_UNKNOWN:
      if (lpVar->punkVal == NULL)
        return SetupErrorInfoAndReturn(E_POINTER);
      //get inner object
      hRes = Impl_From_Interface<CNktDvExportedFunction, IUnknown,
                                 INktExportedFunction>(&cExpFunc, lpVar->punkVal);
      if (SUCCEEDED(hRes))
      {
        //create hook
        hRes = lpIntObj->CreateHook(&cHook, cExpFunc, hookFlags);
      }
      else
      {
        //get inner object
        hRes = Impl_From_Interface<CNktDvPdbFunctionSymbol, IUnknown,
                                   INktPdbFunctionSymbol>(&cFuncSymbol, lpVar->punkVal);
        if (SUCCEEDED(hRes))
        {
          //create hook
          hRes = lpIntObj->CreateHook(&cHook, cFuncSymbol, hookFlags);
        }
      }
      break;

    default:
      hRes = E_INVALIDARG;
      break;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
    if (FAILED(hRes))
    {
      lpIntObj->DestroyHook(cHook);
    }
  }
  //done (don't create dummy interface on error)
  *ppHook = cIHook.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::CreateHookForAddress(__in my_ssize_t addr, __in BSTR functionName,
                                             __in LONG hookFlags, __deref_out INktHook **ppHook)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<INktHook> cIHook;
  HRESULT hRes;

  if (ppHook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppHook = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppHook = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  //create hook
  hRes = lpIntObj->CreateHook(&cHook, (LPVOID)addr, functionName, hookFlags);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktHookImpl,INktHook,CNktDvHook>(&cIHook, cHook, InterfaceItemGetCookies());
    if (FAILED(hRes))
    {
      lpIntObj->DestroyHook(cHook);
    }
  }
  //done (don't create dummy interface on error)
  *ppHook = cIHook.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::DestroyHook(__in INktHook *hook)
{
  TNktComPtr<CNktDvHook> cHook;
  HRESULT hRes;
  my_ssize_t nDevId;

  if (hook == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //get internal object
  hRes = hook->get_DeviareId(&nDevId);
  if (SUCCEEDED(hRes))
    hRes = CNktDvEngBaseObj::GetObjectFromDeviareId((CNktDvEngBaseObj**)&cHook, (SIZE_T)nDevId);
  //destroy
  if (SUCCEEDED(hRes))
    lpIntObj->DestroyHook(cHook);
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::LoadAgent(__in VARIANT procOrId)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //load agent
    if (dwPid != 0)
      hRes = lpIntObj->LoadAgent(dwPid);
    else
      hRes = lpIntObj->LoadAgent(cProc);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::UnloadAgent(__in VARIANT procOrId)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //load agent
    if (dwPid != 0)
      hRes = lpIntObj->UnloadAgent(dwPid);
    else
      hRes = lpIntObj->UnloadAgent(cProc);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::LoadCustomDll(__in VARIANT procOrId, __in BSTR dllFileName,
                                      __in VARIANT_BOOL unloadOnAgentShutdown, __in VARIANT_BOOL sync,
                                      __out LONG *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  LONG nActionId;
  HRESULT hRes;

  if (pVal != NULL)
    *pVal = 0;
  if (pVal == NULL || dllFileName == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //load custom dll
    if (dwPid != 0)
      hRes = lpIntObj->LoadCustomDll(&nActionId, dwPid, dllFileName,
                                    Bool_From_VariantBool(unloadOnAgentShutdown),
                                    Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->LoadCustomDll(&nActionId, cProc, dllFileName,
                                    Bool_From_VariantBool(unloadOnAgentShutdown),
                                    Bool_From_VariantBool(sync));
  }
  *pVal = (SUCCEEDED(hRes)) ? nActionId : 0;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::UnloadCustomDll(__in VARIANT procOrId, __in BSTR dllFileName,
                                        __in VARIANT_BOOL sync, __out LONG *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  LONG nActionId;
  HRESULT hRes;

  if (pVal != NULL)
    *pVal = 0;
  if (pVal == NULL || dllFileName == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //unload custom dll
    if (dwPid != 0)
      hRes = lpIntObj->UnloadCustomDll(&nActionId, dwPid, dllFileName, Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->UnloadCustomDll(&nActionId, cProc, dllFileName, Bool_From_VariantBool(sync));
  }
  *pVal = (SUCCEEDED(hRes)) ? nActionId : 0;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::CallCustomApi(__in VARIANT procOrId, __in BSTR dllFileName, __in BSTR apiToCall,
                                      __in VARIANT *params, __in VARIANT_BOOL sync, __out LONG *pVal)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  SIZE_T aParamValues[4];
  LONG i, nActionId, nParamsCount;
  LPVOID lpData;
  HRESULT hRes;

  if (pVal != NULL)
    *pVal = 0;
  //check params
  if (pVal == NULL || dllFileName == NULL || apiToCall == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  params = RealVariant_From_Variant(params);
  // If the variant is VT_EMPTY we received a null reference.
  if (params != NULL && params->vt != VT_EMPTY)
  {
    //get values for custom call
    switch (params->vt)
    {
      case VT_I1:
        aParamValues[0] = (SIZE_T)(SSIZE_T)(params->cVal);
        nParamsCount = 1;
        break;
      case VT_UI1:
        aParamValues[0] = (SIZE_T)(params->bVal);
        nParamsCount = 1;
        break;
      case VT_I2:
        aParamValues[0] = (SIZE_T)(SSIZE_T)(params->iVal);
        nParamsCount = 1;
        break;
      case VT_UI2:
        aParamValues[0] = (SIZE_T)(params->uiVal);
        nParamsCount = 1;
        break;
      case VT_I4:
        aParamValues[0] = (SIZE_T)(SSIZE_T)(params->lVal);
        nParamsCount = 1;
        break;
      case VT_UI4:
        aParamValues[0] = (SIZE_T)(params->ulVal);
        nParamsCount = 1;
        break;
      case VT_INT:
        aParamValues[0] = (SIZE_T)(SSIZE_T)(params->intVal);
        nParamsCount = 1;
        break;
      case VT_UINT:
        aParamValues[0] = (SIZE_T)(params->uintVal);
        nParamsCount = 1;
        break;
      case VT_I8:
        aParamValues[0] = (SIZE_T)(SSIZE_T)(params->llVal);
        nParamsCount = 1;
        break;
      case VT_UI8:
        aParamValues[0] = (SIZE_T)(params->ullVal);
        nParamsCount = 1;
        break;

      case VT_ARRAY|VT_I1:
      case VT_ARRAY|VT_UI1:
      case VT_ARRAY|VT_I2:
      case VT_ARRAY|VT_UI2:
      case VT_ARRAY|VT_I4:
      case VT_ARRAY|VT_UI4:
      case VT_ARRAY|VT_INT:
      case VT_ARRAY|VT_UINT:
      case VT_ARRAY|VT_I8:
      case VT_ARRAY|VT_UI8:
        if (params->parray == NULL)
          return SetupErrorInfoAndReturn(E_POINTER);
        if (params->parray->cDims != 1 ||
            params->parray->rgsabound[0].cElements > 4)
          return SetupErrorInfoAndReturn(E_INVALIDARG);
        nParamsCount = (LONG)(params->parray->rgsabound[0].cElements);
        //get array data
        hRes = ::SafeArrayAccessData(params->parray, (LPVOID*)&lpData);
        if (FAILED(hRes))
          return SetupErrorInfoAndReturn(hRes);
        switch (params->vt)
        {
          case VT_ARRAY|VT_I1:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(SSIZE_T)(((char*)lpData)[i]);
            break;
          case VT_ARRAY|VT_UI1:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(((unsigned char*)lpData)[i]);
            break;
          case VT_ARRAY|VT_I2:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(SSIZE_T)(((short*)lpData)[i]);
            break;
          case VT_ARRAY|VT_UI2:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(((unsigned short*)lpData)[i]);
            break;
          case VT_ARRAY|VT_I4:
          case VT_ARRAY|VT_INT:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(SSIZE_T)(((long*)lpData)[i]);
            break;
          case VT_ARRAY|VT_UI4:
          case VT_ARRAY|VT_UINT:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(((unsigned long*)lpData)[i]);
            break;
          case VT_ARRAY|VT_I8:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(SSIZE_T)(((__int64*)lpData)[i]);
            break;
          case VT_ARRAY|VT_UI8:
            for (i=0; i<nParamsCount; i++)
              aParamValues[i] = (SIZE_T)(((unsigned __int64*)lpData)[i]);
            break;
        }
        ::SafeArrayUnaccessData(params->parray);
        break;

      default:
        return SetupErrorInfoAndReturn(E_INVALIDARG);
    }
  }
  else
  {
    nParamsCount = 0;
  }
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //call custom dll
    if (dwPid != 0)
      hRes = lpIntObj->CallCustomApi(&nActionId, dwPid, dllFileName, apiToCall, nParamsCount,
                                        aParamValues, Bool_From_VariantBool(sync));
    else
      hRes = lpIntObj->CallCustomApi(&nActionId, cProc, dllFileName, apiToCall, nParamsCount,
                                        aParamValues, Bool_From_VariantBool(sync));
  }
  *pVal = (SUCCEEDED(hRes)) ? nActionId : 0;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::ProcessFromPID(__in LONG pid, __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  hRes = CNktDvProcess::CreateFromPID(&cProc, (DWORD)pid);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc,
                                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProc = cIProc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::ProcessMemoryFromPID(__in LONG pid, __deref_out INktProcessMemory **ppProcMem)
{
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<INktProcessMemory> cIProcMem;
  HRESULT hRes;

  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  cProcMem.Attach(CNktDvProcessMemory::CreateForPID((DWORD)pid));
  hRes = (cProcMem != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktSpyMgrImpl::CreateProcess(__in BSTR imagePath, __in VARIANT_BOOL suspended, __out VARIANT *continueEvent,
                                      __deref_out INktProcess **ppProc)
{
  return CreateProcessWithLogon(imagePath, NULL, NULL, suspended, continueEvent, ppProc);
}

HRESULT CNktSpyMgrImpl::CreateProcessWithLogon(__in BSTR imagePath, __in BSTR userName, __in BSTR password,
                                               __in VARIANT_BOOL suspended, __out VARIANT *continueEvent,
                                               __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HANDLE hContinueEvent;
  HRESULT hRes;

  if (continueEvent != NULL)
  {
    ::VariantInit(continueEvent);
    continueEvent->vt = VT_EMPTY;
  }
  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppProc = NULL;
  if (imagePath == NULL ||
      (suspended != VARIANT_FALSE && continueEvent == NULL))
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //create new process
  if (userName == NULL)
  {
    hRes = CNktDvProcess::Create(&cProc, imagePath, Bool_From_VariantBool(suspended), &hContinueEvent);
  }
  else
  {
    hRes = CNktDvProcess::CreateWithLogon(&cProc, imagePath, userName, password, Bool_From_VariantBool(suspended),
                                          &hContinueEvent);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
  }
  //if creation failed...
  if (FAILED(hRes) && cProc != NULL)
  {
    cProc->Terminate(0); //terminate recently create process as soon as possible
  }
  //done
  if (SUCCEEDED(hRes) && continueEvent != NULL && hContinueEvent != NULL)
  {
#ifdef _WIN64
    continueEvent->vt = VT_I8;
    continueEvent->llVal = (LONGLONG)(ULONGLONG)hContinueEvent;
#else _WIN64
    continueEvent->vt = VT_I4;
    continueEvent->lVal = (LONG)(ULONG)hContinueEvent;
#endif //_WIN64
  }
  *ppProc = cIProc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::CreateProcessWithToken(__in BSTR imagePath, __in my_ssize_t token, __in VARIANT_BOOL suspended,
                                               __out VARIANT *continueEvent, __deref_out INktProcess **ppProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktProcess> cIProc;
  HANDLE hContinueEvent;
  HRESULT hRes;

  if (continueEvent != NULL)
  {
    ::VariantInit(continueEvent);
    continueEvent->vt = VT_EMPTY;
  }
  if (ppProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppProc = NULL;
  if (imagePath == NULL ||
      (suspended != VARIANT_FALSE && continueEvent == NULL))
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //create new process
  hRes = CNktDvProcess::CreateWithToken(&cProc, imagePath, (HANDLE)token, Bool_From_VariantBool(suspended),
                                        &hContinueEvent);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
  }
  //if creation failed...
  if (FAILED(hRes) && cProc != NULL)
  {
    cProc->Terminate(0); //terminate recently create process as soon as possible
  }
  //done
  if (SUCCEEDED(hRes) && continueEvent != NULL && hContinueEvent != NULL)
  {
#ifdef _WIN64
    continueEvent->vt = VT_I8;
    continueEvent->llVal = (LONGLONG)(ULONGLONG)hContinueEvent;
#else _WIN64
    continueEvent->vt = VT_I4;
    continueEvent->lVal = (LONG)(ULONG)hContinueEvent;
#endif //_WIN64
  }
  *ppProc = cIProc.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::ResumeProcess(__in INktProcess *pProc, __in VARIANT continueEvent)
{
  HRESULT hRes;
  ULONGLONG nValue;

  if (Numeric_From_Variant(RealVariant_From_Variant(&continueEvent), &nValue) == FALSE || nValue == 0)
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("Resuming process."));
  ::SetEvent((HANDLE)nValue);
  ::CloseHandle((HANDLE)nValue);
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktSpyMgrImpl::TerminateProcess(__in VARIANT procOrId, __in LONG exitCode)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //call terminate process
    if (dwPid != 0)
      hRes = CNktDvProcess::Terminate(dwPid, (DWORD)exitCode);
    else
      cProc->Terminate((DWORD)exitCode);
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::FindProcessId(__in BSTR name, __out LONG *pVal)
{
  DWORD dwPid;
  HRESULT hRes;

  if (pVal != NULL)
    *pVal = 0;
  if (pVal == NULL || name == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  hRes = CNktDvProcess::FindProcessId(name, &dwPid);
  if (SUCCEEDED(hRes))
    *pVal = (LONG)dwPid;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::RealFilePath(__in BSTR name, __out BSTR *pVal)
{
  CNktComBStr cComBStr;
  CNktStringW cStrTempW;
  HRESULT hRes;

  if (pVal == NULL || name == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes) && nInitialized == 0)
    hRes = E_ACCESSDENIED;
  if (SUCCEEDED(hRes))
  {
    hRes = (cStrTempW.Copy(name) != FALSE) ? (CNktDvTools::GetRealFilePath(cStrTempW)) : E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes) && IsFatalHResult(hRes) == FALSE)
    hRes = cComBStr.Set((LPWSTR)cStrTempW);
  if (FAILED(hRes))
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::DbObjects(__in LONG platformBits, __deref_out INktDbObjectsEnum **ppObjsEnum)
{
  TNktComPtr<CNktDvDbObjectsEnumerator> cDbObjsEnum;
  TNktComPtr<INktDbObjectsEnum> cIDbObjsEnum;
  HRESULT hRes;

  if (ppObjsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppObjsEnum = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppObjsEnum = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->GetDbObjectsEnumerator(&cDbObjsEnum, (SIZE_T)(SSIZE_T)platformBits);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectsEnumImpl,INktDbObjectsEnum,
                            CNktDvDbObjectsEnumerator>(&cIDbObjsEnum, cDbObjsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppObjsEnum = cIDbObjsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::DbFunctions(__in LONG platformBits, __deref_out INktDbObjectsEnum **ppFuncsEnum)
{
  TNktComPtr<CNktDvDbObjectsEnumerator> cDbObjsEnum;
  TNktComPtr<INktDbObjectsEnum> cIDbObjsEnum;
  HRESULT hRes;

  if (ppFuncsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppFuncsEnum = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppFuncsEnum = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->GetDbFunctionsEnumerator(&cDbObjsEnum, (SIZE_T)(SSIZE_T)platformBits);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectsEnumImpl,INktDbObjectsEnum,
                            CNktDvDbObjectsEnumerator>(&cIDbObjsEnum, cDbObjsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppFuncsEnum = cIDbObjsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::DbModules(__in LONG platformBits, __deref_out INktDbModulesEnum **ppModsEnum)
{
  TNktComPtr<CNktDvDbModulesEnumerator> cDbModsEnum;
  TNktComPtr<INktDbModulesEnum> cIDbModsEnum;
  HRESULT hRes;

  if (ppModsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *ppModsEnum = NULL;
    return SetupErrorInfoAndReturn(E_FAIL);
  }
  if (nInitialized == 0)
  {
    *ppModsEnum = NULL;
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  }
  hRes = lpIntObj->GetDbModulesEnumerator(&cDbModsEnum, (SIZE_T)(SSIZE_T)platformBits);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModulesEnumImpl,INktDbModulesEnum,
                            CNktDvDbModulesEnumerator>(&cIDbModsEnum, cDbModsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppModsEnum = cIDbModsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::ProcessHandle(__in VARIANT procOrId, __in LONG desiredAccess, __out my_ssize_t *pVal)
{
  TNktComPtr<CNktDvDbModulesEnumerator> cDbModsEnum;
  TNktComPtr<INktDbModulesEnum> cIDbModsEnum;
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HANDLE hProc;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *pVal = 0;
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get window proc
    if (dwPid == 0)
      dwPid = cProc->GetProcessId();
    hRes = lpIntObj->GetProcessHandle(&hProc, dwPid, (DWORD)desiredAccess);
  }
  if (SUCCEEDED(hRes))
  {
    *pVal = (my_ssize_t)hProc;
  }
  //done
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktSpyMgrImpl::ProcessMemory(__in VARIANT procOrId, __deref_out INktProcessMemory **ppProcMem)
{
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<INktProcessMemory> cIProcMem;
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;

  if (ppProcMem == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppProcMem =  NULL;
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get process memory
    if (dwPid != 0)
      hRes = lpIntObj->GetProcessMemory(&cProcMem, dwPid);
    else
      hRes = lpIntObj->GetProcessMemory(&cProcMem, cProc);
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

HRESULT CNktSpyMgrImpl::GetWindowProc(__in VARIANT procOrId, __in my_ssize_t hWnd, __out my_ssize_t* ppWndProc)
{
  TNktComPtr<CNktDvProcess> cProc;
  DWORD dwPid;
  HRESULT hRes;
  LPVOID lpWndProc;

  if (ppWndProc == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  *ppWndProc =  NULL;
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(E_FAIL);
  if (nInitialized == 0)
    return SetupErrorInfoAndReturn(E_ACCESSDENIED);
  //check procOrId param
  hRes = ProcId_From_Variant(&cProc, &dwPid, RealVariant_From_Variant(&procOrId));
  if (SUCCEEDED(hRes))
  {
    //get window proc
    if (dwPid != 0)
      hRes = lpIntObj->HelperGetWndProc(&lpWndProc, dwPid, (HWND)hWnd);
    else
      hRes = lpIntObj->HelperGetWndProc(&lpWndProc, cProc, (HWND)hWnd);
  }
  if (SUCCEEDED(hRes))
  {
    *ppWndProc = (my_ssize_t)lpWndProc;
  }
  //done (don't create dummy interface on error)
  return SetupErrorInfoAndReturn(hRes);
}
