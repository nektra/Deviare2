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

//-----------------------------------------------------------

class CNktDvBaseShutdownMutex : public CNktMemMgrObj
{
public:
  CNktDvBaseShutdownMutex();

  virtual HRESULT Initialize();

  __inline BOOL IsSystemActive()
    {
    return (nSystemActive != 0) ? TRUE : FALSE;
    };

  virtual VOID Lock()=0;
  virtual VOID Unlock()=0;

  virtual BOOL IsShuttingDown()=0;

protected:
  virtual VOID StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex);

private:
  LONG volatile nSystemActive;
};

//-----------------------------------------------------------

class CNktDvShutdownMutex : public CNktDvBaseShutdownMutex
{
public:
  CNktDvShutdownMutex();
  virtual ~CNktDvShutdownMutex();

  virtual HRESULT Initialize();
  virtual VOID Finalize();

  virtual BOOL SignalShutdown();
  virtual VOID StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex);

  virtual VOID Lock();
  virtual VOID Unlock();

  virtual BOOL IsShuttingDown();

  virtual HANDLE GetEvent();

private:
  CNktFastMutex cMtx;
  CNktEvent cShutdownEvent;
  CNktEvent cNoLockEvent;
  LONG volatile nLockCount;
};

//-----------------------------------------------------------

class CNktDvLightShutdownMutex : public CNktDvBaseShutdownMutex
{
public:
  CNktDvLightShutdownMutex();
  virtual ~CNktDvLightShutdownMutex();

  virtual HRESULT Initialize();
  virtual VOID Finalize();

  virtual VOID StartShutdownAndWaitPending(__inout CNktFastMutex *lpMutex);

  virtual VOID Lock();
  virtual VOID Unlock();

  virtual BOOL IsShuttingDown();

private:
  LONG volatile nShutdownSignal;
  LONG volatile nNoLockSignal;
  LONG volatile nLockCount;
};

//-----------------------------------------------------------

class CNktDvShutdownAutoLock
{
public:
  CNktDvShutdownAutoLock(__inout CNktDvBaseShutdownMutex *_lpLock)
    {
    lpLock = _lpLock;
    if (lpLock != NULL)
      lpLock->Lock();
    return;
    };

  virtual ~CNktDvShutdownAutoLock()
    {
    if (lpLock != NULL)
      lpLock->Unlock();
    return;
    };

private:
  CNktDvBaseShutdownMutex *lpLock;
};

//-----------------------------------------------------------

class CNktDvAutoCancellableFastMutex
{
public:
  CNktDvAutoCancellableFastMutex(__inout CNktFastMutex *lpFastMutex, __in CNktDvBaseShutdownMutex *lpSdMutex)
    {
    BOOL bIsMultiProcessor;

    NKT_ASSERT(lpFastMutex != NULL);
    NKT_ASSERT(lpSdMutex != NULL);
    lpFM = lpFastMutex;
    lpSD = lpSdMutex;
    bLockHeld = FALSE;
    bIsMultiProcessor = CNktFastMutex::IsMultiProcessor();
    while (lpSdMutex->IsShuttingDown() == FALSE)
    {
      if (lpFM->TryLock() != FALSE)
      {
        bLockHeld = TRUE;
        break;
      }
      if (bIsMultiProcessor == FALSE)
        ::SwitchToThread();
      else
        ::Sleep(1);
    }
    return;
    };

  virtual ~CNktDvAutoCancellableFastMutex()
    {
    if (bLockHeld != FALSE)
      lpFM->Unlock();
    return;
    };

  virtual BOOL IsLockHeld() const
    {
    return bLockHeld;
    };

private:
  CNktFastMutex *lpFM;
  CNktDvBaseShutdownMutex *lpSD;
  BOOL bLockHeld;
};

//-----------------------------------------------------------

