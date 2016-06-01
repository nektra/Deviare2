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

#include "EngBaseObj.h"
#include "MemoryManager.h"
#include "ArrayList.h"
#include "StringLiteW.h"

#ifndef NKT_SIZE_T_MAX
  #define NKT_SIZE_T_MAX  ((SIZE_T)(-1))
#endif //NKT_SIZE_T_MAX

#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

//-----------------------------------------------------------

class CNktDvEngDatabase;
class CNktDvDbObjectsEnumerator;
class CNktDvDbModule;
class CNktDvDbModuleNoRef;
class CNktDvDbModulesEnumerator;

class CNktDvDbObjectNoRef : public CNktMemMgrObj
{
public:
  typedef enum {
    ftNone=0, ftSignedByte, ftUnsignedByte, ftSignedWord, ftUnsignedWord,
    ftSignedDoubleWord, ftUnsignedDoubleWord, ftSignedQuadWord, ftUnsignedQuadWord,
    ftFloat, ftDouble, ftLongDouble, ftVoid, ftAnsiChar, ftWideChar,
    ftMAX //DON'T use this value. it serves at a fundamentals count
  } eFundamentalType;

  typedef enum {
    clsNull=-1, clsFundamental=0, clsStruct, clsUnion, clsArray, clsPointer, clsReference,
    clsEnumeration, clsFunction, clsFunctionType,
    clsClassConstructor, clsClassDestructor, clsClassOperatorMethod, clsClassMethod,
    clsClassConverter,
    clsMAX //DON'T use this value. it serves at a classes count
  } eClass;

  typedef enum {
    ccNA=-1, ccStdCall=0, ccCDecl=1, ccFastCall=2, ccThisCall=3
  } eCallingConvention;

  typedef enum {
    fflgIsPure=0x10, fflgIsThrow=0x20, fflgIsNoThrow=0x40, fflgIsNoReturn=0x80,
    fflgIsConst=0x100, fflgIsDeprecated=0x200, fflgIsNonNull=0x400, fflgIsMalloc=0x800,
    fflgIsExternal=0x1000, fflgIsDllImport=0x2000, fflgIsDllExport=0x4000, fflgIsFormat=0x8000
  } eFunctionFlags;

  typedef enum {
    suflgHasConstructor=0x10, suflgHasDestructor=0x20,
    suflgHasVirtual=0x40, suflgHasInheritance=0x80
  } eStructUnionFlags;

  typedef enum {
    iflgIsVolatile=0x1, iflgIsConstant=0x2
  } eItemFlags;

  typedef enum {
    mtNA=-1, mtPublic=0x10, mtPrivate=0x20, mtProtected=0x40
  } eItemMemberType;

  typedef enum {
    stNone=0, stAnsi, stWide
  } eStringType;

  CNktDvDbObjectNoRef();
  virtual ~CNktDvDbObjectNoRef();

  virtual ULONG GetId() const;

  virtual LPWSTR GetNamespace() const;

  virtual eClass GetClass() const;
  virtual eFundamentalType GetFundamentalType() const;

  virtual LPWSTR GetDeclaration() const; //returns NULL on error

  virtual LPWSTR GetName() const;

  virtual SIZE_T GetSize() const;
  virtual SIZE_T GetAlign() const;

  virtual BOOL IsArray() const;
  virtual SIZE_T GetArrayMaxHint() const;

  virtual eStringType GetStringType() const;
  virtual BOOL IsAnsiString() const
    {
    return (GetStringType() == CNktDvDbObjectNoRef::stAnsi) ? TRUE : FALSE;
    };
  virtual BOOL IsWideString() const
    {
    return (GetStringType() == CNktDvDbObjectNoRef::stWide) ? TRUE : FALSE;
    };

  virtual BOOL IsGuid() const;

  virtual CNktDvDbObjectNoRef* GetEvaluated(__in SIZE_T nArrayIndex=0) const;
  virtual CNktDvDbObjectNoRef* GetFullEvaluated() const;

  virtual int GetFlags() const; //for structs, unions, functions & function types

  virtual SIZE_T GetItemsCount() const;
  virtual SIZE_T NameToIndex(__in LPCWSTR szNameW) const;
  virtual HRESULT GetItems(__deref_out CNktDvDbObjectsEnumerator **lplpEnum);
  virtual CNktDvDbObjectNoRef* GetItem(__in SIZE_T nIndex) const;
  virtual LPWSTR GetItemName(__in SIZE_T nIndex) const;
  virtual int GetItemFlags(__in SIZE_T nIndex) const;
  virtual eItemMemberType GetItemMemberType(__in SIZE_T nIndex) const;
  virtual SIZE_T GetItemStructOffset(__in SIZE_T nIndex) const;
  virtual SIZE_T GetItemStructBits(__in SIZE_T nIndex) const;
  virtual SIZE_T GetItemEnumValue(__in SIZE_T nIndex) const;
  virtual LPWSTR GetItemDeclaration(__in SIZE_T nIndex) const; //returns NULL on error

  virtual eCallingConvention GetFuncCallingConvention() const;

  virtual CNktDvDbObjectNoRef* GetFunctionReturn() const;
  virtual int GetFunctionReturnFlags() const;
  virtual LPWSTR GetFunctionReturnDeclaration() const; //returns NULL on error

  static SIZE_T GetFundamentalTypeSize(__in eFundamentalType nFundType);

  virtual HRESULT GetModules(__deref_out CNktDvDbModulesEnumerator **lplpEnum);

private:
  friend class CNktDvEngDatabase;
#ifdef NKT_DV_DBGENERATOR
  friend class CDvDbCreator;
#endif //NKT_DV_DBGENERATOR
#ifdef NKT_DV_DBMERGE
  friend class CDbMergeDvDatabase;
  friend class CNktDvDbMerge;
#endif //NKT_DV_DBGENERATOR
#ifdef NKT_DV_DB2SQLITE
  friend class CDb2SQLiteDvDatabase;
  friend class CNktDvDb2SQLite;
#endif //NKT_DV_DBGENERATOR

  friend class CNktDvDbObject;

#pragma pack(1)
  typedef struct {
    ULONG nId;
    ULONG nClass;
    ULONG nFundamentalType;
    ULONG nNameOffset;
    ULONG nSize, nAlign;
    ULONG nArraySizeHint;
    ULONG nStructUnionFunctionFlags;
    ULONG nNamespaceOffset;
    ULONGLONG nHashValue;
  } STORED_DATA, *LPSTORED_DATA;

  typedef struct {
    ULONG nDbObjectId;
    ULONG nNameOffset;
    ULONG nStructOffset, nStructBits;
    LONG nEnumValue;
    ULONG nFlags;
  } STORED_CHILD_ITEM, *LPSTORED_CHILD_ITEM;

  typedef struct {
    ULONG nId;
    ULONG nNameOffset;
  } STORED_MODULE, *LPSTORED_MODULE;

  typedef struct {
    CNktDvDbObjectNoRef *lpObject;
    STORED_CHILD_ITEM NKT_UNALIGNED *lpStoredChildItem;
    LPWSTR szNameW;
    LPWSTR volatile szDeclarationW;
  } CHILD_ITEM, *LPCHILD_ITEM;
#pragma pack()

  virtual VOID BuildDeclaration();
  virtual VOID BuildItemDeclaration(__in SIZE_T nIndex);
  //----
  virtual HRESULT BuildDefinitionString(__inout CNktStringW &cStrDeclW,
                                        __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuildChildDefinitionString(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex);
  virtual HRESULT GetFundamentalDefStr(__inout CNktStringW &cStrDeclW,
                                       __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldStructDefStr(__inout CNktStringW &cStrDeclW,
                                     __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldFunctionDefStr(__inout CNktStringW &cStrDeclW,
                                       __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldEnumDefStr(__inout CNktStringW &cStrDeclW,
                                   __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuildArgumentStr(__inout CNktStringW &cStrDeclW,
                                   __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldStructArgStr(__inout CNktStringW &cStrDeclW,
                                     __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldPointerArgStr(__inout CNktStringW &cStrDeclW,
                                      __in const CNktDvDbObjectNoRef *lpDbObj, __in BOOL bIsRef);
  virtual HRESULT BuilldArrayArgStr(__inout CNktStringW &cStrDeclW,
                                    __in const CNktDvDbObjectNoRef *lpDbObj);
  virtual HRESULT BuilldStructChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex);
  virtual HRESULT BuilldFunctionChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex);
  virtual HRESULT BuilldEnumChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex);

private:
  STORED_DATA NKT_UNALIGNED *lpStoredData;
  LPWSTR szNamespaceW;
  LPWSTR szNameW;
  LPWSTR volatile szDeclW;
  CHILD_ITEM *lpFirstChild;
  SIZE_T nChildsCount;
  TNktArrayList<CNktDvDbModuleNoRef*,8> aModulesList;
  //----
#ifdef NKT_DV_DBGENERATOR
#pragma pack(1)
  typedef struct {
    CNktDvDbObject *lpObject;
    ULONG nDbObjectId;
    LPWSTR szNameW;
    ULONG nStructOffset, nStructBits;
    LONG nEnumValue;
    ULONG nFlags;
    LPWSTR szDeclarationW;
  } DBGEN_CHILD_ITEM, *LPDBGEN_CHILD_ITEM;
#pragma pack()

  struct tagCreationHelpers {
    ULONG nId, nClass, nFundamentalType, nSize, nAlign;
    ULONG nArraySizeHint, nStructUnionFunctionFlags;
    ULONG nStructOffset, nStructBits, nEnumValue, nFlags;
    LPWSTR szNameW;
    LPWSTR szNamespaceW;
    TNktArrayList4Structs<DBGEN_CHILD_ITEM,8> aChildsList;
    ULONGLONG nHashValue;
  } sCreationHelpers;
#endif //NKT_DV_DBGENERATOR
  //----
  CNktDvEngDatabase* lpDvDB;
};

//-----------------------------------------------------------

class CNktDvDbObject : public CNktDvObject, public CNktDvDbObjectNoRef
{
public:
  CNktDvDbObject();

  static CNktDvDbObject* CreateFromNoRef(__in CNktDvDbObjectNoRef *lpBase);

private:
  TNktComPtr<CNktDvEngDatabase> cDvDB;
};

//-----------------------------------------------------------

class CNktDvDbObjectsEnumerator : public TNktDvEnumerator<CNktDvDbObject,512>
{
public:
  CNktDvDbObjectsEnumerator();
  virtual ~CNktDvDbObjectsEnumerator();

  virtual HRESULT GetItemById(__deref_out CNktDvDbObject** lplpDbObj, __in SIZE_T nId);
  virtual HRESULT GetItemByName(__deref_out CNktDvDbObject** lplpDbObj, __in_nz_opt LPCWSTR szNameW,
                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  friend class CNktDvSpyMgr;
  friend class CNktDvDbModule;
  friend class CNktDvDbModuleNoRef;
  friend class CNktDvDbObjectNoRef;

  virtual HRESULT Initialize(__in CNktDvEngDatabase *lpDvDB, __in BOOL bOnlyFunctions);

private:
  TNktAutoFreePtr<ULONG> cNameIndexList;
  //----
  CNktDvEngDatabase* lpDvDB;
};

//-----------------------------------------------------------

