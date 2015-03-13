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

#ifndef _MYSPYMGR_H
#define _MYSPYMGR_H

//-----------------------------------------------------------

#include "EngineTest.h"
#include "SpyMgr.h"

//-----------------------------------------------------------

class CMySpyMgr : public CSpyMgr
{
public:
  CMySpyMgr();
  ~CMySpyMgr();

  HRESULT OnCustomDllLoad(__in Deviare2::INktProcess *proc, __in long actionId, __in long actionResult);
  HRESULT OnCustomDllUnload(__in Deviare2::INktProcess *proc, __in long actionId,
                            __in long actionResult);
  HRESULT OnCustomApiCall(__in Deviare2::INktProcess *proc, __in long actionId, __in long actionResult,
                          __in long apiResult);
  HRESULT OnHookStateChanged(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                             __in Deviare2::eNktHookState newState,
                             __in Deviare2::eNktHookState oldState);
  HRESULT OnFunctionCalled(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                           __in Deviare2::INktHookCallInfo *callInfo);
  HRESULT OnCreateProcessCall(__in Deviare2::INktProcess *proc, __in long childPid, __in long mainThreadId,
                              __in VARIANT_BOOL is64BitProcess, __in VARIANT_BOOL canHookNow);
  HRESULT OnLoadLibraryCall(__in Deviare2::INktProcess *proc, __in BSTR dllName,
                            __in VARIANT moduleHandle);
  HRESULT OnFreeLibraryCall(__in Deviare2::INktProcess *proc, __in VARIANT moduleHandle);
  HRESULT OnProcessStarted(__in Deviare2::INktProcess *proc);
  HRESULT OnProcessTerminated(__in Deviare2::INktProcess *proc);
  HRESULT OnAgentLoad(__in Deviare2::INktProcess *proc, __in long errorCode);
  HRESULT OnAgentUnload(__in Deviare2::INktProcess *proc);

private:
  VOID PrintParamDetails(__in Deviare2::INktParam *lpParam, __in SIZE_T nCurrTabSize, __in SIZE_T nDepth,
                         __in long nPlatformBits);
  VOID PrintSpaces(__in SIZE_T nCount);
};

//-----------------------------------------------------------

#endif //_MYSPYMGR_H
