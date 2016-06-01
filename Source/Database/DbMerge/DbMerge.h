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

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "..\..\Common\StringLiteW.h"
#include "..\..\Common\ArrayList.h"
#include "..\..\Common\ObjectEnums.h"
#include "..\..\Common\Database.h"
#include "..\..\SupportLibs\Lz4\lz4.h"

//-----------------------------------------------------------

#define ARRAY_LEN(x)                (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

class CDbMergeDvDatabase
{
public:
  TNktAutoFreePtr<BYTE> aDecompressedData;
  SIZE_T nDecompressedSize;
  //----
  LPWSTR szSuperStringW;
  SIZE_T nSuperStringSize;
  //----
  SIZE_T nDbObjectsListCount;
  TNktAutoFreePtr<CNktDvDbObjectNoRef::LPSTORED_DATA> aStoredDbObjects;
  TNktAutoFreePtr<SIZE_T> aStoredDbObjects_ChildsCount;
  TNktAutoFreePtr<CNktDvDbObjectNoRef::LPSTORED_CHILD_ITEM> aStoredDbObjects_FirstChild;
  //----
  SIZE_T nDbModulesListCount;
  TNktAutoFreePtr<CNktDvDbObjectNoRef::LPSTORED_MODULE> aStoredDbModules;
  //----
  SIZE_T nDbModuleFunctionsCount;
  TNktAutoFreePtr<ULONG> aStoredDbModuleFunctions;
  //----
  TNktAutoFreePtr<SIZE_T> aStoredDbObjects_RemappedIndex;
  TNktAutoFreePtr<SIZE_T> aStoredDbModules_RemappedIndex;

  //output
  TNktAutoFreePtr<CNktDvDbObjectNoRef::STORED_DATA> aOutputDbObjects;
  TNktAutoFreePtr<CNktDvDbObjectNoRef::STORED_CHILD_ITEM> aOutputDbObjectChilds;
  TNktAutoFreePtr<CNktDvDbObjectNoRef::STORED_MODULE> aOutputDbModules;
  TNktAutoFreePtr<ULONG> aOutputDbModuleFunctions;
  TNktAutoFreePtr<WCHAR> szOutputSuperStringW;
};

//-----------------------------------------------------------

extern ULONG nProcessingBits;
extern CNktStringW cStrAppPathW;
extern CNktStringW cStrOutputDbW, cStrOutputPathW;
extern TNktArrayListWithFree<LPWSTR> aInputDatabases;
extern TNktArrayListWithFree<LPWSTR> aInputNamespaces;
extern TNktArrayListWithDelete<CDbMergeDvDatabase*> aInputDbs;
extern CDbMergeDvDatabase cMergedDb;

//-----------------------------------------------------------

VOID PrintAndLogNoMemoryError();
VOID PrintAndLog(LPCWSTR szFormatW, ...);
VOID PrintAndLogSeparator();
VOID LogOnly(LPCWSTR szFormatW, ...);

BOOL IsAbortKeyPressed();

//-----------------------------------------------------------

class CNktDvDbMerge
{
public:
  typedef TNktArrayListWithFree<LPWSTR, 1024> WSTRING_LIST;

  static BOOL LoadInputDbs();
  static BOOL BuldMerged();
  static BOOL SaveMerged(LPCWSTR szTargetFileW);

private:
  static LPWSTR AddStringToStringList(WSTRING_LIST &aStringsList, LPCWSTR sW);
  static SIZE_T FindStringInStringList(WSTRING_LIST &aStringsList, LPCWSTR sW);
  static int StringList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2);
};

//-----------------------------------------------------------

