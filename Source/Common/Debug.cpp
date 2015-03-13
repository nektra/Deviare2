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

#include <Windows.h>
#include <stdio.h>
#include "Debug.h"
#include "WaitableObjects.h"
#include "RegistrySettings.h"

//-----------------------------------------------------------

#ifndef NKT_DV_ARRAYLEN
  #define NKT_DV_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))
#endif //NKT_DV_ARRAYLEN

#define OUTPUT_BUFFER_SIZE                              4096
//#define USE_LOCAL_ALLOC

//-----------------------------------------------------------

static int nDbgLevelMask = DEFAULT_DBGLEVEL_MASK;
static struct {
  LONG volatile nLock;
  LONG volatile nInitialized;
  CRITICAL_SECTION cs;
} sDebugOutput = { 0 };

//-----------------------------------------------------------

static VOID SendDebugOutput(__in LPCSTR szBufA, __in SIZE_T nBufLen);

//-----------------------------------------------------------

namespace Nektra
{

// set nDbgLevelMask
VOID SetDebugLevel(__in int nNewDebugLevelMask)
{
  nDbgLevelMask = nNewDebugLevelMask;
  return;
}

int GetDebugLevel()
{
  return nDbgLevelMask;
}

BOOL CheckDebugLevel(__in int nMask)
{
#if (defined(NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY))
  if ((CNktDvRegistrySettings::GetDebugLevelMask() & (DWORD)nMask) != 0)
    return TRUE;
#endif //NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY
  return ((nDbgLevelMask & nMask) != 0) ? TRUE : FALSE;
}

// DebugPrint
VOID DebugPrintA(__in_z LPCSTR szFormatA, ...)
{
  va_list ap;

  va_start(ap, szFormatA);
  DebugPrintV_A(szFormatA, ap);
  va_end(ap);
  return;
}

VOID DebugPrintV_A(__in_z LPCSTR szFormatA, va_list ap)
{
  if (szFormatA != NULL)
  {
    int i;
#ifdef USE_LOCAL_ALLOC
    LPSTR szBufA = (LPSTR)::LocalAlloc(LMEM_FIXED, OUTPUT_BUFFER_SIZE);
    if (szBufA != NULL)
    {
#else //USE_LOCAL_ALLOC
    CHAR szBufA[OUTPUT_BUFFER_SIZE];
#endif //USE_LOCAL_ALLOC
      i = vsnprintf_s(szBufA, OUTPUT_BUFFER_SIZE, _TRUNCATE, szFormatA, ap);
      if (i < 0)
        i = 0;
      else if (i > OUTPUT_BUFFER_SIZE-1)
        i = OUTPUT_BUFFER_SIZE-1;
      szBufA[i] = 0;
      SendDebugOutput(szBufA, i);
#ifdef USE_LOCAL_ALLOC
      ::LocalFree((HLOCAL)szBufA);
    }
#endif //USE_LOCAL_ALLOC
  }
  return;
}

VOID DebugPrintLnA(__in_z LPCSTR szFormatA, ...)
{
  va_list ap;

  va_start(ap, szFormatA);
  DebugPrintLnV_A(szFormatA, ap);
  va_end(ap);
  return;
}

VOID DebugPrintLnV_A(__in_z LPCSTR szFormatA, va_list ap)
{
  if (szFormatA != NULL)
  {
    int i;
#ifdef USE_LOCAL_ALLOC
    LPSTR szBufA = (LPSTR)::LocalAlloc(LMEM_FIXED, OUTPUT_BUFFER_SIZE);
    if (szBufA != NULL)
    {
#else //USE_LOCAL_ALLOC
    CHAR szBufA[OUTPUT_BUFFER_SIZE];
#endif //USE_LOCAL_ALLOC
      i = vsnprintf_s(szBufA, OUTPUT_BUFFER_SIZE, _TRUNCATE, szFormatA, ap);
      if (i < 0)
        i = 0;
      else if (i > OUTPUT_BUFFER_SIZE-2)
        i = OUTPUT_BUFFER_SIZE-2;
      szBufA[i++] = '\n';
      szBufA[i] = 0;
      SendDebugOutput(szBufA, i);
#ifdef USE_LOCAL_ALLOC
      ::LocalFree((HLOCAL)szBufA);
    }
#endif //USE_LOCAL_ALLOC
  }
  return;
}

} //namespace

//-----------------------------------------------------------

#ifdef NKT_FUNCTION_TIMING_ENABLED

CNktFunctionTimingDebug::CNktFunctionTimingDebug(__in_z LPCSTR _szNameA)
{
  szNameA = _szNameA;
  if (szNameA == NULL)
    szNameA = "";
  NKT_DEBUGPRINTLNA(Nektra::dlFunctionTiming, ("%s: Enter\n", szNameA));
  if (::QueryPerformanceFrequency(&liFreq) == FALSE ||
      ::QueryPerformanceCounter(&liStart) == FALSE)
  {
    liFreq.QuadPart = 0;
    dwEnterTime = ::GetTickCount();
  }
  return;
}

CNktFunctionTimingDebug::~CNktFunctionTimingDebug()
{
  double nDbl;
  DWORD dw;

  if (liFreq.QuadPart != 0)
  {
    ::QueryPerformanceCounter(&liEnd);
    if (liEnd.QuadPart >= liStart.QuadPart)
      liEnd.QuadPart -= liStart.QuadPart;
    else
      liEnd.QuadPart = (~liStart.QuadPart)+liEnd.QuadPart+1ui64;
    nDbl = double(liEnd.QuadPart) / (double(liFreq.QuadPart) / 1000.0);
  }
  else
  {
    dw = ::GetTickCount();
    if (dw >= dwEnterTime)
      dw -= dwEnterTime;
    else
      dw = (~dwEnterTime)+dw+1UL;
    nDbl = (double)dw;
  }
  NKT_DEBUGPRINTLNA(Nektra::dlFunctionTiming, ("%s Leave [Elapsed %.3fms]\n", szNameA, (float)nDbl));
  return;
}
#endif //NKT_FUNCTION_TIMING_ENABLED

//-----------------------------------------------------------

static VOID SendDebugOutput(__in LPCSTR szBufA, __in SIZE_T nBufLen)
{
  static LONG volatile nIsVistaOrLater = -1;
  static WCHAR szLogFileNameW[2048] = { 0 };
  static HANDLE hLogFile = NULL;

  if (nIsVistaOrLater < 0)
  {
    NktInterlockedExchange(&nIsVistaOrLater, (LOBYTE(LOWORD(::GetVersion())) >= 6) ? 1 : 0);
  }
  if (sDebugOutput.nInitialized == 0)
  {
    CNktSimpleLockNonReentrant cAccessLock(&sDebugOutput.nLock);

    if (sDebugOutput.nInitialized == 0)
    {
      ::InitializeCriticalSection(&(sDebugOutput.cs));
      NktInterlockedExchange(&(sDebugOutput.nInitialized), 1);
    }
  }
  //----
  if (CheckDebugLevel(Nektra::dlWriteOutputToFile) != FALSE)
  {
    SIZE_T k;
    DWORD dw;

    ::EnterCriticalSection(&(sDebugOutput.cs));
    try
    {
      if (szLogFileNameW[0] == 0)
      {
        ::GetTempPathW(2000, szLogFileNameW);
        szLogFileNameW[1999] = 0;
        k = wcslen(szLogFileNameW);
        if (k > 0 && szLogFileNameW[k-1] != L'\\')
          szLogFileNameW[k++] = L'\\';
        swprintf_s(szLogFileNameW+k, 2048-k, L"Deviare%lu.log", ::GetCurrentProcessId());
      }
      if (hLogFile == NULL)
      {
        hLogFile = ::CreateFileW(szLogFileNameW, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL, NULL);
        if (hLogFile == NULL)
          hLogFile = INVALID_HANDLE_VALUE;
      }
      if (hLogFile != INVALID_HANDLE_VALUE)
      {
        ::WriteFile(hLogFile, szBufA, (DWORD)nBufLen, &dw, NULL);
      }
    }
    catch (...)
    { }
    ::LeaveCriticalSection(&(sDebugOutput.cs));
  }
  else if (nIsVistaOrLater >= 1 /*||
           CheckDebugLevel(Nektra::dlEnableOutputOnXP) != FALSE*/)
  {
    //because OutputDebugStringA problems with Windows 2003, we only enable it on Vista or later
    ::EnterCriticalSection(&(sDebugOutput.cs));
    ::OutputDebugStringA(szBufA);
    ::LeaveCriticalSection(&(sDebugOutput.cs));
  }
  return;
}
