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
#include "DllMain.h"

//-----------------------------------------------------------
// CNktToolsImpl

class ATL_NO_VTABLE CNktToolsImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                    public CComCoClass<CNktToolsImpl, &CLSID_NktTools>,
                                    public CNktMemMgrObj,
                                    public IObjectSafetyImpl<CNktToolsImpl,
                                                             INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                    public IDispatchImpl<INktTools, &IID_INktTools, &LIBID_Deviare2,
                                                         DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>,
                                    public IInterfaceItemImpl<CNktToolsImpl>
{
public:
  CNktToolsImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                    CComCoClass<CNktToolsImpl, &CLSID_NktTools>(),
                    CNktMemMgrObj(),
                    IObjectSafetyImpl<CNktToolsImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                    IDispatchImpl<INktTools, &IID_INktTools, &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                  DEVIARECOM_MinorVersion>(),
                    IInterfaceItemImpl<CNktToolsImpl>()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktTools",
                                 L"1", L"NktTools Class", CLSID_NktTools,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktToolsImpl)
    COM_INTERFACE_ENTRY(INktTools)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
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

  HRESULT InvalidateInterface()
    {
    return S_OK;
    };

public:
  STDMETHODIMP ExtractIcon(__in BSTR executablePath, __in LONG index, __deref_out IPicture **ppIcon);

  STDMETHODIMP GetFileNameFromHandle(__in my_ssize_t _handle, __in VARIANT procOrId,
                                     __deref_out BSTR *pVal);

  STDMETHODIMP LocateFunctionSymbolInPdb(__in BSTR fileName, __in BSTR functionName,
                                         __in BSTR symbolServerPath, __in BSTR localCachePath,
                                         __deref_out INktPdbFunctionSymbol **ppSymbol);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_AUTO(__uuidof(NktTools), CNktToolsImpl)
