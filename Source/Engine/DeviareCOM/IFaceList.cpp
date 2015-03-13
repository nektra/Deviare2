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
#include "IFaceList.h"

//-----------------------------------------------------------

CNktFastMutex IInterfaceItem::cMtx;
TNktLnkLst<IInterfaceItem::CItem> IInterfaceItem::cList;

//-----------------------------------------------------------

VOID IInterfaceItem::InterfaceItem_Add(__in IInterfaceItem::CItem *lpItem)
{
  CNktAutoFastMutex cLock(&cMtx);

  cList.PushTail(lpItem);
  return;
}

VOID IInterfaceItem::InterfaceItem_Remove(__in IInterfaceItem::CItem *lpItem)
{
  CNktAutoFastMutex cLock(&cMtx);

  lpItem->RemoveNode();
  return;
}

VOID IInterfaceItem::InterfaceItem_InvalidateAll(__in DWORD dwCookieA, __in DWORD dwCookieB)
{
  CNktAutoFastMutex cLock(&cMtx);
  TNktLnkLst<CItem>::Iterator it;
  CItem *lpItem;
  TNktComPtr<IInterfaceItem> cIFaceItem;
  BOOL bAll;

  bAll = ((dwCookieA|dwCookieB) == 0) ? TRUE : FALSE;
  for (lpItem=it.Begin(cList); lpItem!=NULL; lpItem=it.Next())
  {
    if (bAll != FALSE ||
        (dwCookieA == lpItem->dwCookie[0] && dwCookieB == lpItem->dwCookie[1]))
    {
      try
      {
        cIFaceItem.Release();
      }
      catch (...)
      { }
      try
      {
        if (SUCCEEDED(lpItem->lpUnk->QueryInterface(__uuidof(IInterfaceItem), (void**)&cIFaceItem)))
        {
          lpItem->RemoveNode();
          cIFaceItem->InterfaceItem_CallInvalidate();
        }
      }
      catch (...)
      {
        try
        {
          lpItem->RemoveNode();
        }
        catch (...)
        { }
      }
    }
  }
  return;
}
