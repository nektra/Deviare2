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

typedef CNktObjectWrapper<CNktDvDbObject> CNktDvDbObjectWrapper;

//-----------------------------------------------------------
// CNktDbObjectImpl

class ATL_NO_VTABLE CNktDbObjectImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                       public CComCoClass<CNktDbObjectImpl, &CLSID_NktDbObject>,
                                       public CNktMemMgrObj,
                                       public IObjectSafetyImpl<CNktDbObjectImpl,
                                                                INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                       public CNktObjectBaseImpl<CNktDbObjectImpl, INktDbObject,
                                                                 CNktDvDbObjectWrapper, &CLSID_NktDbObject,
                                                                 &IID_INktDbObject, &LIBID_Deviare2>,
                                       public IInterfaceItemImpl<CNktDbObjectImpl>,
                                       public INktDvEngBaseComLink
{
public:
  CNktDbObjectImpl(__in CNktDvDbObjectWrapper *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                       CComCoClass<CNktDbObjectImpl, &CLSID_NktDbObject>(),
                       CNktMemMgrObj(),
                       IObjectSafetyImpl<CNktDbObjectImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                       CNktObjectBaseImpl<CNktDbObjectImpl, INktDbObject, CNktDvDbObjectWrapper,
                                          &CLSID_NktDbObject, &IID_INktDbObject,
                                          &LIBID_Deviare2>(lpIntObj),
                       IInterfaceItemImpl<CNktDbObjectImpl>(),
                       INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktDbObject",
                                 L"1", L"NktDbObject Class", CLSID_NktDbObject,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktDbObjectImpl)
    COM_INTERFACE_ENTRY(INktDbObject)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktDbObjectImpl,CNktDvDbObjectWrapper)

public:
  STDMETHODIMP get_Id(__deref_out LONG *pVal);

  STDMETHODIMP get_Class(__deref_out eNktDboClass *pVal);

  STDMETHODIMP get_FundamentalType(__deref_out eNktDboFundamentalType *pVal);

  STDMETHODIMP get_Declaration(__deref_out BSTR *pVal);

  STDMETHODIMP get_Name(__deref_out BSTR *pVal);

  STDMETHODIMP get_Size(__deref_out LONG *pVal);

  STDMETHODIMP get_Align(__deref_out LONG *pVal);

  STDMETHODIMP get_IsArray(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_ArrayMaxHint(__deref_out LONG *pVal);

  STDMETHODIMP get_IsAnsiString(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_IsWideString(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_IsGuid(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP Evaluate(__deref_out INktDbObject **ppDbObject);

  STDMETHODIMP IndexedEvaluate(__in LONG index,__deref_out INktDbObject **ppDbObject);

  STDMETHODIMP FullEvaluate(__deref_out INktDbObject **ppDbObject);

  STDMETHODIMP get_Flags(__deref_out LONG *pVal);

  STDMETHODIMP get_ItemsCount(__deref_out LONG *pVal);

  STDMETHODIMP NameToIndex(__in BSTR name, __deref_out LONG *pVal);

  STDMETHODIMP Items(__deref_out INktDbObjectsEnum **ppDbObjEnum);

  STDMETHODIMP Item(__in LONG index, __deref_out INktDbObject **ppDbObject);

  STDMETHODIMP ItemName(__in LONG index, __deref_out BSTR *pVal);

  STDMETHODIMP ItemFlags(__in LONG index, __deref_out LONG *pVal);

  STDMETHODIMP ItemMemberType(__in LONG index, __deref_out eNktDboItemMemberType *pVal);

  STDMETHODIMP ItemStructOffset(__in LONG index, __deref_out LONG *pVal);

  STDMETHODIMP ItemStructBits(__in LONG index, __deref_out LONG *pVal);

  STDMETHODIMP ItemEnumValue(__in LONG index, __deref_out my_ssize_t *pVal);

  STDMETHODIMP ItemDeclaration(__in LONG index, __deref_out BSTR *pVal);

  STDMETHODIMP get_FuncCallingConvention(__deref_out eNktDboCallingConvention *pVal);

  STDMETHODIMP get_FunctionReturn(__deref_out INktDbObject **ppDbObject);

  STDMETHODIMP get_FunctionReturnFlags(__deref_out LONG *pVal);

  STDMETHODIMP get_FunctionReturnDeclaration(__deref_out BSTR *pVal);

  STDMETHODIMP DbModules(__deref_out INktDbModulesEnum **ppDbModsEnum);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktDbObject), CNktDbObjectImpl)
