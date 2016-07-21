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

#include "Agent.h"
#include "HookEngineInternals.h"

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

CHookEntry::CHookEntry() : CNktDvObject(), TNktLnkLstNode<CHookEntry>()
{
  dwId = 0;
  lpDbFunc = NULL;
  nStackReturnSize = NKT_SIZE_T_MAX;
  nFlags = HOOKENG_FLAG_CallPreCall|HOOKENG_FLAG_CallPostCall;
  hProcDll = NULL;
  sHookInfo.dwCookie = 0;
  NktInterlockedExchange(&nCallCounter, 0);
  NktInterlockedExchange(&nUsageCount, 0);
  nktMemSet(&sHookLibInfo, 0, sizeof(sHookLibInfo));
  lpOrigProc = NULL;
  lpSuperHookStub = NULL;
  lpAfterCallMark = NULL;
  nktMemSet(aModifiedEntrypointCode, 0, sizeof(aModifiedEntrypointCode));
  nInstalledCode = 0;
  bChangedInformed = FALSE;
  return;
}

CHookEntry::~CHookEntry()
{
  if (bAppIsExiting == FALSE)
  {
    //if (hProcDll != NULL) WE ARE NOT INCREMENTING REFERENCES
    //  ::FreeLibrary(hProcDll);
    DetachAllCustomHandlers();
  }
  return;
}

HRESULT CHookEntry::AttachCustomHandler(__in LPCWSTR szDllNameW, __in SIZE_T nFlags, __in LPCWSTR szParametersW,
                                        __in CNktDvHookEngineCallback *lpHookEngCallback)
{
  TNktComPtr<CHookCustomHandlerMgr::CPluginInstance> cPluginInstance;
  TNktComPtr<IDispatch> cHookInfoLocal;
  BSTR bstrFunctionName;
  ULONG_PTR nActCtxCookie;
  HRESULT hRes;

  //check parameters
  if (szDllNameW == NULL)
    return E_POINTER;
  if (szDllNameW[0] == 0)
    return E_INVALIDARG;
  //load plugin
  cPluginInstance.Attach(NKT_MEMMGR_NEW CHookCustomHandlerMgr::CPluginInstance);
  if (cPluginInstance == NULL)
    return E_OUTOFMEMORY;
  nActCtxCookie = 0;
  cPluginInstance->nFlags = (ULONG)nFlags;
  hRes = cCustomHandlersMgr->LoadPlugin(&(cPluginInstance->cPlugin), szDllNameW);
  if (SUCCEEDED(hRes))
  {
    //create hookinfo if not done yet
    if (sHookInfo.cDispatch == NULL)
      hRes = cCustomHandlersMgr->CreateHookInfo(&(sHookInfo.cDispatch), &(sHookInfo.dwCookie), this, lpHookEngCallback);
  }
  //----
  if (SUCCEEDED(hRes))
  {
    hRes = cCustomHandlersMgr->ActivateDeviareComContext(&nActCtxCookie);
    if (SUCCEEDED(hRes))
      cHookInfoLocal = sHookInfo.cDispatch;
  }
  if (SUCCEEDED(hRes))
  {
    bstrFunctionName = NULL;
    hRes = cPluginInstance->cPlugin->Call_OnHookAdded(cPluginInstance, cHookInfoLocal,
                                                      (DWORD)(aPluginInstancesList.GetCount()+1), szParametersW);
    if (SUCCEEDED(hRes))
    {
      hRes = cPluginInstance->cPlugin->Call_GetFunctionCallbackName(cPluginInstance, cHookInfoLocal,
                                           (DWORD)(aPluginInstancesList.GetCount()+1), &bstrFunctionName);
    }
    if (SUCCEEDED(hRes) && bstrFunctionName != NULL && bstrFunctionName[0] != 0)
    {
      if (cPluginInstance->cPlugin->sDotNet.cType != NULL)
      {
#ifdef USE_MARSHALLING
        hRes = cPluginInstance->cPlugin->FindDotNetMethod(bstrFunctionName,
                                             &(cPluginInstance->cOnFunctionCallReRoute.cDotNetStream));
#else //USE_MARSHALLING
        hRes = cPluginInstance->cPlugin->FindDotNetMethod(bstrFunctionName,
                                                          &(cPluginInstance->cOnFunctionCallReRoute.cMethod));
#endif //USE_MARSHALLING
      }
      else
      {
        hRes = CNktDvTools::GetProcAddressW(&(cPluginInstance->cOnFunctionCallReRoute.lpProc),
                                            cPluginInstance->cPlugin->hDll, bstrFunctionName);
      }
    }
    if (bstrFunctionName != NULL)
      ::SysFreeString(bstrFunctionName);
  }
  if (nActCtxCookie != 0)
    cCustomHandlersMgr->DeactivateDeviareComContext(nActCtxCookie);
  if (SUCCEEDED(hRes))
  {
    //add to list
    aPluginInstancesList.PushTail(cPluginInstance.Detach());
  }
  else
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookCustomHandlerMgr[Call_OnHookAdded]: "
                      "hRes=%08X (%S) [%S @ %IXh ]", ::GetTickCount(), hRes, szDllNameW,
                      (lpDbFunc != NULL && lpDbFunc->GetName()[0] != 0) ? (lpDbFunc->GetName()) : L"???",
                      (SIZE_T)(sHookLibInfo.lpProcToHook)));
    cPluginInstance.Release();
    cCustomHandlersMgr->FreeUnusedPlugins();
  }
  return hRes;
}

HRESULT CHookEntry::ProcessCustomHandler(__deref_out CNktDvParamsEnumerator **lplpCustomParamsEnum,
                                         __inout LPVOID lpCallInfo, //CNktDvHookEngine::CALLINFO*
                                         __inout CNktDvParamsEnumerator *lpParamsEnum, __inout CNktDvParam *lpResult,
                                         __inout LPBYTE lpIntercallCustomData,
                                         __in CNktDvHookEngineCallback *lpHookEngCallback)
{
  CNktAutoFastMutex cLock(this);
  TNktComPtr<IDispatch> cHookInfoLocal;
  TNktComPtr<IStream> cStream;
  ULONG_PTR nActCtxCookie;
  HRESULT hRes;

  if (lplpCustomParamsEnum == NULL)
    return E_POINTER;
  *lplpCustomParamsEnum = NULL;
  if (cCustomHandlersMgr == NULL)
    return E_FAIL;
  if (aPluginInstancesList.IsEmpty() != FALSE)
    return S_OK;
  hRes = cCustomHandlersMgr->ActivateDeviareComContext(&nActCtxCookie);
  if (FAILED(hRes))
  {
    nActCtxCookie = 0;
    hRes = S_OK;
  }
  if (SUCCEEDED(hRes))
    cHookInfoLocal = sHookInfo.cDispatch;
  if (SUCCEEDED(hRes))
  {
    hRes = cCustomHandlersMgr->ProcessFunctionCall(this, cHookInfoLocal, lpHookEngCallback,
                                                   &aPluginInstancesList, lplpCustomParamsEnum, lpCallInfo,
                                                   lpParamsEnum, lpResult, lpIntercallCustomData);
  }
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlHookEngine, ("%lu) CHookEntry[ProcessCustomHandler]: "
                      "hRes=%08X [%S @ %IXh ]", ::GetTickCount(), hRes,
                      (lpDbFunc != NULL && lpDbFunc->GetName()[0] != 0) ? (lpDbFunc->GetName()) : L"???",
                      (SIZE_T)(sHookLibInfo.lpProcToHook)));
  }
  if (nActCtxCookie != 0)
    cCustomHandlersMgr->DeactivateDeviareComContext(nActCtxCookie);
  return hRes;
}

VOID CHookEntry::DetachAllCustomHandlers()
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CHookCustomHandlerMgr::CPluginInstance>::Iterator it;
  CHookCustomHandlerMgr::CPluginInstance *lpPluginInstance;
  DWORD dwIdx;

  if (cCustomHandlersMgr != NULL)
  {
    if (sHookInfo.cDispatch != NULL)
    {
      ULONG_PTR nActCtxCookie;

      cCustomHandlersMgr->ActivateDeviareComContext(&nActCtxCookie);
      //----
      for (lpPluginInstance=it.Begin(aPluginInstancesList),dwIdx=1; lpPluginInstance!=NULL;
        lpPluginInstance=it.Next(),dwIdx++)
      {
        lpPluginInstance->cPlugin->Call_OnHookRemoved(lpPluginInstance, sHookInfo.cDispatch, dwIdx);
      }
      //----
      if (nActCtxCookie != 0)
        cCustomHandlersMgr->DeactivateDeviareComContext(nActCtxCookie);
    }
    if (sHookInfo.dwCookie != 0)
      cCustomHandlersMgr->InvalidateHookInfo(sHookInfo.dwCookie);
    sHookInfo.cDispatch.Release();
  }
  //----
  while ((lpPluginInstance=aPluginInstancesList.PopHead()) != NULL)
    lpPluginInstance->Release();
  if (cCustomHandlersMgr != NULL)
    cCustomHandlersMgr->FreeUnusedPlugins();
  return;
}
