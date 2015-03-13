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

BOOL CNktDvDbMerge::LoadInputDbs()
{
  CDbMergeDvDatabase *lpDb;
  HRESULT hRes;
  TNktAutoFreePtr<BYTE> aCompressedData;
  HANDLE hFile = NULL;
  SIZE_T i, k, nCount, nCompressedSize;
  Fnv64_t nHashValue, nStoredHash;
  DWORD dw;
  LPBYTE s;

  PrintAndLog(L"Loading input databases... ");
  for (k=0; k<aInputDatabases.GetCount(); k++)
  {
    lpDb = new CDbMergeDvDatabase;
    if (lpDb == NULL)
    {
err_nomem:
      if (hFile != NULL)
        ::CloseHandle(hFile);
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    if (aInputDbs.AddElement(lpDb) == FALSE)
    {
      delete lpDb;
      goto err_nomem;
    }

    //open database
    hFile = ::CreateFileW(aInputDatabases[k], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
    {
      hFile = NULL;
dbload_err:
      hRes = NKT_HRESULT_FROM_LASTERROR();
dbload_err2:
      if (hRes == E_OUTOFMEMORY)
        goto err_nomem;
      PrintAndLog(L"\nError: Cannot load database [%s] (%08X).\n", aInputDatabases[k], (ULONG)hRes);
      return FALSE;
    }
    //load into memory
    nCompressedSize = (SIZE_T)::GetFileSize(hFile, NULL);
    if (nCompressedSize <= (17 + sizeof(ULONG) + sizeof(Fnv64_t)) ||
      nCompressedSize >= 0x0FFFFFFF)
    {
dbload_err_fail:
      hRes = E_FAIL;
      goto dbload_err2;
    }
    aCompressedData.Reset();
    aCompressedData.Attach((LPBYTE)nktMemMalloc(nCompressedSize));
    if (aCompressedData == NULL)
      goto err_nomem;
    if (::ReadFile(hFile, aCompressedData.Get(), (DWORD)nCompressedSize, &dw, NULL) == FALSE)
      goto dbload_err;
    if (nCompressedSize != (SIZE_T)dw)
      goto dbload_err;
    nCompressedSize -= (17 + sizeof(ULONG) + sizeof(Fnv64_t));
    //check signature and hash
    s = aCompressedData.Get();
    if (nktMemCompare(s, "Deviare Database\x1A", 17) != 0)
      goto dbload_err_fail;
    s += 17;
    dw = *((ULONG NKT_UNALIGNED *)s);
    lpDb->nDecompressedSize = (SIZE_T)dw;
    s += sizeof(ULONG);
    nStoredHash = *((Fnv64_t NKT_UNALIGNED *)s);
    s += sizeof(Fnv64_t);
    nHashValue = fnv_64a_buf(&dw, sizeof(dw), FNV1A_64_INIT);
    nHashValue = fnv_64a_buf(s, nCompressedSize, nHashValue);
    if (nHashValue != nStoredHash)
      goto dbload_err_fail; //hash mismatch
    //create shared memory for decompressed data
    lpDb->aDecompressedData.Attach((LPBYTE)nktMemMalloc(lpDb->nDecompressedSize));
    if (lpDb->aDecompressedData == NULL)
      goto err_nomem;
    try
    {
      i = (SIZE_T)LZ4_uncompress_unknownOutputSize((char*)s, (char*)(lpDb->aDecompressedData.Get()),
                                                   (int)nCompressedSize, (int)(lpDb->nDecompressedSize));
    }
    catch (...)
    {
      i = NKT_SIZE_T_MAX; //force fail on error
    }
    if (lpDb->nDecompressedSize != i)
      goto dbload_err_fail;
    //close file
    ::CloseHandle(hFile);
    hFile = NULL;
    //initialize internal pointers
    s = lpDb->aDecompressedData.Get();
    //read super string
    nktMemCopy(&dw, s, sizeof(dw));
    s += sizeof(dw);
    lpDb->szSuperStringW = (LPWSTR)s;
    lpDb->nSuperStringSize = (SIZE_T)dw;
    s += lpDb->nSuperStringSize;
    //read DbObjects count
    nktMemCopy(&dw, s, sizeof(dw));
    s += sizeof(dw);
    lpDb->nDbObjectsListCount = (SIZE_T)dw;
    if (lpDb->nDbObjectsListCount == 0)
    {
      hRes = NKT_DVERR_CannotLoadDatabase;
      goto dbload_err2;
    }
    lpDb->aStoredDbObjects.Attach((CNktDvDbObjectNoRef::LPSTORED_DATA*)nktMemMalloc(
                          lpDb->nDbObjectsListCount * sizeof(CNktDvDbObjectNoRef::LPSTORED_DATA)));
    lpDb->aStoredDbObjects_ChildsCount.Attach((SIZE_T*)nktMemMalloc(lpDb->nDbObjectsListCount *
                                                                    sizeof(SIZE_T)));
    lpDb->aStoredDbObjects_FirstChild.Attach((CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM*)nktMemMalloc(
                          lpDb->nDbObjectsListCount * sizeof(CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM)));
    if (lpDb->aStoredDbObjects == NULL || lpDb->aStoredDbObjects_ChildsCount == NULL ||
        lpDb->aStoredDbObjects_FirstChild == NULL)
      goto err_nomem;
    //count DbObject childs
    for (i=nCount=0; i<lpDb->nDbObjectsListCount; i++)
    {
      lpDb->aStoredDbObjects[i] = (CNktDvDbObjectNoRef::LPSTORED_DATA)s;
      s += sizeof(CNktDvDbObjectNoRef::STORED_DATA);
      //read DbObject childs count
      dw = *((DWORD NKT_UNALIGNED *)s);
      s += sizeof(dw);
      lpDb->aStoredDbObjects_ChildsCount[i] = (SIZE_T)dw;
      nCount += (SIZE_T)dw;
      lpDb->aStoredDbObjects_FirstChild[i] = (CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM)s;
      s += (SIZE_T)dw * sizeof(CNktDvDbObjectNoRef::STORED_CHILD_ITEM);
    }
    /*
    //process DbObject's
    for (i=nCount=0; i<nDbObjectsListCount; i++)
    {
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
    */
    //read DB_MODULEs count
    dw = *((DWORD NKT_UNALIGNED *)s);
    s += sizeof(dw);
    lpDb->nDbModulesListCount = (SIZE_T)dw;
    if (lpDb->nDbModulesListCount == 0)
    {
      hRes = NKT_DVERR_CannotLoadDatabase;
      goto dbload_err2;
    }
    lpDb->aStoredDbModules.Attach((CNktDvDbObjectNoRef::LPSTORED_MODULE*)nktMemMalloc(
                          lpDb->nDbModulesListCount * sizeof(CNktDvDbObjectNoRef::LPSTORED_MODULE)));
    if (lpDb->aStoredDbModules == NULL)
      goto err_nomem;
    for (i=0; i<lpDb->nDbModulesListCount; i++)
    {
      lpDb->aStoredDbModules[i] = (CNktDvDbObjectNoRef::LPSTORED_MODULE)s;
      s += sizeof(CNktDvDbObjectNoRef::STORED_MODULE);
    }
    //read DB_MODULE_FUNCTIONs count
    dw = *((DWORD NKT_UNALIGNED *)s);
    s += sizeof(dw);
    lpDb->nDbModuleFunctionsCount = (SIZE_T)dw;
    if (lpDb->nDbModuleFunctionsCount > 0)
    {
      lpDb->aStoredDbModuleFunctions.Attach((ULONG*)nktMemMalloc(lpDb->nDbModuleFunctionsCount * 2 *
                                                                 sizeof(ULONG)));
      if (lpDb->aStoredDbModuleFunctions == NULL)
        goto err_nomem;
    }
    for (i=0; i<lpDb->nDbModuleFunctionsCount; i++)
    {
      //read DB_MODULE_FUNCTION
      lpDb->aStoredDbModuleFunctions[i<<1] = *((ULONG NKT_UNALIGNED *)s);
      s += sizeof(ULONG);
      lpDb->aStoredDbModuleFunctions[(i<<1)+1] = *((ULONG NKT_UNALIGNED *)s);
      s += sizeof(ULONG);
    }
    /*
    //sort modules by name
    cDbModuleNameIndex.Attach((ULONG*)nktMemMalloc(nDbModulesListCount*sizeof(ULONG)));
    if (cDbModuleNameIndex == NULL)
      goto err_nomem;
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
    //sort objects by name
    cDbObjectNameIndex.Attach((ULONG*)nktMemMalloc(nDbObjectsListCount*sizeof(ULONG)));
    if (cDbObjectNameIndex == NULL)
      goto err_nomem;
    for (i=0; i<nDbObjectsListCount; i++)
      cDbObjectNameIndex[i] = (ULONG)i;
    qsort_s(cDbObjectNameIndex.Get(), nDbObjectsListCount, sizeof(ULONG), SortDbObjectByName,
      lpDbObjectsList);
    //sort objects by class and name
    cDbObjectClassAndNameIndex.Attach((ULONG*)nktMemMalloc(nDbObjectsListCount*sizeof(ULONG)));
    if (cDbObjectClassAndNameIndex == NULL)
      goto err_nomem;
    for (i=0; i<nDbObjectsListCount; i++)
      cDbObjectClassAndNameIndex[i] = (ULONG)i;
    qsort_s(cDbObjectClassAndNameIndex.Get(), nDbObjectsListCount, sizeof(ULONG), SortDbObjectByClassAndName,
      lpDbObjectsList);
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
    */
  }
  PrintAndLog(L"OK\n");
  return TRUE;
}
