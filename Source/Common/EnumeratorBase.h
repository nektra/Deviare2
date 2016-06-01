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

#include "EngBaseObj.h"

//-----------------------------------------------------------

template <class Obj, SIZE_T nGranularity=128>
class TNktDvEnumerator : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  TNktDvEnumerator() : TNktDvEngBaseObjWithLock<CNktFastMutex>(FALSE)
    {
    nCurrIndex = 0;
    return;
    };

  virtual ~TNktDvEnumerator()
    {
    CNktAutoFastMutex cLock(&GetLockObject());

    aObjectsList.RemoveAllElements();
    return;
    };

  virtual SIZE_T GetCount()
    {
    return aObjectsList.GetCount();
    };

  virtual HRESULT GetAt(__in SIZE_T nIndex, __deref_out Obj** lpObj)
    {
    if (lpObj == NULL)
      return E_POINTER;
    if (nIndex >= aObjectsList.GetCount())
    {
      *lpObj = NULL;
      return E_INVALIDARG;
    }
    *lpObj = aObjectsList[nIndex];
    (*lpObj)->AddRef();
    return S_OK;
    };

  virtual HRESULT First(__deref_out Obj** lpObj)
    {
    CNktAutoFastMutex cLock(&GetLockObject());

    if (lpObj == NULL)
      return E_POINTER;
    if (aObjectsList.GetCount() == 0)
    {
      *lpObj = NULL;
      return E_FAIL;
    }
    *lpObj = aObjectsList[nCurrIndex = 0];
    (*lpObj)->AddRef();
    return S_OK;
    };

  virtual HRESULT Last(__deref_out Obj** lpObj)
    {
    CNktAutoFastMutex cLock(&GetLockObject());
    SIZE_T nCount;

    if (lpObj == NULL)
      return E_POINTER;
    nCount = aObjectsList.GetCount();
    if (nCount == 0)
    {
      *lpObj = NULL;
      return E_FAIL;
    }
    *lpObj = aObjectsList[nCurrIndex = nCount-1];
    (*lpObj)->AddRef();
    return S_OK;
    };

  virtual HRESULT Next(__deref_out Obj** lpObj)
    {
    CNktAutoFastMutex cLock(&GetLockObject());
    SIZE_T nCount;

    if (lpObj == NULL)
      return E_POINTER;
    nCount = aObjectsList.GetCount();
    if (nCount == 0 || nCurrIndex >= nCount-1)
    {
      *lpObj = NULL;
      return E_FAIL;
    }
    *lpObj = aObjectsList[++nCurrIndex];
    (*lpObj)->AddRef();
    return S_OK;
    };

  virtual HRESULT Previous(__deref_out Obj** lpObj)
    {
    CNktAutoFastMutex cLock(&GetLockObject());
    SIZE_T nCount;

    if (lpObj == NULL)
      return E_POINTER;
    nCount = aObjectsList.GetCount();
    if (nCount == 0 || nCurrIndex == 0)
    {
      *lpObj = NULL;
      return E_FAIL;
    }
    *lpObj = aObjectsList[--nCurrIndex];
    (*lpObj)->AddRef();
    return S_OK;
    };

protected:
  virtual HRESULT Add(__inout Obj *lpObj)
    {
    TNktComPtr<Obj> cObjLock(lpObj);

    NKT_ASSERT(lpObj != NULL);
    if (aObjectsList.AddElement(lpObj) == FALSE)
      return E_OUTOFMEMORY;
    lpObj->AddRef();
    return S_OK;
    };

protected:
  TNktArrayListWithRelease<Obj*,nGranularity> aObjectsList;
  SIZE_T volatile nCurrIndex;
};

//-----------------------------------------------------------

