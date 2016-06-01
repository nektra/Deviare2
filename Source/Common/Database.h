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

class CNktDvEngDatabase;
class CNktDvDbModule;
#include "EngDefines.h"
#include "DbObject.h"
#include "EngBaseObj.h"
#include "FnvHash.h"

//-----------------------------------------------------------

class CNktDvDbModuleNoRef : public CNktMemMgrObj
{
public:
  CNktDvDbModuleNoRef();

  ULONG GetId() const;
  virtual LPWSTR GetName() const;

  virtual SIZE_T GetFunctionsCount() const;
  virtual CNktDvDbObjectNoRef* GetFunctionItem(__in SIZE_T nIndex) const;

  virtual HRESULT GetFunctions(__deref_out CNktDvDbObjectsEnumerator **lplpEnum);

private:
  friend class CNktDvEngDatabase;
  friend class CNktDvDbModule;

  ULONG nId;
  LPWSTR szNameW;
  TNktArrayList<CNktDvDbObjectNoRef*,64> aFunctionsList;
  //----
  CNktDvEngDatabase* lpDvDB;
};

//-----------------------------------------------------------

class CNktDvDbModule : public CNktDvObject, public CNktDvDbModuleNoRef
{
public:
  CNktDvDbModule();

  static CNktDvDbModule* CreateFromNoRef(__in CNktDvDbModuleNoRef *lpBase);

private:
  TNktComPtr<CNktDvEngDatabase> cDvDB;
};

//-----------------------------------------------------------

class CNktDvEngDatabase : public CNktDvObject, private CNktFastMutex
{
public:
  CNktDvEngDatabase();
  virtual ~CNktDvEngDatabase();

  static HRESULT Get(__deref_out CNktDvEngDatabase **lplpDb, __in HANDLE hFileMap,
                     __in SIZE_T nPlatformBits);

  virtual ULONG __stdcall Release(); //override

  virtual HANDLE GetDbFileMapping() const;

  virtual ULONG GetModulesCount() const;
  virtual CNktDvDbModuleNoRef* GetModule(__in ULONG nModId) const;
  virtual CNktDvDbModuleNoRef* FindModuleByName(__in_nz_opt LPCWSTR szNameW,
                                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;
  virtual ULONG GetModuleFunctionsCount(__in CNktDvDbModuleNoRef *lpDbMod) const;
  virtual CNktDvDbObjectNoRef* GetModuleFunction(__in CNktDvDbModuleNoRef *lpDbMod,
                                                 __in ULONG nFuncId) const;
  virtual CNktDvDbObjectNoRef* FindModuleFunctionByName(__in CNktDvDbModuleNoRef *lpDbMod,
                                                        __in LPCWSTR szNameW,
                                                        __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetObjectsCount() const;
  virtual CNktDvDbObjectNoRef* GetObject(__in ULONG nObjId) const;
  virtual CNktDvDbObjectNoRef* FindObjectById(__in ULONG nId) const;
  virtual CNktDvDbObjectNoRef* FindObjectByName(__in_nz_opt LPCWSTR szNameW,
                                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual CNktDvDbObjectNoRef* GetFundamental(__in CNktDvDbObjectNoRef::eFundamentalType nFundType) const;

  virtual ULONG GetFunctionsCount() const;
  virtual CNktDvDbObjectNoRef* GetFunction(__in ULONG nFuncId) const;
  virtual CNktDvDbObjectNoRef* FindFunctionByName(__in_nz_opt LPCWSTR szNameW,
                                                  __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetStructsCount() const;
  virtual CNktDvDbObjectNoRef* GetStruct(__in ULONG nStructId) const;
  virtual CNktDvDbObjectNoRef* FindStructByName(__in_nz_opt LPCWSTR szNameW,
                                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetUnionsCount() const;
  virtual CNktDvDbObjectNoRef* GetUnion(__in ULONG nUnionId) const;
  virtual CNktDvDbObjectNoRef* FindUnionByName(__in_nz_opt LPCWSTR szNameW,
                                               __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetArraysCount() const;
  virtual CNktDvDbObjectNoRef* GetArray(__in ULONG nId) const;

  virtual ULONG GetPointersCount() const;
  virtual CNktDvDbObjectNoRef* GetPointer(__in ULONG nPtrId) const;
  virtual CNktDvDbObjectNoRef* FindPointerByName(__in_nz_opt LPCWSTR szNameW,
                                                 __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetReferencesCount() const;
  virtual CNktDvDbObjectNoRef* GetReference(__in ULONG nRefId) const;
  virtual CNktDvDbObjectNoRef* FindReferenceByName(__in_nz_opt LPCWSTR szNameW,
                                                   __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetEnumsCount() const;
  virtual CNktDvDbObjectNoRef* GetEnum(__in ULONG nEnumId) const;
  virtual CNktDvDbObjectNoRef* FindEnumByName(__in_nz_opt LPCWSTR szNameW,
                                              __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

  virtual ULONG GetFunctionTypesCount() const;
  virtual CNktDvDbObjectNoRef* GetFunctionType(__in ULONG nFuncId) const;
  virtual CNktDvDbObjectNoRef* FindFunctionTypeByName(__in_nz_opt LPCWSTR szNameW,
                                                      __in SIZE_T nNameLen=NKT_SIZE_T_MAX) const;

private:
  friend class CNktDvSpyMgr;
  friend class CNktDvDbObjectsEnumerator;

  static HRESULT CreateMapping(__out HANDLE *lphFileMap, __in_z LPCWSTR szDatabasePathW,
                               __in SIZE_T _nPlatformBits);

  virtual HRESULT Initialize(__in HANDLE hFileMap, __in SIZE_T nPlatformBits);
  virtual VOID Finalize();
  virtual CNktDvDbObjectNoRef* FindObjectByClassAndIndex(__in CNktDvDbObjectNoRef::eClass nClass,
                                                         __in ULONG nId);
  virtual CNktDvDbObjectNoRef* FindObjectByClassAndName(__in CNktDvDbObjectNoRef::eClass nClass,
                                                        __in_nz_opt LPCWSTR szNameW, __in SIZE_T nNameLen);

  static HRESULT ParseNamespaceAndName(__out LPCWSTR &szNamespaceW, __out SIZE_T &nNamespaceLen,
                                       __inout LPCWSTR &szNameW, __inout SIZE_T &nNameLen);

private:
  LPWSTR szSuperStringW;
  CNktDvDbModuleNoRef *lpDbModulesList;
  SIZE_T nDbModulesListCount;
  CNktDvDbObjectNoRef *lpDbObjectsList;
  SIZE_T nDbObjectsListCount;
  TNktArrayList4Structs<CNktDvDbObjectNoRef::CHILD_ITEM> aDbObjectChildsList;
  TNktAutoFreePtr<ULONG> cDbModuleNameIndex, cDbObjectNameIndex;
  TNktAutoFreePtr<ULONG> cDbObjectClassAndNameIndex;
  CNktDvDbObjectNoRef *lpDboFundamentals[CNktDvDbObjectNoRef::ftMAX-1];
  struct tagObjectClassPos {
    ULONG nStart, nCount;
  } sDbObjectClassPos[CNktDvDbObjectNoRef::clsMAX+1];
  SIZE_T nPlatformBits;
  //----
  HANDLE hDbMapFile;
  LPBYTE lpDbBuffer;
};

//-----------------------------------------------------------

class CNktDvDbModulesEnumerator : public TNktDvEnumerator<CNktDvDbModule,32>
{
public:
  CNktDvDbModulesEnumerator() : TNktDvEnumerator<CNktDvDbModule,32>()
    {
    return;
    };

  virtual HRESULT GetItemById(__deref_out CNktDvDbModule** lplpDbMod, __in SIZE_T nId);
  virtual HRESULT GetItemByName(__deref_out CNktDvDbModule** lplpDbMod, __in_nz_opt LPCWSTR szNameW,
                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  friend class CNktDvSpyMgr;
  friend class CNktDvDbObjectNoRef;

  virtual HRESULT Initialize(__in CNktDvEngDatabase *lpDvDB);

private:
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  TNktAutoFreePtr<ULONG> cNameIndexList;
};

//-----------------------------------------------------------

