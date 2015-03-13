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
// CNktParamsEnumImpl

class ATL_NO_VTABLE CNktParamsEnumImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                         public CComCoClass<CNktParamsEnumImpl, &CLSID_NktParamsEnum>,
                                         public CNktMemMgrObj,
                                         public IObjectSafetyImpl<CNktParamsEnumImpl,
                                                                  INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                         public CNktObjectBaseImpl<CNktParamsEnumImpl, INktParamsEnum,
                                                    CNktDvParamsEnumerator, &CLSID_NktParamsEnum,
                                                    &IID_INktParamsEnum, &LIBID_Deviare2>,
                                         public IInterfaceItemImpl<CNktParamsEnumImpl>,
                                         public INktDvEngBaseComLink
{
public:
  CNktParamsEnumImpl(__in CNktDvParamsEnumerator *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                         CComCoClass<CNktParamsEnumImpl, &CLSID_NktParamsEnum>(),
                         CNktMemMgrObj(),
                         IObjectSafetyImpl<CNktParamsEnumImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                         CNktObjectBaseImpl<CNktParamsEnumImpl, INktParamsEnum, CNktDvParamsEnumerator,
                                            &CLSID_NktParamsEnum, &IID_INktParamsEnum,
                                            &LIBID_Deviare2>(lpIntObj),
                         IInterfaceItemImpl<CNktParamsEnumImpl>(),
                         INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktParamsEnum",
                                 L"1", L"NktParamsEnum Class", CLSID_NktParamsEnum,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktParamsEnumImpl)
    COM_INTERFACE_ENTRY(INktParamsEnum)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktParamsEnumImpl,CNktDvParamsEnumerator)

public:
  STDMETHODIMP get_Count(__deref_out LONG *pVal);

  STDMETHODIMP GetAt(__in LONG index, __deref_out INktParam **ppParam);

  STDMETHODIMP First(__deref_out INktParam **ppParam);

  STDMETHODIMP Last(__deref_out INktParam **ppParam);

  STDMETHODIMP Next(__deref_out INktParam **ppParam);

  STDMETHODIMP Previous(__deref_out INktParam **ppParam);

  STDMETHODIMP GetByDeviareId(__in my_ssize_t devId, __deref_out INktParam **ppParam);

  STDMETHODIMP get__NewEnum(__deref_out IUnknown** ppUnk);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktParamsEnum), CNktParamsEnumImpl)
