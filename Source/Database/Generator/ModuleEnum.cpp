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

#include "ModuleEnum.h"
#include "ConfigLdr.h"

//-----------------------------------------------------------

static BOOL CompareFuncName(LPCWSTR szSrcW, LPCSTR szSrcA);
static ULONG StripId(LPCWSTR szIdW);

//-----------------------------------------------------------

BOOL DoModEnum()
{
  ULONG nIndexM, nFuncId, nModIdx, nRowsCount;
  LPWSTR sW, szModuleW;
  LPSTR sA;
  HMODULE hMod;
  CNktStringW cStrTempW;
  IMAGE_DOS_HEADER *lpDosHdr;
  IMAGE_OPTIONAL_HEADER *lpOptHdr;
  IMAGE_EXPORT_DIRECTORY *lpExpDir;
  DWORD i, dwFuncIdx, *lpAddrNamesRVA;
  CNktSQLiteConnector::eErrorCode nDbErr;
  CNktComBStr cTempBStr;
  WCHAR szBufW[24];

  for (nIndexM=nModIdx=0; ; nIndexM++)
  {
    //check for abort
    if (IsAbortKeyPressed() != FALSE)
      return FALSE;
    //get module name
    szModuleW = ConfigGetModuleName(nIndexM);
    if (szModuleW == NULL)
      break;
    PrintAndLog(L"Processing module: '%s'... ", szModuleW);
    hMod = ::LoadLibraryW(szModuleW);
    if (hMod == NULL)
    {
      PrintAndLog(L"Not found.\n");
      continue;
    }
    //get the export table
    lpDosHdr = (IMAGE_DOS_HEADER*)hMod;
    lpOptHdr =  (IMAGE_OPTIONAL_HEADER*)((BYTE*)hMod + lpDosHdr->e_lfanew + 24);
    if (lpOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
        lpOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
    {
      PrintAndLog(L"No exports found.\n");
      ::FreeLibrary(hMod);
      continue;
    }
    lpExpDir = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)hMod + lpOptHdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    lpAddrNamesRVA = (DWORD*)((BYTE*)hMod + lpExpDir->AddressOfNames);
    if (lpExpDir->NumberOfNames == 0)
    {
      PrintAndLog(L"No named exports found.\n");
      ::FreeLibrary(hMod);
      continue;
    }
    //process this module
    nRowsCount = 0;
    for (dwFuncIdx=0; dwFuncIdx<lpExpDir->NumberOfNames; dwFuncIdx++)
    {
      //check for abort
      if (IsAbortKeyPressed() != FALSE)
        return FALSE;
      swprintf_s(szBufW, 24, L"(%.2lf%%)", ((float)dwFuncIdx / (float)(lpExpDir->NumberOfNames)) * 100.0f);
      wprintf_s(L"%s", szBufW);
      for (i=0; szBufW[i]!=0; i++)
        szBufW[i] = L'\b';
      wprintf_s(L"%s", szBufW);
      //get export name
      sA = (LPSTR)((BYTE*)hMod + lpAddrNamesRVA[dwFuncIdx]);
      //find function
      for (nFuncId=0; (SIZE_T)nFuncId<aFunctionsList.GetCount(); nFuncId++)
      {
        sW = aFunctionsList[nFuncId];
        while ((*sW) >= L'0' && (*sW) <= L'9')
          sW++;
        NKT_ASSERT(*sW == L'-');
        sW++;
        if (CompareFuncName(sW, sA) != FALSE)
        {
          i = StripId(aFunctionsList[nFuncId]);
          break;
        }
      }
      if ((SIZE_T)nFuncId >= aFunctionsList.GetCount())
      {
        LogOnly(L"\nWARNING: Api '%S' of module '%s' not found in header file.", sA, szModuleW);
        continue;
      }
      nFuncId = (ULONG)i;
      //add this entry
      if (nRowsCount == 0)
      {
        //first insert module row
        nModIdx++;
        sW = szModuleW + wcslen(szModuleW);
        while (sW>szModuleW && *(sW-1)!=L'\\' && *(sW-1)!=L'/')
          sW--;
        //add row
        if (cStrTempW.Copy(L"INSERT INTO Modules (Id,Name) VALUES (") == FALSE ||
            cStrTempW.Concat(nModIdx) == FALSE ||
            cStrTempW.Concat(L",'") == FALSE ||
            cStrTempW.Concat(sW) == FALSE ||
            cStrTempW.Concat(L"');") == FALSE)
        {
dme_err_nomem:
          PrintAndLogNoMemoryError();
          return FALSE;
        }
        nDbErr = cDB.QueryExecute(1, cStrTempW);
        if (nDbErr != CNktSQLiteConnector::noError)
        {
dme_err_query:
          if (nDbErr == CNktSQLiteConnector::errNoMem)
            goto dme_err_nomem;
          PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
          return FALSE;
        }
      }
      //add row
      if (cStrTempW.Copy(L"INSERT INTO ModulesFuncs (ModId,FuncId) VALUES (") == FALSE ||
          cStrTempW.Concat(nModIdx) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nFuncId) == FALSE ||
          cStrTempW.Concat(L");") == FALSE)
        goto dme_err_nomem;
      nDbErr = cDB.QueryExecute(1, cStrTempW);
     if (nDbErr != CNktSQLiteConnector::noError)
       goto dme_err_query;
     nRowsCount++;
    }
    PrintAndLog(L"GOT %lu of %lu EXPORT%s.\n", nRowsCount, lpExpDir->NumberOfNames,
                (lpExpDir->NumberOfNames != 1) ? L"S" : L"");
    ::FreeLibrary(hMod);
  }
  return TRUE;
}

static BOOL CompareFuncName(LPCWSTR szSrcW, LPCSTR szSrcA)
{
  WCHAR szTempW[4];

  while (*szSrcW != 0)
  {
    if (*((LPBYTE)szSrcA) < 128)
    {
      if (*((USHORT*)szSrcW) != (USHORT)*((LPBYTE)szSrcA))
        return FALSE;
    }
    else
    {
      if (::MultiByteToWideChar(CP_ACP, 0, szSrcA, 1, szTempW, 4) != 1)
        return FALSE;
      if (*szSrcW != szTempW[0])
        return FALSE;
    }
    szSrcA++;
    szSrcW++;
  }
  return (*szSrcA == 0) ? TRUE : FALSE;
}

static ULONG StripId(LPCWSTR szIdW)
{
  ULONG k, nVal;

  nVal = 0;
  while (szIdW[0]>=L'0' && szIdW[0]<=L'9')
  {
    k = nVal * 10;
    NKT_ASSERT(k >= nVal);
    nVal = k + (ULONG)(szIdW[0] - L'0');
    NKT_ASSERT(nVal >= k);
    szIdW++;
  }
  return nVal;
}
