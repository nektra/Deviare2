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
// CNktProcessesEnumImpl

class ATL_NO_VTABLE CNktProcessesEnumImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                            public CComCoClass<CNktProcessesEnumImpl,
                                                               &CLSID_NktProcessesEnum>,
                                            public CNktMemMgrObj,
                                            public IObjectSafetyImpl<CNktProcessesEnumImpl,
                                                                     INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                            public CNktObjectBaseImpl<CNktProcessesEnumImpl,
                                                      INktProcessesEnum, CNktDvProcessesEnumerator,
                                                      &CLSID_NktProcessesEnum,
                                                      &IID_INktProcessesEnum, &LIBID_Deviare2>,
                                            public IInterfaceItemImpl<CNktProcessesEnumImpl>,
                                            public INktDvEngBaseComLink
{
public:
  CNktProcessesEnumImpl(__in CNktDvProcessesEnumerator *lpIntObj) :
                            CComObjectRootEx<CComMultiThreadModel>(),
                            CComCoClass<CNktProcessesEnumImpl, &CLSID_NktProcessesEnum>(),
                            CNktMemMgrObj(),
                            IObjectSafetyImpl<CNktProcessesEnumImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                            CNktObjectBaseImpl<CNktProcessesEnumImpl, INktProcessesEnum,
                                               CNktDvProcessesEnumerator, &CLSID_NktProcessesEnum,
                                               &IID_INktProcessesEnum, &LIBID_Deviare2>(lpIntObj),
                            IInterfaceItemImpl<CNktProcessesEnumImpl>(),
                            INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktProcessesEnum",
                                 L"1", L"NktProcessesEnum Class", CLSID_NktProcessesEnum,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktProcessesEnumImpl)
    COM_INTERFACE_ENTRY(INktProcessesEnum)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktProcessesEnumImpl,CNktDvProcessesEnumerator)

public:
  STDMETHODIMP get_Count(__deref_out LONG *pVal);

  STDMETHODIMP GetAt(__in LONG index, __deref_out INktProcess **ppProc);

  STDMETHODIMP First(__deref_out INktProcess **ppProc);

  STDMETHODIMP Last(__deref_out INktProcess **ppProc);

  STDMETHODIMP Next(__deref_out INktProcess **ppProc);

  STDMETHODIMP Previous(__deref_out INktProcess **ppProc);

  STDMETHODIMP GetByDeviareId(__in my_ssize_t devId, __deref_out INktProcess **ppProc);

  STDMETHODIMP GetById(__in LONG _id, __deref_out INktProcess **ppProc);

  STDMETHODIMP GetByName(__in BSTR name, __deref_out INktProcess **ppProc);

  STDMETHODIMP get__NewEnum(__deref_out IUnknown** ppUnk);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktProcessesEnum), CNktProcessesEnumImpl)
