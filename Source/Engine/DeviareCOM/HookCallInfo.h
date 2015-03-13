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
// CNktHookCallInfoImpl

class ATL_NO_VTABLE CNktHookCallInfoImpl : public CComObjectRootEx<CComMultiThreadModel>,
                                           public CComCoClass<CNktHookCallInfoImpl,
                                                              &CLSID_NktHookCallInfo>,
                                           public CNktMemMgrObj,
                                           public IObjectSafetyImpl<CNktHookCallInfoImpl,
                                                                    INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                                           public CNktObjectBaseImpl<CNktHookCallInfoImpl,
                                                      INktHookCallInfo, CNktDvHookCallInfo,
                                                      &CLSID_NktHookCallInfo, &IID_INktHookCallInfo,
                                                      &LIBID_Deviare2>,
                                           public IInterfaceItemImpl<CNktHookCallInfoImpl>,
                                           public INktDvEngBaseComLink
{
public:
  CNktHookCallInfoImpl(__in CNktDvHookCallInfo *lpIntObj) : CComObjectRootEx<CComMultiThreadModel>(),
                           CComCoClass<CNktHookCallInfoImpl, &CLSID_NktHookCallInfo>(),
                           CNktMemMgrObj(),
                           IObjectSafetyImpl<CNktHookCallInfoImpl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                           CNktObjectBaseImpl<CNktHookCallInfoImpl, INktHookCallInfo, CNktDvHookCallInfo,
                                              &CLSID_NktHookCallInfo, &IID_INktHookCallInfo,
                                              &LIBID_Deviare2>(lpIntObj),
                           IInterfaceItemImpl<CNktHookCallInfoImpl>(),
                           INktDvEngBaseComLink()
    {
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktHookCallInfo",
                                 L"1", L"NktHookCallInfo Class", CLSID_NktHookCallInfo,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktHookCallInfoImpl)
    COM_INTERFACE_ENTRY(INktHookCallInfo)
    COM_INTERFACE_ENTRY(INktObject)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IObjectSafety)
    COM_INTERFACE_ENTRY(IInterfaceItem)
    COM_INTERFACE_ENTRY(INktDvEngBaseComLink)
    COM_INTERFACE_ENTRY_AGGREGATE(IID_IMarshal, cUnkMarshaler.p)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_GET_CONTROLLING_UNKNOWN()

  OBJBASE_DECLARE_APIS_1(CNktHookCallInfoImpl,CNktDvHookCallInfo)

public:
  STDMETHODIMP get_IsPreCall(__deref_out VARIANT_BOOL *pVal);

  STDMETHODIMP get_Cookie(__deref_out LONG *pVal);

  STDMETHODIMP get_LastError(__deref_out LONG *pVal);

  STDMETHODIMP put_LastError(__in LONG newValue);

  STDMETHODIMP get_CurrentTimeMs(__deref_out double *pVal);

  STDMETHODIMP get_ElapsedTimeMs(__deref_out double *pVal);

  STDMETHODIMP get_ChildsElapsedTimeMs(__deref_out double *pVal);

  STDMETHODIMP get_KernelModeTimeMs(__deref_out double *pVal);

  STDMETHODIMP get_UserModeTimeMs(__deref_out double *pVal);

  STDMETHODIMP get_CpuClockCycles(__deref_out unsigned __int64 *pVal);

  STDMETHODIMP Hook(__deref_out INktHook **ppHook);

  STDMETHODIMP Process(__deref_out INktProcess **ppProc);

  STDMETHODIMP get_ThreadId(__deref_out LONG *pVal);

  STDMETHODIMP get_ChainDepth(__deref_out LONG *pVal);

  STDMETHODIMP StackTrace(__deref_out INktStackTrace **ppStackTrace);

  STDMETHODIMP Params(__deref_out INktParamsEnum **ppParamsEnum);

  STDMETHODIMP Result(__deref_out INktParam **ppParam);

  STDMETHODIMP CustomParams(__deref_out INktParamsEnum **ppParamsEnum);

  STDMETHODIMP get_Register(__in eNktRegister asmReg, __deref_out my_ssize_t *pVal);

  STDMETHODIMP put_Register(__in eNktRegister asmReg, __in my_ssize_t newValue);

  STDMETHODIMP get_FloatingRegister(__in eNktRegister asmReg, __deref_out double *pVal);

  STDMETHODIMP put_FloatingRegister(__in eNktRegister asmReg, __in double newValue);

  STDMETHODIMP SkipCall(void);

  STDMETHODIMP ReadIntercallData(__in my_ssize_t destAddr, __in my_ssize_t offset, __in my_ssize_t bytes);

  STDMETHODIMP WriteIntercallData(__in my_ssize_t srcAddr, __in my_ssize_t offset, __in my_ssize_t bytes);

private:
  CComPtr<IUnknown> cUnkMarshaler;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktHookCallInfo), CNktHookCallInfoImpl)
