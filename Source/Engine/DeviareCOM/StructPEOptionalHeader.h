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
// CNktStructPEOptionalHeaderImpl

class ATL_NO_VTABLE CNktStructPEOptionalHeaderImpl : public CComObjectRootEx<CComMultiThreadModel>,
                        public CComCoClass<CNktStructPEOptionalHeaderImpl,
                                           &CLSID_NktStructPEOptionalHeader>,
                        public CNktMemMgrObj,
                        public IObjectSafetyImpl<CNktStructPEOptionalHeaderImpl,
                                                 INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                        public IDispatchImpl<INktStructPEOptionalHeader, &IID_INktStructPEOptionalHeader,
                                             &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                             DEVIARECOM_MinorVersion>,
                        public IInterfaceItemImpl<CNktStructPEOptionalHeaderImpl>
{
public:
  CNktStructPEOptionalHeaderImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                                     CComCoClass<CNktStructPEOptionalHeaderImpl,
                                                 &CLSID_NktStructPEOptionalHeader>(),
                                     CNktMemMgrObj(),
                                     IObjectSafetyImpl<CNktStructPEOptionalHeaderImpl,
                                                       INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                                     IDispatchImpl<INktStructPEOptionalHeader,
                                                   &IID_INktStructPEOptionalHeader, &LIBID_Deviare2,
                                                   DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>(),
                                     IInterfaceItemImpl<CNktStructPEOptionalHeaderImpl>()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktStructPEOptionalHeader",
                                 L"1", L"NktStructPEOptionalHeader Class",
                                 CLSID_NktStructPEOptionalHeader, LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktStructPEOptionalHeaderImpl)
    COM_INTERFACE_ENTRY(INktStructPEOptionalHeader)
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
  STDMETHODIMP get_Magic(__deref_out LONG *pVal);

  STDMETHODIMP get_MajorLinkerVersion(__deref_out short *pVal);

  STDMETHODIMP get_MinorLinkerVersion(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfCode(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfInitializedData(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfUninitializedData(__deref_out LONG *pVal);

  STDMETHODIMP get_AddressOfEntryPoint(__deref_out LONG *pVal);

  STDMETHODIMP get_BaseOfCode(__deref_out LONG *pVal);

  STDMETHODIMP get_ImageBase(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_SectionAlignment(__deref_out LONG *pVal);

  STDMETHODIMP get_FileAlignment(__deref_out LONG *pVal);

  STDMETHODIMP get_MajorOperatingSystemVersion(__deref_out short *pVal);

  STDMETHODIMP get_MinorOperatingSystemVersion(__deref_out short *pVal);

  STDMETHODIMP get_MajorImageVersion(__deref_out short *pVal);

  STDMETHODIMP get_MinorImageVersion(__deref_out short *pVal);

  STDMETHODIMP get_MajorSubsystemVersion(__deref_out short *pVal);

  STDMETHODIMP get_MinorSubsystemVersion(__deref_out short *pVal);

  STDMETHODIMP get_Win32VersionValue(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfImage(__deref_out LONG *pVal);

  STDMETHODIMP get_SizeOfHeaders(__deref_out LONG *pVal);

  STDMETHODIMP get_CheckSum(__deref_out LONG *pVal);

  STDMETHODIMP get_Subsystem(__deref_out short *pVal);

  STDMETHODIMP get_DllCharacteristics(__deref_out short *pVal);

  STDMETHODIMP get_SizeOfStackReserve(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_SizeOfStackCommit(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_SizeOfHeapReserve(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_SizeOfHeapCommit(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_LoaderFlags(__deref_out LONG *pVal);

  STDMETHODIMP get_NumberOfRvaAndSizes(__deref_out LONG *pVal);

  STDMETHODIMP get_DataDirectoryStart(__in LONG index, __deref_out my_ssize_t *pVal);

  STDMETHODIMP get_DataDirectoryEnd(__in LONG index, __deref_out my_ssize_t *pVal);

private:
  friend class CNktProcessImpl;
  friend class CNktModuleImpl;

  HRESULT Initialize(__in NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in SIZE_T nPlatformBits);

  short nMagic;
  short nMajorLinkerVersion;
  short nMinorLinkerVersion;
  LONG nSizeOfCode;
  LONG nSizeOfInitializedData;
  LONG nSizeOfUninitializedData;
  LONG nAddressOfEntryPoint;
  LONG nBaseOfCode;
  my_ssize_t nImageBase;
  LONG nSectionAlignment;
  LONG nFileAlignment;
  short nMajorOperatingSystemVersion;
  short nMinorOperatingSystemVersion;
  short nMajorImageVersion;
  short nMinorImageVersion;
  short nMajorSubsystemVersion;
  short nMinorSubsystemVersion;
  LONG nWin32VersionValue;
  LONG nSizeOfImage;
  LONG nSizeOfHeaders;
  LONG nCheckSum;
  short nSubsystem;
  short nDllCharacteristics;
  my_ssize_t nSizeOfStackReserve;
  my_ssize_t nSizeOfStackCommit;
  my_ssize_t nSizeOfHeapReserve;
  my_ssize_t nSizeOfHeapCommit;
  LONG nLoaderFlags;
  LONG nNumberOfRvaAndSizes;
  struct {
    my_ssize_t nStart;
    my_ssize_t nEnd;
  } aDataDirectory[16];
  //----
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktStructPEOptionalHeader), CNktStructPEOptionalHeaderImpl)
