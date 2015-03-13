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

#include "Module.h"
#include "Tools.h"
#include "EngObjCache.h"
#include "ProcessMemory.h"
#include "DynamicAPIs.h"
#include "Process.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

class CInternalExpFuncCache
{
public:
  typedef TNktDvEngObjCache<CNktDvExportedFunctionsEnumerator, 17, 32> TMyExpFuncCache;

  CInternalExpFuncCache()
    {
    lpCache = NKT_MEMMGR_NEW TMyExpFuncCache;
    return;
    };

  ~CInternalExpFuncCache()
    {
    if (lpCache != NULL)
    {
      BOOL bShutdownInProgress;

      nktDvDynApis_RtlDllShutdownInProgress(&bShutdownInProgress);
      if (bShutdownInProgress == FALSE)
        delete lpCache;
      lpCache = NULL;
    }
    return;
    };

  virtual VOID Flush()
    {
    if (lpCache != NULL)
      lpCache->RemoveAll();
    return;
    };

public:
  TMyExpFuncCache *lpCache;
};

static CInternalExpFuncCache cExpFuncCache;

//-----------------------------------------------------------

CNktDvModule::CNktDvModule() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  dwProcessId = 0;
  nPlatformBits = 0;
  lpBaseAddress = NULL;
  nBaseSize = 0;
  hModule = NULL;
  nNameOffsetInPathStr = 0;
  nKey = 0;
  return;
}

CNktDvModule::~CNktDvModule()
{
  return;
}

HRESULT CNktDvModule::GetExportedFunctionsEnumerator(__deref_out CNktDvExportedFunctionsEnumerator **lplpEnum)
{
  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  if (dwProcessId == 0)
    return E_INVALIDARG;
  if (cExpFuncEnum == NULL)
  {
    CNktAutoFastMutex cLock(&GetLockObject());
    TNktComPtr<CNktDvModulesEnumerator> cModEnum;
    TNktComPtr<CNktDvExportedFunctionsEnumerator> cTempEnum, cTempEnumClone;
    SIZE_T nCreate;
    HRESULT hRes;

    if (cExpFuncEnum == NULL)
    {
      if (nKey != 0 && cExpFuncCache.lpCache != NULL)
      {
        nCreate = 0; //do nothing
        cTempEnum.Attach(cExpFuncCache.lpCache->Get(nKey));
        if (cTempEnum != NULL)
        {
          //clone enumerator
          cTempEnumClone.Attach(cTempEnum->Clone());
          if (cTempEnumClone == NULL)
            return E_OUTOFMEMORY;
          cTempEnum.Attach(cTempEnumClone.Detach());
        }
        else
        {
          nCreate = 1; //create and save
        }
      }
      else
      {
        nCreate = 2; //only create
      }
      if (nCreate != 0)
      {
        cTempEnum.Attach(NKT_MEMMGR_NEW CNktDvExportedFunctionsEnumerator);
        if (cTempEnum == NULL)
          return E_OUTOFMEMORY;
        if (nPlatformBits != 0)
        {
          hRes = CNktDvProcess::GetModulesEnumerator(&cModEnum, dwProcessId);
          if (SUCCEEDED(hRes))
            hRes = cTempEnum->Initialize(this, cModEnum);
          if (FAILED(hRes))
            return hRes;
        }
        if (nCreate == 1)
        {
          //if adding to the list, always return a clone
          hRes = cExpFuncCache.lpCache->Add(nKey, cTempEnum, TRUE);
          if (SUCCEEDED(hRes))
          {
            //clone enumerator
            cTempEnumClone.Attach(cTempEnum->Clone());
            if (cTempEnumClone != NULL)
              cTempEnum.Attach(cTempEnumClone.Detach());
            else
              hRes = E_OUTOFMEMORY;
          }
          if (FAILED(hRes))
            return hRes;
        }
      }
      _ReadWriteBarrier();
      cExpFuncEnum = cTempEnum;
      _ReadWriteBarrier();
    }
  }
  //done
  *lplpEnum = cExpFuncEnum;
  (*lplpEnum)->AddRef();
  return S_OK;
}

HRESULT CNktDvModule::FindExportedFunctionByOrdinal(__deref_out CNktDvExportedFunction **lplpFunc,
                                                    __in SIZE_T nOrdinal)
{
  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncEnum;
  TNktComPtr<CNktDvExportedFunction> cExpFunc;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  if (nOrdinal == 0|| dwProcessId == 0)
    return E_INVALIDARG;
  //find export
  hRes = GetExportedFunctionsEnumerator(&cExpFuncEnum);
  if (FAILED(hRes))
    return hRes;
  hRes = cExpFuncEnum->First(&cExpFunc);
  while (SUCCEEDED(hRes))
  {
    if (nOrdinal == cExpFunc->GetOrdinal())
    {
      *lplpFunc = cExpFunc.Detach();
      return S_OK;
    }
    cExpFunc.Release();
    hRes = cExpFuncEnum->Next(&cExpFunc);
  }
  return hRes;
}

HRESULT CNktDvModule::FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                                 __in_nz_opt LPCWSTR szFunctionNameW,
                                                 __in SIZE_T nFunctionNameLen)
{
  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncEnum;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  if (szFunctionNameW == NULL)
    return E_POINTER;
  if (nFunctionNameLen == NKT_SIZE_T_MAX)
    nFunctionNameLen = wcslen(szFunctionNameW);
  if (nFunctionNameLen == 0)
    return E_INVALIDARG;
  if (dwProcessId == 0)
    return E_FAIL;
  //find export
  hRes = GetExportedFunctionsEnumerator(&cExpFuncEnum);
  if (FAILED(hRes))
    return hRes;
  //get item
  return cExpFuncEnum->GetItemByName(lplpFunc, szFunctionNameW, nFunctionNameLen);
}

HRESULT CNktDvModule::FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                    __in_opt LPVOID lpAddr, __in BOOL bSearchNearest)
{
  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncEnum;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  if (dwProcessId == 0)
    return E_FAIL;
  //find export
  hRes = GetExportedFunctionsEnumerator(&cExpFuncEnum);
  if (FAILED(hRes))
    return hRes;
  //get item
  return cExpFuncEnum->GetItemByAddress(lplpFunc, lpAddr, bSearchNearest);
}

HRESULT CNktDvModule::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvModule [Base:%IXh / %s]", GetBaseAddress(), GetFileName()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

VOID CNktDvModule::FlushCache()
{
  cExpFuncCache.Flush();
  return;
}
