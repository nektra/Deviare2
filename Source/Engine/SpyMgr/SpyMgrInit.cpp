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

#include "SpyMgr.h"
#include "..\..\Common\ProcessMemory.h"
#include "..\..\Common\Tools.h"
#include "..\..\Common\ProcessHandle.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#if defined _M_IX86
  #define HOOKID_XOR 0x83EFB548UL
#elif defined _M_X64
  #define HOOKID_XOR 0x83EFB5486BC3921Bui64
#endif

//-----------------------------------------------------------

HRESULT CNktDvSpyMgr::Initialize(__in_z LPCWSTR szDatabasePathW, __in_z LPCWSTR szAgentPathW,
                                 __in DWORD dwFlags)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;

  NktInterlockedExchange(&nNextHookId, 0);
  //initialize shutdown locker
  hRes = cShutdownMtx.Initialize();
  //set agent path
  if (SUCCEEDED(hRes))
  {
    if (szAgentPathW != NULL && szAgentPathW[0] != 0)
    {
      if (cStrAgentPathW.Copy(szAgentPathW) != FALSE)
        hRes = CNktDvTools::FixFolderStr(cStrAgentPathW);
      else
        hRes = E_OUTOFMEMORY;
    }
    else
    {
      hRes = CNktDvTools::GetModulePath(cStrAgentPathW, NULL);
    }
  }
  //load database(s)
  if (SUCCEEDED(hRes))
  {
    HANDLE hFileMap;

    cDvDB_32.Release();
    hRes = CNktDvEngDatabase::CreateMapping(&hFileMap, (szDatabasePathW != NULL) ? szDatabasePathW : L"", 32);
    if (SUCCEEDED(hRes))
    {
      hRes = CNktDvEngDatabase::Get(&cDvDB_32, hFileMap, 32);
      if (FAILED(hRes))
        ::CloseHandle(hFileMap);
    }
  }
#if defined _M_X64
  if (SUCCEEDED(hRes))
  {
    HANDLE hFileMap;

    cDvDB_64.Release();
    hRes = CNktDvEngDatabase::CreateMapping(&hFileMap, (szDatabasePathW != NULL) ? szDatabasePathW : L"", 64);
    if (SUCCEEDED(hRes))
    {
      hRes = CNktDvEngDatabase::Get(&cDvDB_64, hFileMap, 64);
      if (FAILED(hRes))
        ::CloseHandle(hFileMap);
    }
  }
#endif //_M_X64
  //init process handle manager
  if (SUCCEEDED(hRes))
  {
    cProcHndlMgr.Release();
    hRes = CNktDvProcessHandlesMgr::Get(&cProcHndlMgr);
  }
  //initialize process list updater
  if (SUCCEEDED(hRes))
    hRes = cProcList.Initialize();
  //initialize transport manager
  if (SUCCEEDED(hRes))
  {
    SIZE_T nProcCount = CNktDvTools::GetProcessorsCount();
    hRes = cTransport.Initialize(NKT_DV_TRANSPORTENGINE_WORKERTHREADS_PER_CPU * nProcCount,
                                 NKT_DV_TRANSPORTENGINE_DISPATCHERTHREADS_PER_CPU * nProcCount);
  }
  //initialize internal worker
  if (SUCCEEDED(hRes))
  {
    hRes = (sDelayedHook.cAvailableEvent.Create(TRUE, FALSE) != FALSE) ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hRes))
    {
      if (Start() != FALSE)
        SetThreadName("Dv-SpyManagerMain");
      else
        hRes = E_OUTOFMEMORY;
    }
  }
  //done
  if (FAILED(hRes))
    Finalize();
  return hRes;
}

VOID CNktDvSpyMgr::Finalize()
{
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvHooksEnumerator> cHookEnum;
  TNktComPtr<CNktDvProcess> cProc;
  CAgentController *lpAgentCtl;
  CNktDvHook::eState nOldState;
  SIZE_T i, nCount;

  cShutdownMtx.StartShutdownAndWaitPending(&GetLockObject());
  Stop();
  sDelayedHook.cAvailableEvent.Destroy();
  sDelayedHook.aItems.RemoveAllElements();
  cProcList.Finalize();
  //inform all hook items as being removed
  for (;;)
  {
    nCount = aHookItemsListByHookId.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (aHookItemsListByHookId[i]->nState != CNktDvHook::stRemoved)
        break;
    }
    if (i >= nCount)
      break;
    //remove this item
    nOldState = aHookItemsListByHookId[i]->nState;
    aHookItemsListByHookId[i]->nState = CNktDvHook::stRemoved; //avoid double removal
    cHook = aHookItemsListByHookId[i]->cHook;
    cProc = aHookItemsListByHookId[i]->cProc;
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
    }
    RemoveHookItem(aHookItemsListByHookId[i]);
  }
  //remove agent controllers
  while (aAgentControllersList.GetCount() > 0)
  {
    lpAgentCtl = aAgentControllersList[0];
    aAgentControllersList.RemoveElementAt(0);
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      delete lpAgentCtl;
    }
  }
  //remove hook items
  while (aHookItemsListByHookId.GetCount() > 0)
    RemoveHookItem(aHookItemsListByHookId[i]);
  //kill transport
  cTransport.Finalize();
  //detach hooks
  while (1)
  {
    {
      CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

      cHook = sHooks.cList.PopHead();
    }
    if (cHook == NULL)
      break;
    cHook->Unlink();
    ((CNktDvHook*)cHook)->Release(); //release the internal hook reference
  }
  //detach hook enumerations
  while (1)
  {
    {
      CNktSimpleLockNonReentrant cHooksEnumLock(&(sHooksEnum.nMutex));

      cHookEnum = sHooksEnum.cList.PopHead();
    }
    if (cHookEnum == NULL)
      break;
    cHookEnum->Unlink();
  }
  //kill shutdown mutex
  cShutdownMtx.Finalize();
  //flush objects cache lists
  cDvDB_32.Release();
#if defined _M_X64
  cDvDB_64.Release();
#endif //_M_X64
  cProcHndlMgr.Release();
  CNktDvModule::FlushCache();
  CNktDvProcess::FlushCache();
  cStrAgentPathW.Empty();
  return;
}

VOID CNktDvSpyMgr::ThreadProc()
{
  HANDLE hEvents[1];
  DWORD dwTimeout, dwHitEvent;

  OnThreadStarted();
  //----
  hEvents[0] = sDelayedHook.cAvailableEvent.GetEventHandle();
  dwTimeout = INFINITE;
  while (CheckForAbort(dwTimeout, 1, hEvents, &dwHitEvent) == FALSE)
  {
    dwTimeout = INFINITE;
    {
      CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
      CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
      TNktComPtr<CNktDvProcess> cProc;
      DWORD dw, dwChildPid;
      SIZE_T i, nCount;
      HANDLE hProc;
      HRESULT hRes;
      BOOL bHandled;
      FILETIME sCreationTime, sExitTime, sKernelTime, sUserTime;

      if (cLock.IsLockHeld() != FALSE)
      {
        nCount = sDelayedHook.aItems.GetCount();
        if (nCount > 0)
        {
          for (i=0; i<nCount; i++)
          {
            if (cShutdownMtx.IsShuttingDown() != FALSE || cShutdownMtx.IsSystemActive() == FALSE)
              break;
            cProc = sDelayedHook.aItems[i]->cProc;
            dwChildPid = sDelayedHook.aItems[i]->dwChildPid;
            bHandled = TRUE;
            hProc = cProcHndlMgr->GetHandle(dwChildPid, SYNCHRONIZE|PROCESS_QUERY_INFORMATION|
                                                        PROCESS_VM_READ);
            hRes = (hProc != NULL) ? S_OK : E_ACCESSDENIED;
            if (SUCCEEDED(hRes))
            {
              {
                CNktAutoFastMutexReverse cLockRev(&GetLockObject());

                dw = ::WaitForInputIdle(hProc, 2);
              }
              if (dw > 0)
              {
                //more that 2 seconds... assume it is initialized
                ::GetProcessTimes(hProc, &sCreationTime, &sExitTime, &sKernelTime, &sUserTime);
                if (sUserTime.dwHighDateTime == 0 && sUserTime.dwLowDateTime < 20000000)
                  bHandled = FALSE;
              }
              if (bHandled != FALSE)
                HookExecCreateProcessAutoHook(cProc, dwChildPid);
              ::CloseHandle(hProc);
            }
            if (bHandled != FALSE)
            {
              sDelayedHook.aItems.RemoveElementAt(i);
              dwTimeout = 50;
            }
          }
        }
        else
        {
          sDelayedHook.cAvailableEvent.Reset();
        }
      }
    }
  }
  //----
  OnThreadEnded();
  return;
}
