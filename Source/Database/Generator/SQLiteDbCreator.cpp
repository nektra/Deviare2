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

#include "SQLiteDbCreator.h"
#include "ParseCompiledXML.h"
#include "ModuleEnum.h"

//-----------------------------------------------------------

static BOOL CreateTables();

//-----------------------------------------------------------

BOOL CreateSQLiteDatabase(LPCWSTR szDestFileNameW)
{
  CNktSQLiteConnector::eErrorCode nDbErr;
  HRESULT hRes;

  //create database
  PrintAndLog(L"Creating temporary database...");
  nDbErr = cDB.Create(szDestFileNameW);
  if (nDbErr == CNktSQLiteConnector::errNoMem)
  {
bd_err_nomem:
    PrintAndLogNoMemoryError();
    cDB.Close();
    cXmlHelper.Reset();
    return FALSE;
  }
  if (nDbErr != CNktSQLiteConnector::noError)
  {
    PrintAndLog(L"\nError: Cannot create database (%lu).\n", (ULONG)nDbErr);
    return FALSE;
  }
  if (CreateTables() == FALSE) {
    cDB.Close();
    return FALSE;
  }
  PrintAndLog(L"OK\n");
  //build database
  PrintAndLogSeparator();
  PrintAndLog(L"Parsing compiled file... ");
  hRes = cXmlDoc->get_documentElement(&cXmlRootElem);
  if (hRes == ERROR_OUTOFMEMORY)
    goto bd_err_nomem;
  if (FAILED(hRes) || cXmlRootElem == NULL)
  {
    PrintAndLog(L"\nError: Cannot get root element (0x%08X).\n", (ULONG)hRes);
    cDB.Close();
    return FALSE;
  }
  hRes = cXmlHelper.Initialize(cXmlRootElem);
  if (hRes == ERROR_OUTOFMEMORY)
    goto bd_err_nomem;
  if (FAILED(hRes) || cXmlRootElem == NULL)
  {
    PrintAndLog(L"\nError: Cannot initialize xml helpers (0x%08X).\n", (ULONG)hRes);
    cDB.Close();
    cXmlHelper.Reset();
    return FALSE;
  }
  if (ParseCompiledXML() == FALSE)
  {
    cDB.Close();
    cXmlHelper.Reset();
    return FALSE;
  }
  PrintAndLog(L"OK\n");
  PrintAndLogSeparator();
  PrintAndLog(L"Parsing modules...\n");
  if (DoModEnum() == FALSE) {
    cDB.Close();
    cXmlHelper.Reset();
    return FALSE;
  }
  PrintAndLog(L"OK\n");
  //done
  cXmlHelper.Reset();
  return TRUE;
}

#define EXEC_QUERY() nDbErr = cDB.QueryExecute(1, sW);                           \
                     if (nDbErr != CNktSQLiteConnector::noError) goto bd_err_query
static BOOL CreateTables()
{
  CNktSQLiteConnector::eErrorCode nDbErr;
  LPWSTR sW;

  //modules table
  sW = L"CREATE TABLE Modules ( "
         L"Id   INTEGER NOT NULL ON CONFLICT FAIL, "
         L"Name VARCHAR(128) NOT NULL ON CONFLICT FAIL UNIQUE ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  nDbErr = cDB.QueryExecute(1, sW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
bd_err_query:
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      PrintAndLogNoMemoryError();
    else
      PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", sW, (ULONG)nDbErr);
    return FALSE;
  }
  //modules table indexes
  sW = L"CREATE INDEX Module_Name ON Modules ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //functions table
  sW = L"CREATE TABLE Functions ( "
         L"Id           INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Class        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name         VARCHAR(512)                            UNIQUE ON CONFLICT FAIL, " //may be NULL
         L"Flags        INTEGER      NOT NULL ON CONFLICT FAIL, " //0x01=IsExternal / 0x02=IsDllImport /
                                                                  //0x04=__stdcall / 0x08=__cdecl /
                                                                  //0x0C=__fastcall
         L"ReturnTypeId INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"ReturnClass  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //functions table indexes
  sW = L"CREATE INDEX Function_Name ON Functions ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //functions in modules table
  sW = L"CREATE TABLE ModulesFuncs ( "
         L"ModId        INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Modules(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"FuncId       INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Functions(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"PRIMARY KEY (ModId ASC, FuncId ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //functions arguments table
  sW = L"CREATE TABLE FunctionsArgs ( "
         L"FuncId    INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Functions(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name      VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (FuncId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //structs table
  sW = L"CREATE TABLE Structs ( "
         L"Id     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name   VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Size   INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Flags  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //structs table indexes
  sW = L"CREATE INDEX Struct_Name ON Structs ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //struct members table
  sW = L"CREATE TABLE StructsMembers ( "
         L"StructId  INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Structs(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name      VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Offset    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Bits      INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Flags     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (StructId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //struct bases table
  sW = L"CREATE TABLE StructsBases ( "
         L"StructId  INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Structs(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"BaseId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (StructId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //unions table
  sW = L"CREATE TABLE Unions ( "
         L"Id     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name   VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Size   INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Flags  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //unions table indexes
  sW = L"CREATE INDEX Union_Name ON Unions ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //unions members table
  sW = L"CREATE TABLE UnionsMembers ( "
         L"UnionId   INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Unions(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name      VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Offset    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Bits      INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Flags     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (UnionId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //struct bases table
  sW = L"CREATE TABLE UnionsBases ( "
         L"UnionId  INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Unions(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"BaseId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (UnionId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //typedefs table
  sW = L"CREATE TABLE TypeDefs ( "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name      VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //typedefs table indexes
  sW = L"CREATE INDEX TypeDef_Name ON TypeDefs ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //arrays table
  sW = L"CREATE TABLE Arrays ( "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Max       INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Size      INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //pointers table
  sW = L"CREATE TABLE Pointers ( "
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Size      INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //references
  sW = L"CREATE TABLE XReferences ( " //References is a reserved word
         L"Id        INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Size      INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeId    INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"TypeClass INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //enumerations table
  sW = L"CREATE TABLE Enumerations ( "
         L"Id     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name   VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Size   INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Align  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  //enumerations table indexes
  sW = L"CREATE INDEX Enumeration_Name ON Enumerations ( "
         L"Name COLLATE NOCASE ASC "
       L");";
  EXEC_QUERY();
  //enumerations values table
  sW = L"CREATE TABLE EnumerationsValues ( "
         L"EnumId INTEGER      NOT NULL ON CONFLICT FAIL REFERENCES Enumerations(Id) "
           L"ON DELETE CASCADE ON UPDATE CASCADE, "
         L"Id     INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"Name   VARCHAR(512) NOT NULL ON CONFLICT FAIL, "
         L"Value  INTEGER      NOT NULL ON CONFLICT FAIL, "
         L"PRIMARY KEY (EnumId ASC, Id ASC) ON CONFLICT FAIL "
       L");";
  EXEC_QUERY();
  return TRUE;
}
#undef EXEC_QUERY
