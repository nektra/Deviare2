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

#include "resource.h"
#ifdef _WIN64
  #include "DeviareCOM64_i.h"
#else //_WIN64
  #include "DeviareCOM_i.h"
#endif //_WIN64
#include "CustomRegistryMap.h"
#include "MiscHelpers.h"
#include "IFaceList.h"
#include "NktComObject.h"

//-----------------------------------------------------------

template <class ImplementationClass, class InterfaceClass, class InternalObject,
          const IID* lpClsId, const IID* lpIid, const GUID* lpLibId>
class ATL_NO_VTABLE CNktObjectBaseImpl : public IDispatchImpl<InterfaceClass, lpIid, lpLibId,
                                                              DEVIARECOM_MajorVersion,
                                                              DEVIARECOM_MinorVersion>
{
public:
  CNktObjectBaseImpl(__in InternalObject *_lpIntObj) : IDispatchImpl<InterfaceClass, lpIid, lpLibId,
                                       DEVIARECOM_MajorVersion, DEVIARECOM_MinorVersion>()
    {
    lpIntObj = _lpIntObj;
    hPermamentErrorRes = S_OK;
    return;
    };

  ~CNktObjectBaseImpl()
    {
    if (lpIntObj != NULL)
      lpIntObj->RemoveComLink();
    return;
    };

  STDMETHOD(get_UserData)(__deref_out my_ssize_t* pVal)
    {
    if (pVal == NULL)
      return SetupErrorInfoAndReturn(E_POINTER);
    *pVal = (SUCCEEDED(hPermamentErrorRes)) ? (my_ssize_t)(lpIntObj->GetUserData()) : 0;
    return S_OK;
    };

  STDMETHOD(put_UserData)(__in my_ssize_t newValue)
    {
    if (SUCCEEDED(hPermamentErrorRes))
      lpIntObj->SetUserData((SIZE_T)newValue);
    return S_OK;
    };

  STDMETHOD(get_InterfaceError)(__deref_out HRESULT *pVal)
    {
    if (pVal == NULL)
      return SetupErrorInfoAndReturn(E_POINTER);
    *pVal = hPermamentErrorRes;
    return S_OK;
    };

  STDMETHOD(get_DeviareId)(__deref_out my_ssize_t *pVal)
    {
    CNktComBStr cComBStr;

    if (pVal == NULL)
      return SetupErrorInfoAndReturn(E_POINTER);
    *pVal = (my_ssize_t)(lpIntObj->GetDeviareId());
    return S_OK;
    };

  STDMETHOD(ToString)(__deref_out BSTR *pVal)
    {
    CNktStringW cStrTempW;
    CNktComBStr cComBStr;
    HRESULT hRes;

    if (pVal == NULL)
      return SetupErrorInfoAndReturn(E_POINTER);
    hRes = (SUCCEEDED(hPermamentErrorRes)) ? (lpIntObj->ToString(cStrTempW)) : hPermamentErrorRes;
    if (SUCCEEDED(hRes))
      hRes = cComBStr.Set(cStrTempW);
    if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
    {
      if (FAILED(cComBStr.Set(L"")))
        hRes = E_OUTOFMEMORY;
    }
    *pVal = cComBStr.Detach();
    return SetupErrorInfoAndReturn(hRes);
    };

  STDMETHOD(GetObjectFromDeviareId)(__in my_ssize_t nId, __deref_out IDispatch **ppDisp)
    {
    TNktComPtr<CNktDvEngBaseObj> cObj;
    HRESULT hRes;

    if (ppDisp != NULL)
    {
      hRes = CNktDvEngBaseObj::GetObjectFromDeviareId(&cObj, (SIZE_T)nId);
      if (SUCCEEDED(hRes))
      {
        hRes = GetIDispatchFromBaseObj(cObj, 0, ppDisp); //cookie won't be available
      }
      else
      {
        *ppDisp = NULL;
      }
    }
    else
    {
      hRes = E_POINTER;
    }
    return SetupErrorInfoAndReturn(hRes);
    };

  __inline VOID SetPermanentError(__in HRESULT hRes)
    {
    NktInterlockedExchange(&hPermamentErrorRes, hRes);
    return;
    };

  __inline HRESULT SetupPermanentErrorInfoAndReturn(__in BOOL bForceFatal=FALSE)
    {
    return ::SetupErrorInfoAndReturn(*lpIid, hPermamentErrorRes, bForceFatal);
    };

protected:
  /*
  template <class ImplementationClass2, class InterfaceClass2>
  friend HRESULT GetDummyInterfaceHelper(__deref_out InterfaceClass2 **lplpObj, __in HRESULT hLastError);
  */

  __inline HRESULT SetupErrorInfoAndReturn(__in HRESULT hErrorCodeRes, __in BOOL bForceFatal=FALSE)
    {
    return ::SetupErrorInfoAndReturn(*lpIid, hErrorCodeRes, bForceFatal);
    };

protected:
  InternalObject *lpIntObj;
  HRESULT volatile hPermamentErrorRes;
};

//-----------------------------------------------------------

template <class OBJCLASS>
class CNktObjectWrapper : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  static CNktObjectWrapper* Create(__in OBJCLASS *lpObj)
    {
    return NKT_MEMMGR_NEW CNktObjectWrapper(lpObj);
    };

  HRESULT ToString(__inout CNktStringW &cStrDest)
    {
    cStrDest.Empty();
    return S_OK;
    };

  __inline OBJCLASS* GetObj()
    {
    return cObj;
    };

protected:
  CNktObjectWrapper(__in OBJCLASS *lpObj) : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
    {
    NKT_ASSERT(lpObj != NULL);
    cObj = lpObj;
    return;
    };

private:
  TNktComPtr<OBJCLASS> cObj;
};

//-----------------------------------------------------------

#define OBJBASE_DECLARE_APIS_1(T, IntObj)                                    \
    HRESULT FinalConstruct()                                                 \
      {                                                                      \
      InterfaceItemAdd();                                                    \
      return ::CoCreateFreeThreadedMarshaler(GetControllingUnknown(),        \
                                             &(cUnkMarshaler.p));            \
      };                                                                     \
    void FinalRelease()                                                      \
      {                                                                      \
      cUnkMarshaler.Release();                                               \
      return;                                                                \
      };                                                                     \
    ULONG InternalAddRef()                                                   \
      {                                                                      \
      if (lpIntObj == NULL)                                                  \
        return CComObjectRootEx<CComMultiThreadModel>::InternalAddRef();     \
      return lpIntObj->AddRef();                                             \
      };                                                                     \
    ULONG InternalRelease()                                                  \
      {                                                                      \
      if (lpIntObj == NULL)                                                  \
      {                                                                      \
        ULONG l = CComObjectRootEx<CComMultiThreadModel>::InternalRelease(); \
        if (l == 0)                                                          \
          delete this;                                                       \
        return l;                                                            \
      }                                                                      \
      return lpIntObj->Release();                                            \
      };                                                                     \
    VOID InvalidateInterface()                                               \
      { SetPermanentError(E_FAIL); };                                        \
    HRESULT STDMETHODCALLTYPE INktDvEngBaseLink_Destroy()                    \
      {                                                                      \
      TNktComObject<T,IntObj> *pT;                                           \
      pT = static_cast<TNktComObject<T,IntObj>*>(this);                      \
      delete pT;                                                             \
      return S_OK;                                                           \
      };
