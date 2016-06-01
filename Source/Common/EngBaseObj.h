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
#include <ole2.h>
#include <unknwn.h>
#include "Debug.h"
#include "AutoPtr.h"
#include "ComPtr.h"
#include "LinkedList.h"
#include "ArrayList.h"
#include "StringLiteW.h"
#include "WaitableObjects.h"
#include "Threads.h"

//-----------------------------------------------------------

class CNktDvObject : public IUnknown, public CNktMemMgrObj
{
public:
  CNktDvObject();
  virtual ~CNktDvObject();

  virtual HRESULT __stdcall QueryInterface(__in REFIID riid, __deref_out void **ppvObject)
    {
    if (ppvObject == NULL)
      return E_POINTER;
    *ppvObject = NULL;
    return E_NOTIMPL;
    };

  virtual ULONG __stdcall AddRef();
  virtual ULONG __stdcall Release();

  virtual ULONG GetRefCount() const;

  virtual VOID Lock();
  virtual VOID Unlock();

private:
  LONG volatile nRefCount;
  LONG volatile nIsDestroying;
};

//-----------------------------------------------------------

extern const IID IID_INktDvEngBaseComLink;
MIDL_INTERFACE("81A6E59C-7780-4610-A29A-36BBF977B002")
INktDvEngBaseComLink : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE INktDvEngBaseLink_Destroy() = 0;
};

//-----------------------------------------------------------

class CNktDvEngBaseObj : public CNktDvObject, public TNktLnkLstNode<CNktDvEngBaseObj>
{
public:
  CNktDvEngBaseObj(__in BOOL bEnumerable);
  virtual ~CNktDvEngBaseObj();

  virtual SIZE_T GetDeviareId() const;
  static HRESULT GetObjectFromDeviareId(__deref_out CNktDvEngBaseObj** lplpObj, __in SIZE_T nId);

  virtual VOID SetUserData(__in SIZE_T nData);
  virtual SIZE_T GetUserData() const;

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

  typedef HRESULT (__stdcall *lpfnCreateComLink)(__deref_out INktDvEngBaseComLink **lplpComLink,
                                                 __in CNktDvEngBaseObj *lpObj, __in LPVOID lpUserParam);
  virtual HRESULT GetComLink(__deref_out INktDvEngBaseComLink **lplpComLink,
                             __in lpfnCreateComLink lpCreateFunc, __in LPVOID lpUserParam);
  virtual INktDvEngBaseComLink* GetComLinkNoAddRef()
    {
    return lpComLink;
    };
  virtual VOID RemoveComLink();

  //--------

  static VOID CleanUp();
  static BOOL CanUnloadNow();
  static VOID DumpObjects();

private:
  SIZE_T volatile nUserData;
  BOOL bEnumerable;
  SIZE_T volatile nTag[3];
  INktDvEngBaseComLink *lpComLink;
};

//-----------------------------------------------------------

template <class _lock_class>
class TNktDvEngBaseObjWithLock : public CNktDvEngBaseObj
{
public:
  TNktDvEngBaseObjWithLock(__in BOOL bEnumerable) : CNktDvEngBaseObj(bEnumerable)
    {
    return;
    };

  virtual VOID Lock()
    {
    cLock.Lock();
    return;
    };

  virtual VOID Unlock()
    {
    cLock.Unlock();
    return;
    };

  virtual _lock_class& GetLockObject()
    {
    return cLock;
    };

private:
  _lock_class cLock;
};

//-----------------------------------------------------------

template <class C>
class TNktDvEngBaseObjNoRef : public C
{
public:
  virtual ULONG __stdcall AddRef()
    {
    return nRefCount;
    };

  virtual ULONG __stdcall Release()
    {
    return nRefCount;
    };
};

//-----------------------------------------------------------

class CNktDvEngBaseObjAutoLock
{
public:
  CNktDvEngBaseObjAutoLock(__inout CNktDvEngBaseObj *_lpBaseObj)
    {
    lpBaseObj = _lpBaseObj;
    NKT_ASSERT(lpBaseObj != NULL);
    lpBaseObj->Lock();
    return;
    };
  virtual ~CNktDvEngBaseObjAutoLock()
    {
    lpBaseObj->Unlock();
    return;
    };

private:
  CNktDvEngBaseObj *lpBaseObj;
};

//-----------------------------------------------------------

#include "EnumeratorBase.h"

class CNktDvEngBaseObjEnumerator : public TNktDvEnumerator<CNktDvEngBaseObj>
{
public:
  CNktDvEngBaseObjEnumerator() : TNktDvEnumerator<CNktDvEngBaseObj>()
    {
    return;
    };

protected:
  virtual HRESULT ToString(__inout CNktStringW &cStrDest)
    {
    if (cStrDest.Format(L"CNktDvEngBaseObjEnumerator [%Iu items]", GetCount()) == FALSE)
      return E_OUTOFMEMORY;
    return S_OK;
    };
};

//-----------------------------------------------------------

