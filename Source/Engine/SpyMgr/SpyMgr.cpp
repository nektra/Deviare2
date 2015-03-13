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

#include "SpyMgr.h"
#include "..\..\Common\ProcessMemory.h"
#include "..\..\Common\Tools.h"
#include "..\..\Common\ProcessHandle.h"
#include "..\..\Common\FnvHash.h"
#include "..\..\Common\RegistrySettings.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#pragma warning(disable : 4355)
CNktDvSpyMgr::CNktDvSpyMgr() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE),
                               CNktDvInternalProcessesListCallback(), CNktDvTransportEngineCallback(),
                               CNktThread(), cTransport(this), cProcList(this)
{
  NktInterlockedExchange(&nNextHookId, 0);
  NktInterlockedExchange(&nNextBatchHookId, 0);
  NktInterlockedExchange(&(sHooks.nMutex), 0);
  NktInterlockedExchange(&(sHooksEnum.nMutex), 0);
  nktMemSet(&sSettingsOverride, 0, sizeof(sSettingsOverride));
  return;
}
#pragma warning(default : 4355)

CNktDvSpyMgr::~CNktDvSpyMgr()
{
  Finalize();
  return;
}

HRESULT CNktDvSpyMgr::SettingOverride(__in_z LPCWSTR szSettingW, __in DWORD dwValue)
{
  if (szSettingW == NULL)
    return E_POINTER;
  if (_wcsicmp(szSettingW, L"SpyMgrDebugLevelMask") == 0)
  {
    CNktDvRegistrySettings::SetDebugLevelMask(dwValue);
    Nektra::SetDebugLevel(dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentDebugLevelMask") == 0)
  {
    sSettingsOverride.nValidFields |= 0x01;
    sSettingsOverride.dwDebugFlags = dwValue;
  }
  else if (_wcsicmp(szSettingW, L"SpyMgrMemMgrPoolMaxAge") == 0)
  {
    CNktDvRegistrySettings::SetMemMgrPoolMaxAge(dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentMemMgrPoolMaxAge") == 0)
  {
    sSettingsOverride.nValidFields |= 0x02;
    sSettingsOverride.dwMemMgrPoolMaxAge = dwValue;
  }
  else if (_wcsicmp(szSettingW, L"SpyMgrMemMgrPoolCheckFrequency") == 0)
  {
    CNktDvRegistrySettings::SetMemMgrPoolCheckFrequency(dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentMemMgrPoolCheckFrequency") == 0)
  {
    sSettingsOverride.nValidFields |= 0x04;
    sSettingsOverride.dwMemMgrPoolCheckFrequency = dwValue;
  }
  else if (_wcsicmp(szSettingW, L"SpyMgrTransportMaxFreeBufferList") == 0)
  {
    CNktDvRegistrySettings::SetTransportMaxFreeBufferList(dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentTransportMaxFreeBufferList") == 0)
  {
    sSettingsOverride.nValidFields |= 0x08;
    sSettingsOverride.dwTransportMaxFreeBufferList = dwValue;
  }
  else if (_wcsicmp(szSettingW, L"SpyMgrTransportMaxInUseMessageCount") == 0)
  {
    CNktDvRegistrySettings::SetSpyMgrTransportMaxInUseMessageCount((LONG)dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentTransportMaxInUseMessageCount") == 0)
  {
    sSettingsOverride.nValidFields |= 0x10;
    sSettingsOverride.nAgentTransportMaxInUseMessageCount = (LONG)dwValue;
  }
  else if (_wcsicmp(szSettingW, L"SpyMgrTransportMessageDelay") == 0)
  {
    CNktDvRegistrySettings::SetSpyMgrTransportMessageDelay(dwValue);
  }
  else if (_wcsicmp(szSettingW, L"AgentTransportMessageDelay") == 0)
  {
    sSettingsOverride.nValidFields |= 0x20;
    sSettingsOverride.dwAgentTransportMessageDelay = dwValue;
  }
  else if (_wcsicmp(szSettingW, L"AgentLoadTimeout") == 0)
  {
    CNktDvRegistrySettings::SetAgentLoadTimeout(dwValue);
  }
  else
  {
    return E_INVALIDARG;
  }
  return S_OK;
}

HRESULT CNktDvSpyMgr::GetProcessesEnumerator(__deref_out CNktDvProcessesEnumerator **lplpEnum)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  return cProcList.GetEnumerator(lplpEnum);
}

HRESULT CNktDvSpyMgr::GetHooksEnumerator(__deref_out CNktDvHooksEnumerator **lplpEnum)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cEnum;
  TNktLnkLst<CNktDvHook>::Iterator it;
  CNktDvHook *lpHook;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    for (lpHook=it.Begin(sHooks.cList); lpHook!=NULL; lpHook=it.Next())
    {
      hRes = cEnum->Add(lpHook);
      if (FAILED(hRes))
        return hRes;
    }
  }
  //set parent and add to list
  cEnum->lpEngine = this;
  {
    CNktSimpleLockNonReentrant cHooksEnumLock(&(sHooksEnum.nMutex));

    sHooksEnum.cList.PushTail((CNktDvHooksEnumerator*)cEnum);
  }
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

HRESULT CNktDvSpyMgr::CreateHooksCollection(__deref_out CNktDvHooksEnumerator **lplpEnum)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cEnum;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  //set parent and add to list
  cEnum->lpEngine = this;
  {
    CNktSimpleLockNonReentrant cHooksEnumLock(&(sHooksEnum.nMutex));

    sHooksEnum.cList.PushTail((CNktDvHooksEnumerator*)cEnum);
  }
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

HRESULT CNktDvSpyMgr::CreateHook(__deref_out CNktDvHook **lplpHook, __in CNktDvExportedFunction *lpExpFunc,
                                 __in LONG nHookFlags)
{
  TNktComPtr<CNktDvExportedFunction> cExpFuncAutoRef(lpExpFunc);

  if (lplpHook == NULL)
    return E_POINTER;
  *lplpHook = NULL;
  if (lpExpFunc == NULL)
    return E_POINTER;
  return CreateHook(lplpHook, lpExpFunc->GetCompleteName(), nHookFlags);
}

HRESULT CNktDvSpyMgr::CreateHook(__deref_out CNktDvHook **lplpHook, __in_z LPCWSTR szFunctionNameW,
                                 __in LONG nHookFlags)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cNewHook;
  CNktDvTools::FUNCTION_PARSE_INFO sFuncParseInfo;
  SIZE_T k;
  HRESULT hRes;
  CNktStringW cStrTempW;

  if (lplpHook == NULL)
    return E_POINTER;
  *lplpHook = NULL;
  //check names
  hRes = CNktDvTools::ParseFunctionName(sFuncParseInfo, szFunctionNameW);
  if (FAILED(hRes))
    return hRes;
  //check flags
  if ((nHookFlags & (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall)) == 
                    (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall) ||
      (nHookFlags & (CNktDvHook::flgOnly32Bits|CNktDvHook::flgOnly64Bits)) ==
                    (CNktDvHook::flgOnly32Bits|CNktDvHook::flgOnly64Bits))
    return E_INVALIDARG;
  //more checks
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //create hook
  cNewHook.Attach(NKT_MEMMGR_NEW CNktDvHook);
  if (cNewHook == NULL)
    return E_OUTOFMEMORY;
  k = cNewHook->GetDeviareId();
  cNewHook->dwHookId = (DWORD)fnv_32a_buf(&k, sizeof(k), FNV1A_32_INIT);
  cNewHook->dwHookId = (DWORD)fnv_32a_buf(&sFuncParseInfo, sizeof(sFuncParseInfo),
                                          (Fnv32_t)(cNewHook->dwHookId));
  if (cNewHook->cStrFunctionNameW.Copy(szFunctionNameW) == FALSE)
    return E_OUTOFMEMORY;
  cNewHook->sFuncParseInfo = sFuncParseInfo;
  NktInterlockedExchange(&(cNewHook->nFlags), (LONG)nHookFlags);
  cNewHook->nAddressOrOffset = 0;
  //try to find the DbObject that belongs to this function name
  if (sFuncParseInfo.nDllNameLen > 0)
    cNewHook->lpDbModule32 = cDvDB_32->FindModuleByName(szFunctionNameW, sFuncParseInfo.nDllNameLen);
  if (sFuncParseInfo.nApiOrdinal == NKT_SIZE_T_MAX && sFuncParseInfo.nApiNameLen > 0)
    cNewHook->lpDbObject32 = cDvDB_32->FindFunctionByName(szFunctionNameW+sFuncParseInfo.nApiNameStart,
                                                          sFuncParseInfo.nApiNameLen);
#if defined _M_X64
  if (sFuncParseInfo.nDllNameLen > 0)
    cNewHook->lpDbModule64 = cDvDB_64->FindModuleByName(szFunctionNameW, sFuncParseInfo.nDllNameLen);
  if (sFuncParseInfo.nApiOrdinal == NKT_SIZE_T_MAX && sFuncParseInfo.nApiNameLen > 0)
    cNewHook->lpDbObject64 = cDvDB_64->FindFunctionByName(szFunctionNameW+sFuncParseInfo.nApiNameStart,
                                                          sFuncParseInfo.nApiNameLen);
#endif //_M_X64
  //set parent and add to list
  cNewHook->lpEngine = this;
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    sHooks.cList.PushTail((CNktDvHook*)cNewHook);
  }
  *lplpHook = cNewHook.Detach();
  (*lplpHook)->AddRef(); //internally mantain a hook reference
  return S_OK;
}

HRESULT CNktDvSpyMgr::CreateHook(__deref_out CNktDvHook **lplpHook, __in CNktDvPdbFunctionSymbol *lpSymbol,
                                 __in LONG nHookFlags)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cNewHook;
  CNktStringW cStrTempW;

  if (lplpHook == NULL)
    return E_POINTER;
  *lplpHook = NULL;
  //check symbol
  if (lpSymbol == NULL)
    return E_POINTER;
  if (lpSymbol->GetName()[0] == 0 || lpSymbol->GetModuleName()[0] == 0)
    return E_INVALIDARG;
  //check flags
  if ((nHookFlags & (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall)) == 
                    (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall) ||
      (nHookFlags & (CNktDvHook::flgOnly32Bits|CNktDvHook::flgOnly64Bits)) != 0 ||
      lpSymbol->GetAddressOffset() == 0)
    return E_INVALIDARG;
  switch (lpSymbol->GetPlatformBits())
  {
    case 32:
      nHookFlags |= CNktDvHook::flgOnly32Bits;
      break;
    case 64:
      nHookFlags |= CNktDvHook::flgOnly64Bits;
      break;
    default:
      return E_INVALIDARG;
  }
  nHookFlags |= CNktDvHook::flgAddressIsOffset;
  return CreateHook(lplpHook, (LPVOID)(lpSymbol->GetAddressOffset()), lpSymbol->GetCompleteName(),
                    nHookFlags);
}

HRESULT CNktDvSpyMgr::CreateHook(__deref_out CNktDvHook **lplpHook, __in LPVOID lpAddress,
                                 __in_z LPCWSTR szFunctionNameW, __in LONG nHookFlags)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cNewHook;
  TNktComPtr<CHookItem> cHookItem;
  CNktDvTools::FUNCTION_PARSE_INFO sFuncParseInfo;
  SIZE_T k;
  HRESULT hRes;

  if (lplpHook != NULL)
    *lplpHook = NULL;
  if (lplpHook == NULL)
    return E_POINTER;
  if (szFunctionNameW == NULL)
    szFunctionNameW = L"";
  nktMemSet(&sFuncParseInfo, 0, sizeof(sFuncParseInfo));
  if (szFunctionNameW[0] != 0)
  {
    //check names
    hRes = CNktDvTools::ParseFunctionName(sFuncParseInfo, szFunctionNameW);
    if (FAILED(hRes))
      return hRes;
    if (sFuncParseInfo.nApiNameLen == 0)
      return E_INVALIDARG;
  }
  //check flags
  if ((nHookFlags & (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall)) == 
                    (CNktDvHook::flgOnlyPreCall|CNktDvHook::flgOnlyPostCall) ||
      (nHookFlags & (CNktDvHook::flgOnly32Bits|CNktDvHook::flgOnly64Bits)) ==
                    (CNktDvHook::flgOnly32Bits|CNktDvHook::flgOnly64Bits))
    return E_INVALIDARG;
  if ((nHookFlags & CNktDvHook::flgAddressIsOffset) != 0 && sFuncParseInfo.nDllNameLen == 0)
    return E_INVALIDARG; //offset address requires a module name
  //more checks
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //create hook
  cNewHook.Attach(NKT_MEMMGR_NEW CNktDvHook);
  if (cNewHook == NULL)
    return E_OUTOFMEMORY;
  k = cNewHook->GetDeviareId();
  cNewHook->dwHookId = (DWORD)fnv_32a_buf(&k, sizeof(k), FNV1A_32_INIT);
  cNewHook->dwHookId = (DWORD)fnv_32a_buf(&sFuncParseInfo, sizeof(sFuncParseInfo),
                                          (Fnv32_t)(cNewHook->dwHookId));
  if (cNewHook->cStrFunctionNameW.Copy(szFunctionNameW) == FALSE)
    return E_OUTOFMEMORY;
  cNewHook->sFuncParseInfo = sFuncParseInfo;
  NktInterlockedExchange(&(cNewHook->nFlags), (LONG)nHookFlags);
  cNewHook->nAddressOrOffset = (SIZE_T)lpAddress;
  //try to find the DbObject that belongs to this function name
  if (sFuncParseInfo.nDllNameLen > 0)
    cNewHook->lpDbModule32 = cDvDB_32->FindModuleByName(szFunctionNameW, sFuncParseInfo.nDllNameLen);
  if (sFuncParseInfo.nApiOrdinal == NKT_SIZE_T_MAX && sFuncParseInfo.nApiNameLen > 0)
    cNewHook->lpDbObject32 = cDvDB_32->FindFunctionByName(szFunctionNameW+sFuncParseInfo.nApiNameStart,
                                                          sFuncParseInfo.nApiNameLen);
#if defined _M_X64
  if (sFuncParseInfo.nDllNameLen > 0)
    cNewHook->lpDbModule64 = cDvDB_64->FindModuleByName(szFunctionNameW, sFuncParseInfo.nDllNameLen);
  if (sFuncParseInfo.nApiOrdinal == NKT_SIZE_T_MAX && sFuncParseInfo.nApiNameLen > 0)
    cNewHook->lpDbObject64 = cDvDB_64->FindFunctionByName(szFunctionNameW+sFuncParseInfo.nApiNameStart,
                                                          sFuncParseInfo.nApiNameLen);
#endif //_M_X64
  //set parent and add to list
  cNewHook->lpEngine = this;
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    sHooks.cList.PushTail((CNktDvHook*)cNewHook);
  }
  *lplpHook = cNewHook.Detach();
  (*lplpHook)->AddRef(); //internally mantain a hook reference
  return S_OK;
}

HRESULT CNktDvSpyMgr::DestroyHook(__inout CNktDvHook *lpHook)
{
  TNktComPtr<CNktDvHook> cHook(lpHook);

  if (lpHook == NULL)
    return E_POINTER;
  cHook->Destroy();
  return S_OK;
}

HRESULT CNktDvSpyMgr::LoadAgent(__in CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  HRESULT hRes, hAgentLoadRes;

  if (lpProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  //inform about agent load
  if (hAgentLoadRes != S_FALSE)
    OnAgentLoad(lpProc, hAgentLoadRes);
  return hRes;
}

HRESULT CNktDvSpyMgr::LoadAgent(__in DWORD dwPid)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return LoadAgent(cProc);
}

HRESULT CNktDvSpyMgr::UnloadAgent(__in CNktDvProcess *lpProc)
{
  if (lpProc == NULL)
    return E_POINTER;
  return UnloadAgent(lpProc->GetProcessId());
}

HRESULT CNktDvSpyMgr::UnloadAgent(__in DWORD dwPid)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  if (dwPid == 0 || dwPid == ::GetCurrentProcessId())
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    CAgentControllerShutdownLock cAgentCtlSdLock;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, dwPid);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() == CAgentController::stShuttingDown)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  if (lpAgentCtl != NULL && SUCCEEDED(hRes))
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::LoadCustomDll(__out PLONG lpnActionId, __in CNktDvProcess *lpProc, __in_z LPCWSTR szDllFileNameW,
                                    __in BOOL bUnloadOnAgentShutdown, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  NKT_DV_TMSG_LOADDLL sMsg;
  TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_LOADDLL_CALLBACK> cMsgResp;
  SIZE_T nDllFileNameLen;
  HRESULT hRes, hAgentLoadRes;

  if (lpnActionId != NULL)
    *lpnActionId = 0;
  if (lpProc == NULL || szDllFileNameW == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId() ||
      szDllFileNameW[0] == 0)
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  //inform about agent load
  if (hAgentLoadRes != S_FALSE)
    OnAgentLoad(lpProc, hAgentLoadRes);
  if (SUCCEEDED(hRes))
  {
    //prepare data
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_LoadDll, cTransport.GetNextMsgId(), NULL,
                                     (bSync != FALSE) ? &cMsgResp : NULL);
  }
  if (SUCCEEDED(hRes))
  {
    if (lpnActionId != NULL)
      *lpnActionId = (LONG)sMsg.dwMsgId;
    sMsg.bUnloadOnAgentShutdown = bUnloadOnAgentShutdown;
    nDllFileNameLen = wcslen(szDllFileNameW);
    if (nDllFileNameLen >= NKT_DV_ARRAYLEN(sMsg.szDllNameW))
    {
      hRes = cTransport.SendBigPacketMsg(&(sMsg.dwDllNameBigPacketId), lpProc->GetProcessId(),
                                         szDllFileNameW, nDllFileNameLen*sizeof(WCHAR));
    }
    else
    {
      wcscpy_s(sMsg.szDllNameW, NKT_DV_ARRAYLEN(sMsg.szDllNameW), szDllFileNameW);
      sMsg.dwDllNameBigPacketId = 0;
    }
  }
  //send message
  if (SUCCEEDED(hRes))
    hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
  if (SUCCEEDED(hRes) && bSync != FALSE)
  {
    hRes = cMsgResp.Wait(cShutdownMtx.GetEvent(), lpAgentCtl->GetShutdownMtx().GetEvent());
    if (SUCCEEDED(hRes))
      hRes = cMsgResp->hLoadDllRes;
  }
  return hRes;
}

HRESULT CNktDvSpyMgr::LoadCustomDll(__out PLONG lpnActionId, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                    __in BOOL bUnloadOnAgentShutdown, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return LoadCustomDll(lpnActionId, cProc, szDllFileNameW, bUnloadOnAgentShutdown, bSync);
}

HRESULT CNktDvSpyMgr::UnloadCustomDll(__out PLONG lpnActionId, __in CNktDvProcess *lpProc,
                                      __in_z LPCWSTR szDllFileNameW, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  NKT_DV_TMSG_UNLOADDLL sMsg;
  TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_UNLOADDLL_CALLBACK> cMsgResp;
  SIZE_T nDllFileNameLen;
  HRESULT hRes, hAgentLoadRes;

  if (lpnActionId != NULL)
    *lpnActionId = 0;
  if (lpProc == NULL || szDllFileNameW == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId() ||
      szDllFileNameW[0] == 0)
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  //inform about agent load
  if (hAgentLoadRes != S_FALSE)
    OnAgentLoad(lpProc, hAgentLoadRes);
  if (SUCCEEDED(hRes))
  {
    //prepare data
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_UnloadDll, cTransport.GetNextMsgId(), NULL,
                                     (bSync != FALSE) ? &cMsgResp : NULL);
  }
  if (SUCCEEDED(hRes))
  {
    if (lpnActionId != NULL)
      *lpnActionId = (LONG)sMsg.dwMsgId;
    nDllFileNameLen = wcslen(szDllFileNameW);
    if (nDllFileNameLen >= NKT_DV_ARRAYLEN(sMsg.szDllNameW))
    {
      hRes = cTransport.SendBigPacketMsg(&(sMsg.dwDllNameBigPacketId), lpProc->GetProcessId(),
                                         szDllFileNameW, nDllFileNameLen*sizeof(WCHAR));
    }
    else
    {
      wcscpy_s(sMsg.szDllNameW, NKT_DV_ARRAYLEN(sMsg.szDllNameW), szDllFileNameW);
      sMsg.dwDllNameBigPacketId = 0;
    }
  }
  //send message
  if (SUCCEEDED(hRes))
    hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
  if (SUCCEEDED(hRes) && bSync != FALSE)
  {
    hRes = cMsgResp.Wait(cShutdownMtx.GetEvent(), lpAgentCtl->GetShutdownMtx().GetEvent());
    if (SUCCEEDED(hRes))
      hRes = cMsgResp->hUnloadDllRes;
  }
  return hRes;
}

HRESULT CNktDvSpyMgr::UnloadCustomDll(__out PLONG lpnActionId, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                      __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return UnloadCustomDll(lpnActionId, cProc, szDllFileNameW, bSync);
}

HRESULT CNktDvSpyMgr::CallCustomApi(__out PLONG lpnActionIdOrResult, __in CNktDvProcess *lpProc,
                                    __in_z LPCWSTR szDllFileNameW, __in_z LPCWSTR szApiToCallW,
                                    __in LONG nParamsCount, __in_opt PSIZE_T lpnParams, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  NKT_DV_TMSG_CALLCUSTOMAPI sMsg;
  TNktAutoFreePtr<CHAR> cStrApiToCallA;
  TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK> cMsgResp;
  SIZE_T i, nDllFileNameLen;
  HRESULT hRes, hAgentLoadRes;

  if (lpnActionIdOrResult != NULL)
    *lpnActionIdOrResult = 0;
  if (lpProc == NULL || szDllFileNameW == NULL || szApiToCallW == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId() ||
      szDllFileNameW[0] == 0 || szApiToCallW[0] == 0 ||
      nParamsCount > 4)
    return E_INVALIDARG;
  if (nParamsCount > 0 && lpnParams == NULL)
    return E_POINTER;
  cStrApiToCallA.Attach(CNktDvTools::Wide2Ansi(szApiToCallW));
  if (cStrApiToCallA == NULL)
    return E_OUTOFMEMORY;
  if (strlen(cStrApiToCallA) >= NKT_DV_ARRAYLEN(sMsg.szApiToCallA))
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  //inform about agent load
  if (hAgentLoadRes != S_FALSE)
    OnAgentLoad(lpProc, hAgentLoadRes);
  if (SUCCEEDED(hRes))
  {
    //prepare data
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_CallApi, cTransport.GetNextMsgId(), NULL,
                                     (bSync != FALSE) ? &cMsgResp : NULL);
  }
  if (SUCCEEDED(hRes))
  {
    if (bSync == FALSE && lpnActionIdOrResult != NULL)
      *lpnActionIdOrResult = (LONG)sMsg.dwMsgId;
    nDllFileNameLen = wcslen(szDllFileNameW);
    if (nDllFileNameLen >= NKT_DV_ARRAYLEN(sMsg.szDllNameW))
    {
      hRes = cTransport.SendBigPacketMsg(&(sMsg.dwDllNameBigPacketId), lpProc->GetProcessId(),
                                         szDllFileNameW, nDllFileNameLen*sizeof(WCHAR));
    }
    else
    {
      wcscpy_s(sMsg.szDllNameW, NKT_DV_ARRAYLEN(sMsg.szDllNameW), szDllFileNameW);
      sMsg.dwDllNameBigPacketId = 0;
    }
  }
  if (SUCCEEDED(hRes))
  {
    strcpy_s(sMsg.szApiToCallA, NKT_DV_ARRAYLEN(sMsg.szApiToCallA), (LPSTR)cStrApiToCallA);
    sMsg.nParamsCount = (ULONG)nParamsCount;
    for (i=0; i<(SIZE_T)nParamsCount; i++)
      sMsg.nParameters[i] = (ULONGLONG)(lpnParams[i]);
    //send message
    hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
  }
  if (SUCCEEDED(hRes) && bSync != FALSE)
  {
    hRes = cMsgResp.Wait(cShutdownMtx.GetEvent(), lpAgentCtl->GetShutdownMtx().GetEvent());
    if (SUCCEEDED(hRes))
    {
      if (cMsgResp->bDllNotFound != FALSE || cMsgResp->bApiNotFound != FALSE)
        hRes = NKT_DVERR_NotFound;
      else if (cMsgResp->bExceptionRaised != FALSE)
        hRes = NKT_DVERR_ExceptionRaised;
      else if (lpnActionIdOrResult != NULL)
        *lpnActionIdOrResult = (SIZE_T)(ULONG)(cMsgResp->nResult);
    }
  }
  return hRes;
}

HRESULT CNktDvSpyMgr::CallCustomApi(__out PLONG lpnActionIdOrResult, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                    __in_z LPCWSTR szApiToCallW, __in LONG nParamsCount, __in_opt PSIZE_T lpnParams,
                                    __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return CallCustomApi(lpnActionIdOrResult, cProc, szDllFileNameW, szApiToCallW, nParamsCount, lpnParams,
                       bSync);
}

HRESULT CNktDvSpyMgr::HelperGetWndProc(__out LPVOID* lplpWndProc, __in CNktDvProcess *lpProc, __in HWND hWnd)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  NKT_DV_TMSG_HELPERCALL sMsg;
  TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_HELPERCALL_CALLBACK> cMsgResp;
  HRESULT hRes, hAgentLoadRes;

  if (lplpWndProc != NULL)
    *lplpWndProc = NULL;
  if (lpProc == NULL || lplpWndProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || hWnd == NULL)
    return E_INVALIDARG;
  if (lpProc->GetProcessId() == ::GetCurrentProcessId())
  {
    *lplpWndProc = (LPVOID)::GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    return S_OK;
  }
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //initialize agent if not done yet
    hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
    if (SUCCEEDED(hRes))
    {
      cAgentCtlSdLock.Attach(lpAgentCtl);
      if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
        hRes = NKT_DVERR_Cancelled;
    }
  }
  //inform about agent load
  if (hAgentLoadRes != S_FALSE)
    OnAgentLoad(lpProc, hAgentLoadRes);
  //prepare data
  if (SUCCEEDED(hRes))
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_HelperCall, cTransport.GetNextMsgId(), NULL, &cMsgResp);
  if (SUCCEEDED(hRes))
  {
    sMsg.dwCallCode = NKT_DV_TMSG_CODE_HELPERCALL_GetWndProc;
    sMsg.sGetWndProc.hWnd = NKT_PTR_2_ULONGLONG(hWnd);
    //send message
    hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
  }
  if (SUCCEEDED(hRes))
    hRes = cMsgResp.Wait(cShutdownMtx.GetEvent(), lpAgentCtl->GetShutdownMtx().GetEvent());
  if (SUCCEEDED(hRes))
    *lplpWndProc = (LPVOID)(cMsgResp->sGetWndProc.lpWndProc);
  return hRes;
}

HRESULT CNktDvSpyMgr::HelperGetWndProc(__out LPVOID* lplpWndProc, __in DWORD dwPid, __in HWND hWnd)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  if (lplpWndProc != NULL)
    *lplpWndProc = NULL;
  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return HelperGetWndProc(lplpWndProc, cProc, hWnd);
}

HRESULT CNktDvSpyMgr::GetDbObjectsEnumerator(__deref_out CNktDvDbObjectsEnumerator **lplpEnum,
                                             __in SIZE_T nPlatformBits)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvDbObjectsEnumerator> cEnum;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbObjectsEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  switch (nPlatformBits)
  {
    case 32:
      hRes = cEnum->Initialize(cDvDB_32, FALSE);
      break;
#if defined _M_X64
    case 64:
      hRes = cEnum->Initialize(cDvDB_64, FALSE);
      break;
#endif //_M_X64
    default:
      hRes = E_INVALIDARG;
      break;
  }
  if (FAILED(hRes))
    return hRes;
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

HRESULT CNktDvSpyMgr::GetDbFunctionsEnumerator(__deref_out CNktDvDbObjectsEnumerator **lplpEnum,
                                               __in SIZE_T nPlatformBits)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvDbObjectsEnumerator> cEnum;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbObjectsEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  switch (nPlatformBits)
  {
    case 32:
      hRes = cEnum->Initialize(cDvDB_32, TRUE);
      break;
#if defined _M_X64
    case 64:
      hRes = cEnum->Initialize(cDvDB_64, TRUE);
      break;
#endif //_M_X64
    default:
      hRes = E_INVALIDARG;
      break;
  }
  if (FAILED(hRes))
    return hRes;
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

HRESULT CNktDvSpyMgr::GetDbModulesEnumerator(__deref_out CNktDvDbModulesEnumerator **lplpEnum,
                                             __in SIZE_T nPlatformBits)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvDbModulesEnumerator> cEnum;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbModulesEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  switch (nPlatformBits)
  {
  case 32:
    hRes = cEnum->Initialize(cDvDB_32);
    break;
#if defined _M_X64
  case 64:
    hRes = cEnum->Initialize(cDvDB_64);
    break;
#endif //_M_X64
  default:
    hRes = E_INVALIDARG;
    break;
  }
  if (FAILED(hRes))
    return hRes;
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

HRESULT CNktDvSpyMgr::GetProcessHandle(__out HANDLE *lphProc, __in DWORD dwPid,
                                       __in DWORD dwDesiredAccess)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

  if (lphProc == NULL)
    return E_POINTER;
  *lphProc = NULL;
  if (dwPid == 0)
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //open target process
  *lphProc = cProcHndlMgr->GetHandle(dwPid, dwDesiredAccess, TRUE);
  if ((*lphProc) == NULL)
    return E_ACCESSDENIED;
  return S_OK;
}

HRESULT CNktDvSpyMgr::GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem, __in DWORD dwPid)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

  if (lplpProcMem == NULL)
    return E_POINTER;
  *lplpProcMem = NULL;
  if (dwPid == 0)
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //create process memory
  *lplpProcMem = CNktDvProcessMemory::CreateForPID(dwPid);
  return ((*lplpProcMem) != NULL) ? S_OK : E_OUTOFMEMORY;
}

HRESULT CNktDvSpyMgr::GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem,
                                       __in CNktDvProcess *lpProc)
{
  if (lplpProcMem != NULL)
    *lplpProcMem = NULL;
  if (lpProc == NULL || lplpProcMem == NULL)
    return E_POINTER;
  return GetProcessMemory(lplpProcMem, lpProc->GetProcessId());
}

VOID CNktDvSpyMgr::OnCustomDllLoad(__inout CNktDvProcess *lpProc, __in LONG nActionId,
                                   __in HRESULT hActionResult)
{
  return;
}

VOID CNktDvSpyMgr::OnCustomDllUnload(__inout CNktDvProcess *lpProc, __in LONG nActionId,
                                     __in HRESULT hActionResult)
{
  return;
}

VOID CNktDvSpyMgr::OnCustomApiCall(__inout CNktDvProcess *lpProc, __in LONG nActionId,
                                   __in HRESULT hActionResult, __in LONG nApiResult)
{
  return;
}

VOID CNktDvSpyMgr::OnHookStateChanged(__in CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                      __in CNktDvHook::eState nNewState, __in CNktDvHook::eState nOldState)
{
  return;
}

VOID CNktDvSpyMgr::OnHookCalled(__in CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                __in CNktDvHookCallInfo *lpCallInfo)
{
  return;
}

VOID CNktDvSpyMgr::OnCreateProcessCall(__in CNktDvProcess *lpProc, __in DWORD dwPid,
                                       __in DWORD dwMainThreadId, __in BOOL bIs64BitProcess,
                                       __in BOOL bCanHookNow)
{
  return;
}

VOID CNktDvSpyMgr::OnLoadLibraryCall(__inout CNktDvProcess *lpProc, __in_z LPCWSTR szLibraryNameW,
                                     __in HMODULE hModule)
{
  return;
}

VOID CNktDvSpyMgr::OnFreeLibraryCall(__inout CNktDvProcess *lpProc, __in HMODULE hModule)
{
  return;
}

VOID CNktDvSpyMgr::OnProcessAdded(__inout CNktDvProcess *lpProc)
{
  return;
}

VOID CNktDvSpyMgr::OnProcessRemoved(__inout CNktDvProcess *lpProc)
{
  return;
}

VOID CNktDvSpyMgr::OnAgentLoad(__inout CNktDvProcess *lpProc, __in HRESULT hErrorCode)
{
  return;
}

VOID CNktDvSpyMgr::OnAgentUnload(__inout CNktDvProcess *lpProc)
{
  return;
}

SIZE_T CNktDvSpyMgr::OnCustomMessage(__inout CNktDvProcess *lpProc, __in SIZE_T nMsgCode,
                                     __in SIZE_T nMsgParam)
{
  return 0;
}

VOID CNktDvSpyMgr::OnHookOverwritten(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc)
{
  return;
}

VOID CNktDvSpyMgr::OnThreadStarted()
{
  return;
}

VOID CNktDvSpyMgr::OnThreadEnded()
{
  return;
}

HRESULT CNktDvSpyMgr::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvSpyMgr") == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

BOOL CNktDvSpyMgr::CanUnloadNow()
{
  return CNktDvEngBaseObj::CanUnloadNow();
}

VOID CNktDvSpyMgr::DumpObjects()
{
  CNktDvEngBaseObj::DumpObjects();
  return;
}

VOID CNktDvSpyMgr::IPL_OnProcessAdded(__inout CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  if (cShutdownMtx.IsShuttingDown() == FALSE && cShutdownMtx.IsSystemActive() != FALSE)
    OnProcessAdded(lpProc);
  return;
}

VOID CNktDvSpyMgr::IPL_OnProcessRemoved(__inout CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  if (cShutdownMtx.IsShuttingDown() == FALSE && cShutdownMtx.IsSystemActive() != FALSE)
  {
    HookRemoveProcessItems(lpProc);
    OnProcessRemoved(lpProc);
  }
  return;
}

VOID CNktDvSpyMgr::IPL_WorkerThreadStarted()
{
  OnThreadStarted();
  return;
}

VOID CNktDvSpyMgr::IPL_WorkerThreadEnded()
{
  OnThreadEnded();
  return;
}

HRESULT CNktDvSpyMgr::TEC_OnAgentMessage(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                         __in NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                         __in HANDLE hAgentDupProc,
                                         __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  CAgentControllerShutdownLock cAgentCtlSdLock;
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return NKT_DVERR_Cancelled;
    hRes = GetAgentController(&lpAgentCtl, dwPid);
    if (FAILED(hRes))
      return NKT_DVERR_Cancelled;
    cAgentCtlSdLock.Attach(lpAgentCtl);
    if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
      return NKT_DVERR_Cancelled;
    cProcMem = lpAgentCtl->GetProcessMemory();
  }
  //if the message was sent and uses a callback
  if (lpMsg->nPtrCallback != NULL && (lpMsg->dwMsgId & NKT_DV_TMSG_ID_CALLBACK) != 0)
  {
    CNktDvTransportMessageCallbackHelperBase *lpCallback;

    try
    {
      lpCallback = (CNktDvTransportMessageCallbackHelperBase*)(lpMsg->nPtrCallback);
      hRes = lpCallback->CopyResponseAndSignal(lpMsg, nMsgSize);
    }
    catch (...)
    {
      hRes = NKT_DVERR_ExceptionRaised;
    }
    return hRes;
  }
  //process message
  switch (lpMsg->dwMsgCode)
  {
    case NKT_DV_TMSG_CODE_LoadDllCallback:
      if (nMsgSize < sizeof(NKT_DV_TMSG_LOADDLL_CALLBACK))
        break;
      return OnAgentMsg_LoadDllCallback(lpAgentCtl->cProc,
                                        reinterpret_cast<NKT_DV_TMSG_LOADDLL_CALLBACK*>(lpMsg));

    case NKT_DV_TMSG_CODE_UnloadDllCallback:
      if (nMsgSize < sizeof(NKT_DV_TMSG_UNLOADDLL_CALLBACK))
        break;
      return OnAgentMsg_UnloadDllCallback(lpAgentCtl->cProc,
                                          reinterpret_cast<NKT_DV_TMSG_UNLOADDLL_CALLBACK*>(lpMsg));

    case NKT_DV_TMSG_CODE_CallApiCallback:
      if (nMsgSize < sizeof(NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK))
        break;
      return OnAgentMsg_CallCustomApiCallback(lpAgentCtl->cProc,
                                           reinterpret_cast<NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK*>(lpMsg));

    case NKT_DV_TMSG_CODE_OnCreateProcessCalled:
      if (nMsgSize < sizeof(NKT_DV_TMSG_ONCREATEPROCESS))
        break;
      return OnAgentMsg_CreateProcess(lpAgentCtl->cProc, reinterpret_cast<NKT_DV_TMSG_ONCREATEPROCESS*>(lpMsg));

    case NKT_DV_TMSG_CODE_OnLoadLibraryCalled:
      if (nMsgSize < sizeof(NKT_DV_TMSG_ONLOADLIBRARY))
        break;
      return OnAgentMsg_LoadLibrary(lpAgentCtl, reinterpret_cast<NKT_DV_TMSG_ONLOADLIBRARY*>(lpMsg),
                                    lpConnBigData);

    case NKT_DV_TMSG_CODE_OnFreeLibraryCalled:
      if (nMsgSize < sizeof(NKT_DV_TMSG_ONFREELIBRARY))
        break;
      return OnAgentMsg_FreeLibrary(lpAgentCtl, reinterpret_cast<NKT_DV_TMSG_ONFREELIBRARY*>(lpMsg));

    case NKT_DV_TMSG_CODE_OnHookCalled:
      {
      TNktComPtr<CNktDvModulesEnumerator> cModEnum;

      if (nMsgSize < sizeof(NKT_DV_TMSG_ONHOOKCALLED))
        break;
      hRes = lpAgentCtl->GetModulesCache(&cModEnum);
      if (SUCCEEDED(hRes))
        hRes = OnAgentMsg_HookCalled(lpAgentCtl->cProc, reinterpret_cast<NKT_DV_TMSG_ONHOOKCALLED*>(lpMsg),
                                     cModEnum, cProcMem, lpConnBigData);
      }
      return hRes;

    case NKT_DV_TMSG_CODE_HookStateChange:
      if (nMsgSize < sizeof(NKT_DV_TMSG_HOOKSTATECHANGE))
        break;
      return OnAgentMsg_HookStateChanged(lpAgentCtl->cProc,
                                         reinterpret_cast<NKT_DV_TMSG_HOOKSTATECHANGE*>(lpMsg));

    case NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86:
      if (nMsgSize < sizeof(NKT_DV_TMSG_SUSPENDX64THREADFROMX86))
        break;
      return OnAgentMsg_SuspendX64ThreadFromX86(lpAgentCtl->cProc, hAgentDupProc,
                                           reinterpret_cast<NKT_DV_TMSG_SUSPENDX64THREADFROMX86*>(lpMsg));

    case NKT_DV_TMSG_CODE_InvalidateAgentCtrlDllCache:
      if (nMsgSize < sizeof(NKT_DV_TMSG_COMMON))
        break;
      lpAgentCtl->MarkModulesCacheAsDirty(NULL);
      return S_OK;

    case NKT_DV_TMSG_CODE_CustomMessage:
      if (nMsgSize < sizeof(NKT_DV_TMSG_CUSTOMMESSAGE))
        break;
      return OnAgentMsg_CustomMessage(lpAgentCtl->cProc, cProcMem,
                                      reinterpret_cast<NKT_DV_TMSG_CUSTOMMESSAGE*>(lpMsg));

    case NKT_DV_TMSG_CODE_HookOverwritten:
      if (nMsgSize < sizeof(NKT_DV_TMSG_HOOKOVERWRITTEN))
        break;
      return OnAgentMsg_HooksOverwritten(lpAgentCtl->cProc, reinterpret_cast<NKT_DV_TMSG_HOOKOVERWRITTEN*>(lpMsg));

      /*
    case NKT_DV_TMSG_CODE_InitialConnectionMessage:
      if (nMsgSize < sizeof(NKT_DV_TMSG_INITIALCONNECTIONMESSAGE))
        break;
      return OnAgentMsg_InitialConnectionMessage(lpAgentCtl, cProcMem,
                                                 reinterpret_cast<NKT_DV_TMSG_INITIALCONNECTIONMESSAGE*>(lpMsg));
      */
  }
  NKT_DEBUGPRINTLNA(Nektra::dlEngine|Nektra::dlTransport, (
                     "%lu) Engine[Invalid message received]: Code=%lu, Id=%lu", ::GetTickCount(),
                     lpMsg->dwMsgCode, lpMsg->dwMsgId));
  return E_FAIL;
}

VOID CNktDvSpyMgr::TEC_OnConnectionClosed(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  TNktComPtr<CNktDvProcess> cProc;
  CAgentController *lpAgentCtl;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    SIZE_T nIndex;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return;
    lpAgentCtl = NULL;
    nIndex = FindAgentController(dwPid);
    if (nIndex != NKT_SIZE_T_MAX)
    {
      lpAgentCtl = aAgentControllersList[nIndex];
      aAgentControllersList.RemoveElementAt(nIndex);
    }
    //remove all hooks of dwPid
    HookRemoveBecauseAgentShutdown(dwPid);
  }
  if (lpAgentCtl != NULL)
  {
    cProc = lpAgentCtl->cProc;
    delete lpAgentCtl;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlEngine|Nektra::dlTransport, ("%lu) Engine[Connection closed]: Pid=%lu",
                     ::GetTickCount(), dwPid));
  //rise agent unload event
  if (cProc != NULL)
    OnAgentUnload(cProc);
  return;
}

VOID CNktDvSpyMgr::TEC_OnTransportError(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                        __in HRESULT hRes)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  TNktComPtr<CNktDvProcess> cProc;
  CAgentController *lpAgentCtl;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    SIZE_T nIndex;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return;
    lpAgentCtl = NULL;
    nIndex = FindAgentController(dwPid);
    if (nIndex != NKT_SIZE_T_MAX)
    {
      lpAgentCtl = aAgentControllersList[nIndex];
      aAgentControllersList.RemoveElementAt(nIndex);
    }
    //remove all hooks of dwPid
    HookRemoveBecauseAgentShutdown(dwPid);
  }
  if (lpAgentCtl != NULL)
  {
    cProc = lpAgentCtl->cProc;
    delete lpAgentCtl;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlEngine|Nektra::dlTransport, (
                     "%lu) Engine[Transport error]: Pid=%lu, hRes=%08X", ::GetTickCount(), dwPid, hRes));
  if (cProc != NULL)
    OnAgentUnload(cProc);
  return;
}

VOID CNktDvSpyMgr::TEC_WorkerThreadStarted()
{
  OnThreadStarted();
  return;
}

VOID CNktDvSpyMgr::TEC_WorkerThreadEnded()
{
  OnThreadEnded();
  return;
}

VOID CNktDvSpyMgr::TEC_DispatcherThreadStarted()
{
  OnThreadStarted();
  return;
}

VOID CNktDvSpyMgr::TEC_DispatcherThreadEnded()
{
  OnThreadEnded();
  return;
}

HRESULT CNktDvSpyMgr::ActivateAgent(__inout CNktDvProcess *lpProc)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes, hAgentLoadRes;

  {
    CAgentControllerShutdownLock cAgentCtlSdLock;

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Activate agent]: Pid=%lu", ::GetTickCount(),
                       lpProc->GetProcessId()));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Activate agent]: Pid=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), hRes));
    //inform about agent load
    if (hAgentLoadRes != S_FALSE)
      OnAgentLoad(lpProc, hAgentLoadRes);
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::SendAddHookCommand(__inout CNktDvProcess *lpProc, __inout CNktDvHook *lpHook,
                                         __in DWORD dwHookId, __in DWORD dwBatchId, __in LPVOID lpAddress,
                                         __in BOOL bSync)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes, hAgentLoadRes;

  {
    CAgentControllerShutdownLock cAgentCtlSdLock;
    NKT_DV_TMSG_ADDHOOK sMsg;
    LPWSTR sW;
    SIZE_T nLen;
    CNktEvent cProcessedEvent;
    HANDLE hWaitEvents[3];

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Add hook]: Pid=%lu, Hook=%lu, Batch=%lu",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, TRUE, &hAgentLoadRes);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    //inform about agent load
    if (hAgentLoadRes != S_FALSE)
      OnAgentLoad(lpProc, hAgentLoadRes);
    //create wait event
    if (SUCCEEDED(hRes))
    {
      if (bSync != FALSE)
      {
        if (cProcessedEvent.Create(TRUE, FALSE) != FALSE)
        {
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = lpAgentCtl->GetShutdownMtx().GetEvent();
          hWaitEvents[2] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        hWaitEvents[2] = NULL;
      }
    }
    //prepare data
    if (SUCCEEDED(hRes))
    {
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_AddHook, cTransport.GetNextMsgId(),
                                       hWaitEvents[2], NULL);
    }
    if (SUCCEEDED(hRes))
    {
      sMsg.dwHookId = dwHookId;
      sMsg.dwSpyMgrHookId = lpHook->GetId();
      sMsg.dwBatchId = dwBatchId;
      sMsg.lpAddress = NKT_PTR_2_ULONGLONG(lpAddress);
      sMsg.nFlags = 0;
      if ((lpHook->GetFlags() & CNktDvHook::flgAsyncCallbacks) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_AsyncCallbacks;
      if ((lpHook->GetFlags() & CNktDvHook::flgOnlyPreCall) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPreCall;
      else if ((lpHook->GetFlags() & CNktDvHook::flgOnlyPostCall) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_OnlyPostCall;
      if ((lpHook->GetFlags() & CNktDvHook::flgDontCallIfLoaderLocked) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_DontCallOnLdrLock;
      if ((lpHook->GetFlags() & CNktDvHook::flgDontCallCustomHandlersOnLdrLock) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_DontCallCustomHandlersOnLdrLock;
      if ((lpHook->GetFlags() & CNktDvHook::flgInvalidateCache) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_InvalidateCache;
      if ((lpHook->GetFlags() & CNktDvHook::flgDontSkipJumps) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_DontSkipJumps;
      if ((lpHook->GetFlags() & CNktDvHook::flgDisableStackWalk) != 0)
        sMsg.nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_DisableStackWalk;
      switch (lpProc->GetPlatformBits())
      {
        case 32:
          sMsg.nDbFuncId = (lpHook->lpDbObject32 != NULL) ? (lpHook->lpDbObject32->GetId()) : 0;
          break;
#if defined _M_X64
        case 64:
          sMsg.nDbFuncId = (lpHook->lpDbObject64 != NULL) ? (lpHook->lpDbObject64->GetId()) : 0;
          break;
#endif //_M_X64
        default:
          sMsg.nDbFuncId = 0;
          break;
      }
      sW = lpHook->GetFunctionName();
      nLen = wcslen(sW);
      if (nLen >= NKT_DV_ARRAYLEN(sMsg.szFunctionNameW))
      {
        hRes = cTransport.SendBigPacketMsg(&(sMsg.dwFunctionNameBigPacketId), lpProc->GetProcessId(), sW,
                                           nLen*sizeof(WCHAR));
      }
      else
      {
        wcscpy_s(sMsg.szFunctionNameW, NKT_DV_ARRAYLEN(sMsg.szFunctionNameW), sW);
        sMsg.dwFunctionNameBigPacketId = 0;
      }
    }
    if (SUCCEEDED(hRes))
      hRes = lpHook->SendCustomHandlerData(&(sMsg.dwCustomHandlersBigPacketId), lpProc->GetProcessId(), &cTransport);
    //send message
    if (SUCCEEDED(hRes))
      hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
    if (SUCCEEDED(hRes) && bSync != FALSE)
    {
      //wait until processed
      switch (::WaitForMultipleObjects(3, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+2:
          break;
        default:
          hRes = E_FAIL;
          break;
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Add hook]: Pid=%lu, Hook=%lu, Batch=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId, hRes));
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::SendRemoveHookCommand(__inout CNktDvProcess *lpProc, __in DWORD dwHookId,
                                            __in DWORD dwBatchId, __in BOOL bSync)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  {
    CAgentControllerShutdownLock cAgentCtlSdLock;
    NKT_DV_TMSG_REMOVEHOOK sMsg;
    CNktEvent cProcessedEvent;
    HANDLE hWaitEvents[3];

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Remove hook]: Pid=%lu, Hook=%lu, Batch=%lu",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, FALSE, NULL);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    //create wait event
    if (SUCCEEDED(hRes))
    {
      if (bSync != FALSE)
      {
        if (cProcessedEvent.Create(TRUE, FALSE) != FALSE)
        {
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = lpAgentCtl->GetShutdownMtx().GetEvent();
          hWaitEvents[2] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        hWaitEvents[2] = NULL;
      }
    }
    //prepare data
    if (SUCCEEDED(hRes))
    {
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_RemoveHook, cTransport.GetNextMsgId(),
                                       hWaitEvents[2], NULL);
    }
    if (SUCCEEDED(hRes))
    {
      sMsg.dwHookId = dwHookId;
      sMsg.dwBatchId = dwBatchId;
      //send message
      hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
    }
    if (SUCCEEDED(hRes) && bSync != FALSE)
    {
      //wait until processed
      switch (::WaitForMultipleObjects(3, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+2:
          break;
        default:
          hRes = E_FAIL;
          break;
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Remove hook]: Pid=%lu, Hook=%lu, Batch=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId, hRes));
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::SendEnableHookCommand(__inout CNktDvProcess *lpProc, __in DWORD dwHookId, __in DWORD dwBatchId,
                                            __in BOOL bEnable, __in BOOL bSync)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  {
    CAgentControllerShutdownLock cAgentCtlSdLock;
    NKT_DV_TMSG_ENABLEHOOK sMsg;
    CNktEvent cProcessedEvent;
    HANDLE hWaitEvents[3];

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Enable hook]: Pid=%lu, Hook=%lu, Batch=%lu, %s",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId,
                       (bEnable != FALSE) ? "True" : "False"));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, FALSE, NULL);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    if (SUCCEEDED(hRes))
    {
      //create wait event
      if (bSync != FALSE)
      {
        if (cProcessedEvent.Create(TRUE, FALSE) != FALSE)
        {
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = lpAgentCtl->GetShutdownMtx().GetEvent();
          hWaitEvents[2] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        hWaitEvents[2] = NULL;
      }
    }
    //prepare data
    if (SUCCEEDED(hRes))
    {
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_EnableHook, cTransport.GetNextMsgId(),
                                       hWaitEvents[2], NULL);
    }
    if (SUCCEEDED(hRes))
    {
      sMsg.dwHookId = dwHookId;
      sMsg.dwBatchId = dwBatchId;
      sMsg.bEnable = bEnable;
      //send message
      hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
    }
    if (SUCCEEDED(hRes) && bSync != FALSE)
    {
      //wait until processed
      switch (::WaitForMultipleObjects(3, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+2:
          break;
        default:
          hRes = E_FAIL;
          break;
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Enable hook]: Pid=%lu, Hook=%lu, Batch=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), dwHookId, dwBatchId, hRes));
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::SendBatchHookExecCommand(__in CNktDvProcess *lpProc, __in DWORD dwBatchId,
                                               __in BOOL bSync)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  {
    CAgentControllerShutdownLock cAgentCtlSdLock;
    NKT_DV_TMSG_BATCHHOOKEXEC sMsg;
    CNktEvent cProcessedEvent;
    HANDLE hWaitEvents[3];

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Batch hook exec]: Pid=%lu, Batch=%lu",
                       ::GetTickCount(), lpProc->GetProcessId(), dwBatchId));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, FALSE, NULL);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    //create wait event
    if (SUCCEEDED(hRes))
    {
      if (bSync != FALSE)
      {
        if (cProcessedEvent.Create(TRUE, FALSE) != FALSE)
        {
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = lpAgentCtl->GetShutdownMtx().GetEvent();
          hWaitEvents[2] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        hWaitEvents[2] = NULL;
      }
    }
    if (SUCCEEDED(hRes))
    {
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_BatchHookExec, cTransport.GetNextMsgId(),
                                       hWaitEvents[2], NULL);
    }
    if (SUCCEEDED(hRes))
    {
      sMsg.dwBatchId = dwBatchId;
      //send message
      hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sMsg, sizeof(sMsg));
    }
    if (SUCCEEDED(hRes) && bSync != FALSE)
    {
      //wait until processed
      switch (::WaitForMultipleObjects(3, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+2:
          break;
        default:
          hRes = E_FAIL;
          break;
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Batch hook exec]: Pid=%lu, Batch=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), dwBatchId, hRes));
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::SendDirectCommand(__in CNktDvProcess *lpProc, __in DWORD dwCmdId, __in NKT_DV_TMSG_COMMON *lpMsg,
                                        __in SIZE_T nMsgSize, __in CNktDvTransportMessageCallbackHelperBase *lpCallBack,
                                        __in BOOL bSync)
{
  CAgentController *lpAgentCtl;
  HRESULT hRes;

  if (lpCallBack != NULL)
    bSync = TRUE;
  {
    CAgentControllerShutdownLock cAgentCtlSdLock;
    CNktEvent cProcessedEvent;
    HANDLE hWaitEvents[3];

    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[SendDirectCommand]: Pid=%lu, Cmd=%lu",
                       ::GetTickCount(), lpProc->GetProcessId(), dwCmdId));
    lpAgentCtl = NULL;
    {
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

      //initialize agent if not done yet
      hRes = GetAgentController(&lpAgentCtl, lpProc, FALSE, NULL);
      if (SUCCEEDED(hRes))
      {
        cAgentCtlSdLock.Attach(lpAgentCtl);
        if (lpAgentCtl->GetAgentState() != CAgentController::stActive)
          hRes = NKT_DVERR_Cancelled;
      }
    }
    //create wait event
    if (SUCCEEDED(hRes))
    {
      if (bSync != FALSE && lpCallBack == NULL)
      {
        if (cProcessedEvent.Create(TRUE, FALSE) != FALSE)
        {
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = lpAgentCtl->GetShutdownMtx().GetEvent();
          hWaitEvents[2] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        hWaitEvents[2] = NULL;
      }
    }
    //prepare data
    if (SUCCEEDED(hRes))
      hRes = NktDvTransportInitMessage(lpMsg, dwCmdId, cTransport.GetNextMsgId(), hWaitEvents[2], lpCallBack);
    //send message
    if (SUCCEEDED(hRes))
      hRes = cTransport.SendMsg(lpProc->GetProcessId(), lpMsg, nMsgSize);
    //wait until processed
    if (SUCCEEDED(hRes))
    {
      if (lpCallBack != NULL)
      {
        hRes = lpCallBack->Wait(cShutdownMtx.GetEvent(), lpAgentCtl->GetShutdownMtx().GetEvent());
      }
      else if (bSync != FALSE)
      {
        switch (::WaitForMultipleObjects(3, hWaitEvents, FALSE, INFINITE))
        {
          case WAIT_OBJECT_0:
          case WAIT_OBJECT_0+1:
            hRes = NKT_DVERR_Cancelled;
            break;
          case WAIT_OBJECT_0+2:
            break;
          default:
            hRes = E_FAIL;
            break;
        }
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Engine[Batch hook exec]: Pid=%lu, Cmd=%lu, hRes=%08X",
                       ::GetTickCount(), lpProc->GetProcessId(), dwCmdId, hRes));
  }
  if (lpAgentCtl != NULL &&
      FAILED(hRes) && hRes != NKT_DVERR_NotFound && hRes != NKT_DVERR_Cancelled)
    RemoveAgentController(lpAgentCtl);
  return hRes;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_LoadDllCallback(__inout CNktDvProcess *lpProc,
                                                 __in NKT_DV_TMSG_LOADDLL_CALLBACK *lpMsg)
{
  OnCustomDllLoad(lpProc, lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK), lpMsg->hLoadDllRes);
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_UnloadDllCallback(__inout CNktDvProcess *lpProc,
                                                   __in NKT_DV_TMSG_UNLOADDLL_CALLBACK *lpMsg)
{
  OnCustomDllUnload(lpProc, lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK), lpMsg->hUnloadDllRes);
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_CallCustomApiCallback(__inout CNktDvProcess *lpProc,
                                                       __in NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK *lpMsg)
{
  HRESULT hRes;

  if (lpMsg->bDllNotFound != FALSE || lpMsg->bApiNotFound != FALSE)
    hRes = NKT_DVERR_NotFound;
  else if (lpMsg->bExceptionRaised != FALSE)
    hRes = NKT_DVERR_ExceptionRaised;
  else
    hRes = S_OK;
  OnCustomApiCall(lpProc, lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK), hRes, lpMsg->nResult);
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_CreateProcess(__inout CNktDvProcess *lpProc,
                                               __in NKT_DV_TMSG_ONCREATEPROCESS *lpMsg)
{
  DELAYEDHOOK_DATA *lpNewData;
  HRESULT hRes;

  //add the handle to the list
  hRes = cProcHndlMgr->AddHandle(lpMsg->dwPid, (HANDLE)(lpMsg->hProcess), PROCESS_ALL_ACCESS);
  if (FAILED(hRes))
  {
    ::CloseHandle((HANDLE)(lpMsg->hProcess));
    return hRes;
  }
  //----
  if (lpMsg->bCanHookNow != FALSE)
  {
    HookExecCreateProcessAutoHook(lpProc, lpMsg->dwPid);
  }
  else
  {
    lpNewData = new DELAYEDHOOK_DATA;
    if (lpNewData == NULL)
      return E_OUTOFMEMORY;
    lpNewData->cProc = lpProc;
    lpNewData->dwChildPid = lpMsg->dwPid;
    if (sDelayedHook.aItems.AddElement(lpNewData) == FALSE)
    {
      delete lpNewData;
      return E_OUTOFMEMORY;
    }
    sDelayedHook.cAvailableEvent.Set();
  }
  OnCreateProcessCall(lpProc, lpMsg->dwPid, lpMsg->dwTid, lpMsg->bIs64BitProcess, lpMsg->bCanHookNow);
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_LoadLibrary(__in CAgentController *lpAgentCtl, __in NKT_DV_TMSG_ONLOADLIBRARY *lpMsg,
                                             __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvTransportBigData::CParser cDllNameBP(lpConnBigData, lpMsg->dwDllNameBigPacketId);
  ULONG nCount;
  CNktStringW cStrNameW;
  HINSTANCE hDll;
  LPBYTE s;
  SIZE_T nDataLen, nLen;

  //build user dll data and add to list
  s = cDllNameBP.GetData();
  nDataLen = cDllNameBP.GetLength();
  if (nDataLen < sizeof(ULONG))
    return NKT_DVERR_InvalidTransportData;
  nCount = *((ULONG NKT_UNALIGNED*)s);
  s += sizeof(ULONG);
  nDataLen -= sizeof(ULONG);
  while (nCount > 0)
  {
    if (nDataLen < sizeof(ULONGLONG)+sizeof(ULONG))
      return NKT_DVERR_InvalidTransportData;
    hDll = (HINSTANCE)(*((ULONGLONG NKT_UNALIGNED*)s));
    s += sizeof(ULONGLONG);
    nDataLen -= sizeof(ULONGLONG);
    //----
    lpAgentCtl->MarkModulesCacheAsDirty(hDll);
    //----
    nLen = (SIZE_T)(*((ULONG NKT_UNALIGNED*)s));
    s += sizeof(ULONG);
    nDataLen -= sizeof(ULONG);
    if (nDataLen < nLen || (nLen & 1) != 0)
      return NKT_DVERR_InvalidTransportData;
    if (cStrNameW.CopyN((LPWSTR)s, nLen/sizeof(WCHAR)) == FALSE)
      return E_OUTOFMEMORY;
    s += nLen;
    nDataLen -= nLen;
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Agent [LoadDll]: %S", ::GetTickCount(), (LPWSTR)cStrNameW));
    HookExecLoadLibraryAutoHook(lpAgentCtl->cProc, (LPWSTR)cStrNameW);
    OnLoadLibraryCall(lpAgentCtl->cProc, (LPWSTR)cStrNameW, hDll);
    nCount--;
  }
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_FreeLibrary(__in CAgentController *lpAgentCtl, __in NKT_DV_TMSG_ONFREELIBRARY *lpMsg)
{
  HINSTANCE hDll;
  SIZE_T i;

  for (i=0; i<NKT_DV_ARRAYLEN(lpMsg->aInstances) && lpMsg->aInstances[i]!=0; i++)
  {
    hDll = (HINSTANCE)(lpMsg->aInstances[i]);
    lpAgentCtl->MarkModulesCacheAsDirty(hDll);
    OnFreeLibraryCall(lpAgentCtl->cProc, hDll);
  }
  return S_OK;
}

#define remcurrcallstate32 ((LPNKT_DV_CURRENT_CALL_STATE32)(lpMsg->lpRemoteCurrCallState))
#define remcurrcallstate64 ((LPNKT_DV_CURRENT_CALL_STATE64)(lpMsg->lpRemoteCurrCallState))
HRESULT CNktDvSpyMgr::OnAgentMsg_HookCalled(__inout CNktDvProcess *lpProc, __in NKT_DV_TMSG_ONHOOKCALLED *lpMsg,
                                 __inout CNktDvModulesEnumerator *lpModEnum, __inout CNktDvProcessMemory *lpProcMem,
                                 __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvTransportBigData::CParser cCustomParamsBP(lpConnBigData, lpMsg->sParamsData.dwCustomParamsBigPacketId);
  TNktComPtr<CHookItem> cHookItem;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    SIZE_T nIndex;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    nIndex = FindHookItemByHookId(lpMsg->dwHookId);
    if (nIndex != NKT_SIZE_T_MAX)
      cHookItem = aHookItemsListByHookId[nIndex];
    if (cHookItem == NULL ||
        (cHookItem->nState != CNktDvHook::stActive && cHookItem->nState != CNktDvHook::stActivating))
      return S_OK;
  }
  if (cHookItem != NULL)
  {
    TNktComPtr<CNktDvHookCallInfo> cHookCallInfo;
    TNktComPtr<CNktDvParam::CCallStateContainer> cCallState;
    TNktComPtr<CNktDvParam> cTempParam;
    CNktDvDbObjectNoRef *lpDbObject;
    SIZE_T i, k, k2, nCount, nDataLen, nProcBits;
    int nFundType;
    LPBYTE s;
    HRESULT hRes;

    cHookCallInfo.Attach(NKT_MEMMGR_NEW CNktDvHookCallInfo);
    if (cHookCallInfo == NULL)
      return E_OUTOFMEMORY;
    switch (nProcBits = cHookItem->cProc->GetPlatformBits())
    {
      case 32:
        cCallState.Attach(NKT_MEMMGR_NEW CNktDvParam::CCallStateContainer(remcurrcallstate32));
        if (cCallState == NULL)
          return E_OUTOFMEMORY;
        lpDbObject = cHookItem->cHook->lpDbObject32;
        break;
#if defined _M_X64
      case 64:
        cCallState.Attach(NKT_MEMMGR_NEW CNktDvParam::CCallStateContainer(remcurrcallstate64));
        if (cCallState == NULL)
          return E_OUTOFMEMORY;
        lpDbObject = cHookItem->cHook->lpDbObject64;
        break;
#endif //_M_X64
      default:
        return E_FAIL;
    }
    cHookCallInfo->bPreCall = lpMsg->bPreCall;
    cHookCallInfo->nCookie = lpMsg->nCookie;
    cHookCallInfo->cHook = cHookItem->cHook;
    cHookCallInfo->cProc = cHookItem->cProc;
    cHookCallInfo->cModEnum = lpModEnum;
    cHookCallInfo->cCallState = cCallState;
    cHookCallInfo->lpRemoteIntercallCustomData = lpMsg->lpRemoteIntercallCustomData;
    cHookCallInfo->dwOsLastError = lpMsg->dwOsLastErrorCopy;
    for (i=0; i<NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth; i++)
      cHookCallInfo->nStackTrace[i] = (SIZE_T)(lpMsg->nStackTraceCopy[i]);
    cHookCallInfo->nCurrentTimeMs = lpMsg->nCurrentTimeMs;
    cHookCallInfo->nElapsedTimeMs = lpMsg->nElapsedTimeMs;
    cHookCallInfo->nChildsElapsedTimeMs = lpMsg->nChildsElapsedTimeMs;
    cHookCallInfo->nKernelTimeMs = lpMsg->nKernelTimeMs;
    cHookCallInfo->nUserTimeMs = lpMsg->nUserTimeMs;
    cHookCallInfo->nCpuClockCycles = lpMsg->nCpuClockCycles;
    cHookCallInfo->dwThreadId = lpMsg->dwThreadId;
    cHookCallInfo->dwChainDepth = lpMsg->dwChainDepth;
    //get function params from cache
    cHookCallInfo->cFuncParams.Attach(cHookItem->cFuncParamCache.GetFuncParam());
    cHookCallInfo->cCustomParams.Attach(NKT_MEMMGR_NEW CNktDvParamsEnumerator);
    if (cHookCallInfo->cFuncParams == NULL || cHookCallInfo->cCustomParams == NULL)
      return E_OUTOFMEMORY;
    cHookCallInfo->cProcMem = cHookCallInfo->cFuncParams->cResult->cProcMem;
    //parse parameters data
    nCount = cHookCallInfo->cFuncParams->cParameters->GetCount();
    if (nCount != lpMsg->sParamsData.nParamsCount)
      return NKT_DVERR_InvalidTransportData;
    if (lpDbObject != NULL)
    {
      if (nCount > NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams))
        nCount = NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams);
      for (i=0; i<nCount; i++)
      {
        cTempParam.Release();
        hRes = cHookCallInfo->cFuncParams->cParameters->GetAt(i, &cTempParam);
        if (FAILED(hRes))
          return hRes;
        cTempParam->SpyMgr_Init((ENktDvMiscEnums::eNktDvRegister)(lpMsg->sParamsData.sParams[i].nRegister),
                                (LPVOID)(lpMsg->sParamsData.sParams[i].lpAddress), cCallState);
      }
    }
    else
    {
      if (lpMsg->sParamsData.nParamsCount != 0)
        return NKT_DVERR_InvalidTransportData;
    }
    //parse return value
    cHookCallInfo->cFuncParams->cResult->SpyMgr_Init(
                       (ENktDvMiscEnums::eNktDvRegister)(lpMsg->sParamsData.sResult.nRegister),
                       (LPVOID)(lpMsg->sParamsData.sResult.lpAddress), cCallState);
    //parse custom parameters
    if (lpMsg->sParamsData.nParamsCount > NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams) ||
        lpMsg->sParamsData.nCustomParamsCount > 0)
    {
      if (lpMsg->sParamsData.dwCustomParamsBigPacketId == 0)
        return NKT_DVERR_InvalidTransportData;
      s = cCustomParamsBP.GetData();
      if (s == NULL)
        return NKT_DVERR_InvalidTransportData;
      nDataLen = cCustomParamsBP.GetLength();
      //----
      nCount = ((SIZE_T)(lpMsg->sParamsData.nParamsCount) > NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams)) ?
                  ((SIZE_T)(lpMsg->sParamsData.nParamsCount) - NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams)) : 0;
      k = NKT_DV_ALIGN_ULONGLONG(nCount * sizeof(NKT_DV_TMSG_ONHOOKCALLED_PARAM));
      if (nDataLen < k)
        return NKT_DVERR_InvalidTransportData;
      nDataLen -= k;
      for (i=0; i<nCount; i++)
      {
        cTempParam.Release();
        hRes = cHookCallInfo->cFuncParams->cParameters->GetAt(i+
                                                              NKT_DV_ARRAYLEN(lpMsg->sParamsData.sParams),
                                                              &cTempParam);
        if (FAILED(hRes))
          return hRes;
        cTempParam->SpyMgr_Init(
               (ENktDvMiscEnums::eNktDvRegister)(((NKT_DV_TMSG_ONHOOKCALLED_PARAM*)s)->nRegister),
               (LPVOID)(((NKT_DV_TMSG_ONHOOKCALLED_PARAM*)s)->lpAddress), cCallState);
        s += sizeof(NKT_DV_TMSG_ONHOOKCALLED_PARAM);
      }
      s = (LPBYTE)NKT_DV_ALIGN_ULONGLONG(s);
      //----
      for (i=0; i<lpMsg->sParamsData.nCustomParamsCount; i++)
      {
        if (nDataLen < 2*sizeof(ULONGLONG)+sizeof(int))
          return NKT_DVERR_InvalidTransportData;
        k = (SIZE_T)*((ULONGLONG*)s); //get value size
        k2 = (SIZE_T)*((ULONGLONG*)(s+sizeof(ULONGLONG))); //get name size
        nFundType = *((int*)(s+2*sizeof(ULONGLONG))); //get fundamental type
        //check length
        if (nDataLen < NKT_DV_ALIGN_ULONGLONG(2*sizeof(ULONGLONG)+sizeof(int)+k+k2))
          return NKT_DVERR_InvalidTransportData;
        //create new param
        cTempParam.Release();
        cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nProcBits));
        if (cTempParam == NULL)
          return E_OUTOFMEMORY;
        switch (nProcBits)
        {
          case 32:
            cTempParam->lpDbObj = cDvDB_32->GetFundamental(
                                                (CNktDvDbObjectNoRef::eFundamentalType)nFundType);
            if (cTempParam->lpDbObj == NULL)
              return NKT_DVERR_InvalidTransportData;
            break;
#if defined _M_X64
          case 64:
            cTempParam->lpDbObj = cDvDB_64->GetFundamental(
                                                (CNktDvDbObjectNoRef::eFundamentalType)nFundType);
            if (cTempParam->lpDbObj == NULL)
              return NKT_DVERR_InvalidTransportData;
            break;
#endif //_M_X64
        }
        hRes = cTempParam->SpyMgr_Init4Custom(cHookCallInfo->cProcMem, nFundType,
                               (LPWSTR)(s+2*sizeof(ULONGLONG)+sizeof(int)), k2 / sizeof(WCHAR),
                               s+(2*sizeof(ULONGLONG)+sizeof(int)+k2), k, cCallState);
        if (FAILED(hRes))
          return hRes;
        k = NKT_DV_ALIGN_ULONGLONG(2*sizeof(ULONGLONG)+sizeof(int)+k+k2);
        s += k;
        nDataLen -= k;
        //----
        hRes = cHookCallInfo->cCustomParams->Add(cTempParam);
        if (FAILED(hRes))
          return hRes;
      }
    }
    //invalidate cache if needed
    if ((cHookItem->cHook->GetFlags() & CNktDvHook::flgInvalidateCache) != 0)
      CNktDvProcess::MarkModulesEnumeratorAsDirty(cHookItem->cProc->GetProcessId(), NULL);
    //call callback
    OnHookCalled(cHookItem->cHook, cHookItem->cProc, cHookCallInfo);
    //invalidate callinfo
    cHookCallInfo->Invalidate();
    //if a synchronized call, write callback info
    if (lpMsg->bASync == FALSE)
    {
      //check changes in last error code
      if (cHookCallInfo->dwOsLastError != lpMsg->dwOsLastErrorCopy)
      {
        switch (nProcBits)
        {
          case 32:
            hRes = lpProcMem->WriteDWord(&(remcurrcallstate32->dwOsLastError),
                                         cHookCallInfo->dwOsLastError);
            break;
#if defined _M_X64
          case 64:
            hRes = lpProcMem->WriteDWord(&(remcurrcallstate64->dwOsLastError),
                                         cHookCallInfo->dwOsLastError);
            break;
#endif //_M_X64
          default:
            hRes = E_FAIL;
            break;
        }
        if (FAILED(hRes))
          return hRes;
      }
      //skip call?
      if (cHookCallInfo->bSkipCall != FALSE)
      {
        switch (nProcBits)
        {
          case 32:
            hRes = lpProcMem->WriteDWord(&(remcurrcallstate32->dwSkipCall), 1);
            break;
#if defined _M_X64
          case 64:
            hRes = lpProcMem->WriteDWord(&(remcurrcallstate64->dwSkipCall), 1);
            break;
#endif //_M_X64
          default:
            hRes = E_FAIL;
            break;
        }
        if (FAILED(hRes))
          return hRes;
      }
    }
  }
  return S_OK;
}
#undef remcurrcallstate32
#undef remcurrcallstate64

HRESULT CNktDvSpyMgr::OnAgentMsg_HookStateChanged(__inout CNktDvProcess *lpProc,
                                                  __in NKT_DV_TMSG_HOOKSTATECHANGE *lpMsg)
{
  CNktDvHook::eState nNewState, nOldState;
  TNktComPtr<CHookItem> cHookItem;

  nNewState = (CNktDvHook::eState)-1;
  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    SIZE_T nIndex;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    nIndex = FindHookItemByHookId(lpMsg->dwHookId);
    if (nIndex != NKT_SIZE_T_MAX)
      cHookItem = aHookItemsListByHookId[nIndex];
    if (cHookItem != NULL)
    {
      nOldState = cHookItem->nState;
      switch (lpMsg->dwNewState)
      {
        case NKT_DV_TMSG_HOOKSTATECHANGE_Installed:
          if (nOldState == CNktDvHook::stActivating)
            cHookItem->nState = nNewState = CNktDvHook::stActive;
          break;
        case NKT_DV_TMSG_HOOKSTATECHANGE_Enabled:
          if (nOldState == CNktDvHook::stDisabled)
            cHookItem->nState = nNewState = CNktDvHook::stActive;
          break;
        case NKT_DV_TMSG_HOOKSTATECHANGE_Disabled:
          if (nOldState == CNktDvHook::stActive)
            cHookItem->nState = nNewState = CNktDvHook::stDisabled;
          break;
      }
    }
  }
  if (nNewState != (CNktDvHook::eState)-1 && cHookItem != NULL)
    OnHookStateChanged(cHookItem->cHook, cHookItem->cProc, nNewState, nOldState);
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_SuspendX64ThreadFromX86(__inout CNktDvProcess *lpProc,
                                                         __in HANDLE hAgentDupProc,
                                                         __in NKT_DV_TMSG_SUSPENDX64THREADFROMX86 *lpMsg)
{
  NKT_DV_TMSG_SUSPENDX64THREADFROMX86_CALLBACK sCallBackMsg;
  HANDLE hReadyEvent, hContinueEvent, hEventCopy[2];
  HRESULT hRes;

  //add the handle to the list
  hRes = cProcHndlMgr->AddHandle(lpProc->GetProcessId(), (HANDLE)(lpMsg->hProcess), PROCESS_ALL_ACCESS);
  if (FAILED(hRes))
  {
    ::CloseHandle((HANDLE)(lpMsg->hProcess));
    return hRes;
  }
  //build and send callback message
  NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86Callback,
                                     lpMsg);
  sCallBackMsg.hRes = CNktDvTools::SuspendAfterCreateProcessW(&hReadyEvent, &hContinueEvent,
                          ::GetCurrentProcess(), (HANDLE)(lpMsg->hProcess), (HANDLE)(lpMsg->hMainThread));
  sCallBackMsg.hReadyEvent = sCallBackMsg.hContinueEvent = NULL;
  if (SUCCEEDED(sCallBackMsg.hRes))
  {
    if (::DuplicateHandle(::GetCurrentProcess(), hReadyEvent, hAgentDupProc, &hEventCopy[0], 0, FALSE,
                          DUPLICATE_SAME_ACCESS) != FALSE &&
        ::DuplicateHandle(::GetCurrentProcess(), hContinueEvent, hAgentDupProc, &hEventCopy[1], 0, FALSE,
                          DUPLICATE_SAME_ACCESS) != FALSE)
    {
      sCallBackMsg.hReadyEvent = NKT_PTR_2_ULONGLONG(hEventCopy[0]);
      sCallBackMsg.hContinueEvent = NKT_PTR_2_ULONGLONG(hEventCopy[1]);
    }
    else
    {
      sCallBackMsg.hRes = NKT_HRESULT_FROM_LASTERROR();
    }
  }
  //send callback message
  hRes = cTransport.SendMsg(lpProc->GetProcessId(), &sCallBackMsg, sizeof(sCallBackMsg));
  if (hContinueEvent != NULL)
  {
    if (FAILED(hRes))
      ::SetEvent(hContinueEvent); //on failed transmition, enable the process to continue its execution
    ::CloseHandle(hContinueEvent);
  }
  if (hReadyEvent != NULL)
    ::CloseHandle(hReadyEvent);
  //done
  return hRes;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_CustomMessage(__inout CNktDvProcess *lpProc, __inout CNktDvProcessMemory *lpProcMem,
                                               __in NKT_DV_TMSG_CUSTOMMESSAGE *lpMsg)
{
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  SIZE_T nRetVal;
  HRESULT hRes;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
  }
  nRetVal = OnCustomMessage(cProc, (SIZE_T)(lpMsg->nCode), (SIZE_T)(lpMsg->nParam));
  if (lpMsg->bASync == FALSE)
    hRes = lpProcMem->WriteQWord((LPVOID)(lpMsg->nAddressOfReturnValue), (ULONGLONG)nRetVal);
  else
    hRes = S_OK;
  return S_OK;
}

HRESULT CNktDvSpyMgr::OnAgentMsg_HooksOverwritten(__inout CNktDvProcess *lpProc,
                                                  __in NKT_DV_TMSG_HOOKOVERWRITTEN *lpMsg)
{
  TNktComPtr<CHookItem> cHookItem;
  TNktArrayListWithRelease<CHookItem*,256> cHookItemsList;
  SIZE_T nHookIdx, nHooksCount;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    DWORD i;
    SIZE_T nIndex;

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    for (i=0; i<lpMsg->dwCount; i++)
    {
      cHookItem.Release();
      nIndex = FindHookItemByHookId(lpMsg->aHookIdsList[i]);
      if (nIndex != NKT_SIZE_T_MAX)
        cHookItem = aHookItemsListByHookId[nIndex];
      if (cHookItem != NULL && cHookItem->nState != CNktDvHook::stRemoved)
      {
        if (cHookItemsList.AddElement(cHookItem) == FALSE)
          return E_OUTOFMEMORY;
        ((CHookItem*)cHookItem)->AddRef();
      }
    }
  }
  //send messages
  nHooksCount = cHookItemsList.GetCount();
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHookItem = cHookItemsList[nHookIdx];
    if (cHookItem->nState != CNktDvHook::stRemoved)
      OnHookOverwritten(cHookItem->cHook, cHookItem->cProc);
  }
  return S_OK;
}

/*
#define ICM_READVALUE(_dest, _desttype, _struct, _method)         \
  if (_struct.nSize < (ULONGLONG)sizeof(_desttype))               \
    return NKT_DVERR_InvalidTransportData;                        \
  hRes = lpProcMem->_method(_dest, (LPVOID)(_struct.nPtr), 1);    \
  if (FAILED(hRes))                                               \
    return hRes;                                                  \
  _struct.nPtr += sizeof(_desttype);                              \
  _struct.nSize -= sizeof(_desttype)
HRESULT CNktDvSpyMgr::OnAgentMsg_InitialConnectionMessage(__in CAgentController *lpAgentCtl,
                                                          __inout CNktDvProcessMemory *lpProcMem,
                                                          __in NKT_DV_TMSG_INITIALCONNECTIONMESSAGE *lpMsg)
{
  CNktStringW cStrTempW;
  DWORD dwCount, dwNameLen;
  LPWSTR sW;
  ULONGLONG hDll;
  HRESULT hRes;

  NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Agent [InitialConnectionMessage]", ::GetTickCount()));
  //--------
  ICM_READVALUE(&dwCount, ULONG, lpMsg->sModifiedImage, ReadDWord);
  while (dwCount > 0)
  {
    dwCount--;
  }
  //--------
  ICM_READVALUE(&dwCount, ULONG, lpMsg->sDllInfo, ReadDWord);
  while (dwCount > 0)
  {
    ICM_READVALUE(&hDll, ULONGLONG, lpMsg->sDllInfo, ReadQWord);
    lpAgentCtl->MarkModulesCacheAsDirty((HINSTANCE)hDll);
    //----
    ICM_READVALUE(&dwNameLen, ULONG, lpMsg->sDllInfo, ReadDWord);
    if (cStrTempW.EnsureBuffer(dwNameLen / sizeof(WCHAR)) == FALSE)
      return E_OUTOFMEMORY;
    sW = (LPWSTR)cStrTempW;
    if (lpMsg->sDllInfo.nSize < (ULONGLONG)dwNameLen || (dwNameLen & 1) != 0)
      return NKT_DVERR_InvalidTransportData;
    if (dwNameLen > 0)
    {
      hRes = lpProcMem->Read(sW, (LPVOID)(lpMsg->sDllInfo.nPtr), (SIZE_T)dwNameLen);
      if (FAILED(hRes))
        return hRes;
      lpMsg->sDllInfo.nPtr += (SIZE_T)dwNameLen;
      lpMsg->sDllInfo.nSize -= (SIZE_T)dwNameLen;
    }
    sW[dwNameLen / sizeof(WCHAR)] = 0;
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) Agent [Init-LoadDll]: %S", ::GetTickCount(), sW));
    HookExecLoadLibraryAutoHook(lpAgentCtl->cProc, sW);
    OnLoadLibraryCall(lpAgentCtl->cProc, sW, (HINSTANCE)hDll);
    //----
    dwCount--;
  }
  return S_OK;
}
*/
