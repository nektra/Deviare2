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

#include "StdAfx.h"
#include "ParamsEnum.h"
#include "Param.h"
#include "DbObject.h"
#include "ProcessMemory.h"
#include "..\..\Common\Tools.h"

//-----------------------------------------------------------

#define RETURN_FATAL_ERRORS                            FALSE

//-----------------------------------------------------------
// CNktParamImpl

HRESULT CNktParamImpl::get_Address(__deref_out my_ssize_t *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get address
  *pVal = (my_ssize_t)(lpIntObj->GetAddress());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_Class(__deref_out eNktDboClass *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = clsNull;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get class
  *pVal = (eNktDboClass)(lpIntObj->GetClass());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_BasicType(__deref_out eNktDboFundamentalType *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = ftNone;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get basic type
  *pVal = (eNktDboFundamentalType)(lpIntObj->GetBasicType());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_Name(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  LPWSTR sW;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get name
    sW = lpIntObj->GetName();
    hRes = S_OK;
  }
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set(sW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_TypeName(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  LPWSTR sW;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get type name
    sW = lpIntObj->GetTypeName();
    hRes = S_OK;
  }
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set(sW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_Declaration(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  LPWSTR sW;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get declaration
    sW = lpIntObj->GetDeclaration();
    hRes = S_OK;
  }
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set(sW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_Flags(__deref_out LONG *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get flags
  *pVal = (LONG)(lpIntObj->GetFlags());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_IsPointer(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is pointer"
  *pVal = VariantBool_From_Bool(lpIntObj->IsPointer());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_IsArray(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is array"
  *pVal = VariantBool_From_Bool(lpIntObj->IsArray());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_ArrayMaxHint(__deref_out LONG *pVal)
{
  HRESULT hRes;
  SIZE_T nCount;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "max hint"
  nCount = lpIntObj->GetArrayMaxHint();
  *pVal = (nCount <= LONG_MAX) ? (LONG)nCount : LONG_MAX;
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_IsAnsiString(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is ansi string"
  *pVal = VariantBool_From_Bool(lpIntObj->IsAnsiString());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_IsWideString(__deref_out VARIANT_BOOL *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_FALSE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is wide string"
  *pVal = VariantBool_From_Bool(lpIntObj->IsWideString());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::get_IntResourceString(__deref_out LONG *pVal)
{
  unsigned short nId;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is int resource"
  hRes = lpIntObj->IsIntResourceString(&nId);
  *pVal = (LONG)((ULONG)nId);
  if (hRes == S_FALSE)
    hRes = S_OK;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_GuidString(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  CNktStringW cStrTempW;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get guid string
    hRes = lpIntObj->IsGuidString(cStrTempW);
    if (hRes == S_FALSE)
      hRes = S_OK;
  }
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set((LPWSTR)cStrTempW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::Evaluate(__deref_out INktParam **ppParam)
{
  return IndexedEvaluate(0, ppParam);
}

HRESULT CNktParamImpl::IndexedEvaluate(__in LONG index, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get evaluated
    hRes = (index >= 0) ? (lpIntObj->GetEvaluated(&cParam, (SIZE_T)index)) : E_INVALIDARG;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, hRes);
  }
  //done
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::FullEvaluate(__deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get full evaluated
    hRes = lpIntObj->GetFullEvaluated(&cParam);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, hRes);
  }
  //done
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_FieldsCount(__deref_out LONG *pVal)
{
  HRESULT hRes;
  SIZE_T nCount;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get fields count
  nCount = lpIntObj->GetFieldsCount();
  *pVal = (nCount <= LONG_MAX) ? (LONG)nCount : LONG_MAX;
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktParamImpl::Field(__in VARIANT indexOrName, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  VARIANT *lpVar;
  ULONGLONG nIndex;
  BOOL bIndexSigned;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    lpVar = RealVariant_From_Variant(&indexOrName);
    if (lpVar->vt == VT_BSTR)
    {
      hRes = lpIntObj->GetField(&cParam, (LPWSTR)(lpVar->bstrVal));
    }
    else if (Numeric_From_Variant(lpVar, &nIndex, &bIndexSigned) != FALSE)
    {
      //get indexed field
      if ((bIndexSigned == FALSE || (LONGLONG)nIndex >= 0) &&
          nIndex < (ULONGLONG)(lpIntObj->GetFieldsCount()))
        hRes = lpIntObj->GetField(&cParam, (SIZE_T)nIndex);
      else
        hRes = E_INVALIDARG;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, hRes);
  }
  //done
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::Fields(__deref_out INktParamsEnum **ppParamsEnum)
{
  TNktComPtr<CNktDvParamsEnumerator> cParamsEnum;
  TNktComPtr<INktParamsEnum> cIParamsEnum;
  HRESULT hRes;

  if (ppParamsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get fields
    hRes = lpIntObj->GetFields(&cParamsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamsEnumImpl,INktParamsEnum,
                            CNktDvParamsEnumerator>(&cIParamsEnum, cParamsEnum,
                                                    InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamsEnumImpl,INktParamsEnum,
                                   CNktDvParamsEnumerator>(&cIParamsEnum, hRes);
  }
  //done
  *ppParamsEnum = cIParamsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::get_Value(__deref_out VARIANT *pVal)
{
  return get_ValueAt(0, pVal);
}

HRESULT CNktParamImpl::put_Value(__in VARIANT var)
{
  return put_ValueAt(0, var);
}

HRESULT CNktParamImpl::get_ValueAt(__in LONG index, __deref_out VARIANT *pVal)
{
  unsigned short nResId;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  ::VariantInit(pVal);
  pVal->vt = VT_NULL;
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
  //process parameter
  hRes = lpIntObj->IsIntResourceString(&nResId);
  if (SUCCEEDED(hRes) && nResId > 0)
  {
    CNktComBStr cComBStr;
    WCHAR szBufW[64];

    pVal->vt = VT_BSTR;
    swprintf_s(szBufW, NKT_DV_ARRAYLEN(szBufW), L"#%lu", (ULONG)nResId);
    hRes = cComBStr.Set(szBufW);
    if (SUCCEEDED(hRes))
      pVal->bstrVal = cComBStr.Detach();
  }
  else
  {
    //check if string
    switch (lpIntObj->GetStringType())
    {
      case CNktDvParam::stAnsi:
      case CNktDvParam::stWide:
        pVal->vt = VT_BSTR;
        hRes = ReadString(&(pVal->bstrVal));
        break;

      default:
        //check for basic types
        switch (lpIntObj->GetBasicType())
        {
          case CNktDvDbObject::ftAnsiChar:
          case CNktDvDbObject::ftSignedByte:
            pVal->vt = VT_I1;
            hRes = get_CharValAt(index, (signed char*)&(pVal->cVal));
            break;

          case CNktDvDbObject::ftUnsignedByte:
            pVal->vt = VT_UI1;
            hRes = get_ByteValAt(index, &(pVal->bVal));
            break;

          case CNktDvDbObject::ftSignedWord:
            pVal->vt = VT_I2;
            hRes = get_ShortValAt(index, &(pVal->iVal));
            break;

          case CNktDvDbObject::ftUnsignedWord:
          case CNktDvDbObject::ftWideChar:
            pVal->vt = VT_UI2;
            hRes = get_UShortValAt(index, &(pVal->uiVal));
            break;

          case CNktDvDbObject::ftSignedDoubleWord:
            pVal->vt = VT_I4;
            hRes = get_LongValAt(index, &(pVal->lVal));
            break;

          case CNktDvDbObject::ftUnsignedDoubleWord:
            pVal->vt = VT_UI4;
            hRes = get_ULongValAt(index, &(pVal->ulVal));
            break;

          case CNktDvDbObject::ftSignedQuadWord:
            pVal->vt = VT_I8;
            hRes = get_LongLongValAt(index, &(pVal->llVal));
            break;

          case CNktDvDbObject::ftUnsignedQuadWord:
            pVal->vt = VT_UI8;
            hRes = get_ULongLongValAt(index, &(pVal->ullVal));
            break;

          case CNktDvDbObject::ftFloat:
            pVal->vt = VT_R4;
            hRes = get_FloatValAt(index, &(pVal->fltVal));
            break;

          case CNktDvDbObject::ftDouble:
          case CNktDvDbObject::ftLongDouble:
            pVal->vt = VT_R8;
            hRes = get_DoubleValAt(index, &(pVal->dblVal));
            break;

          default:
            hRes = E_INVALIDARG;
            break;
        }
        break;
    }
  }
  if (FAILED(hRes))
    pVal->vt = VT_NULL;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_ValueAt(__in LONG index, __in VARIANT var)
{
  LPBYTE lpData;
  SIZE_T k, nItemsCount;
  VARIANT *lpVar;
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //write
    lpVar = RealVariant_From_Variant(&var);
    if (lpVar == NULL)
      return SetupErrorInfoAndReturn(E_POINTER);
    switch (lpVar->vt)
    {
      case VT_I1:
        hRes = put_CharValAt(index, lpVar->cVal);
        break;
      case VT_UI1:
        hRes = put_ByteValAt(index, lpVar->bVal);
        break;
      case VT_I2:
        hRes = put_ShortValAt(index, lpVar->iVal);
        break;
      case VT_UI2:
        hRes = put_UShortValAt(index, lpVar->uiVal);
        break;
      case VT_I4:
        hRes = put_LongValAt(index, lpVar->lVal);
        break;
      case VT_UI4:
        hRes = put_ULongValAt(index, lpVar->ulVal);
        break;
      case VT_INT:
        hRes = put_LongValAt(index, lpVar->intVal);
        break;
      case VT_UINT:
        hRes = put_ULongValAt(index, lpVar->uintVal);
        break;
      case VT_I8:
        hRes = put_LongLongValAt(index, lpVar->llVal);
        break;
      case VT_UI8:
        hRes = put_ULongLongValAt(index, lpVar->ullVal);
        break;
      case VT_R4:
        hRes = put_FloatValAt(index, lpVar->fltVal);
        break;
      case VT_R8:
        hRes = put_DoubleValAt(index, lpVar->dblVal);
        break;

      case VT_BSTR:
      case VT_BYREF|VT_BSTR:
        {
        BSTR bstrTemp;

        if (lpVar->vt == VT_BSTR)
          bstrTemp = lpVar->bstrVal;
        else if (lpVar->pbstrVal != NULL)
          bstrTemp = *lpVar->pbstrVal;
        else
          bstrTemp = NULL;
        if (bstrTemp == NULL)
        {
          hRes = E_POINTER;
        }
        else
        {
          switch (lpIntObj->GetStringType())
          {
            case CNktDvParam::stAnsi:
              hRes = WriteString(lpVar->bstrVal, VARIANT_TRUE, VARIANT_TRUE);
              break;
            case CNktDvParam::stWide:
              hRes = WriteString(lpVar->bstrVal, VARIANT_FALSE, VARIANT_TRUE);
              break;
            default:
              hRes = E_INVALIDARG;
              break;
          }
        }
        }
        break;

      case VT_BYREF|VT_I1:
        hRes = (lpVar->pcVal != NULL) ? put_CharValAt(index, *(lpVar->pcVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_UI1:
        hRes = (lpVar->pbVal != NULL) ? put_ByteValAt(index, *(lpVar->pbVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_I2:
        hRes = (lpVar->piVal != NULL) ? put_ShortValAt(index, *(lpVar->piVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_UI2:
        hRes = (lpVar->puiVal != NULL) ? put_UShortValAt(index, *(lpVar->puiVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_I4:
        hRes = (lpVar->plVal != NULL) ? put_LongValAt(index, *(lpVar->plVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_UI4:
        hRes = (lpVar->pulVal != NULL) ? put_ULongValAt(index, *(lpVar->pulVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_INT:
        hRes = (lpVar->pintVal != NULL) ? put_LongValAt(index, *(lpVar->pintVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_UINT:
        hRes = (lpVar->puintVal != NULL) ? put_ULongValAt(index, *(lpVar->puintVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_I8:
        hRes = (lpVar->pllVal != NULL) ? put_LongLongValAt(index, *(lpVar->pllVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_UI8:
        hRes = (lpVar->pullVal != NULL) ? put_ULongLongValAt(index, *(lpVar->pullVal)) : E_POINTER;
        break;
      case VT_BYREF|VT_R4:
        hRes = (lpVar->pfltVal != NULL) ? put_FloatValAt(index, *lpVar->pfltVal) : E_POINTER;
        break;
      case VT_BYREF|VT_R8:
        hRes = (lpVar->pdblVal != NULL) ? put_DoubleValAt(index, *(lpVar->pdblVal)) : E_POINTER;
        break;

      case VT_ARRAY|VT_I1:
      case VT_ARRAY|VT_UI1:
      case VT_ARRAY|VT_I2:
      case VT_ARRAY|VT_UI2:
      case VT_ARRAY|VT_I4:
      case VT_ARRAY|VT_UI4:
      case VT_ARRAY|VT_INT:
      case VT_ARRAY|VT_UINT:
      case VT_ARRAY|VT_I8:
      case VT_ARRAY|VT_UI8:
      case VT_ARRAY|VT_R4:
      case VT_ARRAY|VT_R8:
        if (lpVar->parray == NULL)
        {
          hRes = E_POINTER;
          break;
        }
        if (lpVar->parray->cDims != 1)
        {
          hRes = E_INVALIDARG;
          break;
        }
        nItemsCount = (SIZE_T)(lpVar->parray->rgsabound[0].cElements);
        //get array data
        hRes = ::SafeArrayAccessData(lpVar->parray, (LPVOID*)&lpData);
        if (FAILED(hRes))
          break;
        switch (lpVar->vt)
        {
          case VT_ARRAY|VT_I1:
          case VT_ARRAY|VT_UI1:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_ByteValAt(index, *((unsigned char*)lpData));
              lpData += sizeof(unsigned char);
            }
            break;

          case VT_ARRAY|VT_I2:
          case VT_ARRAY|VT_UI2:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_UShortValAt(index, *((unsigned short*)lpData));
              lpData += sizeof(unsigned short);
            }
            break;

          case VT_ARRAY|VT_I4:
          case VT_ARRAY|VT_INT:
          case VT_ARRAY|VT_UI4:
          case VT_ARRAY|VT_UINT:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_ULongValAt(index, *((unsigned long*)lpData));
              lpData += sizeof(unsigned long);
            }
            break;

          case VT_ARRAY|VT_I8:
          case VT_ARRAY|VT_UI8:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_ULongLongValAt(index, *((unsigned __int64*)lpData));
              lpData += sizeof(unsigned __int64);
            }
            break;

          case VT_ARRAY|VT_R4:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_FloatValAt(index, *((float*)lpData));
              lpData += sizeof(float);
            }
            break;

          case VT_ARRAY|VT_R8:
            for (k = 0; SUCCEEDED(hRes) && k < nItemsCount; k++,index++)
            {
              hRes = put_DoubleValAt(index, *((double*)lpData));
              lpData += sizeof(double);
            }
            break;
        }
        ::SafeArrayUnaccessData(lpVar->parray);
        break;

      default:
        hRes = E_INVALIDARG;
        break;
    }
  }
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_ValueArray(__in LONG count, __deref_out VARIANT *pVal)
{
  return get_ValueArrayAt(0, count, pVal);
}

HRESULT CNktParamImpl::get_ValueArrayAt(__in LONG index, __in LONG count, __deref_out VARIANT *pVal)
{
  TNktComPtr<CNktDvParam> cCurrParam;
  SAFEARRAYBOUND sNewBounds;
  CNktDvDbObjectNoRef::eFundamentalType nType;
  HRESULT hRes;
  LPBYTE d;
  my_ssize_t k;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  ::VariantInit(pVal);
  pVal->vt = VT_NULL;
  if (index < 0 || count < 0)
  {
    return SetupErrorInfoAndReturn(E_INVALIDARG, RETURN_FATAL_ERRORS);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    pVal->scode = hRes;
    return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
  }
  //check if pointer-to...
  if (lpIntObj->IsPointer() != FALSE)
  {
    hRes = lpIntObj->GetEvaluated(&cCurrParam, 0);
    if (FAILED(hRes))
    {
      pVal->scode = hRes;
      return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
    }
  }
  else
  {
    cCurrParam = lpIntObj;
  }
  //create array
  switch (nType = cCurrParam->GetBasicType())
  {
    case CNktDvDbObject::ftAnsiChar:
    case CNktDvDbObject::ftSignedByte:
      pVal->vt = VT_ARRAY|VT_I1;
      break;

    case CNktDvDbObject::ftUnsignedByte:
      pVal->vt = VT_ARRAY|VT_UI1;
      break;

    case CNktDvDbObject::ftSignedWord:
      pVal->vt = VT_ARRAY|VT_I2;
      break;

    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftWideChar:
      pVal->vt = VT_ARRAY|VT_UI2;
      break;

    case CNktDvDbObject::ftSignedDoubleWord:
      pVal->vt = VT_ARRAY|VT_I4;
      break;

    case CNktDvDbObject::ftUnsignedDoubleWord:
      pVal->vt = VT_ARRAY|VT_UI4;
      break;

    case CNktDvDbObject::ftSignedQuadWord:
      pVal->vt = VT_ARRAY|VT_I8;
      break;

    case CNktDvDbObject::ftUnsignedQuadWord:
      pVal->vt = VT_ARRAY|VT_UI8;
      break;

    case CNktDvDbObject::ftFloat:
      pVal->vt = VT_ARRAY|VT_R4;
      break;

    case CNktDvDbObject::ftDouble:
    case CNktDvDbObject::ftLongDouble:
      pVal->vt = VT_ARRAY|VT_R8;
      break;

    default:
      pVal->scode = E_INVALIDARG;
      return SetupErrorInfoAndReturn(E_INVALIDARG, RETURN_FATAL_ERRORS);
      break;
  }
  pVal->parray = ::SafeArrayCreateVector(pVal->vt & (~VT_ARRAY), 0, (ULONG)count);
  if (pVal->parray == NULL)
  {
    pVal->scode = E_OUTOFMEMORY;
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY, RETURN_FATAL_ERRORS);
  }
  //read data
  d = (LPBYTE)(pVal->parray->pvData);
  hRes = S_OK;
  switch (nType)
  {
    case CNktDvDbObject::ftAnsiChar:
    case CNktDvDbObject::ftSignedByte:
    case CNktDvDbObject::ftUnsignedByte:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_ByteValAt(index, (unsigned char*)d);
         d += sizeof(unsigned char);
      }
      break;

    case CNktDvDbObject::ftSignedWord:
    case CNktDvDbObject::ftUnsignedWord:
    case CNktDvDbObject::ftWideChar:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_UShortValAt(index, (unsigned short*)d);
         d += sizeof(unsigned short);
      }
      break;

    case CNktDvDbObject::ftSignedDoubleWord:
    case CNktDvDbObject::ftUnsignedDoubleWord:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_ULongValAt(index, (ULONG*)d);
         d += sizeof(ULONG);
      }
      break;

    case CNktDvDbObject::ftSignedQuadWord:
    case CNktDvDbObject::ftUnsignedQuadWord:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_ULongLongValAt(index, (ULONGLONG*)d);
         d += sizeof(ULONGLONG);
      }
      break;

    case CNktDvDbObject::ftFloat:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_FloatValAt(index, (float*)d);
         d += sizeof(float);
      }
      break;

    case CNktDvDbObject::ftDouble:
    case CNktDvDbObject::ftLongDouble:
      for (k=0; SUCCEEDED(hRes) && k < count; k++,index++)
      {
         hRes = get_DoubleValAt(index, (double*)d);
         d += sizeof(double);
      }
      break;
  }
  if (k > 0 && k < count)
    hRes = NKT_DVERR_PartialCopy;
  if (hRes == NKT_DVERR_PartialCopy || hRes == E_ACCESSDENIED)
  {
    sNewBounds.lLbound = 0;
#ifdef _WIN64
    sNewBounds.cElements = (ULONG)(ULONGLONG)k;
#else //_WIN64
    sNewBounds.cElements = (ULONG)k;
#endif
    hRes = ::SafeArrayRedim(pVal->parray, &sNewBounds);
    if (FAILED(hRes))
      goto err_dodestroy;
  }
  else if (FAILED(hRes))
  {
err_dodestroy:
    ::SafeArrayDestroy(pVal->parray);
    pVal->vt = VT_NULL;
  }
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::ReadString(__deref_out BSTR *pVal)
{
  union {
    LPVOID lpStr;
    LPSTR szDestA;
    LPWSTR szDestW;
  };
  CNktComBStr cComBStr;
  HRESULT hRes;
  BOOL bIsAnsi;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  lpStr = NULL;
  bIsAnsi = TRUE;
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //read string
    switch (lpIntObj->GetStringType())
    {
      case CNktDvParam::stAnsi:
        hRes = lpIntObj->GetAnsiString(&szDestA);
        break;
      case CNktDvParam::stWide:
        bIsAnsi = FALSE;
        hRes = lpIntObj->GetWideString(&szDestW);
        break;
      default:
        hRes = E_FAIL;
        break;
    }
  }
  if (SUCCEEDED(hRes))
  {
    if (bIsAnsi != FALSE)
      hRes = cComBStr.Set(szDestA);
    else
      hRes = cComBStr.Set(szDestW);
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  if (lpStr != NULL)
    lpIntObj->FreeString(lpStr);
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::ReadStringN(__in LONG maxChars, __deref_out BSTR *pVal)
{
  union {
    LPVOID lpStr;
    LPSTR szDestA;
    LPWSTR szDestW;
  };
  CNktComBStr cComBStr;
  HRESULT hRes;
  BOOL bIsAnsi;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  lpStr = NULL;
  bIsAnsi = TRUE;
  if (maxChars < 0)
  {
    hRes = E_INVALIDARG;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
  }
  if (SUCCEEDED(hRes))
  {
    //read string
    switch (lpIntObj->GetStringType())
    {
      case CNktDvParam::stAnsi:
        hRes = lpIntObj->GetAnsiString(&szDestA, 0, (SIZE_T)maxChars);
        break;
      case CNktDvParam::stWide:
        bIsAnsi = FALSE;
        hRes = lpIntObj->GetWideString(&szDestW, 0, (SIZE_T)maxChars);
        break;
      default:
        hRes = E_FAIL;
        break;
    }
  }
  if (SUCCEEDED(hRes))
  {
    if (bIsAnsi != FALSE)
      hRes = cComBStr.Set(szDestA);
    else
      hRes = cComBStr.Set(szDestW);
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  if (lpStr != NULL)
    lpIntObj->FreeString(lpStr);
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::WriteString(__in BSTR str, __in VARIANT_BOOL isAnsi, __in VARIANT_BOOL writeNul)
{
  HRESULT hRes;

  if (str == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    switch (lpIntObj->GetStringType())
    {
      case CNktDvParam::stAnsi:
        {
        TNktAutoFreePtr<CHAR> sA;

        sA.Attach(CNktDvTools::Wide2Ansi(str));
        hRes = (sA != NULL) ? (lpIntObj->SetAnsiString(sA, 0, Bool_From_VariantBool(writeNul))) :
                              E_OUTOFMEMORY;
        }
        break;

      case CNktDvParam::stWide:
        hRes = lpIntObj->SetWideString(str, 0, Bool_From_VariantBool(writeNul));
        break;

      default:
        hRes = E_FAIL;
        break;
    }
  }
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::StringLength(__deref_out LONG *pVal)
{
  SIZE_T nChars;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //read string length
    switch (lpIntObj->GetStringType())
    {
      case CNktDvParam::stAnsi:
        hRes = lpIntObj->GetAnsiStringLength(&nChars);
        break;

      case CNktDvParam::stWide:
        hRes = lpIntObj->GetWideStringLength(&nChars);
        break;

      default:
        hRes = E_FAIL;
        break;
    }
  }
  if (nChars > LONG_MAX)
    nChars = LONG_MAX;
  *pVal = (SUCCEEDED(hRes)) ? (LONG)nChars : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_CharVal(__deref_out signed char *pVal)
{
  return get_CharValAt(0, pVal);
}

HRESULT CNktParamImpl::put_CharVal(__in signed char newValue)
{
  return put_CharValAt(0, newValue);
}

HRESULT CNktParamImpl::get_ByteVal(__deref_out unsigned char *pVal)
{
  return get_ByteValAt(0, pVal);
}

HRESULT CNktParamImpl::put_ByteVal(__in unsigned char newValue)
{
  return put_ByteValAt(0, newValue);
}

HRESULT CNktParamImpl::get_ShortVal(__deref_out short *pVal)
{
  return get_ShortValAt(0, pVal);
}

HRESULT CNktParamImpl::put_ShortVal(__in short newValue)
{
  return put_ShortValAt(0, newValue);
}

HRESULT CNktParamImpl::get_UShortVal(__deref_out unsigned short *pVal)
{
  return get_UShortValAt(0, pVal);
}

HRESULT CNktParamImpl::put_UShortVal(__in unsigned short newValue)
{
  return put_UShortValAt(0, newValue);
}

HRESULT CNktParamImpl::get_LongVal(__deref_out long *pVal)
{
  return get_LongValAt(0, pVal);
}

HRESULT CNktParamImpl::put_LongVal(__in long newValue)
{
  return put_LongValAt(0, newValue);
}

HRESULT CNktParamImpl::get_ULongVal(__deref_out unsigned long *pVal)
{
  return get_ULongValAt(0, pVal);
}

HRESULT CNktParamImpl::put_ULongVal(__in unsigned long newValue)
{
  return put_ULongValAt(0, newValue);
}

HRESULT CNktParamImpl::get_LongLongVal(__deref_out __int64 *pVal)
{
  return get_LongLongValAt(0, pVal);
}

HRESULT CNktParamImpl::put_LongLongVal(__in __int64 newValue)
{
  return put_LongLongValAt(0, newValue);
}

HRESULT CNktParamImpl::get_ULongLongVal(__deref_out unsigned __int64 *pVal)
{
  return get_ULongLongValAt(0, pVal);
}

HRESULT CNktParamImpl::put_ULongLongVal(__in unsigned __int64 newValue)
{
  return put_ULongLongValAt(0, newValue);
}

HRESULT CNktParamImpl::get_SSizeTVal(__deref_out my_ssize_t *pVal)
{
  return get_SSizeTValAt(0, pVal);
}

HRESULT CNktParamImpl::put_SSizeTVal(__in my_ssize_t newValue)
{
  return put_SSizeTValAt(0, newValue);
}

HRESULT CNktParamImpl::get_SizeTVal(__deref_out my_size_t *pVal)
{
  return get_SizeTValAt(0, pVal);
}

HRESULT CNktParamImpl::put_SizeTVal(__in my_size_t newValue)
{
  return put_SizeTValAt(0, newValue);
}

HRESULT CNktParamImpl::get_FloatVal(__deref_out float *pVal)
{
  return get_FloatValAt(0, pVal);
}

HRESULT CNktParamImpl::put_FloatVal(__in float newValue)
{
  return put_FloatValAt(0, newValue);
}

HRESULT CNktParamImpl::get_DoubleVal(__deref_out double *pVal)
{
  return get_DoubleValAt(0, pVal);
}

HRESULT CNktParamImpl::put_DoubleVal(__in double newValue)
{
  return put_DoubleValAt(0, newValue);
}

HRESULT CNktParamImpl::get_CharValAt(__in LONG index, __deref_out signed char *pVal)
{
  return get_ByteValAt(index, (unsigned char*)pVal);
}

HRESULT CNktParamImpl::put_CharValAt(__in LONG index, __in signed char newValue)
{
  return put_ByteValAt(index, (unsigned char)newValue);
}

HRESULT CNktParamImpl::get_ByteValAt(__in LONG index, __deref_out unsigned char *pVal)
{
  HRESULT hRes;
  unsigned char nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetByte(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_ByteValAt(__in LONG index, __in unsigned char newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetByte(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_ShortValAt(__in LONG index, __deref_out short *pVal)
{
  return get_UShortValAt(index, (unsigned short*)pVal);
}

HRESULT CNktParamImpl::put_ShortValAt(__in LONG index, __in short newValue)
{
  return put_UShortValAt(index, (unsigned short)newValue);
}

HRESULT CNktParamImpl::get_UShortValAt( __in LONG index, __deref_out unsigned short *pVal)
{
  HRESULT hRes;
  unsigned short nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetWord(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_UShortValAt(__in LONG index, __in unsigned short newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetWord(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_LongValAt(__in LONG index, __deref_out long *pVal)
{
  return get_ULongValAt(index, (ULONG*)pVal);
}

HRESULT CNktParamImpl::put_LongValAt(__in LONG index, __in long newValue)
{
  return put_ULongValAt(index, (ULONG)newValue);
}

HRESULT CNktParamImpl::get_ULongValAt(__in LONG index, __deref_out unsigned long *pVal)
{
  HRESULT hRes;
  DWORD nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetDWord(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_ULongValAt(__in LONG index, __in unsigned long newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetDWord(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_LongLongValAt(__in LONG index, __deref_out __int64 *pVal)
{
  return get_ULongLongValAt(index, (ULONGLONG*)pVal);
}

HRESULT CNktParamImpl::put_LongLongValAt(__in LONG index, __in __int64 newValue)
{
  return put_ULongLongValAt(index, (ULONGLONG)newValue);
}

HRESULT CNktParamImpl::get_ULongLongValAt(__in LONG index, __deref_out unsigned __int64 *pVal)
{
  HRESULT hRes;
  ULONGLONG nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetQWord(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_ULongLongValAt(__in LONG index, __in unsigned __int64 newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetQWord(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_SSizeTValAt(__in LONG index, __deref_out my_ssize_t *pVal)
{
  return get_SizeTValAt(index, (my_size_t*)pVal);
}

HRESULT CNktParamImpl::put_SSizeTValAt(__in LONG index, __in my_ssize_t newValue)
{
  return put_SizeTValAt(index, (my_size_t)newValue);
}

HRESULT CNktParamImpl::get_SizeTValAt(__in LONG index, __deref_out my_size_t *pVal)
{
  HRESULT hRes;
  SIZE_T nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetSizeT(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? (my_ssize_t)nVal : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_SizeTValAt(__in LONG index, __in my_size_t newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetSizeT((SIZE_T)newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_FloatValAt(__in LONG index, __deref_out float *pVal)
{
  HRESULT hRes;
  float nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetFloat(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0.0f;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_FloatValAt(__in LONG index, __in float newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetFloat(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_DoubleValAt(__in LONG index, __deref_out double *pVal)
{
  HRESULT hRes;
  double nVal;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get value
    hRes = (index >= 0) ? (lpIntObj->GetDouble(&nVal, (SIZE_T)index)) : E_INVALIDARG;
  }
  *pVal = (SUCCEEDED(hRes)) ? nVal : 0.0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_DoubleValAt(__in LONG index, __in double newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set value
  hRes = (index >= 0) ? (lpIntObj->SetDouble(newValue, (SIZE_T)index)) : E_INVALIDARG;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_PointerVal(__deref_out my_ssize_t *pVal)
{
  LPVOID lpValue;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER, RETURN_FATAL_ERRORS);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = 0;
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  }
  //get pointer value
  hRes = lpIntObj->GetPointer(&lpValue);
  *pVal = (SUCCEEDED(hRes)) ? (my_ssize_t)lpValue : 0;
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::put_PointerVal(__in my_ssize_t newValue)
{
  HRESULT hRes;

  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
    return SetupPermanentErrorInfoAndReturn(RETURN_FATAL_ERRORS);
  //set pointer value
  hRes = lpIntObj->SetPointer((LPVOID)newValue);
  return SetupErrorInfoAndReturn(hRes, RETURN_FATAL_ERRORS);
}

HRESULT CNktParamImpl::get_IsNullPointer(__deref_out VARIANT_BOOL *pVal)
{
  BOOL bIsNull;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = VARIANT_TRUE;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get "is null pointer"
  hRes = lpIntObj->IsNullPointer(&bIsNull);
  *pVal = (SUCCEEDED(hRes)) ? VariantBool_From_Bool(bIsNull) : VARIANT_FALSE;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::Memory(__deref_out INktProcessMemory **ppProcMem)
{
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  TNktComPtr<INktProcessMemory> cIProcMem;
  HRESULT hRes;

  if (ppProcMem == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get memory reader/writer
    hRes = lpIntObj->GetProcessMemory(&cProcMem);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktProcessMemoryImpl,INktProcessMemory,
                            CNktDvProcessMemory>(&cIProcMem, cProcMem, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppProcMem = cIProcMem.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktParamImpl::CastTo(__in BSTR newType, __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  if (newType == NULL)
  {
    hRes = E_POINTER;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
  }
  if (SUCCEEDED(hRes))
  {
    //get "cast to"
    hRes = lpIntObj->CastTo(&cParam, newType);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  if (FAILED(hRes))
  {
    //on error create dummy interface
    hRes = GetDummyInterfaceHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, hRes);
  }
  //done
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}
