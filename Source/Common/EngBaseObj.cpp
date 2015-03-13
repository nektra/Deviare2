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

#include "EngBaseObj.h"

//-----------------------------------------------------------

#if defined _M_IX86
  #define TAG_VALUE 0x34DEAD35UL
#elif defined _M_X64
  #define TAG_VALUE 0x34C3DEAD35BEBE32ui64
#else
  #error Unsupported platform
#endif

//-----------------------------------------------------------

const IID IID_INktDvEngBaseComLink = { // {81A6E59C-7780-4610-A29A-36BBF977B002}
  0x81a6e59c, 0x7780, 0x4610, { 0xa2, 0x9a, 0x36, 0xbb, 0xf9, 0x77, 0xb0, 0x2 }
};
static LONG volatile nListMtx = 0;
static TNktLnkLst<CNktDvEngBaseObj> cObjList;
static LONG volatile nComLinkCreateMtx = 0;

//-----------------------------------------------------------

static _inline SIZE_T ROL(SIZE_T val, SIZE_T bits)
{
  return (SIZE_T)(val << bits) | (SIZE_T)(val >> (sizeof(SIZE_T)*8-bits));
}

//-----------------------------------------------------------

CNktDvObject::CNktDvObject() : CNktMemMgrObj()
{
  NktInterlockedExchange(&nRefCount, 1);
  NktInterlockedExchange(&nIsDestroying, 0);
  return;
}

CNktDvObject::~CNktDvObject()
{
  NKT_ASSERT(nRefCount == 0);
  return;
}

ULONG CNktDvObject::AddRef()
{
  return (ULONG)NktInterlockedIncrement(&nRefCount);
}

ULONG CNktDvObject::Release()
{
  LONG nCount;

  nCount = NktInterlockedDecrement(&nRefCount);
  if (nCount == 0)
  {
    //avoid double deletion when referencing objects while destroying
    if (NktInterlockedExchange(&nIsDestroying, 1) == 0)
    {
      delete this;
    }
    else
    {
      NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("Release called on a released object: %IXh", (SIZE_T)this));
      NKT_ASSERT(FALSE);
    }
  }
  return (ULONG)nCount;
}

ULONG CNktDvObject::GetRefCount() const
{
  return nRefCount;
}

VOID CNktDvObject::Lock()
{
  return;
}

VOID CNktDvObject::Unlock()
{
  return;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

CNktDvEngBaseObj::CNktDvEngBaseObj(__in BOOL _bEnumerable) : CNktDvObject(),
                                                             TNktLnkLstNode<CNktDvEngBaseObj>()
{
#if defined _M_IX86
  NktInterlockedExchange((LONG volatile*)&nUserData, 0);
#elif defined _M_X64
  InterlockedExchange64((LONGLONG volatile*)&nUserData, 0);
#endif
  bEnumerable = _bEnumerable;
#if defined _M_IX86
  NktInterlockedExchange((LONG volatile*)&nTag[0], (LONG)this ^ TAG_VALUE);
  NktInterlockedExchange((LONG volatile*)&nTag[1], nTag[0] ^ (LONG)ROL(TAG_VALUE, 7));
  NktInterlockedExchange((LONG volatile*)&nTag[2], nTag[0] ^ (LONG)ROL(TAG_VALUE, 13));
#elif defined _M_X64
  InterlockedExchange64((LONGLONG volatile*)&nTag[0], (LONGLONG)this ^ TAG_VALUE);
  InterlockedExchange64((LONGLONG volatile*)&nTag[1], nTag[0] ^ (LONGLONG)ROL(TAG_VALUE, 7));
  InterlockedExchange64((LONGLONG volatile*)&nTag[2], nTag[0] ^ (LONGLONG)ROL(TAG_VALUE, 13));
#endif
  lpComLink = NULL;
  {
    CNktSimpleLockNonReentrant cListLock(&nListMtx);

    cObjList.PushTail(this);
  }
  return;
}

CNktDvEngBaseObj::~CNktDvEngBaseObj()
{
  if (lpComLink != NULL)
  {
    try
    {
      lpComLink->INktDvEngBaseLink_Destroy();
    }
    catch (...)
    { }
    lpComLink = NULL;
  }
#if defined _M_IX86
  NktInterlockedExchange((LONG volatile*)&nTag[0], 0);
  NktInterlockedExchange((LONG volatile*)&nTag[1], 0);
  NktInterlockedExchange((LONG volatile*)&nTag[2], 0);
#elif defined _M_X64
  InterlockedExchange64((LONGLONG volatile*)&nTag[0], 0);
  InterlockedExchange64((LONGLONG volatile*)&nTag[1], 0);
  InterlockedExchange64((LONGLONG volatile*)&nTag[2], 0);
#endif
  {
    CNktSimpleLockNonReentrant cListLock(&nListMtx);

    cObjList.Remove(this);
  }
  return;
}

SIZE_T CNktDvEngBaseObj::GetDeviareId() const
{
  return (SIZE_T)this;
}

HRESULT CNktDvEngBaseObj::GetObjectFromDeviareId(__deref_out CNktDvEngBaseObj** lplpObj, __in SIZE_T nId)
{
  CNktDvEngBaseObj *lpObj;
  HRESULT hRes;
  SIZE_T i;

  if (lplpObj == NULL)
    return E_POINTER;
  *lplpObj = NULL;
  hRes = E_FAIL;
  __try
  {
    lpObj = (CNktDvEngBaseObj*)nId;
    i = (SIZE_T)lpObj ^ TAG_VALUE;
    if (lpObj != NULL &&
        lpObj->nTag[0] == i ||
        lpObj->nTag[1] == (i ^ ROL(TAG_VALUE,  7)) ||
        lpObj->nTag[2] == (i ^ ROL(TAG_VALUE, 13)))
    {
      *lplpObj = lpObj;
      lpObj->AddRef();
      hRes = S_OK;
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  { }
  return hRes;
}

VOID CNktDvEngBaseObj::SetUserData(__in SIZE_T nData)
{
#if defined _M_IX86
  NktInterlockedExchange((LONG volatile*)&nUserData, (LONG)nData);
#elif defined _M_X64
  InterlockedExchange64((LONGLONG volatile*)&nUserData, (LONGLONG)nData);
#endif
  return;
}

SIZE_T CNktDvEngBaseObj::GetUserData() const
{
  return nUserData;
}

HRESULT CNktDvEngBaseObj::ToString(__inout CNktStringW &cStrDest)
{
  cStrDest.Empty();
  return S_OK;
}

HRESULT CNktDvEngBaseObj::GetComLink(__deref_out INktDvEngBaseComLink **lplpComLink,
                                     __in lpfnCreateComLink lpCreateFunc, __in LPVOID lpUserParam)
{
  if (lplpComLink == NULL)
    return E_POINTER;
  if (lpComLink == NULL && lpCreateFunc != NULL)
  {
    CNktSimpleLockNonReentrant cComLinkCreateLock(&nComLinkCreateMtx);
    INktDvEngBaseComLink *lpTempComLink;
    HRESULT hRes;

    if (lpComLink == NULL && lpCreateFunc != NULL)
    {
      hRes = lpCreateFunc(&lpTempComLink, this, lpUserParam);
      if (FAILED(hRes))
        return hRes;
      NKT_ASSERT(lpTempComLink != NULL);
      lpComLink = lpTempComLink;
    }
  }
  *lplpComLink = lpComLink;
  if (lpComLink == NULL)
    return E_NOINTERFACE;
  lpComLink->AddRef();
  return S_OK;
}

VOID CNktDvEngBaseObj::RemoveComLink()
{
  CNktSimpleLockNonReentrant cComLinkCreateLock(&nComLinkCreateMtx);

  lpComLink = NULL;
  return;
}

VOID CNktDvEngBaseObj::CleanUp()
{
  CNktSimpleLockNonReentrant cListLock(&nListMtx);
  TNktLnkLst<CNktDvEngBaseObj>::Iterator it;
  CNktDvEngBaseObj *lpBaseObj;

#ifdef _DEBUG
  if (CanUnloadNow() == FALSE)
  {
    DumpObjects();
    NKT_ASSERT(FALSE);
  }
#endif //_DEBUG
  //detach objects in list (if leaks exists)
  for (lpBaseObj=it.Begin(cObjList); lpBaseObj!=NULL; lpBaseObj=it.Next())
    cObjList.Remove(lpBaseObj);
  return;
}

BOOL CNktDvEngBaseObj::CanUnloadNow()
{
  CNktSimpleLockNonReentrant cListLock(&nListMtx);

  return cObjList.IsEmpty();
}

VOID CNktDvEngBaseObj::DumpObjects()
{
  CNktSimpleLockNonReentrant cListLock(&nListMtx);
  TNktLnkLst<CNktDvEngBaseObj>::Iterator it;
  CNktDvEngBaseObj *lpBaseObj;
  CNktStringW cStrTempW;

  for (lpBaseObj=it.Begin(cObjList); lpBaseObj!=NULL; lpBaseObj=it.Next())
  {
    cStrTempW.Empty();
    lpBaseObj->ToString(cStrTempW);
    NKT_DEBUGPRINTLNA(Nektra::dlObjectDump, ("DumpObjects: %IXh => Ref:%ld / %S",
                      lpBaseObj, lpBaseObj->GetRefCount(), (LPWSTR)cStrTempW));
  }
  return;
}
