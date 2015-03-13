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

#include "DbGenerator.h"
#include <conio.h>
#include "Parser.h"
#include "ConfigLdr.h"
#include "SQLiteDbCreator.h"
#include "SQLiteDbLoader.h"
#include "DvDbCreator.h"

#ifdef _DEBUG
  //#define DO_NOT_CREATE_SQLITE_FILE
#else //_DEBUG
  #define DO_NOT_CREATE_SQLITE_FILE
#endif //_DEBUG

//-----------------------------------------------------------

#if defined _M_IX86
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\SqLiteLib_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\SqLiteLib.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib.lib")
  #endif //_DEBUG
#elif defined _M_X64
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\SqLiteLib64_Debug.lib")
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\SqLiteLib64.lib")
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

//88D96A05-F192-11D4-A65F-0040963251E5
const GUID CLSID_DOMDocument60 = {
  0x88D96A05, 0xF192, 0x11D4, { 0xA6, 0x5F, 0x00, 0x40, 0x96, 0x32, 0x51, 0xE5 }
};

//2933BF96-7B36-11D2-B20E-00C04F983E60
const GUID IID_IXMLDOMDocument3 = {
  0x2933BF96, 0x7B36, 0x11D2, { 0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60 }
};

static CNktComDll cComDll; //this static must be the first declared one
static HANDLE hLogFile = NULL;
ULONG nProcessingBits = 0;
CNktStringW cStrNamespaceW;
TNktComPtr<IXMLDOMDocument3> cXmlDoc;
CNktStringW cStrAppPathW;
CNktStringW cStrFile2ProcessW;
CNktStringW cStrOutputPathW;
CNktSQLiteConnector cDB;
CXmlHelper cXmlHelper;
TNktComPtr<IXMLDOMElement> cXmlRootElem;
TNktArrayListWithFree<LPWSTR,512> aFunctionsList;
static CSQLiteDbLoader cSrcData;
static CDvDbCreator cDestData;

//-----------------------------------------------------------

int wmain(int argc, WCHAR* argv[])
{
  CNktStringW cStrTempW[3];
  CNktStringW cStrTempXmlFile;
  SECURITY_ATTRIBUTES sSa;
  STARTUPINFOW sSiW;
  PROCESS_INFORMATION sPi;
  WCHAR szTempXmlFileW[4096], *sW;
  DWORD dwTemp;
  SIZE_T k;

  if (argc < 2)
  {
    wprintf(L"Usage: DbGenerator%lu.exe input-header-file.h [options]\n\n",
#if defined _M_IX86
            32
#elif defined _M_X64
            64
#endif
            );
    wprintf(L"Where 'options' can be:\n");
    wprintf(L"  /x86 or /x64: Force 32/64-bit mode processing. (Default: executable bits)\n");
    wprintf(L"  /out=folder : Sets the output folder (Default: executable path)\n");
    goto error_exit;
  }
  if (argc > 2)
  {
    for (k=2; k<(SIZE_T)argc; k++)
    {
      if (_wcsicmp(argv[k], L"/x86") == 0)
      {
        if (nProcessingBits != 0)
        {
err_procmodebits_alreadyspec:
          wprintf(L"Error: Processing bits mode already specified.\n");
          goto error_exit;
        }
        nProcessingBits = 32;
      }
      else if (_wcsicmp(argv[k], L"/x64") == 0)
      {
        if (nProcessingBits != 0)
          goto err_procmodebits_alreadyspec;
        nProcessingBits = 64;
      }
      else if (_wcsnicmp(argv[k], L"/out:", 5) == 0 ||
               _wcsnicmp(argv[k], L"/out=", 5) == 0)
      {
        if (cStrOutputPathW.IsEmpty() == FALSE)
        {
          wprintf(L"Error: Output path already specified.\n");
          goto error_exit;
        }
        if (cStrOutputPathW.Copy(argv[k]+5) == FALSE)
          goto err_nomem;
        if (cStrOutputPathW.IsEmpty() != FALSE)
        {
err_invalid_outpath:
          wprintf(L"Error: Invalid output path.\n");
          goto error_exit;
        }
        for (sW=cStrOutputPathW; *sW!=0; sW++)
        {
          if (sW[0] == L'/')
            sW[0] = L'\\';
        }
      }
      else
      {
        wprintf(L"Error: Invalid command line parameter.\n");
        goto error_exit;
      }
    }
  }
  if (nProcessingBits == 0)
  {
#if defined _M_IX86
    nProcessingBits = 32;
#elif defined _M_X64
    nProcessingBits = 64;
#endif
  }
  //get app's folder name
  if (cStrAppPathW.EnsureBuffer(16384) == FALSE)
  {
err_nomem:
    wprintf(L"Error: Not enough memory.\n");
    goto error_exit;
  }
  ::GetModuleFileNameW(NULL, cStrAppPathW, 16384);
  cStrAppPathW.Refresh();
  k = cStrAppPathW.GetLength();
  sW = (LPWSTR)cStrAppPathW;
  while (k>0 && sW[k-1]!=L'\\' && sW[k-1]!=L'/')
    k--;
  sW[k] = 0;
  //get header full path
  if (cStrFile2ProcessW.EnsureBuffer(16384) == FALSE)
    goto err_nomem;
  if (::GetFullPathNameW(argv[1], 16384, (LPWSTR)cStrFile2ProcessW, NULL) == 0)
  {
    wprintf(L"Error: Invalid header file name to process.\n");
    goto error_exit;
  }
  cStrFile2ProcessW.Refresh();
  //get output full path
  if (cStrOutputPathW.IsEmpty() == FALSE)
  {
    if (cStrTempW[0].EnsureBuffer(16384) == FALSE)
      goto err_nomem;
    if (::GetFullPathNameW((LPWSTR)cStrOutputPathW, 16384, (LPWSTR)cStrTempW[0], NULL) == 0)
      goto err_invalid_outpath;
    cStrTempW[0].Refresh();
    if (cStrOutputPathW.Copy((LPWSTR)cStrTempW[0]) == FALSE)
      goto err_nomem;
  }
  else
  {
    if (cStrOutputPathW.EnsureBuffer(16384) == FALSE)
      goto err_nomem;
    ::GetCurrentDirectoryW(16384, (LPWSTR)cStrOutputPathW);
    cStrOutputPathW.Refresh();
  }
  k = cStrOutputPathW.GetLength();
  if (k > 0 && ((LPWSTR)cStrOutputPathW)[k-1] != L'\\')
  {
    if (cStrOutputPathW.Concat(L"\\") == FALSE)
      goto err_nomem;
  }

  //create log file
  if (cStrTempW[0].Format(L"%sDbGenerator%lu.log", (LPWSTR)cStrOutputPathW, nProcessingBits) == FALSE)
    goto err_nomem;
  nktMemSet(&sSa, 0, sizeof(sSa));
  sSa.nLength = sizeof(sSa);
  sSa.bInheritHandle = TRUE;
  hLogFile = ::CreateFileW((LPWSTR)cStrTempW[0], GENERIC_WRITE, FILE_SHARE_READ, &sSa,
                           CREATE_ALWAYS, 0, NULL);
  if (hLogFile == NULL || hLogFile == INVALID_HANDLE_VALUE)
  {
    wprintf(L"\nError: Cannot create log file\n");
    goto error_exit;
  }

  //initialize COM
  if (FAILED(cComDll.Initialize(COINIT_MULTITHREADED)))
  {
    PrintAndLog(L"\nError: Cannot initialize COM.\n");
    goto error_exit;
  }

  //load application configuration
  PrintAndLog(L"Loading configuration file... ");
  if (cStrTempW[0].Format(L"%sconfig.xml", (LPWSTR)cStrAppPathW) == FALSE)
  {
err_nomem2:
    PrintAndLogNoMemoryError();
    goto error_exit;
  }
  if (LoadConfig((LPWSTR)cStrTempW[0]) == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  PrintAndLog(L"OK\n");

  //compile input header file
  if (cStrTempW[1].Format(L"%s..\\SupportFiles\\gccxml%lu\\", (LPWSTR)cStrAppPathW, nProcessingBits) == FALSE)
    goto err_nomem2;
  //build temp xml output file name
  ::GetTempFileNameW((LPWSTR)cStrOutputPathW, L"tmp", 0, szTempXmlFileW);
  PrintAndLogSeparator();
  PrintAndLog(L"Compiling header file (%s)...\n", (LPWSTR)cStrFile2ProcessW);
  if (cStrTempW[0].Format(L"\"%sgccxml.EXE\" \"%s\" \"-fxml=%s\" --gccxml-config \"%sconfig%lu\"",
                          (LPWSTR)cStrTempW[1], (LPWSTR)cStrFile2ProcessW, szTempXmlFileW,
                          (LPWSTR)cStrTempW[1], nProcessingBits) == FALSE)
    goto err_nomem2;
  nktMemSet(&sSiW, 0, sizeof(sSiW));
  sSiW.cb = (DWORD)sizeof(sSiW);
  sSiW.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW | STARTF_FORCEOFFFEEDBACK;
  sSiW.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
  sSiW.hStdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
  sSiW.hStdOutput = sSiW.hStdError = hLogFile;
  sSiW.wShowWindow = SW_HIDE;
  nktMemSet(&sPi, 0, sizeof(sPi));
  if (::CreateProcessW(NULL, (LPWSTR)cStrTempW[0], NULL, NULL, TRUE, 0, NULL, (LPWSTR)cStrTempW[1],
                       &sSiW, &sPi) == FALSE)
  {
    dwTemp = ::GetLastError();
#ifndef _DEBUG
    ::DeleteFileW(szTempXmlFileW);
#endif //_DEBUG
    PrintAndLog(L"Error spawning process (%08X)\n", dwTemp);
    goto error_exit;
  }
  ::WaitForSingleObject(sPi.hProcess, INFINITE);
  ::GetExitCodeProcess(sPi.hProcess, &dwTemp);
  ::CloseHandle(sPi.hThread);
  ::CloseHandle(sPi.hProcess);
  if (dwTemp != 0)
  {
#ifndef _DEBUG
    ::DeleteFileW(szTempXmlFileW);
#endif //_DEBUG
    PrintAndLog(L"Error: Compilation failed\n");
    goto error_exit;
  }
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  PrintAndLog(L"Compilation succeeded\n");
  PrintAndLogSeparator();

  //load compiled xml
  PrintAndLog(L"Loading compiled file... ");
  cXmlDoc = LoadXML(szTempXmlFileW);
#ifndef _DEBUG
  ::DeleteFileW(szTempXmlFileW);
#endif //_DEBUG
  if (cXmlDoc == NULL)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  PrintAndLog(L"OK\n");

  //build database
  PrintAndLogSeparator();
#ifdef DO_NOT_CREATE_SQLITE_FILE
  if (CreateSQLiteDatabase(L":memory:") == FALSE)
    goto error_exit;
#else //DO_NOT_CREATE_SQLITE_FILE
  if (cStrTempW[0].Format(L"%sdeviare%lu.sqlite", (LPWSTR)cStrOutputPathW, nProcessingBits) == FALSE)
    goto err_nomem2;
  ::DeleteFileW((LPWSTR)cStrTempW[0]);
  if (CreateSQLiteDatabase((LPWSTR)cStrTempW[0]) == FALSE)
    goto error_exit;
#endif //DO_NOT_CREATE_SQLITE_FILE
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;

  //free some memory
  cXmlHelper.Reset();
  cXmlDoc.Release();
  cXmlRootElem.Release();
  //PHASE 2... build final database
  PrintAndLogSeparator();
  if (cSrcData.LoadDatabase(cDB) == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  if (cSrcData.SortData() == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  if (cSrcData.CheckDataIntegrity() == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  //free some memory
  cDB.Close();

  //transform to basic types
  PrintAndLogSeparator();
  if (cSrcData.BuildBasicTypes() == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  //build objects
  PrintAndLogSeparator();
  if (cDestData.BuildObjects(cSrcData) == FALSE)
    goto error_exit;
  if (IsAbortKeyPressed() != FALSE) //check for abort
    goto error_exit;
  //free some memory
  cSrcData.FreeALotOfStuff();
  //hash objects
  cDestData.BuildHashes();
  //save objects into database
  PrintAndLogSeparator();
  if (cStrTempW[0].Format(L"%sdeviare%lu.db", (LPWSTR)cStrOutputPathW, nProcessingBits) == FALSE)
    goto err_nomem2;
  if (cDestData.SaveDatabase((LPWSTR)cStrTempW[0], cSrcData) == FALSE)
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

IXMLDOMDocument3* LoadXML(LPCWSTR szSrcFileNameW)
{
  TNktComPtr<IXMLDOMDocument3> cXmlDoc;
  CNktComVariant cVtSrcFile;
  HRESULT hRes;
  VARIANT_BOOL bStatus;

  hRes = CoCreateInstance(CLSID_DOMDocument60, NULL, CLSCTX_INPROC_SERVER,
                          IID_IXMLDOMDocument3, (void**)&cXmlDoc);
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot create IXMLDOMDocument3 object.\n");
    return NULL;
  }
  hRes = cXmlDoc->put_async(VARIANT_FALSE);
  if (SUCCEEDED(hRes))
    hRes = cXmlDoc->put_validateOnParse(VARIANT_FALSE);
  if (SUCCEEDED(hRes))
    hRes = cXmlDoc->put_resolveExternals(VARIANT_FALSE);
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot initialize IXMLDOMDocument3 object.\n");
    return NULL;
  }
  cVtSrcFile.sVt.bstrVal = ::SysAllocString(szSrcFileNameW);
  if (cVtSrcFile.sVt.bstrVal == NULL)
  {
    PrintAndLogNoMemoryError();
    return NULL;
  }
  V_VT(&cVtSrcFile) = VT_BSTR;
  hRes = cXmlDoc->load(cVtSrcFile, &bStatus);
  if (bStatus == VARIANT_FALSE)
  {
    TNktComPtr<IXMLDOMParseError> cObjError;
    CNktComBStr bStr;

    hRes = cXmlDoc->get_parseError(&cObjError);
    if (SUCCEEDED(hRes) && cObjError!=NULL)
    {
      hRes = cObjError->get_reason(&bStr);
      cObjError.Release();
    }
    PrintAndLog(L"\nError: Cannot load xml file.");
    if (bStr != NULL)
      PrintAndLog(L" (%s)", (LPWSTR)bStr);
    PrintAndLog(L"\n");
    return NULL;
  }
  return cXmlDoc.Detach();
}

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
