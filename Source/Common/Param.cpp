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

#include "Param.h"
#include "Tools.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#define FUNDTYPE_MASK_FULL            0xFFFFFFFFFFFFFFFFui64
#define IS_PARAM_ACCESS_ALLOWED                            \
              (nEnvironment != CNktDvParam::eEnvSpyMgr ||  \
               cCallStateCont->IsValid() != FALSE)

//-----------------------------------------------------------

static BOOL IsGuidName(__in LPCWSTR szNameW);

//-----------------------------------------------------------

CNktDvParam::CNktDvParam(__in SIZE_T _nPlatformBits) : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  NKT_ASSERT(_nPlatformBits == 32 || _nPlatformBits == 64);
  szNameW = szTypeNameW = szDeclW = NULL;
  lpAddress = NULL;
  nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  nFlags = 0;
  bReadOnly = FALSE;
  nEnvironment = CNktDvParam::eEnvUserDefined;
  lpDbObj = lpDbParentObj = NULL;
  sCustomValue.nFundType = CNktDvDbObject::ftNone;
  sCustomValue.nSize = 0;
  nktMemSet(&sFundamentalExtra, 0, sizeof(sFundamentalExtra));
  nPlatformBits = _nPlatformBits;
  return;
}

CNktDvParam::~CNktDvParam()
{
  return;
}

LPVOID CNktDvParam::GetAddress() const
{
  return (sCustomValue.nFundType == CNktDvDbObject::ftNone) ? lpAddress : NULL;
}

CNktDvDbObjectNoRef::eClass CNktDvParam::GetClass() const
{
  return (sCustomValue.nFundType != CNktDvDbObject::ftNone) ? CNktDvDbObjectNoRef::clsFundamental :
                                                            (lpDbObj->GetClass());
}

CNktDvDbObjectNoRef::eFundamentalType CNktDvParam::GetBasicType() const
{
  /*
  CNktDbObject *lpDbChildObj;
  */

  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    return sCustomValue.nFundType;
  /*
  switch (lpDbObj->GetClass())
  {
    case CNktDbObject::clsPointer:
    case CNktDbObject::clsReference:
      lpDbChildObj = lpDbObj->GetItem(0);
      return lpDbChildObj->GetFundamentalType();
  }
  */
  return lpDbObj->GetFundamentalType();
}

LPWSTR CNktDvParam::GetName() const
{
  return (szNameW != NULL) ? szNameW : L"";
}

LPWSTR CNktDvParam::GetTypeName() const
{
  return (sCustomValue.nFundType == CNktDvDbObject::ftNone && szTypeNameW != NULL) ? szTypeNameW : L"";
}

LPWSTR CNktDvParam::GetDeclaration() const
{
  return (szDeclW != NULL) ? szDeclW : L"";
}

int CNktDvParam::GetFlags() const
{
  return nFlags;
}

BOOL CNktDvParam::IsPointer() const
{
  if (sCustomValue.nFundType == CNktDvDbObject::ftNone)
  {
    switch (lpDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsPointer:
      case CNktDvDbObjectNoRef::clsReference:
        return TRUE;
    }
  }
  return FALSE;
}

BOOL CNktDvParam::IsArray() const
{
  switch (sCustomValue.nFundType)
  {
    case CNktDvDbObject::ftNone:
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsArray)
        return TRUE;
      break;
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftSignedDoubleWord:
    case CNktDvDbObject::ftUnsignedDoubleWord:
    case CNktDvDbObject::ftSignedQuadWord:
    case CNktDvDbObject::ftUnsignedQuadWord:
    case CNktDvDbObject::ftFloat:
    case CNktDvDbObject::ftDouble:
      if ((sCustomValue.nSize / (sFundamentalExtra.nSize >> 3)) > 1)
        return TRUE;
      break;
  }
  return FALSE;
}

SIZE_T CNktDvParam::GetArrayMaxHint() const
{
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    return sCustomValue.nSize / (sFundamentalExtra.nSize >> 3);
  return lpDbObj->GetArrayMaxHint();
}

CNktDvParam::eStringType CNktDvParam::GetStringType() const
{
  CNktDvDbObjectNoRef *lpDbChildObj;

  switch (sCustomValue.nFundType)
  {
    case CNktDvDbObject::ftNone:
      switch (lpDbObj->GetClass())
      {
        case CNktDvDbObjectNoRef::clsPointer:
        case CNktDvDbObjectNoRef::clsArray:
          lpDbChildObj = lpDbObj->GetItem(0);
          if (lpDbChildObj != NULL)
          {
            switch (lpDbChildObj->GetFundamentalType())
            {
              case CNktDvDbObject::ftAnsiChar:
              case CNktDvDbObject::ftSignedByte:
              case CNktDvDbObject::ftUnsignedByte:
                return CNktDvParam::stAnsi;

              case CNktDvDbObject::ftWideChar:
              case CNktDvDbObject::ftUnsignedWord:
              case CNktDvDbObject::ftSignedWord:
                return CNktDvParam::stWide;
            }
          }
          break;
      }
      break;

    case CNktDvDbObject::ftAnsiChar:
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
      return CNktDvParam::stAnsi;

    case CNktDvDbObject::ftWideChar:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftSignedWord:
      return CNktDvParam::stWide;
  }
  return CNktDvParam::stNone;
}

HRESULT CNktDvParam::IsIntResourceString(__out unsigned short *lpnId) const
{
  CNktAutoFastMutex cLock(&const_cast<CNktDvParam*>(this)->GetLockObject());
  CNktDvDbObjectNoRef *lpDbChildObj;
  LPBYTE lpValue;
  HRESULT hRes;

  if (sCustomValue.nFundType == CNktDvDbObject::ftNone &&
      lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsPointer)
  {
    lpDbChildObj = lpDbObj->GetItem(0);
    if (lpDbChildObj != NULL)
    {
      switch (lpDbChildObj->GetFundamentalType())
      {
        case CNktDvDbObject::ftAnsiChar:
        case CNktDvDbObject::ftWideChar:
          if (lpAddress != NULL)
          {
            //internal pointer is not null
            hRes = const_cast<CNktDvParam*>(this)->ReadPointer(&lpValue, lpAddress);
            if (FAILED(hRes))
            {
              if (lpnId != NULL)
                *lpnId = 0;
              return hRes;
            }
            if (IS_INTRESOURCE(lpValue))
            {
              if (lpnId != NULL)
                *lpnId = LOWORD(lpValue);
              return S_OK;
            }
          }
          break;
      }
    }
  }
  if (lpnId != NULL)
    *lpnId = 0;
  return S_FALSE;
}

HRESULT CNktDvParam::IsGuidString(__inout CNktStringW &cStrDest) const
{
  CNktDvParam *lpThis = const_cast<CNktDvParam*>(this);
  CNktAutoFastMutex cLock(&(lpThis->GetLockObject()));
  CNktDvDbObjectNoRef *lpDbChildObj;
  LPBYTE lpSrcAddr;
  GUID sGuid;
  HRESULT hRes;

  cStrDest.Empty();
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    return S_FALSE;
  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
    case CNktDvDbObjectNoRef::clsReference:
      lpDbChildObj = lpDbObj->GetItem(0);
      if (lpDbChildObj == NULL)
        break; //pointer to nothing
      if (lpDbChildObj->GetClass() != CNktDvDbObjectNoRef::clsStruct ||
          IsGuidName(lpDbChildObj->GetName()) == FALSE)
        break; //not a GUID struct
      if (lpAddress == NULL)
        return E_FAIL; //internal pointer is null
      hRes = lpThis->ReadPointer(&lpSrcAddr, lpAddress);
      if (FAILED(hRes))
        return hRes;
      goto read_guid_struct;

    case CNktDvDbObjectNoRef::clsStruct:
      if (IsGuidName(lpDbObj->GetName()) == FALSE)
        break; //not a GUID struct
      lpSrcAddr = (LPBYTE)lpAddress;
read_guid_struct:
      if (lpSrcAddr == NULL)
        return E_FAIL; //internal pointer is null
      hRes = lpThis->ReadMemoryBytes(&sGuid, lpSrcAddr, sizeof(sGuid), FALSE);
      if (FAILED(hRes))
        return hRes;
      if (cStrDest.Format(L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", sGuid.Data1, sGuid.Data2,
                          sGuid.Data3, sGuid.Data4[0], sGuid.Data4[1], sGuid.Data4[2], sGuid.Data4[3],
                          sGuid.Data4[4], sGuid.Data4[5], sGuid.Data4[6], sGuid.Data4[7]) == FALSE)
        return E_OUTOFMEMORY;
      return S_OK;
  }
  return S_FALSE;
}

HRESULT CNktDvParam::GetEvaluated(__deref_out CNktDvParam **lplpParam, __in SIZE_T nArrayIndex)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktComPtr<CNktDvParam> cTempParam;
  CNktDvDbObjectNoRef *lpDbChildObj;
  LPBYTE lpNewAddr;
  HRESULT hRes;

  if (lplpParam == NULL)
    return E_POINTER;
  *lplpParam = NULL;
  //----
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
  {
    cTempParam = this;
  }
  else
  {
    switch (lpDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsPointer:
      case CNktDvDbObjectNoRef::clsReference:
        lpDbChildObj = lpDbObj->GetItem(0);
        if (lpDbChildObj == NULL)
          return E_FAIL; //pointer to nothing
        if (lpAddress == NULL)
          return E_FAIL; //internal pointer is null
        hRes = ReadPointer(&lpNewAddr, lpAddress);
        if (FAILED(hRes))
          return hRes;
ge_createparam:
        //create new param
        cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
        if (cTempParam == NULL)
          return E_OUTOFMEMORY;
        cTempParam->szNameW = lpDbObj->GetItemName(0);
        NKT_ASSERT(cTempParam->szNameW != NULL);
        cTempParam->szTypeNameW = lpDbChildObj->GetName();
        NKT_ASSERT(cTempParam->szTypeNameW != NULL);
        cTempParam->szDeclW = lpDbObj->GetItemDeclaration(0);
        if (cTempParam->szDeclW == NULL)
          return E_OUTOFMEMORY;
        cTempParam->nFlags = lpDbObj->GetItemFlags(0);
        cTempParam->lpAddress = lpNewAddr + (nArrayIndex * ((lpDbChildObj->GetSize() + 7) >> 3));
        cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
        cTempParam->bReadOnly = bReadOnly;
        cTempParam->nEnvironment = nEnvironment;
        cTempParam->lpDbObj = lpDbChildObj;
        cTempParam->lpDbParentObj = lpDbObj;
        cTempParam->cProcMem = cProcMem;
        cTempParam->cCallStateCont = cCallStateCont;
        cTempParam->Init2();
        break;

      case CNktDvDbObjectNoRef::clsArray:
        lpDbChildObj = lpDbObj->GetItem(0);
        if (lpDbChildObj == NULL)
          return E_FAIL; //pointer to nothing
        if (lpAddress == NULL)
          return E_FAIL; //internal pointer is null
        lpNewAddr = (LPBYTE)lpAddress + (nArrayIndex * ((lpDbChildObj->GetSize() + 7) >> 3));
        goto ge_createparam;

      default:
        cTempParam = this;
        break;
    }
  }
  //done
  *lplpParam = cTempParam.Detach();
  return S_OK;
}

HRESULT CNktDvParam::GetFullEvaluated(__deref_out CNktDvParam **lplpParam)
{
  TNktComPtr<CNktDvParam> cEvParam, cCurrEvParam;
  HRESULT hRes;

  if (lplpParam == NULL)
    return E_POINTER;
  *lplpParam = NULL;
  cCurrEvParam = this;
  hRes = GetEvaluated(&cEvParam, 0);
  while (SUCCEEDED(hRes) && cEvParam != cCurrEvParam)
  {
    cCurrEvParam = cEvParam;
    cEvParam.Release();
    hRes = cCurrEvParam->GetEvaluated(&cEvParam, 0);
  }
  if (SUCCEEDED(hRes))
    *lplpParam = cEvParam.Detach();
  return hRes;
}

SIZE_T CNktDvParam::GetFieldsCount() const
{
  if (sCustomValue.nFundType == CNktDvDbObject::ftNone)
  {
    switch (lpDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsFunction:
      case CNktDvDbObjectNoRef::clsFunctionType:
      case CNktDvDbObjectNoRef::clsClassConstructor:
      case CNktDvDbObjectNoRef::clsClassDestructor:
      case CNktDvDbObjectNoRef::clsClassOperatorMethod:
      case CNktDvDbObjectNoRef::clsClassMethod:
      case CNktDvDbObjectNoRef::clsClassConverter:
      case CNktDvDbObjectNoRef::clsStruct:
      case CNktDvDbObjectNoRef::clsUnion:
        return lpDbObj->GetItemsCount();
    }
  }
  return 0;
}

HRESULT CNktDvParam::GetField(__deref_out CNktDvParam **lplpParam, __in SIZE_T nIndex)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktComPtr<CNktDvParam> cTempParam;
  CNktDvDbObjectNoRef *lpDbChildObj;
  CNktDvDbObjectNoRef::eClass nClass;
  LPBYTE lpNewAddr;
  SIZE_T nShift, nBits;

  if (lplpParam == NULL)
    return E_POINTER;
  *lplpParam = NULL;
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    return E_FAIL;
  switch (nClass = lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
      if (nIndex >= lpDbObj->GetItemsCount())
        return E_INVALIDARG;
      lpDbChildObj = lpDbObj->GetItem(nIndex);
      if (lpDbChildObj == NULL)
        return E_FAIL; //pointer to nothing
      //get struct address
      nShift = lpDbObj->GetItemStructOffset(nIndex);
      lpNewAddr = (LPBYTE)lpAddress + (nShift >> 3);
      nShift &= 7;
      //create new param
      cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
      if (cTempParam == NULL)
        return E_OUTOFMEMORY;
      cTempParam->szNameW = lpDbObj->GetItemName(nIndex);
      NKT_ASSERT(cTempParam->szNameW != NULL);
      cTempParam->szTypeNameW = lpDbChildObj->GetName();
      NKT_ASSERT(cTempParam->szTypeNameW != NULL);
      cTempParam->szDeclW = lpDbObj->GetItemDeclaration(nIndex);
      if (cTempParam->szDeclW == NULL)
        return E_OUTOFMEMORY;
      cTempParam->nFlags = lpDbObj->GetItemFlags(nIndex);
      cTempParam->lpAddress = lpNewAddr;
      cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
      cTempParam->bReadOnly = bReadOnly;
      cTempParam->nEnvironment = nEnvironment;
      cTempParam->lpDbObj = lpDbChildObj;
      cTempParam->lpDbParentObj = lpDbObj;
      cTempParam->cProcMem = cProcMem;
      cTempParam->cCallStateCont = cCallStateCont;
      if (lpDbChildObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
      {
        cTempParam->sFundamentalExtra.nSize = lpDbChildObj->GetSize();
        nBits = lpDbObj->GetItemStructBits(nIndex);
        if (nBits == 0 || nBits >= cTempParam->sFundamentalExtra.nSize)
        {
          cTempParam->sFundamentalExtra.nShiftLeft = 0;
          cTempParam->sFundamentalExtra.nMask = FUNDTYPE_MASK_FULL;
        }
        else
        {
          cTempParam->sFundamentalExtra.nShiftLeft = nShift;
          if (nBits+nShift > cTempParam->sFundamentalExtra.nSize)
            nBits = cTempParam->sFundamentalExtra.nSize - nShift;
          cTempParam->sFundamentalExtra.nMask = ((1ui64 << nBits) - 1ui64) << nShift;
        }
      }
      else
      {
        cTempParam->Init2();
      }
      break;
    default:
      return E_FAIL;
  }
  //done
  *lplpParam = cTempParam.Detach();
  return S_OK;
}

HRESULT CNktDvParam::GetField(__deref_out CNktDvParam **lplpParam, __in LPCWSTR szFieldNameW)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  SIZE_T nIndex, nCount;

  if (lplpParam != NULL)
    *lplpParam = NULL;
  if (lplpParam == NULL || szFieldNameW == NULL)
    return E_POINTER;
  if (szFieldNameW[0] == 0)
    return E_INVALIDARG;
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    return E_FAIL;
  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsStruct:
    case CNktDvDbObjectNoRef::clsUnion:
      nCount = lpDbObj->GetItemsCount();
      for (nIndex=0; nIndex<nCount; nIndex++)
      {
        if (wcscmp(szFieldNameW, lpDbObj->GetItemName(nIndex)) == 0)
          return GetField(lplpParam, nIndex);
      }
      return NKT_DVERR_NotFound;
  }
  return E_FAIL;
}

HRESULT CNktDvParam::GetFields(__deref_out CNktDvParamsEnumerator **lplpParamEnum)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  TNktComPtr<CNktDvParamsEnumerator> cTempParamEnum;
  TNktComPtr<CNktDvParam> cTempParam;
  SIZE_T i, nCount;
  HRESULT hRes;

  if (lplpParamEnum == NULL)
    return E_POINTER;
  *lplpParamEnum = NULL;
  //----
  cTempParamEnum.Attach(NKT_MEMMGR_NEW CNktDvParamsEnumerator);
  if (cTempParamEnum == NULL)
    return E_OUTOFMEMORY;
  nCount = GetFieldsCount();
  for (i=0; i<nCount; i++)
  {
    cTempParam.Release();
    hRes = GetField(&cTempParam, i);
    if (SUCCEEDED(hRes))
      hRes = cTempParamEnum->Add(cTempParam);
    if (FAILED(hRes))
      return hRes;
  }
  //done
  *lplpParamEnum = cTempParamEnum.Detach();
  return S_OK;
}

HRESULT CNktDvParam::GetByte(__out unsigned char *lpValue, __in SIZE_T nIndex)
{
  return GetValueCommon(lpValue, nIndex, CNktDvDbObject::ftUnsignedByte);
}

HRESULT CNktDvParam::SetByte(__in unsigned char nValue, __in SIZE_T nIndex)
{
  return SetValueCommon(&nValue, nIndex, CNktDvDbObject::ftUnsignedByte);
}

HRESULT CNktDvParam::GetWord(__out unsigned short *lpValue, __in SIZE_T nIndex)
{
  return GetValueCommon(lpValue, nIndex, CNktDvDbObject::ftUnsignedWord);
}

HRESULT CNktDvParam::SetWord(__in unsigned short nValue, __in SIZE_T nIndex)
{
  return SetValueCommon(&nValue, nIndex, CNktDvDbObject::ftUnsignedWord);
}

HRESULT CNktDvParam::GetDWord(__out unsigned long *lpValue, __in SIZE_T nIndex)
{
  return GetValueCommon(lpValue, nIndex, CNktDvDbObject::ftUnsignedDoubleWord);
}

HRESULT CNktDvParam::SetDWord(__in unsigned long nValue, __in SIZE_T nIndex)
{
  return SetValueCommon(&nValue, nIndex, CNktDvDbObject::ftUnsignedDoubleWord);
}

HRESULT CNktDvParam::GetQWord(__out unsigned __int64 *lpValue, __in SIZE_T nIndex)
{
  return GetValueCommon(lpValue, nIndex, CNktDvDbObject::ftUnsignedQuadWord);
}

HRESULT CNktDvParam::SetQWord(__in unsigned __int64 nValue, __in SIZE_T nIndex)
{
  return SetValueCommon(&nValue, nIndex, CNktDvDbObject::ftUnsignedQuadWord);
}

HRESULT CNktDvParam::GetFloat(__out float *lpValue, __in SIZE_T nIndex)
{
  double nDbl;
  HRESULT hRes;

  if (lpValue == NULL)
    return E_POINTER;
  hRes = GetValueCommon(&nDbl, nIndex, CNktDvDbObject::ftDouble);
  *lpValue = (SUCCEEDED(hRes)) ? (float)nDbl : 0.0f;
  return hRes;
}

HRESULT CNktDvParam::SetFloat(__in float nValue, __in SIZE_T nIndex)
{
  double nDbl;

  nDbl = (double)nValue;
  return SetValueCommon(&nDbl, nIndex, CNktDvDbObject::ftDouble);
}

HRESULT CNktDvParam::GetDouble(__out double *lpValue, __in SIZE_T nIndex)
{
  return GetValueCommon(lpValue, nIndex, CNktDvDbObject::ftDouble);
}

HRESULT CNktDvParam::SetDouble(__in double nValue, __in SIZE_T nIndex)
{
  return SetValueCommon(&nValue, nIndex, CNktDvDbObject::ftDouble);
}

HRESULT CNktDvParam::GetLongDouble(__out long double *lpValue, __in SIZE_T nIndex)
{
  double nDbl;
  HRESULT hRes;

  if (lpValue == NULL)
    return E_POINTER;
  hRes = GetValueCommon(&nDbl, nIndex, CNktDvDbObject::ftDouble);
  *lpValue = (SUCCEEDED(hRes)) ? (long double)nDbl : 0.0;
  return hRes;
}

HRESULT CNktDvParam::SetLongDouble(__in long double nValue, __in SIZE_T nIndex)
{
  double nDbl;

  nDbl = (double)nValue;
  return SetValueCommon(&nDbl, nIndex, CNktDvDbObject::ftDouble);
}

HRESULT CNktDvParam::GetAnsiString(__out LPSTR *lpszValue, __in SIZE_T nStart, __in SIZE_T nMaxLength)
{
  return GetStringCommon((LPVOID*)lpszValue, NULL, nStart, nMaxLength, FALSE);
}

HRESULT CNktDvParam::SetAnsiString(__in LPSTR szValue, __in SIZE_T nStart, __in BOOL bIncludeEOS)
{
  return SetStringCommon(szValue, nStart, bIncludeEOS, FALSE);
}

HRESULT CNktDvParam::GetWideString(__out LPWSTR *lpszValue, __in SIZE_T nStart, __in SIZE_T nMaxLength)
{
  return GetStringCommon((LPVOID*)lpszValue, NULL, nStart, nMaxLength, TRUE);
}

HRESULT CNktDvParam::SetWideString(__in LPWSTR szValue, __in SIZE_T nStart, __in BOOL bIncludeEOS)
{
  return SetStringCommon(szValue, nStart, bIncludeEOS, TRUE);
}

HRESULT CNktDvParam::GetAnsiStringLength(__out SIZE_T *lpnLength, SIZE_T nIndex)
{
  return GetStringCommon(NULL, lpnLength, 0, NKT_SIZE_T_MAX, FALSE);
}

HRESULT CNktDvParam::GetWideStringLength(__out SIZE_T *lpnLength, SIZE_T nIndex)
{
  return GetStringCommon(NULL, lpnLength, 0, NKT_SIZE_T_MAX, TRUE);
}

VOID CNktDvParam::FreeString(__inout LPVOID szString)
{
  nktMemFree(szString);
  return;
}

HRESULT CNktDvParam::GetSizeT(__out SIZE_T *lpValue, __in SIZE_T nIndex)
{
  HRESULT hRes;
  union {
    ULONG dw;
    ULONGLONG qw;
  };

  if (lpValue == NULL)
    return E_POINTER;
  switch (nPlatformBits)
  {
    case 32:
      hRes = GetDWord(&dw, nIndex);
      *lpValue = (SUCCEEDED(hRes)) ? (SIZE_T)dw : 0;
      break;

#if defined _M_X64
    case 64:
      hRes = GetQWord(&qw, nIndex);
      *lpValue = (SUCCEEDED(hRes)) ? (SIZE_T)qw : 0;
      break;
#endif //_M_X64

    default:
      hRes = E_FAIL;
      *lpValue = 0;
      break;
  }
  return hRes;
}

HRESULT CNktDvParam::SetSizeT(__in SIZE_T nValue, __in SIZE_T nIndex)
{
  switch (nPlatformBits)
  {
    case 32:
#if defined _M_X64
      if ((ULONGLONG)nValue > 0xFFFFFFFFUi64)
        break;
#endif //_M_X64
      return SetDWord((ULONG)nValue, nIndex);

#if defined _M_X64
    case 64:
      return SetQWord((ULONGLONG)nValue, nIndex);
#endif //_M_X64
  }
  return E_FAIL;
}

HRESULT CNktDvParam::GetPointer(__out LPVOID *lplpValue)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;

  if (lplpValue == NULL)
    return E_POINTER;
  hRes = E_FAIL;
  if (IS_PARAM_ACCESS_ALLOWED)
  {
    //param access allowed
    switch (sCustomValue.nFundType)
    {
      case CNktDvDbObject::ftNone:
        switch (lpDbObj->GetClass())
        {
          case CNktDvDbObjectNoRef::clsPointer:
          case CNktDvDbObjectNoRef::clsReference:
            if (lpAddress != NULL) //internal pointer is not null
              hRes = ReadPointer((LPBYTE*)lplpValue, lpAddress);
            break;
          case CNktDvDbObjectNoRef::clsFundamental:
            switch (lpDbObj->GetFundamentalType())
            {
              case CNktDvDbObjectNoRef::ftSignedDoubleWord:
              case CNktDvDbObjectNoRef::ftUnsignedDoubleWord:
                if (lpAddress != NULL && nPlatformBits == 32)
                {
                  //internal pointer is not null and platform bits is ok
                  hRes = ReadPointer((LPBYTE*)lplpValue, lpAddress);
                }
                break;

              case CNktDvDbObjectNoRef::ftSignedQuadWord:
              case CNktDvDbObjectNoRef::ftUnsignedQuadWord:
                if (lpAddress != NULL && nPlatformBits == 64)
                {
                  //internal pointer is not null and platform bits is ok
                  hRes = ReadPointer((LPBYTE*)lplpValue, lpAddress);
                }
                break;
            }
            break;
        }
        break;

      case CNktDvDbObjectNoRef::ftSignedDoubleWord:
      case CNktDvDbObjectNoRef::ftUnsignedDoubleWord:
        if (nPlatformBits == 32)
        {
          unsigned long nTemp32;

          if (SUCCEEDED(GetDWord(&nTemp32, 0)))
          {
            *lplpValue = (LPVOID)((ULONG_PTR)nTemp32);
            hRes = S_OK;
          }
        }
        break;

#if defined _M_X64
      case CNktDvDbObjectNoRef::ftSignedQuadWord:
      case CNktDvDbObjectNoRef::ftUnsignedQuadWord:
        if (nPlatformBits == 64)
        {
          unsigned __int64 nTemp64;

          if (SUCCEEDED(GetQWord(&nTemp64, 0)))
          {
            *lplpValue = (LPVOID)nTemp64;
            hRes = S_OK;
          }
        }
        break;
#endif //_M_X64
    }
  }
  if (FAILED(hRes))
    *lplpValue = NULL;
  return hRes;
}

HRESULT CNktDvParam::SetPointer(__in LPVOID lpValue)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  union {
    ULONG nValue32;
    ULONGLONG nValue64;
  };

  if (IS_PARAM_ACCESS_ALLOWED &&
      sCustomValue.nFundType == CNktDvDbObject::ftNone &&
      bReadOnly == FALSE)
  {
    switch (lpDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsPointer:
      case CNktDvDbObjectNoRef::clsReference:
        if (lpAddress != NULL)
        {
          switch (nPlatformBits)
          {
            case 32:
#if defined _M_X64
              if ((ULONGLONG)lpValue > 0xFFFFFFFFUi64)
                break;
#endif //_M_X64
              nValue32 = (ULONG)((ULONG_PTR)lpValue);
              return WriteMemoryBytes(lpAddress, &nValue32, sizeof(nValue32));
#if defined _M_X64
            case 64:
              nValue64 = (ULONGLONG)lpValue;
              return WriteMemoryBytes(lpAddress, &nValue64, sizeof(nValue64));
#endif //_M_X64
          }
        }
        break;
    }
  }
  return E_FAIL;
}

HRESULT CNktDvParam::IsNullPointer(__out LPBOOL lpbIsNull)
{
  LPVOID lpValue;
  HRESULT hRes;

  if (lpbIsNull == NULL)
    return E_POINTER;
  *lpbIsNull = TRUE;
  hRes = GetPointer(&lpValue);
  if (SUCCEEDED(hRes) && lpValue != NULL)
    *lpbIsNull = FALSE;
  return hRes;
}

HRESULT CNktDvParam::GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem)
{
  if (lplpProcMem == NULL)
    return E_POINTER;
  *lplpProcMem = cProcMem;
  NKT_ASSERT(*lplpProcMem != NULL);
  (*lplpProcMem)->AddRef();
  return S_OK;
}

HRESULT CNktDvParam::CastTo(__deref_out CNktDvParam **lplpParam, __in_z LPCWSTR szNewTypeW)
{
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  TNktComPtr<CNktDvParam> cTempParam;
  CNktDvDbObjectNoRef *lpDbNewObj;
  HRESULT hRes;

  if (lplpParam != NULL)
    *lplpParam = NULL;
  if (lplpParam == NULL || szNewTypeW == NULL)
    return E_POINTER;
  if (szNewTypeW[0] == 0)
    return E_INVALIDARG;
  hRes = CNktDvEngDatabase::Get(&cDvDB, NULL, nPlatformBits);
  if (FAILED(hRes))
    return hRes;
  lpDbNewObj = cDvDB->FindObjectByName(szNewTypeW);
  if (lpDbNewObj == NULL)
    return NKT_DVERR_NotFound;
  //----
  cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
  if (cTempParam == NULL)
    return E_OUTOFMEMORY;
  cTempParam->szDeclW = lpDbNewObj->GetDeclaration();
  if (cTempParam->szDeclW == NULL)
    return E_OUTOFMEMORY;
  cTempParam->szNameW = szNameW;
  cTempParam->szTypeNameW = lpDbNewObj->GetName();
  NKT_ASSERT(cTempParam->szTypeNameW != NULL);
  cTempParam->nFlags = lpDbNewObj->GetFlags();
  cTempParam->lpAddress = lpAddress;
  cTempParam->nInternalAsmRegisterIndex = nInternalAsmRegisterIndex;
  cTempParam->bReadOnly = bReadOnly;
  cTempParam->nEnvironment = nEnvironment;
  cTempParam->lpDbObj = lpDbNewObj;
  cTempParam->lpDbParentObj = NULL;
  cTempParam->cProcMem = cProcMem;
  cTempParam->cCallStateCont = cCallStateCont;
  cTempParam->sCustomValue.nFundType = sCustomValue.nFundType;
  cTempParam->sCustomValue.nSize = sCustomValue.nSize;
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
  {
    if (sCustomValue.nSize > 0)
    {
      if (sCustomValue.nSize <= NKT_DV_ARRAYLEN(sCustomValue.aShortValue))
      {
        cTempParam->lpAddress = cTempParam->sCustomValue.aShortValue;
      }
      else
      {
        cTempParam->sCustomValue.cLongValue.Attach((LPBYTE)nktMemMalloc(sCustomValue.nSize));
        if (cTempParam->sCustomValue.cLongValue == NULL)
          return NULL;
        cTempParam->lpAddress = cTempParam->sCustomValue.cLongValue.Get();
      }
      nktMemCopy(cTempParam->lpAddress, lpAddress, sCustomValue.nSize);
    }
    else
      cTempParam->lpAddress = NULL;
  }
  cTempParam->Init2();
  //done
  *lplpParam = cTempParam.Detach();
  return S_OK;
}

HRESULT CNktDvParam::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvParam") == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

VOID CNktDvParam::SpyMgr_Init(__in ENktDvMiscEnums::eNktDvRegister _nAsmRegisterIndex,
                              __in LPVOID _lpAddress, __in CCallStateContainer *lpCallStateCont)
{
  cCallStateCont = lpCallStateCont;
  bReadOnly = FALSE;
  nEnvironment = CNktDvParam::eEnvSpyMgr;
  nInternalAsmRegisterIndex = _nAsmRegisterIndex;
  lpAddress = _lpAddress;
  Init2();
  return;
}

HRESULT CNktDvParam::SpyMgr_Init4Custom(__in CNktDvProcessMemory *lpProcMem, __in int nFundType,
                                        __in LPWSTR _szNameW, __in SIZE_T nNameLen, __in LPVOID lpSrc,
                                        __in SIZE_T nSrcSize, __in CCallStateContainer *lpCallStateCont)
{
  NKT_ASSERT(_szNameW != NULL);
  NKT_ASSERT(lpProcMem != NULL);
  cProcMem = lpProcMem;
  cCallStateCont = lpCallStateCont;
  nEnvironment = CNktDvParam::eEnvSpyMgr;
  bReadOnly = TRUE;
  nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  if (sCustomValue.cStrNameW.CopyN(_szNameW, nNameLen) == FALSE)
    return E_OUTOFMEMORY;
  szNameW = (LPWSTR)(sCustomValue.cStrNameW);
  sCustomValue.nFundType = (CNktDvDbObjectNoRef::eFundamentalType)nFundType;
  sCustomValue.nSize = nSrcSize;
  if (nSrcSize > 0)
  {
    if (nSrcSize <= NKT_DV_ARRAYLEN(sCustomValue.aShortValue))
    {
      lpAddress = sCustomValue.aShortValue;
    }
    else
    {
      sCustomValue.cLongValue.Attach((LPBYTE)nktMemMalloc(nSrcSize));
      if (sCustomValue.cLongValue == NULL)
        return E_OUTOFMEMORY;
      lpAddress = sCustomValue.cLongValue.Get();
    }
    nktMemCopy(lpAddress, lpSrc, nSrcSize);
  }
  else
  {
    lpAddress = NULL;
  }
  Init2();
  return S_OK;
}

VOID CNktDvParam::Agent_Init()
{
  cCallStateCont = NULL;
  bReadOnly = FALSE;
  nEnvironment = CNktDvParam::eEnvAgent;
  Init2();
  return;
}

HRESULT CNktDvParam::Agent_Init4Custom(__in CNktDvProcessMemory *lpProcMem, __in int nFundType,
                                       __in LPCWSTR _szNameW, __in LPVOID lpSrc, __in SIZE_T nSrcSize)
{
  if (_szNameW == NULL)
    return E_POINTER;
  NKT_ASSERT(lpProcMem != NULL);
  cProcMem = lpProcMem;
  cCallStateCont = NULL;
  nEnvironment = CNktDvParam::eEnvAgent;
  bReadOnly = TRUE;
  nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  sCustomValue.nFundType = (CNktDvDbObjectNoRef::eFundamentalType)nFundType;
  sCustomValue.nSize = nSrcSize;
  if (sCustomValue.cStrNameW.Copy(_szNameW) == FALSE)
    return E_OUTOFMEMORY;
  szNameW = (LPWSTR)(sCustomValue.cStrNameW);
  if (nSrcSize > 0)
  {
    if (nSrcSize <= NKT_DV_ARRAYLEN(sCustomValue.aShortValue))
    {
      lpAddress = sCustomValue.aShortValue;
    }
    else
    {
      sCustomValue.cLongValue.Attach((LPBYTE)nktMemMalloc(nSrcSize));
      if (sCustomValue.cLongValue == NULL)
        return E_OUTOFMEMORY;
      lpAddress = sCustomValue.cLongValue.Get();
    }
    nktMemCopy(lpAddress, lpSrc, nSrcSize);
  }
  else
  {
    lpAddress = NULL;
  }
  Init2();
  return S_OK;
}

HRESULT CNktDvParam::User_Init(__in CNktDvProcessMemory *lpProcMem, __in LPVOID _lpAddress,
                               __in_z LPCWSTR szTypeW)
{
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  HRESULT hRes;

  if (szTypeW == NULL)
    return E_POINTER;
  if (szTypeW[0] == 0)
    return E_INVALIDARG;
  hRes = CNktDvEngDatabase::Get(&cDvDB, NULL, nPlatformBits);
  if (FAILED(hRes))
    return hRes;
  lpDbObj = cDvDB->FindObjectByName(szTypeW);
  if (lpDbObj == NULL)
    return NKT_DVERR_NotFound;
  lpDbParentObj = NULL;
  NKT_ASSERT(lpProcMem != NULL);
  cProcMem = lpProcMem;
  cCallStateCont = NULL;
  nEnvironment = CNktDvParam::eEnvUserDefined;
  szDeclW = lpDbObj->GetDeclaration();
  if (szDeclW == NULL)
    return E_OUTOFMEMORY;
  szTypeNameW = lpDbObj->GetName();
  NKT_ASSERT(szTypeNameW != NULL);
  nFlags = lpDbObj->GetFlags();
  lpAddress = _lpAddress;
  nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  bReadOnly = FALSE;
  //done
  Init2();
  return S_OK;
}

VOID CNktDvParam::Init2()
{
  if (sCustomValue.nFundType != CNktDvDbObject::ftNone)
    sFundamentalExtra.nSize = CNktDvDbObject::GetFundamentalTypeSize(sCustomValue.nFundType);
  else if (lpDbObj != NULL && lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
    sFundamentalExtra.nSize = lpDbObj->GetSize();
  else
    sFundamentalExtra.nSize = 0;
  sFundamentalExtra.nShiftLeft = 0;
  sFundamentalExtra.nMask = FUNDTYPE_MASK_FULL;
  return;
}

__inline BOOL CNktDvParam::AreFundamentalTypeCompatible(
                               __in CNktDvDbObjectNoRef::eFundamentalType nDestFundType,
                               __in CNktDvDbObjectNoRef::eFundamentalType nSrcFundType)
{
  switch (nDestFundType)
  {
    case CNktDvDbObject::ftUnsignedByte:
      return (nSrcFundType == CNktDvDbObject::ftSignedByte ||
              nSrcFundType == CNktDvDbObject::ftUnsignedByte ||
              nSrcFundType == CNktDvDbObject::ftAnsiChar) ? TRUE : FALSE;

    case CNktDvDbObject::ftUnsignedWord:
      return (nSrcFundType == CNktDvDbObject::ftSignedWord ||
              nSrcFundType == CNktDvDbObject::ftUnsignedWord ||
              nSrcFundType == CNktDvDbObject::ftWideChar) ? TRUE : FALSE;

    case CNktDvDbObject::ftUnsignedDoubleWord:
      return (nSrcFundType == CNktDvDbObject::ftSignedDoubleWord ||
              nSrcFundType == CNktDvDbObject::ftUnsignedDoubleWord) ? TRUE : FALSE;

    case CNktDvDbObject::ftUnsignedQuadWord:
      return (nSrcFundType == CNktDvDbObject::ftSignedQuadWord ||
              nSrcFundType == CNktDvDbObject::ftUnsignedQuadWord) ? TRUE : FALSE;

    case CNktDvDbObject::ftDouble:
      return (nSrcFundType == CNktDvDbObject::ftFloat ||
              nSrcFundType == CNktDvDbObject::ftDouble ||
              nSrcFundType == CNktDvDbObject::ftLongDouble) ? TRUE : FALSE;
  }
  return FALSE;
}

HRESULT CNktDvParam::GetValueCommon(__out LPVOID lpValue, __in SIZE_T nIndex,
                                    __in CNktDvDbObjectNoRef::eFundamentalType nDestFundType)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  CNktDvDbObjectNoRef *lpDbChildObj;
  CNktDvDbObjectNoRef::eFundamentalType nSrcFundType;
  LPBYTE lpSrcAddr;
  HRESULT hRes;
  union {
    unsigned char n8;
    unsigned short n16;
    unsigned long n32;
    unsigned __int64 n64;
    float nFlt;
    long double nLngDbl;
  } uTemp;

  if (lpValue == NULL)
    return E_POINTER;
  if (!IS_PARAM_ACCESS_ALLOWED)
  {
    hRes = E_FAIL; //param access not allowed
    goto gvc_end;
  }
  if ((lpSrcAddr = (LPBYTE)lpAddress) == NULL)
  {
    hRes = E_FAIL; //internal pointer is null
    goto gvc_end;
  }
  if (sCustomValue.nFundType != 0)
  {
    nSrcFundType = sCustomValue.nFundType;
    if (AreFundamentalTypeCompatible(nDestFundType, nSrcFundType) == FALSE)
    {
      hRes = E_FAIL;
      goto gvc_end;
    }
    if (nIndex >= (sCustomValue.nSize / (sFundamentalExtra.nSize >> 3)))
    {
      hRes = E_INVALIDARG;
      goto gvc_end;
    }
    lpSrcAddr += nIndex * (sFundamentalExtra.nSize >> 3);
  }
  else
  {
    //check if we are dealing with a pointer, reference or array
    switch (lpDbObj->GetClass())
    {
      case CNktDvDbObjectNoRef::clsPointer:
      case CNktDvDbObjectNoRef::clsReference:
        lpDbChildObj = lpDbObj->GetItem(0);
        if (lpDbChildObj == NULL)
        {
          hRes = E_FAIL;
          goto gvc_end;
        }
        //check if pointed-to object is fundamental and compatible
        nSrcFundType = lpDbChildObj->GetFundamentalType();
        if (AreFundamentalTypeCompatible(nDestFundType, nSrcFundType) == FALSE)
        {
          hRes = E_FAIL;
          goto gvc_end;
        }
        hRes = ReadPointer(&lpSrcAddr, lpAddress);
        if (FAILED(hRes))
          goto gvc_end;
        if (lpSrcAddr == NULL)
        {
          hRes = E_FAIL; //internal pointer is null
          goto gvc_end;
        }
        break;

      case CNktDvDbObjectNoRef::clsArray:
        lpDbChildObj = lpDbObj->GetItem(0);
        if (lpDbChildObj == NULL)
        {
          hRes = E_FAIL;
          goto gvc_end;
        }
        nSrcFundType = lpDbChildObj->GetFundamentalType();
        if (AreFundamentalTypeCompatible(nDestFundType, nSrcFundType) == FALSE)
        {
          hRes = E_FAIL;
          goto gvc_end;
        }
        break;

      case CNktDvDbObjectNoRef::clsFundamental:
      case CNktDvDbObjectNoRef::clsEnumeration:
        nSrcFundType = lpDbObj->GetFundamentalType();
        if (AreFundamentalTypeCompatible(nDestFundType, nSrcFundType) == FALSE)
        {
          hRes = E_FAIL;
          goto gvc_end;
        }
        break;

      default:
        hRes = E_FAIL; //internal pointer is null
        goto gvc_end;
    }
    lpSrcAddr += nIndex * (CNktDvDbObject::GetFundamentalTypeSize(nSrcFundType) >> 3);
  }
  switch (nSrcFundType)
  {
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
    case CNktDvDbObject::ftAnsiChar:
      hRes = ReadMemoryBytes(&uTemp.n8, lpSrcAddr, sizeof(unsigned char),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
      {
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
        {
          uTemp.n8 &= (unsigned char)(sFundamentalExtra.nMask);
          uTemp.n8 >>= sFundamentalExtra.nShiftLeft;
        }
        *((unsigned char*)lpValue) = uTemp.n8;
      }
      break;

    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftWideChar:
      hRes = ReadMemoryBytes(&uTemp.n16, lpSrcAddr, sizeof(unsigned short),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
      {
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
        {
          uTemp.n16 &= (unsigned short)(sFundamentalExtra.nMask);
          uTemp.n16 >>= sFundamentalExtra.nShiftLeft;
        }
        *((unsigned short*)lpValue) = uTemp.n16;
      }
      break;

    case CNktDvDbObject::ftSignedDoubleWord:
    case CNktDvDbObject::ftUnsignedDoubleWord:
      hRes = ReadMemoryBytes(&uTemp.n32, lpSrcAddr, sizeof(unsigned long),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
      {
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
        {
          uTemp.n32 &= (unsigned long)(sFundamentalExtra.nMask);
          uTemp.n32 >>= sFundamentalExtra.nShiftLeft;
        }
        *((unsigned long*)lpValue) = uTemp.n32;
      }
      break;

    case CNktDvDbObject::ftSignedQuadWord:
    case CNktDvDbObject::ftUnsignedQuadWord:
      hRes = ReadMemoryBytes(&uTemp.n64, lpSrcAddr, sizeof(unsigned __int64),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
      {
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental)
        {
          uTemp.n64 &= (unsigned __int64)(sFundamentalExtra.nMask);
          uTemp.n64 >>= sFundamentalExtra.nShiftLeft;
        }
        *((unsigned __int64*)lpValue) = uTemp.n64;
      }
      break;

    case CNktDvDbObject::ftFloat:
      hRes = ReadMemoryBytes(&uTemp.nFlt, lpSrcAddr, sizeof(float),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
        *((double*)lpValue) = (double)(uTemp.nFlt);
      break;

    case CNktDvDbObject::ftDouble:
      hRes = ReadMemoryBytes(lpValue, lpSrcAddr, sizeof(double),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      break;

    case CNktDvDbObject::ftLongDouble:
      hRes = ReadMemoryBytes(&uTemp.nLngDbl, lpSrcAddr, sizeof(long double),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (SUCCEEDED(hRes))
        *((double*)lpValue) = (double)(uTemp.nLngDbl);
      break;

    default:
      hRes = E_FAIL;
      break;
  }
gvc_end:
  if (FAILED(hRes))
  {
    switch (nDestFundType)
    {
      case CNktDvDbObject::ftUnsignedByte:
        *((BYTE*)lpValue) = 0;
        break;
      case CNktDvDbObject::ftUnsignedWord:
        *((WORD*)lpValue) = 0;
        break;
      case CNktDvDbObject::ftUnsignedDoubleWord:
        *((ULONG*)lpValue) = 0;
        break;
      case CNktDvDbObject::ftUnsignedQuadWord:
        *((ULONGLONG*)lpValue) = 0;
        break;
      case CNktDvDbObject::ftDouble:
        *((double*)lpValue) = 0.0;
        break;
      default:
        NKT_ASSERT(FALSE);
        break;
    }
  }
  return hRes;
}

HRESULT CNktDvParam::SetValueCommon(__in LPVOID lpValue, __in SIZE_T nIndex,
                                    __in CNktDvDbObjectNoRef::eFundamentalType nSrcFundType)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  CNktDvDbObjectNoRef *lpDbChildObj;
  CNktDvDbObjectNoRef::eFundamentalType nDestFundType;
  LPBYTE lpDestAddr;
  HRESULT hRes;
  union {
    unsigned char n8;
    unsigned short n16;
    unsigned long n32;
    unsigned __int64 n64;
    float nFlt;
    long double nLngDbl;
  } uTemp, uTemp2;

  if (lpValue == NULL)
    return E_POINTER;
  if (bReadOnly != FALSE || (!IS_PARAM_ACCESS_ALLOWED)) //read only or param access not allowed
    return E_FAIL;
  if ((lpDestAddr = (LPBYTE)lpAddress) == NULL)
    return E_FAIL; //internal pointer is null
  if (sCustomValue.nFundType != 0)
    return E_FAIL; //custom values are not writable
  //check if we are dealing with a pointer, reference or array
  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
    case CNktDvDbObjectNoRef::clsReference:
      lpDbChildObj = lpDbObj->GetItem(0);
      if (lpDbChildObj == NULL)
        return E_FAIL;
      //check if pointed-to object is fundamental and compatible
      nDestFundType = lpDbChildObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible(nSrcFundType, nDestFundType) == FALSE) //rev. check is intentional
        return E_FAIL;
      hRes = ReadPointer(&lpDestAddr, lpAddress);
      if (FAILED(hRes))
        return hRes;
      if (lpDestAddr == NULL)
        return E_FAIL; //internal pointer is null
      break;

    case CNktDvDbObjectNoRef::clsArray:
      lpDbChildObj = lpDbObj->GetItem(0);
      if (lpDbChildObj == NULL)
        return E_FAIL;
      nDestFundType = lpDbChildObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible(nSrcFundType, nDestFundType) == FALSE) //rev. check is intentional
        return E_FAIL;
      break;

    case CNktDvDbObjectNoRef::clsFundamental:
    case CNktDvDbObjectNoRef::clsEnumeration:
      nDestFundType = lpDbObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible(nSrcFundType, nDestFundType) == FALSE) //rev. check is intentional
        return E_FAIL;
      break;

    default:
      return E_FAIL;
  }
  lpDestAddr += nIndex * (CNktDvDbObject::GetFundamentalTypeSize(nDestFundType) >> 3);
  switch (nDestFundType)
  {
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
    case CNktDvDbObject::ftAnsiChar:
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
          sFundamentalExtra.nMask != FUNDTYPE_MASK_FULL)
      {
        //slow way because the bit-field
        uTemp.n8 = (*((unsigned char*)lpValue)) << sFundamentalExtra.nShiftLeft;
        uTemp.n8 &= (unsigned char)(sFundamentalExtra.nMask);
        hRes = ReadMemoryBytes(&uTemp2.n8, lpDestAddr, sizeof(unsigned char), FALSE);
        if (SUCCEEDED(hRes))
        {
          uTemp.n8 |= (uTemp2.n8 & (~(unsigned char)(sFundamentalExtra.nMask)));
          hRes = WriteMemoryBytes(lpDestAddr, &uTemp.n8, sizeof(unsigned char));
        }
      }
      else
      {
        hRes = WriteMemoryBytes(lpDestAddr, lpValue, sizeof(unsigned char));
      }
      break;

    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftWideChar:
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
          sFundamentalExtra.nMask != FUNDTYPE_MASK_FULL)
      {
        //slow way because the bit-field
        uTemp.n16 = (*((unsigned short*)lpValue)) << sFundamentalExtra.nShiftLeft;
        uTemp.n16 &= (unsigned short)(sFundamentalExtra.nMask);
        hRes = ReadMemoryBytes(&uTemp2.n16, lpDestAddr, sizeof(unsigned short), FALSE);
        if (SUCCEEDED(hRes))
        {
          uTemp.n16 |= (uTemp2.n16 & (~(unsigned short)(sFundamentalExtra.nMask)));
          hRes = WriteMemoryBytes(lpDestAddr, &uTemp.n16, sizeof(unsigned short));
        }
      }
      else
      {
        hRes = WriteMemoryBytes(lpDestAddr, lpValue, sizeof(unsigned short));
      }
      break;

    case CNktDvDbObject::ftSignedDoubleWord:
    case CNktDvDbObject::ftUnsignedDoubleWord:
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
          sFundamentalExtra.nMask != FUNDTYPE_MASK_FULL)
      {
        //slow way because the bit-field
        uTemp.n32 = (*((unsigned long*)lpValue)) << sFundamentalExtra.nShiftLeft;
        uTemp.n32 &= (unsigned long)(sFundamentalExtra.nMask);
        hRes = ReadMemoryBytes(&uTemp2.n32, lpDestAddr, sizeof(unsigned long), FALSE);
        if (SUCCEEDED(hRes))
        {
          uTemp.n32 |= (uTemp2.n32 & (~(unsigned long)(sFundamentalExtra.nMask)));
          hRes = WriteMemoryBytes(lpDestAddr, &uTemp.n32, sizeof(unsigned long));
        }
      }
      else
      {
        hRes = WriteMemoryBytes(lpDestAddr, lpValue, sizeof(unsigned long));
      }
      break;

    case CNktDvDbObject::ftSignedQuadWord:
    case CNktDvDbObject::ftUnsignedQuadWord:
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
          sFundamentalExtra.nMask != FUNDTYPE_MASK_FULL)
      {
        //slow way because the bit-field
        uTemp.n64 = (*((unsigned __int64*)lpValue)) << sFundamentalExtra.nShiftLeft;
        uTemp.n64 &= (unsigned __int64)(sFundamentalExtra.nMask);
        hRes = ReadMemoryBytes(&uTemp2.n64, lpDestAddr, sizeof(unsigned __int64), FALSE);
        if (SUCCEEDED(hRes))
        {
          uTemp.n64 |= (uTemp2.n64 & (~(unsigned __int64)(sFundamentalExtra.nMask)));
          hRes = WriteMemoryBytes(lpDestAddr, &uTemp.n64, sizeof(unsigned __int64));
        }
      }
      else
      {
        hRes = WriteMemoryBytes(lpDestAddr, lpValue, sizeof(unsigned __int64));
      }
      break;

    case CNktDvDbObject::ftFloat:
      uTemp.nFlt = (float)(*((double*)lpValue));
      hRes = WriteMemoryBytes(lpDestAddr, &uTemp.nFlt, sizeof(float));
      break;

    case CNktDvDbObject::ftDouble:
      hRes = WriteMemoryBytes(lpDestAddr, lpValue, sizeof(double));
      break;

    case CNktDvDbObject::ftLongDouble:
      uTemp.nLngDbl = (long double)(*((double*)lpValue));
      hRes = WriteMemoryBytes(lpDestAddr, &uTemp.nLngDbl, sizeof(long double));
      break;

    default:
      hRes = E_FAIL;
      break;
  }
  return hRes;
}

HRESULT CNktDvParam::GetStringCommon(__out LPVOID *lpszValue, __out SIZE_T *lpnLength, __in SIZE_T nStart,
                                     __in SIZE_T nMaxLength, __in BOOL bIsWide)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  CNktDvDbObjectNoRef *lpDbChildObj;
  CNktDvDbObjectNoRef::eFundamentalType nSrcFundType;
  LPBYTE lpSrcAddr;
  union {
    CHAR a[32];
    WCHAR w[32];
  } uTempBuf;
  union {
    LPSTR a;
    LPWSTR w;
  } uTempStr;
  SIZE_T k, nChunkSize, nArrayMax, nTempLength;
  HRESULT hRes;

  if (lpszValue == NULL && lpnLength == NULL)
    return E_POINTER;
  if (!IS_PARAM_ACCESS_ALLOWED)
  {
    hRes = E_FAIL; //param access not allowed
    goto gsc_end;
  }
  if ((lpSrcAddr = (LPBYTE)lpAddress) == NULL)
  {
    hRes = E_FAIL; //internal pointer is null
    goto gsc_end;
  }
  switch (sCustomValue.nFundType)
  {
    case CNktDvDbObject::ftUnsignedByte:
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftAnsiChar:
      if (bIsWide != FALSE)
      {
        hRes = E_FAIL;
        goto gsc_end;
      }
      nArrayMax = sCustomValue.nSize / (sFundamentalExtra.nSize >> 3);
      break;

    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftWideChar:
      if (bIsWide == FALSE)
      {
        hRes = E_FAIL;
        goto gsc_end;
      }
      nArrayMax = sCustomValue.nSize / (sFundamentalExtra.nSize >> 3);
      break;

    case CNktDvDbObject::ftNone:
      switch (lpDbObj->GetClass())
      {
        case CNktDvDbObjectNoRef::clsPointer:
          lpDbChildObj = lpDbObj->GetItem(0);
          if (lpDbChildObj == NULL)
          {
            hRes = E_FAIL;
            goto gsc_end;
          }
          //check if pointed-to object is fundamental and compatible
          nSrcFundType = lpDbChildObj->GetFundamentalType();
          if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                                CNktDvDbObject::ftUnsignedWord,
                                           nSrcFundType) == FALSE)
          {
            hRes = E_FAIL;
            goto gsc_end;
          }
          hRes = ReadPointer(&lpSrcAddr, lpAddress);
          if (FAILED(hRes))
            goto gsc_end;
          if (lpSrcAddr == NULL)
          {
            hRes = E_FAIL; //internal pointer is null
            goto gsc_end;
          }
          nArrayMax = NKT_SIZE_T_MAX;
          break;

        case CNktDvDbObjectNoRef::clsArray:
          lpDbChildObj = lpDbObj->GetItem(0);
          if (lpDbChildObj == NULL)
          {
            hRes = E_FAIL;
            goto gsc_end;
          }
          nSrcFundType = lpDbChildObj->GetFundamentalType();
          if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                                CNktDvDbObject::ftUnsignedWord,
                                           nSrcFundType) == FALSE)
          {
            hRes = E_FAIL;
            goto gsc_end;
          }
          if ((nArrayMax = lpDbObj->GetArrayMaxHint()) < 2) //ignore arrays of 0 and 1 elements
            nArrayMax = NKT_SIZE_T_MAX;
          break;

        case CNktDvDbObjectNoRef::clsFundamental:
          nSrcFundType = lpDbObj->GetFundamentalType();
          if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                                CNktDvDbObject::ftUnsignedWord,
                                           nSrcFundType) == FALSE)
          {
            hRes = E_FAIL;
            goto gsc_end;
          }
          nArrayMax = 1;
          break;

        default:
          hRes = E_FAIL;
          goto gsc_end;
      }
      break;

    default:
      hRes = E_FAIL;
      goto gsc_end;
  }
  //we should calculate the string length
  nTempLength = 0;
  if (bIsWide == FALSE)
  {
    uTempStr.a = (LPSTR)lpSrcAddr;
    nChunkSize = NKT_DV_ARRAYLEN(uTempBuf.a); //first try in 32-chars chunks
    while (nTempLength < nArrayMax)
    {
      if (nChunkSize > nArrayMax-nTempLength)
        nChunkSize = nArrayMax - nTempLength;
      hRes = ReadMemoryBytes(uTempBuf.a, uTempStr.a, nChunkSize*sizeof(uTempBuf.a[0]),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (FAILED(hRes))
      {
        if (nChunkSize > 1)
        {
          nChunkSize = 1; //drop to 1-char chunks
          continue;
        }
        goto gsc_end;
      }
      for (k=0; k<nChunkSize && uTempBuf.a[k]!=0; k++);
      uTempStr.a += k;
      nTempLength += k;
      if (k < nChunkSize)
        break; //got end of string
    }
  }
  else
  {
    uTempStr.w = (LPWSTR)lpSrcAddr;
    nChunkSize = NKT_DV_ARRAYLEN(uTempBuf.w); //first try in 32-chars chunks
    while (nTempLength < nArrayMax)
    {
      if (nChunkSize > nArrayMax-nTempLength)
        nChunkSize = nArrayMax - nTempLength;
      hRes = ReadMemoryBytes(uTempBuf.w, uTempStr.w, nChunkSize*sizeof(uTempBuf.w[0]),
                             (sCustomValue.nFundType != 0) ? TRUE : FALSE);
      if (FAILED(hRes))
      {
        if (nChunkSize > 1)
        {
          nChunkSize = 1; //drop to 1-char chunks
          continue;
        }
        goto gsc_end;
      }
      for (k=0; k<nChunkSize && uTempBuf.w[k]!=0; k++);
      uTempStr.w += k;
      nTempLength += k;
      if (k < nChunkSize)
        break; //got end of string
    }
  }
  if (lpnLength != NULL)
    *lpnLength = nTempLength;
  //we have the length, now get the real string
  if (lpszValue != NULL)
  {
    if (nStart > nTempLength)
      nStart = nTempLength;
    if (nMaxLength > nTempLength)
      nMaxLength = nTempLength;
    if (nStart >= nTempLength-nMaxLength)
      nMaxLength = nTempLength - nStart;
    if (nMaxLength > 0)
    {
      if (bIsWide == FALSE)
      {
        uTempStr.a = (LPSTR)nktMemMalloc((nMaxLength+1)*sizeof(CHAR));
        if (uTempStr.a == NULL)
        {
          hRes = E_OUTOFMEMORY;
          goto gsc_end;
        }
        hRes = ReadMemoryBytes(uTempStr.a, (LPSTR)lpSrcAddr + nStart, nMaxLength*sizeof(CHAR),
                               (sCustomValue.nFundType != 0) ? TRUE : FALSE);
        if (FAILED(hRes))
        {
          nktMemFree(uTempStr.a);
          goto gsc_end;
        }
        uTempStr.a[nMaxLength] = 0;
        *lpszValue = uTempStr.a;
      }
      else
      {
        uTempStr.w = (LPWSTR)nktMemMalloc((nMaxLength+1)*sizeof(WCHAR));
        if (uTempStr.w == NULL)
        {
          hRes = E_OUTOFMEMORY;
          goto gsc_end;
        }
        hRes = ReadMemoryBytes(uTempStr.w, (LPWSTR)lpSrcAddr + nStart, nMaxLength*sizeof(WCHAR),
                               (sCustomValue.nFundType != 0) ? TRUE : FALSE);
        if (FAILED(hRes))
        {
          nktMemFree(uTempStr.w);
          goto gsc_end;
        }
        uTempStr.w[nMaxLength] = 0;
        *lpszValue = uTempStr.w;
      }

    }
    else
    {
      if (bIsWide == FALSE)
      {
        *lpszValue = uTempStr.a = (LPSTR)nktMemMalloc(1*sizeof(CHAR));
        if (uTempStr.a == NULL)
        {
          hRes = E_OUTOFMEMORY;
          goto gsc_end;
        }
        uTempStr.a[0] = 0;
      }
      else
      {
        *lpszValue = uTempStr.w = (LPWSTR)nktMemMalloc(1*sizeof(WCHAR));
        if (uTempStr.w == NULL)
        {
          hRes = E_OUTOFMEMORY;
          goto gsc_end;
        }
        uTempStr.w[0] = 0;
      }
    }
  }
  hRes = S_OK;

gsc_end:
  if (FAILED(hRes))
  {
    if (lpszValue != NULL)
      *lpszValue = NULL;
    if (lpnLength != NULL)
      *lpnLength = 0;
  }
  return hRes;
}

HRESULT CNktDvParam::SetStringCommon(__in LPVOID szValue, __in SIZE_T nStart, __in BOOL bIncludeEOS,
                                     __in BOOL bIsWide)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  CNktDvDbObjectNoRef *lpDbChildObj;
  CNktDvDbObjectNoRef::eFundamentalType nDestFundType;
  LPBYTE lpDestAddr;
  SIZE_T nArrayMax, nSrcLength;
  HRESULT hRes;

  if (szValue == NULL)
    return E_POINTER;
  if (bReadOnly != FALSE || (!IS_PARAM_ACCESS_ALLOWED)) //read only or param access not allowed
    return E_FAIL;
  if ((lpDestAddr = (LPBYTE)lpAddress) == NULL)
    return E_FAIL; //internal pointer is null
  if (sCustomValue.nFundType != 0)
    return E_FAIL; //custom values are not writable
  //check if we are dealing with a pointer, reference or array
  switch (lpDbObj->GetClass())
  {
    case CNktDvDbObjectNoRef::clsPointer:
      lpDbChildObj = lpDbObj->GetItem(0);
      if (lpDbChildObj == NULL)
        return E_FAIL;
      //check if pointed-to object is fundamental and compatible
      nDestFundType = lpDbChildObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                            CNktDvDbObject::ftUnsignedWord,
                                       nDestFundType) == FALSE)
        return E_FAIL;
      hRes = ReadPointer(&lpDestAddr, lpAddress);
      if (FAILED(hRes))
        return hRes;
      if (lpDestAddr == NULL)
        return E_FAIL; //internal pointer is null
      nArrayMax = NKT_SIZE_T_MAX;
      break;

    case CNktDvDbObjectNoRef::clsArray:
      lpDbChildObj = lpDbObj->GetItem(0);
      if (lpDbChildObj == NULL)
        return E_FAIL;
      nDestFundType = lpDbChildObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                            CNktDvDbObject::ftUnsignedWord,
                                       nDestFundType) == FALSE)
        return E_FAIL;
      if ((nArrayMax = lpDbObj->GetArrayMaxHint()) < 2) //ignore arrays of 0 and 1 elements
        nArrayMax = NKT_SIZE_T_MAX;
      break;

    case CNktDvDbObjectNoRef::clsFundamental:
      nDestFundType = lpDbObj->GetFundamentalType();
      if (AreFundamentalTypeCompatible((bIsWide == FALSE) ? CNktDvDbObject::ftUnsignedByte :
                                                            CNktDvDbObject::ftUnsignedWord,
                                       nDestFundType) == FALSE)
        return E_FAIL;
      nArrayMax = 1;
      break;

    default:
      return E_FAIL;
  }
  lpDestAddr += nStart * (CNktDvDbObject::GetFundamentalTypeSize(nDestFundType) >> 3);
  nSrcLength = (bIsWide == FALSE) ? strlen((LPSTR)szValue) : wcslen((LPWSTR)szValue);
  if (bIncludeEOS != FALSE)
    nSrcLength++;
  if (nStart > nArrayMax)
      nStart = nArrayMax;
  if (nSrcLength > nArrayMax)
    nSrcLength = nArrayMax;
  if (nStart >= nArrayMax-nSrcLength)
    nSrcLength = nArrayMax - nStart;
  switch (nDestFundType)
  {
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
    case CNktDvDbObject::ftAnsiChar:
      if (nSrcLength > 0)
        hRes = WriteMemoryBytes(lpDestAddr, szValue, nSrcLength*sizeof(CHAR));
      else
        hRes = S_OK;
      break;

    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftWideChar:
      if (nSrcLength > 0)
        hRes = WriteMemoryBytes(lpDestAddr, szValue, nSrcLength*sizeof(WCHAR));
      else
        hRes = S_OK;
      break;

    default:
      hRes = E_FAIL;
      break;
  }
  return hRes;
}

__inline HRESULT CNktDvParam::ReadMemoryBytes(__out LPVOID lpDest, __in LPCVOID lpSrc, __in SIZE_T nCount,
                                              __in BOOL bForceLocal)
{
  if (bForceLocal != FALSE)
  {
    SIZE_T nCopied;

    nCopied = nktTryMemCopy(lpDest, lpSrc, nCount);
    if (nCopied == nCount)
      return S_OK;
    return (nCopied > 0) ? NKT_DVERR_PartialCopy : E_ACCESSDENIED;
  }
  NKT_ASSERT(cProcMem != NULL);
  return cProcMem->Read(lpDest, lpSrc, nCount);
}

__inline HRESULT CNktDvParam::WriteMemoryBytes(__out LPVOID lpDest, __in LPCVOID lpSrc,
                                               __in SIZE_T nCount)
{
  NKT_ASSERT(cProcMem != NULL);
  return cProcMem->Write(lpDest, lpSrc, nCount);
}

HRESULT CNktDvParam::ReadPointer(__out LPBYTE *lplpDest, __in LPVOID lpAddress)
{
  union {
    ULONG nValue32;
    ULONGLONG nValue64;
  };
  HRESULT hRes;

  NKT_ASSERT(lplpDest != NULL);
  switch (nPlatformBits)
  {
    case 32:
      hRes = ReadMemoryBytes(&nValue32, lpAddress, sizeof(nValue32), FALSE);
      *lplpDest = (SUCCEEDED(hRes)) ? (LPBYTE)((ULONG_PTR)nValue32) : NULL;
      break;
#if defined _M_X64
    case 64:
      hRes = ReadMemoryBytes(&nValue64, lpAddress, sizeof(nValue64), FALSE);
      *lplpDest = (SUCCEEDED(hRes)) ? (LPBYTE)nValue64 : NULL;
      break;
#endif //_M_X64
    default:
      hRes = E_FAIL;
      *lplpDest = NULL;
      break;
  }
  return hRes;
}

CNktDvParam* CNktDvParam::Clone()
{
  TNktComPtr<CNktDvParam> cTempParam;

  cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
  if (cTempParam == NULL)
    return NULL;
  cTempParam->szDeclW = szDeclW;
  cTempParam->szTypeNameW = szTypeNameW;
  cTempParam->nFlags = nFlags;
  cTempParam->lpAddress = lpAddress;
  cTempParam->nInternalAsmRegisterIndex = nInternalAsmRegisterIndex;
  cTempParam->bReadOnly = bReadOnly;
  cTempParam->nEnvironment = nEnvironment;
  cTempParam->lpDbObj = lpDbObj;
  cTempParam->lpDbParentObj = NULL;
  cTempParam->cProcMem = cProcMem;
  cTempParam->sCustomValue.nFundType = sCustomValue.nFundType;
  if (sCustomValue.nFundType == CNktDvDbObject::ftNone)
  {
    cTempParam->sCustomValue.nSize = 0;
    cTempParam->szNameW = szNameW;
  }
  else
  {
    cTempParam->sCustomValue.nSize = sCustomValue.nSize;
    if (cTempParam->sCustomValue.cStrNameW.Copy((LPWSTR)sCustomValue.cStrNameW) == FALSE)
      return NULL;
    cTempParam->szNameW = (LPWSTR)(cTempParam->sCustomValue.cStrNameW);
    if (sCustomValue.nSize <= NKT_DV_ARRAYLEN(sCustomValue.aShortValue))
    {
      cTempParam->lpAddress = cTempParam->sCustomValue.aShortValue;
    }
    else
    {
      cTempParam->sCustomValue.cLongValue.Attach((LPBYTE)nktMemMalloc(sCustomValue.nSize));
      if (cTempParam->sCustomValue.cLongValue == NULL)
        return NULL;
      cTempParam->lpAddress = cTempParam->sCustomValue.cLongValue.Get();
    }
    nktMemCopy(cTempParam->lpAddress, lpAddress, sCustomValue.nSize);
  }
  nktMemCopy(&(cTempParam->sFundamentalExtra), &sFundamentalExtra, sizeof(sFundamentalExtra));
  cTempParam->cCallStateCont = cCallStateCont;
  //done
  return cTempParam.Detach();
}

//-----------------------------------------------------------

static BOOL IsGuidName(__in LPCWSTR szNameW)
{
  return (wcscmp(szNameW, L"_GUID") == 0 ||
          wcscmp(szNameW, L"GUID") == 0 ||
          wcscmp(szNameW, L"IID") == 0 ||
          wcscmp(szNameW, L"CLSID") == 0 ||
          wcscmp(szNameW, L"UUID") == 0) ? TRUE : FALSE;
}
