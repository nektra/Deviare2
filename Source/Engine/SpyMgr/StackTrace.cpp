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

#include "StackTrace.h"
#include "SpyMgr.h"
#include "..\..\Common\Tools.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

CNktDvStackTrace::CNktDvStackTrace() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  SIZE_T i;

  for (i=0; i<NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth; i++)
  {
    hModInit[i] = S_FALSE;
    hExpFuncInit[i] = S_FALSE;
    szSymbolW[i] = NULL;
  }
  return;
}

CNktDvStackTrace::~CNktDvStackTrace()
{
  SIZE_T i;

  for (i=0; i<NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth; i++)
    nktMemFree(szSymbolW[i]);
  return;
}

HRESULT CNktDvStackTrace::GetNearestFunction(__deref_out CNktDvExportedFunction **lplpFunc,
                                             __in SIZE_T nDepth)
{
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  if (nDepth >= NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth)
    return E_INVALIDARG;
  if (nStackTrace[nDepth] == 0)
    return NKT_DVERR_NotFound;
  {
    CNktAutoFastMutex cLock(&GetLockObject());

    hRes = InitExpFunc(nDepth);
    if (FAILED(hRes))
      return hRes;
    if (cExpFunc[nDepth] == NULL)
      return NKT_DVERR_NotFound;
    *lplpFunc = cExpFunc[nDepth];
    (*lplpFunc)->AddRef();
  }
  return S_OK;
}

HRESULT CNktDvStackTrace::GetModule(__deref_out CNktDvModule **lplpMod, __in SIZE_T nDepth)
{
  HRESULT hRes;

  if (lplpMod == NULL)
    return E_POINTER;
  *lplpMod = NULL;
  if (nDepth >= NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth)
    return E_INVALIDARG;
  if (nStackTrace[nDepth] == 0)
    return NKT_DVERR_NotFound;
  {
    CNktAutoFastMutex cLock(&GetLockObject());

    hRes = InitMod(nDepth);
    if (FAILED(hRes))
      return hRes;
    if (cMod[nDepth] == NULL)
      return NKT_DVERR_NotFound;
    *lplpMod = cMod[nDepth];
    (*lplpMod)->AddRef();
  }
  return S_OK;
}

SIZE_T CNktDvStackTrace::GetOffset(__in SIZE_T nDepth) const
{
  CNktDvStackTrace *lpThis = const_cast<CNktDvStackTrace*>(this);
  SIZE_T nOfs;

  if (nDepth >= NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth || nStackTrace[nDepth] == 0)
    return 0;
  nOfs = nStackTrace[nDepth];
  {
    CNktAutoFastMutex cLock(&(lpThis->GetLockObject()));

    if (SUCCEEDED(lpThis->InitExpFunc(nDepth)))
    {
      if (cExpFunc[nDepth] != NULL)
        nOfs -= cExpFunc[nDepth]->GetAddr();
      else if (cMod[nDepth] != NULL)
        nOfs -= (SIZE_T)(cMod[nDepth]->GetBaseAddress());
    }
  }
  return nOfs;
}

LPVOID CNktDvStackTrace::GetAddress(__in SIZE_T nDepth) const
{
  return (nDepth < NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth) ? (LPVOID)nStackTrace[nDepth] : NULL;
}

LPWSTR CNktDvStackTrace::GetNearestSymbol(__in SIZE_T nDepth) const
{
  CNktDvStackTrace *lpThis = const_cast<CNktDvStackTrace*>(this);
  LPWSTR sW;

  sW = NULL;
  if (nDepth < NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth &&
      nStackTrace[nDepth] != 0)
  {
    CNktAutoFastMutex cLock(&(lpThis->GetLockObject()));
    LPWSTR szCopyW;
    SIZE_T nLen;
    HRESULT hRes;

    if (szSymbolW[nDepth] == NULL)
    {
      hRes = lpThis->InitExpFunc(nDepth);
      if (FAILED(hRes))
        return NULL;
      if (cExpFunc[nDepth] != NULL)
      {
        sW = cExpFunc[nDepth]->GetCompleteName();
      }
      else if (cMod[nDepth] != NULL)
      {
        sW = cMod[nDepth]->GetFileName();
      }
      else
      {
        sW = L"";
      }
      nLen = wcslen(sW) + 1;
      szCopyW = (LPWSTR)nktMemMalloc(nLen * sizeof(WCHAR));
      if (szCopyW == NULL)
        return NULL;
      nktMemCopy(szCopyW, sW, nLen * sizeof(WCHAR));
      _ReadWriteBarrier();
      lpThis->szSymbolW[nDepth] = szCopyW;
      _ReadWriteBarrier();
    }
    sW = szSymbolW[nDepth];
  }
  return sW;
}

HRESULT CNktDvStackTrace::GetParams(__deref_out CNktDvParamsEnumerator **lplpParamEnum,
                                    __deref_out CNktDvParam **lplpResult, __in SIZE_T nDepth)
{
  if (lplpParamEnum != NULL)
    *lplpParamEnum = NULL;
  if (lplpResult != NULL)
    *lplpResult = NULL;
  if (lplpParamEnum == NULL && lplpResult == NULL)
    return E_POINTER;
  if (nDepth >= NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth)
    return E_INVALIDARG;
  if (nStackTrace[nDepth] == 0)
    return NKT_DVERR_NotFound;
  return E_NOTIMPL;
}

HRESULT CNktDvStackTrace::InitModEnum()
{
  HRESULT hRes;

  if (cModEnum == NULL)
  {
    hRes = cProc->GetModulesEnumeratorV(&cModEnum);
    if (FAILED(hRes))
      return hRes;
  }
  return S_OK;
}

HRESULT CNktDvStackTrace::InitMod(__in SIZE_T nDepth)
{
  HRESULT hRes;

  hRes = InitModEnum();
  if (SUCCEEDED(hRes))
  {
    if (hModInit[nDepth] == S_FALSE)
    {
      hModInit[nDepth] = cModEnum->GetItemByAddress(&cMod[nDepth], (LPVOID)nStackTrace[nDepth],
                                                    CNktDvModule::smFindContaining);
      if (hModInit[nDepth] == NKT_DVERR_NotFound)
        hModInit[nDepth] = S_OK;
    }
    hRes = hModInit[nDepth];
  }
  return hRes;
}

HRESULT CNktDvStackTrace::InitExpFunc(__in SIZE_T nDepth)
{
  HRESULT hRes;

  hRes = InitMod(nDepth);
  if (SUCCEEDED(hRes))
  {
    if (hExpFuncInit[nDepth] == S_FALSE)
    {
      if (cMod[nDepth] != NULL)
      {
        hExpFuncInit[nDepth] = cMod[nDepth]->FindExportedFunctionByAddress(&cExpFunc[nDepth],
                                                 (LPVOID)nStackTrace[nDepth], TRUE);
        if (hExpFuncInit[nDepth] == NKT_DVERR_NotFound)
          hExpFuncInit[nDepth] = S_OK;
      }
      else
      {
        //if no module was found, assume cExpFunc[nDepth] is ok and null
        hExpFuncInit[nDepth] = S_OK;
      }
    }
    hRes = hExpFuncInit[nDepth];
  }
  return hRes;
}
