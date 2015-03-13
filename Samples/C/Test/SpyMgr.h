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

#ifndef _SPYMGR_H
#define _SPYMGR_H

//-----------------------------------------------------------

#include "EngineTest.h"

#if defined _M_IX86
  #import "../../../bin/DeviareCOM.dll" raw_interfaces_only, named_guids, raw_dispinterfaces, auto_rename
#elif defined _M_X64
  #import "../../../bin/DeviareCOM64.dll" raw_interfaces_only, named_guids, raw_dispinterfaces, auto_rename
#else
  #error Unsupported platform
#endif

//-----------------------------------------------------------

#if defined _M_IX86
  #define my_ssize_t long
#elif defined _M_X64
  #define my_ssize_t __int64
#endif

//-----------------------------------------------------------

class CSpyMgr
{
public:
  CSpyMgr();
  ~CSpyMgr();

  HRESULT Initialize();
  VOID Finalize();

  Deviare2::INktSpyMgr* Get()
    {
    return cSpyMgr;
    };

  virtual HRESULT OnCustomDllLoad(__in Deviare2::INktProcess *proc, __in long actionId,
                                  __in long actionResult);
  virtual HRESULT OnCustomDllUnload(__in Deviare2::INktProcess *proc, __in long actionId,
                                    __in long actionResult);
  virtual HRESULT OnCustomApiCall(__in Deviare2::INktProcess *proc, __in long actionId,
                                  __in long actionResult, __in long apiResult);
  virtual HRESULT OnHookStateChanged(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                     __in Deviare2::eNktHookState newState,
                                     __in Deviare2::eNktHookState oldState);
  virtual HRESULT OnFunctionCalled(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                   __in Deviare2::INktHookCallInfo *callInfo);
  virtual HRESULT OnCreateProcessCall(__in Deviare2::INktProcess *proc, __in long childPid,
                                      __in long mainThreadId,
                                      __in VARIANT_BOOL is64BitProcess, __in VARIANT_BOOL canHookNow);
  virtual HRESULT OnLoadLibraryCall(__in Deviare2::INktProcess *proc, __in BSTR dllName,
                                    __in VARIANT moduleHandle);
  virtual HRESULT OnFreeLibraryCall(__in Deviare2::INktProcess *proc, __in VARIANT moduleHandle);
  virtual HRESULT OnProcessStarted(__in Deviare2::INktProcess *proc);
  virtual HRESULT OnProcessTerminated(__in Deviare2::INktProcess *proc);
  virtual HRESULT OnAgentLoad(__in Deviare2::INktProcess *proc, __in long errorCode);
  virtual HRESULT OnAgentUnload(__in Deviare2::INktProcess *proc);

protected:
  HINSTANCE hDeviareComDll;
  HRESULT hResCoInit;
  CComPtr<Deviare2::INktSpyMgr> cSpyMgr;
  LPVOID lpSpyMgrEvents;
};

//-----------------------------------------------------------

#endif //_SPYMGR_H
