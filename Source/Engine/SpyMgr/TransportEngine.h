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
#include "..\..\Common\ShutdownMutex.h"
#include "..\..\Common\Threads.h"
#include "..\..\Common\LinkedList.h"
class CNktDvTransportEngineCallback;

//-----------------------------------------------------------

#define NKT_DV_TRANSPORT_MAX_LISTENING_CONNECTIONS        12
#define NKT_DV_MAX_FREE_BUFFERS_PER_CONNECTION            25

//-----------------------------------------------------------

class CNktDvTransportEngine : private CNktFastMutex, public CNktMemMgrObj
{
public:
  CNktDvTransportEngine(__in CNktDvTransportEngineCallback *_lpCallback);
  virtual ~CNktDvTransportEngine();

  virtual HRESULT Initialize(__in SIZE_T nWorkerThreadsCount,
                             __in SIZE_T _nDispatcherThreadsCount);
  virtual VOID Finalize();

  virtual HRESULT ConnectAgent(__in DWORD dwPid, __in DWORD dwTimeout);
  virtual VOID DisconnectAgent(__in DWORD dwPid);

  virtual HRESULT SendMsg(__in DWORD dwPid, __inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize);
  virtual HRESULT SendBigPacketMsg(__out LPDWORD lpdwBigPacketId, __in DWORD dwPid, __in LPCVOID lpData,
                                   __in SIZE_T nDataSize);
  virtual DWORD GetNextMsgId();

private:
  class CConnection : public CNktDvObject, protected CNktFastMutex,
                      private TNktLnkLstNode<CConnection>
  {
  public:
    CConnection(__in CNktDvTransportEngine *lpTransport, __in DWORD dwPid);
    virtual ~CConnection();

    virtual DWORD GetPid()
      {
      return dwPid;
      };

  private:
    virtual HRESULT Initialize(__inout HANDLE &hIOCP, __in DWORD dwTimeout);
    virtual VOID Shutdown();
    virtual CNktDvTransportOverlapped* GetBuffer();
    virtual VOID FreeBuffer(__inout CNktDvTransportOverlapped *lpOvr);
    virtual VOID DiscardAllBuffers();
    virtual VOID AddBufferToRwList(__inout CNktDvTransportOverlapped *lpOvr);
    virtual VOID AddBufferInSortedReadedQueue(__inout CNktDvTransportOverlapped *lpOvr);
    virtual VOID AddBufferInSortedToWriteQueue(__inout CNktDvTransportOverlapped *lpOvr);
    virtual CNktDvTransportOverlapped* GetBufferFromSortedReadedQueue(__in LONG volatile nOrder);
    virtual CNktDvTransportOverlapped* GetBufferFromSortedToWriteQueue(__in LONG volatile nOrder);
    virtual HRESULT SendReadPacket();
    virtual VOID FillSecurity(__out SECURITY_ATTRIBUTES &sSecAttrib, __out SECURITY_DESCRIPTOR &sSecDesc);

  private:
    friend class CNktDvTransportEngine;

    DWORD dwPid;
    CNktDvTransportEngine *lpTransport;
    HANDLE hIocpCopy;
    LONG volatile nRefCount;
    //----
    HANDLE hPipe;
    HANDLE hAgentProc;
    CNktFastMutex cBufferListMtx;
    TNktLnkLst<CNktDvTransportOverlapped> cFreeBufferList;
    TNktLnkLst<CNktDvTransportOverlapped> cActiveRwBufferList;
    TNktLnkLst<CNktDvTransportOverlapped> cReadedQueueBufferList;
    TNktLnkLst<CNktDvTransportOverlapped> cToWriteQueueBufferList;
    LONG volatile nFreeBufferListCount;
    LONG volatile nInUseMessageCount;
    SIZE_T nState;
    LONG volatile nNextReadOrder;
    LONG volatile nNextReadOrderToProcess;
    LONG volatile nNextWriteOrder;
    LONG volatile nNextWriteOrderToProcess;
    TNktComPtr<CNktDvTransportBigData> cBigData;
  };

  class CDispatcher : public CNktDvObject, public CNktFastMutex, public TNktLnkLstNode<CDispatcher>
  {
  public:
    CDispatcher();
    ~CDispatcher();

    HRESULT AddOverlapped(__inout CNktDvTransportOverlapped *lpOvr, __in BOOL bInFront);
    CNktDvTransportOverlapped *PeekNextOverlapped();
    VOID RemoveConnection(__in CConnection *lpConn);

  private:
    friend class CNktDvTransportEngine;

    TNktClassWorkerThread<CNktDvTransportEngine> cThread;
    TNktLnkLst<CNktDvTransportOverlapped> cQueueBufferList;
    HANDLE hEvent;
    LONG volatile nLastWorkTick;
    LONG volatile nIsWorking;
  };

  class CConnectionAutoRef
  {
  public:
    CConnectionAutoRef()
      {
      lpConn = NULL;
      return;
      };

    CConnectionAutoRef(__inout CConnection *_lpConn, __in BOOL bAddRef=TRUE)
      {
      lpConn = _lpConn;
      if (lpConn != NULL && bAddRef != FALSE)
        lpConn->AddRef();
      return;
      };

    virtual ~CConnectionAutoRef()
      {
      if (lpConn != NULL)
        lpConn->Release();
      return;
      };

    virtual VOID Attach(__inout CConnection *_lpConn, __in BOOL bAddRef=TRUE)
      {
      NKT_ASSERT(lpConn == NULL);
      lpConn = _lpConn;
      if (lpConn != NULL && bAddRef != FALSE)
        lpConn->AddRef();
      return;
      };

  private:
    CConnection *lpConn;
  };

private:
  virtual VOID InternalDisconnectAgent(__in DWORD dwPid);
  virtual VOID InternalDisconnectAgent2(__inout CConnection *lpConn, __in BOOL bCallCallback);
  virtual CConnection* FindConnectionAndAddRef(__in DWORD dwPid);
  virtual VOID WorkerThreadProc(__in SIZE_T nIndex);
  virtual VOID DispatcherThreadProc(__in SIZE_T nDispatcherPtr);
  virtual HRESULT DispatchReadMessage(__inout CConnection *lpConn,
                                        __inout CNktDvTransportOverlapped *lpOvr);
  virtual HRESULT SignalProcessedEvent(__inout CNktDvTransportOverlapped *lpOvr);

private:
  CNktDvShutdownMutex cShutdownMtx;
  HANDLE hIOCP;
  DWORD dwMePid;
  SIZE_T nWorkerThreadsCount;
  TNktClassWorkerThread<CNktDvTransportEngine> *lpWorkerThreads;
  TNktLnkLst<CConnection> cConnList;
  CNktDvTransportEngineCallback *lpCallback;
  LONG volatile nMsgId;
  SIZE_T nDispatchersCount;
  TNktArrayListWithRelease<CDispatcher*> cFixedDispatchersList;
  struct {
    CNktFastMutex cMtx;
    TNktLnkLst<CDispatcher> cList;
  } sOneShotDispatchers;
};

//-----------------------------------------------------------

class CNktDvTransportEngineCallback
{
public:
  friend class CNktDvTransportEngine;

  virtual HRESULT TEC_OnAgentMessage(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                     __in NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                     __in HANDLE hAgentDupProc,
                                     __inout CNktDvTransportBigData *lpConnBigData)=0;
  virtual VOID TEC_OnConnectionClosed(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid)=0;
  virtual VOID TEC_OnTransportError(__inout CNktDvTransportEngine *lpTransport, __in DWORD dwPid,
                                    __in HRESULT hRes)=0;
  virtual VOID TEC_WorkerThreadStarted()=0;
  virtual VOID TEC_WorkerThreadEnded()=0;
  virtual VOID TEC_DispatcherThreadStarted()=0;
  virtual VOID TEC_DispatcherThreadEnded()=0;
};

//-----------------------------------------------------------

