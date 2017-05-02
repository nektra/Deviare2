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

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

HRESULT CNktDvSpyMgr::HookDestroy(__inout CNktDvHook *lpHook)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  NKT_ASSERT(lpHook != NULL);
  TNktComPtr<CNktDvHook> cHook(lpHook);
  CNktDvHook *lpHook2;
  TNktComPtr<CNktDvProcess> cProc;
  CNktDvHook::eState nOldState;
  TNktLnkLst<CNktDvHook>::Iterator it;
  DWORD dwHookId;
  SIZE_T i, nCount;

  //detach hook
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    for (lpHook2=it.Begin(sHooks.cList); lpHook2!=NULL; lpHook2=it.Next())
    {
      if (cHook->GetDeviareId() == lpHook2->GetDeviareId())
      {
        sHooks.cList.Remove((CNktDvHook*)cHook);
        break;
      }
    }
  }
  if (lpHook2 != NULL)
  {
    cHook->Unlink();
    ((CNktDvHook*)cHook)->Release(); //release the internal hook reference
  }
  //inform hook items as being removed
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  for (;;)
  {
    nCount = aHookItemsListByHookId.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (cHook->GetDeviareId() == aHookItemsListByHookId[i]->cHook->GetDeviareId() &&
          aHookItemsListByHookId[i]->nState != CNktDvHook::stRemoved)
        break;
    }
    if (i >= nCount)
      break;
    nOldState = aHookItemsListByHookId[i]->nState;
    aHookItemsListByHookId[i]->nState = CNktDvHook::stRemoved; //avoid double removal
    dwHookId = aHookItemsListByHookId[i]->dwHookId;
    cProc = aHookItemsListByHookId[i]->cProc;
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
      SendRemoveHookCommand(cProc, dwHookId, 0, FALSE); //ignore result - no batch mode
    }
    RemoveHookItem(dwHookId);
  }
  return S_OK;
}

VOID CNktDvSpyMgr::HooksEnumDestroy(__inout CNktDvHooksEnumerator *lpHooksEnum)
{
  CNktSimpleLockNonReentrant cHooksEnumLock(&(sHooksEnum.nMutex));

  NKT_ASSERT(lpHooksEnum != NULL);
  sHooksEnum.cList.Remove(lpHooksEnum);
  lpHooksEnum->Unlink();
  return;
}

HRESULT CNktDvSpyMgr::HookAddProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                     __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  if (lpHook == NULL || lpProc == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookAddProcess(cHooksEnum, lpProc, bSync);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookAddProcess(__inout CNktDvHooksEnumerator *lpHooksEnum,
                                     __inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktArrayListWithRelease<CHookItem*,256> cHookItemsList;
  TNktComPtr<CHookItem> cHookItem;
  CNktDvHook::eState nOldState;
  LPWSTR szFuncW;
  DWORD dwHookId, dwBatchHookId;
  SIZE_T nHookIdx, nHooksCount, nAddress;
  BOOL bBatchHookUsed;
  HRESULT hRes, hReturnRes;

  if (lpHooksEnum == NULL || lpProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //loop
  nHooksCount = cHooksEnum->GetCount();
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHook.Release();
    if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
      break;
    //check if already exist
    if (FindHookItemByHookAndProc(cHook, cProc) != NKT_SIZE_T_MAX)
      continue;
    if ((cHook->nFlags & CNktDvHook::flgOnly32Bits) != 0)
    {
      if (cProc->GetPlatformBits() != 32)
        continue; //skip this item
    }
    else if ((cHook->nFlags & CNktDvHook::flgOnly64Bits) != 0)
    {
      if (cProc->GetPlatformBits() != 64)
        continue; //skip this item
    }
    //add this process to the list
    dwHookId = GetNextHookId();
    cHookItem.Release();
    hRes = AddHookItem(&cHookItem, cHook, cProc, dwHookId);
    if (FAILED(hRes))
    {
hap_err1:
      nHooksCount = cHookItemsList.GetCount();
      for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
        RemoveHookItem(cHookItemsList[nHookIdx]);
      return hRes;
    }
    //if autoactivating, add to the list
    if ((cHook->GetFlags() & CNktDvHook::flgAutoHookActive) != 0)
    {
      if (cHookItemsList.AddElement(cHookItem) == FALSE)
      {
        hRes = E_OUTOFMEMORY;
        goto hap_err1;
      }
      NktInterlockedBitSet(&(cHookItem->nInternalFlags), 0);
      ((CHookItem*)cHookItem)->AddRef();
    }
  }
  //try to do actual hooking
  hReturnRes = S_OK;
  bBatchHookUsed = FALSE;
  nHooksCount = cHookItemsList.GetCount();
  dwBatchHookId = (nHooksCount > 1) ? GetNextBatchHookId() : 0;
  NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookAddProcess[Initiating]: Batch=%lu",
                    ::GetTickCount(), dwBatchHookId));
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHookItem = cHookItemsList[nHookIdx];
    if (cHookItem->nState != CNktDvHook::stInactive)
      continue; //may be sent in another thread
    cHook = cHookItem->cHook;
    nOldState = cHookItem->nState;
    dwHookId = cHookItem->dwHookId;
    nAddress = cHook->nAddressOrOffset;
    szFuncW = cHook->GetFunctionName();
    if (nAddress == 0)
    {
      //find the address of this hook in the specified process id
      hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
      if (SUCCEEDED(hRes))
      {
        cExpFunc.Release();
        hRes = cModEnum->FindExportedFunctionByName(&cExpFunc, szFuncW);
      }
      //NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) FindExportedFunctionByName: %S / hRes=%08X",
      //                   ::GetTickCount(), szFuncW, hRes));
      if (SUCCEEDED(hRes))
        nAddress = cExpFunc->GetAddr();
      else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
        hRes = S_OK;
    }
    else
    {
      if ((cHook->GetFlags() & CNktDvHook::flgAddressIsOffset) != 0)
      {
        hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
        if (SUCCEEDED(hRes))
        {
          cMod.Release();
          hRes = cModEnum->GetItemByName(&cMod, szFuncW, cHook->sFuncParseInfo.nDllNameLen);
        }
        if (SUCCEEDED(hRes))
        {
          nAddress += (SIZE_T)(cMod->GetBaseAddress());
        }
        else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
        {
          nAddress = 0;
          hRes = S_OK;
        }
      }
      else if ((cHook->GetFlags() & CNktDvHook::flgDontCheckAddress) == 0)
      {
        hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
        if (SUCCEEDED(hRes))
        {
          hRes = HookCheckIfAddressIsAvailable(cModEnum, (LPVOID)nAddress);
          if (hRes == S_FALSE)
          {
            nAddress = 0;
            hRes = S_OK;
          }
        }
        else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
        {
          nAddress = 0;
          hRes = S_OK;
        }
      }
      else
      {
        hRes = S_OK;
      }
    }
    if (nAddress != 0)
    {
      bBatchHookUsed = TRUE;
      cHookItem->nState = CNktDvHook::stActivating;
      {
        CNktAutoFastMutexReverse cLockRev(&GetLockObject());

        OnHookStateChanged(cHook, cProc, CNktDvHook::stActivating, nOldState);
        hRes = SendAddHookCommand(cProc, cHook, dwHookId, dwBatchHookId, (LPVOID)nAddress,
                                  (nHooksCount > 1) ? FALSE : bSync);
      }
      nOldState = CNktDvHook::stActivating;
    }
    else if (nOldState != CNktDvHook::stNotAvailable)
    {
      cHookItem->nState = CNktDvHook::stNotAvailable;
      {
        CNktAutoFastMutexReverse cLockRev(&GetLockObject());

        OnHookStateChanged(cHook, cProc, CNktDvHook::stNotAvailable, nOldState);
        hRes = ActivateAgent(cProc);
      }
      nOldState = CNktDvHook::stNotAvailable;
    }
    cHookItem->hLastError = hRes;
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookAddProcess[Signal Error]: %S / hRes=%08X",
                         ::GetTickCount(), (LPWSTR)(cHookItem->cHook->cStrFunctionNameW), hRes));
      if (nOldState != CNktDvHook::stError && nOldState != CNktDvHook::stRemoved)
      {
        cHookItem->nState = CNktDvHook::stError;
        {
          CNktAutoFastMutexReverse cLockRev(&GetLockObject());

          OnHookStateChanged(cHook, cProc, CNktDvHook::stError, nOldState);
        }
      }
    }
    if (hRes == E_OUTOFMEMORY)
      hReturnRes = hRes; //only return fatal errors
  }
  //process batch
  if (nHooksCount > 1 && bBatchHookUsed != FALSE)
  {
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      hRes = SendBatchHookExecCommand(cProc, dwBatchHookId, bSync);
    }
    if (hRes == E_OUTOFMEMORY)
      hReturnRes = hRes; //only return fatal errors
    if (FAILED(hRes))
    {
      for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
      {
        cHookItem = cHookItemsList[nHookIdx];
        cHookItem->hLastError = hRes;
        NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookAddProcess[Signal Error (Batch)]: %S / "
                           "hRes=%08X", ::GetTickCount(), (LPWSTR)(cHookItem->cHook->cStrFunctionNameW),
                           hRes));
        nOldState = cHookItem->nState;
        if (nOldState != CNktDvHook::stError && nOldState != CNktDvHook::stRemoved)
        {
          cHookItem->nState = CNktDvHook::stError;
          {
            CNktAutoFastMutexReverse cLockRev(&GetLockObject());

            OnHookStateChanged(cHookItem->cHook, cHookItem->cProc, CNktDvHook::stError, nOldState);
          }
        }
      }
    }
  }
  //almost done
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
    NktInterlockedBitReset(&(cHookItemsList[nHookIdx]->nInternalFlags), 0);
  NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookAddProcess[Exiting]: Batch=%lu / hRes=%08X",
                    ::GetTickCount(), dwBatchHookId, hReturnRes));
  return hReturnRes;
}

HRESULT CNktDvSpyMgr::HookRemoveProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                        __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  if (lpHook == NULL || lpProc == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookRemoveProcess(cHooksEnum, lpProc, bSync);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookRemoveProcess(__inout CNktDvHooksEnumerator *lpHooksEnum,
                                        __inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  CNktDvHook::eState nOldState;
  DWORD dwHookId, dwBatchHookId;
  SIZE_T nIndex, nHookIdx, nHooksCount;
  BOOL bBatchHookUsed;
  HRESULT hRes, hReturnRes;

  if (lpHooksEnum == NULL || lpProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //loop
  hReturnRes = S_OK;
  nHooksCount = cHooksEnum->GetCount();
  dwBatchHookId = (nHooksCount > 1) ? GetNextBatchHookId() : 0;
  NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookRemoveProcess[Initiating]: Batch=%lu",
                    ::GetTickCount(), dwBatchHookId));
  bBatchHookUsed = FALSE;
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHook.Release();
    if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
      break;
    //inform hook items as being removed
    nIndex = FindHookItemByHookAndProc(cHook, cProc);
    if (nIndex == NKT_SIZE_T_MAX ||
        aHookItemsListByHookAndProc[nIndex]->nState == CNktDvHook::stRemoved)
      continue;
    //do remove
    nOldState = aHookItemsListByHookAndProc[nIndex]->nState;
    aHookItemsListByHookAndProc[nIndex]->nState = CNktDvHook::stRemoved; //avoid double removal
    dwHookId = aHookItemsListByHookAndProc[nIndex]->dwHookId;
    bBatchHookUsed = TRUE;
    NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookRemoveProcess[Removing]: %S",
                       ::GetTickCount(), (LPWSTR)(aHookItemsListByHookAndProc[nIndex]->cHook->
                                                  cStrFunctionNameW)));
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
      hRes = SendRemoveHookCommand(cProc, dwHookId, dwBatchHookId, FALSE); //ignore result
    }
    if (hRes == E_OUTOFMEMORY)
      hReturnRes = hRes; //only return fatal errors
    RemoveHookItem(dwHookId);
  }
  //process batch
  if (nHooksCount > 1 && bBatchHookUsed != FALSE)
  {
    CNktAutoFastMutexReverse cLockRev(&GetLockObject());

    hRes = SendBatchHookExecCommand(cProc, dwBatchHookId, FALSE);
    if (hRes == E_OUTOFMEMORY)
      hReturnRes = hRes; //only return fatal errors
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookRemoveProcess[Exiting]: Batch=%lu / hRes=%08X",
                    ::GetTickCount(), dwBatchHookId, hReturnRes));
  return hReturnRes;
}

VOID CNktDvSpyMgr::HookRemoveProcessItems(__inout CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  CNktDvHook::eState nOldState;
  DWORD dwHookId, dwPid;
  SIZE_T nIndex, nHookItemsCount;

  NKT_ASSERT(lpProc != NULL);
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return;
  //loop
  dwPid = lpProc->GetProcessId();
  while (1)
  {
    nHookItemsCount = aHookItemsListByHookAndProc.GetCount();
    for (nIndex=0; nIndex<nHookItemsCount; nIndex++)
    {
      if (aHookItemsListByHookAndProc[nIndex]->cProc->GetProcessId() == dwPid)
        break;
    }
    if (nIndex >= nHookItemsCount)
      break;
    //do remove
    nOldState = aHookItemsListByHookAndProc[nIndex]->nState;
    aHookItemsListByHookAndProc[nIndex]->nState = CNktDvHook::stRemoved; //avoid double removal
    dwHookId = aHookItemsListByHookAndProc[nIndex]->dwHookId;
    cHook = aHookItemsListByHookAndProc[nIndex]->cHook;
    NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookRemoveProcessItems[Removing]: %S",
                      ::GetTickCount(), (LPWSTR)(aHookItemsListByHookAndProc[nIndex]->cHook->
                      cStrFunctionNameW)));
    if (nOldState != CNktDvHook::stRemoved)
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
    }
    RemoveHookItem(dwHookId);
  }
  //done
  return;
}

HRESULT CNktDvSpyMgr::HookEnableProcess(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                        __in BOOL bEnable, __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  if (lpHook == NULL || lpProc == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookEnableProcess(cHooksEnum, lpProc, bEnable, bSync);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookEnableProcess(__inout CNktDvHooksEnumerator *lpHooksEnum,
                                        __inout CNktDvProcess *lpProc, __in BOOL bEnable,__in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  CNktDvHook::eState nOldState;
  DWORD dwHookId;
  SIZE_T nIndex, nHookIdx, nHooksCount;
  HRESULT hRes, hReturnRes;

  if (lpHooksEnum == NULL || lpProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //loop
  nHooksCount = cHooksEnum->GetCount();
  hReturnRes = S_OK;
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHook.Release();
    if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
      break;
    //get hook item
    nIndex = FindHookItemByHookAndProc(cHook, cProc);
    if (nIndex == NKT_SIZE_T_MAX)
      continue; //return NKT_DVERR_NotFound;
    nOldState = aHookItemsListByHookAndProc[nIndex]->nState;
    if (nOldState == CNktDvHook::stRemoved)
      continue; //return NKT_DVERR_NotFound;
    if (nOldState != CNktDvHook::stActivating && nOldState != CNktDvHook::stActive && nOldState != CNktDvHook::stDisabled)
      continue; //return NKT_DVERR_HookIsInactive;
    dwHookId = aHookItemsListByHookAndProc[nIndex]->dwHookId;
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      //we dont use batch in enable/disable because it is fast in agent
      hRes = SendEnableHookCommand(cProc, dwHookId, 0, bEnable, bSync);
    }
    aHookItemsListByHookAndProc[nIndex]->hLastError = hRes;
    if (FAILED(hRes))
    {
      NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookEnable[Signal Error]: %S / hRes=%08X",
                         ::GetTickCount(), (LPWSTR)(cHook->cStrFunctionNameW), hRes));
      nOldState = aHookItemsListByHookAndProc[nIndex]->nState;
      if (nOldState != CNktDvHook::stError && nOldState != CNktDvHook::stRemoved)
      {
        aHookItemsListByHookAndProc[nIndex]->nState = CNktDvHook::stError;
        {
          CNktAutoFastMutexReverse cLockRev(&GetLockObject());

          OnHookStateChanged(cHook, cProc, CNktDvHook::stError, nOldState);
        }
      }
    }
    if (hRes == E_OUTOFMEMORY)
      hReturnRes = hRes; //only return fatal errors
  }
  //done
  return hReturnRes;
}

CNktDvHook::eState CNktDvSpyMgr::HookGetProcessState(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cHook(lpHook);
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  SIZE_T nIndex;

  if (lpHook == NULL || lpProc == NULL ||
      lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return CNktDvHook::stNotFound;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return CNktDvHook::stInactive;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return CNktDvHook::stInactive; //not initialized
  //get hook item
  nIndex = FindHookItemByHookAndProc(cHook, cProc);
  if (nIndex == NKT_SIZE_T_MAX)
    return CNktDvHook::stRemoved;
  return aHookItemsListByHookAndProc[nIndex]->nState;
}

HRESULT CNktDvSpyMgr::HookQueryOverwrite(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc,
                                         __inout TNktArrayList<BYTE> &cResult)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  cResult.RemoveAllElements();
  if (lpHook == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookQueryOverwrite(cHooksEnum, lpProc, cResult);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookQueryOverwrite(__inout CNktDvHooksEnumerator *lpHooksEnum, __inout CNktDvProcess *lpProc,
                                         __inout TNktArrayList<BYTE> &cResult)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  CNktDvHook::eState nState;
  DWORD dw;
  NKT_DV_TMSG_HELPERCALL sMsg;
  SIZE_T nIndex, nHookIdx, nHooksCount, aIndexes[NKT_DV_ARRAYLEN(sMsg.sQueryHookOverwrite.aHookIdsList)];
  HRESULT hRes, hReturnRes;

  cResult.RemoveAllElements();
  if (lpHooksEnum == NULL || lpProc == NULL)
    return E_POINTER;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //init result buffer
  nHooksCount = cHooksEnum->GetCount();
  if (nHooksCount == 0)
    return S_OK;
  if (cResult.SetCount(nHooksCount) == FALSE)
    return E_OUTOFMEMORY;
  nktMemSet(cResult.GetBuffer(), 0, nHooksCount);
  //loop
  hReturnRes = S_OK;
  nHookIdx = 0;
  while (nHookIdx<nHooksCount && SUCCEEDED(hReturnRes))
  {
    for (sMsg.sQueryHookOverwrite.dwCount=0; nHookIdx<nHooksCount &&
         (SIZE_T)(sMsg.sQueryHookOverwrite.dwCount)<NKT_DV_ARRAYLEN(aIndexes); nHookIdx++)
    {
      cHook.Release();
      if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
        continue;
      //get hook item
      nIndex = FindHookItemByHookAndProc(cHook, cProc);
      if (nIndex == NKT_SIZE_T_MAX)
        continue;
      //check state
      nState = aHookItemsListByHookAndProc[nIndex]->nState;
      if (nState == CNktDvHook::stRemoved ||
          (nState != CNktDvHook::stActivating && nState != CNktDvHook::stActive))
        continue;
      //query this item
      sMsg.sQueryHookOverwrite.aHookIdsList[sMsg.sQueryHookOverwrite.dwCount] =
                aHookItemsListByHookAndProc[nIndex]->dwHookId;
      aIndexes[sMsg.sQueryHookOverwrite.dwCount] = nHookIdx;
      (sMsg.sQueryHookOverwrite.dwCount)++;
    }
    //send message
    if (sMsg.sQueryHookOverwrite.dwCount > 0)
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());
      TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_HELPERCALL_CALLBACK> cMsgResp;
      LPBYTE s, d;

      sMsg.dwCallCode = NKT_DV_TMSG_CODE_HELPERCALL_QueryHookOverwrite;
      hRes = SendDirectCommand(cProc, NKT_DV_TMSG_CODE_HelperCall, &sMsg, sizeof(sMsg), &cMsgResp, TRUE);
      if (SUCCEEDED(hRes))
      {
        s = cMsgResp->sQueryHookOverwrite.aOverwritten;
        d = cResult.GetBuffer();
        for (dw=0; dw<sMsg.sQueryHookOverwrite.dwCount; dw++)
          d[aIndexes[dw]] = s[dw];
      }
      else
      {
        NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookCheckOverwrite: hRes=%08X", ::GetTickCount(), hRes));
      }
      if (hRes == E_OUTOFMEMORY || hRes == NKT_DVERR_Cancelled || hRes == E_FAIL)
        hReturnRes = hRes; //only return fatal errors
    }
  }
  //done
  return hReturnRes;
}

HRESULT CNktDvSpyMgr::HookGetLastError(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHook> cHook(lpHook);
  TNktComPtr<CNktDvProcess> cProc(lpProc);
  SIZE_T nIndex;

  if (lpHook == NULL || lpProc == NULL)
    return NKT_DVERR_NotFound;
  if (lpProc->GetProcessId() == 0 || lpProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_HookIsInactive;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return NKT_DVERR_HookIsInactive; //not initialized
  //get hook item
  nIndex = FindHookItemByHookAndProc(cHook, cProc);
  if (nIndex == NKT_SIZE_T_MAX)
    return NKT_DVERR_NotFound;
  return aHookItemsListByHookAndProc[nIndex]->hLastError;
}

HRESULT CNktDvSpyMgr::HookInstall(__inout CNktDvHook *lpHook, __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  if (lpHook == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookInstall(cHooksEnum, bSync, NULL);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookInstall(__inout CNktDvHooksEnumerator *lpHooksEnum, __in BOOL bSync,
                                  __inout_opt CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  TNktComPtr<CNktDvModule> cMod;
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  TNktComPtr<CHookItem> cHookItem;
  TNktArrayListWithRelease<CHookItem*,256> cHookItemsList;
  LPWSTR szFuncW;
  CNktDvHook::eState nOldState;
  SIZE_T i, j, nCurr, nFirst, nCount, nHookIdx, nHooksCount, nAddress;
  DWORD dwHookId, dwBatchHookId, dwRestrictPID;
  BOOL bBatchHookUsed;
  HRESULT hRes, hReturnRes;

  if (lpHooksEnum == NULL)
    return E_POINTER;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //build hook items list
  dwRestrictPID = (lpProc != NULL) ? (lpProc->GetProcessId()) : 0;
  nCount = aHookItemsListByHookId.GetCount();
  nHooksCount = cHooksEnum->GetCount();
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHook.Release();
    if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
      break;
    for (i=0; i<nCount; i++)
    {
      cHookItem = aHookItemsListByHookId[i];
      if (cHook->GetDeviareId() == cHookItem->cHook->GetDeviareId() &&
          (cHookItem->nState == CNktDvHook::stInactive ||
           cHookItem->nState == CNktDvHook::stNotAvailable) &&
          (dwRestrictPID == 0 || cHookItem->cProc->GetProcessId() == dwRestrictPID) &&
          NktInterlockedBitTestAndSet(&(cHookItem->nInternalFlags), 0) == FALSE)
      {
        if (cHookItemsList.SortedInsert(cHookItem, &CNktDvSpyMgr::HookInstallUninstallCompare,
                                        NULL) == FALSE)
        {
          nCount = cHookItemsList.GetCount();
          for (i=0; i<nCount; i++)
            NktInterlockedBitReset(&(cHookItemsList[i]->nInternalFlags), 0);
          return E_OUTOFMEMORY;
        }
        ((CHookItem*)cHookItem)->AddRef();
      }
    }
  }
  //try to do actual hooking
  nCount = cHookItemsList.GetCount();
  hReturnRes = S_OK;
  nCurr = 0;
  while (nCurr < nCount)
  {
    //check for batch hooking
    nFirst = nCurr++;
    cProc = cHookItemsList[nFirst]->cProc;
    while (nCurr < nCount &&
           cHookItemsList[nCurr]->cProc->GetProcessId() == cProc->GetProcessId())
      nCurr++;
    //hook from "nFirst" to "nCurr"
    dwBatchHookId = (nCurr > nFirst+1) ? GetNextBatchHookId() : 0;
    NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookInstall[Initiating]: Batch=%lu",
                      ::GetTickCount(), dwBatchHookId));
    bBatchHookUsed = FALSE;
    cModEnum = NULL;
    for (i=nFirst; i<nCurr; i++)
    {
      cHookItem = cHookItemsList[i];
      if (cHookItem->nState != CNktDvHook::stInactive &&
          cHookItem->nState != CNktDvHook::stNotAvailable)
        continue;
      cHook = cHookItem->cHook;
      nOldState = cHookItem->nState;
      dwHookId = cHookItem->dwHookId;
      nAddress = cHook->nAddressOrOffset;
      szFuncW = cHook->GetFunctionName();
      if (nAddress == 0)
      {
        //obtain address of the hook
        hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
        if (SUCCEEDED(hRes))
        {
          NKT_ASSERT(cModEnum != NULL);
          cExpFunc.Release();
          hRes = cModEnum->FindExportedFunctionByName(&cExpFunc, szFuncW);
        }
        //NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) FindExportedFunctionByName: %S / hRes=%08X",
        //                   ::GetTickCount(), szFuncW, hRes));
        if (SUCCEEDED(hRes))
          nAddress = cExpFunc->GetAddr();
        else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
          hRes = S_OK;
      }
      else
      {
        if ((cHook->GetFlags() & CNktDvHook::flgAddressIsOffset) != 0)
        {
          hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
          if (SUCCEEDED(hRes))
          {
            cMod.Release();
            hRes = cModEnum->GetItemByName(&cMod, szFuncW, cHook->sFuncParseInfo.nDllNameLen);
          }
          if (SUCCEEDED(hRes))
          {
            nAddress += (SIZE_T)(cMod->GetBaseAddress());
          }
          else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
          {
            nAddress = 0;
            hRes = S_OK;
          }
        }
        else if ((cHook->GetFlags() & CNktDvHook::flgDontCheckAddress) == 0)
        {
          hRes = (cModEnum == NULL) ? (cProc->GetModulesEnumeratorV(&cModEnum)) : S_OK;
          if (SUCCEEDED(hRes))
          {
            NKT_ASSERT(cModEnum != NULL);
            hRes = HookCheckIfAddressIsAvailable(cModEnum, (LPVOID)nAddress);
            if (hRes == S_FALSE)
            {
              nAddress = 0;
              hRes = S_OK;
            }
          }
          else if (hRes == NKT_DVERR_NotFound || hRes == E_FAIL)
          {
            nAddress = 0;
            hRes = S_OK;
          }
        }
        else
        {
          hRes = S_OK;
        }
      }
      if (nAddress != 0)
      {
        bBatchHookUsed = TRUE;
        cHookItem->nState = CNktDvHook::stActivating;
        {
          CNktAutoFastMutexReverse cLockRev(&GetLockObject());

          OnHookStateChanged(cHook, cProc, CNktDvHook::stActivating, nOldState);
          hRes = SendAddHookCommand(cProc, cHook, dwHookId, dwBatchHookId, (LPVOID)nAddress,
                                    (dwBatchHookId != 0) ? FALSE : bSync);
        }
        nOldState = CNktDvHook::stActivating;
      }
      else if (nOldState != CNktDvHook::stNotAvailable)
      {
        cHookItem->nState = CNktDvHook::stNotAvailable;
        {
          CNktAutoFastMutexReverse cLockRev(&GetLockObject());

          OnHookStateChanged(cHook, cProc, CNktDvHook::stNotAvailable, nOldState);
          hRes = ActivateAgent(cProc);
        }
        nOldState = CNktDvHook::stNotAvailable;
      }
      cHookItem->hLastError = hRes;
      if (FAILED(hRes))
      {
        NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookInstall[Signal Error]: %S / hRes=%08X",
                           ::GetTickCount(), (LPWSTR)(cHook->cStrFunctionNameW), hRes));
        if (nOldState != CNktDvHook::stError && nOldState != CNktDvHook::stRemoved)
        {
          cHookItem->nState = CNktDvHook::stError;
          {
            CNktAutoFastMutexReverse cLockRev(&GetLockObject());

            OnHookStateChanged(cHook, cProc, CNktDvHook::stError, nOldState);
          }
        }
      }
      if (hRes == E_OUTOFMEMORY)
        hReturnRes = hRes; //only return fatal errors
    }
    if (dwBatchHookId != 0 && bBatchHookUsed != FALSE)
    {
      //finish batch
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      hRes = SendBatchHookExecCommand(cProc, dwBatchHookId, bSync);
    }
    else
    {
      hRes = S_OK;
    }
    if (FAILED(hRes))
    {
      for (j=nFirst; j<nCurr; j++)
      {
        cHookItem = cHookItemsList[j];
        cHookItem->hLastError = hRes;
        NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookInstall[Signal Error (Batch)]: %S / "
                           "hRes=%08X", ::GetTickCount(), (LPWSTR)(cHookItem->cHook->cStrFunctionNameW),
                           hRes));
        nOldState = cHookItem->nState;
        if (nOldState != CNktDvHook::stError)
        {
          cHookItem->nState = CNktDvHook::stError;
          {
            CNktAutoFastMutexReverse cLockRev(&GetLockObject());

            OnHookStateChanged(cHookItem->cHook, cHookItem->cProc, CNktDvHook::stError, nOldState);
          }
        }
      }
      if (hRes == E_OUTOFMEMORY)
        hReturnRes = hRes; //only return fatal errors
    }
    NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookInstall[Exiting]: Batch=%lu / hRes=%08X",
                      ::GetTickCount(), dwBatchHookId, hReturnRes));
  }
  //almost done
  for (i=0; i<nCount; i++)
    NktInterlockedBitReset(&(cHookItemsList[i]->nInternalFlags), 0);
  return hReturnRes;
}

HRESULT CNktDvSpyMgr::HookUninstall(__inout CNktDvHook *lpHook, __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum;
  TNktComPtr<CNktDvHook> cHook(lpHook);
  HRESULT hRes;

  if (lpHook == NULL)
    return E_POINTER;
  cHooksEnum.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  hRes = (cHooksEnum != NULL) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
    hRes = cHooksEnum->Add(lpHook);
  if (SUCCEEDED(hRes))
    hRes = HookUninstall(cHooksEnum, bSync, NULL);
  return hRes;
}

HRESULT CNktDvSpyMgr::HookUninstall(__inout CNktDvHooksEnumerator *lpHooksEnum, __in BOOL bSync,
                                    __inout_opt CNktDvProcess *lpProc)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksEnum(lpHooksEnum);
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CHookItem> cHookItem;
  TNktArrayListWithRelease<CHookItem*,256> cHookItemsList;
  CNktDvHook::eState nOldState;
  SIZE_T i, nFirst, nCurr, nCount, nHookIdx, nHooksCount;
  DWORD dwHookId, dwBatchHookId, dwRestrictPID;
  BOOL bBatchHookUsed;
  HRESULT hRes, hReturnRes;

  if (lpHooksEnum == NULL)
    return E_POINTER;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL; //not initialized
  //build hook items list
  dwRestrictPID = (lpProc != NULL) ? (lpProc->GetProcessId()) : 0;
  nHooksCount = cHooksEnum->GetCount();
  for (nHookIdx=0; nHookIdx<nHooksCount; nHookIdx++)
  {
    cHook.Release();
    if (FAILED(cHooksEnum->GetAt(nHookIdx, &cHook)))
      break;
    nCount = aHookItemsListByHookId.GetCount();
    for (i=0; i<nCount; i++)
    {
      cHookItem = aHookItemsListByHookId[i];
      if (cHook->GetDeviareId() == cHookItem->cHook->GetDeviareId() &&
          (cHookItem->nState == CNktDvHook::stActivating ||
           cHookItem->nState == CNktDvHook::stActive ||
           cHookItem->nState == CNktDvHook::stDisabled) &&
          (dwRestrictPID == 0 || cHookItem->cProc->GetProcessId() == dwRestrictPID) &&
          NktInterlockedBitTestAndSet(&(cHookItem->nInternalFlags), 1) == FALSE)
      {
        if (cHookItemsList.SortedInsert(cHookItem, &CNktDvSpyMgr::HookInstallUninstallCompare,
                                        NULL) == FALSE)
        {
          nCount = cHookItemsList.GetCount();
          for (i=0; i<nCount; i++)
            NktInterlockedBitReset(&(cHookItemsList[i]->nInternalFlags), 1);
          return E_OUTOFMEMORY;
        }
        ((CHookItem*)cHookItem)->AddRef();
      }
    }
  }
  //try to do actual unhooking
  nCount = cHookItemsList.GetCount();
  hReturnRes = S_OK;
  nCurr = 0;
  while (nCurr < nCount)
  {
    //check for batch unhooking
    nFirst = nCurr++;
    cProc = cHookItemsList[nFirst]->cProc;
    while (nCurr < nCount &&
           cHookItemsList[nCurr]->cProc->GetProcessId() == cProc->GetProcessId())
      nCurr++;
    //unhook from "nFirst" to "nCurr"
    dwBatchHookId = (nCurr > nFirst+1) ? GetNextBatchHookId() : 0;
    bBatchHookUsed = FALSE;
    for (i=nFirst; i<nCurr; i++)
    {
      cHookItem = cHookItemsList[i];
      cHook = cHookItem->cHook;
      nOldState = cHookItem->nState;
      dwHookId = cHookItem->dwHookId;
      if (nOldState != CNktDvHook::stRemoved)
      {
        //double check removed state to avoid removing a hook already removed in another thread
        bBatchHookUsed = TRUE;
        cHookItem->nState = CNktDvHook::stRemoved;
        NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookUninstall[Removing]: %S", ::GetTickCount(),
                           (LPWSTR)(cHook->cStrFunctionNameW)));
        {
          CNktAutoFastMutexReverse cLockRev(&GetLockObject());

          OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
          hRes = SendRemoveHookCommand(cProc, dwHookId, dwBatchHookId, bSync);
        }
        if (hRes == E_OUTOFMEMORY)
          hReturnRes = hRes; //only return fatal errors
      }
      RemoveHookItem(dwHookId);
    }
    if (dwBatchHookId != 0 && bBatchHookUsed != FALSE)
    {
      //finish batch
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      hRes = SendBatchHookExecCommand(cProc, dwBatchHookId, bSync);
    }
    else
    {
      hRes = S_OK;
    }
    if (FAILED(hRes))
    {
      //if batch command fails it is almost sure that the connection will be terminated do don't worry
      if (hRes == E_OUTOFMEMORY)
        hReturnRes = hRes; //only return fatal errors
    }
  }
  //almost done
  for (i=0; i<nCount; i++)
    NktInterlockedBitReset(&(cHookItemsList[i]->nInternalFlags), 1);
  return hReturnRes;
}

VOID CNktDvSpyMgr::HookRemoveBecauseAgentShutdown(__in DWORD dwPid)
{
  TNktComPtr<CNktDvHook> cHook;
  TNktComPtr<CNktDvProcess> cProc;
  CNktDvHook::eState nOldState;
  SIZE_T i, nCount;
  DWORD dwHookId;

  //CNktDvEngine is already locked by the caller
  //inform hook items as being removed
  for (;;)
  {
    nCount = aHookItemsListByHookId.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (dwPid == aHookItemsListByHookId[i]->cProc->GetProcessId() &&
          aHookItemsListByHookId[i]->nState != CNktDvHook::stRemoved)
        break;
    }
    if (i >= nCount)
      break;
    nOldState = aHookItemsListByHookId[i]->nState;
    aHookItemsListByHookId[i]->nState = CNktDvHook::stRemoved; //avoid double removal
    cHook = aHookItemsListByHookId[i]->cHook;
    cProc = aHookItemsListByHookId[i]->cProc;
    dwHookId = aHookItemsListByHookId[i]->dwHookId;
    NKT_DEBUGPRINTLNA(Nektra::dlInformation, ("%lu) HookRemoveBecauseAgentShutdown[Removing]: %S",
                       ::GetTickCount(), (LPWSTR)(cHook->cStrFunctionNameW)));
    {
      CNktAutoFastMutexReverse cLockRev(&GetLockObject());

      OnHookStateChanged(cHook, cProc, CNktDvHook::stRemoved, nOldState);
    }
    RemoveHookItem(dwHookId);
  }
  return;
}

HRESULT CNktDvSpyMgr::HookExecCreateProcessAutoHook(__inout CNktDvProcess *lpProc, __in DWORD dwChildPid)
{
  //called from an agent message
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksCopy;
  CNktDvHook *lpHook;
  TNktComPtr<CNktDvProcess> cChildProc;
  TNktLnkLst<CNktDvHook>::Iterator it;
  LPWSTR szPathW[2], szNameW[2];
  LONG nHookFlags;
  HRESULT hRes;

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  //check all hooks
  cHooksCopy.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  if (cHooksCopy == NULL)
    return E_OUTOFMEMORY;
  szPathW[0] = lpProc->GetPath();
  szNameW[0] = lpProc->GetFileName();
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    for (lpHook=it.Begin(sHooks.cList); lpHook!=NULL; lpHook=it.Next())
    {
      nHookFlags = lpHook->GetFlags();
      if ((nHookFlags & (CNktDvHook::flgAutoHookActive|CNktDvHook::flgAutoHookChildProcess)) ==
                        (CNktDvHook::flgAutoHookActive|CNktDvHook::flgAutoHookChildProcess))
      {
        if (cChildProc == NULL)
        {
          hRes = CNktDvProcess::CreateFromPID(&cChildProc, dwChildPid);
          if (FAILED(hRes))
            return hRes;
        }
        if ((nHookFlags & CNktDvHook::flgRestrictAutoHookToSameExecutable) != 0)
        {
          //compare executable name
          szPathW[1] = cChildProc->GetPath();
          szNameW[1] = cChildProc->GetFileName();
          if (szNameW[0] != szPathW[0] && szNameW[1] != szPathW[1])
          {
            //compare full path if both specified
            if (_wcsicmp(szPathW[0], szPathW[1]) != 0)
              continue;
          }
          else
          {
            //compare only names
            if (_wcsicmp(szNameW[0], szNameW[1]) != 0)
              continue;
          }
        }
        if (FAILED(cHooksCopy->Add(lpHook)))
          return E_OUTOFMEMORY;
      }
    }
  }
  hRes = S_OK;
  if (cHooksCopy->GetCount() > 0)
  {
    CNktAutoFastMutexReverse cLockRev(&GetLockObject());

    hRes = HookAddProcess(cHooksCopy, cChildProc, TRUE);
  }
  return hRes;
}

HRESULT CNktDvSpyMgr::HookExecLoadLibraryAutoHook(__inout CNktDvProcess *lpProc, __in_z LPCWSTR szDllNameW)
{
  //called from an agent message
  CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
  TNktComPtr<CNktDvHooksEnumerator> cHooksCopy;
  TNktLnkLst<CNktDvHook>::Iterator it;
  CNktDvHook *lpHook;
  HRESULT hRes;

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  //check all hooks
  cHooksCopy.Attach(NKT_MEMMGR_NEW CNktDvHooksEnumerator);
  if (cHooksCopy == NULL)
    return E_OUTOFMEMORY;
  {
    CNktSimpleLockNonReentrant cHooksLock(&(sHooks.nMutex));

    for (lpHook=it.Begin(sHooks.cList); lpHook!=NULL; lpHook=it.Next())
    {
      if ((lpHook->GetFlags() & CNktDvHook::flgAutoHookActive) != 0)
      {
        if (FAILED(cHooksCopy->Add(lpHook)))
          return E_OUTOFMEMORY;
      }
    }
  }
  hRes = S_OK;
  if (cHooksCopy->GetCount() > 0)
  {
    CNktAutoFastMutexReverse cLockRev(&GetLockObject());

    //hRes = HookInstall(cHooksCopy, FALSE); //should be TRUE but hangs if recursive call to LdrLoadDll
    hRes = HookInstall(cHooksCopy, TRUE, lpProc);
  }
  return hRes;
}

HRESULT CNktDvSpyMgr::HookCheckIfAddressIsAvailable(__inout CNktDvModulesEnumerator *lpModEnum,
                                                    __in LPVOID lpAddress)
{
  TNktComPtr<CNktDvModule> cMod;
  HRESULT hRes;

  hRes = lpModEnum->GetItemByAddress(&cMod, lpAddress, CNktDvModule::smFindContaining);
  if (hRes == E_FAIL || hRes == NKT_DVERR_NotFound)
    hRes = S_FALSE;
  return hRes;
}

int CNktDvSpyMgr::HookInstallUninstallCompare(LPVOID, CHookItem **lplpElem1, CHookItem **lplpElem2)
{
  int res;

  res = NKT_DV_CMP<DWORD>((*lplpElem1)->cProc->GetProcessId(), (*lplpElem2)->cProc->GetProcessId());
  if (res == 0)
  {
    res = NKT_DV_CMP<SIZE_T>((*lplpElem1)->cHook->nAddressOrOffset, (*lplpElem2)->cHook->nAddressOrOffset);
    if (res == 0)
    {
      res = _wcsicmp((*lplpElem1)->cHook->GetFunctionName(), (*lplpElem2)->cHook->GetFunctionName());
    }
  }
  if (res == 0)
  {
    res = NKT_DV_CMP<SIZE_T>((*lplpElem1)->cHook->GetId(), (*lplpElem2)->cHook->GetId());
  }
  return res;
}
