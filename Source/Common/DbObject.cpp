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
#include "Database.h"
#include <crtdbg.h>
#include "WaitableObjects.h"
#include "ComPtr.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

typedef struct tagSearchDbObjectByName_Ctx {
  CNktDvDbObject **lpBuffer;
  struct tagParsedName {
    LPCWSTR szNamespaceW;
    SIZE_T nNamespaceLen;
    LPCWSTR szNameW;
    SIZE_T nNameLen;
  } sParsedName;
} sSearchDbObjectsEnumByName_Ctx;

//-----------------------------------------------------------

static LONG volatile nDeclCreationMtx = 0;

//-----------------------------------------------------------

static int __cdecl SortDbObjectsEnumByName(void *lpContext, const void *lpIdx1, const void *lpIdx2);
static int __cdecl SearchDbObjectsEnumById(void *lpContext, const void *lpKey, const void *lpObj);
static int __cdecl SearchDbObjectsEnumByName(void *lpContext, const void *lpKey, const void *lpObj);
static BOOL IsGuidName(__in LPCWSTR szNameW);

static __inline int MyStrCmpW(__in const LPCWSTR szStr1, __in SIZE_T nStr1Len, __in const LPWSTR szStr2)
{
  int nRes = wcsncmp(szStr1, szStr2, nStr1Len);
  if (nRes == 0 && szStr2[nStr1Len] != 0)
    nRes = -1; //szStr2 greater than szStr1
  return nRes;
}

//-----------------------------------------------------------

CNktDvDbObjectNoRef::CNktDvDbObjectNoRef() : CNktMemMgrObj()
{
  lpStoredData = NULL;
  szNameW = szNamespaceW = szDeclW = NULL;
  lpFirstChild = NULL;
  nChildsCount = 0;
  //----
#ifdef NKT_DV_DBGENERATOR
  sCreationHelpers.nId = sCreationHelpers.nClass = sCreationHelpers.nFundamentalType = 0;
  sCreationHelpers.nSize = sCreationHelpers.nAlign = 0;
  sCreationHelpers.nArraySizeHint = sCreationHelpers.nStructUnionFunctionFlags = 0;
  sCreationHelpers.nStructOffset = sCreationHelpers.nStructBits = 0;
  sCreationHelpers.nEnumValue = sCreationHelpers.nFlags = 0;
  sCreationHelpers.szNameW = sCreationHelpers.szNamespaceW = NULL;
  sCreationHelpers.nHashValue = 0;
#endif //NKT_DV_DBGENERATOR
  lpDvDB = NULL;
  return;
}

CNktDvDbObjectNoRef::~CNktDvDbObjectNoRef()
{
  if (szDeclW != NULL)
    nktMemFree(szDeclW);
  return;
}

ULONG CNktDvDbObjectNoRef::GetId() const
{
  return lpStoredData->nId;
}

LPWSTR CNktDvDbObjectNoRef::GetNamespace() const
{
  return (szNamespaceW != NULL) ? szNamespaceW : L"";
}

CNktDvDbObjectNoRef::eClass CNktDvDbObjectNoRef::GetClass() const
{
  return (CNktDvDbObjectNoRef::eClass)(lpStoredData->nClass);
}

CNktDvDbObjectNoRef::eFundamentalType CNktDvDbObjectNoRef::GetFundamentalType() const
{
  return (CNktDvDbObjectNoRef::eFundamentalType)(lpStoredData->nFundamentalType);
}

LPWSTR CNktDvDbObjectNoRef::GetDeclaration() const
{
  if (szDeclW == NULL)
  {
    (const_cast<CNktDvDbObjectNoRef*>(this))->BuildDeclaration();
    if (szDeclW == NULL)
      return NULL;
  }
  return (szDeclW[0] != L'~') ? szDeclW : L"";
}

LPWSTR CNktDvDbObjectNoRef::GetName() const
{
  return (szNameW != NULL) ? szNameW : L"";
}

SIZE_T CNktDvDbObjectNoRef::GetSize() const
{
  return (SIZE_T)(lpStoredData->nSize);
}

SIZE_T CNktDvDbObjectNoRef::GetAlign() const
{
  return (SIZE_T)(lpStoredData->nAlign);
}

BOOL CNktDvDbObjectNoRef::IsArray() const
{
  return (lpStoredData->nArraySizeHint != ULONG_MAX) ? TRUE : FALSE;
}

SIZE_T CNktDvDbObjectNoRef::GetArrayMaxHint() const
{
  ULONG h = lpStoredData->nArraySizeHint;
  return (h != ULONG_MAX) ? (SIZE_T)h : 0;
}

CNktDvDbObjectNoRef::eStringType CNktDvDbObjectNoRef::GetStringType() const
{
  CNktDvDbObjectNoRef *lpDbChildObj;

  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
    case CNktDvDbObjectNoRef::clsArray:
      lpDbChildObj = GetItem(0);
      if (lpDbChildObj != NULL)
      {
        switch (lpDbChildObj->GetFundamentalType())
        {
          case CNktDvDbObjectNoRef::ftAnsiChar:
          case CNktDvDbObjectNoRef::ftSignedByte:
          case CNktDvDbObjectNoRef::ftUnsignedByte:
            return CNktDvDbObjectNoRef::stAnsi;

          case CNktDvDbObjectNoRef::ftWideChar:
          case CNktDvDbObjectNoRef::ftUnsignedWord:
          case CNktDvDbObjectNoRef::ftSignedWord:
            return CNktDvDbObjectNoRef::stWide;
        }
      }
      break;
  }
  return CNktDvDbObjectNoRef::stNone;
}

BOOL CNktDvDbObjectNoRef::IsGuid() const
{
  CNktDvDbObjectNoRef *lpDbChildObj;

  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
    case CNktDvDbObjectNoRef::clsReference:
      lpDbChildObj = GetItem(0);
      if (lpDbChildObj == NULL)
        break; //pointer to nothing
      if (lpDbChildObj->GetClass() != CNktDvDbObjectNoRef::clsStruct ||
          IsGuidName(lpDbChildObj->GetName()) == FALSE)
        break; //not a GUID struct
      return TRUE;

    case CNktDvDbObjectNoRef::clsStruct:
      if (IsGuidName(GetName()) == FALSE)
        break; //not a GUID struct
      return TRUE;
  }
  return S_FALSE;
}

CNktDvDbObjectNoRef* CNktDvDbObjectNoRef::GetEvaluated(__in SIZE_T nArrayIndex) const
{
  CNktDvDbObjectNoRef *lpDbChildObj;

  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
    case CNktDvDbObjectNoRef::clsReference:
    case CNktDvDbObjectNoRef::clsArray:
      lpDbChildObj = GetItem(0);
      if (lpDbChildObj != NULL)
        return lpDbChildObj;
      break;
  }
  return const_cast<CNktDvDbObjectNoRef*>(this);
}

CNktDvDbObjectNoRef* CNktDvDbObjectNoRef::GetFullEvaluated() const
{
  CNktDvDbObjectNoRef *lpEv, *lpCurrEv;

  lpCurrEv = const_cast<CNktDvDbObjectNoRef*>(this);
  while (1)
  {
    lpEv = lpCurrEv->GetEvaluated(0);
    if (lpEv == lpCurrEv)
      break;
    lpCurrEv = lpEv;
  }
  return lpCurrEv;
}

int CNktDvDbObjectNoRef::GetFlags() const
{
  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      return (int)(lpStoredData->nStructUnionFunctionFlags & 0x7FFFFFF0);
  }
  return 0;
}

SIZE_T CNktDvDbObjectNoRef::GetItemsCount() const
{
  SIZE_T nCount;

  nCount = nChildsCount;
  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      nCount--;
  }
  return nCount;
}

SIZE_T CNktDvDbObjectNoRef::NameToIndex(__in LPCWSTR szNameW) const
{
  SIZE_T i, nCount;

  if (szNameW == NULL)
    return NKT_SIZE_T_MAX;
  nCount = GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    if (_wcsicmp(szNameW, lpFirstChild[i].szNameW) == 0)
      return i;
  }
  return NKT_SIZE_T_MAX;
}

HRESULT CNktDvDbObjectNoRef::GetItems(__deref_out CNktDvDbObjectsEnumerator **lplpEnum)
{
  TNktComPtr<CNktDvDbObjectsEnumerator> cEnum;
  TNktComPtr<CNktDvDbObject> cDbObj;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  //----
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbObjectsEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  nCount = GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    cDbObj.Release();
    cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(GetItem(i)));
    if (cDbObj == NULL)
      return E_OUTOFMEMORY;
    hRes = cEnum->Add(cDbObj);
    if (FAILED(hRes))
      return hRes;
  }
  //done
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

CNktDvDbObjectNoRef* CNktDvDbObjectNoRef::GetItem(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount() &&
          lpFirstChild[nIndex].lpObject != NULL) ? lpFirstChild[nIndex].lpObject : NULL;
}

LPWSTR CNktDvDbObjectNoRef::GetItemName(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount()) ? lpFirstChild[nIndex].szNameW : L"";
}

int CNktDvDbObjectNoRef::GetItemFlags(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount()) ? (int)(lpFirstChild[nIndex].lpStoredChildItem->nFlags & 0x0F) : 0;
}

CNktDvDbObjectNoRef::eItemMemberType CNktDvDbObjectNoRef::GetItemMemberType(__in SIZE_T nIndex) const
{
  switch ((nIndex < GetItemsCount()) ? (int)(lpFirstChild[nIndex].lpStoredChildItem->nFlags & 0xF0) : 0)
  {
    case CNktDvDbObjectNoRef::mtPublic:
      return CNktDvDbObjectNoRef::mtPublic;
    case CNktDvDbObjectNoRef::mtPrivate:
      return CNktDvDbObjectNoRef::mtPrivate;
    case CNktDvDbObjectNoRef::mtProtected:
      return CNktDvDbObjectNoRef::mtProtected;
  }
  return CNktDvDbObjectNoRef::mtNA;
}

SIZE_T CNktDvDbObjectNoRef::GetItemStructOffset(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount()) ? (SIZE_T)(lpFirstChild[nIndex].lpStoredChildItem->nStructOffset) : 0;
}

SIZE_T CNktDvDbObjectNoRef::GetItemStructBits(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount()) ? (SIZE_T)(lpFirstChild[nIndex].lpStoredChildItem->nStructBits) : 0;
}

SIZE_T CNktDvDbObjectNoRef::GetItemEnumValue(__in SIZE_T nIndex) const
{
  return (nIndex < GetItemsCount()) ? (SIZE_T)(lpFirstChild[nIndex].lpStoredChildItem->nEnumValue) : 0;
}

LPWSTR CNktDvDbObjectNoRef::GetItemDeclaration(__in SIZE_T nIndex) const
{
  if (nIndex >= GetItemsCount())
    return L"";
  if (lpFirstChild[nIndex].szDeclarationW == NULL)
  {
    (const_cast<CNktDvDbObjectNoRef*>(this))->BuildItemDeclaration(nIndex);
    if (lpFirstChild[nIndex].szDeclarationW == NULL)
      return NULL;
  }
  return (lpFirstChild[nIndex].szDeclarationW[0] != L'~') ? lpFirstChild[nIndex].szDeclarationW : L"";
}

CNktDvDbObjectNoRef::eCallingConvention CNktDvDbObjectNoRef::GetFuncCallingConvention() const
{
  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      switch (lpStoredData->nStructUnionFunctionFlags & 0x0F)
      {
        case CNktDvDbObjectNoRef::ccCDecl:
        case CNktDvDbObjectNoRef::ccFastCall:
        case CNktDvDbObjectNoRef::ccThisCall:
          return (CNktDvDbObjectNoRef::eCallingConvention)(lpStoredData->nStructUnionFunctionFlags & 0x0F);
      }
      return CNktDvDbObjectNoRef::ccStdCall;
  }
  return CNktDvDbObjectNoRef::ccNA;
}

CNktDvDbObjectNoRef* CNktDvDbObjectNoRef::GetFunctionReturn() const
{
  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      return lpFirstChild[nChildsCount-1].lpObject;
  }
  return NULL;
}

int CNktDvDbObjectNoRef::GetFunctionReturnFlags() const
{
  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      return (int)(lpFirstChild[nChildsCount-1].lpStoredChildItem->nFlags & 0x0F);
  }
  return 0;
}

LPWSTR CNktDvDbObjectNoRef::GetFunctionReturnDeclaration() const
{
  SIZE_T nIndex;

  switch (GetClass())
  {
    case CNktDvDbObjectNoRef::clsFunction:
    case CNktDvDbObjectNoRef::clsFunctionType:
    case CNktDvDbObjectNoRef::clsClassConstructor:
    case CNktDvDbObjectNoRef::clsClassDestructor:
    case CNktDvDbObjectNoRef::clsClassOperatorMethod:
    case CNktDvDbObjectNoRef::clsClassMethod:
    case CNktDvDbObjectNoRef::clsClassConverter:
      nIndex = nChildsCount - 1;
      if (lpFirstChild[nIndex].szDeclarationW == NULL) {
        (const_cast<CNktDvDbObjectNoRef*>(this))->BuildItemDeclaration(nIndex);
        if (lpFirstChild[nIndex].szDeclarationW == NULL)
          return NULL;
      }
      return (lpFirstChild[nIndex].szDeclarationW[0] != L'~') ? lpFirstChild[nIndex].szDeclarationW : L"";
  }
  return L"";
}

SIZE_T CNktDvDbObjectNoRef::GetFundamentalTypeSize(__in CNktDvDbObjectNoRef::eFundamentalType nFundType)
{
  switch (nFundType)
  {
    case CNktDvDbObjectNoRef::ftSignedByte:
    case CNktDvDbObjectNoRef::ftUnsignedByte:
    case CNktDvDbObjectNoRef::ftAnsiChar:
      return 8;
    case CNktDvDbObjectNoRef::ftSignedWord:
    case CNktDvDbObjectNoRef::ftUnsignedWord:
    case CNktDvDbObjectNoRef::ftWideChar:
      return 16;
    case CNktDvDbObjectNoRef::ftSignedDoubleWord:
    case CNktDvDbObjectNoRef::ftUnsignedDoubleWord:
    case CNktDvDbObjectNoRef::ftFloat:
      return 32;
    case CNktDvDbObjectNoRef::ftSignedQuadWord:
    case CNktDvDbObjectNoRef::ftUnsignedQuadWord:
    case CNktDvDbObjectNoRef::ftDouble:
    case CNktDvDbObjectNoRef::ftLongDouble:
      return 64;
  }
  return 0;
}

HRESULT CNktDvDbObjectNoRef::GetModules(__deref_out CNktDvDbModulesEnumerator **lplpEnum)
{
  TNktComPtr<CNktDvDbModulesEnumerator> cEnum;
  TNktComPtr<CNktDvDbModule> cDbMod;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (lplpEnum == NULL)
    return E_POINTER;
  *lplpEnum = NULL;
  cEnum.Attach(NKT_MEMMGR_NEW CNktDvDbModulesEnumerator);
  if (cEnum == NULL)
    return E_OUTOFMEMORY;
  nCount = aModulesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    cDbMod.Release();
    cDbMod.Attach(CNktDvDbModule::CreateFromNoRef(aModulesList[i]));
    if (cDbMod == NULL)
      return E_OUTOFMEMORY;
    hRes = cEnum->Add(cDbMod);
    if (FAILED(hRes))
      return hRes;
  }
  //done
  *lplpEnum = cEnum.Detach();
  return S_OK;
}

VOID CNktDvDbObjectNoRef::BuildDeclaration()
{
  CNktSimpleLockNonReentrant cDeclCreationLock(&nDeclCreationMtx);
  CNktStringW cStrTempW;

  if (szDeclW == NULL)
  {
    if (FAILED(BuildDefinitionString(cStrTempW, this)))
      return;
    if (cStrTempW.GetLength() == 0)
    {
      if (cStrTempW.Copy(L"~") == FALSE)
        return;
    }
    szDeclW = CNktStringW::Duplicate((LPWSTR)cStrTempW);
  }
  return;
}

VOID CNktDvDbObjectNoRef::BuildItemDeclaration(__in SIZE_T nIndex)
{
  CNktSimpleLockNonReentrant cDeclCreationLock(&nDeclCreationMtx);
  CNktStringW cStrTempW;

  if (lpFirstChild[nIndex].szDeclarationW == NULL)
  {
    if (FAILED(BuildChildDefinitionString(cStrTempW, nIndex)))
      return;
    if (cStrTempW.GetLength() == 0)
    {
      if (cStrTempW.Copy(L"~") == FALSE)
        return;
    }
    _ReadWriteBarrier();
    lpFirstChild[nIndex].szDeclarationW = CNktStringW::Duplicate((LPWSTR)cStrTempW);
    _ReadWriteBarrier();
  }
  return;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvDbObject::CNktDvDbObject() : CNktDvObject(), CNktDvDbObjectNoRef()
{
  return;
}

CNktDvDbObject* CNktDvDbObject::CreateFromNoRef(__in CNktDvDbObjectNoRef *lpBase)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktStringW cStrTempW;
  SIZE_T nCount;

  if (lpBase == NULL)
    return NULL;
  cDbObj.Attach(NKT_MEMMGR_NEW CNktDvDbObject);
  if (cDbObj != NULL)
  {
    cDbObj->lpStoredData = lpBase->lpStoredData;
    cDbObj->szNameW = lpBase->szNameW;
    cDbObj->lpFirstChild = lpBase->lpFirstChild;
    cDbObj->nChildsCount = lpBase->nChildsCount;
    cDbObj->szDeclW = NULL;
    nCount = lpBase->aModulesList.GetCount();
    if (nCount > 0)
    {
      if (cDbObj->aModulesList.SetCount(nCount) == FALSE)
        return NULL;
      nktMemCopy(cDbObj->aModulesList.GetBuffer(), lpBase->aModulesList.GetBuffer(),
                 nCount * sizeof(CNktDvDbModuleNoRef*));
    }
    //----
    cDbObj->cDvDB = lpBase->lpDvDB;
  }
  return cDbObj.Detach();
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvDbObjectsEnumerator::CNktDvDbObjectsEnumerator() : TNktDvEnumerator<CNktDvDbObject,512>()
{
  lpDvDB = NULL;
  return;
}

CNktDvDbObjectsEnumerator::~CNktDvDbObjectsEnumerator()
{
  if (lpDvDB != NULL)
    lpDvDB->Release();
  return;
}

HRESULT CNktDvDbObjectsEnumerator::GetItemById(__deref_out CNktDvDbObject** lplpDbObj,
                                               __in SIZE_T nId)
{
  CNktDvDbObject **lplpDbObjItem;
  SIZE_T nCount;

  if (lplpDbObj == NULL)
    return E_POINTER;
  *lplpDbObj = NULL;
  nCount = GetCount();
  if (nCount > 0)
  {
    lplpDbObjItem = (CNktDvDbObject**)bsearch_s(&nId, aObjectsList.GetBuffer(), nCount,
                                                sizeof(CNktDvDbObject*), SearchDbObjectsEnumById, NULL);
    if (lplpDbObjItem != NULL)
    {
      *lplpDbObj = *lplpDbObjItem;
      (*lplpDbObj)->AddRef();
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvDbObjectsEnumerator::GetItemByName(__deref_out CNktDvDbObject** lplpDbObj,
                                                 __in_nz_opt LPCWSTR szNameW, __in SIZE_T nNameLen)
{
  sSearchDbObjectsEnumByName_Ctx sCtx;
  SIZE_T nCount;
  ULONG *lpIdx, *lpStart;
  HRESULT hRes;

  if (lplpDbObj == NULL)
    return E_POINTER;
  *lplpDbObj = NULL;
  sCtx.sParsedName.szNameW = szNameW;
  sCtx.sParsedName.nNameLen = nNameLen;
  hRes = CNktDvEngDatabase::ParseNamespaceAndName(sCtx.sParsedName.szNamespaceW, sCtx.sParsedName.nNamespaceLen,
                                                  sCtx.sParsedName.szNameW, sCtx.sParsedName.nNameLen);
  if (FAILED(hRes))
    return hRes;
  sCtx.lpBuffer = aObjectsList.GetBuffer();
  lpStart = cNameIndexList.Get();
  nCount = GetCount();
fobcan_restart:
  //try to locate namespace/name object
  lpIdx = (ULONG*)bsearch_s(&sCtx, lpStart, nCount, sizeof(ULONG), SearchDbObjectsEnumByName, NULL);
  //on fail...
  if (lpIdx == NULL)
  {
    if (sCtx.sParsedName.nNamespaceLen == 0)
    {
      //...no namespace was specified to find the first item with any namespace
      sCtx.sParsedName.nNamespaceLen = NKT_SIZE_T_MAX;
      lpIdx = (ULONG*)bsearch_s(&sCtx, lpStart, nCount, sizeof(ULONG), SearchDbObjectsEnumByName, NULL);
      if (lpIdx != NULL)
      {
        //a match was found, move upwards until the first item
        while (lpIdx > lpStart)
        {
          if (MyStrCmpW(sCtx.sParsedName.szNameW, sCtx.sParsedName.nNameLen,
              sCtx.lpBuffer[*(lpIdx-1)]->GetName()) != 0)
            break;
          lpIdx--;
        }
      }
    }
    else
    {
      //...a namespace was provided but may be part of the name and not a namespace
      //so rebuild parsed name data and retry with the full name
      sCtx.sParsedName.nNameLen += (SIZE_T)(sCtx.sParsedName.szNameW - sCtx.sParsedName.szNamespaceW);
      sCtx.sParsedName.szNameW = sCtx.sParsedName.szNamespaceW;
      sCtx.sParsedName.szNamespaceW = L"";
      sCtx.sParsedName.nNamespaceLen = 0;
      goto fobcan_restart;
    }
  }
  if (lpIdx != NULL)
  {
    *lplpDbObj = sCtx.lpBuffer[*lpIdx];
    (*lplpDbObj)->AddRef();
    return S_OK;
  }
  return NKT_DVERR_NotFound;
}

HRESULT CNktDvDbObjectsEnumerator::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvDbObjectsEnumerator [%Iu items]", GetCount()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

HRESULT CNktDvDbObjectsEnumerator::Initialize(__in CNktDvEngDatabase *_lpDvDB,
                                              __in BOOL bOnlyFunctions)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktDvDbObjectNoRef *lpDbObjNoRef;
  ULONG i, nCount;
  HRESULT hRes;

  NKT_ASSERT(_lpDvDB != NULL);
  lpDvDB = _lpDvDB;
  lpDvDB->AddRef();
  //----
  nCount = (bOnlyFunctions == FALSE) ? (lpDvDB->GetObjectsCount()) : (lpDvDB->GetFunctionsCount());
  for (i=0; i<nCount; i++)
  {
    lpDbObjNoRef = (bOnlyFunctions == FALSE) ? (lpDvDB->GetObject(i)) : (lpDvDB->GetFunction(i));
    cDbObj.Release();
    cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjNoRef));
    if (cDbObj == NULL)
      return E_OUTOFMEMORY;
    hRes = Add(cDbObj);
    if (FAILED(hRes))
      return hRes;
  }
  if (nCount > 0)
  {
    cNameIndexList.Attach((ULONG*)nktMemMalloc(nCount*sizeof(ULONG)));
    if (cNameIndexList == NULL)
      return E_OUTOFMEMORY;
    for (i=0; i<nCount; i++)
      cNameIndexList[i] = i;
    qsort_s(cNameIndexList.Get(), nCount, sizeof(ULONG), SortDbObjectsEnumByName,
            aObjectsList.GetBuffer());
  }
  return S_OK;
}

//-----------------------------------------------------------

static int __cdecl SortDbObjectsEnumByName(void *lpContext, const void *lpIdx1, const void *lpIdx2)
{
  CNktDvDbObject *lpDbObj[2];
  int res;

  lpDbObj[0] = reinterpret_cast<CNktDvDbObject**>(lpContext)[*((ULONG*)lpIdx1)];
  lpDbObj[1] = reinterpret_cast<CNktDvDbObject**>(lpContext)[*((ULONG*)lpIdx2)];
  //sort by name
  res = wcscmp(lpDbObj[0]->GetName(), lpDbObj[1]->GetName());
  if (res != 0)
    return res;
  //subsort by namespace
  return wcscmp(lpDbObj[0]->GetNamespace(), lpDbObj[1]->GetNamespace());
}

static int __cdecl SearchDbObjectsEnumById(void *lpContext, const void *lpKey, const void *lpObj)
{
  CNktDvDbObject *lpDbObj;

  lpDbObj = *reinterpret_cast<CNktDvDbObject**>(const_cast<void*>(lpObj));
  return NKT_DV_CMP<SIZE_T>(*((SIZE_T*)lpKey), (SIZE_T)(lpDbObj->GetId()));
}

static int __cdecl SearchDbObjectsEnumByName(void *lpContext, const void *lpKey, const void *lpObj)
{
  sSearchDbObjectsEnumByName_Ctx *lpCtx = (sSearchDbObjectsEnumByName_Ctx*)lpKey;
  CNktDvDbObject *lpDbObj;
  int res;

  lpDbObj = lpCtx->lpBuffer[*((ULONG*)lpObj)];
  res = MyStrCmpW(lpCtx->sParsedName.szNameW, lpCtx->sParsedName.nNameLen,
                  lpDbObj->GetName());
  if (res == 0 && lpCtx->sParsedName.nNamespaceLen != NKT_SIZE_T_MAX)
  {
    res = MyStrCmpW(lpCtx->sParsedName.szNamespaceW, lpCtx->sParsedName.nNamespaceLen,
                    lpDbObj->GetNamespace());
  }
  return res;
}

static BOOL IsGuidName(__in LPCWSTR szNameW)
{
  return (wcscmp(szNameW, L"_GUID") == 0 || wcscmp(szNameW, L"GUID") == 0 ||
          wcscmp(szNameW, L"IID") == 0 || wcscmp(szNameW, L"CLSID") == 0 ||
          wcscmp(szNameW, L"UUID") == 0) ? TRUE : FALSE;
}
