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

#include "Process.h"
#include "Tools.h"
#include "ProcessHandle.h"
#include "EngObjCache.h"
#include "ProcessMemory.h"
#include <userenv.h>

#pragma comment(lib, "Userenv.lib")

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

typedef struct {
  DWORD dwPid;
  HWND hWnd;
} EWP_FindProcessWindow_Data;


//-----------------------------------------------------------

typedef BOOL (WINAPI *lpfnCreateProcessAsUserW)(__in_opt HANDLE hToken, __in_opt LPCWSTR lpApplicationName,
                                __inout_opt LPWSTR lpCommandLine, __in_opt LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                __in_opt LPSECURITY_ATTRIBUTES lpThreadAttributes, __in BOOL bInheritHandles,
                                __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment,
                                __in_opt LPCWSTR lpCurrentDirectory, __in LPSTARTUPINFOW lpStartupInfo,
                                __out LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL (WINAPI *lpfnCreateProcessWithTokenW)(__in HANDLE hToken, __in DWORD dwLogonFlags,
                                __in_opt LPCWSTR lpApplicationName, __inout_opt LPWSTR lpCommandLine,
                                __in DWORD dwCreationFlags, __in_opt LPVOID lpEnvironment,
                                __in_opt LPCWSTR lpCurrentDirectory, __in LPSTARTUPINFOW lpStartupInfo,
                                __out LPPROCESS_INFORMATION lpProcessInformation);

//-----------------------------------------------------------

static BOOL CALLBACK EWP_FindProcessWindow(__in HWND hWnd, __in LPARAM lParam);

//-----------------------------------------------------------

class CInternalModEnumCache : private CNktFastMutex
{
public:
  typedef TNktDvEngObjCache<CNktDvModulesEnumerator, 17, 32> TMyModEnumCache;

  CInternalModEnumCache()
    {
    lpCache = NKT_MEMMGR_NEW TMyModEnumCache;
    return;
    };

  ~CInternalModEnumCache()
    {
    if (lpCache != NULL)
    {
      BOOL bShutdownInProgress;

      nktDvDynApis_RtlDllShutdownInProgress(&bShutdownInProgress);
      if (bShutdownInProgress == FALSE)
        delete lpCache;
      lpCache = NULL;
    }
    return;
    };

  virtual HRESULT Get(__deref_out CNktDvModulesEnumerator **lplpModEnum, __in DWORD dwPid, __in SIZE_T nPlatformBits)
    {
    TNktComPtr<CNktDvModulesEnumerator> cModEnum, cModEnumClone;
    HRESULT hRes;

    if (lplpModEnum == NULL)
      return E_POINTER;
    *lplpModEnum = NULL;
    if (dwPid == 0)
      return E_INVALIDARG;
    if (lpCache == NULL)
      return E_OUTOFMEMORY;
    if (nPlatformBits == -1)
    {
      TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
      HANDLE hProcess;

      hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
      if (SUCCEEDED(hRes))
      {
        hProcess = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
        if (hProcess != NULL)
        {
          nPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
          ::CloseHandle(hProcess);
        }
      }
    }
    //get from cache
    {
      CNktAutoFastMutex cLock(lpCache->GetLock((Fnv64_t)dwPid));

      cModEnum.Attach(lpCache->Get((Fnv64_t)dwPid));
      if (cModEnum != NULL &&
          cModEnum->GetPlatformBits() != nPlatformBits)
        cModEnum.Release(); //release on platforms bits mismatch
      //check dirty
      if (cModEnum != NULL)
      {
        if (FAILED(cModEnum->CheckDirtyModules()))
          cModEnum.Release(); //release on fail and rebuild from scratch
      }
      //if we have a modenum, clone it
      if (cModEnum != NULL)
      {
        cModEnumClone.Attach(cModEnum->Clone());
        if (cModEnumClone == NULL)
          return E_OUTOFMEMORY;
        cModEnum.Attach(cModEnumClone.Detach());
      }
      else
      {
        lpCache->Remove((Fnv64_t)dwPid);
      }
    }
    //do we have an enumerator
    if (cModEnum == NULL)
    {
      //create a new one
      cModEnum.Attach(NKT_MEMMGR_NEW CNktDvModulesEnumerator);
      if (cModEnum == NULL)
        return E_OUTOFMEMORY;
      hRes = cModEnum->Initialize(dwPid, nPlatformBits);
      if (SUCCEEDED(hRes))
        hRes = lpCache->Add((Fnv64_t)dwPid, cModEnum, TRUE);
      if (SUCCEEDED(hRes))
      {
        //always return a clone
        cModEnumClone.Attach(cModEnum->Clone());
        if (cModEnumClone != NULL)
          cModEnum.Attach(cModEnumClone.Detach());
        else
          hRes = E_OUTOFMEMORY;
      }
      if (FAILED(hRes))
        return hRes;
    }
    //done
    *lplpModEnum = cModEnum.Detach();
    return S_OK;
    };

  virtual VOID Remove(__in DWORD dwPid, __in HINSTANCE hDll)
    {
    TNktComPtr<CNktDvModulesEnumerator> cModEnum;

    if (lpCache != NULL)
    {
      CNktAutoFastMutex cLock(lpCache->GetLock((Fnv64_t)dwPid));

      if (hDll == NULL)
      {
        lpCache->Remove((Fnv64_t)dwPid);
      }
      else
      {
        //mark it as dirty
        cModEnum.Attach(lpCache->Get((Fnv64_t)dwPid));
        if (cModEnum != NULL)
        {
          if (FAILED(cModEnum->MarkModuleAsDirty(hDll)))
            lpCache->Remove((Fnv64_t)dwPid);
        }
      }
    }
    return;
    };

  virtual VOID Flush()
    {
    if (lpCache != NULL)
      lpCache->RemoveAll();
    return;
    };

public:
  TMyModEnumCache *lpCache;
};

static CInternalModEnumCache cModEnumCache;

//-----------------------------------------------------------

CNktDvProcess::CNktDvProcess() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  dwProcessID = dwParentProcessID = 0;
  nPlatformBits = 0;
  nThreadsCount = nCpuUsage = 0;
  nPriClassBase = 0;
  dwInternalFlags = 0;
  nNameOffsetInPathStr = 0;
  return;
}

CNktDvProcess::~CNktDvProcess()
{
  return;
}

HRESULT CNktDvProcess::CreateFromPID(__deref_out CNktDvProcess **lplpProc, __in DWORD dwPid)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  TNktComPtr<CNktDvProcess> cProc;
  HANDLE hSnapshot;
  PROCESSENTRY32W sPe32_W;
  HANDLE hProc;
  HRESULT hRes;
  BOOL bFirst;

  if (lplpProc == NULL)
    return E_POINTER;
  *lplpProc = NULL;
  if (dwPid == 0)
    return E_INVALIDARG;
  //find process from snapshot
  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return NKT_HRESULT_FROM_LASTERROR();
  bFirst = TRUE;
  do
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
    if (SUCCEEDED(hRes) && sPe32_W.th32ProcessID == dwPid)
      break;
    sPe32_W.th32ProcessID = 0;
    bFirst = FALSE;
  }
  while (SUCCEEDED(hRes));
  ::CloseHandle(hSnapshot);
  DWORD currPid = ::GetCurrentProcessId();
  MANDATORY_LEVEL level = CNktDvTools::GetProcessIntegrityLevel(currPid);
  if (sPe32_W.th32ProcessID == 0 && (level >= MandatoryLevelMedium || currPid != dwPid))
    return NKT_DVERR_NotFound; //process not found
  cProc.Attach(NKT_MEMMGR_NEW CNktDvProcess);
  if (cProc == NULL)
    return E_OUTOFMEMORY;
  if (sPe32_W.th32ProcessID == 0 && level < MandatoryLevelMedium && currPid == dwPid) {
    cProc->dwProcessID = currPid;
    cProc->dwParentProcessID = 0;
    cProc->nThreadsCount = 1;
    cProc->nCpuUsage = 0;
    cProc->nPriClassBase = 0;
  }
  else {
    cProc->dwProcessID = sPe32_W.th32ProcessID;
    cProc->dwParentProcessID = sPe32_W.th32ParentProcessID;
    cProc->nThreadsCount = (SIZE_T)(sPe32_W.cntThreads);
    cProc->nCpuUsage = (SIZE_T)(sPe32_W.cntUsage);
    cProc->nPriClassBase = sPe32_W.pcPriClassBase;
  }
  //----
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  hProc = (SUCCEEDED(hRes)) ? (cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION)) : NULL;
  //if hProc is null only the 'szExeFileW' field will be set
  hRes = cProc->InitInfo(hProc, sPe32_W.szExeFile);
  if (sPe32_W.th32ProcessID == 0 && level < MandatoryLevelMedium && currPid == dwPid) {
#if defined _M_IX86
    cProc->nPlatformBits = 32;
#elif defined _M_X64
    cProc->nPlatformBits = 64;
#endif
  }
  if (hProc != NULL)
    ::CloseHandle(hProc);
  if (SUCCEEDED(hRes))
    *lplpProc = cProc.Detach();
  return hRes;
}

HRESULT CNktDvProcess::IsActive(__in DWORD dwWaitTimeMs)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  EWP_FindProcessWindow_Data sData;
  HANDLE hProc;
  HRESULT hRes;
  DWORD_PTR dwRes;
  DWORD dwExitCode;

  //find the process window
  if (dwProcessID == 0)
    return E_FAIL;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(dwProcessID, PROCESS_QUERY_INFORMATION|SYNCHRONIZE);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    if (::GetExitCodeProcess(hProc, &dwExitCode) != FALSE &&
        dwExitCode != STILL_ACTIVE)
      hRes = S_FALSE;
    if (hRes == S_OK &&
        ::WaitForSingleObject(hProc, dwWaitTimeMs) == WAIT_OBJECT_0)
      hRes = S_FALSE;
    ::CloseHandle(hProc);
  }
  else
  {
    hRes = S_OK;
    sData.dwPid = dwProcessID;
    sData.hWnd = NULL;
    ::EnumWindows(EWP_FindProcessWindow, (LPARAM)&sData);
    if (sData.hWnd != NULL)
    {
      //send a null message
      ::SendMessageTimeoutW(sData.hWnd, WM_NULL, 0, 0, SMTO_ABORTIFHUNG|SMTO_NOTIMEOUTIFNOTHUNG,
                            (UINT)dwWaitTimeMs, &dwRes);
      if (::GetLastError() == ERROR_TIMEOUT)
        hRes = S_FALSE;
    }
  }
  return hRes;
}

HRESULT CNktDvProcess::GetHandle(__out HANDLE *lphProc, __in DWORD dwDesiredAccess)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;

  if (lphProc == NULL)
    return E_POINTER;
  *lphProc = NULL;
  if (dwProcessID == 0)
    return E_FAIL;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    *lphProc = cPhMgr->GetHandle(dwProcessID, dwDesiredAccess, TRUE);
    if ((*lphProc) == NULL)
      hRes = E_ACCESSDENIED;
  }
  return hRes;
}

HRESULT CNktDvProcess::Terminate(__in DWORD dwExitCode)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HANDLE hProc;
  HRESULT hRes;

  if (dwProcessID == 0)
    return E_FAIL;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(dwProcessID, PROCESS_TERMINATE|SYNCHRONIZE);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    if (::TerminateProcess(hProc, dwExitCode) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    ::CloseHandle(hProc);
  }
  return hRes;
}

HRESULT CNktDvProcess::GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem)
{
  if (lplpProcMem == NULL)
    return E_POINTER;
  *lplpProcMem = NULL;
  if (dwProcessID == 0)
    return E_INVALIDARG;
  *lplpProcMem = CNktDvProcessMemory::CreateForPID(dwProcessID);
  //done
  return ((*lplpProcMem) != NULL) ? S_OK : E_OUTOFMEMORY;
}

HRESULT CNktDvProcess::GetModulesEnumerator(__deref_out CNktDvModulesEnumerator **lplpEnum, __in DWORD dwPid)
{
  return cModEnumCache.Get(lplpEnum, dwPid, (SIZE_T)-1);
}

HRESULT CNktDvProcess::GetModulesEnumeratorV(__deref_out CNktDvModulesEnumerator **lplpEnum)
{
  return cModEnumCache.Get(lplpEnum, dwProcessID, nPlatformBits);
}

VOID CNktDvProcess::MarkModulesEnumeratorAsDirty(__in DWORD dwPid, __in HINSTANCE hDll)
{
  cModEnumCache.Remove(dwPid, hDll);
  return;
}

VOID CNktDvProcess::MarkModulesEnumeratorAsDirtyV(__in HINSTANCE hDll)
{
  cModEnumCache.Remove(dwProcessID, hDll);
  return;
}

HRESULT CNktDvProcess::FindModuleByName(__deref_out CNktDvModule **lplpMod, __in_z LPCWSTR szDllNameW)
{
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  HRESULT hRes;

  if (lplpMod != NULL)
    *lplpMod = NULL;
  if (lplpMod == NULL || szDllNameW == NULL)
    return E_POINTER;
  if (szDllNameW[0] == 0 || dwProcessID == 0)
    return E_INVALIDARG;
  //find module
  hRes = GetModulesEnumeratorV(&cModEnum);
  if (FAILED(hRes))
    return hRes;
  //get item
  return cModEnum->GetItemByName(lplpMod, szDllNameW);
}

HRESULT CNktDvProcess::FindModuleByAddress(__deref_out CNktDvModule **lplpMod, __in_opt LPVOID lpAddr,
                                           __in CNktDvModule::eSearchMode nSearchMode)
{
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  HRESULT hRes;

  if (lplpMod == NULL)
    return E_POINTER;
  *lplpMod = NULL;
  if (dwProcessID == 0)
    return E_INVALIDARG;
  //find module
  hRes = GetModulesEnumeratorV(&cModEnum);
  if (FAILED(hRes))
    return hRes;
  //get item
  return cModEnum->GetItemByAddress(lplpMod, lpAddr, nSearchMode);
}

HRESULT CNktDvProcess::FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                                  __in_z LPCWSTR szDllFuncW)
{
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  //find module
  hRes = GetModulesEnumeratorV(&cModEnum);
  //find export
  if (SUCCEEDED(hRes))
    hRes = cModEnum->FindExportedFunctionByName(lplpFunc, szDllFuncW);
  //done
  return hRes;
}

HRESULT CNktDvProcess::FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                     __in_opt LPVOID lpAddr,
                                                     __in CNktDvModule::eSearchMode nSearchMode)
{
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  TNktComPtr<CNktDvModule> cMod;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  //find module
  hRes = GetModulesEnumeratorV(&cModEnum);
  //find export
  if (SUCCEEDED(hRes))
    hRes = cModEnum->FindExportedFunctionByAddress(lplpFunc, lpAddr, nSearchMode);
  //done
  return hRes;
}

HRESULT CNktDvProcess::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvProcess [PID:%04I32u / %s]", GetProcessId(), GetFileName()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvProcess::InitInfo(__in HANDLE hProcess, __in_z_opt LPCWSTR _cExeNameW)
{
  CNktStringW cStrTempW;
  LPWSTR szStartW, sW;
  HRESULT hRes;

  if (hProcess != NULL)
  {
    nPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
    //get process user name
    hRes = CNktDvTools::GetProcessUserName(cStrTempW, hProcess);
    if (SUCCEEDED(hRes))
    {
      hRes = CNktDvTools::SplitUserNameAndDomain(cStrUserNameW, cStrRefDomainW, cStrTempW);
      if (FAILED(hRes))
        return hRes;
    }
    //get logon sid
    hRes = CNktDvTools::GetLogonSid(cStrSidW, hProcess);
    //get executable name
    hRes = CNktDvTools::GetExecutableFileName(cStrPathW, hProcess, NULL);
  }
  else
  {
    nPlatformBits = 0;
  }
  if (cStrPathW.GetLength() == 0 && _cExeNameW != NULL)
  {
    //get executable name from PROCESSENTRY32
    if (cStrPathW.Copy(_cExeNameW) == FALSE)
      return E_OUTOFMEMORY;
  }
  //get the name offset in the path
  szStartW = (LPWSTR)cStrPathW;
  sW = szStartW + cStrPathW.GetLength();
  while (sW > szStartW && *(sW-1) != L'\\')
    sW--;
  nNameOffsetInPathStr = (SIZE_T)(sW - szStartW);
  //done
  return S_OK;
}

HRESULT CNktDvProcess::Create(__deref_out CNktDvProcess **lplpProc, __in_z_opt LPCWSTR szImagePathW,
                              __in BOOL bSuspended, __out_opt LPHANDLE lphContinueEvent)
{
  return CreateWithLogon(lplpProc, szImagePathW, NULL, NULL, bSuspended, lphContinueEvent);
}

HRESULT CNktDvProcess::CreateWithLogon(__deref_out CNktDvProcess **lplpProc, __in_z LPCWSTR szImagePathW,
                                       __in_z LPCWSTR szUserNameW, __in_z LPCWSTR szPasswordW,
                                       __in BOOL bSuspended, __out_opt LPHANDLE lphContinueEvent)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  CNktStringW cStrTempW;
  STARTUPINFOW sSiW;
  PROCESS_INFORMATION sPi;
  BOOL bCloseProcessHandle;
  HANDLE hEvents[2], hReadyEvent, hContinueEvent, hToken;
  LPVOID lpEnvBlock;
  LPWSTR szUserProfileFolderW;
  DWORD dwSize;
  HRESULT hRes;

  if (lplpProc != NULL)
    *lplpProc = NULL;
  if (lphContinueEvent != NULL)
    *lphContinueEvent = NULL;
  if (szImagePathW == NULL)
    return E_POINTER;
  if (szImagePathW[0] == 0)
    return E_INVALIDARG;
  if (bSuspended != FALSE && lphContinueEvent == NULL)
    return E_POINTER;
  nktMemSet(&sSiW, 0, sizeof(sSiW));
  sSiW.cb = sizeof(sSiW);
  //::GetStartupInfoW(&sSiW);
  nktMemSet(&sPi, 0, sizeof(sPi));
  //CreateProcessW() and related requires a read/write parameter (see MSDN)
  if (cStrTempW.Copy(szImagePathW) == FALSE)
    return E_OUTOFMEMORY;
  hRes = S_OK;
  if (szUserNameW == NULL || szUserNameW[0] == 0)
  {
    if (::CreateProcessW(NULL, (LPWSTR)cStrTempW, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE|
                         CREATE_NEW_CONSOLE|CREATE_SUSPENDED, NULL, NULL, &sSiW, &sPi) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  else
  {
    hToken = NULL;
    lpEnvBlock = NULL;
    szUserProfileFolderW = NULL;
    if (szPasswordW == NULL)
      szPasswordW = L"";
    if (::LogonUserW(szUserNameW, NULL, szPasswordW, LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT,
                     &hToken) != FALSE)
    {
      if (::ImpersonateLoggedOnUser(hToken) != FALSE)
      {
        dwSize = 0;
        ::GetUserProfileDirectoryW(hToken, NULL, &dwSize);
        szUserProfileFolderW = (LPWSTR)nktMemMalloc((dwSize+1)*sizeof(WCHAR));
        if (szUserProfileFolderW != NULL)
        {
          if (::GetUserProfileDirectoryW(hToken, szUserProfileFolderW, &dwSize) == FALSE ||
              ::CreateEnvironmentBlock(&lpEnvBlock, hToken, FALSE) == FALSE)
            hRes = NKT_HRESULT_FROM_LASTERROR();
        }
        else
        {
          hRes = E_OUTOFMEMORY;
        }
        ::RevertToSelf();
      }
      else
      {
        hRes = NKT_HRESULT_FROM_LASTERROR();
      }
    }
    else
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    /*
    if (SUCCEEDED(hRes) &&
        ::CreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE, NULL, (LPWSTR)cStrTempW,
                                  CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT|
                                  CREATE_SUSPENDED, lpEnvBlock, szUserProfileFolderW, &sSiW, &sPi) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    */
    if (SUCCEEDED(hRes))
    {
      if (::CreateProcessWithLogonW(szUserNameW, NULL, szPasswordW, LOGON_WITH_PROFILE, NULL, (LPWSTR)cStrTempW,
                                    CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT|
                                    CREATE_SUSPENDED, lpEnvBlock, szUserProfileFolderW, &sSiW, &sPi) == FALSE)
        hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    if (lpEnvBlock != NULL)
      ::DestroyEnvironmentBlock(lpEnvBlock);
    if (szUserProfileFolderW != NULL)
      nktMemFree(szUserProfileFolderW);
    if (hToken != NULL && hToken != INVALID_HANDLE_VALUE)
      ::CloseHandle(hToken);
  }
  bCloseProcessHandle = TRUE;
  if (SUCCEEDED(hRes))
  {
    hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
    if (SUCCEEDED(hRes))
    {
      hRes = cPhMgr->AddHandle(sPi.dwProcessId, sPi.hProcess, PROCESS_ALL_ACCESS);
      if (SUCCEEDED(hRes))
        bCloseProcessHandle = FALSE;
    }
    hRes = CNktDvTools::SuspendAfterCreateProcessW(&hReadyEvent, &hContinueEvent, ::GetCurrentProcess(),
                                                   sPi.hProcess, sPi.hThread);
    if (SUCCEEDED(hRes))
    {
      ::ResumeThread(sPi.hThread);
      //wait until initialization completes (don't worry about return code)
      hEvents[0] = sPi.hThread;
      hEvents[1] = hReadyEvent;
      ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
      ::CloseHandle(hReadyEvent); //not used so close
    }
  }
  if (SUCCEEDED(hRes) && lplpProc != NULL)
  {
    cProc.Attach(NKT_MEMMGR_NEW CNktDvProcess);
    if (cProc == NULL)
    {
      hRes = E_OUTOFMEMORY;
    }
    else
    {
      cProc->dwProcessID = sPi.dwProcessId;
      cProc->dwParentProcessID = ::GetCurrentProcessId();
      cProc->nThreadsCount = 1;
      cProc->nCpuUsage = 0;
      cProc->nPriClassBase = (LONG)::GetPriorityClass(sPi.hProcess);
      hRes = cProc->InitInfo(sPi.hProcess, NULL);
    }
  }
  if (SUCCEEDED(hRes))
  {
    if (bSuspended != FALSE)
    {
      *lphContinueEvent = hContinueEvent;
    }
    else
    {
      ::SetEvent(hContinueEvent);
      ::CloseHandle(hContinueEvent);
    }
    *lplpProc = cProc.Detach();
  }
  else if (bSuspended != FALSE)
  {
    ::TerminateProcess(sPi.hProcess, 0);
    if (cPhMgr != NULL && bCloseProcessHandle == FALSE)
    {
      cPhMgr->RemoveHandle(sPi.dwProcessId);
    }
  }
  if (sPi.hThread != NULL)
    ::CloseHandle(sPi.hThread);
  if (sPi.hProcess != NULL && bCloseProcessHandle != FALSE)
    ::CloseHandle(sPi.hProcess);
  return hRes;
}

HRESULT CNktDvProcess::CreateWithToken(__deref_out CNktDvProcess **lplpProc, __in_z LPCWSTR szImagePathW,
                                       __in_z HANDLE hToken, __in BOOL bSuspended, __out_opt LPHANDLE lphContinueEvent)
{
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  CNktStringW cStrTempW;
  STARTUPINFOW sSiW;
  PROCESS_INFORMATION sPi;
  BOOL bCloseProcessHandle;
  HANDLE hEvents[2], hReadyEvent, hContinueEvent;
  LPVOID lpEnvBlock;
  LPWSTR szUserProfileFolderW;
  lpfnCreateProcessWithTokenW fnCreateProcessWithTokenW;
  lpfnCreateProcessAsUserW fnCreateProcessAsUserW;
  HINSTANCE hDll;
  DWORD dwSize;
  HRESULT hRes;

  if (lplpProc != NULL)
    *lplpProc = NULL;
  if (lphContinueEvent != NULL)
    *lphContinueEvent = NULL;
  if (szImagePathW == NULL)
    return E_POINTER;
  if (szImagePathW[0] == 0 || hToken == NULL || hToken == INVALID_HANDLE_VALUE)
    return E_INVALIDARG;
  if (bSuspended != FALSE && lphContinueEvent == NULL)
    return E_POINTER;
  nktMemSet(&sSiW, 0, sizeof(sSiW));
  sSiW.cb = sizeof(sSiW);
  //::GetStartupInfoW(&sSiW);
  nktMemSet(&sPi, 0, sizeof(sPi));
  //CreateProcessW() and related requires a read/write parameter (see MSDN)
  if (cStrTempW.Copy(szImagePathW) == FALSE)
    return E_OUTOFMEMORY;
  hRes = S_OK;
  lpEnvBlock = NULL;
  szUserProfileFolderW = NULL;
  if (::ImpersonateLoggedOnUser(hToken) != FALSE)
  {
    dwSize = 0;
    ::GetUserProfileDirectoryW(hToken, NULL, &dwSize);
    szUserProfileFolderW = (LPWSTR)nktMemMalloc((dwSize+1)*sizeof(WCHAR));
    if (szUserProfileFolderW != NULL)
    {
      if (::GetUserProfileDirectoryW(hToken, szUserProfileFolderW, &dwSize) == FALSE ||
          ::CreateEnvironmentBlock(&lpEnvBlock, hToken, FALSE) == FALSE)
        hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
    ::RevertToSelf();
  }
  else
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (SUCCEEDED(hRes))
  {
    fnCreateProcessWithTokenW = NULL;
    fnCreateProcessAsUserW = NULL;
    hDll = GetModuleHandleW(L"advapi32.dll"); //already loaded because of 'ImpersonateLoggedOnUser'
    if (hDll != NULL)
    {
      fnCreateProcessWithTokenW = (lpfnCreateProcessWithTokenW)::GetProcAddress(hDll, "CreateProcessWithTokenW");
      fnCreateProcessAsUserW = (lpfnCreateProcessAsUserW)::GetProcAddress(hDll, "CreateProcessAsUserW");
    }
    if (fnCreateProcessWithTokenW != NULL)
    {
      if (fnCreateProcessWithTokenW(hToken, LOGON_WITH_PROFILE, NULL, (LPWSTR)cStrTempW,
                                    CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT|
                                    CREATE_SUSPENDED, lpEnvBlock, szUserProfileFolderW, &sSiW, &sPi) == FALSE)
        hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    else if (fnCreateProcessAsUserW != NULL)
    {
      if (fnCreateProcessAsUserW(hToken, NULL, (LPWSTR)cStrTempW, NULL, NULL, FALSE,
                                 CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT|
                                 CREATE_SUSPENDED, lpEnvBlock, szUserProfileFolderW, &sSiW, &sPi) == FALSE)
        hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    else
    {
      hRes = NKT_DVERR_NotSupported;
    }
  }
  if (lpEnvBlock != NULL)
    ::DestroyEnvironmentBlock(lpEnvBlock);
  if (szUserProfileFolderW != NULL)
    nktMemFree(szUserProfileFolderW);
  bCloseProcessHandle = TRUE;
  if (SUCCEEDED(hRes))
  {
    hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
    if (SUCCEEDED(hRes))
    {
      hRes = cPhMgr->AddHandle(sPi.dwProcessId, sPi.hProcess, PROCESS_ALL_ACCESS);
      if (SUCCEEDED(hRes))
        bCloseProcessHandle = FALSE;
    }
    hRes = CNktDvTools::SuspendAfterCreateProcessW(&hReadyEvent, &hContinueEvent, ::GetCurrentProcess(),
                                                   sPi.hProcess, sPi.hThread);
    if (SUCCEEDED(hRes))
    {
      ::ResumeThread(sPi.hThread);
      //wait until initialization completes (don't worry about return code)
      hEvents[0] = sPi.hThread;
      hEvents[1] = hReadyEvent;
      ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
      ::CloseHandle(hReadyEvent); //not used so close
    }
  }
  if (SUCCEEDED(hRes) && lplpProc != NULL)
  {
    cProc.Attach(NKT_MEMMGR_NEW CNktDvProcess);
    if (cProc == NULL)
    {
      hRes = E_OUTOFMEMORY;
    }
    else
    {
      cProc->dwProcessID = sPi.dwProcessId;
      cProc->dwParentProcessID = ::GetCurrentProcessId();
      cProc->nThreadsCount = 1;
      cProc->nCpuUsage = 0;
      cProc->nPriClassBase = (LONG)::GetPriorityClass(sPi.hProcess);
      hRes = cProc->InitInfo(sPi.hProcess, NULL);
    }
  }
  if (SUCCEEDED(hRes))
  {
    if (bSuspended != FALSE)
    {
      *lphContinueEvent = hContinueEvent;
    }
    else
    {
      ::SetEvent(hContinueEvent);
      ::CloseHandle(hContinueEvent);
    }
    *lplpProc = cProc.Detach();
  }
  else if (bSuspended != FALSE)
  {
    ::TerminateProcess(sPi.hProcess, 0);
    if (cPhMgr != NULL && bCloseProcessHandle == FALSE)
    {
      cPhMgr->RemoveHandle(sPi.dwProcessId);
      bCloseProcessHandle = TRUE;
    }
  }
  if (sPi.hThread != NULL)
    ::CloseHandle(sPi.hThread);
  if (sPi.hProcess != NULL && bCloseProcessHandle != FALSE)
    ::CloseHandle(sPi.hProcess);
  return hRes;
}

HRESULT CNktDvProcess::Terminate(__in DWORD dwPid, __in DWORD dwExitCode)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HANDLE hProc;
  HRESULT hRes;

  if (dwPid == 0)
    return E_FAIL;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(dwPid, PROCESS_TERMINATE|SYNCHRONIZE);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    if (::TerminateProcess(hProc, dwExitCode) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    ::CloseHandle(hProc);
  }
  return hRes;
}

//NOTE: This is not taken from the internal list
HRESULT CNktDvProcess::FindProcessId(__in_z LPCWSTR szName, __out LPDWORD lpdwPid)
{
  HANDLE hSnapshot;
  PROCESSENTRY32W sPe32_W;
  BOOL bFirst;
  HRESULT hRes;

  if (lpdwPid != NULL)
    *lpdwPid = 0;
  if (szName == NULL || lpdwPid == NULL)
    return E_POINTER;
  if (szName[0] == 0)
    return E_INVALIDARG;
  //create process snapshot
  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot == INVALID_HANDLE_VALUE)
    return NKT_HRESULT_FROM_LASTERROR();
  bFirst = TRUE;
  for (;;)
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
      break;
    if (_wcsicmp(sPe32_W.szExeFile, szName) == 0)
    {
      *lpdwPid = sPe32_W.th32ProcessID;
      ::CloseHandle(hSnapshot);
      return S_OK;
    }
    bFirst = FALSE;
  }
  //close snapshot
  ::CloseHandle(hSnapshot);
  return NKT_DVERR_NotFound;
}

VOID CNktDvProcess::FlushCache()
{
  cModEnumCache.Flush();
  return;
}

//-----------------------------------------------------------

static BOOL CALLBACK EWP_FindProcessWindow(__in HWND hWnd, __in LPARAM lParam)
{
  DWORD dwPid;
  LONG_PTR nStyle;

  nStyle = ::GetWindowLongPtr(hWnd, GWL_STYLE);
  ::GetWindowThreadProcessId(hWnd, &dwPid);
  if ((nStyle & WS_CHILD) == 0 &&
      dwPid == ((EWP_FindProcessWindow_Data*)lParam)->dwPid)
  {
    ((EWP_FindProcessWindow_Data*)lParam)->hWnd = hWnd;
    return FALSE;
  }
  return TRUE;
}
