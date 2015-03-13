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
// CNktParamImpl

class ATL_NO_VTABLE CNktParamImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                    public CComCoClass<CNktParamImpl, &CLSID_NktParam>,
                                    public CNktMemMgrObj,
                                    public IObjectSafetyImpl<CNktParamImpl,
                                                             INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                    public CNktObjectBaseImpl<CNktParamImpl, INktParam, CNktDvParam,
                                                              &CLSID_NktParam, &IID_INktParam,
                                                              &LIBID_Deviare2>,
                                    public IInterfaceItemImpl<CNktParamImpl>,
                                    public INktDvEngBaseComLink
{
public:
  CNktParamImpl(__in CNktDvParam *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                    CComCoClass<CNktParamImpl, &CLSID_NktParam>(),
                    CNktMemMgrObj(),
                    IObjectSafetyImpl<CNktParamImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                    CNktObjectBaseImpl<CNktParamImpl, INktParam, CNktDvParam, &CLSID_NktParam,
                                       &IID_INktParam, &LIBID_Deviare2>(lpIntObj),
                    IInterfaceItemImpl<CNktParamImpl>(),
                    INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktParam",
                                 L"1", L"NktParam Class", CLSID_NktParam,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktParamImpl)
    COM_INTERFACE_ENTRY(INktParam)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktParamImpl,CNktDvParam)

public:
  STDMETHODIMP get_Address(__deref_out my_ssize_t *pVal);

  STDMETHODIMP get_Class(__deref_out eNktDboClass *pVal);

  STDMETHODIMP get_BasicType(__deref_out eNktDboFundamentalType *pVal);

  STDMETHODIMP get_Name(__deref_out BSTR *pVal);

  STDMETHODIMP get_TypeName(__deref_out BSTR *pVal);

  STDMETHODIMP get_Declaration(__deref_out BSTR *pVal);

  STDMETHODIMP get_Flags(__deref_out LONG *pVal);

  STDMETHODIMP get_IsPointer(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_IsArray(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_ArrayMaxHint(__deref_out LONG *pVal);

  STDMETHODIMP get_IsAnsiString(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_IsWideString(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_GuidString(__deref_out BSTR *pVal);

  STDMETHODIMP get_IntResourceString(__deref_out LONG *pVal);

  STDMETHODIMP Evaluate(__deref_out INktParam **ppParam);

  STDMETHODIMP IndexedEvaluate(__in LONG index, __deref_out INktParam **ppParam);

  STDMETHODIMP FullEvaluate(__deref_out INktParam **ppParam);

  STDMETHODIMP get_FieldsCount(__deref_out LONG *pVal);

  STDMETHODIMP Field(__in VARIANT indexOrName, __deref_out INktParam **ppParam);

  STDMETHODIMP Fields(__deref_out INktParamsEnum **ppParamsEnum);

  STDMETHODIMP get_Value(__deref_out VARIANT *pVal);

  STDMETHODIMP put_Value(__in VARIANT var);

  STDMETHODIMP get_ValueAt(__in LONG index, __deref_out VARIANT *pVal);

  STDMETHODIMP put_ValueAt(__in LONG index, __in VARIANT var);

  STDMETHODIMP get_ValueArray(__in LONG count, __deref_out VARIANT *pVal);

  STDMETHODIMP get_ValueArrayAt(__in LONG index, __in LONG count, __deref_out VARIANT *pVal);

  STDMETHODIMP ReadString(__deref_out BSTR *pVal);

  STDMETHODIMP ReadStringN(__in LONG maxChars, __deref_out BSTR *pVal);

  STDMETHODIMP WriteString(__in BSTR str, __in VARIANT_BOOL isAnsi, __in VARIANT_BOOL writeNul);

  STDMETHODIMP StringLength(__deref_out LONG *pVal);

  STDMETHODIMP get_CharVal(__deref_out signed char *pVal);

  STDMETHODIMP put_CharVal(__in signed char newValue);

  STDMETHODIMP get_ByteVal(__deref_out unsigned char *pVal);

  STDMETHODIMP put_ByteVal(__in unsigned char newValue);

  STDMETHODIMP get_ShortVal(__deref_out short *pVal);

  STDMETHODIMP put_ShortVal(__in short newValue);

  STDMETHODIMP get_UShortVal(__deref_out unsigned short *pVal);

  STDMETHODIMP put_UShortVal(__in unsigned short newValue);

  STDMETHODIMP get_LongVal(__deref_out long *pVal);

  STDMETHODIMP put_LongVal(__in long newValue);

  STDMETHODIMP get_ULongVal(__deref_out unsigned long *pVal);

  STDMETHODIMP put_ULongVal(__in unsigned long newValue);

  STDMETHODIMP get_LongLongVal(__deref_out __int64 *pVal);

  STDMETHODIMP put_LongLongVal(__in __int64 newValue);

  STDMETHODIMP get_ULongLongVal(__deref_out unsigned __int64 *pVal);

  STDMETHODIMP put_ULongLongVal(__in unsigned __int64 newValue);

  STDMETHODIMP get_SSizeTVal(__deref_out my_ssize_t *pVal);

  STDMETHODIMP put_SSizeTVal(__in my_ssize_t newValue);

  STDMETHODIMP get_SizeTVal(__deref_out my_size_t *pVal);

  STDMETHODIMP put_SizeTVal(__in my_size_t newValue);

  STDMETHODIMP get_FloatVal(__deref_out float *pVal);

  STDMETHODIMP put_FloatVal(__in float newValue);

  STDMETHODIMP get_DoubleVal(__deref_out double *pVal);

  STDMETHODIMP put_DoubleVal(__in double newValue);

  STDMETHODIMP get_CharValAt(__in LONG index, __deref_out signed char *pVal);

  STDMETHODIMP put_CharValAt(__in LONG index, __in signed char newValue);

  STDMETHODIMP get_ByteValAt(__in LONG index, __deref_out unsigned char *pVal);

  STDMETHODIMP put_ByteValAt(__in LONG index, __in unsigned char newValue);

  STDMETHODIMP get_ShortValAt(__in LONG index, __deref_out short *pVal);

  STDMETHODIMP put_ShortValAt(__in LONG index, __in short newValue);

  STDMETHODIMP get_UShortValAt( __in LONG index, __deref_out unsigned short *pVal);

  STDMETHODIMP put_UShortValAt(__in LONG index, __in unsigned short newValue);

  STDMETHODIMP get_LongValAt(__in LONG index, __deref_out long *pVal);

  STDMETHODIMP put_LongValAt(__in LONG index, __in long newValue);

  STDMETHODIMP get_ULongValAt(__in LONG index, __deref_out unsigned long *pVal);

  STDMETHODIMP put_ULongValAt(__in LONG index, __in unsigned long newValue);

  STDMETHODIMP get_LongLongValAt(__in LONG index, __deref_out __int64 *pVal);

  STDMETHODIMP put_LongLongValAt(__in LONG index, __in __int64 newValue);

  STDMETHODIMP get_ULongLongValAt(__in LONG index, __deref_out unsigned __int64 *pVal);

  STDMETHODIMP put_ULongLongValAt(__in LONG index, __in unsigned __int64 newValue);

  STDMETHODIMP get_SSizeTValAt(__in LONG index, __deref_out my_ssize_t *pVal);

  STDMETHODIMP put_SSizeTValAt(__in LONG index, __in my_ssize_t newValue);

  STDMETHODIMP get_SizeTValAt(__in LONG index, __deref_out my_size_t *pVal);

  STDMETHODIMP put_SizeTValAt(__in LONG index, __in my_size_t newValue);

  STDMETHODIMP get_FloatValAt(__in LONG index, __deref_out float *pVal);

  STDMETHODIMP put_FloatValAt(__in LONG index, __in float newValue);

  STDMETHODIMP get_DoubleValAt(__in LONG index, __deref_out double *pVal);

  STDMETHODIMP put_DoubleValAt(__in LONG index, __in double newValue);

  STDMETHODIMP get_PointerVal(__deref_out my_ssize_t *pVal);

  STDMETHODIMP put_PointerVal(__in my_ssize_t newValue);

  STDMETHODIMP get_IsNullPointer(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP Memory(__deref_out INktProcessMemory **ppProcMem);

  STDMETHODIMP CastTo(__in BSTR newType, __deref_out INktParam **ppParam);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktParam), CNktParamImpl)
