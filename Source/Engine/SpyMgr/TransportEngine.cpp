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

#include "TransportEngine.h"
#include "..\..\Common\ProcessHandle.h"
#include "..\..\Common\FnvHash.h"
#include "..\..\Common\RegistrySettings.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define READ_AHEAD                                         8

#define XSTATE_Disconnected                                0
#define XSTATE_Connected                                   1
#define XSTATE_Destroying                                  2

#define XTYPE_Free                                         0
#define XTYPE_Read                                         1
#define XTYPE_WriteRequest                                 2
#define XTYPE_Write                                        3
#define XTYPE_Discard                                      4

#define MAX_TEMP_DISPATCHER_SLEEPTIME                   5000

//-----------------------------------------------------------

CNktDvTransportEngine::CNktDvTransportEngine(__in CNktDvTransportEngineCallback *_lpCallback) :
                                 CNktFastMutex(), CNktMemMgrObj()
{
  NKT_ASSERT(_lpCallback != NULL);
  lpCallback = _lpCallback;
  hIOCP = NULL;
  dwMePid = ::GetCurrentProcessId();
  NktInterlockedExchange(&nMsgId, 0);
  lpWorkerThreads = NULL;
  nWorkerThreadsCount = nDispatchersCount = 0;
  return;
}

CNktDvTransportEngine::~CNktDvTransportEngine()
{
  Finalize();
  return;
}

HRESULT CNktDvTransportEngine::Initialize(__in SIZE_T _nWorkerThreadsCount, __in SIZE_T _nDispatcherThreadsCount)
{
  CNktAutoFastMutex cLock(this);
  CDispatcher *lpDisp;
  CHAR szBufA[128];
  HRESULT hRes;
  SIZE_T i;

  if (_nWorkerThreadsCount < 1 || _nDispatcherThreadsCount < 1)
    return E_INVALIDARG;
  //----
  Finalize();
  //reinitialize message id
  NktInterlockedExchange(&nMsgId, 0);
  //create shutdown event
  hRes = cShutdownMtx.Initialize();
  if (SUCCEEDED(hRes))
  {
    //initialize variables
    if ((nWorkerThreadsCount = _nWorkerThreadsCount) > 32)
      nWorkerThreadsCount = 32;
    if ((nDispatchersCount = _nDispatcherThreadsCount) > 128)
      nDispatchersCount = 32;
    //create I/O completion port
    hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, (DWORD)nWorkerThreadsCount);
    if (hIOCP == NULL)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  //start the worker threads
  if (SUCCEEDED(hRes))
  {
    lpWorkerThreads = NKT_MEMMGR_NEW TNktClassWorkerThread<CNktDvTransportEngine>[nWorkerThreadsCount];
    if (lpWorkerThreads != NULL)
    {
      for (i=0; i<nWorkerThreadsCount; i++)
      {
        if (lpWorkerThreads[i].Start(this, &CNktDvTransportEngine::WorkerThreadProc, i) == FALSE)
        {
          hRes = E_OUTOFMEMORY;
          break;
        }
        sprintf_s(szBufA, NKT_DV_ARRAYLEN(szBufA), "Dv-CommEngine/Worker/%lu", (ULONG)i);
        lpWorkerThreads[i].SetThreadName(szBufA);
      }
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
  }
  //create the dispatchers
  for (i=0; i<nDispatchersCount && SUCCEEDED(hRes); i++)
  {
    lpDisp = NKT_MEMMGR_NEW CDispatcher;
    if (lpDisp == NULL)
    {
      hRes = E_OUTOFMEMORY;
      break;
    }
    if (cFixedDispatchersList.AddElement(lpDisp) == FALSE)
    {
      delete lpDisp;
      hRes = E_OUTOFMEMORY;
      break;
    }
    lpDisp->hEvent = ::CreateEventW(NULL, TRUE, FALSE, NULL);
    if (lpDisp->hEvent == NULL ||
        lpDisp->cThread.Start(this, &CNktDvTransportEngine::DispatcherThreadProc, (SIZE_T)lpDisp) == FALSE)
    {
      hRes = E_OUTOFMEMORY;
      break;
    }
    sprintf_s(szBufA, NKT_DV_ARRAYLEN(szBufA), "Dv-CommEngine/Dispatcher/%lu", (ULONG)i);
    lpDisp->cThread.SetThreadName(szBufA);
  }
  //done
  if (FAILED(hRes))
    Finalize();
  return hRes;
}

VOID CNktDvTransportEngine::Finalize()
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CConnection>::Iterator itConn;
  CConnection *lpConn;
  TNktLnkLst<CDispatcher>::Iterator itDisp;
  TNktComPtr<CDispatcher> cDisp;
  SIZE_T i, nCount;

  //signal shutdown
  cShutdownMtx.StartShutdownAndWaitPending(this);
  //kill active connections
  for (lpConn=itConn.Begin(cConnList); lpConn!=NULL; lpConn=itConn.Next())
  {
    InternalDisconnectAgent2(lpConn, FALSE);
  }
  //wait kill
  while (cConnList.IsEmpty() == FALSE)
  {
    {
      CNktAutoFastMutexReverse cLockRev(this);

      ::Sleep(10);
    }
  }
  //stop the dispatchers
  do
  {
    cDisp.Release();
    {
      CNktAutoFastMutex cLock(&(sOneShotDispatchers.cMtx));
      CDispatcher *lpDisp;

      for (lpDisp=itDisp.Begin(sOneShotDispatchers.cList); lpDisp!=NULL; lpDisp=itDisp.Next())
      {
        if (lpDisp->cThread.IsRunning() != FALSE)
        {
          cDisp = lpDisp;
          break;
        }
      }
    }
    if (cDisp != NULL)
    {
      CNktAutoFastMutexReverse cLockRev(this);

      cDisp->cThread.Stop();
    }
  }
  while (cDisp != NULL);
  cDisp.Release();
  //----
  nCount = cFixedDispatchersList.GetCount();
  for (i=0; i<nCount; i++)
  {
    {
      CNktAutoFastMutexReverse cLockRev(this);

      cFixedDispatchersList[i]->cThread.Stop();
    }
  }
  //stop the workers
  if (lpWorkerThreads != NULL && hIOCP != NULL)
  {
    for (i=0; i<nWorkerThreadsCount; i++)
      ::PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);
  }
  if (lpWorkerThreads != NULL)
  {
    for (i=0; i<nWorkerThreadsCount; i++)
    {
      {
        CNktAutoFastMutexReverse cLockRev(this);

        lpWorkerThreads[i].Stop();
      }
    }
  }
  //destroy dispatchers
  do
  {
    cDisp.Release();
    {
      CNktAutoFastMutex cLock(&(sOneShotDispatchers.cMtx));

      cDisp.Attach(sOneShotDispatchers.cList.PopHead());
    }
  }
  while (cDisp != NULL);
  cDisp.Release();
  cFixedDispatchersList.RemoveAllElements();
  nDispatchersCount = 0;
  //destroy worker threads
  if (lpWorkerThreads != NULL)
  {
    delete [] lpWorkerThreads;
    lpWorkerThreads = NULL;
  }
  nWorkerThreadsCount = 0;
  //close I/O completion port
  if (hIOCP != NULL)
  {
    ::CloseHandle(hIOCP);
    hIOCP = NULL;
  }
  //clear misc variables
  cShutdownMtx.Finalize();
  return;
}

HRESULT CNktDvTransportEngine::ConnectAgent(__in DWORD dwPid, __in DWORD dwTimeout)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);
  CConnection *lpConn;
  HRESULT hRes;

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  if (dwPid == 0 || dwPid == dwMePid)
    return E_INVALIDARG;
  //check if connection already exists
  {
    CConnectionAutoRef cConnAutoRef(lpConn = FindConnectionAndAddRef(dwPid), FALSE);

    if (lpConn != NULL)
      return NKT_DVERR_AlreadyExists;
  }
  //----
  lpConn = NKT_MEMMGR_NEW CConnection(this, dwPid);
  if (lpConn == NULL)
    return E_OUTOFMEMORY;
  //create pipe
  hRes = lpConn->Initialize(hIOCP, dwTimeout);
  if (FAILED(hRes))
  {
    lpConn->Release();
    return hRes;
  }
  //add to connection list
  cConnList.PushTail(lpConn);
  return S_OK;
}

VOID CNktDvTransportEngine::DisconnectAgent(__in DWORD dwPid)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return; //not initialized
  InternalDisconnectAgent(dwPid);
  return;
}

HRESULT CNktDvTransportEngine::SendMsg(__in DWORD dwPid, __inout NKT_DV_TMSG_COMMON *lpMsg,
                                       __in SIZE_T nMsgSize)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CConnectionAutoRef cConnAutoRef;
  CConnection *lpConn;
  CNktDvTransportOverlapped *lpOvr;
  HRESULT hRes;

  if (lpMsg == NULL)
    return E_POINTER;
  if (nMsgSize < sizeof(NKT_DV_TMSG_COMMON) || nMsgSize > NKT_DV_TRANSPORT_MAX_DATA_SIZE)
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //find connection
    cConnAutoRef.Attach(lpConn = FindConnectionAndAddRef(dwPid), FALSE);
    if (lpConn == NULL)
      return NKT_DVERR_NotFound;
    NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendMsg]: Pid=%lu, Code=%lu, Id=%lu",
                       ::GetTickCount(), dwPid, lpMsg->dwMsgCode & (~NKT_DV_TMSG_CODE_CALLBACK),
                       lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK)));
  }
  {
    CNktAutoFastMutex cConnAutoLock(lpConn);

    if (lpConn->nState != XSTATE_Connected)
      return E_FAIL; //not initialized
    //get a free buffer
    lpOvr = lpConn->GetBuffer();
    if (lpOvr == NULL)
      return E_OUTOFMEMORY;
    //fill buffer
    nktMemCopy(lpOvr->sMsg.aBuf, lpMsg, nMsgSize);
    //do write prepare
    lpOvr->dwType = XTYPE_WriteRequest;
    lpOvr->nOrder = NktInterlockedIncrement(&(lpConn->nNextWriteOrder));
    lpOvr->dwWrittenBytes = (DWORD)nMsgSize;
    lpConn->AddBufferToRwList(lpOvr);
    hRes = S_OK;
    if (::PostQueuedCompletionStatus(lpConn->hIocpCopy, 0, (ULONG_PTR)lpConn, &(lpOvr->sOvr)) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (FAILED(hRes))
  {
    lpConn->FreeBuffer(lpOvr);
    return hRes;
  }
  //done
  lpConn->AddRef();
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendMsg]: Pid=%lu, Ovr=%IXh",
                     ::GetTickCount(), dwPid, lpOvr));
  return S_OK;
}

HRESULT CNktDvTransportEngine::SendBigPacketMsg(__out LPDWORD lpdwBigPacketId, __in DWORD dwPid, __in LPCVOID lpData,
                                                __in SIZE_T nDataSize)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CConnectionAutoRef cConnAutoRef;
  CConnection *lpConn;
  CNktDvTransportOverlapped *lpOvr;
  NKT_DV_TMSG_BIGDATA *lpMsgBd;
  HRESULT hRes;
  SIZE_T nThisBlockLen;
  DWORD dwOrderNo;
  const BYTE *s;

  if (lpdwBigPacketId == NULL)
    return E_POINTER;
  if (nDataSize < 1)
    return E_INVALIDARG;
  {
    CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);

    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
      return NKT_DVERR_Cancelled;
    if (cShutdownMtx.IsSystemActive() == FALSE)
      return E_FAIL; //not initialized
    //find connection
    cConnAutoRef.Attach(lpConn = FindConnectionAndAddRef(dwPid), FALSE);
    if (lpConn == NULL)
      return NKT_DVERR_NotFound;
    NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendBigPacketMsg]: Pid=%lu, Size=%Iu",
                       ::GetTickCount(), dwPid, nDataSize));
  }
  //process the message
  {
    CNktAutoFastMutex cConnAutoLock(lpConn);

    if (lpConn->nState != XSTATE_Connected)
      return E_FAIL; //not initialized
  }
  //NOTE: Ensure that thread sending big packets is the same that later will send
  //      the real message containing the packet id to avoid writing problems
  s = (const BYTE *)lpData;
  *lpdwBigPacketId = lpConn->cBigData->GetNextBuildId();
  for (dwOrderNo=1; nDataSize>0; dwOrderNo++)
  {
    //get a free buffer
    lpOvr = lpConn->GetBuffer();
    if (lpOvr == NULL)
      return E_OUTOFMEMORY;
    //build message with data
    lpMsgBd = (NKT_DV_TMSG_BIGDATA*)&(lpOvr->sMsg.sMsgCommon);
    hRes = NktDvTransportInitMessage(lpMsgBd, NKT_DV_TMSG_CODE_BigData, GetNextMsgId(), NULL, NULL);
    if (FAILED(hRes))
    {
      lpConn->FreeBuffer(lpOvr);
      return hRes;
    }
    lpMsgBd->sBdHeader.dwThreadId = 0;
    lpMsgBd->sBdHeader.dwId = *lpdwBigPacketId;
    lpMsgBd->sBdHeader.dwOrderNo = dwOrderNo;
    if ((nThisBlockLen=nDataSize) > sizeof(lpMsgBd->aData))
      nThisBlockLen = sizeof(lpMsgBd->aData);
    lpMsgBd->sBdHeader.dwDataLen = (DWORD)nThisBlockLen;
    nktMemCopy(lpMsgBd->aData, s, nThisBlockLen);
    //do write prepare
    lpOvr->dwType = XTYPE_WriteRequest;
    lpOvr->nOrder = NktInterlockedIncrement(&(lpConn->nNextWriteOrder));
    lpOvr->dwWrittenBytes = (DWORD)(sizeof(lpOvr->sMsg.sMsgCommon) + sizeof(lpMsgBd->sBdHeader) +
                                    nThisBlockLen);
    lpConn->AddBufferToRwList(lpOvr);
    if (::PostQueuedCompletionStatus(lpConn->hIocpCopy, 0, (ULONG_PTR)lpConn, &(lpOvr->sOvr)) == FALSE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      lpConn->FreeBuffer(lpOvr);
      return hRes;
    }
    //done
    lpConn->AddRef();
    //advance source data
    s += nThisBlockLen;
    nDataSize -= nThisBlockLen;
    NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendBigPacketMsg]: Pid=%lu, Ovr=%IXh, "
                       "Id=%lu, Order=%lu, Size=%lu", ::GetTickCount(), dwPid, lpOvr, *lpdwBigPacketId,
                       dwOrderNo, (DWORD)nThisBlockLen));
  }
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendBigPacketMsg]: Pid=%lu",
                     ::GetTickCount(), dwPid));
  return S_OK;
}

DWORD CNktDvTransportEngine::GetNextMsgId()
{
  LONG nId;

  for (;;)
  {
    nId = NktInterlockedIncrement(&nMsgId);
    if (nId > 0 && nId < 0x80000000)
      break;
    NktInterlockedExchange(&nMsgId, 0);
  }
  return (DWORD)nId;
}

VOID CNktDvTransportEngine::InternalDisconnectAgent(__in DWORD dwPid)
{
  CConnection *lpConn;
  CConnectionAutoRef cConnAutoRef(lpConn = FindConnectionAndAddRef(dwPid), FALSE);

  //find connection
  if (lpConn != NULL)
  {
    InternalDisconnectAgent2(lpConn, TRUE);
  }
  return;
}

VOID CNktDvTransportEngine::InternalDisconnectAgent2(__inout CNktDvTransportEngine::CConnection *lpConn,
                                                     __in BOOL bCallCallback)
{
  BOOL bDoRelease;
  SIZE_T i, nCount;

  //remove packets from dispatchers if any
  nCount = cFixedDispatchersList.GetCount();
  for (i=0; i<nCount; i++)
    cFixedDispatchersList[i]->RemoveConnection(lpConn);
  {
    CNktAutoFastMutex cLock(&(sOneShotDispatchers.cMtx));
    TNktLnkLst<CDispatcher>::Iterator it;
    CDispatcher *lpDisp;

    for (lpDisp=it.Begin(sOneShotDispatchers.cList); lpDisp!=NULL; lpDisp=it.Next())
      lpDisp->RemoveConnection(lpConn);
  }
  //release?
  bDoRelease = (lpConn->nState != XSTATE_Destroying) ? TRUE : FALSE;
  //shutdown
  lpConn->Shutdown();
  //call callback
  if (bCallCallback != FALSE && bDoRelease != FALSE)
  {
    CNktAutoFastMutexReverse cLockRev(this);

    lpCallback->TEC_OnConnectionClosed(this, lpConn->dwPid);
  }
  //extra release for destroying
  if (bDoRelease != FALSE)
    lpConn->Release();
  return;
}

CNktDvTransportEngine::CConnection* CNktDvTransportEngine::FindConnectionAndAddRef(__in DWORD dwPid)
{
  TNktLnkLst<CConnection>::Iterator it;
  CConnection *lpConn;

  //find process
  for (lpConn=it.Begin(cConnList); lpConn!=NULL; lpConn=it.Next())
  {
    if (dwPid == lpConn->dwPid)
    {
      lpConn->AddRef();
      return lpConn;
    }
  }
  return NULL;
}

VOID CNktDvTransportEngine::WorkerThreadProc(__in SIZE_T nIndex)
{
  CConnection *lpConn;
  LPOVERLAPPED _lpOvr;
  CNktDvTransportOverlapped *lpOvr;
  HRESULT hRes;
  HANDLE hEventCopy;
  BOOL b, bReadPending;
  DWORD dwNumberOfBytes, dwWritten;

  lpCallback->TEC_WorkerThreadStarted();
  //----
  for (;;)
  {
    lpConn = NULL;
    lpOvr = NULL;
    b = ::GetQueuedCompletionStatus(hIOCP, &dwNumberOfBytes, (PULONG_PTR)&lpConn, &_lpOvr, INFINITE);
    if (lpConn == NULL)
    {
      break; //a completion key of 0 is posted to the iocp to request us to shut down...
    }
    hRes = (b != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
    {
      CConnectionAutoRef cConnAutoRef(lpConn);

      lpOvr = CNktDvTransportOverlapped::ClassFromOverlapped(_lpOvr);
      bReadPending = FALSE;
      if (_lpOvr != NULL)
      {
        switch (lpOvr->dwType)
        {
          case XTYPE_Read:
            lpOvr->dwReadedBytes = dwNumberOfBytes;
            //add to read queue
            if (SUCCEEDED(hRes))
            {
              lpConn->AddBufferInSortedReadedQueue(lpOvr);
              hRes = lpConn->SendReadPacket(); //setup read-ahead
            }
            NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[ReadCompleted]: Ovr=%IXh, "
                                                    "hRes=%08X",
                               ::GetTickCount(), lpOvr, hRes));
            bReadPending = TRUE;
            break;

          case XTYPE_WriteRequest:
            {
            CNktAutoFastMutex cLock(lpConn);

            if (SUCCEEDED(hRes))
              lpConn->AddBufferInSortedToWriteQueue(lpOvr);
            while (SUCCEEDED(hRes) && cShutdownMtx.IsSystemActive() != FALSE &&
                   cShutdownMtx.IsShuttingDown() == FALSE)
            {
              //get next sequenced block to write
              lpOvr = lpConn->GetBufferFromSortedToWriteQueue(lpConn->nNextWriteOrderToProcess);
              if (lpOvr == NULL)
                break;
              if (lpOvr->dwType != XTYPE_Discard)
              {
                lpConn->AddBufferToRwList(lpOvr);
                //do real write
                lpOvr->dwType = XTYPE_Write;
                if (lpOvr->sMsg.sMsgCommon.hProcessedEvent != NULL)
                {
                  if (::DuplicateHandle(::GetCurrentProcess(),
                                        (HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent),
                                        lpConn->hAgentProc, &hEventCopy, 0, FALSE,
                                        DUPLICATE_SAME_ACCESS) != FALSE)
                    lpOvr->sMsg.sMsgCommon.hProcessedEvent = NKT_PTR_2_ULONGLONG(hEventCopy);
                  else
                    hRes = NKT_HRESULT_FROM_LASTERROR();
                }
                if (SUCCEEDED(hRes))
                {
                  if (::WriteFile(lpConn->hPipe, &(lpOvr->sMsg), lpOvr->dwWrittenBytes, &dwWritten,
                                  &(lpOvr->sOvr)) == FALSE)
                  {
                    hRes = NKT_HRESULT_FROM_LASTERROR();
                    if (hRes == NKT_HRESULT_FROM_WIN32(ERROR_IO_PENDING))
                      hRes = S_OK;
                  }
                }
                if (SUCCEEDED(hRes))
                  lpConn->AddRef();
                else
                  lpConn->FreeBuffer(lpOvr);
                NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[Write]: Ovr=%IXh, "
                                                        "hRes=%08X",
                                   ::GetTickCount(), lpOvr, hRes));
              }
              else
              {
                lpConn->FreeBuffer(lpOvr);
              }
              NktInterlockedIncrement(&(lpConn->nNextWriteOrderToProcess));
            }
            }
            break;

          case XTYPE_Write:
            if (SUCCEEDED(hRes) && lpOvr->dwWrittenBytes != dwNumberOfBytes)
              hRes = NKT_HRESULT_FROM_WIN32(ERROR_WRITE_FAULT);
            NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[WriteCompleted]: Ovr=%IXh, "
                                                    "hRes=%08X",
                               ::GetTickCount(), lpOvr, hRes));
            lpConn->FreeBuffer(lpOvr);
            break;

          case XTYPE_Discard:
            lpConn->FreeBuffer(lpOvr);
            break;

          default:
            NKT_ASSERT(FALSE);
            lpConn->FreeBuffer(lpOvr);
            break;
        }
      }
      else
      {
        hRes = NKT_DVERR_InvalidTransportData;
      }
      if (SUCCEEDED(hRes) && bReadPending != FALSE)
      {
        while (SUCCEEDED(hRes) && cShutdownMtx.IsSystemActive() != FALSE &&
               cShutdownMtx.IsShuttingDown() == FALSE)
        {
          //get next sequenced block readed
          lpOvr = lpConn->GetBufferFromSortedReadedQueue(lpConn->nNextReadOrderToProcess);
          if (lpOvr == NULL)
            break;
          hRes = DispatchReadMessage(lpConn, lpOvr);
          NktInterlockedIncrement(&(lpConn->nNextReadOrderToProcess));
        }
      }
      if (FAILED(hRes))
      {
        if (cShutdownMtx.IsSystemActive() != FALSE && cShutdownMtx.IsShuttingDown() == FALSE)
          lpCallback->TEC_OnTransportError(this, lpConn->dwPid, hRes);
        DisconnectAgent(lpConn->dwPid);
      }
      lpConn->Release();
    }
  }
  //----
  lpCallback->TEC_WorkerThreadEnded();
  return;
}

VOID CNktDvTransportEngine::DispatcherThreadProc(__in SIZE_T nDispatcherPtr)
{
#define MESSAGES_PER_ROUND 4
  CNktThread *lpThread;
  CNktDvTransportOverlapped *lpOvr[MESSAGES_PER_ROUND];
  HANDLE hAvailableEvent;
  DWORD dwHitEvent;
  CConnection *lpConn;
  SIZE_T nProcessed, k, nRetrieved;
  CDispatcher *lpDispatcher;
  HRESULT hRes;

  lpDispatcher = (CDispatcher*)nDispatcherPtr;
  //----
  lpCallback->TEC_WorkerThreadStarted();
  //----
  lpThread = &(lpDispatcher->cThread);
  hAvailableEvent = lpDispatcher->hEvent;
  while (1)
  {
    if (lpThread->CheckForAbort(INFINITE, 1, &hAvailableEvent, &dwHitEvent) != FALSE)
      break;
    if (dwHitEvent == 1)
    {
      ::ResetEvent(hAvailableEvent);
      //init working
      NktInterlockedExchange(&(lpDispatcher->nIsWorking), 1);
      while (lpThread->CheckForAbort(0) == FALSE)
      {
        {
          CNktAutoFastMutex cLock(lpDispatcher);

          lpOvr[0] = lpDispatcher->PeekNextOverlapped();
          if (lpOvr[0] == NULL)
            break;
          lpOvr[0]->RemoveNode(); //remove from list
          //try to get more messages
          for (nRetrieved=1; nRetrieved<MESSAGES_PER_ROUND; nRetrieved++)
          {
            lpOvr[nRetrieved] = lpDispatcher->PeekNextOverlapped();
            if (lpOvr[nRetrieved] == NULL ||
                lpOvr[nRetrieved]->sEngineInternals.lpConn != lpOvr[0]->sEngineInternals.lpConn ||
                lpOvr[nRetrieved]->sEngineInternals.dwThreadId != lpOvr[0]->sEngineInternals.dwThreadId)
              break;
            if (lpOvr[0]->sMsg.sMsgCommon.dwMsgCode == NKT_DV_TMSG_CODE_BigData)
            {
              if (lpOvr[nRetrieved]->sMsg.sMsgCommon.dwMsgCode != NKT_DV_TMSG_CODE_BigData)
                break;
            }
            else
            {
              if (lpOvr[nRetrieved]->sMsg.sMsgCommon.dwMsgCode == NKT_DV_TMSG_CODE_BigData)
                break;
            }
            lpOvr[nRetrieved]->RemoveNode(); //remove from list
          }
        }
        //at this point we have up to 'MESSAGES_PER_ROUND' messages to process of the same
        //connection/tid
        lpConn = (CConnection*)(lpOvr[0]->sEngineInternals.lpConn);
        hRes = S_OK;
        if (lpOvr[0]->sMsg.sMsgCommon.dwMsgCode == NKT_DV_TMSG_CODE_BigData)
        {
          CNktAutoFastMutex cConnAutoLock(lpConn);

          //try to process a sequence of "NKT_DV_TMSG_CODE_BigData" in the same round
          for (nProcessed=0; nProcessed<nRetrieved && SUCCEEDED(hRes); nProcessed++)
          {
            hRes = lpConn->cBigData->ProcessPacket(&(lpOvr[nProcessed]->sMsg.sMsgCommon));
            //signal client message event
            if (SUCCEEDED(hRes))
              hRes = SignalProcessedEvent(lpOvr[nProcessed]);
            //free buffer
            lpConn->FreeBuffer(lpOvr[nProcessed]);
          }
        }
        else
        {
          //try to process a sequence of standard packets
          for (nProcessed=0; nProcessed<nRetrieved && SUCCEEDED(hRes); nProcessed++)
          {
            //process
            hRes = lpCallback->TEC_OnAgentMessage(this, lpConn->dwPid,
                                                  &(lpOvr[nProcessed]->sMsg.sMsgCommon),
                                                  (SIZE_T)(lpOvr[nProcessed]->dwReadedBytes),
                                                  lpConn->hAgentProc, lpConn->cBigData);
            //signal client message event
            if (SUCCEEDED(hRes))
              hRes = SignalProcessedEvent(lpOvr[nProcessed]);
          }
          //free processed buffers
          {
            CNktAutoFastMutex cConnAutoLock(lpConn);

            for (k=0; k<nProcessed; k++)
              lpConn->FreeBuffer(lpOvr[k]);
          }
        }
        //if some packets were not processed, it is almost sure that it is because of an error
        if (nProcessed < nRetrieved)
        {
          //requeue non-processed buffers at the front
          CNktAutoFastMutex cLock(lpDispatcher);

          for (k=nRetrieved; k>nProcessed; )
            lpDispatcher->AddOverlapped(lpOvr[--k], TRUE);
        }
        //release connection one for each processed packet from the list
        for (k=0; k<nProcessed; k++)
          lpConn->Release();
        //handle errors
        if (FAILED(hRes))
        {
          if (cShutdownMtx.IsSystemActive() != FALSE && cShutdownMtx.IsShuttingDown() == FALSE)
            lpCallback->TEC_OnTransportError(this, lpConn->dwPid, hRes);
          DisconnectAgent(lpConn->dwPid);
        }
      }
      //done working
      NktInterlockedExchange(&(lpDispatcher->nLastWorkTick), (LONG)::GetTickCount());
      NktInterlockedExchange(&(lpDispatcher->nIsWorking), 0);
    }
  }
  //----
  lpCallback->TEC_WorkerThreadEnded();
#undef MESSAGES_PER_ROUND
  return;
}

HRESULT CNktDvTransportEngine::DispatchReadMessage(__inout CConnection *lpConn,
                                                     __inout CNktDvTransportOverlapped *lpOvr)
{
  union {
    NKT_DV_TMSG_ONHOOKCALLED *lpHookCalledMsg;
    NKT_DV_TMSG_BIGDATA *lpBigDataMsg;
    NKT_DV_TMSG_CUSTOMMESSAGE *lpCustomMessageMsg;
  };
  TNktComPtr<CDispatcher> cDisp;
  Fnv32_t nHash;
  DWORD dwTid;
  HRESULT hRes;

  if (lpOvr->dwReadedBytes < sizeof(NKT_DV_TMSG_COMMON) ||
      lpOvr->dwReadedBytes > NKT_DV_TRANSPORT_MAX_DATA_SIZE)
  {
    lpConn->FreeBuffer(lpOvr);
    return NKT_DVERR_InvalidTransportData;
  }
  if (lpOvr->dwType == XTYPE_Discard)
  {
    lpConn->FreeBuffer(lpOvr);
    return S_OK;
  }
  /*
  //check if initial message
  if (lpOvr->sMsg.sMsgCommon.dwMsgCode == NKT_DV_TMSG_CODE_InitialConnectionMessage)
  {
    //initial message has special handling
    hRes = lpCallback->TEC_OnAgentMessage(this, lpConn->dwPid, &(lpOvr->sMsg.sMsgCommon),
                                          (SIZE_T)(lpOvr->dwReadedBytes), lpConn->hAgentProc, lpConn->cBigData);
    //signal client message event
    if (SUCCEEDED(hRes))
      hRes = SignalProcessedEvent(lpOvr);
    //done
    lpConn->FreeBuffer(lpOvr);
    return hRes;
  }
  */
  //system messages are re-routed to first dispatcher
  dwTid = 0;
  if (nDispatchersCount > 1)
  {
    switch (lpOvr->sMsg.sMsgCommon.dwMsgCode)
    {
      case NKT_DV_TMSG_CODE_OnHookCalled:
        if (lpOvr->dwReadedBytes < sizeof(NKT_DV_TMSG_ONHOOKCALLED))
        {
          lpConn->FreeBuffer(lpOvr);
          return NKT_DVERR_InvalidTransportData;
        }
        lpHookCalledMsg = reinterpret_cast<NKT_DV_TMSG_ONHOOKCALLED*>(&(lpOvr->sMsg));
        dwTid = lpHookCalledMsg->dwThreadId;
        break;

      case NKT_DV_TMSG_CODE_BigData:
        if (lpOvr->dwReadedBytes < sizeof(struct NKT_DV_TMSG_BIGDATA::tagBdHeader))
        {
          lpConn->FreeBuffer(lpOvr);
          return NKT_DVERR_InvalidTransportData;
        }
        lpBigDataMsg = reinterpret_cast<NKT_DV_TMSG_BIGDATA*>(&(lpOvr->sMsg));
        dwTid = lpBigDataMsg->sBdHeader.dwThreadId;
        break;

      case NKT_DV_TMSG_CODE_CustomMessage:
        if (lpOvr->dwReadedBytes < sizeof(NKT_DV_TMSG_CUSTOMMESSAGE))
        {
          lpConn->FreeBuffer(lpOvr);
          return NKT_DVERR_InvalidTransportData;
        }
        lpCustomMessageMsg = reinterpret_cast<NKT_DV_TMSG_CUSTOMMESSAGE*>(&(lpOvr->sMsg));
        dwTid = lpCustomMessageMsg->dwThreadId;
        break;
    }
  }
  if (nDispatchersCount < 2)
  {
    cDisp = cFixedDispatchersList[0];
  }
  else if (dwTid != 0)
  {
    nHash = fnv_32a_buf(&(lpConn->dwPid), sizeof(lpConn->dwPid), FNV1A_32_INIT);
    nHash = fnv_32a_buf(&dwTid, sizeof(dwTid), nHash);
    cDisp =  cFixedDispatchersList[1 + ((SIZE_T)nHash % (nDispatchersCount-1))];
  }
  else
  {
    switch (lpOvr->sMsg.sMsgCommon.dwMsgCode)
    {
      case NKT_DV_TMSG_CODE_BigData:
      case NKT_DV_TMSG_CODE_OnLoadLibraryCalled:
      case NKT_DV_TMSG_CODE_OnFreeLibraryCalled:
        /*
      case NKT_DV_TMSG_CODE_InitialConnectionMessage:
        */
        cDisp = cFixedDispatchersList[0];
        break;

      default:
        {
        //start a new temp dispatcher
        TNktLnkLst<CDispatcher>::Iterator it;
        TNktComPtr<CDispatcher> cToRemoveDisp;
        CDispatcher *lpDisp;
        DWORD dwLastTick, dwCurrTick;

        dwCurrTick = ::GetTickCount();
drm_loop1:
        cDisp.Release();
        cToRemoveDisp.Release();
        {
          CNktAutoFastMutex cLock(&(sOneShotDispatchers.cMtx));

          for (lpDisp=it.Begin(sOneShotDispatchers.cList); lpDisp!=NULL; lpDisp=it.Next())
          {
            if (lpDisp->nIsWorking == 0)
              break;
          }
          if (lpDisp != NULL)
          {
            cDisp = lpDisp;
            NktInterlockedExchange(&(lpDisp->nIsWorking), 1);
            //----
            dwLastTick = (DWORD)(lpDisp->nLastWorkTick);
            if (dwLastTick != 0)
            {
              if (dwCurrTick > dwLastTick+MAX_TEMP_DISPATCHER_SLEEPTIME || dwCurrTick < dwLastTick)
              {
                //kill this item
                lpDisp->RemoveNode();
                cToRemoveDisp.Attach(lpDisp);
              }
            }
          }
        }
        if (cToRemoveDisp != NULL)
        {
          cToRemoveDisp->cThread.Stop();
          goto drm_loop1;
        }
        if (cDisp == NULL)
        {
          //create new item
          cDisp.Attach(NKT_MEMMGR_NEW CDispatcher);
          if (cDisp == NULL)
            return E_OUTOFMEMORY;
          cDisp->hEvent = ::CreateEventW(NULL, TRUE, FALSE, NULL);
          if (cDisp->hEvent == NULL)
            return E_OUTOFMEMORY;
          if (cDisp->cThread.Start(this, &CNktDvTransportEngine::DispatcherThreadProc,
                                   (SIZE_T)(CDispatcher*)cDisp) == FALSE)
            return E_OUTOFMEMORY;
          cDisp->cThread.SetThreadName("Dv-CommEngine/TempDispatcher");
          //add to list
          {
            CNktAutoFastMutex cLock(&(sOneShotDispatchers.cMtx));

            lpDisp = cDisp;
            sOneShotDispatchers.cList.PushTail(lpDisp);
            lpDisp->AddRef();
          }
        }
        }
        break;
    }
  }
  lpOvr->sEngineInternals.lpConn = lpConn;
  lpConn->AddRef();
  lpOvr->sEngineInternals.dwThreadId = dwTid;
  lpOvr->RemoveNode();
  {
    CNktAutoFastMutex cLock((CDispatcher*)cDisp);

    hRes = cDisp->AddOverlapped(lpOvr, FALSE);
  }
  return hRes;
}

HRESULT CNktDvTransportEngine::SignalProcessedEvent(__inout CNktDvTransportOverlapped *lpOvr)
{
  HRESULT hRes;

  hRes = S_OK;
  if (lpOvr->sMsg.sMsgCommon.hProcessedEvent != NULL)
  {
    //signal client message event
    if (::SetEvent((HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent)) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    ::CloseHandle((HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent));
  }
  return hRes;
}

//-----------------------------------------------------------

CNktDvTransportEngine::CConnection::CConnection(__inout CNktDvTransportEngine *_lpTransport,
                                                __in DWORD _dwPid) : CNktDvObject(), CNktFastMutex(),
                                                                     TNktLnkLstNode<CConnection>()
{
  lpTransport = _lpTransport;
  dwPid = _dwPid;
  hPipe = NULL;
  hAgentProc = NULL;
  nState = XSTATE_Disconnected;
  hIocpCopy = NULL;
  NktInterlockedExchange(&nNextReadOrder, 0);
  NktInterlockedExchange(&nNextReadOrderToProcess, 1);
  NktInterlockedExchange(&nNextWriteOrder, 0);
  NktInterlockedExchange(&nNextWriteOrderToProcess, 1);
  NktInterlockedExchange(&nInUseMessageCount, 0);
  NktInterlockedExchange(&nFreeBufferListCount, 0);
  return;
}

CNktDvTransportEngine::CConnection::~CConnection()
{
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpOvr;

  //close handles
  if (hAgentProc != NULL)
  {
    ::CloseHandle(hAgentProc);
    hAgentProc = NULL;
  }
  if (hPipe != NULL)
  {
    ::CloseHandle(hPipe);
    hPipe = NULL;
  }
  //free buffers
  {
    CNktAutoFastMutex cConnAutoLock(this);

    for (lpOvr=it.Begin(cToWriteQueueBufferList); lpOvr!=NULL; lpOvr=it.Next())
    {
      cToWriteQueueBufferList.Remove(lpOvr);
      delete lpOvr;
    }
    for (lpOvr=it.Begin(cReadedQueueBufferList); lpOvr!=NULL; lpOvr=it.Next())
    {
      cReadedQueueBufferList.Remove(lpOvr);
      delete lpOvr;
    }
    for (lpOvr=it.Begin(cActiveRwBufferList); lpOvr!=NULL; lpOvr=it.Next())
    {
      cActiveRwBufferList.Remove(lpOvr);
      delete lpOvr;
    }
    for (lpOvr=it.Begin(cFreeBufferList); lpOvr!=NULL; lpOvr=it.Next())
    {
      cFreeBufferList.Remove(lpOvr);
      delete lpOvr;
    }
  }
  cBigData.Release();
  {
    CNktAutoFastMutex cTransportLock(lpTransport);

    RemoveNode();
  }
  return;
}

HRESULT CNktDvTransportEngine::CConnection::Initialize(__inout HANDLE &hIOCP, __in DWORD dwTimeout)
{
  CNktDvTransportEngine::CConnectionAutoRef cAutoRef(this);
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  WCHAR szBufW[256];
  SECURITY_ATTRIBUTES sSecAttrib;
  SECURITY_DESCRIPTOR sSecDesc;
  HRESULT hRes;
  DWORD i, dwMode;

  cBigData.Attach(NKT_MEMMGR_NEW CNktDvTransportBigData);
  if (cBigData == NULL)
    return E_OUTOFMEMORY;
  //connect to agent
  FillSecurity(sSecAttrib, sSecDesc);
  swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"\\\\.\\pipe\\Deviare_%lu", dwPid);
  for (;;)
  {
    hPipe = ::CreateFileW(szBufW, GENERIC_READ|GENERIC_WRITE, 0, &sSecAttrib, OPEN_EXISTING,
                          FILE_FLAG_WRITE_THROUGH|FILE_FLAG_OVERLAPPED, NULL);
    if (hPipe != NULL && hPipe != INVALID_HANDLE_VALUE)
      break;
    hRes = NKT_HRESULT_FROM_LASTERROR();
    hPipe = NULL;
    if (hRes != NKT_HRESULT_FROM_WIN32(ERROR_PIPE_BUSY))
      return hRes;
    //all pipe instances are busy, so wait for 1 second
    if (::WaitNamedPipe(szBufW, 100) == FALSE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      if (hRes != NKT_HRESULT_FROM_WIN32(ERROR_SEM_TIMEOUT))
        return hRes;
    }
    if (dwTimeout > 100)
      dwTimeout -= 100;
    else if (dwTimeout == 0)
      return NKT_HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    else
      dwTimeout = 0;
  }
  //change state
  nState = XSTATE_Connected;
  //pipe connected; change to message-read mode
  dwMode = PIPE_READMODE_MESSAGE;
  if (::SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  /*
  //wait for the first message
  for (;;)
  {
    NKT_DV_TMSG_COMMON sMsg;
    DWORD dwReaded;

    if (::PeekNamedPipe(hPipe, &sMsg, (DWORD)sizeof(sMsg), &dwReaded, NULL, NULL) != FALSE &&
        dwReaded >= (DWORD)sizeof(sMsg))
    {
      if (sMsg.dwMsgCode == NKT_DV_TMSG_CODE_InitialConnectionMessage)
        break; //all is OK
      _ASSERT(FALSE);
      return NKT_DVERR_InvalidTransportData;
    }
    if (dwTimeout == 0)
      return NKT_HRESULT_FROM_WIN32(ERROR_TIMEOUT);
    if (dwTimeout > 100)
    {
      ::Sleep(100);
      dwTimeout -= 100;
    }
    else
    {
      ::Sleep(dwTimeout);
      dwTimeout = 0;
    }
  }
  */
  //associate the pipe with an I/O completion port
  if (::CreateIoCompletionPort(hPipe, hIOCP, (ULONG_PTR)this, 0) == NULL)
    return NKT_HRESULT_FROM_LASTERROR();
  hIocpCopy = hIOCP;
  //open agent process for handle duplication
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hAgentProc = cPhMgr->GetHandle(dwPid, SYNCHRONIZE|PROCESS_DUP_HANDLE);
  if (hAgentProc == NULL)
    return E_ACCESSDENIED;
  //setup read-ahead
  for (i=0; i<READ_AHEAD; i++)
  {
    hRes = SendReadPacket();
    if (FAILED(hRes))
      return hRes;
  }
  return S_OK;
}

VOID CNktDvTransportEngine::CConnection::Shutdown()
{
  BOOL bDiscardBuffers;

  bDiscardBuffers = FALSE;
  {
    CNktAutoFastMutex cLock(this);

    if (nState != XSTATE_Destroying)
    {
      //destroy pipe
      if (hPipe != NULL)
      {
        ::DisconnectNamedPipe(hPipe);
        ::CloseHandle(hPipe);
        hPipe = NULL;
      }
      //set new state
      nState = XSTATE_Destroying;
      bDiscardBuffers = TRUE;
    }
  }
  if (bDiscardBuffers != FALSE)
    DiscardAllBuffers();
  return;
}

CNktDvTransportOverlapped* CNktDvTransportEngine::CConnection::GetBuffer()
{
  CNktDvTransportOverlapped *lpOvr;
  LONG nCount;
  DWORD dwDelay;
  SIZE_T nRetries;

  {
    CNktAutoFastMutex cBufferListLock(&cBufferListMtx);

    lpOvr = cFreeBufferList.PopHead();
    if (lpOvr != NULL)
      NktInterlockedDecrement(&nFreeBufferListCount);
  }
  if (lpOvr == NULL)
    lpOvr = NKT_MEMMGR_NEW CNktDvTransportOverlapped; //create a new one if free list is empty
  if (lpOvr != NULL)
  {
    nCount = CNktDvRegistrySettings::GetSpyMgrTransportMaxInUseMessageCount();
    if (NktInterlockedIncrement(&nInUseMessageCount) >= nCount)
    {
      dwDelay = CNktDvRegistrySettings::GetSpyMgrTransportMessageDelay();
      for (nRetries=30; nRetries>0 && nInUseMessageCount>=nCount; nRetries--)
        ::Sleep(dwDelay);
    }
  }
  return lpOvr;
}

VOID CNktDvTransportEngine::CConnection::FreeBuffer(__inout CNktDvTransportOverlapped *lpOvr)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);

  lpOvr->RemoveNode();
  lpOvr->dwType = XTYPE_Free;
  if (nFreeBufferListCount >= CNktDvRegistrySettings::GetTransportMaxFreeBufferList())
  {
    delete lpOvr;
  }
  else
  {
    NktInterlockedIncrement(&nFreeBufferListCount);
    cFreeBufferList.PushTail(lpOvr);
  }
  NktInterlockedDecrement(&nInUseMessageCount);
  return;
}

VOID CNktDvTransportEngine::CConnection::DiscardAllBuffers()
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpOvr;

  //mark active buffers as discard
  for (lpOvr=it.Begin(cActiveRwBufferList); lpOvr!=NULL; lpOvr=it.Next())
    lpOvr->dwType = XTYPE_Discard;
  for (lpOvr=it.Begin(cReadedQueueBufferList); lpOvr!=NULL; lpOvr=it.Next())
    lpOvr->dwType = XTYPE_Discard;
  for (lpOvr=it.Begin(cToWriteQueueBufferList); lpOvr!=NULL; lpOvr=it.Next())
    lpOvr->dwType = XTYPE_Discard;
  return;
}

VOID CNktDvTransportEngine::CConnection::AddBufferToRwList(__inout CNktDvTransportOverlapped *lpOvr)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);

  cActiveRwBufferList.PushTail(lpOvr);
  return;
}


VOID CNktDvTransportEngine::CConnection::AddBufferInSortedReadedQueue(
                                                          __inout CNktDvTransportOverlapped *lpOvr)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpCurrOvr;

  lpOvr->RemoveNode();
  for (lpCurrOvr=it.Begin(cReadedQueueBufferList); lpCurrOvr!=NULL; lpCurrOvr=it.Next())
  {
    if (lpOvr->nOrder < lpCurrOvr->nOrder)
    {
      cReadedQueueBufferList.PushBefore(lpOvr, lpCurrOvr);
      return;
    }
  }
  cReadedQueueBufferList.PushTail(lpOvr);
  return;
}

VOID CNktDvTransportEngine::CConnection::AddBufferInSortedToWriteQueue(
                                                          __inout CNktDvTransportOverlapped *lpOvr)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpCurrOvr;

  lpOvr->RemoveNode();
  for (lpCurrOvr=it.Begin(cToWriteQueueBufferList); lpCurrOvr!=NULL; lpCurrOvr=it.Next())
  {
    if (lpOvr->nOrder < lpCurrOvr->nOrder)
    {
      cToWriteQueueBufferList.PushBefore(lpOvr, lpCurrOvr);
      return;
    }
  }
  cToWriteQueueBufferList.PushTail(lpOvr);
  return;
}

CNktDvTransportOverlapped* CNktDvTransportEngine::CConnection::GetBufferFromSortedReadedQueue(
                                                                        __in LONG volatile nOrder)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);
  CNktDvTransportOverlapped *lpOvr;

  lpOvr = cReadedQueueBufferList.GetHead();
  if (lpOvr != NULL && lpOvr->nOrder == nOrder)
  {
    cReadedQueueBufferList.Remove(lpOvr);
    return lpOvr;
  }
  return NULL;
}

CNktDvTransportOverlapped* CNktDvTransportEngine::CConnection::GetBufferFromSortedToWriteQueue(
                                                                        __in LONG volatile nOrder)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);
  CNktDvTransportOverlapped *lpOvr;

  lpOvr = cToWriteQueueBufferList.GetHead();
  if (lpOvr != NULL && lpOvr->nOrder == nOrder)
  {
    cToWriteQueueBufferList.Remove(lpOvr);
    return lpOvr;
  }
  return NULL;
}

HRESULT CNktDvTransportEngine::CConnection::SendReadPacket()
{
  CNktDvTransportOverlapped *lpOvr;
  DWORD dwReaded;
  HRESULT hRes;

  //prepare buffer
  lpOvr = GetBuffer();
  if (lpOvr == NULL)
    return E_OUTOFMEMORY;
  //do read
  {
    CNktAutoFastMutex cLock(this);

    lpOvr->dwType = XTYPE_Read;
    lpOvr->nOrder = NktInterlockedIncrement(&nNextReadOrder);
    AddBufferToRwList(lpOvr);
    hRes = (::ReadFile(hPipe, &(lpOvr->sMsg), sizeof(lpOvr->sMsg), &dwReaded, &(lpOvr->sOvr)) != FALSE)
                  ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  }
  if (FAILED(hRes) && hRes != NKT_HRESULT_FROM_WIN32(ERROR_IO_PENDING))
  {
    FreeBuffer(lpOvr);
    return hRes;
  }
  AddRef();
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportEngine[SendReadPacket]: Ovr=%IXh%s",
                     ::GetTickCount(), lpOvr, (hRes != S_OK) ? " (pending)" : ""));
  return S_OK;
}

VOID CNktDvTransportEngine::CConnection::FillSecurity(__inout SECURITY_ATTRIBUTES &sSecAttrib,
                                                      __inout SECURITY_DESCRIPTOR &sSecDesc)
{
  ::InitializeSecurityDescriptor(&sSecDesc, SECURITY_DESCRIPTOR_REVISION);
  ::SetSecurityDescriptorDacl(&sSecDesc, TRUE, NULL, FALSE);
  sSecAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  sSecAttrib.bInheritHandle = TRUE;
  sSecAttrib.lpSecurityDescriptor = &sSecDesc;
  return;
}

//-----------------------------------------------------------

CNktDvTransportEngine::CDispatcher::CDispatcher() : CNktDvObject(), CNktFastMutex(), TNktLnkLstNode<CDispatcher>()
{
  hEvent = NULL;
  NktInterlockedExchange(&nIsWorking, 0);
  NktInterlockedExchange(&nLastWorkTick, 0);
  return;
}

CNktDvTransportEngine::CDispatcher::~CDispatcher()
{
  if (hEvent != NULL)
    ::CloseHandle(hEvent);
  return;
}

HRESULT CNktDvTransportEngine::CDispatcher::AddOverlapped(__inout CNktDvTransportOverlapped *lpOvr,
                                                          __in BOOL bInFront)
{
  //NOTE: Caller must hold the lock
  if (bInFront == FALSE)
  {
    cQueueBufferList.PushTail(lpOvr);
    return (::SetEvent(hEvent) != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  }
  cQueueBufferList.PushHead(lpOvr);
  return S_OK;
}

CNktDvTransportOverlapped* CNktDvTransportEngine::CDispatcher::PeekNextOverlapped()
{
  //NOTE: Caller must hold the lock
  return cQueueBufferList.GetHead();
}

VOID CNktDvTransportEngine::CDispatcher::RemoveConnection(__in CNktDvTransportEngine::CConnection *lpConn)
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpOvr;

  for (lpOvr=it.Begin(cQueueBufferList); lpOvr!=NULL; lpOvr=it.Next())
  {
    if (lpOvr->sEngineInternals.lpConn == lpConn)
    {
      lpConn->FreeBuffer(lpOvr);
      lpConn->Release();
    }
  }
  return;
}
