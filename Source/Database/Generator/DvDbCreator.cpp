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

#include "DvDbCreator.h"
#include <limits.h>
#include "..\..\Common\StringLiteW.h"
#include "..\..\Common\FnvHash.h"
#include "..\..\SupportLibs\Lz4\lz4.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

CDvDbCreator::CDvDbCreator() : CNktMemMgrObj()
{
  return;
}

CDvDbCreator::~CDvDbCreator()
{
  aObjectsList.RemoveAllElements();
  return;
}

BOOL CDvDbCreator::BuildObjects(CSQLiteDbLoader &cLdr)
{
  SRC_DBO_MAPPING sDboMap, *lpDboMap;
  CNktDvDbObject *lpDbObj;
  SIZE_T i, j, k, nCount;

  PrintAndLog(L"Building objects... ");
  //add strings
  nCount = cLdr.aStringsList.GetCount();
  if (aStringOffsetsList.SetCount(nCount) == FALSE)
  {
bo_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  for (i=k=0; i<nCount; i++)
  {
    aStringOffsetsList[i] = k;
    k += wcslen(cLdr.aStringsList[i]) + 1;
  }
  nSuperStringSize = (ULONG)(k*sizeof(WCHAR));
  cSuperStringW.Attach((LPWSTR)nktMemMalloc(nSuperStringSize));
  if (cSuperStringW.Get() == NULL)
    goto bo_err_nomem;
  for (i=k=0; i<nCount; i++)
  {
    j = wcslen(cLdr.aStringsList[i]) + 1;
    nktMemCopy(cSuperStringW.Get()+k, cLdr.aStringsList[i], j*sizeof(WCHAR));
    k += j;
  }
  //add empty object
  lpDbObj = AddDbObject();
  if (lpDbObj == NULL)
    goto bo_err_nomem;
  //basic types (setup mappings)
  nCount = cLdr.aBasicTypesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFundamental;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Fundamental;
    sDboMap.nTypeId = cLdr.aBasicTypesList[i].nId;
    sDboMap.nFlags = cLdr.aBasicTypesList[i].nTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|
                                                           NKT_DBOBJCLASSFLAG_IsVolatile);
    sDboMap.lpObj = lpDbObj;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //structs (setup mappings)
  nCount = cLdr.aStructList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsStruct;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Struct;
    sDboMap.nTypeId = cLdr.aStructList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //unions (setup mappings)
  nCount = cLdr.aUnionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsUnion;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Union;
    sDboMap.nTypeId = cLdr.aUnionList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //arrays (setup mappings)
  nCount = cLdr.aArrayList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsArray;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Array;
    sDboMap.nTypeId = cLdr.aArrayList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //pointers (setup mappings)
  nCount = cLdr.aPointerList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsPointer;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Pointer;
    sDboMap.nTypeId = cLdr.aPointerList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //references (setup mappings)
  nCount = cLdr.aReferenceList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsReference;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Reference;
    sDboMap.nTypeId = cLdr.aReferenceList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //enumerations (setup mappings)
  nCount = cLdr.aEnumList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsEnumeration;
    sDboMap.nTypeClass = NKT_DBOBJCLASS_Enumeration;
    sDboMap.nTypeId = cLdr.aEnumList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //functions (setup mappings)
  nCount = cLdr.aFunctionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = AddDbObject();
    if (lpDbObj == NULL)
      goto bo_err_nomem;
    lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFunctionType;
    sDboMap.nTypeClass = cLdr.aFunctionList[i].nClass;
    switch (sDboMap.nTypeClass)
    {
      case NKT_DBOBJCLASS_Function:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFunction;
        break;
      case NKT_DBOBJCLASS_FunctionType:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFunctionType;
        break;
      case NKT_DBOBJCLASS_ClassConstructor:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassConstructor;
        break;
      case NKT_DBOBJCLASS_ClassDestructor:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassDestructor;
        break;
      case NKT_DBOBJCLASS_ClassOperatorMethod:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassOperatorMethod;
        break;
      case NKT_DBOBJCLASS_ClassMethod:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassMethod;
        break;
      case NKT_DBOBJCLASS_ClassConverter:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassConverter;
        break;
      default:
        PrintAndLog(L"\nError: Invalid function class [%lu].\n", sDboMap.nTypeClass);
        return FALSE;
    }
    sDboMap.nTypeId = cLdr.aFunctionList[i].nId;
    sDboMap.lpObj = lpDbObj;
    sDboMap.nFlags = 0;
    if (aMappingsList.AddElement(&sDboMap) == FALSE)
      goto bo_err_nomem;
  }
  //add default namespace to mappings with is empty in generated databases
  nCount = aMappingsList.GetCount();
  for (i=0; i<nCount; i++)
  {
    aMappingsList[i].lpObj->sCreationHelpers.szNamespaceW = cSuperStringW.Get();
  }
  //sort mappings
  qsort_s(aMappingsList.GetBuffer(), nCount, sizeof(SRC_DBO_MAPPING), DboMap_Compare, NULL);
  //basic types (fill object)
  nCount = cLdr.aBasicTypesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", ((double)(i+1)/(double)nCount)*20.0f);
    lpDboMap = DboMap_Find(cLdr.aBasicTypesList[i].nId, NKT_DBOBJCLASS_Fundamental);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    k = cLdr.FindStringInStringList(cLdr.aBasicTypesList[i].szNameW);
    NKT_ASSERT(k != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[k];
    switch (cLdr.aBasicTypesList[i].nTypeClass & NKT_DBOBJCLASS_MASK)
    {
      case NKT_DBOBJCLASS_Fundamental:
        lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFundamental;
        lpDbObj->sCreationHelpers.nFundamentalType = (CNktDvDbObjectNoRef::eFundamentalType)
                                              (cLdr.aBasicTypesList[i].nTypeId);
        break;
      case NKT_DBOBJCLASS_Struct:
        if (BO_ProcessStruct(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                             cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Union:
        if (BO_ProcessUnion(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                            cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Array:
        if (BO_ProcessArray(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                            cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Pointer:
        if (BO_ProcessPointer(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                              cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Reference:
        if (BO_ProcessReference(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                                cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Enumeration:
        if (BO_ProcessEnumeration(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                                  cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      case NKT_DBOBJCLASS_Function:
      case NKT_DBOBJCLASS_FunctionType:
      case NKT_DBOBJCLASS_ClassConstructor:
      case NKT_DBOBJCLASS_ClassDestructor:
      case NKT_DBOBJCLASS_ClassOperatorMethod:
      case NKT_DBOBJCLASS_ClassMethod:
      case NKT_DBOBJCLASS_ClassConverter:
        if (BO_ProcessFunction(lpDbObj, cLdr, cLdr.aBasicTypesList[i].nTypeId,
                               cLdr.aBasicTypesList[i].nTypeClass) == FALSE)
          return FALSE;
        break;
      default:
        NKT_ASSERT(FALSE);
        break;
    }
  }
  //structs (fill object)
  nCount = cLdr.aStructList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 20.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aStructList[i].nId, NKT_DBOBJCLASS_Struct);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    k = cLdr.FindStringInStringList(cLdr.aStructList[i].szNameW);
    NKT_ASSERT(k != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[k];
    if (BO_ProcessStruct(lpDbObj, cLdr, cLdr.aStructList[i].nId, NKT_DBOBJCLASS_Struct) == FALSE)
      return FALSE;
  }
  //unions (fill object)
  nCount = cLdr.aUnionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 30.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aUnionList[i].nId, NKT_DBOBJCLASS_Union);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    k = cLdr.FindStringInStringList(cLdr.aUnionList[i].szNameW);
    NKT_ASSERT(k != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[k];
    if (BO_ProcessUnion(lpDbObj, cLdr, cLdr.aUnionList[i].nId, NKT_DBOBJCLASS_Union) == FALSE)
      return FALSE;
  }
  //arrays (fill object)
  nCount = cLdr.aArrayList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 40.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aArrayList[i].nId, NKT_DBOBJCLASS_Array);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    if (BO_ProcessArray(lpDbObj, cLdr, cLdr.aArrayList[i].nId, NKT_DBOBJCLASS_Array) == FALSE)
      return FALSE;
  }
  //pointers (fill object)
  nCount = cLdr.aPointerList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 50.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aPointerList[i].nId, NKT_DBOBJCLASS_Pointer);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    if (BO_ProcessPointer(lpDbObj, cLdr, cLdr.aPointerList[i].nId, NKT_DBOBJCLASS_Pointer) == FALSE)
      return FALSE;
  }
  //references (fill object)
  nCount = cLdr.aReferenceList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 60.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aReferenceList[i].nId, NKT_DBOBJCLASS_Reference);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    if (BO_ProcessReference(lpDbObj, cLdr, cLdr.aReferenceList[i].nId, NKT_DBOBJCLASS_Reference) == FALSE)
      return FALSE;
  }
  //enumerations (fill object)
  nCount = cLdr.aEnumList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 70.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aEnumList[i].nId, NKT_DBOBJCLASS_Enumeration);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    if (BO_ProcessEnumeration(lpDbObj, cLdr, cLdr.aEnumList[i].nId, NKT_DBOBJCLASS_Enumeration) == FALSE)
      return FALSE;
  }
  //functions (fill object)
  nCount = cLdr.aFunctionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    //xxx wprintf(L"\rBuilding DB-Objects (%.2lf%%)... ", 80.0f+((double)(i+1)/(double)nCount)*10.0f);
    lpDboMap = DboMap_Find(cLdr.aFunctionList[i].nId, cLdr.aFunctionList[i].nClass);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbObj != NULL);
    if (BO_ProcessFunction(lpDbObj, cLdr, cLdr.aFunctionList[i].nId,
                           cLdr.aFunctionList[i].nClass) == FALSE)
      return FALSE;
  }
  //fix fundamental type sizes
  //xxx wprintf(L"\rFixing fundamental type sizes... ");
  nCount = aObjectsList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (aObjectsList[i]->sCreationHelpers.nClass == CNktDvDbObjectNoRef::clsFundamental)
    {
      aObjectsList[i]->sCreationHelpers.nSize = CNktDvDbObject::GetFundamentalTypeSize(
        (CNktDvDbObjectNoRef::eFundamentalType)(aObjectsList[i]->sCreationHelpers.nFundamentalType));
    }
  }
  //done
  PrintAndLog(L"OK\n");
  return TRUE;
}

VOID CDvDbCreator::BuildHashes()
{
  SIZE_T i, nCount;
  Fnv64_t nHashValue;

  PrintAndLog(L"Hashing objects... ");
  nCount = aObjectsList.GetCount();
  for (i=0; i<nCount; i++)
  {
    nHashValue = FNV1A_64_INIT;
    BuildHashes_GetObjectHash(i, 0, nHashValue);
    aObjectsList[i]->sCreationHelpers.nHashValue = (ULONGLONG)nHashValue;
  }
  PrintAndLog(L"OK\n");
  return;
}

VOID CDvDbCreator::BuildHashes_GetObjectHash(SIZE_T nIndex, SIZE_T nDepth, Fnv64_t &nHashValue)
{
  CNktDvDbObject::LPDBGEN_CHILD_ITEM lpChildItem;
  ULONG nTemp32;
  SIZE_T k, nChildsCount, nLen;

  if (nDepth > 4)
    return;
  //hash this object
  nTemp32 = (ULONG)(aObjectsList[nIndex]->sCreationHelpers.nClass);
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  nTemp32 = (ULONG)(aObjectsList[nIndex]->sCreationHelpers.nFundamentalType);
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  if (aObjectsList[nIndex]->sCreationHelpers.szNameW != NULL &&
      aObjectsList[nIndex]->sCreationHelpers.szNameW[0] != 0)
  {
    nLen = wcslen(aObjectsList[nIndex]->sCreationHelpers.szNameW);
    nHashValue = fnv_64a_buf(aObjectsList[nIndex]->sCreationHelpers.szNameW, nLen*sizeof(WCHAR),
                             nHashValue);
  }
  //----
  if (aObjectsList[nIndex]->sCreationHelpers.szNamespaceW != NULL &&
      aObjectsList[nIndex]->sCreationHelpers.szNamespaceW[0] != 0)
  {
    nLen = wcslen(aObjectsList[nIndex]->sCreationHelpers.szNamespaceW);
    nHashValue = fnv_64a_buf(aObjectsList[nIndex]->sCreationHelpers.szNamespaceW, nLen*sizeof(WCHAR),
                             nHashValue);
  }
  //----
  nTemp32 = aObjectsList[nIndex]->sCreationHelpers.nSize;
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  nTemp32 = aObjectsList[nIndex]->sCreationHelpers.nAlign;
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  nTemp32 = aObjectsList[nIndex]->sCreationHelpers.nArraySizeHint;
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  nTemp32 = aObjectsList[nIndex]->sCreationHelpers.nStructUnionFunctionFlags;
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  //----
  nChildsCount = aObjectsList[nIndex]->sCreationHelpers.aChildsList.GetCount();
  nTemp32 = (ULONG)nChildsCount;
  nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  for (k=0; k<nChildsCount; k++)
  {
    lpChildItem = aObjectsList[nIndex]->sCreationHelpers.aChildsList.GetBuffer() + k;
    if (lpChildItem->lpObject != NULL)
    {
      BuildHashes_GetObjectHash(lpChildItem->lpObject->sCreationHelpers.nId-1, nDepth+1, nHashValue);
    }
    //----
    if (lpChildItem->szNameW != NULL && lpChildItem->szNameW[0] != 0)
    {
      nLen = wcslen(lpChildItem->szNameW);
      nHashValue = fnv_64a_buf(lpChildItem->szNameW, nLen*sizeof(WCHAR), nHashValue);
    }
    //----
    nTemp32 = lpChildItem->nStructOffset;
    nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
    //----
    nTemp32 = lpChildItem->nStructBits;
    nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
    //----
    nTemp32 = (ULONG)(lpChildItem->nEnumValue);
    nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
    //----
    nTemp32 = lpChildItem->nFlags;
    nHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), nHashValue);
  }
  return;
}


BOOL CDvDbCreator::SaveDatabase(LPCWSTR szDestFileNameW, CSQLiteDbLoader &cLdr)
{
  HANDLE hFile;
  SRC_DBO_MAPPING *lpDboMap;
  HRESULT hRes;
  Fnv64_t nDbHashValue;
  CNktDvDbObject::STORED_DATA sDboData;
  CNktDvDbObject::STORED_CHILD_ITEM sDboChildItem;
  CNktDvDbObject::STORED_MODULE sDboModData;
  CNktDvDbObject::LPDBGEN_CHILD_ITEM lpChildItem;
  SIZE_T i, k, nCount, nChildsCount;
  ULONG nTemp32;
  int nRes;
  struct {
    LPBYTE lpPtr;
    SIZE_T nLen, nSize;
  } sDest;
  LPBYTE lpCompressedDest;

  nktMemSet(&sDest, 0, sizeof(sDest));
  lpCompressedDest = NULL;
  //create database
  PrintAndLog(L"Creating database... ");
  hFile = ::CreateFileW(szDestFileNameW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                        NULL);
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    hFile = NULL;
    if (hRes == E_OUTOFMEMORY)
    {
sd_err_nomem:
      if (sDest.lpPtr != NULL)
        nktMemFree(sDest.lpPtr);
      if (lpCompressedDest != NULL)
        nktMemFree(lpCompressedDest);
      if (hFile != NULL)
        ::CloseHandle(hFile);
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    PrintAndLog(L"\nError: Cannot create database (%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  PrintAndLog(L"OK\n");
  //fill with data
  PrintAndLog(L"Storing data... ");
  hRes = WriteDbData(hFile, "Deviare Database\x1A", 17);
  if (FAILED(hRes))
  {
sd_err_cantwritedb:
    if (hRes == E_OUTOFMEMORY)
      goto sd_err_nomem;
    if (sDest.lpPtr != NULL)
      nktMemFree(sDest.lpPtr);
    if (lpCompressedDest != NULL)
      nktMemFree(lpCompressedDest);
    ::CloseHandle(hFile);
    PrintAndLog(L"\nError: Cannot write database (%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  //write Super String size
  if (AddDbData(&nSuperStringSize, sizeof(nSuperStringSize), &(sDest.lpPtr), &(sDest.nLen),
                &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  if (AddDbData(cSuperStringW.Get(), nSuperStringSize, &(sDest.lpPtr), &(sDest.nLen),
                &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  //write DbObjects count
  nCount = aObjectsList.GetCount();
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //write DbObject
    nktMemSet(&sDboData, 0, sizeof(sDboData));
    sDboData.nId = aObjectsList[i]->sCreationHelpers.nId;
    sDboData.nClass = (ULONG)(aObjectsList[i]->sCreationHelpers.nClass);
    sDboData.nFundamentalType = (ULONG)(aObjectsList[i]->sCreationHelpers.nFundamentalType);
    //first string is empty string
    sDboData.nNameOffset = (aObjectsList[i]->sCreationHelpers.szNameW == NULL) ? 0 :
                                 (ULONG)(aObjectsList[i]->sCreationHelpers.szNameW - cSuperStringW.Get());
    sDboData.nNamespaceOffset = (aObjectsList[i]->sCreationHelpers.szNamespaceW == NULL) ? 0 :
                            (ULONG)(aObjectsList[i]->sCreationHelpers.szNamespaceW - cSuperStringW.Get());
    sDboData.nSize = aObjectsList[i]->sCreationHelpers.nSize;
    sDboData.nAlign = aObjectsList[i]->sCreationHelpers.nAlign;
    sDboData.nArraySizeHint = aObjectsList[i]->sCreationHelpers.nArraySizeHint;
    sDboData.nStructUnionFunctionFlags = aObjectsList[i]->sCreationHelpers.nStructUnionFunctionFlags;
    sDboData.nHashValue = aObjectsList[i]->sCreationHelpers.nHashValue;
    //store data
    if (AddDbData(&sDboData, sizeof(sDboData), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    //write DbObject childs count
    nChildsCount = aObjectsList[i]->sCreationHelpers.aChildsList.GetCount();
    nTemp32 = (ULONG)nChildsCount;
    if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    for (k=0; k<nChildsCount; k++)
    {
      //write DbObject child
      lpChildItem = aObjectsList[i]->sCreationHelpers.aChildsList.GetBuffer() + k;
      nktMemSet(&sDboChildItem, 0, sizeof(sDboChildItem));
      sDboChildItem.nDbObjectId = (lpChildItem->lpObject == NULL) ? 0 :
                                                   (lpChildItem->lpObject->sCreationHelpers.nId);
      sDboChildItem.nNameOffset = (lpChildItem->szNameW == NULL) ? 0 : //first string is empty string
                                                   (ULONG)(lpChildItem->szNameW - cSuperStringW.Get());
      sDboChildItem.nStructOffset = lpChildItem->nStructOffset;
      sDboChildItem.nStructBits = lpChildItem->nStructBits;
      sDboChildItem.nEnumValue = lpChildItem->nEnumValue;
      sDboChildItem.nFlags = lpChildItem->nFlags;
      if (AddDbData(&sDboChildItem, sizeof(sDboChildItem), &(sDest.lpPtr), &(sDest.nLen),
                    &(sDest.nSize)) == FALSE)
        goto sd_err_nomem;
    }
  }
  //write DB_MODULEs count
  nCount = cLdr.aModuleList.GetCount();
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //if ((i & 31) == 0)
    //  wprintf(L"\rStoring data (%.2lf%%)... ", 70.0f+((float)i / (float)nCount) * 10.0f);
    //write DB_MODULE
    nktMemSet(&sDboModData, 0, sizeof(sDboModData));
    sDboModData.nId = cLdr.aModuleList[i].nId;
    k = cLdr.FindStringInStringList(cLdr.aModuleList[i].szNameW);
    NKT_ASSERT(k != NKT_SIZE_T_MAX);
    sDboModData.nNameOffset = aStringOffsetsList[k];
    if (AddDbData(&sDboModData, sizeof(sDboModData), &(sDest.lpPtr), &(sDest.nLen),
                  &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
  }
  //write DB_MODULE_FUNCTIONs count
  nCount = cLdr.aModuleFunctionList.GetCount();
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //if ((i & 31) == 0)
    //  wprintf(L"\rStoring data (%.2lf%%)... ", 80.0f+((float)i / (float)nCount) * 10.0f);
    //find dbo map
    lpDboMap = DboMap_Find(cLdr.aModuleFunctionList[i].nFuncId, NKT_DBOBJCLASS_Function);
    NKT_ASSERT(lpDboMap != NULL);
    NKT_ASSERT(lpDboMap->lpObj != NULL);
    //write module id
    if (AddDbData(&(cLdr.aModuleFunctionList[i].nModId), sizeof(cLdr.aModuleFunctionList[i].nModId),
                  &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    //write objects id
    if (AddDbData(&(lpDboMap->lpObj->sCreationHelpers.nId), sizeof(lpDboMap->lpObj->sCreationHelpers.nId), &(sDest.lpPtr),
                  &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
  }
  //compress data
  k = sDest.nLen + (sDest.nLen / 250);
  if (k < 8)
    k = 8;
  lpCompressedDest = (LPBYTE)nktMemMalloc(k);
  if (lpCompressedDest == NULL)
    goto sd_err_nomem;
  nRes = LZ4_compress((char*)sDest.lpPtr, (char*)lpCompressedDest, (int)sDest.nLen);
  if (nRes < 0)
    goto sd_err_nomem;
  //write original size
  nTemp32 = (ULONG)sDest.nLen;
  hRes = WriteDbData(hFile, &nTemp32, sizeof(nTemp32));
  if (FAILED(hRes))
    goto sd_err_cantwritedb;
  //calculate and write hash
  nDbHashValue = fnv_64a_buf(&nTemp32, sizeof(nTemp32), FNV1A_64_INIT);
  nDbHashValue = fnv_64a_buf(lpCompressedDest, (SIZE_T)nRes, nDbHashValue);
  hRes = WriteDbData(hFile, &nDbHashValue, sizeof(nDbHashValue));
  if (FAILED(hRes))
    goto sd_err_cantwritedb;
  //write data
  hRes = WriteDbData(hFile, lpCompressedDest, (SIZE_T)nRes);
  if (FAILED(hRes))
    goto sd_err_cantwritedb;
  //done
  PrintAndLog(L"OK\n");
  nktMemFree(sDest.lpPtr);
  nktMemFree(lpCompressedDest);
  ::CloseHandle(hFile);
  return TRUE;
}
/*
BOOL CDvDbCreator::SaveGPerfData(LPCWSTR szDestFileNameW)
{
  HANDLE hFile;
  HRESULT hRes;
  SIZE_T i, nCount;
  char szBufA[512];

  //create database
  wprintf(L"Creating GPerf data... ");
  hFile = ::CreateFileW(szDestFileNameW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                        NULL);
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    hFile = NULL;
    if (hRes == E_OUTOFMEMORY) {
sd_err_nomem:
      if (hFile != NULL)
        ::CloseHandle(hFile);
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    PrintAndLog(L"\nError: Cannot create GPerf input data (%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  wprintf(L"OK\n");
  //fill with data
  wprintf(L"Storing data... ");
  //write DbObjects count
  nCount = aObjectsList.GetCount();
  for (i=0; i<nCount; i++) {
    if ((i & 31) == 0)
      wprintf(L"\rStoring data (%.2lf%%)... ", ((float)i / (float)nCount) * 70.0f);
    if (aObjectsList[i]->sCreationHelpers.szNameW[0] == 0)
      sprintf_s(szBufA, 512, "_unnamed_%lu\r\n", i);
    else
      sprintf_s(szBufA, 512, "%S\r\n", aObjectsList[i]->sCreationHelpers.szNameW);
    hRes = WriteDbData(hFile, szBufA, strlen(szBufA));
    if (FAILED(hRes)) {
      if (hRes == E_OUTOFMEMORY)
        goto sd_err_nomem;
      ::CloseHandle(hFile);
      PrintAndLog(L"\nError: Cannot write GPerf input data (%08X).\n", (ULONG)hRes);
      return FALSE;
    }
  }
  //done
  wprintf(L"\rStoring data... OK                             \n");
  ::CloseHandle(hFile);
  return TRUE;
}
*/

BOOL CDvDbCreator::AddDbData(LPVOID lpData, SIZE_T nDataSize, LPBYTE *lplpDest, PSIZE_T lpnDestLen,
                             PSIZE_T lpnDestSize)
{
  NKT_ASSERT(lpData != NULL);
  NKT_ASSERT(nDataSize > 0);
  if ((*lpnDestLen) + nDataSize > (*lpnDestSize)) {
    LPBYTE lpNewBlock;
    SIZE_T k;

    k = ((*lpnDestLen) + nDataSize + 0x7FFFFF) & (~0x7FFFFF);
    lpNewBlock = (LPBYTE)nktMemMalloc(k);
    if (lpNewBlock == NULL)
      return FALSE;
    if ((*lpnDestLen) > 0)
      nktMemCopy(lpNewBlock, *lplpDest, *lpnDestLen);
    if ((*lplpDest) != NULL)
      nktMemFree(*lplpDest);
    *lplpDest = lpNewBlock;
    *lpnDestSize = k;
  }
  nktMemCopy((*lplpDest)+(*lpnDestLen), lpData, nDataSize);
  *lpnDestLen += nDataSize;
  return TRUE;
}

HRESULT CDvDbCreator::WriteDbData(HANDLE hFile, LPVOID lpData, SIZE_T nDataSize)
{
  DWORD dwWritten;

  NKT_ASSERT(lpData != NULL);
  NKT_ASSERT(nDataSize > 0);
  if (::WriteFile(hFile, lpData, (DWORD)nDataSize, &dwWritten, NULL) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  return ((DWORD)nDataSize == dwWritten) ? S_OK : E_FAIL;
}

CNktDvDbObject* CDvDbCreator::AddDbObject()
{
  CNktDvDbObject *lpDbObj;

  lpDbObj = NKT_MEMMGR_NEW CNktDvDbObject;
  if (lpDbObj == NULL)
    return NULL;
  if (aObjectsList.AddElement(lpDbObj) == FALSE) {
    delete lpDbObj;
    return NULL;
  }
  lpDbObj->sCreationHelpers.nId = aObjectsList.GetCount();
  return lpDbObj;
}

//-----------------------------------------------------------

CDvDbCreator::SRC_DBO_MAPPING* CDvDbCreator::DboMap_Find(ULONG nTypeId, ULONG nTypeClass)
{
  SRC_DBO_MAPPING sKey;

  sKey.nTypeClass = nTypeClass;
  sKey.nTypeId = nTypeId;
  return (SRC_DBO_MAPPING*)bsearch_s(&sKey, aMappingsList.GetBuffer(), aMappingsList.GetCount(),
                                     sizeof(SRC_DBO_MAPPING), DboMap_Compare, NULL);
}

int CDvDbCreator::DboMap_Compare(void *ctx, const void *lp1, const void *lp2)
{
  if (((SRC_DBO_MAPPING*)lp1)->nTypeClass < ((SRC_DBO_MAPPING*)lp2)->nTypeClass)
    return -1;
  if (((SRC_DBO_MAPPING*)lp1)->nTypeClass > ((SRC_DBO_MAPPING*)lp2)->nTypeClass)
    return 1;
  if (((SRC_DBO_MAPPING*)lp1)->nTypeId < ((SRC_DBO_MAPPING*)lp2)->nTypeId)
    return -1;
  if (((SRC_DBO_MAPPING*)lp1)->nTypeId > ((SRC_DBO_MAPPING*)lp2)->nTypeId)
    return 1;
  return 0;
}

BOOL CDvDbCreator::BO_ProcessStruct(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                    ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_STRUCT *lpStruct, *lpStructB;
  CSQLiteDbLoader::DB_STRUCT_BASE *lpStructBase;
  CSQLiteDbLoader::DB_STRUCT_MEMBER *lpStructMember;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;
  ULONG nBaseIdx, nMemberIdx;
  SIZE_T nStrIdx;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsStruct;
  //get struct info
  lpStruct = cLdr.FindStruct(nTypeId);
  if (lpStruct == NULL)
    return TRUE; //abstract struct
  lpDbObj->sCreationHelpers.nAlign = (SIZE_T)(lpStruct->nAlign);
  lpDbObj->sCreationHelpers.nSize = (SIZE_T)(lpStruct->nSize);
  MapStructUnionFlags(lpDbObj, lpStruct->nFlags);
  if (lpDbObj->sCreationHelpers.szNameW == NULL)
  {
    ULONG nStrIdx = cLdr.FindStringInStringList(lpStruct->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
  }
  //process bases first
  for (nBaseIdx=1; ; nBaseIdx++) {
    lpStructBase = cLdr.FindStructBase(nTypeId, nBaseIdx);
    if (lpStructBase == NULL)
      break;
    lpStructB = cLdr.FindStruct(lpStructBase->nBaseId);
    NKT_ASSERT(lpStructB != NULL);
    for (nMemberIdx=1; ; nMemberIdx++) {
      lpStructMember = cLdr.FindStructMember(lpStructB->nId, nMemberIdx);
      if (lpStructMember == NULL)
        break;
      lpDboMap = DboMap_Find(lpStructMember->nTypeId, lpStructMember->nTypeClass & NKT_DBOBJCLASS_MASK);
      NKT_ASSERT(lpDboMap != NULL);
      lpDbChildObj = lpDboMap->lpObj;
      NKT_ASSERT(lpDbChildObj != NULL);
      //----
      nktMemSet(&sChildItem, 0, sizeof(sChildItem));
      sChildItem.lpObject = lpDbChildObj;
      nStrIdx = cLdr.FindStringInStringList(lpStructMember->szNameW);
      NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
      sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
      sChildItem.nStructOffset = lpStructMember->nOffset; //offset includes base structs sizes
      sChildItem.nStructBits = lpStructMember->nBits;
      switch (lpStructMember->nFlags & NKT_DBOBJFLAG_MEMBER_MASK) {
        case NKT_DBOBJFLAG_ProtectedMember:
          sChildItem.nFlags |= CNktDvDbObject::mtProtected;
          break;
        case NKT_DBOBJFLAG_PrivateMember:
          sChildItem.nFlags |= CNktDvDbObject::mtPrivate;
          break;
        default:
          sChildItem.nFlags |= CNktDvDbObject::mtPublic;
          break;
      }
      if ((lpStructMember->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
        if ((cLdr.aBasicTypesList[lpStructMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant)
            != 0)
          sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
        if ((cLdr.aBasicTypesList[lpStructMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile)
            != 0)
          sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
      }
      if ((lpStructMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
      if ((lpStructMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
      if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
bops_err_nomem:
        PrintAndLogNoMemoryError();
        return FALSE;
      }
    }
  }
  //process struct
  for (nMemberIdx=1; ; nMemberIdx++) {
    lpStructMember = cLdr.FindStructMember(lpStruct->nId, nMemberIdx);
    if (lpStructMember == NULL)
      break;
    lpDboMap = DboMap_Find(lpStructMember->nTypeId, lpStructMember->nTypeClass & NKT_DBOBJCLASS_MASK);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbChildObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbChildObj != NULL);
    //----
    nktMemSet(&sChildItem, 0, sizeof(sChildItem));
    sChildItem.lpObject = lpDbChildObj;
    nStrIdx = cLdr.FindStringInStringList(lpStructMember->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
    sChildItem.nStructOffset = lpStructMember->nOffset; //offset includes base structs sizes
    sChildItem.nStructBits = lpStructMember->nBits;
    switch (lpStructMember->nFlags & NKT_DBOBJFLAG_MEMBER_MASK) {
      case NKT_DBOBJFLAG_ProtectedMember:
        sChildItem.nFlags |= CNktDvDbObject::mtProtected;
        break;
      case NKT_DBOBJFLAG_PrivateMember:
        sChildItem.nFlags |= CNktDvDbObject::mtPrivate;
        break;
      default:
        sChildItem.nFlags |= CNktDvDbObject::mtPublic;
        break;
    }
    if ((lpStructMember->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
      if ((cLdr.aBasicTypesList[lpStructMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant)
          != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
      if ((cLdr.aBasicTypesList[lpStructMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile)
          != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    }
    if ((lpStructMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((lpStructMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE)
      goto bops_err_nomem;
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessUnion(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                   ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_UNION *lpUnion, *lpUnionB;
  CSQLiteDbLoader::DB_UNION_BASE *lpUnionBase;
  CSQLiteDbLoader::DB_UNION_MEMBER *lpUnionMember;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;
  ULONG nBaseIdx, nMemberIdx;
  SIZE_T nStrIdx;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsUnion;
  //get union info
  lpUnion = cLdr.FindUnion(nTypeId);
  if (lpUnion == NULL)
    return TRUE; //abstract union
  lpDbObj->sCreationHelpers.nAlign = (SIZE_T)(lpUnion->nAlign);
  lpDbObj->sCreationHelpers.nSize = (SIZE_T)(lpUnion->nSize);
  MapStructUnionFlags(lpDbObj, lpUnion->nFlags);
  if (lpDbObj->sCreationHelpers.szNameW == NULL)
  {
    ULONG nStrIdx = cLdr.FindStringInStringList(lpUnion->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
  }
  //process bases first
  for (nBaseIdx=1; ; nBaseIdx++) {
    lpUnionBase = cLdr.FindUnionBase(nTypeId, nBaseIdx);
    if (lpUnionBase == NULL)
      break;
    lpUnionB = cLdr.FindUnion(lpUnionBase->nBaseId);
    NKT_ASSERT(lpUnionB != NULL);
    for (nMemberIdx=1; ; nMemberIdx++) {
      lpUnionMember = cLdr.FindUnionMember(lpUnionB->nId, nMemberIdx);
      if (lpUnionMember == NULL)
        break;
      lpDboMap = DboMap_Find(lpUnionMember->nTypeId, lpUnionMember->nTypeClass & NKT_DBOBJCLASS_MASK);
      NKT_ASSERT(lpDboMap != NULL);
      lpDbChildObj = lpDboMap->lpObj;
      NKT_ASSERT(lpDbChildObj != NULL);
      //----
      nktMemSet(&sChildItem, 0, sizeof(sChildItem));
      sChildItem.lpObject = lpDbChildObj;
      nStrIdx = cLdr.FindStringInStringList(lpUnionMember->szNameW);
      NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
      sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
      sChildItem.nStructOffset = lpUnionMember->nOffset; //offset includes base unions sizes
      sChildItem.nStructBits = lpUnionMember->nBits;
      switch (lpUnionMember->nFlags & NKT_DBOBJFLAG_MEMBER_MASK) {
        case NKT_DBOBJFLAG_ProtectedMember:
          sChildItem.nFlags |= CNktDvDbObject::mtProtected;
          break;
        case NKT_DBOBJFLAG_PrivateMember:
          sChildItem.nFlags |= CNktDvDbObject::mtPrivate;
          break;
        default:
          sChildItem.nFlags |= CNktDvDbObject::mtPublic;
          break;
      }
      if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
        if ((cLdr.aBasicTypesList[lpUnionMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant)
            != 0)
          sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
        if ((cLdr.aBasicTypesList[lpUnionMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile)
            != 0)
          sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
      }
      if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
      if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
      if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
bopu_err_nomem:
        PrintAndLogNoMemoryError();
        return FALSE;
      }
    }
  }
  //process union
  for (nMemberIdx=1; ; nMemberIdx++) {
    lpUnionMember = cLdr.FindUnionMember(lpUnion->nId, nMemberIdx);
    if (lpUnionMember == NULL)
      break;
    lpDboMap = DboMap_Find(lpUnionMember->nTypeId, lpUnionMember->nTypeClass & NKT_DBOBJCLASS_MASK);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbChildObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbChildObj != NULL);
    //----
    nktMemSet(&sChildItem, 0, sizeof(sChildItem));
    sChildItem.lpObject = lpDbChildObj;
    nStrIdx = cLdr.FindStringInStringList(lpUnionMember->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
    sChildItem.nStructOffset = lpUnionMember->nOffset; //offset includes base unions sizes
    sChildItem.nStructBits = lpUnionMember->nBits;
    switch (lpUnionMember->nFlags & NKT_DBOBJFLAG_MEMBER_MASK) {
      case NKT_DBOBJFLAG_ProtectedMember:
        sChildItem.nFlags |= CNktDvDbObject::mtProtected;
        break;
      case NKT_DBOBJFLAG_PrivateMember:
        sChildItem.nFlags |= CNktDvDbObject::mtPrivate;
        break;
      default:
        sChildItem.nFlags |= CNktDvDbObject::mtPublic;
        break;
    }
    if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
      if ((cLdr.aBasicTypesList[lpUnionMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant)
          != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
      if ((cLdr.aBasicTypesList[lpUnionMember->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile)
          != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    }
    if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((lpUnionMember->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE)
      goto bopu_err_nomem;
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessArray(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                   ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_ARRAY *lpArray;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsArray;
  //get array info
  lpArray = cLdr.FindArray(nTypeId);
  NKT_ASSERT(lpArray != NULL);
  lpDbObj->sCreationHelpers.nSize = lpArray->nSize;
  lpDbObj->sCreationHelpers.nAlign = lpArray->nAlign;
  lpDbObj->sCreationHelpers.nArraySizeHint = lpArray->nMax;
  //process array
  lpDboMap = DboMap_Find(lpArray->nTypeId, lpArray->nTypeClass & NKT_DBOBJCLASS_MASK);
  NKT_ASSERT(lpDboMap != NULL);
  lpDbChildObj = lpDboMap->lpObj;
  NKT_ASSERT(lpDbChildObj != NULL);
  //----
  nktMemSet(&sChildItem, 0, sizeof(sChildItem));
  sChildItem.lpObject = lpDbChildObj;
  if ((lpArray->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
    if ((cLdr.aBasicTypesList[lpArray->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((cLdr.aBasicTypesList[lpArray->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  }
  if ((lpArray->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
  if ((lpArray->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessPointer(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                     ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_POINTER *lpPointer;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsPointer;
  //get pointer info
  lpPointer = cLdr.FindPointer(nTypeId);
  NKT_ASSERT(lpPointer != NULL);
  lpDbObj->sCreationHelpers.nSize = (SIZE_T)(lpPointer->nSize);
  lpDbObj->sCreationHelpers.nAlign = (SIZE_T)(lpPointer->nAlign);
  //process pointer
  lpDboMap = DboMap_Find(lpPointer->nTypeId, lpPointer->nTypeClass & NKT_DBOBJCLASS_MASK);
  if (lpDboMap == NULL) {
    //may point to an opaque structure
    switch (lpPointer->nTypeClass & NKT_DBOBJCLASS_MASK) {
      case NKT_DBOBJCLASS_Struct:
      case NKT_DBOBJCLASS_Union:
        return TRUE;
    }
  }
  lpDbChildObj = lpDboMap->lpObj;
  NKT_ASSERT(lpDbChildObj != NULL);
  //----
  nktMemSet(&sChildItem, 0, sizeof(sChildItem));
  sChildItem.lpObject = lpDbChildObj;
  if ((lpPointer->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
    if ((cLdr.aBasicTypesList[lpPointer->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((cLdr.aBasicTypesList[lpPointer->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  }
  if ((lpPointer->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
  if ((lpPointer->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessReference(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                       ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_REFERENCE *lpReference;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsReference;
  //get reference info
  lpReference = cLdr.FindReference(nTypeId);
  NKT_ASSERT(lpReference != NULL);
  lpDbObj->sCreationHelpers.nSize = (SIZE_T)(lpReference->nSize);
  lpDbObj->sCreationHelpers.nAlign = (SIZE_T)(lpReference->nAlign);
  //process reference
  lpDboMap = DboMap_Find(lpReference->nTypeId, lpReference->nTypeClass & NKT_DBOBJCLASS_MASK);
  if (lpDboMap == NULL) {
    //may point to an opaque structure
    switch (lpReference->nTypeClass & NKT_DBOBJCLASS_MASK) {
      case NKT_DBOBJCLASS_Struct:
      case NKT_DBOBJCLASS_Union:
        return TRUE;
    }
  }
  lpDbChildObj = lpDboMap->lpObj;
  NKT_ASSERT(lpDbChildObj != NULL);
  //----
  nktMemSet(&sChildItem, 0, sizeof(sChildItem));
  sChildItem.lpObject = lpDbChildObj;
  if ((lpReference->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
    if ((cLdr.aBasicTypesList[lpReference->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((cLdr.aBasicTypesList[lpReference->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  }
  if ((lpReference->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
  if ((lpReference->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessEnumeration(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                         ULONG nTypeClass)
{
  CSQLiteDbLoader::DB_ENUM *lpEnum;
  CSQLiteDbLoader::DB_ENUM_VALUE *lpEnumValue;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;
  ULONG nValueIdx;
  SIZE_T nStrIdx;

  lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsEnumeration;
  //get enum info
  lpEnum = cLdr.FindEnum(nTypeId);
  NKT_ASSERT(lpEnum != NULL);
  lpDbObj->sCreationHelpers.nAlign = (SIZE_T)(lpEnum->nAlign);
  lpDbObj->sCreationHelpers.nSize = (SIZE_T)(lpEnum->nSize);
  if (lpEnum->nSize <= 8)
    lpDbObj->sCreationHelpers.nFundamentalType = CNktDvDbObject::ftUnsignedByte;
  else if (lpEnum->nSize <= 16)
    lpDbObj->sCreationHelpers.nFundamentalType = CNktDvDbObject::ftUnsignedWord;
  else if (lpEnum->nSize <= 32)
    lpDbObj->sCreationHelpers.nFundamentalType = CNktDvDbObject::ftUnsignedDoubleWord;
  else if (lpEnum->nSize <= 64)
    lpDbObj->sCreationHelpers.nFundamentalType = CNktDvDbObject::ftUnsignedQuadWord;
  if (lpDbObj->sCreationHelpers.szNameW == NULL)
  {
    ULONG nStrIdx = cLdr.FindStringInStringList(lpEnum->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
  }
  //process enum
  for (nValueIdx=1; ; nValueIdx++) {
    lpEnumValue = cLdr.FindEnumValue(lpEnum->nId, nValueIdx);
    if (lpEnumValue == NULL)
      break;
    //----
    nktMemSet(&sChildItem, 0, sizeof(sChildItem));
    nStrIdx = cLdr.FindStringInStringList(lpEnumValue->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
    sChildItem.nEnumValue = lpEnumValue->nValue;
    if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
      PrintAndLogNoMemoryError();
      return FALSE;
    }
  }
  return TRUE;
}

BOOL CDvDbCreator::BO_ProcessFunction(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                                      ULONG nTypeClass)
{
  SRC_DBO_MAPPING *lpDboMap;
  CNktDvDbObject *lpDbChildObj;
  CSQLiteDbLoader::DB_FUNCTION *lpFunc;
  CSQLiteDbLoader::DB_FUNCTION_ARG *lpFuncArg;
  CNktDvDbObject::DBGEN_CHILD_ITEM sChildItem;
  ULONG nArgIdx;
  SIZE_T nStrIdx;

  switch (nTypeClass)
  {
    case NKT_DBOBJCLASS_Function:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFunction;
      break;
    case NKT_DBOBJCLASS_FunctionType:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsFunctionType;
      break;
    case NKT_DBOBJCLASS_ClassConstructor:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassConstructor;
      break;
    case NKT_DBOBJCLASS_ClassDestructor:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassDestructor;
      break;
    case NKT_DBOBJCLASS_ClassOperatorMethod:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassOperatorMethod;
      break;
    case NKT_DBOBJCLASS_ClassMethod:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassMethod;
      break;
    case NKT_DBOBJCLASS_ClassConverter:
      lpDbObj->sCreationHelpers.nClass = CNktDvDbObjectNoRef::clsClassConverter;
      break;
    default:
      PrintAndLog(L"\nError: Invalid function class [%lu].\n", nTypeClass);
      return FALSE;
  }
  //get function type info
  lpFunc = cLdr.FindFunction(nTypeId);
  NKT_ASSERT(lpFunc != NULL);
  MapFunctionFlags(lpDbObj, lpFunc->nFlags);
  //get function info
  if (lpDbObj->sCreationHelpers.szNameW == NULL)
  {
    ULONG nStrIdx = cLdr.FindStringInStringList(lpFunc->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    lpDbObj->sCreationHelpers.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
  }
  //process function
  for (nArgIdx=1; ; nArgIdx++) {
    lpFuncArg = cLdr.FindFunctionArg(lpFunc->nId, nArgIdx);
    if (lpFuncArg == NULL)
      break;
    lpDboMap = DboMap_Find(lpFuncArg->nTypeId, lpFuncArg->nTypeClass & NKT_DBOBJCLASS_MASK);
    NKT_ASSERT(lpDboMap != NULL);
    lpDbChildObj = lpDboMap->lpObj;
    NKT_ASSERT(lpDbChildObj != NULL);
    //----
    nktMemSet(&sChildItem, 0, sizeof(sChildItem));
    sChildItem.lpObject = lpDbChildObj;
    nStrIdx = cLdr.FindStringInStringList(lpFuncArg->szNameW);
    NKT_ASSERT(nStrIdx != NKT_SIZE_T_MAX);
    sChildItem.szNameW = cSuperStringW.Get() + aStringOffsetsList[nStrIdx];
    if ((lpFuncArg->nTypeClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
      if ((cLdr.aBasicTypesList[lpFuncArg->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
      if ((cLdr.aBasicTypesList[lpFuncArg->nTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
        sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    }
    if ((lpFuncArg->nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((lpFuncArg->nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
    if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE) {
bopf_err_nomem:
      PrintAndLogNoMemoryError();
      return FALSE;
    }
  }
  //process function return
  lpDboMap = DboMap_Find(lpFunc->nReturnTypeId, lpFunc->nReturnClass & NKT_DBOBJCLASS_MASK);
  NKT_ASSERT(lpDboMap != NULL);
  lpDbChildObj = lpDboMap->lpObj;
  NKT_ASSERT(lpDbChildObj != NULL);
  //----
  nktMemSet(&sChildItem, 0, sizeof(sChildItem));
  sChildItem.lpObject = lpDbChildObj;
  if ((lpFunc->nReturnClass & NKT_DBOBJCLASS_MASK) == NKT_DBOBJCLASS_Fundamental) {
    if ((cLdr.aBasicTypesList[lpFunc->nReturnTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
    if ((cLdr.aBasicTypesList[lpFunc->nReturnTypeId-1].nTypeClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
      sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  }
  if ((lpFunc->nReturnClass & NKT_DBOBJCLASSFLAG_IsConstant) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsConstant;
  if ((lpFunc->nReturnClass & NKT_DBOBJCLASSFLAG_IsVolatile) != 0)
    sChildItem.nFlags |= CNktDvDbObject::iflgIsVolatile;
  if (lpDbObj->sCreationHelpers.aChildsList.AddElement(&sChildItem) == FALSE)
    goto bopf_err_nomem;
  return TRUE;
}

VOID CDvDbCreator::MapStructUnionFlags(CNktDvDbObject *lpDbObj, ULONG nFlags)
{
  if ((nFlags & NKT_DBOBJFLAG_HasConstructor) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::suflgHasConstructor;
  if ((nFlags & NKT_DBOBJFLAG_HasDestructor) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::suflgHasDestructor;
  if ((nFlags & NKT_DBOBJFLAG_HasVirtual) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::suflgHasVirtual;
  if ((nFlags & NKT_DBOBJFLAG_HasInheritance) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::suflgHasInheritance;
  return;
}

VOID CDvDbCreator::MapFunctionFlags(CNktDvDbObject *lpDbObj, ULONG nFlags)
{
  switch (nFlags & NKT_DBOBJFLAG_CALLINGTYPE_MASK) {
    case NKT_DBOBJFLAG_CDecl:
      lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObjectNoRef::ccCDecl;
      break;
    case NKT_DBOBJFLAG_FastCall:
      lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObjectNoRef::ccFastCall;
      break;
    case NKT_DBOBJFLAG_ThisCall:
      lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObjectNoRef::ccThisCall;
      break;
    default:
      lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObjectNoRef::ccStdCall;
      break;
  }
  if ((nFlags & NKT_DBOBJFLAG_IsExternal) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsExternal;
  if ((nFlags & NKT_DBOBJFLAG_IsDllImport) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsDllImport;
  if ((nFlags & NKT_DBOBJFLAG_IsPure) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsPure;
  if ((nFlags & NKT_DBOBJFLAG_Throw) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsThrow;
  if ((nFlags & NKT_DBOBJFLAG_NoThrow) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsNoThrow;
  if ((nFlags & NKT_DBOBJFLAG_NoReturn) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsNoReturn;
  if ((nFlags & NKT_DBOBJFLAG_IsConst) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsConst;
  if ((nFlags & NKT_DBOBJFLAG_Deprecated) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsDeprecated;
  if ((nFlags & NKT_DBOBJFLAG_NonNull) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsNonNull;
  if ((nFlags & NKT_DBOBJFLAG_Malloc) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsMalloc;
  if ((nFlags & NKT_DBOBJFLAG_IsDllExport) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsMalloc;
  if ((nFlags & NKT_DBOBJFLAG_Format) != 0)
    lpDbObj->sCreationHelpers.nStructUnionFunctionFlags |= CNktDvDbObject::fflgIsFormat;
  return;
}
