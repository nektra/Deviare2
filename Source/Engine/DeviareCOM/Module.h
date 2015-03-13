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
#include "StructPEFileHeader.h"
#include "StructPEOptionalHeader.h"
#include "StructPESections.h"

//-----------------------------------------------------------
// CNktModuleImpl

class ATL_NO_VTABLE CNktModuleImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                     public CComCoClass<CNktModuleImpl, &CLSID_NktModule>,
                                     public CNktMemMgrObj,
                                     public IObjectSafetyImpl<CNktModuleImpl,
                                                              INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                     public CNktObjectBaseImpl<CNktModuleImpl, INktModule, CNktDvModule,
                                                               &CLSID_NktModule, &IID_INktModule,
                                                               &LIBID_Deviare2>,
                                     public IInterfaceItemImpl<CNktModuleImpl>,
                                     public INktDvEngBaseComLink
{
public:
  CNktModuleImpl(__in CNktDvModule *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                     CComCoClass<CNktModuleImpl, &CLSID_NktModule>(),
                     CNktMemMgrObj(),
                     IObjectSafetyImpl<CNktModuleImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                     CNktObjectBaseImpl<CNktModuleImpl, INktModule, CNktDvModule, &CLSID_NktModule,
                                        &IID_INktModule, &LIBID_Deviare2>(lpIntObj),
                     IInterfaceItemImpl<CNktModuleImpl>(),
                     INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktModule",
                                 L"1", L"NktModule Class", CLSID_NktModule,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktModuleImpl)
    COM_INTERFACE_ENTRY(INktModule)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktModuleImpl,CNktDvModule)

public:
  STDMETHODIMP get_ProcessId(__deref_out LONG *pVal);

  STDMETHODIMP get_PlatformBits(__deref_out LONG *pVal);

  STDMETHODIMP get_BaseAddress(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Size(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Name(__deref_out BSTR *pVal);

  STDMETHODIMP get_Path(__deref_out BSTR *pVal);

  STDMETHODIMP get_FileName(__deref_out BSTR *pVal);

  STDMETHODIMP get_Handle(__deref_out my_ssize_t *pVal);

  STDMETHODIMP ExportedFunctions(__deref_out INktExportedFunctionsEnum **ppExpFunc);

  STDMETHODIMP FunctionByOrdinal(__in LONG ordinal, __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP FunctionByName(__in BSTR name, __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP FunctionByAddress(__in my_ssize_t addr, __in VARIANT_BOOL searchNearest,
                                 __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP FileHeader(__deref_out INktStructPEFileHeader **ppVal);

  STDMETHODIMP OptionalHeader(__deref_out INktStructPEOptionalHeader **ppVal);

  STDMETHODIMP Sections(__deref_out INktStructPESections **ppVal);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktModule), CNktModuleImpl)
