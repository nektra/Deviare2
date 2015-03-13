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

//-----------------------------------------------------------
// CNktModulesEnumImpl

class ATL_NO_VTABLE CNktModulesEnumImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                          public CComCoClass<CNktModulesEnumImpl, &CLSID_NktModulesEnum>,
                                          public CNktMemMgrObj,
                                          public IObjectSafetyImpl<CNktModulesEnumImpl,
                                                                   INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                          public CNktObjectBaseImpl<CNktModulesEnumImpl, INktModulesEnum,
                                                     CNktDvModulesEnumerator, &CLSID_NktModulesEnum,
                                                     &IID_INktModulesEnum, &LIBID_Deviare2>,
                                          public IInterfaceItemImpl<CNktModulesEnumImpl>,
                                          public INktDvEngBaseComLink
{
public:
  CNktModulesEnumImpl(__in CNktDvModulesEnumerator *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                          CComCoClass<CNktModulesEnumImpl, &CLSID_NktModulesEnum>(),
                          CNktMemMgrObj(),
                          IObjectSafetyImpl<CNktModulesEnumImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                          CNktObjectBaseImpl<CNktModulesEnumImpl, INktModulesEnum, CNktDvModulesEnumerator,
                                             &CLSID_NktModulesEnum, &IID_INktModulesEnum,
                                             &LIBID_Deviare2>(lpIntObj),
                          IInterfaceItemImpl<CNktModulesEnumImpl>(),
                          INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktModulesEnum",
                                 L"1", L"NktModulesEnum Class", CLSID_NktModulesEnum,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktModulesEnumImpl)
    COM_INTERFACE_ENTRY(INktModulesEnum)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktModulesEnumImpl,CNktDvModulesEnumerator)

public:
  STDMETHODIMP get_Count(__deref_out LONG *pVal);

  STDMETHODIMP GetAt(__in LONG index, __deref_out INktModule **ppMod);

  STDMETHODIMP First(__deref_out INktModule **ppMod);

  STDMETHODIMP Last(__deref_out INktModule **ppMod);

  STDMETHODIMP Next(__deref_out INktModule **ppMod);

  STDMETHODIMP Previous(__deref_out INktModule **ppMod);

  STDMETHODIMP GetByDeviareId(__in my_ssize_t devId, __deref_out INktModule **ppMod);

  STDMETHODIMP GetByName(__in BSTR dllName, __deref_out INktModule **ppMod);

  STDMETHODIMP GetByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                            __deref_out INktModule **ppMod);

  STDMETHODIMP FunctionByName(__in BSTR dllFunc, __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP FunctionByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                                 __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP get__NewEnum(__deref_out IUnknown** ppUnk);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktModulesEnum), CNktModulesEnumImpl)
