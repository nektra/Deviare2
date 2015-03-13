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

#include "StringLiteW.h"
#include <crtdbg.h>

//-----------------------------------------------------------

#define X_ALIGN(x, _size)                                  \
                        (((x)+((_size)-1)) & (~((_size)-1)))
#define X_ARRAYLEN(x)               (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

CNktStringW::CNktStringW()
{
  szStrW = NULL;
  nSize = nLen = 0;
  return;
}

CNktStringW::~CNktStringW()
{
  Empty();
  return;
}

VOID CNktStringW::Empty()
{
  if (szStrW != NULL)
    free(szStrW);
  szStrW = NULL;
  nSize = nLen = 0;
  return;
}

VOID CNktStringW::Refresh()
{
  if (szStrW != NULL)
  {
    szStrW[nSize-1] = 0;
    nLen = wcslen(szStrW);
  }
  else
  {
    nLen = 0;
  }
  return;
}

BOOL CNktStringW::Copy(__in_z_opt LPCSTR szSrcA)
{
  Empty();
  return Concat(szSrcA);
}

BOOL CNktStringW::CopyN(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen)
{
  Empty();
  return ConcatN(szSrcA, nSrcLen);
}

BOOL CNktStringW::Concat(__in_z_opt LPCSTR szSrcA)
{
  SIZE_T nSrcLen;

  nSrcLen = (szSrcA != NULL) ? strlen(szSrcA) : 0;
  return ConcatN(szSrcA, nSrcLen);
}

BOOL CNktStringW::ConcatN(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen)
{
  int nDestLen;

  if (nSrcLen == 0)
    return TRUE;
  if (szSrcA == NULL)
    return FALSE;
  nDestLen = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, NULL, 0);
  if (nDestLen > 0)
  {
    if (nLen+(SIZE_T)nDestLen < nLen)
      return FALSE; //overflow
    if (EnsureBuffer(nLen+nDestLen) == FALSE)
      return FALSE;
    ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, szStrW+nLen, nDestLen);
    nLen += (SIZE_T)nDestLen;
    szStrW[nLen] = 0;
  }
  return TRUE;
}

BOOL CNktStringW::Copy(__in_z_opt LPCWSTR szSrcW)
{
  Empty();
  return Concat(szSrcW);
}

BOOL CNktStringW::CopyN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen)
{
  Empty();
  return ConcatN(szSrcW, nSrcLen);
}

BOOL CNktStringW::Concat(__in_z_opt LPCWSTR szSrcW)
{
  SIZE_T nSrcLen;

  nSrcLen = (szSrcW != NULL) ? wcslen(szSrcW) : 0;
  return ConcatN(szSrcW, nSrcLen);
}

BOOL CNktStringW::ConcatN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen)
{
  if (nSrcLen == 0)
    return TRUE;
  if (szSrcW == NULL)
    return FALSE;
  if (nLen+nSrcLen < nLen)
    return FALSE; //overflow
  if (EnsureBuffer(nLen+nSrcLen) == FALSE)
    return FALSE;
  memcpy(szStrW+nLen, szSrcW, nSrcLen*sizeof(WCHAR));
  nLen += nSrcLen;
  szStrW[nLen] = 0;
  return TRUE;
}

BOOL CNktStringW::Copy(__in LONG nSrc)
{
  Empty();
  return Concat(nSrc);
}

BOOL CNktStringW::Concat(__in LONG nSrc)
{
  return Concat((LONGLONG)nSrc);
}

BOOL CNktStringW::Copy(__in ULONG nSrc)
{
  Empty();
  return Concat(nSrc);
}

BOOL CNktStringW::Concat(__in ULONG nSrc)
{
  return Concat((ULONGLONG)nSrc);
}

BOOL CNktStringW::Copy(__in LONGLONG nSrc)
{
  Empty();
  return Concat(nSrc);
}

BOOL CNktStringW::Concat(__in LONGLONG nSrc)
{
  WCHAR szTempW[128];

  swprintf_s(szTempW, X_ARRAYLEN(szTempW), L"%I64d", nSrc);
  return Concat(szTempW);
}

BOOL CNktStringW::Copy(__in ULONGLONG nSrc)
{
  Empty();
  return Concat(nSrc);
}

BOOL CNktStringW::Concat(__in ULONGLONG nSrc)
{
  WCHAR szTempW[128];

  swprintf_s(szTempW, X_ARRAYLEN(szTempW), L"%I64u", nSrc);
  return Concat(szTempW);
}

BOOL CNktStringW::Copy(__in float nSrc, __in int nDigits)
{
  Empty();
  return Concat(nSrc, nDigits);
}

BOOL CNktStringW::Concat(__in float nSrc, __in int nDigits)
{
  return Concat((double)nSrc, nDigits);
}

BOOL CNktStringW::Copy(__in double nSrc, __in int nDigits)
{
  Empty();
  return Concat(nSrc, nDigits);
}

BOOL CNktStringW::Concat(__in double nSrc, __in int nDigits)
{
  WCHAR szTempW[128], szFmtW[16];

  if (nDigits < 0)
    nDigits = 0;
  if (nDigits > 15)
    nDigits = 15;
  swprintf_s(szFmtW, X_ARRAYLEN(szFmtW), L"%%.%ldf", (long)nDigits);
  swprintf_s(szTempW, X_ARRAYLEN(szTempW), szFmtW, nSrc);
  return Concat(szTempW);
}

BOOL CNktStringW::Format(__in_z LPCSTR szFormatA, ...)
{
  va_list argptr;
  BOOL b;

  Empty();
  if (szFormatA == NULL)
    return FALSE;
  va_start(argptr, szFormatA);
  b = FormatV(szFormatA, argptr);
  va_end(argptr);
  return b;
}

BOOL CNktStringW::Format(__in_z LPCWSTR szFormatW, ...)
{
  va_list argptr;
  BOOL b;

  Empty();
  if (szFormatW == NULL)
    return FALSE;
  va_start(argptr, szFormatW);
  b = FormatV(szFormatW, argptr);
  va_end(argptr);
  return b;
}

BOOL CNktStringW::FormatV(__in_z LPCSTR szFormatA, __in va_list argptr)
{
  Empty();
  return AppendFormatV(szFormatA, argptr);
}

BOOL CNktStringW::FormatV(__in_z LPCWSTR szFormatW, __in va_list argptr)
{
  Empty();
  return AppendFormatV(szFormatW, argptr);
}

BOOL CNktStringW::AppendFormat(__in_z LPCSTR szFormatA, ...)
{
  va_list argptr;
  BOOL b;

  if (szFormatA == NULL)
    return FALSE;
  va_start(argptr, szFormatA);
  b = AppendFormatV(szFormatA, argptr);
  va_end(argptr);
  return b;
}

BOOL CNktStringW::AppendFormat(__in_z LPCWSTR szFormatW, ...)
{
  va_list argptr;
  BOOL b;

  if (szFormatW == NULL)
    return FALSE;
  va_start(argptr, szFormatW);
  b = AppendFormatV(szFormatW, argptr);
  va_end(argptr);
  return b;
}

BOOL CNktStringW::AppendFormatV(__in LPCSTR szFormatA, __in va_list argptr)
{
  LPSTR szTempA;
  int nChars;
  BOOL b;

  if (szFormatA == NULL)
    return FALSE;
  nChars = _vscprintf(szFormatA, argptr);
  if (nChars > 0)
  {
    szTempA = (LPSTR)malloc((size_t)(nChars+1) * sizeof(CHAR));
    if (szTempA == NULL)
      return FALSE;
    vsprintf_s(szTempA, (size_t)nChars+1, szFormatA, argptr);
    b = ConcatN(szTempA, (size_t)nChars);
    free(szTempA);
    if (b == FALSE)
      return FALSE;
  }
  return TRUE;
}

BOOL CNktStringW::AppendFormatV(__in LPCWSTR szFormatW, __in va_list argptr)
{
  int nChars;

  if (szFormatW == NULL)
    return FALSE;
  nChars = _vscwprintf(szFormatW, argptr);
  if (nChars > 0)
  {
    if (nLen+(SIZE_T)nChars+1 < nLen)
      return FALSE; //overflow
    if (EnsureBuffer(nLen+(SIZE_T)nChars+1) == FALSE)
      return FALSE;
    vswprintf_s(szStrW+nLen, (size_t)nChars+1, szFormatW, argptr);
    nLen += (SIZE_T)nChars;
    szStrW[nLen] = 0;
  }
  return TRUE;
}

BOOL CNktStringW::Insert(__in_z_opt LPCWSTR szSrcW, __in SIZE_T nInsertPosition)
{
  SIZE_T nSrcLen;

  nSrcLen = (szSrcW != NULL) ? wcslen(szSrcW) : 0;
  return InsertN(szSrcW, nInsertPosition, nSrcLen);
}

BOOL CNktStringW::InsertN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nInsertPosition, __in SIZE_T nSrcLen)
{
  if (nSrcLen == 0)
    return TRUE;
  if (szSrcW == NULL)
    return FALSE;
  if (nLen+nSrcLen < nLen)
    return FALSE; //overflow
  if (EnsureBuffer(nLen+nSrcLen) == FALSE)
    return FALSE;
  if (nInsertPosition > nLen)
    nInsertPosition = nLen;
  memmove(szStrW+nInsertPosition+nSrcLen, szStrW+nInsertPosition, (nLen-nInsertPosition)*sizeof(WCHAR));
  memcpy(szStrW+nInsertPosition, szSrcW, nSrcLen*sizeof(WCHAR));
  nLen += nSrcLen;
  szStrW[nLen] = 0;
  return TRUE;
}

VOID CNktStringW::Delete(__in SIZE_T nStartChar, __in SIZE_T nChars)
{
  LPWSTR s;
  SIZE_T k;

  if (nLen > 0)
  {
    if (nStartChar > nLen)
      nStartChar = nLen;
    if (nStartChar+nChars > nLen ||
        nStartChar+nChars < nStartChar) //overflow
      nChars = nLen-nStartChar;
    k = nLen - nChars - nStartChar;
    s = szStrW + nStartChar;
    nLen -= nChars;
    memmove(s, s+nChars, k*sizeof(WCHAR));
    szStrW[nLen] = 0;
  }
  return;
}

LPWSTR CNktStringW::Detach()
{
  LPWSTR szRetW;

  szRetW = szStrW;
  szStrW = NULL;
  nSize = nLen = 0;
  return szRetW;
}

BOOL CNktStringW::EnsureBuffer(__in SIZE_T nChars)
{
  LPWSTR szNewStrW;

  nChars++;
  if (nChars >= nSize)
  {
    if (nSize > 0)
      nChars = X_ALIGN(nChars, 512);
    szNewStrW = (LPWSTR)malloc(nChars*sizeof(WCHAR));
    if (szNewStrW == NULL)
      return FALSE;
    memcpy(szNewStrW, szStrW, nLen*sizeof(WCHAR));
    szNewStrW[nLen] = 0;
    if (szStrW != NULL)
      free(szStrW);
    szStrW = szNewStrW;
    nSize = nChars;
  }
  return TRUE;
}
