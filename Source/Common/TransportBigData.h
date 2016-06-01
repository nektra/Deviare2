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

#include "EngBaseObj.h"
#include "TransportCommon.h"

//-----------------------------------------------------------

class CNktDvTransportBigData : public CNktDvObject
{
public:
  CNktDvTransportBigData();
  virtual ~CNktDvTransportBigData();

  virtual VOID Reset();

  virtual HRESULT ProcessPacket(__in NKT_DV_TMSG_COMMON *lpMsg);

  virtual DWORD GetNextBuildId();

  virtual SIZE_T GetLength(__in DWORD dwId);
  virtual LPBYTE GetData(__in DWORD dwId);
  virtual VOID Remove(__in DWORD dwId);

  virtual VOID Lock()
    {
    cMtx.Lock();
    return;
    };

  virtual VOID Unlock()
    {
    cMtx.Unlock();
    return;
    };

  class CParser
  {
  public:
    CParser()
      {
      dwPacketId = 0;
      lpData = NULL;
      nLength = 0;
      return;
      };
    CParser(__in CNktDvTransportBigData *lpBigData, __in DWORD dwPacketId)
      {
      Setup(lpBigData, dwPacketId);
      return;
      };
    virtual ~CParser()
      {
      if (dwPacketId != 0)
        cBigData->Remove(dwPacketId);
      return;
      };

    virtual VOID Setup(__in CNktDvTransportBigData *lpBigData, __in DWORD _dwPacketId)
      {
      NKT_ASSERT(lpBigData != NULL);
      cBigData = lpBigData;
      dwPacketId = _dwPacketId;
      if (dwPacketId != 0)
      {
        lpData = cBigData->GetData(dwPacketId);
        nLength = cBigData->GetLength(dwPacketId);
        NKT_ASSERT(lpData != NULL);
        NKT_ASSERT(nLength > 0);
      }
      else
      {
        lpData = NULL;
        nLength = 0;
      }
      return;
      };

    virtual LPBYTE GetData() const
      {
      return lpData;
      };
    virtual SIZE_T GetLength() const
      {
      return nLength;
      };

  private:
    TNktComPtr<CNktDvTransportBigData> cBigData;
    DWORD dwPacketId;
    SIZE_T nLength;
    LPBYTE lpData;
  };

private:
  typedef struct tagBIGDATA_BLOCK {
    DWORD dwId;
    LONG volatile nLastOrderNo;
    LPBYTE lpData;
    SIZE_T nLen, nSize;
  } BIGDATA_BLOCK;

  class BIGDATA_BLOCK_Remove
  {
  public:
    virtual VOID OnItemFinalize(__inout BIGDATA_BLOCK& item)
      {
      if (item.lpData != NULL)
        nktMemFree(item.lpData);
      return;
      };
  };

  static int BlockCompare(__in void *, __in BIGDATA_BLOCK *lpElem1, __in BIGDATA_BLOCK *lpElem2);

  CNktFastMutex cMtx;
  TNktArrayList4Structs<BIGDATA_BLOCK,32,BIGDATA_BLOCK_Remove> aBlocksList;
  LONG volatile nNextBuildPacketId;
};

//-----------------------------------------------------------

