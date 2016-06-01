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

#pragma once

#include <windows.h>
//NOTE: On x64 VS2008 has a bug like this:
//      http://connect.microsoft.com/VisualStudio/feedback/details/573262/incorrect-alignment-with-x64-optimizer-and-movaps
//      But that patch is only for VS2010. Damn!
//      Also applied last patches found (KB974116 KB976656) and the problem still persists.
//
//FIX: In sqlite3.c and SQLiteConnector.cpp DO NOT set "Maximize speed (/O2)". Instead use "Custom" and set the following:
//       * Enable Instrinsic Functions: NO
//       * Favor Fast Code (/Ot)
//     And in Code Generation area:
//       * Enable String Pooling: Yes (/GF)
//       * Buffer Security Check: No (/GS-)

//-----------------------------------------------------------

class CNktSQLiteConnector
{
public:
  typedef enum {
    noError=0,
    errNoMem,
    errInvalidParam,
    errInvalidData,
    errCantCreateDb,
    errCantOpenDb,
    errDbIsCorrupted,
    errNoDbOpened,
    errNoQueryInProgress,
    errQueryHasNoResultSet,
    errNoTransactionsInProgress,
    errTooManyTransactions,
    errCannotExecQuery,
    errNoRowWasFetched,
    errEndOfResultSetReached,
    errInvalidDataType,
    errNullValue,
    errOverflow
  } eErrorCode;

  CNktSQLiteConnector();
  ~CNktSQLiteConnector();

  eErrorCode Create(LPCSTR szFileNameA);
  eErrorCode Create(LPCWSTR szFileNameW);
  eErrorCode CreateMemoryDB();
  eErrorCode Open(LPCSTR szFileNameA, BOOL bReadOnly=FALSE);
  eErrorCode Open(LPCWSTR szFileNameW, BOOL bReadOnly=FALSE);
  VOID Close();

  eErrorCode QueryExecute(ULONG nQueryId, LPCSTR szQueryA);
  eErrorCode QueryExecute(ULONG nQueryId, LPCWSTR szQueryW);

  ULONGLONG QueryGetAffectedRows(ULONG nQueryId);
  eErrorCode QueryFetchNextRow(ULONG nQueryId);

  ULONGLONG QueryGetAutoIncrementValue(ULONG nQueryId);
  ULONG QueryGetFieldsCount(ULONG nQueryId);

  eErrorCode QueryGetFieldName(LPSTR *lplpStringA, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldName(LPWSTR *lplpStringW, ULONG nQueryId, ULONG nColumnIndex);

  ULONG QueryGetColumnIndex(ULONG nQueryId, LPCSTR szNameA);
  ULONG QueryGetColumnIndex(ULONG nQueryId, LPCWSTR szNameW);

  eErrorCode QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, ULONG nColumnIndex);

  eErrorCode QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, ULONG nColumnIndex, ULONGLONG nStartPos=0, ULONGLONG nLength=0xFFFFFFFFFFFFFFFFui64);
  eErrorCode QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, ULONG nColumnIndex);

  eErrorCode QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex);
  eErrorCode QueryGetFieldValue(double *lpnValue, ULONG nQueryId, ULONG nColumnIndex);

  eErrorCode QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, LPCWSTR szColumnNameW);

  eErrorCode QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, LPCSTR szColumnNameA, ULONGLONG nStartPos=0, ULONGLONG nLength=0xFFFFFFFFFFFFFFFFui64);
  eErrorCode QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, LPCWSTR szColumnNameW, ULONGLONG nStartPos=0, ULONGLONG nLength=0xFFFFFFFFFFFFFFFFui64);
  eErrorCode QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW);
  eErrorCode QueryGetFieldValue(double *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA);
  eErrorCode QueryGetFieldValue(double *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW);

  eErrorCode QueryClose(ULONG nQueryId);

  eErrorCode TransactionStart();
  eErrorCode TransactionCommit();
  eErrorCode TransactionRollback();

private:
  LPVOID lpOpaqueData;
};

//-----------------------------------------------------------

