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

#pragma once

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include "..\..\Common\ComPtr.h"
#include "..\..\Common\StringLiteW.h"

//-----------------------------------------------------------

class CAutoTypeAttr
{
public:
  CAutoTypeAttr()
    {
    lpTypeAttr = NULL;
    lpTypeInfo = NULL;
    return;
    };

  ~CAutoTypeAttr()
    {
    if (lpTypeInfo != NULL && lpTypeAttr != NULL)
      lpTypeInfo->ReleaseTypeAttr(lpTypeAttr);
    return;
    };

  HRESULT Set(__in ITypeInfo *_lpTypeInfo)
    {
    if (lpTypeInfo != NULL && lpTypeAttr != NULL)
      lpTypeInfo->ReleaseTypeAttr(lpTypeAttr);
    lpTypeInfo = NULL;
    lpTypeAttr = NULL;
    //----
    if (_lpTypeInfo != NULL)
    {
      HRESULT hRes = _lpTypeInfo->GetTypeAttr(&lpTypeAttr);
      if (FAILED(hRes))
        return hRes;
    }
    lpTypeInfo = _lpTypeInfo;
    return S_OK;
    };

  TYPEATTR* operator->() const
    {
    NKT_ASSERT(lpTypeAttr != NULL);
    return lpTypeAttr;
    };

  TYPEATTR* Get()
    {
    return lpTypeAttr;
    };

private:
  TYPEATTR* lpTypeAttr;
  ITypeInfo *lpTypeInfo;
};

//-----------------------------------------------------------

class CAutoVarDesc
{
public:
  CAutoVarDesc()
    {
    lpVarDesc = NULL;
    lpTypeInfo = NULL;
    return;
    };

  ~CAutoVarDesc()
    {
    if (lpTypeInfo != NULL && lpVarDesc != NULL)
      lpTypeInfo->ReleaseVarDesc(lpVarDesc);
    return;
    };

  HRESULT Set(__in ITypeInfo *_lpTypeInfo, __in WORD i)
    {
    if (lpTypeInfo != NULL && lpVarDesc != NULL)
      lpTypeInfo->ReleaseVarDesc(lpVarDesc);
    lpTypeInfo = NULL;
    lpVarDesc = NULL;
    //----
    if (_lpTypeInfo != NULL)
    {
      HRESULT hRes = _lpTypeInfo->GetVarDesc(i, &lpVarDesc);
      if (FAILED(hRes))
        return hRes;
    }
    lpTypeInfo = _lpTypeInfo;
    return S_OK;
    };

  VARDESC* operator->() const
    {
    NKT_ASSERT(lpVarDesc != NULL);
    return lpVarDesc;
    };

  VARDESC* Get()
    {
    return lpVarDesc;
    };

private:
  VARDESC* lpVarDesc;
  ITypeInfo *lpTypeInfo;
};

//-----------------------------------------------------------

class CAutoFuncDesc
{
public:
  CAutoFuncDesc()
    {
    lpFuncDesc = NULL;
    lpTypeInfo = NULL;
    return;
    };

  ~CAutoFuncDesc()
    {
    if (lpTypeInfo != NULL && lpFuncDesc != NULL)
      lpTypeInfo->ReleaseFuncDesc(lpFuncDesc);
    return;
    };

  HRESULT Set(__in ITypeInfo *_lpTypeInfo, __in UINT i)
    {
    if (lpTypeInfo != NULL && lpFuncDesc != NULL)
      lpTypeInfo->ReleaseFuncDesc(lpFuncDesc);
    lpTypeInfo = NULL;
    lpFuncDesc = NULL;
    //----
    if (_lpTypeInfo != NULL)
    {
      HRESULT hRes = _lpTypeInfo->GetFuncDesc(i, &lpFuncDesc);
      if (FAILED(hRes))
        return hRes;
    }
    lpTypeInfo = _lpTypeInfo;
    return S_OK;
    };

  FUNCDESC* operator->() const
    {
    NKT_ASSERT(lpFuncDesc != NULL);
    return lpFuncDesc;
    };

  FUNCDESC* Get()
    {
    return lpFuncDesc;
    };

private:
  FUNCDESC* lpFuncDesc;
  ITypeInfo *lpTypeInfo;
};

//-----------------------------------------------------------

LPWSTR miscGetScodeString(__in SCODE sc, __out_z WCHAR szTempBufW[32]);

//-----------------------------------------------------------

