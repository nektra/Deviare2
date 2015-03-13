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
#include "..\..\Common\ParamsEnum.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define MAX_PROCMEM_PER_PROCESS                            8

//-----------------------------------------------------------

HRESULT CNktDvSpyMgr::AddHookItem(__deref_out CHookItem **lplpItem, __inout CNktDvHook *lpHook,
                                  __inout CNktDvProcess *lpProc, __in DWORD dwHookId)
{
  TNktComPtr<CHookItem> cNewHookItem;
  HRESULT hRes;

  if (lplpItem == NULL)
    return E_POINTER;
  *lplpItem = NULL;
  cNewHookItem.Attach(NKT_MEMMGR_NEW CHookItem);
  if (cNewHookItem == NULL)
    return E_OUTOFMEMORY;
  cNewHookItem->cHook = lpHook;
  cNewHookItem->cProc = lpProc;
  cNewHookItem->dwHookId = dwHookId;
  //----
  cNewHookItem->cProcMem.Attach(CNktDvProcessMemory::CreateForPID(lpProc->GetProcessId()));
  if (cNewHookItem->cProcMem == NULL)
    return E_OUTOFMEMORY;
  switch (lpProc->GetPlatformBits())
  {
    case 32:
      hRes = cNewHookItem->cFuncParamCache.Initialize(lpHook->lpDbObject32, cNewHookItem->cProcMem, 32);
      break;
#if defined _M_X64
    case 64:
      hRes = cNewHookItem->cFuncParamCache.Initialize(lpHook->lpDbObject64, cNewHookItem->cProcMem, 64);
      break;
#endif //_M_X64
    default:
      hRes = E_FAIL;
      break;
  }
  if (FAILED(hRes))
    return hRes;
  if (aHookItemsListByHookId.SortedInsert(cNewHookItem,
                                          &CNktDvSpyMgr::CHookItem::HookItemCompareByHookId) == FALSE)
    return E_OUTOFMEMORY;
  if (aHookItemsListByHookAndProc.SortedInsert(cNewHookItem,
                                        &CNktDvSpyMgr::CHookItem::HookItemCompareByHookAndProc) == FALSE)
  {
    cNewHookItem.Detach(); //RemoveHookItem will release the new item
    RemoveHookItem(dwHookId);
    return E_OUTOFMEMORY;
  }
  *lplpItem = cNewHookItem.Detach();
  (*lplpItem)->AddRef();
  return S_OK;
}

VOID CNktDvSpyMgr::RemoveHookItem(__in DWORD dwHookId)
{
  CHookItem *lpItem;
  SIZE_T nIndex;

  nIndex = FindHookItemByHookId(dwHookId);
  if (nIndex != NKT_SIZE_T_MAX)
  {
    lpItem = aHookItemsListByHookId[nIndex];
    aHookItemsListByHookId.RemoveElementAt(nIndex);
    nIndex = FindHookItemByHookAndProc(lpItem->cHook, lpItem->cProc);
    if (nIndex != NKT_SIZE_T_MAX)
      aHookItemsListByHookAndProc.RemoveElementAt(nIndex);
    lpItem->Release();
  }
  return;
}

VOID CNktDvSpyMgr::RemoveHookItem(__inout CHookItem *lpItem)
{
  SIZE_T nIndex;

  nIndex = FindHookItemByHookId(lpItem->dwHookId);
  if (nIndex != NKT_SIZE_T_MAX)
    aHookItemsListByHookId.RemoveElementAt(nIndex);
  nIndex = FindHookItemByHookAndProc(lpItem->cHook, lpItem->cProc);
  if (nIndex != NKT_SIZE_T_MAX)
    aHookItemsListByHookAndProc.RemoveElementAt(nIndex);
  lpItem->Release();
  return;
}

SIZE_T CNktDvSpyMgr::FindHookItemByHookId(__in DWORD dwHookId)
{
  CHookItem **lplpItem, **lplpBuffer;

  lplpBuffer = aHookItemsListByHookId.GetBuffer();
  lplpItem = (CHookItem**)bsearch_s(&dwHookId, lplpBuffer, aHookItemsListByHookId.GetCount(),
                                    sizeof(CHookItem*),
                                    reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>
                                        (&CNktDvSpyMgr::CHookItem::HookItemCompareByHookId2),
                                    NULL);
  return (lplpItem != NULL) ? (SIZE_T)(lplpItem-lplpBuffer) : NKT_SIZE_T_MAX;
}

SIZE_T CNktDvSpyMgr::FindHookItemByHookAndProc(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc)
{
  CHookItem **lplpItem, **lplpBuffer;
  SIZE_T nHookId, nProcId;

  lplpBuffer = aHookItemsListByHookAndProc.GetBuffer();
  nHookId = lpHook->GetId();
  nProcId = (SIZE_T)(lpProc->GetProcessId());
  lplpItem = (CHookItem**)bsearch_s(&nHookId, lplpBuffer, aHookItemsListByHookAndProc.GetCount(),
                                    sizeof(CHookItem*),
                                    reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>
                                        (&CNktDvSpyMgr::CHookItem::HookItemCompareByHookAndProc2),
                                    &nProcId);
  return (lplpItem != NULL) ? (SIZE_T)(lplpItem-lplpBuffer) : NKT_SIZE_T_MAX;
}

DWORD CNktDvSpyMgr::GetNextHookId()
{
  DWORD dwId;

  do
  {
    dwId = (DWORD)NktInterlockedIncrement(&nNextHookId);
    if (dwId >= 0x80000000)
    {
      NktInterlockedExchange(&nNextHookId, 0);
      dwId = 0;
    }
  }
  while (dwId == 0);
  return dwId;
}

DWORD CNktDvSpyMgr::GetNextBatchHookId()
{
  DWORD dwId;

  do
  {
    dwId = (DWORD)NktInterlockedIncrement(&nNextBatchHookId);
    if (dwId >= 0x80000000)
    {
      NktInterlockedExchange(&nNextBatchHookId, 0);
      dwId = 0;
    }
  }
  while (dwId == 0);
  return dwId;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvSpyMgr::CHookItem::CHookItem() : CNktDvObject()
{
  nState = CNktDvHook::stInactive;
  NktInterlockedExchange(&nInternalFlags, 0);
  hLastError = S_OK;
  return;
}

CNktDvSpyMgr::CHookItem::~CHookItem()
{
  return;
}

int CNktDvSpyMgr::CHookItem::HookItemCompareByHookId(__in void *, __in CHookItem **lpElem1,
                                                     __in CHookItem **lpElem2)
{
  return NKT_DV_CMP<DWORD>((*lpElem1)->dwHookId, (*lpElem2)->dwHookId);
}

int CNktDvSpyMgr::CHookItem::HookItemCompareByHookAndProc(__in void *, __in CHookItem **lpElem1,
                                                          __in CHookItem **lpElem2)
{
  int res = NKT_DV_CMP<SIZE_T>((*lpElem1)->cHook->GetId(), (*lpElem2)->cHook->GetId());
  if (res != 0)
    return res;
  return NKT_DV_CMP<DWORD>((*lpElem1)->cProc->GetProcessId(), (*lpElem2)->cProc->GetProcessId());
}

int CNktDvSpyMgr::CHookItem::HookItemCompareByHookId2(__in void *, __in LPDWORD lpElem1,
                                                      __in CHookItem **lpElem2)
{
  return NKT_DV_CMP<DWORD>(*lpElem1, (*lpElem2)->dwHookId);
}

int CNktDvSpyMgr::CHookItem::HookItemCompareByHookAndProc2(__in SIZE_T *lpCtx, __in SIZE_T *lpElem1,
                                                           __in CHookItem **lpElem2)
{
  int res = NKT_DV_CMP<SIZE_T>(*lpElem1, (*lpElem2)->cHook->GetId());
  if (res != 0)
    return res;
  return NKT_DV_CMP<SIZE_T>(*lpCtx, (SIZE_T)((*lpElem2)->cProc->GetProcessId()));
}
