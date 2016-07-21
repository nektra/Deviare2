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

#include "HookEngine.h"
#include "..\..\Common\Debug.h"
#include "..\..\Common\Tools.h"
#include "..\..\Common\FunctionParamsCache.h"
class CHookEngCallDataEntry;
class CHookCustomHandlerMgr;
//class CHookCustomHandlerMgr::CPluginInstance;

//-----------------------------------------------------------

#define IDX_UNINSTALL_DISABLE_FLAGS                        0
#define IDX_HOOK_ENGINE_PTR                                1
#define IDX_HOOK_ENTRY_PTR                                 2
#define IDX_PRECALL_ADDR                                   3
#define IDX_POSTCALL_ADDR                                  4
#define IDX_ORIGINAL_STUB_AND_JUMP                         5

//-----------------------------------------------------------

#define HOOKENG_MAX_ORIGINAL_STUB_SIZE                    64
#define HOOKENG_MAX_STUB_SIZE                             64
#define HOOKENG_JUMP_TO_HOOK_SIZE                          5

#define HOOKENG_FLAG_AsyncCallbacks                   0x0001
#define HOOKENG_FLAG_CallPreCall                      0x0002
#define HOOKENG_FLAG_CallPostCall                     0x0004
#define HOOKENG_FLAG_DontCallOnLdrLock                0x0008
#define HOOKENG_FLAG_DontCallCustomHandlersOnLdrLock  0x0010
#define HOOKENG_FLAG_InvalidateCache                  0x0020
#define HOOKENG_FLAG_DisableStackWalk                 0x0040

//-----------------------------------------------------------

class CHookEntry : public CNktDvObject, public TNktLnkLstNode<CHookEntry>, public CNktFastMutex
{
public:
  CHookEntry();
  virtual ~CHookEntry();

  HRESULT AttachCustomHandler(__in LPCWSTR szDllNameW, __in SIZE_T nFlags, __in LPCWSTR szParametersW,
                              __in CNktDvHookEngineCallback *lpHookEngCallback);

  HRESULT ProcessCustomHandler(__deref_out CNktDvParamsEnumerator **lplpCustomParamsEnum,
                               __inout LPVOID lpCallInfo, //CNktDvHookEngine::CALLINFO*
                               __inout CNktDvParamsEnumerator *lpParamsEnum,
                               __inout CNktDvParam *lpResult, __inout LPBYTE lpIntercallCustomData,
                               __in CNktDvHookEngineCallback *lpHookEngCallback);
  VOID DetachAllCustomHandlers();

public:
  DWORD dwId, dwSpyMgrId;
  CNktStringW cStrFunctionNameW;
  CNktDvDbObjectNoRef *lpDbFunc;
  SIZE_T nStackReturnSize;
  ULONG nFlags;
  HINSTANCE hProcDll;
  struct {
    TNktComPtr<IDispatch> cDispatch;
    DWORD dwCookie;
  } sHookInfo;
  //----
  LONG volatile nCallCounter;
  LONG volatile nUsageCount;
  //----
  CNktHookLib::HOOK_INFO sHookLibInfo;
  //----
  LPVOID lpOrigProc;
  LPBYTE lpSuperHookStub;
  LPVOID lpAfterCallMark;
  BOOL bChangedInformed;
  BYTE aModifiedEntrypointCode[5];
  //----
  CNktDvFunctionParamsCache cFuncParamCache;
  LONG volatile nInstalledCode; //0-not installed / 1-installed / 2-marked for deletion /
                                //3-uninstall but don't free
  //----
  TNktComPtr<CHookCustomHandlerMgr> cCustomHandlersMgr;
  CHookCustomHandlerMgr::CPluginInstanceList aPluginInstancesList;
};
