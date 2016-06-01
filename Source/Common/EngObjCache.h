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

#include "EngDefines.h"
#include "FnvHash.h"
#include "AutoPtr.h"
#include "ArrayList.h"
#include "WaitableObjects.h"

//-----------------------------------------------------------

template <class ObjClass, SIZE_T nHashSize, SIZE_T nMaxCacheSize>
class TNktDvEngObjCache : public CNktMemMgrObj
{
public:
  TNktDvEngObjCache() : CNktMemMgrObj()
    {
    NktInterlockedExchange(&nTick, 0);
    return;
    };

  virtual ~TNktDvEngObjCache()
    {
    RemoveAll();
    return;
    };

  virtual HRESULT Add(__in Fnv64_t nKey, __inout ObjClass *lpObj, __in BOOL bReplaceIfAlreadyExists)
    {
    TNktComPtr<ObjClass> cObjAutoRef(lpObj);
    SIZE_T nListIndex = Key2ListIndex(nKey);

    if (lpObj == NULL)
      return E_POINTER;
    {
      CNktAutoFastMutex cLock(&cListMtx[nListIndex]);
      LIST_ITEM sNewItem;
      SIZE_T nObjIdx;

      sNewItem.nKey = nKey;
      sNewItem.lpObj = lpObj;
      sNewItem.nTick = (ULONG)NktInterlockedIncrement(&nTick);
      nObjIdx = cLists[nListIndex].BinarySearch(&sNewItem, &ListItemCompare, NULL);
      if (nObjIdx != NKT_SIZE_T_MAX)
      {
        if (bReplaceIfAlreadyExists == FALSE)
          return NKT_DVERR_AlreadyExists;
        cLists[nListIndex][nObjIdx].lpObj->Release();
        cLists[nListIndex][nObjIdx] = sNewItem;
      }
      else
      {
        if (cLists[nListIndex].GetCount() >= nMaxCacheSize)
          RemoveExtraItem(nListIndex);
        if (cLists[nListIndex].SortedInsert(&sNewItem, &ListItemCompare, NULL) == FALSE)
          return E_OUTOFMEMORY;
      }
      lpObj->AddRef();
    }
    return S_OK;
    };

  virtual VOID Remove(__in Fnv64_t nKey)
    {
    SIZE_T nListIndex = Key2ListIndex(nKey);
    {
      CNktAutoFastMutex cLock(&cListMtx[nListIndex]);
      LIST_ITEM sItem;
      SIZE_T nObjIdx;

      sItem.nKey = nKey;
      nObjIdx = cLists[nListIndex].BinarySearch(&sItem, &ListItemCompare, NULL);
      if (nObjIdx != NKT_SIZE_T_MAX)
      {
        cLists[nListIndex][nObjIdx].lpObj->Release();
        cLists[nListIndex].RemoveElementAt(nObjIdx);
      }
    }
    return;
    };

  virtual VOID Remove(__in Fnv64_t nKey, __out ObjClass **lplpObj)
    {
    SIZE_T nListIndex = Key2ListIndex(nKey);
    *lplpObj = NULL;
    {
      CNktAutoFastMutex cLock(&cListMtx[nListIndex]);
      LIST_ITEM sItem;
      SIZE_T nObjIdx;

      _ASSERT(lplpObj != NULL);
      sItem.nKey = nKey;
      nObjIdx = cLists[nListIndex].BinarySearch(&sItem, &ListItemCompare, NULL);
      if (nObjIdx != NKT_SIZE_T_MAX)
      {
        *lplpObj = cLists[nListIndex][nObjIdx].lpObj;
        cLists[nListIndex].RemoveElementAt(nObjIdx);
      }
    }
    return;
    };

  virtual VOID RemoveAll()
    {
    SIZE_T i, nListIndex, nCount;

    for (nListIndex=0; nListIndex<nHashSize; nListIndex++)
    {
      CNktAutoFastMutex cLock(&cListMtx[nListIndex]);

      nCount = cLists[nListIndex].GetCount();
      for (i=0; i<nCount; i++)
        cLists[nListIndex][i].lpObj->Release();
      cLists[nListIndex].RemoveAllElements();
    }
    return;
    };

  virtual ObjClass* Get(__in Fnv64_t nKey)
    {
    SIZE_T nListIndex = Key2ListIndex(nKey);
    {
      CNktAutoFastMutex cLock(&cListMtx[nListIndex]);
      LIST_ITEM sItem;
      SIZE_T nObjIdx;

      sItem.nKey = nKey;
      nObjIdx = cLists[nListIndex].BinarySearch(&sItem, &ListItemCompare, NULL);
      if (nObjIdx != NKT_SIZE_T_MAX)
      {
        cLists[nListIndex][nObjIdx].lpObj->AddRef();
        cLists[nListIndex][nObjIdx].nTick = (ULONG)NktInterlockedIncrement(&nTick);
        return cLists[nListIndex][nObjIdx].lpObj;
      }
    }
    return NULL;
    };

  virtual CNktFastMutex* GetLock(__in Fnv64_t nKey)
    {
    SIZE_T nListIndex = Key2ListIndex(nKey);
    return &cListMtx[nListIndex];
    };

private:
  typedef struct tagLIST_ITEM {
    Fnv64_t nKey;
    ULONG nTick;
    ObjClass *lpObj;
  } LIST_ITEM;

  typedef typename TNktDvEngObjCache::LIST_ITEM CacheListItem;

  virtual SIZE_T Key2ListIndex(__in Fnv64_t nKey)
    {
    return (SIZE_T)nKey % nHashSize;
    };

  virtual VOID RemoveExtraItem(__in SIZE_T nListIndex)
    {
    SIZE_T i, nCount, nIdx;
    ULONG nMinTick;

    nCount = cLists[nListIndex].GetCount();
    if (nCount > 0)
    {
      nMinTick = cLists[nListIndex][nIdx = 0].nTick;
      for (i=1; i<nCount; i++)
      {
        if (cLists[nListIndex][i].nTick < nMinTick)
          nMinTick = cLists[nListIndex][nIdx = i].nTick;
      }
      cLists[nListIndex][nIdx].lpObj->Release();
      cLists[nListIndex].RemoveElementAt(nIdx);
    }
    return;
    };

  static int ListItemCompare(__in void *, __in CacheListItem *lpElem1, __in CacheListItem *lpElem2)
    {
    return NKT_DV_CMP<Fnv64_t>(lpElem1->nKey, lpElem2->nKey);
    };

private:
  LONG volatile nTick;
  CNktFastMutex cListMtx[nHashSize];
  TNktArrayList4Structs<LIST_ITEM> cLists[nHashSize];
};

//-----------------------------------------------------------

