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

#include "Agent.h"
#include "..\..\Common\Database.h"
#include "..\..\Common\ShutdownMutex.h"
#include "..\..\Common\FnvHash.h"
#include "..\..\Common\StringLiteW.h"
#include "..\..\Common\WaitableObjects.h"
#include "..\..\Common\LinkedList.h"
#include "..\..\Common\ProcessHandle.h"
#include "TransportAgent.h"
#include "HookEngineInternals.h"
#include "TlsData.h"

//-----------------------------------------------------------

class CDvAgentMgr : public CNktFastMutex, public CNktMemMgrObj, protected CNktDvTransportAgentCallback,
                    protected CNktDvHookEngineCallback
{
public:
  CDvAgentMgr();
  virtual ~CDvAgentMgr();

  HRESULT Initialize(__in LPNKT_DV_AGENTINITDATA lpInitData, __in HANDLE _hShutdownEvent,
                     __in HANDLE _hMainThread);
  VOID Finalize();

  HRESULT StartServer();
  VOID Run();

  BOOL CheckIfInTrampoline(__in SIZE_T nCurrIP);

  CNktHookLib& HookLib()
    {
    return cHookEngine.HookLib();
    };

public:
  HINSTANCE hAgentInst;
  DWORD dwPID;

private:
  class CCreatedProcessesItem : public CNktMemMgrObj, public TNktLnkLstNode<CCreatedProcessesItem>
  {
  public:
    CCreatedProcessesItem() : CNktMemMgrObj(), TNktLnkLstNode<CCreatedProcessesItem>()
      {
      nktMemSet(&sMsg, 0, sizeof(sMsg));
      nSuspendCount = 0;
      hProcess = hThread = hReadyEvent = hContinueEvent = NULL;
      return;
      };

    ~CCreatedProcessesItem()
      {
      if (hThread != NULL)
        ::CloseHandle(hThread);
      if (hProcess != NULL)
        ::CloseHandle(hProcess);
      if (hReadyEvent != NULL)
        ::CloseHandle(hReadyEvent);
      if (hContinueEvent != NULL)
      {
        ::SetEvent(hContinueEvent);
        ::CloseHandle(hContinueEvent);
      }
      return;
      };

  public:
    NKT_DV_TMSG_ONCREATEPROCESS sMsg;
    HANDLE hProcess, hThread;
    SIZE_T nSuspendCount;
    HANDLE hReadyEvent, hContinueEvent;
  };

  virtual VOID TAC_OnConnectionEstablished(__inout CNktDvTransportAgent *lpTransport);
  virtual VOID TAC_OnConnectionClosed(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes);
  virtual HRESULT TAC_OnEngineMessage(__inout CNktDvTransportAgent *lpTransport,
                                      __inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                      __inout CNktDvTransportBigData *lpConnBigData);
  virtual VOID TAC_OnTransportError(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes);

  virtual BOOL HEC_IsSystemThread();
  virtual VOID HEC_OnError(__in HRESULT hRes);
  virtual HRESULT HEC_OnHookCalled(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                           __inout LPBYTE lpIntercallCustomData,
                           __in CNktDvFunctionParamsCache::FUNCTION_PARAMS *lpFuncParams,
                           __in CNktDvParamsEnumerator *lpCustomParamsEnum);
  virtual HRESULT HEC_OnSendCustomMessage(__out SIZE_T *lpnRetVal, __in SIZE_T nMsgCode,
                                          __in SIZE_T nMsgParam, __in BOOL bSync);
  virtual HRESULT HEC_OnThreadIgnore(__in DWORD dwTid, __in BOOL bIgnore);
  virtual HRESULT HEC_OnHookOverwritten(__in LPDWORD aHookIdsList, __in SIZE_T nCount);
  virtual HRESULT HEC_OnInvalidateCache(__in HINSTANCE hDll);

  virtual HRESULT OnInternalCreateProcess(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                          __inout LPBYTE lpIntercallCustomData);
  virtual HRESULT OnInternalLdrLoadDll(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                       __inout LPBYTE lpIntercallCustomData);
  virtual HRESULT OnInternalLdrUnloadDll(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                         __inout LPBYTE lpIntercallCustomData);
  virtual HRESULT OnInternalNtSuspendThread(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                            __inout LPBYTE lpIntercallCustomData);
  virtual HRESULT OnInternalNtResumeThread(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                           __inout LPBYTE lpIntercallCustomData, __in BOOL bAlert);
  virtual HRESULT OnInternalNtMapViewOfSection(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                               __inout LPBYTE lpIntercallCustomData);
  virtual HRESULT OnInternalNtUnmapViewOfSection(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                                 __inout LPBYTE lpIntercallCustomData);

  virtual HRESULT OnUserHook(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                             __in CNktDvFunctionParamsCache::FUNCTION_PARAMS *lpFuncParams,
                             __inout CNktDvParamsEnumerator *lpCustomParamsEnum,
                             __inout LPBYTE lpIntercallCustomData);

  virtual HRESULT OnEngMsg_LoadDll(__inout NKT_DV_TMSG_LOADDLL *lpMsg,
                                   __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnEngMsg_UnloadDll(__inout NKT_DV_TMSG_UNLOADDLL *lpMsg,
                                     __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnEngMsg_CallCustomApi(__inout NKT_DV_TMSG_CALLCUSTOMAPI *lpMsg,
                                         __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnEngMsg_AddHook(__inout NKT_DV_TMSG_ADDHOOK *lpMsg,
                                   __inout CNktDvTransportBigData *lpConnBigData);
  virtual HRESULT OnEngMsg_RemoveHook(__inout NKT_DV_TMSG_REMOVEHOOK *lpMsg);
  virtual HRESULT OnEngMsg_EnableHook(__inout NKT_DV_TMSG_ENABLEHOOK *lpMsg);

  virtual HRESULT OnEngMsg_BatchHookExec(__inout NKT_DV_TMSG_BATCHHOOKEXEC *lpMsg);

  virtual HRESULT OnEngMsg_HelperCall(__inout NKT_DV_TMSG_HELPERCALL *lpMsg);

  virtual HRESULT SendCreateProcessMsg(__in CCreatedProcessesItem *lpCpItem);

  virtual HRESULT CheckOverwrittenHooks();

  virtual HRESULT CheckDllModificationsWithImageFile(__in HINSTANCE hDll, __inout TNktArrayList<BYTE, 16384> &cBuf);

private:
  typedef struct {
    LONG nInternalId;
    LPWSTR szDllNameW;
    BOOL bUnloadOnExit;
    HINSTANCE hDll;
  } USER_DLL;

  class USER_DLL_Remove
  {
  public:
    virtual VOID OnItemFinalize(__inout USER_DLL &item)
      {
      if (item.szDllNameW != NULL)
        nktMemFree(item.szDllNameW);
      if (item.bUnloadOnExit != FALSE && item.hDll != NULL)
        ::FreeLibrary(item.hDll);
      return;
      };
  };

  class CLoadedModule : public CNktMemMgrObj, public TNktLnkLstNode<CLoadedModule>
  {
  public:
    HINSTANCE hInst;
    CNktStringW cStrNameW;
    BOOL bLoaderWasLocked;
    BOOL bUnloading;
  };

  class CBatchHookExecData : public CNktMemMgrObj, public TNktLnkLstNode<CBatchHookExecData>
  {
  public:
    typedef enum {
      typAddHook=1, typRemoveHook, typMarkedForDeletion=-1
    } eType;

    CBatchHookExecData(eType _nType) : CNktMemMgrObj(), TNktLnkLstNode<CBatchHookExecData>()
      {
      nType = _nType;
      nktMemSet(&sHookInfo, 0, sizeof(sHookInfo));
      return;
      };

    virtual ~CBatchHookExecData()
      {
      RemoveNode();
      return;
      };

  public:
    eType nType;
    DWORD dwBatchId;
    //add hook
    CNktDvTransportBigData::CParser cFunctionNameBP;
    CNktDvTransportBigData::CParser cCustomHandlersBP;
    CNktStringW cStrFuncW;
    CNktDvHookEngine::HOOKINFO sHookInfo;
    //remove hook
    DWORD dwHookId;
  };

private:
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  TNktComPtr<CNktDvProcessHandlesMgr> cProcHndlMgr;

  CNktDvShutdownMutex cShutdownMtx;
  HANDLE hShutdownEvent, hServerProcDup, hMainThread;
  ULONG nServerPlatformBits;
  DWORD dwRunThread;
  CNktDvTransportAgent cTransport;
  LONG volatile nTransportActive;
  LONG volatile nLoadedDllsNextInternalId;
  TNktArrayList4Structs<USER_DLL,32,USER_DLL_Remove> aLoadedUserDllsList;
  LONG volatile nSendLoadedUnloadedDllAsync;
  DWORD dwAgentInitFlags;

  CNktDvHookEngine cHookEngine;
  HINSTANCE hNtDll, hKernel32Dll, hKernelBaseDll;

  struct tagLoadedModules {
    CNktFastMutex cLock;
    TNktLnkLst<CLoadedModule> cList;
  } sLoadedModules;

  TNktLnkLst<CCreatedProcessesItem> cCreatedProcessesList;

  struct {
    CNktFastMutex cMtx;
    TNktLnkLst<CBatchHookExecData> cList;
  } sBatchHookExec;

  struct {
    CNktFastMutex cMtx;
    TNktArrayList<DWORD> cList;
  } sThreadIgnore;

  LONG volatile nDelayedCheckOverrides;
};

//-----------------------------------------------------------

