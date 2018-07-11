/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#pragma once

#include "DllMain.h"
class CNktHookLibImpl;
class CNktHookInfoImpl;
class CNktHookProcessInfoImpl;

//-----------------------------------------------------------

class ATL_NO_VTABLE CNktHookLibImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                      public CComCoClass<CNktHookLibImpl, &CLSID_NktHookLib>,
                                      public IObjectSafetyImpl<CNktHookLibImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                      public IDispatchImpl<INktHookLib, &IID_INktHookLib, &LIBID_DeviareLite, 1, 0>
{
public:
  CNktHookLibImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                      CComCoClass<CNktHookLibImpl, &CLSID_NktHookLib>(),
                      IObjectSafetyImpl<CNktHookLibImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                      IDispatchImpl<INktHookLib, &IID_INktHookLib, &LIBID_DeviareLite, 1, 0>()
    {
    return;
    };

  ~CNktHookLibImpl()
    {
    cHookLib.UnhookAll();
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareLite.NktHookLib", L"1", L"NktHookLib Class",
                                 CLSID_NktHookLib, LIBID_DeviareLite, L"Neutral")

  BEGIN_COM_MAP(CNktHookLibImpl)
    COM_INTERFACE_ENTRY(INktHookLib)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  // ISupportsErrorInfo
  STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  HRESULT FinalConstruct()
    {
    HRESULT hRes = DotNetCoreHooks::Initialize();
    if (SUCCEEDED(hRes))
      hRes = ::CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &(cUnkMarshaler.p));
    return hRes;
    };

  void FinalRelease()
    {
    cUnkMarshaler.Release();
    return;
    };

public:
  STDMETHOD(Hook)(__in VARIANT itemsToHook, __in LONG flags);
  STDMETHOD(RemoteHook)(__in VARIANT itemsToHook, __in LONG pid, __in LONG flags);

  STDMETHOD(Unhook)(__in VARIANT itemsToUnhook);
  STDMETHOD(UnhookProcess)(__in LONG pid);
  STDMETHOD(UnhookAll)();

  STDMETHOD(EnableHook)(__in VARIANT items, __in VARIANT_BOOL enable);

  STDMETHOD(RemoveHook)(__in VARIANT items, __in VARIANT_BOOL disable);

  STDMETHOD(put_SuspendThreadsWhileHooking)(__in VARIANT_BOOL enable);
  STDMETHOD(get_SuspendThreadsWhileHooking)(__out VARIANT_BOOL *enable);

  STDMETHOD(put_ShowDebugOutput)(__in VARIANT_BOOL enable);
  STDMETHOD(get_ShowDebugOutput)(__out VARIANT_BOOL *enable);

  STDMETHOD(GetModuleBaseAddress)(__in BSTR moduleName, __out my_ssize_t *baseAddress);
  STDMETHOD(GetRemoteModuleBaseAddress)(__in LONG pid, __in BSTR moduleName, __in VARIANT_BOOL scanMappedImages,
                                        __out my_ssize_t *baseAddress);

  STDMETHOD(GetProcedureAddress)(__in my_ssize_t moduleBaseAddress, __in BSTR procName, __out my_ssize_t *funcAddress);
  STDMETHOD(GetRemoteProcedureAddress)(__in LONG pid, __in my_ssize_t moduleBaseAddress, __in BSTR procName,
                                       __out my_ssize_t *funcAddress);

  STDMETHOD(CreateProcessWithDll)(__in BSTR applicationName, __in BSTR commandLine, __in my_ssize_t processAttributes,
                                  __in my_ssize_t threadAttributes, __in VARIANT_BOOL inheritHandles,
                                  __in LONG creationFlags, __in BSTR environment, __in BSTR currentDirectory,
                                  __in my_ssize_t startupInfo, __in BSTR dllName, __in my_ssize_t signalCompletedEvent,
                                  __in BSTR initFunctionName, __deref_out INktHookProcessInfo **ppProcInfo);

  STDMETHOD(CreateProcessWithLogonAndDll)(__in BSTR userName, __in BSTR domain, __in BSTR password,
                                          __in LONG logonFlags, __in BSTR applicationName, __in BSTR commandLine,
                                          __in LONG creationFlags, __in BSTR environment, __in BSTR currentDirectory,
                                          __in my_ssize_t startupInfo, __in BSTR dllName,
                                          __in my_ssize_t signalCompletedEvent, __in BSTR initFunctionName,
                                          __deref_out INktHookProcessInfo **ppProcInfo);

  STDMETHOD(CreateProcessWithTokenAndDll)(__in my_ssize_t token, __in LONG logonFlags, __in BSTR applicationName,
                                          __in BSTR commandLine, __in LONG creationFlags, __in BSTR environment,
                                          __in BSTR currentDirectory, __in my_ssize_t startupInfo, __in BSTR dllName,
                                          __in my_ssize_t signalCompletedEvent, __in BSTR initFunctionName,
                                          __deref_out INktHookProcessInfo **ppProcInfo);

  STDMETHOD(InjectDll)(__in LONG processId, __in BSTR dllName, __in_opt BSTR initFunctionName=NULL,
                       __in_opt LONG processInitWaitTimeoutMs=5000, __out_opt my_ssize_t *injectorThreadHandle=NULL);

  STDMETHOD(InjectDllH)(__in my_ssize_t processHandle, __in BSTR dllName, __in_opt BSTR initFunctionName=NULL,
                        __in_opt LONG processInitWaitTimeoutMs=5000, __out_opt my_ssize_t *injectorThreadHandle=NULL);

private:
  friend class CNktHookInfoImpl;

  class CHookInfo
  {
  public:
    CHookInfo();
    ~CHookInfo();

    HRESULT Init(__in VARIANT items);
    VOID StoreInfo();

  public:
    CNktHookLib::HOOK_INFO *lpInfo;
    INktHookInfo **lplpHookInfoPtr;
    SIZE_T nCount;
  };

private:
  CNktHookLib cHookLib;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

//-----------------------------------------------------------

OBJECT_ENTRY_AUTO(__uuidof(NktHookLib), CNktHookLibImpl)
