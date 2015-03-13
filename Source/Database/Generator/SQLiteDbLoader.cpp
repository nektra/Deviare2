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

#include "SQLiteDbLoader.h"
#include <limits.h>

//-----------------------------------------------------------

#define X_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

static int __cdecl Compare_DB_MODULE(LPVOID lpContext, const CSQLiteDbLoader::DB_MODULE *lpStruct1,
                                     const CSQLiteDbLoader::DB_MODULE *lpStruct2);
static int __cdecl Compare_DB_MODULE_FUNCTION(LPVOID lpContext,
                                              const CSQLiteDbLoader::DB_MODULE_FUNCTION *lpStruct1,
                                              const CSQLiteDbLoader::DB_MODULE_FUNCTION *lpStruct2);
static int __cdecl Compare_DB_FUNCTION(LPVOID lpContext, const CSQLiteDbLoader::DB_FUNCTION *lpStruct1,
                                       const CSQLiteDbLoader::DB_FUNCTION *lpStruct2);
static int __cdecl Compare_DB_FUNCTION_ARG(LPVOID lpContext,
                                           const CSQLiteDbLoader::DB_FUNCTION_ARG *lpStruct1,
                                           const CSQLiteDbLoader::DB_FUNCTION_ARG *lpStruct2);
static int __cdecl Compare_DB_STRUCT(LPVOID lpContext, const CSQLiteDbLoader::DB_STRUCT *lpStruct1,
                                     const CSQLiteDbLoader::DB_STRUCT *lpStruct2);
static int __cdecl Compare_DB_STRUCT_MEMBER(LPVOID lpContext,
                                            const CSQLiteDbLoader::DB_STRUCT_MEMBER *lpStruct1,
                                            const CSQLiteDbLoader::DB_STRUCT_MEMBER *lpStruct2);
static int __cdecl Compare_DB_STRUCT_BASE(LPVOID lpContext, const CSQLiteDbLoader::DB_STRUCT_BASE *lpStruct1,
                                          const CSQLiteDbLoader::DB_STRUCT_BASE *lpStruct2);
static int __cdecl Compare_DB_UNION(LPVOID lpContext, const CSQLiteDbLoader::DB_UNION *lpStruct1,
                                    const CSQLiteDbLoader::DB_UNION *lpStruct2);
static int __cdecl Compare_DB_UNION_MEMBER(LPVOID lpContext,
                                           const CSQLiteDbLoader::DB_UNION_MEMBER *lpStruct1,
                                           const CSQLiteDbLoader::DB_UNION_MEMBER *lpStruct2);
static int __cdecl Compare_DB_UNION_BASE(LPVOID lpContext, const CSQLiteDbLoader::DB_UNION_BASE *lpStruct1,
                                         const CSQLiteDbLoader::DB_UNION_BASE *lpStruct2);
static int __cdecl Compare_DB_TYPEDEF(LPVOID lpContext, const CSQLiteDbLoader::DB_TYPEDEF *lpStruct1,
                                      const CSQLiteDbLoader::DB_TYPEDEF *lpStruct2);
static int __cdecl Compare_DB_ARRAY(LPVOID lpContext, const CSQLiteDbLoader::DB_ARRAY *lpStruct1,
                                    const CSQLiteDbLoader::DB_ARRAY *lpStruct2);
static int __cdecl Compare_DB_POINTER(LPVOID lpContext, const CSQLiteDbLoader::DB_POINTER *lpStruct1,
                                      const CSQLiteDbLoader::DB_POINTER *lpStruct2);
static int __cdecl Compare_DB_REFERENCE(LPVOID lpContext, const CSQLiteDbLoader::DB_REFERENCE *lpStruct1,
                                        const CSQLiteDbLoader::DB_REFERENCE *lpStruct2);
static int __cdecl Compare_DB_ENUM(LPVOID lpContext, const CSQLiteDbLoader::DB_ENUM *lpStruct1,
                                   const CSQLiteDbLoader::DB_ENUM *lpStruct2);
static int __cdecl Compare_DB_ENUM_VALUE(LPVOID lpContext, const CSQLiteDbLoader::DB_ENUM_VALUE *lpStruct1,
                                         const CSQLiteDbLoader::DB_ENUM_VALUE *lpStruct2);
static BOOL IntegrityCheck_With_TypeClass(CSQLiteDbLoader *lpLdr, ULONG nId, ULONG nClass);
static VOID ReplaceWithBasicType(ULONG &nTypeId, ULONG &nTypeClass,
                                 TNktArrayList<ULONG> &aTypedefId2BasicTypeIndex);
static int StringList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2);

//-----------------------------------------------------------

CSQLiteDbLoader::CSQLiteDbLoader() : CNktMemMgrObj()
{
  return;
}

CSQLiteDbLoader::~CSQLiteDbLoader()
{
  return;
}

BOOL CSQLiteDbLoader::LoadDatabase(CNktSQLiteConnector &cDB)
{
  CNktSQLiteConnector::eErrorCode nDbErr;
  LPWSTR szQueryW;

  //add null string first
  if (AddStringToStringList(L"") == NULL)
  {
ld_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  //modules table
  PrintAndLog(L"Reading modules... ");
  szQueryW = L"SELECT * FROM Modules ORDER BY Id ASC;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
  {
ld_err_query_exec:
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", szQueryW, (ULONG)nDbErr);
    return FALSE;
  }
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_MODULE sNewModule;

    nDbErr = cDB.QueryGetFieldValue(&sNewModule.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewModule.szNameW, 1, 2, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aModuleList.AddElement(&sNewModule) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
  {
ld_err_query_fetch:
    PrintAndLog(L"\nError: Cannot retrieve query row [%s] (%lu).\n", szQueryW, (ULONG)nDbErr);
    return FALSE;
  }
  PrintAndLog(L"OK\n");

  //modules' functions table
  PrintAndLog(L"Reading functions of modules table... ");
  szQueryW = L"SELECT * FROM ModulesFuncs ORDER BY ModId ASC, FuncId ASC;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_MODULE_FUNCTION sNewModuleFunction;

    nDbErr = cDB.QueryGetFieldValue(&sNewModuleFunction.nModId, 1, 1); //mod id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewModuleFunction.nFuncId, 1, 2); //func id
    if (nDbErr == CNktSQLiteConnector::noError) {
      if (aModuleFunctionList.AddElement(&sNewModuleFunction) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //functions table
  PrintAndLog(L"Reading functions table... ");
  szQueryW = L"SELECT * FROM Functions;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_FUNCTION sNewFunction;

    nDbErr = cDB.QueryGetFieldValue(&sNewFunction.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunction.nClass, 1, 2); //class
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewFunction.szNameW, 1, 3, TRUE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunction.nFlags, 1, 4); //flags
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunction.nReturnTypeId, 1, 5); //return type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunction.nReturnClass, 1, 6); //return class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aFunctionList.AddElement(&sNewFunction) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //functions' arguments table
  PrintAndLog(L"Reading arguments of functions table... ");
  szQueryW = L"SELECT * FROM FunctionsArgs;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_FUNCTION_ARG sNewFunctionArg;

    nDbErr = cDB.QueryGetFieldValue(&sNewFunctionArg.nFuncId, 1, 1); //func id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunctionArg.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewFunctionArg.szNameW, 1, 3, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunctionArg.nTypeId, 1, 4); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewFunctionArg.nTypeClass, 1, 5); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aFunctionArgList.AddElement(&sNewFunctionArg) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //structures table
  PrintAndLog(L"Reading structures table... ");
  szQueryW = L"SELECT * FROM Structs;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_STRUCT sNewStruct;

    nDbErr = cDB.QueryGetFieldValue(&sNewStruct.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewStruct.szNameW, 1, 2, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStruct.nSize, 1, 3); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStruct.nAlign, 1, 4); //align
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStruct.nFlags, 1, 5); //flags
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aStructList.AddElement(&sNewStruct) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //structures' members table
  PrintAndLog(L"Reading members of structures table... ");
  szQueryW = L"SELECT * FROM StructsMembers;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_STRUCT_MEMBER sNewStructMember;

    nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nStructId, 1, 1); //struct id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewStructMember.szNameW, 1, 3, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nOffset, 1, 4); //offset
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nBits, 1, 5); //bits
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nFlags, 1, 6); //flags
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nTypeId, 1, 7); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructMember.nTypeClass, 1, 8); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aStructMemberList.AddElement(&sNewStructMember) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //structures' bases table
  PrintAndLog(L"Reading bases of structures table... ");
  szQueryW = L"SELECT * FROM StructsBases;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_STRUCT_BASE sNewStructBase;

    nDbErr = cDB.QueryGetFieldValue(&sNewStructBase.nStructId, 1, 1); //struct id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructBase.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewStructBase.nBaseId, 1, 3); //base id
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aStructBaseList.AddElement(&sNewStructBase) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //unions table
  PrintAndLog(L"Reading unions table... ");
  szQueryW = L"SELECT * FROM Unions;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_UNION sNewUnion;

    nDbErr = cDB.QueryGetFieldValue(&sNewUnion.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewUnion.szNameW, 1, 2, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnion.nSize, 1, 3); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnion.nAlign, 1, 4); //align
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnion.nFlags, 1, 5); //flags
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aUnionList.AddElement(&sNewUnion) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //unions' members table
  PrintAndLog(L"Reading members of unions table... ");
  szQueryW = L"SELECT * FROM UnionsMembers;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_UNION_MEMBER sNewUnionMember;

    nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nUnionId, 1, 1); //union id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewUnionMember.szNameW, 1, 3, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nOffset, 1, 4); //offset
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nBits, 1, 5); //bits
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nFlags, 1, 6); //flags
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nTypeId, 1, 7); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionMember.nTypeClass, 1, 8); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aUnionMemberList.AddElement(&sNewUnionMember) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //unions' bases table
  PrintAndLog(L"Reading bases of unions table... ");
  szQueryW = L"SELECT * FROM UnionsBases;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_UNION_BASE sNewUnionBase;

    nDbErr = cDB.QueryGetFieldValue(&sNewUnionBase.nUnionId, 1, 1); //union id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionBase.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewUnionBase.nBaseId, 1, 3); //base id
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aUnionBaseList.AddElement(&sNewUnionBase) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //typedefs table
  PrintAndLog(L"Reading typedefs table... ");
  szQueryW = L"SELECT * FROM TypeDefs;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_TYPEDEF sNewTypeDef;

    nDbErr = cDB.QueryGetFieldValue(&sNewTypeDef.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewTypeDef.szNameW, 1, 2, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewTypeDef.nTypeId, 1, 3); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewTypeDef.nTypeClass, 1, 4); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aTypeDefList.AddElement(&sNewTypeDef) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //arrays table
  PrintAndLog(L"Reading arrays table... ");
  szQueryW = L"SELECT * FROM Arrays;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_ARRAY sNewArray;

    nDbErr = cDB.QueryGetFieldValue(&sNewArray.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewArray.nMax, 1, 2); //max
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewArray.nSize, 1, 3); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewArray.nAlign, 1, 4); //align
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewArray.nTypeId, 1, 5); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewArray.nTypeClass, 1, 6); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aArrayList.AddElement(&sNewArray) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //pointers table
  PrintAndLog(L"Reading pointers table... ");
  szQueryW = L"SELECT * FROM Pointers;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_POINTER sNewPointer;

    nDbErr = cDB.QueryGetFieldValue(&sNewPointer.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewPointer.nSize, 1, 2); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewPointer.nAlign, 1, 3); //align
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewPointer.nTypeId, 1, 4); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewPointer.nTypeClass, 1, 5); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aPointerList.AddElement(&sNewPointer) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //references table
  PrintAndLog(L"Reading references table... ");
  szQueryW = L"SELECT * FROM XReferences;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError) {
    DB_REFERENCE sNewReference;

    nDbErr = cDB.QueryGetFieldValue(&sNewReference.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewReference.nSize, 1, 2); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewReference.nAlign, 1, 3); //align
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewReference.nTypeId, 1, 4); //type id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewReference.nTypeClass, 1, 5); //type class
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aReferenceList.AddElement(&sNewReference) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //enumerations table
  PrintAndLog(L"Reading enumerations table... ");
  szQueryW = L"SELECT * FROM Enumerations;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_ENUM sNewEnum;

    nDbErr = cDB.QueryGetFieldValue(&sNewEnum.nId, 1, 1); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewEnum.szNameW, 1, 2, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewEnum.nSize, 1, 3); //size
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewEnum.nAlign, 1, 4); //align
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aEnumList.AddElement(&sNewEnum) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");

  //enumerations' values table
  PrintAndLog(L"Reading values of enumerations table... ");
  szQueryW = L"SELECT * FROM EnumerationsValues;";
  nDbErr = cDB.QueryExecute(1, szQueryW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::noError)
    goto ld_err_query_exec;
  nDbErr = cDB.QueryFetchNextRow(1);
  while (nDbErr == CNktSQLiteConnector::noError)
  {
    DB_ENUM_VALUE sNewEnumValue;

    nDbErr = cDB.QueryGetFieldValue(&sNewEnumValue.nEnumId, 1, 1); //enum id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue(&sNewEnumValue.nId, 1, 2); //id
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = QueryGetFieldTextAndAddToStringList(cDB, &sNewEnumValue.szNameW, 1, 3, FALSE); //name
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryGetFieldValue((LONG*)&sNewEnumValue.nValue, 1, 4); //value
    if (nDbErr == CNktSQLiteConnector::noError)
    {
      if (aEnumValueList.AddElement(&sNewEnumValue) == FALSE)
        nDbErr = CNktSQLiteConnector::errNoMem;
    }
    if (nDbErr == CNktSQLiteConnector::noError)
      nDbErr = cDB.QueryFetchNextRow(1);
  }
  if (nDbErr == CNktSQLiteConnector::errNoMem)
    goto ld_err_nomem;
  if (nDbErr != CNktSQLiteConnector::errEndOfResultSetReached &&
      nDbErr != CNktSQLiteConnector::errQueryHasNoResultSet)
    goto ld_err_query_fetch;
  PrintAndLog(L"OK\n");
  return TRUE;
}

BOOL CSQLiteDbLoader::SortData()
{
  PrintAndLog(L"Sorting... ");
  aModuleList.QuickSort(Compare_DB_MODULE);
  aModuleFunctionList.QuickSort(Compare_DB_MODULE_FUNCTION);
  aFunctionList.QuickSort(Compare_DB_FUNCTION);
  aFunctionArgList.QuickSort(Compare_DB_FUNCTION_ARG);
  aStructList.QuickSort(Compare_DB_STRUCT);
  aStructMemberList.QuickSort(Compare_DB_STRUCT_MEMBER);
  aStructBaseList.QuickSort(Compare_DB_STRUCT_BASE);
  aUnionList.QuickSort(Compare_DB_UNION);
  aUnionMemberList.QuickSort(Compare_DB_UNION_MEMBER);
  aUnionBaseList.QuickSort(Compare_DB_UNION_BASE);
  aTypeDefList.QuickSort(Compare_DB_TYPEDEF);
  aArrayList.QuickSort(Compare_DB_ARRAY);
  aPointerList.QuickSort(Compare_DB_POINTER);
  aReferenceList.QuickSort(Compare_DB_REFERENCE);
  aEnumList.QuickSort(Compare_DB_ENUM);
  aEnumValueList.QuickSort(Compare_DB_ENUM_VALUE);
  PrintAndLog(L"OK\n");
  return TRUE;
}

BOOL CSQLiteDbLoader::CheckDataIntegrity()
{
  SIZE_T i, nCount;

  PrintAndLog(L"Checking data integrity... ");
  //module/function
  nCount = aModuleFunctionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (FindModule(aModuleFunctionList[i].nModId) == NULL)
    {
      PrintAndLog(L"\nError: Cannot find module with id:%lu.\n", aModuleFunctionList[i].nModId);
      return FALSE;
    }
    if (FindFunction(aModuleFunctionList[i].nFuncId) == NULL)
    {
      PrintAndLog(L"\nError: Cannot find function with id:%lu.\n", aModuleFunctionList[i].nFuncId);
      return FALSE;
    }
  }
  //function-return
  nCount = aFunctionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IntegrityCheck_With_TypeClass(this, aFunctionList[i].nReturnTypeId,
                                      aFunctionList[i].nReturnClass) == FALSE)
      return FALSE;
  }
  //function-arguments
  nCount = aFunctionArgList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IntegrityCheck_With_TypeClass(this, aFunctionArgList[i].nTypeId,
                                      aFunctionArgList[i].nTypeClass) == FALSE)
      return FALSE;
  }
  //struct-members
  nCount = aStructMemberList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IntegrityCheck_With_TypeClass(this, aStructMemberList[i].nTypeId,
                                      aStructMemberList[i].nTypeClass) == FALSE)
      return FALSE;
  }
  //struct-bases
  nCount = aStructBaseList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (FindStruct(aStructBaseList[i].nBaseId) == NULL)
    {
      PrintAndLog(L"\nError: Cannot find struct with id:%lu.\n", aStructBaseList[i].nBaseId);
      return FALSE;
    }
  }
  //union-members
  nCount = aUnionMemberList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IntegrityCheck_With_TypeClass(this, aUnionMemberList[i].nTypeId,
                                      aUnionMemberList[i].nTypeClass) == FALSE)
      return FALSE;
  }
  //union-bases
  nCount = aUnionBaseList.GetCount();
  for (i=0; i<nCount; i++) {
    if (FindUnion(aUnionBaseList[i].nBaseId) == NULL) {
      PrintAndLog(L"\nError: Cannot find union with id:%lu.\n", aUnionBaseList[i].nBaseId);
      return FALSE;
    }
  }
  //typedefs
  nCount = aTypeDefList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IntegrityCheck_With_TypeClass(this, aTypeDefList[i].nTypeId, aTypeDefList[i].nTypeClass) == FALSE)
      return FALSE;
  }
  PrintAndLog(L"OK\n");
  return TRUE;
}

BOOL CSQLiteDbLoader::BuildBasicTypes()
{
  static const LPWSTR szFundTypeNamesW[NKT_DBFUNDTYPE_MAX-NKT_DBFUNDTYPE_MIN+1] = {
    L"char", L"unsigned char", L"short", L"unsigned short", L"long",
    L"unsigned long", L"long long", L"unsigned long long", L"float",
    L"double", L"long double", L"void", L"char", L"wchar_t"
  };
  TNktArrayList<ULONG> aTypedefId2BasicTypeIndex;
  DB_BASIC_TYPE sBasicType;
  SIZE_T i, nCount;
  ULONG nCurrClass, nCurrId, nCurrFlags, nMax;

  PrintAndLog(L"Building basic types... ");
  //convert fundamental types
  for (i=NKT_DBFUNDTYPE_MIN; i<=NKT_DBFUNDTYPE_MAX; i++)
  {
    nktMemSet(&sBasicType, 0, sizeof(sBasicType));
    sBasicType.nOrigTypeId = sBasicType.nTypeId = (LONG)i;
    sBasicType.nOrigTypeClass = sBasicType.nTypeClass = NKT_DBOBJCLASS_Fundamental;
    sBasicType.szNameW = AddStringToStringList(szFundTypeNamesW[i-NKT_DBFUNDTYPE_MIN]);
    if (sBasicType.szNameW == NULL)
    {
bdbol_err_nomem:
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    sBasicType.nId = (ULONG)i;
    if (aBasicTypesList.AddElement(&sBasicType) == FALSE)
      goto bdbol_err_nomem;
  }
  //convert typedefs
  nCount = aTypeDefList.GetCount();
  nMax = 0;
  for (i=0; i<nCount; i++)
  {
    if (aTypeDefList[i].nId > nMax)
      nMax = aTypeDefList[i].nId;
  }
  if (aTypedefId2BasicTypeIndex.SetCount((SIZE_T)nMax+1) == FALSE)
    goto bdbol_err_nomem;
  nktMemSet(aTypedefId2BasicTypeIndex.GetBuffer(), 0xFF, ((SIZE_T)nMax+1)*sizeof(ULONG));
  for (i=0; i<nCount; i++)
  {
    //PrintAndLog(L"\rBuilding basic types (%.2lf%%)... ", ((double)(i+1)/(double)nCount)*50.0);
    NKT_ASSERT(aTypedefId2BasicTypeIndex[aTypeDefList[i].nId] == ULONG_MAX);
    aTypedefId2BasicTypeIndex[aTypeDefList[i].nId] = (ULONG)aBasicTypesList.GetCount() + 1;
    //----
    nktMemSet(&sBasicType, 0, sizeof(sBasicType));
    sBasicType.nOrigTypeId = aTypeDefList[i].nId;
    sBasicType.nOrigTypeClass = NKT_DBOBJCLASS_Typedef;
    sBasicType.szNameW = AddStringToStringList(aTypeDefList[i].szNameW);
    if (sBasicType.szNameW == NULL)
      goto bdbol_err_nomem;
    sBasicType.nId = (ULONG)aBasicTypesList.GetCount() + 1;
    nCurrFlags = aTypeDefList[i].nTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|NKT_DBOBJCLASSFLAG_IsVolatile);
    nCurrClass = aTypeDefList[i].nTypeClass & NKT_DBOBJCLASS_MASK;
    nCurrId = aTypeDefList[i].nTypeId;
    while (nCurrClass == NKT_DBOBJCLASS_Typedef) {
      DB_TYPEDEF *lpTypeDef;

      lpTypeDef = FindTypeDef(nCurrId);
      NKT_ASSERT(lpTypeDef != NULL);
      nCurrFlags |= lpTypeDef->nTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|NKT_DBOBJCLASSFLAG_IsVolatile);
      nCurrClass = lpTypeDef->nTypeClass & NKT_DBOBJCLASS_MASK;
      nCurrId = lpTypeDef->nTypeId;
    }
    sBasicType.nTypeId = nCurrId;
    sBasicType.nTypeClass = nCurrClass | nCurrFlags;
    if (aBasicTypesList.AddElement(&sBasicType) == FALSE)
      goto bdbol_err_nomem;
  }
  PrintAndLog(L"OK\n");
  //--------
  PrintAndLog(L"Replacing targets with basic types... ");
  //function-return
  nCount = aFunctionList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aFunctionList[i].nReturnTypeId, aFunctionList[i].nReturnClass,
                         aTypedefId2BasicTypeIndex);
  }
  //function-arguments
  nCount = aFunctionArgList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aFunctionArgList[i].nTypeId, aFunctionArgList[i].nTypeClass,
                         aTypedefId2BasicTypeIndex);
  }
  //struct-members
  nCount = aStructMemberList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aStructMemberList[i].nTypeId, aStructMemberList[i].nTypeClass,
                         aTypedefId2BasicTypeIndex);
  }
  //union-members
  nCount = aUnionMemberList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aUnionMemberList[i].nTypeId, aUnionMemberList[i].nTypeClass,
                         aTypedefId2BasicTypeIndex);
  }
  //arrays
  nCount = aArrayList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aArrayList[i].nTypeId, aArrayList[i].nTypeClass, aTypedefId2BasicTypeIndex);
  }
  //pointers
  nCount = aPointerList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aPointerList[i].nTypeId, aPointerList[i].nTypeClass, aTypedefId2BasicTypeIndex);
  }
  //references
  nCount = aReferenceList.GetCount();
  for (i=0; i<nCount; i++)
  {
    ReplaceWithBasicType(aReferenceList[i].nTypeId, aReferenceList[i].nTypeClass,
                         aTypedefId2BasicTypeIndex);
  }
  PrintAndLog(L"OK\n");
  return TRUE;
}

VOID CSQLiteDbLoader::FreeALotOfStuff()
{
  aFunctionList.RemoveAllElements();
  aFunctionArgList.RemoveAllElements();
  aStructList.RemoveAllElements();
  aStructMemberList.RemoveAllElements();
  aStructBaseList.RemoveAllElements();
  aUnionList.RemoveAllElements();
  aUnionMemberList.RemoveAllElements();
  aUnionBaseList.RemoveAllElements();
  aTypeDefList.RemoveAllElements();
  aArrayList.RemoveAllElements();
  aPointerList.RemoveAllElements();
  aReferenceList.RemoveAllElements();
  aEnumList.RemoveAllElements();
  aEnumValueList.RemoveAllElements();
  aBasicTypesList.RemoveAllElements();
  return;
}

CSQLiteDbLoader::DB_MODULE* CSQLiteDbLoader::FindModule(ULONG nModId)
{
  DB_MODULE sSearch;
  SIZE_T nRes;

  sSearch.nId = nModId;
  nRes = aModuleList.BinarySearch(&sSearch, Compare_DB_MODULE);
  return (nRes != (SIZE_T)-1) ? (aModuleList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_MODULE_FUNCTION* CSQLiteDbLoader::FindModuleFunction(ULONG nModId, ULONG nFuncId)
{
  DB_MODULE_FUNCTION sSearch;
  SIZE_T nRes;

  sSearch.nModId = nModId;
  sSearch.nFuncId = nFuncId;
  nRes = aModuleFunctionList.BinarySearch(&sSearch, Compare_DB_MODULE_FUNCTION);
  return (nRes != (SIZE_T)-1) ? (aModuleFunctionList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_FUNCTION* CSQLiteDbLoader::FindFunction(ULONG nFuncId)
{
  DB_FUNCTION sSearch;
  SIZE_T nRes;

  sSearch.nId = nFuncId;
  nRes = aFunctionList.BinarySearch(&sSearch, Compare_DB_FUNCTION);
  return (nRes != (SIZE_T)-1) ? (aFunctionList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_FUNCTION_ARG* CSQLiteDbLoader::FindFunctionArg(ULONG nFuncId, ULONG nArgIdx)
{
  DB_FUNCTION_ARG sSearch;
  SIZE_T nRes;

  sSearch.nFuncId = nFuncId;
  sSearch.nId = nArgIdx;
  nRes = aFunctionArgList.BinarySearch(&sSearch, Compare_DB_FUNCTION_ARG);
  return (nRes != (SIZE_T)-1) ? (aFunctionArgList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_STRUCT* CSQLiteDbLoader::FindStruct(ULONG nStructId)
{
  DB_STRUCT sSearch;
  SIZE_T nRes;

  sSearch.nId = nStructId;
  nRes = aStructList.BinarySearch(&sSearch, Compare_DB_STRUCT);
  return (nRes != (SIZE_T)-1) ? (aStructList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_STRUCT_MEMBER* CSQLiteDbLoader::FindStructMember(ULONG nStructId, ULONG nMemberIdx)
{
  DB_STRUCT_MEMBER sSearch;
  SIZE_T nRes;

  sSearch.nStructId = nStructId;
  sSearch.nId = nMemberIdx;
  nRes = aStructMemberList.BinarySearch(&sSearch, Compare_DB_STRUCT_MEMBER);
  return (nRes != (SIZE_T)-1) ? (aStructMemberList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_STRUCT_BASE* CSQLiteDbLoader::FindStructBase(ULONG nStructId, ULONG nBaseIdx)
{
  DB_STRUCT_BASE sSearch;
  SIZE_T nRes;

  sSearch.nStructId = nStructId;
  sSearch.nId = nBaseIdx;
  nRes = aStructBaseList.BinarySearch(&sSearch, Compare_DB_STRUCT_BASE);
  return (nRes != (SIZE_T)-1) ? (aStructBaseList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_UNION* CSQLiteDbLoader::FindUnion(ULONG nUnionId)
{
  DB_UNION sSearch;
  SIZE_T nRes;

  sSearch.nId = nUnionId;
  nRes = aUnionList.BinarySearch(&sSearch, Compare_DB_UNION);
  return (nRes != (SIZE_T)-1) ? (aUnionList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_UNION_MEMBER* CSQLiteDbLoader::FindUnionMember(ULONG nUnionId, ULONG nMemberIdx)
{
  DB_UNION_MEMBER sSearch;
  SIZE_T nRes;

  sSearch.nUnionId = nUnionId;
  sSearch.nId = nMemberIdx;
  nRes = aUnionMemberList.BinarySearch(&sSearch, Compare_DB_UNION_MEMBER);
  return (nRes != (SIZE_T)-1) ? (aUnionMemberList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_UNION_BASE* CSQLiteDbLoader::FindUnionBase(ULONG nUnionId, ULONG nBaseIdx)
{
  DB_UNION_BASE sSearch;
  SIZE_T nRes;

  sSearch.nUnionId = nUnionId;
  sSearch.nId = nBaseIdx;
  nRes = aUnionBaseList.BinarySearch(&sSearch, Compare_DB_UNION_BASE);
  return (nRes != (SIZE_T)-1) ? (aUnionBaseList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_TYPEDEF* CSQLiteDbLoader::FindTypeDef(ULONG nId)
{
  DB_TYPEDEF sSearch;
  SIZE_T nRes;

  sSearch.nId = nId;
  nRes = aTypeDefList.BinarySearch(&sSearch, Compare_DB_TYPEDEF);
  return (nRes != (SIZE_T)-1) ? (aTypeDefList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_ARRAY* CSQLiteDbLoader::FindArray(ULONG nId)
{
  DB_ARRAY sSearch;
  SIZE_T nRes;

  sSearch.nId = nId;
  nRes = aArrayList.BinarySearch(&sSearch, Compare_DB_ARRAY);
  return (nRes != (SIZE_T)-1) ? (aArrayList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_POINTER* CSQLiteDbLoader::FindPointer(ULONG nPtrId)
{
  DB_POINTER sSearch;
  SIZE_T nRes;

  sSearch.nId = nPtrId;
  nRes = aPointerList.BinarySearch(&sSearch, Compare_DB_POINTER);
  return (nRes != (SIZE_T)-1) ? (aPointerList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_REFERENCE* CSQLiteDbLoader::FindReference(ULONG nRefId)
{
  DB_REFERENCE sSearch;
  SIZE_T nRes;

  sSearch.nId = nRefId;
  nRes = aReferenceList.BinarySearch(&sSearch, Compare_DB_REFERENCE);
  return (nRes != (SIZE_T)-1) ? (aReferenceList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_ENUM* CSQLiteDbLoader::FindEnum(ULONG nEnumId)
{
  DB_ENUM sSearch;
  SIZE_T nRes;

  sSearch.nId = nEnumId;
  nRes = aEnumList.BinarySearch(&sSearch, Compare_DB_ENUM);
  return (nRes != (SIZE_T)-1) ? (aEnumList.GetBuffer()+nRes) : NULL;
}

CSQLiteDbLoader::DB_ENUM_VALUE* CSQLiteDbLoader::FindEnumValue(ULONG nEnumId, ULONG nValId)
{
  DB_ENUM_VALUE sSearch;
  SIZE_T nRes;

  sSearch.nEnumId = nEnumId;
  sSearch.nId = nValId;
  nRes = aEnumValueList.BinarySearch(&sSearch, Compare_DB_ENUM_VALUE);
  return (nRes != (SIZE_T)-1) ? (aEnumValueList.GetBuffer()+nRes) : NULL;
}

SIZE_T CSQLiteDbLoader::FindStringInStringList(LPCWSTR sW)
{
  if (sW == NULL)
    sW = L"";
  return aStringsList.BinarySearch((LPWSTR*)&sW, StringList_Compare, NULL);
}

CNktSQLiteConnector::eErrorCode CSQLiteDbLoader::QueryGetFieldTextAndAddToStringList(
              CNktSQLiteConnector &cDB, LPWSTR *szDestW, ULONG nQueryId, ULONG nColumnIndex,
              BOOL bMayBeNull)
{
  CNktSQLiteConnector::eErrorCode nDbErr;
  LPWSTR sW;

  nDbErr = cDB.QueryGetFieldValue(&sW, nQueryId, nColumnIndex);
  if (bMayBeNull != FALSE && nDbErr == CNktSQLiteConnector::errNullValue)
    sW = L"";
  else if (nDbErr != CNktSQLiteConnector::noError)
    return nDbErr;
  sW = AddStringToStringList(sW);
  if (sW == NULL)
    return CNktSQLiteConnector::errNoMem;
  *szDestW = sW;
  return CNktSQLiteConnector::noError;
}

LPWSTR CSQLiteDbLoader::AddStringToStringList(LPCWSTR sW)
{
  CNktStringW cStrTempW;
  LPWSTR *lpsW;

  lpsW = aStringsList.BinarySearchPtr((LPWSTR*)&sW, StringList_Compare, NULL);
  if (lpsW != NULL)
    return *lpsW;
  if (sW[0] != 0)
  {
    if (cStrTempW.Copy(sW) == FALSE)
      return NULL;
  }
  else
  {
    if (cStrTempW.EnsureBuffer(1) == FALSE)
      return NULL;
    ((LPWSTR)cStrTempW)[0] = 0;
    cStrTempW.Refresh();
  }
  if (aStringsList.SortedInsert((LPWSTR)cStrTempW, StringList_Compare, NULL) == FALSE)
    return NULL;
  return cStrTempW.Detach();
}

//-----------------------------------------------------------

static int __cdecl Compare_DB_MODULE(LPVOID lpContext, const CSQLiteDbLoader::DB_MODULE *lpStruct1,
                                     const CSQLiteDbLoader::DB_MODULE *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_MODULE_FUNCTION(LPVOID lpContext,
                                              const CSQLiteDbLoader::DB_MODULE_FUNCTION *lpStruct1,
                                              const CSQLiteDbLoader::DB_MODULE_FUNCTION *lpStruct2)
{
  if (lpStruct1->nModId < lpStruct2->nModId)
    return -1;
  if (lpStruct1->nModId > lpStruct2->nModId)
    return 1;
  if (lpStruct1->nFuncId < lpStruct2->nFuncId)
    return -1;
  if (lpStruct1->nFuncId > lpStruct2->nFuncId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_FUNCTION(LPVOID lpContext, const CSQLiteDbLoader::DB_FUNCTION *lpStruct1,
                                       const CSQLiteDbLoader::DB_FUNCTION *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_FUNCTION_ARG(LPVOID lpContext,
                                           const CSQLiteDbLoader::DB_FUNCTION_ARG *lpStruct1,
                                           const CSQLiteDbLoader::DB_FUNCTION_ARG *lpStruct2)
{
  if (lpStruct1->nFuncId < lpStruct2->nFuncId)
    return -1;
  if (lpStruct1->nFuncId > lpStruct2->nFuncId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_STRUCT(LPVOID lpContext, const CSQLiteDbLoader::DB_STRUCT *lpStruct1,
                                     const CSQLiteDbLoader::DB_STRUCT *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_STRUCT_MEMBER(LPVOID lpContext,
                                            const CSQLiteDbLoader::DB_STRUCT_MEMBER *lpStruct1,
                                            const CSQLiteDbLoader::DB_STRUCT_MEMBER *lpStruct2)
{
  if (lpStruct1->nStructId < lpStruct2->nStructId)
    return -1;
  if (lpStruct1->nStructId > lpStruct2->nStructId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_STRUCT_BASE(LPVOID lpContext, const CSQLiteDbLoader::DB_STRUCT_BASE *lpStruct1,
                                          const CSQLiteDbLoader::DB_STRUCT_BASE *lpStruct2)
{
  if (lpStruct1->nStructId < lpStruct2->nStructId)
    return -1;
  if (lpStruct1->nStructId > lpStruct2->nStructId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_UNION(LPVOID lpContext, const CSQLiteDbLoader::DB_UNION *lpStruct1,
                                    const CSQLiteDbLoader::DB_UNION *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_UNION_MEMBER(LPVOID lpContext,
                                           const CSQLiteDbLoader::DB_UNION_MEMBER *lpStruct1,
                                           const CSQLiteDbLoader::DB_UNION_MEMBER *lpStruct2)
{
  if (lpStruct1->nUnionId < lpStruct2->nUnionId)
    return -1;
  if (lpStruct1->nUnionId > lpStruct2->nUnionId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_UNION_BASE(LPVOID lpContext, const CSQLiteDbLoader::DB_UNION_BASE *lpStruct1,
                                         const CSQLiteDbLoader::DB_UNION_BASE *lpStruct2)
{
  if (lpStruct1->nUnionId < lpStruct2->nUnionId)
    return -1;
  if (lpStruct1->nUnionId > lpStruct2->nUnionId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_TYPEDEF(LPVOID lpContext, const CSQLiteDbLoader::DB_TYPEDEF *lpStruct1,
                                      const CSQLiteDbLoader::DB_TYPEDEF *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_ARRAY(LPVOID lpContext, const CSQLiteDbLoader::DB_ARRAY *lpStruct1,
                                    const CSQLiteDbLoader::DB_ARRAY *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_POINTER(LPVOID lpContext, const CSQLiteDbLoader::DB_POINTER *lpStruct1,
                                      const CSQLiteDbLoader::DB_POINTER *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_REFERENCE(LPVOID lpContext, const CSQLiteDbLoader::DB_REFERENCE *lpStruct1,
                                        const CSQLiteDbLoader::DB_REFERENCE *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_ENUM(LPVOID lpContext, const CSQLiteDbLoader::DB_ENUM *lpStruct1,
                                   const CSQLiteDbLoader::DB_ENUM *lpStruct2)
{
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static int __cdecl Compare_DB_ENUM_VALUE(LPVOID lpContext, const CSQLiteDbLoader::DB_ENUM_VALUE *lpStruct1,
                                         const CSQLiteDbLoader::DB_ENUM_VALUE *lpStruct2)
{
  if (lpStruct1->nEnumId < lpStruct2->nEnumId)
    return -1;
  if (lpStruct1->nEnumId > lpStruct2->nEnumId)
    return 1;
  if (lpStruct1->nId < lpStruct2->nId)
    return -1;
  if (lpStruct1->nId > lpStruct2->nId)
    return 1;
  return 0;
}

static BOOL IntegrityCheck_With_TypeClass(CSQLiteDbLoader *lpLdr, ULONG nId, ULONG nClass)
{
  switch (nClass & NKT_DBOBJCLASS_MASK)
  {
    case NKT_DBOBJCLASS_Fundamental:
      if (nId<NKT_DBFUNDTYPE_MIN || nId>NKT_DBFUNDTYPE_MAX)
      {
        PrintAndLog(L"\nError: Cannot find fundamental type with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    case NKT_DBOBJCLASS_Struct:
    case NKT_DBOBJCLASS_Union:
      //struct and union may not exist
      break;
    case NKT_DBOBJCLASS_Typedef:
      if (lpLdr->FindTypeDef(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find typedef with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    case NKT_DBOBJCLASS_Array:
      if (lpLdr->FindArray(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find array with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    case NKT_DBOBJCLASS_Pointer:
      if (lpLdr->FindPointer(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find pointer with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    case NKT_DBOBJCLASS_Reference:
      if (lpLdr->FindReference(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find reference with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    case NKT_DBOBJCLASS_Enumeration:
      if (lpLdr->FindEnum(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find enumeration with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    //case NKT_DBOBJCLASS_Function:
    case NKT_DBOBJCLASS_FunctionType:
    case NKT_DBOBJCLASS_ClassConstructor:
    case NKT_DBOBJCLASS_ClassDestructor:
    case NKT_DBOBJCLASS_ClassOperatorMethod:
    case NKT_DBOBJCLASS_ClassMethod:
    case NKT_DBOBJCLASS_ClassConverter:
      if (lpLdr->FindFunction(nId) == NULL)
      {
        PrintAndLog(L"\nError: Cannot find function with id:%lu.\n", nId);
        return FALSE;
      }
      break;
    default:
      PrintAndLog(L"\nError: Invalid type class :%lu.\n", nClass & NKT_DBOBJCLASS_MASK);
      return FALSE;
  }
  return TRUE;
}

static VOID ReplaceWithBasicType(ULONG &nTypeId, ULONG &nTypeClass,
                                 TNktArrayList<ULONG> &aTypedefId2BasicTypeIndex)
{
  switch (nTypeClass & NKT_DBOBJCLASS_MASK)
  {
    case NKT_DBOBJCLASS_Fundamental:
      nTypeClass = nTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|NKT_DBOBJCLASSFLAG_IsVolatile);
      nTypeClass |= NKT_DBOBJCLASS_Fundamental;
      NKT_ASSERT(nTypeId>=NKT_DBFUNDTYPE_MIN && nTypeId<=NKT_DBFUNDTYPE_MAX);
      break;
    case NKT_DBOBJCLASS_Typedef:
      nTypeClass = nTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|NKT_DBOBJCLASSFLAG_IsVolatile);
      nTypeClass |= NKT_DBOBJCLASS_Fundamental;
      NKT_ASSERT(aTypedefId2BasicTypeIndex[nTypeId] != ULONG_MAX);
      nTypeId = aTypedefId2BasicTypeIndex[nTypeId];
      break;
  }
  return;
}

static int StringList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2)
{
  return wcscmp(*lpElem1, *lpElem2);
}
