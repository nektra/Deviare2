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
#include "HookEvents.h"

//-----------------------------------------------------------
// CNktHookImpl

class ATL_NO_VTABLE CNktHookImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                   public CComCoClass<CNktHookImpl, &CLSID_NktHook>,
                                   public CNktMemMgrObj,
                                   public CNktHookEventsImpl<CNktHookImpl>,
                                   public IConnectionPointContainerImpl<CNktHookImpl>,
                                   public IProvideClassInfo2Impl<&CLSID_NktHook, &__uuidof(DNktHookEvents),
                                                                 &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                                                 DEVIARECOM_MinorVersion>,
                                   public IObjectSafetyImpl<CNktHookImpl,
                                                            INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                   public CNktObjectBaseImpl<CNktHookImpl, INktHook, CNktDvHook,
                                                             &CLSID_NktHook, &IID_INktHook,
                                                             &LIBID_Deviare2>,
                                   public IInterfaceItemImpl<CNktHookImpl>,
                                   public INktDvEngBaseComLink
{
public:
  CNktHookImpl(__in CNktDvHook *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                   CComCoClass<CNktHookImpl, &CLSID_NktHook>(),
                   CNktMemMgrObj(),
                   CNktHookEventsImpl<CNktHookImpl>(),
                   IConnectionPointContainerImpl<CNktHookImpl>(),
                   IProvideClassInfo2Impl<&CLSID_NktHook, &__uuidof(DNktHookEvents), &LIBID_Deviare2,
                                          DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>(),
                   IObjectSafetyImpl<CNktHookImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                   CNktObjectBaseImpl<CNktHookImpl, INktHook, CNktDvHook, &CLSID_NktHook, &IID_INktHook,
                                      &LIBID_Deviare2>(lpIntObj),
                   IInterfaceItemImpl<CNktHookImpl>(),
                   INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktHook",
                                 L"1", L"NktHook Class", CLSID_NktHook,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktHookImpl)
    COM_INTERFACE_ENTRY(INktHook)
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

  BEGIN_CONNECTION_POINT_MAP(CNktHookImpl)
    CONNECTION_POINT_ENTRY(__uuidof(DNktHookEvents))
  END_CONNECTION_POINT_MAP()

  OBJBASE_DECLARE_APIS_1(CNktHookImpl,CNktDvHook)

public:
  STDMETHODIMP get_Id(__deref_out my_ssize_t *pVal);

  STDMETHODIMP Destroy();

  STDMETHODIMP Attach(__in VARIANT procOrId, __in VARIANT_BOOL sync);

  STDMETHODIMP Detach(__in VARIANT procOrId, __in VARIANT_BOOL sync);

  STDMETHODIMP Enable(__in VARIANT procOrId, __in VARIANT_BOOL enable, __in VARIANT_BOOL sync);

  STDMETHODIMP State(__in VARIANT procOrId, __out eNktHookState *pVal);

  STDMETHODIMP QueryOverwrite(__in VARIANT procOrId, __out VARIANT_BOOL *pVal);

  STDMETHODIMP LastError(__in VARIANT procOrId, __out LONG *pVal);

  STDMETHODIMP Hook(__in VARIANT_BOOL sync);

  STDMETHODIMP Unhook(__in VARIANT_BOOL sync);

  STDMETHODIMP AddCustomHandler(__in BSTR handlerDll, __in LONG handlerFlags, __in BSTR parameters);

  STDMETHODIMP RemoveAllHandlers();

  STDMETHODIMP get_FunctionName(__out BSTR *pVal);

  STDMETHODIMP get_Flags(__out LONG *pVal);

protected:
  friend class CNktHooksEnumImpl;

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktHook), CNktHookImpl)
