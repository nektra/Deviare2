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

#include "InternalProcessesList.h"
#include "..\..\Common\Tools.h"
#include "..\..\Common\ProcessHandle.h"
#include "..\..\Common\AutoPtr.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define UPDATE_PERIOD_MS                                 200

//-----------------------------------------------------------

CNktDvInternalProcessesList::CNktDvInternalProcessesList(CNktDvInternalProcessesListCallback *_lpCB) :
                                 CNktThread(), CNktFastMutex()
{
  NKT_ASSERT(_lpCB != NULL);
  lpCallback = _lpCB;
  return;
}

CNktDvInternalProcessesList::~CNktDvInternalProcessesList()
{
  Finalize();
  return;
}

HRESULT CNktDvInternalProcessesList::Initialize()
{
  CNktAutoFastMutex cLock(this);
  HRESULT hRes;

  Finalize();
  //shutdown locker
  hRes = cShutdownMtx.Initialize();
  //refresh
  if (SUCCEEDED(hRes))
    hRes = Refresh(FALSE);
  //start background thread
  if (SUCCEEDED(hRes))
  {
    if (Start() == FALSE)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
    SetThreadName("Dv-SpyMgrProcessList");
  if (FAILED(hRes))
    Finalize();
  return hRes;
}

VOID CNktDvInternalProcessesList::Finalize()
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CProcessItem>::Iterator it;
  CProcessItem *lpItem;

  //signal shutdown
  cShutdownMtx.StartShutdownAndWaitPending(this);
  //stop worker thread
  Stop();
  //clear data
  for (lpItem=it.Begin(cProcessList); lpItem!=NULL; lpItem=it.Next())
  {
    delete lpItem;
  }
  cShutdownMtx.Finalize();
  return;
}

HRESULT CNktDvInternalProcessesList::Refresh(__in BOOL bNotify)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);
  TNktLnkLst<CProcessItem>::Iterator it;
  CProcessItem *lpItem;
  HANDLE hSnapshot;
  PROCESSENTRY32W sPe32_W;
  HRESULT hRes;
  BOOL bFirst;
  TNktLnkLst<CProcessItem> cAddedProcList, cRemovedProcList;

  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL;
  //mark active processes
  for (lpItem=it.Begin(cProcessList); lpItem!=NULL; lpItem=it.Next())
    lpItem->bRemoveMark = TRUE;
  //create process snapshot
  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return NKT_HRESULT_FROM_LASTERROR();
  bFirst = TRUE;
  while (1)
  {
    nktMemSet(&sPe32_W, 0, sizeof(sPe32_W));
    sPe32_W.dwSize = (DWORD)sizeof(sPe32_W);
    ::SetLastError(NOERROR);
    //try
    //{
    hRes = ((bFirst != FALSE) ? (::Process32FirstW(hSnapshot, &sPe32_W)) :
                                (::Process32NextW(hSnapshot, &sPe32_W))) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
    //}
    //catch (...)
    //{
    //  hRes = NKT_DVERR_ExceptionRaised;
    //}
    if (FAILED(hRes))
    {
      if (hRes == NKT_HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS) ||
          hRes == NKT_HRESULT_FROM_WIN32(ERROR_NO_MORE_FILES))
        hRes = S_OK;
      break;
    }
    bFirst = FALSE;
    //check if this process already exists
    for (lpItem=it.Begin(cProcessList); lpItem!=NULL; lpItem=it.Next())
    {
      if (lpItem->cProc->GetProcessId() == sPe32_W.th32ProcessID)
        break;
    }
    if (lpItem != NULL)
    {
      //process still active
      lpItem->bRemoveMark = FALSE;
    }
    else
    {
      hRes = AddItem(cAddedProcList, &sPe32_W);
      if (FAILED(hRes))
        break;
    }
  }
  //close snapshot
  ::CloseHandle(hSnapshot);
  //move marked items to the "to remove" list
  for (lpItem=it.Begin(cProcessList); lpItem!=NULL; lpItem=it.Next())
  {
    if (lpItem->bRemoveMark != FALSE)
    {
      cProcessList.Remove(lpItem);
      cRemovedProcList.PushTail(lpItem);
    }
  }
  //do notifications
  if (bNotify != FALSE)
  {
    CNktAutoFastMutexReverse cLockRev(this);

    ExecNotification(cAddedProcList, cRemovedProcList);
  }
  //add new processes to the main list
  for (lpItem=it.Begin(cAddedProcList); lpItem!=NULL; lpItem=it.Next())
  {
    cAddedProcList.Remove(lpItem);
    cProcessList.PushTail(lpItem);
  }
  //remove ended process from the main list
  for (lpItem=it.Begin(cRemovedProcList); lpItem!=NULL; lpItem=it.Next())
  {
    CNktDvProcess::MarkModulesEnumeratorAsDirty(lpItem->cProc->GetProcessId(), NULL);
    delete lpItem;
  }
  //done
  return hRes;
}

HRESULT CNktDvInternalProcessesList::GetEnumerator(__out CNktDvProcessesEnumerator **lplpEnum)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  CNktDvAutoCancellableFastMutex cLock(this, &cShutdownMtx);
  TNktComPtr<CNktDvProcessesEnumerator> cEnum;
  TNktLnkLst<CProcessItem>::Iterator it;
  CProcessItem *lpItem;
  HRESULT hRes;

  if (lplpEnum != NULL)
    *lplpEnum = NULL;
  if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  if (cShutdownMtx.IsSystemActive() == FALSE)
    return E_FAIL;
  if (lplpEnum == NULL)
    return E_POINTER;
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvProcessesEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  for (lpItem=it.Begin(cProcessList); lpItem!=NULL; lpItem=it.Next())
  {
    hRes = cEnum->Add(lpItem->cProc);
    if (FAILED(hRes))
      return hRes;
  }
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

VOID CNktDvInternalProcessesList::ThreadProc()
{
  lpCallback->IPL_WorkerThreadStarted();
  //----
  while (CheckForAbort(UPDATE_PERIOD_MS) == FALSE)
    Refresh(TRUE);
  //----
  lpCallback->IPL_WorkerThreadEnded();
  return;
}

HRESULT CNktDvInternalProcessesList::AddItem(__inout TNktLnkLst<CProcessItem> &cNewProcList,
                                             __in LPPROCESSENTRY32W lpEntryW)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  TNktAutoDeletePtr<CProcessItem> cNewItem;
  HRESULT hRes;
  HANDLE hProc;
  DWORD dwExitCode;

  if (lpEntryW->th32ProcessID == 0)
    return S_OK; //ignore system processes
  if (lpEntryW->th32ProcessID == 4)
    lpEntryW->szExeFile[0] = 0;
  //create new item
  cNewItem.Attach(NKT_MEMMGR_NEW CProcessItem);
  if (cNewItem == NULL)
    return E_OUTOFMEMORY;
  cNewItem->cProc.Attach(NKT_MEMMGR_NEW CNktDvProcess);
  if (cNewItem->cProc == NULL)
    return E_OUTOFMEMORY;
  cNewItem->cProc->dwProcessID = lpEntryW->th32ProcessID;
  cNewItem->cProc->dwParentProcessID = lpEntryW->th32ParentProcessID;
  cNewItem->cProc->nThreadsCount = (SIZE_T)(lpEntryW->cntThreads);
  cNewItem->cProc->nCpuUsage = (SIZE_T)(lpEntryW->cntUsage);
  cNewItem->cProc->nPriClassBase = lpEntryW->pcPriClassBase;
  cNewItem->cProc->dwInternalFlags = 2;
  //open the process to get info
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(lpEntryW->th32ProcessID, SYNCHRONIZE|PROCESS_QUERY_INFORMATION|
                              PROCESS_VM_READ);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    if (::GetExitCodeProcess(hProc, &dwExitCode) != FALSE &&
        dwExitCode != STILL_ACTIVE)
    {
      ::CloseHandle(hProc);
      return S_OK;
    }
  }
  //if hProcess is null only the 'szExeFileW' field will be set
  hRes = cNewItem->cProc->InitInfo(hProc, lpEntryW->szExeFile);
  //----
  if (SUCCEEDED(hRes))
  {
    cNewProcList.PushTail(cNewItem.Detach());
  }
  //done
  ::CloseHandle(hProc);
  return hRes;
}

VOID CNktDvInternalProcessesList::ExecNotification(__inout TNktLnkLst<CProcessItem> &cAddedProcList,
                                                   __inout TNktLnkLst<CProcessItem> &cRemovedProcList)
{
  TNktLnkLst<CProcessItem>::Iterator it;
  TNktComPtr<CNktDvProcess> cProcessAutoRef;
  CProcessItem *lpItem;

  //removed processes
  for (lpItem=it.Begin(cRemovedProcList); lpItem!=NULL; lpItem=it.Next())
  {
    cProcessAutoRef = lpItem->cProc;
    lpCallback->IPL_OnProcessRemoved(lpItem->cProc);
  }
  //added processes
  for (lpItem=it.Begin(cAddedProcList); lpItem!=NULL; lpItem=it.Next())
  {
    cProcessAutoRef = lpItem->cProc;
    lpCallback->IPL_OnProcessAdded(lpItem->cProc);
  }
  return;
}
