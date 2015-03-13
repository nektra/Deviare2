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

//-----------------------------------------------------------
// CNktComObject

template <class Impl, class IntObj>
class TNktComObject : public Impl
{
public:
  typedef Impl _BaseClass;

  TNktComObject(__in IntObj *p) throw() : Impl(p)
    {
    _pAtlModule->Lock();
    return;
    };

  virtual ~TNktComObject() throw()
    {
    FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
    _AtlDebugInterfacesModule.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
    _pAtlModule->Unlock();
    return;
    };

  STDMETHOD_(ULONG, AddRef)()
    {
    return InternalAddRef();
    };

  STDMETHOD_(ULONG, Release)()
    {
    return InternalRelease();
    };

  STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw()
    {
    return _InternalQueryInterface(iid, ppvObject);
    };

  template <class Q>
  HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) throw()
    {
    return QueryInterface(__uuidof(Q), (void**)pp);
    };

  static HRESULT WINAPI CreateComObject(__deref_out TNktComObject<Impl,IntObj>** pp,
                                        __in IntObj *lpIntObj, __in LPDWORD lpdwCookies);

  template <class Interface>
  static HRESULT WINAPI GetComLink(__deref_out Interface** pp, __in IntObj *lpIntObj,
                                   __in LPDWORD lpdwCookies)
    {
    TNktComPtr<INktDvEngBaseComLink> cComLink;
    HRESULT hRes;

    NKT_ASSERT(pp != NULL);
    *pp = NULL;
    hRes = lpIntObj->GetComLink(&cComLink, &TNktComObject<Impl,IntObj>::GetComLink_Create, lpdwCookies);
    if (SUCCEEDED(hRes))
      hRes = cComLink->QueryInterface<Interface>(pp);
    return hRes;
    };

private:
  static HRESULT __stdcall GetComLink_Create(__deref_out INktDvEngBaseComLink **lplpComLink,
                                             __in CNktDvEngBaseObj *lpObj, __in LPVOID lpUserParam);
};

template <class Impl, class IntObj>
HRESULT WINAPI TNktComObject<Impl,IntObj>::CreateComObject(__deref_out TNktComObject<Impl,IntObj>** pp,
                                                           __in IntObj *lpIntObj, __in LPDWORD lpdwCookies)
{
  TNktComObject<Impl,IntObj>* p;
  HRESULT hRes;

  ATLASSERT(pp != NULL);
  if (pp == NULL)
    return E_POINTER;
  *pp = NULL;
  hRes = E_OUTOFMEMORY;
  p = NULL;
  ATLTRY((p = new TNktComObject<Impl,IntObj>(lpIntObj)))
  if (p != NULL)
  {
    if (lpIntObj == NULL)
      p->InternalAddRef();
    p->SetVoid(NULL);
    p->InternalFinalConstructAddRef();
    hRes = p->_AtlInitialConstruct();
    if (SUCCEEDED(hRes))
      hRes = p->FinalConstruct();
    if (SUCCEEDED(hRes))
      hRes = p->_AtlFinalConstruct();
    if (SUCCEEDED(hRes))
      p->InterfaceItemSetCookies(lpdwCookies);
    p->InternalFinalConstructRelease();
    if (hRes != S_OK)
    {
      if (lpIntObj == NULL)
        p->InternalRelease();
      else
        delete p;
      p = NULL;
    }
  }
  *pp = p;
  return hRes;
}

template <class Impl, class IntObj>
HRESULT __stdcall TNktComObject<Impl,IntObj>::GetComLink_Create(
                        __deref_out INktDvEngBaseComLink **lplpComLink, __in CNktDvEngBaseObj *lpObj,
                        __in LPVOID lpUserParam)
{
  TNktComObject<Impl,IntObj> *lpPtr;
  HRESULT hRes;

  hRes = CreateComObject(&lpPtr, (IntObj*)lpObj, (LPDWORD)lpUserParam);
  if (SUCCEEDED(hRes))
  {
    hRes = lpPtr->QueryInterface(__uuidof(INktDvEngBaseComLink), (void**)lplpComLink);
    if (SUCCEEDED(hRes))
    {
      //because the QI does an AddRef, we restore the counter
      lpPtr->InternalRelease();
    }
    else
    {
      NKT_ASSERT(FALSE); //should not happen
      delete lpPtr;
    }
  }
  return hRes;
}

//------------------------------------------------------------------------------------------

template <class Impl, class Interface, class IntObj>
HRESULT GetComLinkHelper(__deref_out Interface **lplpObj, __inout IntObj *lpIntObj,
                         __in LPDWORD lpdwCookies)
{
  return TNktComObject<Impl,IntObj>::GetComLink(lplpObj, lpIntObj, lpdwCookies);
}
