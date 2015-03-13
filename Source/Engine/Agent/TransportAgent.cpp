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

#include "Agent.h"
#include "TransportAgent.h"
#include "..\..\Common\RegistrySettings.h"
#include "..\..\Common\DynamicAPIs.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define READ_AHEAD                                         4

#define XTYPE_Free                                         0
#define XTYPE_Connect                                      1
#define XTYPE_RetryConnect                                 2
#define XTYPE_Read                                         3
#define XTYPE_WriteRequest                                 4
#define XTYPE_Write                                        5
#define XTYPE_Discard                                      6

#define BITSET(_array, _bit)    _array[_bit>>3] |= 1 << (_bit&7)
#define BITRESET(_array, _bit)  _array[_bit>>3] &= ~(1 << (_bit&7))
#define BITTEST(_array, _bit)   (_array[_bit>>3] & (1 << (_bit&7)))

//-----------------------------------------------------------

CNktDvTransportAgent::CNktDvTransportAgent() : CNktFastMutex(), CNktMemMgrObj()
{
  hIOCP = NULL;
  hPipe = NULL;
  //----
  dwEnginePid = 0;
  dwMePid = ::GetCurrentProcessId();
  hEngineProc = NULL;
  lpCallback = NULL;
  NktInterlockedExchange(&nMsgId, 0);
  NktInterlockedExchange(&nNextReadOrder, 0);
  NktInterlockedExchange(&nNextReadOrderToProcess, 1);
  NktInterlockedExchange(&nNextWriteOrder, 0);
  NktInterlockedExchange(&nNextWriteOrderToProcess, 1);
  NktInterlockedExchange(&nThreadsEnding, 0);
  nWorkerThreadsCount = 0;
  NktInterlockedExchange(&nInUseMessageCount, 0);
  NktInterlockedExchange(&nFreeBufferListCount, 0);
  return;
}

CNktDvTransportAgent::~CNktDvTransportAgent()
{
  if (bAppIsExiting == FALSE)
  {
    Finalize();
  }
  return;
}

HRESULT CNktDvTransportAgent::Initialize(__in DWORD _dwEnginePid, __in HANDLE _hServerProcDup,
                                         __in SIZE_T _nWorkerThreadsCount,
                                         __in CNktDvTransportAgentCallback *_lpCallback)
{
  CNktAutoFastMutex cLock(this);
  SECURITY_ATTRIBUTES sSecAttrib;
  SECURITY_DESCRIPTOR sSecDesc;
  HRESULT hRes;
  union {
    CHAR szBufA[256];
    WCHAR szBufW[256];
  };
  SIZE_T i;

  if (_dwEnginePid == 0 || _lpCallback == NULL)
    return E_FAIL;
  dwEnginePid = _dwEnginePid;
  lpCallback = _lpCallback;
  dwMePid = ::GetCurrentProcessId();
  hEngineProc = _hServerProcDup;
  //initialize variables
  if ((nWorkerThreadsCount = _nWorkerThreadsCount) > 32)
    nWorkerThreadsCount = 32;
  NktInterlockedExchange(&nMsgId, 0);
  NktInterlockedExchange(&nNextReadOrder, 0);
  NktInterlockedExchange(&nNextReadOrderToProcess, 1);
  NktInterlockedExchange(&nThreadsEnding, 0);
  //create shutdown event
  if (cShutdownEvent.Create(TRUE, FALSE) == FALSE)
  {
init_err_getoserr:
    hRes = NKT_HRESULT_FROM_LASTERROR();
init_err:
    Finalize();
    return hRes;
  }
  //init big data
  cBigData.Attach(NKT_MEMMGR_NEW CNktDvTransportBigData);
  if (cBigData == NULL)
  {
    hRes = E_OUTOFMEMORY;
    goto init_err;
  }
  //create agent(server) pipe
  FillSecurity(sSecAttrib, sSecDesc);
  swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"\\\\.\\pipe\\Deviare_%lu", dwMePid);
  hPipe = ::CreateNamedPipeW(szBufW, PIPE_ACCESS_DUPLEX|FILE_FLAG_WRITE_THROUGH|FILE_FLAG_OVERLAPPED,
                             PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT, 1,
                             NKT_DV_TRANSPORT_BUFFER_SIZE, NKT_DV_TRANSPORT_BUFFER_SIZE, 1000,
                             &sSecAttrib);
  if (hPipe == NULL && hPipe == INVALID_HANDLE_VALUE)
    goto init_err_getoserr;
  //create I/O completion port
  hIOCP = ::CreateIoCompletionPort(hPipe, NULL, (ULONG_PTR)this, (DWORD)nWorkerThreadsCount);
  if (hIOCP == NULL)
    goto init_err_getoserr;
  //start the worker threads (worker 0 is the pipe alive checker)
  lpWorkerThreads = NKT_MEMMGR_NEW TNktClassWorkerThread<CNktDvTransportAgent>[nWorkerThreadsCount];
  if (lpWorkerThreads == NULL)
  {
    hRes = E_OUTOFMEMORY;
    goto init_err;
  }
  for (i=0; i<nWorkerThreadsCount; i++)
  {
    if (lpWorkerThreads[i].Start(this, &CNktDvTransportAgent::WorkerThreadProc, i) == FALSE)
    {
      hRes = E_OUTOFMEMORY;
      goto init_err;
    }
    sprintf_s(szBufA, NKT_DV_ARRAYLEN(szBufA), "Dv-CommAgent/%lu", (ULONG)i);
    lpWorkerThreads[i].SetThreadName(szBufA);
  }
  return S_OK;
}

VOID CNktDvTransportAgent::Finalize()
{
  TNktLnkLst<CNktDvTransportOverlapped>::Iterator it;
  CNktDvTransportOverlapped *lpOvr;
  SIZE_T i;

  {
    CNktAutoFastMutex cLock(this);

    //signal shutdown
    cShutdownEvent.Set();
    //close pipe
    if (hPipe != NULL)
    {
      ::DisconnectNamedPipe(hPipe);
      ::CloseHandle(hPipe);
      hPipe = NULL;
    }
  }
  //stop the workers
  {
    CNktAutoFastMutex cLock(this);

    if (lpWorkerThreads != NULL && hIOCP != NULL)
    {
      for (i=0; i<nWorkerThreadsCount; i++)
        ::PostQueuedCompletionStatus(hIOCP, 0, 0, NULL);
    }
  }
  if (lpWorkerThreads != NULL)
  {
    for (i=0; i<nWorkerThreadsCount; i++)
      lpWorkerThreads[i].Stop();
  }
  {
    CNktAutoFastMutex cLock(this);

    lpCallback = NULL;
    //destroy worker threads
    if (lpWorkerThreads != NULL)
    {
      delete [] lpWorkerThreads;
      lpWorkerThreads = NULL;
    }
    nWorkerThreadsCount = 0;
    //free buffers
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
    //clear misc variables
    if (hEngineProc != NULL)
    {
      ::CloseHandle(hEngineProc);
      hEngineProc = NULL;
    }
    cBigData.Release();
    //close I/O completion port
    if (hIOCP != NULL)
    {
      ::CloseHandle(hIOCP);
      hIOCP = NULL;
    }
    dwEnginePid = dwMePid = 0;
    lpCallback = NULL;
    cShutdownEvent.Destroy();
  }
  DiscardAllBuffers();
  return;
}

HRESULT CNktDvTransportAgent::StartServer()
{
  //CNktAutoFastMutex cLock(this);

  if (lpCallback == NULL || cShutdownEvent.Wait(0) != FALSE)
    return NKT_DVERR_Cancelled;
  //send connect
  return SendConnectPacket();
}

HRESULT CNktDvTransportAgent::SendMsg(__inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize)
{
  CNktDvTransportOverlapped *lpOvr;
  HRESULT hRes;

  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendMsg]: Code=%lu, Id=%lu",
                     ::GetTickCount(), lpMsg->dwMsgCode & (~NKT_DV_TMSG_CODE_CALLBACK),
                     lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK)));
  if (lpMsg == NULL)
    return E_POINTER;
  if (nMsgSize < sizeof(NKT_DV_TMSG_COMMON) || nMsgSize > NKT_DV_TRANSPORT_MAX_DATA_SIZE)
    return E_INVALIDARG;
  {
    CNktAutoFastMutex cLock(this);

    if (lpCallback == NULL || cShutdownEvent.Wait(0) != FALSE)
      return NKT_DVERR_Cancelled;
    //get a free buffer
    lpOvr = GetBuffer();
    if (lpOvr == NULL)
      return E_OUTOFMEMORY;
    //fill buffer
    nktMemCopy(lpOvr->sMsg.aBuf, lpMsg, nMsgSize);
    //do write prepare
    lpOvr->dwType = XTYPE_WriteRequest;
    lpOvr->nOrder = NktInterlockedIncrement(&nNextWriteOrder);
    lpOvr->dwWrittenBytes = (DWORD)nMsgSize;
    AddBufferToRwList(lpOvr);
    hRes = S_OK;
    if (::PostQueuedCompletionStatus(hIOCP, 0, (ULONG_PTR)this, &(lpOvr->sOvr)) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (FAILED(hRes))
  {
    FreeBuffer(lpOvr);
    return hRes;
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendMsg]: Ovr=%IXh", ::GetTickCount(),
                     lpOvr));
  return S_OK;
}

HRESULT CNktDvTransportAgent::SendBigPacketMsg(__out LPDWORD lpdwBigPacketId, __in LPCVOID lpData,
                                               __in SIZE_T nDataSize, __in DWORD dwTid)
{
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
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendBigPacketMsg]: Size=%Iu",
                     ::GetTickCount(), nDataSize));
  {
    CNktAutoFastMutex cLock(this);

    if (lpCallback == NULL || cShutdownEvent.Wait(0) != FALSE)
      return NKT_DVERR_Cancelled;
    //process the message
    s = (const BYTE *)lpData;
    *lpdwBigPacketId = cBigData->GetNextBuildId();
    for (dwOrderNo=1; nDataSize>0; dwOrderNo++)
    {
      //get a free buffer
      lpOvr = GetBuffer();
      if (lpOvr == NULL)
        return E_OUTOFMEMORY;
      //build message with data
      lpMsgBd = (NKT_DV_TMSG_BIGDATA*)&(lpOvr->sMsg.sMsgCommon);
      hRes = NktDvTransportInitMessage(lpMsgBd, NKT_DV_TMSG_CODE_BigData, GetNextMsgId(), NULL, NULL);
      if (FAILED(hRes))
      {
        FreeBuffer(lpOvr);
        return hRes;
      }
      lpMsgBd->sBdHeader.dwThreadId = dwTid;
      lpMsgBd->sBdHeader.dwId = *lpdwBigPacketId;
      lpMsgBd->sBdHeader.dwOrderNo = dwOrderNo;
      if ((nThisBlockLen=nDataSize) > sizeof(lpMsgBd->aData))
        nThisBlockLen = sizeof(lpMsgBd->aData);
      lpMsgBd->sBdHeader.dwDataLen = (DWORD)nThisBlockLen;
      nktMemCopy(lpMsgBd->aData, s, nThisBlockLen);
      //do write prepare
      lpOvr->dwType = XTYPE_WriteRequest;
      lpOvr->nOrder = NktInterlockedIncrement(&nNextWriteOrder);
      lpOvr->dwWrittenBytes = (DWORD)(sizeof(lpOvr->sMsg.sMsgCommon) + sizeof(lpMsgBd->sBdHeader) +
                                      nThisBlockLen);
      AddBufferToRwList(lpOvr);
      if (::PostQueuedCompletionStatus(hIOCP, 0, (ULONG_PTR)this, &(lpOvr->sOvr)) == FALSE)
      {
        hRes = NKT_HRESULT_FROM_LASTERROR();
        FreeBuffer(lpOvr);
        return hRes;
      }
      //done
      //advance source data
      s += nThisBlockLen;
      nDataSize -= nThisBlockLen;
      NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendBigPacketMsg]: Ovr=%IXh, Id=%lu, "
                         "Order=%lu, Size=%lu", ::GetTickCount(), lpOvr, *lpdwBigPacketId, dwOrderNo,
                         (DWORD)nThisBlockLen));
    }
  }
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendBigPacketMsg]: Done",
                     ::GetTickCount()));
  return S_OK;
}

DWORD CNktDvTransportAgent::GetNextMsgId()
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

BOOL CNktDvTransportAgent::IsTransportThread(__in DWORD dwTid)
{
  SIZE_T i;

  //NOTE: This routine usually is called inside a hook. Because hooks are finalized before transport
  //      using a lock should NOT be necessary
  for (i=0; i<nWorkerThreadsCount; i++)
  {
    if (dwTid == lpWorkerThreads[i].GetThreadId())
      return TRUE;
  }
  return FALSE;
}

VOID CNktDvTransportAgent::WorkerThreadProc(__in SIZE_T nIndex)
{
  LPOVERLAPPED _lpOvr;
  CNktDvTransportOverlapped *lpOvr;
  HRESULT hRes, hCoInitRes;
  HANDLE hEventCopy;
  BOOL b, bCallConnected, bReadPending;
  DWORD dwNumberOfBytes, dwWritten;
  ULONG_PTR ulDummyComplKey;
  SIZE_T i;

  hCoInitRes = S_FALSE;
  //b = ::ImpersonateSelf(SecurityImpersonation);
  //if (b != FALSE)
  //  ::RevertToSelf();
  bCallConnected = FALSE;
  for (;;)
  {
    lpOvr = NULL;
    b = ::GetQueuedCompletionStatus(hIOCP, &dwNumberOfBytes, &ulDummyComplKey, &_lpOvr, INFINITE);
    if (ulDummyComplKey == 0)
    {
      hRes = S_OK;
      break; //a completion key of 0 is posted to the iocp to request us to shut down...
    }
    hRes = (b != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
    {
      bReadPending = FALSE;
      if (_lpOvr != NULL)
      {
        lpOvr = CNktDvTransportOverlapped::ClassFromOverlapped(_lpOvr);
        switch (lpOvr->dwType)
        {
          case XTYPE_RetryConnect:
            if (SUCCEEDED(hRes))
              hRes = SendConnectPacket();
            NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[RetryConnect]: Ovr=%IXh, "
                              "hRes=%08X", ::GetTickCount(), lpOvr, hRes));
            FreeBuffer(lpOvr);
            break;

          case XTYPE_Connect:
            if (SUCCEEDED(hRes))
            {
              //setup read-ahead
              for (i=0; i<READ_AHEAD && SUCCEEDED(hRes); i++)
                hRes = SendReadPacket();
              NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[ConnectCompleted]: Ovr=%IXh, "
                                "hRes=%08X", ::GetTickCount(), lpOvr, hRes));
              bCallConnected = TRUE;
            }
            else
            {
              hRes = SendConnectPacket();
            }
            FreeBuffer(lpOvr);
            break;

          case XTYPE_Read:
            lpOvr->dwReadedBytes = dwNumberOfBytes;
            //add to read queue
            if (SUCCEEDED(hRes))
            {
              AddBufferInSortedReadedQueue(lpOvr);
              hRes = SendReadPacket(); //setup read-ahead
            }
            NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[ReadCompleted]: Ovr=%IXh, "
                              "hRes=%08X", ::GetTickCount(), lpOvr, hRes));
            bReadPending = TRUE;
            break;

          case XTYPE_WriteRequest:
            if (SUCCEEDED(hRes))
              AddBufferInSortedToWriteQueue(lpOvr);
            while (SUCCEEDED(hRes) && cShutdownEvent.Wait(0) == FALSE)
            {
              //get next sequenced block to write
              lpOvr = GetBufferFromSortedToWriteQueue(nNextWriteOrderToProcess);
              if (lpOvr == NULL)
                break;
              if (lpOvr->dwType != XTYPE_Discard)
              {
                //do real write
                AddBufferToRwList(lpOvr);
                lpOvr->dwType = XTYPE_Write;
                if (lpOvr->sMsg.sMsgCommon.hProcessedEvent != NULL)
                {
                  hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(),
                                                      (HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent), hEngineProc,
                                                      &hEventCopy, 0, FALSE, DUPLICATE_SAME_ACCESS);
                  if (SUCCEEDED(hRes))
                    lpOvr->sMsg.sMsgCommon.hProcessedEvent = NKT_PTR_2_ULONGLONG(hEventCopy);
                }
                if (SUCCEEDED(hRes))
                {
                  if (::WriteFile(hPipe, &(lpOvr->sMsg), lpOvr->dwWrittenBytes, &dwWritten,
                                  &(lpOvr->sOvr)) == FALSE)
                  {
                    hRes = NKT_HRESULT_FROM_LASTERROR();
                    if (hRes == NKT_HRESULT_FROM_WIN32(ERROR_IO_PENDING))
                      hRes = S_OK;
                  }
                }
                if (FAILED(hRes))
                  FreeBuffer(lpOvr);
                NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[Write]: Ovr=%IXh, hRes=%08X",
                                   ::GetTickCount(), lpOvr, hRes));
              }
              else
              {
                FreeBuffer(lpOvr);
              }
              NktInterlockedIncrement(&nNextWriteOrderToProcess);
            }
            break;

          case XTYPE_Write:
            if (SUCCEEDED(hRes) && lpOvr->dwWrittenBytes != dwNumberOfBytes)
              hRes = NKT_HRESULT_FROM_WIN32(ERROR_WRITE_FAULT);
            NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[WriteCompleted]: Ovr=%IXh, "
                                                    "hRes=%08X",
                               ::GetTickCount(), lpOvr, hRes));
            FreeBuffer(lpOvr);
            break;

          case XTYPE_Discard:
            FreeBuffer(lpOvr);
            break;

          default:
            NKT_ASSERT(FALSE);
            FreeBuffer(lpOvr);
            break;
        }
      }
      else
      {
        hRes = NKT_DVERR_InvalidTransportData;
      }
      if (SUCCEEDED(hRes) && bReadPending != FALSE)
      {
        while (SUCCEEDED(hRes) && cShutdownEvent.Wait(0) == FALSE)
        {
          //get next sequenced block readed
          lpOvr = GetBufferFromSortedReadedQueue(nNextReadOrderToProcess);
          if (lpOvr == NULL)
            break;
          if (lpOvr->dwReadedBytes >= sizeof(NKT_DV_TMSG_COMMON) &&
              lpOvr->dwReadedBytes <= NKT_DV_TRANSPORT_MAX_DATA_SIZE)
          {
            if (lpOvr->dwType != XTYPE_Discard)
            {
              //check for a big data packet
              if (lpOvr->sMsg.sMsgCommon.dwMsgCode == NKT_DV_TMSG_CODE_BigData)
              {
                hRes = cBigData->ProcessPacket(&(lpOvr->sMsg.sMsgCommon));
              }
              else
              {
                //process standard packets
                InitializeWorkerThreadCOM(hCoInitRes, &(lpOvr->sMsg.sMsgCommon), (SIZE_T)(lpOvr->dwReadedBytes));
                hRes = lpCallback->TAC_OnEngineMessage(this, &(lpOvr->sMsg.sMsgCommon),
                                                       (SIZE_T)(lpOvr->dwReadedBytes), cBigData);
              }
              //signal server message event
              if (SUCCEEDED(hRes))
                hRes = SignalProcessedEvent(lpOvr);
            }
          }
          else
          {
            hRes = NKT_DVERR_InvalidTransportData;
          }
          FreeBuffer(lpOvr);
          NktInterlockedIncrement(&nNextReadOrderToProcess);
        }
      }
    }
    if (FAILED(hRes))
      break;
    if (bCallConnected != FALSE)
    {
      bCallConnected = FALSE;
      lpCallback->TAC_OnConnectionEstablished(this);
    }
  }
  //ending
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[Ending-%lu]: hRes=%08X", ::GetTickCount(),
                     (ULONG)nIndex, hRes));
  if (NktInterlockedCompareExchange(&nThreadsEnding, 1, 0) == 0)
  {
    ::CancelIo(hPipe);
    if (FAILED(hRes))
    {
      if (hRes != NKT_HRESULT_FROM_WIN32(ERROR_PIPE_NOT_CONNECTED) &&
          hRes != NKT_HRESULT_FROM_WIN32(ERROR_BROKEN_PIPE))
      {
        if (cShutdownEvent.Wait(0) == FALSE)
          lpCallback->TAC_OnTransportError(this, hRes);
      }
      else
      {
        hRes = S_OK;
      }
    }
    ::DisconnectNamedPipe(hPipe);
    lpCallback->TAC_OnConnectionClosed(this, hRes);
  }
  if (hCoInitRes == S_OK)
    ::CoUninitialize();
  return;
}

CNktDvTransportOverlapped* CNktDvTransportAgent::GetBuffer()
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
    nCount = CNktDvRegistrySettings::GetAgentTransportMaxInUseMessageCount();
    if (NktInterlockedIncrement(&nInUseMessageCount) >= nCount)
    {
      dwDelay = CNktDvRegistrySettings::GetAgentTransportMessageDelay();
      for (nRetries=30; nRetries>0 && nInUseMessageCount>=nCount; nRetries--)
        ::Sleep(dwDelay);
    }
  }
  return lpOvr;
}

VOID CNktDvTransportAgent::FreeBuffer(__inout CNktDvTransportOverlapped *lpOvr)
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

VOID CNktDvTransportAgent::DiscardAllBuffers()
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

VOID CNktDvTransportAgent::AddBufferToRwList(__inout CNktDvTransportOverlapped *lpOvr)
{
  CNktAutoFastMutex cBufferListLock(&cBufferListMtx);

  cActiveRwBufferList.PushTail(lpOvr);
  return;
}

VOID CNktDvTransportAgent::AddBufferInSortedReadedQueue(__inout CNktDvTransportOverlapped *lpOvr)
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

VOID CNktDvTransportAgent::AddBufferInSortedToWriteQueue(__inout CNktDvTransportOverlapped *lpOvr)
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

CNktDvTransportOverlapped* CNktDvTransportAgent::GetBufferFromSortedReadedQueue(
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

CNktDvTransportOverlapped* CNktDvTransportAgent::GetBufferFromSortedToWriteQueue(
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

HRESULT CNktDvTransportAgent::SendConnectPacket()
{
  CNktDvTransportOverlapped *lpOvr;
  HRESULT hRes;

  //prepare buffer
  lpOvr = GetBuffer();
  if (lpOvr == NULL)
    return E_OUTOFMEMORY;
  //do connect
  {
    CNktAutoFastMutex cLock(this);

    lpOvr->dwType = XTYPE_Connect;
    AddBufferToRwList(lpOvr);
    if (::ConnectNamedPipe(hPipe, &(lpOvr->sOvr)) == FALSE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      if (hRes != NKT_HRESULT_FROM_WIN32(ERROR_PIPE_CONNECTED) &&
          hRes != NKT_HRESULT_FROM_WIN32(ERROR_IO_PENDING))
      {
        //on error try again on next round
        lpOvr->dwType = XTYPE_RetryConnect;
        if (::PostQueuedCompletionStatus(hIOCP, 0, (ULONG_PTR)this, &(lpOvr->sOvr)) == FALSE)
        {
          hRes = NKT_HRESULT_FROM_LASTERROR();
          FreeBuffer(lpOvr);
          return hRes;
        }
      }
    }
  }
  return S_OK;
}

HRESULT CNktDvTransportAgent::SendReadPacket()
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
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportAgent[SendReadPacket]: Ovr=%IXh%s",
                     ::GetTickCount(), lpOvr, (hRes != S_OK) ? L" (pending)" : L""));
  return S_OK;
}

VOID CNktDvTransportAgent::FillSecurity(__out SECURITY_ATTRIBUTES &sSecAttrib,
                                        __out SECURITY_DESCRIPTOR &sSecDesc)
{
  nktMemSet(&sSecDesc, 0, sizeof(sSecDesc));
  sSecDesc.Revision = SECURITY_DESCRIPTOR_REVISION1;
  //::InitializeSecurityDescriptor(&sSecDesc, SECURITY_DESCRIPTOR_REVISION);
  //::SetSecurityDescriptorDacl(&sSecDesc, TRUE, NULL, FALSE);
  sSecDesc.Control |= SE_DACL_PRESENT;
  sSecDesc.Control &= ~(SE_DACL_DEFAULTED);
  sSecAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  sSecAttrib.bInheritHandle = TRUE;
  sSecAttrib.lpSecurityDescriptor = &sSecDesc;
  return;
}

HRESULT CNktDvTransportAgent::SignalProcessedEvent(__inout CNktDvTransportOverlapped *lpOvr)
{
  HRESULT hRes;

  hRes = S_OK;
  if (lpOvr->sMsg.sMsgCommon.hProcessedEvent != NULL)
  {
    //signal server message event
    if (::SetEvent((HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent)) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    ::CloseHandle((HANDLE)(lpOvr->sMsg.sMsgCommon.hProcessedEvent));
  }
  return hRes;
}

VOID CNktDvTransportAgent::InitializeWorkerThreadCOM(__inout HRESULT &hCoInitRes, __inout NKT_DV_TMSG_COMMON *lpMsg,
                                                     __in SIZE_T nMsgSize)
{
  /*
  if (hCoInitRes == S_FALSE)
  {
    BOOL b = FALSE;

    switch (lpMsg->dwMsgCode)
    {
      case NKT_DV_TMSG_CODE_AddHook:
        if (nMsgSize >= sizeof(NKT_DV_TMSG_ADDHOOK))
        {
          NKT_DV_TMSG_ADDHOOK *m = reinterpret_cast<NKT_DV_TMSG_ADDHOOK*>(lpMsg);
          if (m->dwCustomHandlersBigPacketId != 0)
            b = TRUE; //if addhook contains a custom handler => initialize COM
        }
        break;
    }
    if (b != FALSE)
      hCoInitRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED|COINIT_DISABLE_OLE1DDE|COINIT_SPEED_OVER_MEMORY);
  }
  */
  return;
}
