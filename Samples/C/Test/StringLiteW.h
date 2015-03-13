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

#ifndef _NKT_STRINGW_H
#define _NKT_STRINGW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

//-----------------------------------------------------------

class CNktStringW
{
public:
  CNktStringW();
  virtual ~CNktStringW();

  virtual operator LPWSTR()
    {
    return (szStrW != NULL) ? szStrW : L"";
    };
  virtual operator LPCWSTR()
    {
    return (szStrW != NULL) ? szStrW : L"";
    };

  virtual VOID Empty();

  virtual VOID Refresh();

  virtual SIZE_T GetLength() const
    {
    return (szStrW != NULL) ? wcslen(szStrW) : 0;
    };

  virtual BOOL IsEmpty() const
    {
    return (szStrW != NULL && szStrW[0] != 0) ? FALSE : TRUE;
    };

  virtual BOOL Copy(__in_z_opt LPCSTR szSrcA);
  virtual BOOL CopyN(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen);
  virtual BOOL Concat(__in_z_opt LPCSTR szSrcA);
  virtual BOOL ConcatN(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen);

  virtual BOOL Copy(__in_z_opt LPCWSTR szSrcW);
  virtual BOOL CopyN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen);
  virtual BOOL Concat(__in_z_opt LPCWSTR szSrcW);
  virtual BOOL ConcatN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen);

  virtual BOOL Copy(__in LONG nSrc);
  virtual BOOL Concat(__in LONG nSrc);
  virtual BOOL Copy(__in ULONG nSrc);
  virtual BOOL Concat(__in ULONG nSrc);

  virtual BOOL Copy(__in LONGLONG nSrc);
  virtual BOOL Concat(__in LONGLONG nSrc);
  virtual BOOL Copy(__in ULONGLONG nSrc);
  virtual BOOL Concat(__in ULONGLONG nSrc);

  virtual BOOL Copy(__in float nSrc, __in int nDigits=2);
  virtual BOOL Concat(__in float nSrc, __in int nDigits=2);
  virtual BOOL Copy(__in double nSrc, __in int nDigits=2);
  virtual BOOL Concat(__in double nSrc, __in int nDigits=2);

  virtual BOOL Format(__in_z LPCSTR szFormatA, ...);
  virtual BOOL Format(__in_z LPCWSTR szFormatW, ...);
  virtual BOOL FormatV(__in_z LPCSTR szFormatA, __in va_list argptr);
  virtual BOOL FormatV(__in_z LPCWSTR szFormatW, __in va_list argptr);

  virtual BOOL AppendFormat(__in_z LPCSTR szFormatA, ...);
  virtual BOOL AppendFormat(__in_z LPCWSTR szFormatW, ...);
  virtual BOOL AppendFormatV(__in_z LPCSTR szFormatA, __in va_list argptr);
  virtual BOOL AppendFormatV(__in_z LPCWSTR szFormatW, __in va_list argptr);

  virtual BOOL Insert(__in_z_opt LPCWSTR szSrcW, __in SIZE_T nInsertPosition);
  virtual BOOL InsertN(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nInsertPosition, __in SIZE_T nSrcLen);
  virtual VOID Delete(__in SIZE_T nStartChar, __in SIZE_T nChars);

  virtual LPWSTR Detach();

  virtual BOOL EnsureBuffer(__in SIZE_T nChars);

private:
  LPWSTR szStrW;
  SIZE_T nSize, nLen;
};

//-----------------------------------------------------------

#endif //_NKT_STRINGW_H
