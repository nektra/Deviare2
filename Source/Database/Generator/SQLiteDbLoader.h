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

#include "DbGenerator.h"
#include "..\..\Common\MemoryManager.h"

//-----------------------------------------------------------

class CSQLiteDbLoader : public CNktMemMgrObj
{
public:
#pragma pack(1)
  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
  } DB_MODULE;

  typedef struct {
    ULONG nModId;
    ULONG nFuncId;
  } DB_MODULE_FUNCTION;

  typedef struct {
    ULONG nId;
    ULONG nClass;
    LPWSTR szNameW;
    ULONG nFlags;
    ULONG nReturnTypeId;
    ULONG nReturnClass;
  } DB_FUNCTION;

  typedef struct {
    ULONG nFuncId;
    ULONG nId;
    LPWSTR szNameW;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_FUNCTION_ARG;

  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
    ULONG nSize;
    ULONG nAlign;
    ULONG nFlags;
  } DB_STRUCT;

  typedef struct {
    ULONG nStructId;
    ULONG nId;
    LPWSTR szNameW;
    ULONG nOffset;
    ULONG nBits;
    ULONG nFlags;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_STRUCT_MEMBER;

  typedef struct {
    ULONG nStructId;
    ULONG nId;
    ULONG nBaseId;
  } DB_STRUCT_BASE;

  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
    ULONG nSize;
    ULONG nAlign;
    ULONG nFlags;
  } DB_UNION;

  typedef struct {
    ULONG nUnionId;
    ULONG nId;
    LPWSTR szNameW;
    ULONG nOffset;
    ULONG nBits;
    ULONG nFlags;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_UNION_MEMBER;

  typedef struct {
    ULONG nUnionId;
    ULONG nId;
    ULONG nBaseId;
  } DB_UNION_BASE;

  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_TYPEDEF;

  typedef struct {
    ULONG nId;
    ULONG nMax;
    ULONG nSize;
    ULONG nAlign;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_ARRAY;

  typedef struct {
    ULONG nId;
    ULONG nSize;
    ULONG nAlign;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_POINTER;

  typedef struct {
    ULONG nId;
    ULONG nSize;
    ULONG nAlign;
    ULONG nTypeId;
    ULONG nTypeClass;
  } DB_REFERENCE;

  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
    ULONG nSize;
    ULONG nAlign;
  } DB_ENUM;

  typedef struct {
    ULONG nEnumId;
    ULONG nId;
    LPWSTR szNameW;
    LONG nValue;
  } DB_ENUM_VALUE;

  typedef struct {
    ULONG nId;
    LPWSTR szNameW;
    ULONG nTypeId;
    ULONG nTypeClass;
    ULONG nOrigTypeId;
    ULONG nOrigTypeClass;
  } DB_BASIC_TYPE;
#pragma pack()

public:
  CSQLiteDbLoader();
  ~CSQLiteDbLoader();

  BOOL LoadDatabase(CNktSQLiteConnector &cDB);
  BOOL SortData();
  BOOL CheckDataIntegrity();
  BOOL BuildBasicTypes();

  VOID FreeALotOfStuff();

  DB_MODULE* FindModule(ULONG nModId);
  DB_MODULE_FUNCTION* FindModuleFunction(ULONG nModId, ULONG nFuncId);
  DB_FUNCTION* FindFunction(ULONG nFuncId);
  DB_FUNCTION_ARG* FindFunctionArg(ULONG nFuncId, ULONG nArgIdx);
  DB_STRUCT* FindStruct(ULONG nStructId);
  DB_STRUCT_MEMBER* FindStructMember(ULONG nStructId, ULONG nMemberIdx);
  DB_STRUCT_BASE* FindStructBase(ULONG nStructId, ULONG nBaseIdx);
  DB_UNION* FindUnion(ULONG nUnionId);
  DB_UNION_MEMBER* FindUnionMember(ULONG nUnionId, ULONG nMemberIdx);
  DB_UNION_BASE* FindUnionBase(ULONG nUnionId, ULONG nBaseIdx);
  DB_TYPEDEF* FindTypeDef(ULONG nId);
  DB_ARRAY* FindArray(ULONG nId);
  DB_POINTER* FindPointer(ULONG nPtrId);
  DB_REFERENCE* FindReference(ULONG nRefId);
  DB_ENUM* FindEnum(ULONG nEnumId);
  DB_ENUM_VALUE* FindEnumValue(ULONG nEnumId, ULONG nValId);

  SIZE_T FindStringInStringList(LPCWSTR sW);

private:
  CNktSQLiteConnector::eErrorCode QueryGetFieldTextAndAddToStringList(CNktSQLiteConnector &cDB,
             LPWSTR *szDestW, ULONG nQueryId, ULONG nColumnIndex, BOOL bMayBeNull);

  LPWSTR AddStringToStringList(LPCWSTR sW);

public:
  TNktArrayList4Structs<DB_MODULE,4096> aModuleList;
  TNktArrayList4Structs<DB_MODULE_FUNCTION,4096> aModuleFunctionList;
  TNktArrayList4Structs<DB_FUNCTION,4096> aFunctionList;
  TNktArrayList4Structs<DB_FUNCTION_ARG,4096> aFunctionArgList;
  TNktArrayList4Structs<DB_STRUCT,4096> aStructList;
  TNktArrayList4Structs<DB_STRUCT_MEMBER,4096> aStructMemberList;
  TNktArrayList4Structs<DB_STRUCT_BASE,4096> aStructBaseList;
  TNktArrayList4Structs<DB_UNION,4096> aUnionList;
  TNktArrayList4Structs<DB_UNION_MEMBER,4096> aUnionMemberList;
  TNktArrayList4Structs<DB_UNION_BASE,4096> aUnionBaseList;
  TNktArrayList4Structs<DB_TYPEDEF,4096> aTypeDefList;
  TNktArrayList4Structs<DB_ARRAY,4096> aArrayList;
  TNktArrayList4Structs<DB_POINTER,4096> aPointerList;
  TNktArrayList4Structs<DB_REFERENCE,4096> aReferenceList;
  TNktArrayList4Structs<DB_ENUM,4096> aEnumList;
  TNktArrayList4Structs<DB_ENUM_VALUE,4096> aEnumValueList;
  //----
  TNktArrayList4Structs<DB_BASIC_TYPE,4096> aBasicTypesList;
  //----
  TNktArrayListWithFree<LPWSTR, 1024> aStringsList;
};

//-----------------------------------------------------------

