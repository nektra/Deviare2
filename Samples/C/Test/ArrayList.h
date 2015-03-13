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

#ifndef _NKT_ARRAYLIST_H
#define _NKT_ARRAYLIST_H

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <crtdbg.h>

//-----------------------------------------------------------

template <typename TType>
class TNktArrayListItemRemove
{
public:
  virtual VOID OnItemFinalize(__inout TType& item)
  {
    return;
  };
};

//-----------------------------------------------------------

template <typename TType, SIZE_T nGranularity=32, class ItemRemoveClass=TNktArrayListItemRemove<TType>>
class TNktArrayList : protected ItemRemoveClass
{
public:
  TNktArrayList() : ItemRemoveClass()
    {
    lpItems = NULL;
    nCount = nSize = 0;
    return;
    };

  virtual ~TNktArrayList()
    {
    RemoveAllElements();
    return;
    };

  virtual BOOL AddElement(__in TType elem)
    {
    TType *lpNewItems;
    SIZE_T _nSize;

    if (nCount >= nSize)
    {
      //don't granulate on first allocation
      _nSize = (nSize+nGranularity) / nGranularity;
      _nSize *= nGranularity;
      lpNewItems = (TType*)malloc(_nSize*sizeof(TType));
      if (lpNewItems == NULL)
        return FALSE;
      //copy old
      if (nCount > 0)
        memcpy(lpNewItems, lpItems, nCount*sizeof(TType));
      //release old pointer
      if (lpItems != NULL)
        free(lpItems);
      lpItems = lpNewItems;
      nSize = _nSize;
    }
    lpItems[nCount++] = elem;
    return TRUE;
    };

  virtual TType& operator[](__in SIZE_T nIndex)
    {
    _ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };
  virtual const TType& operator[](__in SIZE_T nIndex) const
    {
    _ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };

  virtual SIZE_T GetCount() const
    {
    return nCount;
    };

  virtual BOOL IsEmpty() const
    {
    return (nCount == 0) ? TRUE : FALSE;
    };

  virtual TType* GetBuffer() const
    {
    return lpItems;
    };

  virtual VOID RemoveAllElements()
    {
    SIZE_T i;

    for (i=0; i<nCount; i++)
      OnItemFinalize(lpItems[i]);
    if (lpItems != NULL)
    {
      free(lpItems);
      lpItems = NULL;
    }
    nCount = nSize = 0;
    return;
    };

protected:
  virtual VOID OnItemFinalize(__inout TType& item)
    {
    return;
    };

private:
  TType *lpItems;
  SIZE_T nCount, nSize;
};

//-----------------------------------------------------------

template <typename TType>
class TNktArrayListItemRemove_Free
{
public:
  virtual VOID OnItemFinalize(__inout TType& item)
    {
    if (item != NULL)
      free(item);
    return;
    };
};

template <typename TType, SIZE_T nGranularity=32>
class TNktArrayListWithFree : public TNktArrayList<TType,nGranularity,
                                                   TNktArrayListItemRemove_Free<TType>>
{
};

//-----------------------------------------------------------

#endif //_NKT_ARRAYLIST_H
