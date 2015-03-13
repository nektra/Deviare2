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
// CNktStructPESectionsImpl

class ATL_NO_VTABLE CNktStructPESectionsImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                               public CComCoClass<CNktStructPESectionsImpl,
                                                                  &CLSID_NktStructPESections>,
                                               public CNktMemMgrObj,
                                               public IObjectSafetyImpl<CNktStructPESectionsImpl,
                                                          INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                               public IDispatchImpl<INktStructPESections,
                                                        &IID_INktStructPESections, &LIBID_Deviare2,
                                                        DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>,
                                               public IInterfaceItemImpl<CNktStructPESectionsImpl>
{
public:
  CNktStructPESectionsImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                               CComCoClass<CNktStructPESectionsImpl, &CLSID_NktStructPESections>(),
                               CNktMemMgrObj(),
                               IObjectSafetyImpl<CNktStructPESectionsImpl,
                                                 INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                               IDispatchImpl<INktStructPESections, &IID_INktStructPESections,
                                             &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                             DEVIARECOM_MinorVersion>(),
                               IInterfaceItemImpl<CNktStructPESectionsImpl>()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktStructPESections",
                                 L"1", L"NktStructPESections Class", CLSID_NktStructPESections,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktStructPESectionsImpl)
    COM_INTERFACE_ENTRY(INktStructPESections)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
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
  STDMETHODIMP get_Count(__deref_out LONG *pVal);

  STDMETHODIMP get_Name(__in LONG index, __deref_out BSTR *pVal);

  STDMETHODIMP get_StartAddress(__in LONG index, __deref_out my_ssize_t *pVal);

  STDMETHODIMP get_EndAddress(__in LONG index, __deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Characteristics(__in LONG index, __deref_out LONG *pVal);

private:
  friend class CNktProcessImpl;
  friend class CNktModuleImpl;

  HRESULT Initialize(__in CNktDvTools::PROCESSSECTION *lpSections, __in SIZE_T nSecCount);

  typedef struct tagSECTION {
    WCHAR szNameW[10];
    my_ssize_t nStartAddress, nEndAddress;
    LONG nCharacteristics;
  } SECTION;

  TNktArrayList4Structs<SECTION> aSectionsList;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktStructPESections), CNktStructPESectionsImpl)
