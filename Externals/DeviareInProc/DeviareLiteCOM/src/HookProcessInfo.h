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

//-----------------------------------------------------------

class ATL_NO_VTABLE CNktHookProcessInfoImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                              public CComCoClass<CNktHookProcessInfoImpl, &CLSID_NktHookProcessInfo>,
                                              public IObjectSafetyImpl<CNktHookProcessInfoImpl,
                                                                       INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                              public IDispatchImpl<INktHookProcessInfo, &IID_INktHookProcessInfo,
                                                                   &LIBID_DeviareLite, 1, 0>
{
public:
  CNktHookProcessInfoImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                       CComCoClass<CNktHookProcessInfoImpl, &CLSID_NktHookProcessInfo>(),
                       IObjectSafetyImpl<CNktHookProcessInfoImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                       IDispatchImpl<INktHookProcessInfo, &IID_INktHookProcessInfo, &LIBID_DeviareLite, 1, 0>()
    {
    memset(&sProcInfo, 0, sizeof(sProcInfo));
    return;
    };

  ~CNktHookProcessInfoImpl()
    {
    if (sProcInfo.hThread != NULL)
      ::CloseHandle(sProcInfo.hThread);
    if (sProcInfo.hProcess != NULL)
      ::CloseHandle(sProcInfo.hProcess);
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareLite.NktHookProcessInfo", L"1",
                                 L"NktHookProcessInfo Class", CLSID_NktHookProcessInfo, LIBID_DeviareLite, L"Neutral")

  BEGIN_COM_MAP(CNktHookProcessInfoImpl)
    COM_INTERFACE_ENTRY(INktHookProcessInfo)
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
  STDMETHOD(get_ProcessHandle)(__out my_ssize_t *procHandle);
  STDMETHOD(get_ThreadHandle)(__out my_ssize_t *threadHandle);
  STDMETHOD(get_ProcessId)(__out LONG *pid);
  STDMETHOD(get_ThreadId)(__out LONG *tid);

private:
  friend class CNktHookLibImpl;

  PROCESS_INFORMATION sProcInfo;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

//-----------------------------------------------------------

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktHookProcessInfo), CNktHookProcessInfoImpl)
