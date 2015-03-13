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

#include "Hook.h"
#include "SpyMgr.h"
#include "..\..\Common\Tools.h"

//-----------------------------------------------------------

CNktDvHook::CNktDvHook() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE), TNktLnkLstNode<CNktDvHook>()
{
  NktInterlockedExchange(&nFlags, 0);
  //----
  lpEngine = NULL;
  bDestroyed = FALSE;
  lpDbModule32 = NULL;
  lpDbObject32 = NULL;
#if defined _M_X64
  lpDbModule64 = NULL;
  lpDbObject64 = NULL;
#endif //_M_X64
  nAddressOrOffset = 0;
  nktMemSet(&sFuncParseInfo, 0, sizeof(sFuncParseInfo));
  return;
}

CNktDvHook::~CNktDvHook()
{
  NKT_ASSERT(lpEngine == NULL);
  NKT_ASSERT(TNktLnkLstNode<CNktDvHook>::GetLinkedList() == NULL); //should not belong to any list
  RemoveAllHandlers(TRUE);
  return;
}

VOID CNktDvHook::Destroy()
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this); //because DestroyHook will decrement the reference count
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  if (cEngineAutoRef != NULL)
    cEngineAutoRef->HookDestroy(this);
  return;
}

HRESULT CNktDvHook::AddProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookAddProcess(this, lpProc, bSync)) : E_FAIL;
}

HRESULT CNktDvHook::AddProcess(__in DWORD dwPid, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return AddProcess(cProc, bSync);
}

HRESULT CNktDvHook::RemoveProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookRemoveProcess(this, lpProc, bSync)) : E_FAIL;
}

HRESULT CNktDvHook::RemoveProcess(__in DWORD dwPid, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return RemoveProcess(cProc, bSync);
}

HRESULT CNktDvHook::EnableProcess(__inout CNktDvProcess *lpProc, __in BOOL bEnable, __in BOOL bSync)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookEnableProcess(this, lpProc, bEnable, bSync)) :
                                    E_FAIL;
}

HRESULT CNktDvHook::EnableProcess(__in DWORD dwPid, __in BOOL bEnable, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return EnableProcess(cProc, bEnable, bSync);
}

HRESULT CNktDvHook::Hook(__in BOOL bSync)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
    NktInterlockedOr(&nFlags, (LONG)CNktDvHook::flgAutoHookActive);
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookInstall(this, bSync)) : E_FAIL;
}

HRESULT CNktDvHook::Unhook(__in BOOL bSync)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
    NktInterlockedAnd(&nFlags, ~((LONG)CNktDvHook::flgAutoHookActive));
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookUninstall(this, bSync)) : E_FAIL;
}

HRESULT CNktDvHook::AddCustomHandler(__in_z LPCWSTR szHandlerDllW, __in LONG nFlags,
                                     __in_z LPCWSTR szParametersW)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktAutoFreePtr<CUSTOM_HANDLER_ITEM> cNewItem;
  SIZE_T i, nLen, nParamsLen;

  if (szHandlerDllW == NULL)
    return E_POINTER;
  if (szHandlerDllW[0] == 0)
    return E_INVALIDARG;
  //add new handler
  nLen = wcslen(szHandlerDllW);
  nParamsLen = (szParametersW != NULL) ? wcslen(szParametersW) : 0;
  i = (nLen+nParamsLen+1);
  if (nLen+nParamsLen < nLen ||
      i < nLen || i < nParamsLen ||
      (i*sizeof(WCHAR)) < i)
    return E_OUTOFMEMORY;
  i *= sizeof(WCHAR);
  if (i+sizeof(CUSTOM_HANDLER_ITEM) < i)
    return E_OUTOFMEMORY;
  cNewItem.Attach((CUSTOM_HANDLER_ITEM*)nktMemMalloc(sizeof(CUSTOM_HANDLER_ITEM)+i));
  if (cNewItem == NULL)
    return E_OUTOFMEMORY;
  cNewItem->nFlags = (ULONG)nFlags;
  memcpy(cNewItem->szDllNameW, szHandlerDllW, nLen*sizeof(WCHAR));
  cNewItem->szDllNameW[nLen] = 0;
  cNewItem->szParametersW = cNewItem->szDllNameW + (nLen+1);
  memcpy(cNewItem->szParametersW, szParametersW, nParamsLen*sizeof(WCHAR));
  cNewItem->szParametersW[nParamsLen] = 0;
  if (aHandlerDllsList.AddElement((CUSTOM_HANDLER_ITEM*)cNewItem) == FALSE)
    return E_OUTOFMEMORY;
  cNewItem.Detach(); //leave handler list to manage the item
  return S_OK;
}

VOID CNktDvHook::RemoveAllHandlers(__in BOOL bInDestructor)
{
  if (bInDestructor == FALSE)
    AddRef();
  {
    CNktAutoFastMutex cLock(&GetLockObject());

    aHandlerDllsList.RemoveAllElements();
  }
  if (bInDestructor == FALSE)
    Release();
  return;
}

HRESULT CNktDvHook::SendCustomHandlerData(__out LPDWORD lpdwBigPacketId, __in DWORD dwPid,
                                          __in CNktDvTransportEngine *lpTransport)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  CNktAutoFastMutex cLock(&GetLockObject());
  LPBYTE lpData, d;
  SIZE_T i, nCount, k[2], nDataSize;
  HRESULT hRes;

  NKT_ASSERT(lpdwBigPacketId != NULL);
  *lpdwBigPacketId = 0;
  nCount = aHandlerDllsList.GetCount();
  if (nCount == 0)
    return S_OK;
  //calculate data size
  nDataSize = sizeof(ULONG);
  for (i=0; i<nCount; i++)
  {
    nDataSize += 3*sizeof(ULONG);
    nDataSize += wcslen(aHandlerDllsList[i]->szDllNameW) * sizeof(WCHAR);
    nDataSize += wcslen(aHandlerDllsList[i]->szParametersW) * sizeof(WCHAR);
  }
  lpData = d = (LPBYTE)nktMemMalloc(nDataSize);
  if (lpData == NULL)
    return E_OUTOFMEMORY;
  //fill with count
  *((ULONG NKT_UNALIGNED *)d) = (ULONG)nCount;
  d += sizeof(ULONG);
  //fill with data
  for (i=0; i<nCount; i++)
  {
    *((ULONG NKT_UNALIGNED *)d) = aHandlerDllsList[i]->nFlags;
    d += sizeof(ULONG);
    k[0] = wcslen(aHandlerDllsList[i]->szDllNameW);
    k[1] = wcslen(aHandlerDllsList[i]->szParametersW);
    *((ULONG NKT_UNALIGNED *)d) = (ULONG)k[0];
    d += sizeof(ULONG);
    *((ULONG NKT_UNALIGNED *)d) = (ULONG)k[1];
    d += sizeof(ULONG);
    k[0] *= sizeof(WCHAR);
    k[1] *= sizeof(WCHAR);
    nktMemCopy(d, aHandlerDllsList[i]->szDllNameW, k[0]);
    d += k[0];
    nktMemCopy(d, aHandlerDllsList[i]->szParametersW, k[1]);
    d += k[1];
  }
  hRes = lpTransport->SendBigPacketMsg(lpdwBigPacketId, dwPid, lpData, nDataSize);
  nktMemFree(lpData);
  return hRes;
}

CNktDvHook::eState CNktDvHook::GetProcessState(__in CNktDvProcess *lpProc)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookGetProcessState(this, lpProc)) :
                                    CNktDvHook::stNotFound;
}

CNktDvHook::eState CNktDvHook::GetProcessState(__in DWORD dwPid)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return CNktDvHook::stError;
  return GetProcessState(cProc);
}

HRESULT CNktDvHook::QueryOverwrite(__in CNktDvProcess *lpProc)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;
  TNktArrayList<BYTE> cResult;
  HRESULT hRes;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  if (cEngineAutoRef == NULL)
    return NKT_DVERR_NotFound;
  hRes = cEngineAutoRef->HookQueryOverwrite(this, lpProc, cResult);
  if (SUCCEEDED(hRes))
  {
    _ASSERT(cResult.GetCount() == 1);
    if (cResult[0] == 0)
      hRes = S_FALSE;
  }
  return hRes;
}

HRESULT CNktDvHook::QueryOverwrite(__in DWORD dwPid)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return E_OUTOFMEMORY;
  return QueryOverwrite(cProc);
}

HRESULT CNktDvHook::GetLastErrorAtProc(__in CNktDvProcess *lpProc)
{
  TNktComPtr<CNktDvHook> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookGetLastError(this, lpProc)) : NKT_DVERR_NotFound;
}

HRESULT CNktDvHook::GetLastErrorAtProc(__in DWORD dwPid)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return E_OUTOFMEMORY;
  return GetLastErrorAtProc(cProc);
}

VOID CNktDvHook::Unlink()
{
  CNktAutoFastMutex cLock(&GetLockObject());

  lpEngine = NULL;
  return;
}

HRESULT CNktDvHook::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvHook [ID:0x%IX / %s]", GetId(), GetFunctionName()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

//-----------------------------------------------------------

CNktDvHooksEnumerator::CNktDvHooksEnumerator() : TNktDvEnumerator<CNktDvHook>(),
                                                 TNktLnkLstNode<CNktDvHooksEnumerator>()
{
  lpEngine = NULL;
  bDestroyed = FALSE;
  return;
}

CNktDvHooksEnumerator::~CNktDvHooksEnumerator()
{
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  if (cEngineAutoRef != NULL)
    cEngineAutoRef->HooksEnumDestroy(this);
  //should not belong to any list
  NKT_ASSERT(TNktLnkLstNode<CNktDvHooksEnumerator>::GetLinkedList() == NULL); 
  return;
}

HRESULT CNktDvHooksEnumerator::Add(__inout CNktDvHook *lpObj)
{
  return TNktDvEnumerator<CNktDvHook>::Add(lpObj);
}

HRESULT CNktDvHooksEnumerator::AddProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookAddProcess(this, lpProc, bSync)) : E_FAIL;
}

HRESULT CNktDvHooksEnumerator::AddProcess(__in DWORD dwPid, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return AddProcess(cProc, bSync);
}

HRESULT CNktDvHooksEnumerator::RemoveProcess(__inout CNktDvProcess *lpProc, __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookRemoveProcess(this, lpProc, bSync)) : E_FAIL;
}

HRESULT CNktDvHooksEnumerator::RemoveProcess(__in DWORD dwPid, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return RemoveProcess(cProc, bSync);
}

HRESULT CNktDvHooksEnumerator::EnableProcess(__inout CNktDvProcess *lpProc, __in BOOL bEnable,
                                             __in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvProcess> cProcAutoRef(lpProc);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookEnableProcess(this, lpProc, bEnable, bSync)) :
                                    E_FAIL;
}

HRESULT CNktDvHooksEnumerator::EnableProcess(__in DWORD dwPid, __in BOOL bEnable, __in BOOL bSync)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
    return hRes;
  return EnableProcess(cProc, bEnable, bSync);
}

HRESULT CNktDvHooksEnumerator::Hook(__in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());
    SIZE_T i, nCount;

    nCount = aObjectsList.GetCount();
    for (i=0; i<nCount; i++)
    {
      NktInterlockedOr(&(aObjectsList[i]->nFlags), (LONG)CNktDvHook::flgAutoHookActive);
    }
    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookInstall(this, bSync)) : E_FAIL;
}

HRESULT CNktDvHooksEnumerator::Unhook(__in BOOL bSync)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    SIZE_T i, nCount;

    nCount = aObjectsList.GetCount();
    for (i=0; i<nCount; i++)
    {
      NktInterlockedAnd(&(aObjectsList[i]->nFlags), ~((LONG)CNktDvHook::flgAutoHookActive));
    }
    cEngineAutoRef = lpEngine;
  }
  return (cEngineAutoRef != NULL) ? (cEngineAutoRef->HookUninstall(this, bSync)) : E_FAIL;
}

HRESULT CNktDvHooksEnumerator::QueryOverwrite(__inout CNktDvProcess *lpProc, __inout TNktArrayList<BYTE> &cResult)
{
  TNktComPtr<CNktDvHooksEnumerator> cHookAutoRef(this);
  TNktComPtr<CNktDvSpyMgr> cEngineAutoRef;

  {
    CNktAutoFastMutex cLock(&GetLockObject());

    cEngineAutoRef = lpEngine;
  }
  if (cEngineAutoRef == NULL)
  {
    cResult.RemoveAllElements();
    return E_FAIL;
  }
  return cEngineAutoRef->HookQueryOverwrite(this, lpProc, cResult);
}

HRESULT CNktDvHooksEnumerator::QueryOverwrite(__in DWORD dwPid, __inout TNktArrayList<BYTE> &cResult)
{
  TNktComPtr<CNktDvProcess> cProc;
  HRESULT hRes;

  hRes = CNktDvProcess::CreateFromPID(&cProc, dwPid);
  if (FAILED(hRes))
  {
    cResult.RemoveAllElements();
    return hRes;
  }
  return QueryOverwrite(cProc, cResult);
}

HRESULT CNktDvHooksEnumerator::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvHooksEnumerator [%Iu items]", GetCount()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

VOID CNktDvHooksEnumerator::Unlink()
{
  CNktAutoFastMutex cLock(&GetLockObject());

  lpEngine = NULL;
  return;
}
