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
#include "ProcessEvents.h"
#include "StructPEFileHeader.h"
#include "StructPEOptionalHeader.h"
#include "StructPESections.h"

//-----------------------------------------------------------
// CNktProcessImpl

class ATL_NO_VTABLE CNktProcessImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                      public CComCoClass<CNktProcessImpl, &CLSID_NktProcess>,
                                      public CNktMemMgrObj,
                                      public IObjectSafetyImpl<CNktProcessImpl,
                                                               INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                      public CNktProcessEventsImpl<CNktProcessImpl>,
                                      public IConnectionPointContainerImpl<CNktProcessImpl>,
                                      public IProvideClassInfo2Impl<&CLSID_NktProcess,
                                                 &__uuidof(DNktProcessEvents), &LIBID_Deviare2,
                                                 DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>,
                                      public CNktObjectBaseImpl<CNktProcessImpl, INktProcess,
                                                                CNktDvProcess, &CLSID_NktProcess,
                                                                &IID_INktProcess, &LIBID_Deviare2>,
                                      public IInterfaceItemImpl<CNktProcessImpl>,
                                      public INktDvEngBaseComLink
{
public:
  CNktProcessImpl(__in CNktDvProcess *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                      CComCoClass<CNktProcessImpl, &CLSID_NktProcess>(),
                      CNktMemMgrObj(),
                      IObjectSafetyImpl<CNktProcessImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                      CNktProcessEventsImpl<CNktProcessImpl>(),
                      IConnectionPointContainerImpl<CNktProcessImpl>(),
                      IProvideClassInfo2Impl<&CLSID_NktProcess, &__uuidof(DNktProcessEvents),
                                             &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                             DEVIARECOM_MinorVersion>(),
                      CNktObjectBaseImpl<CNktProcessImpl, INktProcess, CNktDvProcess,
                                         &CLSID_NktProcess, &IID_INktProcess, &LIBID_Deviare2>(lpIntObj),
                      IInterfaceItemImpl<CNktProcessImpl>(),
                      INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktProcess",
                                 L"1", L"NktProcess Class", CLSID_NktProcess,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktProcessImpl)
    COM_INTERFACE_ENTRY(INktProcess)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IProvideClassInfo2)
    COM_INTERFACE_ENTRY(IProvideClassInfo)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IConnectionPointContainer)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  BEGIN_CONNECTION_POINT_MAP(CNktProcessImpl)
    CONNECTION_POINT_ENTRY(__uuidof(DNktProcessEvents))
  END_CONNECTION_POINT_MAP()

  OBJBASE_DECLARE_APIS_1(CNktProcessImpl,CNktDvProcess)

public:
  STDMETHODIMP get_Id(__deref_out LONG *pVal);

  STDMETHODIMP get_ParentId(__deref_out LONG *pVal);

  STDMETHODIMP get_PlatformBits(__deref_out LONG *pVal);

  STDMETHODIMP get_ThreadsCount(__deref_out LONG *pVal);

  STDMETHODIMP get_PriorityBaseClass(__deref_out LONG *pVal);

  STDMETHODIMP get_Path(__deref_out BSTR *pVal);

  STDMETHODIMP get_Name(__deref_out BSTR *pVal);

  STDMETHODIMP get_UserName(__deref_out BSTR *pVal);

  STDMETHODIMP get_DomainName(__deref_out BSTR *pVal);

  STDMETHODIMP get_LogonSID(__deref_out BSTR *pVal);

  STDMETHODIMP get_Icon(__deref_out IPicture **ppIcon);

  STDMETHODIMP get_DefaultIcon(__deref_out IPicture **ppIcon);

  STDMETHODIMP get_IsActive(__in LONG waitTimeMs, __deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP Terminate(__in LONG exitCode);

  STDMETHODIMP Modules(__deref_out INktModulesEnum **ppModEnum);

  STDMETHODIMP ModuleByName(__in BSTR name, __deref_out INktModule **ppMod);

  STDMETHODIMP ModuleByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                                            __deref_out INktModule **ppMod);

  STDMETHODIMP FunctionByName(__in BSTR name, __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP FunctionByAddress(__in my_ssize_t addr, __in eNktSearchMode searchMode,
                                 __deref_out INktExportedFunction **ppExpFunc);

  STDMETHODIMP Handle(__in LONG desiredAccess, __deref_out my_ssize_t *pVal);

  STDMETHODIMP FileHeader(__deref_out INktStructPEFileHeader **ppVal);

  STDMETHODIMP OptionalHeader(__deref_out INktStructPEOptionalHeader **ppVal);

  STDMETHODIMP Sections(__deref_out INktStructPESections **ppVal);

  STDMETHODIMP get_Privilege(__in BSTR privilegeName, __deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP put_Privilege(__in BSTR privilegeName, __in VARIANT_BOOL newValue);

  STDMETHODIMP Memory(__deref_out INktProcessMemory **ppProcMem);

  STDMETHODIMP InvalidateCache(__in my_ssize_t hDll);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktProcess), CNktProcessImpl)
