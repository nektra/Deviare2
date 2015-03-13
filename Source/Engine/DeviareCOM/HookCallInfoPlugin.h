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
#include "InternalCreate.h"

//-----------------------------------------------------------
// CNktHookCallInfoPluginImpl

class ATL_NO_VTABLE CNktHookCallInfoPluginImpl : public CComObjectRootEx<CComMultiThreadModel>,
                           public CComCoClass<CNktHookCallInfoPluginImpl, &CLSID_NktHookCallInfoPlugin>,
                           public CNktMemMgrObj,
                           public IObjectSafetyImpl<CNktHookCallInfoPluginImpl,
                                                    INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
                           public IDispatchImpl<INktHookCallInfoPlugin, &IID_INktHookCallInfoPlugin,
                                                &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                                DEVIARECOM_MinorVersion>,
                           public IInterfaceItemImpl<CNktHookCallInfoPluginImpl>
{
public:
  CNktHookCallInfoPluginImpl() : CComObjectRootEx<CComMultiThreadModel>(),
                                 CComCoClass<CNktHookCallInfoPluginImpl, &CLSID_NktHookCallInfoPlugin>(),
                                 CNktMemMgrObj(),
                                 IObjectSafetyImpl<CNktHookCallInfoPluginImpl,
                                                   INTERFACESAFE_FOR_UNTRUSTED_CALLER>(),
                                 IDispatchImpl<INktHookCallInfoPlugin, &IID_INktHookCallInfoPlugin,
                                               &LIBID_Deviare2, DEVIARECOM_MajorVersion,
                                               DEVIARECOM_MinorVersion>(),
                                 IInterfaceItemImpl<CNktHookCallInfoPluginImpl>()
    {
    NktInterlockedExchange(&nValid, 1);
    NktInterlockedExchange(&nLock, 0);
    lpIntercallCustomData = NULL;
    return;
    };

  DECLARE_REGISTRY_RESOURCEID_EX(IDR_INTERFACEREGISTRAR, L"DeviareCOM.NktHookCallInfoPlugin",
                                 L"1", L"NktHookCallInfoPlugin Class", CLSID_NktHookCallInfoPlugin,
                                 LIBID_Deviare2, L"Neutral")

  BEGIN_COM_MAP(CNktHookCallInfoPluginImpl)
    COM_INTERFACE_ENTRY(INktHookCallInfoPlugin)
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

  HRESULT InvalidateInterface();

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

  STDMETHODIMP get_HookInfo(__deref_out INktHookInfo **ppHookInfo);

  STDMETHODIMP get_ThreadId(__deref_out LONG *pVal);

  STDMETHODIMP get_ChainDepth(__deref_out LONG *pVal);

  STDMETHODIMP CurrentProcess(__deref_out INktProcess **ppProc);

  STDMETHODIMP StackTrace(__deref_out INktStackTrace **ppStackTrace);

  STDMETHODIMP Params(__deref_out INktParamsEnum **ppParamsEnum);

  STDMETHODIMP Result(__deref_out INktParam **ppParam);

  STDMETHODIMP get_Register(__in eNktRegister asmReg, __deref_out my_ssize_t *pVal);

  STDMETHODIMP put_Register(__in eNktRegister asmReg, __in my_ssize_t newValue);

  STDMETHODIMP get_FloatingRegister(__in eNktRegister asmReg, __deref_out double *pVal);

  STDMETHODIMP put_FloatingRegister(__in eNktRegister asmReg, __in double newValue);

  STDMETHODIMP SkipCall();

  STDMETHODIMP FilterSpyMgrEvent();

  STDMETHODIMP Add(__in BSTR name, __in VARIANT value);

  STDMETHODIMP AddString(__in BSTR name, __in BSTR value);

  STDMETHODIMP AddChar(__in BSTR name, __in signed char value);

  STDMETHODIMP AddByte(__in BSTR name, __in unsigned char value);

  STDMETHODIMP AddShort(__in BSTR name, __in short value);

  STDMETHODIMP AddUShort(__in BSTR name, __in unsigned short value);

  STDMETHODIMP AddLong(__in BSTR name, __in long value);

  STDMETHODIMP AddULong(__in BSTR name, __in unsigned long value);

  STDMETHODIMP AddLongLong(__in BSTR name, __in __int64 value);

  STDMETHODIMP AddULongLong(__in BSTR name, __in unsigned __int64 value);

  STDMETHODIMP AddSSizeT(__in BSTR name, __in my_ssize_t value);

  STDMETHODIMP AddSizeT(__in BSTR name, __in my_size_t value);

  STDMETHODIMP AddFloat(__in BSTR name, __in float value);

  STDMETHODIMP AddDouble(__in BSTR name, __in double value);

  STDMETHODIMP ReadIntercallData(__in my_ssize_t destAddr, __in my_ssize_t offset, __in my_ssize_t bytes);

  STDMETHODIMP WriteIntercallData(__in my_ssize_t srcAddr, __in my_ssize_t offset, __in my_ssize_t bytes);

  static HRESULT InternalCreate(__in DV_AGENTCOM_HOOKCALLINFOPLUGIN_CREATE_DATA *lpData,
                                __deref_out IDispatch **lplpDisp);

private:
  HRESULT AddCommon(__in int nFundType, __in BSTR name, __in LPVOID lpSrc, __in SIZE_T nSrcSize);

  TNktComPtr<CNktDvParamsEnumerator> cCustomParamsEnum;
  DV_AGENTCOM_HOOKCALLINFOPLUGIN_CALLINFO sCallInfo;
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<INktParamsEnum> cIParamsEnum;
  TNktComPtr<INktParam> cIResult;
  LPBYTE lpIntercallCustomData;
  TNktComPtr<INktHookInfo> cHookInfo;
  LONG volatile *lpnFilterSpyMgrEvent;
  //----
  LONG volatile nValid;
  LONG volatile nLock;
  //----
  CComPtr<IUnknown> cUnkMarshaler;
  TNktComPtr<CNktDvProcessMemory> cProcMem;
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(NktHookCallInfoPlugin), CNktHookCallInfoPluginImpl)
