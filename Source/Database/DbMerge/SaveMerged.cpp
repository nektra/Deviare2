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

static BOOL AddDbData(LPVOID lpData, SIZE_T nDataSize, LPBYTE *lplpDest, PSIZE_T lpnDestLen,
                      PSIZE_T lpnDestSize);
static HRESULT WriteDbData(HANDLE hFile, LPVOID lpData, SIZE_T nDataSize);

//-----------------------------------------------------------

BOOL CNktDvDbMerge::SaveMerged(LPCWSTR szTargetFileW)
{

  HANDLE hFile;
  HRESULT hRes;
  Fnv64_t nDbHashValue;
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
  PrintAndLog(L"Saving merged database... ");
  hFile = ::CreateFileW((LPWSTR)cStrOutputDbW, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL, NULL);
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
  nTemp32 = (ULONG)cMergedDb.nSuperStringSize;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  if (AddDbData(cMergedDb.szOutputSuperStringW.Get(), cMergedDb.nSuperStringSize, &(sDest.lpPtr),
                &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  //write DbObjects count
  nCount = cMergedDb.nDbObjectsListCount;
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //write DbObject
    if (AddDbData(cMergedDb.aOutputDbObjects.Get()+i, sizeof(CNktDvDbObject::STORED_DATA),
                  &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    //write DbObject childs count
    nChildsCount = cMergedDb.aStoredDbObjects_ChildsCount[i];
    nTemp32 = (ULONG)nChildsCount;
    if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    for (k=0; k<nChildsCount; k++)
    {
      //write DbObject child
      if (AddDbData(cMergedDb.aStoredDbObjects_FirstChild[i] + k,
                    sizeof(CNktDvDbObject::STORED_CHILD_ITEM), &(sDest.lpPtr), &(sDest.nLen),
                    &(sDest.nSize)) == FALSE)
        goto sd_err_nomem;
    }
  }
  //write DB_MODULEs count
  nCount = cMergedDb.nDbModulesListCount;
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //write DB_MODULE
    if (AddDbData(cMergedDb.aOutputDbModules.Get()+i, sizeof(CNktDvDbObject::STORED_MODULE),
                  &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
  }
  //write DB_MODULE_FUNCTIONs count
  nCount = cMergedDb.nDbModuleFunctionsCount;
  nTemp32 = (ULONG)nCount;
  if (AddDbData(&nTemp32, sizeof(nTemp32), &(sDest.lpPtr), &(sDest.nLen), &(sDest.nSize)) == FALSE)
    goto sd_err_nomem;
  for (i=0; i<nCount; i++)
  {
    //find dbo map
    //write module id
    if (AddDbData(cMergedDb.aOutputDbModuleFunctions.Get()+(i<<1), sizeof(ULONG), &(sDest.lpPtr),
                  &(sDest.nLen), &(sDest.nSize)) == FALSE)
      goto sd_err_nomem;
    //write objects id
    if (AddDbData(cMergedDb.aOutputDbModuleFunctions.Get()+(i<<1)+1, sizeof(ULONG), &(sDest.lpPtr),
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

static BOOL AddDbData(LPVOID lpData, SIZE_T nDataSize, LPBYTE *lplpDest, PSIZE_T lpnDestLen,
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

static HRESULT WriteDbData(HANDLE hFile, LPVOID lpData, SIZE_T nDataSize)
{
  DWORD dwWritten;

  NKT_ASSERT(lpData != NULL);
  NKT_ASSERT(nDataSize > 0);
  if (::WriteFile(hFile, lpData, (DWORD)nDataSize, &dwWritten, NULL) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  return ((DWORD)nDataSize == dwWritten) ? S_OK : E_FAIL;
}
