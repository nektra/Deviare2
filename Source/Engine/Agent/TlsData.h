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

#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\TransportCommon.h"
#include "..\..\Common\LinkedList.h"

//-----------------------------------------------------------

class CNktDvHookCallDataBase : public CNktMemMgrObj, public TNktLnkLstNode<CNktDvHookCallDataBase>
{
public:
  CNktDvHookCallDataBase() : CNktMemMgrObj(), TNktLnkLstNode<CNktDvHookCallDataBase>()
    {
    return;
    };

  virtual ~CNktDvHookCallDataBase()
    {
    RemoveNode();
    return;
    };
};

//-----------------------------------------------------------

class CNktDvTlsData : public CNktDvObject, public TNktLnkLstNode<CNktDvTlsData>
{
public:
  CNktDvTlsData();
  virtual ~CNktDvTlsData();

  HRESULT Initialize();

  DWORD GetTid()
    {
    return dwTid;
    };

  VOID ThreadTimesAccumulate(__in FILETIME &ftCreationTime, __in double nCurrentKernelTimeMs,
                             __in double nCurrentUserTimeMs, __in ULONG64 nCurrentCpuClockCycles);
  VOID ThreadTimesSetLast(__in double nCurrentKernelTimeMs, __in double nCurrentUserTimeMs,
                          __in ULONG64 nCurrentCpuClockCycles);

public:
  BOOL bInCallLock;
  //----
  struct tagHookCallData {
    TNktLnkLst<CNktDvHookCallDataBase> cInUseStack;
    TNktLnkLst<CNktDvHookCallDataBase> cFreeList;
  } sHookCallData;
  //----
  struct {
    LONG nInitFlags;
    SYSTEMTIME sCreationTime;
    double nKernelTimeMs;
    double nUserTimeMs;
    ULONG64 nCpuClockCycles;
    struct {
      double nKernelTimeMs;
      double nUserTimeMs;
      ULONG64 nCpuClockCycles;
    } sLast;
  } sThreadTimes;
  //----
  BOOL bSendLoadedUnloadedDllAsync;

private:
  DWORD dwTid;
  LONG volatile nRefCount;
  LONG volatile nIsDestroying;
};

//-----------------------------------------------------------

HRESULT nktDvTlsData_Initialize();
VOID nktDvTlsData_Finalize();

HRESULT nktDvTlsData_Get(__out CNktDvTlsData **lplpTlsData, __in BOOL bCreate=TRUE);
VOID nktDvTlsData_OnThreadExit();

BOOL nktDvTlsData_CheckInUseStack(__in DWORD dwTid);

//-----------------------------------------------------------

