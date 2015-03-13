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

#include "FastLogger.h"
#include <process.h>

//-----------------------------------------------------------

static unsigned int __stdcall _ThreadProc(void* pArg);

//-----------------------------------------------------------

CNktDvFastLogger::CNktDvFastLogger() : CNktFastMutex()
{
  lpFirstMsg = lpLastMsg = NULL;
  hFile = NULL;
  return;
}

CNktDvFastLogger::~CNktDvFastLogger()
{
  Finalize();
  return;
}

HRESULT CNktDvFastLogger::Initialize(__in LPCWSTR szLogFileNameW, __in BOOL bAppend)
{
  CNktAutoFastMutex cLock(this);
  union {
    LARGE_INTEGER li;
    DWORD dwWritten;
  };
  DWORD dwOsErr;

  if (szLogFileNameW == NULL)
    return E_POINTER;
  if (szLogFileNameW[0] == 0)
    return E_INVALIDARG;
  Finalize();
  //----
  hFile = ::CreateFile(szLogFileNameW, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                       (bAppend == FALSE) ? CREATE_ALWAYS : OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
  {
    dwOsErr = ::GetLastError();
    hFile = NULL;
    if (dwOsErr == ERROR_NOT_ENOUGH_MEMORY)
      dwOsErr = ERROR_OUTOFMEMORY;
    return HRESULT_FROM_WIN32(dwOsErr);
  }
  if (bAppend != FALSE)
  {
    li.QuadPart = 0;
    ::SetFilePointerEx(hFile, li, NULL, FILE_END);
  }
  else
  {
    ::WriteFile(hFile, "\xFF\xFE", 2, &dwWritten, NULL);
  }
  //----
  if (cQuitEv.Create(TRUE, FALSE) == FALSE ||
      cWakeUpEv.Create(TRUE, FALSE) == FALSE ||
      (hThread = (HANDLE)_beginthreadex(NULL, 0, &_ThreadProc, this, 0, NULL)) == 0)
  {
    Finalize();
    return E_OUTOFMEMORY;
  }
  return S_OK;
}

VOID CNktDvFastLogger::Finalize()
{
  if (hThread != NULL)
  {
    cQuitEv.Set();
    ::WaitForSingleObject(hThread, INFINITE);
    ::CloseHandle(hThread);
    hThread = NULL;
  }
  {
    CNktAutoFastMutex cLock(this);
    LPMSG_ITEM lpCurrMsg, lpNextMsg;

    if (hFile != NULL)
    {
      ::CloseHandle(hFile);
      hFile = NULL;
    }
    //----
    cWakeUpEv.Destroy();
    //----
    for (lpCurrMsg=lpFirstMsg; lpCurrMsg!=NULL; lpCurrMsg=lpNextMsg)
    {
      lpNextMsg = lpCurrMsg->lpNext;
      free(lpCurrMsg);
    }
    lpFirstMsg = lpLastMsg = NULL;
  }
  return;
}

HRESULT CNktDvFastLogger::Log(__in LPCWSTR szFormatW, ...)
{
  va_list argptr;
  HRESULT hRes;

  va_start(argptr, szFormatW);
  hRes = LogV(szFormatW, argptr);
  va_end(argptr);
  return hRes;
}

HRESULT CNktDvFastLogger::LogV(__in LPCWSTR szFormatW, __inout va_list argptr)
{
  MSG_ITEM *lpNewItem;
  int nChars;

  if (szFormatW == NULL)
    return E_POINTER;
  nChars = _vscwprintf(szFormatW, argptr);
  if (nChars > 0)
  {
    lpNewItem = (LPMSG_ITEM)malloc(sizeof(MSG_ITEM)+nChars*sizeof(WCHAR));
    if (lpNewItem == NULL)
      return E_OUTOFMEMORY;
    lpNewItem->lpNext = NULL;
    lpNewItem->dwLengthInBytes = nChars*sizeof(WCHAR);
    _vsnwprintf_s(lpNewItem->szMessageW, (size_t)nChars+1, _TRUNCATE, szFormatW, argptr);
    {
      CNktAutoFastMutex cLock(this);

      if (hFile == NULL)
        return E_FAIL;
      if (lpLastMsg != NULL)
        lpLastMsg->lpNext = lpNewItem;
      lpLastMsg = lpNewItem;
      if (lpFirstMsg == NULL)
        lpFirstMsg = lpNewItem;
      cWakeUpEv.Set();
    }
  }
  return S_OK;
}

VOID CNktDvFastLogger::ThreadProc()
{
  DWORD dwHitEv;
  HANDLE hEvents[2];

  hEvents[0] = cQuitEv.GetEventHandle();
  hEvents[1] = cWakeUpEv.GetEventHandle();
  while (1)
  {
    dwHitEv = ::WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
    if (dwHitEv == WAIT_OBJECT_0)
      break;
    cWakeUpEv.Reset();
    if (dwHitEv == WAIT_OBJECT_0+1)
    {
      if (::WaitForSingleObject(hEvents[0], 200) == WAIT_OBJECT_0)
        break;
      WriteMessages(FALSE);
    }
  }
  WriteMessages(TRUE); //flush
  return;
}

VOID CNktDvFastLogger::WriteMessages(__in BOOL bFlush)
{
  CNktAutoFastMutex cLock(this);
  LPMSG_ITEM lpCurrMsg;
  DWORD dwWritten;
  SIZE_T nCount;

  nCount = (bFlush != FALSE) ? ((SIZE_T)-1) : 100;
  while (lpFirstMsg != NULL && nCount > 0)
  {
    lpCurrMsg = lpFirstMsg;
    lpFirstMsg = lpCurrMsg->lpNext;
    if (lpLastMsg == lpCurrMsg)
      lpLastMsg = NULL;
    ::WriteFile(hFile, lpCurrMsg->szMessageW, lpCurrMsg->dwLengthInBytes, &dwWritten, NULL);
    free(lpCurrMsg);
    nCount--;
  }
  return;
}

//-----------------------------------------------------------

static unsigned int __stdcall _ThreadProc(void* pArg)
{
  ((CNktDvFastLogger*)pArg)->ThreadProc();
  return 0;
}
