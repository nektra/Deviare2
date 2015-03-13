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

#pragma once

#include "ObjectBase.h"
#include "DllMain.h"
#include "SpyMgrEvents.h"

//-----------------------------------------------------------
// CNktSpyMgrEngineImpl

class CNktSpyMgrEngineImpl : public CNktDvSpyMgr
{
public:
  VOID OnCustomDllLoad(__in CNktDvProcess *lpProc, __in LONG nActionId, __in HRESULT hActionResult);
  VOID OnCustomDllUnload(__in CNktDvProcess *lpProc, __in LONG nActionId, __in HRESULT hActionResult);
  VOID OnCustomApiCall(__in CNktDvProcess *lpProc, __in LONG nActionId, __in HRESULT hActionResult,
                       __in LONG nApiResult);

  VOID OnHookStateChanged(__in CNktDvHook *lpHook, __in CNktDvProcess *lpProc,
                          __in CNktDvHook::eState nNewState, __in CNktDvHook::eState nOldState);
  VOID OnHookCalled(__in CNktDvHook *lpHook, __in CNktDvProcess *lpProc,
                    __in CNktDvHookCallInfo *lpCallInfo);

  VOID OnCreateProcessCall(__in CNktDvProcess *lpProc, __in DWORD dwPid, __in DWORD dwMainThreadId,
                           __in BOOL bIs64BitProcess, __in BOOL bCanHookNow);
  VOID OnLoadLibraryCall(__in CNktDvProcess *lpProc, __in LPCWSTR szLibraryNameW, __in HMODULE hModule);
  VOID OnFreeLibraryCall(__in CNktDvProcess *lpProc, __in HMODULE hModule);

  VOID OnProcessAdded(__in CNktDvProcess *lpProc);
  VOID OnProcessRemoved(__in CNktDvProcess *lpProc);

  VOID OnAgentLoad(__inout CNktDvProcess *lpProc, __in HRESULT hErrorCode);
  VOID OnAgentUnload(__inout CNktDvProcess *lpProc);

  SIZE_T OnCustomMessage(__inout CNktDvProcess *lpProc, __in SIZE_T nMsgCode, __in SIZE_T nMsgParam);

  VOID OnHookOverwritten(__inout CNktDvHook *lpHook, __inout CNktDvProcess *lpProc);

  VOID OnThreadStarted();
  VOID OnThreadEnded();
};

//-----------------------------------------------------------
// CNktSpyMgrImpl

class ATL_NO_VTABLE CNktSpyMgrImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                     public CComCoClass<CNktSpyMgrImpl, &CLSID_NktSpyMgr>,
                                     public CNktMemMgrObj,
                                     public CNktSpyMgrEventsImpl<CNktSpyMgrImpl>,
                                     public IConnectionPointContainerImpl<CNktSpyMgrImpl>,
                                     public IProvideClassInfo2Impl<&CLSID_NktSpyMgr,
                                                      &__uuidof(DNktSpyMgrEvents), &LIBID_Deviare2,
                                                      DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>,
                                     public IObjectSafetyImpl<CNktSpyMgrImpl,
                                                              INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                     public CNktObjectBaseImpl<CNktSpyMgrImpl, INktSpyMgr, CNktDvSpyMgr,
                                                               &CLSID_NktSpyMgr, &IID_INktSpyMgr,
                                                               &LIBID_Deviare2>,
                                     public IInterfaceItemImpl<CNktSpyMgrImpl>,
                                     public INktDvEngBaseComLink
{
public:
  CNktSpyMgrImpl(__in CNktDvSpyMgr *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                     CComCoClass<CNktSpyMgrImpl, &CLSID_NktSpyMgr>(),
                     CNktMemMgrObj(),
                     CNktSpyMgrEventsImpl<CNktSpyMgrImpl>(),
                     IConnectionPointContainerImpl<CNktSpyMgrImpl>(),
                     IProvideClassInfo2Impl<&CLSID_NktSpyMgr, &__uuidof(DNktSpyMgrEvents),
                                            &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                            DEVIARECOM_MinorVersion>(),
                     IObjectSafetyImpl<CNktSpyMgrImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                     CNktObjectBaseImpl<CNktSpyMgrImpl, INktSpyMgr, CNktDvSpyMgr, &CLSID_NktSpyMgr,
                                        &IID_INktSpyMgr, &LIBID_Deviare2>(lpIntObj),
                     IInterfaceItemImpl<CNktSpyMgrImpl>(),
                     INktDvEngBaseComLink()
    {
    NktInterlockedExchange(&nInitialized, 0);
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktSpyMgr",
                                 L"1", L"NktSpyMgr Class", CLSID_NktSpyMgr,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktSpyMgrImpl)
    COM_INTERFACE_ENTRY(INktSpyMgr)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  BEGIN_CONNECTION_POINT_MAP(CNktSpyMgrImpl)
    CONNECTION_POINT_ENTRY(__uuidof(DNktSpyMgrEvents))
  END_CONNECTION_POINT_MAP()

  DECLARE_CLASSFACTORY()

  typedef CNktSpyMgrImpl _CreatorClass;

  OBJBASE_DECLARE_APIS_1(CNktSpyMgrImpl,CNktDvSpyMgr)

  static HRESULT WINAPI CreateInstance(void* pv, REFIID riid, LPVOID* ppv);

public:
  STDMETHODIMP SettingOverride(__in BSTR setting, __in LONG newValue);

  STDMETHODIMP get_DatabasePath(__out BSTR *pVal);

  STDMETHODIMP put_DatabasePath(__in BSTR newValue);

  STDMETHODIMP get_AgentPath(__out BSTR *pVal);

  STDMETHODIMP put_AgentPath(__in BSTR newValue);

  STDMETHODIMP Initialize(__out LONG *pVal);

  STDMETHODIMP Processes(__deref_out INktProcessesEnum **ppProcsEnum);

  STDMETHODIMP Hooks(__deref_out INktHooksEnum **ppHooksEnum);

  STDMETHODIMP CreateHooksCollection(__deref_out INktHooksEnum **ppHooksColl);

  STDMETHODIMP CreateHook(__in VARIANT func, __in LONG hookFlags, __deref_out INktHook **ppHook);

  STDMETHODIMP CreateHookForAddress(__in my_ssize_t addr, __in BSTR functionName, __in LONG hookFlags,
                                    __deref_out INktHook **ppHook);

  STDMETHODIMP DestroyHook(__in INktHook *hook);

  STDMETHODIMP LoadAgent(__in VARIANT procOrId);

  STDMETHODIMP UnloadAgent(__in VARIANT procOrId);

  STDMETHODIMP LoadCustomDll(__in VARIANT procOrId, __in BSTR dllFileName,
                             __in VARIANT_BOOL unloadOnAgentShutdown, __in VARIANT_BOOL sync,
                             __out LONG *pVal);

  STDMETHODIMP UnloadCustomDll(__in VARIANT procOrId, __in BSTR dllFileName, __in VARIANT_BOOL sync,
                               __out LONG *pVal);

  STDMETHODIMP CallCustomApi(__in VARIANT procOrId, __in BSTR dllFileName, __in BSTR apiToCall,
                             __in VARIANT *params, __in VARIANT_BOOL sync, __out LONG *pVal);

  STDMETHODIMP ProcessFromPID(__in LONG pid, __deref_out INktProcess **ppProc);

  STDMETHODIMP ProcessMemoryFromPID(__in LONG pid, __deref_out INktProcessMemory **ppProcMem);

  STDMETHODIMP CreateProcess(__in BSTR imagePath, __in VARIANT_BOOL suspended, __out VARIANT *continueEvent,
                             __deref_out INktProcess **ppProc);

  STDMETHODIMP CreateProcessWithLogon(__in BSTR imagePath, __in BSTR userName, __in BSTR password,
                                      __in VARIANT_BOOL suspended, __out VARIANT *continueEvent,
                                      __deref_out INktProcess **ppProc);

  STDMETHODIMP CreateProcessWithToken(__in BSTR imagePath, __in my_ssize_t token, __in VARIANT_BOOL suspended,
                                      __out VARIANT *continueEvent, __deref_out INktProcess **ppProc);

  STDMETHODIMP ResumeProcess(__in INktProcess *pProc, __in VARIANT continueEvent);

  STDMETHODIMP TerminateProcess(__in VARIANT procOrId, __in LONG exitCode);

  STDMETHODIMP FindProcessId(__in BSTR name, __out LONG *pVal);

  STDMETHODIMP RealFilePath(__in BSTR name, __out BSTR *pVal);

  STDMETHODIMP DbObjects(__in LONG platformBits, __deref_out INktDbObjectsEnum **ppObjsEnum);

  STDMETHODIMP DbFunctions(__in LONG platformBits, __deref_out INktDbObjectsEnum **ppFuncsEnum);

  STDMETHODIMP DbModules(__in LONG platformBits, __deref_out INktDbModulesEnum **ppModsEnum);

  STDMETHODIMP ProcessHandle(__in VARIANT procOrId, __in LONG desiredAccess, __out my_ssize_t *pVal);

  STDMETHODIMP ProcessMemory(__in VARIANT procOrId, __deref_out INktProcessMemory **ppProcMem);

  STDMETHODIMP GetWindowProc(__in VARIANT procOrId, __in my_ssize_t hWnd, __out my_ssize_t* ppWndProc);

private:
  CNktStringW cStrLicenseKeyW;
  CNktStringW cStrDbPathW;
  CNktStringW cStrAgentPathW;
  LONG volatile nInitialized;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_AUTO(__uuidof(NktSpyMgr), CNktSpyMgrImpl)
