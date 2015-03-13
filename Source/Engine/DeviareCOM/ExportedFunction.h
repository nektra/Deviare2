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
// CNktExportedFunctionImpl

class ATL_NO_VTABLE CNktExportedFunctionImpl : public CComObjectRootEx<CComMultiThreadModel>,
                         public CComCoClass<CNktExportedFunctionImpl, &CLSID_NktExportedFunction>,
                         public CNktMemMgrObj,
                         public IObjectSafetyImpl<CNktExportedFunctionImpl,
                                                  INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                         public CNktObjectBaseImpl<CNktExportedFunctionImpl, INktExportedFunction,
                                                   CNktDvExportedFunction, &CLSID_NktExportedFunction,
                                                   &IID_INktExportedFunction, &LIBID_Deviare2>,
                         public IInterfaceItemImpl<CNktExportedFunctionImpl>,
                         public INktDvEngBaseComLink
{
public:
  CNktExportedFunctionImpl(__in CNktDvExportedFunction *lpIntObj) :
                               CComObjectRootEx<CComMultiThreadModel>(),
                               CComCoClass<CNktExportedFunctionImpl, &CLSID_NktExportedFunction>(),
                               CNktMemMgrObj(),
                               IObjectSafetyImpl<CNktExportedFunctionImpl,
                                                 INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                               CNktObjectBaseImpl<CNktExportedFunctionImpl, INktExportedFunction,
                                                  CNktDvExportedFunction, &CLSID_NktExportedFunction,
                                                  &IID_INktExportedFunction, &LIBID_Deviare2>(lpIntObj),
                               IInterfaceItemImpl<CNktExportedFunctionImpl>(),
                               INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktExportedFunction",
                                 L"1", L"NktExportedFunction Class", CLSID_NktExportedFunction,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktExportedFunctionImpl)
    COM_INTERFACE_ENTRY(INktExportedFunction)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktExportedFunctionImpl,CNktDvExportedFunction)

public:
  STDMETHODIMP get_Ordinal(__deref_out LONG *pVal);

  STDMETHODIMP get_Name(__deref_out BSTR *pVal);

  STDMETHODIMP get_ModuleName(__deref_out BSTR *pVal);

  STDMETHODIMP get_CompleteName(__deref_out BSTR *pVal);

  STDMETHODIMP get_IsForwarded(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_ForwardedApiName(__deref_out BSTR *pVal);

  STDMETHODIMP get_ForwardedModuleName(__deref_out BSTR *pVal);

  STDMETHODIMP get_ForwardedCompleteName(__deref_out BSTR *pVal);

  STDMETHODIMP get_AddrBase(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_AddrOffset(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Addr(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_CallingConv(__deref_out eNktDboCallingConvention *pVal);

  STDMETHODIMP DbObject(__deref_out INktDbObject **ppDbObj);

  STDMETHODIMP get_HasDeclaration(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_Declaration(__deref_out BSTR *pVal);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktExportedFunction), CNktExportedFunctionImpl)
