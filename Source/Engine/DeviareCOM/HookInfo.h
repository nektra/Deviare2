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
#include "InternalCreate.h"

//-----------------------------------------------------------
// CNktHookInfoImpl

class ATL_NO_VTABLE CNktHookInfoImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                       public CComCoClass<CNktHookInfoImpl, &CLSID_NktHookInfo>,
                                       public CNktMemMgrObj,
                                       public IObjectSafetyImpl<CNktHookInfoImpl,
                                                                INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                       public IDispatchImpl<INktHookInfo, &IID_INktHookInfo,
                                                            &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                                            DEVIARECOM_MinorVersion>,
                                       public IInterfaceItemImpl<CNktHookInfoImpl>
{
public:
  CNktHookInfoImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                       CComCoClass<CNktHookInfoImpl, &CLSID_NktHookInfo>(),
                       CNktMemMgrObj(),
                       IObjectSafetyImpl<CNktHookInfoImpl,
                                         INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                       IDispatchImpl<INktHookInfo, &IID_INktHookInfo,
                                     &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                     DEVIARECOM_MinorVersion>(),
                       IInterfaceItemImpl<CNktHookInfoImpl>()
    {
    NktInterlockedExchange(&nValid, 1);
    NktInterlockedExchange(&nLock, 0);
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktHookInfo",
                                 L"1", L"NktHookInfo Class", CLSID_NktHookInfo,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktHookInfoImpl)
    COM_INTERFACE_ENTRY(INktHookInfo)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  HRESULT FinalConstruct()
    {
    InterfaceItemAdd();
    return ::CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &(cUnkMarshaler.p));
    };

  void FinalRelease()
    {
    cUnkMarshaler.Release();
    return;
    };

  HRESULT InvalidateInterface();

public:
  STDMETHODIMP get_Id(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_FunctionName(__deref_out BSTR *pVal);

  STDMETHODIMP get_Address(__deref_out my_ssize_t *pVal);

  STDMETHODIMP DbObject(__deref_out INktDbObject **ppDbObject);

  STDMETHODIMP SendCustomMessage(__in my_ssize_t msgCode, __in my_ssize_t msgParam, __in VARIANT_BOOL sync,
                                 __out my_ssize_t *pRetVal);

  STDMETHODIMP ThreadIgnore(__in LONG theadId, __in VARIANT_BOOL ignore);

  STDMETHODIMP InvalidateCache(__in my_ssize_t hDll);

  STDMETHODIMP CurrentProcess(__deref_out INktProcess **ppProc);

  static HRESULT InternalCreate(__in DV_AGENTCOM_HOOKINFO_CREATE_DATA *lpData, __deref_out IDispatch **lplpDisp);

private:
  DWORD dwId;
  SIZE_T nAddress;
  LPWSTR szFunctionNameW;
  CNktDvDbObjectNoRef *lpDbFunc;
  DV_AGENTCOM_HOOKCALLINFO_CALLTRAMPOLINE sCallTrampoline;
  TNktComPtr<CNktDvProcess> cProc;
  //----
  LONG volatile nValid;
  LONG volatile nLock;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktHookInfo), CNktHookInfoImpl)
