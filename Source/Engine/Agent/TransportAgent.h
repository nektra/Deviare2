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

#include "..\..\Common\TransportCommon.h"
#include "..\..\Common\TransportBigData.h"
#include "..\..\Common\Threads.h"
class CNktDvTransportAgentCallback;

//-----------------------------------------------------------

class CNktDvTransportAgent : private CNktFastMutex, public CNktMemMgrObj
{
public:
  CNktDvTransportAgent();
  virtual ~CNktDvTransportAgent();

  virtual HRESULT Initialize(__in DWORD dwEnginePid, __in HANDLE hServerProcDup,
                             __in SIZE_T _nWorkerThreadsCount,
                             __in CNktDvTransportAgentCallback *lpCallback);
  virtual VOID Finalize();

  virtual HRESULT StartServer();

  virtual HRESULT SendMsg(__inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize);
  virtual HRESULT SendBigPacketMsg(__out LPDWORD lpdwBigPacketId, __in LPCVOID lpData, __in SIZE_T nDataSize,
                                   __in DWORD dwTid);
  virtual DWORD GetNextMsgId();

  virtual BOOL IsTransportThread(__in DWORD dwTid);

private:
  virtual VOID WorkerThreadProc(__in SIZE_T nIndex);
  virtual CNktDvTransportOverlapped* GetBuffer();
  virtual VOID FreeBuffer(__inout CNktDvTransportOverlapped *lpOvr);
  virtual VOID DiscardAllBuffers();
  virtual VOID AddBufferToRwList(__inout CNktDvTransportOverlapped *lpOvr);
  virtual VOID AddBufferInSortedReadedQueue(__inout CNktDvTransportOverlapped *lpOvr);
  virtual VOID AddBufferInSortedToWriteQueue(__inout CNktDvTransportOverlapped *lpOvr);
  virtual CNktDvTransportOverlapped* GetBufferFromSortedReadedQueue(__in LONG volatile nOrder);
  virtual CNktDvTransportOverlapped* GetBufferFromSortedToWriteQueue(__in LONG volatile nOrder);
  virtual HRESULT SendConnectPacket();
  virtual HRESULT SendReadPacket();
  virtual VOID FillSecurity(__out SECURITY_ATTRIBUTES &sSecAttrib, __out SECURITY_DESCRIPTOR &sSecDesc);
  virtual HRESULT SignalProcessedEvent(__inout CNktDvTransportOverlapped *lpOvr);
  virtual VOID InitializeWorkerThreadCOM(__inout HRESULT &hCoInitRes, __inout NKT_DV_TMSG_COMMON *lpMsg,
                                         __in SIZE_T nMsgSize);

private:
  HANDLE hIOCP;
  HANDLE hPipe;
  CNktEvent cShutdownEvent;
  CNktFastMutex cBufferListMtx;
  TNktLnkLst<CNktDvTransportOverlapped> cFreeBufferList;
  TNktLnkLst<CNktDvTransportOverlapped> cActiveRwBufferList;
  TNktLnkLst<CNktDvTransportOverlapped> cReadedQueueBufferList;
  TNktLnkLst<CNktDvTransportOverlapped> cToWriteQueueBufferList;
  LONG volatile nFreeBufferListCount;
  LONG volatile nInUseMessageCount;
  DWORD dwEnginePid, dwMePid;
  HANDLE hEngineProc;
  CNktDvTransportAgentCallback *lpCallback;
  LONG volatile nMsgId;
  LONG volatile nNextReadOrder;
  LONG volatile nNextReadOrderToProcess;
  LONG volatile nNextWriteOrder;
  LONG volatile nNextWriteOrderToProcess;
  LONG volatile nThreadsEnding;
  SIZE_T nWorkerThreadsCount;
  TNktClassWorkerThread<CNktDvTransportAgent> *lpWorkerThreads;
  TNktComPtr<CNktDvTransportBigData> cBigData;
};

//-----------------------------------------------------------

class CNktDvTransportAgentCallback
{
public:
  friend class CNktDvTransportAgent;

  virtual VOID TAC_OnConnectionEstablished(__inout CNktDvTransportAgent *lpTransport)=0;
  virtual VOID TAC_OnConnectionClosed(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes)=0;
  virtual HRESULT TAC_OnEngineMessage(__inout CNktDvTransportAgent *lpTransport,
                                      __inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                      __inout CNktDvTransportBigData *lpConnBigData)=0;
  virtual VOID TAC_OnTransportError(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes)=0;
};

//-----------------------------------------------------------

