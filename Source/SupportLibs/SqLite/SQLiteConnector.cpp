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

#include "SQLiteConnector.h"
#include "sqlite3.h"
#include <stdio.h>
#include <crtdbg.h>

//-----------------------------------------------------------

#ifdef _DEBUG
  #undef SL_THIS_FILE
  static char SL_THIS_FILE[] = __FILE__;
#endif //_DEBUG

//-----------------------------------------------------------

#define MAX_QUERY_COUNT                                    3

#define QUERY_FLAGS_ROW_RETRIEVED                     0x0001
#define QUERY_FLAGS_FIRST_ROW                         0x0002
#define QUERY_FLAGS_EOF_REACHED                       0x0004
#define QUERY_FLAGS_NO_RESULT_SET                     0x0008

#define SQLITE_CALCULATED_FIELD                      1000000

//-----------------------------------------------------------

typedef struct {
  LPSTR szNameA;
  LPWSTR szNameW;
  LONG nType, nCurrType;
  LPSTR szDataA;
  LPWSTR szDataW;
  BOOL bWideStrIsValid;
  size_t nDataSizeW;
} MY_SQLITE_FIELD;

typedef struct {
  LPSTR szFileNameUtf8;
  sqlite3 *lpDB;
  ULONG nTransactionCounter;
  struct tagQueries {
    sqlite3_stmt *lpStmt;
    ULONGLONG nLastInsertId;
    ULONGLONG nAffectedRows;
    ULONG nFlags;
    ULONG nFieldsCount;
    MY_SQLITE_FIELD *lpFields;
  } sQueries[MAX_QUERY_COUNT];
} MY_SQLITE_DATA;

#define db_data ((MY_SQLITE_DATA*)lpOpaqueData)

//-----------------------------------------------------------

static const BYTE aUtf8FirstByteMark[4] = {0x00, 0xC0, 0xE0, 0xF0};

//-----------------------------------------------------------

static CNktSQLiteConnector::eErrorCode Ansi2Wide(LPWSTR *lpszDestW, LPCSTR szSrcA);
static CNktSQLiteConnector::eErrorCode Wide2Ansi(LPSTR *lpszDestA, LPCWSTR szSrcW);
static CNktSQLiteConnector::eErrorCode Wide2Utf8(LPSTR *lpszDestA, LPCWSTR szSrcW);
static CNktSQLiteConnector::eErrorCode DoOpenDatabase(LPCWSTR szFileNameW, int flags, MY_SQLITE_DATA **lplpData);
static CNktSQLiteConnector::eErrorCode DoQueryGetColumnIndex(ULONG *lpIndex, MY_SQLITE_DATA *lpData, ULONG nQueryId, LPCSTR szNameA);
static CNktSQLiteConnector::eErrorCode DoQueryGetColumnIndex(ULONG *lpIndex, MY_SQLITE_DATA *lpData, ULONG nQueryId, LPCWSTR szNameW);
static LONG GetTypeFromName(LPCSTR szNameA);
static CNktSQLiteConnector::eErrorCode Wide2LongLong(LONGLONG *lpnValue, LPCWSTR sW);
static CNktSQLiteConnector::eErrorCode Wide2ULongLong(ULONGLONG *lpnValue, LPCWSTR sW);

//-----------------------------------------------------------

CNktSQLiteConnector::CNktSQLiteConnector()
{
  lpOpaqueData = NULL;
  return;
}

CNktSQLiteConnector::~CNktSQLiteConnector()
{
  Close();
  return;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::Create(LPCSTR szFileNameA)
{
  LPWSTR szFileNameW;
  eErrorCode nErr;

  if (Ansi2Wide(&szFileNameW, szFileNameA) != CNktSQLiteConnector::noError)
    return CNktSQLiteConnector::errNoMem;
  nErr = Create(szFileNameW);
  free(szFileNameW);
  return nErr;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::Create(LPCWSTR szFileNameW)
{
  eErrorCode nErr;
  Close();
  nErr = DoOpenDatabase(szFileNameW, SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE|
                        SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, (MY_SQLITE_DATA**)&lpOpaqueData);
  if (nErr == CNktSQLiteConnector::errCantOpenDb)
    nErr = CNktSQLiteConnector::errCantCreateDb;
  return nErr;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::Open(LPCSTR szFileNameA, BOOL bReadOnly)
{
  LPWSTR szFileNameW;
  CNktSQLiteConnector::eErrorCode nErr;

  if (Ansi2Wide(&szFileNameW, szFileNameA) != CNktSQLiteConnector::noError)
    return CNktSQLiteConnector::errNoMem;
  nErr = Open(szFileNameW, bReadOnly);
  free(szFileNameW);
  return nErr;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::CreateMemoryDB()
{
  return Create(L":memory:");
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::Open(LPCWSTR szFileNameW, BOOL bReadOnly)
{
  int flags;

  Close();
  flags = SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_SHAREDCACHE;
  flags |= (bReadOnly == FALSE) ? SQLITE_OPEN_READWRITE : SQLITE_OPEN_READONLY;
  return DoOpenDatabase(szFileNameW, flags, (MY_SQLITE_DATA**)&lpOpaqueData);
}

VOID CNktSQLiteConnector::Close()
{
  ULONG i, nRetry;

  if (lpOpaqueData != NULL) {
    for (i=0; i<MAX_QUERY_COUNT; i++)
      QueryClose(i);
    nRetry = 0;
    while (nRetry<20 && db_data->nTransactionCounter>0) {
      if (TransactionRollback() != CNktSQLiteConnector::noError)
        nRetry++;
    }
    if (db_data->lpDB != NULL)
      sqlite3_close(db_data->lpDB);
    if (db_data->szFileNameUtf8 != NULL)
      free(db_data->szFileNameUtf8);
    free(lpOpaqueData);
    lpOpaqueData = NULL;
  }
  return;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryExecute(ULONG nQueryId, LPCSTR szQueryA)
{
  LPWSTR szQueryW;
  CNktSQLiteConnector::eErrorCode nErr;

  if (Ansi2Wide(&szQueryW, szQueryA) != CNktSQLiteConnector::noError)
    return CNktSQLiteConnector::errNoMem;
  nErr = QueryExecute(nQueryId, szQueryW);
  free(szQueryW);
  return nErr;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryExecute(ULONG nQueryId, LPCWSTR szQueryW)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;
  const char *szLeftover;
  LPSTR szQueryUtf8, szA;
  LPWSTR szW;
  int res;
  ULONG ulCol;
  size_t nLen;

  if (szQueryW==NULL || szQueryW[0]==0 ||
      nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  nErr = Wide2Utf8(&szQueryUtf8, szQueryW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  QueryClose(nQueryId);
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  //prepare sql
  res = sqlite3_prepare_v2(db_data->lpDB, szQueryUtf8, -1, &(lpQuery->lpStmt), &szLeftover);
  free(szQueryUtf8);
  if (res != SQLITE_OK) {
    lpQuery->lpStmt = NULL;
    return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
  }
  //execute query and optionally get first row
  switch (res = sqlite3_step(lpQuery->lpStmt)) {
    case SQLITE_ROW:
      lpQuery->nFlags |= QUERY_FLAGS_FIRST_ROW;
      break;
    case SQLITE_DONE:
    case SQLITE_OK:
      lpQuery->nFlags |= QUERY_FLAGS_NO_RESULT_SET;
      lpQuery->nLastInsertId = (ULONGLONG)sqlite3_last_insert_rowid(db_data->lpDB);
      lpQuery->nAffectedRows = (ULONGLONG)sqlite3_changes(db_data->lpDB);
      break;
    default:
      sqlite3_finalize(lpQuery->lpStmt);
      memset(lpQuery, 0, sizeof(MY_SQLITE_DATA::tagQueries));
      return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
  }
  //get fields count
  if ((lpQuery->nFlags & QUERY_FLAGS_FIRST_ROW) != 0) {
    lpQuery->nFieldsCount = (ULONG)sqlite3_column_count(lpQuery->lpStmt);
    if (lpQuery->nFieldsCount > 0) {
      //process fields
      lpField = lpQuery->lpFields = (MY_SQLITE_FIELD*)malloc((size_t)(lpQuery->nFieldsCount) * sizeof(MY_SQLITE_FIELD));
      if (lpQuery->lpFields == NULL) {
        sqlite3_finalize(lpQuery->lpStmt);
        memset(lpQuery, 0, sizeof(MY_SQLITE_DATA::tagQueries));
        return CNktSQLiteConnector::errNoMem;
      }
      memset(lpQuery->lpFields, 0, (size_t)(lpQuery->nFieldsCount) * sizeof(MY_SQLITE_FIELD));
      //process each field
      for (ulCol=0; ulCol<lpQuery->nFieldsCount; ulCol++,lpField++) {
        //get column type
        szA = (LPSTR)sqlite3_column_decltype(lpQuery->lpStmt, (int)ulCol);
        lpField->nType = (szA != NULL) ? GetTypeFromName(szA) : SQLITE_CALCULATED_FIELD;
        if (lpField->nType < 0)
          lpField->nType = SQLITE_NULL; //if a field is unsupported treat it like NULL
        //get column name
        szW = (LPWSTR)sqlite3_column_name16(lpQuery->lpStmt, (int)ulCol);
        if (szW == NULL)
          szW = L"";
        nLen = wcslen(szW) + 1;
        lpField->szNameW = (LPWSTR)malloc(nLen*sizeof(WCHAR));
        if (lpField->szNameW == NULL) {
          for (ulCol=0; ulCol<lpQuery->nFieldsCount; ulCol++) {
            if (lpQuery->lpFields[ulCol].szNameW != NULL)
              free(lpQuery->lpFields[ulCol].szNameW);
          }
          free(lpQuery->lpFields);
          sqlite3_finalize(lpQuery->lpStmt);
          memset(lpQuery, 0, sizeof(MY_SQLITE_DATA::tagQueries));
          return CNktSQLiteConnector::errNoMem;
        }
        memcpy(lpField->szNameW, szW, nLen*sizeof(WCHAR));
        //get retrieved field row type
        lpField->nCurrType = (lpField->nType >= 0) ? (LONG)sqlite3_column_type(lpQuery->lpStmt, (int)ulCol) : SQLITE_NULL;
      }
    }
  }
  return CNktSQLiteConnector::noError;
}

ULONGLONG CNktSQLiteConnector::QueryGetAffectedRows(ULONG nQueryId)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;

  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT ||
      lpOpaqueData == NULL)
    return 0ui64;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  return (lpQuery->lpStmt != NULL) ? (lpQuery->nAffectedRows) : 0ui64;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryFetchNextRow(ULONG nQueryId)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  int res;
  ULONG ulCol;

  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  if ((lpQuery->nFlags & QUERY_FLAGS_FIRST_ROW) != 0) {
    //first row is retrieved when executing the statement
    //so, on the first QueryFetchNextRow do a no-op
    lpQuery->nFlags &= ~QUERY_FLAGS_FIRST_ROW;
    lpQuery->nFlags |= QUERY_FLAGS_ROW_RETRIEVED;
    (lpQuery->nAffectedRows)++;
  }
  else {
    lpQuery->nFlags &= ~QUERY_FLAGS_ROW_RETRIEVED;
    switch (res = sqlite3_step(lpQuery->lpStmt)) {
      case SQLITE_ROW:
        lpQuery->nFlags |= QUERY_FLAGS_ROW_RETRIEVED;
        (lpQuery->nAffectedRows)++;
        //get retrieved field row type
        lpField = lpQuery->lpFields;
        for (ulCol=0; ulCol<lpQuery->nFieldsCount; ulCol++,lpField++) {
          if (lpField->nType >= 0)
            lpField->nCurrType = (LONG)sqlite3_column_type(lpQuery->lpStmt, (int)ulCol);
        }
        break;
      case SQLITE_DONE:
      case SQLITE_OK:
        lpQuery->nFlags |= QUERY_FLAGS_EOF_REACHED;
        return CNktSQLiteConnector::errEndOfResultSetReached;

      default:
        return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
    }
  }
  lpField = lpQuery->lpFields;
  for (ulCol=0; ulCol<lpQuery->nFieldsCount; ulCol++,lpField++) {
    lpField->bWideStrIsValid = FALSE;
    if (lpField->szDataA != NULL) {
      free(lpField->szDataA);
      lpField->szDataA = NULL;
    }
  }
  return CNktSQLiteConnector::noError;
}

ULONGLONG CNktSQLiteConnector::QueryGetAutoIncrementValue(ULONG nQueryId)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;

  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT ||
      lpOpaqueData == NULL)
    return 0ui64;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  return (lpQuery->lpStmt != NULL) ? (lpQuery->nLastInsertId) : 0ui64;
}

ULONG CNktSQLiteConnector::QueryGetFieldsCount(ULONG nQueryId)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;

  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT ||
      lpOpaqueData == NULL)
    return 0UL;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  return (lpQuery->lpStmt != NULL) ? (lpQuery->nFieldsCount) : 0UL;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldName(LPSTR *lplpStringA, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;

  if (lplpStringA == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lplpStringA = NULL;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  if (lpField->szNameA == NULL) {
    nErr = Wide2Ansi(&(lpField->szNameA), lpField->szNameW);
    if (nErr != CNktSQLiteConnector::noError)
      return nErr;
  }
  *lplpStringA = lpField->szNameA;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldName(LPWSTR *lplpStringW, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;

  if (lplpStringW == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lplpStringW = NULL;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  *lplpStringW = lpField->szNameW;
  return CNktSQLiteConnector::noError;
}

ULONG CNktSQLiteConnector::QueryGetColumnIndex(ULONG nQueryId, LPCSTR szNameA)
{
  ULONG ulColIndex;

  DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szNameA);
  return ulColIndex;
}

ULONG CNktSQLiteConnector::QueryGetColumnIndex(ULONG nQueryId, LPCWSTR szNameW)
{
  ULONG ulColIndex;

  DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szNameW);
  return ulColIndex;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, ULONG nColumnIndex)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONGLONG ullLen;

  if (lpnLength == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnLength = 0UL;
  nErr = QueryGetFieldLength(&ullLen, nQueryId, nColumnIndex);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  if (ullLen > 0xFFFFFFFFui64)
    return CNktSQLiteConnector::errOverflow;
  *lpnLength = (ULONG)ullLen;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;
  ULONGLONG nLen;
  LPSTR sA;
  LPWSTR sW;

  if (lpnLength == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnLength = 0ui64;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  //----
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  switch (lpField->nType) {
    case SQLITE_CALCULATED_FIELD:
    case SQLITE_INTEGER:
    case SQLITE_FLOAT:
    case SQLITE_TEXT:
      if (lpField->szDataA == NULL) {
        nErr = QueryGetFieldValue(&sA, nQueryId, nColumnIndex);
        if (nErr != CNktSQLiteConnector::noError)
          return nErr;
      }
      nLen = (ULONGLONG)strlen(lpField->szDataA);
      if (lpField->szDataW == NULL) {
        nErr = QueryGetFieldValue(&sW, nQueryId, nColumnIndex);
        if (nErr != CNktSQLiteConnector::noError)
          return nErr;
      }
      *lpnLength = (ULONGLONG)wcslen(lpField->szDataW);
      if (*lpnLength < nLen)
        *lpnLength = nLen;
      return CNktSQLiteConnector::noError;
    case SQLITE_BLOB:
      *lpnLength = (ULONGLONG)(LONGLONG)sqlite3_column_bytes(lpQuery->lpStmt, (int)(nColumnIndex-1));
      return CNktSQLiteConnector::noError;
  }
  return CNktSQLiteConnector::errNullValue;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, ULONG nColumnIndex, ULONGLONG nStartPos, ULONGLONG nLength)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  LPBYTE lpData;
  ULONGLONG nDataLen;

  if (lpDestRawData == NULL ||
      nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  switch (lpField->nType) {
    case SQLITE_NULL:
      return CNktSQLiteConnector::errNullValue;
    case SQLITE_BLOB:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
      break;
    default:
      return CNktSQLiteConnector::errInvalidDataType;
  }
  nDataLen = (ULONGLONG)(LONGLONG)sqlite3_column_bytes(lpQuery->lpStmt, (int)(nColumnIndex-1));
  lpData = (LPBYTE)sqlite3_column_blob(lpQuery->lpStmt, (int)(nColumnIndex-1));
  if (lpData == NULL)
    return CNktSQLiteConnector::errNullValue;

  if (nStartPos > nDataLen) {
    nStartPos = nDataLen;
    nLength = 0;
  }
  if (nStartPos+nLength > nDataLen)
    nLength = nDataLen - nStartPos;
  if (nLength > 0)
    memcpy(lpDestRawData, lpData+(SIZE_T)nStartPos, (SIZE_T)nLength);
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;
  LPWSTR sW;

  if (lplpStringA == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lplpStringA = NULL;
  nErr = QueryGetFieldValue(&sW, nQueryId, nColumnIndex);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  if (lpField->szDataA == NULL) {
    nErr = Wide2Ansi(&(lpField->szDataA), sW);
    if (nErr != CNktSQLiteConnector::noError)
      return nErr;
  }
  *lplpStringA = lpField->szDataA;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  union {
    LONGLONG llVal;
    double dblVal;
    LPWSTR sW;
  };
  size_t nLen;

  if (lplpStringW == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lplpStringW = NULL;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  //----
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  if (lpField->bWideStrIsValid == FALSE) {
    switch (lpField->nType) {
      case SQLITE_NULL:
        return CNktSQLiteConnector::errNullValue;

      case SQLITE_CALCULATED_FIELD:
        if (lpField->nCurrType == SQLITE_INTEGER)
          goto qgfv_case_integer;
        if (lpField->nCurrType == SQLITE_FLOAT)
          goto qgfv_case_float;
        //by default fall into SQLITE_TEXT

      case SQLITE_TEXT:
        if (lpField->nCurrType == SQLITE_NULL)
          return CNktSQLiteConnector::errNullValue;
        sW = (LPWSTR)sqlite3_column_text16(lpQuery->lpStmt, (int)(nColumnIndex-1));
        if (sW == NULL)
          return CNktSQLiteConnector::errNoMem;
        nLen = wcslen(sW) + 1;
        if (lpField->nDataSizeW < nLen) {
          if (lpField->szDataW != NULL) {
            free(lpField->szDataW);
            lpField->szDataW = NULL;
          }
          lpField->nDataSizeW = 0;
          lpField->szDataW = (LPWSTR)malloc(nLen*sizeof(WCHAR));
          if (lpField->szDataW == NULL)
            return CNktSQLiteConnector::errNoMem;
          lpField->nDataSizeW = nLen;
        }
        wcscpy_s(lpField->szDataW, lpField->nDataSizeW, sW);
        break;

      case SQLITE_INTEGER:
        if (lpField->nCurrType == SQLITE_NULL)
          return CNktSQLiteConnector::errNullValue;
qgfv_case_integer:
        llVal = sqlite3_column_int64(lpQuery->lpStmt, (int)(nColumnIndex-1));
        if (lpField->nDataSizeW < 24) {
          if (lpField->szDataW != NULL) {
            free(lpField->szDataW);
            lpField->szDataW = NULL;
          }
          lpField->nDataSizeW = 0;
          lpField->szDataW = (LPWSTR)malloc(24*sizeof(WCHAR));
          if (lpField->szDataW == NULL)
            return CNktSQLiteConnector::errNoMem;
          lpField->nDataSizeW = 24;
        }
        swprintf_s(lpField->szDataW, lpField->nDataSizeW, L"%I64d", llVal);
        break;

      case SQLITE_FLOAT:
        if (lpField->nCurrType == SQLITE_NULL)
          return CNktSQLiteConnector::errNullValue;
qgfv_case_float:
        dblVal = sqlite3_column_double(lpQuery->lpStmt, (int)(nColumnIndex-1));
        if (lpField->nDataSizeW < 336) {
          if (lpField->szDataW != NULL) {
            free(lpField->szDataW);
            lpField->szDataW = NULL;
          }
          lpField->nDataSizeW = 0;
          lpField->szDataW = (LPWSTR)malloc(336*sizeof(WCHAR));
          if (lpField->szDataW == NULL)
            return CNktSQLiteConnector::errNoMem;
          lpField->nDataSizeW = 336;
        }
        swprintf_s(lpField->szDataW, lpField->nDataSizeW, L"%.15lf", dblVal);
        break;

      default:
        return CNktSQLiteConnector::errInvalidDataType;
    }
    lpField->bWideStrIsValid = TRUE;
  }
  *lplpStringW = lpField->szDataW;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex)
{
  CNktSQLiteConnector::eErrorCode nErr;
  LONGLONG llVal;

  if (lpnValue == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnValue = 0L;
  nErr = QueryGetFieldValue(&llVal, nQueryId, nColumnIndex);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  if (llVal < -0x80000000i64 || llVal > 0x7FFFFFFFi64)
    return CNktSQLiteConnector::errOverflow;
  *lpnValue = (LONG)llVal;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONGLONG ullVal;

  if (lpnValue == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnValue = 0UL;
  nErr = QueryGetFieldValue(&ullVal, nQueryId, nColumnIndex);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  if (ullVal > 0xFFFFFFFFi64)
    return CNktSQLiteConnector::errOverflow;
  *lpnValue = (ULONG)ullVal;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;
  union {
    LONGLONG llVal;
    double dblVal;
    LPWSTR sW;
  };

  if (lpnValue == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnValue = 0i64;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  //----
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  switch (lpField->nType) {
    case SQLITE_NULL:
      return CNktSQLiteConnector::errNullValue;

    case SQLITE_CALCULATED_FIELD:
      if (lpField->nCurrType == SQLITE_INTEGER)
        goto qgfv_case_integer;
      if (lpField->nCurrType == SQLITE_FLOAT)
        goto qgfv_case_float;
      //by default fall into SQLITE_TEXT

    case SQLITE_TEXT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
      sW = (LPWSTR)sqlite3_column_text16(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (sW == NULL)
        return CNktSQLiteConnector::errNoMem;
      nErr = Wide2LongLong(lpnValue, sW);
      if (nErr != CNktSQLiteConnector::noError)
        return nErr;
      break;

    case SQLITE_INTEGER:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_integer:
      llVal = sqlite3_column_int64(lpQuery->lpStmt, (int)(nColumnIndex-1));
      *lpnValue = llVal;
      break;

    case SQLITE_FLOAT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_float:
      dblVal = sqlite3_column_double(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (dblVal < (double)LLONG_MIN || dblVal > (double)LLONG_MAX)
        return CNktSQLiteConnector::errOverflow;
      *lpnValue = (LONGLONG)dblVal;
      break;

    default:
      return CNktSQLiteConnector::errInvalidDataType;
  }
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  CNktSQLiteConnector::eErrorCode nErr;
  union {
    LONGLONG llVal;
    double dblVal;
    LPWSTR sW;
  };

  if (lpnValue == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnValue = 0ui64;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  //----
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  switch (lpField->nType) {
    case SQLITE_NULL:
      return CNktSQLiteConnector::errNullValue;

    case SQLITE_CALCULATED_FIELD:
      if (lpField->nCurrType == SQLITE_INTEGER)
        goto qgfv_case_integer;
      if (lpField->nCurrType == SQLITE_FLOAT)
        goto qgfv_case_float;
      //by default fall into SQLITE_TEXT

    case SQLITE_TEXT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
      sW = (LPWSTR)sqlite3_column_text16(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (sW == NULL)
        return CNktSQLiteConnector::errNoMem;
      nErr = Wide2ULongLong(lpnValue, sW);
      if (nErr != CNktSQLiteConnector::noError)
        return nErr;
      break;

    case SQLITE_INTEGER:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_integer:
      llVal = sqlite3_column_int64(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (llVal < 0i64)
        return CNktSQLiteConnector::errOverflow;
      *lpnValue = (ULONGLONG)llVal;
      break;

    case SQLITE_FLOAT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_float:
      dblVal = sqlite3_column_double(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (dblVal < 0 || dblVal > (double)ULLONG_MAX)
        return CNktSQLiteConnector::errOverflow;
      *lpnValue = (ULONGLONG)dblVal;
      break;

    default:
      return CNktSQLiteConnector::errInvalidDataType;
  }
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(double *lpnValue, ULONG nQueryId, ULONG nColumnIndex)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  MY_SQLITE_FIELD *lpField;
  union {
    LONGLONG llVal;
    LPWSTR sW;
  };

  if (lpnValue == NULL)
    return CNktSQLiteConnector::errInvalidParam;
  *lpnValue = 0i64;
  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  if (nColumnIndex<1 || nColumnIndex>lpQuery->nFieldsCount)
    return CNktSQLiteConnector::errInvalidParam;
  if ((lpQuery->nFlags & QUERY_FLAGS_ROW_RETRIEVED) == 0)
    return CNktSQLiteConnector::errNoRowWasFetched;
  if ((lpQuery->nFlags & QUERY_FLAGS_EOF_REACHED) != 0)
    return CNktSQLiteConnector::errEndOfResultSetReached;
  //----
  lpField = lpQuery->lpFields + (nColumnIndex-1);
  switch (lpField->nType) {
    case SQLITE_NULL:
      return CNktSQLiteConnector::errNullValue;

    case SQLITE_CALCULATED_FIELD:
      if (lpField->nCurrType == SQLITE_INTEGER)
        goto qgfv_case_integer;
      if (lpField->nCurrType == SQLITE_FLOAT)
        goto qgfv_case_float;
      //by default fall into SQLITE_TEXT

    case SQLITE_TEXT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
      sW = (LPWSTR)sqlite3_column_text16(lpQuery->lpStmt, (int)(nColumnIndex-1));
      if (sW == NULL)
        return CNktSQLiteConnector::errNoMem;
      *lpnValue = _wtof(sW);
      break;

    case SQLITE_INTEGER:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_integer:
      llVal = sqlite3_column_int64(lpQuery->lpStmt, (int)(nColumnIndex-1));
      *lpnValue = (double)llVal;
      break;

    case SQLITE_FLOAT:
      if (lpField->nCurrType == SQLITE_NULL)
        return CNktSQLiteConnector::errNullValue;
qgfv_case_float:
      *lpnValue = sqlite3_column_double(lpQuery->lpStmt, (int)(nColumnIndex-1));
      break;

    default:
      return CNktSQLiteConnector::errInvalidDataType;
  }
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnLength != NULL)
    *lpnLength = 0UL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldLength(lpnLength, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONG *lpnLength, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnLength != NULL)
    *lpnLength = 0UL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldLength(lpnLength, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnLength != NULL)
    *lpnLength = 0ui64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldLength(lpnLength, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldLength(ULONGLONG *lpnLength, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnLength != NULL)
    *lpnLength = 0ui64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldLength(lpnLength, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, LPCSTR szColumnNameA, ULONGLONG nStartPos, ULONGLONG nLength)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldBlobValue(lpDestRawData, nQueryId, ulColIndex, nStartPos, nLength);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldBlobValue(LPVOID lpDestRawData, ULONG nQueryId, LPCWSTR szColumnNameW, ULONGLONG nStartPos, ULONGLONG nLength)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldBlobValue(lpDestRawData, nQueryId, ulColIndex, nStartPos, nLength);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lplpStringA != NULL)
    *lplpStringA = NULL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lplpStringA, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPSTR *lplpStringA, ULONG nQueryId, LPCWSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lplpStringA != NULL)
    *lplpStringA = NULL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lplpStringA, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lplpStringW != NULL)
    *lplpStringW = NULL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lplpStringW, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LPWSTR *lplpStringW, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lplpStringW != NULL)
    *lplpStringW = NULL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lplpStringW, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0L;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0L;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0UL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0UL;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0i64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(LONGLONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0i64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0ui64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(ULONGLONG *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0ui64;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(double *lpnValue, ULONG nQueryId, LPCSTR szColumnNameA)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0.0;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameA);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryGetFieldValue(double *lpnValue, ULONG nQueryId, LPCWSTR szColumnNameW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONG ulColIndex;

  if (lpnValue != NULL)
    *lpnValue = 0.0;
  nErr = DoQueryGetColumnIndex(&ulColIndex, db_data, nQueryId, szColumnNameW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  return QueryGetFieldValue(lpnValue, nQueryId, ulColIndex);
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::QueryClose(ULONG nQueryId)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  ULONG i;

  if (nQueryId<1 || nQueryId>MAX_QUERY_COUNT)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = &(db_data->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  sqlite3_finalize(lpQuery->lpStmt);
  if (lpQuery->lpFields != NULL) {
    for (i=0; i<lpQuery->nFieldsCount; i++) {
      if (lpQuery->lpFields[i].szNameA != NULL)
        free(lpQuery->lpFields[i].szNameA);
      if (lpQuery->lpFields[i].szNameW != NULL)
        free(lpQuery->lpFields[i].szNameW);
      if (lpQuery->lpFields[i].szDataA != NULL)
        free(lpQuery->lpFields[i].szDataA);
      if (lpQuery->lpFields[i].szDataW != NULL)
        free(lpQuery->lpFields[i].szDataW);
    }
    free(lpQuery->lpFields);
  }
  memset(lpQuery, 0, sizeof(MY_SQLITE_DATA::tagQueries));
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::TransactionStart()
{
  sqlite3_stmt *lpStmt;
  const char *szLeftover;
  int res;

  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  if (db_data->nTransactionCounter > 1024)
    return CNktSQLiteConnector::errTooManyTransactions;
  //----
  lpStmt = NULL;
  res = sqlite3_prepare_v2(db_data->lpDB, "BEGIN TRANSACTION", -1, &lpStmt, &szLeftover);
  if (res != SQLITE_OK)
    return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
  sqlite3_step(lpStmt);
  sqlite3_finalize(lpStmt);
  (db_data->nTransactionCounter)++;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::TransactionCommit()
{
  sqlite3_stmt *lpStmt;
  const char *szLeftover;
  int res;

  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  if (db_data->nTransactionCounter == 0)
    return CNktSQLiteConnector::errNoTransactionsInProgress;
  //----
  lpStmt = NULL;
  res = sqlite3_prepare_v2(db_data->lpDB, "COMMIT TRANSACTION", -1, &lpStmt, &szLeftover);
  if (res != SQLITE_OK)
    return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
  sqlite3_step(lpStmt);
  sqlite3_finalize(lpStmt);
  (db_data->nTransactionCounter)--;
  return CNktSQLiteConnector::noError;
}

CNktSQLiteConnector::eErrorCode CNktSQLiteConnector::TransactionRollback()
{
  sqlite3_stmt *lpStmt;
  const char *szLeftover;
  int res;

  if (lpOpaqueData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  if (db_data->nTransactionCounter == 0)
    return CNktSQLiteConnector::errNoTransactionsInProgress;
  //----
  lpStmt = NULL;
  res = sqlite3_prepare_v2(db_data->lpDB, "ROLLBACK TRANSACTION", -1, &lpStmt, &szLeftover);
  if (res != SQLITE_OK)
    return (res == SQLITE_NOMEM) ? CNktSQLiteConnector::errNoMem : CNktSQLiteConnector::errCannotExecQuery;
  sqlite3_step(lpStmt);
  sqlite3_finalize(lpStmt);
  (db_data->nTransactionCounter)--;
  return CNktSQLiteConnector::noError;
}

//-----------------------------------------------------------

static CNktSQLiteConnector::eErrorCode Ansi2Wide(LPWSTR *lpszDestW, LPCSTR szSrcA)
{
  size_t nSrcLen, nDestLen;

  _ASSERT(lpszDestW != NULL);
  if (szSrcA == NULL)
    szSrcA = "";
  nSrcLen = strlen(szSrcA);
  nDestLen = (size_t)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, NULL, 0);
  *lpszDestW = (LPWSTR)malloc((nDestLen+1)*sizeof(WCHAR));
  if (*lpszDestW == NULL)
    return CNktSQLiteConnector::errNoMem;
  nDestLen = (size_t)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, *lpszDestW, (int)(nDestLen+1));
  (*lpszDestW)[nDestLen] = 0;
  return CNktSQLiteConnector::noError;
}

static CNktSQLiteConnector::eErrorCode Wide2Ansi(LPSTR *lpszDestA, LPCWSTR szSrcW)
{
  size_t nSrcLen, nDestLen;

  _ASSERT(lpszDestA != NULL);
  if (szSrcW == NULL)
    szSrcW = L"";
  nSrcLen = wcslen(szSrcW);
  nDestLen = (size_t)::WideCharToMultiByte(CP_ACP, 0, szSrcW, (int)nSrcLen, NULL, 0, NULL, NULL);
  *lpszDestA = (LPSTR)malloc((nDestLen+1));
  if (*lpszDestA == NULL)
    return CNktSQLiteConnector::errNoMem;
  nDestLen = (size_t)::WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, szSrcW, (int)nSrcLen, *lpszDestA, (int)(nDestLen+1), NULL, NULL);
  (*lpszDestA)[nDestLen] = 0;
  return CNktSQLiteConnector::noError;
}

static CNktSQLiteConnector::eErrorCode Wide2Utf8(LPSTR *lpszDestA, LPCWSTR szSrcW)
{
  size_t nPass, i, k, nSrcLen, nDestLen;
  DWORD dwEncVal;

  if (szSrcW == NULL)
    szSrcW = L"";
  nSrcLen = wcslen(szSrcW);
  for (nPass=1; nPass<=2; nPass++) {
    if (nPass == 2) {
      *lpszDestA = (LPSTR)malloc(nDestLen+1);
      if (*lpszDestA == NULL)
        return CNktSQLiteConnector::errNoMem;
    }
    for (i=nDestLen=0; i<nSrcLen; i++) {
      dwEncVal = (DWORD)(szSrcW[i]);
      if (szSrcW[i]>=0xD800 && szSrcW[i]<=0xDBFF) {
        if (szSrcW[i+1]<0xDC00 || szSrcW[i+1]>0xDFFF)
          return CNktSQLiteConnector::errInvalidData;
        dwEncVal = (dwEncVal - 0xD800UL) << 10;
        dwEncVal += 0x10000;
        dwEncVal = (DWORD)(szSrcW[i+1] - 0xDC00);
        _ASSERT(dwEncVal <= 0x10FFFF);
      }
      if (dwEncVal < 0x80)
        k = 1;
      else if (dwEncVal < 0x800)
        k = 2;
      else if (dwEncVal < 0x10000)
        k = 3;
      else
        k = 4;
      if (nPass == 2) {
        switch (k) {
          case 4:
            (*lpszDestA)[nDestLen+3] = (BYTE)((dwEncVal | 0x80) & 0xBF);
            dwEncVal >>= 6;
          case 3:
            (*lpszDestA)[nDestLen+2] = (BYTE)((dwEncVal | 0x80) & 0xBF);
            dwEncVal >>= 6;
          case 2:
            (*lpszDestA)[nDestLen+1] = (BYTE)((dwEncVal | 0x80) & 0xBF);
            dwEncVal >>= 6;
          case 1:
            (*lpszDestA)[nDestLen+0] =  (BYTE)dwEncVal | aUtf8FirstByteMark[k-1];
            break;
        }
      }
      nDestLen += k;
    }
  }
  (*lpszDestA)[nDestLen] = 0;
  return CNktSQLiteConnector::noError;
}

static CNktSQLiteConnector::eErrorCode DoOpenDatabase(LPCWSTR szFileNameW, int flags, MY_SQLITE_DATA **lplpData)
{
  CNktSQLiteConnector::eErrorCode nErr;
  int res;

  _ASSERT(lplpData != NULL);
  if (szFileNameW == NULL || szFileNameW[0]==0)
    return CNktSQLiteConnector::errInvalidParam;
  //----
  *lplpData = (MY_SQLITE_DATA*)malloc(sizeof(MY_SQLITE_DATA));
  if (*lplpData == NULL)
    return CNktSQLiteConnector::errNoMem;
  memset(*lplpData, 0, sizeof(MY_SQLITE_DATA));
  nErr = Wide2Utf8(&((*lplpData)->szFileNameUtf8), szFileNameW);
  if (nErr != CNktSQLiteConnector::noError) {
    free(*lplpData);
    *lplpData = NULL;
    return nErr;
  }
  res = sqlite3_open_v2((*lplpData)->szFileNameUtf8, &((*lplpData)->lpDB), flags, NULL);
  if (res != SQLITE_OK) {
    switch (res) {
      case SQLITE_NOMEM:
        nErr = CNktSQLiteConnector::errNoMem;
        break;
      case SQLITE_CORRUPT:
      case SQLITE_PROTOCOL:
      case SQLITE_MISMATCH:
        nErr = CNktSQLiteConnector::errDbIsCorrupted;
        break;
      default:
        nErr = CNktSQLiteConnector::errCantOpenDb;
        break;
    }
    free((*lplpData)->szFileNameUtf8);
    free(*lplpData);
    *lplpData = NULL;
    return nErr;
  }
  return CNktSQLiteConnector::noError;
}

static CNktSQLiteConnector::eErrorCode DoQueryGetColumnIndex(ULONG *lpIndex, MY_SQLITE_DATA *lpData, ULONG nQueryId, LPCSTR szNameA)
{
  LPWSTR szNameW;
  CNktSQLiteConnector::eErrorCode nErr;

  if (szNameA==NULL || szNameA[0]==0)
    return CNktSQLiteConnector::errInvalidParam;
  if (Ansi2Wide(&szNameW, szNameA) != CNktSQLiteConnector::noError)
    return CNktSQLiteConnector::errNoMem;
  nErr = DoQueryGetColumnIndex(lpIndex, lpData, nQueryId, szNameW);
  free(szNameW);
  return nErr;
}

static CNktSQLiteConnector::eErrorCode DoQueryGetColumnIndex(ULONG *lpIndex, MY_SQLITE_DATA *lpData, ULONG nQueryId, LPCWSTR szNameW)
{
  MY_SQLITE_DATA::tagQueries *lpQuery;
  ULONG i;

  *lpIndex = 0;
  if (szNameW==NULL || szNameW[0]==0)
    return CNktSQLiteConnector::errInvalidParam;
  if (lpData == NULL)
    return CNktSQLiteConnector::errNoDbOpened;
  //----
  lpQuery = (MY_SQLITE_DATA::tagQueries*)&(lpData->sQueries[nQueryId-1]);
  if (lpQuery->lpStmt == NULL)
    return CNktSQLiteConnector::errNoQueryInProgress;
  if ((lpQuery->nFlags & QUERY_FLAGS_NO_RESULT_SET) != 0)
    return CNktSQLiteConnector::errQueryHasNoResultSet;
  for (i=0; i<lpQuery->nFieldsCount; i++) {
    if (_wcsicmp(lpQuery->lpFields[i].szNameW, szNameW) == 0) {
      *lpIndex = i;
      return CNktSQLiteConnector::noError;
    }
  }
  return CNktSQLiteConnector::errInvalidParam;
}

static LONG GetTypeFromName(LPCSTR szNameA)
{
  static const struct tagTypeItem {
    LPCSTR szName;
    LONG nType;
  } aTypesList[] = {
    { "AUTOINCREMENT", SQLITE_INTEGER },
    { "BIGINT", SQLITE_INTEGER },
    { "BINARY", SQLITE_BLOB },
    { "BIT", SQLITE_INTEGER },
    { "BLOB", SQLITE_BLOB },
    { "BOOL", SQLITE_INTEGER },
    { "CHAR", SQLITE_TEXT },
    { "COUNTER", SQLITE_INTEGER },
    { "CURRENCY", SQLITE_FLOAT },
    { "DECIMAL", SQLITE_FLOAT },
    { "DOUBLE", SQLITE_FLOAT },
    { "FLOAT", SQLITE_FLOAT },
    { "GENERAL", SQLITE_BLOB },
    { "GUID", SQLITE_TEXT },
    { "IDENTITY", SQLITE_INTEGER },
    { "IMAGE", SQLITE_BLOB },
    { "INT", SQLITE_INTEGER },
    { "INTEGER", SQLITE_INTEGER },
    { "LOGICAL", SQLITE_INTEGER },
    { "LONG", SQLITE_INTEGER },
    { "LONGCHAR", SQLITE_TEXT },
    { "LONGTEXT", SQLITE_TEXT },
    { "LONGVARCHAR", SQLITE_TEXT },
    { "MEMO", SQLITE_TEXT },
    { "MONEY", SQLITE_FLOAT },
    { "NCHAR", SQLITE_TEXT },
    { "NOTE", SQLITE_TEXT },
    { "NTEXT", SQLITE_TEXT },
    { "NUMERIC", SQLITE_FLOAT },
    { "NVARCHAR", SQLITE_TEXT },
    { "OLEOBJECT", SQLITE_BLOB },
    { "REAL", SQLITE_FLOAT },
    { "SMALLINT", SQLITE_INTEGER },
    { "STRING", SQLITE_TEXT },
    { "TEXT", SQLITE_TEXT },
    { "TINYINT", SQLITE_INTEGER },
    { "UNIQUEIDENTIFIER", SQLITE_TEXT },
    { "VARBINARY", SQLITE_BLOB },
    { "VARCHAR", SQLITE_TEXT },
    { "YESNO", SQLITE_INTEGER }
  };
  struct tagTypeItem *lpLo, *lpHi, *lpMid;
  size_t nHalf, nCount, nNameLen;
  int nResult;

  for (nNameLen=0; (szNameA[nNameLen]>='A' && szNameA[nNameLen]<='Z') || (szNameA[nNameLen]>='a' && szNameA[nNameLen]<='z'); nNameLen++);
  nCount = sizeof(aTypesList) / sizeof(aTypesList[0]);
  lpLo = const_cast<struct tagTypeItem*>(&aTypesList[0]);
  lpHi = lpLo + (nCount-1);
  while (lpLo <= lpHi) {
    if ((nHalf = nCount/ 2) != 0) {
      lpMid = lpLo + (((nCount & 1) != 0) ? nHalf : (nHalf-1));
      nResult = _strnicmp(lpMid->szName, szNameA, nNameLen);
      if (nResult==0 && lpMid->szName[nNameLen]==0)
        return lpMid->nType;
      if (nResult >= 0) {
        lpHi = lpMid-1;
        nCount = ((nCount & 1) != 0) ? nHalf : (nHalf-1);
      }
      else {
        lpLo = lpMid+1;
        nCount = nHalf;
      }
    }
    else {
      if (_strnicmp(lpLo->szName, szNameA, nNameLen)==0 && lpLo->szName[nNameLen]==0)
        return lpLo->nType;
      break;
    }
  }
  return -1;
}

static CNktSQLiteConnector::eErrorCode Wide2LongLong(LONGLONG *lpnValue, LPCWSTR sW)
{
  CNktSQLiteConnector::eErrorCode nErr;
  ULONGLONG ullVal;
  BOOL bNeg;

  while (sW[0]==L'\t' && sW[0]==L' ')
    sW++;
  bNeg = FALSE;
  if (sW[0] == L'-') {
    sW++;
    bNeg = TRUE;
  }
  while (sW[0]==L'\t' && sW[0]==L' ')
    sW++;
  nErr = Wide2ULongLong(&ullVal, sW);
  if (nErr != CNktSQLiteConnector::noError)
    return nErr;
  if (bNeg!=FALSE && ullVal==0x8000000000000000ui64) {
    *lpnValue = -0x8000000000000000i64;
  }
  else {
    if (ullVal > 0x7FFFFFFFFFFFFFFFui64)
      CNktSQLiteConnector::errOverflow;
    *lpnValue = (LONGLONG)ullVal;
    if (bNeg != FALSE)
      *lpnValue = -(*lpnValue);
  }
  return CNktSQLiteConnector::noError;
}

static CNktSQLiteConnector::eErrorCode Wide2ULongLong(ULONGLONG *lpnValue, LPCWSTR sW)
{
  LONGLONG llVal, llNewVal;

  llVal = 0;
  while (*sW != 0) {
    if (sW[0]<=L'0' || sW[0]>=L'9')
      return CNktSQLiteConnector::errInvalidData;
    llNewVal = llVal * 10ui64;
    if (llNewVal < llVal)
      return CNktSQLiteConnector::errOverflow;
    llVal = llNewVal + (ULONGLONG)(sW[0] - L'0');
    if (llVal < llNewVal)
      return CNktSQLiteConnector::errOverflow;
    sW++;
  }
  *lpnValue = llVal;
  return CNktSQLiteConnector::noError;
}
