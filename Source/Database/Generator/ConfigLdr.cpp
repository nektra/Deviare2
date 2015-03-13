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

#include "ConfigLdr.h"

//-----------------------------------------------------------

class CConfig_Modules : public CNktMemMgrObj
{
public:
  CConfig_Modules();
  ~CConfig_Modules();

  BOOL Add(LPCWSTR szModuleW);
  LPWSTR Get(SIZE_T nIndex);

private:
  LPWSTR *lpItems;
  SIZE_T nCount, nSize;
};

class CConfig_BasicTypes : public CNktMemMgrObj
{
public:
  CConfig_BasicTypes();
  ~CConfig_BasicTypes();

  BOOL Add(LPCWSTR szNameW, LONG nFundType);
  LPWSTR Get(SIZE_T nIndex, LONG &nFundType);

private:
  typedef struct {
    LONG nFundType;
    WCHAR szNameW[1];
  } ITEM;

  ITEM **lpItems;
  SIZE_T nCount, nSize;
};

//-----------------------------------------------------------

static CConfig_Modules cCfgMod;
static CConfig_BasicTypes cCfgBasTyp;

//-----------------------------------------------------------

static BOOL ParseConfig_Modules(IXMLDOMElement *lpModElem);
static BOOL ParseConfig_BasicTypes(IXMLDOMElement *lpBtElem);
static BOOL ExpandEnvVars(CNktComBStr &cBStr);

//-----------------------------------------------------------

BOOL LoadConfig(LPCWSTR szSrcFileNameW)
{
  TNktComPtr<IXMLDOMDocument3> cXmlDocConfig;
  TNktComPtr<IXMLDOMElement> cXmlRootElem, cCurrElem, cNextElem;
  CNktComBStr cTempBStr;
  HRESULT hRes;

  cXmlDocConfig = LoadXML(szSrcFileNameW);
  if (cXmlDocConfig == NULL)
    return FALSE;
  //----
  hRes = cXmlDocConfig->get_documentElement(&cXmlRootElem);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_cnf_on_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes) || cXmlRootElem == NULL)
  {
    PrintAndLog(L"\nError: Cannot get root element (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  hRes = CXmlHelper::GetFirstChildElement(cXmlRootElem, &cCurrElem);
  if (hRes == E_OUTOFMEMORY)
    goto pxml_cnf_on_err_nomem;
  if (FAILED(hRes))
  {
pxml_cnf_parse_error:
    PrintAndLog(L"\nError: Cannot parse XML (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  while (cCurrElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cCurrElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_on_err_nomem;
    if (FAILED(hRes))
    {
      PrintAndLog(L"\nError: Cannot get element name (0x%08X).\n", (ULONG)hRes);
      return FALSE;
    }
    if (wcscmp((LPWSTR)cTempBStr, L"modules") == 0)
    {
      if (ParseConfig_Modules(cCurrElem) == FALSE)
        return FALSE;
    }
    else if (wcscmp((LPWSTR)cTempBStr, L"basicTypes") == 0)
    {
      if (ParseConfig_BasicTypes(cCurrElem) == FALSE)
        return FALSE;
    }
    else
    {
      PrintAndLog(L"\nError: Invalid tag name (%s).\n", (LPWSTR)cTempBStr);
      return FALSE;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cCurrElem, &cNextElem);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_on_err_nomem;
    if (FAILED(hRes))
      goto pxml_cnf_parse_error;
    cCurrElem.Attach(cNextElem.Detach());
  }
  //----
  return TRUE;
}

LPWSTR ConfigGetModuleName(ULONG nIndex)
{
  return cCfgMod.Get(nIndex);
}

LONG ConfigGetBasicTypeFromName(LPCWSTR szNameW)
{
  SIZE_T nIndex;
  LONG nFundType;
  LPWSTR sW;

  nIndex = 0;
  while (true) {
    sW = cCfgBasTyp.Get(nIndex, nFundType);
    if (sW == NULL)
      break;
    if (wcscmp(sW, szNameW) == 0)
      return nFundType;
    nIndex++;
  }
  return -1;
}

//-----------------------------------------------------------

static BOOL ParseConfig_Modules(IXMLDOMElement *lpModElem)
{
  TNktComPtr<IXMLDOMElement> cCurrElem, cNextElem;
  CNktComBStr cTempBStr;
  HRESULT hRes;

  hRes = CXmlHelper::GetFirstChildElement(lpModElem, &cCurrElem);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_cnf_mod_on_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
pxml_cnf_mod_parse_error:
    PrintAndLog(L"\nError: Cannot parse XML (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  while (cCurrElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cCurrElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_mod_on_err_nomem;
    if (FAILED(hRes)) {
      PrintAndLog(L"\nError: Cannot get element name (0x%08X).\n", (ULONG)hRes);
      return FALSE;
    }
    if (wcscmp((LPWSTR)cTempBStr, L"module") == 0)
    {
      //get text
      cTempBStr.Reset();
      hRes = CXmlHelper::GetElementText(cCurrElem, &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_cnf_mod_on_err_nomem;
      if (FAILED(hRes)) {
        PrintAndLog(L"\nError: Cannot get element text (0x%08X).\n", (ULONG)hRes);
        return FALSE;
      }
      if (cTempBStr[0] == 0) {
        PrintAndLog(L"\nError: No text found in 'module' tag.\n");
        return FALSE;
      }
      //expand environment variables
      if (ExpandEnvVars(cTempBStr) == FALSE)
        goto pxml_cnf_mod_on_err_nomem;
      //add item
      if (cCfgMod.Add(cTempBStr) == FALSE)
        goto pxml_cnf_mod_on_err_nomem;
    }
    else
    {
      PrintAndLog(L"\nError: Invalid tag name (%s).\n", (LPWSTR)cTempBStr);
      return FALSE;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cCurrElem, &cNextElem);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_mod_on_err_nomem;
    if (FAILED(hRes))
      goto pxml_cnf_mod_parse_error;
    cCurrElem.Attach(cNextElem.Detach());
  }
  return TRUE;
}

static BOOL ParseConfig_BasicTypes(IXMLDOMElement *lpBtElem)
{
  TNktComPtr<IXMLDOMElement> cCurrElem, cNextElem;
  CNktComBStr cTempBStr;
  HRESULT hRes;
  LONG nFundType;

  hRes = CXmlHelper::GetFirstChildElement(lpBtElem, &cCurrElem);
  if (hRes == E_OUTOFMEMORY)
  {
pxml_cnf_bastyp_on_err_nomem:
    PrintAndLogNoMemoryError();
    return FALSE;
  }
  if (FAILED(hRes))
  {
pxml_cnf_bastyp_parse_error:
    PrintAndLog(L"\nError: Cannot parse XML (0x%08X).\n", (ULONG)hRes);
    return FALSE;
  }
  while (cCurrElem != NULL)
  {
    //get element name
    cTempBStr.Reset();
    hRes = CXmlHelper::GetElementName(cCurrElem, &cTempBStr);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_bastyp_on_err_nomem;
    if (FAILED(hRes)) {
      PrintAndLog(L"\nError: Cannot get element name (0x%08X).\n", (ULONG)hRes);
      return FALSE;
    }
    if (wcscmp((LPWSTR)cTempBStr, L"type") == 0)
    {
      //get fundamental type
      cTempBStr.Reset();
      hRes = CXmlHelper::GetAttributeValue(cCurrElem, L"target", &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_cnf_bastyp_on_err_nomem;
      if (FAILED(hRes)) {
        PrintAndLog(L"\nError: Cannot get element 'target' attribute (0x%08X).\n", (ULONG)hRes);
        return FALSE;
      }
      if (wcscmp((LPWSTR)cTempBStr, L"uint8") == 0)
        nFundType = NKT_DBFUNDTYPE_SignedByte;
      else if (wcscmp((LPWSTR)cTempBStr, L"int8") == 0)
        nFundType = NKT_DBFUNDTYPE_UnsignedByte;
      else if (wcscmp((LPWSTR)cTempBStr, L"uint16") == 0)
        nFundType = NKT_DBFUNDTYPE_SignedWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"int16") == 0)
        nFundType = NKT_DBFUNDTYPE_UnsignedWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"uint32") == 0)
        nFundType = NKT_DBFUNDTYPE_SignedDoubleWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"int32") == 0)
        nFundType = NKT_DBFUNDTYPE_UnsignedDoubleWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"uint64") == 0)
        nFundType = NKT_DBFUNDTYPE_SignedQuadWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"int64") == 0)
        nFundType = NKT_DBFUNDTYPE_UnsignedQuadWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"size_t") == 0)
        nFundType = (nProcessingBits == 32) ? NKT_DBFUNDTYPE_UnsignedDoubleWord :
                                              NKT_DBFUNDTYPE_UnsignedQuadWord;
      else if (wcscmp((LPWSTR)cTempBStr, L"ssize_t") == 0)
        nFundType = (nProcessingBits == 32) ? NKT_DBFUNDTYPE_SignedDoubleWord :
                                              NKT_DBFUNDTYPE_SignedQuadWord;
      else
      {
        PrintAndLog(L"\nError: 'Target' attribute must be [u]int{8|16|32|64}|[s]size_t (%s).\n",
                    (LPWSTR)cTempBStr);
        return FALSE;
      }
      //get text
      cTempBStr.Reset();
      hRes = CXmlHelper::GetElementText(cCurrElem, &cTempBStr);
      if (hRes == E_OUTOFMEMORY)
        goto pxml_cnf_bastyp_on_err_nomem;
      if (FAILED(hRes))
      {
        PrintAndLog(L"\nError: Cannot get element text (0x%08X).\n", (ULONG)hRes);
        return FALSE;
      }
      //expand environment variables
      if (ExpandEnvVars(cTempBStr) == FALSE)
        goto pxml_cnf_bastyp_on_err_nomem;
      //add item
      if (cCfgBasTyp.Add(cTempBStr, nFundType) == FALSE)
        goto pxml_cnf_bastyp_on_err_nomem;
    }
    else
    {
      PrintAndLog(L"\nError: Invalid tag name (%s).\n", (LPWSTR)cTempBStr);
      return FALSE;
    }
    //get next element
    hRes = CXmlHelper::GetNextElement(cCurrElem, &cNextElem);
    if (hRes == E_OUTOFMEMORY)
      goto pxml_cnf_bastyp_on_err_nomem;
    if (FAILED(hRes))
      goto pxml_cnf_bastyp_parse_error;
    cCurrElem.Attach(cNextElem.Detach());
  }
  return TRUE;
}

//-----------------------------------------------------------

CConfig_Modules::CConfig_Modules() : CNktMemMgrObj()
{
  lpItems = NULL;
  nCount = nSize = 0;
  return;
}

CConfig_Modules::~CConfig_Modules()
{
  SIZE_T i;

  for (i=0; i<nCount; i++)
    nktMemFree(lpItems[i]);
  nktMemFree(lpItems);
  return;
}

BOOL CConfig_Modules::Add(LPCWSTR szModuleW)
{
  LPWSTR *lpNewItems, szCopyW;
  size_t nLen;

  NKT_ASSERT(szModuleW!=0 && szModuleW[0]!=0);
  if (nCount >= nSize)
  {
    lpNewItems = (LPWSTR*)nktMemMalloc((nSize+2048)*sizeof(LPWSTR));
    if (lpNewItems == NULL)
      return FALSE;
    nSize += 2048;
    nktMemCopy(lpNewItems, lpItems, nCount*sizeof(LPWSTR));
    if (lpItems != NULL)
      nktMemFree(lpItems);
    lpItems = lpNewItems;
  }
  nLen = wcslen(szModuleW) + 1;
  szCopyW = (LPWSTR)nktMemMalloc(nLen*sizeof(WCHAR));
  if (szCopyW == NULL)
    return FALSE;
  nktMemCopy(szCopyW, szModuleW, nLen*sizeof(WCHAR));
  lpItems[nCount++] = szCopyW;
  return TRUE;
}

LPWSTR CConfig_Modules::Get(SIZE_T nIndex)
{
  if (nIndex >= nCount)
    return NULL;
  return lpItems[nIndex];
}

//-----------------------------------------------------------

CConfig_BasicTypes::CConfig_BasicTypes() : CNktMemMgrObj()
{
  lpItems = NULL;
  nCount = nSize = 0;
  return;
}

CConfig_BasicTypes::~CConfig_BasicTypes()
{
  SIZE_T i;

  for (i=0; i<nCount; i++)
    nktMemFree(lpItems[i]);
  nktMemFree(lpItems);
  return;
}

BOOL CConfig_BasicTypes::Add(LPCWSTR szNameW, LONG nFundType)
{
  ITEM **lpNewItems, *lpItem;
  size_t nLen;

  NKT_ASSERT(szNameW!=0 && szNameW[0]!=0 && nFundType!=0);
  if (nCount >= nSize)
  {
    lpNewItems = (ITEM**)nktMemMalloc((nSize+2048)*sizeof(ITEM*));
    if (lpNewItems == NULL)
      return FALSE;
    nSize += 2048;
    nktMemCopy(lpNewItems, lpItems, nCount*sizeof(ITEM*));
    if (lpItems != NULL)
      nktMemFree(lpItems);
    lpItems = lpNewItems;
  }
  nLen = wcslen(szNameW) + 1;
  lpItem = (ITEM*)nktMemMalloc(sizeof(LONG)+nLen*sizeof(WCHAR));
  if (lpItem == NULL)
    return FALSE;
  lpItem->nFundType = nFundType;
  nktMemCopy(lpItem->szNameW, szNameW, nLen*sizeof(WCHAR));
  lpItems[nCount++] = lpItem;
  return TRUE;
}

LPWSTR CConfig_BasicTypes::Get(SIZE_T nIndex, LONG &nFundType)
{
  if (nIndex >= nCount)
  {
    nFundType = 0;
    return NULL;
  }
  nFundType = lpItems[nIndex]->nFundType;
  return lpItems[nIndex]->szNameW;
}

static BOOL ExpandEnvVars(CNktComBStr &cBStr)
{
  CNktStringW cStrTempW;
  WCHAR szBufW[4096], szTempW[1024], *sW;
  SIZE_T k, nLen;
  BOOL bLastSlash;

  bLastSlash = FALSE;
  sW = (LPWSTR)cBStr;
  NKT_ASSERT(sW != NULL);
  while (*sW != 0)
  {
    if (*sW == L'%')
    {
      if (sW[1]==0 || sW[1]==L'%')
      {
        if (cStrTempW.ConcatN(sW, 1) == FALSE)
          return FALSE;
        sW += 2;
        bLastSlash = FALSE;
      }
      else {
        szTempW[0] = *sW++;
        for (k=1; k<1020; k++)
        {
          szTempW[k] = *sW++;
          if (szTempW[k] == L'%')
            break;
        }
        szTempW[k+1] = 0;
        szBufW[0] = 0;
        ::ExpandEnvironmentStringsW(szTempW, szBufW, 4096);
        szBufW[4095] = 0;
        nLen = wcslen(szBufW);
        k = 0;
        if (bLastSlash!=FALSE && szBufW[k]==L'\\')
        {
          k++;
          nLen--;
        }
        if (nLen > 0)
        {
          if (cStrTempW.ConcatN(szBufW+k, nLen) == FALSE)
            return FALSE;
          bLastSlash = (szBufW[k+nLen-1] == L'\\') ? TRUE : FALSE;
        }
      }
    }
    else if (*sW==L'/' || *sW==L'\\')
    {
      if (bLastSlash == FALSE)
      {
        if (cStrTempW.ConcatN(L"\\", 1) == FALSE)
          return FALSE;
        bLastSlash = TRUE;
      }
      sW++;
    }
    else
    {
      if (cStrTempW.ConcatN(sW, 1) == FALSE)
        return FALSE;
      bLastSlash = FALSE;
      sW++;
    }
  }
  return (SUCCEEDED(cBStr.Set((LPWSTR)cStrTempW))) ? TRUE : FALSE;
}
