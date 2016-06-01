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
#include "..\..\Common\ExportedFunction.h"
#include "..\..\Common\Process.h"
#include "..\..\Common\TransportBigData.h"
#include "..\..\Common\ParamsEnum.h"
#include "..\..\Common\Param.h"
#include "..\..\Common\FunctionParamsCache.h"
#include "..\..\Common\LinkedList.h"
#include "StackTrace.h"
class CNktDvInternalHooksMgr;
class CNktDvTransportEngine;

//-----------------------------------------------------------

class CNktDvHook : public TNktDvEngBaseObjWithLock<CNktFastMutex>,
                   public TNktLnkLstNode<CNktDvHook>
{
public:
  typedef enum {
    stInactive=0, stActivating, stActive, stDisabled, stRemoved, stNotAvailable,
    stNotFound=-1, stError=-2
  } eState;

  typedef enum {
    flgAutoHookChildProcess=0x0001,
    flgRestrictAutoHookToSameExecutable=0x0002,
    flgAutoHookActive=0x0004,
    flgAsyncCallbacks=0x0008,
    flgOnlyPreCall=0x0010, flgOnlyPostCall=0x0020,
    flgDontCheckAddress=0x0040,
    flgDontCallIfLoaderLocked=0x0080,
    flgDontCallCustomHandlersOnLdrLock=0x0100,
    flgOnly32Bits=0x0200, flgOnly64Bits=0x0400,
    flgAddressIsOffset=0x0800,
    flgInvalidateCache=0x1000,
    flgDontSkipJumps=0x2000,
    flgDisableStackWalk=0x4000
  } eFlags;

  typedef enum {
    flgChDontCallIfLoaderLocked=0x0001,
    flgChDisableExceptionChecks=0x0002
  } eCustomHandlerFlags;

  CNktDvHook();
  virtual ~CNktDvHook();

  virtual VOID Destroy();

  virtual DWORD GetId() const
    {
    return dwHookId;
    };

  virtual HRESULT AddProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);
  virtual HRESULT AddProcess(__in DWORD dwPid, __in BOOL bSync=FALSE);
  virtual HRESULT RemoveProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);
  virtual HRESULT RemoveProcess(__in DWORD dwPid, __in BOOL bSync=FALSE);
  virtual HRESULT EnableProcess(__inout CNktDvProcess *lpProc, __in BOOL bEnable, __in BOOL bSync=FALSE);
  virtual HRESULT EnableProcess(__in DWORD dwPid, __in BOOL bEnable, __in BOOL bSync=FALSE);
  virtual eState GetProcessState(__in CNktDvProcess *lpProc);
  virtual eState GetProcessState(__in DWORD dwPid);
  virtual HRESULT QueryOverwrite(__in CNktDvProcess *lpProc);
  virtual HRESULT QueryOverwrite(__in DWORD dwPid);
  virtual HRESULT GetLastErrorAtProc(__in CNktDvProcess *lpProc);
  virtual HRESULT GetLastErrorAtProc(__in DWORD dwPid);

  virtual HRESULT Hook(__in BOOL bSync=FALSE);
  virtual HRESULT Unhook(__in BOOL bSync=FALSE);

  virtual HRESULT AddCustomHandler(__in_z LPCWSTR szHandlerDllW, __in LONG nFlags,
                                   __in_z LPCWSTR szParametersW);
  virtual VOID RemoveAllHandlers(__in BOOL bInDestructor=FALSE);

  virtual LPWSTR GetFunctionName()
    {
    return cStrFunctionNameW;
    };

  virtual LONG GetFlags()
    {
    return nFlags;
    };

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

private:
  friend class CNktDvSpyMgr;
  friend class CNktDvHooksEnumerator;

  typedef struct {
    ULONG nFlags;
    LPWSTR szParametersW;
    WCHAR szDllNameW[1];
  } CUSTOM_HANDLER_ITEM;

  virtual HRESULT SendCustomHandlerData(__out LPDWORD lpdwBigPacketId, __in DWORD dwPid,
                                        __in CNktDvTransportEngine *lpTransport);

  virtual VOID Unlink();

  DWORD dwHookId;
  CNktStringW cStrFunctionNameW;
  CNktDvTools::FUNCTION_PARSE_INFO sFuncParseInfo;
  TNktArrayListWithFree<CUSTOM_HANDLER_ITEM*> aHandlerDllsList;
  LONG volatile nFlags;
  //----
  CNktDvSpyMgr *lpEngine;
  BOOL bDestroyed;
  CNktDvDbModuleNoRef *lpDbModule32;
  CNktDvDbObjectNoRef *lpDbObject32;
#if defined _M_X64
  CNktDvDbModuleNoRef *lpDbModule64;
  CNktDvDbObjectNoRef *lpDbObject64;
#endif //_M_X64
  SIZE_T nAddressOrOffset;
};

//-----------------------------------------------------------

class CNktDvHookCallInfo : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  CNktDvHookCallInfo();
  ~CNktDvHookCallInfo();

  virtual BOOL IsPreCall() const
    {
    return bPreCall;
    };

  virtual LONG GetCookie() const
    {
    return nCookie;
    };

  virtual HRESULT SetLastError(__in DWORD dwOsLastError);
  virtual DWORD GetLastError() const
    {
    CNktAutoFastMutex cLock(&const_cast<CNktDvHookCallInfo*>(this)-> GetLockObject());

    return dwOsLastError;
    };

  virtual double GetCurrentTimeMs() const
    {
    return nCurrentTimeMs;
    };

  virtual double GetElapsedTimeMs() const
    {
    return nElapsedTimeMs;
    };
  virtual double GetChildsElapsedTimeMs() const
    {
    return nChildsElapsedTimeMs;
    };

  virtual double GetKernelModeTimeMs() const
    {
    return nKernelTimeMs;
    };
  virtual double GetUserModeTimeMs() const
    {
    return nUserTimeMs;
    };
  virtual ULONGLONG GetCpuClockCycles() const
    {
    return nCpuClockCycles;
    };

  virtual HRESULT GetHook(__deref_out CNktDvHook **lplpHook);

  virtual HRESULT GetProcess(__deref_out CNktDvProcess **lplpProc);

  virtual DWORD GetThreadId() const
    {
    return dwThreadId;
    };

  virtual DWORD GetChainDepth() const
    {
    return dwChainDepth;
    };

  virtual HRESULT GetStackTrace(__deref_out CNktDvStackTrace **lplpStackTrace);

  virtual HRESULT GetFunctionParameters(__deref_out CNktDvParamsEnumerator **lplpFuncParams);
  virtual HRESULT GetFunctionResult(__deref_out CNktDvParam **lplpFuncResult);

  virtual HRESULT GetCustomParameters(__deref_out CNktDvParamsEnumerator **lplpFuncCustomParams);

  virtual SIZE_T GetRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister) const;
  virtual HRESULT SetRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister, __in SIZE_T nValue);

  virtual double GetFloatingRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister) const;
  virtual HRESULT SetFloatingRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                           __in double nValue);

  virtual HRESULT SkipCall();

  virtual HRESULT ReadIntercallData(__in LPVOID lpDestAddr, __in SIZE_T nOffset, __in SIZE_T nBytes);
  virtual HRESULT WriteIntercallData(__in LPVOID lpSrcAddr, __in SIZE_T nOffset, __in SIZE_T nBytes);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

private:
  friend class CNktDvSpyMgr;

  virtual VOID Invalidate();

  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  //---
  BOOL bPreCall;
  LONG nCookie;
  DWORD dwOsLastError;
  SIZE_T nStackTrace[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  double nCurrentTimeMs, nElapsedTimeMs, nChildsElapsedTimeMs;
  double nKernelTimeMs, nUserTimeMs;
  ULONGLONG nCpuClockCycles;
  DWORD dwThreadId;
  DWORD dwChainDepth;
  TNktComPtr<CNktDvFunctionParamsCache::FUNCTION_PARAMS> cFuncParams;
  TNktComPtr<CNktDvParamsEnumerator> cCustomParams;
  BOOL bSkipCall;
  BOOL bValid;
  TNktComPtr<CNktDvParam::CCallStateContainer> cCallState;
  ULONGLONG lpRemoteIntercallCustomData;
};

//-----------------------------------------------------------

class CNktDvHooksEnumerator : public TNktDvEnumerator<CNktDvHook>,
                              public TNktLnkLstNode<CNktDvHooksEnumerator>
{
public:
  CNktDvHooksEnumerator();
  virtual ~CNktDvHooksEnumerator();

  virtual HRESULT Add(__inout CNktDvHook *lpObj);

  virtual HRESULT AddProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);
  virtual HRESULT AddProcess(__in DWORD dwPid, __in BOOL bSync=FALSE);
  virtual HRESULT RemoveProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync=FALSE);
  virtual HRESULT RemoveProcess(__in DWORD dwPid, __in BOOL bSync=FALSE);
  virtual HRESULT EnableProcess(__inout CNktDvProcess *lpProc, __in BOOL bEnable, __in BOOL bSync=FALSE);
  virtual HRESULT EnableProcess(__in DWORD dwPid, __in BOOL bEnable, __in BOOL bSync=FALSE);

  virtual HRESULT Hook(__in BOOL bSync=FALSE);
  virtual HRESULT Unhook(__in BOOL bSync=FALSE);

  virtual HRESULT QueryOverwrite(__inout CNktDvProcess *lpProc, __inout TNktArrayList<BYTE> &cResult);
  virtual HRESULT QueryOverwrite(__in DWORD dwPid, __inout TNktArrayList<BYTE> &cResult);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

private:
  friend class CNktDvSpyMgr;

  VOID Unlink();

  CNktDvSpyMgr *lpEngine;
  BOOL bDestroyed;
};

//-----------------------------------------------------------

