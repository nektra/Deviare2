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

#include "TransportBigData.h"

//-----------------------------------------------------------

CNktDvTransportBigData::CNktDvTransportBigData() : CNktDvObject()
{
  NktInterlockedExchange(&nNextBuildPacketId, 0);
  return;
}

CNktDvTransportBigData::~CNktDvTransportBigData()
{
  Reset();
  return;
}

VOID CNktDvTransportBigData::Reset()
{
  CNktAutoFastMutex cLock(&cMtx);

  aBlocksList.RemoveAllElements();
  //----
  NktInterlockedExchange(&nNextBuildPacketId, 0);
  return;
}

HRESULT CNktDvTransportBigData::ProcessPacket(__in NKT_DV_TMSG_COMMON *lpMsg)
{
  CNktAutoFastMutex cLock(&cMtx);
  BIGDATA_BLOCK sBlock, *lpBlock;
  LPBYTE lpPtr;
  SIZE_T k, nLen;
  HRESULT hRes;

  NKT_ASSERT(lpMsg != NULL);
  NKT_ASSERT(lpMsg->dwMsgCode == NKT_DV_TMSG_CODE_BigData);
  //find block or create a new one
  k = 0;
  hRes = S_OK;
  sBlock.dwId = ((NKT_DV_TMSG_BIGDATA*)lpMsg)->sBdHeader.dwId;
  lpBlock = aBlocksList.BinarySearchPtr(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL);
  if (lpBlock != NULL && ((NKT_DV_TMSG_BIGDATA*)lpMsg)->sBdHeader.dwOrderNo == 1)
  {
    //reset length if "id" already existed (this only can occur if the "id" sent by the
    //other party did a round-trip but the bigdata was not processed (usually by an error)
    NktInterlockedExchange(&(lpBlock->nLastOrderNo), 0);
    lpBlock->nLen = 0;
    k = 1;
  }
  else if (lpBlock == NULL)
  {
    NktInterlockedExchange(&(sBlock.nLastOrderNo), 0);
    sBlock.lpData = (LPBYTE)nktMemMalloc(sBlock.nSize = 32768);
    if (sBlock.lpData != NULL)
    {
      sBlock.nLen = 0;
      if (aBlocksList.SortedInsert(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL) == FALSE)
      {
        nktMemFree(sBlock.lpData);
        hRes = E_OUTOFMEMORY;
      }
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    if (SUCCEEDED(hRes))
    {
      lpBlock = aBlocksList.BinarySearchPtr(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL);
      NKT_ASSERT(lpBlock != NULL);
      k = 2;
    }
  }
  //check block order
  if (SUCCEEDED(hRes))
  {
    if (((NKT_DV_TMSG_BIGDATA*)lpMsg)->sBdHeader.dwOrderNo != (DWORD)(lpBlock->nLastOrderNo)+1)
      hRes = E_FAIL;
  }
  //add data
  if (SUCCEEDED(hRes))
  {
    nLen = (SIZE_T)(((NKT_DV_TMSG_BIGDATA*)lpMsg)->sBdHeader.dwDataLen);
    if (nLen > 0)
    {
      if (nLen+lpBlock->nLen < nLen)
      {
        hRes = E_OUTOFMEMORY; //overflow
      }
      else if (nLen+lpBlock->nLen > lpBlock->nSize)
      {
        k = (nLen + lpBlock->nLen + 32767) & (~32767);
        lpPtr = (LPBYTE)nktMemRealloc(lpBlock->lpData, k);
        if (lpPtr != NULL)
        {
          lpBlock->lpData = lpPtr;
          lpBlock->nSize = k;
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
      }
      if (SUCCEEDED(hRes))
      {
        nktMemCopy(lpBlock->lpData+lpBlock->nLen, ((NKT_DV_TMSG_BIGDATA*)lpMsg)->aData, nLen);
        lpBlock->nLen += nLen;
      }
    }
  }
  if (SUCCEEDED(hRes))
  {
    NktInterlockedIncrement(&(lpBlock->nLastOrderNo));
  }
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportBigData[ProcessPacket]: Id=%lu, "
                    "Order=%lu(%lu)%s, hRes=%08X", ::GetTickCount(), sBlock.dwId,
                    (lpBlock != NULL) ? (DWORD)(lpBlock->nLastOrderNo) : 0,
                    ((NKT_DV_TMSG_BIGDATA*)lpMsg)->sBdHeader.dwOrderNo, (k == 0) ? "" :
                    ((k==1) ? " (recycle)" : " (new)"), hRes));
  return hRes;
}

DWORD CNktDvTransportBigData::GetNextBuildId()
{
  LONG nId;

  do {
    nId = NktInterlockedIncrement(&nNextBuildPacketId);
  }
  while (nId == 0);
  return (DWORD)nId;
}

SIZE_T CNktDvTransportBigData::GetLength(__in DWORD dwId)
{
  CNktAutoFastMutex cLock(&cMtx);
  BIGDATA_BLOCK sBlock, *lpBlock;

  sBlock.dwId = dwId;
  lpBlock = aBlocksList.BinarySearchPtr(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL);
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportBigData[GetLength]: Id=%lu%s", ::GetTickCount(),
                     dwId, (lpBlock!=NULL) ? "" : " (not found)"));
  return (lpBlock != NULL) ? (lpBlock->nLen) : NULL;
}

LPBYTE CNktDvTransportBigData::GetData(__in DWORD dwId)
{
  CNktAutoFastMutex cLock(&cMtx);
  BIGDATA_BLOCK sBlock, *lpBlock;

  sBlock.dwId = dwId;
  lpBlock = aBlocksList.BinarySearchPtr(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL);
  return (lpBlock != NULL) ? (lpBlock->lpData) : NULL;
}

VOID CNktDvTransportBigData::Remove(__in DWORD dwId)
{
  CNktAutoFastMutex cLock(&cMtx);
  BIGDATA_BLOCK sBlock;
  SIZE_T nIndex;

  sBlock.dwId = dwId;
  nIndex = aBlocksList.BinarySearch(&sBlock, &CNktDvTransportBigData::BlockCompare, NULL);
  NKT_DEBUGPRINTLNA(Nektra::dlTransport, ("%lu) TransportBigData[Release]: Id=%lu%s", ::GetTickCount(),
                     dwId, (nIndex!=NKT_SIZE_T_MAX) ? "" : " (not found)"));
  if (nIndex != NKT_SIZE_T_MAX)
    aBlocksList.RemoveElementAt(nIndex);
  return;
}

int CNktDvTransportBigData::BlockCompare(__in void *, __in BIGDATA_BLOCK *lpElem1,
                                         __in BIGDATA_BLOCK *lpElem2)
{
  return NKT_DV_CMP<DWORD>(lpElem1->dwId, lpElem2->dwId);
}
