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
#include "SQLiteDbLoader.h"
#include "..\..\Common\MemoryManager.h"
#include "..\..\Common\DbObject.h"
#include "..\..\Common\FnvHash.h"

//-----------------------------------------------------------

class CDvDbCreator : public CNktMemMgrObj
{
public:
  CDvDbCreator();
  ~CDvDbCreator();

  BOOL BuildObjects(CSQLiteDbLoader &cLdr);
  BOOL DoSomeFixes();
  VOID BuildHashes();
  BOOL SaveDatabase(LPCWSTR szDestFileNameW, CSQLiteDbLoader &cLdr);

private:
  typedef struct {
    ULONG nTypeId;
    ULONG nTypeClass;
    CNktDvDbObject *lpObj;
    ULONG nFlags;
  } SRC_DBO_MAPPING;

  VOID BuildHashes_GetObjectHash(SIZE_T nIndex, SIZE_T nDepth, Fnv64_t &nHashValue);

  BOOL AddDbData(LPVOID lpData, SIZE_T nDataSize, LPBYTE *lplpDest, PSIZE_T lpnDestLen,
                 PSIZE_T lpnDestSize);
  HRESULT WriteDbData(HANDLE hFile, LPVOID lpData, SIZE_T nDataSize);
  CNktDvDbObject* AddDbObject();

  SRC_DBO_MAPPING* DboMap_Find(ULONG nTypeId, ULONG nTypeClass);
  static int DboMap_Compare(void *ctx, const void *lp1, const void *lp2);
  BOOL BO_ProcessStruct(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId, ULONG nTypeClass);
  BOOL BO_ProcessUnion(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId, ULONG nTypeClass);
  BOOL BO_ProcessArray(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId, ULONG nTypeClass);
  BOOL BO_ProcessPointer(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId, ULONG nTypeClass);
  BOOL BO_ProcessReference(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                           ULONG nTypeClass);
  BOOL BO_ProcessEnumeration(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId,
                             ULONG nTypeClass);
  BOOL BO_ProcessFunction(CNktDvDbObject *lpDbObj, CSQLiteDbLoader &cLdr, ULONG nTypeId, ULONG nTypeClass);
  VOID MapStructUnionFlags(CNktDvDbObject *lpDbObj, ULONG nFlags);
  VOID MapFunctionFlags(CNktDvDbObject *lpDbObj, ULONG nFlags);

private:
  TNktArrayListWithRelease<CNktDvDbObject*,32768> aObjectsList;
  TNktArrayList4Structs<SRC_DBO_MAPPING> aMappingsList;
  TNktAutoFreePtr<WCHAR> cSuperStringW;
  ULONG nSuperStringSize;
  TNktArrayList<SIZE_T> aStringOffsetsList;
};

//-----------------------------------------------------------

