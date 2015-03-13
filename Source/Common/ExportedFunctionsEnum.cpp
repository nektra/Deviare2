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

#include "ExportedFunctionsEnum.h"
#include "..\Common\ProcessMemory.h"
#include "..\Common\NtInternals.h"
#include "ModulesEnum.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

typedef struct {
  CNktDvExportedFunction **lpBuffer;
  SIZE_T nNameLen;
} sSearchByName_Ctx;

//-----------------------------------------------------------

static int __cdecl SearchByIndex(void *lpContext, const void *lpKey, const void *elem);
static int __cdecl SearchByName(void *lpContext, const void *lpKey, const void *elem);
static int __cdecl SearchByAddr(void *lpContext, const void *lpKey, const void *elem);
static int __cdecl SortByName(void *lpContext, const void *elem1, const void *elem2);
static int __cdecl SortByAddr(void *lpContext, const void *elem1, const void *elem2);

static __inline int MyStrCmpW(__in const LPWSTR szStr1, __in SIZE_T nStr1Len, __in const LPWSTR szStr2)
{
  int nRes = _wcsnicmp(szStr1, szStr2, nStr1Len);
  if (nRes == 0 && szStr2[nStr1Len] != 0)
    nRes = -1; //szStr2 greater than szStr1
  return nRes;
}

//-----------------------------------------------------------

HRESULT CNktDvExportedFunctionsEnumerator::GetItemByOrdinal(__deref_out CNktDvExportedFunction** lplpFunc,
                                                            __in SIZE_T nOrdinal)
{
  CNktDvExportedFunction **lpFunc;
  SIZE_T nCount;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  nCount = GetCount();
  if (nCount > 0)
  {
    lpFunc = (CNktDvExportedFunction**)bsearch_s(&nOrdinal, aObjectsList.GetBuffer(), nCount,
                                                 sizeof(CNktDvExportedFunction*), SearchByIndex, NULL);
    if (lpFunc != NULL)
    {
      *lplpFunc = *lpFunc;
      (*lplpFunc)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvExportedFunctionsEnumerator::GetItemByName(__deref_out CNktDvExportedFunction** lplpFunc,
                                                         __in_nz_opt LPCWSTR szNameW, __in SIZE_T nNameLen)
{
  CNktDvTools::FUNCTION_PARSE_INFO sFuncParseInfo;
  sSearchByName_Ctx sCtx;
  SIZE_T nCount, *lpnIdx;
  HRESULT hRes;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = NULL;
  //parse function name
  hRes = CNktDvTools::ParseFunctionName(sFuncParseInfo, szNameW, nNameLen);
  if (FAILED(hRes))
    return hRes;
  if (sFuncParseInfo.nDllNameLen != 0)
    return E_INVALIDARG;
  //if ordinal value was specified
  if (sFuncParseInfo.nApiOrdinal != NKT_SIZE_T_MAX)
    return GetItemByOrdinal(lplpFunc, sFuncParseInfo.nApiOrdinal);
  //search the name
  nCount = GetCount();
  if (nCount > 0)
  {
    sCtx.lpBuffer = aObjectsList.GetBuffer();
    sCtx.nNameLen = sFuncParseInfo.nApiNameLen;
    lpnIdx = (SIZE_T*)bsearch_s(szNameW+sFuncParseInfo.nApiNameStart, cNameIndexList.Get(), nCount,
                                sizeof(SIZE_T), SearchByName, &sCtx);
    if (lpnIdx != NULL)
    {
      *lplpFunc = aObjectsList[*lpnIdx];
      (*lplpFunc)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvExportedFunctionsEnumerator::GetItemByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                            __in_opt LPVOID lpAddr,
                                                            __in BOOL bSearchNearest)
{
  CNktDvExportedFunction *lpFunc;
  SIZE_T nCount, nLo, nHi, nMiddle, nAddr, *lpnIdx;

  if (lplpFunc == NULL)
    return E_POINTER;
  *lplpFunc = lpFunc = NULL;
  nCount = GetCount();
  if (nCount > 0)
  {
    nAddr = (SIZE_T)lpAddr;
    if (bSearchNearest == FALSE)
    {
      lpnIdx = (SIZE_T*)bsearch_s(&nAddr, cAddrIndexList.Get(), nCount, sizeof(SIZE_T), SearchByAddr,
                                  aObjectsList.GetBuffer());
      if (lpnIdx != NULL)
        lpFunc = aObjectsList[*lpnIdx];
    }
    else
    {
      //search "insertion point"
      for (nLo=0,nHi=nCount; nLo<nHi; )
      {
        nMiddle = (nLo+nHi) / 2;
        lpFunc = aObjectsList[cAddrIndexList[nMiddle]];
        if (lpFunc->GetAddr() <= nAddr)
          nLo = nMiddle+1;
        else
          nHi = nMiddle;
      }
      lpFunc = (nLo > 0) ? aObjectsList[cAddrIndexList[nLo-1]] : NULL;
    }
    if (lpFunc != NULL)
    {
      *lplpFunc = lpFunc;
      lpFunc->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvExportedFunctionsEnumerator::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvExportedFunctionsEnumerator [%Iu items]", GetCount()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvExportedFunctionsEnumerator::Initialize(__in CNktDvModule *lpModule,
                                                      __in CNktDvModulesEnumerator *lpModEnum)
{
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<CNktDvModule> cFwdMod;
  CNktStringW cStrReplDllW, cStrTempW[3];
  SIZE_T i, j, nCount, nModPlatformBits, nExpDirSize, nFwdExpDirSize;
  NKT_DV_IMAGE_NT_HEADER sNtLdr, sFwdNtLdr;
  IMAGE_EXPORT_DIRECTORY sExpDir, sFwdExpDir;
  HANDLE hProcess;
  LPBYTE lpBaseAddr, lpExpDir, lpFwdExpDir, lpFuncAddr, lpFwdFuncAddr, lpFwdBaseAddr;
  LPDWORD lpdwAddressOfFunctions, lpdwAddressOfNames, lpdwFwdAddressOfFunctions, lpdwFwdAddressOfNames;
  LPWORD lpwAddressOfNameOrdinals, lpwFwdAddressOfNameOrdinals;
  TNktComPtr<CNktDvExportedFunction> cFunc;
  LPSTR sA;
  LPWSTR sW;
  DWORD dwTemp32;
  LONG nTemp32;
  WORD wTemp16;
  HRESULT hRes;

  hRes = CNktDvEngDatabase::Get(&cDvDB, NULL, lpModule->GetPlatformBits());
  if (FAILED(hRes) && hRes != E_INVALIDARG)
    return hRes;
  cProcMem.Attach(CNktDvProcessMemory::CreateForPID(lpModule->GetProcessId()));
  if (cProcMem == NULL)
    return E_OUTOFMEMORY;
  hProcess = cProcMem->GetReadAccessHandle();
  if (hProcess == NULL)
    return E_ACCESSDENIED;
  lpBaseAddr = lpModule->GetBaseAddress();
  if (lpBaseAddr == NULL)
    return E_FAIL;
  //check image type
  hRes = nktDvNtCheckImageType(&sNtLdr, hProcess, lpBaseAddr);
  if (FAILED(hRes))
    return hRes;
  nModPlatformBits = (SIZE_T)hRes;
  //export directory address
  switch (nModPlatformBits)
  {
    case 32:
      if (sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
          sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
        return S_OK; //empty or no export table in module
      lpExpDir = lpBaseAddr + (SIZE_T)(sNtLdr.u32.OptionalHeader.
                                       DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      nExpDirSize = (SIZE_T)(sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
      hRes = cProcMem->Read(&sExpDir, lpExpDir, sizeof(sExpDir));
      break;
#if defined _M_X64
    case 64:
      if (sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
          sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
        return S_OK; //empty or no export table in module
      lpExpDir = lpBaseAddr + (SIZE_T)(sNtLdr.u64.OptionalHeader.
                                       DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      nExpDirSize = (SIZE_T)(sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
      hRes = cProcMem->Read(&sExpDir, lpExpDir, sizeof(sExpDir));
      break;
#endif //_M_X64
    default:
      return E_FAIL;
  }
  if (FAILED(hRes))
    return hRes;
  //get addresses
  lpdwAddressOfFunctions = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfFunctions));
  lpdwAddressOfNames = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNames));
  lpwAddressOfNameOrdinals = (LPWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNameOrdinals));
  //process each exported function
  for (i=0; i<(SIZE_T)(sExpDir.NumberOfFunctions); i++)
  {
    //get address of function
    hRes = cProcMem->Read(&dwTemp32, lpdwAddressOfFunctions+i, sizeof(DWORD));
    if (FAILED(hRes))
      return hRes;
    if (dwTemp32 == NULL)
      continue;
    //create new function
    cFunc.Attach(NKT_MEMMGR_NEW CNktDvExportedFunction);
    if (cFunc == NULL)
      return E_OUTOFMEMORY;
    cFunc->cDvDB = cDvDB;
    //set the ordinal
    cFunc->nOrdinal = i + (SIZE_T)(sExpDir.Base);
    cFunc->nAddrBase = (SIZE_T)lpBaseAddr;
    cFunc->nAddrOffset = (SIZE_T)dwTemp32;
    //set the module name
    if (cFunc->cStrModuleNameW.Copy(lpModule->GetFileName()) == FALSE)
      return E_OUTOFMEMORY;
    //retrieve function address
    lpFuncAddr = lpBaseAddr + cFunc->nAddrOffset;
    if (lpFuncAddr >= lpExpDir && lpFuncAddr < lpExpDir+nExpDirSize)
    {
      //it is a forward declaration
      cFunc->bIsForwarded = TRUE;
      //get the module name
      hRes = cProcMem->ReadStringA(&sA, lpFuncAddr, 2048);
      if (FAILED(hRes))
        return hRes;
      hRes = (cFunc->cStrForwardedModuleNameW.Copy(sA) != FALSE) ? S_OK : E_OUTOFMEMORY;
      nktMemFree(sA);
      if (FAILED(hRes))
        return hRes;
      //find a dot in module name
      for (sW=cFunc->cStrForwardedModuleNameW; *sW!=0 && *sW!=L'.'; sW++);
      if (*sW != L'.')
        continue; //invalid forward, skip this item and continue
      //split api name
      if (cFunc->cStrForwardedApiNameW.Copy(sW+1) == FALSE)
        return E_OUTOFMEMORY;
      if (cFunc->cStrForwardedApiNameW.IsEmpty() != FALSE)
        continue; //invalid forward, skip this item and continue
      //truncate module name
      *sW = 0;
      cFunc->cStrForwardedModuleNameW.Refresh();
      if (cFunc->cStrForwardedModuleNameW.IsEmpty() != FALSE)
        continue; //invalid forward, skip this item and continue
      if (cFunc->cStrForwardedModuleNameW.Concat(L".dll") == FALSE)
        return E_OUTOFMEMORY;
      if (cStrTempW[0].Copy((LPWSTR)(cFunc->cStrForwardedModuleNameW)) == FALSE ||
          cStrTempW[1].Copy((LPWSTR)(cFunc->cStrForwardedApiNameW)) == FALSE)
        return E_OUTOFMEMORY;
      //find the address of the forwarded function
      for (;;) //we may have to recurse
      {
        cFwdMod.Release();
        hRes = lpModEnum->GetItemByName(&cFwdMod, (LPWSTR)cStrTempW[0]);
        lpFwdBaseAddr = (SUCCEEDED(hRes)) ? cFwdMod->GetBaseAddress() : NULL;
        if (lpFwdBaseAddr == NULL)
        {
          hRes = nktDvNtFindApiMapSet(cStrReplDllW, hProcess, nModPlatformBits, (LPWSTR)cStrTempW[0],
                                      (LPWSTR)(cFunc->cStrModuleNameW));
          if (hRes != NKT_DVERR_NotFound)
          {
            if (FAILED(hRes))
              return hRes;
            cFwdMod.Release();
            hRes = lpModEnum->GetItemByName(&cFwdMod, (LPWSTR)cStrReplDllW);
            lpFwdBaseAddr = (SUCCEEDED(hRes)) ? cFwdMod->GetBaseAddress() : NULL;
          }
        }
        if (lpFwdBaseAddr == NULL)
          break; //invalid forward, halt
        //check image type
        hRes = nktDvNtCheckImageType(&sFwdNtLdr, hProcess, lpFwdBaseAddr);
        if (FAILED(hRes))
          return hRes;
        if (nModPlatformBits != (SIZE_T)hRes)
        {
          lpFwdBaseAddr = NULL; //invalid forward, skip this item and continue
          break;
        }
        //export directory address
        lpFwdExpDir = NULL;
        switch (nModPlatformBits)
        {
          case 32:
            if (sFwdNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].
                                             VirtualAddress == 0 ||
                sFwdNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
              break; //empty or no export table in module
            lpFwdExpDir = lpFwdBaseAddr + (SIZE_T)(sFwdNtLdr.u32.OptionalHeader.DataDirectory[
                                                  IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
            nFwdExpDirSize = (SIZE_T)(sFwdNtLdr.u32.OptionalHeader.DataDirectory[
                                          IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
            hRes = cProcMem->Read(&sFwdExpDir, lpFwdExpDir, sizeof(sFwdExpDir));
            break;
#if defined _M_X64
          case 64:
            if (sFwdNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].
                                             VirtualAddress == 0 ||
                sFwdNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
              break; //empty or no export table in module
            lpFwdExpDir = lpFwdBaseAddr + (SIZE_T)(sFwdNtLdr.u64.OptionalHeader.DataDirectory[
                                                  IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
            nFwdExpDirSize = (SIZE_T)(sFwdNtLdr.u64.OptionalHeader.DataDirectory[
                                          IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
            hRes = cProcMem->Read(&sFwdExpDir, lpFwdExpDir, sizeof(sFwdExpDir));
            break;
#endif //_M_X64
        }
        if (FAILED(hRes))
          return hRes;
        if (lpFwdExpDir == NULL)
        {
          lpFwdBaseAddr = NULL; //empty or no export table in module, halt
          break;
        }
        //get addresses
        lpdwFwdAddressOfFunctions = (LPDWORD)(lpFwdBaseAddr + (SIZE_T)(sFwdExpDir.AddressOfFunctions));
        lpdwFwdAddressOfNames = (LPDWORD)(lpFwdBaseAddr + (SIZE_T)(sFwdExpDir.AddressOfNames));
        lpwFwdAddressOfNameOrdinals = (LPWORD)(lpFwdBaseAddr + (SIZE_T)(sFwdExpDir.AddressOfNameOrdinals));
        //find the api in the forwarded module
        sW = cStrTempW[1];
        if (sW[0] == L'#')
        {
          //find the ordinal entry
          nTemp32 = _wtol(sW+1) - (LONG)(sFwdExpDir.Base);
        }
        else
        {
          //find the named entry in the exported names
          SIZE_T nLow, nHigh;
          int res;

          nTemp32 = -1; //set a fake ordinal if name is not found
          if (sFwdExpDir.NumberOfNames > 0)
          {
            nLow = 1;
            nHigh = (SIZE_T)(sFwdExpDir.NumberOfNames);
            while (nLow <= nHigh)
            {
              j = nLow + (nHigh-nLow) / 2;
              //get the name address
              hRes = cProcMem->Read(&dwTemp32, lpdwFwdAddressOfNames+(j-1), sizeof(DWORD));
              if (FAILED(hRes))
                return hRes;
              if (dwTemp32 == NULL)
              {
                nLow = 1;  nHigh = 0;
                break;
              }
              //get the name
              hRes = cProcMem->ReadStringA(&sA, lpFwdBaseAddr+(SIZE_T)dwTemp32, 2048);
              if (FAILED(hRes))
                return hRes;
              hRes = (cStrTempW[2].Copy(sA) != FALSE) ? S_OK : E_OUTOFMEMORY;
              nktMemFree(sA);
              if (FAILED(hRes))
                return hRes;
              //check if it is the name we are looking for
              res = wcscmp((LPWSTR)cStrTempW[2], sW);
              if (res == 0)
                break;
              if (res < 0)
                nLow = j + 1;
              else
                nHigh = j - 1;
            }
            if (nLow <= nHigh)
            {
              //get the ordinal
              hRes = cProcMem->Read(&wTemp16, lpwFwdAddressOfNameOrdinals+(j-1), sizeof(WORD));
              if (FAILED(hRes))
                return hRes;
              nTemp32 = (LONG)(DWORD)wTemp16;
            }
          }
        }
        if (nTemp32 < 0 || (DWORD)nTemp32 >= sFwdExpDir.NumberOfFunctions)
        {
          lpFwdBaseAddr = NULL; //invalid forward, halt
          break;
        }
        //get address of function
        hRes = cProcMem->Read(&dwTemp32, lpdwFwdAddressOfFunctions+nTemp32, sizeof(DWORD));
        if (FAILED(hRes))
          return hRes;
        if (dwTemp32 == NULL)
        {
          lpFwdBaseAddr = NULL; //invalid forward, halt
          break;
        }
        lpFwdFuncAddr = lpFwdBaseAddr + (SIZE_T)dwTemp32;
        if (lpFwdFuncAddr < lpFwdExpDir || lpFwdFuncAddr >= lpFwdExpDir+nFwdExpDirSize)
        {
          //we hit the destination, fill info
          cFunc->nAddrBase = (SIZE_T)lpFwdBaseAddr;
          cFunc->nAddrOffset = (SIZE_T)dwTemp32;
          break; //we are done
        }
        //we hit a recursive forward, get the module name
        hRes = cProcMem->ReadStringA(&sA, lpFwdFuncAddr, 2048);
        if (FAILED(hRes))
          return hRes;
        hRes = (cStrTempW[0].Copy(sA) != FALSE) ? S_OK : E_OUTOFMEMORY;
        nktMemFree(sA);
        if (FAILED(hRes))
          return hRes;
        //find a dot in module name
        for (sW=cStrTempW[0]; *sW!=0 && *sW!=L'.'; sW++);
        if (*sW != L'.')
        {
          lpFwdBaseAddr = NULL;
          break; //invalid forward, halt
        }
        //split api name
        if (cStrTempW[1].Copy(sW+1) == FALSE)
          return E_OUTOFMEMORY;
        if (cStrTempW[1].IsEmpty() != FALSE)
        {
          lpFwdBaseAddr = NULL;
          break; //invalid forward, halt
        }
        //truncate module name
        *sW = 0;
        cStrTempW[0].Refresh();
        if (cStrTempW[0].IsEmpty() != FALSE)
        {
          lpFwdBaseAddr = NULL;
          break; //invalid forward, halt
        }
        if (cStrTempW[0].Concat(L".dll") == FALSE)
          return E_OUTOFMEMORY;
        //recurse...
      }
      if (lpFwdBaseAddr == NULL)
        continue; //invalid forward, skip this item and continue
    }
    //add new item to the list
    hRes = Add(cFunc);
    if (FAILED(hRes))
      return hRes;
    cFunc.Release();
  }
  //process the exported names
  for (i=0; i<(SIZE_T)(sExpDir.NumberOfNames); i++)
  {
    //get the ordinal
    hRes = cProcMem->Read(&wTemp16, lpwAddressOfNameOrdinals+i, sizeof(WORD));
    if (FAILED(hRes))
      return hRes;
    //find the function
    cFunc.Release();
    hRes = GetItemByOrdinal(&cFunc, (SIZE_T)wTemp16+(SIZE_T)(sExpDir.Base));
    if (FAILED(hRes))
      continue; //a name pointing to an invalid ordinal? skip
    //get the name address
    hRes = cProcMem->Read(&dwTemp32, lpdwAddressOfNames+i, sizeof(DWORD));
    if (FAILED(hRes))
      return hRes;
    if (dwTemp32 == NULL)
      continue;
    //get the name
    hRes = cProcMem->ReadStringA(&sA, lpBaseAddr+(SIZE_T)dwTemp32, 2048);
    if (FAILED(hRes))
      return hRes;
    hRes = (cFunc->cStrNameW.Copy(sA) != FALSE) ? S_OK : E_OUTOFMEMORY;
    nktMemFree(sA);
    if (FAILED(hRes))
      return hRes;
  }
  //set complete name
  nCount = GetCount();
  for (i=0; i<nCount; i++)
  {
    if (aObjectsList[i]->cStrCompleteNameW.Copy(aObjectsList[i]->cStrModuleNameW) == FALSE ||
        aObjectsList[i]->cStrCompleteNameW.Concat(L"!") == FALSE)
      return E_OUTOFMEMORY;
    if (aObjectsList[i]->cStrNameW.IsEmpty() == FALSE)
    {
      if (aObjectsList[i]->cStrCompleteNameW.Concat(aObjectsList[i]->cStrNameW) == FALSE)
        return E_OUTOFMEMORY;
    }
    else
    {
      if (aObjectsList[i]->cStrCompleteNameW.Concat(L"#") == FALSE ||
          aObjectsList[i]->cStrCompleteNameW.Concat(aObjectsList[i]->nOrdinal) == FALSE)
        return E_OUTOFMEMORY;
    }
    //and forwarded name
    if (aObjectsList[i]->bIsForwarded != FALSE)
    {
      if (aObjectsList[i]->cStrForwardedCompleteNameW.Copy(aObjectsList[i]->cStrForwardedModuleNameW) ==
                                       FALSE ||
          aObjectsList[i]->cStrForwardedCompleteNameW.Concat(L"!") == FALSE ||
          aObjectsList[i]->cStrForwardedCompleteNameW.Concat(aObjectsList[i]->cStrForwardedApiNameW) ==
                                       FALSE)
        return E_OUTOFMEMORY;
    }
  }
  //build sorted lists
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
  //search DBObject of each item
  nCount = GetCount();
  if (cDvDB != NULL)
  {
    for (i=0; i<nCount; i++)
    {
      if (aObjectsList[i]->cStrNameW.IsEmpty() == FALSE)
        aObjectsList[i]->lpDbObj = cDvDB->FindFunctionByName(aObjectsList[i]->cStrNameW);
    }
  }
  return S_OK;
}

CNktDvExportedFunctionsEnumerator* CNktDvExportedFunctionsEnumerator::Clone()
{

  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncEnum;
  SIZE_T i, nCount;

  cExpFuncEnum.Attach(NKT_MEMMGR_NEW CNktDvExportedFunctionsEnumerator);
  if (cExpFuncEnum == NULL)
    return NULL;
  cExpFuncEnum->cDvDB = cDvDB;
  nCount = GetCount();
  for (i=0; i<nCount; i++)
  {
    if (FAILED(cExpFuncEnum->Add(aObjectsList[i])))
      return NULL;
  }
  cExpFuncEnum->cNameIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
  cExpFuncEnum->cAddrIndexList.Attach((SIZE_T*)nktMemMalloc(nCount*sizeof(SIZE_T)));
  if (cExpFuncEnum->cNameIndexList == NULL ||
      cExpFuncEnum->cAddrIndexList == NULL)
    return NULL;
  nktMemCopy(cExpFuncEnum->cNameIndexList.Get(), cNameIndexList.Get(), nCount*sizeof(SIZE_T));
  nktMemCopy(cExpFuncEnum->cAddrIndexList.Get(), cAddrIndexList.Get(), nCount*sizeof(SIZE_T));
  return cExpFuncEnum.Detach();
}

//-----------------------------------------------------------

static int __cdecl SearchByIndex(void *lpContext, const void *lpKey, const void *elem)
{
  return NKT_DV_CMP<SIZE_T>(*((SIZE_T*)lpKey),
             (*reinterpret_cast<CNktDvExportedFunction**>(const_cast<void*>(elem)))->GetOrdinal());
}

static int __cdecl SearchByName(void *lpContext, const void *lpKey, const void *elem)
{
  return MyStrCmpW((LPWSTR)lpKey, ((sSearchByName_Ctx*)lpContext)->nNameLen, 
                   (((sSearchByName_Ctx*)lpContext)->lpBuffer[*((SIZE_T*)elem)])->GetName());
}

static int __cdecl SearchByAddr(void *lpContext, const void *lpKey, const void *elem)
{
  return NKT_DV_CMP<SIZE_T>(*((SIZE_T*)lpKey),
             (reinterpret_cast<CNktDvExportedFunction**>(lpContext)[*((SIZE_T*)elem)])->GetAddr());
}

static int __cdecl SortByName(void *lpContext, const void *elem1, const void *elem2)
{
  LPWSTR szNameW[2];

  szNameW[0] = (reinterpret_cast<CNktDvExportedFunction**>(lpContext)[*((SIZE_T*)elem1)])->GetName();
  szNameW[1] = (reinterpret_cast<CNktDvExportedFunction**>(lpContext)[*((SIZE_T*)elem2)])->GetName();
  return _wcsicmp(szNameW[0], szNameW[1]);
}

static int __cdecl SortByAddr(void *lpContext, const void *elem1, const void *elem2)
{
  return NKT_DV_CMP<SIZE_T>(
             (reinterpret_cast<CNktDvExportedFunction**>(lpContext)[*((SIZE_T*)elem1)])->GetAddr(),
             (reinterpret_cast<CNktDvExportedFunction**>(lpContext)[*((SIZE_T*)elem2)])->GetAddr());
}
