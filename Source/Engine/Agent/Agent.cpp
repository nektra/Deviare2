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
#include "..\..\Common\Tools.h"
#include "HookCustomHandlerMgr.h"
#include "HookEngineMiscHelpers.h"
#include "..\..\Common\RegistrySettings.h"
#include <tlhelp32.h>
#include "DetectApp.h"
#include <VersionHelpers.h>

#ifdef _DEBUG
  #define ATTACH_2_DEBUGGER_AT_STARTUP
#else //_DEBUG
  //#define ATTACH_2_DEBUGGER_AT_STARTUP
#endif //_DEBUG

#if defined _M_IX86
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Externals\\DeviareInProc\\lib\\2017\\NktHookLib_Debug.lib")
#else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib.lib")
    #pragma comment(lib, "..\\..\\..\\Externals\\DeviareInProc\\lib\\2017\\NktHookLib.lib")
  #endif //_DEBUG
#elif defined _M_X64
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Externals\\DeviareInProc\\lib\\2017\\NktHookLib64_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64.lib")
    #pragma comment(lib, "..\\..\\..\\Externals\\DeviareInProc\\lib\\2017\\NktHookLib64.lib")
  #endif //_DEBUG
#else
  #error Unsupported platform
#endif

//-----------------------------------------------------------

typedef struct {
  HANDLE hInitCompletedEvent;
  HRESULT volatile hRes;
} MAINTHREAD_PARAMS;

//-----------------------------------------------------------

typedef VOID (WINAPI *lpfnExitProcess)(__in UINT uExitCode);
typedef VOID (WINAPI *lpfnCorExitProcess)(__in int uExitCode);
typedef BOOL (WINAPI *lpfnTerminateProcess)(__in HANDLE hProcess, __in UINT uExitCode);
typedef HRESULT (WINAPI *lpfnCoInitializeSecurity)(__in_opt PSECURITY_DESCRIPTOR pSecDesc, __in LONG cAuthSvc,
                                         __in SOLE_AUTHENTICATION_SERVICE *asAuthSvc, __in_opt LPVOID pReserved1,
                                         __in DWORD dwAuthnLevel, __in DWORD dwImpLevel, __in_opt LPVOID pAuthList,
                                         __in DWORD dwCapabilities, __in_opt LPVOID pReserved3);
#if defined _M_X64
typedef BOOLEAN (NTAPI *lpfnRtlInstallFunctionTableCallback)(__in DWORD64 TableIdentifier, __in DWORD64 BaseAddress,
                                                   __in DWORD Length, __in PGET_RUNTIME_FUNCTION_CALLBACK Callback,
                                                   __in PVOID Context, __in PCWSTR OutOfProcessCallbackDll);
#endif //_M_X64

//-----------------------------------------------------------

static CNktEvent cShutdownEvent;
static CDvAgentMgr cAgentMgr;
static HANDLE hMainThread = NULL;
static DWORD dwMainThreadId = 0;
static LONG volatile nManualShutdown = 0;
static LONG volatile nMiniHooksLock = 0;
#if defined _M_IX86
static CNktHookLib::HOOK_INFO sMiniHooks[4] = { 0 };
#elif defined _M_X64
static CNktHookLib::HOOK_INFO sMiniHooks[5] = { 0 };
#endif
#define MINIHOOK_ExitProcess          0
#define MINIHOOK_CorExitProcess       1
#define MINIHOOK_TerminateProcess     2
#define MINIHOOK_CoInitializeSecurity 3
#if defined _M_X64
  #define MINIHOOK_RtlInstallFunctionTableCallback 4
#endif //_M_X64
BOOL bAppIsExiting = FALSE;
HINSTANCE hAgentDllInstance = NULL;

//-----------------------------------------------------------

static DWORD WINAPI MainThreadProc(__inout LPVOID lpParam);
static VOID Finalize(__in BOOL bFinishMainThread);
static VOID WINAPI OnExitProcessCalled(__in UINT uExitCode);
static VOID WINAPI OnCorExitProcessCalled(__in int uExitCode);
static BOOL WINAPI OnTerminateProcessCalled(__in HANDLE hProcess, __in UINT uExitCode);
static HRESULT WINAPI OnCoInitializeSecurityCalled(__in_opt PSECURITY_DESCRIPTOR pSecDesc, __in LONG cAuthSvc,
                                            __in SOLE_AUTHENTICATION_SERVICE *asAuthSvc, __in_opt LPVOID pReserved1,
                                            __in DWORD dwAuthnLevel, __in DWORD dwImpLevel, __in_opt LPVOID pAuthList,
                                            __in DWORD dwCapabilities, __in_opt LPVOID pReserved3);
#if defined _M_X64
static BOOLEAN NTAPI OnRtlInstallFunctionTableCallbackCalled(__in DWORD64 TableIdentifier, __in DWORD64 BaseAddress,
                                                      __in DWORD Length, __in PGET_RUNTIME_FUNCTION_CALLBACK Callback,
                                                      __in PVOID Context, __in PCWSTR OutOfProcessCallbackDll);
#endif //_M_X64
static VOID ShutdownMainThread();
static VOID EnsureNoThreadsInModule();
#ifdef ATTACH_2_DEBUGGER_AT_STARTUP
static BOOL AttachCurrentProcessToDebugger();
#endif //ATTACH_2_DEBUGGER_AT_STARTUP

//-----------------------------------------------------------

extern "C" {

BOOL APIENTRY DllMain(__in HMODULE hModule, __in DWORD ulReasonForCall, __in LPVOID lpReserved)
{
  switch (ulReasonForCall)
  {
    case DLL_PROCESS_ATTACH:
      cAgentMgr.hAgentInst = hAgentDllInstance = (HINSTANCE)hModule;
      break;

    case DLL_PROCESS_DETACH:
      ::DisableThreadLibraryCalls(hModule);
      if (lpReserved == NULL)
      {
        //NULL=FreeLibrary was called / NON-NULL=process terminating
        ::CloseHandle(hMainThread);
        hMainThread = NULL;
      }
      else
      {
        bAppIsExiting = TRUE;
        cShutdownEvent.Set();
      }
      Finalize(FALSE);
      nktDvTlsData_OnThreadExit();
      break;

    case DLL_THREAD_DETACH:
      nktDvTlsData_OnThreadExit();
      nktMemMgrOnThreadExit();
      break;
  }
  return TRUE;
}

__declspec(dllexport) HRESULT APIENTRY Initialize(__in LPNKT_DV_AGENTINITDATA lpInitData)
{
  CNktAutoFastMutex cAgentLock(&cAgentMgr);
  CNktEvent cInitCompletedEvent;
  WCHAR szEventNameW[128];
  MAINTHREAD_PARAMS sThreadParams;
  DWORD dwMainThreadId;
  HANDLE hEvents[2];

  if (!IsWindowsXPOrGreater())
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[GetVersion]: hRes=%08X", ::GetTickCount(), E_NOTIMPL));
    return E_NOTIMPL; //reject pre-XP
  }
  //Nektra::DebugBreak();
#ifdef ATTACH_2_DEBUGGER_AT_STARTUP
  AttachCurrentProcessToDebugger();
#endif //ATTACH_2_DEBUGGER_AT_STARTUP

  //NOTE: DO NOT call OutputDebugStringX inside this function and its children. They raises an
  //      unhandled exception in x64. I believe it is related to the dynamic code generated
  //      when creating the remote thread because the process PE file doesn't have the needed
  //      UNWIND information. (May the .xdata and .pdata section dynamically changed?)
  if (lpInitData == NULL ||
      (lpInitData->nServerPlatformBits != 32 && lpInitData->nServerPlatformBits != 64) ||
      lpInitData->dwServerProcessId == 0 || lpInitData->dwServerProcessId == ::GetCurrentProcessId() ||
      lpInitData->hServerProcDupHandle == NULL || lpInitData->hDbFileMapDupHandle == NULL)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[InitData]: hRes=%08X",
                       ::GetTickCount(), E_FAIL));
    return E_FAIL;
  }
  if (lpInitData->nProtocolVersion != NKT_DV_TRANSPORT_PROTOCOL_VERSION)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[InitData]: hRes=%08X",
                       ::GetTickCount(), NKT_DVERR_ProtocolVersionMismatch));
    return NKT_DVERR_ProtocolVersionMismatch;
  }
  if (hMainThread != NULL)
    return S_OK; //already initialized
  //overrides
  if (lpInitData->sSettingsOverride.nValidFields != 0)
  {
    LPNKT_DV_AGENTSETTINGOVERRIDE lpSet = &(lpInitData->sSettingsOverride);

    if ((lpSet->nValidFields & 0x01) != 0)
      CNktDvRegistrySettings::SetDebugLevelMask(lpSet->dwDebugFlags);
    if ((lpSet->nValidFields & 0x02) != 0)
      CNktDvRegistrySettings::SetMemMgrPoolMaxAge(lpSet->dwMemMgrPoolMaxAge);
    if ((lpSet->nValidFields & 0x04) != 0)
      CNktDvRegistrySettings::SetMemMgrPoolCheckFrequency(lpSet->dwMemMgrPoolCheckFrequency);
    if ((lpSet->nValidFields & 0x08) != 0)
      CNktDvRegistrySettings::SetTransportMaxFreeBufferList(lpSet->dwTransportMaxFreeBufferList);
    if ((lpSet->nValidFields & 0x10) != 0)
      CNktDvRegistrySettings::SetAgentTransportMaxInUseMessageCount(lpSet->nAgentTransportMaxInUseMessageCount);
    if ((lpSet->nValidFields & 0x20) != 0)
      CNktDvRegistrySettings::SetAgentTransportMessageDelay(lpSet->dwAgentTransportMessageDelay);
  }
  //create initialization complete event
  if (cInitCompletedEvent.Create(TRUE, FALSE) == FALSE)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[CreateEvent1]: hRes=%08X",
                       ::GetTickCount(), E_OUTOFMEMORY));
    return E_OUTOFMEMORY;
  }
  //create shutdown event
  swprintf_s(szEventNameW, NKT_DV_ARRAYLEN(szEventNameW), L"RemoteBridgeAgentShutdown_%08X",
             ::GetCurrentProcessId());
  if (cShutdownEvent.GetEventHandle() == NULL)
  {
    if (cShutdownEvent.Create(TRUE, FALSE, szEventNameW) == FALSE)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[CreateEvent2]: hRes=%08X",
                         ::GetTickCount(), E_OUTOFMEMORY));
      return E_OUTOFMEMORY;
    }
  }
  else
  {
    cShutdownEvent.Reset();
  }
  //----
  cAgentMgr.Finalize();
  //create main running thread suspended
  hMainThread = ::CreateThread(NULL, 0, MainThreadProc, &sThreadParams, CREATE_SUSPENDED, &dwMainThreadId);
  if (hMainThread == NULL)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[CreateThread]: hRes=%08X",
                       ::GetTickCount(), E_OUTOFMEMORY));
    return E_OUTOFMEMORY;
  }
  //init agent engine
  hEvents[0] = hMainThread;
  hEvents[1] = sThreadParams.hInitCompletedEvent = cInitCompletedEvent.GetEventHandle();
  sThreadParams.hRes = cAgentMgr.Initialize(lpInitData, cShutdownEvent.GetEventHandle(), hMainThread);
  if (FAILED(sThreadParams.hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[MgrInitialize]: hRes=%08X",
                       ::GetTickCount(), sThreadParams.hRes));
    ::CloseHandle((HANDLE)(lpInitData->hServerProcDupHandle));
    ::TerminateThread(hMainThread, 0);
    ::CloseHandle(hMainThread);
    return sThreadParams.hRes;
  }
  //resume thread and wait for initialization phase 2
  ::ResumeThread(hMainThread);
  CNktThread::SetThreadName(dwMainThreadId, "Dv-AgentMain");
  switch (::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE))
  {
    case WAIT_OBJECT_0:
      if (SUCCEEDED(sThreadParams.hRes))
        sThreadParams.hRes = NKT_DVERR_Cancelled;
      NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[WaitForMultipleObjects]: hRes=%08X",
                         ::GetTickCount(), sThreadParams.hRes));
      hMainThread = NULL;
      ::CloseHandle(hMainThread);
      cAgentMgr.Finalize();
      return sThreadParams.hRes;

    case WAIT_OBJECT_0+1:
      break;

    default:
      NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentInitialize[WaitForMultipleObjects]: hRes=%08X",
                         ::GetTickCount(), E_FAIL));
      ::TerminateThread(hMainThread, 0);
      ::CloseHandle(hMainThread);
      hMainThread = NULL;
      cAgentMgr.Finalize();
      return E_FAIL;
  }
  return S_OK;
}

}; //extern "C"

//-----------------------------------------------------------

static DWORD WINAPI MainThreadProc(__inout LPVOID lpParam)
{
  MAINTHREAD_PARAMS *lpThreadParams = (MAINTHREAD_PARAMS*)lpParam;
  BOOL bExit;
  HRESULT hRes; //local copy to avoid race conditions when SetEvent is called

  //setup min hooks
  hRes = InstallMiniHooks();
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMainThreadProc[InstallMiniHooks]: hRes=%08X",
                    ::GetTickCount(), hRes));
  //because in XP all I/O operation is cancelled when the thread exits, i moved the transport server initialization
  //here (in Vista/7 I/O associated with a completion port is not cancelled)
  if (SUCCEEDED(hRes))
    hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (SUCCEEDED(hRes))
  {
    hRes = cAgentMgr.StartServer(); //no need to use a lock here
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMainThreadProc[StartServer]: hRes=%08X",
                       ::GetTickCount(), hRes));
  }
  if (SUCCEEDED(hRes))
  {
    lpThreadParams->hRes = hRes;
    if (::SetEvent(lpThreadParams->hInitCompletedEvent) == FALSE)
    {
      NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) AgentMainThreadProc[SetEvent]: hRes=%08X",
                         ::GetTickCount(), E_OUTOFMEMORY));
      lpThreadParams->hRes = E_OUTOFMEMORY;
    }
  }
  if (FAILED(hRes))
  {
    Finalize(FALSE);
    return 0;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) MainThreadProc [Entering]", ::GetTickCount()));
  cAgentMgr.Run();
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) MainThreadProc [Start exiting]", ::GetTickCount()));
  //avoid problems if shutdown is signalled while process is detaching
  bExit = FALSE;
  {
    CNktAutoFastMutex cAgentLock(&cAgentMgr);

    if (nManualShutdown == 0)
      bExit = TRUE;
    cAgentMgr.Finalize();
    ::CloseHandle(hMainThread);
    hMainThread = NULL;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) MainThreadProc [Exiting]: %lu", ::GetTickCount(), (DWORD)bExit));
  if (bExit != FALSE)
  {
    EnsureNoThreadsInModule();
    //----
    ::FreeLibraryAndExitThread(cAgentMgr.hAgentInst, 0);
  }
  //NOTE: No need to call "nktMemMgrOnThreadExit()" because DLL_THREAD_DETACH will handle it.
  return 0;
}

HRESULT InstallMiniHooks()
{
  CNktSimpleLockNonReentrant cLock(&nMiniHooksLock);
  HINSTANCE hDll;
  HRESULT hRes;

  if (sMiniHooks[MINIHOOK_ExitProcess].nHookId == 0)
  {
    hRes = NKT_DVERR_NotFound;
    hDll = ::GetModuleHandleW(L"kernel32.dll");
    if (hDll != NULL)
    {
      sMiniHooks[MINIHOOK_ExitProcess].lpProcToHook = ::GetProcAddress(hDll, "ExitProcess");
      if (sMiniHooks[MINIHOOK_ExitProcess].lpProcToHook != NULL)
      {
        sMiniHooks[MINIHOOK_ExitProcess].lpNewProcAddr = OnExitProcessCalled;
        hRes = HRESULT_FROM_WIN32(cAgentMgr.HookLib().Hook(&sMiniHooks[MINIHOOK_ExitProcess], 1));
      }
    }
    if (FAILED(hRes))
      return hRes;
  }
  if (sMiniHooks[MINIHOOK_CorExitProcess].nHookId == 0)
  {
    hDll = ::GetModuleHandleW(L"mscoree.dll");
    if (hDll != NULL)
    {
      sMiniHooks[MINIHOOK_CorExitProcess].lpProcToHook = ::GetProcAddress(hDll, "CorExitProcess");
      if (sMiniHooks[MINIHOOK_CorExitProcess].lpProcToHook != NULL)
      {
        sMiniHooks[MINIHOOK_CorExitProcess].lpNewProcAddr = OnCorExitProcessCalled;
        hRes = HRESULT_FROM_WIN32(cAgentMgr.HookLib().Hook(&sMiniHooks[MINIHOOK_CorExitProcess], 1));
        if (FAILED(hRes))
          return hRes;
        MiscHelpers::FindDllAndIncrementUsageCount(sMiniHooks[MINIHOOK_CorExitProcess].lpProcToHook);
      }
    }
  }
  if (sMiniHooks[MINIHOOK_TerminateProcess].nHookId == 0)
  {
    hRes = NKT_DVERR_NotFound;
    hDll = ::GetModuleHandleW(L"kernel32.dll");
    if (hDll != NULL)
    {
      sMiniHooks[MINIHOOK_TerminateProcess].lpProcToHook = ::GetProcAddress(hDll, "TerminateProcess");
      if (sMiniHooks[MINIHOOK_TerminateProcess].lpProcToHook != NULL)
      {
        sMiniHooks[MINIHOOK_TerminateProcess].lpNewProcAddr = OnTerminateProcessCalled;
        hRes = HRESULT_FROM_WIN32(cAgentMgr.HookLib().Hook(&sMiniHooks[MINIHOOK_TerminateProcess], 1));
      }
    }
    if (FAILED(hRes))
      return hRes;
  }
  if (sMiniHooks[MINIHOOK_CoInitializeSecurity].nHookId == 0)
  {
    hRes = NKT_DVERR_NotFound;
    hDll = ::GetModuleHandleW(L"ole32.dll");
    if (hDll != NULL)
    {
      sMiniHooks[MINIHOOK_CoInitializeSecurity].lpProcToHook = ::GetProcAddress(hDll, "CoInitializeSecurity");
      if (sMiniHooks[MINIHOOK_CoInitializeSecurity].lpProcToHook != NULL)
      {
        sMiniHooks[MINIHOOK_CoInitializeSecurity].lpNewProcAddr = OnCoInitializeSecurityCalled;
        hRes = HRESULT_FROM_WIN32(cAgentMgr.HookLib().Hook(&sMiniHooks[MINIHOOK_CoInitializeSecurity], 1));
        if (SUCCEEDED(hRes))
          MiscHelpers::FindDllAndIncrementUsageCount(sMiniHooks[MINIHOOK_CoInitializeSecurity].lpProcToHook);
      }
    }
    if (FAILED(hRes))
      return hRes;
  }
#if defined _M_X64
  if (sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback].nHookId == 0)
  {
    hRes = NKT_DVERR_NotFound;
    hDll = ::GetModuleHandleW(L"ntdll.dll");
    if (hDll != NULL)
    {
      sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback].lpProcToHook = ::GetProcAddress(hDll, "RtlInstallFunctionTableCallback");
      if (sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback].lpProcToHook != NULL)
      {
        sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback].lpNewProcAddr = OnRtlInstallFunctionTableCallbackCalled;
        hRes = HRESULT_FROM_WIN32(cAgentMgr.HookLib().Hook(&sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback], 1));
      }
    }
    if (FAILED(hRes))
      return hRes;
  }
#endif //_M_X64
  return S_OK;
}

static VOID Finalize(__in BOOL bFinishMainThread)
{
  BOOL bCallDetach;

  bCallDetach = FALSE;
  {
    CNktSimpleLockNonReentrant cLock(&nMiniHooksLock);
    SIZE_T i;

    for (i=0; i<sizeof(sMiniHooks)/sizeof(sMiniHooks[0]); i++)
    {
      if (sMiniHooks[i].nHookId != 0)
      {
        cAgentMgr.HookLib().RemoveHook(sMiniHooks[i].nHookId, TRUE);
        sMiniHooks[i].nHookId = 0;
        if (bFinishMainThread != FALSE)
          bCallDetach = TRUE;
      }
    }
  }
  if (bCallDetach != FALSE)
    ShutdownMainThread();
  return;
}

static VOID WINAPI OnExitProcessCalled(__in UINT uExitCode)
{
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnExitProcessCalled]", ::GetTickCount()));
  //try to shutdown DLL (MiniHooks_Uninstall with TRUE will call shutdown)
  //bAppIsExiting = TRUE;
  Finalize(TRUE);
  lpfnExitProcess(sMiniHooks[MINIHOOK_ExitProcess].lpCallOriginal)(uExitCode);
  return;
}

static VOID WINAPI OnCorExitProcessCalled(__in int uExitCode)
{
  NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnCorExitProcessCalled]", ::GetTickCount()));
  //try to shutdown DLL (MiniHooks_Uninstall with TRUE will call shutdown)
  //bAppIsExiting = TRUE;
  Finalize(TRUE);
  lpfnCorExitProcess(sMiniHooks[MINIHOOK_CorExitProcess].lpCallOriginal)(uExitCode);
  return;
}

static BOOL WINAPI OnTerminateProcessCalled(__in HANDLE hProcess, __in UINT uExitCode)
{
  if (hProcess == (HANDLE)-1)
  {
    NKT_DEBUGPRINTLNA(Nektra::dlAgent, ("%lu) Agent [OnTerminateProcessCalled]", ::GetTickCount()));
    //try to shutdown DLL (MiniHooks_Uninstall with TRUE will call shutdown)
    bAppIsExiting = TRUE;
    Finalize(TRUE);
  }
  return lpfnTerminateProcess(sMiniHooks[MINIHOOK_TerminateProcess].lpCallOriginal)(hProcess, uExitCode);
}

static HRESULT WINAPI OnCoInitializeSecurityCalled(__in_opt PSECURITY_DESCRIPTOR pSecDesc, __in LONG cAuthSvc,
                                            __in SOLE_AUTHENTICATION_SERVICE *asAuthSvc, __in_opt LPVOID pReserved1,
                                            __in DWORD dwAuthnLevel, __in DWORD dwImpLevel, __in_opt LPVOID pAuthList,
                                            __in DWORD dwCapabilities, __in_opt LPVOID pReserved3)
{
  if (DetectApplication() == appInternetExplorer8 && IsRunningElevated() == FALSE)
  {
    return lpfnCoInitializeSecurity(sMiniHooks[MINIHOOK_CoInitializeSecurity].lpCallOriginal)(NULL, -1, NULL, NULL,
                                    RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
  }
  return lpfnCoInitializeSecurity(sMiniHooks[MINIHOOK_CoInitializeSecurity].lpCallOriginal)(pSecDesc, cAuthSvc, asAuthSvc,
                                  pReserved1, dwAuthnLevel, dwImpLevel, pAuthList, dwCapabilities, pReserved3);
}

#if defined _M_X64
static BOOLEAN NTAPI OnRtlInstallFunctionTableCallbackCalled(__in DWORD64 TableIdentifier, __in DWORD64 BaseAddress,
                                                      __in DWORD Length, __in PGET_RUNTIME_FUNCTION_CALLBACK Callback,
                                                      __in PVOID Context, __in PCWSTR OutOfProcessCallbackDll)
{
  NKT_DV_LDR_DATA_TABLE_ENTRY *lpEntry;

  //Office x64 hack... check if "Callback" is inside MSO.DLL, OSPPCEXT.DLL, OSPPOBJS.DLL and OSPPSVC.EXE
  if (SUCCEEDED(nktDvDynApis_LdrFindEntryForAddress((PVOID)Callback, (PVOID*)&lpEntry)))
  {
    switch (lpEntry->BaseDllName.Length)
    {
      case 14:
        if (_wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"mso.dll", 7) == 0)
          return TRUE;
        break;
      case 22:
        if (_wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"osppsvc.exe", 11) == 0 ||
            _wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"pidgenx.dll", 11) == 0)
          return TRUE;
        break;
      case 24:
        if (_wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"osppcext.dll", 12) == 0 ||
            _wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"osppobjs.dll", 12) == 0 ||
            _wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"outlfltr.dll", 12) == 0)
          return TRUE;
        break;
      case 28:
        if (_wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"ipcsecproc.dll", 14) == 0)
          return TRUE;
        break;
      case 42:
        if (_wcsnicmp((LPWSTR)(lpEntry->BaseDllName.Buffer), L"olicenseheartbeat.exe", 21) == 0)
          return TRUE;
        break;
    }
  }
  //call original
  return lpfnRtlInstallFunctionTableCallback(sMiniHooks[MINIHOOK_RtlInstallFunctionTableCallback].lpCallOriginal)
             (TableIdentifier, BaseAddress, Length, Callback, Context, OutOfProcessCallbackDll);
}
#endif //_M_X64

static VOID ShutdownMainThread()
{
  SIZE_T nAction;

  nAction = 0; //do nothing
  {
    CNktAutoFastMutex cAgentLock(&cAgentMgr);

    NktInterlockedExchange(&nManualShutdown, 1);
    if (hMainThread != NULL)
    {
      //try to do a graceful shutdown
      if (cShutdownEvent.Set() != FALSE)
        nAction = 1; //wait
      else
        nAction = 2; //kill
    }
  }
  switch (nAction)
  {
    case 1: //wait
      CNktThread::CoWaitAndDispatchMessages(INFINITE, 1, &hMainThread);
      //::WaitForSingleObject(hMainThread, INFINITE);
      break;
    case 2: //kill
      ::TerminateThread(hMainThread, 0);
      break;
  }
  cShutdownEvent.Set(); //second dummy chance
  {
    CNktAutoFastMutex cAgentLock(&cAgentMgr);

    if (hMainThread != NULL)
    {
      ::CloseHandle(hMainThread);
      hMainThread = NULL;
    }
  }
  return;
}

static VOID EnsureNoThreadsInModule()
{
  HANDLE hSnapShot, hThread;
  CONTEXT sThreadCtx;
  THREADENTRY32 sTe;
  DWORD dwCurrPid, dwCurrTid;
  SIZE_T nCurrIP, nStartIP, nEndIP;
  BOOL bContinue;

  nStartIP = (SIZE_T)(cAgentMgr.hAgentInst);
  nEndIP = nStartIP + CNktDvTools::GetModuleSize(cAgentMgr.hAgentInst);
  //check
  dwCurrPid = ::GetCurrentProcessId();
  dwCurrTid = ::GetCurrentThreadId();
  for (bContinue=TRUE; bContinue!=FALSE; )
  {
    bContinue = FALSE;
    hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapShot != INVALID_HANDLE_VALUE)
    {
      nktMemSet(&sTe, 0, sizeof(sTe));
      sTe.dwSize = (DWORD)sizeof(sTe);
      if (::Thread32First(hSnapShot, &sTe) != FALSE)
      {
        do
        {
          if (sTe.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + (DWORD)sizeof(sTe.th32OwnerProcessID))
          {
            if (sTe.th32OwnerProcessID == dwCurrPid && sTe.th32ThreadID != dwCurrTid)
            {
              //check if some code is still in use
              if (nktDvTlsData_CheckInUseStack(sTe.th32ThreadID) != FALSE)
              {
                bContinue = TRUE;
                break;
              }
              //check if eip/rip is in dll
              hThread = ::OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION | THREAD_SUSPEND_RESUME, FALSE, sTe.th32ThreadID);
              if (hThread != NULL)
              {
                nktDvDynApis_NtSuspendThread(hThread);
                //get thread position
                nktMemSet(&sThreadCtx, 0, sizeof(sThreadCtx));
                sThreadCtx.ContextFlags = CONTEXT_FULL;
                if (::GetThreadContext(hThread, &sThreadCtx) != FALSE)
                {
#if defined _M_X64
                  nCurrIP = (SIZE_T)sThreadCtx.Rip;
#else //_M_X64
                  nCurrIP = (SIZE_T)sThreadCtx.Eip;
#endif //_M_X64
                  //check if inside this dll... shouldn't be
                  if (nCurrIP >= nStartIP && nCurrIP < nEndIP)
                    bContinue = TRUE;
                  //check if inside a trampoline
                  if (bContinue == FALSE && cAgentMgr.CheckIfInTrampoline(nCurrIP) != FALSE)
                    bContinue = TRUE;
                }
                //resume thread and close handle
                ::ResumeThread(hThread);
                ::CloseHandle(hThread);
                if (bContinue != FALSE)
                  break;
              }
            }
          }
          sTe.dwSize = (DWORD)sizeof(sTe);
        }
        while (::Thread32Next(hSnapShot, &sTe) != FALSE);
      }
      ::CloseHandle(hSnapShot);
    }
    if (bContinue != FALSE)
      ::Sleep(20);
  }
  return;
}

#ifdef ATTACH_2_DEBUGGER_AT_STARTUP
static BOOL AttachCurrentProcessToDebugger()
{
  STARTUPINFO sSi;
  PROCESS_INFORMATION sPi;
  CNktStringW cStrTempW;
  SIZE_T i;
  BOOL b;
  DWORD dwExitCode;

  if (::IsDebuggerPresent() == FALSE)
  {
    nktMemSet(&sSi, 0, sizeof(sSi));
    nktMemSet(&sPi, 0, sizeof(sPi));
    if (FAILED(CNktDvTools::GetSystem32Path(cStrTempW)) ||
        cStrTempW.Insert(L"\"", 0) == FALSE ||
        cStrTempW.Concat(L"VSJitDebugger.exe\" -p ") == FALSE ||
        cStrTempW.Concat(::GetCurrentProcessId()) == FALSE)
      return FALSE;
    b = ::CreateProcessW(NULL, (LPWSTR)cStrTempW, NULL, NULL, FALSE, 0, NULL, NULL, &sSi, &sPi);
    if (b != FALSE)
    {
      ::WaitForSingleObject(sPi.hProcess, INFINITE);
      ::GetExitCodeProcess(sPi.hProcess, &dwExitCode);
      if (dwExitCode != 0)
        b = FALSE;
    }
    if (sPi.hThread != NULL)
      ::CloseHandle(sPi.hThread);
    if (sPi.hProcess != NULL)
      ::CloseHandle(sPi.hProcess);
    if (b == FALSE)
      return FALSE;
    for (i=0; i<5*60; i++)
    {
      if (::IsDebuggerPresent() != FALSE)
        break;
      ::Sleep(200);
    }
  }
  Nektra::DebugBreak();
  return TRUE;
}
#endif //ATTACH_2_DEBUGGER_AT_STARTUP
