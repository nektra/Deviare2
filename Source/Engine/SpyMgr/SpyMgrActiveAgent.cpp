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
#include "..\..\Common\AgentCommon.h"
#include "..\..\Common\ProcessHandle.h"
#include "..\..\Common\RegistrySettings.h"
#include <Aclapi.h>
#include <Sddl.h>

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define DEFAULT_KERNEL32_CHECK_RETRIES                    10

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED      0x00000001

//#define P_PROC_ACCESS (PROCESS_ALL_ACCESS & ~(PROCESS_SET_SESSIONID|PROCESS_CREATE_PROCESS| \
//                                              PROCESS_SET_QUOTA))
//#define P_PROC_ACCESS PROCESS_ALL_ACCESS
#define P_PROC_ACCESS (PROCESS_CREATE_THREAD|PROCESS_SET_INFORMATION|PROCESS_QUERY_INFORMATION|  \
                       PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ|PROCESS_DUP_HANDLE| \
                       STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE)

#define MY_SIZE_ALIGN(x, size_of_size)                     \
                     NKT_DV_ALIGN((SIZE_T)(x), size_of_size)

#define T_TOKEN_ACCESS (TOKEN_QUERY|TOKEN_READ|TOKEN_QUERY_SOURCE|TOKEN_EXECUTE| \
                        TOKEN_IMPERSONATE|TOKEN_DUPLICATE)

//-----------------------------------------------------------

#define X_BYTE_ENC(x, y) (x)
static
#include "Asm\Injector_x86.inl"
#if defined _M_X64
  static
  #include "Asm\Injector_x64.inl"
#endif //_M_X64

//-----------------------------------------------------------

typedef struct {
  ULONG Size;
  ULONG Unknown1;
  ULONG Unknown2;
  PULONG Unknown3;
  ULONG Unknown4;
  ULONG Unknown5;
  ULONG Unknown6;
  PULONG Unknown7;
  ULONG Unknown8;
} NTCREATETHREADEXBUFFER;

typedef LONG (WINAPI *lpfnNtCreateThreadEx)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess,
                                            LPVOID ObjectAttributes, HANDLE ProcessHandle,
                                            LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
                                            ULONG CreateFlags, SIZE_T ZeroBits, SIZE_T SizeOfStackCommit,
                                            SIZE_T SizeOfStackReserve, LPVOID lpBytesBuffer);
typedef DWORD (WINAPI *lpfnGetSecurityInfo)(__in HANDLE handle, __in  SE_OBJECT_TYPE ObjectType,
                            __in  SECURITY_INFORMATION SecurityInfo, __out_opt PSID *ppsidOwner,
                            __out_opt PSID *ppsidGroup, __out_opt PACL *ppDacl, __out_opt PACL *ppSacl,
                            __out_opt PSECURITY_DESCRIPTOR *ppSecurityDescriptor);

//-----------------------------------------------------------

static HANDLE GetSystemAccountToken();

//-----------------------------------------------------------

SIZE_T CNktDvSpyMgr::FindAgentController(__in DWORD dwPid)
{
  CAgentController **lpRes, **lpItems;

  lpItems = aAgentControllersList.GetBuffer();
  lpRes = (CAgentController**)bsearch_s(&dwPid, lpItems, aAgentControllersList.GetCount(),
                                        sizeof(CAgentController*),
                                        reinterpret_cast<int (__cdecl *)(void*,const void*,const void*)>
                                            (&CNktDvSpyMgr::CAgentController::AgentControllerCompare2),
                                        NULL);
  return (lpRes != NULL) ? (SIZE_T)(lpRes-lpItems) : NKT_SIZE_T_MAX;
}

HRESULT CNktDvSpyMgr::AddAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                         __inout CNktDvProcess *lpProc)
{
  TNktAutoDeletePtr<CAgentController> cAgentCtl;
  SIZE_T nIndex;
  HRESULT hRes;

  NKT_ASSERT(lplpAgentCtl != NULL);
  NKT_ASSERT(lpProc != NULL);
  *lplpAgentCtl = NULL;
  if (lpProc->IsActive(0) != S_OK)
    return E_FAIL;
  //----
  cAgentCtl.Attach(NKT_MEMMGR_NEW CAgentController(lpProc, &cTransport));
  if (cAgentCtl == NULL ||
      aAgentControllersList.SortedInsert(cAgentCtl, &CNktDvSpyMgr::CAgentController::AgentControllerCompare) == FALSE)
    return E_OUTOFMEMORY;
  hRes = cAgentCtl->InitializeAgent(CNktDvRegistrySettings::GetAgentLoadTimeout(), (LPWSTR)cStrAgentPathW,
                                    &sSettingsOverride);
  if (FAILED(hRes))
  {
    nIndex = FindAgentController(lpProc->GetProcessId());
    NKT_ASSERT(nIndex != NKT_SIZE_T_MAX);
    aAgentControllersList.RemoveElementAt(nIndex);
    return hRes;
  }
  *lplpAgentCtl = cAgentCtl.Detach();
  return S_OK;
}

VOID CNktDvSpyMgr::RemoveAgentController(__inout CNktDvSpyMgr::CAgentController *lpAgentCtl)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  TNktComPtr<CNktDvProcess> cProc;

  {
    CNktDvAutoCancellableFastMutex cLock(&GetLockObject(), &cShutdownMtx);
    SIZE_T i, nCount;

    NKT_ASSERT(lpAgentCtl != NULL);
    if (cLock.IsLockHeld() == FALSE || cShutdownMtx.IsShuttingDown() != FALSE ||
        cShutdownMtx.IsSystemActive() == FALSE)
      return;
    nCount = aAgentControllersList.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (aAgentControllersList[i] == lpAgentCtl)
      {
        aAgentControllersList.RemoveElementAt(i);
        break;
      }
    }
    if (i >= nCount)
      lpAgentCtl = NULL; //not found -> may be removed in another thread
  }
  if (lpAgentCtl != NULL)
  {
    cProc = lpAgentCtl->cProc;
    delete lpAgentCtl;
  }
  //rise agent unload event
  if (cProc != NULL)
    OnAgentUnload(cProc);
  return;
}

HRESULT CNktDvSpyMgr::GetAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                         __inout CNktDvProcess *lpProc, __in BOOL bCreateIfNotFound,
                                         __out HRESULT *lphLoadResult)
{
  SIZE_T nIndex;
  HRESULT hRes;

  if (lphLoadResult != NULL)
    *lphLoadResult = S_FALSE;
  nIndex = FindAgentController(lpProc->GetProcessId());
  if (nIndex != NKT_SIZE_T_MAX)
  {
    *lplpAgentCtl = aAgentControllersList[nIndex];
    hRes = S_OK;
  }
  else if (bCreateIfNotFound != FALSE)
  {
    hRes = AddAgentController(lplpAgentCtl, lpProc);
    if (lphLoadResult != NULL)
      *lphLoadResult = hRes;
  }
  else
  {
    hRes = NKT_DVERR_NotFound;
  }
  if (SUCCEEDED(hRes))
    (*lplpAgentCtl)->GetShutdownMtx().Lock();
  return hRes;
}

HRESULT CNktDvSpyMgr::GetAgentController(__deref_out CNktDvSpyMgr::CAgentController **lplpAgentCtl,
                                         __in DWORD dwPid)
{
  SIZE_T nIndex;
  HRESULT hRes;

  nIndex = FindAgentController(dwPid);
  if (nIndex != NKT_SIZE_T_MAX)
  {
    *lplpAgentCtl = aAgentControllersList[nIndex];
    hRes = S_OK;
  }
  else
  {
    hRes = NKT_DVERR_NotFound;
  }
  if (SUCCEEDED(hRes))
    (*lplpAgentCtl)->GetShutdownMtx().Lock();
  return hRes;
}

int CNktDvSpyMgr::CAgentController::AgentControllerCompare(__in void *, __in CAgentController **lpElem1,
                                                           __in CAgentController **lpElem2)
{
  return NKT_DV_CMP<DWORD>((*lpElem1)->cProc->GetProcessId(), (*lpElem2)->cProc->GetProcessId());
}

int CNktDvSpyMgr::CAgentController::AgentControllerCompare2(__in void *, __in LPDWORD lpElem1,
                                                            __in CAgentController **lpElem2)
{
  return NKT_DV_CMP<DWORD>(*lpElem1, (*lpElem2)->cProc->GetProcessId());
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvSpyMgr::CAgentController::CAgentController(__inout CNktDvProcess *lpProc,
                                                 __inout CNktDvTransportEngine *_lpTransport) :
                                                 CNktFastMutex(), CNktMemMgrObj()
{
  NKT_ASSERT(lpProc != NULL);
  NKT_ASSERT(_lpTransport != NULL);
  cProc = lpProc;
  lpTransport = _lpTransport;
  return;
}

CNktDvSpyMgr::CAgentController::~CAgentController()
{
  InternalShutdownAgent(TRUE);
  return;
}

HRESULT CNktDvSpyMgr::CAgentController::InitializeAgent(__in DWORD dwTimeout, __in LPCWSTR szAgentPathw,
                                                        __in LPNKT_DV_AGENTSETTINGOVERRIDE lpSettingsOverride)
{
  CNktAutoFastMutex cLock(this);
  HRESULT hRes;

  NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) AgentController[InitializeAgent]: Pid=%lu", ::GetTickCount(),
                     cProc->GetProcessId()));
  //create active agent object
  hRes = cShutdownMtx.Initialize();
  if (SUCCEEDED(hRes))
  {
    //create memory reader/writer
    cProcMem.Attach(CNktDvProcessMemory::CreateForPID(cProc->GetProcessId()));
    if (cProcMem == NULL)
      hRes = E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    //inject agent
    hRes = LoadAgentIntoProcess(szAgentPathw, lpSettingsOverride);
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) AgentController[LoadAgentIntoProcess]: hRes=%08X",
                       ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    //connect to agent
    hRes = lpTransport->ConnectAgent(cProc->GetProcessId(), dwTimeout);
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) AgentController[ConnectAgent]: hRes=%08X",
                       ::GetTickCount(), hRes));
  }
  if (FAILED(hRes))
  {
    UnloadAgentFromProcess();
  }
  NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) AgentController[InitializeAgent]: hRes=%08X",
                     ::GetTickCount(), hRes));
  return hRes;
}

VOID CNktDvSpyMgr::CAgentController::ShutdownAgent()
{
  InternalShutdownAgent(FALSE);
  return;
}

CNktDvSpyMgr::CAgentController::eState CNktDvSpyMgr::CAgentController::GetAgentState()
{
  CNktAutoFastMutex cLock(this);

  if (cShutdownMtx.GetEvent() == NULL)
    return CNktDvSpyMgr::CAgentController::stInactive;
  return (cShutdownMtx.IsShuttingDown() == FALSE) ? CNktDvSpyMgr::CAgentController::stActive :
                                                    CNktDvSpyMgr::CAgentController::stShuttingDown;
}

HRESULT CNktDvSpyMgr::CAgentController::GetModulesCache(__deref_out CNktDvModulesEnumerator **lplpModEnum)
{
  NKT_ASSERT(lplpModEnum != NULL);
  return cProc->GetModulesEnumeratorV(lplpModEnum);
}

VOID CNktDvSpyMgr::CAgentController::InternalShutdownAgent(__in BOOL bWait)
{
  CNktAutoFastMutex cLock(this);

  //signal shut down
  if (bWait != FALSE)
    cShutdownMtx.StartShutdownAndWaitPending(this);
  else
    cShutdownMtx.SignalShutdown();
  //disconnect agent, unload and free memory reader/writer
  lpTransport->DisconnectAgent(cProc->GetProcessId());
  UnloadAgentFromProcess();
  cProcMem.Release();
  cShutdownMtx.Finalize();
  NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) AgentController[ShutdownAgent]: Done (%s)", ::GetTickCount(),
                    (bWait != FALSE) ? "Wait" : "No-Wait"));
  return;
}

HRESULT CNktDvSpyMgr::CAgentController::LoadAgentIntoProcess(__in LPCWSTR szAgentPathW,
                                                             __in LPNKT_DV_AGENTSETTINGOVERRIDE lpSettingsOverride)
{
  static SID_IDENTIFIER_AUTHORITY aMandatoryLabelAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  LPBYTE lpInjCodeAddr, lpDest;
  SIZE_T k, nToWrite, nTotalInjCodeSize, nInjCodeSize, nAgentPathLen, nAgentNameLen, nSizeOfSize;
  HMODULE hNtDll;
  lpfnNtCreateThreadEx fnNtCreateThreadEx;
  BOOL bIs64BitProcess;
  HANDLE hProc, hProc2, hRemThread, hDestHandle[2], hToken[4];
  DWORD dw, dwTid, dwRes;
  SECURITY_ATTRIBUTES sSecAttrib;
  SECURITY_DESCRIPTOR sSecDesc;
  OBJECT_ATTRIBUTES sObjAttr;
  struct {
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[2];
  } sTokPriv, sTokPrivOrig;
  HRESULT hRes;

  NKT_ASSERT(cProc != NULL);
  NKT_ASSERT(szAgentPathW != NULL && szAgentPathW[0] != 0);
  if (cProc->GetProcessId() == 0 || cProc->GetProcessId() == ::GetCurrentProcessId())
    return E_INVALIDARG; //reject self inject
  if ((::GetVersion() & 0x80000000) != 0)
    return E_NOTIMPL; //reject win9x
  //check if the target process platform is the same than us
  switch (cProc->GetPlatformBits())
  {
    case 32:
#if defined _M_X64
    case 64:
#endif //_M_X64
      break;
    default:
      return E_ACCESSDENIED;
  }
  //get database
  hRes = CNktDvEngDatabase::Get(&cDvDB, NULL, cProc->GetPlatformBits());
  if (FAILED(hRes))
    return hRes;
  lpDest = NULL;
  hDestHandle[0] = hDestHandle[1] = hProc = hProc2 = NULL;
  hRemThread = hToken[0] = hToken[1] = hToken[2] = hToken[3] = NULL;
  nktMemSet(&sTokPriv, 0, sizeof(sTokPriv));
  nktMemSet(&sTokPrivOrig, 0, sizeof(sTokPrivOrig));
  //open target process
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(cProc->GetProcessId(), P_PROC_ACCESS);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[OpenProcess]: hRes=%08X",
                       ::GetTickCount(), hRes));
    return hRes;
  }
  //check if the target process platform is the same than us
  switch (cProc->GetPlatformBits())
  {
    case 32:
      bIs64BitProcess = FALSE;
      lpInjCodeAddr = aInjectorX86;
      nInjCodeSize = NKT_DV_ARRAYLEN(aInjectorX86);
      break;
#if defined _M_X64
    case 64:
      bIs64BitProcess = TRUE;
      lpInjCodeAddr = aInjectorX64;
      nInjCodeSize = NKT_DV_ARRAYLEN(aInjectorX64);
      break;
#endif //_M_X64
    default:
      NKT_ASSERT(FALSE);
  }
  //get agent path length
  nAgentPathLen = wcslen(szAgentPathW);
  nAgentNameLen = (bIs64BitProcess == FALSE) ? 11 : 13;
  //allocate memory for inject code in target process
  nSizeOfSize = (bIs64BitProcess != FALSE) ? sizeof(ULONGLONG) : sizeof(ULONG);
  nTotalInjCodeSize = MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize);  //injected code size
  nTotalInjCodeSize += 3 * nSizeOfSize; //... plus pointer to Dll path + pointer to initdata + server process token
  nTotalInjCodeSize += MY_SIZE_ALIGN(sizeof(NKT_DV_AGENTINITDATA), nSizeOfSize); //... plus agent initialization data
  nTotalInjCodeSize += MY_SIZE_ALIGN((nAgentPathLen+nAgentNameLen+1)*sizeof(WCHAR), nSizeOfSize); //...plus len of dll
  cProcMem.Attach(CNktDvProcessMemory::CreateForPID(cProc->GetProcessId()));
  if (cProcMem == NULL)
  {
    hRes = E_OUTOFMEMORY;
    goto laip_end;
  }
  hRes = cProcMem->AllocMem((LPVOID*)&lpDest, nTotalInjCodeSize, TRUE);
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[AllocMem]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  //create basic security
  ::InitializeSecurityDescriptor(&sSecDesc, SECURITY_DESCRIPTOR_REVISION);
  ::SetSecurityDescriptorDacl(&sSecDesc, TRUE, NULL, FALSE);
  sSecAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  sSecAttrib.bInheritHandle = TRUE;
  sSecAttrib.lpSecurityDescriptor = &sSecDesc;
  /*
  if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ALL_ACCESS_P, &hToken[0]) == FALSE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[OpenProcessToken]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  if (::LookupPrivilegeValueW(NULL, L"SeImpersonatePrivilege",
                              &(sTokPriv.Privileges[sTokPriv.PrivilegeCount].Luid)) != FALSE)
  {
    sTokPriv.Privileges[sTokPriv.PrivilegeCount].Attributes = SE_PRIVILEGE_ENABLED;
    sTokPriv.PrivilegeCount++;
  }
  /*
  if (::LookupPrivilegeValueW(NULL, L"SeAssignPrimaryTokenPrivilege",
                              &(sTokPriv.Privileges[sTokPriv.PrivilegeCount].Luid)) != FALSE)
  {
    sTokPriv.Privileges[sTokPriv.PrivilegeCount].Attributes = SE_PRIVILEGE_ENABLED;
    sTokPriv.PrivilegeCount++;
  }
  * /
  if (sTokPriv.PrivilegeCount > 0)
  {
    DWORD dw = (DWORD)sizeof(sTokPriv);
    ::AdjustTokenPrivileges(hToken[0], 0, (TOKEN_PRIVILEGES*)&sTokPriv, (DWORD)sizeof(sTokPriv),
                            (TOKEN_PRIVILEGES*)&sTokPrivOrig, &dw);
  }
  BYTE aSidBuf[FIELD_OFFSET(SID, SubAuthority) + sizeof(ULONG)];
  TOKEN_MANDATORY_LABEL sTokenIntegLabel;

  ::InitializeSid((PSID)aSidBuf, &aMandatoryLabelAuthority, 1);
  ((PISID)aSidBuf)->SubAuthority[0] = MANDATORY_LEVEL_TO_MANDATORY_RID(MandatoryLevelMedium);
  sTokenIntegLabel.Label.Sid = (PSID)aSidBuf;
  sTokenIntegLabel.Label.Attributes = SE_GROUP_INTEGRITY;
  if (::DuplicateTokenEx(hToken[0], MAXIMUM_ALLOWED, &sSecAttrib, SecurityImpersonation, TokenImpersonation,
                         &hToken[1]) == FALSE ||
      ::CreateRestrictedToken(hToken[1], 0, 0, NULL, 0, NULL, 0, NULL, &hToken[2]) == FALSE ||
      ::SetTokenInformation(hToken[2], TokenIntegrityLevel, &sTokenIntegLabel, (DWORD)sizeof(sTokenIntegLabel)) == FALSE ||
      ::DuplicateHandle(::GetCurrentProcess(), hToken[2], hProc, &hToken[3], 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[DuplicateToken]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  */

  //NOTE: In order to Impersonation to succeed, the target process must contain "SeImpersonatePrivilege". If it hasn't
  //      the SetThreadToken will succeed but effective token will be only "Indentification" and LoadLibrary will fail.
  hToken[2] = NULL;
  //hToken[2] = GetSystemAccountToken();
  if (hToken[2] != NULL)
  {
    if (::LookupPrivilegeValueW(NULL, L"SeImpersonatePrivilege",
                                &(sTokPriv.Privileges[sTokPriv.PrivilegeCount].Luid)) != FALSE)
    {
      sTokPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
      sTokPriv.PrivilegeCount = 1;
      hRes = (::AdjustTokenPrivileges(hToken[2], 0, (TOKEN_PRIVILEGES*)&sTokPriv, (DWORD)sizeof(sTokPriv),
                                      NULL, NULL) != FALSE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
      NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[AdjustTokenPrivileges]: hRes=%08X",
                                          ::GetTickCount(), hRes));
    }
    if (::DuplicateHandle(::GetCurrentProcess(), hToken[2], hProc, &hToken[3], 0, FALSE,
                          DUPLICATE_SAME_ACCESS) == FALSE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[DuplicateToken]: hRes=%08X",
                                          ::GetTickCount(), hRes));
      goto laip_end;
    }
  }
  //duplicate handles to be used by the agent for event duplication and waitXXX functions
  hProc2 = CNktDvProcessHandlesMgr::CreateHandle(::GetCurrentProcessId(), SYNCHRONIZE|PROCESS_DUP_HANDLE);
  hRes = (hProc2 != NULL) ? S_OK : E_ACCESSDENIED;
  if (SUCCEEDED(hRes))
  {
    if (::DuplicateHandle(::GetCurrentProcess(), hProc2, hProc, &hDestHandle[0], 0, FALSE,
                          DUPLICATE_SAME_ACCESS) == FALSE ||
        ::DuplicateHandle(::GetCurrentProcess(), cDvDB->GetDbFileMapping(), hProc, &hDestHandle[1],
                          0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[DuplicateHandle]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  //write code
  hRes = cProcMem->Write(lpDest, lpInjCodeAddr, nInjCodeSize);
  if (SUCCEEDED(hRes))
  {
    //write addr of "DllNameW" string
    k = MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize);
    nToWrite = (SIZE_T)lpDest + MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize) + 3*nSizeOfSize +
               MY_SIZE_ALIGN(sizeof(NKT_DV_AGENTINITDATA), nSizeOfSize);
    if (bIs64BitProcess == FALSE)
      hRes = cProcMem->WriteDWord(lpDest+k, (DWORD)nToWrite);
    else
      hRes = cProcMem->WriteQWord(lpDest+k, (ULONGLONG)nToWrite);
  }
  if (SUCCEEDED(hRes))
  {
    //write addr of "InitData"
    nToWrite = (SIZE_T)lpDest + MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize) + 3*nSizeOfSize;
    if (bIs64BitProcess == FALSE)
      hRes = cProcMem->WriteDWord(lpDest+k+nSizeOfSize, (DWORD)nToWrite);
    else
      hRes = cProcMem->WriteQWord(lpDest+k+nSizeOfSize, (ULONGLONG)nToWrite);
  }
  if (SUCCEEDED(hRes))
  {
    //write server token
    if (bIs64BitProcess == FALSE)
      hRes = cProcMem->WriteDWord(lpDest+k+2*nSizeOfSize, (DWORD)((ULONG_PTR)hToken[3]));
    else
      hRes = cProcMem->WriteQWord(lpDest+k+2*nSizeOfSize, (ULONGLONG)hToken[3]);
  }
  if (SUCCEEDED(hRes))
  {
    //write "DllNameW" string
    k = MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize) + 3*nSizeOfSize +
        MY_SIZE_ALIGN(sizeof(NKT_DV_AGENTINITDATA), nSizeOfSize);
    hRes = cProcMem->WriteStringW(lpDest+k, szAgentPathW, nAgentPathLen, FALSE);
    if (SUCCEEDED(hRes))
    {
      if (bIs64BitProcess == FALSE)
        hRes = cProcMem->WriteStringW(lpDest+k+nAgentPathLen*sizeof(WCHAR), L"DvAgent.DLL");
      else
        hRes = cProcMem->WriteStringW(lpDest+k+nAgentPathLen*sizeof(WCHAR), L"DvAgent64.DLL");
    }
  }
  if (SUCCEEDED(hRes))
  {
    NKT_DV_AGENTINITDATA sInitData;

    //write NKT_DV_AGENTINITDATA
    sInitData.hServerProcDupHandle = NKT_PTR_2_ULONGLONG(hDestHandle[0]);
    sInitData.hDbFileMapDupHandle = NKT_PTR_2_ULONGLONG(hDestHandle[1]);
    sInitData.nServerPlatformBits = sizeof(SIZE_T) << 3;
    sInitData.dwServerProcessId = ::GetCurrentProcessId();
    sInitData.nProtocolVersion = NKT_DV_TRANSPORT_PROTOCOL_VERSION;
    sInitData.hServerTokenDupHandle = NKT_PTR_2_ULONGLONG(hToken[3]);
    sInitData.nFlags = 0;
    nktMemCopy(&(sInitData.sSettingsOverride), lpSettingsOverride, sizeof(sInitData.sSettingsOverride));
    k = MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize) + 3*nSizeOfSize;
    hRes = cProcMem->Write(lpDest+k, &sInitData, sizeof(sInitData));
  }
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[Write]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  //flush cache and change page protection
  ::FlushInstructionCache(hProc, lpInjCodeAddr, nTotalInjCodeSize);
  dw = 0;
  ::VirtualProtectEx(hProc, lpInjCodeAddr, nTotalInjCodeSize, PAGE_EXECUTE_READ, &dw);
  //execute remote code
  hNtDll = ::GetModuleHandleW(L"ntdll.dll");
  fnNtCreateThreadEx = (hNtDll != NULL) ? (lpfnNtCreateThreadEx)::GetProcAddress(hNtDll, "NtCreateThreadEx") : NULL;
  if (fnNtCreateThreadEx == NULL)
  {
    hRemThread = ::CreateRemoteThread(hProc, &sSecAttrib, 0, (LPTHREAD_START_ROUTINE)lpDest,
                                      lpDest+MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize), CREATE_SUSPENDED, &dwTid);
    hRes = (hRemThread != NULL) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  }
  else
  {
    InitializeObjectAttributes(&sObjAttr, NULL, (sSecAttrib.bInheritHandle != FALSE) ? OBJ_INHERIT : 0,
                               NULL, &sSecDesc);
    //InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
    hRes = HRESULT_FROM_NT(fnNtCreateThreadEx(&hRemThread, 0x001FFFFFUL, &sObjAttr, hProc,
                                              (LPTHREAD_START_ROUTINE)lpDest,
                                              lpDest+MY_SIZE_ALIGN(nInjCodeSize, nSizeOfSize),
                                              THREAD_CREATE_FLAGS_CREATE_SUSPENDED, 0, NULL, NULL, NULL));
    if (FAILED(hRes))
      hRemThread = NULL;
  }
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[CreateRemoteThread]: hRes=%08X",
                       ::GetTickCount(), hRes));
    goto laip_end;
  }
  //run remote loader
  ::ResumeThread(hRemThread);
  dwRes = ::WaitForSingleObject(hRemThread, INFINITE);
  if (dwRes != WAIT_OBJECT_0)
  {
    //don't free memory if wait failed to avoid possible hang in target process
    lpDest = NULL;
    hRes = E_FAIL;
    NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[WaitForSingleObject]: hRes=%08X, "
                                         "dwRes=%08X",
                       ::GetTickCount(), hRes, dwRes));
    goto laip_end;
  }
  ::GetExitCodeThread(hRemThread, &dwRes);
  hRes = (HRESULT)dwRes;
  NKT_DEBUGPRINTLNA(Nektra::dlEngine, ("%lu) LoadAgentIntoProcess[Injection]: hRes=%08X",
                     ::GetTickCount(), hRes));
  if (SUCCEEDED(hRes))
  {
    //on success, don't close duplicated handles on target process
    hToken[3] = NULL;
    hDestHandle[0] = hDestHandle[1] = NULL;
  }
laip_end:
  if (sTokPrivOrig.PrivilegeCount > 0)
    ::AdjustTokenPrivileges(hToken[0], 0, (TOKEN_PRIVILEGES*)&sTokPrivOrig, (DWORD)sizeof(sTokPrivOrig), NULL, NULL);
  if (hDestHandle[1] != NULL && hDestHandle[1] != INVALID_HANDLE_VALUE)
    ::DuplicateHandle(hProc, hDestHandle[1], hProc, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
  if (hDestHandle[0] != NULL && hDestHandle[0] != INVALID_HANDLE_VALUE)
    ::DuplicateHandle(hProc, hDestHandle[0], hProc, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
  if (hToken[3] != NULL && hToken[3] != INVALID_HANDLE_VALUE)
    ::DuplicateHandle(hProc, hToken[3], hProc, NULL, 0, FALSE, DUPLICATE_CLOSE_SOURCE);
  for (k=0; k<3; k++)
  {
    if (hToken[k] != NULL && hToken[k] != INVALID_HANDLE_VALUE)
      ::CloseHandle(hToken[k]);
  }
  if (hProc2 != NULL)
    ::CloseHandle(hProc2);
  if (hProc != NULL)
    ::CloseHandle(hProc);
  if (hRemThread != NULL)
    ::CloseHandle(hRemThread);
  if (lpDest != NULL)
    cProcMem->FreeMem(lpDest);
  return hRes;
}

HRESULT CNktDvSpyMgr::CAgentController::UnloadAgentFromProcess()
{
  CNktEvent cShutdownEvent;
  WCHAR szEventNameW[128];

  //create shutdown event
  swprintf_s(szEventNameW, NKT_DV_ARRAYLEN(szEventNameW), L"DeviareAgentShutdown_%08X",
             cProc->GetProcessId());
  if (cShutdownEvent.Open(szEventNameW) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  cShutdownEvent.Set();
  cShutdownEvent.Destroy();
  return S_OK;
}

//-----------------------------------------------------------

static DWORD GetWinlogonProcessId()
{
  HANDLE hSnapshot;
  PROCESSENTRY32W sPe32_W;
  BOOL b, bIsVista;
  OSVERSIONINFOW sOvi;
  CNktStringW cStrTempW;

  nktMemSet(&sOvi, 0, sizeof(sOvi));
  sOvi.dwOSVersionInfoSize = sizeof(sOvi);
  bIsVista = (::GetVersionEx(&sOvi) != FALSE && sOvi.dwPlatformId == VER_PLATFORM_WIN32_NT &&
              sOvi.dwMajorVersion >= 6) ? TRUE : FALSE;
  //create process snapshot
  hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnapshot != INVALID_HANDLE_VALUE)
  {
    nktMemSet(&sPe32_W, 0, sizeof(sPe32_W));
    sPe32_W.dwSize = (DWORD)sizeof(sPe32_W);
    b = ::Process32FirstW(hSnapshot, &sPe32_W);
    while (b != FALSE)
    {
      if (_wcsicmp(sPe32_W.szExeFile, L"winlogon.exe") == 0)
      {
        if (bIsVista != FALSE)
        {
          if (CNktDvTools::GetProcessIntegrityLevel(sPe32_W.th32ProcessID) == MandatoryLevelSystem)
          {
            ::CloseHandle(hSnapshot);
            return sPe32_W.th32ProcessID;
          }
        }
        else
        {
          if (SUCCEEDED(CNktDvTools::GetProcessUserName(cStrTempW, sPe32_W.th32ProcessID)) &&
              _wcsicmp((LPWSTR)cStrTempW, L"SYSTEM") == 0)
          {
            ::CloseHandle(hSnapshot);
            return sPe32_W.th32ProcessID;
          }
        }
      }
      nktMemSet(&sPe32_W, 0, sizeof(sPe32_W));
      sPe32_W.dwSize = (DWORD)sizeof(sPe32_W);
      b = ::Process32NextW(hSnapshot, &sPe32_W);
    }
    ::CloseHandle(hSnapshot);
  }
  return 0;
}

static HANDLE GetSystemAccountToken()
{
  DWORD dwPid;
  HANDLE hProc, hToken, hImpToken;

  hImpToken = NULL;
  dwPid = GetWinlogonProcessId();
  if (dwPid != 0)
  {
    hProc = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwPid);
    if (hProc != NULL)
    {
      if (::OpenProcessToken(hProc, MAXIMUM_ALLOWED, &hToken) != FALSE)
      {
        ::DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenImpersonation, &hImpToken);
        ::CloseHandle(hToken);
      }
      ::CloseHandle(hProc);
    }
  }
  return hImpToken;
}
