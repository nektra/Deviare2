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
#include <conio.h>

//-----------------------------------------------------------

#if defined _M_IX86
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib.lib")
  #endif //_DEBUG
#elif defined _M_X64
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64.lib")
  #endif //_DEBUG
#else
  #error Unsupported platform
#endif

#ifdef _DEBUG
  #define DEBUG_KEYPRESS_ON_EXIT() _getch()
#else //_DEBUG
  #define DEBUG_KEYPRESS_ON_EXIT()
#endif //_DEBUG

//-----------------------------------------------------------

static HANDLE hLogFile = NULL;
ULONG nProcessingBits = 0;
CNktStringW cStrAppPathW;
CNktStringW cStrOutputDbW, cStrOutputPathW;
TNktArrayListWithFree<LPWSTR> aInputDatabases;
TNktArrayListWithFree<LPWSTR> aInputNamespaces;
TNktArrayListWithDelete<CDbMergeDvDatabase*> aInputDbs;
CDbMergeDvDatabase cMergedDb;

//-----------------------------------------------------------

int wmain(int argc, WCHAR* argv[])
{
  CNktStringW cStrTempW;
  SECURITY_ATTRIBUTES sSa;
  LPWSTR sW;
  SIZE_T i, k;

#if defined _M_IX86
  nProcessingBits = 32;
#elif defined _M_X64
  nProcessingBits = 64;
#endif
  if (argc < 5 || ((argc-5) % 2) != 0)
  {
    wprintf(L"Usage: DbMerge%lu.exe output.db base-input.db input-db-1.db namespace-1 "
                                                          L"[input-db-2.db namespace-2 ...]\n",
            nProcessingBits);
    goto error_exit;
  }
  //output database
  if (cStrOutputDbW.Copy(argv[1]) == FALSE ||
      cStrTempW.Copy(argv[2]) == FALSE)
  {
err_nomem:
    wprintf(L"Error: Not enough memory.\n");
    goto error_exit;
  }
  if (cStrOutputDbW.IsEmpty() != FALSE)
  {
    wprintf(L"Error: Output database not specified.\n");
    goto error_exit;
  }
  if (cStrTempW.IsEmpty() != FALSE)
  {
    wprintf(L"Error: Base database not specified.\n");
    goto error_exit;
  }
  //base input database
  if (aInputDatabases.AddElement((LPWSTR)cStrTempW) == FALSE)
    goto err_nomem;
  cStrTempW.Detach();
  if (cStrTempW.EnsureBuffer(1) == FALSE)
    goto err_nomem;
  ((LPWSTR)cStrTempW)[0] = 0;
  if (aInputNamespaces.AddElement((LPWSTR)cStrTempW) == FALSE)
    goto err_nomem;
  cStrTempW.Detach();
  //other databases
  for (k=3; k<(SIZE_T)argc; k+=2)
  {
    //add another database to the list
    if (cStrTempW.Copy(argv[k]) == FALSE)
      goto err_nomem;
    if (aInputDatabases.AddElement((LPWSTR)cStrTempW) == FALSE)
      goto err_nomem;
    cStrTempW.Detach();
    //process the namespace
    if (cStrTempW.Copy(argv[k+1]) == FALSE)
      goto err_nomem;
    //check valid namespace
    sW = (LPWSTR)cStrTempW;
    if (cStrTempW.GetLength() > 32)
    {
      wprintf(L"Error: Namespace identifier too long (max. 32 characters).\n");
      goto error_exit;
    }
    while (sW[0] == L'_')
      sW++;
    if (sW[0] == 0)
    {
      wprintf(L"Error: Invalid namespace identifier.\n");
      goto error_exit;
    }
    if (sW[0] >= L'0' && sW[0] <= L'9')
    {
      wprintf(L"Error: A letter must appear before a digit in namespace identifier.\n");
      goto error_exit;
    }
    while (*sW != 0)
    {
      if (!((sW[0] >= L'A' && sW[0] <= L'Z') ||
            (sW[0] >= L'a' && sW[0] <= L'z') ||
            (sW[0] >= L'0' && sW[0] <= L'9')))
      {
        wprintf(L"Error: Only digits and letters are allowed in namespace identifier.\n");
        goto error_exit;
      }
      sW++;
    }
    for (i=0; i<aInputNamespaces.GetCount(); i++)
    {
      if (wcscmp(aInputNamespaces[i], (LPWSTR)cStrTempW) == 0)
      {
        wprintf(L"Error: Duplicate namespace identifier found.\n");
        goto error_exit;
      }
    }
    if (aInputNamespaces.AddElement((LPWSTR)cStrTempW) == FALSE)
      goto err_nomem;
    cStrTempW.Detach();
  }
  //get app's folder name
  if (cStrAppPathW.EnsureBuffer(16384) == FALSE)
    goto err_nomem;
  ::GetModuleFileNameW(NULL, cStrAppPathW, 16384);
  cStrAppPathW.Refresh();
  k = cStrAppPathW.GetLength();
  sW = (LPWSTR)cStrAppPathW;
  while (k>0 && sW[k-1]!=L'\\' && sW[k-1]!=L'/')
    k--;
  sW[k] = 0;
  //get input full path
  for (k=0; k<aInputDatabases.GetCount(); k++)
  {
    if (cStrTempW.EnsureBuffer(16384) == FALSE)
      goto err_nomem;
    if (::GetFullPathNameW(aInputDatabases[k], 16384, (LPWSTR)cStrTempW, NULL) == 0)
    {
      wprintf(L"Error: Invalid input database path (%s).\n", aInputDatabases[k]);
      goto error_exit;
    }
    cStrTempW.Refresh();
    nktMemFree(aInputDatabases[k]);
    aInputDatabases[k] = cStrTempW.Detach();
  }
  //get output full path
  if (cStrTempW.EnsureBuffer(16384) == FALSE)
    goto err_nomem;
  if (::GetFullPathNameW((LPWSTR)cStrOutputDbW, 16384, (LPWSTR)cStrTempW, NULL) == 0)
  {
    wprintf(L"Error: Invalid output database path.\n");
    goto error_exit;
  }
  cStrTempW.Refresh();
  if (cStrOutputDbW.Copy((LPWSTR)cStrTempW) == FALSE)
    goto err_nomem;
  //get output path
  if (cStrOutputPathW.Copy((LPWSTR)cStrOutputDbW) == FALSE)
    goto err_nomem;
  k = cStrOutputPathW.GetLength();
  sW = (LPWSTR)cStrOutputPathW;
  while (k>0 && sW[k-1]!=L'\\' && sW[k-1]!=L'/')
    k--;
  sW[k] = 0;
  cStrOutputPathW.Refresh();
  //create log file
  if (cStrTempW.Format(L"%sDbMerge%lu.log", (LPWSTR)cStrOutputPathW, nProcessingBits) == FALSE)
    goto err_nomem;
  nktMemSet(&sSa, 0, sizeof(sSa));
  sSa.nLength = sizeof(sSa);
  sSa.bInheritHandle = TRUE;
  hLogFile = ::CreateFileW((LPWSTR)cStrTempW, GENERIC_WRITE, FILE_SHARE_READ, &sSa, CREATE_ALWAYS, 0, NULL);
  if (hLogFile == NULL || hLogFile == INVALID_HANDLE_VALUE)
  {
    wprintf(L"Error: Cannot create log file\n");
    goto error_exit;
  }
  //load databases into memory
  if (CNktDvDbMerge::LoadInputDbs() == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  //build new one
  if (CNktDvDbMerge::BuldMerged() == FALSE)
    goto error_exit;
  //and save
  if (CNktDvDbMerge::SaveMerged((LPWSTR)cStrOutputDbW) == FALSE)
    goto error_exit;

  //end
  PrintAndLogSeparator();
  PrintAndLog(L"Database successfully generated!\n");
  ::CloseHandle(hLogFile);
  DEBUG_KEYPRESS_ON_EXIT();
  return 0;

error_exit:
  ::CloseHandle(hLogFile);
  DEBUG_KEYPRESS_ON_EXIT();
  return 1;
}

//-----------------------------------------------------------

VOID PrintAndLog(LPCWSTR szFormatW, ...)
{
  CNktStringW cStrTempW;
  va_list argptr;
  SIZE_T i, nLen;
  DWORD dwWritten;
  LPSTR sA;
  LPWSTR sW;

  if (szFormatW == NULL)
    return;
  va_start(argptr, szFormatW);
  if (cStrTempW.FormatV(szFormatW, argptr) == FALSE)
  {
    va_end(argptr);
    return;
  }
  va_end(argptr);
  //print to console
  wprintf_s(L"%s", (LPWSTR)cStrTempW);
  //convert \n to \r\n
  for (;;)
  {
    nLen = cStrTempW.GetLength();
    sW = (LPWSTR)cStrTempW;
    for (i=0; i<nLen; i++)
    {
      if (sW[i] == L'\n' && (i == 0 || sW[i-1] != L'\r'))
        break;
    }
    if (i >= nLen)
      break;
    if (cStrTempW.InsertN(L"\r", i, 1) == FALSE)
      return;
  }
  nLen = cStrTempW.GetLength();
  i = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)cStrTempW, (int)nLen, NULL, 0, NULL, NULL);
  sA = (LPSTR)nktMemMalloc((i+1)*sizeof(CHAR));
  if (sA != NULL)
  {
    i = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)cStrTempW, (int)nLen, sA, (int)(i+1), NULL, NULL);
    ::WriteFile(hLogFile, sA, (DWORD)i, &dwWritten, NULL);
    nktMemFree(sA);
  }
  return;
}

VOID LogOnly(LPCWSTR szFormatW, ...)
{
  CNktStringW cStrTempW;
  va_list argptr;
  SIZE_T i, nLen;
  DWORD dwWritten;
  LPSTR sA;
  LPWSTR sW;

  if (szFormatW == NULL)
    return;
  va_start(argptr, szFormatW);
  if (cStrTempW.FormatV(szFormatW, argptr) == FALSE)
  {
    va_end(argptr);
    return;
  }
  va_end(argptr);
  //convert \n to \r\n
  for (;;)
  {
    nLen = cStrTempW.GetLength();
    sW = (LPWSTR)cStrTempW;
    for (i=0; i<nLen; i++)
    {
      if (sW[i] == L'\n' && (i == 0 || sW[i-1] != L'\r'))
        break;
    }
    if (i >= nLen)
      break;
    if (cStrTempW.InsertN(L"\r", i, 1) == FALSE)
      return;
  }
  nLen = cStrTempW.GetLength();
  i = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)cStrTempW, (int)nLen, NULL, 0, NULL, NULL);
  sA = (LPSTR)nktMemMalloc((i+1)*sizeof(CHAR));
  if (sA != NULL)
  {
    i = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, (LPWSTR)cStrTempW, (int)nLen, sA, (int)(i+1), NULL, NULL);
    ::WriteFile(hLogFile, sA, (DWORD)i, &dwWritten, NULL);
    nktMemFree(sA);
  }
  return;
}

VOID PrintAndLogNoMemoryError()
{
  PrintAndLog(L"\nError: Not enough memory.\n");
  return;
}

VOID PrintAndLogSeparator()
{
  PrintAndLog(L"----------------------------------------\n");
  return;
}

BOOL IsAbortKeyPressed()
{
  //check for abort
  if (_kbhit() != 0)
  {
    if (_getch() == 27)
    {
      PrintAndLog(L"\nInfo: Aborted by user.\n");
      return TRUE;
    }
  }
  return FALSE;
}
