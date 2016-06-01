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

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include "..\..\Common\ComPtr.h"
#include "..\..\Common\StringLiteW.h"
#include "..\..\Common\ArrayList.h"
#include "..\..\Common\AutoPtr.h"
#include "misc.h"

#if defined _M_IX86
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib.lib")
  #endif //_DEBUG
#elif defined _M_X64
  #ifdef _DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64_Debug.lib")
  #else //_DEBUG
    #pragma comment(lib, "..\\..\\..\\Libs\\Lz4Lib64.lib")
  #endif //_DEBUG
#else
  #error Unsupported platform
#endif

//http://spec.winprog.org/typeinf2/
//-----------------------------------------------------------

#define MAX_FUNCTION_NAMES 64

#define X_ARRAYLEN(x) (sizeof(x)/sizeof(x[0]))

#define E_CANNOTWRITEFILE HRESULT_FROM_WIN32(ERROR_WRITE_FAULT)

#define EXIT_ON_ERROR(hRes) if (FAILED(hRes)) return hRes

//-----------------------------------------------------------

typedef TNktArrayListWithFree<LPWSTR> WSTRING_LIST;

typedef struct {
  TNktComPtr<ITypeInfo> cTypeInfo;
  TYPEKIND nKind;
  TNktAutoFreePtr<WCHAR> szNameW;
  WSTRING_LIST sDependencyList;
} TYPEINFO_ITEM;

typedef TNktArrayListWithDelete<TYPEINFO_ITEM*> TYPEINFO_LIST;

typedef TNktArrayListWithRelease<ITypeInfo*> ITYPEINFO_LIST;

//-----------------------------------------------------------

static HRESULT DoLoadTypeLib(__deref_out ITypeLib **lplpTypeLib, __in LPCTSTR szFileName);

static HRESULT Process(__in FILE *fp, __in ITypeLib *lpTypeLib);

static HRESULT BuildTypeList(__in ITypeLib *lpTypeLib, __inout TYPEINFO_LIST &aTypeInfoList);
static HRESULT BuildTypeListDependencies(__in ITypeLib *lpTypeLib, __in TYPEINFO_LIST &aTypeInfoList);
static HRESULT FindTypedefDependencies(__in ITypeLib *lpTypeLib, __in TYPEINFO_ITEM *lpTypeInfoItem);
static HRESULT FindInterfaceDependencies(__in ITypeLib *lpTypeLib, __in TYPEINFO_ITEM *lpTypeInfoItem);
static HRESULT AddTypeInfoItemDependency(__in TYPEINFO_ITEM *lpTypeInfoItem, __in_z LPWSTR szW);
static VOID RemoveArrayAndPunctuatorChars(__inout CNktStringW &cStrW);

static VOID SortTypeList(__in ITypeLib *lpTypeLib, __inout TYPEINFO_LIST &aTypeInfoList);
static int __cdecl SortTypeList_Compare(void *, const void *lpElem1, const void *lpElem2);

static HRESULT ProcessHeader(__in FILE *fp, __in ITypeLib *lpTypeLib);
static HRESULT ProcessTypedef(__in FILE *fp, __in TYPEINFO_ITEM *lpTypeInfoItem);
static HRESULT ProcessInterface(__in FILE *fp, __in TYPEINFO_ITEM *lpTypeInfoItem, __in SIZE_T nPass);

static HRESULT ProcessVariable(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                               __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent);
static HRESULT ProcessConst(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                            __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent, __in BOOL bShowConst);
static HRESULT ProcessFunction(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                               __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent, __in LPWSTR szParentThisW);

static HRESULT CustomTypeToString(__inout CNktStringW &cStrDestW, __in HREFTYPE hRefType,
                                  __in ITypeInfo *lpTypeInfo);
static HRESULT TypeDescToString(__inout CNktStringW &cStrDestW, __in TYPEDESC *lpTypeDesc,
                                __in ITypeInfo *lpTypeInfo, __in BOOL bDependencyCheck);

static BOOL GuidFromSz(__in LPCTSTR s, __out GUID *lpGuid);
static HRESULT GetTypeLibVersion(__in REFGUID rGuid, __out WORD &wVerMajor, __out WORD &wVerMinor);

static BOOL WriteIndent(__in FILE *fp, __in SIZE_T nIndent);
static BOOL WriteBSTR(__in FILE *fp, __in BSTR bstr);
static BOOL WriteString(__in FILE *fp, __in_z LPCWSTR szStringW, __in_opt SIZE_T nLength=NKT_SIZE_T_MAX);
static BOOL WriteSeparator(__in FILE *fp);

static HRESULT BuildInterfaceBaseList(ITYPEINFO_LIST &cTypeInfoList, __in ITypeInfo *lpTypeInfo);

//-----------------------------------------------------------

#define WRITEANSI_AND_CHECK(fp, str)                       \
  if (fprintf(fp, "%s", str) == 0) return E_CANNOTWRITEFILE
#define WRITEWIDE_AND_CHECK(fp, str)                       \
  if (fprintf(fp, "%S", str) == 0) return E_CANNOTWRITEFILE
#define WRITEINDENT_AND_CHECK(fp, ind)                     \
  if (WriteIndent(fp, ind) == FALSE) return E_CANNOTWRITEFILE
#define WRITEBSTR_AND_CHECK(fp, bstr)                      \
  if (WriteBSTR(fp, bstr) == FALSE) return E_CANNOTWRITEFILE
#define WRITESTRING_AND_CHECK(fp, szStringW)               \
  if (WriteString(fp, szStringW) == FALSE) return E_CANNOTWRITEFILE
#define WRITESTRINGLEN_AND_CHECK(fp, szStringW, nLength)   \
  if (WriteString(fp, szStringW, nLength) == FALSE) return E_CANNOTWRITEFILE
#define WRITESEPARATOR_AND_CHECK(fp)                       \
  if (WriteSeparator(fp) == FALSE) return E_CANNOTWRITEFILE

//-----------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
  TNktComPtr<ITypeLib> cTypeLib;
  FILE *fp;
  HRESULT hRes;

  if (argc != 3)
  {
    _tprintf_s(_T("Use: Tlb2H input output-h-file.\n\n"));
    _tprintf_s(_T("Where 'input' can be a .TLB, .DLL or .OCX file or a registry GUID\n"));
    _tprintf_s(_T("  and 'output' is target header filename.\n"));
    return 1;
  }
  hRes = ::CoInitialize(0);
  if (FAILED(hRes))
  {
    _tprintf_s(_T("Error (%08X): Cannot initialize COM.\n"), (ULONG)hRes);
    return 1;
  }
  hRes = DoLoadTypeLib(&cTypeLib, argv[1]);
  if (FAILED(hRes))
  {
    ::CoUninitialize();
    _tprintf_s(_T("Error (%08X): Cannot load type library.\n"), (ULONG)hRes);
    return 1;
  }
  //write destinantion file
  if (_tfopen_s(&fp, argv[2], _T("wb")) != 0)
  {
    ::CoUninitialize();
    _tprintf_s(_T("Error: Cannot create destination file.\n"));
    return 1;
  }
  hRes = Process(fp, cTypeLib);
  fclose(fp);
  ::CoUninitialize();
  if (FAILED(hRes))
  {
    if (hRes == E_CANNOTWRITEFILE)
      _tprintf_s(_T("Error: Cannot write destination file.\n"));
    else
      _tprintf_s(_T("Error (%08X): Cannot process type library.\n"), (ULONG)hRes);
    return 1;
  }
  //done
  _tprintf_s(_T("Success!\n"));
  return 0;
}

//-----------------------------------------------------------

static HRESULT DoLoadTypeLib(__deref_out ITypeLib **lplpTypeLib, __in LPCTSTR szFileName)
{
  GUID sGuid;

  if (lplpTypeLib == NULL || szFileName == NULL)
    return E_POINTER;
  *lplpTypeLib = NULL;
  if (GuidFromSz(szFileName, &sGuid) != FALSE)
  {
    WORD wVerMajor, wVerMinor;
    HRESULT hRes;

    hRes = GetTypeLibVersion(sGuid, wVerMajor, wVerMinor);
    EXIT_ON_ERROR(hRes);
    return ::LoadRegTypeLib(sGuid, wVerMajor, wVerMinor, ::GetUserDefaultLCID(), lplpTypeLib);
  }
  return ::LoadTypeLibEx(szFileName, REGKIND_NONE, lplpTypeLib);
}

static HRESULT Process(__in FILE *fp, __in ITypeLib *lpTypeLib)
{
  TYPEINFO_LIST aTypeInfoList;
  SIZE_T i;
  HRESULT hRes;
  BOOL b;

  if (lpTypeLib == NULL)
    return E_POINTER;
  hRes = BuildTypeList(lpTypeLib, aTypeInfoList);
  EXIT_ON_ERROR(hRes);
  hRes = BuildTypeListDependencies(lpTypeLib, aTypeInfoList);
  EXIT_ON_ERROR(hRes);
  SortTypeList(lpTypeLib, aTypeInfoList);
  hRes = ProcessHeader(fp, lpTypeLib);
  EXIT_ON_ERROR(hRes);
  for (i=0; i<aTypeInfoList.GetCount(); i++)
  {
    if (aTypeInfoList[i]->nKind == TKIND_ENUM)
    {
      hRes = ProcessTypedef(fp, aTypeInfoList[i]);
      EXIT_ON_ERROR(hRes);
      WRITEANSI_AND_CHECK(fp, "\r\n");
      WRITESEPARATOR_AND_CHECK(fp);
    }
  }
  for (i=0; i<aTypeInfoList.GetCount(); i++)
  {
    switch (aTypeInfoList[i]->nKind)
    {
      case TKIND_RECORD:
      case TKIND_UNION:
      case TKIND_ALIAS:
        hRes = ProcessTypedef(fp, aTypeInfoList[i]);
        EXIT_ON_ERROR(hRes);
        WRITEANSI_AND_CHECK(fp, "\r\n");
        WRITESEPARATOR_AND_CHECK(fp);
        break;
    }
  }
  for (i=0,b=FALSE; i<aTypeInfoList.GetCount(); i++)
  {
    if (aTypeInfoList[i]->nKind == TKIND_INTERFACE)
    {
      hRes = ProcessInterface(fp, aTypeInfoList[i], 1);
      EXIT_ON_ERROR(hRes);
      b = TRUE;
    }
  }
  if (b != FALSE)
  {
    WRITEANSI_AND_CHECK(fp, "\r\n");
    WRITESEPARATOR_AND_CHECK(fp);
  }
  for (i=0; i<aTypeInfoList.GetCount(); i++)
  {
    if (aTypeInfoList[i]->nKind == TKIND_INTERFACE)
    {
      hRes = ProcessInterface(fp, aTypeInfoList[i], 2);
      EXIT_ON_ERROR(hRes);
      WRITEANSI_AND_CHECK(fp, "\r\n");
      WRITESEPARATOR_AND_CHECK(fp);
    }
  }
  return S_OK;
}

static HRESULT BuildTypeList(__in ITypeLib *lpTypeLib, __inout TYPEINFO_LIST &aTypeInfoList)
{
  TNktAutoDeletePtr<TYPEINFO_ITEM> cTypeInfoItem;
  TNktComPtr<ITypeInfo> cTypeInfo;
  CNktStringW cStrTempW;
  UINT i, nCount;
  HRESULT hRes;
  TYPEKIND nKind;
  CNktComBStr bstrName;

  nCount = lpTypeLib->GetTypeInfoCount();
  for (i=0; i<nCount; i++)
  {
    hRes = lpTypeLib->GetTypeInfoType(i, &nKind);
    EXIT_ON_ERROR(hRes);
    cTypeInfo.Release();
    hRes = lpTypeLib->GetTypeInfo(i, &cTypeInfo);
    EXIT_ON_ERROR(hRes);
    switch (nKind)
    {
      case TKIND_DISPATCH:
        {
        TNktComPtr<ITypeInfo> cIntTypeInfo;
        HREFTYPE hRefType;
        CAutoTypeAttr cTypeAttr;

        //check if dispatch interface has an interface inside
        hRes = cTypeInfo->GetRefTypeOfImplType(-1, &hRefType);
        if (SUCCEEDED(hRes))
          hRes = cTypeInfo->GetRefTypeInfo(hRefType, &cIntTypeInfo);
        if (SUCCEEDED(hRes))
          hRes = cTypeAttr.Set(cIntTypeInfo);
        if (FAILED(hRes))
          break;
        if (cTypeAttr->typekind != TKIND_INTERFACE)
          break;
        //got the internal interface
        nKind = TKIND_INTERFACE;
        cTypeInfo = cIntTypeInfo;
        }
        //fall into interface

      case TKIND_ENUM:
      case TKIND_RECORD:
      case TKIND_UNION:
      case TKIND_ALIAS:
      case TKIND_INTERFACE:
        cTypeInfoItem.Attach(new TYPEINFO_ITEM);
        if (cTypeInfoItem == NULL)
          return E_OUTOFMEMORY;
        cTypeInfoItem->nKind = nKind;
        cTypeInfoItem->cTypeInfo = cTypeInfo;
        bstrName.Reset();
        hRes = cTypeInfoItem->cTypeInfo->GetDocumentation(MEMBERID_NIL, &bstrName, 0, 0, 0);
        EXIT_ON_ERROR(hRes);
        if (bstrName != NULL && bstrName[0] != 0)
        {
          if (cStrTempW.Copy(bstrName) == FALSE)
            return E_OUTOFMEMORY;
          cTypeInfoItem->szNameW.Attach(cStrTempW.Detach());
        }
        if (aTypeInfoList.AddElement(cTypeInfoItem) == FALSE)
          return E_OUTOFMEMORY;
        cTypeInfoItem.Detach();
        break;
    }
  }
  return S_OK;
}

//------------------------------------------------------------------------------------

static HRESULT BuildTypeListDependencies(__in ITypeLib *lpTypeLib, TYPEINFO_LIST &aTypeInfoList)
{
  SIZE_T i;
  HRESULT hRes;

  for (i=0; i<aTypeInfoList.GetCount(); i++)
  {
    switch (aTypeInfoList[i]->nKind)
    {
      //case TKIND_ENUM: //enums has no dependencies
      case TKIND_RECORD:
      case TKIND_UNION:
      case TKIND_ALIAS:
        hRes = FindTypedefDependencies(lpTypeLib, aTypeInfoList[i]);
        EXIT_ON_ERROR(hRes);
        break;
      case TKIND_INTERFACE:
        hRes = FindInterfaceDependencies(lpTypeLib, aTypeInfoList[i]);
        EXIT_ON_ERROR(hRes);
        break;
    }
  }
  return S_OK;
}

static HRESULT FindTypedefDependencies(__in ITypeLib *lpTypeLib, __in TYPEINFO_ITEM *lpTypeInfoItem)
{
  CAutoTypeAttr cTypeAttr;
  CAutoVarDesc cVarDesc;
  CNktStringW cStrTempW;
  WORD i;
  HRESULT hRes;

  hRes = cTypeAttr.Set(lpTypeInfoItem->cTypeInfo);
  EXIT_ON_ERROR(hRes);
  //write file
  switch (cTypeAttr->typekind)
  {
    case TKIND_ALIAS:
      //typedef => add the base of the typedef to the dependency list
      cStrTempW.Empty();
      hRes = TypeDescToString(cStrTempW, &(cTypeAttr->tdescAlias), lpTypeInfoItem->cTypeInfo, TRUE);
      EXIT_ON_ERROR(hRes);
      RemoveArrayAndPunctuatorChars(cStrTempW);
      hRes = AddTypeInfoItemDependency(lpTypeInfoItem, cStrTempW);
      EXIT_ON_ERROR(hRes);
      break;

    case TKIND_RECORD:
    case TKIND_UNION:
      //struct or union => add custom-type fields to the dependency list
      for (i=0; i<cTypeAttr->cVars; i++)
      {
        hRes = cVarDesc.Set(lpTypeInfoItem->cTypeInfo, i);
        EXIT_ON_ERROR(hRes);
        if (cVarDesc->varkind != VAR_CONST)
        {
          cStrTempW.Empty();
          if ((cVarDesc->elemdescVar.tdesc.vt & 0x0FFF) == VT_CARRAY)
          {
            hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc.lpadesc->tdescElem),
                                    lpTypeInfoItem->cTypeInfo, TRUE);
          }
          else
          {
            hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc), lpTypeInfoItem->cTypeInfo,
                                    TRUE);
          }
          EXIT_ON_ERROR(hRes);
          RemoveArrayAndPunctuatorChars(cStrTempW);
          hRes = AddTypeInfoItemDependency(lpTypeInfoItem, cStrTempW);
          EXIT_ON_ERROR(hRes);
        }
      }
      break;
  }
  return S_OK;
}

static HRESULT FindInterfaceDependencies(__in ITypeLib *lpTypeLib, __in TYPEINFO_ITEM *lpTypeInfoItem)
{
  ITYPEINFO_LIST cTypeInfoList;
  CAutoTypeAttr cTypeAttr;
  CAutoVarDesc cVarDesc;
  CAutoFuncDesc cFuncDesc;
  CNktStringW cStrTempW;
  CNktComBStr bstrName;
  WORD i, j;
  HRESULT hRes;
  SIZE_T k1;

  //build parents
  hRes = BuildInterfaceBaseList(cTypeInfoList, lpTypeInfoItem->cTypeInfo);
  EXIT_ON_ERROR(hRes);
  for (k1=0; k1<cTypeInfoList.GetCount()-1; k1++)
  {
    bstrName.Reset();
    hRes = cTypeInfoList[k1]->GetDocumentation(MEMBERID_NIL, &bstrName, 0, 0, 0);
    EXIT_ON_ERROR(hRes);
    if (bstrName != NULL && bstrName[0] != 0)
    {
      hRes = AddTypeInfoItemDependency(lpTypeInfoItem, bstrName);
      EXIT_ON_ERROR(hRes);
    }
  }
  for (k1=0; k1<cTypeInfoList.GetCount(); k1++)
  {
    hRes = cTypeAttr.Set(cTypeInfoList[k1]);
    EXIT_ON_ERROR(hRes);
    for (i=0; i<cTypeAttr->cFuncs; i++)
    {
      hRes = cFuncDesc.Set(cTypeInfoList[k1], i);
      EXIT_ON_ERROR(hRes);
      cStrTempW.Empty();
      hRes = TypeDescToString(cStrTempW, &(cFuncDesc->elemdescFunc.tdesc), cTypeInfoList[k1], TRUE);
      EXIT_ON_ERROR(hRes);
      RemoveArrayAndPunctuatorChars(cStrTempW);
      hRes = AddTypeInfoItemDependency(lpTypeInfoItem, cStrTempW);
      EXIT_ON_ERROR(hRes);
      for (j=0; j<(UINT)(cFuncDesc->cParams); j++)
      {
        cStrTempW.Empty();
        if ((cFuncDesc->lprgelemdescParam[j].tdesc.vt & 0x0FFF) == VT_CARRAY)
        {
          hRes = TypeDescToString(cStrTempW, &(cFuncDesc->lprgelemdescParam[j].tdesc.lpadesc->tdescElem),
                                  cTypeInfoList[k1], TRUE);
        }
        else
        {
          hRes = TypeDescToString(cStrTempW, &(cFuncDesc->lprgelemdescParam[j].tdesc), cTypeInfoList[k1],
                                  TRUE);
        }
        EXIT_ON_ERROR(hRes);
        RemoveArrayAndPunctuatorChars(cStrTempW);
        hRes = AddTypeInfoItemDependency(lpTypeInfoItem, cStrTempW);
        EXIT_ON_ERROR(hRes);
      }
    }
    for (i=0; i<cTypeAttr->cVars; i++)
    {
      hRes = cVarDesc.Set(cTypeInfoList[k1], i);
      EXIT_ON_ERROR(hRes);
      if (cVarDesc->varkind != VAR_CONST)
      {
        cStrTempW.Empty();
        if ((cVarDesc->elemdescVar.tdesc.vt & 0x0FFF) == VT_CARRAY)
        {
          hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc.lpadesc->tdescElem),
                                  cTypeInfoList[k1], TRUE);
        }
        else
        {
          hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc), cTypeInfoList[k1], TRUE);
        }
        EXIT_ON_ERROR(hRes);
        RemoveArrayAndPunctuatorChars(cStrTempW);
        hRes = AddTypeInfoItemDependency(lpTypeInfoItem, cStrTempW);
        EXIT_ON_ERROR(hRes);
      }
    }
  }
  return S_OK;
}

static HRESULT AddTypeInfoItemDependency(__in TYPEINFO_ITEM *lpTypeInfoItem, __in_z LPWSTR szW)
{
  CNktStringW cStrTempW;
  LPWSTR szStartW;

  while (*szW != 0)
  {
    while (szW[0] == L' ')
      szW++;
    szStartW = szW;
    while (szW[0] != 0 && szW[0] != L' ')
      szW++;
    if (szW > szStartW)
    {
      if (cStrTempW.CopyN(szStartW, (SIZE_T)(szW-szStartW)) == FALSE)
        return E_OUTOFMEMORY;
      if (lpTypeInfoItem->sDependencyList.AddElement(cStrTempW) == FALSE)
        return E_OUTOFMEMORY;
      cStrTempW.Detach();
    }
  }
  return S_OK;
}

static VOID RemoveArrayAndPunctuatorChars(__inout CNktStringW &cStrW)
{
  LPWSTR sW;

  sW = cStrW;
  while (*sW != 0)
  {
    if (sW[0] == L'[') {
      while (sW[0] != 0 && sW[0] != L']')
      {
        sW[0] = L' ';
        sW++;
      }
      if (sW[0] == L']')
      {
        sW[0] = L' ';
        sW++;
      }
    }
    else if ((sW[0] < L'0' || sW[0] > L'9') &&
      (sW[0] < L'A' || sW[0] > L'Z') &&
      (sW[0] < L'a' || sW[0] > L'z') &&
      sW[0] != L'_')
    {
      sW[0] = L' ';
      sW++;
    }
    else
    {
      sW++;
    }
  }
  return;
}

//------------------------------------------------------------------------------------

static VOID SortTypeList(__in ITypeLib *lpTypeLib, __inout TYPEINFO_LIST &aTypeInfoList)
{
  qsort_s(aTypeInfoList.GetBuffer(), aTypeInfoList.GetCount(), sizeof(TYPEINFO_ITEM*),
          &SortTypeList_Compare, NULL);
  return;
}

static int __cdecl SortTypeList_Compare(void *, const void *lpElem1, const void *lpElem2)
{
  TYPEINFO_ITEM* lpTi1 = *((TYPEINFO_ITEM**)lpElem1);
  TYPEINFO_ITEM* lpTi2 = *((TYPEINFO_ITEM**)lpElem2);
  SIZE_T i, nCount;

  //move enums to the beginning
  if (lpTi1->nKind == TKIND_ENUM && lpTi2->nKind != TKIND_ENUM)
    return -1;
  if (lpTi1->nKind != TKIND_ENUM && lpTi2->nKind == TKIND_ENUM)
    return 1;
  //move interfaces to the end
  if (lpTi1->nKind == TKIND_INTERFACE && lpTi2->nKind != TKIND_INTERFACE)
    return 1;
  if (lpTi1->nKind != TKIND_INTERFACE && lpTi2->nKind == TKIND_INTERFACE)
    return -1;
  //else check dependencies
  //check if lpTi1 depends on lpTi2
  nCount = lpTi1->sDependencyList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (wcscmp(lpTi1->sDependencyList[i], lpTi2->szNameW) == 0)
      return -1; //lpTi1 depends on lpTi2
  }
  //check if lpTi2 depends on lpTi1
  nCount = lpTi2->sDependencyList.GetCount();
  for (i=0; i<nCount; i++)
  {
    if (wcscmp(lpTi2->sDependencyList[i], lpTi1->szNameW) == 0)
      return 1; //lpTi2 depends on lpTi1
  }
  //else don't change order
  return 0;
}

//------------------------------------------------------------------------------------

static HRESULT ProcessHeader(__in FILE *fp, __in ITypeLib *lpTypeLib)
{
  TLIBATTR *lpLibAttr;
  CHAR szBufA[256];
  CNktComBStr bstrName, bstrDoc;
  LPWSTR sW;
  GUID sLibGuid;
  HRESULT hRes;

  WRITEANSI_AND_CHECK(fp, "// Generated .H file using Deviare's TLB-to-Header application\r\n");
  WRITEANSI_AND_CHECK(fp, "//------------------------------------------------------------\r\n");
  hRes = lpTypeLib->GetLibAttr(&lpLibAttr);
  EXIT_ON_ERROR(hRes);
  memcpy(&sLibGuid, &(lpLibAttr->guid), sizeof(sLibGuid));
  lpTypeLib->ReleaseTLibAttr(lpLibAttr);
  //----
  hRes = lpTypeLib->GetDocumentation(MEMBERID_NIL, &bstrName, &bstrDoc, 0, 0);
  EXIT_ON_ERROR(hRes);
  //----
  sW = (LPWSTR)(BSTR)bstrDoc;
  if (sW == NULL || sW[0] == 0)
    sW = (LPWSTR)(BSTR)bstrName;
  if (sW != NULL && sW[0] != 0)
  {
    WRITEANSI_AND_CHECK(fp, "// Name: ");
    WRITEWIDE_AND_CHECK(fp, sW);
    WRITEANSI_AND_CHECK(fp, "\r\n");
  }
  WRITEANSI_AND_CHECK(fp, "// Guid: ");
  _snprintf_s(szBufA, X_ARRAYLEN(szBufA), _TRUNCATE, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
              sLibGuid.Data1, sLibGuid.Data2, sLibGuid.Data3,
              sLibGuid.Data4[0], sLibGuid.Data4[1], sLibGuid.Data4[2], sLibGuid.Data4[3],
              sLibGuid.Data4[4], sLibGuid.Data4[5], sLibGuid.Data4[6], sLibGuid.Data4[7]);
  WRITEANSI_AND_CHECK(fp, szBufA);
  WRITEANSI_AND_CHECK(fp, "\r\n");
  //done
  return S_OK;
}

static HRESULT ProcessTypedef(__in FILE *fp, __in TYPEINFO_ITEM *lpTypeInfoItem)
{
  CAutoTypeAttr cTypeAttr;
  CNktStringW cStrTempW;
  WORD i;
  HRESULT hRes;

  hRes = cTypeAttr.Set(lpTypeInfoItem->cTypeInfo);
  EXIT_ON_ERROR(hRes);
  //write file
  WRITEANSI_AND_CHECK(fp, "\r\n");
  WRITEANSI_AND_CHECK(fp, "typedef ");
  switch (cTypeAttr->typekind)
  {
    case TKIND_RECORD:
      WRITEANSI_AND_CHECK(fp, "struct {\r\n");
      break;
    case TKIND_UNION:
      WRITEANSI_AND_CHECK(fp, "union {\r\n");
      break;
    case TKIND_ALIAS: //typedef
      cStrTempW.Empty();
      hRes = TypeDescToString(cStrTempW, &(cTypeAttr->tdescAlias), lpTypeInfoItem->cTypeInfo, FALSE);
      EXIT_ON_ERROR(hRes);
      WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
      WRITEANSI_AND_CHECK(fp, " ");
      break;
    case TKIND_ENUM:
      WRITEANSI_AND_CHECK(fp, "enum {\r\n");
      break;
  }
  switch (cTypeAttr->typekind)
  {
    case TKIND_RECORD:
    case TKIND_UNION:
      for (i=0; i<cTypeAttr->cVars; i++)
      {
        hRes = ProcessVariable(fp, i, cTypeAttr.Get(), lpTypeInfoItem->cTypeInfo, 1);
        EXIT_ON_ERROR(hRes);
      }
      WRITEANSI_AND_CHECK(fp, "} ");
      break;

    case TKIND_ENUM:
      for (i=0; i<cTypeAttr->cVars; i++)
      {
        hRes = ProcessConst(fp, i, cTypeAttr.Get(), lpTypeInfoItem->cTypeInfo, 1, FALSE);
        EXIT_ON_ERROR(hRes);
        if (i < cTypeAttr->cVars-1)
        {
          WRITEANSI_AND_CHECK(fp, ",");
        }
        WRITEANSI_AND_CHECK(fp, "\r\n");
      }
      WRITEANSI_AND_CHECK(fp, "} ");
      break;
  }
  WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
  WRITEANSI_AND_CHECK(fp, ";\r\n");
  return S_OK;
}

static HRESULT ProcessInterface(__in FILE *fp, __in TYPEINFO_ITEM *lpTypeInfoItem, __in SIZE_T nPass)
{
  ITYPEINFO_LIST cTypeInfoList;
  CAutoTypeAttr cTypeAttr, cTypeAttr2;
  CNktStringW cStrTempW;
  WORD i;
  HRESULT hRes;
  SIZE_T k1;
  BOOL b;

  if (nPass == 1)
  {
    WRITEANSI_AND_CHECK(fp, "\r\n");
    WRITEANSI_AND_CHECK(fp, "typedef struct ");
    WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
    WRITEANSI_AND_CHECK(fp, " ");
    WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
    WRITEANSI_AND_CHECK(fp, ";\r\n");
    return S_OK;
  }
  //second pass
  hRes = cTypeAttr.Set(lpTypeInfoItem->cTypeInfo);
  EXIT_ON_ERROR(hRes);
  //typedef struct IUnknown IUnknown;
  WRITEANSI_AND_CHECK(fp, "\r\n");
  WRITEANSI_AND_CHECK(fp, "typedef struct ");
  WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
  WRITEANSI_AND_CHECK(fp, "Vtbl {\r\n");
  //build parents
  hRes = BuildInterfaceBaseList(cTypeInfoList, lpTypeInfoItem->cTypeInfo);
  EXIT_ON_ERROR(hRes);
  for (k1=0; k1<cTypeInfoList.GetCount(); k1++)
  {
    hRes = cTypeAttr2.Set(cTypeInfoList[k1]);
    EXIT_ON_ERROR(hRes);
    for (i=0; i<cTypeAttr2->cFuncs; i++)
    {
      hRes = ProcessFunction(fp, i, cTypeAttr2.Get(), cTypeInfoList[k1], 1, lpTypeInfoItem->szNameW);
      EXIT_ON_ERROR(hRes);
      b = TRUE;
    }
  }
  for (k1=0; k1<cTypeInfoList.GetCount(); k1++)
  {
    hRes = cTypeAttr2.Set(cTypeInfoList[k1]);
    EXIT_ON_ERROR(hRes);
    for (i=0; i<cTypeAttr2->cVars; i++)
    {
      if (b != FALSE)
      {
        WRITEANSI_AND_CHECK(fp, "\r\n");
        b = FALSE;
      }
      hRes = ProcessVariable(fp, i, cTypeAttr2.Get(), cTypeInfoList[k1], 1);
      EXIT_ON_ERROR(hRes);
    }
  }
  WRITEANSI_AND_CHECK(fp, "};\r\n\r\n");
  //----
  WRITEANSI_AND_CHECK(fp, "struct ");
  WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
  WRITEANSI_AND_CHECK(fp, " {\r\n");
  WRITEINDENT_AND_CHECK(fp, 1);
  WRITEANSI_AND_CHECK(fp, "struct ");
  WRITEWIDE_AND_CHECK(fp, (LPWSTR)(lpTypeInfoItem->szNameW));
  WRITEANSI_AND_CHECK(fp, "Vtbl *lpVtbl;\r\n");
  WRITEANSI_AND_CHECK(fp, "};\r\n");
  return S_OK;
}

static HRESULT ProcessVariable(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                               __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent)
{
  CAutoVarDesc cVarDesc;
  CNktStringW cStrTempW;
  CNktComBStr bstrMemberName;
  BSTR bstr;
  HRESULT hRes;
  UINT nNamesCount;
  WORD i;
  char szBufA[32];

  hRes = cVarDesc.Set(lpTypeInfo, nIndex);
  EXIT_ON_ERROR(hRes);
  if (cVarDesc->varkind == VAR_CONST)
  {
    _tprintf_s(_T("Error: Variables should not be constants.\n"));
    return E_NOTIMPL;
  }
  hRes = lpTypeInfo->GetNames(cVarDesc->memid, &bstr, 1, &nNamesCount);
  EXIT_ON_ERROR(hRes);
  if (nNamesCount == 0 || bstr == NULL)
  {
    _tprintf_s(_T("Error: Unnamed variables are not supported.\n"));
    return E_NOTIMPL;
  }
  bstrMemberName.Attach(bstr);
  WRITEINDENT_AND_CHECK(fp, nIndent);
  if ((cVarDesc->elemdescVar.tdesc.vt & 0x0FFF) == VT_CARRAY)
  {
    // type name[n]
    cStrTempW.Empty();
    hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc.lpadesc->tdescElem), lpTypeInfo,
                            FALSE);
    EXIT_ON_ERROR(hRes);
    WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
    WRITEANSI_AND_CHECK(fp, " ");
    WRITEBSTR_AND_CHECK(fp, bstrMemberName);
    for (i=0; i<cVarDesc->elemdescVar.tdesc.lpadesc->cDims; i++)
    {
      _snprintf_s(szBufA, X_ARRAYLEN(szBufA), _TRUNCATE, "[%d]",
                  cVarDesc->elemdescVar.tdesc.lpadesc->rgbounds[i].cElements);
      WRITEANSI_AND_CHECK(fp, szBufA);
    }
  }
  else
  {
    cStrTempW.Empty();
    hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc), lpTypeInfo, FALSE);
    EXIT_ON_ERROR(hRes);
    WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
    WRITEANSI_AND_CHECK(fp, " ");
    WRITEBSTR_AND_CHECK(fp, bstrMemberName);
  }
  WRITEANSI_AND_CHECK(fp, ";\r\n");
  return S_OK;
}

static HRESULT ProcessConst(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                            __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent, __in BOOL bShowConst)
{
  CAutoVarDesc cVarDesc;
  CNktStringW cStrTempW;
  CNktComBStr bstrMemberName;
  BSTR bstr;
  HRESULT hRes;
  UINT nNamesCount;
  CNktComVariant vtValue;

  hRes = cVarDesc.Set(lpTypeInfo, nIndex);
  EXIT_ON_ERROR(hRes);
  if (cVarDesc->varkind != VAR_CONST)
  {
    _tprintf_s(_T("Error: Constant not declared as constant.\n"));
    return E_NOTIMPL;
  }
  cStrTempW.Empty();
  hRes = TypeDescToString(cStrTempW, &(cVarDesc->elemdescVar.tdesc), lpTypeInfo, FALSE);
  EXIT_ON_ERROR(hRes);
  hRes = ::VariantChangeType(&vtValue, cVarDesc->lpvarValue, 0, VT_BSTR);
  if (FAILED(hRes))
  {
    WCHAR szTempBufW[32], *sW;

    if (cVarDesc->lpvarValue->vt != VT_ERROR && cVarDesc->lpvarValue->vt != VT_HRESULT)
      return hRes;
    sW = miscGetScodeString(cVarDesc->lpvarValue->scode, szTempBufW);
    vtValue.sVt.vt = VT_BSTR;
    vtValue.sVt.bstrVal = ::SysAllocString(sW);
    if (vtValue.sVt.bstrVal == NULL)
      return E_OUTOFMEMORY;
  }
  hRes = lpTypeInfo->GetNames(cVarDesc->memid, &bstr, 1, &nNamesCount);
  EXIT_ON_ERROR(hRes);
  if (nNamesCount == 0 || bstr == NULL)
  {
    _tprintf_s(_T("Error: Unnamed variables are not supported.\n"));
    return E_NOTIMPL;
  }
  bstrMemberName.Attach(bstr);
  WRITEINDENT_AND_CHECK(fp, nIndent);
  if (bShowConst != FALSE)
  {
    WRITEANSI_AND_CHECK(fp, "const ");
    WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
    WRITEANSI_AND_CHECK(fp, " ");
  }
  WRITEBSTR_AND_CHECK(fp, bstrMemberName);
  WRITEANSI_AND_CHECK(fp, " = ");
  if (cVarDesc->lpvarValue->vt == VT_BSTR)
  {
    WRITEANSI_AND_CHECK(fp, "\"");
    WRITEBSTR_AND_CHECK(fp, vtValue.sVt.bstrVal);
    WRITEANSI_AND_CHECK(fp, "\"");
  }
  else
  {
    WRITEBSTR_AND_CHECK(fp, vtValue.sVt.bstrVal);
  }
  if (bShowConst != FALSE)
  {
    WRITEANSI_AND_CHECK(fp, ";\r\n");
  }
  return S_OK;
}

static HRESULT ProcessFunction(__in FILE *fp, __in WORD nIndex, __in TYPEATTR* lpTypeAttr,
                               __in ITypeInfo *lpTypeInfo, __in SIZE_T nIndent, __in LPWSTR szParentThisW)
{
  CAutoFuncDesc cFuncDesc;
  CNktStringW cStrTempW;
  CNktComBStr bstrNames[MAX_FUNCTION_NAMES];
  BSTR bstr[MAX_FUNCTION_NAMES];
  HRESULT hRes;
  CNktComVariant vtValue;
  UINT i, j, nNamesCount;
  char szBufA[32];

  hRes = cFuncDesc.Set(lpTypeInfo, nIndex);
  EXIT_ON_ERROR(hRes);
  //Write return type
  cStrTempW.Empty();
  hRes = TypeDescToString(cStrTempW, &(cFuncDesc->elemdescFunc.tdesc), lpTypeInfo, FALSE);
  EXIT_ON_ERROR(hRes);
  WRITEINDENT_AND_CHECK(fp, nIndent);
  WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
  WRITEANSI_AND_CHECK(fp, " (");
  if (lpTypeAttr->typekind != TKIND_DISPATCH)
  {
    // Write calling convention
    switch (cFuncDesc->callconv)
    {
      case CC_CDECL:
        WRITEANSI_AND_CHECK(fp, "_cdecl ");
        break;
      case CC_PASCAL:
        WRITEANSI_AND_CHECK(fp, "__pascal ");
        break;
      case CC_STDCALL:
        WRITEANSI_AND_CHECK(fp, "__stdcall ");
        break;
      default:
        _tprintf_s(_T("Error: Unsupported calling convetion '%04X'.\n"), cFuncDesc->callconv);
        return E_NOTIMPL;
    }
  }
  else
  {
    WRITEANSI_AND_CHECK(fp, "__stdcall ");
  }
  WRITEANSI_AND_CHECK(fp, "*");
  // Write methodname
  //
  // Problem:  If a property has the propput or propputref attributes the
  // 'right hand side' (rhs) is *always* the last parameter and MkTypeLib
  // strips the parameter name.  Thus you will always get 1 less name
  // back from ::GetNames than normal.
  //
  // Thus for the example below
  //  [propput] void Color([in] VARIANT rgb, [in] VARIANT rgb2 );
  // without taking this into consderation the output would be
  //  [propput] void Color([in] VARIANT rgb, [in] VARIANT );
  // when it should be
  //  [propput] void Color([in] VARIANT rgb, [in] VARIANT rhs );
  //
  // Another weirdness comes from a bug (which will never be fixed)
  // where optional parameters on property functions were allowed.
  // Because they were allowed by accident people used them, so they
  // are still allowed.
  nNamesCount = 0;
  hRes = lpTypeInfo->GetNames(cFuncDesc->memid, bstr, MAX_FUNCTION_NAMES, &nNamesCount);
  EXIT_ON_ERROR(hRes);
  for (i=0; i<nNamesCount; i++)
    bstrNames[i].Attach(bstr[i]);
  // fix for 'rhs' problem
  if (nNamesCount < (UINT)cFuncDesc->cParams+1)
  {
    hRes = bstrNames[nNamesCount++].Set(L"rhs");
    EXIT_ON_ERROR(hRes);
  }
  NKT_ASSERT(nNamesCount == cFuncDesc->cParams+1);
  if ((cFuncDesc->invkind & INVOKE_PROPERTYGET) != 0)
  {
    WRITEANSI_AND_CHECK(fp, "get_");
  }
  else if ((cFuncDesc->invkind & INVOKE_PROPERTYPUT) != 0)
  {
    WRITEANSI_AND_CHECK(fp, "put_");
  }
  else if ((cFuncDesc->invkind & INVOKE_PROPERTYPUTREF) != 0)
  {
    WRITEANSI_AND_CHECK(fp, "putref_");
  }
  WRITEBSTR_AND_CHECK(fp, bstrNames[0]);
  WRITEANSI_AND_CHECK(fp, ")(");
  if (szParentThisW != NULL)
  {
    WRITEWIDE_AND_CHECK(fp, szParentThisW);
    WRITEANSI_AND_CHECK(fp, "* This");
  }
  // params have the format
  //   [attributes] type paramname
  // where attributes can be
  //   in, out, optional, string   (string is not valid for TKIND_MODULE)
  for (i=0; i<(UINT)(cFuncDesc->cParams); i++)
  {
    if (i > 0 || szParentThisW != NULL)
    {
      WRITEANSI_AND_CHECK(fp, ", ");
    }
    // type
    if ((cFuncDesc->lprgelemdescParam[i].tdesc.vt & 0x0FFF) == VT_CARRAY)
    {
      // type name[n]
      cStrTempW.Empty();
      hRes = TypeDescToString(cStrTempW, &(cFuncDesc->lprgelemdescParam[i].tdesc.lpadesc->tdescElem),
                              lpTypeInfo, FALSE);
      EXIT_ON_ERROR(hRes);
      WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
      WRITEANSI_AND_CHECK(fp, " ");
      WRITEBSTR_AND_CHECK(fp, bstrNames[i+1]);
      for (j=0; j<cFuncDesc->lprgelemdescParam[i].tdesc.lpadesc->cDims; j++)
      {
        _snprintf_s(szBufA, X_ARRAYLEN(szBufA), _TRUNCATE, "[%d]",
                    cFuncDesc->lprgelemdescParam[i].tdesc.lpadesc->rgbounds[j].cElements);
        WRITEANSI_AND_CHECK(fp, szBufA);
      }
    }
    else
    {
      cStrTempW.Empty();
      hRes = TypeDescToString(cStrTempW, &(cFuncDesc->lprgelemdescParam[i].tdesc), lpTypeInfo, FALSE);
      EXIT_ON_ERROR(hRes);
      WRITEWIDE_AND_CHECK(fp, (LPWSTR)cStrTempW);
      WRITEANSI_AND_CHECK(fp, " ");
      WRITEBSTR_AND_CHECK(fp, bstrNames[i+1]);
    }
  }
  WRITEANSI_AND_CHECK(fp, ");\r\n");
  return S_OK;
}

static HRESULT CustomTypeToString(__inout CNktStringW &cStrDestW, __in HREFTYPE hRefType,
                                  __in ITypeInfo *lpTypeInfo)
{
  TNktComPtr<ITypeInfo> cCustTypeInfo;
  HRESULT hRes;
  CNktComBStr bstrType;

  hRes = lpTypeInfo->GetRefTypeInfo(hRefType, &cCustTypeInfo);
  if (SUCCEEDED(hRes))
    hRes = cCustTypeInfo->GetDocumentation(-1, &bstrType, 0, 0, 0);
  if (FAILED(hRes))
    return hRes;
  if (cStrDestW.Concat(bstrType) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

static HRESULT TypeDescToString(__inout CNktStringW &cStrDestW, __in TYPEDESC *lpTypeDesc,
                                __in ITypeInfo *lpTypeInfo, __in BOOL bDependencyCheck)
{
  static const LPWSTR szVT_W[] = {
    L"void",             //VT_EMPTY           = 0,   // [V]   [P]  nothing
    L"null",             //VT_NULL            = 1,   // [V]        SQL style Null
    L"short",            //VT_I2              = 2,   // [V][T][P]  2 byte signed int
    L"long",             //VT_I4              = 3,   // [V][T][P]  4 byte signed int
    L"single",           //VT_R4              = 4,   // [V][T][P]  4 byte real
    L"double",           //VT_R8              = 5,   // [V][T][P]  8 byte real
    L"CURRENCY",         //VT_CY              = 6,   // [V][T][P]  currency
    L"DATE",             //VT_DATE            = 7,   // [V][T][P]  date
    L"BSTR",             //VT_BSTR            = 8,   // [V][T][P]  binary string
    L"IDispatch*",       //VT_DISPATCH        = 9,   // [V][T]     IDispatch FAR*
    L"SCODE",            //VT_ERROR           = 10,  // [V][T]     SCODE
    L"boolean",          //VT_BOOL            = 11,  // [V][T][P]  True=-1, False=0
    L"VARIANT",          //VT_VARIANT         = 12,  // [V][T][P]  VARIANT FAR*
    L"IUnknown*",        //VT_UNKNOWN         = 13,  // [V][T]     IUnknown FAR*
    L"wchar_t",          //VT_WBSTR           = 14,  // [V][T]     wide binary string
    L"",                 //                   = 15,
    L"char",             //VT_I1              = 16,  //    [T]     signed char
    L"unsigned char",    //VT_UI1             = 17,  //    [T]     unsigned char
    L"unsigned short",   //VT_UI2             = 18,  //    [T]     unsigned short
    L"unsigned long",    //VT_UI4             = 19,  //    [T]     unsigned short
    L"int64",            //VT_I8              = 20,  //    [T][P]  signed 64-bit int
    L"uint64",           //VT_UI8             = 21,  //    [T]     unsigned 64-bit int
    L"int",              //VT_INT             = 22,  //    [T]     signed machine int
    L"unsigned int",     //VT_UINT            = 23,  //    [T]     unsigned machine int
    L"void",             //VT_VOID            = 24,  //    [T]     C style void
    L"HRESULT",          //VT_HRESULT         = 25,  //    [T]
    L"PTR",              //VT_PTR             = 26,  //    [T]     pointer type
    L"SAFEARRAY",        //VT_SAFEARRAY       = 27,  //    [T]     (use VT_ARRAY in VARIANT)
    L"CARRAY",           //VT_CARRAY          = 28,  //    [T]     C style array
    L"USERDEFINED",      //VT_USERDEFINED     = 29,  //    [T]     user defined type
    L"LPSTR",            //VT_LPSTR           = 30,  //    [T][P]  null terminated string
    L"LPWSTR",           //VT_LPWSTR          = 31,  //    [T][P]  wide null terminated string
    L"",                 //                   = 32,
    L"",                 //                   = 33,
    L"",                 //                   = 34,
    L"",                 //                   = 35,
    L"",                 //                   = 36,
    L"",                 //                   = 37,
    L"",                 //                   = 38,
    L"",                 //                   = 39,
    L"",                 //                   = 40,
    L"",                 //                   = 41,
    L"",                 //                   = 42,
    L"",                 //                   = 43,
    L"",                 //                   = 44,
    L"",                 //                   = 45,
    L"",                 //                   = 46,
    L"",                 //                   = 47,
    L"",                 //                   = 48,
    L"",                 //                   = 49,
    L"",                 //                   = 50,
    L"",                 //                   = 51,
    L"",                 //                   = 52,
    L"",                 //                   = 53,
    L"",                 //                   = 54,
    L"",                 //                   = 55,
    L"",                 //                   = 56,
    L"",                 //                   = 57,
    L"",                 //                   = 58,
    L"",                 //                   = 59,
    L"",                 //                   = 60,
    L"",                 //                   = 61,
    L"",                 //                   = 62,
    L"",                 //                   = 63,
    L"FILETIME",         //VT_FILETIME        = 64,  //       [P]  FILETIME
    L"BLOB",             //VT_BLOB            = 65,  //       [P]  Length prefixed bytes
    L"STREAM",           //VT_STREAM          = 66,  //       [P]  Name of the stream follows
    L"STORAGE",          //VT_STORAGE         = 67,  //       [P]  Name of the storage follows
    L"STREAMED_OBJECT",  //VT_STREAMED_OBJECT = 68,  //       [P]  Stream contains an object
    L"STORED_OBJECT",    //VT_STORED_OBJECT   = 69,  //       [P]  Storage contains an object
    L"BLOB_OBJECT",      //VT_BLOB_OBJECT     = 70,  //       [P]  Blob contains an object
    L"CF",               //VT_CF              = 71,  //       [P]  Clipboard format
    L"CLSID",            //VT_CLSID           = 72   //       [P]  A Class ID
  };
  HRESULT hRes;
  CNktStringW cStrTempW;

  if (lpTypeDesc->vt == VT_PTR)
  {
    hRes = TypeDescToString(cStrTempW, lpTypeDesc->lptdesc, lpTypeInfo, bDependencyCheck);
    if (FAILED(hRes))
      return hRes;
    if (cStrDestW.Concat(cStrTempW) == FALSE)
      return E_OUTOFMEMORY;
    if (bDependencyCheck == FALSE)
    {
      //don't add unneeded stuff if dependecy check
      if (cStrDestW.Concat(L"*") == FALSE)
        return E_OUTOFMEMORY;
    }
    else
    {
      //add a separator
      if (cStrDestW.Concat(L" ") == FALSE)
        return E_OUTOFMEMORY;
    }
    return S_OK;
  }
  if ((lpTypeDesc->vt & 0x0FFF) == VT_SAFEARRAY)
  {
    if (bDependencyCheck == FALSE)
    {
      //don't add unneeded stuff if dependecy check
      if (cStrDestW.Concat(L"SAFEARRAY*") == FALSE)
        return E_OUTOFMEMORY;
    }
    return S_OK;
  }
  if ((lpTypeDesc->vt & 0x0FFF) == VT_CARRAY)
  {
    USHORT i;

    hRes = TypeDescToString(cStrTempW, &(lpTypeDesc->lpadesc->tdescElem), lpTypeInfo, bDependencyCheck);
    if (FAILED(hRes))
      return hRes;
    if (cStrDestW.Concat(cStrTempW) == FALSE)
      return E_OUTOFMEMORY;
    if (bDependencyCheck == FALSE)
    {
      //don't add unneeded stuff if dependecy check
      for (i=0; i<lpTypeDesc->lpadesc->cDims; i++)
      {
        if (cStrDestW.Concat(L"[") == FALSE ||
            cStrDestW.Concat(lpTypeDesc->lpadesc->rgbounds[i].cElements) == FALSE ||
            cStrDestW.Concat(L"]") == FALSE)
          return E_OUTOFMEMORY;
      }
    }
    else
    {
      //add a separator
      if (cStrDestW.Concat(L" ") == FALSE)
        return E_OUTOFMEMORY;
    }
    return S_OK;
  }
  if (lpTypeDesc->vt == VT_USERDEFINED)
  {
    return CustomTypeToString(cStrDestW, lpTypeDesc->hreftype, lpTypeInfo);
  }
  if ((lpTypeDesc->vt & (~0xF000)) > VT_CLSID)
  {
    _tprintf_s(_T("Error: Unsupported type '%04X' found.\n"), lpTypeDesc->vt);
    return E_NOTIMPL;
  }
  if (bDependencyCheck == FALSE)
  {
    //don't add unneeded stuff if dependecy check
    if (cStrDestW.Concat(szVT_W[lpTypeDesc->vt & (~0xF000)]) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

static __inline BYTE Char2Hex(__in TCHAR ch)
{
  if (ch >= _T('0') && ch <= _T('9'))
    return (BYTE)ch - 48;
  if (ch >= _T('A') && ch <= _T('F'))
    return (BYTE)ch - 55;
  if (ch >= _T('a') && ch <= _T('f'))
    return (BYTE)ch - 87;
  return 255;
}

static BOOL GuidFromSz(__in LPCTSTR s, __out GUID *lpGuid)
{
  SIZE_T i;
  BYTE val, val2;
  LPCTSTR s2;

  if (s == NULL || s[0] != _T('{'))
    return FALSE;
  memset(lpGuid, 0, sizeof(GUID));
  //parse {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
  for (i=1; i<9; i++)
  {
    val = Char2Hex(s[i]);
    if (val == 255)
      return FALSE;
    lpGuid->Data1 = (lpGuid->Data1 << 4) | val;
  }
  if (s[9] != _T('-'))
    return FALSE;
  for (i=10; i<14; i++)
  {
    val = Char2Hex(s[i]);
    if (val == 255)
      return FALSE;
    lpGuid->Data2 = (lpGuid->Data2 << 4) | val;
  }
  if (s[14] != _T('-'))
    return FALSE;
  for (i=15; i<19; i++)
  {
    val = Char2Hex(s[i]);
    if (val == 255)
      return FALSE;
    lpGuid->Data3 = (lpGuid->Data3 << 4) | val;
  }
  if (s[19] != _T('-'))
    return FALSE;
  s2 = s+20;
  for (i=0; i<8; i++)
  {
    if (i == 2)
    {
      if (*s2 != _T('-'))
        return FALSE;
      s2++;
    }
    val = Char2Hex(s2[0]);
    if (val == 255)
      return FALSE;
    val2 = Char2Hex(s2[1]);
    if (val2 == 255)
      return FALSE;
    lpGuid->Data4[i] = (val << 4) | val2;
    s2 += 2;
  }
  if (s[37] != _T('}') && s[38] == 0)
    return FALSE;
  return TRUE;
}

static HRESULT GetTypeLibVersion(__in REFGUID rGuid, __out WORD &wVerMajor, __out WORD &wVerMinor)
{
  TCHAR szBuf[256], *sT;
  LONG lRes;
  DWORD dwIndex;
  WORD wMj, wMi;
  HKEY hTypeLibKey;

  wVerMajor = wVerMinor = 0;
  _sntprintf_s(szBuf, X_ARRAYLEN(szBuf), _TRUNCATE,
               _T("TypeLib\\{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}"),
               rGuid.Data1, rGuid.Data2, rGuid.Data3,
               rGuid.Data4[0], rGuid.Data4[1], rGuid.Data4[2], rGuid.Data4[3],
               rGuid.Data4[4], rGuid.Data4[5], rGuid.Data4[6], rGuid.Data4[7]);
  lRes = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, szBuf, 0, KEY_READ, &hTypeLibKey);
  if (lRes != ERROR_SUCCESS)
  {
    if (lRes == ERROR_NOT_FOUND ||
        lRes == ERROR_FILE_NOT_FOUND ||
        lRes == ERROR_PATH_NOT_FOUND)
      return TYPE_E_LIBNOTREGISTERED;
    return HRESULT_FROM_WIN32(lRes);
  }
  for (dwIndex=0; ::RegEnumKey(hTypeLibKey, dwIndex, szBuf, X_ARRAYLEN(szBuf))==ERROR_SUCCESS; dwIndex++)
  {
    wMj = (WORD)_ttoi(szBuf);
    sT = _tcsrchr(szBuf, _T('.'));
    wMi = (sT != NULL) ? (WORD)_ttoi(sT+1) : 0;
    if (wMj > wVerMajor ||
        (wMj == wVerMajor && wMi > wVerMinor))
    {
      wVerMajor = wMj;
      wVerMinor = wMi;
    }
  }
  ::RegCloseKey(hTypeLibKey);
  return (wVerMajor != 0) ? S_OK : TYPE_E_LIBNOTREGISTERED;
}

static BOOL WriteIndent(__in FILE *fp, __in SIZE_T nIndent)
{
  while (nIndent > 0)
  {
    if (fprintf(fp, "    ") == 0)
      return FALSE;
    nIndent--;
  }
  return TRUE;
}

static BOOL WriteBSTR(__in FILE *fp, __in BSTR bstr)
{
  if (bstr == NULL)
    return TRUE;
  return WriteString(fp, (LPCWSTR)bstr, (SIZE_T)::SysStringLen(bstr));
}

static BOOL WriteString(__in FILE *fp, __in_z LPCWSTR szStringW, __in_opt SIZE_T nLength)
{
  SIZE_T i, j, nStart;
  WCHAR szTempW[32];

  if (szStringW == NULL)
    return S_OK;
  if (nLength == NKT_SIZE_T_MAX)
    nLength = wcslen(szStringW);
  for (i=0; i<nLength; i++)
  {
    nStart = i;
    for (j=0; j<X_ARRAYLEN(szTempW)-1 && i<nLength; j++)
    {
      if (iswprint(szStringW[i]) == 0 || szStringW[i] == L'\"' || szStringW[i] == L'\\')
        break;
      szTempW[j] = szStringW[i++];
    }
    if (i > nStart)
    {
      szTempW[j] = 0;
      if (fprintf(fp, "%S", szTempW) == 0)
        return FALSE;
    }
    while (i < nLength)
    {
      if (iswprint(szStringW[i]) != 0 && szStringW[i] != L'\"' && szStringW[i] != L'\\')
        break;
      switch (szStringW[i])
      {
        case L'\"':
          if (fprintf(fp, "\\\"") == 0)
            return FALSE;
          break;
        case L'\\':
          if (fprintf(fp, "\\\\") == 0)
            return FALSE;
          break;
        case L'\a':
          if (fprintf(fp, "\\a") == 0)
            return FALSE;
          break;
        case L'\b':
          if (fprintf(fp, "\\b") == 0)
            return FALSE;
          break;
        case L'\f':
          if (fprintf(fp, "\\f") == 0)
            return FALSE;
          break;
        case L'\n':
          if (fprintf(fp, "\\n") == 0)
            return FALSE;
          break;
        case L'\r':
          if (fprintf(fp, "\\r") == 0)
            return FALSE;
          break;
        case L'\t':
          if (fprintf(fp, "\\t") == 0)
            return FALSE;
          break;
        case L'\v':
          if (fprintf(fp, "\\v") == 0)
            return FALSE;
          break;
        case L'\0':
          if (fprintf(fp, "\\0") == 0)
            return FALSE;
          break;
        default:
          if (fprintf(fp, "\\x%02X", szStringW[i]) == 0)
            return FALSE;
          break;
      }
      i++;
    }
  }
  return TRUE;
}

static BOOL WriteSeparator(__in FILE *fp)
{
  if (fprintf(fp, "//------------------------------------------------------------\r\n") == 0)
    return FALSE;
  return TRUE;
}

static HRESULT BuildInterfaceBaseList(ITYPEINFO_LIST &cTypeInfoList, __in ITypeInfo *lpTypeInfo)
{
  typedef struct {
    GUID sGuid;
    BOOL bProcessed;
  } TYPEINFOEXTRADATA;
  TNktArrayList4Structs<TYPEINFOEXTRADATA> cTypeInfoExtraData;
  TYPEINFOEXTRADATA sTempTied;
  CAutoTypeAttr cTypeAttr, cTypeAttr2;
  CNktStringW cStrTempW;
  TNktComPtr<ITypeInfo> cChildTypeInfo;
  SIZE_T k1, k2, nCount;
  WORD i, j;
  HRESULT hRes;
  HREFTYPE hRef;

  hRes = cTypeAttr.Set(lpTypeInfo);
  EXIT_ON_ERROR(hRes);
  //build parents
  sTempTied.sGuid = cTypeAttr->guid;
  sTempTied.bProcessed = FALSE;
  if (cTypeInfoExtraData.AddElement(&sTempTied) == FALSE ||
      cTypeInfoList.AddElement(lpTypeInfo) == FALSE)
    return E_OUTOFMEMORY;
  lpTypeInfo->AddRef();
  nCount = cTypeInfoList.GetCount();
  while (1)
  {
    for (k1=0; k1<nCount && cTypeInfoExtraData[k1].bProcessed!=FALSE; k1++);
    if (k1 >= nCount)
      break; //we are done building the tree
    cTypeInfoExtraData[k1].bProcessed = TRUE;
    hRes = cTypeAttr.Set(cTypeInfoList[k1]);
    EXIT_ON_ERROR(hRes);
    for (i=j=0; i<cTypeAttr->cImplTypes; i++)
    {
      hRes = cTypeInfoList[k1]->GetRefTypeOfImplType(i, &hRef);
      EXIT_ON_ERROR(hRes);
      cChildTypeInfo.Release();
      hRes = cTypeInfoList[k1]->GetRefTypeInfo(hRef, &cChildTypeInfo);
      EXIT_ON_ERROR(hRes);
      hRes = cTypeAttr2.Set(cChildTypeInfo);
      EXIT_ON_ERROR(hRes);
      if (cTypeAttr2->typekind != TKIND_INTERFACE)
        continue;
      sTempTied.sGuid = cTypeAttr2->guid;
      sTempTied.bProcessed = FALSE;
      for (k2=0; k2<nCount; k2++)
      {
        if (memcmp(&sTempTied.sGuid, &(cTypeInfoExtraData[k2].sGuid), sizeof(GUID)) == 0)
          break;
      }
      if (k2 >= nCount)
      {
        //not in the list, add
        if (cTypeInfoExtraData.InsertElementAt(&sTempTied, k1+(SIZE_T)j) == FALSE ||
            cTypeInfoList.InsertElementAt(cChildTypeInfo, k1+(SIZE_T)j) == FALSE)
          return E_OUTOFMEMORY;
        cChildTypeInfo.Detach(); //cTypeInfoList will keep the reference
        j++;
        nCount++;
      }
    }
  }
  return S_OK;
}
