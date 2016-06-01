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

//-----------------------------------------------------------

// {0B01D6C5-A6D1-4e52-B4D0-87368E780AC3}
static const IID sInternalIid[2] = {
  { 0xB01D6C5, 0xA6D1, 0x4E52, { 0xB4, 0xD0, 0x87, 0x36, 0x8E, 0x78, 0x0A, 0xC3 } },
  { 0xB01D6C5, 0xA6D1, 0x4E52, { 0xB4, 0xD0, 0x87, 0x36, 0x8E, 0x78, 0x0A, 0xC4 } }
};

//-----------------------------------------------------------

typedef HRESULT (__stdcall *lpfn_DV_AGENTCOM_HOOKCALLINFO_SendCustomMessage)(__in LPVOID lpInternal,
                                                          __out SIZE_T *lpnRetVal, __in SIZE_T nMsgCode,
                                                          __in SIZE_T nMsgParam, __in BOOL bSync);
typedef HRESULT (__stdcall *lpfn_DV_AGENTCOM_HOOKCALLINFO_ThreadIgnore)(__in LPVOID lpInternal, __in DWORD dwTid,
                                                                        __in BOOL bIgnore);
typedef HRESULT (__stdcall *lpfn_DV_AGENTCOM_HOOKCALLINFO_InvalidateCache)(__in LPVOID lpInternal, __in SIZE_T hDll);

typedef struct {
  DWORD dwHookId;
  BOOL bIsPreCall;
  DWORD dwCookie[2];
#if defined _M_IX86
  LPNKT_DV_CURRENT_CALL_STATE32 lpCallState;
#elif defined _M_X64
  LPNKT_DV_CURRENT_CALL_STATE64 lpCallState;
#endif //_M_X64
  DWORD dwThreadId;
  double nCurrentTimeMs, nElapsedTimeMs, nChildsElapsedTimeMs;
  double nKernelTimeMs, nUserTimeMs;
  ULONGLONG nCpuClockCycles;
  DWORD dwChainDepth;
} DV_AGENTCOM_HOOKCALLINFOPLUGIN_CALLINFO;

typedef struct {
  lpfn_DV_AGENTCOM_HOOKCALLINFO_SendCustomMessage fnSendCustomMessage;
  lpfn_DV_AGENTCOM_HOOKCALLINFO_ThreadIgnore fnThreadIgnore;
  lpfn_DV_AGENTCOM_HOOKCALLINFO_InvalidateCache fnInvalidateCache;
  LPVOID lpInternal;
} DV_AGENTCOM_HOOKCALLINFO_CALLTRAMPOLINE;

typedef struct {
  CNktDvParamsEnumerator *lpCustomParamsEnum;
  DV_AGENTCOM_HOOKCALLINFOPLUGIN_CALLINFO sCallInfo;
  CNktDvProcess *lpProc;
  CNktDvParamsEnumerator *lpParamsEnum;
  CNktDvParam *lpResult;
  LPBYTE lpIntercallCustomData;
  IDispatch *lpHookInfo;
  LONG volatile *lpnFilterSpyMgrEvent;
} DV_AGENTCOM_HOOKCALLINFOPLUGIN_CREATE_DATA;

typedef struct {
  DWORD dwId;
  SIZE_T nAddress;
  CNktDvDbObjectNoRef *lpDbFunc;
  LPWSTR szFunctionNameW;
  DV_AGENTCOM_HOOKCALLINFO_CALLTRAMPOLINE sCallTrampoline;
  CNktDvProcess *lpProc;
  DWORD dwCookie[2];
} DV_AGENTCOM_HOOKINFO_CREATE_DATA;

//-----------------------------------------------------------

