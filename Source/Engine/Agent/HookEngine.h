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

#ifndef _NKT_DV_HOOKENGINE_H
#define _NKT_DV_HOOKENGINE_H

#include "Agent.h"
#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\TransportCommon.h"
#include "..\..\Common\ProcessMemory.h"
#include "..\..\Common\FunctionParamsCache.h"
#include "TlsData.h"
#include "HookEngineCallDataEntry.h"
#include "HookEngineInternals.h"
#include "HookCustomHandlerMgr.h"
class CHookEntry;
class CHookEngJumpBlockEntry;
class CNktDvHookEngineCallback;
class CNktDvHookEngineAutoTlsLock;

//-----------------------------------------------------------

class CNktDvHookEngine : private CNktFastMutex, public CNktMemMgrObj
{
public:
  typedef enum {
    phPreCall=1, phPostCall
  } ePhase;

  typedef struct tagHOOKINFO {
    DWORD dwHookId, dwSpyMgrHookId;
    LPVOID lpProcToHook;
    LPCWSTR szFunctionNameW;
    CNktDvDbObjectNoRef *lpDbFunc;
    ULONG nFlags; //same as NKT_DV_TMSG_ADDHOOK_FLAG_...
    struct tagCustomHandlers {
      LPBYTE lpData;
      SIZE_T nDataSize;
    } sCustomHandlers;
  } HOOKINFO;

  typedef struct tagCALLINFO {
    DWORD dwHookId;
    ePhase nPhase;
    LONG nCookie;
    CNktDvDbObjectNoRef *lpDbFunc;
    BOOL bAsyncCallbacks;
#if defined _M_IX86
    LPNKT_DV_CURRENT_CALL_STATE32 lpCallState;
#elif defined _M_X64
    LPNKT_DV_CURRENT_CALL_STATE64 lpCallState;
#endif //_M_X64
    DWORD dwThreadId;
    double nCurrentTimeMs;
    double nElapsedTimeMs, nChildsElapsedTimeMs;
    double nKernelTimeMs, nUserTimeMs;
    ULONGLONG nCpuClockCycles;
    DWORD dwChainDepth;
  } CALLINFO, *LPCALLINFO;

  CNktDvHookEngine(__in CNktDvHookEngineCallback *lpCallback);
  virtual ~CNktDvHookEngine();

  virtual HRESULT Initialize();
  virtual VOID Finalize();

  virtual HRESULT Hook(__in HOOKINFO aHookInfo[], __in SIZE_T nCount);
  virtual HRESULT Unhook(__in LPDWORD lpdwHookIds, __in SIZE_T nCount);
  virtual VOID DllUnloadUnhook(__in HINSTANCE hDll);
  virtual VOID UnhookAll();
  virtual HRESULT EnableHook(__in DWORD dwHookId, __in BOOL bEnable);

  virtual BOOL CheckIfInTrampoline(__in SIZE_T nCurrIP);

  virtual HRESULT CheckOverwrittenHooks();
  virtual HRESULT QueryOverwrittenHooks(__in SIZE_T nCount, __in LPDWORD lpdwHookIds, __out LPBYTE lpnResults);

#if defined _M_IX86
  static VOID GetStackTrace(__out SIZE_T *lpnOutput, __in SIZE_T nCount, __in SIZE_T nSkipCount, __in ULONG Esp,
                            __in ULONG Ebp, __in SIZE_T nStackLimitLow, __in SIZE_T nStackLimitHigh,
                            __inout CNktDvTlsData *lpTlsData);
#elif defined _M_X64
  static VOID GetStackTrace(__out SIZE_T *lpnOutput, __in SIZE_T nCount, __in SIZE_T nSkipCount,
                            __in ULONGLONG Rip, __in ULONGLONG Rsp, __in SIZE_T nStackLimitLow,
                            __in SIZE_T nStackLimitHigh, __inout CNktDvTlsData *lpTlsData);
#endif
  static VOID GetStackLimits(__in HANDLE hThread, __out SIZE_T &nLow, __out SIZE_T &nHigh);

private:
  friend static SIZE_T __stdcall PreCallCommon(__inout CNktDvHookEngine*, __in LPVOID, __in SIZE_T);
  friend static SIZE_T __stdcall PostCallCommon(__inout CNktDvHookEngine*, __in LPVOID, __in SIZE_T);

  virtual LONG GetNextCallCookie();
  virtual SIZE_T PreCall(__in LPVOID lpHookEntry, __inout CNktDvTlsData *lpTlsData, __in SIZE_T nStackPointer,
                         __inout DWORD &dwOsLastError);
  virtual SIZE_T PostCall(__in LPVOID lpHookEntry, __inout CNktDvTlsData *lpTlsData, __in SIZE_T nStackPointer,
                          __inout DWORD &dwOsLastError);

  virtual LPBYTE AllocInjectedCode(__in LPVOID lpRefAddr);
  virtual VOID FreeInjectedCode(__in CHookEntry *lpHookEntry);

private:
  friend SIZE_T __stdcall PreCallCommon(__in CNktDvHookEngine*, __in LPVOID, __in SIZE_T);
  friend SIZE_T __stdcall PostCallCommon(__in CNktDvHookEngine*, __in LPVOID, __in SIZE_T);

  typedef struct {
    LPBYTE lpBaseAddress;
    LPBYTE lpFirstFreeItem;
    SIZE_T nFreeItemsCount;
  } INJCODE_BLOCK;

  SIZE_T nInjectedCodeMaxSize;
  TNktArrayList4Structs<INJCODE_BLOCK> cInjectedCodeBlocks;

  CNktDvHookEngineCallback *lpCallback;
  TNktLnkLst<CHookEntry> cHooksList;
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  //----
  LARGE_INTEGER liTimerFreq;
  LONG volatile nCallCounter;
  //----
  TNktComPtr<CHookCustomHandlerMgr> cCustomHandlersMgr;
};

//-----------------------------------------------------------

class CNktDvHookEngineCallback
{
public:
  friend class CNktDvHookEngine;

  virtual BOOL HEC_IsSystemThread()=0;
  virtual VOID HEC_OnError(__in HRESULT hRes)=0;
  virtual HRESULT HEC_OnHookCalled(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                   __inout LPBYTE lpIntercallCustomData,
                                   __in CNktDvFunctionParamsCache::FUNCTION_PARAMS *lpFuncParams,
                                   __in CNktDvParamsEnumerator *lpCustomParamsEnum)=0;
  virtual HRESULT HEC_OnSendCustomMessage(__out SIZE_T *lpnRetVal, __in SIZE_T nMsgCode,
                                          __in SIZE_T nMsgParam, __in BOOL bSync)=0;
  virtual HRESULT HEC_OnThreadIgnore(__in DWORD dwTid, __in BOOL bIgnore)=0;
  virtual HRESULT HEC_OnHookOverwritten(__in LPDWORD aHookIdsList, __in SIZE_T nCount)=0;
  virtual HRESULT HEC_OnInvalidateCache(__in HINSTANCE hDll)=0;
};

//-----------------------------------------------------------

class CNktDvHookEngineAutoTlsLock
{
public:
  CNktDvHookEngineAutoTlsLock()
    {
    bAlreadyLocked = TRUE; //avoid recursion
    hRes = nktDvTlsData_Get(&lpTlsData);
    if (SUCCEEDED(hRes) && lpTlsData->bInCallLock == FALSE)
    {
      bAlreadyLocked = FALSE;
      lpTlsData->bInCallLock = TRUE;
    }
    return;
    };

  virtual ~CNktDvHookEngineAutoTlsLock()
    {
    if (lpTlsData != NULL)
    {
      if (bAlreadyLocked == FALSE)
        lpTlsData->bInCallLock = FALSE;
      lpTlsData->Release();
    }
    return;
    };

  virtual CNktDvTlsData* GetData()
    {
    return lpTlsData;
    };

  virtual BOOL IsAlreadyLocked() const
    {
    return bAlreadyLocked;
    };

  virtual HRESULT GetError() const
    {
    return hRes;
    };

private:
  CNktDvTlsData *lpTlsData;
  HRESULT hRes;
  BOOL bAlreadyLocked;
};

//-----------------------------------------------------------

HRESULT nktDvHookEng_MiniHookInstall(__in LPVOID lpAddress2Hook, __in LPVOID lpNewFunction,
                                     __out LPVOID *lplpCallOriginal, __out LONG volatile **lplpnDisableHook);

//-----------------------------------------------------------

#endif //_NKT_DV_HOOKENGINE_H
