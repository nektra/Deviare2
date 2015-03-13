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
#include "DbObject.h"
#include "DbObjectsEnum.h"
#include "DbModulesEnum.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktDbObjectImpl

HRESULT CNktDbObjectImpl::get_Id(__deref_out LONG *pVal)
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
  //get count
  *pVal = (LONG)(lpIntObj->GetObj()->GetId());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_Class(__deref_out eNktDboClass *pVal)
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
  *pVal = (eNktDboClass)(lpIntObj->GetObj()->GetClass());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_FundamentalType(__deref_out eNktDboFundamentalType *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    *pVal = ftNone;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get type
  *pVal = (eNktDboFundamentalType)(lpIntObj->GetObj()->GetFundamentalType());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_Declaration(__deref_out BSTR *pVal)
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
    sW = lpIntObj->GetObj()->GetDeclaration();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktDbObjectImpl::get_Name(__deref_out BSTR *pVal)
{
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get name
    hRes = cComBStr.Set(lpIntObj->GetObj()->GetName());
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::get_Size(__deref_out LONG *pVal)
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
  //get size
  *pVal = (LONG)(lpIntObj->GetObj()->GetSize());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_Align(__deref_out LONG *pVal)
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
  //get size
  *pVal = (LONG)(lpIntObj->GetObj()->GetAlign());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_IsArray(__deref_out VARIANT_BOOL *pVal)
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
  *pVal = VariantBool_From_Bool(lpIntObj->GetObj()->IsArray());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_ArrayMaxHint(__deref_out LONG *pVal)
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
  //get array max hint
  *pVal = (LONG)(lpIntObj->GetObj()->GetArrayMaxHint());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_IsAnsiString(__deref_out VARIANT_BOOL *pVal)
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
  *pVal = VariantBool_From_Bool(lpIntObj->GetObj()->IsAnsiString());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_IsWideString(__deref_out VARIANT_BOOL *pVal)
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
  *pVal = VariantBool_From_Bool(lpIntObj->GetObj()->IsWideString());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_IsGuid(__deref_out VARIANT_BOOL *pVal)
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
  //get "is guid"
  *pVal = VariantBool_From_Bool(lpIntObj->GetObj()->IsGuid());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::Evaluate(__deref_out INktDbObject **ppDbObject)
{
  return IndexedEvaluate(0, ppDbObject);
}

HRESULT CNktDbObjectImpl::IndexedEvaluate(__in LONG index, __deref_out INktDbObject **ppDbObject)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktDvDbObjectNoRef *lpDbObjOrig;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObject == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get evaluated
    lpDbObjOrig = (index >= 0) ? (lpIntObj->GetObj()->GetEvaluated((SIZE_T)index)) : NULL;
    if (lpDbObjOrig != NULL)
    {
      cDbObj.Release();
      cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjOrig));
      if (cDbObj != NULL)
        cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObject = cIDbObj.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::FullEvaluate(__deref_out INktDbObject **ppDbObject)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktDvDbObjectNoRef *lpDbObjOrig;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObject == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get full evaluated
    lpDbObjOrig = lpIntObj->GetObj()->GetFullEvaluated();
    cDbObj.Release();
    cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjOrig));
    if (cDbObj != NULL)
      cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
    hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObject = cIDbObj.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::get_Flags(__deref_out LONG *pVal)
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
  *pVal = (LONG)(lpIntObj->GetObj()->GetFlags());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_ItemsCount(__deref_out LONG *pVal)
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
  //get items count
  *pVal = (LONG)(lpIntObj->GetObj()->GetItemsCount());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::NameToIndex(__in BSTR name, __deref_out LONG *pVal)
{
  HRESULT hRes;
  SIZE_T nIndex;

  if (name == NULL || pVal == NULL)
  {
    if (pVal != NULL)
      *pVal = -1;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = -1;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get size
  nIndex = lpIntObj->GetObj()->NameToIndex(name);
  *pVal = (LONG)nIndex;
  return SetupErrorInfoAndReturn((nIndex != NKT_SIZE_T_MAX) ? S_OK : E_INVALIDARG);
}

HRESULT CNktDbObjectImpl::Items(__deref_out INktDbObjectsEnum **ppDbObjEnum)
{
  TNktComPtr<CNktDvDbObjectsEnumerator> cDbObjsEnum;
  TNktComPtr<INktDbObjectsEnum> cIDbObjsEnum;
  HRESULT hRes;

  if (ppDbObjEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    hRes = lpIntObj->GetObj()->GetItems(&cDbObjsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectsEnumImpl,INktDbObjectsEnum,
                            CNktDvDbObjectsEnumerator>(&cIDbObjsEnum, cDbObjsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObjEnum = cIDbObjsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::Item(__in LONG index, __deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktDvDbObjectNoRef *lpDbObjOrig;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get item
    lpDbObjOrig = (index >= 0) ? (lpIntObj->GetObj()->GetItem((SIZE_T)index)) : NULL;
    if (lpDbObjOrig != NULL)
    {
      cDbObj.Release();
      cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjOrig));
      if (cDbObj != NULL)
        cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = cIDbObj.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemName(__in LONG index, __deref_out BSTR *pVal)
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
    sW = (index >= 0) ? (lpIntObj->GetObj()->GetItemName((SIZE_T)index)) : NULL;
    hRes = (sW != NULL) ? S_OK : E_INVALIDARG;
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

HRESULT CNktDbObjectImpl::ItemFlags(__in LONG index, __deref_out LONG *pVal)
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
  //get item flags
  if (index >= 0)
  {
    *pVal = (my_ssize_t)(lpIntObj->GetObj()->GetItemFlags((SIZE_T)index));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemMemberType(__in LONG index, __deref_out eNktDboItemMemberType *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = mtNA;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get item flags
  if (index >= 0)
  {
    *pVal = (eNktDboItemMemberType)(lpIntObj->GetObj()->GetItemMemberType((SIZE_T)index));
    hRes = S_OK;
  }
  else
  {
    *pVal = mtNA;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemStructOffset(__in LONG index, __deref_out LONG *pVal)
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
  //get item struct offset
  if (index >= 0)
  {
    *pVal = (LONG)(lpIntObj->GetObj()->GetItemStructOffset((SIZE_T)index));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemStructBits(__in LONG index, __deref_out LONG *pVal)
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
  //get item struct bits
  if (index >= 0)
  {
    *pVal = (LONG)(lpIntObj->GetObj()->GetItemStructBits((SIZE_T)index));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemEnumValue(__in LONG index, __deref_out my_ssize_t *pVal)
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
  //get item enum value
  if (index >= 0)
  {
    *pVal = (my_ssize_t)(lpIntObj->GetObj()->GetItemEnumValue((SIZE_T)index));
    hRes = S_OK;
  }
  else
  {
    *pVal = 0;
    hRes = E_INVALIDARG;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::ItemDeclaration(__in LONG index, __deref_out BSTR *pVal)
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
    //get item declaration
    if (index >= 0 && (SIZE_T)index < lpIntObj->GetObj()->GetItemsCount())
    {
      sW = lpIntObj->GetObj()->GetItemDeclaration((SIZE_T)index);
      hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
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

HRESULT CNktDbObjectImpl::get_FuncCallingConvention(__deref_out eNktDboCallingConvention *pVal)
{
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (FAILED(hRes))
  {
    *pVal = ccNA;
    return SetupPermanentErrorInfoAndReturn();
  }
  //get items count
  *pVal = (eNktDboCallingConvention)(lpIntObj->GetObj()->GetFuncCallingConvention());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_FunctionReturn(__deref_out INktDbObject **ppDbObj)
{
  TNktComPtr<CNktDvDbObject> cDbObj;
  CNktDvDbObjectNoRef *lpDbObjOrig;
  TNktComPtr<CNktDvDbObjectWrapper> cDbObjWrp;
  TNktComPtr<INktDbObject> cIDbObj;
  HRESULT hRes;

  if (ppDbObj == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get function return
    lpDbObjOrig = lpIntObj->GetObj()->GetFunctionReturn();
    if (lpDbObjOrig != NULL)
    {
      cDbObj.Release();
      cDbObj.Attach(CNktDvDbObject::CreateFromNoRef(lpDbObjOrig));
      if (cDbObj != NULL)
        cDbObjWrp.Attach(CNktDvDbObjectWrapper::Create(cDbObj));
      hRes = (cDbObjWrp != NULL) ? S_OK : E_OUTOFMEMORY;
    }
    else
    {
      hRes = E_INVALIDARG;
    }
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectImpl,INktDbObject,CNktDvDbObjectWrapper>(&cIDbObj, cDbObjWrp,
                                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObj = cIDbObj.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktDbObjectImpl::get_FunctionReturnFlags(__deref_out LONG *pVal)
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
  //get function return flags
  *pVal = (LONG)(lpIntObj->GetObj()->GetFunctionReturnFlags());
  return SetupErrorInfoAndReturn(S_OK);
}

HRESULT CNktDbObjectImpl::get_FunctionReturnDeclaration(__deref_out BSTR *pVal)
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
    //get function return declaration
    sW = lpIntObj->GetObj()->GetFunctionReturnDeclaration();
    hRes = (sW != NULL) ? S_OK : E_OUTOFMEMORY;
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

HRESULT CNktDbObjectImpl::DbModules(__deref_out INktDbModulesEnum **ppDbModsEnum)
{
  TNktComPtr<CNktDvDbModulesEnumerator> cDbModsEnum;
  TNktComPtr<INktDbModulesEnum> cIDbModsEnum;
  HRESULT hRes;

  if (ppDbModsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get function return
    hRes = lpIntObj->GetObj()->GetModules(&cDbModsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbModulesEnumImpl,INktDbModulesEnum,
                            CNktDvDbModulesEnumerator>(&cIDbModsEnum, cDbModsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbModsEnum = cIDbModsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}
