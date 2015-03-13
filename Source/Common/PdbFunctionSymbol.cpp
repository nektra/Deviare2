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

#include "PdbFunctionSymbol.h"
#include <dia2.h>
#include <cvconst.h>
#include "ArrayList.h"
#include "StringLiteW.h"
#include "Tools.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

class CNktDvPdbSymbolManager : public CNktDvObject, public CNktFastMutex
{
public:
  CNktDvPdbSymbolManager();
  ~CNktDvPdbSymbolManager();

  static HRESULT Get(__deref_out CNktDvPdbSymbolManager **lplpPdbMgr);

  virtual ULONG __stdcall Release(); //override

  HRESULT Initialize();

  HRESULT GetDataSource(__in_z LPCWSTR szFileNameW, __in LPCWSTR szSymbolPathW,
                        __deref_out IDiaDataSource **lplpDataSrc);

  static HRESULT FindSymbolAddress(__in IDiaDataSource *lpDataSrc, __in_z LPCWSTR szSymbolW,
                                   __out SIZE_T *lpnAddress, __out DWORD *lpdwImageFileMachine);

private:
  typedef struct {
    LPWSTR szDllNameW;
    IDiaDataSource *lpDataSrc;
  } DATASRC_ITEM;

  static int DataSrcCompare(__in void *, __in DATASRC_ITEM *lpElem1, __in DATASRC_ITEM *lpElem2);
  static HRESULT GetSymbolFunctionName(__in IDiaSymbol *lpSymbol, __out BSTR *lpName);

  HINSTANCE hDll;
  TNktComPtr<IClassFactory> cCf;
  TNktArrayList4Structs<DATASRC_ITEM> aDataSrcList;
};

//-----------------------------------------------------------

static LONG volatile nDvPdbMgrLock = 0;
static CNktDvPdbSymbolManager *lpDvPdbMgr = NULL;

//-----------------------------------------------------------

CNktDvPdbFunctionSymbol::CNktDvPdbFunctionSymbol() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  nOffset = nPlatformBits = 0;
  return;
}

CNktDvPdbFunctionSymbol::~CNktDvPdbFunctionSymbol()
{
  return;
}

HRESULT CNktDvPdbFunctionSymbol::Create(__in_z LPCWSTR szDllNameW, __in_z LPCWSTR szFunctionNameW,
                         __in_z LPCWSTR szSymbolServerPathW, __in_z LPCWSTR szLocalCachePathW,
                         __deref_out CNktDvPdbFunctionSymbol **lplpSymbol)
{
  TNktComPtr<CNktDvPdbFunctionSymbol> cSym;
  HRESULT hRes;

  if (lplpSymbol != NULL)
    *lplpSymbol = NULL;
  if (lplpSymbol == NULL || szDllNameW == NULL || szFunctionNameW == NULL)
    return E_POINTER;
  if (szDllNameW[0] == NULL || szFunctionNameW[0] == NULL)
    return E_INVALIDARG;
  //get manager
  cSym.Attach(NKT_MEMMGR_NEW CNktDvPdbFunctionSymbol);
  if (cSym == NULL)
    return E_OUTOFMEMORY;
  hRes = CNktDvPdbSymbolManager::Get(&(cSym->cSymMgr));
  if (SUCCEEDED(hRes))
  {
    CNktAutoFastMutex cLock(cSym->cSymMgr);
    TNktComPtr<IDiaDataSource> cDataSrc;
    CNktStringW cStrDllPathW, cStrTempW, cStrSymbolPathW;
    DWORD dwImgType;

    hRes = CNktDvTools::SplitFileNameAndPath(cSym->cStrModuleNameW, cStrDllPathW, szDllNameW);
    if (SUCCEEDED(hRes))
    {
      if (cSym->cStrNameW.Copy(szFunctionNameW) == FALSE ||
          cSym->cStrCompleteNameW.Copy((LPWSTR)(cSym->cStrModuleNameW)) == FALSE ||
          cSym->cStrCompleteNameW.Concat(L"!") == FALSE ||
          cSym->cStrCompleteNameW.Concat((LPWSTR)(cSym->cStrNameW)) == FALSE)
        hRes = E_OUTOFMEMORY;
    }
    if (SUCCEEDED(hRes))
    {
      if (szLocalCachePathW != NULL && szLocalCachePathW[0] != 0)
      {
        if (szSymbolServerPathW != NULL && szSymbolServerPathW[0] != 0)
        {
          if (cStrSymbolPathW.Format(L"SRV*%s*%s", szLocalCachePathW, szSymbolServerPathW) == FALSE)
            hRes = E_OUTOFMEMORY;
        }
        else
        {
          if (cStrSymbolPathW.Copy(szLocalCachePathW) == FALSE)
            hRes = E_OUTOFMEMORY;
        }
      }
      else
      {
        if (szSymbolServerPathW != NULL && szSymbolServerPathW[0] != 0)
        {
          if (cStrSymbolPathW.Format(L"SRV**%s", szSymbolServerPathW) == FALSE)
            hRes = E_OUTOFMEMORY;
        }
      }
    }
    if (SUCCEEDED(hRes))
    {
      //get offsets
      hRes = cSym->cSymMgr->GetDataSource(szDllNameW, (cStrSymbolPathW.IsEmpty() == FALSE) ?
                                                       (LPWSTR)cStrSymbolPathW : NULL, &cDataSrc);
      if (SUCCEEDED(hRes))
      {
        hRes = cSym->cSymMgr->FindSymbolAddress(cDataSrc, szFunctionNameW, &(cSym->nOffset), &dwImgType);
        if (SUCCEEDED(hRes))
        {
          switch (dwImgType)
          {
          case IMAGE_FILE_MACHINE_I386:
            cSym->nPlatformBits = 32;
            break;
          case IMAGE_FILE_MACHINE_AMD64:
            cSym->nPlatformBits = 64;
            break;
          }
        }
      }
    }
  }
  *lplpSymbol = cSym.Detach();
  return hRes;
}



HRESULT CNktDvPdbFunctionSymbol::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvPdbFunctionSymbol [%s @ %IXh (%Iu-bits)]", GetName(), nOffset,
                      nPlatformBits) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

//-----------------------------------------------------------

CNktDvPdbSymbolManager::CNktDvPdbSymbolManager() : CNktDvObject(), CNktFastMutex()
{
  hDll = NULL;
  return;
}

CNktDvPdbSymbolManager::~CNktDvPdbSymbolManager()
{
  SIZE_T i, nCount;

  lpDvPdbMgr = NULL;
  //release objects before freeing the dll
  nCount = aDataSrcList.GetCount();
  for (i=0; i<nCount; i++)
  {
    nktMemFree(aDataSrcList[i].szDllNameW);
    aDataSrcList[i].lpDataSrc->Release();
  }
  aDataSrcList.RemoveAllElements();
  cCf.Release();
  //free library
  if (hDll != NULL)
    ::FreeLibrary(hDll);
  return;
}

HRESULT CNktDvPdbSymbolManager::Get(__deref_out CNktDvPdbSymbolManager **lplpPdbMgr)
{
  HRESULT hRes;

  if (lplpPdbMgr == NULL)
    return E_POINTER;
  hRes = S_OK;
  {
    CNktSimpleLockNonReentrant cLock(&nDvPdbMgrLock);

    if (lpDvPdbMgr == NULL)
    {
      lpDvPdbMgr = NKT_MEMMGR_NEW CNktDvPdbSymbolManager;
      hRes = (lpDvPdbMgr != NULL) ? S_OK : E_OUTOFMEMORY;
      if (SUCCEEDED(hRes))
        hRes = lpDvPdbMgr->Initialize();
    }
    else
    {
      lpDvPdbMgr->AddRef();
    }
  }
  if (SUCCEEDED(hRes))
  {
    *lplpPdbMgr = lpDvPdbMgr;
  }
  else
  {
    NKT_DV_RELEASE(lpDvPdbMgr);
    *lplpPdbMgr = NULL;
  }
  return hRes;
}

ULONG CNktDvPdbSymbolManager::Release()
{
  CNktSimpleLockNonReentrant cLock(&nDvPdbMgrLock);

  return CNktDvObject::Release();
}

HRESULT CNktDvPdbSymbolManager::Initialize()
{
  static const ULONG nVersions[] = { 110, 90, 80, 71, 70, 0 };
  typedef HRESULT (__stdcall *lpfnDllGetClassObject)(__in REFCLSID rclsid, __in REFIID riid,
                   __deref_out LPVOID FAR* ppv);
  lpfnDllGetClassObject fnDllGetClassObject;
  WCHAR szBufW[32];
  int i;
  HRESULT hRes;

  for (i=0; nVersions[i]!=0; i++)
  {
    swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"msdia%lu.dll", nVersions[i]);
    hDll = ::LoadLibraryW(szBufW);
    if (hDll != NULL)
      break;
  }
  hRes = NKT_DVERR_NotFound;
  if (hDll != NULL)
  {
    fnDllGetClassObject = (lpfnDllGetClassObject)::GetProcAddress(hDll, "DllGetClassObject");
    if (fnDllGetClassObject != NULL)
      hRes = fnDllGetClassObject(__uuidof(DiaSourceAlt), IID_IClassFactory, (void**)&cCf);
  }
  return hRes;
}

HRESULT CNktDvPdbSymbolManager::GetDataSource(__in_z LPCWSTR szFileNameW, __in LPCWSTR szSymbolPathW,
                                              __deref_out IDiaDataSource **lplpDataSrc)
{
  HRESULT hRes;
  CNktStringW cStrTempW;
  DATASRC_ITEM sItem, *lpItem;

  if (lplpDataSrc != NULL)
    *lplpDataSrc = NULL;
  if (szFileNameW == NULL || lplpDataSrc == NULL)
    return E_POINTER;
  if (szFileNameW[0] == 0)
    return E_INVALIDARG;
  //check if data source for this executable is already loaded
  sItem.szDllNameW = (LPWSTR)szFileNameW;
  lpItem = aDataSrcList.BinarySearchPtr(&sItem, &CNktDvPdbSymbolManager::DataSrcCompare, NULL);
  if (lpItem != NULL)
  {
    *lplpDataSrc = lpItem->lpDataSrc;
    lpItem->lpDataSrc->AddRef();
    hRes = S_OK;
  }
  else
  {
    //else create a new one
    hRes = (cStrTempW.Copy(szFileNameW) != FALSE) ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hRes))
      hRes = cCf->CreateInstance(NULL, __uuidof(IDiaDataSource), (void**)&(sItem.lpDataSrc));
    if (SUCCEEDED(hRes))
    {
      hRes = sItem.lpDataSrc->loadDataFromPdb(szFileNameW);
      if (FAILED(hRes))
        hRes = sItem.lpDataSrc->loadDataForExe(szFileNameW, szSymbolPathW, NULL);
    }
    if (SUCCEEDED(hRes))
    {
      sItem.szDllNameW = cStrTempW.Detach();
      if (aDataSrcList.SortedInsert(&sItem, &CNktDvPdbSymbolManager::DataSrcCompare, NULL) == FALSE)
      {
        sItem.lpDataSrc->Release();
        nktMemFree(sItem.szDllNameW);
        hRes = E_OUTOFMEMORY;
      }
    }
    if (SUCCEEDED(hRes))
    {
      *lplpDataSrc = sItem.lpDataSrc;
      sItem.lpDataSrc->AddRef();
    }
  }
  return hRes;
}

HRESULT CNktDvPdbSymbolManager::FindSymbolAddress(__in IDiaDataSource *lpDataSrc, __in_z LPCWSTR szSymbolW,
                                                  __out SIZE_T *lpnAddress,
                                                  __out DWORD *lpdwImageFileMachine)
{
  TNktComPtr<IDiaSession> cSession;
  TNktComPtr<IDiaEnumSymbolsByAddr> cSymbolsByAddr;
  TNktComPtr<IDiaSymbol> cSymbol;
  ULONG nCount;
  BOOL b;
  DWORD dwTag, dwLoc, dwMachineType, dwRva;
  BSTR bStrName;
  HRESULT hRes;

  NKT_ASSERT(lpDataSrc != NULL && lpdwImageFileMachine != NULL);
  NKT_ASSERT(szSymbolW != NULL && szSymbolW[0] != 0);
  NKT_ASSERT(lpnAddress != NULL);
  hRes = lpDataSrc->openSession(&cSession);
  if (SUCCEEDED(hRes))
  {
    dwMachineType = 0;
    hRes = cSession->get_globalScope(&cSymbol);
    if (SUCCEEDED(hRes))
    {
      hRes = cSymbol->get_machineType(&dwMachineType);
      if (SUCCEEDED(hRes) &&
#if defined _M_IX86
          dwMachineType != IMAGE_FILE_MACHINE_I386
#elif defined _M_X64
          dwMachineType != IMAGE_FILE_MACHINE_I386 && dwMachineType != IMAGE_FILE_MACHINE_AMD64
#endif
          )
        hRes = NKT_DVERR_NotFound;
    }
  }
  if (SUCCEEDED(hRes))
  {
    hRes = cSession->getSymbolsByAddr(&cSymbolsByAddr);
    if (SUCCEEDED(hRes))
    {
      cSymbol.Release();
      hRes = cSymbolsByAddr->symbolByAddr(1, 0, &cSymbol);
    }
  }
  if (FAILED(hRes))
    return hRes;
  while (SUCCEEDED(hRes))
  {
    hRes = cSymbol->get_symTag(&dwTag);
    if (SUCCEEDED(hRes) &&
        (dwTag == SymTagFunction || dwTag == SymTagPublicSymbol))
    {
      b = FALSE;
      bStrName = NULL;
      dwRva = dwLoc = 0;
      hRes = cSymbol->get_code(&b);
      if (SUCCEEDED(hRes))
        hRes = cSymbol->get_locationType(&dwLoc);
      if (SUCCEEDED(hRes))
        hRes = cSymbol->get_relativeVirtualAddress(&dwRva);
      if (SUCCEEDED(hRes) && b != FALSE && dwLoc == LocIsStatic && dwRva != 0)
      {
        hRes = GetSymbolFunctionName(cSymbol, &bStrName);
        if (SUCCEEDED(hRes) && bStrName != NULL &&
            //CNktDvTools::stristrW(bStrName, szSymbolW) != NULL
            wcscmp(bStrName, szSymbolW) == 0)
        {
          if (bStrName != NULL)
          {
            //with DiaDataSourceAlt we must use LocalFree instead of SysFreeString
            ::LocalFree((LPBYTE)bStrName-4);
          }
          *lpnAddress = (SIZE_T)dwRva;
          *lpdwImageFileMachine = dwMachineType;
          return S_OK;
        }
        if (bStrName != NULL)
        {
          //with DiaDataSourceAlt we must use LocalFree instead of SysFreeString
          ::LocalFree((LPBYTE)bStrName-4);
        }
      }
    }
    cSymbol.Release();
    nCount = 0;
    hRes = cSymbolsByAddr->Next(1, &cSymbol, &nCount);
    if (SUCCEEDED(hRes) && nCount != 1)
      hRes = E_FAIL;
  }
  return NKT_DVERR_NotFound;
}

int CNktDvPdbSymbolManager::DataSrcCompare(__in void *, __in DATASRC_ITEM *lpElem1,
                                           __in DATASRC_ITEM *lpElem2)
{
  return _wcsicmp(lpElem1->szDllNameW, lpElem2->szDllNameW);
}

HRESULT CNktDvPdbSymbolManager::GetSymbolFunctionName(__in IDiaSymbol *lpSymbol, __out BSTR *lpName)
{
  HRESULT hRes;
  BOOL bCppName;
  SIZE_T nLen;
  LPWSTR sW[2];

  hRes = lpSymbol->get_undecoratedNameEx(0x87FE, lpName);
  if (hRes != S_OK)
    hRes = lpSymbol->get_name(lpName);
  if (SUCCEEDED(hRes) && (*lpName) == NULL)
    hRes = E_FAIL;
  if (hRes == S_OK)
  {
    bCppName = (wcschr(*lpName, L':') != NULL) ? TRUE : FALSE;
    if (bCppName == FALSE)
    {
      if ((*lpName)[0] == L'_' || (*lpName)[0] == L'@')
      {
        //remove first char if stdcall or fastcall
        nLen = wcslen(*lpName);
        nktMemMove((*lpName), (*lpName)+1, (nLen+1)*sizeof(WCHAR));
      }
    }
    //strip stack size and/or params size
    sW[0] = wcschr(*lpName, L'(');
    sW[1] = wcschr(*lpName, L'@');
    if (sW[0] != NULL && sW[1] != NULL)
    {
      if (sW[0] < sW[1])
        sW[0][0] = 0;
      else
        sW[1][0] = 0;
    }
    else if (sW[0] != NULL)
    {
      sW[0][0] = 0;
    }
    else if (sW[1] != NULL)
    {
      sW[1][0] = 0;
    }
  }
  return hRes;
}
