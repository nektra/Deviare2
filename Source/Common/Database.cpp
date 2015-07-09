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

#include "Database.h"
#include "..\SupportLibs\Lz4\lz4.h"
#include "AutoPtr.h"
#include "Tools.h"
#include "FnvHash.h"
#include <search.h>
#include <sddl.h>

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//#define DUMP_DEFINITIONS_TO_FILE //use only for checking

//-----------------------------------------------------------

typedef struct {
  CNktDvDbModuleNoRef *lpDbModulesList;
  LPCWSTR szNameW;
  SIZE_T nNameLen;
} sSearchDbModuleByName_Ctx;

typedef struct tagSearchDbObjectByName_Ctx {
  CNktDvDbObjectNoRef *lpDbObjectsList;
  struct tagParsedName {
    LPCWSTR szNamespaceW;
    SIZE_T nNamespaceLen;
    LPCWSTR szNameW;
    SIZE_T nNameLen;
  } sParsedName;
} sSearchDbObjectByName_Ctx;

typedef struct {
  CNktDvDbModule **lpBuffer;
  LPCWSTR szNameW;
  SIZE_T nNameLen;
} sSearchDbModulesEnumByName_Ctx;

//-----------------------------------------------------------

static LONG volatile nDvDbLock = 0;
static CNktDvEngDatabase *lpDvDB_32 = NULL;
#if defined _M_X64
static CNktDvEngDatabase *lpDvDB_64 = NULL;
#endif //_M_X64

//-----------------------------------------------------------

static int __cdecl SearchDbModuleById(void *lpContext, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbModuleByName(void *, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbModuleFunctionByName(void *lpContext, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbObjectById(void *lpContext, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbObjectByName(void *, const void *lpKey, const void *lpObj);
static int __cdecl SortDbModuleByName(void *lpContext, const void *lpIdx1, const void *lpIdx2);
static int __cdecl SortDbModuleFunctionsByName(void *lpContext, const void *lpIdx1, const void *lpIdx2);
static int __cdecl SortDbObjectByName(void *lpContext, const void *lpIdx1, const void *lpIdx2);
static int __cdecl SortDbObjectByClassAndName(void *lpContext, const void *lpIdx1, const void *lpIdx2);

static int __cdecl SortDbModulesEnumByName(void *lpContext, const void *lpIdx1, const void *lpIdx2);
static int __cdecl SearchDbModulesEnumById(void *lpContext, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbModulesEnumByName(void *, const void *lpKey, const void *lpObj);
static SIZE_T DecompressDatabase(__in LPBYTE lpDest, __in LPBYTE lpSrc, __in SIZE_T nCompressedSize,
                                 __in SIZE_T nDecompressedSize);

static __inline int MyStrCmpW(__in const LPCWSTR szStr1, __in SIZE_T nStr1Len, __in const LPCWSTR szStr2)
{
  int nRes = wcsncmp(szStr1, szStr2, nStr1Len);
  if (nRes == 0 && szStr2[nStr1Len] != 0)
    nRes = -1; //szStr2 greater than szStr1
  return nRes;
}

static __inline int MyStrICmpW(__in const LPCWSTR szStr1, __in SIZE_T nStr1Len, __in const LPCWSTR szStr2)
{
  int nRes = _wcsnicmp(szStr1, szStr2, nStr1Len);
  if (nRes == 0 && szStr2[nStr1Len] != 0)
    nRes = -1; //szStr2 greater than szStr1
  return nRes;
}

//-----------------------------------------------------------

CNktDvEngDatabase::CNktDvEngDatabase() : CNktDvObject(), CNktFastMutex()
{
  lpDbModulesList = NULL;
  lpDbObjectsList = NULL;
  nDbModulesListCount = nDbObjectsListCount = 0;
  nktMemSet(sDbObjectClassPos, 0, sizeof(sDbObjectClassPos));
  nPlatformBits = 0;
  hDbMapFile = NULL;
  lpDbBuffer = NULL;
  return;
}

CNktDvEngDatabase::~CNktDvEngDatabase()
{
  BOOL bShutdownInProgress;

  nktDvDynApis_RtlDllShutdownInProgress(&bShutdownInProgress);
  if (bShutdownInProgress == FALSE)
    CNktFastMutex::Lock();
  //--------
  switch (nPlatformBits)
  {
    case 32:
      lpDvDB_32 = NULL;
      break;
#if defined _M_X64
    case 64:
      lpDvDB_64 = NULL;
      break;
#endif //_M_X64
  }
  //----
  Finalize();
  //--------
  if (bShutdownInProgress == FALSE)
    CNktFastMutex::Unlock();
  return;
}

HRESULT CNktDvEngDatabase::Get(__deref_out CNktDvEngDatabase **lplpDb, __in HANDLE hFileMap,
                               __in SIZE_T nPlatformBits)
{
  CNktDvEngDatabase **lplpDvDB;
  HRESULT hRes;

  if (lplpDb == NULL)
    return E_POINTER;
  switch (nPlatformBits)
  {
    case 32:
      lplpDvDB = &lpDvDB_32;
      break;
#if defined _M_X64
    case 64:
      lplpDvDB = &lpDvDB_64;
      break;
#endif //_M_X64
    default:
      *lplpDb = NULL;
      return E_INVALIDARG;
  }
  hRes = S_OK;
  {
    CNktSimpleLockNonReentrant cLock(&nDvDbLock);

    if ((*lplpDvDB) == NULL)
    {
      if (hFileMap != NULL)
      {
        *lplpDvDB = NKT_MEMMGR_NEW CNktDvEngDatabase;
        hRes = ((*lplpDvDB) != NULL) ? S_OK : E_OUTOFMEMORY;
        if (SUCCEEDED(hRes))
        {
          try
          {
            hRes = (*lplpDvDB)->Initialize(hFileMap, nPlatformBits);
          }
          catch (...)
          {
            hRes = NKT_DVERR_CannotLoadDatabase;
          }
        }
      }
      else
      {
        hRes = NKT_DVERR_NotFound;
      }
    }
    else
    {
      (*lplpDvDB)->AddRef();
    }
  }
  if (SUCCEEDED(hRes))
  {
    *lplpDb = (*lplpDvDB);
  }
  else
  {
    NKT_DV_RELEASE(*lplpDvDB);
    *lplpDb = NULL;
  }
  return hRes;
}

ULONG CNktDvEngDatabase::Release()
{
  CNktSimpleLockNonReentrant cLock(&nDvDbLock);

  return CNktDvObject::Release();
}

HANDLE CNktDvEngDatabase::GetDbFileMapping() const
{
  CNktAutoFastMutex cLock(const_cast<CNktDvEngDatabase*>(this));

  return hDbMapFile;
}

ULONG CNktDvEngDatabase::GetModulesCount() const
{
  return nDbModulesListCount;
}

CNktDvDbModuleNoRef* CNktDvEngDatabase::GetModule(__in ULONG nModId) const
{
  return ((SIZE_T)nModId < nDbModulesListCount) ? &lpDbModulesList[nModId] : NULL;
}

CNktDvDbModuleNoRef* CNktDvEngDatabase::FindModuleByName(__in_nz_opt LPCWSTR szNameW,
                                                         __in SIZE_T nNameLen) const
{
  sSearchDbModuleByName_Ctx sCtx;
  ULONG *lpIdx;

  if (szNameW == NULL)
    return NULL;
  if (nNameLen == NKT_SIZE_T_MAX)
    nNameLen = wcslen(szNameW);
  if (nNameLen == 0)
    return NULL;
  sCtx.lpDbModulesList = lpDbModulesList;
  sCtx.szNameW = szNameW;
  sCtx.nNameLen = nNameLen;
  lpIdx = (ULONG*)bsearch_s(&sCtx, (const_cast<TNktAutoFreePtr<ULONG>&>(cDbModuleNameIndex)).Get(),
                            nDbModulesListCount, sizeof(ULONG), SearchDbModuleByName, NULL);
  return (lpIdx != NULL) ? &lpDbModulesList[*lpIdx] : NULL;
}

ULONG CNktDvEngDatabase::GetModuleFunctionsCount(__in CNktDvDbModuleNoRef *lpDbMod) const
{
  NKT_ASSERT(lpDbMod != NULL);
  return lpDbMod->aFunctionsList.GetCount();
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetModuleFunction(__in CNktDvDbModuleNoRef *lpDbMod,
                                                     __in ULONG nFuncId) const
{
  NKT_ASSERT(lpDbMod != NULL);
  return (nFuncId < lpDbMod->aFunctionsList.GetCount()) ? lpDbMod->aFunctionsList[nFuncId] : NULL;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindModuleFunctionByName(__in_nz_opt CNktDvDbModuleNoRef *lpDbMod,
                                                            __in LPCWSTR szNameW,
                                                            __in SIZE_T nNameLen) const
{
  CNktDvDbObjectNoRef **lplpDbObj;

  if (lpDbMod == NULL || szNameW == NULL)
    return NULL;
  if (nNameLen == NKT_SIZE_T_MAX)
    nNameLen = wcslen(szNameW);
  if (nNameLen == 0)
    return NULL;
  NKT_ASSERT(lpDbMod != NULL);
  lplpDbObj = (CNktDvDbObjectNoRef**)bsearch_s(szNameW, lpDbMod->aFunctionsList.GetBuffer(),
                                         lpDbMod->aFunctionsList.GetCount(), sizeof(CNktDvDbObjectNoRef*),
                                         SearchDbModuleFunctionByName, &nNameLen);
  return (lplpDbObj != NULL) ? (*lplpDbObj) : NULL;
}

ULONG CNktDvEngDatabase::GetObjectsCount() const
{
  return nDbObjectsListCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetObject(__in ULONG nObjId) const
{
  return (nObjId < nDbObjectsListCount) ? &lpDbObjectsList[nObjId] : NULL;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindObjectById(__in ULONG nId) const
{
  return (CNktDvDbObjectNoRef*)bsearch_s(&nId, lpDbObjectsList, nDbObjectsListCount,
                                         sizeof(CNktDvDbObjectNoRef), SearchDbObjectById, NULL);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindObjectByName(__in_nz_opt LPCWSTR szNameW,
                                                         __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsMAX,
                                                                        szNameW, nNameLen);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetFundamental(__in CNktDvDbObjectNoRef::eFundamentalType
                                                       nFundType) const
{
  return ((int)nFundType >= 1 && (int)nFundType < CNktDvDbObjectNoRef::ftMAX) ?
                  lpDboFundamentals[(int)nFundType - 1] : NULL;
}

ULONG CNktDvEngDatabase::GetFunctionsCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsFunction           +1].nCount +
         sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsClassConstructor   +1].nCount +
         sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsClassDestructor    +1].nCount +
         sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsClassOperatorMethod+1].nCount +
         sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsClassMethod        +1].nCount +
         sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsClassConverter     +1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetFunction(__in ULONG nFuncId) const
{
  static CNktDvDbObjectNoRef::eClass aIndexes[6] = {
    CNktDvDbObjectNoRef::clsFunction,
    CNktDvDbObjectNoRef::clsClassConstructor,
    CNktDvDbObjectNoRef::clsClassDestructor,
    CNktDvDbObjectNoRef::clsClassOperatorMethod,
    CNktDvDbObjectNoRef::clsClassMethod,
    CNktDvDbObjectNoRef::clsClassConverter
  };
  SIZE_T i;

  for (i=0; i<6; i++)
  {
    if (nFuncId < sDbObjectClassPos[(int)aIndexes[i]+1].nCount)
    {
      return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(aIndexes[i], nFuncId);
    }
    nFuncId -= sDbObjectClassPos[aIndexes[i]+1].nCount;
  }
  return NULL;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindFunctionByName(__in_nz_opt LPCWSTR szNameW,
                                                           __in SIZE_T nNameLen) const
{
  static CNktDvDbObjectNoRef::eClass aIndexes[6] = {
    CNktDvDbObjectNoRef::clsFunction,
    CNktDvDbObjectNoRef::clsClassConstructor,
    CNktDvDbObjectNoRef::clsClassDestructor,
    CNktDvDbObjectNoRef::clsClassOperatorMethod,
    CNktDvDbObjectNoRef::clsClassMethod,
    CNktDvDbObjectNoRef::clsClassConverter
  };
  CNktDvDbObjectNoRef *lpDbObj, *lpDbObjTemp, *lpDbObjTempChild;
  CNktDvDbObjectNoRef::eClass nClass;
  SIZE_T i, k, nCount, nLastSep;
  LPWSTR sW;

  if (szNameW == NULL)
    return NULL;
  for (i=0; i<6; i++)
  {
    lpDbObj = const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(aIndexes[i], szNameW, nNameLen);
    if (lpDbObj != NULL)
      return lpDbObj;
  }
  //try to find an interface method
  if (nNameLen == NKT_SIZE_T_MAX)
    nNameLen = wcslen(szNameW);
  //find the last "::" before a parenthesis (if any)
  if (nNameLen < 2)
    goto next1;
  nLastSep = NKT_SIZE_T_MAX;
  for (k=0; k<nNameLen; k++)
  {
    if (szNameW[k] == L'(')
      break;
    if (szNameW[k] == L':' && szNameW[k+1] == L':' && szNameW[k+2] != L':')
      nLastSep = k;
  }
  if (nLastSep == 0 || nLastSep == NKT_SIZE_T_MAX || nLastSep == nNameLen-2 ||
      szNameW[nNameLen+2] == L'(')
    goto next1; //not a valid separator position
  //get a struct
  lpDbObjTemp = const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(
                                CNktDvDbObjectNoRef::clsStruct, szNameW, nLastSep);
  if (lpDbObjTemp == NULL)
    goto next1;
  //check if we are dealing with a struct that contains a vtable pointer as the first field
  nCount = lpDbObjTemp->GetItemsCount();
  for (i=1; i<nCount; i++)
  {
    lpDbObjTempChild = lpDbObjTemp->GetItem(i);
    nClass = lpDbObjTempChild->GetClass();
    if (nClass != CNktDvDbObjectNoRef::clsClassConstructor &&
        nClass != CNktDvDbObjectNoRef::clsClassDestructor &&
        nClass != CNktDvDbObjectNoRef::clsClassOperatorMethod &&
        nClass != CNktDvDbObjectNoRef::clsClassMethod)
      break;
  }
  lpDbObjTempChild = lpDbObjTemp->GetItem(0);
  if (i >= nCount &&
      lpDbObjTempChild->GetClass() == CNktDvDbObjectNoRef::clsPointer)
  {
    lpDbObjTempChild = lpDbObjTempChild->GetItem(0);
    if (lpDbObjTempChild != NULL &&
        lpDbObjTempChild->GetClass() == CNktDvDbObjectNoRef::clsStruct)
      lpDbObjTemp = lpDbObjTempChild; //got the real vtable struct
  }
  //now search the method
  nCount = lpDbObjTemp->GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    sW = lpDbObjTemp->GetItemName(i);
    if (MyStrCmpW(szNameW+(nLastSep+2), nNameLen-(nLastSep+2), sW) == 0)
    {
      lpDbObjTempChild = lpDbObjTemp->GetItem(i);
      if (lpDbObjTempChild->GetClass() == CNktDvDbObjectNoRef::clsPointer)
      {
        lpDbObjTempChild = lpDbObjTempChild->GetItem(0);
        if (lpDbObjTempChild != NULL &&
            lpDbObjTempChild->GetClass() == CNktDvDbObjectNoRef::clsFunctionType)
          return lpDbObjTempChild; //got the function
      }
    }
  }
next1:
  return NULL;
}

ULONG CNktDvEngDatabase::GetStructsCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsStruct+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetStruct(__in ULONG nStructId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsStruct,
                                                                         nStructId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindStructByName(__in_nz_opt LPCWSTR szNameW,
                                                    __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsStruct, szNameW,
                                                                        nNameLen);
}

ULONG CNktDvEngDatabase::GetUnionsCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsUnion+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetUnion(__in ULONG nUnionId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsUnion,
                                                                         nUnionId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindUnionByName(__in_nz_opt LPCWSTR szNameW, __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsUnion, szNameW,
                                                                        nNameLen);
}

ULONG CNktDvEngDatabase::GetArraysCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsArray+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetArray(__in ULONG nId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsArray, nId);
}

ULONG CNktDvEngDatabase::GetPointersCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsPointer+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetPointer(__in ULONG nPtrId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsPointer,
                                                                         nPtrId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindPointerByName(__in_nz_opt LPCWSTR szNameW,
                                                     __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsPointer,
                                                                        szNameW, nNameLen);
}

ULONG CNktDvEngDatabase::GetReferencesCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsReference+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetReference(__in ULONG nRefId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsReference,
                                                                         nRefId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindReferenceByName(__in_nz_opt LPCWSTR szNameW,
                                                       __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsReference,
                                                                        szNameW, nNameLen);
}

ULONG CNktDvEngDatabase::GetEnumsCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsEnumeration+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetEnum(__in ULONG nEnumId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsEnumeration,
                                                                         nEnumId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindEnumByName(__in_nz_opt LPCWSTR szNameW,
                                                  __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsEnumeration,
                                                                        szNameW, nNameLen);
}

ULONG CNktDvEngDatabase::GetFunctionTypesCount() const
{
  return sDbObjectClassPos[(int)CNktDvDbObjectNoRef::clsFunctionType+1].nCount;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::GetFunctionType(__in ULONG nFuncId) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndIndex(CNktDvDbObjectNoRef::clsFunctionType,
                                                                         nFuncId);
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindFunctionTypeByName(__in_nz_opt LPCWSTR szNameW,
                                                          __in SIZE_T nNameLen) const
{
  return const_cast<CNktDvEngDatabase*>(this)->FindObjectByClassAndName(CNktDvDbObjectNoRef::clsFunctionType,
                                                                        szNameW, nNameLen);
}

HRESULT CNktDvEngDatabase::CreateMapping(__out HANDLE *lphFileMap, __in_z LPCWSTR szDatabasePathW,
                                         __in SIZE_T _nPlatformBits)
{
  /*
  static LPCWSTR szW2kSID = L"D:P(D;OICI;GA;;;BG)" //deny guests
                            L"(D;OICI;GA;;;AU)" //deny authenticated users
                            L"(A;OICI;GA;;;SY)" //allow system
                            L"(A;OICI;GA;;;BA)"; //allow administrators
  static LPCWSTR szWXPOrLaterSID = L"D:P(D;OICI;GA;;;BG)" //deny guests
                                   L"(D;OICI;GA;;;AN)" //deny anonymous logon (XP and later)
                                   L"(D;OICI;GA;;;AU)" //deny authenticated users
                                   L"(A;OICI;GA;;;SY)" //allow system
                                   L"(A;OICI;GA;;;BA)"; //allow administrators
  */
  TNktAutoFreePtr<BYTE> aCompressedData;
  CNktStringW cStrDbFileNameW;
  SECURITY_ATTRIBUTES sSa;
  HRESULT hRes;
  HANDLE hFile, hFileMap;
  LPBYTE lpBuf;
  BYTE NKT_UNALIGNED *s;
  SIZE_T i, nCompressedSize, nDecompressedSize;
  ULONG dw;
  Fnv64_t nHashValue, nStoredHash;

  if (lphFileMap == NULL)
    return E_POINTER;
  *lphFileMap = NULL;
  hFile = hFileMap = NULL;
  lpBuf = NULL;
  //parse database filename
  if (szDatabasePathW == NULL || szDatabasePathW[0] == 0)
  {
    hRes = CNktDvTools::GetModulePath(cStrDbFileNameW, NULL);
  }
  else
  {
    if (cStrDbFileNameW.Copy(szDatabasePathW) != FALSE)
      hRes = CNktDvTools::FixFolderStr(cStrDbFileNameW);
    else
      hRes = E_OUTOFMEMORY;
  }
  if (FAILED(hRes))
  {
cdbm_err:
    if (lpBuf != NULL)
      ::UnmapViewOfFile(lpBuf);
    if (hFileMap != NULL && hFileMap != INVALID_HANDLE_VALUE)
      ::CloseHandle(hFileMap);
    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
      ::CloseHandle(hFile);
    return (hRes != E_OUTOFMEMORY) ? hRes/*NKT_DVERR_CannotLoadDatabase*/ : E_OUTOFMEMORY;
  }
  switch (_nPlatformBits)
  {
    case 32:
      if (cStrDbFileNameW.Concat(L"Deviare32.db") == FALSE)
        return E_OUTOFMEMORY;
      break;
#if defined _M_X64
    case 64:
      if (cStrDbFileNameW.Concat(L"Deviare64.db") == FALSE)
        return E_OUTOFMEMORY;
      break;
#endif //_M_X64
    default:
      return E_INVALIDARG;
  }
  //open database
  hFile = ::CreateFileW(cStrDbFileNameW, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
  {
cdbm_err_lasterr:
    hRes = NKT_HRESULT_FROM_LASTERROR();
    goto cdbm_err;
  }
  //load into memory
  nCompressedSize = (SIZE_T)::GetFileSize(hFile, NULL);
  if (nCompressedSize <= (17 + sizeof(ULONG) + sizeof(Fnv64_t)) ||
      nCompressedSize >= 0x0FFFFFFF)
  {
cdbm_err_fail:
    hRes = E_FAIL;
    goto cdbm_err;
  }
  aCompressedData.Attach((LPBYTE)nktMemMalloc(nCompressedSize));
  if (aCompressedData == NULL)
  {
    hRes = E_OUTOFMEMORY;
    goto cdbm_err;
  }
  if (::ReadFile(hFile, aCompressedData.Get(), (DWORD)nCompressedSize, &dw, NULL) == FALSE)
    goto cdbm_err_lasterr;
  if (nCompressedSize != (SIZE_T)dw)
    goto cdbm_err_fail;
  nCompressedSize -= (17 + sizeof(ULONG) + sizeof(Fnv64_t));
  //check signature and hash
  s = aCompressedData.Get();
  if (nktMemCompare(s, "Deviare Database\x1A", 17) != 0)
    goto cdbm_err_fail;
  s += 17;
  dw = *((ULONG NKT_UNALIGNED *)s);
  nDecompressedSize = (SIZE_T)dw;
  s += sizeof(ULONG);
  nStoredHash = *((Fnv64_t NKT_UNALIGNED *)s);
  s += sizeof(Fnv64_t);
  nHashValue = fnv_64a_buf(&dw, sizeof(dw), FNV1A_64_INIT);
  nHashValue = fnv_64a_buf(s, nCompressedSize, nHashValue);
  if (nHashValue != nStoredHash)
    goto cdbm_err_fail; //hash mismatch
  //create shared memory for decompressed data
  nktMemSet(&sSa, 0, sizeof(sSa));
  sSa.nLength = sizeof(SECURITY_ATTRIBUTES);
  dw = ::GetVersion();
  dw = ((LOBYTE(LOWORD(dw))) < 5 ||
        ((LOBYTE(LOWORD(dw))) == 5 && HIBYTE(LOWORD(dw)) == 0)) ? 0 : 1;
  //create a descriptor to allow access to administrators and system account. the rest will be denied by default
  //http://msdn.microsoft.com/en-us/library/windows/hardware/ff563667(v=vs.85).aspx
  //FILE_MAP_ALL_ACCESS | GENERIC_ALL = 0xF01F001F
  if (::ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:P(A;;0xF01F001F;;;SY)(A;;0xF01F001F;;;BA)",
                             SDDL_REVISION_1, &(sSa.lpSecurityDescriptor), NULL) == FALSE)
    goto cdbm_err_lasterr;
  /*
  if (::ConvertStringSecurityDescriptorToSecurityDescriptorW((dw == 0) ? szW2kSID : szWXPOrLaterSID,
                SDDL_REVISION_1, &(sSa.lpSecurityDescriptor), NULL) == FALSE)
    goto cdbm_err_lasterr;
  */
  hFileMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, &sSa, PAGE_READWRITE|SEC_COMMIT, 0,
                                 (DWORD)nDecompressedSize, NULL);
  hRes = (hFileMap != NULL && hFileMap != INVALID_HANDLE_VALUE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  ::LocalFree(sSa.lpSecurityDescriptor);
  if (FAILED(hRes))
    goto cdbm_err;
  lpBuf = (LPBYTE)::MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, nDecompressedSize);
  if (lpBuf == NULL)
    goto cdbm_err_lasterr;
  i = DecompressDatabase(lpBuf, s, nCompressedSize, nDecompressedSize);
  if (nDecompressedSize != i)
  {
    hRes = E_FAIL;
    goto cdbm_err;
  }
  //done
  ::UnmapViewOfFile(lpBuf);
  ::CloseHandle(hFile);
  *lphFileMap = hFileMap;
  return S_OK;
}

HRESULT CNktDvEngDatabase::Initialize(__in HANDLE hFileMap, __in SIZE_T _nPlatformBits)
{
  CNktAutoFastMutex cLock(this);
  HRESULT hRes;
  CNktDvDbModuleNoRef *lpDbMod;
  CNktDvDbObjectNoRef *lpDbObj;
  ULONG nDbModId, nDbObjId;
  int nLastClass, nCurrClass;
  SIZE_T i, j, nCount;
  DWORD dw;
  BYTE NKT_UNALIGNED *s, *s2;

  if (hDbMapFile != NULL)
    return S_OK; //only one instance
  //----
  if (hFileMap == NULL)
    return E_INVALIDARG;
  if (_nPlatformBits != 32
#if defined _M_X64
      && _nPlatformBits != 64
#endif //_M_X64
      )
    return E_INVALIDARG;
  //open view
  nPlatformBits = _nPlatformBits;
  hDbMapFile = hFileMap;
  lpDbBuffer = (LPBYTE)::MapViewOfFile(hDbMapFile, FILE_MAP_READ, 0, 0, 0);
  if (lpDbBuffer == NULL)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
init_err_cont:
    Finalize();
    return hRes;
  }
  //start copying data
  s = lpDbBuffer;
  //read super string
  nktMemCopy(&dw, s, sizeof(dw));
  s += sizeof(dw);
  szSuperStringW = (LPWSTR)s;
  s += (SIZE_T)dw;
  //read DbObjects count
  nktMemCopy(&dw, s, sizeof(dw));
  s += sizeof(dw);
  nDbObjectsListCount = (SIZE_T)dw;
  if (nDbObjectsListCount == 0)
  {
init_err_dbloadfail:
    hRes = NKT_DVERR_CannotLoadDatabase;
    goto init_err_cont;
  }
  lpDbObjectsList = NKT_MEMMGR_NEW CNktDvDbObjectNoRef[nDbObjectsListCount];
  if (lpDbObjectsList == NULL)
  {
init_err_nomem:
    hRes = E_OUTOFMEMORY;
    goto init_err_cont;
  }
  for (i=0; i<nDbObjectsListCount; i++)
    lpDbObjectsList[i].lpDvDB = this;
  //count DbObject childs
  for (i=nCount=0, s2=s; i<nDbObjectsListCount; i++)
  {
    s2 += sizeof(CNktDvDbObjectNoRef::STORED_DATA);
    //read DbObject childs count
    dw = *((DWORD NKT_UNALIGNED *)s2);
    s2 += sizeof(dw);
    nCount += (SIZE_T)dw;
    s2 += (SIZE_T)dw * sizeof(CNktDvDbObjectNoRef::STORED_CHILD_ITEM);
  }
  if (nCount > 0 &&
      aDbObjectChildsList.SetCount(nCount) == FALSE)
    goto init_err_nomem;
  nktMemSet(aDbObjectChildsList.GetBuffer(), 0, sizeof(nCount)*sizeof(CNktDvDbObjectNoRef::CHILD_ITEM));
  //process DbObject's
  for (i=nCount=0; i<nDbObjectsListCount; i++)
  {
    //read DbObject
    lpDbObjectsList[i].lpStoredData = (CNktDvDbObjectNoRef::LPSTORED_DATA)s;
    s += sizeof(CNktDvDbObjectNoRef::STORED_DATA);
    lpDbObjectsList[i].szNameW = szSuperStringW + (SIZE_T)(lpDbObjectsList[i].lpStoredData->nNameOffset);
    lpDbObjectsList[i].szNamespaceW = szSuperStringW +
                                      (SIZE_T)(lpDbObjectsList[i].lpStoredData->nNamespaceOffset);
    lpDbObjectsList[i].lpFirstChild =  NULL;
    lpDbObjectsList[i].nChildsCount = 0;
    if ((int)(lpDbObjectsList[i].lpStoredData->nClass) < CNktDvDbObjectNoRef::clsNull ||
        (int)(lpDbObjectsList[i].lpStoredData->nClass) >= CNktDvDbObjectNoRef::clsMAX)
      goto init_err_dbloadfail;
    //read DbObject childs
    dw = *((DWORD NKT_UNALIGNED *)s);
    s += sizeof(dw);
    lpDbObjectsList[i].lpFirstChild = aDbObjectChildsList.GetBuffer() + nCount;
    lpDbObjectsList[i].nChildsCount = (SIZE_T)dw;
    while (dw > 0)
    {
      aDbObjectChildsList[nCount].lpStoredChildItem = (CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM)s;
      s += sizeof(CNktDvDbObjectNoRef::STORED_CHILD_ITEM);
      aDbObjectChildsList[nCount].szNameW = szSuperStringW +
                (SIZE_T)(aDbObjectChildsList[nCount].lpStoredChildItem->nNameOffset);
      nCount++;
      dw--;
    }
  }
  //replace child object ids with pointers
  for (i=0; i<nDbObjectsListCount; i++)
  {
    nCount = lpDbObjectsList[i].nChildsCount;
    for (j=0; j<nCount; j++)
    {
      nDbObjId = lpDbObjectsList[i].lpFirstChild[j].lpStoredChildItem->nDbObjectId;
      if (nDbObjId != 0)
      {
        //search object
        lpDbObj = (CNktDvDbObjectNoRef*)bsearch_s(&nDbObjId, lpDbObjectsList, nDbObjectsListCount,
                                                  sizeof(CNktDvDbObjectNoRef), SearchDbObjectById, NULL);
        if (lpDbObj == NULL)
          goto init_err_dbloadfail;
      }
      else
      {
        lpDbObj = NULL;
      }
      lpDbObjectsList[i].lpFirstChild[j].lpObject = lpDbObj;
    }
  }
  //read DB_MODULEs count
  dw = *((DWORD NKT_UNALIGNED *)s);
  s += sizeof(dw);
  nDbModulesListCount = (SIZE_T)dw;
  if (nDbModulesListCount > 0)
  {
    lpDbModulesList = NKT_MEMMGR_NEW CNktDvDbModuleNoRef[nDbModulesListCount];
    if (lpDbModulesList == NULL)
      goto init_err_nomem;
    for (i=0; i<nDbModulesListCount; i++)
    {
      CNktDvDbObjectNoRef::STORED_MODULE NKT_UNALIGNED *lpStoredDbMod;

      lpDbModulesList[i].lpDvDB = this;
      //read DB_MODULE
      lpStoredDbMod = (CNktDvDbObjectNoRef::LPSTORED_MODULE)s;
      s += sizeof(CNktDvDbObjectNoRef::STORED_MODULE);
      lpDbModulesList[i].nId = lpStoredDbMod->nId;
      lpDbModulesList[i].szNameW = szSuperStringW + (SIZE_T)(lpStoredDbMod->nNameOffset);
    }
  }
  //read DB_MODULE_FUNCTIONs count
  dw = *((DWORD NKT_UNALIGNED *)s);
  s += sizeof(dw);
  nCount = (SIZE_T)dw;
  if (nCount != 0 && nDbModulesListCount == 0)
    goto init_err_dbloadfail; //invalid database
  for (i=0; i<nCount; i++)
  {
    //read DB_MODULE_FUNCTION
    nDbModId = *((ULONG NKT_UNALIGNED *)s);
    s += sizeof(nDbModId);
    nDbObjId = *((ULONG NKT_UNALIGNED *)s);
    s += sizeof(nDbObjId);
    //search module
    lpDbMod = (CNktDvDbModuleNoRef*)bsearch_s(&nDbModId, lpDbModulesList, nDbModulesListCount,
                                              sizeof(CNktDvDbModuleNoRef), SearchDbModuleById, NULL);
    if (lpDbMod == NULL)
      goto init_err_dbloadfail;
    //search object
    lpDbObj = (CNktDvDbObjectNoRef*)bsearch_s(&nDbObjId, lpDbObjectsList, nDbObjectsListCount,
                                              sizeof(CNktDvDbObjectNoRef), SearchDbObjectById, NULL);
    if (lpDbObj == NULL)
      goto init_err_dbloadfail;
    if (lpDbObj->GetClass() != CNktDvDbObjectNoRef::clsFunction)
      goto init_err_dbloadfail; //invalid
    //add this object to the module list
    if (lpDbMod->aFunctionsList.AddElement(lpDbObj) == FALSE)
      goto init_err_nomem;
    //add this module to the object list
    if (lpDbObj->aModulesList.AddElement(lpDbMod) == FALSE)
      goto init_err_nomem;
  }
  //sort modules by name
  if (nDbModulesListCount > 0)
  {
    cDbModuleNameIndex.Attach((ULONG*)nktMemMalloc(nDbModulesListCount*sizeof(ULONG)));
    if (cDbModuleNameIndex == NULL)
      goto init_err_nomem;
    for (i=0; i<nDbModulesListCount; i++)
      cDbModuleNameIndex[i] = (ULONG)i;
    qsort_s(cDbModuleNameIndex.Get(), nDbModulesListCount, sizeof(ULONG), SortDbModuleByName,
            lpDbModulesList);
    //sort module functions by name
    for (i=0; i<nDbModulesListCount; i++)
    {
      qsort_s(lpDbModulesList[i].aFunctionsList.GetBuffer(), lpDbModulesList[i].aFunctionsList.GetCount(),
              sizeof(CNktDvDbObjectNoRef*), SortDbModuleFunctionsByName, NULL);
    }
  }
  //sort objects by name
  cDbObjectNameIndex.Attach((ULONG*)nktMemMalloc(nDbObjectsListCount*sizeof(ULONG)));
  if (cDbObjectNameIndex == NULL)
    goto init_err_nomem;
  for (i=0; i<nDbObjectsListCount; i++)
    cDbObjectNameIndex[i] = (ULONG)i;
  qsort_s(cDbObjectNameIndex.Get(), nDbObjectsListCount, sizeof(ULONG), SortDbObjectByName,
          lpDbObjectsList);
  //sort objects by class and name
  cDbObjectClassAndNameIndex.Attach((ULONG*)nktMemMalloc(nDbObjectsListCount*sizeof(ULONG)));
  if (cDbObjectClassAndNameIndex == NULL)
    goto init_err_nomem;
  for (i=0; i<nDbObjectsListCount; i++)
    cDbObjectClassAndNameIndex[i] = (ULONG)i;
  qsort_s(cDbObjectClassAndNameIndex.Get(), nDbObjectsListCount, sizeof(ULONG),
          SortDbObjectByClassAndName, lpDbObjectsList);
  //get the first item of each class
  nktMemSet(&sDbObjectClassPos, 0, sizeof(sDbObjectClassPos));
  nLastClass = -1;
  for (i=0; i<nDbObjectsListCount; i++)
  {
    nCurrClass = (int)(lpDbObjectsList[cDbObjectClassAndNameIndex[i]].lpStoredData->nClass);
    if (nCurrClass != nLastClass)
    {
      sDbObjectClassPos[nCurrClass+1].nStart = (ULONG)i;
      sDbObjectClassPos[nCurrClass+1].nCount = 1;
      nLastClass = nCurrClass;
    }
    else
    {
      sDbObjectClassPos[nLastClass+1].nCount++;
    }
  }
  //find fundamentals
  nktMemSet(lpDboFundamentals, 0, sizeof(lpDboFundamentals));
  i = (SIZE_T)(sDbObjectClassPos[CNktDvDbObjectNoRef::clsFundamental+1].nStart);
  nCount = (SIZE_T)(sDbObjectClassPos[CNktDvDbObjectNoRef::clsFundamental+1].nCount);
  while (nCount > 0)
  {
    int k = (int)(lpDbObjectsList[i].GetFundamentalType());
    if (k > 0 && k < CNktDvDbObjectNoRef::ftMAX &&
        lpDboFundamentals[k-1] == NULL)
      lpDboFundamentals[k-1] = &lpDbObjectsList[i];
    i++;
    nCount--;
  }
  for (i=0; i<NKT_DV_ARRAYLEN(lpDboFundamentals); i++)
  {
    if (lpDboFundamentals[i] == NULL)
      goto init_err_dbloadfail;
  }
  //done
#ifdef DUMP_DEFINITIONS_TO_FILE
  {
    FILE *fp = fopen("c:\\deviare_definitions.txt", "wt");

    for (i=0; i<nDbObjectsListCount; i++)
    {
      fwprintf(fp, L"%s\n", lpDbObjectsList[i].GetDeclaration());
    }
    fclose(fp);
  }
#endif //DUMP_DEFINITIONS_TO_FILE
  return S_OK;
}

VOID CNktDvEngDatabase::Finalize()
{
  CNktAutoFastMutex cLock(this);
  SIZE_T i, nCount;

  if (lpDbModulesList != NULL)
  {
    delete [] lpDbModulesList;
    lpDbModulesList = NULL;
  }
  if (lpDbObjectsList != NULL)
  {
    delete [] lpDbObjectsList;
    lpDbObjectsList = NULL;
  }
  nCount = aDbObjectChildsList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (aDbObjectChildsList[i].szDeclarationW != NULL)
      nktMemFree(aDbObjectChildsList[i].szDeclarationW);
  }
  aDbObjectChildsList.RemoveAllElements();
  nDbModulesListCount = nDbObjectsListCount = 0;
  cDbModuleNameIndex.Reset();
  cDbObjectNameIndex.Reset();
  cDbObjectClassAndNameIndex.Reset();
  nPlatformBits = 0;
  nktMemSet(sDbObjectClassPos, 0, sizeof(sDbObjectClassPos));
  if (lpDbBuffer != NULL)
  {
    ::UnmapViewOfFile(lpDbBuffer);
    lpDbBuffer = NULL;
  }
  if (hDbMapFile != NULL)
  {
    ::CloseHandle(hDbMapFile);
    hDbMapFile = NULL;
  }
  return;
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindObjectByClassAndIndex(__in CNktDvDbObjectNoRef::eClass nClass,
                                                                  __in ULONG nId)
{
  NKT_ASSERT(nClass>=CNktDvDbObjectNoRef::clsNull && nClass<CNktDvDbObjectNoRef::clsMAX);
  if (nId >= sDbObjectClassPos[(int)nClass+1].nCount)
    return NULL;
  return &lpDbObjectsList[cDbObjectClassAndNameIndex[sDbObjectClassPos[(int)nClass+1].nStart +
                                                     (SIZE_T)nId]];
}

CNktDvDbObjectNoRef* CNktDvEngDatabase::FindObjectByClassAndName(__in CNktDvDbObjectNoRef::eClass nClass,
                                                                 __in_nz_opt LPCWSTR szNameW,
                                                                 __in SIZE_T nNameLen)
{
  sSearchDbObjectByName_Ctx sCtx;
  ULONG *lpIdx, *lpStart;
  SIZE_T nCount;

  NKT_ASSERT(nClass>=CNktDvDbObjectNoRef::clsNull && nClass<=CNktDvDbObjectNoRef::clsMAX);
  sCtx.sParsedName.szNameW = szNameW;
  sCtx.sParsedName.nNameLen = nNameLen;
  if (FAILED(ParseNamespaceAndName(sCtx.sParsedName.szNamespaceW, sCtx.sParsedName.nNamespaceLen,
                                   sCtx.sParsedName.szNameW, sCtx.sParsedName.nNameLen)))
    return NULL;
  sCtx.lpDbObjectsList = lpDbObjectsList;
  if (nClass == CNktDvDbObjectNoRef::clsMAX)
  {
    lpStart = cDbObjectNameIndex.Get();
    nCount = nDbObjectsListCount;
  }
  else
  {
    lpStart = cDbObjectClassAndNameIndex.Get() + (SIZE_T)(sDbObjectClassPos[(int)nClass+1].nStart);
    nCount = (SIZE_T)(sDbObjectClassPos[(int)nClass+1].nCount);
  }
fobcan_restart:
  //try to locate namespace/name object
  lpIdx = (ULONG*)bsearch_s(&sCtx, lpStart, nCount, sizeof(ULONG), SearchDbObjectByName, NULL);
  //on fail...
  if (lpIdx == NULL)
  {
    if (sCtx.sParsedName.nNamespaceLen == 0)
    {
      //...no namespace was specified to find the first item with any namespace
      sCtx.sParsedName.nNamespaceLen = NKT_SIZE_T_MAX;
      lpIdx = (ULONG*)bsearch_s(&sCtx, lpStart, nCount, sizeof(ULONG), SearchDbObjectByName, NULL);
      if (lpIdx != NULL)
      {
        //a match was found, move upwards until the first item
        while (lpIdx > lpStart)
        {
          if (MyStrCmpW(sCtx.sParsedName.szNameW, sCtx.sParsedName.nNameLen,
                        lpDbObjectsList[*(lpIdx-1)].GetName()) != 0)
            break;
          lpIdx--;
        }
      }
    }
    else
    {
      //...a namespace was provided but may be part of the name and not a namespace
      //so rebuild parsed name data and retry with the full name
      sCtx.sParsedName.nNameLen += (SIZE_T)(sCtx.sParsedName.szNameW - sCtx.sParsedName.szNamespaceW);
      sCtx.sParsedName.szNameW = sCtx.sParsedName.szNamespaceW;
      sCtx.sParsedName.szNamespaceW = L"";
      sCtx.sParsedName.nNamespaceLen = 0;
      goto fobcan_restart;
    }
  }
  return (lpIdx != NULL) ? &lpDbObjectsList[*lpIdx] : NULL;
}

HRESULT CNktDvEngDatabase::ParseNamespaceAndName(__out LPCWSTR &szNamespaceW, __out SIZE_T &nNamespaceLen,
                                                 __inout LPCWSTR &szNameW, __inout SIZE_T &nNameLen)
{
  LPCWSTR szSrcNameW;
  SIZE_T nSrcNameLen, k, k2;

  szSrcNameW = szNameW;
  nSrcNameLen = nNameLen;
  szNamespaceW = szNameW = NULL;
  nNamespaceLen = nNameLen = 0;
  if (szSrcNameW == NULL)
    return E_POINTER;
  if (nSrcNameLen == NKT_SIZE_T_MAX)
    nSrcNameLen = wcslen(szSrcNameW);
  //find namespace separator
  //first skip return values at beginning
  //look for arguments start if any
  for (k=0; k<nSrcNameLen && szSrcNameW[k] != L'('; k++);
  for (k2=k; k2>0 && szSrcNameW[k2-1]!=L' '; k2--);
  //k2 points to the first character after last space (after return value)
  szSrcNameW += k2;
  nSrcNameLen -= k2;
  szNamespaceW = szSrcNameW;
  //find and check for a valid namespace
  while (nSrcNameLen > 0 && szSrcNameW[0] == L'_')
  {
    if (nNamespaceLen >= 32)
    {
nan_not_a_namespace:
      szNameW = szNamespaceW;
      nNameLen = nNamespaceLen;
      szNamespaceW = L"";
      nNamespaceLen = 0;
      goto nan_process_name;
    }
    nNamespaceLen++;
    szSrcNameW++;
    nSrcNameLen--;
  }
  if (nSrcNameLen > 0 && szSrcNameW[0] >= L'0' && szSrcNameW[0] <= L'9')
    goto nan_not_a_namespace;
  while (nSrcNameLen > 0)
  {
    if (nSrcNameLen > 1 && szSrcNameW[0] == L':' && szSrcNameW[1] == L':')
      break;
    if (!((szSrcNameW[0] >= L'A' && szSrcNameW[0] <= L'Z') ||
          (szSrcNameW[0] >= L'a' && szSrcNameW[0] <= L'z') ||
          (szSrcNameW[0] >= L'0' && szSrcNameW[0] <= L'9')))
      goto nan_not_a_namespace;
    if (nNamespaceLen >= 32)
      goto nan_not_a_namespace;
    nNamespaceLen++;
    szSrcNameW++;
    nSrcNameLen--;
  }
  if (nSrcNameLen == 0)
    goto nan_not_a_namespace;
  //if we reach here, we have a valid (may be empty) namespace with the double colon separator
  NKT_ASSERT(nSrcNameLen > 1 && szSrcNameW[0] == L':' && szSrcNameW[1] == L':');
  nSrcNameLen -= 2;
  szSrcNameW += 2;
  szNameW = szSrcNameW;

nan_process_name:
  nNameLen += nSrcNameLen;
  while (nNameLen > 0 && szNameW[nNameLen-1] == L' ')
    nNameLen--;
  if (nNameLen == 0)
    return E_INVALIDARG;
  return S_OK;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvDbModuleNoRef::CNktDvDbModuleNoRef() : CNktMemMgrObj()
{
  nId = 0;
  szNameW = NULL;
  //----
  lpDvDB = NULL;
  return;
}

ULONG CNktDvDbModuleNoRef::GetId() const
{
  return nId;
}

LPWSTR CNktDvDbModuleNoRef::GetName() const
{
  return (szNameW != NULL) ? szNameW : L"";
}

SIZE_T CNktDvDbModuleNoRef::GetFunctionsCount() const
{
  return aFunctionsList.GetCount();
}

CNktDvDbObjectNoRef* CNktDvDbModuleNoRef::GetFunctionItem(__in SIZE_T nIndex) const
{
  NKT_ASSERT(nIndex < aFunctionsList.GetCount());
  return aFunctionsList[nIndex];
}

HRESULT CNktDvDbModuleNoRef::GetFunctions(__deref_out CNktDvDbObjectsEnumerator **lplpEnum)
{
  TNktComPtr<CNktDvDbObjectsEnumerator> cEnum;
  TNktComPtr<CNktDvDbObject> cDbObj;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbObjectsEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  nCount = aFunctionsList.GetCount();
  for (i=0; i<nCount; i++)
  {
    cDbObj.Release();
    cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(aFunctionsList[i]));
    if (cDbObj == NULL)
      return E_OUTOFMEMORY;
    hRes = cEnum->Add(cDbObj);
    if (FAILED(hRes))
      return hRes;
  }
  //done
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvDbModule::CNktDvDbModule() : CNktDvObject(), CNktDvDbModuleNoRef()
{
  return;
}

CNktDvDbModule* CNktDvDbModule::CreateFromNoRef(__in CNktDvDbModuleNoRef *lpBase)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  SIZE_T nCount;

  if (lpBase == NULL)
    return NULL;
  cDbMod.Attach(NKT_MEMMGR_NEW CNktDvDbModule);
  if (cDbMod != NULL)
  {
    cDbMod->nId = lpBase->nId;
    cDbMod->szNameW = lpBase->szNameW;
    nCount = lpBase->aFunctionsList.GetCount();
    if (cDbMod->aFunctionsList.SetCount(nCount) == FALSE)
      return NULL;
    nktMemCopy(cDbMod->aFunctionsList.GetBuffer(), lpBase->aFunctionsList.GetBuffer(),
               nCount*sizeof(CNktDvDbObjectNoRef*));
    //----
    cDbMod->cDvDB = lpBase->lpDvDB;
  }
  return cDbMod.Detach();
}

//-----------------------------------------------------------
//-----------------------------------------------------------

HRESULT CNktDvDbModulesEnumerator::GetItemById(__deref_out CNktDvDbModule** lplpDbMod,
                                               __in SIZE_T nId)
{
  CNktDvDbModule **lpDbMod;
  SIZE_T nCount;

  if (lplpDbMod == NULL)
    return E_POINTER;
  *lplpDbMod = NULL;
  nCount = GetCount();
  if (nCount > 0)
  {
    lpDbMod = (CNktDvDbModule**)bsearch_s(&nId, aObjectsList.GetBuffer(), nCount,
                                          sizeof(CNktDvDbModule*), SearchDbModulesEnumById, NULL);
    if (lpDbMod != NULL)
    {
      *lplpDbMod = *lpDbMod;
      (*lplpDbMod)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvDbModulesEnumerator::GetItemByName(__deref_out CNktDvDbModule** lplpDbMod,
                                                 __in_nz_opt LPCWSTR szNameW, __in SIZE_T nNameLen)
{
  sSearchDbModulesEnumByName_Ctx sCtx;
  SIZE_T nCount, *lpnIdx;

  if (lplpDbMod == NULL)
    return E_POINTER;
  *lplpDbMod = NULL;
  if (szNameW == NULL)
    return E_POINTER;
  if (nNameLen == NKT_SIZE_T_MAX)
    nNameLen = wcslen(szNameW);
  if (nNameLen == 0)
    return E_FAIL;
  //search the name
  nCount = GetCount();
  if (nCount > 0)
  {
    sCtx.lpBuffer = aObjectsList.GetBuffer();
    sCtx.szNameW = szNameW;
    sCtx.nNameLen = nNameLen;
    lpnIdx = (SIZE_T*)bsearch_s(&sCtx, cNameIndexList.Get(), nCount, sizeof(ULONG), SearchDbModulesEnumByName, NULL);
    if (lpnIdx != NULL)
    {
      *lplpDbMod = aObjectsList[*lpnIdx];
      (*lplpDbMod)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvDbModulesEnumerator::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvDbModulesEnumerator [%Iu items]", GetCount()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvDbModulesEnumerator::Initialize(__in CNktDvEngDatabase *lpDvDB)
{
  TNktComPtr<CNktDvDbModule> cDbMod;
  ULONG i, nCount;
  HRESULT hRes;

  NKT_ASSERT(lpDvDB != NULL);
  cDvDB = lpDvDB;
  //----
  nCount = lpDvDB->GetModulesCount();
  for (i=0; i<nCount; i++)
  {
    cDbMod.Release();
    cDbMod.Attach(CNktDvDbModule::CreateFromNoRef(lpDvDB->GetModule(i)));
    if (cDbMod == NULL)
      return E_OUTOFMEMORY;
    hRes = Add(cDbMod);
    if (FAILED(hRes))
      return hRes;
  }
  if (nCount > 0)
  {
    cNameIndexList.Attach((ULONG*)nktMemMalloc(nCount*sizeof(ULONG)));
    if (cNameIndexList == NULL)
      return E_OUTOFMEMORY;
    for (i=0; i<nCount; i++)
      cNameIndexList[i] = i;
    qsort_s(cNameIndexList.Get(), nCount, sizeof(ULONG), SortDbModulesEnumByName,
            aObjectsList.GetBuffer());
  }
  return S_OK;
}

//-----------------------------------------------------------

static int __cdecl SearchDbModuleById(void *lpContext, const void *lpKey, const void *lpObj)
{
  return NKT_DV_CMP<ULONG>(*((ULONG*)lpKey),
                           reinterpret_cast<const CNktDvDbModuleNoRef*>(lpObj)->GetId());
}

static int __cdecl SearchDbModuleByName(void *, const void *lpKey, const void *lpObj)
{
  sSearchDbModuleByName_Ctx *lpCtx = (sSearchDbModuleByName_Ctx*)lpKey;

  return MyStrICmpW(lpCtx->szNameW, lpCtx->nNameLen, lpCtx->lpDbModulesList[*((ULONG*)lpObj)].GetName());
}

static int __cdecl SearchDbModuleFunctionByName(void *lpContext, const void *lpKey, const void *lpObj)
{
  return MyStrICmpW((LPWSTR)lpKey, *((PSIZE_T)lpContext),
                    (*(reinterpret_cast<CNktDvDbObjectNoRef**>(const_cast<void*>(lpObj))))->GetName());
}

static int __cdecl SearchDbObjectById(void *lpContext, const void *lpKey, const void *lpObj)
{
  return NKT_DV_CMP<ULONG>(*((ULONG*)lpKey),
                           reinterpret_cast<const CNktDvDbObjectNoRef*>(lpObj)->GetId());
}

static int __cdecl SearchDbObjectByName(void *, const void *lpKey, const void *lpObj)
{
  sSearchDbObjectByName_Ctx *lpCtx = (sSearchDbObjectByName_Ctx*)lpKey;
  int res;

  res = MyStrCmpW(lpCtx->sParsedName.szNameW, lpCtx->sParsedName.nNameLen,
                  lpCtx->lpDbObjectsList[*((ULONG*)lpObj)].GetName());
  if (res == 0 && lpCtx->sParsedName.nNamespaceLen != NKT_SIZE_T_MAX)
  {
    res = MyStrCmpW(lpCtx->sParsedName.szNamespaceW, lpCtx->sParsedName.nNamespaceLen,
                    lpCtx->lpDbObjectsList[*((ULONG*)lpObj)].GetNamespace());
  }
  return res;
}

static int __cdecl SortDbModuleByName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  return _wcsicmp(reinterpret_cast<const CNktDvDbModuleNoRef*>(lpContext)[*((ULONG*)lpIdx1)].GetName(),
                  reinterpret_cast<const CNktDvDbModuleNoRef*>(lpContext)[*((ULONG*)lpIdx2)].GetName());
}

static int __cdecl SortDbModuleFunctionsByName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  const CNktDvDbObjectNoRef *lpDbObj[2];
  int res;

  lpDbObj[0] = *(reinterpret_cast<CNktDvDbObjectNoRef**>(const_cast<void*>(lpIdx1)));
  lpDbObj[1] = *(reinterpret_cast<CNktDvDbObjectNoRef**>(const_cast<void*>(lpIdx2)));
  //sort by name
  res = wcscmp(lpDbObj[0]->GetName(), lpDbObj[1]->GetName());
  if (res != 0)
    return res;
  //subsort by namespace
  return wcscmp(lpDbObj[0]->GetNamespace(), lpDbObj[1]->GetNamespace());
}

static int __cdecl SortDbObjectByName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  const CNktDvDbObjectNoRef *lpDbObj[2];
  int res;

  lpDbObj[0] = &(reinterpret_cast<const CNktDvDbObjectNoRef*>(lpContext)[*((ULONG*)lpIdx1)]);
  lpDbObj[1] = &(reinterpret_cast<const CNktDvDbObjectNoRef*>(lpContext)[*((ULONG*)lpIdx2)]);
  //sort by name
  res = wcscmp(lpDbObj[0]->GetName(), lpDbObj[1]->GetName());
  if (res != 0)
    return res;
  //subsort by namespace
  return wcscmp(lpDbObj[0]->GetNamespace(), lpDbObj[1]->GetNamespace());
}

static int __cdecl SortDbObjectByClassAndName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  const CNktDvDbObjectNoRef *lpDbObj[2];
  int res;

  lpDbObj[0] = &(reinterpret_cast<const CNktDvDbObjectNoRef*>(lpContext)[*((ULONG*)lpIdx1)]);
  lpDbObj[1] = &(reinterpret_cast<const CNktDvDbObjectNoRef*>(lpContext)[*((ULONG*)lpIdx2)]);
  //sort by class
  res = NKT_DV_CMP<SIZE_T>((SIZE_T)(lpDbObj[0]->GetClass()), (SIZE_T)(lpDbObj[1]->GetClass()));
  if (res != 0)
    return res;
  //subsort by name
  res = wcscmp(lpDbObj[0]->GetName(), lpDbObj[1]->GetName());
  if (res != 0)
    return res;
  //subsubsort by namespace
  return wcscmp(lpDbObj[0]->GetNamespace(), lpDbObj[1]->GetNamespace());
}

//-----------------------------------------------------------

static int __cdecl SortDbModulesEnumByName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  return _wcsicmp(reinterpret_cast<const CNktDvDbModule**>(lpContext)[*((ULONG*)lpIdx1)]->GetName(),
                  reinterpret_cast<const CNktDvDbModule**>(lpContext)[*((ULONG*)lpIdx2)]->GetName());
}

static int __cdecl SearchDbModulesEnumById(void *lpContext, const void *lpKey, const void *lpObj)
{
  return NKT_DV_CMP<SIZE_T>(*((SIZE_T*)lpKey),
             (SIZE_T)((*reinterpret_cast<CNktDvDbModule**>(const_cast<void*>(lpObj)))->GetId()));
}

static int __cdecl SearchDbModulesEnumByName(void *, const void *lpKey, const void *lpObj)
{
  sSearchDbModulesEnumByName_Ctx *lpCtx = (sSearchDbModulesEnumByName_Ctx*)lpKey;

  return MyStrICmpW(lpCtx->szNameW, lpCtx->nNameLen, lpCtx->lpBuffer[*((ULONG*)lpObj)]->GetName());
}

static SIZE_T DecompressDatabase(__in LPBYTE lpDest, __in LPBYTE lpSrc, __in SIZE_T nCompressedSize,
                                 __in SIZE_T nDecompressedSize)
{
  SIZE_T i;

  __try
  {
    i = (SIZE_T)LZ4_uncompress_unknownOutputSize((char*)lpSrc, (char*)lpDest, (int)nCompressedSize,
                                                 (int)nDecompressedSize);
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  {
    i = NKT_SIZE_T_MAX; //force fail on error
  }
  return i;
}
