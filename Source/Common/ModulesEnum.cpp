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

#include "ModulesEnum.h"
#include "Tools.h"
#include "ProcessMemory.h"
#include "ProcessHandle.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

#define MODENUM_INITIALIZE_RETRIES                         4

//-----------------------------------------------------------

typedef struct {
  CNktDvModule** lplpModules;
  SIZE_T nNameLen;
} sSearchModuleByName_Ctx;

//-----------------------------------------------------------
/*
class CInternalModCache : private CNktFastMutex
{
public:
  typedef TNktDvEngObjCache<CNktDvModules, 17, 32> TMyModCache;

  CInternalModCache()
   {
    lpCache = NKT_MEMMGR_NEW TMyModCache;
    return;
    };

  ~CInternalModCache()
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

  virtual HRESULT Get(__deref_out CNktDvModule **lplpMod, __in DWORD dwPid, __in SIZE_T nPlatformBits)
    {
    TNktComPtr<CNktDvModule> cMod;
    HRESULT hRes;

    if (lplpMod == NULL)
      return E_POINTER;
    *lplpMod = NULL;
    if (dwPid == 0)
      return E_INVALIDARG;
    if (lpCache == NULL)
      return E_OUTOFMEMORY;
    cMod.Attach(lpCache->Get((Fnv64_t)dwPid));
    if (cMod != NULL)
    {
      if (cMod->GetPlatformBits() == nPlatformBits)
      {
        //clone module
        cModClone.Attach(cMod->Clone());
        if (cModClone == NULL)
          return E_OUTOFMEMORY;
        cMod.Attach(cModClone.Detach());
      }
      else
      {
        //release on platforms bits mismatch
        cMod.Release();
      }
    }
    if (cMod == NULL)
    {
      cMod.Attach(NKT_MEMMGR_NEW CNktDvModule);
      if (cMod == NULL)
        return E_OUTOFMEMORY;
      hRes = cMod->Initialize(dwPid, nPlatformBits);
      if (SUCCEEDED(hRes))
        hRes = lpCache->Add((Fnv64_t)dwPid, cMod, TRUE);
      if (FAILED(hRes))
        return hRes;
    }
    //done
    *lplpMod = cMod.Detach();
    return S_OK;
  };

  virtual VOID Remove(__in DWORD dwPid)
  {
    if (lpCache != NULL)
      lpCache->Remove((Fnv64_t)dwPid);
    return;
  };

  virtual VOID Flush()
  {
    if (lpCache != NULL)
      lpCache->RemoveAll();
    return;
  };

public:
  TMyModCache *lpCache;
};

static CInternalModCache cModCache;
*/
//-----------------------------------------------------------

static int __cdecl SearchByName(void *context, const void *key, const void *elem);
static int __cdecl SearchByAddr(void *context, const void *key, const void *elem);
static int __cdecl SortByName(void *context, const void *elem1, const void *elem2);
static int __cdecl SortByAddr(void *context, const void *elem1, const void *elem2);

static __inline int MyStrICmpW(__in const LPCWSTR szStr1, __in SIZE_T nStr1Len, __in const LPCWSTR szStr2)
{
  int nRes = _wcsnicmp(szStr1, szStr2, nStr1Len);
  if (nRes == 0 && szStr2[nStr1Len] != 0)
    nRes = -1; //szStr2 greater than szStr1
  return nRes;
}

//-----------------------------------------------------------

HRESULT CNktDvModulesEnumerator::GetItemByName(__deref_out CNktDvModule **lplpMod,
                                               __in_z LPCWSTR szDllNameW, __in SIZE_T nDllNameLen)
{
  sSearchModuleByName_Ctx sCtx;
  SIZE_T nCount, *lpnIdx;

  if (lplpMod != NULL)
    *lplpMod = NULL;
  if (lplpMod == NULL || szDllNameW == NULL)
    return E_POINTER;
  if (nDllNameLen == NKT_SIZE_T_MAX)
    nDllNameLen = wcslen(szDllNameW);
  if (szDllNameW[0] == 0 || nDllNameLen == 0)
    return E_INVALIDARG;
  nCount = GetCount();
  if (nCount > 0)
  {
    sCtx.lplpModules = aObjectsList.GetBuffer();
    sCtx.nNameLen = nDllNameLen;
    lpnIdx = (SIZE_T*)bsearch_s(szDllNameW, cNameIndexList.Get(), nCount, sizeof(SIZE_T), SearchByName,
                                &sCtx);
    if (lpnIdx != NULL)
    {
      *lplpMod = aObjectsList[*lpnIdx];
      (*lplpMod)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvModulesEnumerator::GetItemByAddress(__deref_out CNktDvModule **lplpMod,
                                                  __in_opt LPVOID lpAddr,
                                                  __in CNktDvModule::eSearchMode nSearchMode)
{
  CNktDvModule *lpMod;
  SIZE_T nCount, nLo, nHi, nMiddle, *lpnIdx;

  if (lplpMod == NULL)
    return E_POINTER;
  *lplpMod = lpMod = NULL;
  nCount = GetCount();
  if (nCount > 0)
  {
    if (nSearchMode == CNktDvModule::smExactMatch)
    {
      lpnIdx = (SIZE_T*)bsearch_s(&lpAddr, cAddrIndexList.Get(), nCount, sizeof(SIZE_T), SearchByAddr,
                                  aObjectsList.GetBuffer());
      if (lpnIdx != NULL)
        lpMod = aObjectsList[*lpnIdx];
    }
    else
    {
      //search "insertion point"
      for (nLo=0,nHi=nCount; nLo<nHi; )
      {
        nMiddle = (nLo+nHi) / 2;
        lpMod = aObjectsList[cAddrIndexList[nMiddle]];
        if (lpMod->GetBaseAddress() <= (LPBYTE)lpAddr)
          nLo = nMiddle+1;
        else
          nHi = nMiddle;
      }
      lpMod = (nLo > 0) ? aObjectsList[cAddrIndexList[nLo-1]] : NULL;
    }
    if (lpMod != NULL &&
        nSearchMode == CNktDvModule::smFindContaining &&
        (LPBYTE)lpAddr >= lpMod->GetBaseAddress()+lpMod->GetSize())
      lpMod = NULL;
    if (lpMod != NULL)
    {
      *lplpMod = lpMod;
      lpMod->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvModulesEnumerator::FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                                            __in_z LPCWSTR szDllFuncW)
{
  TNktComPtr<CNktDvModule> cMod;
  CNktDvTools::FUNCTION_PARSE_INFO sFuncParseInfo;
  LPWSTR sW;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  hRes = CNktDvTools::ParseFunctionName(sFuncParseInfo, szDllFuncW);
  if (FAILED(hRes))
    return hRes;
  if (sFuncParseInfo.nDllNameLen == 0)
    return E_INVALIDARG;
  //find module
  hRes = First(&cMod);
  while (SUCCEEDED(hRes))
  {
    sW = cMod->GetFileName();
    if (_wcsnicmp(sW, szDllFuncW, sFuncParseInfo.nDllNameLen) == 0 &&
        sW[sFuncParseInfo.nDllNameLen] == 0)
      break;
    cMod.Release();
    hRes = Next(&cMod);
  }
  if (FAILED(hRes))
  {
    if (hRes == E_FAIL)
      hRes = NKT_DVERR_NotFound;
    return hRes;
  }
  //find export
  if (sFuncParseInfo.nApiOrdinal != NKT_SIZE_T_MAX)
    hRes = cMod->FindExportedFunctionByOrdinal(lplpFunc, sFuncParseInfo.nApiOrdinal);
  else
    hRes = cMod->FindExportedFunctionByName(lplpFunc, szDllFuncW+sFuncParseInfo.nApiNameStart,
                                            sFuncParseInfo.nApiNameLen);
  //done
  return hRes;
}

HRESULT CNktDvModulesEnumerator::FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                     __in_opt LPVOID lpAddr, __in CNktDvModule::eSearchMode nSearchMode)
{
  TNktComPtr<CNktDvModule> cMod;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  //find module
  hRes = GetItemByAddress(&cMod, lpAddr, CNktDvModule::smGetNearest);
  if (FAILED(hRes))
    return hRes;
  //find export
  return cMod->FindExportedFunctionByAddress(lplpFunc, lpAddr,
                                             (nSearchMode == CNktDvModule::smExactMatch) ? FALSE : TRUE);
}

HRESULT CNktDvModulesEnumerator::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvModulesEnumerator [%Iu items]", GetCount()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvModulesEnumerator::Initialize(__in DWORD dwProcessId, __in SIZE_T nProcPlatformBits)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  TNktArrayList<HMODULE> aModulesList;
  TNktArrayListWithDelete<CNktDvTools::MODULE_INFO*> aMapList;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, nCount;
  HRESULT hRes;
  HANDLE hProc;

  if (dwProcessId == 0)
    return E_INVALIDARG;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  hProc = NULL;
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(dwProcessId, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    for (i=MODENUM_INITIALIZE_RETRIES; i>0; i--)
    {
      hRes = CNktDvTools::GetModules(hProc, nProcPlatformBits, aModulesList);
      if (hRes != E_FAIL)
        break;
      if (i > 1)
        ::Sleep(100);
    }
  }
  if (SUCCEEDED(hRes))
  {
    nCount = aModulesList.GetCount();
    for (i=0; i<nCount; i++)
    {
      hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProc, aModulesList[i], sModInfo);
      if (hRes != E_FAIL)
      {
        if (FAILED(hRes))
          break;
        if (sModInfo.nPlatformBits == nProcPlatformBits || sModInfo.nPlatformBits == 0)
        {
          hRes = AddItem(&sModInfo);
          if (FAILED(hRes))
            break;
        }
        else
        {
          hRes = S_OK;
        }
      }
      else
      {
        hRes = S_OK;
      }
    }
  }
  if (SUCCEEDED(hRes) || hRes == E_FAIL)
  {
    hRes = CNktDvTools::GetMappedFiles(dwProcessId, hProc, nProcPlatformBits, aModulesList.GetBuffer(),
                                       aModulesList.GetCount(), aMapList);
    nCount = aMapList.GetCount();
    for (i=0; i<nCount && SUCCEEDED(hRes); i++)
      hRes = AddItem(aMapList[i]);
  }
  if (hProc != NULL)
    ::CloseHandle(hProc);
  if (SUCCEEDED(hRes))
    hRes = RebuildSortedLists();
  if (SUCCEEDED(hRes))
  {
    dwPid = dwProcessId;
    nPlatformBits = nProcPlatformBits;
  }
  return hRes;
}

HRESULT CNktDvModulesEnumerator::AddItem(__in CNktDvTools::MODULE_INFO *lpModInfo)
{
  TNktComPtr<CNktDvModule> cNewModuleItem;

  //create new item
  cNewModuleItem.Attach(NKT_MEMMGR_NEW CNktDvModule);
  if (cNewModuleItem == NULL)
    return E_OUTOFMEMORY;
  cNewModuleItem->dwProcessId = lpModInfo->dwProcessId;
  cNewModuleItem->nPlatformBits = lpModInfo->nPlatformBits;
  cNewModuleItem->nKey = lpModInfo->nKey;
  cNewModuleItem->lpBaseAddress = lpModInfo->lpBaseAddr;
  cNewModuleItem->nBaseSize = lpModInfo->nBaseSize;
  cNewModuleItem->hModule = lpModInfo->hModule;
  if (cNewModuleItem->cStrPathW.Copy((LPWSTR)(lpModInfo->cStrExePathW)) == FALSE)
    return E_OUTOFMEMORY;
  cNewModuleItem->nNameOffsetInPathStr = lpModInfo->nOffsetToModuleName;
  return Add(cNewModuleItem); //add new item to the list
}

CNktDvModulesEnumerator* CNktDvModulesEnumerator::Clone()
{
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  SIZE_T i, nCount;

  cModEnum.Attach(NKT_MEMMGR_NEW CNktDvModulesEnumerator);
  if (cModEnum == NULL)
    return NULL;
  cModEnum->dwPid = dwPid;
  cModEnum->nPlatformBits = nPlatformBits;
  nCount = GetCount();
  for (i=0; i<nCount; i++)
  {
    if (FAILED(cModEnum->Add(aObjectsList[i])))
      return NULL;
  }
  cModEnum->cNameIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
  cModEnum->cAddrIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
  if (cModEnum->cNameIndexList == NULL ||
      cModEnum->cAddrIndexList == NULL)
    return NULL;
  nktMemCopy(cModEnum->cNameIndexList.Get(), cNameIndexList.Get(), nCount*sizeof(SIZE_T));
  nktMemCopy(cModEnum->cAddrIndexList.Get(), cAddrIndexList.Get(), nCount*sizeof(SIZE_T));
  return cModEnum.Detach();
}

HRESULT CNktDvModulesEnumerator::MarkModuleAsDirty(__in HINSTANCE hDll)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  SIZE_T i, nCount;

  nCount = sDirtyModules.aList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (sDirtyModules.aList[i] == hDll)
      return S_OK;
  }
  if (sDirtyModules.aList.AddElement(hDll) == FALSE)
    return E_OUTOFMEMORY;
  NktInterlockedExchange(&(sDirtyModules.nDoCheck), 1);
  return S_OK;
}

HRESULT CNktDvModulesEnumerator::CheckDirtyModules()
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, k, nCount, nDirtyCount;
  HINSTANCE hDll;
  HANDLE hProc;
  HRESULT hRes;

  if (sDirtyModules.nDoCheck == 0)
    return S_FALSE;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  hProc = NULL;
  if (SUCCEEDED(hRes))
  {
    hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
    if (hProc == NULL)
      hRes = E_ACCESSDENIED;
  }
  if (SUCCEEDED(hRes))
  {
    CNktAutoFastMutex cLock(&GetLockObject());

    if (sDirtyModules.nDoCheck != 0)
    {
      nCount = GetCount();
      nDirtyCount = sDirtyModules.aList.GetCount();
      for (k=0; SUCCEEDED(hRes) && k<nDirtyCount; k++)
      {
        hDll = sDirtyModules.aList[k];
        for (i=0; i<nCount; i++)
        {
          if (aObjectsList[i]->GetHandle() == hDll)
          {
            aObjectsList.RemoveElementAt(i);
            nCount--;
            break;
          }
        }
        hRes = CNktDvTools::GetModuleInfo(dwPid, hProc, hDll, sModInfo);
        if (hRes != E_FAIL)
        {
          if (SUCCEEDED(hRes) &&
              (sModInfo.nPlatformBits == nPlatformBits || sModInfo.nPlatformBits == 0))
            hRes = AddItem(&sModInfo);
        }
        else
        {
          hRes = S_OK;
        }
      }
    }
    if (SUCCEEDED(hRes))
      hRes = RebuildSortedLists();
    if (SUCCEEDED(hRes))
    {
      sDirtyModules.aList.RemoveAllElements();
      NktInterlockedExchange(&(sDirtyModules.nDoCheck), 0);
    }
  }
  if (hProc != NULL)
    ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvModulesEnumerator::RebuildSortedLists()
{
  SIZE_T i, nCount;

  cNameIndexList.Reset();
  cAddrIndexList.Reset();
  nCount = GetCount();
  if (nCount > 0)
  {
    cNameIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
    cAddrIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
    if (cNameIndexList == NULL || cAddrIndexList == NULL)
      return E_OUTOFMEMORY;
    for (i=0; i<nCount; i++)
      cNameIndexList[i] = cAddrIndexList[i] = i;
    qsort_s(cNameIndexList.Get(), nCount, sizeof(SIZE_T), SortByName, aObjectsList.GetBuffer());
    qsort_s(cAddrIndexList.Get(), nCount, sizeof(SIZE_T), SortByAddr, aObjectsList.GetBuffer());
  }
  return S_OK;
}

//-----------------------------------------------------------

static int __cdecl SearchByName(void *context, const void *key, const void *elem)
{
  CNktDvModule *lpMod;

  lpMod = ((sSearchModuleByName_Ctx*)context)->lplpModules[*((SIZE_T*)elem)];
  return MyStrICmpW((LPWSTR)key, ((sSearchModuleByName_Ctx*)context)->nNameLen, lpMod->GetFileName());
}

static int __cdecl SearchByAddr(void *context, const void *key, const void *elem)
{
  return NKT_DV_CMP<SIZE_T>((SIZE_T)(*((LPBYTE*)key)), 
                            (SIZE_T)((reinterpret_cast<CNktDvModule**>(context)[*((SIZE_T*)elem)])->
                                                                               GetBaseAddress()));
}

static int __cdecl SortByName(void *context, const void *elem1, const void *elem2)
{
  CNktDvModule *lpMod[2];

  lpMod[0] = reinterpret_cast<CNktDvModule**>(context)[*((SIZE_T*)elem1)];
  lpMod[1] = reinterpret_cast<CNktDvModule**>(context)[*((SIZE_T*)elem2)];
  return _wcsicmp(lpMod[0]->GetFileName(), lpMod[1]->GetFileName());
}

static int __cdecl SortByAddr(void *context, const void *elem1, const void *elem2)
{
  LPBYTE lpAddr[2];

  lpAddr[0] = (reinterpret_cast<CNktDvModule**>(context)[*((SIZE_T*)elem1)])->GetBaseAddress();
  lpAddr[1] = (reinterpret_cast<CNktDvModule**>(context)[*((SIZE_T*)elem2)])->GetBaseAddress();
  return NKT_DV_CMP<SIZE_T>((SIZE_T)(lpAddr[0]), (SIZE_T)(lpAddr[1]));
}
