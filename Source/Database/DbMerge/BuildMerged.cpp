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

#include "DbMerge.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

typedef struct {
  SIZE_T nDbIndex;
  SIZE_T nStoredDataIndex;
  ULONGLONG nHashValue;
} NEW_STORED_DATA;

typedef struct {
  SIZE_T nDbIndex;
  SIZE_T nStoredModuleIndex;
} NEW_STORED_MODULE;

typedef struct {
  ULONG nModId;
  ULONG nFuncId;
} NEW_STORED_MODULEFUNCTION;

//-----------------------------------------------------------

BOOL CNktDvDbMerge::BuldMerged()
{
  TNktArrayList4Structs<NEW_STORED_DATA,1024> aNewStoreDataList;
  TNktArrayList4Structs<NEW_STORED_MODULE,1024> aNewStoreModulesList;
  TNktArrayList4Structs<NEW_STORED_MODULEFUNCTION,1024> aNewStoreModulesFunctionsList;
  TNktArrayList4Structs<SIZE_T> aNewStoreData_HashSort;
  TNktArrayList4Structs<SIZE_T> aNewStoreModules_HashSort;
  WSTRING_LIST aStringsList;
  TNktArrayList<ULONG> aStringOffsetsList;
  union {
    NEW_STORED_DATA sData;
    NEW_STORED_MODULE sMod;
    NEW_STORED_MODULEFUNCTION sModFunc;
  } uNew;
  CDbMergeDvDatabase *lpDb, *lpDb2;
  CNktDvDbObjectNoRef::LPSTORED_DATA lpSrcObj, lpDestObj;
  CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM lpSrcChildObj, lpDestChildObj;
  CNktDvDbObjectNoRef::LPSTORED_MODULE lpDestMod;
  SIZE_T i, k, nIndex, nCount, nCounter, nTotal, nMin, nMax, nChildCounter, nChildsCount;
  LPWSTR sW[2];
  int res;

  PrintAndLog(L"Merging objects... ");
  for (k=nTotal=0; k<aInputDbs.GetCount(); k++)
    nTotal += aInputDbs[k]->nDbObjectsListCount;
  for (k=nCounter=0; k<aInputDbs.GetCount(); k++)
  {
    lpDb = aInputDbs[k];
    lpDb->aStoredDbObjects_RemappedIndex.Attach((SIZE_T*)nktMemMalloc(lpDb->nDbObjectsListCount *
                                                                      sizeof(SIZE_T)));
    if (lpDb->aStoredDbObjects_RemappedIndex == NULL)
    {
err_nomem:
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    for (i=0; i<lpDb->nDbObjectsListCount; i++)
    {
      nCounter++;
      if ((nCounter & 15) == 15 || nCounter == nTotal)
      {
        wprintf_s(L"\rMerging objects (%.2lf)... ", 100.0 * ((double)nCounter/(double)nTotal));
        if (IsAbortKeyPressed() != FALSE) //check for abort
          return FALSE;
      }
      //----
      uNew.sData.nDbIndex = k;
      uNew.sData.nStoredDataIndex = i;
      uNew.sData.nHashValue = lpDb->aStoredDbObjects[i]->nHashValue;
      nMin = 1;
      nMax = nCount = aNewStoreData_HashSort.GetCount();
      res = -1;
      while (nMin <= nMax)
      {
        nIndex = nMin + (nMax-nMin) / 2;
        res = NKT_DV_CMP<ULONGLONG>(uNew.sData.nHashValue,
                                    aNewStoreDataList[aNewStoreData_HashSort[nIndex-1]].nHashValue);
        if (res == 0)
          break;
        if (res < 0)
          nMax = nIndex - 1;
        else
          nMin = nIndex + 1;
      }
      if (res == 0)
      {
        lpDb->aStoredDbObjects_RemappedIndex[i] = aNewStoreData_HashSort[nIndex-1];
      }
      else
      {
        lpDb->aStoredDbObjects_RemappedIndex[i] = aNewStoreDataList.GetCount();
        if (aNewStoreData_HashSort.InsertElementAt(&nCount, nMin-1) == FALSE ||
            aNewStoreDataList.AddElement(&(uNew.sData)) == FALSE)
          goto err_nomem;
      }
    }
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Merging modules... ");
  for (k=nTotal=0; k<aInputDbs.GetCount(); k++)
    nTotal += aInputDbs[k]->nDbModulesListCount;
  for (k=nCounter=0; k<aInputDbs.GetCount(); k++)
  {
    lpDb = aInputDbs[k];
    lpDb->aStoredDbModules_RemappedIndex.Attach((SIZE_T*)nktMemMalloc(lpDb->nDbModulesListCount *
                                                                      sizeof(SIZE_T)));
    if (lpDb->aStoredDbModules_RemappedIndex == NULL)
       goto err_nomem;
    for (i=0; i<lpDb->nDbModulesListCount; i++)
    {
      nCounter++;
      if ((nCounter & 15) == 15 || nCounter == nTotal)
      {
        wprintf_s(L"\rMerging modules (%.2lf)... ", 100.0 * ((double)nCounter/(double)nTotal));
        if (IsAbortKeyPressed() != FALSE) //check for abort
          return FALSE;
      }
      //----
      uNew.sMod.nDbIndex = k;
      uNew.sMod.nStoredModuleIndex = i;
      sW[0] = lpDb->szSuperStringW + lpDb->aStoredDbModules[i]->nNameOffset;
      nMin = 1;
      nMax = nCount = aNewStoreModules_HashSort.GetCount();
      res = -1;
      while (nMin <= nMax)
      {
        nIndex = nMin + (nMax-nMin) / 2;
        lpDb2 = aInputDbs[aNewStoreModulesList[aNewStoreModules_HashSort[nIndex-1]].nDbIndex];
        sW[1] = lpDb2->szSuperStringW +
                lpDb2->aStoredDbModules[aNewStoreModulesList[aNewStoreModules_HashSort[nIndex-1]
                                                            ].nStoredModuleIndex
                                       ]->nNameOffset;
        res = wcscmp(sW[0], sW[1]);
        if (res == 0)
          break;
        if (res < 0)
          nMax = nIndex - 1;
        else
          nMin = nIndex + 1;
      }
      if (res == 0)
      {
        lpDb->aStoredDbModules_RemappedIndex[i] = aNewStoreModules_HashSort[nIndex-1];
      }
      else
      {
        lpDb->aStoredDbModules_RemappedIndex[i] = aNewStoreDataList.GetCount();
        if (aNewStoreModules_HashSort.InsertElementAt(&nCount, nMin-1) == FALSE ||
            aNewStoreModulesList.AddElement(&(uNew.sMod)) == FALSE)
          goto err_nomem;
      }
    }
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Merging module's functions... ");
  for (k=nTotal=0; k<aInputDbs.GetCount(); k++)
    nTotal += aInputDbs[k]->nDbModuleFunctionsCount;
  for (k=nCounter=0; k<aInputDbs.GetCount(); k++)
  {
    lpDb = aInputDbs[k];
    for (i=0; i<lpDb->nDbModuleFunctionsCount; i++)
    {
      nCounter++;
      if ((nCounter & 15) == 15 || nCounter == nTotal)
      {
        wprintf_s(L"\rMerging module's functions (%.2lf)... ", 100.0 * ((double)nCounter/(double)nTotal));
        if (IsAbortKeyPressed() != FALSE) //check for abort
          return FALSE;
      }
      //check where this module and function were remmaped to...
      uNew.sModFunc.nModId = lpDb->aStoredDbModuleFunctions[(i<<1)  ];
      uNew.sModFunc.nFuncId = lpDb->aStoredDbModuleFunctions[(i<<1)+1];
      uNew.sModFunc.nModId = lpDb->aStoredDbModules_RemappedIndex[uNew.sModFunc.nModId-1] + 1;
      uNew.sModFunc.nFuncId = lpDb->aStoredDbObjects_RemappedIndex[uNew.sModFunc.nFuncId-1] + 1;
      nMin = 1;
      nMax = nCount = aNewStoreModulesFunctionsList.GetCount();
      res = -1;
      while (nMin <= nMax)
      {
        nIndex = nMin + (nMax-nMin) / 2;

        res = NKT_DV_CMP<ULONG>(uNew.sModFunc.nModId, aNewStoreModulesFunctionsList[nIndex-1].nModId);
        if (res == 0)
          res = NKT_DV_CMP<ULONG>(uNew.sModFunc.nFuncId, aNewStoreModulesFunctionsList[nIndex-1].nFuncId);
        if (res == 0)
          break;
        if (res < 0)
          nMax = nIndex - 1;
        else
          nMin = nIndex + 1;
      }
      if (res != 0)
      {
        if (aNewStoreModulesFunctionsList.InsertElementAt(&(uNew.sModFunc), nMin-1) == FALSE)
          goto err_nomem;
      }
    }
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Rebuilding objects... ");
  //first add namespaces to string list
  if (AddStringToStringList(aStringsList, L"") == NULL)
    goto err_nomem;
  for (k=0; k<aInputNamespaces.GetCount(); k++)
  {
    if (AddStringToStringList(aStringsList, aInputNamespaces[k]) == NULL)
      goto err_nomem;
  }
  //build new stored DbObjects
  cMergedDb.nDbObjectsListCount = nCount = aNewStoreDataList.GetCount();
  cMergedDb.aOutputDbObjects.Attach((CNktDvDbObjectNoRef::LPSTORED_DATA)nktMemMalloc(
                                            nCount*sizeof(CNktDvDbObjectNoRef::STORED_DATA)));
  cMergedDb.aStoredDbObjects_ChildsCount.Attach((SIZE_T*)nktMemMalloc(nCount * sizeof(SIZE_T)));
  cMergedDb.aStoredDbObjects_FirstChild.Attach((CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM*)nktMemMalloc(
                                            nCount * sizeof(CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM)));
  if (cMergedDb.aOutputDbObjects == NULL ||
      cMergedDb.aStoredDbObjects_ChildsCount == NULL ||
      cMergedDb.aStoredDbObjects_FirstChild == NULL)
    goto err_nomem;
  for (k=nTotal=0; k<nCount; k++)
  {
    lpDb = aInputDbs[aNewStoreDataList[k].nDbIndex];
    nTotal += lpDb->aStoredDbObjects_ChildsCount[aNewStoreDataList[k].nStoredDataIndex];
  }
  cMergedDb.aOutputDbObjectChilds.Attach((CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM)nktMemMalloc(
                                             nTotal * sizeof(CNktDvDbObjectNoRef::STORED_CHILD_ITEM)));
  if (cMergedDb.aOutputDbObjectChilds == NULL)
    goto err_nomem;
  lpDestChildObj = cMergedDb.aOutputDbObjectChilds.Get();
  for (k=nChildCounter=0; k<nCount; k++)
  {
    if ((k & 15) == 15 || k == nCount-1)
    {
      wprintf_s(L"\rRebuilding objects (%.2lf)... ", 100.0 * ((double)(k+1)/(double)nCount));
      if (IsAbortKeyPressed() != FALSE) //check for abort
        return FALSE;
    }
    //----
    lpDb = aInputDbs[aNewStoreDataList[k].nDbIndex];
    lpSrcObj = lpDb->aStoredDbObjects[aNewStoreDataList[k].nStoredDataIndex];
    lpDestObj = cMergedDb.aOutputDbObjects.Get() + k;
    //copy from original
    nktMemCopy(lpDestObj, lpSrcObj, sizeof(CNktDvDbObjectNoRef::STORED_DATA));
    //reassign id
    lpDestObj->nId = (ULONG)k+1;
    //find/add the "name" string
    sW[0] = lpDb->szSuperStringW + lpSrcObj->nNameOffset;
    sW[0] = AddStringToStringList(aStringsList, sW[0]);
    if (sW[0] == NULL)
      goto err_nomem;
    lpDestObj->nNameOffset = lpDestObj->nNamespaceOffset = 0; //will be assigned below
    //process the childs of this object
    nChildsCount = lpDb->aStoredDbObjects_ChildsCount[aNewStoreDataList[k].nStoredDataIndex];
    lpSrcChildObj = lpDb->aStoredDbObjects_FirstChild[aNewStoreDataList[k].nStoredDataIndex];
    cMergedDb.aStoredDbObjects_ChildsCount[k] = nChildsCount;
    cMergedDb.aStoredDbObjects_FirstChild[k] = lpDestChildObj+nChildCounter;
    for (i=0; i<nChildsCount; i++,nChildCounter++)
    {
      //copy from original
      nktMemCopy(lpDestChildObj+nChildCounter, lpSrcChildObj+i,
                 sizeof(CNktDvDbObjectNoRef::STORED_CHILD_ITEM));
      //reassign id
      if (lpDestChildObj[nChildCounter].nDbObjectId != 0)
      {
        lpDestChildObj[nChildCounter].nDbObjectId = 1 + (ULONG)
                     (lpDb->aStoredDbObjects_RemappedIndex[lpDestChildObj[nChildCounter].nDbObjectId-1]);
      }
      //find/add the "name" string
      sW[0] = lpDb->szSuperStringW + lpSrcChildObj[i].nNameOffset;
      sW[0] = AddStringToStringList(aStringsList, sW[0]);
      if (sW[0] == NULL)
        goto err_nomem;
      lpDestChildObj[nChildCounter].nNameOffset = 0; //will be assigned below
    }
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Rebuilding modules... ");
  //build new stored DbModules
  cMergedDb.nDbModulesListCount = nCount = aNewStoreModulesList.GetCount();
  cMergedDb.aOutputDbModules.Attach((CNktDvDbObjectNoRef::LPSTORED_MODULE)nktMemMalloc(
                                           nCount * sizeof(CNktDvDbObjectNoRef::STORED_MODULE)));
  if (cMergedDb.aOutputDbModules == NULL)
    goto err_nomem;
  for (k=0; k<nCount; k++)
  {
    if ((k & 15) == 15 || k == nCount-1)
    {
      wprintf_s(L"\rRebuilding modules (%.2lf)... ", 100.0 * ((double)(k+1)/(double)nCount));
      if (IsAbortKeyPressed() != FALSE) //check for abort
        return FALSE;
    }
    //----
    lpDb = aInputDbs[aNewStoreModulesList[k].nDbIndex];
    lpDestMod = cMergedDb.aOutputDbModules.Get() + k;
    //reassign id
    lpDestMod->nId = (ULONG)k+1;
    //find/add the "name" string
    sW[0] = lpDb->szSuperStringW + lpDb->aStoredDbModules[aNewStoreModulesList[k].nStoredModuleIndex
                                                         ]->nNameOffset;
    sW[0] = AddStringToStringList(aStringsList, sW[0]);
    if (sW[0] == NULL)
      goto err_nomem;
    lpDestMod->nNameOffset = 0; //will be assigned below
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Rebuilding module's functions... ");
  cMergedDb.nDbModuleFunctionsCount = nCount = aNewStoreModulesFunctionsList.GetCount();
  cMergedDb.aOutputDbModuleFunctions.Attach((ULONG*)nktMemMalloc(nCount*2*sizeof(ULONG)));
  if (cMergedDb.aOutputDbModuleFunctions == NULL)
    goto err_nomem;
  for (k=0; k<nCount; k++)
  {
    cMergedDb.aOutputDbModuleFunctions.Get()[(k<<1)] = aNewStoreModulesFunctionsList[k].nModId;
    cMergedDb.aOutputDbModuleFunctions.Get()[(k<<1)+1] = aNewStoreModulesFunctionsList[k].nFuncId;
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Relocating strings... ");
  nCount = aNewStoreDataList.GetCount();
  lpDestChildObj = cMergedDb.aOutputDbObjectChilds.Get();
  for (k=nChildCounter=0; k<nCount; k++)
  {
    if ((k & 15) == 15 || k == nCount-1)
    {
      wprintf_s(L"\rRelocating strings (%.2lf)... ", 80.0 * ((double)(k+1)/(double)nCount));
      if (IsAbortKeyPressed() != FALSE) //check for abort
        return FALSE;
    }
    //----
    lpDb = aInputDbs[aNewStoreDataList[k].nDbIndex];
    lpSrcObj = lpDb->aStoredDbObjects[aNewStoreDataList[k].nStoredDataIndex];
    lpDestObj = cMergedDb.aOutputDbObjects.Get() + k;
    //find the "name" string
    sW[0] = lpDb->szSuperStringW + lpSrcObj->nNameOffset;
    lpDestObj->nNameOffset = (ULONG)FindStringInStringList(aStringsList, sW[0]);
    NKT_ASSERT(lpDestObj->nNameOffset != 0xFFFFFFFFUL);
    //find the "namespace" string
    sW[0] = aInputNamespaces[aNewStoreDataList[k].nDbIndex];
    lpDestObj->nNamespaceOffset = (ULONG)FindStringInStringList(aStringsList, sW[0]);
    NKT_ASSERT(lpDestObj->nNamespaceOffset != 0xFFFFFFFFUL);
    //process the childs of this object
    nChildsCount = lpDb->aStoredDbObjects_ChildsCount[aNewStoreDataList[k].nStoredDataIndex];
    lpSrcChildObj = lpDb->aStoredDbObjects_FirstChild[aNewStoreDataList[k].nStoredDataIndex];
    for (i=0; i<nChildsCount; i++,nChildCounter++)
    {
      //find the "name" string
      sW[0] = lpDb->szSuperStringW + lpSrcChildObj[i].nNameOffset;
      lpDestChildObj[nChildCounter].nNameOffset = (ULONG)FindStringInStringList(aStringsList, sW[0]);
      NKT_ASSERT(lpDestChildObj[nChildCounter].nNameOffset != 0xFFFFFFFFUL);
    }
  }
  //----
  nCount = aNewStoreModulesList.GetCount();
  for (k=0; k<nCount; k++)
  {
    if ((k & 15) == 15 || k == nCount-1)
    {
      wprintf_s(L"\rRelocating strings (%.2lf)... ", 80.0 + 20.0 * ((double)(k+1)/(double)nCount));
      if (IsAbortKeyPressed() != FALSE) //check for abort
        return FALSE;
    }
    //----
    lpDb = aInputDbs[aNewStoreModulesList[k].nDbIndex];
    lpDestMod = cMergedDb.aOutputDbModules.Get() + k;
    //find the "name" string
    sW[0] = lpDb->szSuperStringW + lpDb->aStoredDbModules[aNewStoreModulesList[k].nStoredModuleIndex
                                                         ]->nNameOffset;
    lpDestMod->nNameOffset = (ULONG)FindStringInStringList(aStringsList, sW[0]);
    NKT_ASSERT(lpDestMod->nNameOffset != 0xFFFFFFFFUL);
  }
  PrintAndLog(L"OK\n");
  //----
  PrintAndLog(L"Rebuilding strings... ");
  nCount = aStringsList.GetCount();
  if (aStringOffsetsList.SetCount(nCount) == FALSE)
    goto err_nomem;
  for (i=k=0; i<nCount; i++)
  {
    aStringOffsetsList[i] = (ULONG)k;
    k += wcslen(aStringsList[i]) + 1;
  }
  cMergedDb.nSuperStringSize = k*sizeof(WCHAR);
  cMergedDb.szOutputSuperStringW.Attach((LPWSTR)nktMemMalloc(cMergedDb.nSuperStringSize));
  if (cMergedDb.szOutputSuperStringW == NULL)
    goto err_nomem;
  for (i=k=0; i<nCount; i++)
  {
    nTotal = wcslen(aStringsList[i]) + 1;
    nktMemCopy(cMergedDb.szOutputSuperStringW.Get()+k, aStringsList[i], nTotal*sizeof(WCHAR));
    k += nTotal;
  }
  //replace name offsets
  for (i=0; i<cMergedDb.nDbObjectsListCount; i++)
  {
    lpDestObj = cMergedDb.aOutputDbObjects.Get()+i;
    lpDestObj->nNameOffset = aStringOffsetsList[lpDestObj->nNameOffset];
    lpDestObj->nNamespaceOffset = aStringOffsetsList[lpDestObj->nNamespaceOffset];
    k = cMergedDb.aStoredDbObjects_ChildsCount[i];
    lpDestChildObj = cMergedDb.aStoredDbObjects_FirstChild[i];
    while (k > 0)
    {
      lpDestChildObj->nNameOffset = aStringOffsetsList[lpDestChildObj->nNameOffset];
      lpDestChildObj++;
      k--;
    }
  }
  for (i=0; i<cMergedDb.nDbModulesListCount; i++)
  {
    lpDestMod = cMergedDb.aOutputDbModules.Get()+i;
    lpDestMod->nNameOffset = aStringOffsetsList[lpDestMod->nNameOffset];
  }
  PrintAndLog(L"OK\n");
  if (IsAbortKeyPressed() != FALSE) //check for abort
    return FALSE;
  return TRUE;
}

//-----------------------------------------------------------

SIZE_T CNktDvDbMerge::FindStringInStringList(WSTRING_LIST &aStringsList, LPCWSTR sW)
{
  if (sW == NULL)
    sW = L"";
  return aStringsList.BinarySearch((LPWSTR*)&sW, CNktDvDbMerge::StringList_Compare, NULL);
}

LPWSTR CNktDvDbMerge::AddStringToStringList(WSTRING_LIST &aStringsList, LPCWSTR sW)
{
  CNktStringW cStrTempW;
  LPWSTR *lpsW;

  lpsW = aStringsList.BinarySearchPtr((LPWSTR*)&sW, CNktDvDbMerge::StringList_Compare, NULL);
  if (lpsW != NULL)
    return *lpsW;
  if (sW[0] != 0)
  {
    if (cStrTempW.Copy(sW) == FALSE)
      return NULL;
  }
  else
  {
    if (cStrTempW.EnsureBuffer(1) == FALSE)
      return NULL;
    ((LPWSTR)cStrTempW)[0] = 0;
    cStrTempW.Refresh();
  }
  if (aStringsList.SortedInsert((LPWSTR)cStrTempW, CNktDvDbMerge::StringList_Compare, NULL) == FALSE)
    return NULL;
  return cStrTempW.Detach();
}

int CNktDvDbMerge::StringList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2)
{
  return wcscmp(*lpElem1, *lpElem2);
}
