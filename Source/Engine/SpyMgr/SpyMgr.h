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

#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\ProcessesEnum.h"
#include "..\..\Common\ProcessMemory.h"
#include "..\..\Common\ShutdownMutex.h"
#include "..\..\Common\ParamsEnum.h"
#include "..\..\Common\FunctionParamsCache.h"
#include "..\..\Common\ProcessHandle.h"
#include "..\..\Common\Debug.h"
#include "..\..\Common\PdbFunctionSymbol.h"
#include "..\..\Common\AgentCommon.h"
#include "InternalProcessesList.h"
#include "Hook.h"
#include "TransportEngine.h"

//-----------------------------------------------------------

class CNktDvSpyMgr : public TNktDvEngBaseObjWithLock<CNktFastMutex>, protected CNktDvInternalProcessesListCallback,
                     protected CNktDvTransportEngineCallback, protected CNktThread
{
public:
  CNktDvSpyMgr();
  virtual ~CNktDvSpyMgr();

  virtual HRESULT SettingOverride(__in_z LPCWSTR szSettingW, __in DWORD dwValue);

  virtual HRESULT Initialize(__in_z LPCWSTR szDatabasePathW, __in_z LPCWSTR szAgentPathW,
                             __in DWORD dwFlags);

  virtual HRESULT GetProcessesEnumerator(__deref_out CNktDvProcessesEnumerator **lplpEnum);
  virtual HRESULT GetHooksEnumerator(__deref_out CNktDvHooksEnumerator **lplpEnum);

  virtual HRESULT CreateHooksCollection(__deref_out CNktDvHooksEnumerator **lplpEnum);

  virtual HRESULT CreateHook(__deref_out CNktDvHook **lplpHook, __in CNktDvExportedFunction *lpExpFunc,
                             __in LONG nHookFlags);
  virtual HRESULT CreateHook(__deref_out CNktDvHook **lplpHook, __in_z LPCWSTR szFunctionNameW,
                             __in LONG nHookFlags);
  virtual HRESULT CreateHook(__deref_out CNktDvHook **lplpHook, __in CNktDvPdbFunctionSymbol *lpSymbol,
                             __in LONG nHookFlags);
  virtual HRESULT CreateHook(__deref_out CNktDvHook **lplpHook, __in LPVOID lpAddress,
                             __in_z LPCWSTR szFunctionNameW, __in LONG nHookFlags);
  virtual HRESULT DestroyHook(__inout CNktDvHook *lpHook);

  virtual HRESULT LoadAgent(__in CNktDvProcess *lpProc);
  virtual HRESULT LoadAgent(__in DWORD dwPid);
  virtual HRESULT UnloadAgent(__in CNktDvProcess *lpProc);
  virtual HRESULT UnloadAgent(__in DWORD dwPid);

  virtual HRESULT LoadCustomDll(__out PLONG lpnActionId, __in CNktDvProcess *lpProc, __in LPCWSTR szDllFileNameW,
                                __in BOOL bUnloadOnAgentShutdown=TRUE, __in BOOL bSync=FALSE);
  virtual HRESULT LoadCustomDll(__out PLONG lpnActionId, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                __in BOOL bUnloadOnAgentShutdown=TRUE, __in BOOL bSync=FALSE);
  virtual HRESULT UnloadCustomDll(__out PLONG lpnActionId, __in CNktDvProcess *lpProc,
                                  __in_z LPCWSTR szDllFileNameW, __in BOOL bSync=FALSE);
  virtual HRESULT UnloadCustomDll(__out PLONG lpnActionId, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                  __in BOOL bSync=FALSE);
  virtual HRESULT CallCustomApi(__out PLONG lpnActionIdOrResult, __in CNktDvProcess *lpProc,
                                __in_z LPCWSTR szDllFileNameW, __in_z LPCWSTR szApiToCallW,
                                __in LONG nParamsCount=0, __in_opt PSIZE_T lpnParams=NULL, __in BOOL bSync=FALSE);
  virtual HRESULT CallCustomApi(__out PLONG lpnActionIdOrResult, __in DWORD dwPid, __in_z LPCWSTR szDllFileNameW,
                                __in LPCWSTR szApiToCallW, __in LONG nParamsCount=0, __in_opt PSIZE_T lpnParams=NULL,
                                __in BOOL bSync=FALSE);

  virtual HRESULT HelperGetWndProc(__out LPVOID* lplpWndProc, __in CNktDvProcess *lpProc, __in HWND hWnd);
  virtual HRESULT HelperGetWndProc(__out LPVOID* lplpWndProc, __in DWORD dwPid, __in HWND hWnd);

  virtual HRESULT GetDbObjectsEnumerator(__deref_out CNktDvDbObjectsEnumerator **lplpEnum, __in SIZE_T nPlatformBits);
  virtual HRESULT GetDbFunctionsEnumerator(__deref_out CNktDvDbObjectsEnumerator **lplpEnum, __in SIZE_T nPlatformBits);
  virtual HRESULT GetDbModulesEnumerator(__deref_out CNktDvDbModulesEnumerator **lplpEnum, __in SIZE_T nPlatformBits);

  virtual HRESULT GetProcessHandle(__out HANDLE *lphProc, __in DWORD dwPid, __in DWORD dwDesiredAccess);

  virtual HRESULT GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem, __in DWORD dwPid);
  virtual HRESULT GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem, __in CNktDvProcess *lpProc);

  virtual VOID OnCustomDllLoad(__inout CNktDvProcess *lpProc, __in LONG nActionId, __in HRESULT hActionResult);
  virtual VOID OnCustomDllUnload(__inout CNktDvProcess *lpProc, __in LONG nActionId, __in HRESULT hActionResult);
  virtual VOID OnCustomApiCall(__inout CNktDvProcess *lpProc, __in LONG nActionId, HRESULT hActionResult,
                               __in LONG nApiResult);

  virtual VOID OnHookStateChanged(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                  __in CNktDvHook::eState nNewState, __in CNktDvHook::eState nOldState);
  virtual VOID OnHookCalled(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                            __inout CNktDvHookCallInfo *lpCallInfo);

  virtual VOID OnCreateProcessCall(__inout CNktDvProcess *lpProc, __in DWORD dwPid, __in DWORD dwMainThreadId,
                                   __in BOOL bIs64BitProcess, __in BOOL bCanHookNow);
  virtual VOID OnLoadLibraryCall(__inout CNktDvProcess *lpProc, __in_z LPCWSTR szLibraryNameW, __in HMODULE hModule);
  virtual VOID OnFreeLibraryCall(__inout CNktDvProcess *lpProc, __in HMODULE hModule);

  virtual VOID OnProcessAdded(__inout CNktDvProcess *lpProc);
  virtual VOID OnProcessRemoved(__inout CNktDvProcess *lpProc);

  virtual VOID OnAgentLoad(__inout CNktDvProcess *lpProc, __in HRESULT hErrorCode);
  virtual VOID OnAgentUnload(__inout CNktDvProcess *lpProc);

  virtual SIZE_T OnCustomMessage(__inout CNktDvProcess *lpProc, __in SIZE_T nMsgCode, __in SIZE_T nMsgParam);

  virtual VOID OnHookOverwritten(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc);

  virtual VOID OnThreadStarted();
  virtual VOID OnThreadEnded();

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

  static BOOL CanUnloadNow();
  static VOID DumpObjects();

private:
  friend class CNktDvHook;
  friend class CNktDvHooksEnumerator;
  class CAgentController;

  virtual VOID Finalize();

  virtual VOID ThreadProc();

  HRESULT HookDestroy(__inout CNktDvHook *lpHook);
  virtual VOID HooksEnumDestroy(__inout CNktDvHooksEnumerator *lpHooksEnum);

  virtual HRESULT HookAddProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                 __in BOOL bSync=FALSE);
  virtual HRESULT HookAddProcess(__inout CNktDvHooksEnumerator *lpHooksEnum,
                                 __inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);

  virtual HRESULT HookRemoveProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                    __in BOOL bSync=FALSE);
  virtual HRESULT HookRemoveProcess(__inout CNktDvHooksEnumerator *lpHooksEnum,
                                    __inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);
  virtual VOID HookRemoveProcessItems(__inout CNktDvProcess *lpProc);

  virtual HRESULT HookEnableProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                    __in BOOL bEnable, __in BOOL bSync=FALSE);
  virtual HRESULT HookEnableProcess(__inout CNktDvHooksEnumerator *lpHooksEnum, __inout CNktDvProcess *lpProc,
                                    __in BOOL bEnable, __in BOOL bSync=FALSE);

  virtual CNktDvHook::eState HookGetProcessState(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc);

  virtual HRESULT HookQueryOverwrite(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                     __inout TNktArrayList<BYTE> &cResult);
  virtual HRESULT HookQueryOverwrite(__inout CNktDvHooksEnumerator *lpHooksEnum, __inout CNktDvProcess *lpProc,
                                     __inout TNktArrayList<BYTE> &cResult);

  virtual HRESULT HookGetLastError(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc);

  virtual HRESULT HookInstall(__inout CNktDvHook *lpHook, __in BOOL bSync=FALSE);
  virtual HRESULT HookInstall(__inout CNktDvHooksEnumerator *lpHooksEnum, __in BOOL bSync=FALSE,
                              __inout_opt CNktDvProcess *lpProc=NULL);

  virtual HRESULT HookUninstall(__inout CNktDvHook *lpHook, __in BOOL bSync=FALSE);
  virtual HRESULT HookUninstall(__inout CNktDvHooksEnumerator *lpHooksEnum, __in BOOL bSync=FALSE,
                                __inout_opt CNktDvProcess *lpProc=NULL);

  virtual VOID HookRemoveBecauseAgentShutdown(__in DWORD dwPid);
  virtual HRESULT HookExecCreateProcessAutoHook(__inout CNktDvProcess *lpProc, __in DWORD dwChildPid);
  virtual HRESULT HookExecLoadLibraryAutoHook(__inout CNktDvProcess *lpProc, __in_z LPCWSTR szDllNameW);
  virtual HRESULT HookCheckIfAddressIsAvailable(__inout CNktDvModulesEnumerator *lpModEnum,
                                                __in LPVOID lpAddress);

  //internal process list callbacks
  virtual VOID IPL_OnProcessAdded(__inout CNktDvProcess *lpProc);
  virtual VOID IPL_OnProcessRemoved(__inout CNktDvProcess *lpProc);
  virtual VOID IPL_WorkerThreadStarted();
  virtual VOID IPL_WorkerThreadEnded();

  //transport engine callbacks
  virtual HRESULT TEC_OnAgentMessage(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                     __in NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                     __in HANDLE hAgentDupProc,
                                     __inout CNktDvTransportBigData *lpConnBigData);
  virtual VOID TEC_OnConnectionClosed(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid);
  virtual VOID TEC_OnTransportError(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                    __in HRESULT hRes);
  virtual VOID TEC_WorkerThreadStarted();
  virtual VOID TEC_WorkerThreadEnded();
  virtual VOID TEC_DispatcherThreadStarted();
  virtual VOID TEC_DispatcherThreadEnded();

  virtual HRESULT ActivateAgent(__inout CNktDvProcess *lpProc);
  virtual HRESULT SendAddHookCommand(__inout CNktDvProcess *lpProc, __inout CNktDvHook *lpHook,
                                     __in DWORD dwHookId, __in DWORD dwBatchId, __in LPVOID lpAddress,
                                     __in BOOL bSync);
  virtual HRESULT SendRemoveHookCommand(__in CNktDvProcess *lpProc, __in DWORD dwHookId,
                                        __in DWORD dwBatchId, __in BOOL bSync);
  virtual HRESULT SendEnableHookCommand(__in CNktDvProcess *lpProc, __in DWORD dwHookId,
                                        __in DWORD dwBatchId, __in BOOL bEnable, __in BOOL bSync);
  virtual HRESULT SendBatchHookExecCommand(__in CNktDvProcess *lpProc, __in DWORD dwBatchId,
                                           __in BOOL bSync);

  virtual HRESULT SendDirectCommand(__in CNktDvProcess *lpProc, __in DWORD dwCmdId, __in NKT_DV_TMSG_COMMON *lpMsg,
                                    __in SIZE_T nMsgSize, __in CNktDvTransportMessageCallbackHelperBase *lpCallBack,
                                    __in BOOL bSync);

  virtual HRESULT OnAgentMsg_LoadDllCallback(__inout CNktDvProcess *lpProc,
                                             __in NKT_DV_TMSG_LOADDLL_CALLBACK *lpMsg);
  virtual HRESULT OnAgentMsg_UnloadDllCallback(__inout CNktDvProcess *lpProc,
                                               __in NKT_DV_TMSG_UNLOADDLL_CALLBACK *lpMsg);
  virtual HRESULT OnAgentMsg_CallCustomApiCallback(__inout CNktDvProcess *lpProc,
                                                   __in NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK *lpMsg);
  virtual HRESULT OnAgentMsg_CreateProcess(__inout CNktDvProcess *lpProc,
                                           __in NKT_DV_TMSG_ONCREATEPROCESS *lpMsg);
  virtual HRESULT OnAgentMsg_LoadLibrary(__in CAgentController *lpAgentCtl, __in NKT_DV_TMSG_ONLOADLIBRARY *lpMsg,
                                         __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnAgentMsg_FreeLibrary(__in CAgentController *lpAgentCtl, __in NKT_DV_TMSG_ONFREELIBRARY *lpMsg);
  virtual HRESULT OnAgentMsg_HookCalled(__inout CNktDvProcess *lpProc, __in NKT_DV_TMSG_ONHOOKCALLED *lpMsg,
                                 __inout CNktDvModulesEnumerator *lpModEnum, __inout CNktDvProcessMemory *lpProcMem,
                                 __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnAgentMsg_HookStateChanged(__inout CNktDvProcess *lpProc, __in NKT_DV_TMSG_HOOKSTATECHANGE *lpMsg);
  virtual HRESULT OnAgentMsg_SuspendX64ThreadFromX86(__inout CNktDvProcess *lpProc,
                                 __in HANDLE hAgentDupProc, __in NKT_DV_TMSG_SUSPENDX64THREADFROMX86 *lpMsg);
  virtual HRESULT OnAgentMsg_CustomMessage(__inout CNktDvProcess *lpProc, __inout CNktDvProcessMemory *lpProcMem,
                                           __in NKT_DV_TMSG_CUSTOMMESSAGE *lpMsg);
  virtual HRESULT OnAgentMsg_HooksOverwritten(__inout CNktDvProcess *lpProc, __in NKT_DV_TMSG_HOOKOVERWRITTEN *lpMsg);
  /*
  virtual HRESULT OnAgentMsg_InitialConnectionMessage(__in CAgentController *lpAgentCtl,
                                                      __inout CNktDvProcessMemory *lpProcMem,
                                                      __in NKT_DV_TMSG_INITIALCONNECTIONMESSAGE *lpMsg);
  */

private:
  class CAgentController : public CNktFastMutex, public CNktMemMgrObj
  {
  public:
    typedef enum {
      stInactive, stActive, stShuttingDown
    } eState;

    CAgentController(__inout CNktDvProcess *lpProc, __inout CNktDvTransportEngine *lpTransport);
    virtual ~CAgentController();

    virtual HRESULT InitializeAgent(__in DWORD dwTimeout, __in LPCWSTR szAgentPathw,
                                    __in LPNKT_DV_AGENTSETTINGOVERRIDE lpSettingsOverride);
    virtual VOID ShutdownAgent();

    virtual eState GetAgentState();

    virtual CNktDvShutdownMutex& GetShutdownMtx()
      {
      return cShutdownMtx;
      };

    virtual CNktDvProcessMemory* GetProcessMemory()
      {
      return cProcMem;
      };

    virtual VOID MarkModulesCacheAsDirty(__in HINSTANCE hDll)
      {
      if (cProc != NULL)
        CNktDvProcess::MarkModulesEnumeratorAsDirty(cProc->GetProcessId(), hDll);
      return;
      };

    virtual HRESULT GetModulesCache(__deref_out CNktDvModulesEnumerator **lplpModEnum);

  private:
    friend class CNktDvSpyMgr;

    virtual VOID InternalShutdownAgent(__in BOOL bWait);
    virtual HRESULT LoadAgentIntoProcess(__in LPCWSTR szAgentPathW,
                                         __in LPNKT_DV_AGENTSETTINGOVERRIDE lpSettingsOverride);
    virtual HRESULT UnloadAgentFromProcess();

    static int AgentControllerCompare(__in void *, __in CAgentController **lpElem1,
                                      __in CAgentController **lpElem2);
    static int AgentControllerCompare2(__in void *, __in LPDWORD lpElem1, __in CAgentController **lpElem2);

  private:
    CNktDvTransportEngine *lpTransport;
    TNktComPtr<CNktDvProcess> cProc;
    TNktComPtr<CNktDvProcessMemory> cProcMem;
    CNktDvShutdownMutex cShutdownMtx;
  };

  virtual SIZE_T FindAgentController(__in DWORD dwPid);
  virtual HRESULT AddAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                     __inout CNktDvProcess *lpProc);
  virtual VOID RemoveAgentController(__inout CNktDvSpyMgr::CAgentController *lpAgentCtl);
  virtual HRESULT GetAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                     __inout CNktDvProcess *lpProc, __in BOOL bCreateIfNotFound,
                                     __out HRESULT *lphLoadResult);
  virtual HRESULT GetAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                     __in DWORD dwPid);

  //--------

  class CAgentControllerShutdownLock
  {
  public:
    CAgentControllerShutdownLock()
      {
      lpAgentCtl = NULL;
      return;
      };

    ~CAgentControllerShutdownLock()
      {
      if (lpAgentCtl != NULL)
        lpAgentCtl->GetShutdownMtx().Unlock();
      return;
      };

    VOID Attach(__inout CAgentController *_lpAgentCtl)
      {
      lpAgentCtl = _lpAgentCtl;
      return;
      };

  private:
    CAgentController *lpAgentCtl;
  };

  //--------

  class CHookItem : public CNktDvObject
  {
  public:
    CHookItem();
    virtual ~CHookItem();

  public:
    TNktComPtr<CNktDvHook> cHook;
    TNktComPtr<CNktDvProcess> cProc;
    DWORD dwHookId;
    CNktDvHook::eState nState;
    LONG volatile nInternalFlags;
    TNktComPtr<CNktDvProcessMemory> cProcMem;
    SIZE_T nProcMemIndexCache;
    CNktDvFunctionParamsCache cFuncParamCache;
    HRESULT hLastError;

  private:
    friend class CNktDvSpyMgr;

    static int HookItemCompareByHookId(__in void*, __in CHookItem**, __in CHookItem**);
    static int HookItemCompareByHookAndProc(__in void*, __in CHookItem**, __in CHookItem**);
    static int HookItemCompareByHookId2(__in void*, __in LPDWORD, __in CHookItem**);
    static int HookItemCompareByHookAndProc2(__in SIZE_T*, __in SIZE_T*, __in CHookItem**);
  };

  virtual HRESULT AddHookItem(__deref_out CHookItem **lplpItem, __inout CNktDvHook *lpHook,
                              __inout CNktDvProcess *lpProc, __in DWORD dwHookId);
  virtual VOID RemoveHookItem(__inout CHookItem *lpItem);
  virtual VOID RemoveHookItem(__in DWORD dwHookId);
  virtual SIZE_T FindHookItemByHookId(__in DWORD dwHookId);
  virtual SIZE_T FindHookItemByHookAndProc(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc);
  virtual DWORD GetNextHookId();
  virtual DWORD GetNextBatchHookId();

  static int HookInstallUninstallCompare(LPVOID, CHookItem**, CHookItem**);

  //--------

  TNktComPtr<CNktDvEngDatabase> cDvDB_32;
#if defined _M_X64
  TNktComPtr<CNktDvEngDatabase> cDvDB_64;
#endif //_M_X64
  TNktComPtr<CNktDvProcessHandlesMgr> cProcHndlMgr;
  CNktDvShutdownMutex cShutdownMtx;
  CNktStringW cStrAgentPathW;
  CNktDvTransportEngine cTransport;
  CNktDvInternalProcessesList cProcList;
  struct {
    LONG volatile nMutex;
    TNktLnkLst<CNktDvHook> cList;
  } sHooks;
  struct {
    LONG volatile nMutex;
    TNktLnkLst<CNktDvHooksEnumerator> cList;
  } sHooksEnum;
  TNktArrayList<CAgentController*> aAgentControllersList;
  TNktArrayList<CHookItem*> aHookItemsListByHookId;
  TNktArrayList<CHookItem*> aHookItemsListByHookAndProc;
  LONG volatile nNextHookId;
  LONG volatile nNextBatchHookId;

  typedef struct {
    TNktComPtr<CNktDvProcess> cProc;
    DWORD dwChildPid;
  } DELAYEDHOOK_DATA;

  struct tagDelayedHook {
    CNktEvent cAvailableEvent;
    TNktArrayListWithDelete<DELAYEDHOOK_DATA*> aItems;
  } sDelayedHook;

  LPVOID volatile lpInitData;
  NKT_DV_AGENTSETTINGOVERRIDE sSettingsOverride;
};

//-----------------------------------------------------------

