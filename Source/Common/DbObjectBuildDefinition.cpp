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

#include "DbObject.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static BOOL InsertTabInEachLine(__inout CNktStringW &cStrW);
static BOOL CheckX01(__in CNktStringW &cStrW);
static BOOL ReplaceX01(__inout CNktStringW &cStrW, __in LPCWSTR szNewString);
static BOOL ReplaceX01OrAdd(__inout CNktStringW &cStrW, __in LPCWSTR szNewString);
static BOOL AddItemFlags(__inout CNktStringW &cStrW, __in int nFlags);

//-----------------------------------------------------------

//Ideas for parsing taken from PyCccXml project which uses the "Boost Software License"

HRESULT CNktDvDbObjectNoRef::BuildDefinitionString(__inout CNktStringW &cStrDeclW,
                                                   __in const CNktDvDbObjectNoRef *lpDbObj)
{
  HRESULT hRes;

  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
      hRes = BuilldStructDefStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      hRes = BuilldFunctionDefStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsEnumeration:
      hRes = BuilldEnumDefStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsFundamental:
      hRes = GetFundamentalDefStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsPointer:
      hRes = BuilldPointerArgStr(cStrDeclW, lpDbObj, FALSE);
      break;
    case CNktDvDbObjectNoRef::clsReference:
      hRes = BuilldPointerArgStr(cStrDeclW, lpDbObj, TRUE);
      break;
    case CNktDvDbObjectNoRef::clsArray:
      hRes = BuilldArrayArgStr(cStrDeclW, lpDbObj);
      break;
    default:
      cStrDeclW.Empty();
      hRes = S_OK;
      break;
  }
  while (SUCCEEDED(hRes) && CheckX01(cStrDeclW) != FALSE)
  {
    if (ReplaceX01(cStrDeclW, L"") == FALSE)
      hRes = E_OUTOFMEMORY;
  }
  //strip spaces and tabs at beginning
  while (cStrDeclW.IsEmpty() == FALSE && ((LPWSTR)cStrDeclW)[0] <= 32)
    cStrDeclW.Delete(0, 1);
  return hRes;
}

HRESULT CNktDvDbObjectNoRef::BuildChildDefinitionString(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex)
{
  HRESULT hRes;

  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
      hRes = BuilldStructChildDefStr(cStrDeclW, nIndex);
      break;
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      hRes = BuilldFunctionChildDefStr(cStrDeclW, nIndex);
      break;
    case CNktDvDbObjectNoRef::clsEnumeration:
      hRes = BuilldEnumChildDefStr(cStrDeclW, nIndex);
      break;
    default:
      cStrDeclW.Empty();
      hRes = S_OK;
      break;
  }
  while (SUCCEEDED(hRes) && CheckX01(cStrDeclW) != FALSE)
  {
    if (ReplaceX01(cStrDeclW, L"") == FALSE)
      hRes = E_OUTOFMEMORY;
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::GetFundamentalDefStr(__inout CNktStringW &cStrDeclW,
                                                  __in const CNktDvDbObjectNoRef *lpDbObj)
{
  if (lpDbObj->GetName()[0] != 0)
  {
    if (cStrDeclW.Copy(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  else {
    switch (lpDbObj->GetFundamentalType())
    {
      case CNktDvDbObjectNoRef::ftSignedByte:
        if (cStrDeclW.Copy(L"char") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftUnsignedByte:
        if (cStrDeclW.Copy(L"unsigned char") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftSignedWord:
        if (cStrDeclW.Copy(L"short") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftUnsignedWord:
        if (cStrDeclW.Copy(L"unsigned short") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftSignedDoubleWord:
        if (cStrDeclW.Copy(L"int") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftUnsignedDoubleWord:
        if (cStrDeclW.Copy(L"unsigned int") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftSignedQuadWord:
        if (cStrDeclW.Copy(L"long long") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftUnsignedQuadWord:
        if (cStrDeclW.Copy(L"unsigned long long") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftFloat:
        if (cStrDeclW.Copy(L"float") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftDouble:
        if (cStrDeclW.Copy(L"double") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftLongDouble:
        if (cStrDeclW.Copy(L"long double") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftAnsiChar:
        if (cStrDeclW.Copy(L"char") == FALSE)
          return E_OUTOFMEMORY;
        break;
      case CNktDvDbObjectNoRef::ftWideChar:
        if (cStrDeclW.Copy(L"wchar_t") == FALSE)
          return E_OUTOFMEMORY;
        break;
    }
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldFunctionDefStr(__inout CNktStringW &cStrDeclW,
                                                  __in const CNktDvDbObjectNoRef *lpDbObj)
{
  CNktStringW cStrTempW;
  CNktDvDbObjectNoRef *lpChildDbObj;
  SIZE_T i, nCount;
  HRESULT hRes;

  cStrDeclW.Empty();
  //result
  lpChildDbObj = lpDbObj->GetFunctionReturn();
  hRes = BuildArgumentStr(cStrTempW, lpChildDbObj);
  if (FAILED(hRes))
    return hRes;
  if (cStrDeclW.Copy((LPWSTR)cStrTempW) == FALSE ||
      cStrDeclW.Concat(L" ") == FALSE)
    return E_OUTOFMEMORY;
  switch (lpDbObj->GetFuncCallingConvention())
  {
    case CNktDvDbObjectNoRef::ccStdCall:
      if (cStrDeclW.Concat(L"__stdcall ") == FALSE)
        return E_OUTOFMEMORY;
      break;
    case CNktDvDbObjectNoRef::ccFastCall:
      if (cStrDeclW.Concat(L"__fastcall ") == FALSE)
        return E_OUTOFMEMORY;
      break;
    case CNktDvDbObjectNoRef::ccThisCall:
      if (cStrDeclW.Concat(L"thiscall ") == FALSE)
        return E_OUTOFMEMORY;
      break;
    default:
      if (cStrDeclW.Concat(L"__cdecl ") == FALSE)
        return E_OUTOFMEMORY;
      break;
  }
  if (lpDbObj->GetName()[0] != 0)
  {
    if (cStrDeclW.Concat(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  else {
    if (cStrDeclW.Concat(L"_unnamed_func_" ) == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetId()) == FALSE)
      return E_OUTOFMEMORY;
  }
  if (cStrDeclW.Concat(L"(") == FALSE)
    return E_OUTOFMEMORY;
  //parameters
  nCount = lpDbObj->GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    lpChildDbObj = lpDbObj->GetItem(i);
    hRes = BuildArgumentStr(cStrTempW, lpChildDbObj);
    if (FAILED(hRes))
      return hRes;
    if (AddItemFlags(cStrTempW, lpDbObj->GetItemFlags(i)) == FALSE ||
        ReplaceX01OrAdd(cStrTempW, lpDbObj->GetItemName(i)) == FALSE ||
        (i > 0 && cStrDeclW.Concat(L", ") == FALSE) ||
        cStrDeclW.Concat((LPWSTR)cStrTempW) == FALSE)
      return E_OUTOFMEMORY;
  }
  if (cStrDeclW.Concat(L")") == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldStructDefStr(__inout CNktStringW &cStrDeclW,
                                                __in const CNktDvDbObjectNoRef *lpDbObj)
{
  CNktStringW cStrTempW;
  CNktDvDbObjectNoRef *lpChildDbObj;
  CNktDvDbObjectNoRef::eItemMemberType nLastMemberType, nCurrMemberType;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (cStrDeclW.Copy((lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion) ? L"union" :
                                                                              L"struct") == FALSE ||
      cStrDeclW.Concat(L" {\n") == FALSE)
    return E_OUTOFMEMORY;
  nLastMemberType = CNktDvDbObjectNoRef::mtPublic;
  nCount = lpDbObj->GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    nCurrMemberType = lpDbObj->GetItemMemberType(i);
    if (nLastMemberType != nCurrMemberType && nCurrMemberType != CNktDvDbObjectNoRef::mtNA)
    {
      switch (nCurrMemberType) {
        case mtPublic:
          if (cStrDeclW.Concat(L"public:\n") == FALSE)
            return E_OUTOFMEMORY;
          break;
        case mtPrivate:
          if (cStrDeclW.Concat(L"private:\n") == FALSE)
            return E_OUTOFMEMORY;
          break;
        case mtProtected:
          if (cStrDeclW.Concat(L"protected:\n") == FALSE)
            return E_OUTOFMEMORY;
          break;
      }
      nLastMemberType = nCurrMemberType;
    }
    lpChildDbObj = lpDbObj->GetItem(i);
    switch (lpChildDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsStruct:
      case CNktDvDbObjectNoRef::clsUnion:
        if (lpChildDbObj->GetName()[0] == 0) {
          hRes = BuildDefinitionString(cStrTempW, lpChildDbObj);
          break;
        }
        //fall into 'default'
      default:
        hRes = BuildArgumentStr(cStrTempW, lpChildDbObj);
        break;
    }
    if (FAILED(hRes))
      return hRes;
    if (AddItemFlags(cStrTempW, lpDbObj->GetItemFlags(i)) == FALSE ||
        ReplaceX01OrAdd(cStrTempW, lpDbObj->GetItemName(i)) == FALSE ||
        InsertTabInEachLine(cStrTempW) == FALSE ||
        cStrDeclW.Concat((LPWSTR)cStrTempW) == FALSE ||
        cStrDeclW.Concat(L";\n") == FALSE)
      return E_OUTOFMEMORY;
  }
  if (cStrDeclW.Concat(L"}") == FALSE)
    return E_OUTOFMEMORY;
  if (lpDbObj->GetName()[0] != 0) {
    if (cStrDeclW.Concat(L" ") == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldEnumDefStr(__inout CNktStringW &cStrDeclW,
                                              __in const CNktDvDbObjectNoRef *lpDbObj)
{
  CNktStringW cStrTempW;
  SIZE_T i, nCount;

  if (cStrDeclW.Copy(L"enum {\n") == FALSE)
    return E_OUTOFMEMORY;
  nCount = lpDbObj->GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    if (cStrDeclW.Concat(L"\t") == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetItemName(i)) == FALSE ||
        cStrDeclW.Concat(L"=") == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetItemEnumValue(i)) == FALSE ||
        cStrDeclW.Concat((i < nCount-1) ? L",\n" : L"\n") == FALSE)
      return E_OUTOFMEMORY;
  }
  if (cStrDeclW.Concat(L"}") == FALSE)
    return E_OUTOFMEMORY;
  if (lpDbObj->GetName()[0] != 0) {
    if (cStrDeclW.Concat(L" ") == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuildArgumentStr(__inout CNktStringW &cStrDeclW,
                                              __in const CNktDvDbObjectNoRef *lpDbObj)
{
  HRESULT hRes;

  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
      hRes = BuilldStructArgStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsPointer:
      hRes = BuilldPointerArgStr(cStrDeclW, lpDbObj, FALSE);
      break;
    case CNktDvDbObjectNoRef::clsReference:
      hRes = BuilldPointerArgStr(cStrDeclW, lpDbObj, TRUE);
      break;
    case CNktDvDbObjectNoRef::clsFundamental:
      hRes = GetFundamentalDefStr(cStrDeclW, lpDbObj);
      break;
    case CNktDvDbObjectNoRef::clsEnumeration:
      hRes = (cStrDeclW.Copy((lpDbObj->GetName()[0] != 0) ? lpDbObj->GetName() : L"int") != FALSE) ?
                  S_OK : E_OUTOFMEMORY;
      break;
    case CNktDvDbObjectNoRef::clsArray:
      hRes = BuilldArrayArgStr(cStrDeclW, lpDbObj);
      break;
    default:
      hRes = (cStrDeclW.Copy(L"???") != FALSE) ? S_OK : E_OUTOFMEMORY;
      break;
  }
  return hRes;
}

HRESULT CNktDvDbObjectNoRef::BuilldStructArgStr(__inout CNktStringW &cStrDeclW,
                                                __in const CNktDvDbObjectNoRef *lpDbObj)
{
  if (lpDbObj->GetName()[0] != 0)
  {
    if (cStrDeclW.Copy(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  else {
    if (cStrDeclW.Copy((lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion) ? L"_unnamed_union_" :
                                                                         L"_unnamed_struct_") == FALSE ||
        cStrDeclW.Concat(lpDbObj->GetId()) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldPointerArgStr(__inout CNktStringW &cStrDeclW,
                                                 __in const CNktDvDbObjectNoRef *lpDbObj, __in BOOL bIsRef)
{
  CNktStringW cStrTempW;
  CNktDvDbObjectNoRef *lpChildDbObj, *lpChildDbObj2;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (lpDbObj->GetName()[0] != 0)
  {
    if (cStrDeclW.Copy(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  else if (lpDbObj->GetItemsCount() == 0)
  {
    if (cStrDeclW.Copy((bIsRef == FALSE) ? L"*" : L"&") == FALSE)
      return E_OUTOFMEMORY;
  }
  else {
    lpChildDbObj = lpDbObj->GetItem(0);
    switch (lpChildDbObj->GetClass()) {
      case CNktDvDbObjectNoRef::clsFunctionType:
        lpChildDbObj2 = lpChildDbObj->GetFunctionReturn();
        hRes = BuildArgumentStr(cStrDeclW, lpChildDbObj2);
        if (FAILED(hRes))
          return hRes;
        if (ReplaceX01(cStrDeclW, L"") == FALSE ||
            cStrDeclW.Concat(L" (") == FALSE)
          return E_OUTOFMEMORY;
        switch (lpChildDbObj->GetFuncCallingConvention())
        {
          case CNktDvDbObjectNoRef::ccStdCall:
            if (cStrDeclW.Concat(L"__stdcall ") == FALSE)
              return E_OUTOFMEMORY;
            break;
          case CNktDvDbObjectNoRef::ccFastCall:
            if (cStrDeclW.Concat(L"__fastcall ") == FALSE)
              return E_OUTOFMEMORY;
            break;
          case CNktDvDbObjectNoRef::ccThisCall:
            if (cStrDeclW.Concat(L"thiscall ") == FALSE)
              return E_OUTOFMEMORY;
            break;
          default:
            if (cStrDeclW.Concat(L"__cdecl ") == FALSE)
              return E_OUTOFMEMORY;
            break;
        }
        if (cStrDeclW.Concat(L"*\x01)(") == FALSE)
          return E_OUTOFMEMORY;
        nCount = lpChildDbObj->GetItemsCount();
        for (i=0; i<nCount; i++)
        {
          lpChildDbObj2 = lpChildDbObj->GetItem(i);
          hRes = BuildArgumentStr(cStrTempW, lpChildDbObj2);
          if (FAILED(hRes))
            return hRes;
          if (ReplaceX01OrAdd(cStrTempW, lpChildDbObj2->GetItemName(i)) == FALSE ||
              (i > 0 && cStrDeclW.Concat(L", ") == FALSE) ||
              cStrDeclW.Concat((LPWSTR)cStrTempW) == FALSE)
            return E_OUTOFMEMORY;
        }
        if (cStrDeclW.Concat(L")") == FALSE)
          return E_OUTOFMEMORY;
        break;

      default:
        hRes = BuildArgumentStr(cStrDeclW, lpChildDbObj);
        if (FAILED(hRes))
          return hRes;
        if (AddItemFlags(cStrDeclW, lpDbObj->GetItemFlags(0)) == FALSE ||
            cStrDeclW.Concat((bIsRef == FALSE) ? L"*" : L"&") == FALSE)
          return E_OUTOFMEMORY;
        break;
    }
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldArrayArgStr(__inout CNktStringW &cStrDeclW,
                                               __in const CNktDvDbObjectNoRef *lpDbObj)
{
  CNktStringW cStrTempW;
  CNktDvDbObjectNoRef *lpChildDbObj;
  SIZE_T k;
  HRESULT hRes;

  if (lpDbObj->GetName()[0] != 0)
  {
    if (cStrDeclW.Copy(lpDbObj->GetName()) == FALSE)
      return E_OUTOFMEMORY;
  }
  else if (lpDbObj->GetItemsCount() == 0)
  {
    if (cStrDeclW.Copy(L"???") == FALSE)
      return E_OUTOFMEMORY;
  }
  else {
    lpChildDbObj = lpDbObj->GetItem(0);
    hRes = BuildArgumentStr(cStrDeclW, lpChildDbObj);
    if (FAILED(hRes))
      return hRes;
    if (AddItemFlags(cStrDeclW, lpDbObj->GetItemFlags(0)) == FALSE)
      return E_OUTOFMEMORY;
  }
  k = lpDbObj->GetArrayMaxHint();
  if (cStrTempW.Copy(L"\x01[") == FALSE ||
      (k != 0 && k != NKT_SIZE_T_MAX && cStrTempW.Concat(k) == FALSE) ||
      cStrTempW.Concat(L"]") == FALSE ||
      ReplaceX01OrAdd(cStrDeclW, (LPWSTR)cStrTempW) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldStructChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex)
{
  CNktStringW cStrTempW;
  CNktDvDbObjectNoRef *lpChildDbObj;
  HRESULT hRes;

  lpChildDbObj = GetItem(nIndex);
  hRes = BuildArgumentStr(cStrTempW, lpChildDbObj);
  if (FAILED(hRes))
    return hRes;
  if (AddItemFlags(cStrTempW, GetItemFlags(nIndex)) == FALSE ||
      ReplaceX01OrAdd(cStrTempW, GetItemName(nIndex)) == FALSE ||
      InsertTabInEachLine(cStrTempW) == FALSE ||
      cStrDeclW.Concat((LPWSTR)cStrTempW) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldFunctionChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex)
{
  CNktDvDbObjectNoRef *lpChildDbObj;
  HRESULT hRes;

  cStrDeclW.Empty();
  if (nIndex == nChildsCount-1)
  {
    //result
    lpChildDbObj = GetFunctionReturn();
    hRes = BuildArgumentStr(cStrDeclW, lpChildDbObj);
    if (FAILED(hRes))
      return hRes;
  }
  else {
    lpChildDbObj = GetItem(nIndex);
    hRes = BuildArgumentStr(cStrDeclW, lpChildDbObj);
    if (FAILED(hRes))
      return hRes;
    if (AddItemFlags(cStrDeclW, GetItemFlags(nIndex)) == FALSE ||
        ReplaceX01OrAdd(cStrDeclW, GetItemName(nIndex)) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

HRESULT CNktDvDbObjectNoRef::BuilldEnumChildDefStr(__inout CNktStringW &cStrDeclW, __in SIZE_T nIndex)
{
  if (cStrDeclW.Concat(GetItemName(nIndex)) == FALSE ||
      cStrDeclW.Concat(L"=") == FALSE ||
      cStrDeclW.Concat(GetItemEnumValue(nIndex)) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

//-----------------------------------------------------------

static BOOL InsertTabInEachLine(__inout CNktStringW &cStrW)
{
  SIZE_T i, nLen;
  BOOL bPrevWasNewLine;

  bPrevWasNewLine = TRUE;
  nLen = cStrW.GetLength();
  for (i=0; i<nLen; i++)
  {
    if (bPrevWasNewLine != FALSE)
    {
      if (cStrW.Insert(L"\t", i) == FALSE)
        return FALSE;
      nLen++;
      bPrevWasNewLine = FALSE;
    }
    else {
      if (((LPWSTR)cStrW)[i] == L'\n')
        bPrevWasNewLine = TRUE;
    }
  }
  return TRUE;
}

static BOOL CheckX01(__in CNktStringW &cStrW)
{
  SIZE_T i, nLen;
  
  nLen = cStrW.GetLength();
  for (i=0; i<nLen; i++)
  {
    if (((LPWSTR)cStrW)[i] == L'\x01')
      return TRUE;
  }
  return FALSE;
}

static BOOL ReplaceX01(__inout CNktStringW &cStrW, __in LPCWSTR szNewString)
{
  SIZE_T i, nLen;

  nLen = cStrW.GetLength();
  for (i=0; i<nLen; i++)
  {
    if (((LPWSTR)cStrW)[i] == L'\x01')
    {
      cStrW.Delete(i, 1);
      return cStrW.Insert(szNewString, i);
    }
  }
  return TRUE;
}

static BOOL ReplaceX01OrAdd(__inout CNktStringW &cStrW, __in LPCWSTR szNewString)
{
  SIZE_T i, nLen;

  nLen = cStrW.GetLength();
  for (i=0; i<nLen; i++)
  {
    if (((LPWSTR)cStrW)[i] == L'\x01')
    {
      cStrW.Delete(i, 1);
      return cStrW.Insert(szNewString, i);
    }
  }
  if (szNewString[0] != 0) {
    if (cStrW.Concat(L" ") == FALSE ||
        cStrW.Concat(szNewString) == FALSE)
      return FALSE;
  }
  return TRUE;
}

static BOOL AddItemFlags(__inout CNktStringW &cStrW, __in int nFlags)
{
  if ((nFlags & CNktDvDbObjectNoRef::iflgIsVolatile) != 0)
  {
    if (cStrW.Insert(L"volatile ", 0) == FALSE)
      return FALSE;
  }
  if ((nFlags & CNktDvDbObjectNoRef::iflgIsConstant) != 0)
  {
    if (cStrW.Insert(L"const ", 0) == FALSE)
      return FALSE;
  }
  return TRUE;
}
