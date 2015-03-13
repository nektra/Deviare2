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
#include "DbModule.h"
#include "DbObjectsEnum.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------
// CNktDbModuleImpl

HRESULT CNktDbModuleImpl::get_Id(__deref_out LONG *pVal)
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

HRESULT CNktDbModuleImpl::get_Name(__deref_out BSTR *pVal)
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

HRESULT CNktDbModuleImpl::DbFunctions(__deref_out INktDbObjectsEnum **ppDbObjsEnum)
{
  TNktComPtr<CNktDvDbModuleWrapper> lpIntObj;
  TNktComPtr<CNktDvDbObjectsEnumerator> cDbObjsEnum;
  TNktComPtr<INktDbObjectsEnum> cIDbObjsEnum;
  HRESULT hRes;

  if (ppDbObjsEnum == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get function return
    hRes = lpIntObj->GetObj()->GetFunctions(&cDbObjsEnum);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktDbObjectsEnumImpl,INktDbObjectsEnum,
                            CNktDvDbObjectsEnumerator>(&cIDbObjsEnum, cDbObjsEnum,
                                                       InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppDbObjsEnum = cIDbObjsEnum.Detach();
  return SetupErrorInfoAndReturn(hRes);
}
