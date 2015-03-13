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
#include "IDispEventWithTypeLibImpl.h"

//-----------------------------------------------------------

typedef HRESULT (WINAPI *lpfnDllGetClassObject)(__in REFCLSID rclsid, __in REFIID riid,
                                                __deref_out LPVOID FAR* ppv);
typedef HRESULT (WINAPI *lpfnDllCanUnloadNow)();

//-----------------------------------------------------------

typedef enum {
  dispidNktSpyMgrEventsOnCustomDllLoad = 1,
  dispidNktSpyMgrEventsOnCustomDllUnload,
  dispidNktSpyMgrEventsOnCustomApiCall,
  dispidNktSpyMgrEventsOnHookStateChanged,
  dispidNktSpyMgrEventsOnFunctionCalled,
  dispidNktSpyMgrEventsOnCreateProcessCall,
  dispidNktSpyMgrEventsOnLoadLibraryCall,
  dispidNktSpyMgrEventsOnFreeLibraryCall,
  dispidNktSpyMgrEventsOnProcessStarted,
  dispidNktSpyMgrEventsOnProcessTerminated,
  dispidNktSpyMgrEventsOnAgentLoad,
  dispidNktSpyMgrEventsOnAgentUnload,
} eDispidNktSpyMgrEvents;

//-----------------------------------------------------------

class CSpyMgrEvents : public IDispEventWithTypeLibImpl<1, CSpyMgrEvents,
                                                       &Deviare2::DIID_DNktSpyMgrEvents>
{
public:
  CSpyMgrEvents(__in CSpyMgr *_lpSpyMgr, __in HINSTANCE hDll) : IDispEventWithTypeLibImpl<1,
                                              CSpyMgrEvents, &Deviare2::DIID_DNktSpyMgrEvents>(hDll)
    {
    lpSpyMgr = _lpSpyMgr;
    return;
    };

  HRESULT __stdcall OnCustomDllLoad(__in Deviare2::INktProcess *proc, __in long actionId,
                                    __in long actionResult)
    {
    return lpSpyMgr->OnCustomDllLoad(proc, actionId, actionResult);
    };
  HRESULT __stdcall OnCustomDllUnload(__in Deviare2::INktProcess *proc, __in long actionId,
                                      __in long actionResult)
    {
    return lpSpyMgr->OnCustomDllUnload(proc, actionId, actionResult);
    };

  HRESULT __stdcall OnCustomApiCall(__in Deviare2::INktProcess *proc, __in long actionId,
                                    __in long actionResult, __in long apiResult)
    {
    return lpSpyMgr->OnCustomApiCall(proc, actionId, actionResult, apiResult);
    };

  HRESULT __stdcall OnHookStateChanged(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                       __in Deviare2::eNktHookState newState,
                                       __in Deviare2::eNktHookState oldState)
    {
    return lpSpyMgr->OnHookStateChanged(Hook, proc, newState, oldState);
    };

  HRESULT __stdcall OnFunctionCalled(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                     __in Deviare2::INktHookCallInfo *callInfo)
    {
    return lpSpyMgr->OnFunctionCalled(Hook, proc, callInfo);
    };

  HRESULT __stdcall OnCreateProcessCall(__in Deviare2::INktProcess *proc, __in long childPid,
                                        __in long mainThreadId, __in VARIANT_BOOL is64BitProcess,
                                        __in VARIANT_BOOL canHookNow)
    {
    return lpSpyMgr->OnCreateProcessCall(proc, childPid, mainThreadId, is64BitProcess, canHookNow);
    };

  HRESULT __stdcall OnLoadLibraryCall(__in Deviare2::INktProcess *proc, __in BSTR dllName,
                                      __in VARIANT moduleHandle)
    {
    return lpSpyMgr->OnLoadLibraryCall(proc, dllName, moduleHandle);
    };

  HRESULT __stdcall OnFreeLibraryCall(__in Deviare2::INktProcess *proc,
                                      __in VARIANT moduleHandle)
    {
    return lpSpyMgr->OnFreeLibraryCall(proc, moduleHandle);
    };

  HRESULT __stdcall OnProcessStarted(__in Deviare2::INktProcess *proc)
    {
    return lpSpyMgr->OnProcessStarted(proc);
    };

  HRESULT __stdcall OnProcessTerminated(__in Deviare2::INktProcess *proc)
    {
    return lpSpyMgr->OnProcessTerminated(proc);
    };

  HRESULT __stdcall OnAgentLoad(__in Deviare2::INktProcess *proc, __in long errorCode)
    {
    return lpSpyMgr->OnAgentLoad(proc, errorCode);
    };

  HRESULT __stdcall OnAgentUnload(__in Deviare2::INktProcess *proc)
    {
    return lpSpyMgr->OnAgentUnload(proc);
    };

  BEGIN_SINK_MAP(CSpyMgrEvents)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnCustomDllLoad,
                  OnCustomDllLoad)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnCustomDllUnload,
                  OnCustomDllUnload)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnCustomApiCall,
                  OnCustomApiCall)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnHookStateChanged,
                  OnHookStateChanged)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnFunctionCalled,
                  OnFunctionCalled)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnCreateProcessCall,
                  OnCreateProcessCall)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnLoadLibraryCall,
                  OnLoadLibraryCall)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnFreeLibraryCall,
                  OnFreeLibraryCall)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnProcessStarted,
                  OnProcessStarted)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnProcessTerminated,
                  OnProcessTerminated)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnAgentLoad,
                  OnAgentLoad)
    SINK_ENTRY_EX(1, Deviare2::DIID_DNktSpyMgrEvents, dispidNktSpyMgrEventsOnAgentUnload,
                  OnAgentUnload)
  END_SINK_MAP()

private:
  CSpyMgr *lpSpyMgr;
};

//-----------------------------------------------------------

CSpyMgr::CSpyMgr()
{
  hDeviareComDll = NULL;
  hResCoInit = E_FAIL;
  lpSpyMgrEvents = NULL;
  return;
}

CSpyMgr::~CSpyMgr()
{
  Finalize();
  return;
}

HRESULT CSpyMgr::Initialize()
{
  CComPtr<IClassFactory> cFac;
  lpfnDllGetClassObject fnDllGetClassObject;
  CSpyMgrEvents *_lpSpyMgrEvents;
  HRESULT hRes, hRes2;

  hResCoInit = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
  if (FAILED(hResCoInit) && hResCoInit != RPC_E_CHANGED_MODE)
    return hResCoInit;
  //load library
#if defined _M_IX86
  hDeviareComDll = ::LoadLibraryW(L"DeviareCOM.dll");
#elif defined _M_X64
  hDeviareComDll = ::LoadLibraryW(L"DeviareCOM64.dll");
#else
  #error Unsupported platform
#endif
  if (hDeviareComDll == NULL)
  {
    hRes = HRESULT_FROM_WIN32(::GetLastError());
    Finalize();
    return hRes;
  }
  //create class factory
  fnDllGetClassObject = (lpfnDllGetClassObject)::GetProcAddress(hDeviareComDll, "DllGetClassObject");
  hRes = (fnDllGetClassObject != NULL) ? S_OK : E_FAIL;
  if (SUCCEEDED(hRes))
  {
    hRes = fnDllGetClassObject(Deviare2::CLSID_NktSpyMgr, IID_IClassFactory, (void**)&cFac);
  }
  //create spymgr instance
  if (SUCCEEDED(hRes))
  {
    hRes = cFac->CreateInstance(NULL, Deviare2::IID_INktSpyMgr, (void**)&cSpyMgr);
  }
  //initialize
  if (SUCCEEDED(hRes))
  {
    hRes = cSpyMgr->Initialize(&hRes2);
    if (SUCCEEDED(hRes))
      hRes = hRes2;
  }
  //create sink object and connect
  if (SUCCEEDED(hRes))
  {
    _lpSpyMgrEvents = new CSpyMgrEvents(this, hDeviareComDll);
    if (_lpSpyMgrEvents != NULL)
    {
      lpSpyMgrEvents = _lpSpyMgrEvents;
      hRes = _lpSpyMgrEvents->DispEventAdvise(cSpyMgr, &Deviare2::DIID_DNktSpyMgrEvents);
    }
    else
    {
      hRes = E_OUTOFMEMORY;
    }
  }
  //done
  if (FAILED(hRes))
  {
    cFac.Release();
    Finalize();
  }
  return hRes;
}

VOID CSpyMgr::Finalize()
{
  if (lpSpyMgrEvents != NULL)
  {
    CSpyMgrEvents *_lpSpyMgrEvents = (CSpyMgrEvents*)lpSpyMgrEvents;
    if (_lpSpyMgrEvents->m_dwEventCookie != 0xFEFEFEFE)
      _lpSpyMgrEvents->DispEventUnadvise(cSpyMgr);
    _lpSpyMgrEvents->Release();
    _lpSpyMgrEvents = NULL;
  }
  cSpyMgr.Release();
  if (hDeviareComDll != NULL)
  {
    lpfnDllCanUnloadNow fnDllCanUnloadNow;

    fnDllCanUnloadNow = (lpfnDllCanUnloadNow)::GetProcAddress(hDeviareComDll, "DllCanUnloadNow");
    if (fnDllCanUnloadNow != NULL)
    {
      while (fnDllCanUnloadNow() == S_FALSE)
        ::Sleep(500);
    }
    ::FreeLibrary(hDeviareComDll);
    hDeviareComDll = NULL;
  }
  if (SUCCEEDED(hResCoInit))
  {
    ::CoUninitialize();
    hResCoInit = E_FAIL;
  }
  return;
}

HRESULT CSpyMgr::OnCustomDllLoad(__in Deviare2::INktProcess *proc, __in long actionId,
                                 __in long actionResult)
{
  return S_OK;
}

HRESULT CSpyMgr::OnCustomDllUnload(__in Deviare2::INktProcess *proc, __in long actionId,
                                   __in long actionResult)
{
  return S_OK;
}

HRESULT CSpyMgr::OnCustomApiCall(__in Deviare2::INktProcess *proc, __in long actionId,
                                 __in long actionResult, __in long apiResult)
{
  return S_OK;
}

HRESULT CSpyMgr::OnHookStateChanged(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                    __in Deviare2::eNktHookState newState,
                                    __in Deviare2::eNktHookState oldState)
{
  return S_OK;
}

HRESULT CSpyMgr::OnFunctionCalled(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                  __in Deviare2::INktHookCallInfo *callInfo)
{
  return S_OK;
}

HRESULT CSpyMgr::OnCreateProcessCall(__in Deviare2::INktProcess *proc, __in long pid,
                                     __in long mainThreadId, __in VARIANT_BOOL is64BitProcess,
                                     __in VARIANT_BOOL canHookNow)
{
  return S_OK;
}

HRESULT CSpyMgr::OnLoadLibraryCall(__in Deviare2::INktProcess *proc, __in BSTR dllName,
                                   __in VARIANT moduleHandle)
{
  return S_OK;
}

HRESULT CSpyMgr::OnFreeLibraryCall(__in Deviare2::INktProcess *proc, __in VARIANT moduleHandle)
{
  return S_OK;
}

HRESULT CSpyMgr::OnProcessStarted(__in Deviare2::INktProcess *proc)
{
  return S_OK;
}

HRESULT CSpyMgr::OnProcessTerminated(__in Deviare2::INktProcess *proc)
{
  return S_OK;
}

HRESULT CSpyMgr::OnAgentLoad(__in Deviare2::INktProcess *proc, __in long errorCode)
{
  return S_OK;
}

HRESULT CSpyMgr::OnAgentUnload(__in Deviare2::INktProcess *proc)
{
  return S_OK;
}
