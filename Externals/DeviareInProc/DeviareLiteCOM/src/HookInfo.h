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

//-----------------------------------------------------------

class ATL_NO_VTABLE CNktHookInfoImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                       public CComCoClass<CNktHookInfoImpl, &CLSID_NktHookInfo>,
                                       public IObjectSafetyImpl<CNktHookInfoImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                       public IDispatchImpl<INktHookInfo, &IID_INktHookInfo, &LIBID_DeviareLite, 1, 0>
{
public:
  CNktHookInfoImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                       CComCoClass<CNktHookInfoImpl, &CLSID_NktHookInfo>(),
                       IObjectSafetyImpl<CNktHookInfoImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                       IDispatchImpl<INktHookInfo, &IID_INktHookInfo, &LIBID_DeviareLite, 1, 0>()
    {
    memset(&sInfo, 0, sizeof(sInfo));
    return;
    };

  ~CNktHookInfoImpl()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareLite.NktHookInfo", L"1", L"NktHookInfo Class",
                                 CLSID_NktHookInfo, LIBID_DeviareLite, L"Neutral")

  BEGIN_COM_MAP(CNktHookInfoImpl)
    COM_INTERFACE_ENTRY(INktHookInfo)
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
  STDMETHOD(get_Id)(__out LONG *hookId);

  STDMETHOD(get_OrigProcAddr)(__out my_ssize_t *procAddr);
  STDMETHOD(put_OrigProcAddr)(__in my_ssize_t procAddr);

  STDMETHOD(get_NewProcAddr)(__out my_ssize_t *procAddr);
  STDMETHOD(put_NewProcAddr)(__in my_ssize_t procAddr);

  STDMETHOD(get_CallOriginalAddr)(__out my_ssize_t *procAddr);

private:
  friend class CNktHookLibImpl::CHookInfo;

  CNktHookLib::HOOK_INFO sInfo;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

//-----------------------------------------------------------

OBJECT_ENTRY_AUTO(__uuidof(NktHookInfo), CNktHookInfoImpl)
