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

#include "Agent.h"
#include "AgentMgr.h"
#include "..\..\Common\ModulesEnum.h"
#include "..\..\Common\Tools.h"
#include "..\..\Common\NtInternals.h"
#include "..\..\Common\AutoPtr.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#define INTERNAL_HOOKID_CreateProcessInternalW    0x80000001
#define INTERNAL_HOOKID_LdrLoadDll                0x80000002
#define INTERNAL_HOOKID_LdrUnloadDll              0x80000003
#define INTERNAL_HOOKID_NtSuspendThread           0x80000004
#define INTERNAL_HOOKID_NtResumeThread            0x80000005
#define INTERNAL_HOOKID_NtAlertResumeThread       0x80000006
#define INTERNAL_HOOKID_NtMapViewOfSection        0x80000007
#define INTERNAL_HOOKID_NtUnmapViewOfSection      0x80000008
#define INTERNAL_HOOKID_NtProtectVirtualMemory    0x80000009

//-----------------------------------------------------------

typedef struct {
  LPDWORD lpdwCreationFlags;
  LPPROCESS_INFORMATION lpProcessInformation;
  BOOL bWasSuspended;
} USERDATA_OnInternalCreateProcess;

typedef struct {
  HINSTANCE *lphDll;
  BOOL bIsMsCoreeDll;
} USERDATA_OnInternalLdrLoadDll;

typedef struct {
  HANDLE hThread;
} USERDATA_OnInternalNtSuspendThread;

typedef struct {
  HANDLE hThread;
} USERDATA_OnInternalNtResumeThread;

typedef struct {
  PVOID *pBaseAddress;
} USERDATA_OnInternalNtMapViewOfSection;

typedef struct {
  PVOID BaseAddress;
} USERDATA_OnInternalNtUnmapViewOfSection;

//-----------------------------------------------------------

//NOTE: The original TLS code of this class was commented and rolled back to the usage of a
//      global variable because, for e.g., if a custom hook plugin is loaded, a deadlock may
//      occur because some .net dll's are loaded in another threads.
//      Using a global var can lead into problems if two dll's are loaded at the same time but
//      it is less probable.
class CDvAgentMgrAutoSendDllNotificationAsync
{
public:
  CDvAgentMgrAutoSendDllNotificationAsync(__in LONG volatile *_lpnSendLoadedUnloadedDllAsync)
    {
    lpnSendLoadedUnloadedDllAsync = _lpnSendLoadedUnloadedDllAsync;
    NktInterlockedIncrement(lpnSendLoadedUnloadedDllAsync);
    return;
    };

  virtual ~CDvAgentMgrAutoSendDllNotificationAsync()
    {
    NktInterlockedDecrement(lpnSendLoadedUnloadedDllAsync);
    return;
    };

  static BOOL DoAsyncNotify(LONG volatile *lpnSendLoadedUnloadedDllAsync)
    {
    return ((*lpnSendLoadedUnloadedDllAsync) > 0) ? TRUE : FALSE;
    };

private:
  LONG volatile *lpnSendLoadedUnloadedDllAsync;
};

/*
class CDvAgentMgrAutoTlsSendDllNotificationAsync
{
public:
  CDvAgentMgrAutoTlsSendDllNotificationAsync()
    {
    if (SUCCEEDED(nktDvTlsData_Get(&lpTlsData)))
    {
      bAlreadyLocked = lpTlsData->bSendLoadedUnloadedDllAsync;
      lpTlsData->bSendLoadedUnloadedDllAsync = TRUE;
    }
    return;
    };

  virtual ~CDvAgentMgrAutoTlsSendDllNotificationAsync()
    {
    if (lpTlsData != NULL)
    {
      if (bAlreadyLocked == FALSE)
        lpTlsData->bSendLoadedUnloadedDllAsync = FALSE;
      lpTlsData->Release();
    }
    return;
    };

  static BOOL DoAsyncNotify()
    {
    CNktDvTlsData *lpTlsData;
    BOOL b = FALSE;

    if (SUCCEEDED(nktDvTlsData_Get(&lpTlsData)))
    {
      b = lpTlsData->bSendLoadedUnloadedDllAsync;
      lpTlsData->Release();
    }
    return b;
    };

private:
  CNktDvTlsData *lpTlsData;
  BOOL bAlreadyLocked;
};
*/

//-----------------------------------------------------------

static HRESULT InternalCheckDllModificationsWithImageFile(__in HINSTANCE hDll, __in HANDLE hFile,
                                                          __inout TNktArrayList<BYTE, 16384> &cBuf);
static int ThreadIgnore_Compare(__in void *, __in LPDWORD lpElem1, __in LPDWORD lpElem2);
static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount);
static BOOL SecureMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount);
static BOOL SecureMemIsDiff(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount);
static BOOL OnLdrLoadDll_IsDll(__in PUNICODE_STRING lpDllNameUS, __in LPCWSTR szDllToCheckW);

//-----------------------------------------------------------

#pragma warning(disable : 4355)
CDvAgentMgr::CDvAgentMgr() : CNktFastMutex(), CNktMemMgrObj(), CNktDvTransportAgentCallback(),
                             CNktDvHookEngineCallback(), cHookEngine(this)
#pragma warning(default : 4355)
{
  hAgentInst = NULL;
  dwPID = ::GetCurrentProcessId();
  NktInterlockedExchange(&nTransportActive, 0);
  NktInterlockedExchange(&nLoadedDllsNextInternalId, 0);
  NktInterlockedExchange(&nSendLoadedUnloadedDllAsync, 0);
  dwAgentInitFlags = 0;
  hNtDll = hKernel32Dll = hKernelBaseDll = NULL;
  dwRunThread = 0;
  hShutdownEvent = hServerProcDup = hMainThread = NULL;
  nServerPlatformBits = 0;
  NktInterlockedExchange(&nDelayedCheckOverrides, 0);
  return;
}

CDvAgentMgr::~CDvAgentMgr()
{
  if (bAppIsExiting == FALSE)
    Finalize();
  return;
}

HRESULT CDvAgentMgr::Initialize(__in LPNKT_DV_AGENTINITDATA lpInitData, __in HANDLE _hShutdownEvent,
                                __in HANDLE _hMainThread)
{
  CNktAutoFastMutex cLock(this);
  CNktDvHookEngine::HOOKINFO sHookInfo[9];
  HRESULT hRes;

  if (hShutdownEvent != NULL)
    return NKT_DVERR_OnlyOneInstance;
  hShutdownEvent = _hShutdownEvent;
  hServerProcDup = (HANDLE)(lpInitData->hServerProcDupHandle);
  hMainThread = _hMainThread;
  nServerPlatformBits = lpInitData->nServerPlatformBits;
  dwAgentInitFlags = lpInitData->nFlags;
  //allocate tls slot
  hRes = nktDvTlsData_Initialize();
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[TlsData_Initialize]: hRes=%08X", ::GetTickCount(),
                   hRes));
  if (SUCCEEDED(hRes))
  {
    //shutdown locker
    hRes = cShutdownMtx.Initialize();
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[ShutdownMtx]: hRes=%08X", ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    //load database
    cDvDB.Release();
    hRes = CNktDvEngDatabase::Get(&cDvDB, (LPWSTR)(lpInitData->hDbFileMapDupHandle), sizeof(SIZE_T)<<3);
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[Database]: hRes=%08X", ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    //init process handle manager
    cProcHndlMgr.Release();
    hRes = CNktDvProcessHandlesMgr::Get(&cProcHndlMgr);
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[ProcHndlMgr]: hRes=%08X", ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    //initialize transport
    hRes = cTransport.Initialize(lpInitData->dwServerProcessId, hServerProcDup,
                                 NKT_DV_TRANSPORTAGENT_THREADS_PER_CPU*CNktDvTools::GetProcessorsCount(),
                                 this);
    NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) AgentMgr[Transport]: hRes=%08X",
                       ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    //initialize support dll's
    hNtDll = ::LoadLibraryW(L"ntdll.dll");
    if (hNtDll == NULL)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    hKernel32Dll = ::LoadLibraryW(L"kernel32.dll");
    if (hKernel32Dll == NULL)
      hRes = NKT_HRESULT_FROM_LASTERROR();
    hKernelBaseDll = ::LoadLibraryW(L"kernelbase.dll"); //this dll is optional
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[SupportDll]: hRes=%08X", ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
    CLoadedModule *lpMod;

    while ((lpMod = sLoadedModules.cList.PopTail()) != NULL)
      delete lpMod;
  }
  if (SUCCEEDED(hRes))
  {
    SIZE_T i;

    nktMemSet(sHookInfo, 0, sizeof(sHookInfo));
    //initialize hook engine
    hRes = cHookEngine.Initialize();
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[HookEng]: hRes=%08X", ::GetTickCount(), hRes));
    if (SUCCEEDED(hRes))
    {
      sHookInfo[0].dwHookId = INTERNAL_HOOKID_CreateProcessInternalW;
      if (hKernelBaseDll != NULL)
        sHookInfo[0].lpProcToHook = ::GetProcAddress(hKernelBaseDll, "CreateProcessInternalW");
      if (sHookInfo[0].lpProcToHook == NULL)
        sHookInfo[0].lpProcToHook = ::GetProcAddress(hKernel32Dll, "CreateProcessInternalW");
      //----
      sHookInfo[1].dwHookId = INTERNAL_HOOKID_LdrLoadDll;
      sHookInfo[1].lpProcToHook = ::GetProcAddress(hNtDll, "LdrLoadDll");
      //----
      sHookInfo[2].dwHookId = INTERNAL_HOOKID_LdrUnloadDll;
      sHookInfo[2].lpProcToHook = ::GetProcAddress(hNtDll, "LdrUnloadDll");
      //----
      sHookInfo[3].dwHookId = INTERNAL_HOOKID_NtSuspendThread;
      sHookInfo[3].lpProcToHook = ::GetProcAddress(hNtDll, "NtSuspendThread");
      //----
      sHookInfo[4].dwHookId = INTERNAL_HOOKID_NtResumeThread;
      sHookInfo[4].lpProcToHook = ::GetProcAddress(hNtDll, "NtResumeThread");
      //----
      sHookInfo[5].dwHookId = INTERNAL_HOOKID_NtAlertResumeThread;
      sHookInfo[5].lpProcToHook = ::GetProcAddress(hNtDll, "NtAlertResumeThread");
      //----
      sHookInfo[6].dwHookId = INTERNAL_HOOKID_NtMapViewOfSection;
      sHookInfo[6].lpProcToHook = ::GetProcAddress(hNtDll, "NtMapViewOfSection");
      //----
      sHookInfo[7].dwHookId = INTERNAL_HOOKID_NtUnmapViewOfSection;
      sHookInfo[7].lpProcToHook = ::GetProcAddress(hNtDll, "NtUnmapViewOfSection");
      //----
      sHookInfo[8].dwHookId = INTERNAL_HOOKID_NtProtectVirtualMemory;
      sHookInfo[8].lpProcToHook = ::GetProcAddress(hNtDll, "NtProtectVirtualMemory");
      //----
      if (sHookInfo[0].lpProcToHook != NULL && sHookInfo[1].lpProcToHook != NULL &&
          sHookInfo[2].lpProcToHook != NULL && sHookInfo[3].lpProcToHook != NULL &&
          sHookInfo[3].lpProcToHook != NULL && sHookInfo[5].lpProcToHook != NULL &&
          sHookInfo[6].lpProcToHook != NULL && sHookInfo[7].lpProcToHook != NULL &&
          sHookInfo[8].lpProcToHook != NULL)
      {
        for (i=0; i<NKT_DV_ARRAYLEN(sHookInfo); i++)
          sHookInfo[i].nFlags |= NKT_DV_TMSG_ADDHOOK_FLAG_DisableStackWalk;
        hRes = cHookEngine.Hook(sHookInfo, NKT_DV_ARRAYLEN(sHookInfo), TRUE);
      }
      else
      {
        hRes = NKT_DVERR_NotFound;
      }
    }
    if (SUCCEEDED(hRes))
    {
      //ignore errors on non-hooked items
      for (i=0; i<NKT_DV_ARRAYLEN(sHookInfo); i++)
      {
        if (sHookInfo[i].lpProcToHook != NULL)
          cHookEngine.EnableHook(sHookInfo[i].dwHookId, TRUE);
      }
    }
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMgr[Hooks]: hRes=%08X", ::GetTickCount(), hRes));
  }
  if (FAILED(hRes))
    Finalize();
  return hRes;
}

VOID CDvAgentMgr::Finalize()
{
  CNktAutoFastMutex cLock(this);
  TNktLnkLst<CCreatedProcessesItem>::Iterator it;
  CCreatedProcessesItem *lpCpItem;
  CBatchHookExecData *lpExecData;

  cShutdownMtx.StartShutdownAndWaitPending(this);
  //unhook all
  cHookEngine.Finalize();
  //end transport
  cTransport.Finalize();
  hShutdownEvent = hServerProcDup = hMainThread = NULL;
  nServerPlatformBits = 0;
  dwAgentInitFlags = 0;
  //remove support dll's
  if (hKernelBaseDll != NULL)
  {
    ::FreeLibrary(hKernelBaseDll);
    hKernelBaseDll = NULL;
  }
  if (hKernel32Dll != NULL)
  {
    ::FreeLibrary(hKernel32Dll);
    hKernel32Dll = NULL;
  }
  if (hNtDll != NULL)
  {
    ::FreeLibrary(hNtDll);
    hNtDll = NULL;
  }
  //unload dll's
  {
    CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
    //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

    aLoadedUserDllsList.RemoveAllElements();
  }
  {
    CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
    CLoadedModule *lpMod;

    while ((lpMod = sLoadedModules.cList.PopTail()) != NULL)
      delete lpMod;
  }
  //continue execution of all suspended child processes
  for (lpCpItem=it.Begin(cCreatedProcessesList); lpCpItem!=NULL; lpCpItem=it.Next())
  {
    cCreatedProcessesList.Remove(lpCpItem);
    delete lpCpItem;
  }
  //free not-used batch hook exec
  while (1)
  {
    {
      CNktAutoFastMutex cBatchHookExecListLock(&(sBatchHookExec.cMtx));

      lpExecData = sBatchHookExec.cList.PopHead();
    }
    if (lpExecData == NULL)
      break;
    delete lpExecData;
  }
  //clean more stuff
  sThreadIgnore.cList.RemoveAllElements();
  cDvDB.Release();
  cProcHndlMgr.Release();
  cShutdownMtx.Finalize();
  CNktDvModule::FlushCache();
  CNktDvProcess::FlushCache();
  nktDvTlsData_Finalize();
  return;
}

HRESULT CDvAgentMgr::StartServer()
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  if (cShutdownMtx.IsShuttingDown() != FALSE)
    return NKT_DVERR_Cancelled;
  return cTransport.StartServer();
}

VOID CDvAgentMgr::Run()
{
  HANDLE hEvents[2];

  NktInterlockedExchange((LONG volatile*)&dwRunThread, (LONG)::GetCurrentThreadId());
  ::ResetEvent(hShutdownEvent);
  hEvents[0] = hServerProcDup;
  hEvents[1] = hShutdownEvent;
  //wait until shutdown event is signaled or server process is terminated
  ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
  //end
  NktInterlockedExchange((LONG volatile*)&dwRunThread, 0);
  return;
}

BOOL CDvAgentMgr::CheckIfInTrampoline(__in SIZE_T nCurrIP)
{
  return cHookEngine.CheckIfInTrampoline(nCurrIP);
}

VOID CDvAgentMgr::TAC_OnConnectionEstablished(__inout CNktDvTransportAgent *lpTransport)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  TNktArrayList<HMODULE> aModulesList;
  CNktDvTools::MODULE_INFO sModInfo;
  DWORD dwProcessId;
  HANDLE hProcess;
  TNktArrayList<BYTE, 4096> aDllInfoBuf;
  TNktArrayList<BYTE, 16384> aModifiedImageBuf;
  SIZE_T i, nCount;
  //, hWaitEvents[4]
  //NKT_DV_TMSG_INITIALCONNECTIONMESSAGE sMsg;
  CNktEvent cProcessedEvent;
  HRESULT hRes;

  NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) Agent [Connection established]", ::GetTickCount()));
  dwProcessId = ::GetCurrentProcessId();
  hProcess = ::GetCurrentProcess();
  //create process snapshot
  hRes = (aDllInfoBuf.SetCount(sizeof(ULONG)) != FALSE) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
  {
    *((ULONG NKT_UNALIGNED *)aDllInfoBuf.GetBuffer()) = 0;
    //----
    hRes = CNktDvTools::GetModules(hProcess, sizeof(SIZE_T)<<3, aModulesList);
  }
  if (SUCCEEDED(hRes))
  {
    nCount = aModulesList.GetCount();
    for (i=0; i<nCount && SUCCEEDED(hRes); i++)
    {
      hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProcess, aModulesList[i], sModInfo);
      if (hRes == E_FAIL)
      {
        hRes = S_OK;
        continue;
      }
      if (SUCCEEDED(hRes))
      {
        CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
        TNktLnkLst<CLoadedModule>::Iterator it;
        TNktAutoDeletePtr<CLoadedModule> cMod;
        CLoadedModule *lpMod;

        for (lpMod=it.Begin(sLoadedModules.cList); SUCCEEDED(hRes) && lpMod != NULL; lpMod=it.Next())
        {
          if (lpMod->hInst == aModulesList[i] && lpMod->bUnloading == FALSE)
            break; //only inform first load
        }
        if (lpMod == NULL)
        {
          cMod.Attach(NKT_MEMMGR_NEW CLoadedModule);
          if (cMod != NULL)
          {
            cMod->hInst = aModulesList[i];
            cMod->bLoaderWasLocked = FALSE;
            cMod->bUnloading = FALSE;
            //get module name
            hRes = CNktDvTools::GetExecutableFileName(cMod->cStrNameW, ::GetCurrentProcess(), cMod->hInst);
            if (SUCCEEDED(hRes))
            {
              //add to buffer
              LPBYTE d;
              SIZE_T nOfs, nNameLen;

              nNameLen = cMod->cStrNameW.GetLength() * sizeof(WCHAR);
              nOfs = aDllInfoBuf.GetCount();
              if (aDllInfoBuf.SetCount(nOfs+sizeof(ULONGLONG)+sizeof(ULONG)+nNameLen) != FALSE)
              {
                d = aDllInfoBuf.GetBuffer();
                (*((ULONG NKT_UNALIGNED *)d))++;
                d += nOfs;
                *((ULONGLONG NKT_UNALIGNED*)d) = NKT_PTR_2_ULONGLONG(cMod->hInst);
                d += sizeof(ULONGLONG);
                *((ULONGLONG NKT_UNALIGNED*)d) = (ULONG)nNameLen;
                d += sizeof(ULONG);
                nktMemCopy(d, (LPWSTR)(cMod->cStrNameW), nNameLen);
              }
              else
              {
                hRes = E_OUTOFMEMORY;
              }
            }
            if (SUCCEEDED(hRes))
            {
              //if yes, add to the list and simply return ok
              sLoadedModules.cList.PushTail(cMod.Detach());
            }
          }
          else
          {
            hRes = E_OUTOFMEMORY;
          }
        }
      }
    }
  }
  //activate transport
  NktInterlockedExchange(&nTransportActive, 1);
  //send available dlls
  if (SUCCEEDED(hRes))
  {
    NKT_DV_TMSG_ONLOADLIBRARY sMsg;
    HANDLE hWaitEvents[4];
    CNktEvent cProcessedEvent;

    if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
      hRes = E_OUTOFMEMORY; //error
    if (SUCCEEDED(hRes))
    {
      hWaitEvents[0] = cShutdownMtx.GetEvent();
      hWaitEvents[1] = hShutdownEvent;
      hWaitEvents[2] = hMainThread;
      hWaitEvents[3] = cProcessedEvent.GetEventHandle();
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_OnLoadLibraryCalled, cTransport.GetNextMsgId(),
                                       hWaitEvents[3], NULL);
    }
    //send message
    if (SUCCEEDED(hRes))
    {
      hRes = cTransport.SendBigPacketMsg(&(sMsg.dwDllNameBigPacketId), aDllInfoBuf.GetBuffer(),
                                         aDllInfoBuf.GetCount(), 0);
      if (SUCCEEDED(hRes))
        hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
    }
    //wait until message is processed (if sent sync)
    if (SUCCEEDED(hRes))
    {
      switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
        case WAIT_OBJECT_0+2:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+3:
          break;
        default:
          hRes = E_FAIL; //wait failed (?)
          break;
      }
    }
  }
  /*
  //check modifications to ntdll.dll
  if (SUCCEEDED(hRes))
  {
    hRes = (aModifiedImageBuf.SetCount(sizeof(ULONG)) != FALSE) ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hRes))
    {
      *((ULONG NKT_UNALIGNED *)aModifiedImageBuf.GetBuffer()) = 0;
      //----
      hRes = CheckDllModificationsWithImageFile(::GetModuleHandleW(L"ntdll.dll"), aModifiedImageBuf);
    }
  }
  //send initial message
  hRes = (cProcessedEvent.Create(TRUE, FALSE) != FALSE) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
  {
    hWaitEvents[0] = cShutdownMtx.GetEvent();
    hWaitEvents[1] = hShutdownEvent;
    hWaitEvents[2] = hMainThread;
    hWaitEvents[3] = cProcessedEvent.GetEventHandle();
    //prepare data
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_InitialConnectionMessage, cTransport.GetNextMsgId(),
                                     hWaitEvents[3], NULL);
  }
  //send message
  if (SUCCEEDED(hRes))
  {
    sMsg.sDllInfo.nPtr = (ULONGLONG)aDllInfoBuf.GetBuffer();
    sMsg.sDllInfo.nSize = (ULONGLONG)aDllInfoBuf.GetCount();
    sMsg.sModifiedImage.nPtr = (ULONGLONG)aModifiedImageBuf.GetBuffer();
    sMsg.sModifiedImage.nSize = (ULONGLONG)aModifiedImageBuf.GetCount();
    hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
  }
  //wait until message is processed
  if (SUCCEEDED(hRes))
  {
    switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
    {
      case WAIT_OBJECT_0:
      case WAIT_OBJECT_0+1:
      case WAIT_OBJECT_0+2:
        hRes = NKT_DVERR_Cancelled;
        break;
      case WAIT_OBJECT_0+3:
        break;
      default:
        hRes = E_FAIL; //wait failed (?)
        break;
    }
  }
  */
  //check errors
  if (FAILED(hRes))
    TAC_OnTransportError(lpTransport, hRes);
  NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) Agent [Connection established]: hRes=%08X",
                     ::GetTickCount(), hRes));
  return;
}

VOID CDvAgentMgr::TAC_OnConnectionClosed(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  NktInterlockedExchange(&nTransportActive, 0);
  ::SetEvent(hShutdownEvent);
  NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) Agent [Connection closed]: hRes=%08X",
                     ::GetTickCount(), hRes));
  return;
}

HRESULT CDvAgentMgr::TAC_OnEngineMessage(__inout CNktDvTransportAgent *lpTransport,
                                         __inout NKT_DV_TMSG_COMMON *lpMsg, __in SIZE_T nMsgSize,
                                         __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  //if the message was sent and uses a callback (allowed while shutting down too)
  if (lpMsg->nPtrCallback != NULL && (lpMsg->dwMsgId & NKT_DV_TMSG_ID_CALLBACK) != 0)
  {
    CNktDvTransportMessageCallbackHelperBase *lpCallback;
    HRESULT hRes;

    try
    {
      lpCallback = (CNktDvTransportMessageCallbackHelperBase*)(lpMsg->nPtrCallback);
      hRes = lpCallback->CopyResponseAndSignal(lpMsg, nMsgSize);
    }
    catch (...)
    {
      hRes = NKT_DVERR_ExceptionRaised;
    }
    return hRes;
  }
  if (cShutdownMtx.IsShuttingDown() != FALSE)
    return S_OK;
  switch (lpMsg->dwMsgCode)
  {
    case NKT_DV_TMSG_CODE_LoadDll:
      if (nMsgSize < sizeof(NKT_DV_TMSG_LOADDLL))
        break;
      return OnEngMsg_LoadDll(reinterpret_cast<NKT_DV_TMSG_LOADDLL*>(lpMsg), lpConnBigData);

    case NKT_DV_TMSG_CODE_UnloadDll:
      if (nMsgSize < sizeof(NKT_DV_TMSG_UNLOADDLL))
        break;
      return OnEngMsg_UnloadDll(reinterpret_cast<NKT_DV_TMSG_UNLOADDLL*>(lpMsg), lpConnBigData);

    case NKT_DV_TMSG_CODE_CallApi:
      if (nMsgSize < sizeof(NKT_DV_TMSG_CALLCUSTOMAPI))
        break;
      return OnEngMsg_CallCustomApi(reinterpret_cast<NKT_DV_TMSG_CALLCUSTOMAPI*>(lpMsg), lpConnBigData);

    case NKT_DV_TMSG_CODE_AddHook:
      if (nMsgSize < sizeof(NKT_DV_TMSG_ADDHOOK))
        break;
      return OnEngMsg_AddHook(reinterpret_cast<NKT_DV_TMSG_ADDHOOK*>(lpMsg), lpConnBigData);

    case NKT_DV_TMSG_CODE_RemoveHook:
      if (nMsgSize < sizeof(NKT_DV_TMSG_REMOVEHOOK))
        break;
      return OnEngMsg_RemoveHook(reinterpret_cast<NKT_DV_TMSG_REMOVEHOOK*>(lpMsg));

    case NKT_DV_TMSG_CODE_EnableHook:
      if (nMsgSize < sizeof(NKT_DV_TMSG_ENABLEHOOK))
        break;
      return OnEngMsg_EnableHook(reinterpret_cast<NKT_DV_TMSG_ENABLEHOOK*>(lpMsg));

    case NKT_DV_TMSG_CODE_BatchHookExec:
      if (nMsgSize < sizeof(NKT_DV_TMSG_BATCHHOOKEXEC))
        break;
      return OnEngMsg_BatchHookExec(reinterpret_cast<NKT_DV_TMSG_BATCHHOOKEXEC*>(lpMsg));

    case NKT_DV_TMSG_CODE_HelperCall:
      if (nMsgSize < sizeof(NKT_DV_TMSG_HELPERCALL))
        break;
      return OnEngMsg_HelperCall(reinterpret_cast<NKT_DV_TMSG_HELPERCALL*>(lpMsg));
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) Agent [Invalid message received]: "
                                                          "Code=%lu, Id=%lu",
                     ::GetTickCount(), lpMsg->dwMsgCode & (~NKT_DV_TMSG_CODE_CALLBACK),
                     lpMsg->dwMsgId & (~NKT_DV_TMSG_ID_CALLBACK)));
  return E_FAIL;
}

VOID CDvAgentMgr::TAC_OnTransportError(__inout CNktDvTransportAgent *lpTransport, __in HRESULT hRes)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);

  NKT_DEBUGPRINTLNA(Nektra::dlAgent|Nektra::dlTransport, ("%lu) Agent [Transport error]: hRes=%08X",
                     ::GetTickCount(), hRes));
  NktInterlockedExchange(&nTransportActive, 0);
  ::SetEvent(hShutdownEvent);
  return;
}

BOOL CDvAgentMgr::HEC_IsSystemThread()
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  DWORD dwTid;

  if (cShutdownMtx.IsShuttingDown() != FALSE)
    return TRUE; //if shutting down return => avoid the api hook
  //check if we are dealing with an agent thread
  dwTid = ::GetCurrentThreadId();
  if (dwTid == dwRunThread || dwRunThread == 0)
    return TRUE;
  if (cTransport.IsTransportThread(dwTid) != FALSE)
    return TRUE;
  {
    CNktAutoFastMutex cLock(&(sThreadIgnore.cMtx));

    if (sThreadIgnore.cList.BinarySearch(&dwTid, &ThreadIgnore_Compare) != NKT_SIZE_T_MAX)
      return TRUE;
  }
  return FALSE;
}

VOID CDvAgentMgr::HEC_OnError(__in HRESULT hRes)
{
  //on error (usually memory error) => initiate shutdown
  NktInterlockedExchange(&nTransportActive, 0);
  ::SetEvent(hShutdownEvent);
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnHookError]: hRes=%08X", ::GetTickCount(), hRes));
  return;
}

HRESULT CDvAgentMgr::HEC_OnHookCalled(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                      __inout LPBYTE lpIntercallCustomData,
                                      __in CNktDvFunctionParamsCache::FUNCTION_PARAMS *lpFuncParams,
                                      __in CNktDvParamsEnumerator *lpCustomParamsEnum)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  HRESULT hRes, hRes2;

  if (cShutdownMtx.IsShuttingDown() != FALSE)
    return S_FALSE; //if shutting down return => avoid the api hook
  //if it is an internal hook
  switch (sCallInfo.dwHookId)
  {
    case INTERNAL_HOOKID_CreateProcessInternalW:
      hRes = OnInternalCreateProcess(sCallInfo, lpIntercallCustomData);
      break;

    case INTERNAL_HOOKID_LdrLoadDll:
      hRes = OnInternalLdrLoadDll(sCallInfo, lpIntercallCustomData);
      if (SUCCEEDED(hRes) && sCallInfo.nPhase == CNktDvHookEngine::phPostCall &&
          (nDelayedCheckOverrides & 1) != 0 && nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
      {
        hRes2 = CheckOverwrittenHooks();
        if (FAILED(hRes2))
          hRes = hRes2;
      }
      break;

    case INTERNAL_HOOKID_LdrUnloadDll:
      hRes = OnInternalLdrUnloadDll(sCallInfo, lpIntercallCustomData);
      if (SUCCEEDED(hRes) && sCallInfo.nPhase == CNktDvHookEngine::phPostCall &&
          (nDelayedCheckOverrides & 1) != 0 && nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
      {
        hRes2 = CheckOverwrittenHooks();
        if (FAILED(hRes2))
          hRes = hRes2;
      }
      break;

    case INTERNAL_HOOKID_NtSuspendThread:
      hRes = OnInternalNtSuspendThread(sCallInfo, lpIntercallCustomData);
      break;

    case INTERNAL_HOOKID_NtResumeThread:
      hRes = OnInternalNtResumeThread(sCallInfo, lpIntercallCustomData, FALSE);
      /*
      if (SUCCEEDED(hRes) && sCallInfo.nPhase == CNktDvHookEngine::phPreCall)
      {
        if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
        {
          NktInterlockedBitSet(&nDelayedCheckOverrides, 0);
        }
        else
        {
          hRes2 = CheckOverwrittenHooks();
          if (FAILED(hRes2))
            hRes = hRes2;
        }
      }
      */
      break;

    case INTERNAL_HOOKID_NtAlertResumeThread:
      hRes = OnInternalNtResumeThread(sCallInfo, lpIntercallCustomData, TRUE);
      break;

    case INTERNAL_HOOKID_NtMapViewOfSection:
      hRes = OnInternalNtMapViewOfSection(sCallInfo, lpIntercallCustomData);
      if (SUCCEEDED(hRes) && sCallInfo.nPhase == CNktDvHookEngine::phPostCall)
      {
        if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
        {
          NktInterlockedBitSet(&nDelayedCheckOverrides, 0);
        }
        else
        {
          hRes2 = CheckOverwrittenHooks();
          if (FAILED(hRes2))
            hRes = hRes2;
        }
      }
      break;

    case INTERNAL_HOOKID_NtUnmapViewOfSection:
      hRes = OnInternalNtUnmapViewOfSection(sCallInfo, lpIntercallCustomData);
      break;

    case INTERNAL_HOOKID_NtProtectVirtualMemory:
      hRes = S_OK;
      if (sCallInfo.nPhase != CNktDvHookEngine::phPreCall)
      {
        ULONG NewProt;

        //try to get needed parameters
        //declaration of LdrUnloadDll is the following:
        //  NTSTATUS NTAPI NtProtectVirtualMemory(IN HANDLE ProcessHandle, IN OUT PVOID *UnsafeBaseAddress,
        //                                        IN OUT SIZE_T * UnsafeNumberOfBytesToProtect,
        //                                        IN ULONG NewAccessProtection, OUT PULONG UnsafeOldAccessProtection);
#if defined _M_IX86
        if (SecureMemCopy(&NewProt, (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x10),
                          sizeof(ULONG)) == FALSE)
          NewProt = 0;
#elif defined _M_X64
        NewProt = (ULONG)(sCallInfo.lpCallState->sAsmRegisters.sInteger.R9);
#endif
        if (NewProt == PAGE_EXECUTE || NewProt == PAGE_EXECUTE_READ || NewProt == PAGE_EXECUTE_READWRITE ||
            NewProt == PAGE_EXECUTE_WRITECOPY)
        {
          if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
          {
            NktInterlockedBitSet(&nDelayedCheckOverrides, 0);
          }
          else
          {
            hRes = CheckOverwrittenHooks();
          }
        }
      }
      break;

    default: //user hook
      hRes = OnUserHook(sCallInfo, lpFuncParams, lpCustomParamsEnum, lpIntercallCustomData);
      break;
  }
  return hRes;
}

HRESULT CDvAgentMgr::HEC_OnSendCustomMessage(__out SIZE_T *lpnRetVal, __in SIZE_T nMsgCode,
                                             __in SIZE_T nMsgParam, __in BOOL bSync)
{
  CNktDvShutdownAutoLock cSdLock(&cShutdownMtx);
  NKT_DV_TMSG_CUSTOMMESSAGE sMsg;
  ULONGLONG nRetVal;
  CNktEvent cProcessedEvent;
  HANDLE hWaitEvents[4];
  HRESULT hRes;

  if (lpnRetVal == NULL)
    return E_POINTER;
  *lpnRetVal = 0;
  if (cShutdownMtx.IsShuttingDown() != FALSE)
    return S_FALSE; //if shutting down return => avoid the api hook
  if (nTransportActive == 0)
    return S_FALSE; //ignore call if connection was not established
  //prepare data
  if (bSync != FALSE)
  {
    //create wait event
    if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
      return E_OUTOFMEMORY; //error
    hWaitEvents[0] = cShutdownMtx.GetEvent();
    hWaitEvents[1] = hShutdownEvent;
    hWaitEvents[2] = hMainThread;
    hWaitEvents[3] = cProcessedEvent.GetEventHandle();
  }
  else
  {
    nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
  }
  //prepare data
  hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_CustomMessage, cTransport.GetNextMsgId(),
                                   hWaitEvents[3], NULL);
  if (SUCCEEDED(hRes))
  {
    sMsg.nCode = (ULONGLONG)nMsgCode;
    sMsg.nParam = (ULONGLONG)nMsgParam;
    sMsg.bASync = !bSync;
    sMsg.dwThreadId = ::GetCurrentThreadId();
    sMsg.nAddressOfReturnValue = NKT_PTR_2_ULONGLONG(&nRetVal);
    //send message
    hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
    if (SUCCEEDED(hRes) && hWaitEvents[0] != NULL)
    {
      //wait until message is processed
      switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
      {
        case WAIT_OBJECT_0:
        case WAIT_OBJECT_0+1:
        case WAIT_OBJECT_0+2:
          hRes = NKT_DVERR_Cancelled;
          break;
        case WAIT_OBJECT_0+3:
          break;
        default:
          hRes = E_FAIL; //wait failed (?)
          break;
      }
    }
  }
  if (SUCCEEDED(hRes))
    *lpnRetVal = (SIZE_T)nRetVal;
  return hRes;
}

HRESULT CDvAgentMgr::HEC_OnThreadIgnore(__in DWORD dwTid, __in BOOL bIgnore)
{
  CNktAutoFastMutex cLock(&(sThreadIgnore.cMtx));
  SIZE_T nIdx;

  nIdx = sThreadIgnore.cList.BinarySearch(&dwTid, &ThreadIgnore_Compare);
  if (bIgnore != FALSE)
  {
    if (nIdx == NKT_SIZE_T_MAX)
    {
      if (sThreadIgnore.cList.SortedInsert(dwTid) == FALSE)
        return E_OUTOFMEMORY;
    }
  }
  else
  {
    if (nIdx != NKT_SIZE_T_MAX)
      sThreadIgnore.cList.RemoveElementAt(nIdx);
  }
  return S_OK;
}

HRESULT CDvAgentMgr::HEC_OnHookOverwritten(__in LPDWORD aHookIdsList, __in SIZE_T nCount)
{
  NKT_DV_TMSG_HOOKOVERWRITTEN sMsg;
  SIZE_T nStart, nThisCount;
  HANDLE hWaitEvents[4];
  CNktEvent cProcessedEvent;
  HRESULT hRes;

  if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
    return E_OUTOFMEMORY; //error
  hWaitEvents[0] = cShutdownMtx.GetEvent();
  hWaitEvents[1] = hShutdownEvent;
  hWaitEvents[2] = hMainThread;
  hWaitEvents[3] = cProcessedEvent.GetEventHandle();
  for (nStart=0; nCount>0; nStart+=nThisCount,nCount-=nThisCount)
  {
    nThisCount = nCount;
    if (nThisCount > NKT_DV_ARRAYLEN(sMsg.aHookIdsList))
      nThisCount = NKT_DV_ARRAYLEN(sMsg.aHookIdsList);
    //prepare data
    cProcessedEvent.Reset();
    hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_HookOverwritten, cTransport.GetNextMsgId(),
                                     hWaitEvents[3], NULL);
    if (FAILED(hRes))
      return hRes;
    nktMemCopy(sMsg.aHookIdsList, aHookIdsList+nStart, nThisCount*sizeof(DWORD));
    sMsg.dwCount = (DWORD)nThisCount;
    //send message
    hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
    if (FAILED(hRes))
      return hRes; //error
    //wait until message is processed
    switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
    {
      case WAIT_OBJECT_0:
      case WAIT_OBJECT_0+1:
      case WAIT_OBJECT_0+2:
        hRes = NKT_DVERR_Cancelled;
        break;
      case WAIT_OBJECT_0+3:
        break;
      default:
        hRes = E_FAIL; //wait failed (?)
        break;
    }
    if (hRes != S_OK)
      return hRes;
  }
  return S_OK;
}

HRESULT CDvAgentMgr::HEC_OnInvalidateCache(__in HINSTANCE hDll)
{
  CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), (HINSTANCE)hDll);
  return S_OK;
}

static BOOL Try_PreCall_UserData_OnInternalCreateProcess(__in LPPROCESS_INFORMATION lpProcessInformation,
                                                         __in USERDATA_OnInternalCreateProcess *lpUserData)
{
  BOOL b = FALSE;

  __try
  {
    lpUserData->bWasSuspended = (((*(lpUserData->lpdwCreationFlags)) & CREATE_SUSPENDED) != 0) ? TRUE : FALSE;
    lpUserData->lpProcessInformation = lpProcessInformation;
    //suspend new process' main thread while informing the server
    *(lpUserData->lpdwCreationFlags) |= CREATE_SUSPENDED;
    b = TRUE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

static BOOL Try_PostCall_UserData_OnInternalCreateProcess(__out HANDLE *lphThread, __out HANDLE *lphProcess,
                                                          __out LPDWORD lpdwPid, __out LPDWORD lpdwTid,
                                                          __in USERDATA_OnInternalCreateProcess *lpUserData)
{
  BOOL b = FALSE;

  __try
  {
    *lphThread = lpUserData->lpProcessInformation->hThread;
    *lphProcess = lpUserData->lpProcessInformation->hProcess;
    *lpdwPid = lpUserData->lpProcessInformation->dwProcessId;
    *lpdwTid = lpUserData->lpProcessInformation->dwThreadId;
    b = TRUE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

HRESULT CDvAgentMgr::OnInternalCreateProcess(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                             __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      LPPROCESS_INFORMATION *lplpProcessInformation;
      USERDATA_OnInternalCreateProcess *lpUserData;

      lpUserData = (USERDATA_OnInternalCreateProcess*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of CreateProcessInternalW is the following:
      //  BOOL CreateProcessInternalW(HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
      //           LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
      //           BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR
      //           lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION
      //           lpProcessInformation, PHANDLE hNewToken);
#if defined _M_IX86
      lpUserData->lpdwCreationFlags = (LPDWORD)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x1C);
      lplpProcessInformation = (LPPROCESS_INFORMATION*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp +
                                                        0x2C);
#elif defined _M_X64
      lpUserData->lpdwCreationFlags = (LPDWORD)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rsp + 0x38);
      lplpProcessInformation = (LPPROCESS_INFORMATION*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rsp +
                                                        0x58);
#endif
      if (*lplpProcessInformation == NULL)
        return S_FALSE; //ignore call if ProcInfo is NULL
      if (nTransportActive == 0)
        return S_FALSE; //ignore call if connection was not established
      if (Try_PreCall_UserData_OnInternalCreateProcess(*lplpProcessInformation, lpUserData) == FALSE)
        return S_FALSE; //ignore call if memory is not readable
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      TNktAutoDeletePtr<CCreatedProcessesItem> sCp;
      HANDLE hProcess, hThread, hEvents[2], hEventCopy[2];
      HRESULT hRes;
      USERDATA_OnInternalCreateProcess *lpUserData;
#if defined _M_IX86
      NKT_DV_TMSG_SUSPENDX64THREADFROMX86 sMsg;
      TNktDvTransportMessageCallbackHelper<NKT_DV_TMSG_SUSPENDX64THREADFROMX86_CALLBACK> cMsgResp;
#endif //_M_IX86

      hProcess = hThread = NULL; //initialize some variables
      sCp.Attach(NKT_MEMMGR_NEW CCreatedProcessesItem);
      if (sCp == NULL)
      {
        hRes = E_OUTOFMEMORY;
        goto icp_pc_end;
      }
      sCp->nSuspendCount = 1;
      lpUserData = (USERDATA_OnInternalCreateProcess*)lpIntercallCustomData;
#if defined _M_IX86
      hRes = (sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax != 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax != 0) ? S_OK : S_FALSE;
#endif
      //if create process failed, don't inform the server
      if (hRes != S_OK)
        goto icp_pc_end;
      //get new process data
      if (Try_PostCall_UserData_OnInternalCreateProcess(&hThread, &hProcess, &(sCp->sMsg.dwPid), &(sCp->sMsg.dwTid),
                                                        lpUserData) == FALSE)
        hRes = S_FALSE; //ignore call if memory is not readable
      if (nTransportActive == 0)
        hRes = S_FALSE; //ignore call if connection was not established
      if (hRes == S_OK && hThread == NULL)
        hRes = S_FALSE;
      if (hRes != S_OK)
        goto icp_pc_end;
      //duplicate process & thread handle
      hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), hProcess, ::GetCurrentProcess(), &(sCp->hProcess), 0,
                                          FALSE, DUPLICATE_SAME_ACCESS);
      if (SUCCEEDED(hRes))
      {
        hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), hThread, ::GetCurrentProcess(), &(sCp->hThread), 0,
                                            FALSE, DUPLICATE_SAME_ACCESS);
      }
      if (FAILED(hRes))
        goto icp_pc_end;
      //check if we can hook
      sCp->sMsg.bIs64BitProcess = nktDvDynApis_Is64BitProcess(sCp->hProcess);
      sCp->sMsg.bCanHookNow = TRUE;
      hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), sCp->hProcess, hServerProcDup, &hEventCopy[0], 0,
                                          FALSE, DUPLICATE_SAME_ACCESS);
      if (FAILED(hRes))
        goto icp_pc_end;
      sCp->sMsg.hProcess = NKT_PTR_2_ULONGLONG(hEventCopy[0]);
#if defined _M_IX86
      if (sCp->sMsg.bIs64BitProcess != FALSE)
      {
        if (nServerPlatformBits == 64)
        {
          //we are launching a 64-bit process from a 32-bit one so we need server assistance
          //to execute the SuspendAfterCreateProcessW api
          hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), hThread, hServerProcDup, &hEventCopy[1], 0, FALSE,
                                              DUPLICATE_SAME_ACCESS);
          if (FAILED(hRes))
            goto icp_pc_end;
          //prepare data
          hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_SuspendX64ThreadFromX86,
                                           cTransport.GetNextMsgId(), NULL, &cMsgResp);
          if (FAILED(hRes))
            goto icp_pc_end;
          sMsg.hProcess = NKT_PTR_2_ULONGLONG(hEventCopy[0]);
          sMsg.hMainThread = NKT_PTR_2_ULONGLONG(hEventCopy[1]);
          //send message
          hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
          if (FAILED(hRes))
            goto icp_pc_end;
          hRes = cMsgResp.Wait(cShutdownMtx.GetEvent(), hShutdownEvent, hMainThread);
          if (FAILED(hRes))
            goto icp_pc_end;
          if (FAILED(cMsgResp->hRes))
          {
            hRes = cMsgResp->hRes;
            goto icp_pc_end;
          }
          sCp->hReadyEvent = (HANDLE)(cMsgResp->hReadyEvent);
          sCp->hContinueEvent = (HANDLE)(cMsgResp->hContinueEvent);
          goto icp_pc_cont1;
        }
        else
        {
          //we cannot hook 64-bit processes if using 32-bit server
          hRes = S_FALSE;
          goto icp_pc_end;
        }
      }
#endif //_M_IX86
      //try to complete the child process initialization by adding custom code at entry point
      hRes = CNktDvTools::SuspendAfterCreateProcessW(&(sCp->hReadyEvent), &(sCp->hContinueEvent),
                                                     hServerProcDup, hProcess, hThread);
      if (FAILED(hRes))
        goto icp_pc_end;
#if defined _M_IX86
icp_pc_cont1:
#endif //_M_IX86
      //add the process to the "created processes" list if suspended
      if (lpUserData->bWasSuspended != FALSE)
      {
        CNktAutoFastMutex cLock(this);

        NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnInternalCreateProcess]: Process %lu "
                          "(Thread %lu) created suspended", ::GetTickCount(), sCp->sMsg.dwPid,
                          sCp->sMsg.dwTid));
        cCreatedProcessesList.PushTail(sCp.Detach());
      }
      //we are done here
      hRes = S_OK;
icp_pc_end:
      if (lpUserData->bWasSuspended == FALSE && hThread != NULL)
      {
        nktDvDynApis_NtResumeThread(hThread);
        if (SUCCEEDED(hRes) && sCp != NULL)
        {
          NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnInternalCreateProcess]: Process %lu "
                            "(Thread %lu) created", ::GetTickCount(), sCp->sMsg.dwPid,
                            sCp->sMsg.dwTid));
          //wait until initialization completes (don't worry about return code)
          hEvents[0] = sCp->hThread;
          hEvents[1] = sCp->hReadyEvent;
          ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
          //if not suspended, send message to the server
          hRes = SendCreateProcessMsg(sCp);
        }
      }
      if (hRes != S_OK)
        return hRes;
      }
      break;
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnInternalLdrLoadDll(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                          __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      /*
      NKT_DV_TMSG_COMMON sMsg;
      HANDLE hWaitEvents[4];
      HRESULT hRes;
      CNktEvent cProcessedEvent;
      */
      USERDATA_OnInternalLdrLoadDll *lpUserData;
      PUNICODE_STRING lpUsDllName;

      lpUserData = (USERDATA_OnInternalLdrLoadDll*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of LdrLoadDll is the following:
      //  NTSTATUS NTAPI LdrLoadDll(__in PWCHAR SearchPath, __in PULONG pdwLdrErr,
      //                            __in PUNICODE_STRING DllName, __out PVOID *BaseAddress);
#if defined _M_IX86
      if (SecureMemCopy(&(lpUserData->lphDll), (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x10),
                        sizeof(HINSTANCE*)) == FALSE)
        lpUserData->lphDll = NULL;
      if (SecureMemCopy(&lpUsDllName, (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x0C),
                        sizeof(PUNICODE_STRING)) == FALSE)
        lpUsDllName = NULL;
#elif defined _M_X64
      lpUserData->lphDll = (HINSTANCE*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.R9);
      lpUsDllName = (PUNICODE_STRING)(sCallInfo.lpCallState->sAsmRegisters.sInteger.R8);
#endif
      lpUserData->bIsMsCoreeDll = FALSE;
      if (OnLdrLoadDll_IsDll(lpUsDllName, L"mscoree.dll") != FALSE)
        lpUserData->bIsMsCoreeDll = TRUE;
      /*
      CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId());
      if (nTransportActive == 0)
        return S_FALSE; //ignore call if connection was not established
      //send an invalidate cache command to the engine's agent controller
      if (CDvAgentMgrAutoTlsSendDllNotificationAsync::DoAsyncNotify() == FALSE)
      {
        //create wait event
        if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
          return E_OUTOFMEMORY; //error
        hWaitEvents[0] = cShutdownMtx.GetEvent();
        hWaitEvents[1] = hShutdownEvent;
        hWaitEvents[2] = hMainThread;
        hWaitEvents[3] = cProcessedEvent.GetEventHandle();
      }
      else
      {
        nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
      }
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_InvalidateAgentCtrlDllCache,
                                       cTransport.GetNextMsgId(), hWaitEvents[3], NULL);
      if (SUCCEEDED(hRes))
      {
        //send message
        hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
        if (SUCCEEDED(hRes) && hWaitEvents[0] != NULL)
        {
          //wait until message is processed
          switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
          {
            case WAIT_OBJECT_0:
            case WAIT_OBJECT_0+1:
            case WAIT_OBJECT_0+2:
              hRes = NKT_DVERR_Cancelled;
              break;
            case WAIT_OBJECT_0+3:
              break;
            default:
              hRes = E_FAIL; //wait failed (?)
              break;
          }
        }
      }
      //done
      if (hRes != S_OK)
        return hRes;
      */
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      NKT_DV_TMSG_ONLOADLIBRARY sMsg;
      HINSTANCE hDll;
      HANDLE hWaitEvents[4];
      HRESULT hRes;
      CNktEvent cProcessedEvent;
      USERDATA_OnInternalLdrLoadDll *lpUserData;
      SIZE_T i, nLen;

#if defined _M_IX86
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax)) >= 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax)) >= 0) ? S_OK : S_FALSE;
#endif
      //if module load failed, don't inform the server
      if (hRes != S_OK)
        return S_FALSE;
      lpUserData = (USERDATA_OnInternalLdrLoadDll*)lpIntercallCustomData;
      if (SecureMemCopy(&hDll, lpUserData->lphDll, sizeof(HINSTANCE)) == FALSE)
        return S_FALSE;
      if (hDll == NULL)
        return S_FALSE;
      //----
      if (lpUserData->bIsMsCoreeDll != FALSE)
      {
        hRes = InstallMiniHooks();
        if (FAILED(hRes))
          return hRes; //error
      }
      //----
      CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), hDll);
      if (nTransportActive == 0)
        return S_FALSE; //ignore call if connection was not established
      {
        CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
        TNktLnkLst<CLoadedModule>::Iterator it;
        TNktAutoDeletePtr<CLoadedModule> cMod;
        CLoadedModule *lpMod;
        TNktAutoFreePtr<BYTE> aTempBuf;
        LPBYTE d;

        for (lpMod=it.Begin(sLoadedModules.cList); lpMod!=NULL; lpMod=it.Next())
        {
          if (lpMod->hInst == hDll && lpMod->bUnloading == FALSE)
            return S_FALSE; //only inform first load
        }
        cMod.Attach(NKT_MEMMGR_NEW CLoadedModule);
        if (cMod == NULL)
          return E_OUTOFMEMORY;
        cMod->hInst = hDll;
        cMod->bLoaderWasLocked = nktDvNtIsLoaderLoaderLockHeld(FALSE);
        cMod->bUnloading = FALSE;
        //get module name
        hRes = CNktDvTools::GetExecutableFileName(cMod->cStrNameW, ::GetCurrentProcess(), cMod->hInst);
        if (FAILED(hRes))
          return hRes; //error
        //check if we are inside a loaderlock
        if (cMod->bLoaderWasLocked != FALSE)
        {
          //if yes, add to the list and simply return ok
          sLoadedModules.cList.PushTail(cMod.Detach());
          return S_OK;
        }
        //if not, inform about this dll and all pending ones
        //create wait event
        //NOTE: initial dll messages are sent async
        if (CDvAgentMgrAutoSendDllNotificationAsync::DoAsyncNotify(&nSendLoadedUnloadedDllAsync) == FALSE)
        //if (CDvAgentMgrAutoTlsSendDllNotificationAsync::DoAsyncNotify() == FALSE)
        {
          if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
            return E_OUTOFMEMORY; //error
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = hShutdownEvent;
          hWaitEvents[2] = hMainThread;
          hWaitEvents[3] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
        }
        //prepare data
        hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_OnLoadLibraryCalled, cTransport.GetNextMsgId(),
                                         hWaitEvents[3], NULL);
        if (FAILED(hRes))
          return hRes; //error
        //build dll loaded list
        nLen = sizeof(ULONG) + sizeof(ULONGLONG) + sizeof(ULONG) + cMod->cStrNameW.GetLength() * sizeof(WCHAR);
        i = 1;
        for (lpMod=it.Begin(sLoadedModules.cList); lpMod!=NULL; lpMod=it.Next())
        {
          if (lpMod->bLoaderWasLocked != FALSE)
          {
            nLen += sizeof(ULONGLONG) + sizeof(ULONG) + lpMod->cStrNameW.GetLength() * sizeof(WCHAR);
            i++;
          }
        }
        aTempBuf.Attach(d = (LPBYTE)nktMemMalloc(nLen));
        if (aTempBuf == NULL)
          return E_OUTOFMEMORY;
        //add count, names and instances to buffer
        *((ULONG NKT_UNALIGNED*)d) = (ULONG)i;
        d += sizeof(ULONG);
        *((ULONGLONG NKT_UNALIGNED*)d) = NKT_PTR_2_ULONGLONG(cMod->hInst);
        d += sizeof(ULONGLONG);
        i = cMod->cStrNameW.GetLength() * sizeof(WCHAR);
        *((ULONG NKT_UNALIGNED*)d) = (ULONG)i;
        d += sizeof(ULONG);
        nktMemCopy(d, (LPWSTR)(cMod->cStrNameW), i);
        d += i;
        for (lpMod=it.Begin(sLoadedModules.cList); lpMod!=NULL; lpMod=it.Next())
        {
          if (lpMod->bLoaderWasLocked != FALSE)
          {
            lpMod->bLoaderWasLocked = FALSE;
            *((ULONGLONG NKT_UNALIGNED*)d) = NKT_PTR_2_ULONGLONG(lpMod->hInst);
            d += sizeof(ULONGLONG);
            i = lpMod->cStrNameW.GetLength() * sizeof(WCHAR);
            *((ULONG NKT_UNALIGNED*)d) = (ULONG)i;
            d += sizeof(ULONG);
            nktMemCopy(d, (LPWSTR)(lpMod->cStrNameW), i);
            d += i;
          }
        }
        NKT_ASSERT((SIZE_T)(d-aTempBuf.Get()) == nLen);
        //add the module to the loaded list before sending the message
        sLoadedModules.cList.PushTail(cMod.Detach());
        //send message
        hRes = cTransport.SendBigPacketMsg(&(sMsg.dwDllNameBigPacketId), aTempBuf.Get(), nLen, 0);
        if (SUCCEEDED(hRes))
          hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
        if (FAILED(hRes))
          return hRes; //error
      }
      if (hWaitEvents[0] != NULL)
      {
        //wait until message is processed (if sent sync)
        switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
        {
          case WAIT_OBJECT_0:
          case WAIT_OBJECT_0+1:
          case WAIT_OBJECT_0+2:
            hRes = NKT_DVERR_Cancelled;
            break;
          case WAIT_OBJECT_0+3:
            break;
          default:
            hRes = E_FAIL; //wait failed (?)
            break;
        }
        if (hRes != S_OK)
          return hRes;
      }
      }
      break;
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnInternalLdrUnloadDll(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                            __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      NKT_DV_TMSG_ONFREELIBRARY sMsg;
      HANDLE hWaitEvents[4];
      HINSTANCE hDll;
      HRESULT hRes;
      CLoadedModule *lpMod;
      CNktEvent cProcessedEvent;
      SIZE_T nCount, nUseCount;

      //try to get needed parameters
      //declaration of LdrUnloadDll is the following:
      //  NTSTATUS NTAPI LdrUnloadDll(__in PVOID BaseAddress);
#if defined _M_IX86
      if (SecureMemCopy(&hDll, (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x04),
                        sizeof(HINSTANCE)) == FALSE)
        hDll = NULL;
#elif defined _M_X64
      hDll = (HINSTANCE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rcx);
#endif
      if (hDll == NULL)
        return S_FALSE; //ignore call if handle is NULL
      //get usage count of this dll
      nUseCount = CNktDvTools::GetModuleUsageCount(hDll);
      if (nUseCount > 1)
        return S_FALSE; //only inform when about to be unloaded
      //check list
      {
        CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
        TNktLnkLst<CLoadedModule>::Iterator it;

        for (lpMod=it.Begin(sLoadedModules.cList); lpMod!=NULL; lpMod=it.Next())
        {
          if (lpMod->hInst == hDll && lpMod->bUnloading == FALSE)
            break;
        }
        if (lpMod == NULL)
          return S_FALSE; //only inform if in the list
        //mark as unloading
        lpMod->bUnloading = TRUE;
        //unhook api's in this dll
        cHookEngine.DllUnloadUnhook(hDll);
        CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), hDll);
      }
      //inform about unloads
      while (1)
      {
        if (nTransportActive == 0)
          return S_FALSE; //ignore call if connection was not established
        //only process when loaderlock is not held
        if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE)
          return S_OK;
        //add items to the list
        nktMemSet(&sMsg, 0, sizeof(sMsg));
        {
          CNktAutoFastMutex cLmLock(&(sLoadedModules.cLock));
          TNktLnkLst<CLoadedModule>::Iterator it;

          for (nCount=0,lpMod=it.Begin(sLoadedModules.cList); lpMod!=NULL && nCount<NKT_DV_ARRAYLEN(sMsg.aInstances);
               lpMod=it.Next())
          {
            if (lpMod->bUnloading != FALSE)
            {
              sMsg.aInstances[nCount++] = NKT_PTR_2_ULONGLONG(lpMod->hInst);
              lpMod->RemoveNode();
              delete lpMod;
              break;
            }
          }
        }
        if (nCount == 0)
          break;
        //create wait event
        if (CDvAgentMgrAutoSendDllNotificationAsync::DoAsyncNotify(&nSendLoadedUnloadedDllAsync) == FALSE)
        //if (CDvAgentMgrAutoTlsSendDllNotificationAsync::DoAsyncNotify() == FALSE)
        {
          if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
            return E_OUTOFMEMORY; //error
          hWaitEvents[0] = cShutdownMtx.GetEvent();
          hWaitEvents[1] = hShutdownEvent;
          hWaitEvents[2] = hMainThread;
          hWaitEvents[3] = cProcessedEvent.GetEventHandle();
        }
        else
        {
          nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
        }
        //prepare data
        hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_OnFreeLibraryCalled, cTransport.GetNextMsgId(),
                                         hWaitEvents[3], NULL);
        if (FAILED(hRes))
          return hRes; //error
        //send message
        hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
        if (FAILED(hRes))
          return hRes; //error
        //wait until message is processed (if sent sync)
        if (hWaitEvents[0] != NULL)
        {
          switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
          {
            case WAIT_OBJECT_0:
            case WAIT_OBJECT_0+1:
            case WAIT_OBJECT_0+2:
              hRes = NKT_DVERR_Cancelled;
              break;
            case WAIT_OBJECT_0+3:
              break;
            default:
              hRes = E_FAIL; //wait failed (?)
              break;
          }
          if (hRes != S_OK)
            return hRes;
        }
      }
      }
      break;
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnInternalNtSuspendThread(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                               __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      USERDATA_OnInternalNtSuspendThread *lpUserData;

      lpUserData = (USERDATA_OnInternalNtSuspendThread*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of NtSuspendThread is the following:
      //  NTSYSAPI NTSTATUS NTAPI NtSuspendThread(__in HANDLE ThreadHandle,
      //                                          OUT PULONG PreviousSuspendCount OPTIONAL);
#if defined _M_IX86
      if (SecureMemCopy(&(lpUserData->hThread), (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x04),
                        sizeof(HANDLE)) == FALSE)
        lpUserData->hThread = NULL;
#elif defined _M_X64
      lpUserData->hThread = (HANDLE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rcx);
#endif
      if (lpUserData->hThread == NULL)
        return S_FALSE; //ignore call if handle is NULL
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      USERDATA_OnInternalNtSuspendThread *lpUserData;
      DWORD dwTid;
      HRESULT hRes;

      lpUserData = (USERDATA_OnInternalNtSuspendThread*)lpIntercallCustomData;
#if defined _M_IX86
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax)) >= 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax)) >= 0) ? S_OK : S_FALSE;
#endif
      if (hRes == S_FALSE)
        return S_FALSE; //not found
      //get thread id
      dwTid = CNktDvTools::GetThreadId(lpUserData->hThread);
      //increment suspend count
      {
        CNktAutoFastMutex cLock(this);
        TNktLnkLst<CCreatedProcessesItem>::Iterator it;
        CCreatedProcessesItem *lpCpItem;

        for (lpCpItem=it.Begin(cCreatedProcessesList); lpCpItem!=NULL; lpCpItem=it.Next())
        {
          if ((dwTid != 0) ? (dwTid == lpCpItem->sMsg.dwTid) : (lpCpItem->hThread == lpUserData->hThread))
          {
            NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnInternalNtSuspendThread]: Process %lu "
                              "(Thread %lu) suspend-add (%lu)", ::GetTickCount(), lpCpItem->sMsg.dwPid,
                              lpCpItem->sMsg.dwTid, lpCpItem->nSuspendCount+1));
            (lpCpItem->nSuspendCount)++;
            break;
          }
        }
      }
      }
      break;
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnInternalNtResumeThread(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                              __inout LPBYTE lpIntercallCustomData, __in BOOL bAlert)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      USERDATA_OnInternalNtResumeThread *lpUserData;

      lpUserData = (USERDATA_OnInternalNtResumeThread*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of NtResumeThread and NtAlertResumeThread is the following:
      //  NTSYSAPI NTSTATUS NTAPI NtResumeThread(__in HANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL);
      //  NTSYSAPI NTSTATUS NTAPI NtAlertResumeThread(__in HANDLE ThreadHandle,
      //                                              OUT PULONG SuspendCount OPTIONAL);
#if defined _M_IX86
      if (SecureMemCopy(&(lpUserData->hThread), (LPBYTE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Esp + 0x04),
                        sizeof(HANDLE)) == FALSE)
        lpUserData->hThread = NULL;
#elif defined _M_X64
      lpUserData->hThread = (HANDLE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rcx);
#endif
      if (lpUserData->hThread == NULL)
        return S_FALSE; //ignore call if handle is NULL
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      USERDATA_OnInternalNtResumeThread *lpUserData;
      TNktAutoDeletePtr<CCreatedProcessesItem> sCp;
      HANDLE hEvents[2];
      DWORD dwTid;
      HRESULT hRes;

      lpUserData = (USERDATA_OnInternalNtResumeThread*)lpIntercallCustomData;
#if defined _M_IX86
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax)) >= 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax)) >= 0) ? S_OK : S_FALSE;
#endif
      if (hRes == S_FALSE)
        return S_FALSE; //not found
      //get thread id
      dwTid = CNktDvTools::GetThreadId(lpUserData->hThread);
      //decrement suspend count
      {
        CNktAutoFastMutex cLock(this);
        TNktLnkLst<CCreatedProcessesItem>::Iterator it;
        CCreatedProcessesItem *lpCpItem;

        for (lpCpItem=it.Begin(cCreatedProcessesList); lpCpItem!=NULL; lpCpItem=it.Next())
        {
          if ((dwTid != 0) ? (dwTid == lpCpItem->sMsg.dwTid) : (lpCpItem->hThread == lpUserData->hThread))
          {
            NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnInternalNtSuspendThread]: Process %lu "
                              "(Thread %lu) suspend-sub (%lu)", ::GetTickCount(), lpCpItem->sMsg.dwPid,
                              lpCpItem->sMsg.dwTid, lpCpItem->nSuspendCount-1));
            if ((--(lpCpItem->nSuspendCount)) == 0)
            {
              cCreatedProcessesList.Remove(lpCpItem);
              sCp.Attach(lpCpItem);
            }
            break;
          }
        }
      }
      //send message to the server if suspend count is zero
      if (sCp != NULL)
      {
        //wait until initialization completes (don't worry about return code)
        hEvents[0] = sCp->hThread;
        hEvents[1] = sCp->hReadyEvent;
        ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
        //send message to the server
        if (nTransportActive != 0)
        {
          hRes = SendCreateProcessMsg(sCp);
          if (FAILED(hRes))
            return hRes; //error
        }
      }
      }
      break;
  }
  return S_OK; //continue
}

#if defined _M_IX86
static VOID Try_PreCall_UserData_OnInternalNtMapViewOfSection(__out HANDLE *lphProc,
                                                              __in CNktDvHookEngine::CALLINFO *lpCallInfo,
                                                              __in USERDATA_OnInternalNtMapViewOfSection *lpUserData)
{
  __try
  {
    *lphProc = *((HANDLE*)(lpCallInfo->lpCallState->sAsmRegisters.sInteger.Esp + 0x08));
    lpUserData->pBaseAddress = *((PVOID**)(lpCallInfo->lpCallState->sAsmRegisters.sInteger.Esp + 0x0C));
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    *lphProc = NULL;
    lpUserData->pBaseAddress = NULL;
  }
  return;
}
#endif

HRESULT CDvAgentMgr::OnInternalNtMapViewOfSection(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                                  __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      USERDATA_OnInternalNtMapViewOfSection *lpUserData;
      HANDLE hProc;

      lpUserData = (USERDATA_OnInternalNtMapViewOfSection*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of NtMapViewOfSection is the following:
      //  NTSTATUS NTAPI NtMapViewOfSection(__in HANDLE SectionHandle, __in HANDLE ProcessHandle,
      //                                    __inout PVOID *BaseAddress, __in ULONG_PTR ZeroBits, __in SIZE_T CommitSize,
      //                                    __inout_opt PLARGE_INTEGER SectionOffset, __inout PSIZE_T ViewSize,
      //                                    __in SECTION_INHERIT InheritDisposition, __in ULONG AllocationType,
      //                                    __in ULONG Protect)
#if defined _M_IX86
      Try_PreCall_UserData_OnInternalNtMapViewOfSection(&hProc, &sCallInfo, lpUserData);
#elif defined _M_X64
      hProc = (HANDLE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rdx);
      lpUserData->pBaseAddress = (PVOID*)(sCallInfo.lpCallState->sAsmRegisters.sInteger.R8);
#endif
      if (lpUserData->pBaseAddress == NULL || hProc != ::GetCurrentProcess())
        return S_FALSE; //ignore if not targetting us
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      NKT_DV_TMSG_COMMON sMsg;
      NKT_DV_IMAGE_NT_HEADER sNtHdr;
      HANDLE hWaitEvents[4];
      HINSTANCE hDll;
      HRESULT hRes;
      CNktEvent cProcessedEvent;
      USERDATA_OnInternalNtMapViewOfSection *lpUserData;

#if defined _M_IX86
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax)) >= 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax)) >= 0) ? S_OK : S_FALSE;
#endif
      if (hRes == S_FALSE)
        return S_FALSE; //not found
      lpUserData = (USERDATA_OnInternalNtMapViewOfSection*)lpIntercallCustomData;
      hDll = NULL;
      if (lpUserData->pBaseAddress != NULL)
      {
        if (SecureMemCopy(&hDll, lpUserData->pBaseAddress, sizeof(HINSTANCE)) == FALSE)
          hDll = NULL;
      }
      if (hDll != NULL)
      {
        if (FAILED(nktDvNtCheckImageType(&sNtHdr, ::GetCurrentProcess(), (LPBYTE)hDll)))
          return S_FALSE; //ignore call if not an executable mapping
      }
      CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), hDll);
      if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE ||
          nTransportActive == 0)
        return S_FALSE; //ignore call if connection was not established or loader lock active
      //send an invalidate cache command to the engine's agent controller
      if (CDvAgentMgrAutoSendDllNotificationAsync::DoAsyncNotify(&nSendLoadedUnloadedDllAsync) == FALSE)
      //if (CDvAgentMgrAutoTlsSendDllNotificationAsync::DoAsyncNotify() == FALSE)
      {
        //create wait event
        if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
          return E_OUTOFMEMORY; //error
        hWaitEvents[0] = cShutdownMtx.GetEvent();
        hWaitEvents[1] = hShutdownEvent;
        hWaitEvents[2] = hMainThread;
        hWaitEvents[3] = cProcessedEvent.GetEventHandle();
      }
      else
      {
        nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
      }
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_InvalidateAgentCtrlDllCache,
                                       cTransport.GetNextMsgId(), hWaitEvents[3], NULL);
      if (SUCCEEDED(hRes))
      {
        //send message
        hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
        if (SUCCEEDED(hRes) && hWaitEvents[0] != NULL)
        {
          //wait until message is processed
          switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
          {
            case WAIT_OBJECT_0:
            case WAIT_OBJECT_0+1:
            case WAIT_OBJECT_0+2:
              hRes = NKT_DVERR_Cancelled;
              break;
            case WAIT_OBJECT_0+3:
              break;
            default:
              hRes = E_FAIL; //wait failed (?)
              break;
          }
        }
      }
      //done
      if (hRes != S_OK)
        return hRes;
      }
      break;
  }
  return S_OK; //continue
}

#if defined _M_IX86
static VOID Try_PreCall_UserData_OnInternalNtUnmapViewOfSection(__out HANDLE *lphProc,
                                                  __in CNktDvHookEngine::CALLINFO *lpCallInfo,
                                                  __in USERDATA_OnInternalNtUnmapViewOfSection *lpUserData)
{
  __try
  {
    *lphProc = *((HANDLE*)(lpCallInfo->lpCallState->sAsmRegisters.sInteger.Esp + 0x04));
    lpUserData->BaseAddress = *((PVOID*)(lpCallInfo->lpCallState->sAsmRegisters.sInteger.Esp + 0x08));
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    *lphProc = NULL;
    lpUserData->BaseAddress = NULL;
  }
  return;
}
#endif

HRESULT CDvAgentMgr::OnInternalNtUnmapViewOfSection(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                                    __inout LPBYTE lpIntercallCustomData)
{
  switch (sCallInfo.nPhase)
  {
    case CNktDvHookEngine::phPreCall:
      {
      USERDATA_OnInternalNtUnmapViewOfSection *lpUserData;
      NKT_DV_IMAGE_NT_HEADER sNtHdr;
      HANDLE hProc;

      lpUserData = (USERDATA_OnInternalNtUnmapViewOfSection*)lpIntercallCustomData;
      //try to get needed parameters
      //declaration of NtUnmapViewOfSection is the following:
      //  NTSTATUS NTAPI NtUnmapViewOfSection(__in HANDLE ProcessHandle, __in PVOID BaseAddress);
#if defined _M_IX86
      Try_PreCall_UserData_OnInternalNtUnmapViewOfSection(&hProc, &sCallInfo, lpUserData);
#elif defined _M_X64
      hProc = (HANDLE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rcx);
      lpUserData->BaseAddress = (HINSTANCE)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rdx);
#endif
      if (lpUserData->BaseAddress == NULL || hProc != ::GetCurrentProcess())
        return S_FALSE; //ignore if not targetting us
      if (FAILED(nktDvNtCheckImageType(&sNtHdr, ::GetCurrentProcess(), (LPBYTE)(lpUserData->BaseAddress))))
        return S_FALSE; //ignore call if not an executable unmapping
      }
      break;

    case CNktDvHookEngine::phPostCall:
      {
      NKT_DV_TMSG_COMMON sMsg;
      HANDLE hWaitEvents[4];
      HRESULT hRes;
      CNktEvent cProcessedEvent;
      USERDATA_OnInternalNtUnmapViewOfSection *lpUserData;

#if defined _M_IX86
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Eax)) >= 0) ? S_OK : S_FALSE;
#elif defined _M_X64
      hRes = (((SSIZE_T)(sCallInfo.lpCallState->sAsmRegisters.sInteger.Rax)) >= 0) ? S_OK : S_FALSE;
#endif
      if (hRes == S_FALSE)
        return S_FALSE; //not found
      lpUserData = (USERDATA_OnInternalNtUnmapViewOfSection*)lpIntercallCustomData;
      CNktDvProcess::MarkModulesEnumeratorAsDirty(::GetCurrentProcessId(), (HINSTANCE)(lpUserData->BaseAddress));
      if (nktDvNtIsLoaderLoaderLockHeld(FALSE) != FALSE ||
          nTransportActive == 0)
        return S_FALSE; //ignore call if connection was not established or loader lock active
      //send an invalidate cache command to the engine's agent controller
      if (CDvAgentMgrAutoSendDllNotificationAsync::DoAsyncNotify(&nSendLoadedUnloadedDllAsync) == FALSE)
      //if (CDvAgentMgrAutoTlsSendDllNotificationAsync::DoAsyncNotify() == FALSE)
      {
        //create wait event
        if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
          return E_OUTOFMEMORY; //error
        hWaitEvents[0] = cShutdownMtx.GetEvent();
        hWaitEvents[1] = hShutdownEvent;
        hWaitEvents[2] = hMainThread;
        hWaitEvents[3] = cProcessedEvent.GetEventHandle();
      }
      else
      {
        nktMemSet(hWaitEvents, 0, sizeof(hWaitEvents));
      }
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_InvalidateAgentCtrlDllCache,
                                       cTransport.GetNextMsgId(), hWaitEvents[3], NULL);
      if (SUCCEEDED(hRes))
      {
        //send message
        hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
        if (SUCCEEDED(hRes) && hWaitEvents[0] != NULL)
        {
          //wait until message is processed
          switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
          {
            case WAIT_OBJECT_0:
            case WAIT_OBJECT_0+1:
            case WAIT_OBJECT_0+2:
              hRes = NKT_DVERR_Cancelled;
              break;
            case WAIT_OBJECT_0+3:
              break;
            default:
              hRes = E_FAIL; //wait failed (?)
              break;
          }
        }
      }
      //done
      if (hRes != S_OK)
        return hRes;
      }
      break;
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnUserHook(__inout CNktDvHookEngine::CALLINFO &sCallInfo,
                                __in CNktDvFunctionParamsCache::FUNCTION_PARAMS *lpFuncParams,
                                __inout CNktDvParamsEnumerator *lpCustomParamsEnum,
                                __inout LPBYTE lpIntercallCustomData)
{
  NKT_DV_TMSG_ONHOOKCALLED sMsg;
  HRESULT hRes;
  CNktEvent cProcessedEvent;
  HANDLE hWaitEvents[4];
  TNktAutoFreePtr<BYTE> cData;
  TNktComPtr<CNktDvParam> cParam;
  SIZE_T i, k, k2, nCount, nDataLen;
  LPBYTE d, d2;

  sMsg.dwHookId = sCallInfo.dwHookId;
  sMsg.bPreCall = (sCallInfo.nPhase == CNktDvHookEngine::phPreCall) ? TRUE : FALSE;
  sMsg.nCookie = sCallInfo.nCookie;
  sMsg.lpRemoteCurrCallState = NKT_PTR_2_ULONGLONG(sCallInfo.lpCallState);
  sMsg.lpRemoteIntercallCustomData = NKT_PTR_2_ULONGLONG(lpIntercallCustomData);
  sMsg.dwOsLastErrorCopy = sCallInfo.lpCallState->dwOsLastError;
  for (i=0; i<NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth; i++)
    sMsg.nStackTraceCopy[i] = sCallInfo.lpCallState->nStackTrace[i];
  sMsg.nCurrentTimeMs = sCallInfo.nCurrentTimeMs;
  sMsg.nElapsedTimeMs = sCallInfo.nElapsedTimeMs;
  sMsg.nKernelTimeMs = sCallInfo.nKernelTimeMs;
  sMsg.nUserTimeMs = sCallInfo.nUserTimeMs;
  sMsg.nCpuClockCycles = sCallInfo.nCpuClockCycles;
  sMsg.nChildsElapsedTimeMs = sCallInfo.nChildsElapsedTimeMs;
  sMsg.dwThreadId = sCallInfo.dwThreadId;
  sMsg.bASync = sCallInfo.bAsyncCallbacks;
  sMsg.dwChainDepth = sCallInfo.dwChainDepth;
  //count and process parameters
  sMsg.sParamsData.nParamsCount = 0;
  if (lpFuncParams != NULL && lpFuncParams->cParameters != NULL)
  {
    nCount = lpFuncParams->cParameters->GetCount();
    sMsg.sParamsData.nParamsCount = (ULONGLONG)nCount;
    if (nCount > NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams))
      nCount = NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams);
    for (i=0; i<nCount; i++)
    {
      cParam.Release();
      hRes = lpFuncParams->cParameters->GetAt(i, &cParam);
      if (FAILED(hRes))
        return hRes;
      sMsg.sParamsData.sParams[i].lpAddress = NKT_PTR_2_ULONGLONG(cParam->lpAddress);
      sMsg.sParamsData.sParams[i].nRegister = (int)(cParam->nInternalAsmRegisterIndex);
    }
  }
  //process result
  if (lpFuncParams != NULL && lpFuncParams->cResult != NULL)
  {
    sMsg.sParamsData.sResult.lpAddress = NKT_PTR_2_ULONGLONG(lpFuncParams->cResult->lpAddress);
    sMsg.sParamsData.sResult.nRegister = (int)(lpFuncParams->cResult->nInternalAsmRegisterIndex);
  }
  //process custom parameters and/or many function params
  sMsg.sParamsData.nCustomParamsCount =
       (lpCustomParamsEnum != NULL) ? (ULONGLONG)(lpCustomParamsEnum->GetCount()) : 0Ui64;
  if (sMsg.sParamsData.nCustomParamsCount > 0 ||
      sMsg.sParamsData.nParamsCount > NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams))
  {
    nCount = (sMsg.sParamsData.nParamsCount <= NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams)) ? 0 :
                  (SIZE_T)(sMsg.sParamsData.nParamsCount) - NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams);
    nDataLen = nCount * sizeof(NKT_DV_TMSG_ONHOOKCALLED_PARAM);
    nDataLen = NKT_DV_ALIGN_ULONGLONG(nDataLen);
    for (i=0; i<(SIZE_T)(sMsg.sParamsData.nCustomParamsCount); i++)
    {
      cParam.Release();
      hRes = lpCustomParamsEnum->GetAt(i, &cParam);
      if (FAILED(hRes))
        return hRes;
      k = 2 * sizeof(ULONGLONG) + //data length plus name length
          cParam->sCustomValue.cStrNameW.GetLength()*sizeof(WCHAR) +
          sizeof(int) + //fundamental type
          cParam->sCustomValue.nSize;
      nDataLen += NKT_DV_ALIGN_ULONGLONG(k);
    }
    //----
    cData.Attach(d = (LPBYTE)nktMemMalloc(nDataLen));
    if (cData == NULL)
      return E_OUTOFMEMORY;
    //----
    for (i=0; i<nCount; i++)
    {
      cParam.Release();
      hRes = lpFuncParams->cParameters->GetAt(i+NKT_DV_ARRAYLEN(sMsg.sParamsData.sParams), &cParam);
      if (FAILED(hRes))
        return hRes;
      ((NKT_DV_TMSG_ONHOOKCALLED_PARAM*)d)->lpAddress = NKT_PTR_2_ULONGLONG(cParam->lpAddress);
      ((NKT_DV_TMSG_ONHOOKCALLED_PARAM*)d)->nRegister = (int)(cParam->nInternalAsmRegisterIndex);
      d += sizeof(NKT_DV_TMSG_ONHOOKCALLED_PARAM);
    }
    d = (LPBYTE)NKT_DV_ALIGN_ULONGLONG(d);
    //----
    for (i=0; i<(SIZE_T)(sMsg.sParamsData.nCustomParamsCount); i++)
    {
      cParam.Release();
      hRes = lpCustomParamsEnum->GetAt(i, &cParam);
      if (FAILED(hRes))
        return hRes;
      //store value size
      *((ULONGLONG*)d) = (ULONGLONG)(cParam->sCustomValue.nSize);
      d2 = d + sizeof(ULONGLONG);
      //store name length
      k = cParam->sCustomValue.cStrNameW.GetLength() * sizeof(WCHAR);
      *((ULONGLONG*)d2) = (ULONGLONG)k;
      d2 += sizeof(ULONGLONG);
      //store fundamental type
      *((int*)d2) = (int)(cParam->sCustomValue.nFundType);
      d2 += sizeof(int);
      //store name
      if (k > 0)
      {
        nktMemCopy(d2, (LPWSTR)(cParam->sCustomValue.cStrNameW), k);
        d2 += k;
      }
      if ((k2 = cParam->sCustomValue.nSize) > 0)
      {
        nktMemCopy(d2, (k2 <= NKT_DV_ARRAYLEN(cParam->sCustomValue.aShortValue)) ?
                           cParam->sCustomValue.aShortValue : cParam->sCustomValue.cLongValue.Get(), k2);
      }
      //advance pointer
      d += NKT_DV_ALIGN_ULONGLONG(2*sizeof(ULONGLONG) + sizeof(int) + k + k2);
    }
    //----
    hRes = cTransport.SendBigPacketMsg(&(sMsg.sParamsData.dwCustomParamsBigPacketId), cData.Get(), nDataLen,
                                       sCallInfo.dwThreadId);
    if (FAILED(hRes))
      return hRes;
  }
  else
  {
    sMsg.sParamsData.dwCustomParamsBigPacketId = 0;
  }
  //create wait event
  hWaitEvents[0] = cShutdownMtx.GetEvent();
  hWaitEvents[1] = hShutdownEvent;
  hWaitEvents[2] = hMainThread;
  if (sMsg.bASync == FALSE)
  {
    if (cProcessedEvent.Create(TRUE, FALSE) == FALSE)
      return E_OUTOFMEMORY;
    hWaitEvents[3] = cProcessedEvent.GetEventHandle();
  }
  else
  {
    hWaitEvents[3] = NULL;
  }
  //prepare data
  hRes = NktDvTransportInitMessage(&sMsg, NKT_DV_TMSG_CODE_OnHookCalled, cTransport.GetNextMsgId(),
                                   hWaitEvents[3], NULL);
  if (FAILED(hRes))
    return hRes; //error
  //send message
  hRes = cTransport.SendMsg(&sMsg, sizeof(sMsg));
  if (FAILED(hRes))
    return hRes; //error
  //wait until message is processed
  if (sCallInfo.bAsyncCallbacks == FALSE)
  {
    switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
    {
      case WAIT_OBJECT_0:
      case WAIT_OBJECT_0+1:
      case WAIT_OBJECT_0+2:
        return NKT_DVERR_Cancelled;
      case WAIT_OBJECT_0+3:
        break;
      default:
        return E_FAIL; //wait failed (?)
    }
  }
  return S_OK; //continue
}

HRESULT CDvAgentMgr::OnEngMsg_LoadDll(__inout NKT_DV_TMSG_LOADDLL *lpMsg,
                                      __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvTransportBigData::CParser cDllNameBP(lpConnBigData, lpMsg->dwDllNameBigPacketId);
  USER_DLL sUserDll;
  CNktStringW cStrTempW;
  NKT_DV_TMSG_LOADDLL_CALLBACK sCallBackMsg;
  LPWSTR sW;
  SIZE_T i, nCount;
  HRESULT hRes;

  //build user dll data and add to list
  if (lpMsg->dwDllNameBigPacketId != 0)
  {
    sW = (LPWSTR)(cDllNameBP.GetData());
    i = cDllNameBP.GetLength() / sizeof(WCHAR);
    if (sW == NULL || i == 0)
      return E_FAIL;
    if (cStrTempW.CopyN(sW, i) == FALSE)
      return E_OUTOFMEMORY;
  }
  else
  {
    i = wcsnlen_s(lpMsg->szDllNameW, NKT_DV_ARRAYLEN(lpMsg->szDllNameW));
    if (cStrTempW.CopyN(lpMsg->szDllNameW, i) == FALSE)
      return E_OUTOFMEMORY;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [LoadDll]: %S", ::GetTickCount(), (LPWSTR)cStrTempW));
  sUserDll.nInternalId = NktInterlockedIncrement(&nLoadedDllsNextInternalId);
  sUserDll.szDllNameW = cStrTempW.Detach();
  sUserDll.bUnloadOnExit = lpMsg->bUnloadOnAgentShutdown;
  {
    CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
    //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

    sUserDll.hDll = ::LoadLibraryW(lpMsg->szDllNameW);
    if (sUserDll.hDll != NULL)
    {
      CNktAutoFastMutex cLock(this);

      if (aLoadedUserDllsList.AddElement(&sUserDll) == FALSE)
      {
        ::FreeLibrary(sUserDll.hDll);
        return E_OUTOFMEMORY;
      }
    }
  }
  //build and send callback message
  NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_LoadDllCallback, lpMsg);
  sCallBackMsg.hLoadDllRes = (sUserDll.hDll != NULL) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  hRes = cTransport.SendMsg(&sCallBackMsg, sizeof(sCallBackMsg));
  if (FAILED(hRes))
  {
    CNktAutoFastMutex cLock(this);

    //remove library on send error
    nCount = aLoadedUserDllsList.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (aLoadedUserDllsList[i].nInternalId == sUserDll.nInternalId)
      {
        aLoadedUserDllsList[i].bUnloadOnExit = TRUE;
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          aLoadedUserDllsList.RemoveElementAt(i);
        }
        break;
      }
    }
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [LoadDll]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_UnloadDll(__inout NKT_DV_TMSG_UNLOADDLL *lpMsg,
                                        __inout CNktDvTransportBigData *lpConnBigData)
{
  CNktDvTransportBigData::CParser cDllNameBP(lpConnBigData, lpMsg->dwDllNameBigPacketId);
  NKT_DV_TMSG_UNLOADDLL_CALLBACK sCallBackMsg;
  SIZE_T i, nCount, nDllLen;
  LPWSTR sW;
  HRESULT hRes;

  if (lpMsg->dwDllNameBigPacketId != 0)
  {
    sW = (LPWSTR)(cDllNameBP.GetData());
    nDllLen = cDllNameBP.GetLength() / sizeof(WCHAR);
    if (sW == NULL || nDllLen == 0)
      return E_FAIL;
  }
  else
  {
    sW = lpMsg->szDllNameW;
    nDllLen = wcsnlen_s(lpMsg->szDllNameW, NKT_DV_ARRAYLEN(lpMsg->szDllNameW));
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [UnloadDll]: %.*S", ::GetTickCount(), (int)nDllLen, sW));
  {
    CNktAutoFastMutex cLock(this);

    nCount = aLoadedUserDllsList.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (_wcsnicmp(aLoadedUserDllsList[i].szDllNameW, sW, nDllLen) == 0 &&
          aLoadedUserDllsList[i].szDllNameW[nDllLen] == 0)
      {
        aLoadedUserDllsList[i].bUnloadOnExit = TRUE;
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          aLoadedUserDllsList.RemoveElementAt(i);
        }
        break;
      }
    }
  }
  //build and send callback message
  NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_UnloadDllCallback, lpMsg);
  sCallBackMsg.hUnloadDllRes = (i < nCount) ? S_OK : NKT_DVERR_NotFound;
  hRes = cTransport.SendMsg(&sCallBackMsg, sizeof(sCallBackMsg));
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [UnloadDll]: Res=%08X", hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_CallCustomApi(__inout NKT_DV_TMSG_CALLCUSTOMAPI *lpMsg,
                                            __inout CNktDvTransportBigData *lpConnBigData)
{
  typedef HRESULT (WINAPI *lpfnUserFunc0)();
  typedef HRESULT (WINAPI *lpfnUserFunc1)(SIZE_T nParam1);
  typedef HRESULT (WINAPI *lpfnUserFunc2)(SIZE_T nParam1, SIZE_T nParam2);
  typedef HRESULT (WINAPI *lpfnUserFunc3)(SIZE_T nParam1, SIZE_T nParam2, SIZE_T nParam3);
  typedef HRESULT (WINAPI *lpfnUserFunc4)(SIZE_T nParam1, SIZE_T nParam2, SIZE_T nParam3, SIZE_T nParam4);
  CNktDvTransportBigData::CParser cDllNameBP(lpConnBigData, lpMsg->dwDllNameBigPacketId);
  HINSTANCE hDll;
  LPVOID fnUserFunc;
  NKT_DV_TMSG_CALLCUSTOMAPI_CALLBACK sCallBackMsg;
  SIZE_T i, nCount, nDllLen, nApiLen;
  LPWSTR sW;
  HRESULT hRes;

  if (lpMsg->dwDllNameBigPacketId != 0)
  {
    sW = (LPWSTR)(cDllNameBP.GetData());
    nDllLen = cDllNameBP.GetLength() / sizeof(WCHAR);
    if (sW == NULL || nDllLen == 0)
      return E_FAIL;
  }
  else
  {
    sW = lpMsg->szDllNameW;
    nDllLen = wcsnlen_s(lpMsg->szDllNameW, NKT_DV_ARRAYLEN(lpMsg->szDllNameW));
  }
  nApiLen = strnlen_s(lpMsg->szApiToCallA, NKT_DV_ARRAYLEN(lpMsg->szApiToCallA));
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [CallCustomApi]: %.*S(%.*s)", ::GetTickCount(),
                   (int)nDllLen, sW, (int)nApiLen, lpMsg->szApiToCallA));
  if (lpMsg->nParamsCount > 4)
    lpMsg->nParamsCount = 4;
  //build callback message
  NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_CallApiCallback, lpMsg);
  sCallBackMsg.bDllNotFound = sCallBackMsg.bApiNotFound = sCallBackMsg.bExceptionRaised = FALSE;
  sCallBackMsg.nResult = 0;
  //find dll and function
  hDll = NULL;
  fnUserFunc = NULL;
  {
    CNktAutoFastMutex cLock(this);

    nCount = aLoadedUserDllsList.GetCount();
    for (i=0; i<nCount; i++)
    {
      if (_wcsnicmp(aLoadedUserDllsList[i].szDllNameW, sW, nDllLen) == 0 &&
          aLoadedUserDllsList[i].szDllNameW[nDllLen] == 0)
        break;
    }
    if (i < nCount)
    {
      hDll = ::LoadLibraryW(aLoadedUserDllsList[i].szDllNameW); //temporary increment reference count
      fnUserFunc = ::GetProcAddress(aLoadedUserDllsList[i].hDll, lpMsg->szApiToCallA);
    }
    else
    {
      sCallBackMsg.bDllNotFound = TRUE;
    }
    if (fnUserFunc == NULL)
      sCallBackMsg.bApiNotFound = TRUE;
  }
  //call the api if found
  if (fnUserFunc != NULL)
  {
    try
    {
      switch (lpMsg->nParamsCount)
      {
        case 0:
          sCallBackMsg.nResult = (LONG)((lpfnUserFunc0)fnUserFunc)();
          break;
        case 1:
          sCallBackMsg.nResult = (LONG)((lpfnUserFunc1)fnUserFunc)((SIZE_T)(lpMsg->nParameters[0]));
          break;
        case 2:
          sCallBackMsg.nResult = (LONG)((lpfnUserFunc2)fnUserFunc)((SIZE_T)(lpMsg->nParameters[0]),
                                                                   (SIZE_T)(lpMsg->nParameters[1]));
          break;
        case 3:
          sCallBackMsg.nResult = (LONG)((lpfnUserFunc3)fnUserFunc)((SIZE_T)(lpMsg->nParameters[0]),
                                     (SIZE_T)(lpMsg->nParameters[1]), (SIZE_T)(lpMsg->nParameters[2]));
          break;
        case 4:
          sCallBackMsg.nResult = (LONG)((lpfnUserFunc4)fnUserFunc)((SIZE_T)(lpMsg->nParameters[0]),
                                     (SIZE_T)(lpMsg->nParameters[1]), (SIZE_T)(lpMsg->nParameters[2]),
                                     (SIZE_T)(lpMsg->nParameters[3]));
          break;
      }
    }
    catch (...)
    {
      sCallBackMsg.bExceptionRaised = TRUE;
      sCallBackMsg.nResult = E_FAIL;
    }
  }
  //remove refcount
  if (hDll != NULL)
    ::FreeLibrary(hDll);
  //send callback message
  hRes = cTransport.SendMsg(&sCallBackMsg, sizeof(sCallBackMsg));
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [CallCustomApi]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_AddHook(__inout NKT_DV_TMSG_ADDHOOK *lpMsg,
                                      __inout CNktDvTransportBigData *lpConnBigData)
{
  TNktAutoDeletePtr<CBatchHookExecData> cData;
  NKT_DV_TMSG_HOOKSTATECHANGE sMsgResp;
  SIZE_T nFuncLen;
  LPWSTR sW;
  HRESULT hRes;

  if (lpMsg->dwHookId == 0 || lpMsg->dwHookId >= 0x80000000 || lpMsg->lpAddress == NULL)
    return E_FAIL;
  //process data
  cData.Attach(NKT_MEMMGR_NEW CBatchHookExecData(CBatchHookExecData::typAddHook));
  if (cData == NULL)
    return E_OUTOFMEMORY;
  cData->cFunctionNameBP.Setup(lpConnBigData, lpMsg->dwFunctionNameBigPacketId);
  cData->cCustomHandlersBP.Setup(lpConnBigData, lpMsg->dwCustomHandlersBigPacketId);
  if (lpMsg->dwFunctionNameBigPacketId != 0)
  {
    sW = (LPWSTR)(cData->cFunctionNameBP.GetData());
    nFuncLen = cData->cFunctionNameBP.GetLength() / sizeof(WCHAR);
    if (sW == NULL || nFuncLen == 0)
      return E_FAIL;
  }
  else
  {
    sW = lpMsg->szFunctionNameW;
    nFuncLen = wcsnlen_s(lpMsg->szFunctionNameW, NKT_DV_ARRAYLEN(lpMsg->szFunctionNameW));
  }
  if (cData->cStrFuncW.CopyN(sW, nFuncLen) == FALSE)
    return E_OUTOFMEMORY;
  if (lpMsg->dwCustomHandlersBigPacketId != 0)
  {
    cData->sHookInfo.sCustomHandlers.lpData = cData->cCustomHandlersBP.GetData();
    cData->sHookInfo.sCustomHandlers.nDataSize = cData->cCustomHandlersBP.GetLength();
    if (cData->sHookInfo.sCustomHandlers.lpData == NULL ||
        cData->sHookInfo.sCustomHandlers.nDataSize == 0)
      return E_FAIL;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [AddHook]: Id:%lu / Batch:%lu / %S",
                    ::GetTickCount(), lpMsg->dwHookId, lpMsg->dwBatchId, (LPWSTR)(cData->cStrFuncW)));
  //do hooking
  cData->sHookInfo.dwSpyMgrHookId = lpMsg->dwSpyMgrHookId;
  cData->sHookInfo.lpProcToHook = (LPVOID)(lpMsg->lpAddress);
  cData->sHookInfo.szFunctionNameW = (LPWSTR)(cData->cStrFuncW);
  cData->sHookInfo.lpDbFunc = NULL;
  if (lpMsg->nDbFuncId != 0)
  {
    //find DbObject function
    cData->sHookInfo.lpDbFunc = cDvDB->FindObjectById(lpMsg->nDbFuncId);
    if (cData->sHookInfo.lpDbFunc == NULL)
      return NKT_DVERR_InvalidTransportData;
    switch (cData->sHookInfo.lpDbFunc->GetClass())
    {
      case CNktDvDbObjectNoRef::clsFunction:
      case CNktDvDbObjectNoRef::clsFunctionType:
      case CNktDvDbObjectNoRef::clsClassConstructor:
      case CNktDvDbObjectNoRef::clsClassDestructor:
      case CNktDvDbObjectNoRef::clsClassOperatorMethod:
      case CNktDvDbObjectNoRef::clsClassMethod:
      case CNktDvDbObjectNoRef::clsClassConverter:
        break;
      default:
        return NKT_DVERR_InvalidTransportData;
    }
  }
  cData->sHookInfo.nFlags = lpMsg->nFlags;
  cData->sHookInfo.dwHookId = lpMsg->dwHookId;
  cData->sHookInfo.dwSpyMgrHookId = lpMsg->dwSpyMgrHookId;
  cData->dwBatchId = lpMsg->dwBatchId;
  if (lpMsg->dwBatchId == 0)
  {
    {
      CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
      //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

      hRes = cHookEngine.Hook(&(cData->sHookInfo), 1, FALSE);
    }
    if (SUCCEEDED(hRes))
    {
      cHookEngine.EnableHook(lpMsg->dwHookId, TRUE);
      //prepare data
      hRes = NktDvTransportInitMessage(&sMsgResp, NKT_DV_TMSG_CODE_HookStateChange,
                                       cTransport.GetNextMsgId(), NULL, NULL);
      if (SUCCEEDED(hRes))
      {
        sMsgResp.dwHookId = lpMsg->dwHookId;
        sMsgResp.dwNewState = NKT_DV_TMSG_HOOKSTATECHANGE_Installed;
        hRes = cTransport.SendMsg(&sMsgResp, sizeof(sMsgResp));
      }
    }
  }
  else
  {
    {
      CNktAutoFastMutex cBatchHookExecListLock(&(sBatchHookExec.cMtx));

      sBatchHookExec.cList.PushTail(cData.Detach());
    }
    hRes = S_OK;
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [AddHook]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_RemoveHook(__inout NKT_DV_TMSG_REMOVEHOOK *lpMsg)
{
  TNktAutoDeletePtr<CBatchHookExecData> cData;
  HRESULT hRes;

  if (lpMsg->dwHookId == 0 || lpMsg->dwHookId >= 0x80000000)
    return E_FAIL;
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [RemoveHook]: Id:%lu / Batch:%lu", ::GetTickCount(),
                    lpMsg->dwHookId, lpMsg->dwBatchId));
  //process data
  if (lpMsg->dwBatchId == 0)
  {
    //do unhooking
    {
      CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
      //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

      hRes = cHookEngine.Unhook(&(lpMsg->dwHookId), 1);
    }
    hRes = S_OK;
  }
  else
  {
    cData.Attach(NKT_MEMMGR_NEW CBatchHookExecData(CBatchHookExecData::typRemoveHook));
    if (cData == NULL)
      return E_OUTOFMEMORY;
    cData->dwBatchId = lpMsg->dwBatchId;
    cData->dwHookId = lpMsg->dwHookId;
    {
      CNktAutoFastMutex cBatchHookExecListLock(&(sBatchHookExec.cMtx));

      sBatchHookExec.cList.PushTail(cData.Detach());
    }
    hRes = S_OK;
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [RemoveHook]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_EnableHook(__inout NKT_DV_TMSG_ENABLEHOOK *lpMsg)
{
  NKT_DV_TMSG_HOOKSTATECHANGE sMsgResp;
  HRESULT hRes;

  if (lpMsg->dwHookId == 0 || lpMsg->dwHookId >= 0x80000000)
    return E_FAIL;
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [EnableHook]: Id:%lu  (%s)", ::GetTickCount(),
                     lpMsg->dwHookId, (lpMsg->bEnable != FALSE) ? "Enable" : "Disable"));
  //do hook enable/disable
  hRes = cHookEngine.EnableHook(lpMsg->dwHookId, lpMsg->bEnable);
  //prepare data
  hRes = NktDvTransportInitMessage(&sMsgResp, NKT_DV_TMSG_CODE_HookStateChange, cTransport.GetNextMsgId(),
                                   NULL, NULL);
  if (SUCCEEDED(hRes))
  {
    sMsgResp.dwHookId = lpMsg->dwHookId;
    sMsgResp.dwNewState = (lpMsg->bEnable != FALSE) ? NKT_DV_TMSG_HOOKSTATECHANGE_Enabled :
                                                      NKT_DV_TMSG_HOOKSTATECHANGE_Disabled;
    hRes = cTransport.SendMsg(&sMsgResp, sizeof(sMsgResp));
  }
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [EnableHook]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
}

HRESULT CDvAgentMgr::OnEngMsg_BatchHookExec(__inout NKT_DV_TMSG_BATCHHOOKEXEC *lpMsg)
{
#define MAX_ENTRIES_PER_ROUND 64
  TNktLnkLst<CBatchHookExecData>::Iterator it;
  union {
    CNktDvHookEngine::HOOKINFO sHookInfos[MAX_ENTRIES_PER_ROUND];
    DWORD dwHookIds[MAX_ENTRIES_PER_ROUND];
  };
  CBatchHookExecData *lpHookExecData[MAX_ENTRIES_PER_ROUND], *lpHeData;
  CBatchHookExecData::eType nType;
  SIZE_T i, nEntriesCount;
  HRESULT hRes;
  CNktAutoFastMutex cBatchHookExecListLock(&(sBatchHookExec.cMtx));

  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [BatchHookExec]: BatchId:%lu", ::GetTickCount(),
                     lpMsg->dwBatchId));
  hRes = S_OK;
  nEntriesCount = 0;
  for (lpHeData=it.Begin(sBatchHookExec.cList); lpHeData!=NULL; lpHeData=it.Next())
  {
    if (lpHeData->dwBatchId != lpMsg->dwBatchId ||
        lpHeData->nType == CBatchHookExecData::typMarkedForDeletion)
      continue;
    //process this entry
    lpHookExecData[nEntriesCount] = lpHeData;
    if (nEntriesCount == 0 || lpHookExecData[nEntriesCount]->nType == lpHookExecData[0]->nType)
    {
      //queue this item
      if ((++nEntriesCount) < MAX_ENTRIES_PER_ROUND)
        continue; //look for more
    }
    //if we reached here, we should process this round
    nType = lpHookExecData[0]->nType;
    //mark for deletion
    for (i=0; i<nEntriesCount; i++)
      lpHookExecData[i]->nType = CBatchHookExecData::typMarkedForDeletion;
    //process
    switch (nType)
    {
      case CBatchHookExecData::typAddHook:
        for (i=0; i<nEntriesCount; i++)
          sHookInfos[i] = lpHookExecData[i]->sHookInfo;
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          hRes = cHookEngine.Hook(sHookInfos, nEntriesCount, FALSE);
        }
        if (SUCCEEDED(hRes))
        {
          for (i=0; i<nEntriesCount; i++)
            cHookEngine.EnableHook(sHookInfos[i].dwHookId, TRUE);
          //prepare data
          for (i=0; i<nEntriesCount && SUCCEEDED(hRes); i++)
          {
            NKT_DV_TMSG_HOOKSTATECHANGE sMsgResp;
            
            hRes = NktDvTransportInitMessage(&sMsgResp, NKT_DV_TMSG_CODE_HookStateChange,
                                             cTransport.GetNextMsgId(), NULL, NULL);
            if (SUCCEEDED(hRes))
            {
              sMsgResp.dwHookId = sHookInfos[i].dwHookId;
              sMsgResp.dwNewState = NKT_DV_TMSG_HOOKSTATECHANGE_Installed;
              hRes = cTransport.SendMsg(&sMsgResp, sizeof(sMsgResp));
            }
          }
        }
        break;

      case CBatchHookExecData::typRemoveHook:
        for (i=0; i<nEntriesCount; i++)
          dwHookIds[i] = lpHookExecData[i]->dwHookId;
        //do unhooking
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          hRes = cHookEngine.Unhook(dwHookIds, nEntriesCount);
        }
        hRes = S_OK;
        break;
    }
    if (FAILED(hRes))
      goto beh_end;
    nEntriesCount = 0;
  }
  if (nEntriesCount > 0)
  {
    nType = lpHookExecData[0]->nType;
    //mark for deletion
    for (i=0; i<nEntriesCount; i++)
      lpHookExecData[i]->nType = CBatchHookExecData::typMarkedForDeletion;
    //process remaining items in round
    switch (nType)
    {
      case CBatchHookExecData::typAddHook:
        for (i=0; i<nEntriesCount; i++)
          sHookInfos[i] = lpHookExecData[i]->sHookInfo;
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          hRes = cHookEngine.Hook(sHookInfos, nEntriesCount, FALSE);
        }
        if (SUCCEEDED(hRes))
        {
          for (i=0; i<nEntriesCount; i++)
            cHookEngine.EnableHook(sHookInfos[i].dwHookId, TRUE);
          //prepare data
          for (i=0; i<nEntriesCount && SUCCEEDED(hRes); i++)
          {
            NKT_DV_TMSG_HOOKSTATECHANGE sMsgResp;

            hRes = NktDvTransportInitMessage(&sMsgResp, NKT_DV_TMSG_CODE_HookStateChange,
                                             cTransport.GetNextMsgId(), NULL, NULL);
            if (SUCCEEDED(hRes))
            {
              sMsgResp.dwHookId = sHookInfos[i].dwHookId;
              sMsgResp.dwNewState = NKT_DV_TMSG_HOOKSTATECHANGE_Installed;
              hRes = cTransport.SendMsg(&sMsgResp, sizeof(sMsgResp));
            }
          }
        }
        break;

      case CBatchHookExecData::typRemoveHook:
        for (i=0; i<nEntriesCount; i++)
          dwHookIds[i] = lpHookExecData[i]->dwHookId;
        //do unhooking
        {
          CDvAgentMgrAutoSendDllNotificationAsync cAutoNotifyAsync(&nSendLoadedUnloadedDllAsync);
          //CDvAgentMgrAutoTlsSendDllNotificationAsync cAutoTlsNotifyAsync;

          hRes = cHookEngine.Unhook(dwHookIds, nEntriesCount);
        }
        hRes = S_OK;
        break;
    }
  }
  //remove used entries
  for (lpHeData=it.Begin(sBatchHookExec.cList); lpHeData!=NULL; lpHeData=it.Next())
  {
    if (lpHeData->nType == CBatchHookExecData::typMarkedForDeletion)
      delete lpHeData;
  }
beh_end:
  //done
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [BatchHookExec]: Res=%08X", ::GetTickCount(), hRes));
  return hRes;
#undef MAX_ENTRIES_PER_ROUND
}

HRESULT CDvAgentMgr::OnEngMsg_HelperCall(__inout NKT_DV_TMSG_HELPERCALL *lpMsg)
{
  NKT_DV_TMSG_HELPERCALL_CALLBACK sCallBackMsg;
  HRESULT hRes;

  switch (lpMsg->dwCallCode)
  {
    case NKT_DV_TMSG_CODE_HELPERCALL_GetWndProc:
      //build and send callback message
      NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_HelperCallCallback, lpMsg);
      sCallBackMsg.sGetWndProc.lpWndProc = NKT_PTR_2_ULONGLONG(::GetWindowLongPtr((HWND)(lpMsg->sGetWndProc.hWnd),
                                                                                  GWLP_WNDPROC));
      //send callback message
      hRes = cTransport.SendMsg(&sCallBackMsg, sizeof(sCallBackMsg));
      //done
      return hRes;

    case NKT_DV_TMSG_CODE_HELPERCALL_QueryHookOverwrite:
      //build and send callback message
      NktDvTransportInitMessage4Callback(&sCallBackMsg, NKT_DV_TMSG_CODE_HelperCallCallback, lpMsg);
      hRes = cHookEngine.QueryOverwrittenHooks((SIZE_T)(lpMsg->sQueryHookOverwrite.dwCount),
                                               lpMsg->sQueryHookOverwrite.aHookIdsList,
                                               sCallBackMsg.sQueryHookOverwrite.aOverwritten);
      //send callback message
      if (SUCCEEDED(hRes))
        hRes = cTransport.SendMsg(&sCallBackMsg, sizeof(sCallBackMsg));
      //done
      return hRes;
  }
  return E_INVALIDARG;
}

HRESULT CDvAgentMgr::SendCreateProcessMsg(__in CCreatedProcessesItem *lpCpItem)
{
  CNktEvent cProcessedEvent;
  HANDLE hWaitEvents[4];
  HRESULT hRes;

  NKT_ASSERT(lpCpItem != NULL);
  //create wait event
  hRes = (cProcessedEvent.Create(TRUE, FALSE) != FALSE) ? S_OK : E_OUTOFMEMORY;
  if (SUCCEEDED(hRes))
  {
    hWaitEvents[0] = cShutdownMtx.GetEvent();
    hWaitEvents[1] = hShutdownEvent;
    hWaitEvents[2] = hMainThread;
    hWaitEvents[3] = cProcessedEvent.GetEventHandle();
    //prepare data
    hRes = NktDvTransportInitMessage(&(lpCpItem->sMsg), NKT_DV_TMSG_CODE_OnCreateProcessCalled,
                                     cTransport.GetNextMsgId(), hWaitEvents[3], NULL);
  }
  if (SUCCEEDED(hRes))
  {
    //send message
    hRes = cTransport.SendMsg(&(lpCpItem->sMsg), sizeof(lpCpItem->sMsg));
  }
  if (SUCCEEDED(hRes))
  {
    //wait until message is processed
    switch (::WaitForMultipleObjects(4, hWaitEvents, FALSE, INFINITE))
    {
      case WAIT_OBJECT_0:
      case WAIT_OBJECT_0+1:
      case WAIT_OBJECT_0+2:
        hRes = NKT_DVERR_Cancelled;
        break;
      case WAIT_OBJECT_0+3:
        break;
      default:
        hRes = E_FAIL; //wait failed (?)
        break;
    }
  }
  return hRes;
}

HRESULT CDvAgentMgr::CheckOverwrittenHooks()
{
  HRESULT hRes;

  if ((dwAgentInitFlags & NKT_DV_AGENTINITDATA_FLAG_InformHookOverrides) == 0)
    return S_OK;
  if (NktInterlockedBitTestAndSet(&nDelayedCheckOverrides, 1) != FALSE)
    return S_OK; //avoid recursion
  //not previously set
  NktInterlockedBitReset(&nDelayedCheckOverrides, 0); //unmark check
  hRes = cHookEngine.CheckOverwrittenHooks();
  NktInterlockedBitReset(&nDelayedCheckOverrides, 1);
  return hRes;
}

HRESULT CDvAgentMgr::CheckDllModificationsWithImageFile(__in HINSTANCE hDll, __inout TNktArrayList<BYTE, 16384> &cBuf)
{
  CNktStringW cStrDllNameW;
  HANDLE hFile;
  HRESULT hRes;

  hRes = CNktDvTools::GetModuleFullName(cStrDllNameW, hDll);
  if (SUCCEEDED(hRes))
  {
    hFile = ::CreateFileW((LPWSTR)cStrDllNameW, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_DELETE, NULL,
                          OPEN_EXISTING, 0, NULL);
    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
      hRes = InternalCheckDllModificationsWithImageFile(hDll, hFile, cBuf);
      ::CloseHandle(hFile);
    }
    else
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
    }
  }
  return hRes;
}

static HRESULT InternalCheckDllModificationsWithImageFile(__in HINSTANCE hDll, __in HANDLE hFile,
                                                          __inout TNktArrayList<BYTE, 16384> &cBuf)
{
  HANDLE hFileMap;
  LPBYTE lpData;
  HRESULT hRes;

  hFileMap = ::CreateFileMappingW(hFile, NULL, PAGE_WRITECOPY, 0, 0, NULL);
  hRes = (hFileMap != NULL) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  if (SUCCEEDED(hRes))
  {
    lpData = (LPBYTE)::MapViewOfFile(hFileMap, FILE_MAP_COPY, 0, 0, 0);
    if (lpData != NULL)
    {
      NKT_DV_IMAGE_NT_HEADER sMemNtHdr, sFileNtHdr;
      LPBYTE lpMemNtHdrAddr, lpFileNtHdrAddr, lpRelStart, lpDest;
      //LPBYTE lpMemSecStart, lpFileSecStart;
      SIZE_T nSecCount, nRelocDelta;
      //SIZE_T i, k, nMismatchStart, nMemSecSize;
      IMAGE_SECTION_HEADER *lpMemImgSect, *lpFileImgSect;
      IMAGE_DATA_DIRECTORY *lpFileBaseRelocDir, *lpFileExportsDir;
      PIMAGE_BASE_RELOCATION lpRel;
      DWORD dwCount, dwRawAddr, *lpdwPatchAddrHL;
#if defined _M_X64
      ULONGLONG *lpnPatchAddr64;
#endif //_M_X64
      USHORT nOffset, *lpRelInfo;

      __try
      {
        hRes = nktDvNtCheckImageType(&sMemNtHdr, ::GetCurrentProcess(), (LPBYTE)hDll, &lpMemNtHdrAddr);
        if (SUCCEEDED(hRes))
        {
          switch (nktDvNtCheckImageType(&sFileNtHdr, ::GetCurrentProcess(), lpData, &lpFileNtHdrAddr))
          {
#if defined _M_IX86
            case 32:
              if (hRes == 32)
              {
                nSecCount = (SIZE_T)(sMemNtHdr.u32.FileHeader.NumberOfSections);
                if (nSecCount == (SIZE_T)(sMemNtHdr.u32.FileHeader.NumberOfSections))
                {
                  lpMemImgSect = (IMAGE_SECTION_HEADER*)(lpMemNtHdrAddr + sizeof(sMemNtHdr.u32));
                  lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpFileNtHdrAddr + sizeof(sFileNtHdr.u32));
                  lpFileBaseRelocDir = &(sFileNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
                  lpFileExportsDir = &(sFileNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
                  nRelocDelta = (SIZE_T)hDll - (SIZE_T)(sFileNtHdr.u32.OptionalHeader.ImageBase);
                  hRes = S_OK;
                }
                else
                {
                  hRes = E_FAIL;
                }
              }
              else
              {
                hRes = E_FAIL;
              }
              break;
#elif defined _M_X64
            case 64:
              if (hRes == 64)
              {
                nSecCount = (SIZE_T)(sMemNtHdr.u64.FileHeader.NumberOfSections);
                if (nSecCount == (SIZE_T)(sMemNtHdr.u64.FileHeader.NumberOfSections))
                {
                  lpMemImgSect = (IMAGE_SECTION_HEADER*)(lpMemNtHdrAddr + sizeof(sMemNtHdr.u64));
                  lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpFileNtHdrAddr + sizeof(sFileNtHdr.u64));
                  lpFileBaseRelocDir = &(sFileNtHdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
                  lpFileExportsDir = &(sFileNtHdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
                  nRelocDelta = (SIZE_T)hDll - (SIZE_T)(sFileNtHdr.u64.OptionalHeader.ImageBase);
                  hRes = S_OK;
                }
                else
                {
                  hRes = E_FAIL;
                }
              }
              else
              {
                hRes = E_FAIL;
              }
              break;
#endif
            default:
              hRes = E_FAIL;
              break;
          }
        }
        //apply relocations to the file image
        if (SUCCEEDED(hRes) && nRelocDelta != 0 && lpFileBaseRelocDir->Size > 0)
        {
          dwRawAddr = HelperConvertVAToRaw(lpFileBaseRelocDir->VirtualAddress, lpFileImgSect, nSecCount);
          if (dwRawAddr != 0)
          {
            lpRelStart = lpData + (SIZE_T)dwRawAddr;
            lpRel = (PIMAGE_BASE_RELOCATION)lpRelStart;
            while ((LPBYTE)lpRel < lpRelStart + (SIZE_T)(lpFileBaseRelocDir->Size) &&
                   lpRel->VirtualAddress > 0)
            {
              dwCount = (lpRel->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;
              dwRawAddr = HelperConvertVAToRaw(lpRel->VirtualAddress, lpFileImgSect, nSecCount);
              if (dwRawAddr == 0)
              {
                hRes = E_FAIL;
                break;
              }
              lpDest = lpData + (SIZE_T)dwRawAddr;
              lpRelInfo = (USHORT*)(lpRel+1);
              while (dwCount > 0)
              {
                //the lower 12 bits define the offset
                nOffset = (*lpRelInfo) & 0xFFF;
                //the upper 4 bits define the type of relocation
                switch ((*lpRelInfo) >> 12)
                {
                  case IMAGE_REL_BASED_ABSOLUTE:
                    break;
                  case IMAGE_REL_BASED_HIGHLOW:
                    // change complete 32 bit address
                    lpdwPatchAddrHL = (DWORD*)(lpDest + (SIZE_T)nOffset);
                    *lpdwPatchAddrHL += (DWORD)nRelocDelta;
                    break;
#if defined _M_X64
                  case IMAGE_REL_BASED_DIR64:
                    lpnPatchAddr64 = (ULONGLONG*)(lpDest + (SIZE_T)nOffset);
                    *lpnPatchAddr64 += (ULONGLONG)nRelocDelta;
                    break;
#endif //_M_X64
                  default:
                    //unknown relocation
                    break;
                }
                //next
                dwCount--;
                lpRelInfo++;
              }
              //advance to next relocation block
              lpRel = (PIMAGE_BASE_RELOCATION)((LPBYTE)lpRel + (SIZE_T)(lpRel->SizeOfBlock));
            }
          }
          else
          {
            hRes = E_FAIL;
          }
        }
        //check exports
        if (SUCCEEDED(hRes) && lpFileExportsDir->VirtualAddress != 0 && lpFileExportsDir->Size > 0)
        {
          IMAGE_EXPORT_DIRECTORY *lpExpDir;
          DWORD i, j, *lpdwAddressOfFunctions, *lpdwAddressOfNames;
          LPBYTE lpMemFuncAddr, lpFileFuncAddr;
          WORD *lpwAddressOfNameOrdinals;
          LPSTR sA;

          dwRawAddr = HelperConvertVAToRaw(lpFileExportsDir->VirtualAddress, lpFileImgSect, nSecCount);
          if (dwRawAddr != 0)
          {
            lpExpDir = (IMAGE_EXPORT_DIRECTORY*)(lpData + (SIZE_T)dwRawAddr);
            dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfFunctions, lpFileImgSect, nSecCount);
          }
          if (dwRawAddr != 0)
          {
            lpdwAddressOfFunctions = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
            dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNames, lpFileImgSect, nSecCount);
          }
          if (dwRawAddr != 0)
          {
            lpdwAddressOfNames = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
            dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNameOrdinals, lpFileImgSect, nSecCount);
          }
          if (dwRawAddr != 0)
          {
            lpwAddressOfNameOrdinals = (LPWORD)(lpData + (SIZE_T)dwRawAddr);
            for (i=0; i<lpExpDir->NumberOfFunctions; i++)
            {
              dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfFunctions[i], lpFileImgSect, nSecCount);
              if (dwRawAddr == 0)
                break;
              lpFileFuncAddr = lpData + (SIZE_T)dwRawAddr;
              if (lpFileFuncAddr >= (LPBYTE)lpExpDir &&
                  lpFileFuncAddr < (LPBYTE)lpExpDir+(SIZE_T)(lpFileExportsDir->Size))
              {
                //a forwarded function => ignore
                continue;
              }
              lpMemFuncAddr = (LPBYTE)hDll + (SIZE_T)lpdwAddressOfFunctions[i];
              if (SecureMemIsDiff(lpMemFuncAddr, lpFileFuncAddr, 5) != FALSE)
              {
                for (j=0; j < lpExpDir->NumberOfNames && (WORD)i != lpwAddressOfNameOrdinals[j]; j++);
                if (j < lpExpDir->NumberOfNames)
                {
                  dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfNames[j], lpFileImgSect, nSecCount);
                  if (dwRawAddr == 0)
                    break;
                  sA = (LPSTR)(lpData + dwRawAddr);
                }
                else
                {
                  sA = NULL;
                }
                j = 0;
              }
            }
            if (i < lpExpDir->NumberOfFunctions)
              hRes = E_FAIL;
          }
          else
          {
            hRes = E_FAIL;
          }
          /*
          for (i=0; i<nSecCount && SUCCEEDED(hRes); i++)
          {
            if ((lpFileImgSect[i].Characteristics & IMAGE_SCN_CNT_CODE) !=
                (lpMemImgSect[i].Characteristics & IMAGE_SCN_CNT_CODE))
            {
              hRes = E_FAIL; //section mismatch?
              continue;
            }
            //ignore non-code sections
            if ((lpMemImgSect[i].Characteristics & IMAGE_SCN_CNT_CODE) == 0)
              continue;
            lpMemSecStart = (LPBYTE)hDll + (SIZE_T)(lpMemImgSect[i].VirtualAddress);
            lpFileSecStart = lpData + (SIZE_T)(lpFileImgSect[i].PointerToRawData);
            nMemSecSize = (SIZE_T)(lpMemImgSect[i].SizeOfRawData);
            if (nMemSecSize > (SIZE_T)(lpFileImgSect[i].Misc.VirtualSize))
              nMemSecSize = (SIZE_T)(lpFileImgSect[i].Misc.VirtualSize);
            //compare sections
            k = 0;
            while (k < nMemSecSize)
            {
              if (lpMemSecStart[k] != lpFileSecStart[k])
              {
                nMismatchStart = k;
                while (k < nMemSecSize && lpMemSecStart[k] != lpFileSecStart[k])
                  k++;
              }
              else
              {
                k++;
              }
            }
          }
          */
        }
      }
      __except (EXCEPTION_EXECUTE_HANDLER)
      {
        hRes = E_FAIL;
      }
      ::UnmapViewOfFile(lpData);
    }
    ::CloseHandle(hFileMap);
  }
  return hRes;
}

//-----------------------------------------------------------

static int ThreadIgnore_Compare(__in void *, __in LPDWORD lpElem1, __in LPDWORD lpElem2)
{
  return NKT_DV_CMP<DWORD>(*lpElem1, *lpElem2);
}

static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount)
{
  SIZE_T i;

  for (i=0; i<nSecCount; i++)
  {
    if (dwVirtAddr >= lpImgSect[i].VirtualAddress &&
        dwVirtAddr < lpImgSect[i].VirtualAddress + lpImgSect[i].Misc.VirtualSize)
      return dwVirtAddr - lpImgSect[i].VirtualAddress + lpImgSect[i].PointerToRawData;
  }
  return 0;
}

static BOOL SecureMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount)
{
  BOOL b;

  __try
  {
    nktMemCopy(lpDest, lpSrc, nCount);
    b = TRUE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

static BOOL SecureMemIsDiff(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount)
{
  BOOL b = FALSE;

  __try
  {
    b = (nktMemCompare(lpBuf1, lpBuf2, nCount) != 0) ? TRUE : FALSE;
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    b = FALSE;
  }
  return b;
}

static BOOL OnLdrLoadDll_IsDll(__in PUNICODE_STRING lpDllNameUS, __in LPCWSTR szDllToCheckW)
{
  BOOL b;
  SIZE_T nStartPos, nLen, nToCheckLen;

  b = FALSE;
  if (lpDllNameUS != NULL)
  {
    nToCheckLen = wcslen(szDllToCheckW);
    __try
    {
      if (lpDllNameUS->Buffer != NULL)
      {
        nStartPos = nLen = (SIZE_T)(lpDllNameUS->Length) / sizeof(WCHAR);
        while (nStartPos > 0 && lpDllNameUS->Buffer[nStartPos-1] != L'/' && lpDllNameUS->Buffer[nStartPos-1] != L'\\')
          nStartPos--;
        if (nLen-nStartPos == nToCheckLen &&
            _wcsnicmp(lpDllNameUS->Buffer+nStartPos, szDllToCheckW, nToCheckLen) == 0)
          b = TRUE;
      }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    { }
  }
  return b;
}
