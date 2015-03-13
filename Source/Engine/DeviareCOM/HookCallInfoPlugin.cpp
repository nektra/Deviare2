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

#include "StdAfx.h"
#include "HookCallInfoPlugin.h"
#include "ParamsEnum.h"
#include "Param.h"
#include "Process.h"
#include "StackTrace.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#if defined _M_IX86
static LPVOID GetAddressForIntRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                       __in LPNKT_DV_CURRENT_CALL_STATE32 lpState);
static LPVOID GetAddressForFloatRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                         __in LPNKT_DV_CURRENT_CALL_STATE32 lpState);
#elif defined _M_X64
static LPVOID GetAddressForIntRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                       __in LPNKT_DV_CURRENT_CALL_STATE64 lpState);
static LPVOID GetAddressForFloatRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                         __in LPNKT_DV_CURRENT_CALL_STATE64 lpState);
#endif //_M_X64

//-----------------------------------------------------------
// CNktHookCallInfoPluginImpl

HRESULT CNktHookCallInfoPluginImpl::get_IsPreCall(__deref_out VARIANT_BOOL *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = VARIANT_FALSE;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get "is precall"
  *pVal = VariantBool_From_Bool(sCallInfo.bIsPreCall);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_Cookie(__deref_out LONG *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get "cookie"
  *pVal = sCallInfo.dwCookie[1];
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_LastError(__deref_out LONG *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = NO_ERROR;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get last error
  *pVal = (LONG)(sCallInfo.lpCallState->dwOsLastError);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::put_LastError(__in LONG newValue)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  //set last error
  sCallInfo.lpCallState->dwOsLastError= (DWORD)newValue;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_CurrentTimeMs(__deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get current time
  *pVal = sCallInfo.nCurrentTimeMs;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_ElapsedTimeMs(__deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get elapsed time
  *pVal = sCallInfo.nElapsedTimeMs;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_ChildsElapsedTimeMs(__deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get childs elapsed time
  *pVal = sCallInfo.nChildsElapsedTimeMs;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_KernelModeTimeMs(__deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get kernel mode time
  *pVal = sCallInfo.nKernelTimeMs;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_UserModeTimeMs(__deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get user mode time
  *pVal = sCallInfo.nUserTimeMs;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_CpuClockCycles(__deref_out unsigned __int64 *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0ui64;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get cpu clock cycles
  *pVal = sCallInfo.nCpuClockCycles;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_HookInfo(__deref_out INktHookInfo **ppHookInfo)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (ppHookInfo == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *ppHookInfo = NULL;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get hook info
  *ppHookInfo = (INktHookInfo*)cHookInfo;
  (*ppHookInfo)->AddRef();
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_ThreadId(__deref_out LONG *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get thread id
  *pVal = (LONG)sCallInfo.dwThreadId;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_ChainDepth(__deref_out LONG *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get chain depth
  *pVal = (LONG)sCallInfo.dwChainDepth;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::CurrentProcess(__deref_out INktProcess **ppProc)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  TNktComPtr<INktProcess> cIProc;
  HRESULT hRes;

  if (ppProc == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  *ppProc = NULL;
  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  //get outer interface
  hRes = GetComLinkHelper<CNktProcessImpl,INktProcess,CNktDvProcess>(&cIProc, cProc, InterfaceItemGetCookies());
  //done (don't create dummy interface on error)
  *ppProc = cIProc.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::StackTrace(__deref_out INktStackTrace **ppStackTrace)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  TNktComPtr<CNktDvStackTrace> cStackTrace;
  TNktComPtr<INktStackTrace> cIStackTrace;
  HRESULT hRes;

  if (ppStackTrace == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  *ppStackTrace = NULL;
  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  //initialize stack trace object
  cStackTrace.Attach(NKT_MEMMGR_NEW CNktDvStackTrace);
  if (cStackTrace != NULL)
  {
    cStackTrace->cProc = cProc;
    nktMemCopy(cStackTrace->nStackTrace, sCallInfo.lpCallState->nStackTrace,
               NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth*sizeof(SIZE_T));
    //get outer interface
    hRes = GetComLinkHelper<CNktStackTraceImpl,INktStackTrace, CNktDvStackTrace>(&cIStackTrace, cStackTrace,
                                                                                 InterfaceItemGetCookies());
  }
  else
  {
    hRes = E_OUTOFMEMORY;
  }
  //done (don't create dummy interface on error)
  *ppStackTrace = cIStackTrace.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::Params(__deref_out INktParamsEnum **ppParamsEnum)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (ppParamsEnum == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *ppParamsEnum = NULL;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get parameters
  *ppParamsEnum = cIParamsEnum;
  (*ppParamsEnum)->AddRef();
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::Result(__deref_out INktParam **ppParam)
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (ppParam == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *ppParam = NULL;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  //get result
  *ppParam = cIResult;
  (*ppParam)->AddRef();
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_Register(__in eNktRegister asmReg, __deref_out my_ssize_t *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  LPVOID lpPtr;

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  lpPtr = GetAddressForIntRegister((ENktDvMiscEnums::eNktDvRegister)asmReg, sCallInfo.lpCallState);
  if (lpPtr == NULL)
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
  }
  //get register value
  *pVal = *((my_ssize_t*)lpPtr);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::put_Register(__in eNktRegister asmReg, __in my_ssize_t newValue)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  LPVOID lpPtr;

  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  lpPtr = GetAddressForIntRegister((ENktDvMiscEnums::eNktDvRegister)asmReg, sCallInfo.lpCallState);
  if (lpPtr == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
  //set register value
  *((my_ssize_t*)lpPtr) = newValue;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::get_FloatingRegister(__in eNktRegister asmReg,
                                                         __deref_out double *pVal)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  LPVOID lpPtr;

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  if (nValid == 0)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  }
  lpPtr = GetAddressForFloatRegister((ENktDvMiscEnums::eNktDvRegister)asmReg, sCallInfo.lpCallState);
  if (lpPtr == NULL)
  {
    *pVal = 0.0;
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
  }
  //get register value
  *pVal = *((double*)lpPtr);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}


HRESULT CNktHookCallInfoPluginImpl::put_FloatingRegister(__in eNktRegister asmReg, __in double newValue)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  LPVOID lpPtr;

  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  lpPtr = GetAddressForFloatRegister((ENktDvMiscEnums::eNktDvRegister)asmReg, sCallInfo.lpCallState);
  if (lpPtr == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
  //set register value
  *((double*)lpPtr) = newValue;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::SkipCall()
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  //----
  sCallInfo.lpCallState->dwSkipCall = 1;
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::FilterSpyMgrEvent()
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  if (nValid == 0)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_FAIL);
  //----
  NktInterlockedExchange(lpnFilterSpyMgrEvent, 1);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, S_OK);
}

HRESULT CNktHookCallInfoPluginImpl::Add(__in BSTR name, __in VARIANT value)
{
  VARIANT *lpVar;
  LPBYTE lpData;
  SIZE_T nItemsCount;
  HRESULT hRes;

  lpVar = &value;
  while (lpVar != NULL && lpVar->vt == (VT_BYREF|VT_VARIANT))
    lpVar = lpVar->pvarVal;
  if (lpVar == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  switch (lpVar->vt)
  {
    case VT_I1:
      return AddChar(name, lpVar->cVal);

    case VT_UI1:
      return AddByte(name, lpVar->bVal);

    case VT_I2:
      return AddShort(name, lpVar->iVal);

    case VT_UI2:
      return AddUShort(name, lpVar->uiVal);

    case VT_I4:
      return AddLong(name, lpVar->lVal);

    case VT_UI4:
      return AddULong(name, lpVar->ulVal);

    case VT_INT:
      return AddLong(name, lpVar->intVal);

    case VT_UINT:
      return AddULong(name, lpVar->uintVal);

    case VT_I8:
      return AddLongLong(name, lpVar->llVal);

    case VT_UI8:
      return AddULongLong(name, lpVar->ullVal);

    case VT_R4:
      return AddFloat(name, lpVar->fltVal);

    case VT_R8:
      return AddDouble(name, lpVar->dblVal);

    case VT_BSTR:
      return AddString(name, lpVar->bstrVal);

    case VT_BYREF|VT_I1:
      if (lpVar->pcVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddChar(name, *(lpVar->pcVal));

    case VT_BYREF|VT_UI1:
      if (lpVar->pbVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddByte(name, *(lpVar->pbVal));

    case VT_BYREF|VT_I2:
      if (lpVar->piVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddShort(name, *(lpVar->piVal));

    case VT_BYREF|VT_UI2:
      if (lpVar->puiVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddUShort(name, *(lpVar->puiVal));

    case VT_BYREF|VT_I4:
      if (lpVar->plVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddLong(name, *(lpVar->plVal));

    case VT_BYREF|VT_UI4:
      if (lpVar->pulVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddULong(name, *(lpVar->pulVal));

    case VT_BYREF|VT_INT:
      if (lpVar->pintVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddLong(name, *(lpVar->pintVal));

    case VT_BYREF|VT_UINT:
      if (lpVar->puintVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddULong(name, *(lpVar->puintVal));

    case VT_BYREF|VT_I8:
      if (lpVar->pllVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddLongLong(name, *(lpVar->pllVal));

    case VT_BYREF|VT_UI8:
      if (lpVar->pullVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddULongLong(name, *(lpVar->pullVal));

    case VT_BYREF|VT_R4:
      if (lpVar->pfltVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddFloat(name, *(lpVar->pfltVal));

    case VT_BYREF|VT_R8:
      if (lpVar->pdblVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddDouble(name, *(lpVar->pdblVal));

    case VT_BYREF|VT_BSTR:
      if (lpVar->pbstrVal == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      return AddString(name, *(lpVar->pbstrVal));

    case VT_ARRAY|VT_I1:
    case VT_ARRAY|VT_UI1:
    case VT_ARRAY|VT_I2:
    case VT_ARRAY|VT_UI2:
    case VT_ARRAY|VT_I4:
    case VT_ARRAY|VT_UI4:
    case VT_ARRAY|VT_INT:
    case VT_ARRAY|VT_UINT:
    case VT_ARRAY|VT_I8:
    case VT_ARRAY|VT_UI8:
    case VT_ARRAY|VT_R4:
    case VT_ARRAY|VT_R8:
      if (lpVar->parray == NULL)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
      if (lpVar->parray->cDims != 1)
        return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
      nItemsCount = (SIZE_T)(lpVar->parray->rgsabound[0].cElements);
      //get array data
      hRes = ::SafeArrayAccessData(lpVar->parray, (LPVOID*)&lpData);
      if (SUCCEEDED(hRes))
      {
        CNktSimpleLockNonReentrant cLock(&nLock);

        switch (lpVar->vt)
        {
          case VT_ARRAY|VT_I1:
            hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, (char*)lpData,
                             nItemsCount*sizeof(char));
            break;

          case VT_ARRAY|VT_UI1:
            hRes = AddCommon(CNktDvDbObject::ftUnsignedByte, name, (unsigned char*)lpData,
                             nItemsCount*sizeof(unsigned char));
            break;

          case VT_ARRAY|VT_I2:
            hRes = AddCommon(CNktDvDbObject::ftSignedWord, name, (short*)lpData,
                             nItemsCount*sizeof(short));
            break;

          case VT_ARRAY|VT_UI2:
            hRes = AddCommon(CNktDvDbObject::ftUnsignedWord, name, (unsigned short*)lpData,
                             nItemsCount*sizeof(unsigned short));
            break;

          case VT_ARRAY|VT_I4:
            hRes = AddCommon(CNktDvDbObject::ftSignedDoubleWord, name, (long*)lpData,
                             nItemsCount*sizeof(long));
            break;

          case VT_ARRAY|VT_UI4:
            hRes = AddCommon(CNktDvDbObject::ftUnsignedDoubleWord, name, (unsigned long*)lpData,
                             nItemsCount*sizeof(unsigned long));
            break;

          case VT_ARRAY|VT_INT:
            hRes = AddCommon(CNktDvDbObject::ftSignedQuadWord, name, (__int64*)lpData,
                             nItemsCount*sizeof(__int64));
            break;

          case VT_ARRAY|VT_UINT:
            hRes = AddCommon(CNktDvDbObject::ftUnsignedQuadWord, name, (unsigned __int64*)lpData,
                             nItemsCount*sizeof(unsigned __int64));
            break;

          case VT_ARRAY|VT_I8:
            hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, (unsigned char*)lpData,
                             nItemsCount*sizeof(unsigned char));
            break;

          case VT_ARRAY|VT_UI8:
            hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, (unsigned char*)lpData,
                             nItemsCount*sizeof(unsigned char));
            break;

          case VT_ARRAY|VT_R4:
            hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, (unsigned char*)lpData,
                             nItemsCount*sizeof(unsigned char));
            break;

          case VT_ARRAY|VT_R8:
            hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, (unsigned char*)lpData,
                             nItemsCount*sizeof(unsigned char));
            break;

          default:
            hRes = E_INVALIDARG;
            break;
        }
        ::SafeArrayUnaccessData(lpVar->parray);
      }
      return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
  }
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_INVALIDARG);
}

HRESULT CNktHookCallInfoPluginImpl::AddString(__in BSTR name, __in BSTR value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  SIZE_T nLen;
  HRESULT hRes;

  if (value == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, E_POINTER);
  nLen = ::SysStringLen(value) * sizeof(WCHAR);
  hRes = AddCommon(CNktDvDbObject::ftWideChar, name, value, nLen);
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddChar(__in BSTR name, __in signed char value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftSignedByte, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddByte(__in BSTR name, __in unsigned char value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftUnsignedByte, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddShort(__in BSTR name, __in short value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftSignedWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddUShort(__in BSTR name, __in unsigned short value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftUnsignedWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddLong(__in BSTR name, __in long value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftSignedDoubleWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddULong(__in BSTR name, __in unsigned long value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftUnsignedDoubleWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddLongLong(__in BSTR name, __in __int64 value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftSignedQuadWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddULongLong(__in BSTR name, __in unsigned __int64 value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftUnsignedQuadWord, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddSSizeT(__in BSTR name, __in my_ssize_t value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

#if defined _M_IX86
  hRes = AddCommon(CNktDvDbObject::ftSignedDoubleWord, name, &value, sizeof(value));
#elif defined _M_X64
  hRes = AddCommon(CNktDvDbObject::ftSignedQuadWord, name, &value, sizeof(value));
#endif //_M_X64
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddSizeT(__in BSTR name, __in my_size_t value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

#if defined _M_IX86
  hRes = AddCommon(CNktDvDbObject::ftUnsignedDoubleWord, name, &value, sizeof(value));
#elif defined _M_X64
  hRes = AddCommon(CNktDvDbObject::ftUnsignedQuadWord, name, &value, sizeof(value));
#endif //_M_X64
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddFloat(__in BSTR name, __in float value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftFloat, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::AddDouble(__in BSTR name, __in double value)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  hRes = AddCommon(CNktDvDbObject::ftDouble, name, &value, sizeof(value));
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::ReadIntercallData(__in my_ssize_t destAddr, __in my_ssize_t offset,
                                                      __in my_ssize_t bytes)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else if (destAddr == NULL)
  {
    hRes = E_POINTER;
  }
  else if (offset < 0 || bytes < 0 || offset >= NKT_DV_HOOKENG_IntercallCustomDataSize ||
           offset+bytes < offset || offset+bytes > NKT_DV_HOOKENG_IntercallCustomDataSize)
  {
    hRes = E_INVALIDARG;
  }
  else
  {
    try
    {
      nktMemCopy((LPBYTE)destAddr, lpIntercallCustomData+offset, (SIZE_T)bytes);
      hRes = S_OK;
    }
    catch (...)
    {
      hRes = E_FAIL;
    }
  }
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::WriteIntercallData(__in my_ssize_t srcAddr, __in my_ssize_t offset,
                                                       __in my_ssize_t bytes)
{
  CNktSimpleLockNonReentrant cLock(&nLock);
  HRESULT hRes;

  if (nValid == 0)
  {
    hRes = E_FAIL;
  }
  else if (srcAddr == NULL)
  {
    hRes = E_POINTER;
  }
  else if (offset < 0 || bytes < 0 || offset >= NKT_DV_HOOKENG_IntercallCustomDataSize ||
           offset+bytes < offset || offset+bytes > NKT_DV_HOOKENG_IntercallCustomDataSize)
  {
    hRes = E_INVALIDARG;
  }
  else
  {
    try
    {
      nktMemCopy(lpIntercallCustomData+offset, (LPBYTE)srcAddr, (SIZE_T)bytes);
      hRes = S_OK;
    }
    catch (...)
    {
      hRes = E_FAIL;
    }
  }
  return ::SetupErrorInfoAndReturn(IID_INktHookCallInfoPlugin, hRes);
}

HRESULT CNktHookCallInfoPluginImpl::InvalidateInterface()
{
  CNktSimpleLockNonReentrant cLock(&nLock);

  NktInterlockedExchange(&nValid, 0);
  cCustomParamsEnum.Release();
  nktMemSet(&sCallInfo, 0, sizeof(sCallInfo));
  cIParamsEnum.Release();
  cIResult.Release();
  return S_OK;
}

HRESULT CNktHookCallInfoPluginImpl::AddCommon(__in int nFundType, __in BSTR name, __in LPVOID lpSrc,
                                              __in SIZE_T nSrcSize)
{
  TNktComPtr<CNktDvParam> cParam;
  HRESULT hRes;

  if (nValid == 0)
    return E_FAIL;
  cParam.Attach(NKT_MEMMGR_NEW CNktDvParam(sizeof(SIZE_T)<<3));
  if (cParam == NULL)
    return E_OUTOFMEMORY;
  if (cProcMem == NULL)
  {
    cProcMem.Attach(CNktDvProcessMemory::CreateForPID(::GetCurrentProcessId()));
    if (cProcMem == NULL)
      return E_OUTOFMEMORY;
  }
  hRes = cParam->Agent_Init4Custom(cProcMem, nFundType, name, lpSrc, nSrcSize);
  if (SUCCEEDED(hRes))
    hRes = cCustomParamsEnum->Add(cParam);
  return hRes;
}

HRESULT CNktHookCallInfoPluginImpl::InternalCreate(__in DV_AGENTCOM_HOOKCALLINFOPLUGIN_CREATE_DATA *lpData,
                                                   __deref_out IDispatch **lplpDisp)
{
  CComObject<CNktHookCallInfoPluginImpl> *lpPtr = NULL;
  HRESULT hRes;

  if (lplpDisp != NULL)
    *lplpDisp = NULL;
  if (lpData == NULL || lplpDisp == NULL)
    return E_POINTER;
  //if we have no "OuterInterface" stored, create a new one
  hRes = CComObject<CNktHookCallInfoPluginImpl>::CreateInstance(&lpPtr);
  //initialize object
  if (SUCCEEDED(hRes))
  {
    lpPtr->AddRef(); //CComObject::CreateInstance returns with zero refcount
    lpPtr->cCustomParamsEnum = lpData->lpCustomParamsEnum;
    nktMemCopy(&(lpPtr->sCallInfo), &(lpData->sCallInfo), sizeof(DV_AGENTCOM_HOOKCALLINFOPLUGIN_CALLINFO));
    lpPtr->lpnFilterSpyMgrEvent = lpData->lpnFilterSpyMgrEvent;
    lpPtr->cProc = lpData->lpProc;
    lpPtr->lpIntercallCustomData = lpData->lpIntercallCustomData;
    lpPtr->InterfaceItemSetCookies(lpData->sCallInfo.dwCookie);
    hRes = GetComLinkHelper<CNktParamsEnumImpl,INktParamsEnum,
                            CNktDvParamsEnumerator>(&(lpPtr->cIParamsEnum), lpData->lpParamsEnum,
                                                    lpData->sCallInfo.dwCookie);
    if (SUCCEEDED(hRes))
    {
      hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&(lpPtr->cIResult), lpData->lpResult,
                                                                   lpData->sCallInfo.dwCookie);
    }
    if (SUCCEEDED(hRes))
    {
      hRes = lpData->lpHookInfo->QueryInterface(__uuidof(INktHookInfo), (void**)&(lpPtr->cHookInfo));
    }
  }
  //final initialization
  if (SUCCEEDED(hRes))
    hRes = lpPtr->QueryInterface<IDispatch>(lplpDisp);
  //decrement extra ref
  if (lpPtr != NULL)
    lpPtr->Release();
  //done
  if (FAILED(hRes))
    *lplpDisp = NULL;
  return hRes;
}

#if defined _M_IX86

static LPVOID GetAddressForIntRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                       __in LPNKT_DV_CURRENT_CALL_STATE32 lpState)
{
  switch (nRegister)
  {
    case ENktDvMiscEnums::asmRegEax:
    case ENktDvMiscEnums::asmRegEaxEdxPair:
      return &(lpState->sAsmRegisters.sInteger.Eax);
    case ENktDvMiscEnums::asmRegEbx:
      return &(lpState->sAsmRegisters.sInteger.Ebx);
    case ENktDvMiscEnums::asmRegEcx:
      return &(lpState->sAsmRegisters.sInteger.Ecx);
    case ENktDvMiscEnums::asmRegEdx:
      return &(lpState->sAsmRegisters.sInteger.Edx);
    case ENktDvMiscEnums::asmRegEsp:
      return &(lpState->sAsmRegisters.sInteger.Esp);
    case ENktDvMiscEnums::asmRegEbp:
      return &(lpState->sAsmRegisters.sInteger.Ebp);
    case ENktDvMiscEnums::asmRegEip:
      return &(lpState->sAsmRegisters.sInteger.Eip);
    case ENktDvMiscEnums::asmRegEsi:
      return &(lpState->sAsmRegisters.sInteger.Esi);
    case ENktDvMiscEnums::asmRegEdi:
      return &(lpState->sAsmRegisters.sInteger.Edi);
  }
  return NULL;
}

static LPVOID GetAddressForFloatRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                         __in LPNKT_DV_CURRENT_CALL_STATE32 lpState)
{
  switch (nRegister)
  {
    case ENktDvMiscEnums::asmRegSt0:
      return &(lpState->sAsmRegisters.sFloating.St0);
  }
  return NULL;
}

#elif defined _M_X64

static LPVOID GetAddressForIntRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                       __in LPNKT_DV_CURRENT_CALL_STATE64 lpState)
{
  switch (nRegister)
  {
    case ENktDvMiscEnums::asmRegRax:
      return &(lpState->sAsmRegisters.sInteger.Rax);
    case ENktDvMiscEnums::asmRegRbx:
      return &(lpState->sAsmRegisters.sInteger.Rbx);
    case ENktDvMiscEnums::asmRegRcx:
      return &(lpState->sAsmRegisters.sInteger.Rcx);
    case ENktDvMiscEnums::asmRegRdx:
      return &(lpState->sAsmRegisters.sInteger.Rdx);
    case ENktDvMiscEnums::asmRegR8:
      return &(lpState->sAsmRegisters.sInteger.R8);
    case ENktDvMiscEnums::asmRegR9:
      return &(lpState->sAsmRegisters.sInteger.R9);
    case ENktDvMiscEnums::asmRegR10:
      return &(lpState->sAsmRegisters.sInteger.R10);
    case ENktDvMiscEnums::asmRegR11:
      return &(lpState->sAsmRegisters.sInteger.R11);
    case ENktDvMiscEnums::asmRegRsp:
      return &(lpState->sAsmRegisters.sInteger.Rsp);
    case ENktDvMiscEnums::asmRegRbp:
      return &(lpState->sAsmRegisters.sInteger.Rbp);
    case ENktDvMiscEnums::asmRegRip:
      return &(lpState->sAsmRegisters.sInteger.Rip);
    case ENktDvMiscEnums::asmRegRsi:
      return &(lpState->sAsmRegisters.sInteger.Rsi);
    case ENktDvMiscEnums::asmRegRdi:
      return &(lpState->sAsmRegisters.sInteger.Rdi);
  }
  return NULL;
}

static LPVOID GetAddressForFloatRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                         __in LPNKT_DV_CURRENT_CALL_STATE64 lpState)
{
  switch (nRegister)
  {
    case ENktDvMiscEnums::asmRegXmm0:
      return &(lpState->sAsmRegisters.sFloating.Xmm0);
    case ENktDvMiscEnums::asmRegXmm1:
      return &(lpState->sAsmRegisters.sFloating.Xmm1);
    case ENktDvMiscEnums::asmRegXmm2:
      return &(lpState->sAsmRegisters.sFloating.Xmm2);
    case ENktDvMiscEnums::asmRegXmm3:
      return &(lpState->sAsmRegisters.sFloating.Xmm3);
  }
  return NULL;
}

#endif //_M_X64
