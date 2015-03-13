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

#include "ProcessHandle.h"
#include "DynamicAPIs.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static LONG volatile nDvPhMgrLock = 0;
static CNktDvProcessHandlesMgr *lpDvPhMgr = NULL;

//-----------------------------------------------------------

static __inline DWORD RecalcAccess(__in DWORD dwAccess)
{
  return ((dwAccess & PROCESS_ALL_ACCESS) == PROCESS_ALL_ACCESS) ? 0xFFFFFFFFUL : dwAccess;
}

//-----------------------------------------------------------

CNktDvProcessHandlesMgr::CNktDvProcessHandlesMgr() : CNktDvObject(), CNktFastMutex(), CNktThread()
{
  return;
}

CNktDvProcessHandlesMgr::~CNktDvProcessHandlesMgr()
{
  {
    CNktAutoFastMutex cLock(this);

    lpDvPhMgr = NULL;
    //----
    sHandlesList.RemoveAllElements();
  }
  //----
  Stop();
  return;
}

HRESULT CNktDvProcessHandlesMgr::Get(__deref_out CNktDvProcessHandlesMgr **lplpPhMgr)
{
  HRESULT hRes;

  if (lplpPhMgr == NULL)
    return E_POINTER;
  hRes = S_OK;
  {
    CNktSimpleLockNonReentrant cLock(&nDvPhMgrLock);

    if (lpDvPhMgr == NULL)
    {
      lpDvPhMgr = NKT_MEMMGR_NEW CNktDvProcessHandlesMgr;
      hRes = (lpDvPhMgr != NULL) ? S_OK : E_OUTOFMEMORY;
      if (SUCCEEDED(hRes))
        hRes = lpDvPhMgr->Initialize();
    }
    else
    {
      lpDvPhMgr->AddRef();
    }
  }
  if (SUCCEEDED(hRes))
  {
    *lplpPhMgr = lpDvPhMgr;
  }
  else
  {
    NKT_DV_RELEASE(lpDvPhMgr);
    *lplpPhMgr = NULL;
  }
  return hRes;
}

ULONG CNktDvProcessHandlesMgr::Release()
{
  CNktSimpleLockNonReentrant cLock(&nDvPhMgrLock);

  return CNktDvObject::Release();
}

HANDLE CNktDvProcessHandlesMgr::CreateHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess)
{
  HANDLE hToken, hProc;
  TOKEN_PRIVILEGES sTokPriv;
  BOOL bRevertToSelf;

  if (dwProcessId == 0)
    return NULL;
  //try to enable SeDebugPrivilege
  bRevertToSelf = FALSE;
  if (::OpenThreadToken(::GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, FALSE, &hToken) == FALSE)
  {
    hToken = NULL;
    if (::ImpersonateSelf(SecurityImpersonation) != FALSE)
    {
      bRevertToSelf = TRUE;
      if (::OpenThreadToken(::GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, FALSE, &hToken) == FALSE)
        hToken = NULL;
    }
  }
  nktMemSet(&sTokPriv, 0, sizeof(sTokPriv));
  if (hToken != NULL)
  {
    sTokPriv.PrivilegeCount = (::LookupPrivilegeValueW(NULL, L"SeDebugPrivilege",
                                                       &(sTokPriv.Privileges[0].Luid)) != FALSE) ? 1 : 0;
  }
  if (sTokPriv.PrivilegeCount > 0)
  {
    sTokPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    ::AdjustTokenPrivileges(hToken, 0, &sTokPriv, sizeof(sTokPriv), NULL, NULL);
  }
  //open process
  hProc = ::OpenProcess(dwDesiredAccess, FALSE, dwProcessId);
  if (hProc == NULL)
  {
    if ((dwDesiredAccess & (PROCESS_QUERY_INFORMATION|PROCESS_QUERY_LIMITED_INFORMATION)) ==
                            PROCESS_QUERY_INFORMATION)
    {
      dwDesiredAccess &= (~PROCESS_QUERY_INFORMATION);
      dwDesiredAccess |= PROCESS_QUERY_LIMITED_INFORMATION;
      hProc = ::OpenProcess(dwDesiredAccess, FALSE, dwProcessId);
    }
  }
  //restore privileges
  if (sTokPriv.PrivilegeCount > 0)
  {
    sTokPriv.Privileges[0].Attributes = 0;
    ::AdjustTokenPrivileges(hToken, 0, &sTokPriv, sizeof(sTokPriv), NULL, NULL);
  }
  if (bRevertToSelf != FALSE)
    ::RevertToSelf();
  if (hToken != NULL)
    ::CloseHandle(hToken);
  //done
  return hProc;
}

HANDLE CNktDvProcessHandlesMgr::InternalGetHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess)
{
  HANDLE hProc;
  ITEM sNewItem;
  DWORD dwNewAccess;
  SIZE_T nIndex;

  nIndex = Find(dwProcessId);
  if (nIndex != NKT_SIZE_T_MAX)
  {
    //the process is already on the list, check if access is better and replace if needed
    dwNewAccess = sHandlesList[nIndex].dwCurrAccess | dwDesiredAccess;
    if (dwNewAccess == sHandlesList[nIndex].dwCurrAccess)
    {
      //the current handle already has the needed access
      hProc = sHandlesList[nIndex].hProc;
    }
    else
    {
      //create a replacement handle
      hProc = CNktDvProcessHandlesMgr::CreateHandle(dwProcessId, dwNewAccess);
      if (hProc != NULL)
      {
        //replace old handle
        ::CloseHandle(sHandlesList[nIndex].hProc);
        sHandlesList[nIndex].hProc = hProc;
        sHandlesList[nIndex].dwCurrAccess = RecalcAccess(dwNewAccess);
      }
    }
  }
  else
  {
    //create handle
    if (dwDesiredAccess == 0)
    {
      dwDesiredAccess = PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_OPERATION|
                        SYNCHRONIZE|STANDARD_RIGHTS_READ;
    }
    hProc = CNktDvProcessHandlesMgr::CreateHandle(dwProcessId, dwDesiredAccess);
    if (hProc != NULL)
    {
      //add the new item
      sNewItem.dwProcId = dwProcessId;
      sNewItem.dwCurrAccess = RecalcAccess(dwDesiredAccess);
      sNewItem.hProc = hProc;
      if (sHandlesList.SortedInsert(&sNewItem, &CNktDvProcessHandlesMgr::InsertById) == FALSE)
      {
        ::CloseHandle(hProc);
        hProc = NULL;
      }
    }
  }
  return hProc;
}

HANDLE CNktDvProcessHandlesMgr::GetHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess,
                                                      __in BOOL bInheritable)
{
  CNktAutoFastMutex cLock(this);
  HANDLE hProc, hProcCopy;

  hProc = InternalGetHandle(dwProcessId, dwDesiredAccess);
  if (hProc == NULL)
    return NULL;
  if (FAILED(nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), hProc, ::GetCurrentProcess(), &hProcCopy, 0,
                                          bInheritable, DUPLICATE_SAME_ACCESS) == FALSE))
    return NULL;
  return hProcCopy;
}

HRESULT CNktDvProcessHandlesMgr::AddHandle(__in DWORD dwProcessId, __in HANDLE hProc,
                                           __in DWORD dwCurrentAccess)
{
  CNktAutoFastMutex cLock(this);
  SIZE_T nIndex;
  DWORD dwNewAccess;
  ITEM sNewItem;

  if (dwProcessId == 0 || hProc == NULL || dwCurrentAccess == 0)
    return E_INVALIDARG;
  nIndex = Find(dwProcessId);
  if (nIndex != NKT_SIZE_T_MAX)
  {
    //the process is already on the list, check if access is better and replace if needed
    dwNewAccess = sHandlesList[nIndex].dwCurrAccess | dwCurrentAccess;
    if (dwNewAccess != sHandlesList[nIndex].dwCurrAccess)
    {
      //new handle has more access flags
      sHandlesList[nIndex].dwCurrAccess = RecalcAccess(dwNewAccess);
      ::CloseHandle(sHandlesList[nIndex].hProc);
      sHandlesList[nIndex].hProc = hProc;
    }
  }
  else
  {
    //add the new item
    sNewItem.dwProcId = dwProcessId;
    sNewItem.dwCurrAccess = RecalcAccess(dwCurrentAccess);
    sNewItem.hProc = hProc;
    if (sHandlesList.SortedInsert(&sNewItem, &CNktDvProcessHandlesMgr::InsertById) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

VOID CNktDvProcessHandlesMgr::RemoveHandle(__in DWORD dwProcessId)
{
  CNktAutoFastMutex cLock(this);
  SIZE_T nIndex;

  nIndex = Find(dwProcessId);
  if (nIndex != NKT_SIZE_T_MAX)
    sHandlesList.RemoveElementAt(nIndex);
  return;
}

HRESULT CNktDvProcessHandlesMgr::Initialize()
{
  CNktAutoFastMutex cLock(this);

  SetThreadName("Dv-ProcessHandlesMgr");
  return (Start() != FALSE) ? S_OK : E_OUTOFMEMORY;
}

VOID CNktDvProcessHandlesMgr::ThreadProc()
{
  while (CheckForAbort(1000) == FALSE)
    RemoveInvalid();
  return;
}

SIZE_T CNktDvProcessHandlesMgr::Find(__in DWORD dwProcessId)
{
  ITEM *lpItem, *lpBuffer;

  lpBuffer = sHandlesList.GetBuffer();
  lpItem = (ITEM*)bsearch_s(&dwProcessId, lpBuffer, sHandlesList.GetCount(), sizeof(ITEM),
                            &CNktDvProcessHandlesMgr::SearchById, NULL);
  return (lpItem != NULL) ? (SIZE_T)(lpItem-lpBuffer) : NKT_SIZE_T_MAX;
}

VOID CNktDvProcessHandlesMgr::RemoveInvalid()
{
  CNktAutoFastMutex cLock(this);
  SIZE_T i, nCount;
  DWORD dwExitCode;

  nCount = sHandlesList.GetCount();
  i = 0;
  while (i < nCount)
  {
    if ((::GetExitCodeProcess(sHandlesList[i].hProc, &dwExitCode) != FALSE &&
         dwExitCode != STILL_ACTIVE) ||
        ::WaitForSingleObject(sHandlesList[i].hProc, 0) == WAIT_OBJECT_0)
    {
      sHandlesList.RemoveElementAt(i);
      nCount--;
    }
    else
    {
      i++;
    }
  }
  return;
}

int __cdecl CNktDvProcessHandlesMgr::InsertById(__in void *, __in ITEM *lpElem1, __in ITEM *lpElem2)
{
  return NKT_DV_CMP<DWORD>(lpElem1->dwProcId, lpElem2->dwProcId);
}

int __cdecl CNktDvProcessHandlesMgr::SearchById(void *lpContext, const void *lpKey, const void *elem)
{
  return NKT_DV_CMP<DWORD>(*((LPDWORD)lpKey), reinterpret_cast<ITEM*>(const_cast<void*>(elem))->dwProcId);
}
