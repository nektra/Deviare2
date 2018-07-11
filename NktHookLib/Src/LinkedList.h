/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#ifndef _NKT_LINKEDLIST_H
#define _NKT_LINKEDLIST_H

#if _MSC_VER > 1000
  #pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#ifndef NKT_ASSERT
  #ifdef _DEBUG
    #define NKT_ASSERT(x) if (!(x)) __debugbreak();
  #else
    #define NKT_ASSERT(x)
  #endif //_DEBUG
#endif //!NKT_ASSERT

template <typename classOrStruct>
class TNktLnkLst;

template <typename classOrStruct>
class TNktLnkLstNode
{
public:
  typedef TNktLnkLst<classOrStruct> _LnkLstList;
  typedef TNktLnkLstNode<classOrStruct> _LnkLstNode;

  TNktLnkLstNode()
    {
    lpNext = lpPrev = NULL;
    lpList = NULL;
    return;
    };

  _inline _LnkLstNode* GetNextNode() const
    {
    return lpNext;
    };

  _inline _LnkLstNode* GetPrevNode() const
    {
    return lpPrev;
    };

  _inline _LnkLstList* GetLinkedList() const
    {
    return lpList;
    };

  _inline classOrStruct* GetEntry() const
    {
    return static_cast<classOrStruct*>(const_cast<_LnkLstNode*>(this));
    };

  _inline classOrStruct* GetNextEntry() const
    {
    return (lpNext != NULL) ? (lpNext->GetEntry()) : NULL;
    };

  _inline classOrStruct* GetPrevEntry() const
    {
    return (lpPrev != NULL) ? (lpPrev->GetEntry()) : NULL;
    };

  _inline VOID RemoveNode()
    {
    if (lpList != NULL)
      lpList->Remove(this);
    return;
    }

private:
  template <typename classOrStruct>
  friend class TNktLnkLst;

  _LnkLstList *lpList;
  _LnkLstNode *lpNext, *lpPrev;
};

//-----------------------------------------------------------

template <typename classOrStruct>
class TNktLnkLst
{
public:
  typedef TNktLnkLst<classOrStruct> _LnkLstList;
  typedef TNktLnkLstNode<classOrStruct> _LnkLstNode;

  TNktLnkLst()
    {
    lpHead = lpTail = NULL;
    return;
    };

  _inline BOOL IsEmpty() const
    {
    return (lpHead == NULL) ? TRUE : FALSE;
    };

  _inline VOID Remove(__inout _LnkLstNode *lpNode)
    {
    NKT_ASSERT(lpNode->lpList == this);
    lpNode->lpList = NULL;
    if (lpHead == lpNode)
      lpHead = lpNode->lpNext;
    if (lpTail == lpNode)
      lpTail = lpNode->lpPrev;
    if (lpNode->lpNext != NULL)
      lpNode->lpNext->lpPrev = lpNode->lpPrev;
    if (lpNode->lpPrev != NULL)
      lpNode->lpPrev->lpNext = lpNode->lpNext;
    lpNode->lpNext = lpNode->lpPrev = NULL;
    return;
    };

  _inline VOID PushHead(__inout _LnkLstNode *lpNode)
    {
    NKT_ASSERT(lpNode != NULL);
    NKT_ASSERT(lpNode->lpList == NULL);
    NKT_ASSERT(lpNode->lpPrev == NULL && lpNode->lpNext == NULL);
    lpNode->lpList = this;
    if (lpHead != NULL)
      lpHead->lpPrev = lpNode;
    lpNode->lpNext = lpHead;
    lpHead = lpNode;
    if (lpTail == NULL)
      lpTail = lpNode;
    return;
    };

  _inline VOID PushTail(__inout _LnkLstNode *lpNode)
    {
    NKT_ASSERT(lpNode != NULL);
    NKT_ASSERT(lpNode->lpList == NULL);
    NKT_ASSERT(lpNode->lpPrev == NULL && lpNode->lpNext == NULL);
    lpNode->lpList = this;
    if (lpTail != NULL)
      lpTail->lpNext = lpNode;
    lpNode->lpPrev = lpTail;
    lpTail = lpNode;
    if (lpHead == NULL)
      lpHead = lpNode;
    return;
    };

  _inline VOID PushAfter(__inout _LnkLstNode *lpNode, __inout _LnkLstNode *lpAfterNode)
    {
    if (lpAfterNode == lpTail)
    {
      PushTail(lpNode);
    }
    else if (lpAfterNode == NULL)
    {
      PushHead(lpNode);
    }
    else
    {
      NKT_ASSERT(lpNode->lpList == NULL);
      NKT_ASSERT(lpNode->lpNext==NULL && lpNode->lpPrev==NULL);
      lpNode->lpList = this;
      lpNode->lpNext = lpAfterNode->lpNext;
      lpNode->lpPrev = lpAfterNode;
      if (lpAfterNode->lpNext != NULL)
        lpAfterNode->lpNext->lpPrev = lpNode;
      lpAfterNode->lpNext = lpNode;
    }
    return;
    };

  _inline VOID PushBefore(__inout _LnkLstNode *lpNode, __inout _LnkLstNode *lpBeforeNode)
    {
    if (lpBeforeNode == lpHead)
    {
      PushHead(lpNode);
    }
    else if (lpBeforeNode == NULL)
    {
      PushTail(lpNode);
    }
    else
    {
      NKT_ASSERT(lpNode->lpList == NULL);
      NKT_ASSERT(lpNode->lpNext==NULL && lpNode->lpPrev==NULL);
      lpNode->lpList = this;
      lpNode->lpNext = lpBeforeNode;
      lpNode->lpPrev = lpBeforeNode->lpPrev;
      if (lpBeforeNode->lpPrev != NULL)
        lpBeforeNode->lpPrev->lpNext = lpNode;
      lpBeforeNode->lpPrev = lpNode;
    }
    return;
    };

  _inline SIZE_T GetCount() const
    {
    Iterator it;
    classOrStruct *lpItem;
    SIZE_T nCount;

    nCount = 0;
    for (lpItem=it.Begin(*const_cast<_LnkLstList*>(this)); lpItem!=NULL; lpItem=it.Next())
      nCount++;
    return nCount;
    };

  _inline classOrStruct* GetHead() const
    {
    return (lpHead != NULL) ? (lpHead->GetEntry()) : NULL;
    };

  _inline classOrStruct* GetTail() const
    {
    return (lpTail != NULL) ? (lpTail->GetEntry()) : NULL;
    };

  _inline classOrStruct* PopHead()
    {
    _LnkLstNode *lpNode;

    if ((lpNode = lpHead) == NULL)
      return NULL;
    Remove(lpHead);
    return lpNode->GetEntry();
    };

  _inline classOrStruct* PopTail()
    {
    _LnkLstNode *lpNode;

    if ((lpNode = lpTail) == NULL)
      return NULL;
    Remove(lpTail);
    return lpNode->GetEntry();
    };

  _inline VOID RemoveAll()
    {
    while (lpHead != NULL)
      Remove(lpHead);
    return;
    };

  //---------------------------------------------------------

  class Iterator
  {
  public:
    classOrStruct* Begin(__in _LnkLstList &_list)
      {
      lpNextCursor = _list.lpHead;
      return Next();
      };

    classOrStruct* Next()
      {
      lpCursor = lpNextCursor;
      if (lpCursor == NULL)
        return NULL;
      lpNextCursor = lpCursor->lpNext;
      return lpCursor->GetEntry();
      };

  private:
    _LnkLstNode *lpCursor, *lpNextCursor;
  };

  //---------------------------------------------------------

  class IteratorRev
  {
  public:
    classOrStruct* Begin(__in _LnkLstList &_list)
      {
      lpNextCursor = _list.lpTail;
      return Next();
      };

    classOrStruct* Next()
      {
      lpCursor = lpNextCursor;
      if (lpCursor == NULL)
        return NULL;
      lpNextCursor = lpCursor->lpPrev;
      return lpCursor->GetEntry();
      };

  private:
    _LnkLstNode *lpCursor, *lpNextCursor;
  };

private:
  _LnkLstNode *lpHead, *lpTail;
};

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_LINKEDLIST_H
