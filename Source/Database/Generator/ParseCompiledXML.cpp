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

#include "ParseCompiledXML.h"
#include "ConfigLdr.h"
#include <conio.h>
#include "..\..\Common\EngDefines.h"

//-----------------------------------------------------------

#define X_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))

typedef struct _OBJ_ID {
  ULONG nId;
  BOOL bIsConstant;
  BOOL bIsVolatile;
} OBJ_ID;

typedef struct {
  ULONG nStructId;
  ULONG nPointerToStructId;
} PTR_2_STRUCT;

typedef struct {
  ULONG nStructId;
  ULONG nMethodId;
} STRUCT_METHOD;

//-----------------------------------------------------------

static TNktArrayList4Structs<PTR_2_STRUCT> aPointersToStructs;
static TNktArrayList4Structs<STRUCT_METHOD> aStructMethods;

//-----------------------------------------------------------

static BOOL ParseXML_FunctionAndFunctionType(IXMLDOMElement *lpFuncElem, OBJ_ID &sFuncId,
                                             SIZE_T nFuncClass);
static BOOL ParseXML_StructAndUnion(IXMLDOMElement *lpStructElem, OBJ_ID &sStructId, BOOL bIsUnion);
static BOOL ParseXML_Typedef(IXMLDOMElement *lpTypeDefElem, OBJ_ID &);
static BOOL ParseXML_Array(IXMLDOMElement *lpArrayElem, OBJ_ID &sArrayId);
static BOOL ParseXML_PointerAndReference(IXMLDOMElement *lpPointerElem, OBJ_ID &sPointerId,
                                         BOOL bIsReference);
static BOOL ParseXML_Enumeration(IXMLDOMElement *lpEnumElem, OBJ_ID &sEnumId);
static VOID CheckFundamentalTypeAndClass(OBJ_ID &sFuncId, LONG nClass);
static BOOL ParseId(OBJ_ID &sDestObjId, LPCWSTR szIdW);
static BOOL ParseLong(LONG &lValue, LPCWSTR szStrW);
static int FunctionsList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2);
static BOOL ParseXML_Replace__lpThis999999999(SIZE_T &nMaxId);
static int __cdecl PointersToStructs_InsertById(__in void *, __in PTR_2_STRUCT *lpElem1,
                                                __in PTR_2_STRUCT *lpElem2);
static int __cdecl PointersToStructs_SearchById(void *lpContext, const void *lpKey, const void *elem);
static int __cdecl PointersToStructs_SearchById_StructOnly(void *lpContext, const void *lpKey,
                                                           const void *elem);
static int __cdecl StructMethods_InsertById(__in void *, __in STRUCT_METHOD *lpElem1,
                                            __in STRUCT_METHOD *lpElem2);
static int __cdecl StructMethods_SearchById(void *lpContext, const void *lpKey, const void *elem);

//-----------------------------------------------------------

BOOL ParseCompiledXML()
{
  TNktComPtr<IXMLDOMElement> cCurrElem, cNextElem;
  OBJ_ID sElemId;
  CNktComBStr cTempBStr;
  HRESULT hRes;
  BOOL b, bInTransaction;
  ULONG i, nIdx;
  CNktSQLiteConnector::eErrorCode nDbErr;
  WCHAR szBufW[24];
  SIZE_T nMaxId;

  nIdx = 0;
  nMaxId = 0;
  bInTransaction = FALSE;
  hRes = CXmlHelper::GetFirstChildElement(cXmlRootElem, &cCurrElem);
  if (FAILED(hRes))
  {
pxml_on_error:
    if (bInTransaction != FALSE)
      nDbErr = cDB.TransactionRollback();
    if (hRes == E_OUTOFMEMORY)
      PrintAndLogNoMemoryError();
    else
      PrintAndLog(L"\nError: Cannot parse XML (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  while (cCurrElem != NULL)
  {
    //check for abort
    if (IsAbortKeyPressed() != FALSE)
      return FALSE;
    //get element id
    cTempBStr.Reset();
    hRes = CXmlHelper::GetAttributeValue(cCurrElem, L"id", &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_on_error;
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot get element id (0x%08X).\n", (ULONG)hRes);
      return FALSE;
    }
    if (cTempBStr[0] == L'_')
    { //only parse id's starting with underscore
      if (ParseId(sElemId, cTempBStr) == FALSE)
      {
        if (bInTransaction != FALSE)
          nDbErr = cDB.TransactionRollback();
        PrintAndLog(L"\nError: Invalid element id (%s).\n", (LPWSTR)cTempBStr);
        return FALSE;
      }
      if (sElemId.nId > nMaxId)
        nMaxId = sElemId.nId;
      //get element name
      cTempBStr.Reset();
      hRes = CXmlHelper::GetElementName(cCurrElem, &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_on_error;
      if (FAILED(hRes))
      {
        if (bInTransaction != FALSE)
          nDbErr = cDB.TransactionRollback();
        PrintAndLog(L"\nError: Cannot get element name (0x%08X) [id:%lu].\n", (ULONG)hRes, sElemId.nId);
        return FALSE;
      }
      swprintf_s(szBufW, 24, L"(%lu)", sElemId.nId);
      wprintf_s(L"%s", szBufW);
      for (i=0; szBufW[i]!=0; i++)
        szBufW[i] = L'\b';
      wprintf_s(L"%s", szBufW);
      //----
      if ((nIdx & 31) == 0)
      {
        if (bInTransaction != FALSE)
        {
          nDbErr = cDB.TransactionCommit();
          if (nDbErr != CNktSQLiteConnector::noError)
          {
            if (nDbErr == CNktSQLiteConnector::errNoMem)
              PrintAndLogNoMemoryError();
            else
              PrintAndLog(L"\nError: Cannot execute query [COMMIT TRANS] (%lu).\n", (ULONG)nDbErr);
            return FALSE;
          }
          bInTransaction = FALSE;
        }
        nDbErr = cDB.TransactionStart();
        if (nDbErr != CNktSQLiteConnector::noError)
        {
          if (nDbErr == CNktSQLiteConnector::errNoMem)
            PrintAndLogNoMemoryError();
          else
            PrintAndLog(L"\nError: Cannot execute query [BEGIN TRANS] (%lu).\n", (ULONG)nDbErr);
          return FALSE;
        }
        bInTransaction = TRUE;
      }
      nIdx++;
      //----
      if (_wcsicmp(cTempBStr, L"function") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_Function);
      else if (_wcsicmp(cTempBStr, L"Struct") == 0 ||
               _wcsicmp(cTempBStr, L"Class") == 0)
        b = ParseXML_StructAndUnion(cCurrElem, sElemId, FALSE);
      else if (_wcsicmp(cTempBStr, L"Union") == 0)
        b = ParseXML_StructAndUnion(cCurrElem, sElemId, TRUE);
      else if (_wcsicmp(cTempBStr, L"Typedef") == 0)
        b = ParseXML_Typedef(cCurrElem, sElemId);
      else if (_wcsicmp(cTempBStr, L"ArrayType") == 0)
        b = ParseXML_Array(cCurrElem, sElemId);
      else if (_wcsicmp(cTempBStr, L"PointerType") == 0)
        b = ParseXML_PointerAndReference(cCurrElem, sElemId, FALSE);
      else if (_wcsicmp(cTempBStr, L"ReferenceType") == 0)
        b = ParseXML_PointerAndReference(cCurrElem, sElemId, TRUE);
      else if (_wcsicmp(cTempBStr, L"Enumeration") == 0)
        b = ParseXML_Enumeration(cCurrElem, sElemId);
      else if (_wcsicmp(cTempBStr, L"FunctionType") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_FunctionType);
      else if (_wcsicmp(cTempBStr, L"Constructor") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_ClassConstructor);
      else if (_wcsicmp(cTempBStr, L"Destructor") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_ClassDestructor);
      else if (_wcsicmp(cTempBStr, L"OperatorMethod") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_ClassOperatorMethod);
      else if (_wcsicmp(cTempBStr, L"Method") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_ClassMethod);
      else if (_wcsicmp(cTempBStr, L"Converter") == 0)
        b = ParseXML_FunctionAndFunctionType(cCurrElem, sElemId, NKT_DBOBJCLASS_ClassConverter);
      else
        b = TRUE;
      if (b == FALSE)
      {
        if (bInTransaction != FALSE)
          nDbErr = cDB.TransactionRollback();
        return FALSE;
      }
      wprintf(L"                        \b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cCurrElem, &cNextElem);
    if (FAILED(hRes))
      goto pxml_on_error;
    cCurrElem.Attach(cNextElem.Detach());
  }
  if (bInTransaction != FALSE)
  {
    nDbErr = cDB.TransactionCommit();
    if (nDbErr != CNktSQLiteConnector::noError)
    {
      if (nDbErr == CNktSQLiteConnector::errNoMem)
        PrintAndLogNoMemoryError();
      else
        PrintAndLog(L"\nError: Cannot execute query [COMMIT TRANS] (%lu).\n", (ULONG)nDbErr);
      return FALSE;
    }
  }
  //replace all '__lpThis',999999999 with real pointers
  if (ParseXML_Replace__lpThis999999999(nMaxId) == FALSE)
    return FALSE;
  aPointersToStructs.RemoveAllElements();
  aStructMethods.RemoveAllElements();
  return TRUE;
}

static BOOL ParseXML_FunctionAndFunctionType(IXMLDOMElement *lpFuncElem, OBJ_ID &sFuncId,
                                             SIZE_T nFuncClass)
{
  CNktComBStr cTempBStr, cNameBStr;
  TNktComPtr<IXMLDOMElement> cArgElem, cNextElem;
  IXMLDOMElement *lpElem;
  OBJ_ID sFuncRetId, sFuncArgTypeId;
  HRESULT hRes;
  CNktStringW cStrTempW;
  LONG nFlags, nArgIdx, nRetClass, nArgTypeClass;
  CNktSQLiteConnector::eErrorCode nDbErr;
  LPWSTR sW;

  if (nFuncClass == NKT_DBOBJCLASS_Function)
  {
    //if "normal" function, check if it is inline
    cTempBStr.Reset();
    hRes = CXmlHelper::GetAttributeValue(lpFuncElem, L"inline", &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
    {
pxml_func_err_nomem:
      PrintAndLogNoMemoryError();
      return FALSE;
    }
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot check if function is inline (0x%08X) [id:%lu].\n", (ULONG)hRes,
                  sFuncId.nId);
      return FALSE;
    }
    if (_wcsicmp(cTempBStr, L"1") == 0)
      return TRUE; //inline functions are not processed
  }
  //----
  nFlags = 0;
  if (nFuncClass != NKT_DBOBJCLASS_FunctionType)
  {
    //get function name
    hRes = CXmlHelper::GetAttributeValue(lpFuncElem, (nFuncClass == NKT_DBOBJCLASS_Function) ?
                                                         L"name" : L"demangled", &cNameBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_func_err_nomem;
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot get function name (0x%08X) [id:%lu].\n", (ULONG)hRes, sFuncId.nId);
      return FALSE;
    }
    if (cNameBStr[0] == 0)
    {
      PrintAndLog(L"\nError: Invalid function name [id:%lu].\n", sFuncId.nId);
      return FALSE;
    }
    //check if class functions has the double colon separator
    if (nFuncClass == NKT_DBOBJCLASS_ClassConstructor ||
        nFuncClass == NKT_DBOBJCLASS_ClassDestructor ||
        nFuncClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
        nFuncClass == NKT_DBOBJCLASS_ClassMethod ||
        nFuncClass == NKT_DBOBJCLASS_ClassConverter)
    {
      sW = wcsstr(cNameBStr, L"::");
      if (sW == NULL || sW == cNameBStr || sW[2] == 0)
        return TRUE; //ignore non-class functions
    }
    //is function external
    cTempBStr.Reset();
    hRes = CXmlHelper::GetAttributeValue(lpFuncElem, L"extern", &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_func_err_nomem;
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot check if function is external (0x%08X) [id:%lu].\n", (ULONG)hRes,
                  sFuncId.nId);
      return FALSE;
    }
    if (_wcsicmp(cTempBStr, L"1") == 0)
      nFlags |= NKT_DBOBJFLAG_IsExternal;
  }
  //check attributes
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpFuncElem, L"attributes", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_func_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot check function attributes (0x%08X) [id:%lu].\n", (ULONG)hRes,
                sFuncId.nId);
    return FALSE;
  }
  for (sW=cTempBStr; *sW!=NULL; )
  {
    if (_wcsnicmp(sW, L"dllimport", 9) == 0 && (sW[9]==32 || sW[9]==0))
    {
      nFlags |= NKT_DBOBJFLAG_IsDllImport;
      sW += 9;
    }
    else if (_wcsnicmp(sW, L"dllexport", 9) == 0 && (sW[9]==32 || sW[9]==0))
    {
      nFlags |= NKT_DBOBJFLAG_IsDllExport;
      sW += 9;
    }
    else if (_wcsnicmp(sW, L"__stdcall__", 11) == 0 && (sW[11]==32 || sW[11]==0))
    {
      nFlags |= NKT_DBOBJFLAG_StdCall;
      sW += 11;
    }
    else if (_wcsnicmp(sW, L"__cdecl__", 9) == 0 && (sW[9]==32 || sW[9]==0))
    {
      nFlags |= NKT_DBOBJFLAG_CDecl;
      sW += 9;
    }
    else if (_wcsnicmp(sW, L"pure", 4) == 0 && (sW[4]==32 || sW[4]==0))
    {
      nFlags |= NKT_DBOBJFLAG_IsPure;
      sW += 4;
    }
    else if (_wcsnicmp(sW, L"throw", 5) == 0 && (sW[5]==32 || sW[5]==0))
    {
      nFlags |= NKT_DBOBJFLAG_Throw;
      sW += 5;
    }
    else if (_wcsnicmp(sW, L"nothrow", 7) == 0 && (sW[7]==32 || sW[7]==0))
    {
      nFlags |= NKT_DBOBJFLAG_NoThrow;
      sW += 7;
    }
    else if (_wcsnicmp(sW, L"noreturn", 8) == 0 && (sW[8]==32 || sW[8]==0))
    {
      nFlags |= NKT_DBOBJFLAG_NoReturn;
      sW += 8;
    }
    else if (_wcsnicmp(sW, L"const", 5) == 0 && (sW[5]==32 || sW[5]==0))
    {
      nFlags |= NKT_DBOBJFLAG_IsConst;
      sW += 5;
    }
    else if (_wcsnicmp(sW, L"deprecated", 10) == 0 && (sW[10]==32 || sW[10]==0))
    {
      nFlags |= NKT_DBOBJFLAG_Deprecated;
      sW += 10;
    }
    else if (_wcsnicmp(sW, L"nonnull", 7) == 0 && (sW[7]==32 || sW[7]==0))
    {
      nFlags |= NKT_DBOBJFLAG_NonNull;
      sW += 7;
    }
    else if (_wcsnicmp(sW, L"nonnull()", 9) == 0 && (sW[9]==32 || sW[9]==0))
    {
      nFlags |= NKT_DBOBJFLAG_NonNull;
      sW += 9;
    }
    else if (_wcsnicmp(sW, L"malloc", 6) == 0 && (sW[6]==32 || sW[6]==0))
    {
      nFlags |= NKT_DBOBJFLAG_Malloc;
      sW += 6;
    }
    else if (_wcsnicmp(sW, L"format", 6) == 0 && (sW[6]==32 || sW[6]==0))
    {
      nFlags |= NKT_DBOBJFLAG_Format;
      sW += 6;
    }
    else if (_wcsnicmp(sW, L"format(,,)", 10) == 0 && (sW[10]==32 || sW[10]==0))
    {
      nFlags |= NKT_DBOBJFLAG_Format;
      sW += 10;
    }
    else
    {
      PrintAndLog(L"\nError: Invalid function attributes (%s) [id:%lu].\n", cTempBStr, sFuncId.nId);
      return FALSE;
    }
    if (*sW != 0)
      sW++;
  }
  //check if this function "ends" with an ellipsis parameter
  hRes = CXmlHelper::GetFirstChildElement(lpFuncElem, &cArgElem);
  if (FAILED(hRes))
  {
pxml_func_cant_get_arg:
    if (hRes == E_OUTOFMEMORY)
      goto pxml_func_err_nomem;
    PrintAndLog(L"\nError: Cannot get function arguments (0x%08X) [id:%lu].\n", (ULONG)hRes, sFuncId.nId);
    return FALSE;
  }
  while (cArgElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cArgElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_func_err_nomem;
    if (FAILED(hRes))
    {
pxml_func_cant_get_argname:
      PrintAndLog(L"\nError: Cannot get function's argument name (0x%08X) [id:%lu].\n", (ULONG)hRes,
                  sFuncId.nId);
      return FALSE;
    }
    if (_wcsicmp(cTempBStr, L"ellipsis") == 0)
    {
      //got an ellipsis so convert a StdCall into a CDecl
      nFlags &= (~NKT_DBOBJFLAG_CALLINGTYPE_MASK);
      nFlags |= NKT_DBOBJFLAG_CDecl;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cArgElem, &cNextElem);
    if (FAILED(hRes))
      goto pxml_func_cant_get_arg;
    cArgElem.Attach(cNextElem.Detach());
  }
  cArgElem.Release();
  cNextElem.Release();
  //constructors, methods and so on are thiscall unless other specified
  if (nFuncClass == NKT_DBOBJCLASS_ClassConstructor ||
      nFuncClass == NKT_DBOBJCLASS_ClassDestructor ||
      nFuncClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
      nFuncClass == NKT_DBOBJCLASS_ClassMethod ||
      nFuncClass == NKT_DBOBJCLASS_ClassConverter)
  {
    if ((nFlags & NKT_DBOBJFLAG_CALLINGTYPE_MASK) == NKT_DBOBJFLAG_StdCall)
    {
      nFlags &= (~NKT_DBOBJFLAG_CALLINGTYPE_MASK);
      nFlags |= NKT_DBOBJFLAG_ThisCall;
    }
  }
  //check return type
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpFuncElem, L"returns", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_func_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get function return (0x%08X) [id:%lu].\n", (ULONG)hRes, sFuncId.nId);
    return FALSE;
  }
  //class methods can "return" nothing
  if (cTempBStr[0] == 0 &&
      (nFuncClass == NKT_DBOBJCLASS_ClassConstructor ||
       nFuncClass == NKT_DBOBJCLASS_ClassDestructor ||
       nFuncClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
       nFuncClass == NKT_DBOBJCLASS_ClassMethod ||
       nFuncClass == NKT_DBOBJCLASS_ClassConverter))
  {
    if (FAILED(cTempBStr.Set(cXmlHelper.GetIdFromFundamentalType(NKT_DBFUNDTYPE_Void))))
      goto pxml_func_err_nomem;
  }
  //parse return value
  if (ParseId(sFuncRetId, cTempBStr) == FALSE)
  {
    PrintAndLog(L"\nError: Invalid function return [id:%lu].\n", sFuncId.nId);
    return FALSE;
  }
  lpElem = cXmlHelper.GetElementFromId(sFuncRetId.nId, FALSE, FALSE);
  if (lpElem == NULL)
  {
    PrintAndLog(L"\nError: Function return type not found (%lu) [id:%lu].\n", sFuncRetId.nId, sFuncId.nId);
    return FALSE;
  }
  nRetClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
  if (nRetClass < 0)
  {
    PrintAndLog(L"\nError: Function return class invalid (%lu) [id:%lu].\n", sFuncRetId.nId, sFuncId.nId);
    return FALSE;
  }
  CheckFundamentalTypeAndClass(sFuncRetId, nRetClass);
  if (sFuncRetId.bIsConstant != FALSE)
    nRetClass |= NKT_DBOBJCLASSFLAG_IsConstant;
  if (sFuncRetId.bIsVolatile != FALSE)
    nRetClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
  //add row
  if (cStrTempW.Copy(L"INSERT INTO Functions (Id,Class,Name,Flags,ReturnTypeId,ReturnClass) "
                     L"VALUES (") == FALSE ||
      cStrTempW.Concat(sFuncId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nFuncClass) == FALSE)
    goto pxml_func_err_nomem;
  if (nFuncClass != NKT_DBOBJCLASS_FunctionType)
  {
    if (cStrTempW.Concat(L",'") == FALSE ||
        cStrTempW.Concat(cNameBStr) == FALSE ||
        cStrTempW.Concat(L"',") == FALSE)
      goto pxml_func_err_nomem;
  }
  else
  {
    if (cStrTempW.Concat(L",NULL,") == FALSE)
      goto pxml_func_err_nomem;
  }
  if (cStrTempW.Concat(nFlags) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(sFuncRetId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nRetClass) == FALSE ||
      cStrTempW.Concat(L");") == FALSE)
    goto pxml_func_err_nomem;
  nDbErr = cDB.QueryExecute(1, cStrTempW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
pxml_func_err_query:
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_func_err_nomem;
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
    return FALSE;
  }
  //add function to list
  if (nFuncClass != NKT_DBOBJCLASS_FunctionType)
  {
    if (cStrTempW.Copy(sFuncId.nId) == FALSE ||
        cStrTempW.Concat(L"-") == FALSE ||
        cStrTempW.Concat(cNameBStr) == FALSE)
      goto pxml_func_err_nomem;
    sW = cStrTempW.Detach();
    if (aFunctionsList.SortedInsert(sW, FunctionsList_Compare, NULL) == FALSE)
    {
      nktMemFree(sW);
      goto pxml_func_err_nomem;
    }
  }
  //process arguments
  nArgIdx = 1;
  //if constructor, method and so on, THIS pointer is passed as the first parameter but
  //not specified in the xml
  if (nFuncClass == NKT_DBOBJCLASS_ClassConstructor ||
      nFuncClass == NKT_DBOBJCLASS_ClassDestructor ||
      nFuncClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
      nFuncClass == NKT_DBOBJCLASS_ClassMethod ||
      nFuncClass == NKT_DBOBJCLASS_ClassConverter)
  {
    //add row (note: this dummy value will be replaced later)
    if (cStrTempW.Copy(L"INSERT INTO FunctionsArgs (FuncId,Id,Name,TypeId,TypeClass) "
                       L"VALUES (") == FALSE ||
        cStrTempW.Concat(sFuncId.nId) == FALSE ||
        cStrTempW.Concat(L",1,'__lpThis',999999999,") == FALSE ||
        cStrTempW.Concat((ULONG)NKT_DBOBJCLASS_Pointer) == FALSE ||
        cStrTempW.Concat(L");") == FALSE)
        goto pxml_func_err_nomem;
    nDbErr = cDB.QueryExecute(1, cStrTempW);
    if (nDbErr != CNktSQLiteConnector::noError)
      goto pxml_func_err_query;
    //increment index
    nArgIdx++;
  }
  //process the rest
  hRes = CXmlHelper::GetFirstChildElement(lpFuncElem, &cArgElem);
  if (FAILED(hRes))
    goto pxml_func_cant_get_arg;
  while (cArgElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cArgElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_func_err_nomem;
    if (FAILED(hRes))
      goto pxml_func_cant_get_argname;
    if (_wcsicmp(cTempBStr, L"argument") == 0)
    {
      if (nFuncClass != NKT_DBOBJCLASS_FunctionType)
      {
        //get argument name (can be empty)
        cNameBStr.Reset();
        hRes = CXmlHelper::GetAttributeValue(cArgElem, L"name", &cNameBStr);
        if (hRes == E_OUTOFMEMORY)
          goto pxml_func_err_nomem;
        if (FAILED(hRes))
        {
          PrintAndLog(L"\nError: Cannot get function's argument name (0x%08X) [id:%lu/%ld].\n",
                      (ULONG)hRes, sFuncId.nId, nArgIdx);
          return FALSE;
        }
      }
      //get argument type
      cTempBStr.Reset();
      hRes = CXmlHelper::GetAttributeValue(cArgElem, L"type", &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_func_err_nomem;
      if (FAILED(hRes))
      {
        PrintAndLog(L"\nError: Cannot get function's argument type (0x%08X) [id:%lu/%ld].\n", (ULONG)hRes,
                    sFuncId.nId, nArgIdx);
        return FALSE;
      }
      if (ParseId(sFuncArgTypeId, cTempBStr) == FALSE)
      {
        PrintAndLog(L"\nError: Invalid function's argument type [id:%lu].\n", sFuncId.nId);
        return FALSE;
      }
      lpElem = cXmlHelper.GetElementFromId(sFuncArgTypeId.nId, FALSE, FALSE);
      if (lpElem == NULL)
      {
        PrintAndLog(L"\nError: Function's argument type not found (%lu) [id:%lu/%ld].\n",
                    sFuncArgTypeId.nId, sFuncId.nId, nArgIdx);
        return FALSE;
      }
      nArgTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
      if (nArgTypeClass < 0)
      {
        PrintAndLog(L"\nError: Function's argument type class invalid (%lu) [id:%lu/%ld].\n",
                    sFuncArgTypeId.nId, sFuncId.nId, nArgIdx);
        return FALSE;
      }
      CheckFundamentalTypeAndClass(sFuncArgTypeId, nArgTypeClass);
      if (sFuncArgTypeId.bIsConstant != FALSE)
        nArgTypeClass |= NKT_DBOBJCLASSFLAG_IsConstant;
      if (sFuncArgTypeId.bIsVolatile != FALSE)
        nArgTypeClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
      //add row
      if (cStrTempW.Copy(L"INSERT INTO FunctionsArgs (FuncId,Id,Name,TypeId,TypeClass) "
                         L"VALUES (") == FALSE ||
          cStrTempW.Concat(sFuncId.nId) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nArgIdx) == FALSE ||
          cStrTempW.Concat(L",'") == FALSE ||
          cStrTempW.Concat((nFuncClass != NKT_DBOBJCLASS_FunctionType) ? cNameBStr : L"") == FALSE ||
          cStrTempW.Concat(L"',") == FALSE ||
          cStrTempW.Concat(sFuncArgTypeId.nId) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nArgTypeClass) == FALSE ||
          cStrTempW.Concat(L");") == FALSE)
        goto pxml_func_err_nomem;
      nDbErr = cDB.QueryExecute(1, cStrTempW);
      if (nDbErr != CNktSQLiteConnector::noError)
        goto pxml_func_err_query;
      //increment index
      nArgIdx++;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cArgElem, &cNextElem);
    if (FAILED(hRes))
      goto pxml_func_cant_get_arg;
    cArgElem.Attach(cNextElem.Detach());
  }
  return TRUE;
}

static BOOL ParseXML_StructAndUnion(IXMLDOMElement *lpStructElem, OBJ_ID &sStructId, BOOL bIsUnion)
{
  CNktComBStr cTempBStr, cNameBStr, cMembersBStr, cBasesBStr;
  TNktComPtr<IXMLDOMElement> cArgElem, cNextElem;
  IXMLDOMElement *lpElem, *lpElem2;
  WCHAR szTempW[128], *sW;
  OBJ_ID sStructMemberTypeId, sStructBaseTypeId;
  HRESULT hRes;
  CNktStringW cStrTempW;
  CNktSQLiteConnector::eErrorCode nDbErr;
  PTR_2_STRUCT sPtr2StructItem, *lpPtr2StructItem;
  STRUCT_METHOD sStructMethod;
  LONG k, nSize, nAlign, nOffset, nBits, nFlags, nPhase, nArtif;
  LONG nMemberIdx, nMemberTypeClass, nBaseIdx, nBaseTypeClass, nSubId;

  //is struct incomplete
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"incomplete", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_struct_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot check if struct is incomplete (0x%08X) [id:%lu].\n", (ULONG)hRes,
                sStructId.nId);
    return FALSE;
  }
  if (_wcsicmp(cTempBStr, L"1") == 0)
    return TRUE; //incomplete structs are not processed
  //----
  //get struct name (can be null)
  cNameBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"name", &cNameBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get struct name (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  //get artificial value
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"artificial", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get struct artificial value (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  nArtif = (_wcsicmp(cTempBStr, L"1") == 0) ? 1 : 0;
  //get struct size
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"size", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get struct size (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  if (ParseLong(nSize, cTempBStr)==FALSE || nSize < (1-nArtif)) {
    PrintAndLog(L"\nError: Invalid struct size [id:%lu].\n", sStructId.nId);
    return FALSE;
  }
  //get struct align
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"align", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get struct align (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  if (ParseLong(nAlign, cTempBStr)==FALSE || nAlign<=0) {
    PrintAndLog(L"\nError: Invalid struct align [id:%lu].\n", sStructId.nId);
    return FALSE;
  }
  //get struct members
  cMembersBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"members", &cMembersBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes)) {
    PrintAndLog(L"\nError: Cannot get struct members (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  //get struct bases
  cBasesBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpStructElem, L"bases", &cBasesBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_struct_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get struct bases (0x%08X) [id:%lu].\n", (ULONG)hRes, sStructId.nId);
    return FALSE;
  }
  //process members in five phases
  nFlags = (cBasesBStr.GetLength() > 0) ? NKT_DBOBJFLAG_HasInheritance : 0;
  for (nPhase=nSubId=1; nPhase<=5; nPhase++)
  {
    if (nPhase == 2) {
      //add main row
      if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
          cStrTempW.Concat((bIsUnion == FALSE) ? L"Structs" : L"Unions") == FALSE ||
          cStrTempW.Concat(L" (Id,Name,Size,Align,Flags) VALUES (") == FALSE ||
          cStrTempW.Concat(sStructId.nId) == FALSE ||
          cStrTempW.Concat(L",'") == FALSE ||
          cStrTempW.Concat(cNameBStr) == FALSE ||
          cStrTempW.Concat(L"',") == FALSE ||
          cStrTempW.Concat(nSize) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nAlign) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nFlags) == FALSE ||
          cStrTempW.Concat(L");") == FALSE)
        goto pxml_struct_err_nomem;
      nDbErr = cDB.QueryExecute(1, cStrTempW);
      if (nDbErr != CNktSQLiteConnector::noError)
      {
pxml_struct_err_query:
        if (nDbErr == CNktSQLiteConnector::errNoMem)
          goto pxml_struct_err_nomem;
        PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
        return FALSE;
      }
    }
    for (sW=cMembersBStr,nMemberIdx=1; *sW!=0; nMemberIdx++)
    {
      for (k=0; k<X_ARRAYLEN(szTempW)-4 && *sW!=0 && *sW!=32; k++)
        szTempW[k] = *sW++;
      if (k >= X_ARRAYLEN(szTempW)-4)
      {
pxml_struct_err_invmember:
        PrintAndLog(L"\nError: Invalid struct's member field index [id:%lu/%ld].\n", sStructId.nId,
                    nMemberIdx);
        return FALSE;
      }
      szTempW[k] = 0;
      if (*sW == L' ')
        sW++;
      //get member field element
      if (ParseId(sStructMemberTypeId, szTempW) == FALSE)
        goto pxml_struct_err_invmember;
      lpElem = cXmlHelper.GetElementFromId(sStructMemberTypeId.nId, FALSE, FALSE);
      if (lpElem == NULL)
      {
        PrintAndLog(L"\nError: Struct's member field index not found (%lu) [id:%lu/%ld].\n",
                    sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
        return FALSE;
      }
      nMemberTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 2);
      if (nMemberTypeClass < 0)
      {
        PrintAndLog(L"\nError: Struct's member field type class invalid (%lu) [id:%lu/%ld].\n",
                    sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
        return FALSE;
      }
      switch (nPhase) {
        case 1:
          //in the first phase, process constructors, destructors and methods to retrieve some data
          if (nMemberTypeClass == NKT_DBOBJCLASS_ClassConstructor)
          {
            //constructors
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"artificial", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot check if constructor is artificial (0x%08X) (%lu) "
                          L"[id:%lu/%ld].\n", (ULONG)hRes, sStructMemberTypeId.nId, sStructId.nId,
                          nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp(cTempBStr, L"1") != 0)
              nFlags |= NKT_DBOBJFLAG_HasConstructor;
            //----
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"virtual", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot check if constructor is virtual (0x%08X) (%lu) [id:%lu/%ld].\n",
                (ULONG)hRes, sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp(cTempBStr, L"1") == 0)
              nFlags |= NKT_DBOBJFLAG_HasVirtual;
          }
          else if (nMemberTypeClass == NKT_DBOBJCLASS_ClassDestructor)
          {
            //destructors
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"artificial", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes)) {
              PrintAndLog(L"\nError: Cannot check if destructor is artificial (0x%08X) (%lu) "
                          L"[id:%lu/%ld].\n", (ULONG)hRes, sStructMemberTypeId.nId, sStructId.nId,
                          nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp(cTempBStr, L"1") != 0)
              nFlags |= NKT_DBOBJFLAG_HasDestructor;
            //----
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"virtual", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot check if destructor is virtual (0x%08X) (%lu) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp(cTempBStr, L"1") == 0)
              nFlags |=  NKT_DBOBJFLAG_HasVirtual;
          }
          else if (nMemberTypeClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
                   nMemberTypeClass == NKT_DBOBJCLASS_ClassMethod ||
                   nMemberTypeClass == NKT_DBOBJCLASS_ClassConverter)
          {
            //methods and operator methods
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"virtual", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot check if method is virtual (0x%08X) (%lu) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp(cTempBStr, L"1") == 0)
              nFlags |= NKT_DBOBJFLAG_HasVirtual;
          }
          break;

        case 2:
          //in the second phase, process real fields
          if (nMemberTypeClass == SPECIAL_DBOBJ_CLASS_Field)
          {
            //get field name (can be null)
            cNameBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"name", &cNameBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member name (0x%08X) [id:%lu/%ldn", (ULONG)hRes,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //get field type
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"type", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member type (0x%08X) [id:%lu/%ld].\n", (ULONG)hRes,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (ParseId(sStructMemberTypeId, cTempBStr) == FALSE)
            {
              PrintAndLog(L"\nError: Invalid struct's member type [id:%lu/%ld].\n", sStructId.nId,
                          nMemberIdx);
              return FALSE;
            }
            lpElem2 = cXmlHelper.GetElementFromId(sStructMemberTypeId.nId, FALSE, FALSE);
            if (lpElem2 == NULL)
            {
              PrintAndLog(L"\nError: Struct's member type not found (%lu) [id:%lu/%ld].\n",
                          sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            nMemberTypeClass = CXmlHelper::GetDbObjectClass(lpElem2, 1);
            if (nMemberTypeClass < 0)
            {
              PrintAndLog(L"\nError: Struct's member type class invalid (%lu) [id:%lu/%ld].\n",
                          sStructMemberTypeId.nId, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            CheckFundamentalTypeAndClass(sStructMemberTypeId, nMemberTypeClass);
            if (sStructMemberTypeId.bIsConstant != FALSE)
              nMemberTypeClass |= NKT_DBOBJCLASSFLAG_IsConstant;
            if (sStructMemberTypeId.bIsVolatile != FALSE)
              nMemberTypeClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
            //get field offset
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"offset", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member offset (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (ParseLong(nOffset, cTempBStr) == FALSE || nOffset < 0)
            {
              PrintAndLog(L"\nError: Invalid struct's member offset [id:%lu/%ld].\n", sStructId.nId,
                          nMemberIdx);
              return FALSE;
            }
            //get field bits
            nBits = 0;
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"bits", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member bits (0x%08X) [id:%lu/%ld].\n", (ULONG)hRes,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if ((LPWSTR)cTempBStr != NULL && ((LPWSTR)cTempBStr)[0] != 0)
            {
              if (ParseLong(nBits, cTempBStr) == FALSE || nBits < 0)
              {
                PrintAndLog(L"\nError: Invalid struct's member offset [id:%lu/%ld].\n", sStructId.nId,
                            nMemberIdx);
                return FALSE;
              }
            }
            //get field access
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"access", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member access (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp((LPWSTR)cTempBStr, L"public") == 0)
              nFlags = NKT_DBOBJFLAG_PubliCMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"protected") == 0)
              nFlags = NKT_DBOBJFLAG_ProtectedMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"private") == 0)
              nFlags = NKT_DBOBJFLAG_PrivateMember;
            else
            {
              PrintAndLog(L"\nError: Invalid struct's member access (%s) [id:%lu/%ld].\n", (LPWSTR)cTempBStr,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //add row
            if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
                cStrTempW.Concat((bIsUnion == FALSE) ? L"StructsMembers (StructId" :
                                                       L"UnionsMembers (UnionId") == FALSE ||
                cStrTempW.Concat(L",Id,Name,Offset,Bits,Flags,TypeId,TypeClass) VALUES (") == FALSE ||
                cStrTempW.Concat(sStructId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nSubId) == FALSE ||
                cStrTempW.Concat(L",'") == FALSE ||
                cStrTempW.Concat(cNameBStr) == FALSE ||
                cStrTempW.Concat(L"',") == FALSE ||
                cStrTempW.Concat(nOffset) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nBits) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nFlags) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(sStructMemberTypeId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nMemberTypeClass) == FALSE ||
                cStrTempW.Concat(L");") == FALSE)
              goto pxml_struct_err_nomem;
            nDbErr = cDB.QueryExecute(1, cStrTempW);
            if (nDbErr != CNktSQLiteConnector::noError)
              goto pxml_struct_err_query;
            //increment index
            nSubId++;
          }
          break;

        case 3:
          //in the third phase, process constructors, destructors and methods
          if (nMemberTypeClass == NKT_DBOBJCLASS_ClassConstructor ||
              nMemberTypeClass == NKT_DBOBJCLASS_ClassDestructor ||
              nMemberTypeClass == NKT_DBOBJCLASS_ClassOperatorMethod ||
              nMemberTypeClass == NKT_DBOBJCLASS_ClassMethod ||
              nMemberTypeClass == NKT_DBOBJCLASS_ClassConverter)
          {
            //get member access
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"access", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member access (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp((LPWSTR)cTempBStr, L"public") == 0)
              nFlags = NKT_DBOBJFLAG_PubliCMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"protected") == 0)
              nFlags = NKT_DBOBJFLAG_ProtectedMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"private") == 0)
              nFlags = NKT_DBOBJFLAG_PrivateMember;
            else
            {
              PrintAndLog(L"\nError: Invalid struct's member access (%s) [id:%lu/%ld].\n", (LPWSTR)cTempBStr,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //add row
            if (cStrTempW.Copy(L"INSERT INTO StructsMembers (StructId,Id,Name,Offset,Bits,Flags,"
                               L"TypeId,TypeClass) VALUES (") == FALSE ||
                cStrTempW.Concat(sStructId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nSubId) == FALSE ||
                cStrTempW.Concat(L",'',0,0,") == FALSE ||
                cStrTempW.Concat(nFlags) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(sStructMemberTypeId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nMemberTypeClass) == FALSE ||
                cStrTempW.Concat(L");") == FALSE)
              goto pxml_struct_err_nomem;
            nDbErr = cDB.QueryExecute(1, cStrTempW);
            if (nDbErr != CNktSQLiteConnector::noError)
              goto pxml_struct_err_query;
            //increment index
            nSubId++;
            sStructMethod.nStructId = sStructId.nId;
            sStructMethod.nMethodId = sStructMemberTypeId.nId;
            if (aStructMethods.SortedInsert(&sStructMethod, &StructMethods_InsertById) == FALSE)
              goto pxml_struct_err_nomem;

          }
          break;

        case 4:
          //in the fourth phase, process enumerators
          if (bIsUnion == FALSE && nMemberTypeClass == NKT_DBOBJCLASS_Enumeration)
          {
            //get enumeration name (can be null)
            cNameBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"name", &cNameBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member name (0x%08X) [id:%lu/%ldn", (ULONG)hRes,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //get field bits
            nBits = 0;
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"bits", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member bits (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if ((LPWSTR)cTempBStr != NULL && ((LPWSTR)cTempBStr)[0] != 0)
            {
              if (ParseLong(nBits, cTempBStr) == FALSE || nBits < 0)
              {
                PrintAndLog(L"\nError: Invalid struct's member offset [id:%lu/%ld].\n", sStructId.nId,
                            nMemberIdx);
                return FALSE;
              }
            }
            //get field access
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"access", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member access (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp((LPWSTR)cTempBStr, L"public") == 0)
              nFlags = NKT_DBOBJFLAG_PubliCMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"protected") == 0)
              nFlags = NKT_DBOBJFLAG_ProtectedMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"private") == 0)
              nFlags = NKT_DBOBJFLAG_PrivateMember;
            else
            {
              PrintAndLog(L"\nError: Invalid struct's member access (%s) [id:%lu/%ld].\n", (LPWSTR)cTempBStr,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //add row
            if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
                cStrTempW.Concat((bIsUnion == FALSE) ? L"StructsMembers (StructId" :
                                                       L"UnionsMembers (UnionId") == FALSE ||
                cStrTempW.Concat(L",Id,Name,Offset,Bits,Flags,TypeId,TypeClass) VALUES (") == FALSE ||
                cStrTempW.Concat(sStructId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nSubId) == FALSE ||
                cStrTempW.Concat(L",'") == FALSE ||
                cStrTempW.Concat(cNameBStr) == FALSE ||
                cStrTempW.Concat(L"',0,") == FALSE ||
                cStrTempW.Concat(nBits) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nFlags) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(sStructMemberTypeId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nMemberTypeClass) == FALSE ||
                cStrTempW.Concat(L");") == FALSE)
              goto pxml_struct_err_nomem;
            nDbErr = cDB.QueryExecute(1, cStrTempW);
            if (nDbErr != CNktSQLiteConnector::noError)
              goto pxml_struct_err_query;
            //increment index
            nSubId++;
          }
          break;

        case 5:
          //in the fifth phase, process typedefs
          if (bIsUnion == FALSE && nMemberTypeClass == NKT_DBOBJCLASS_Typedef)
          {
            //get typedef name
            cNameBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"name", &cNameBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member name (0x%08X) [id:%lu/%ldn", (ULONG)hRes,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //get typedef access
            cTempBStr.Reset();
            hRes = CXmlHelper::GetAttributeValue(lpElem, L"access", &cTempBStr);
            if (hRes == E_OUTOFMEMORY)
              goto pxml_struct_err_nomem;
            if (FAILED(hRes))
            {
              PrintAndLog(L"\nError: Cannot get struct's member access (0x%08X) [id:%lu/%ld].\n",
                          (ULONG)hRes, sStructId.nId, nMemberIdx);
              return FALSE;
            }
            if (_wcsicmp((LPWSTR)cTempBStr, L"public") == 0)
              nFlags = NKT_DBOBJFLAG_PubliCMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"protected") == 0)
              nFlags = NKT_DBOBJFLAG_ProtectedMember;
            else if (_wcsicmp((LPWSTR)cTempBStr, L"private") == 0)
              nFlags = NKT_DBOBJFLAG_PrivateMember;
            else
            {
              PrintAndLog(L"\nError: Invalid struct's member access (%s) [id:%lu/%ld].\n", (LPWSTR)cTempBStr,
                          sStructId.nId, nMemberIdx);
              return FALSE;
            }
            //add row
            if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
                cStrTempW.Concat((bIsUnion == FALSE) ? L"StructsMembers (StructId" :
                                                       L"UnionsMembers (UnionId") == FALSE ||
                cStrTempW.Concat(L",Id,Name,Offset,Bits,Flags,TypeId,TypeClass) VALUES (") == FALSE ||
                cStrTempW.Concat(sStructId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nSubId) == FALSE ||
                cStrTempW.Concat(L",'") == FALSE ||
                cStrTempW.Concat(cNameBStr) == FALSE ||
                cStrTempW.Concat(L"',0,0,") == FALSE ||
                cStrTempW.Concat(nFlags) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(sStructMemberTypeId.nId) == FALSE ||
                cStrTempW.Concat(L",") == FALSE ||
                cStrTempW.Concat(nMemberTypeClass) == FALSE ||
                cStrTempW.Concat(L");") == FALSE)
              goto pxml_struct_err_nomem;
            nDbErr = cDB.QueryExecute(1, cStrTempW);
            if (nDbErr != CNktSQLiteConnector::noError)
              goto pxml_struct_err_query;
            //increment index
            nSubId++;
          }
          break;
      }
    }
  }
  //process bases
  for (sW=cBasesBStr,nBaseIdx=1; *sW!=0; nBaseIdx++)
  {
    for (k=0; k<120 && *sW!=0 && *sW!=32; k++)
      szTempW[k] = *sW++;
    if (k >= 120) {
pxml_struct_err_invbase:
      PrintAndLog(L"\nError: Invalid struct's base index [id:%lu/%ld].\n", sStructId.nId, nBaseIdx);
      return FALSE;
    }
    szTempW[k] = 0;
    if (*sW == L' ')
      sW++;
    //get base element
    if (ParseId(sStructBaseTypeId, szTempW) == FALSE)
      goto pxml_struct_err_invbase;
    lpElem = cXmlHelper.GetElementFromId(sStructBaseTypeId.nId, FALSE, FALSE);
    if (lpElem == NULL)
    {
      PrintAndLog(L"\nError: Struct's base index not found (%lu) [id:%lu/%ld].\n", sStructBaseTypeId.nId,
                  sStructId.nId, nBaseIdx);
      return FALSE;
    }
    nBaseTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
    if (nBaseTypeClass != ((bIsUnion == FALSE) ? NKT_DBOBJCLASS_Struct : NKT_DBOBJCLASS_Union))
    {
      PrintAndLog(L"\nError: Struct's base type class invalid (%lu) [id:%lu/%ld].\n",
                  sStructBaseTypeId.nId, sStructId.nId, nBaseIdx);
      return FALSE;
    }
    //add row
    if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
        cStrTempW.Concat((bIsUnion == FALSE) ? L"StructsBases (StructId" :
                                               L"UnionsBases (UnionId") == FALSE ||
        cStrTempW.Concat(L",Id,BaseId) VALUES (") == FALSE ||
        cStrTempW.Concat(sStructId.nId) == FALSE ||
        cStrTempW.Concat(L",") == FALSE ||
        cStrTempW.Concat(nBaseIdx) == FALSE ||
        cStrTempW.Concat(L",") == FALSE ||
        cStrTempW.Concat(sStructBaseTypeId.nId) == FALSE ||
        cStrTempW.Concat(L");") == FALSE)
      goto pxml_struct_err_nomem;
    nDbErr = cDB.QueryExecute(1, cStrTempW);
    if (nDbErr != CNktSQLiteConnector::noError)
      goto pxml_struct_err_query;
  }
  //add this struct to the pointer-2-structs list
  sPtr2StructItem.nStructId = sStructId.nId;
  sPtr2StructItem.nPointerToStructId = 0;
  lpPtr2StructItem = aPointersToStructs.BinarySearchPtr(&sPtr2StructItem,
                                                        &PointersToStructs_SearchById_StructOnly);
  if (lpPtr2StructItem == NULL)
  {
    if (aPointersToStructs.SortedInsert(&sPtr2StructItem, &PointersToStructs_InsertById) == FALSE)
      goto pxml_struct_err_nomem;
  }
  return TRUE;
}

static BOOL ParseXML_Typedef(IXMLDOMElement *lpTypeDefElem, OBJ_ID &sTypeDefId)
{
  CNktComBStr cTempBStr, cNameBStr;
  IXMLDOMElement *lpElem;
  OBJ_ID sTypedefTypeId;
  HRESULT hRes;
  CNktStringW cStrTempW;
  CNktSQLiteConnector::eErrorCode nDbErr;
  LONG k, nTypeDefTypeClass;

  //get typedef name
  cNameBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpTypeDefElem, L"name", &cNameBStr);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_typedef_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get typedef name (0x%08X) [id:%lu].\n", (ULONG)hRes, sTypeDefId.nId);
    return FALSE;
  }

  if (cNameBStr[0] == 0)
  {
    PrintAndLog(L"\nError: Invalid typedef name [id:%lu].\n", sTypeDefId.nId);
    return FALSE;
  }
  //check if the name is one of the basic types
  k = ConfigGetBasicTypeFromName((LPWSTR)cNameBStr);
  if (k > 0)
  {
    nktMemSet(&sTypedefTypeId, 0, sizeof(sTypedefTypeId));
    sTypedefTypeId.nId = (ULONG)k;
    nTypeDefTypeClass = NKT_DBOBJCLASS_Fundamental;
  }
  else
  {
    //get typedef type
    cTempBStr.Reset();
    hRes = CXmlHelper::GetAttributeValue(lpTypeDefElem, L"type", &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_typedef_err_nomem;
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot get typedef type (0x%08X) [id:%lu].\n", (ULONG)hRes, sTypeDefId.nId);
      return FALSE;
    }
    if (ParseId(sTypedefTypeId, cTempBStr) == FALSE)
    {
      PrintAndLog(L"\nError: Invalid typedef type [id:%lu].\n", sTypeDefId.nId);
      return FALSE;
    }
    lpElem = cXmlHelper.GetElementFromId(sTypedefTypeId.nId, FALSE, FALSE);
    if (lpElem == NULL)
    {
      PrintAndLog(L"\nError: Typedef type not found (%lu) [id:%lu].\n", sTypedefTypeId.nId,
                  sTypeDefId.nId);
      return FALSE;
    }
    nTypeDefTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
    if (nTypeDefTypeClass < 0)
    {
      PrintAndLog(L"\nError: Typedef type class invalid (%lu) [id:%lu].\n", sTypedefTypeId.nId,
                  sTypeDefId.nId);
      return FALSE;
    }
    CheckFundamentalTypeAndClass(sTypedefTypeId, nTypeDefTypeClass);
    if (sTypedefTypeId.bIsConstant != FALSE)
      nTypeDefTypeClass |= NKT_DBOBJCLASSFLAG_IsConstant;
    if (sTypedefTypeId.bIsVolatile != FALSE)
      nTypeDefTypeClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
  }
  //add row
  if (cStrTempW.Copy(L"INSERT INTO TypeDefs (Id,Name,TypeId,TypeClass) VALUES (") == FALSE ||
      cStrTempW.Concat(sTypeDefId.nId) == FALSE ||
      cStrTempW.Concat(L",'") == FALSE ||
      cStrTempW.Concat(cNameBStr) == FALSE ||
      cStrTempW.Concat(L"',") == FALSE ||
      cStrTempW.Concat(sTypedefTypeId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nTypeDefTypeClass) == FALSE ||
      cStrTempW.Concat(L");") == FALSE)
    goto pxml_typedef_err_nomem;
  nDbErr = cDB.QueryExecute(1, cStrTempW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_typedef_err_nomem;
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
    return FALSE;
  }
  return TRUE;
}

static BOOL ParseXML_Array(IXMLDOMElement *lpArrayElem, OBJ_ID &sArrayId)
{
  CNktComBStr cTempBStr, cNameBStr;
  IXMLDOMElement *lpElem;
  OBJ_ID sArrayTypeId;
  HRESULT hRes;
  CNktStringW cStrTempW;
  CNktSQLiteConnector::eErrorCode nDbErr;
  LONG nArrayTypeClass, nMin, nMax, nSize, nAlign;

  //get array type
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpArrayElem, L"type", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_array_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get array type (0x%08X) [id:%lu].\n", (ULONG)hRes, sArrayId.nId);
    return FALSE;
  }
  if (ParseId(sArrayTypeId, cTempBStr) == FALSE)
  {
    PrintAndLog(L"\nError: Invalid array type [id:%lu].\n", sArrayId.nId);
    return FALSE;
  }
  lpElem = cXmlHelper.GetElementFromId(sArrayTypeId.nId, FALSE, FALSE);
  if (lpElem == NULL)
  {
    PrintAndLog(L"\nError: array not found (%lu) [id:%lu].\n", sArrayTypeId.nId, sArrayId.nId);
    return FALSE;
  }
  nArrayTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
  if (nArrayTypeClass < 0)
  {
    PrintAndLog(L"\nError: Array type class invalid (%lu) [id:%lu].\n", sArrayTypeId.nId, sArrayId.nId);
    return FALSE;
  }
  CheckFundamentalTypeAndClass(sArrayTypeId, nArrayTypeClass);
  if (sArrayTypeId.bIsConstant != FALSE)
    nArrayTypeClass |= NKT_DBOBJCLASSFLAG_IsConstant;
  if (sArrayTypeId.bIsVolatile != FALSE)
    nArrayTypeClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
  //get array min
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpArrayElem, L"min", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_array_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get array min (0x%08X) [id:%lu].\n", (ULONG)hRes, sArrayId.nId);
    return FALSE;
  }
  if (ParseLong(nMin, cTempBStr) == FALSE || nMin != 0)
  {
    PrintAndLog(L"\nError: Invalid array min (%lu) [id:%lu].\n", sArrayTypeId.nId, sArrayId.nId);
    return FALSE;
  }
  //get array max
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpArrayElem, L"max", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_array_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get array max (0x%08X) [id:%lu].\n", (ULONG)hRes, sArrayId.nId);
    return FALSE;
  }
  if (cTempBStr[0]==0 || _wcsicmp((LPWSTR)cTempBStr, L"0xffffffffffffffffu")==0)
  {
    nMax = 0;
  }
  else
  {
    if (ParseLong(nMax, cTempBStr)==FALSE || nMax<0)
    {
      PrintAndLog(L"\nError: Invalid array max (%lu) [id:%lu].\n", sArrayTypeId.nId, sArrayId.nId);
      return FALSE;
    }
    nMax++;
  }
  //get array size
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpArrayElem, L"size", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_array_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get array size (0x%08X) [id:%lu].\n", (ULONG)hRes, sArrayId.nId);
    return FALSE;
  }
  nSize = 0;
  if ((cTempBStr[0]!=0 && ParseLong(nSize, cTempBStr)==FALSE) ||
      nSize<0 || (nSize==0 && nMax>0))
  {
    PrintAndLog(L"\nError: Invalid array size [id:%lu].\n", sArrayId.nId);
    return FALSE;
  }
  //get array align
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpArrayElem, L"align", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_array_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get array align (0x%08X) [id:%lu].\n", (ULONG)hRes, sArrayId.nId);
    return FALSE;
  }
  if (ParseLong(nAlign, cTempBStr)==FALSE || nAlign<=0)
  {
    PrintAndLog(L"\nError: Invalid array align [id:%lu].\n", sArrayId.nId);
    return FALSE;
  }
  //add row
  if (cStrTempW.Copy(L"INSERT INTO Arrays (Id,Max,Size,Align,TypeId,TypeClass) VALUES (") == FALSE ||
      cStrTempW.Concat(sArrayId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nMax) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nSize) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nAlign) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(sArrayTypeId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nArrayTypeClass) == FALSE ||
      cStrTempW.Concat(L");") == FALSE)
    goto pxml_array_err_nomem;
  nDbErr = cDB.QueryExecute(1, cStrTempW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_array_err_nomem;
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
    return FALSE;
  }
  return TRUE;
}

static BOOL ParseXML_PointerAndReference(IXMLDOMElement *lpPointerElem, OBJ_ID &sPointerId,
                                         BOOL bIsReference)
{
  CNktComBStr cTempBStr, cNameBStr;
  IXMLDOMElement *lpElem;
  OBJ_ID sPointerTypeId;
  HRESULT hRes;
  CNktStringW cStrTempW;
  CNktSQLiteConnector::eErrorCode nDbErr;
  PTR_2_STRUCT sPtr2StructItem, *lpPtr2StructItem;
  LONG nPointerTypeClass, nSize, nAlign;

  //get pointer type
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpPointerElem, L"type", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_pointer_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get pointer type (0x%08X) [id:%lu].\n", (ULONG)hRes, sPointerId.nId);
    return FALSE;
  }
  if (ParseId(sPointerTypeId, cTempBStr) == FALSE)
  {
    PrintAndLog(L"\nError: Invalid pointer type [id:%lu].\n", sPointerId.nId);
    return FALSE;
  }
  lpElem = cXmlHelper.GetElementFromId(sPointerTypeId.nId, FALSE, FALSE);
  if (lpElem == NULL) {
    PrintAndLog(L"\nError: Pointer not found (%lu) [id:%lu].\n", sPointerTypeId.nId, sPointerId.nId);
    return FALSE;
  }
  nPointerTypeClass = CXmlHelper::GetDbObjectClass(lpElem, 1);
  if (nPointerTypeClass < 0)
  {
    PrintAndLog(L"\nError: Pointer type class invalid (%lu) [id:%lu].\n", sPointerTypeId.nId,
                sPointerId.nId);
    return FALSE;
  }
  CheckFundamentalTypeAndClass(sPointerTypeId, nPointerTypeClass);
  if (sPointerTypeId.bIsConstant != FALSE)
    nPointerTypeClass |= NKT_DBOBJCLASSFLAG_IsConstant;
  if (sPointerTypeId.bIsVolatile != FALSE)
    nPointerTypeClass |= NKT_DBOBJCLASSFLAG_IsVolatile;
  //get pointer size
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpPointerElem, L"size", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_pointer_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get pointer size (0x%08X) [id:%lu].\n", (ULONG)hRes, sPointerId.nId);
    return FALSE;
  }
  if (ParseLong(nSize, cTempBStr)==FALSE || nSize<=0)
  {
    PrintAndLog(L"\nError: Invalid pointer size [id:%lu].\n", sPointerId.nId);
    return FALSE;
  }
  //get pointer align
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpPointerElem, L"align", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_pointer_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get pointer align (0x%08X) [id:%lu].\n", (ULONG)hRes, sPointerId.nId);
    return FALSE;
  }
  if (ParseLong(nAlign, cTempBStr)==FALSE || nAlign<=0)
  {
    PrintAndLog(L"\nError: Invalid pointer align [id:%lu].\n", sPointerId.nId);
    return FALSE;
  }
  //add row
  if (cStrTempW.Copy(L"INSERT INTO ") == FALSE ||
      cStrTempW.Concat((bIsReference == FALSE) ? L"Pointers" : L"XReferences") == FALSE ||
      cStrTempW.Concat(L" (Id,Size,Align,TypeId,TypeClass) VALUES (") == FALSE ||
      cStrTempW.Concat(sPointerId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nSize) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nAlign) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(sPointerTypeId.nId) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nPointerTypeClass) == FALSE ||
      cStrTempW.Concat(L");") == FALSE)
    goto pxml_pointer_err_nomem;
  nDbErr = cDB.QueryExecute(1, cStrTempW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_pointer_err_nomem;
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
    return FALSE;
  }
  //store this pointer to struct in the ptr-2-structs list
  if ((nPointerTypeClass == NKT_DBOBJCLASS_Struct || nPointerTypeClass == NKT_DBOBJCLASS_Union) &&
      (nPointerTypeClass & (NKT_DBOBJCLASSFLAG_IsConstant|NKT_DBOBJCLASSFLAG_IsVolatile)) == 0 &&
      bIsReference == FALSE)
  {
    sPtr2StructItem.nStructId = sPointerTypeId.nId;
    sPtr2StructItem.nPointerToStructId = sPointerId.nId;
    lpPtr2StructItem = aPointersToStructs.BinarySearchPtr(&sPtr2StructItem,
                                                          &PointersToStructs_SearchById_StructOnly);
    if (lpPtr2StructItem != NULL)
    {
      //if a pointer already exists...
      if (lpPtr2StructItem->nPointerToStructId == 0)
      {
        //... and pointer-to was not assigned, remove the old ...
        aPointersToStructs.RemoveElementAt((SIZE_T)(lpPtr2StructItem - aPointersToStructs.GetBuffer()));
        //... and add the replacement
        if (aPointersToStructs.SortedInsert(&sPtr2StructItem, &PointersToStructs_InsertById) == FALSE)
          goto pxml_pointer_err_nomem;
      }
    }
    else
    {
      //if not, add one
      if (aPointersToStructs.SortedInsert(&sPtr2StructItem, &PointersToStructs_InsertById) == FALSE)
        goto pxml_pointer_err_nomem;
    }
  }
  return TRUE;
}

static BOOL ParseXML_Enumeration(IXMLDOMElement *lpEnumElem, OBJ_ID &sEnumId)
{
  CNktComBStr cTempBStr, cNameBStr;
  TNktComPtr<IXMLDOMElement> cEnumValElem, cNextElem;
  HRESULT hRes;
  CNktStringW cStrTempW;
  LONG nValueIdx, nAlign, nSize, nEnumVal;
  CNktSQLiteConnector::eErrorCode nDbErr;

  //get enumtion name (can be null)
  cNameBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpEnumElem, L"name", &cNameBStr);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_enum_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get enum name (0x%08X) [id:%lu].\n", (ULONG)hRes, sEnumId.nId);
    return FALSE;
  }
  //get struct size
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpEnumElem, L"size", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_enum_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get enum size (0x%08X) [id:%lu].\n", (ULONG)hRes, sEnumId.nId);
    return FALSE;
  }
  if (ParseLong(nSize, cTempBStr)==FALSE || nSize<=0)
  {
    PrintAndLog(L"\nError: Invalid enum size [id:%lu].\n", sEnumId.nId);
    return FALSE;
  }
  //get struct align
  cTempBStr.Reset();
  hRes = CXmlHelper::GetAttributeValue(lpEnumElem, L"align", &cTempBStr);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_enum_err_nomem;
  if (FAILED(hRes))
  {
    PrintAndLog(L"\nError: Cannot get enum align (0x%08X) [id:%lu].\n", (ULONG)hRes, sEnumId.nId);
    return FALSE;
  }
  if (ParseLong(nAlign, cTempBStr)==FALSE || nAlign<=0)
  {
    PrintAndLog(L"\nError: Invalid enum align [id:%lu].\n", sEnumId.nId);
    return FALSE;
  }
  //add row
  if (cStrTempW.Copy(L"INSERT INTO Enumerations (Id,Name,Size,Align) VALUES (") == FALSE ||
      cStrTempW.Concat(sEnumId.nId) == FALSE ||
      cStrTempW.Concat(L",'") == FALSE ||
      cStrTempW.Concat(cNameBStr) == FALSE ||
      cStrTempW.Concat(L"',") == FALSE ||
      cStrTempW.Concat(nSize) == FALSE ||
      cStrTempW.Concat(L",") == FALSE ||
      cStrTempW.Concat(nAlign) == FALSE ||
      cStrTempW.Concat(L");") == FALSE)
    goto pxml_enum_err_nomem;
  nDbErr = cDB.QueryExecute(1, cStrTempW);
  if (nDbErr != CNktSQLiteConnector::noError)
  {
pxml_enum_err_query:
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_enum_err_nomem;
    PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrTempW, (ULONG)nDbErr);
    return FALSE;
  }
  //process values
  hRes = CXmlHelper::GetFirstChildElement(lpEnumElem, &cEnumValElem);
  if (FAILED(hRes))
  {
pxml_enum_cant_get_arg:
    if (hRes == E_OUTOFMEMORY)
      goto pxml_enum_err_nomem;
    PrintAndLog(L"\nError: Cannot get enum values (0x%08X) [id:%lu].\n", (ULONG)hRes, sEnumId.nId);
    return FALSE;
  }
  nValueIdx = 1;
  while (cEnumValElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cEnumValElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_enum_err_nomem;
    if (FAILED(hRes)) {
      PrintAndLog(L"\nError: Cannot get enum's value name (0x%08X) [id:%lu].\n", (ULONG)hRes, sEnumId.nId);
      return FALSE;
    }
    if (_wcsicmp(cTempBStr, L"EnumValue") == 0)
    {
      //get value name
      cNameBStr.Reset();
      hRes = CXmlHelper::GetAttributeValue(cEnumValElem, L"name", &cNameBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_enum_err_nomem;
      if (FAILED(hRes))
      {
        PrintAndLog(L"\nError: Cannot get enum's value name (0x%08X) [id:%lu/%ld].\n", (ULONG)hRes,
                    sEnumId.nId, nValueIdx);
        return FALSE;
      }
      if (cNameBStr[0] == 0)
      {
        PrintAndLog(L"\nError: Invalid enum's value name [id:%lu/%ld].\n", sEnumId.nId, nValueIdx);
        return FALSE;
      }
      //get value's value
      cTempBStr.Reset();
      hRes = CXmlHelper::GetAttributeValue(cEnumValElem, L"init", &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_enum_err_nomem;
      if (FAILED(hRes))
      {
        PrintAndLog(L"\nError: Cannot get enum's value init value (0x%08X) [id:%lu/%ld].\n", (ULONG)hRes,
                    sEnumId.nId, nValueIdx);
        return FALSE;
      }
      if (ParseLong(nEnumVal, cTempBStr) == FALSE) {
        PrintAndLog(L"\nError: Invalid enum's value init value [id:%lu/%ld].\n", sEnumId.nId, nValueIdx);
        return FALSE;
      }
      //add row
      if (cStrTempW.Copy(L"INSERT INTO EnumerationsValues (EnumId,Id,Name,Value) VALUES (") == FALSE ||
          cStrTempW.Concat(sEnumId.nId) == FALSE ||
          cStrTempW.Concat(L",") == FALSE ||
          cStrTempW.Concat(nValueIdx) == FALSE ||
          cStrTempW.Concat(L",'") == FALSE ||
          cStrTempW.Concat(cNameBStr) == FALSE ||
          cStrTempW.Concat(L"',") == FALSE ||
          cStrTempW.Concat(nEnumVal) == FALSE ||
          cStrTempW.Concat(L");") == FALSE)
        goto pxml_enum_err_nomem;
      nDbErr = cDB.QueryExecute(1, cStrTempW);
      if (nDbErr != CNktSQLiteConnector::noError)
        goto pxml_enum_err_query;
      //increment index
      nValueIdx++;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cEnumValElem, &cNextElem);
    if (FAILED(hRes))
      goto pxml_enum_cant_get_arg;
    cEnumValElem.Attach(cNextElem.Detach());
  }
  return TRUE;
}

static VOID CheckFundamentalTypeAndClass(OBJ_ID &sFuncId, LONG nClass)
{
  LONG k;

  if (nClass == NKT_DBOBJCLASS_Fundamental)
  {
    k = cXmlHelper.GetFundamentalTypeFromId(sFuncId.nId);
    NKT_ASSERT(k > 0);
    sFuncId.nId = k;
  }
  return;
}

static BOOL ParseId(OBJ_ID &sDestObjId, LPCWSTR szIdW)
{
  ULONG k;

  nktMemSet(&sDestObjId, 0, sizeof(sDestObjId));
  if (szIdW == NULL || szIdW[0] != L'_')
    return FALSE;
  szIdW++;
  while (szIdW[0] >= L'0' && szIdW[0] <= L'9')
  {
    k = sDestObjId.nId * 10;
    if (k < sDestObjId.nId)
      return FALSE; //overflow
    sDestObjId.nId = k + (ULONG)(szIdW[0] - L'0');
    if (sDestObjId.nId < k)
      return FALSE; //overflow
    szIdW++;
  }
  if (sDestObjId.nId == 0)
    return FALSE;
  while (true)
  {
    if (szIdW[0]==L'c' || szIdW[0]==L'C')
      sDestObjId.bIsConstant = TRUE;
    else if (szIdW[0]==L'v' || szIdW[0]==L'V')
      sDestObjId.bIsVolatile = TRUE;
    else
      break;
    szIdW++;
  }
  return (szIdW[0] == 0) ? TRUE : FALSE;
}

static BOOL ParseLong(LONG &lValue, LPCWSTR szStrW)
{
  LONG k;
  BOOL bNeg;

  lValue = 0;
  if (szStrW==NULL || szStrW[0]==0)
    return FALSE;
  bNeg = FALSE;
  if (szStrW[0] == L'-')
  {
    szStrW++;
    if (szStrW[0] == 0)
      return FALSE;
    bNeg = TRUE;
    //special case
    if (wcsncmp(szStrW, L"2147483648", 10) == 0)
    {
      lValue = (LONG)0xFFFFFFFFUL;
      szStrW += 10;
      goto pl_end;
    }
  }
  while (szStrW[0]>=L'0' && szStrW[0]<=L'9')
  {
    k = lValue * 10;
    if (k < lValue)
      return FALSE; //overflow
    lValue = k + (LONG)(szStrW[0] - L'0');
    if (lValue < k)
      return FALSE; //overflow
    szStrW++;
  }
  if (bNeg != FALSE)
    lValue = -lValue;
pl_end:
  if (szStrW[0] == L'u')
  {
    if (lValue < 0)
      return FALSE;
    szStrW++;
  }
  if (szStrW[0] != 0)
    return FALSE;
  return TRUE;
}

static int FunctionsList_Compare(__in void *, __in LPWSTR *lpElem1, __in LPWSTR *lpElem2)
{
  return wcscmp(*lpElem1, *lpElem2);
}

static BOOL ParseXML_Replace__lpThis999999999(SIZE_T &nMaxId)
{
  CNktStringW cStrQueryW;
  CNktSQLiteConnector::eErrorCode nDbErr;
  SIZE_T i, j, nIdx, nCount;
  WCHAR szBufW[32];
  BOOL bInTransaction;
  PTR_2_STRUCT sPtr2StructItem, *lpPtr2StructItem;

  bInTransaction = FALSE;
  //for each pointer-2-struct item that has no pointer, add one
  nCount = aPointersToStructs.GetCount();
  for (i=nIdx=0; i<nCount; i++)
  {
    if (aPointersToStructs[i].nPointerToStructId == 0)
    {
      if (IsAbortKeyPressed() != FALSE)
        return FALSE;
      swprintf_s(szBufW, 32, L"(%.2lf) [This]", ((double)(i+1) / (double)nCount) * 40.0);
      wprintf_s(L"%s", szBufW);
      for (j=0; szBufW[j]!=0; j++)
        szBufW[j] = L'\b';
      wprintf_s(L"%s", szBufW);
      //----
      if ((nIdx & 31) == 0)
      {
        if (bInTransaction != FALSE)
        {
          nDbErr = cDB.TransactionCommit();
          if (nDbErr != CNktSQLiteConnector::noError)
          {
            if (nDbErr == CNktSQLiteConnector::errNoMem)
              PrintAndLogNoMemoryError();
            else
              PrintAndLog(L"\nError: Cannot execute query [COMMIT TRANS] (%lu).\n", (ULONG)nDbErr);
            return FALSE;
          }
          bInTransaction = FALSE;
        }
        nDbErr = cDB.TransactionStart();
        if (nDbErr != CNktSQLiteConnector::noError)
        {
          if (nDbErr == CNktSQLiteConnector::errNoMem)
            PrintAndLogNoMemoryError();
          else
            PrintAndLog(L"\nError: Cannot execute query [BEGIN TRANS] (%lu).\n", (ULONG)nDbErr);
          return FALSE;
        }
        bInTransaction = TRUE;
      }
      nIdx++;
      //----
      nMaxId++;
      aPointersToStructs[i].nPointerToStructId = (ULONG)nMaxId;
      if (cStrQueryW.Copy(L"INSERT INTO Pointers (Id,Size,Align,TypeId,TypeClass) VALUES (") == FALSE ||
          cStrQueryW.Concat(aPointersToStructs[i].nPointerToStructId) == FALSE ||
          cStrQueryW.Concat(L",") == FALSE ||
          cStrQueryW.Concat(nProcessingBits) == FALSE ||
          cStrQueryW.Concat(L",") == FALSE ||
          cStrQueryW.Concat(nProcessingBits) == FALSE ||
          cStrQueryW.Concat(L",") == FALSE ||
          cStrQueryW.Concat(aPointersToStructs[i].nStructId) == FALSE ||
          cStrQueryW.Concat(L",") == FALSE ||
          cStrQueryW.Concat((ULONG)NKT_DBOBJCLASS_Struct) == FALSE ||
          cStrQueryW.Concat(L");") == FALSE)
      {
pxml_repl_lpThis_err_nomem:
        PrintAndLogNoMemoryError();
        return FALSE;
      }
      nDbErr = cDB.QueryExecute(1, cStrQueryW);
      if (nDbErr == CNktSQLiteConnector::errNoMem)
        goto pxml_repl_lpThis_err_nomem;
      if (nDbErr != CNktSQLiteConnector::noError)
      {
pxml_repl_lpThis_query_exec:
        PrintAndLog(L"\nError: Cannot execute query [%s] (%lu).\n", (LPWSTR)cStrQueryW, (ULONG)nDbErr);
        return FALSE;
      }
    }
  }
  //for each struct/method pair
  nCount = aStructMethods.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (IsAbortKeyPressed() != FALSE)
      return FALSE;
    swprintf_s(szBufW, 32, L"(%.2lf) [This]", 40.0 + ((double)(i+1) / (double)nCount) * 60.0);
    wprintf_s(L"%s", szBufW);
    for (j=0; szBufW[j]!=0; j++)
      szBufW[j] = L'\b';
    wprintf_s(L"%s", szBufW);
    //----
    if ((nIdx & 31) == 0)
    {
      if (bInTransaction != FALSE)
      {
        nDbErr = cDB.TransactionCommit();
        if (nDbErr != CNktSQLiteConnector::noError)
        {
          if (nDbErr == CNktSQLiteConnector::errNoMem)
            PrintAndLogNoMemoryError();
          else
            PrintAndLog(L"\nError: Cannot execute query [COMMIT TRANS] (%lu).\n", (ULONG)nDbErr);
          return FALSE;
        }
        bInTransaction = FALSE;
      }
      nDbErr = cDB.TransactionStart();
      if (nDbErr != CNktSQLiteConnector::noError)
      {
        if (nDbErr == CNktSQLiteConnector::errNoMem)
          PrintAndLogNoMemoryError();
        else
          PrintAndLog(L"\nError: Cannot execute query [BEGIN TRANS] (%lu).\n", (ULONG)nDbErr);
        return FALSE;
      }
      bInTransaction = TRUE;
    }
    nIdx++;
    //----
    //find the pointer that belongs to this struct
    sPtr2StructItem.nStructId = aStructMethods[i].nStructId;
    lpPtr2StructItem = aPointersToStructs.BinarySearchPtr(&sPtr2StructItem,
                                                          &PointersToStructs_SearchById_StructOnly);
    NKT_ASSERT(lpPtr2StructItem != NULL);
    //update FunctionsArgs table
    if (cStrQueryW.Copy(L"UPDATE FunctionsArgs SET TypeId=") == FALSE ||
        cStrQueryW.Concat(lpPtr2StructItem->nPointerToStructId) == FALSE ||
        cStrQueryW.Concat(L" WHERE FuncId=") == FALSE ||
        cStrQueryW.Concat(aStructMethods[i].nMethodId) == FALSE ||
        cStrQueryW.Concat(L" AND Id=1;") == FALSE)
      goto pxml_repl_lpThis_err_nomem;
    nDbErr = cDB.QueryExecute(1, cStrQueryW);
    if (nDbErr == CNktSQLiteConnector::errNoMem)
      goto pxml_repl_lpThis_err_nomem;
    if (nDbErr != CNktSQLiteConnector::noError)
      goto pxml_repl_lpThis_query_exec;
  }
  //almost done
  if (bInTransaction != FALSE)
  {
    nDbErr = cDB.TransactionCommit();
    if (nDbErr != CNktSQLiteConnector::noError)
    {
      if (nDbErr == CNktSQLiteConnector::errNoMem)
        PrintAndLogNoMemoryError();
      else
        PrintAndLog(L"\nError: Cannot execute query [COMMIT TRANS] (%lu).\n", (ULONG)nDbErr);
      return FALSE;
    }
  }
  wprintf_s(L"                                "
            L"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  return TRUE;
}

static int __cdecl PointersToStructs_InsertById(__in void *, __in PTR_2_STRUCT *lpElem1,
                                                __in PTR_2_STRUCT *lpElem2)
{
  int res;

  res = NKT_DV_CMP<ULONG>(lpElem1->nStructId, lpElem2->nStructId);
  if (res != 0)
    return res;
  return NKT_DV_CMP<ULONG>(lpElem1->nPointerToStructId, lpElem2->nPointerToStructId);
}

static int __cdecl PointersToStructs_SearchById(void *lpContext, const void *lpKey, const void *elem)
{
  PTR_2_STRUCT *lpPtr, *lpKeyPtr;
  int res;

  lpKeyPtr = reinterpret_cast<PTR_2_STRUCT*>(const_cast<void*>(lpKey));
  lpPtr = reinterpret_cast<PTR_2_STRUCT*>(const_cast<void*>(elem));
  res = NKT_DV_CMP<ULONG>(lpKeyPtr->nStructId, lpPtr->nStructId);
  if (res != 0)
    return res;
  return NKT_DV_CMP<ULONG>(lpKeyPtr->nPointerToStructId, lpPtr->nPointerToStructId);
}

static int __cdecl PointersToStructs_SearchById_StructOnly(void *lpContext, const void *lpKey,
                                                           const void *elem)
{
  PTR_2_STRUCT *lpPtr, *lpKeyPtr;

  lpKeyPtr = reinterpret_cast<PTR_2_STRUCT*>(const_cast<void*>(lpKey));
  lpPtr = reinterpret_cast<PTR_2_STRUCT*>(const_cast<void*>(elem));
  return NKT_DV_CMP<ULONG>(lpKeyPtr->nStructId, lpPtr->nStructId);
}

static int __cdecl StructMethods_InsertById(__in void *, __in STRUCT_METHOD *lpElem1,
                                            __in STRUCT_METHOD *lpElem2)
{
  int res;

  res = NKT_DV_CMP<ULONG>(lpElem1->nStructId, lpElem2->nStructId);
  if (res != 0)
    return res;
  return NKT_DV_CMP<ULONG>(lpElem1->nMethodId, lpElem2->nMethodId);
}

static int __cdecl StructMethods_SearchById(void *lpContext, const void *lpKey, const void *elem)
{
  STRUCT_METHOD *lpPtr, *lpKeyPtr;
  int res;

  lpKeyPtr = reinterpret_cast<STRUCT_METHOD*>(const_cast<void*>(lpKey));
  lpPtr = reinterpret_cast<STRUCT_METHOD*>(const_cast<void*>(elem));
  res = NKT_DV_CMP<ULONG>(lpKeyPtr->nStructId, lpPtr->nStructId);
  if (res != 0)
    return res;
  return NKT_DV_CMP<ULONG>(lpKeyPtr->nMethodId, lpPtr->nMethodId);
}
