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

#include "ShutdownMutex.h"

//-----------------------------------------------------------

CNktDvBaseShutdownMutex::CNktDvBaseShutdownMutex() : CNktMemMgrObj()
{
  NktInterlockedExchange(&nSystemActive, 0);
  return;
}

HRESULT CNktDvBaseShutdownMutex::Initialize()
{
  NktInterlockedExchange(&nSystemActive, 1);
  return S_OK;
}

VOID CNktDvBaseShutdownMutex::StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex)
{
  NktInterlockedExchange(&nSystemActive, 0);
  return;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvShutdownMutex::CNktDvShutdownMutex() : CNktDvBaseShutdownMutex()
{
  NktInterlockedExchange(&nLockCount, 0);
  return;
}

CNktDvShutdownMutex::~CNktDvShutdownMutex()
{
  Finalize();
  //NKT_ASSERT(nLockCount == 0); may be != 0 if app is terminating
  return;
}

HRESULT CNktDvShutdownMutex::Initialize()
{
  CNktAutoFastMutex cLock(&cMtx);

  Finalize();
  //----
  CNktDvBaseShutdownMutex::Initialize();
  if (cShutdownEvent.Create(TRUE, FALSE) == FALSE ||
      cNoLockEvent.Create(TRUE, TRUE) == FALSE)
  {
    Finalize();
    return E_OUTOFMEMORY;
  }
  return S_OK;
}

VOID CNktDvShutdownMutex::Finalize()
{
  CNktAutoFastMutex cLock(&cMtx);

  cNoLockEvent.Destroy();
  cShutdownEvent.Destroy();
  //----
  CNktDvBaseShutdownMutex::StartShutdownAndWaitPending(NULL);
  return;
}

BOOL CNktDvShutdownMutex::SignalShutdown()
{
  BOOL bWait;

  bWait = FALSE;
  {
    CNktAutoFastMutex cLock(&cMtx);

    if (cShutdownEvent.GetEventHandle() != NULL)
    {
      cShutdownEvent.Set();
      bWait = TRUE;
    }
  }
  return bWait;
}

VOID CNktDvShutdownMutex::StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex)
{
  if (SignalShutdown() != FALSE)
  {
    HANDLE hEv;

    hEv = cNoLockEvent.GetEventHandle();
    CNktDvBaseShutdownMutex::StartShutdownAndWaitPending(lpMutex);
    if (lpMutex != NULL)
    {
      CNktAutoFastMutexReverse cLockRev(lpMutex);

      CNktThread::CoWaitAndDispatchMessages(INFINITE, 1, &hEv);
    }
    else
    {
      CNktThread::CoWaitAndDispatchMessages(INFINITE, 1, &hEv);
    }
  }
  return;
}

VOID CNktDvShutdownMutex::Lock()
{
  CNktAutoFastMutex cLock(&cMtx);

  if (NktInterlockedIncrement(&nLockCount) == 1)
    cNoLockEvent.Reset();
  return;
}

VOID CNktDvShutdownMutex::Unlock()
{
  CNktAutoFastMutex cLock(&cMtx);

  if (NktInterlockedDecrement(&nLockCount) == 0)
    cNoLockEvent.Set();
  return;
}

BOOL CNktDvShutdownMutex::IsShuttingDown()
{
  return cShutdownEvent.Wait(0);
}

HANDLE CNktDvShutdownMutex::GetEvent()
{
  return cShutdownEvent.GetEventHandle();
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvLightShutdownMutex::CNktDvLightShutdownMutex() : CNktDvBaseShutdownMutex()
{
  NktInterlockedExchange(&nLockCount, 0);
  NktInterlockedExchange(&nShutdownSignal, 0);
  return;
}

CNktDvLightShutdownMutex::~CNktDvLightShutdownMutex()
{
  Finalize();
  NKT_ASSERT(nLockCount == 0);
  return;
}

HRESULT CNktDvLightShutdownMutex::Initialize()
{
  Finalize();
  //----
  CNktDvBaseShutdownMutex::Initialize();
  NktInterlockedExchange(&nShutdownSignal, 0);
  NktInterlockedExchange(&nNoLockSignal, 1);
  return S_OK;
}

VOID CNktDvLightShutdownMutex::Finalize()
{
  NktInterlockedExchange(&nShutdownSignal, 0);
  //----
  CNktDvBaseShutdownMutex::StartShutdownAndWaitPending(NULL);
  return;
}

VOID CNktDvLightShutdownMutex::StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex)
{
  NktInterlockedExchange(&nShutdownSignal, 1);
  CNktDvBaseShutdownMutex::StartShutdownAndWaitPending(lpMutex);
  while (nLockCount > 0)
  {
    if (lpMutex != NULL)
    {
      CNktAutoFastMutexReverse cLockRev(lpMutex);

      ::Sleep(1);
    }
    else
      ::Sleep(1);
  }
  return;
}

VOID CNktDvLightShutdownMutex::Lock()
{
  NktInterlockedIncrement(&nLockCount);
  return;
}

VOID CNktDvLightShutdownMutex::Unlock()
{
  NktInterlockedDecrement(&nLockCount);
  return;
}

BOOL CNktDvLightShutdownMutex::IsShuttingDown()
{
  return (nShutdownSignal != 0) ? TRUE : FALSE;
}
