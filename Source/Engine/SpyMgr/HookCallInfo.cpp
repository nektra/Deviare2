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

#include "Hook.h"
#include "SpyMgr.h"
#include "..\..\Common\Tools.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

CNktDvHookCallInfo::CNktDvHookCallInfo() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  bValid = TRUE;
  bSkipCall = FALSE;
  lpRemoteIntercallCustomData = NULL;
  return;
}

CNktDvHookCallInfo::~CNktDvHookCallInfo()
{
  return;
}

HRESULT CNktDvHookCallInfo::SetLastError(__in DWORD _dwOsLastError)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (bValid == FALSE)
    return E_FAIL;
  dwOsLastError = _dwOsLastError;
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetHook(__deref_out CNktDvHook **lplpHook)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (lplpHook == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpHook = NULL;
    return E_FAIL;
  }
  //done
  *lplpHook = cHook;
  (*lplpHook)->AddRef();
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetProcess(__deref_out CNktDvProcess **lplpProc)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (lplpProc == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpProc = NULL;
    return E_FAIL;
  }
  //done
  *lplpProc = cProc;
  (*lplpProc)->AddRef();
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetStackTrace(__deref_out CNktDvStackTrace **lplpStackTrace)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktComPtr<CNktDvStackTrace> cStackTrace;

  if (lplpStackTrace == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpStackTrace = NULL;
    return E_FAIL;
  }
  cStackTrace.Attach(NKT_MEMMGR_NEW CNktDvStackTrace);
  if (cStackTrace == NULL)
    return E_OUTOFMEMORY;
  cStackTrace->cProc = cProc;
  cStackTrace->cModEnum = cModEnum;
  nktMemCopy(cStackTrace->nStackTrace, nStackTrace, NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth*sizeof(SIZE_T));
  //done
  *lplpStackTrace = cStackTrace.Detach();
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetFunctionParameters(__deref_out CNktDvParamsEnumerator **lplpFuncParams)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (lplpFuncParams == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpFuncParams = NULL;
    return E_FAIL;
  }
  *lplpFuncParams = cFuncParams->cParameters;
  (*lplpFuncParams)->AddRef();
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetFunctionResult(__deref_out CNktDvParam **lplpFuncResult)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (lplpFuncResult == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpFuncResult = NULL;
    return E_FAIL;
  }
  *lplpFuncResult = cFuncParams->cResult;
  (*lplpFuncResult)->AddRef();
  return S_OK;
}

HRESULT CNktDvHookCallInfo::GetCustomParameters(__deref_out CNktDvParamsEnumerator **lplpFuncCustomParams)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (lplpFuncCustomParams == NULL)
    return E_POINTER;
  if (bValid == FALSE)
  {
    *lplpFuncCustomParams = NULL;
    return E_FAIL;
  }
  *lplpFuncCustomParams = cCustomParams;
  (*lplpFuncCustomParams)->AddRef();
  return S_OK;
}

SIZE_T CNktDvHookCallInfo::GetRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister) const
{
  CNktAutoFastMutex cLock(&const_cast<CNktDvHookCallInfo*>(this)-> GetLockObject());
  LPVOID lpAddr;
  union {
    DWORD dw;
    ULONGLONG ull;
  };
  SIZE_T nVal;

  if (bValid == FALSE)
    return 0;
  lpAddr = cCallState->GetAddressForRegister(nRegister);
  if (lpAddr == NULL)
    return 0;
  if (cCallState->Get32() != NULL)
  {
    if (FAILED(cProcMem->ReadDWord(&dw, lpAddr, 1)))
      dw = 0;
    nVal = (SIZE_T)dw;
  }
  else if (cCallState->Get64() != NULL)
  {
    if (FAILED(cProcMem->ReadQWord(&ull, lpAddr, 1)))
      ull = 0;
    nVal = (SIZE_T)ull;
  }
  else
  {
    nVal = 0;
  }
  return nVal;
}

HRESULT CNktDvHookCallInfo::SetRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                             __in SIZE_T nValue)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  LPVOID lpAddr;
  union {
    DWORD dw;
    ULONGLONG ull;
  };

  if (bValid == FALSE)
    return E_FAIL;
  if (nRegister == ENktDvMiscEnums::asmRegEip || nRegister == ENktDvMiscEnums::asmRegRip ||
      nRegister == ENktDvMiscEnums::asmRegEsp || nRegister == ENktDvMiscEnums::asmRegRsp)
    return E_INVALIDARG;
  lpAddr = cCallState->GetAddressForRegister(nRegister);
  if (lpAddr == NULL)
    return E_INVALIDARG;

  if (cCallState->Get32() != NULL)
  {
    dw = (DWORD)nValue;
    return cProcMem->WriteDWord(lpAddr, dw);
  }
  if (cCallState->Get64() != NULL)
  {
    ull = (ULONGLONG)nValue;
    return cProcMem->WriteQWord(lpAddr, ull);
  }
  return E_FAIL;
}

double CNktDvHookCallInfo::GetFloatingRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister) const
{
  CNktAutoFastMutex cLock(&const_cast<CNktDvHookCallInfo*>(this)-> GetLockObject());
  LPVOID lpAddr;
  double nVal;

  if (bValid == FALSE)
    return 0.0;
  lpAddr = cCallState->GetAddressForRegister(nRegister);
  if (lpAddr == NULL || FAILED(cProcMem->ReadDouble(&nVal, lpAddr, 1)))
    return 0.0;
  return nVal;
}

HRESULT CNktDvHookCallInfo::SetFloatingRegisterValue(__in ENktDvMiscEnums::eNktDvRegister nRegister,
                                                     __in double nValue)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  LPVOID lpAddr;

  if (bValid == FALSE)
    return E_FAIL;
  lpAddr = cCallState->GetAddressForRegister(nRegister);
  if (lpAddr == NULL)
    return E_INVALIDARG;
  return cProcMem->WriteDouble(lpAddr, nValue);
}

HRESULT CNktDvHookCallInfo::SkipCall()
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (bValid == FALSE)
    return E_FAIL;
  bSkipCall = TRUE;
  return S_OK;
}

HRESULT CNktDvHookCallInfo::ReadIntercallData(__in LPVOID lpDestAddr, __in SIZE_T nOffset, __in SIZE_T nBytes)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (bValid == FALSE)
    return E_FAIL;
  if (lpDestAddr == NULL)
    return E_POINTER;
  if (nOffset >= NKT_DV_HOOKENG_IntercallCustomDataSize ||
      nOffset+nBytes < nOffset || nOffset+nBytes > NKT_DV_HOOKENG_IntercallCustomDataSize)
    return E_INVALIDARG;
  return cProcMem->Read(lpDestAddr, (LPVOID)((SIZE_T)lpRemoteIntercallCustomData+nOffset), nBytes);
}

HRESULT CNktDvHookCallInfo::WriteIntercallData(__in LPVOID lpSrcAddr, __in SIZE_T nOffset, __in SIZE_T nBytes)
{
  CNktAutoFastMutex cLock(&GetLockObject());

  if (bValid == FALSE)
    return E_FAIL;
  if (lpSrcAddr == NULL)
    return E_POINTER;
  if (nOffset >= NKT_DV_HOOKENG_IntercallCustomDataSize ||
    nOffset+nBytes < nOffset || nOffset+nBytes > NKT_DV_HOOKENG_IntercallCustomDataSize)
    return E_INVALIDARG;
  return cProcMem->Write((LPVOID)((SIZE_T)lpRemoteIntercallCustomData+nOffset), lpSrcAddr, nBytes);
}

HRESULT CNktDvHookCallInfo::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvHookCallInfo") == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

VOID CNktDvHookCallInfo::Invalidate()
{
  CNktAutoFastMutex cLock(&GetLockObject());

  bValid = FALSE;
  cCallState->Invalidate();
  return;
}
