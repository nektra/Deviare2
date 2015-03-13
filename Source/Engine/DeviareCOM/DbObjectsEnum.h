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
// CNktDbObjectsEnumImpl

class ATL_NO_VTABLE CNktDbObjectsEnumImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                            public CComCoClass<CNktDbObjectsEnumImpl,
                                                               &CLSID_NktDbObjectsEnum>,
                                            public CNktMemMgrObj,
                                            public IObjectSafetyImpl<CNktDbObjectsEnumImpl,
                                                                     INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                            public CNktObjectBaseImpl<CNktDbObjectsEnumImpl,
                                                       INktDbObjectsEnum, CNktDvDbObjectsEnumerator,
                                                       &CLSID_NktDbObjectsEnum, &IID_INktDbObjectsEnum,
                                                       &LIBID_Deviare2>,
                                            public IInterfaceItemImpl<CNktDbObjectsEnumImpl>,
                                            public INktDvEngBaseComLink
{
public:
  CNktDbObjectsEnumImpl(__in CNktDvDbObjectsEnumerator *lpIntObj) :
                            CComObjectRootEx<CComMultiThreadModel>(),
                            CComCoClass<CNktDbObjectsEnumImpl, &CLSID_NktDbObjectsEnum>(),
                            CNktMemMgrObj(),
                            IObjectSafetyImpl<CNktDbObjectsEnumImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                            CNktObjectBaseImpl<CNktDbObjectsEnumImpl, INktDbObjectsEnum,
                                               CNktDvDbObjectsEnumerator, &CLSID_NktDbObjectsEnum,
                                               &IID_INktDbObjectsEnum, &LIBID_Deviare2>(lpIntObj),
                            IInterfaceItemImpl<CNktDbObjectsEnumImpl>(),
                            INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktDbObjectsEnum",
                                 L"1", L"NktDbObjectsEnum Class", CLSID_NktDbObjectsEnum,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktDbObjectsEnumImpl)
    COM_INTERFACE_ENTRY(INktDbObjectsEnum)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktDbObjectsEnumImpl,CNktDvDbObjectsEnumerator)

public:
  STDMETHODIMP get_Count(__deref_out LONG *pVal);

  STDMETHODIMP GetAt(__in LONG index, __deref_out INktDbObject **ppDbObj);

  STDMETHODIMP First(__deref_out INktDbObject **ppDbObj);

  STDMETHODIMP Last(__deref_out INktDbObject **ppDbObj);

  STDMETHODIMP Next(__deref_out INktDbObject **ppDbObj);

  STDMETHODIMP Previous(__deref_out INktDbObject **ppDbObj);

  STDMETHODIMP GetById(__in LONG _id, __deref_out INktDbObject **ppDbObj);

  STDMETHODIMP GetByName(__in BSTR name, __deref_out INktDbObject **ppDbObj);

  STDMETHODIMP get__NewEnum(__deref_out IUnknown** ppUnk);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktDbObjectsEnum), CNktDbObjectsEnumImpl)
