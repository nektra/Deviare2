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
// CNktProcessMemoryImpl

class ATL_NO_VTABLE CNktProcessMemoryImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                            public CComCoClass<CNktProcessMemoryImpl,
                                                               &CLSID_NktProcessMemory>,
                                            public CNktMemMgrObj,
                                            public IObjectSafetyImpl<CNktProcessMemoryImpl,
                                                                     INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                            public CNktObjectBaseImpl<CNktProcessMemoryImpl,
                                                       INktProcessMemory, CNktDvProcessMemory,
                                                       &CLSID_NktProcessMemory, &IID_INktProcessMemory,
                                                       &LIBID_Deviare2>,
                                            public IInterfaceItemImpl<CNktProcessMemoryImpl>,
                                            public INktDvEngBaseComLink
{
public:
  CNktProcessMemoryImpl(__in CNktDvProcessMemory *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                            CComCoClass<CNktProcessMemoryImpl, &CLSID_NktProcessMemory>(),
                            CNktMemMgrObj(),
                            IObjectSafetyImpl<CNktProcessMemoryImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                            CNktObjectBaseImpl<CNktProcessMemoryImpl, INktProcessMemory,
                                               CNktDvProcessMemory, &CLSID_NktProcessMemory,
                                               &IID_INktProcessMemory, &LIBID_Deviare2>(lpIntObj),
                            IInterfaceItemImpl<CNktProcessMemoryImpl>(),
                            INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktProcessMemory",
                                 L"1", L"NktProcessMemory Class", CLSID_NktProcessMemory,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktProcessMemoryImpl)
    COM_INTERFACE_ENTRY(INktProcessMemory)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktProcessMemoryImpl,CNktDvProcessMemory)

public:
  STDMETHODIMP GetProtection(__in my_ssize_t remoteAddr, __in VARIANT_BOOL allocated,
                             __deref_out eNktProtection *pVal);

  STDMETHODIMP ReadMem(__in my_ssize_t localAddr, __in my_ssize_t remoteAddr, __in my_ssize_t bytes,
                       __deref_out my_ssize_t *pReaded);

  STDMETHODIMP WriteMem(__in my_ssize_t remoteAddr, __in my_ssize_t localAddr, __in my_ssize_t bytes,
                        __deref_out my_ssize_t *pWritten);

  STDMETHODIMP Read(__in my_ssize_t remoteAddr, __in eNktDboFundamentalType type,
                    __deref_out VARIANT *pVal);

  STDMETHODIMP ReadArray(__in my_ssize_t remoteAddr, __in eNktDboFundamentalType type, __in LONG count,
                         __deref_out VARIANT *pVal);

  STDMETHODIMP Write(__in my_ssize_t remoteAddr, __in VARIANT var);

  STDMETHODIMP ReadString(__in my_ssize_t remoteAddr, __in VARIANT_BOOL isAnsi, __deref_out BSTR *pVal);

  STDMETHODIMP ReadStringN(__in my_ssize_t remoteAddr, __in VARIANT_BOOL isAnsi, __in LONG maxChars,
                           __deref_out BSTR *pVal);

  STDMETHODIMP WriteString(__in my_ssize_t remoteAddr, __in BSTR str, __in VARIANT_BOOL asAnsi,
                           __in VARIANT_BOOL writeNul);

  STDMETHODIMP StringLength(__in my_ssize_t remoteAddr, __in VARIANT_BOOL asAnsi,
                            __deref_out LONG *pVal);

  STDMETHODIMP get_CharVal(__in my_ssize_t remoteAddr, __deref_out signed char *pVal);

  STDMETHODIMP put_CharVal(__in my_ssize_t remoteAddr, __in signed char newValue);

  STDMETHODIMP get_ByteVal(__in my_ssize_t remoteAddr, __deref_out unsigned char *pVal);

  STDMETHODIMP put_ByteVal(__in my_ssize_t remoteAddr, __in unsigned char newValue);

  STDMETHODIMP get_ShortVal(__in my_ssize_t remoteAddr, __deref_out short *pVal);

  STDMETHODIMP put_ShortVal(__in my_ssize_t remoteAddr, __in short newValue);

  STDMETHODIMP get_UShortVal(__in my_ssize_t remoteAddr, __deref_out unsigned short *pVal);

  STDMETHODIMP put_UShortVal(__in my_ssize_t remoteAddr, __in unsigned short newValue);

  STDMETHODIMP get_LongVal(__in my_ssize_t remoteAddr, __deref_out long *pVal);

  STDMETHODIMP put_LongVal(__in my_ssize_t remoteAddr, __in long newValue);

  STDMETHODIMP get_ULongVal(__in my_ssize_t remoteAddr, __deref_out unsigned long *pVal);

  STDMETHODIMP put_ULongVal(__in my_ssize_t remoteAddr, __in unsigned long newValue);

  STDMETHODIMP get_LongLongVal(__in my_ssize_t remoteAddr, __deref_out __int64 *pVal);

  STDMETHODIMP put_LongLongVal(__in my_ssize_t remoteAddr, __in __int64 newValue);

  STDMETHODIMP get_ULongLongVal(__in my_ssize_t remoteAddr, __deref_out unsigned __int64 *pVal);

  STDMETHODIMP put_ULongLongVal(__in my_ssize_t remoteAddr, __in unsigned __int64 newValue);

  STDMETHODIMP get_SSizeTVal(__in my_ssize_t remoteAddr, __deref_out my_ssize_t *pVal);

  STDMETHODIMP put_SSizeTVal(__in my_ssize_t remoteAddr, __in my_ssize_t newValue);

  STDMETHODIMP get_SizeTVal(__in my_ssize_t remoteAddr, __deref_out my_size_t *pVal);

  STDMETHODIMP put_SizeTVal(__in my_ssize_t remoteAddr, __in my_size_t newValue);

  STDMETHODIMP get_FloatVal(__in my_ssize_t remoteAddr, __deref_out float *pVal);

  STDMETHODIMP put_FloatVal(__in my_ssize_t remoteAddr, __in float newValue);

  STDMETHODIMP get_DoubleVal(__in my_ssize_t remoteAddr, __deref_out double *pVal);

  STDMETHODIMP put_DoubleVal(__in my_ssize_t remoteAddr, __in double newValue);

  STDMETHODIMP BuildParam(__in my_ssize_t remoteAddr,__in BSTR type, __deref_out INktParam **ppParam);

  STDMETHODIMP AllocMem(__in my_ssize_t bytes, __in VARIANT_BOOL executeFlag,
                        __deref_out my_ssize_t *pVal);

  STDMETHODIMP FreeMem(__in my_ssize_t remoteAddr);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktProcessMemory), CNktProcessMemoryImpl)
