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

#include "..\..\Common\LinkedList.h"
#include "..\..\Common\WaitableObjects.h"

//-----------------------------------------------------------

MIDL_INTERFACE("6352725F-0BB5-4DB8-AE89-FDA8FE362196")
IInterfaceItem : public IUnknown
{
public:
  class CItem : public TNktLnkLstNode<CItem>
  {
  public:
    IUnknown *lpUnk;
    DWORD dwCookie[2];
  };

  virtual HRESULT STDMETHODCALLTYPE InterfaceItem_CallInvalidate() = 0;

  static VOID InterfaceItem_Add(__in CItem *lpItem);
  static VOID InterfaceItem_Remove(__in CItem *lpItem);

  static VOID InterfaceItem_InvalidateAll(__in DWORD dwCookieA, __in DWORD dwCookieB);

  template<class ImplementationClass2, class InterfaceClass2>
  static VOID CopyCookieHelper(__in InterfaceClass2 *lpObj, __in DWORD dwCookie)
    {
    if (lpObj != NULL)
    {
      ImplementationClass2* lpImpl;

      lpImpl = static_cast<ImplementationClass2*>(lpObj);
      lpImpl->InterfaceItemSetCookie(dwCookie);
    }
    return;
    };

private:
  static CNktFastMutex cMtx;
  static TNktLnkLst<CItem> cList;
};

//-----------------------------------------------------------

template<class T>
class ATL_NO_VTABLE IInterfaceItemImpl : public IInterfaceItem
{
public:
  IInterfaceItemImpl()
    {
    cIFaceItem.dwCookie[0] = cIFaceItem.dwCookie[1] = 0;
    return;
    };

  ~IInterfaceItemImpl()
    {
    InterfaceItem_Remove(&cIFaceItem);
    return;
    };

  VOID InterfaceItemSetCookies(__in LPDWORD lpdwCookies)
    {
    if (lpdwCookies != NULL)
    {
      cIFaceItem.dwCookie[0] = lpdwCookies[0];
      cIFaceItem.dwCookie[1] = lpdwCookies[1];
    }
    return;
    };

  LPDWORD InterfaceItemGetCookies()
    {
    return cIFaceItem.dwCookie;
    };

  VOID InterfaceItemAdd()
    {
    T *pT = static_cast<T*>(this);

    cIFaceItem.lpUnk = pT->GetUnknown();
    if (cIFaceItem.lpUnk != NULL)
      InterfaceItem_Add(&cIFaceItem);
    return;
    };

  STDMETHODIMP InterfaceItem_CallInvalidate()
    {
    T* pT = static_cast<T*>(this);

    pT->InvalidateInterface();
    return S_OK;
    };

public:
  CItem cIFaceItem;
};
