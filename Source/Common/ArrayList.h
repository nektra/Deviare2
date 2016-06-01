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

#include <windows.h>
#include "MemoryManager.h"
#include <search.h>
#include "Debug.h"

//-----------------------------------------------------------

#ifndef NKT_SIZE_T_MAX
  #define NKT_SIZE_T_MAX  ((SIZE_T)(-1))
#endif //NKT_SIZE_T_MAX

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
class TNktArrayList : public CNktMemMgrObj, protected ItemRemoveClass
{
public:
  TNktArrayList() : CNktMemMgrObj(),ItemRemoveClass()
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
    return InsertElementAt(elem, (SIZE_T)-1);
    };

  virtual BOOL SetElementAt(__in TType elem, __in SIZE_T nIndex)
    {
    if (nIndex >= nCount)
      return FALSE;
    lpItems[nIndex] = elem;
    return TRUE;
    };

  virtual BOOL InsertElementAt(__in TType elem, __in SIZE_T nIndex=(SIZE_T)-1)
    {
    if (nIndex >= nCount)
      nIndex = nCount;
    if (SetSize(nCount+1) == FALSE)
      return FALSE;
    nktMemMove(&lpItems[nIndex+1], &lpItems[nIndex], (nCount-nIndex)*sizeof(TType));
    lpItems[nIndex] = elem;
    nCount++;
    return TRUE;
    };

  virtual BOOL SortedInsert(__in TType elem)
    {
    SIZE_T nIndex, nMin, nMax;

    nMin = 1; //shifted by one to avoid problems with negative indexes
    nMax = nCount; //if count == 0, loop will not enter
    while (nMin <= nMax)
    {
      nIndex = nMin + (nMax - nMin) / 2;
      if (elem == lpItems[nIndex-1])
      {
        nMin = nIndex;
        break;
      }
      if (elem < lpItems[nIndex-1])
        nMax = nIndex - 1;
      else
        nMin = nIndex + 1;
    }
    return InsertElementAt(elem, nMin-1);
    };

  template<class _Comparator>
  BOOL SortedInsert(__in TType elem, __in _Comparator lpCompareFunc, __in_opt LPVOID lpContext=NULL)
    {
    SIZE_T nIndex, nMin, nMax;
    int res;

    if (lpCompareFunc == NULL)
      return FALSE;
    nMin = 1; //shifted by one to avoid problems with negative indexes
    nMax = nCount; //if count == 0, loop will not enter
    while (nMin <= nMax)
    {
      nIndex = nMin + (nMax - nMin) / 2;
      res = lpCompareFunc(lpContext, &elem, lpItems+nIndex-1);
      if (res == 0)
      {
        nMin = nIndex;
        break;
      }
      if (res < 0)
        nMax = nIndex - 1;
      else
        nMin = nIndex + 1;
    }
    return InsertElementAt(elem, nMin-1);
    };

  template<class _Comparator>
  BOOL QuickSort(__in _Comparator lpCompareFunc, __in_opt LPVOID lpContext=NULL)
    {
    if (lpCompareFunc == NULL)
      return FALSE;
    qsort_s(lpItems, nCount, sizeof(TType),
            reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>(lpCompareFunc),
            lpContext);
    return TRUE;
    };

  template<class _Comparator>
  TType* BinarySearchPtr(__in TType *lpKey, __in _Comparator lpSearchFunc, __in_opt LPVOID lpContext=NULL)
    {
    if (lpKey==NULL || lpSearchFunc==NULL)
      return NULL;
    return (TType*)bsearch_s(lpKey, lpItems, nCount, sizeof(TType),
                       reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>(lpSearchFunc),
                       lpContext);
    };

  template<class _Comparator>
  SIZE_T BinarySearch(__in TType *lpKey, __in _Comparator lpSearchFunc, __in_opt LPVOID lpContext=NULL)
    {
    TType *lpRes;

    lpRes = BinarySearchPtr(lpKey, lpSearchFunc, lpContext);
    return (lpRes != NULL) ? (SIZE_T)(lpRes-lpItems) : NKT_SIZE_T_MAX;
    };

  virtual TType GetElementAt(__in SIZE_T nIndex) const
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };

  virtual TType& operator[](__in SIZE_T nIndex)
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };
  virtual const TType& operator[](__in SIZE_T nIndex) const
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };

  virtual SIZE_T GetCount() const
    {
    return nCount;
    };

  virtual BOOL SetCount(__in SIZE_T _nCount)
    {
    SIZE_T i;

    if (_nCount > nCount)
    {
      if (SetSize(_nCount) == FALSE)
        return FALSE;
    }
    if (_nCount < nCount)
    {
      for (i=_nCount; i<nCount; i++)
        OnItemFinalize(lpItems[i]);
    }
    nCount = _nCount;
    return TRUE;
    };

  virtual SIZE_T GetSize() const
    {
    return nSize;
    };

  virtual BOOL SetSize(__in SIZE_T _nSize, __in BOOL bForceRealloc=FALSE)
    {
    TType *lpNewItems;
    SIZE_T i;

    if (_nSize != nSize)
    {
      if (_nSize>nSize || bForceRealloc!=FALSE)
      {
        //first try to allocate new block (or relocated one)
        if (_nSize > 0)
        {
          if (nSize > 0 && bForceRealloc == FALSE)
          {
            //don't granulate on first allocation
            _nSize = (_nSize+nGranularity-1) / nGranularity;
            _nSize *= nGranularity;
          }
          lpNewItems = (TType*)nktMemMalloc(_nSize*sizeof(TType));
          if (lpNewItems == NULL)
            return FALSE;
        }
        else
          lpNewItems = NULL;
        //free old pointers if needed
        if (_nSize < nCount)
        {
          for (i=_nSize; i<nCount; i++)
            OnItemFinalize(lpItems[i]);
          nktMemSet(&lpItems[_nSize], 0, (nCount-_nSize)*sizeof(TType));
        }
        //move old data to new
        if (_nSize > 0)
        {
          //copy old
          i = (nCount <= _nSize) ? nCount : _nSize;
          nktMemCopy(lpNewItems, lpItems, i*sizeof(TType));
          //initialize the rest to null
          if (_nSize > nCount)
            nktMemSet(&lpNewItems[nCount], 0, (_nSize-nCount)*sizeof(TType));
        }
        //release old pointer
        if (lpItems != NULL)
          nktMemFree(lpItems);
        lpItems = lpNewItems;
        nSize = _nSize;
        if (nCount > nSize)
          nCount = nSize;
      }
    }
    return TRUE;
    };

  virtual BOOL IsEmpty() const
    {
    return (nCount == 0) ? TRUE : FALSE;
    };

  virtual SIZE_T GetIndexOf(__in TType elem, __in SIZE_T nStartIndex=0) const
    {
    SIZE_T i;

    for (i=nStartIndex; i<nCount; i++)
    {
      if (elem == lpItems[i])
        return i;
    }
    return (SIZE_T)-1;
    };

  virtual BOOL Contains(__in TType elem) const
    {
    return (GetIndexOf(elem) != (SIZE_T)-1) ? TRUE : FALSE;
    };

  virtual TType GetLastElement() const
    {
    return (nCount > 0) ? GetElementAt(nCount-1) : (TType)0;
    };

  virtual TType* GetBuffer() const
    {
    return lpItems;
    };

  virtual BOOL RemoveElementAt(__in SIZE_T nIndex, __in SIZE_T nItemsCount=1, __in BOOL bDoFinalize=TRUE)
    {
    SIZE_T i;

    if (nIndex >= nCount || nItemsCount == 0 || nIndex+nItemsCount < nIndex)
      return FALSE;
    if (nIndex+nItemsCount > nCount)
      nItemsCount = nCount-nIndex;
    nCount -= nItemsCount;
    if (bDoFinalize != FALSE)
    {
      for (i=0; i<nItemsCount; i++)
        OnItemFinalize(lpItems[nIndex+i]);
    }
    memmove(&lpItems[nIndex], &lpItems[nIndex+nItemsCount], (nCount-nIndex)*sizeof(TType));
    return TRUE;
    };

  virtual VOID RemoveAllElements()
    {
    SIZE_T i;

    for (i=0; i<nCount; i++)
      OnItemFinalize(lpItems[i]);
    if (lpItems != NULL)
    {
      nktMemFree(lpItems);
      lpItems = NULL;
    }
    nCount = nSize = 0;
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
      nktMemFree(item);
    return;
    };
};


template <typename TType, SIZE_T nGranularity=32>
class TNktArrayListWithFree : public TNktArrayList<TType,nGranularity,
                                                   TNktArrayListItemRemove_Free<TType>>
{
};

//-----------------------------------------------------------

template <typename TType>
class TNktArrayListItemRemove_Release
{
public:
  VOID OnItemFinalize(__inout TType& item)
    {
    if (item != NULL)
      item->Release();
    return;
    };
};

template <typename TType, SIZE_T nGranularity=32>
class TNktArrayListWithRelease : public TNktArrayList<TType,nGranularity,
                                                      TNktArrayListItemRemove_Release<TType>>
{
};

//-----------------------------------------------------------

template <typename TType>
class TNktArrayListItemRemove_Delete
{
public:
  virtual VOID OnItemFinalize(__inout TType& item)
    {
    if (item != NULL)
      delete item;
    return;
    };
};


template <typename TType, SIZE_T nGranularity=32>
class TNktArrayListWithDelete : public TNktArrayList<TType,nGranularity,
                                                     TNktArrayListItemRemove_Delete<TType>>
{
};

//-----------------------------------------------------------

template <typename TType, SIZE_T nGranularity=32, class ItemRemoveClass=TNktArrayListItemRemove<TType>>
class TNktArrayList4Structs : public CNktMemMgrObj, protected ItemRemoveClass
{
public:
  TNktArrayList4Structs() : CNktMemMgrObj(),ItemRemoveClass()
    {
    lpItems = NULL;
    nCount = nSize = 0;
    return;
    };

  virtual ~TNktArrayList4Structs()
    {
    RemoveAllElements();
    return;
    };

  virtual BOOL AddElement(__in TType *lpElem)
    {
    return InsertElementAt(lpElem, (SIZE_T)-1);
    };

  virtual BOOL SetElementAt(__in TType *lpElem, __in SIZE_T nIndex)
    {
    if (nIndex >= nCount)
      return FALSE;
    nktMemCopy(&lpItems[nIndex], lpElem, sizeof(TType));
    return TRUE;
    };

  virtual BOOL InsertElementAt(__in TType *lpElem, __in SIZE_T nIndex=(SIZE_T)-1)
    {
    if (nIndex >= nCount)
      nIndex = nCount;
    if (SetSize(nCount+1) == FALSE)
      return FALSE;
    nktMemMove(&lpItems[nIndex+1], &lpItems[nIndex], (nCount-nIndex)*sizeof(TType));
    nktMemCopy(&lpItems[nIndex], lpElem, sizeof(TType));
    nCount++;
    return TRUE;
    };

  template<class _Comparator>
  BOOL SortedInsert(__in TType *lpElem, __in _Comparator lpCompareFunc, __in_opt LPVOID lpContext=NULL)
    {
    SIZE_T nIndex, nMin, nMax;
    int res;

    if (lpElem == NULL && lpCompareFunc == NULL)
      return FALSE;
    if (nCount == 0)
      return InsertElementAt(lpElem);
    nMin = 1; //shifted by one to avoid problems with negative indexes
    nMax = nCount;
    while (nMin <= nMax)
    {
      nIndex = (nMin + nMax) / 2;
      res = lpCompareFunc(lpContext, lpElem, lpItems+nIndex-1);
      if (res == 0)
        return InsertElementAt(lpElem, nIndex-1);
      if (res < 0)
        nMax = nIndex - 1;
      else
        nMin = nIndex + 1;
    }
    return InsertElementAt(lpElem, nMin-1);
    };

  template<class _Comparator>
  BOOL QuickSort(__in _Comparator lpCompareFunc, __in_opt LPVOID lpContext=NULL)
    {
    if (lpCompareFunc == NULL)
      return FALSE;
    qsort_s(lpItems, nCount, sizeof(TType),
            reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>(lpCompareFunc),
            lpContext);
    return TRUE;
    };

  template<class _Comparator>
  TType* BinarySearchPtr(__in TType *lpKey, __in _Comparator lpSearchFunc, __in_opt LPVOID lpContext=NULL)
    {
    if (lpKey==NULL || lpSearchFunc==NULL)
      return NULL;
    return (TType*)bsearch_s(lpKey, lpItems, nCount, sizeof(TType),
                       reinterpret_cast<int (__cdecl *)(void *,const void *,const void *)>(lpSearchFunc),
                       lpContext);
    };

  template<class _Comparator>
  SIZE_T BinarySearch(__in TType *lpKey, __in _Comparator lpSearchFunc, __in_opt LPVOID lpContext=NULL)
    {
    TType *lpRes;

    lpRes = BinarySearchPtr(lpKey, lpSearchFunc, lpContext);
    return (lpRes != NULL) ? (SIZE_T)(lpRes-lpItems) : NKT_SIZE_T_MAX;
    };

  virtual TType& GetElementAt(__in SIZE_T nIndex) const
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };

  virtual TType& operator[](__in SIZE_T nIndex)
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };
  virtual const TType& operator[](__in SIZE_T nIndex) const
    {
    NKT_ASSERT(nIndex < nCount);
    return lpItems[nIndex];
    };

  virtual TType* GetBuffer() const
    {
    return lpItems;
    };

  virtual SIZE_T GetCount() const
    {
    return nCount;
    };

  virtual BOOL SetCount(__in SIZE_T _nCount)
    {
    SIZE_T i;

    if (_nCount > nCount)
    {
      if (SetSize(_nCount) == FALSE)
        return FALSE;
    }
    if (_nCount < nCount)
    {
      for (i=_nCount; i<nCount; i++)
        OnItemFinalize(lpItems[i]);
    }
    nCount = _nCount;
    return TRUE;
    };

  virtual SIZE_T GetSize() const
    {
    return nSize;
    };

  virtual BOOL SetSize(__in SIZE_T _nSize, __in BOOL bForceRealloc=FALSE)
    {
    TType *lpNewItems;
    SIZE_T k;

    if (_nSize != nSize)
    {
      if (_nSize > nSize || bForceRealloc != FALSE)
      {
        if (_nSize > 0)
        {
          if (nSize > 0 && bForceRealloc == FALSE)
          {
            //don't granulate on first allocation
            _nSize = (_nSize+nGranularity-1) / nGranularity;
            _nSize *= nGranularity;
          }
          lpNewItems = (TType*)nktMemMalloc(_nSize*sizeof(TType));
          if (lpNewItems == NULL)
            return FALSE;
        }
        else
          lpNewItems = NULL;
        //free old pointers if needed
        if (_nSize < nCount)
        {
          //WARNING: Check "OnItemDelete" method to avoid errors
          for (k=_nSize; k<nCount; k++)
            OnItemFinalize(lpItems[k]);
          nktMemSet(&lpItems[_nSize], 0, (nCount-_nSize)*sizeof(TType));
        }
        //move old data to new
        if (_nSize > 0)
        {
          //copy old
          k = (nCount <= _nSize) ? nCount : _nSize;
          nktMemCopy(lpNewItems, lpItems, k*sizeof(TType));
          //initialize the rest to null
          if (_nSize > nCount)
            nktMemSet(&lpNewItems[nCount], 0, (_nSize-nCount)*sizeof(TType));
        }
        if (lpItems != NULL)
          nktMemFree(lpItems);
        lpItems = lpNewItems;
        nSize = _nSize;
        if (nCount > nSize)
          nCount = nSize;
      }
    }
    return TRUE;
    };

  virtual BOOL IsEmpty() const
    {
    return (nCount == 0) ? TRUE : FALSE;
    };

  virtual BOOL RemoveElementAt(__in SIZE_T nIndex, __in SIZE_T nItemsCount=1, __in BOOL bDoFinalize=TRUE)
    {
    SIZE_T i;

    if (nIndex >= nCount || nItemsCount == 0 || nIndex+nItemsCount < nIndex)
      return FALSE;
    if (nIndex+nItemsCount > nCount)
      nItemsCount = nCount-nIndex;
    nCount -= nItemsCount;
    if (bDoFinalize != FALSE)
    {
      for (i=0; i<nItemsCount; i++)
        OnItemFinalize(lpItems[nIndex+i]);
    }
    nktMemMove(&lpItems[nIndex], &lpItems[nIndex+nItemsCount], (nCount-nIndex)*sizeof(TType));
    return TRUE;
    };

  virtual VOID RemoveAllElements()
    {
    SIZE_T i;

    for (i=0; i<nCount; i++)
      OnItemFinalize(lpItems[i]);
    if (lpItems != NULL)
    {
      nktMemFree(lpItems);
      lpItems = NULL;
    }
    nCount = nSize = 0;
    return;
    };

  virtual TType* ReserveBlock(__in SIZE_T nItemsCount, __in SIZE_T nIndex=(SIZE_T)-1)
    {
    if (SetSize(nCount+nItemsCount) == FALSE)
      return FALSE;
    if (nIndex > nCount)
      nIndex = nCount;
    nktMemMove(&lpItems[nIndex+nItemsCount], &lpItems[nIndex], (nCount-nIndex)*sizeof(TType));
    nktMemSet(&lpItems[nIndex], 0, nItemsCount*sizeof(TType));
    nCount += nItemsCount;
    return &lpItems[nIndex];
    };

  virtual TType* PopBlock(__in SIZE_T nItemsCount)
    {
    if (nItemsCount > nCount)
      nItemsCount = nCount;
    nCount -= nItemsCount;
    return &lpItems[nCount];
    };

private:
  TType *lpItems;
  SIZE_T nCount, nSize;
};

//-----------------------------------------------------------

