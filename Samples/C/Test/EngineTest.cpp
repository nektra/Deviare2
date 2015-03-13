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

#include "EngineTest.h"
#include "MySpyMgr.h"
#include "FastLogger.h"
#include "TestHook.h"
#include "TestWndProcHook.h"
#include "resource.h"

//-----------------------------------------------------------

CMDLINE_PARAMS sCmdLineParams;
static CNktFastMutex cConsoleMtx;
static CNktDvFastLogger cLogger;

//-----------------------------------------------------------

static BOOL ParseCommandLine(__in int argc, __in WCHAR* argv[]);
static SIZE_T ParseCommandLine_GetUInt(__inout LPWSTR &sW);

//-----------------------------------------------------------

int wmain(__in int argc, __in WCHAR* argv[])
{
  CMySpyMgr cSpyMgr;
  HRESULT hRes;

  hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

  if (ParseCommandLine(argc, argv) == FALSE)
    return 1;
  //----
  wprintf_s(L"Initializing Deviare Engine... ");
  //init logging
  if (sCmdLineParams.cStrLogFileNameW.IsEmpty() == FALSE)
  {
    hRes = cLogger.Initialize(sCmdLineParams.cStrLogFileNameW);
    if (FAILED(hRes))
    {
      wprintf_s(L"Error: Cannot initialize logger (%08X).\n", hRes);
      return -1;
    }
  }
  //init engine
  hRes = cSpyMgr.Initialize();
  if (FAILED(hRes))
  {
    wprintf_s(L"Error: Cannot initialize engine (%08X).\n", hRes);
    return -1;
  }
  wprintf_s(L"OK\n");
  //start test
  switch (sCmdLineParams.nTestType)
  {
    case ttTestHookExec:
    case ttTestHookAttach:
      TestHookExecOrAttach(cSpyMgr.Get());
      break;
    case ttTestWndProcHook:
      TestHookWndProc(cSpyMgr.Get());
      break;
  }
  //----
  cLogger.Finalize();

  cSpyMgr.Finalize();

  ::CoUninitialize();
  return 0;
}

BOOL IsExitKeyPressed()
{
  if (_kbhit() != 0)
  {
    if (_getwch() == 27)
      return TRUE;
  }
  return FALSE;
}

VOID PressAnyKeyToContinue()
{
  wprintf_s(L"Press any key to continue...\n");
  while (_kbhit() == 0)
    ::Sleep(100);
  while (_kbhit() != 0)
    _getwch();
  return;
}

VOID LogPrintNoTick(__in LPCWSTR szFormatW, ...)
{
  WCHAR szBufW[4096];
  va_list argptr;
  int nLen;

  va_start(argptr, szFormatW);
  if (sCmdLineParams.cStrLogFileNameW.IsEmpty() != FALSE)
  {
    CNktAutoFastMutex cConsoleLock(&cConsoleMtx);

    nLen = vswprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), szFormatW, argptr);
    if (nLen < 0)
      nLen = 0;
    szBufW[nLen] = 0;
    wprintf_s(L"%s", szBufW);
    while (nLen>0 && (szBufW[nLen-1]==L'\n' || szBufW[nLen-1]==L'\r'))
      nLen--;
    szBufW[nLen] = 0;
    //NKT_DEBUGPRINTA(Nektra::dlDebug, ("%S", szBufW));
  }
  else
  {
    cLogger.LogV(szFormatW, argptr);
  }
  va_end(argptr);
  return;
}

VOID LogPrint(__in LPCWSTR szFormatW, ...)
{
  WCHAR szBufW[4096];
  va_list argptr;
  DWORD dwTick;
  int i, nLen;

  dwTick = ::GetTickCount();
  nLen = swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"%lu: ", dwTick);
  va_start(argptr, szFormatW);
  i = vswprintf_s(szBufW+nLen, NKT_DV_ARRAYLEN(szBufW)-nLen, szFormatW, argptr);
  if (i > 0)
    nLen += i;
  szBufW[nLen] = 0;
  va_end(argptr);
  if (sCmdLineParams.cStrLogFileNameW.IsEmpty() != FALSE)
  {
    CNktAutoFastMutex cConsoleLock(&cConsoleMtx);

    wprintf_s(L"%s", szBufW);
    while (nLen>0 && (szBufW[nLen-1]==L'\n' || szBufW[nLen-1]==L'\r'))
      nLen--;
    szBufW[nLen] = 0;
    //NKT_DEBUGPRINTA((Nektra::dlDebug, "%S", szBufW));
  }
  else
  {
    cLogger.Log(L"%s", szBufW);
  }
  return;
}

CNktFastMutex* LogGetMutex()
{
  return &cConsoleMtx;
}

//-----------------------------------------------------------

static BOOL ParseCommandLine(__in int argc, __in WCHAR* argv[])
{
#if defined _M_IX86
  static LPCWSTR szExeNameW = L"CTest";
#elif defined _M_X64
  static LPCWSTR szExeNameW = L"CTest64";
#else
  #error Unsupported platform
#endif
  CNktStringW cStrTempW;
  LPWSTR sW;
  SIZE_T i, k, nCurrArg;

  if (argc < 2)
  {
    wprintf_s(L"Usage: %s command command-param [options]\n\n", szExeNameW);
    wprintf_s(L"Where 'command' can be one of the following:\n");
    wprintf_s(L"  exec: Test hooking functionallity in a new application.\n");
    wprintf_s(L"        'command-param' indicates the executable filename.\n");
    wprintf_s(L"  attach: Test hooking functionallity in a running application.\n");
    wprintf_s(L"          'command-param' indicates a comma-separated list of process ids.\n");
    wprintf_s(L"  wndproc: Test hooking functionallity of a window procedure.\n");
    wprintf_s(L"          'command-param' indicates a comma-separated list of window's handles.\n");
    wprintf_s(L"\nCommon 'options' is (see help for each command for more options):\n");
    wprintf_s(L"  -log[=file]: Sends output to a log file.\n");
    wprintf_s(L"  -xml=file: Use another xml file for api hook source.\n");
    wprintf_s(L"  -async: Hooks are processes asynchronously. Only custom parameters are shown.\n");
    wprintf_s(L"  -debugch: Do not call custom handlers inside a try/catch block.\n");
    wprintf_s(L"  -nobatch: Do not use batch hook.\n");
    wprintf_s(L"  -nostack: Do not display call stack.\n");
    wprintf_s(L"  -ch=file: Custom handler to add to each hook (can be specified more than once).\n");
    wprintf_s(L"\n'command-param' is:\n");
    return FALSE;
  }
  //----
  sCmdLineParams.nTestType = (eTestType)-1;
  sCmdLineParams.bBatchMode = TRUE;
  sCmdLineParams.bDontDisplayStack = FALSE;
  sCmdLineParams.bAsyncCallbacks = sCmdLineParams.bCustomHandlerDebug = FALSE;
  for (nCurrArg=1; nCurrArg<(SIZE_T)argc; nCurrArg++)
  {
    if (argv[nCurrArg][0] == L'/' || argv[nCurrArg][0] == L'-')
    {
      //-log
      if (_wcsnicmp(argv[nCurrArg]+1, L"log", 3) == 0 &&
          (argv[nCurrArg][4] == 0 || argv[nCurrArg][4] == L'='))
      {
        if (sCmdLineParams.cStrLogFileNameW.IsEmpty() == FALSE)
        {
          wprintf_s(L"Error: Log option already specified.\n");
          return E_FAIL;
        }
        if (argv[nCurrArg][4] == L'=')
        {
          if (sCmdLineParams.cStrLogFileNameW.Copy(argv[nCurrArg]+5) == FALSE)
          {
pcl_err_nomem:
            wprintf_s(L"Error: Not enough memory.\n");
            return FALSE;
          }
          if (sCmdLineParams.cStrLogFileNameW.IsEmpty() != FALSE)
          {
pcl_err_invalid:
            wprintf_s(L"Error: Invalid command line.\n");
            return FALSE;
          }
        }
        else
        {
          if (sCmdLineParams.cStrLogFileNameW.Format(L"%s.log", szExeNameW) == FALSE)
            goto pcl_err_nomem;
        }
      }
      //-xml
      else if (_wcsnicmp(argv[nCurrArg]+1, L"xml=", 4) == 0)
      {
        if (sCmdLineParams.cStrXmlFileNameW.IsEmpty() == FALSE)
        {
          wprintf_s(L"Error: Xml option already specified.\n");
          return E_FAIL;
        }
        if (sCmdLineParams.cStrXmlFileNameW.Copy(argv[nCurrArg]+5) == FALSE)
          goto pcl_err_nomem;
        if (sCmdLineParams.cStrXmlFileNameW.IsEmpty() != FALSE)
          goto pcl_err_invalid;
      }
      //-ch
      else if (_wcsnicmp(argv[nCurrArg]+1, L"ch=", 3) == 0)
      {
        if (cStrTempW.Copy(argv[nCurrArg]+4) == FALSE)
          goto pcl_err_nomem;
        if (cStrTempW.IsEmpty() != FALSE)
          goto pcl_err_invalid;
        if (sCmdLineParams.aCustomHandlers.AddElement((LPWSTR)cStrTempW) == FALSE)
          goto pcl_err_nomem;
        cStrTempW.Detach();
      }
      //-nobatch
      else if (_wcsicmp(argv[nCurrArg]+1, L"nobatch") == 0)
      {
        if (sCmdLineParams.bBatchMode == FALSE)
        {
          wprintf_s(L"Error: nobatch option already specified.\n");
          return FALSE;
        }
        sCmdLineParams.bBatchMode = FALSE;
      }
      //-nostack
      else if (_wcsicmp(argv[nCurrArg]+1, L"nostack") == 0)
      {
        if (sCmdLineParams.bDontDisplayStack != FALSE)
        {
          wprintf_s(L"Error: nostack option already specified.\n");
          return FALSE;
        }
        sCmdLineParams.bDontDisplayStack = TRUE;
      }
      //-async
      else if (_wcsicmp(argv[nCurrArg]+1, L"async") == 0)
      {
        if (sCmdLineParams.bAsyncCallbacks != FALSE)
        {
          wprintf_s(L"Error: async option already specified.\n");
          return FALSE;
        }
        sCmdLineParams.bAsyncCallbacks = TRUE;
      }
      //-debugch
      else if (_wcsicmp(argv[nCurrArg]+1, L"debugch") == 0)
      {
        if (sCmdLineParams.bCustomHandlerDebug != FALSE)
        {
          wprintf_s(L"Error: debugch option already specified.\n");
          return FALSE;
        }
        sCmdLineParams.bCustomHandlerDebug = TRUE;
      }
      else
      {
        goto pcl_err_invalid;
      }
    }
    else
    {
      if (sCmdLineParams.nTestType != (eTestType)-1)
      {
        wprintf_s(L"Error: Command already specified.\n");
        return FALSE;
      }
      //exec command
      if (_wcsicmp(argv[nCurrArg], L"exec") == 0)
      {
        sCmdLineParams.nTestType = ttTestHookExec;
        if (nCurrArg+1 >= (SIZE_T)argc ||
            argv[nCurrArg+1][0] == L'/' || argv[nCurrArg+1][0] == L'-')
          goto pcl_err_invalid;
        //get executable file name
        if (sCmdLineParams.sExec.cStrExeFileNameW.Copy(argv[nCurrArg+1]) == FALSE)
          goto pcl_err_nomem;
        nCurrArg++; //skip command-param
      }
      //attach command
      else if (_wcsicmp(argv[nCurrArg], L"attach") == 0)
      {
        sCmdLineParams.nTestType = ttTestHookAttach;
        sCmdLineParams.sAttach.nCount = 0;
        if (nCurrArg+1 >= (SIZE_T)argc ||
            argv[nCurrArg+1][0] == L'/' || argv[nCurrArg+1][0] == L'-')
          goto pcl_err_invalid;
        sW = argv[nCurrArg+1];
        while (*sW != 0)
        {
          //get pid
          if (sCmdLineParams.sAttach.nCount >= NKT_DV_ARRAYLEN(sCmdLineParams.sAttach.dwPids))
          {
            wprintf_s(L"Error: Too many process ids specified.\n");
            return FALSE;
          }
          k = ParseCommandLine_GetUInt(sW);
          if (k == (SIZE_T)-1 || k < 1 || k > 0xFFFF)
          {
            wprintf_s(L"Error: Invalid process id.\n");
            return FALSE;
          }
          sCmdLineParams.sAttach.dwPids[sCmdLineParams.sAttach.nCount] = (DWORD)k;
          for (i=0; i<sCmdLineParams.sAttach.nCount; i++)
          {
            if (sCmdLineParams.sAttach.dwPids[i] ==
                sCmdLineParams.sAttach.dwPids[sCmdLineParams.sAttach.nCount])
              break; //already specified
          }
          if (i >= sCmdLineParams.sAttach.nCount)
            sCmdLineParams.sAttach.nCount++;
          //separator or end of param
          if (sW[0] == L',')
            sW++;
          else if (sW[0] != 0)
            goto pcl_err_invalid;
        }
        if (sCmdLineParams.sAttach.nCount == 0)
          goto pcl_err_invalid;
        nCurrArg++; //skip command-param
      }
      //wndproc command
      else if (_wcsicmp(argv[1], L"wndproc") == 0)
      {
        sCmdLineParams.nTestType = ttTestWndProcHook;
        sCmdLineParams.sWndProc.nCount = 0;
        if (nCurrArg+1 >= (SIZE_T)argc ||
            argv[nCurrArg+1][0] == L'/' || argv[nCurrArg+1][0] == L'-')
          goto pcl_err_invalid;
        sW = argv[nCurrArg+1];
        while (*sW != 0)
        {
          //get hwnd
          if (sCmdLineParams.sWndProc.nCount >= NKT_DV_ARRAYLEN(sCmdLineParams.sWndProc.hWnds))
          {
            wprintf_s(L"Error: Too many window handles specified.\n");
            return FALSE;
          }
          k = ParseCommandLine_GetUInt(sW);
          if (k == (SIZE_T)-1 || k < 1 || k > 0xFFFFFFFF)
          {
            wprintf_s(L"Error: Invalid window handle.\n");
            return FALSE;
          }
          sCmdLineParams.sWndProc.hWnds[sCmdLineParams.sWndProc.nCount] = (HWND)k;
          for (i=0; i<sCmdLineParams.sWndProc.nCount; i++)
          {
            if (sCmdLineParams.sWndProc.hWnds[i] ==
                sCmdLineParams.sWndProc.hWnds[sCmdLineParams.sWndProc.nCount])
              break; //already specified
          }
          if (i >= sCmdLineParams.sWndProc.nCount)
            sCmdLineParams.sWndProc.nCount++;
          //separator or end of param
          if (sW[0] == L',')
            sW++;
          else if (sW[0] != 0)
            goto pcl_err_invalid;
        }
        if (sCmdLineParams.sWndProc.nCount == 0)
          goto pcl_err_invalid;
        nCurrArg++; //skip command-param
      }
      //unknown command
      else
      {
        goto pcl_err_invalid;
      }
    }
  }
  if (sCmdLineParams.nTestType == (eTestType)-1)
  {
    wprintf_s(L"Error: Command not specified or invalid.\n");
    return FALSE;
  }
  if (sCmdLineParams.cStrXmlFileNameW.IsEmpty() != FALSE)
  {
    if (sCmdLineParams.cStrXmlFileNameW.EnsureBuffer(10240) == FALSE)
      goto pcl_err_nomem;
    k = (SIZE_T)::GetModuleFileNameW(NULL, sCmdLineParams.cStrXmlFileNameW, 10240);
    while (k > 0 && ((LPWSTR)sCmdLineParams.cStrXmlFileNameW)[k-1] != L'\\')
      k--;
    ((LPWSTR)sCmdLineParams.cStrXmlFileNameW)[k] = 0;
    sCmdLineParams.cStrXmlFileNameW.Refresh();
    if (sCmdLineParams.cStrXmlFileNameW.Concat(L"ctest.hooks.xml") == FALSE)
      goto pcl_err_nomem;
  }
  return TRUE;
}

static SIZE_T ParseCommandLine_GetUInt(__inout LPWSTR &sW)
{
  SIZE_T k, nVal;

  if (sW[0] < L'0' || sW[0] > L'9')
    return (SIZE_T)-1;
  nVal = 0;
  while (sW[0] >= L'0' && sW[0] <= L'9')
  {
    if (nVal*10 < nVal)
      return (SIZE_T)-1; //overflow
    k = (SIZE_T)(sW[0] - L'0');
    nVal *= 10;
    if (nVal+k < nVal)
      return (SIZE_T)-1; //overflow
    nVal += k;
    sW++;
  }
  return nVal;
}
