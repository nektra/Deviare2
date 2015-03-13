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

#include "XmlHelpers.h"
#include <stdio.h>

//-----------------------------------------------------------

#define X_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

CXmlHelper::CXmlHelper() : CNktMemMgrObj()
{
  lpIdElemMapEntries = NULL;
  nIdElemMapEntriesCount = nIdElemMapEntriesSize = 0;
  return;
}

CXmlHelper::~CXmlHelper()
{
  Reset();
  return;
}

VOID CXmlHelper::Reset()
{
  SIZE_T i;

  if (lpIdElemMapEntries != NULL)
  {
    //this takes a long time
    for (i=0; i<nIdElemMapEntriesCount; i++)
    {
      if ((i & 31) == 0)
      {
        wprintf(L"\rFreeing id-elem map entries (%.2lf%%)... ",
                ((float)i / (float)nIdElemMapEntriesCount) * 100.0f);
      }
      lpIdElemMapEntries[i].lpElem->Release();
    }
    wprintf(L"\r                                                                    \r");
    nktMemFree(lpIdElemMapEntries);
    lpIdElemMapEntries = NULL;
    nIdElemMapEntriesCount = nIdElemMapEntriesSize = 0;
  }
  return;
}

HRESULT CXmlHelper::Initialize(IXMLDOMElement *lpRootElem)
{
  TNktComPtr<IXMLDOMElement> cCurrElem, cNextElem;
  CNktComBStr cElemAttrIdBStr, cNameBstr;
  HRESULT hRes;
  SIZE_T i;
  LONG k;

  nktMemSet(aFundTypesIndexes, 0, sizeof(aFundTypesIndexes));
  hRes = GetFirstChildElement(lpRootElem, &cCurrElem);
  if (FAILED(hRes))
    return hRes;
  while (cCurrElem != NULL)
  {
    cElemAttrIdBStr.Reset();
    hRes = GetAttributeValue(cCurrElem, L"id", &cElemAttrIdBStr);
    if (FAILED(hRes))
    {
init_on_error:
      Reset();
      return hRes;
    }
    if (cElemAttrIdBStr[0] != 0)
    {
      //add this element
      if (nIdElemMapEntriesCount >= nIdElemMapEntriesSize)
      {
        ID_ELEM_MAP_ENTRY *lpNew;

        lpNew = (ID_ELEM_MAP_ENTRY*)nktMemMalloc((nIdElemMapEntriesSize+10240)*sizeof(ID_ELEM_MAP_ENTRY));
        if (lpNew == NULL) {
          hRes = E_OUTOFMEMORY;
          goto init_on_error;
        }
        nIdElemMapEntriesSize += 10240;
        if (lpIdElemMapEntries != NULL)
        {
          nktMemCopy(lpNew, lpIdElemMapEntries, nIdElemMapEntriesCount*sizeof(ID_ELEM_MAP_ENTRY));
          nktMemFree(lpIdElemMapEntries);
        }
        lpIdElemMapEntries = lpNew;
      }
      wcsncpy_s(lpIdElemMapEntries[nIdElemMapEntriesCount].szIdW, cElemAttrIdBStr, 16);
      lpIdElemMapEntries[nIdElemMapEntriesCount].szIdW[15] = 0;
      k = GetDbObjectClass(cCurrElem, 1);
      if (k == -1)
      {
        hRes = E_OUTOFMEMORY;
        goto init_on_error;
      }
      if (k == NKT_DBOBJCLASS_Fundamental)
      {
        cNameBstr.Reset();
        hRes = GetAttributeValue(cCurrElem, L"name", &cNameBstr);
        if (FAILED(hRes))
          goto init_on_error;
        if (_wcsicmp((LPWSTR)cNameBstr, L"signed char") == 0)
          k = NKT_DBFUNDTYPE_SignedByte;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"char") == 0)
          k = NKT_DBFUNDTYPE_AnsiChar;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"unsigned char") == 0)
          k = NKT_DBFUNDTYPE_UnsignedByte;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"short int") == 0)
          k = NKT_DBFUNDTYPE_SignedWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"short unsigned int") == 0)
          k = NKT_DBFUNDTYPE_UnsignedWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"int") == 0 ||
                 _wcsicmp((LPWSTR)cNameBstr, L"long int") == 0 ||
                 _wcsicmp((LPWSTR)cNameBstr, L"bool") == 0)
          k = NKT_DBFUNDTYPE_SignedDoubleWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"unsigned int") == 0 ||
                 _wcsicmp((LPWSTR)cNameBstr, L"long unsigned int") == 0)
          k = NKT_DBFUNDTYPE_UnsignedDoubleWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"long long int") == 0)
          k = NKT_DBFUNDTYPE_SignedQuadWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"long long unsigned int") == 0)
          k = NKT_DBFUNDTYPE_UnsignedQuadWord;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"float") == 0)
          k = NKT_DBFUNDTYPE_Float;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"double") == 0)
          k = NKT_DBFUNDTYPE_Double;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"long double") == 0)
          k = NKT_DBFUNDTYPE_LongDouble;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"wchar_t") == 0)
          k = NKT_DBFUNDTYPE_WideChar;
        else if (_wcsicmp((LPWSTR)cNameBstr, L"void") == 0)
          k = NKT_DBFUNDTYPE_Void;
        else {
          hRes = E_FAIL;
          goto init_on_error;
        }
        aFundTypesIndexes[k-NKT_DBFUNDTYPE_MIN] = 1;
      }
      else
        k = 0;
      lpIdElemMapEntries[nIdElemMapEntriesCount].nFundamentalType = (ULONG)k;
      lpIdElemMapEntries[nIdElemMapEntriesCount].lpElem = cCurrElem;
      lpIdElemMapEntries[nIdElemMapEntriesCount].lpElem->AddRef();
      nIdElemMapEntriesCount++;
    }
    //get next element
    hRes = GetNextElement(cCurrElem, &cNextElem);
    if (FAILED(hRes))
      goto init_on_error;
    cCurrElem.Attach(cNextElem.Detach());
  }
  //fast fundamental quick check
  for (k=0; k<X_ARRAYLEN(aFundTypesIndexes); k++) {
    if (aFundTypesIndexes[k] == 0) {
      hRes = E_FAIL;
      goto init_on_error;
    }
  }
  //sort elements by id
  qsort_s(lpIdElemMapEntries, nIdElemMapEntriesCount, sizeof(ID_ELEM_MAP_ENTRY), IdElemMapEntry_Compare,
          NULL);
  //find each fundamental
  nktMemSet(aFundTypesIndexes, 0, sizeof(aFundTypesIndexes));
  for (i=0; i<nIdElemMapEntriesCount; i++)
  {
    if (lpIdElemMapEntries[i].nFundamentalType != 0)
      aFundTypesIndexes[lpIdElemMapEntries[i].nFundamentalType - NKT_DBFUNDTYPE_MIN] = i;
  }
  //fast fundamental quick check (should not happen)
  for (k=0; k<X_ARRAYLEN(aFundTypesIndexes); k++) {
    if (aFundTypesIndexes[k] == 0) {
      hRes = E_FAIL;
      goto init_on_error;
    }
  }
  return S_OK;
}

LPWSTR CXmlHelper::GetIdFromFundamentalType(SIZE_T nFundType)
{
  if (nFundType < NKT_DBFUNDTYPE_MIN || nFundType > NKT_DBFUNDTYPE_MAX)
    return L"";
  return lpIdElemMapEntries[aFundTypesIndexes[nFundType-NKT_DBFUNDTYPE_MIN]].szIdW;
}

IXMLDOMElement* CXmlHelper::GetElementFromFundamentalType(SIZE_T nFundType)
{
  if (nFundType < NKT_DBFUNDTYPE_MIN || nFundType > NKT_DBFUNDTYPE_MAX)
    return NULL;
  return lpIdElemMapEntries[aFundTypesIndexes[nFundType-NKT_DBFUNDTYPE_MIN]].lpElem;
}

IXMLDOMElement* CXmlHelper::GetElementFromIndex(SIZE_T nIndex)
{
  if (nIndex >= nIdElemMapEntriesCount)
    return NULL;
  return lpIdElemMapEntries[nIndex].lpElem;
}

LPWSTR CXmlHelper::GetElementIdFromIndex(SIZE_T nIndex)
{
  if (nIndex >= nIdElemMapEntriesCount)
    return NULL;
  return lpIdElemMapEntries[nIndex].szIdW;
}

IXMLDOMElement* CXmlHelper::GetElementFromId(ULONG nId, BOOL bIsConstant, BOOL bIsVolatile)
{
  WCHAR szBufW[32];

  swprintf_s(szBufW, 32, L"_%lu%s%s", nId, (bIsConstant != FALSE) ? L"c" : L"",
             (bIsVolatile != FALSE) ? L"v" : L"");
  return GetElementFromId(szBufW);
}

IXMLDOMElement* CXmlHelper::GetElementFromId(LPCWSTR szIdW)
{
  ID_ELEM_MAP_ENTRY sKey, *lpRes;

  if (szIdW==NULL || lpIdElemMapEntries==NULL)
    return NULL;
  wcsncpy_s(sKey.szIdW, szIdW, 16);
  sKey.szIdW[15] = 0;
  lpRes = (ID_ELEM_MAP_ENTRY*)bsearch_s(&sKey, lpIdElemMapEntries, nIdElemMapEntriesCount,
                                        sizeof(ID_ELEM_MAP_ENTRY), IdElemMapEntry_Compare, NULL);
  return (lpRes != NULL) ? lpRes->lpElem : NULL;
}

LONG CXmlHelper::GetFundamentalTypeFromId(ULONG nId)
{
  ID_ELEM_MAP_ENTRY sKey, *lpRes;

  if (lpIdElemMapEntries == NULL)
    return NULL;
  swprintf_s(sKey.szIdW, 16, L"_%lu", nId);
  lpRes = (ID_ELEM_MAP_ENTRY*)bsearch_s(&sKey, lpIdElemMapEntries, nIdElemMapEntriesCount,
                                        sizeof(ID_ELEM_MAP_ENTRY), IdElemMapEntry_Compare, NULL);
  return (lpRes != NULL) ? lpRes->nFundamentalType : -1;
}

LONG CXmlHelper::GetDbObjectClass(IXMLDOMElement *lpElem, LONG nType2Check)
{
  CNktComBStr bStr;

  if (FAILED(GetElementName(lpElem, &bStr)))
    return -1;
  switch (nType2Check)
  {
    case 1:
      if (_wcsicmp((LPWSTR)bStr, L"FundamentalType") == 0)
        return NKT_DBOBJCLASS_Fundamental;
      if (_wcsicmp((LPWSTR)bStr, L"Struct") == 0 ||
          _wcsicmp((LPWSTR)bStr, L"Class") == 0)
        return NKT_DBOBJCLASS_Struct;
      if (_wcsicmp((LPWSTR)bStr, L"Union") == 0)
        return NKT_DBOBJCLASS_Union;
      if (_wcsicmp((LPWSTR)bStr, L"Typedef") == 0)
        return NKT_DBOBJCLASS_Typedef;
      if (_wcsicmp((LPWSTR)bStr, L"ArrayType") == 0)
        return NKT_DBOBJCLASS_Array;
      if (_wcsicmp((LPWSTR)bStr, L"PointerType") == 0)
        return NKT_DBOBJCLASS_Pointer;
      if (_wcsicmp((LPWSTR)bStr, L"ReferenceType") == 0)
        return NKT_DBOBJCLASS_Reference;
      if (_wcsicmp((LPWSTR)bStr, L"Enumeration") == 0)
        return NKT_DBOBJCLASS_Enumeration;
      if (_wcsicmp((LPWSTR)bStr, L"FunctionType") == 0)
        return NKT_DBOBJCLASS_FunctionType;
      break;
    case 2:
      if (_wcsicmp((LPWSTR)bStr, L"Field") == 0)
        return SPECIAL_DBOBJ_CLASS_Field;
      if (_wcsicmp((LPWSTR)bStr, L"Constructor") == 0)
        return NKT_DBOBJCLASS_ClassConstructor;
      if (_wcsicmp((LPWSTR)bStr, L"Destructor") == 0)
        return NKT_DBOBJCLASS_ClassDestructor;
      if (_wcsicmp((LPWSTR)bStr, L"OperatorMethod") == 0)
        return NKT_DBOBJCLASS_ClassOperatorMethod;
      if (_wcsicmp((LPWSTR)bStr, L"Method") == 0)
        return NKT_DBOBJCLASS_ClassMethod;
      if (_wcsicmp((LPWSTR)bStr, L"Converter") == 0)
        return NKT_DBOBJCLASS_ClassConverter;
      if (_wcsicmp((LPWSTR)bStr, L"Variable") == 0)
        return SPECIAL_DBOBJ_CLASS_Variable;
      if (_wcsicmp((LPWSTR)bStr, L"Struct") == 0 ||
          _wcsicmp((LPWSTR)bStr, L"Class") == 0)
        return NKT_DBOBJCLASS_Struct;
      if (_wcsicmp((LPWSTR)bStr, L"Union") == 0)
        return NKT_DBOBJCLASS_Union;
      if (_wcsicmp((LPWSTR)bStr, L"Typedef") == 0)
        return NKT_DBOBJCLASS_Typedef;
      if (_wcsicmp((LPWSTR)bStr, L"Enumeration") == 0)
        return NKT_DBOBJCLASS_Enumeration;
      if (_wcsicmp((LPWSTR)bStr, L"FunctionType") == 0)
        return NKT_DBOBJCLASS_FunctionType;
      break;
  }
  return -2;
}

HRESULT CXmlHelper::GetFirstChildElement(IXMLDOMElement *lpElem, IXMLDOMElement **lplpFirstElem)
{
  IXMLDOMNode *lpNode, *lpNextNode;
  DOMNodeType nNodeType;
  HRESULT hRes;

  if (lplpFirstElem == NULL)
    return E_INVALIDARG;
  *lplpFirstElem = NULL;
  if (lpElem == NULL)
    return E_INVALIDARG;
  lpNode = NULL;
  hRes = lpElem->get_firstChild(&lpNode);
  if (FAILED(hRes))
    return hRes;
  while (lpNode != NULL)
  {
    hRes = lpNode->get_nodeType(&nNodeType);
    if (FAILED(hRes))
    {
      lpNode->Release();
      return hRes;
    }
    if (nNodeType == NODE_ELEMENT)
      break;
    lpNextNode = NULL;
    hRes = lpNode->get_nextSibling(&lpNextNode);
    lpNode->Release();
    if (FAILED(hRes))
      return hRes;
    lpNode = lpNextNode;
  }
  if (lpNode != NULL)
  {
    hRes = lpNode->QueryInterface(IID_IXMLDOMElement, (void**)lplpFirstElem);
    lpNode->Release();
    if (FAILED(hRes))
    {
      *lplpFirstElem = NULL;
      return hRes;
    }
    if (*lplpFirstElem == NULL)
      return E_FAIL;
  }
  return S_OK;
}

HRESULT CXmlHelper::GetNextElement(IXMLDOMElement *lpElem, IXMLDOMElement **lplpNextElem)
{
  IXMLDOMNode *lpNode, *lpNextNode;
  DOMNodeType nNodeType;
  HRESULT hRes;

  if (lplpNextElem == NULL)
    return E_INVALIDARG;
  *lplpNextElem = NULL;
  if (lpElem == NULL)
    return E_INVALIDARG;
  lpNode = NULL;
  hRes = lpElem->get_nextSibling(&lpNode);
  if (FAILED(hRes))
    return hRes;
  while (lpNode != NULL)
  {
    hRes = lpNode->get_nodeType(&nNodeType);
    if (FAILED(hRes))
    {
      lpNode->Release();
      return hRes;
    }
    if (nNodeType == NODE_ELEMENT)
      break;
    lpNextNode = NULL;
    hRes = lpNode->get_nextSibling(&lpNextNode);
    lpNode->Release();
    if (FAILED(hRes))
      return hRes;
    lpNode = lpNextNode;
  }
  if (lpNode != NULL)
  {
    hRes = lpNode->QueryInterface(IID_IXMLDOMElement, (void**)lplpNextElem);
    lpNode->Release();
    if (FAILED(hRes))
    {
      *lplpNextElem = NULL;
      return hRes;
    }
    if (*lplpNextElem == NULL)
      return E_FAIL;
  }
  return S_OK;
}

HRESULT CXmlHelper::GetElementName(IXMLDOMElement *lpElem, BSTR *lpbStrDest)
{
  HRESULT hRes;

  if (lpbStrDest == NULL)
    return E_INVALIDARG;
  *lpbStrDest = NULL;
  if (lpElem == NULL)
    return E_INVALIDARG;
  *lpbStrDest = NULL;
  hRes = lpElem->get_nodeName(lpbStrDest);
  if (FAILED(hRes))
    return hRes;
  return (*lpbStrDest != NULL) ? S_OK : E_FAIL;
}

HRESULT CXmlHelper::GetElementText(IXMLDOMElement *lpElem, BSTR *lpbStrDest)
{
  HRESULT hRes;

  if (lpbStrDest == NULL)
    return E_INVALIDARG;
  *lpbStrDest = NULL;
  if (lpElem == NULL)
    return E_INVALIDARG;
  *lpbStrDest = NULL;
  hRes = lpElem->get_text(lpbStrDest);
  if (FAILED(hRes))
    return hRes;
  return (*lpbStrDest != NULL) ? S_OK : E_FAIL;
}

HRESULT CXmlHelper::GetAttributeValue(IXMLDOMElement *lpElem, LPCWSTR szAttrNameW, BSTR *lpbStrDest)
{
  VARIANT sVt;
  HRESULT hRes;

  if (lpbStrDest == NULL)
    return E_INVALIDARG;
  *lpbStrDest = NULL;
  if (lpElem==NULL || szAttrNameW==NULL || szAttrNameW[0]==0)
    return E_INVALIDARG;
  hRes = lpElem->getAttribute((BSTR)szAttrNameW, &sVt);
  if (FAILED(hRes))
    return hRes;
  if (sVt.vt == VT_BSTR)
  {
    *lpbStrDest = sVt.bstrVal;
  }
  else
  {
    ::VariantClear(&sVt);
    *lpbStrDest = ::SysAllocString(L"");
    if (*lpbStrDest == NULL)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

int CXmlHelper::IdElemMapEntry_Compare(void *ctx, const void *lp1, const void *lp2)
{
  return wcscmp(((ID_ELEM_MAP_ENTRY*)lp1)->szIdW, ((ID_ELEM_MAP_ENTRY*)lp2)->szIdW);
}
