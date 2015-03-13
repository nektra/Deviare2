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
// CNktStructPEFileHeaderImpl

class ATL_NO_VTABLE CNktStructPEFileHeaderImpl : public CComObjectRootEx<CComMultiThreadModel>,
                        public CComCoClass<CNktStructPEFileHeaderImpl, &CLSID_NktStructPEFileHeader>,
                        public CNktMemMgrObj,
                        public IObjectSafetyImpl<CNktStructPEFileHeaderImpl,
                                                 INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                        public IDispatchImpl<INktStructPEFileHeader, &IID_INktStructPEFileHeader,
                                             &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                             DEVIARECOM_MinorVersion>,
                        public IInterfaceItemImpl<CNktStructPEFileHeaderImpl>
{
public:
  CNktStructPEFileHeaderImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                                 CComCoClass<CNktStructPEFileHeaderImpl, &CLSID_NktStructPEFileHeader>(),
                                 CNktMemMgrObj(),
                                 IObjectSafetyImpl<CNktStructPEFileHeaderImpl,
                                                   INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                                 IDispatchImpl<INktStructPEFileHeader, &IID_INktStructPEFileHeader,
                                               &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                               DEVIARECOM_MinorVersion>(),
                                 IInterfaceItemImpl<CNktStructPEFileHeaderImpl>()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktStructPEFileHeader",
                                 L"1", L"NktStructPEFileHeader Class", CLSID_NktStructPEFileHeader,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktStructPEFileHeaderImpl)
    COM_INTERFACE_ENTRY(INktStructPEFileHeader)
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
  STDMETHODIMP get_Machine(__deref_out short *pVal);

  STDMETHODIMP get_NumberOfSections(__deref_out LONG *pVal);

  STDMETHODIMP get_TimeDateStamp(__deref_out LONG *pVal);

  STDMETHODIMP get_PointerToSymbolTable(__deref_out LONG *pVal);

  STDMETHODIMP get_NumberOfSymbols(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfOptionalHeader(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Characteristics(__deref_out short *pVal);

private:
  friend class CNktProcessImpl;
  friend class CNktModuleImpl;

  HRESULT Initialize(__in NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in SIZE_T nPlatformBits);

  short nMachine;
  LONG nNumberOfSections;
  LONG nTimeDateStamp;
  LONG nPointerToSymbolTable;
  LONG nNumberOfSymbols;
  my_ssize_t nSizeOfOptionalHeader;
  short nCharacteristics;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktStructPEFileHeader), CNktStructPEFileHeaderImpl)
