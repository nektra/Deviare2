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
#include <ole2.h>
#include "Debug.h"

//-----------------------------------------------------------

#define NKT_VTS_I2              "\x02"      // a 'short'
#define NKT_VTS_I4              "\x03"      // a 'long'
#define NKT_VTS_R4              "\x04"      // a 'float'
#define NKT_VTS_R8              "\x05"      // a 'double'
#define NKT_VTS_CY              "\x06"      // a 'CY' or 'CY*'
#define NKT_VTS_DATE            "\x07"      // a 'DATE'
#define NKT_VTS_BSTRW           "\x08"      // an 'LPCOLESTR'
#define NKT_VTS_DISPATCH        "\x09"      // an 'IDispatch*'
#define NKT_VTS_SCODE           "\x0A"      // an 'SCODE'
#define NKT_VTS_BOOL            "\x0B"      // a 'VARIANT_BOOL'
#define NKT_VTS_VARIANT         "\x0C"      // a 'const VARIANT&' or 'VARIANT*'
#define NKT_VTS_UNKNOWN         "\x0D"      // an 'IUnknown*'
#define NKT_VTS_BSTRA           "\x0E"      // an 'LPCSTR'
#define NKT_VTS_I1              "\x10"      // a 'signed char'
#define NKT_VTS_UI1             "\x11"      // a 'BYTE'
#define NKT_VTS_UI2             "\x12"      // a 'WORD'
#define NKT_VTS_UI4             "\x13"      // a 'DWORD'
#define NKT_VTS_I8              "\x14"      // a 'LONGLONG'
#define NKT_VTS_UI8             "\x15"      // a 'ULONGLONG'

// parameter types: by reference VTs
#define NKT_VTS_PI2             "\x42"      // a 'short*'
#define NKT_VTS_PI4             "\x43"      // a 'long*'
#define NKT_VTS_PR4             "\x44"      // a 'float*'
#define NKT_VTS_PR8             "\x45"      // a 'double*'
#define NKT_VTS_PCY             "\x46"      // a 'CY*'
#define NKT_VTS_PDATE           "\x47"      // a 'DATE*'
#define NKT_VTS_PBSTR           "\x48"      // a 'BSTR*'
#define NKT_VTS_PDISPATCH       "\x49"      // an 'IDispatch**'
#define NKT_VTS_PSCODE          "\x4A"      // an 'SCODE*'
#define NKT_VTS_PBOOL           "\x4B"      // a 'VARIANT_BOOL*'
#define NKT_VTS_PVARIANT        "\x4C"      // a 'VARIANT*'
#define NKT_VTS_PUNKNOWN        "\x4D"      // an 'IUnknown**'
#define NKT_VTS_PI1             "\x50"      // a 'signed char*'
#define NKT_VTS_PUI1            "\x51"      // a 'BYTE*'
#define NKT_VTS_PUI2            "\x52"      // a 'WORD*'
#define NKT_VTS_PUI4            "\x53"      // a 'DWORD*'
#define NKT_VTS_PI8             "\x54"      // a 'LONGLONG*'
#define NKT_VTS_PUI8            "\x55"      // a 'ULONGLONG*'

// special VT_ and VTS_ values
#define NKT_VTS_NONE            NULL        // used for members with 0 params
#define NKT_VTS_BYREF	          0x80

#ifndef VT_BSTRA
  #define VT_BSTRA              14
#endif //VT_BSTRA

#ifndef NKT_SIZE_T_MAX
  #define NKT_SIZE_T_MAX  ((SIZE_T)(-1))
#endif //NKT_SIZE_T_MAX

//-----------------------------------------------------------

HRESULT NktComPtrAdvise(__in IUnknown* lpUnkCP, __in IUnknown* lpUnk, __in const IID& sIid,
                        __out LPDWORD lpdwCookie);
HRESULT NktComPtrUnadvise(__in IUnknown *lpUnkCP, __in const IID& sIid, __in DWORD dwCookie);
HRESULT NktComPtrInvokeHelper(__in IDispatch *lpDisp, __in DISPID dwDispID, __in UINT nFlags,
                              __in VARTYPE vtRet, __out LPVOID lpRetPtr, __in LPBYTE lpParameters, ...);
HRESULT NktComPtrInvokeHelperV(__in IDispatch *lpDisp, __in DISPID dwDispID, __in UINT nFlags,
                               __in VARTYPE vtRet, __out LPVOID lpRetPtr, __in LPBYTE lpParameters,
                               __in va_list argptr);

//-----------------------------------------------------------

template <class T>
class TNktNoAddRefReleaseOnComPtr : public T
{
private:
  STDMETHOD_(ULONG, AddRef)()=0;
  STDMETHOD_(ULONG, Release)()=0;
};

//-----------------------------------------------------------

template <class T>
class TNktComPtrBase : public CNktMemMgrObj
{
public:
  TNktComPtrBase() : CNktMemMgrObj()
    {
    lpPtr = NULL;
    return;
    };

  TNktComPtrBase(__in T* _lpPtr)
    {
    if ((lpPtr = _lpPtr) != NULL)
      _lpPtr->AddRef();
    return;
    };

  TNktComPtrBase(__in const TNktComPtrBase<T>& _lpPtr)
    {
    if ((lpPtr = _lpPtr.lpPtr) != NULL)
      lpPtr->AddRef();
    return;
    };

  ~TNktComPtrBase()
    {
    if (lpPtr != NULL)
      lpPtr->Release();
    return;
    };

  VOID Release()
    {
    T *lpTemp;

    if ((lpTemp = lpPtr) != NULL)
    {
      lpPtr = NULL;
      lpTemp->Release();
    }
    return;
    };

  operator T*() const
    {
    return lpPtr;
    };

  T& operator*() const
    {
    NKT_ASSERT(lpPtr != NULL);
    return *lpPtr;
    };

  //The assert on operator& usually indicates a bug.  If this is really
  //what is needed, however, take the address of the lpPtr member explicitly.
  T** operator&()
    {
    NKT_ASSERT(lpPtr == NULL);
    return &lpPtr;
    };

  TNktNoAddRefReleaseOnComPtr<T>* operator->() const
    {
    NKT_ASSERT(lpPtr != NULL);
    return (TNktNoAddRefReleaseOnComPtr<T>*)lpPtr;
    };

  T* operator=(__in T* _lpPtr)
    {
    if (_lpPtr != lpPtr)
    {
      if (_lpPtr != NULL)
        _lpPtr->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = _lpPtr;
    }
    return _lpPtr;
    };

  T* operator=(__in const TNktComPtrBase<T>& cPtr)
    {
    if (this != &cPtr)
    {
      if (cPtr.lpPtr != NULL)
        cPtr.lpPtr->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = cPtr.lpPtr;
    }
    return lpPtr;
    };

  TNktComPtrBase<T>* Clone()
    {
    TNktComPtrBase<T> *lpNewObj = NKT_MEMMGR_NEW TNktComPtrBase<T>;
    if (lpNewObj == NULL)
      return NULL;
    lpNewObj->lpPtr = lpPtr;
    if (lpPtr != NULL)
      lpPtr->AddRef();
    return lpNewObj;
    };

  BOOL Clone(__inout TNktComPtrBase<T> &cTarget)
    {
    if (this != &cTarget)
    {
      if (cTarget.lpPtr != NULL)
        cTarget.lpPtr->Release();
      cTarget.lpPtr = lpPtr;
      if (lpPtr != NULL)
        lpPtr->AddRef();
    }
    return TRUE;
    };

  BOOL operator!() const
    {
    return (lpPtr == NULL) ? TRUE : FALSE;
    };

  BOOL operator==(T* _lpPtr) const
    {
    return (lpPtr == _lpPtr) ? TRUE : FALSE;
    };

  BOOL IsEqualObject(__in IUnknown *lpOther)
    {
    if (lpPtr==NULL && lpOther==NULL)
      return TRUE;
    if (lpPtr==NULL || lpOther==NULL)
      return FALSE;
    TNktComPtrBase<IUnknown> cUnk1;
    TNktComPtrBase<IUnknown> cUnk2;
    lpPtr->QueryInterface(IID_IUnknown, (void**)&cUnk1);
    lpOther->QueryInterface(IID_IUnknown, (void**)&cUnk2);
    return (cUnk1 == cUnk2) ? TRUE : FALSE;
    };

  VOID Attach(__in T* _lpPtr)
    {
    if (lpPtr != NULL)
      lpPtr->Release();
    lpPtr = _lpPtr;
    return;
    };

  T* Detach()
    {
    T* _lpPtr = lpPtr;
    lpPtr = NULL;
    return _lpPtr;
    };

  HRESULT CopyTo(__out T** lplpT)
    {
    NKT_ASSERT(lplpT != NULL);
    if (lplpT == NULL)
      return E_POINTER;
    *lplpT = lpPtr;
    if (lpPtr != NULL)
      lpPtr->AddRef();
    return S_OK;
    };

  HRESULT SetSite(__in IUnknown* lpUnkParent)
    {
    IObjectWithSite* lpChildSite;
    HRESULT hRes;

    if (lpPtr == NULL)
      return E_POINTER;
    lpChildSite = NULL;
    hRes = lpPtr->QueryInterface(IID_IObjectWithSite, (void**)&lpChildSite);
    if (SUCCEEDED(hRes) && lpChildSite!=NULL)
    {
      hRes = lpChildSite->SetSite(lpUnkParent);
      lpChildSite->Release();
    }
    return hRes;
    };

  HRESULT Advise(__in IUnknown* lpUnk, __in const IID& sIid, __out LPDWORD lpdwCookie)
    {
    return NktComPtrAdvise(lpPtr, lpUnk, sIid, lpdwCookie);
    };

  HRESULT CoCreateInstance(__in REFCLSID rClsID, __in LPUNKNOWN lpUnkOuter=NULL, __in DWORD dwClsContext=CLSCTX_ALL)
    {
    NKT_ASSERT(lpPtr == NULL);
    return ::CoCreateInstance(rClsID, lpUnkOuter, dwClsContext, __uuidof(T), (void**)&lpPtr);
    };

  HRESULT CoCreateInstance(__in LPCOLESTR szProgID, __in LPUNKNOWN lpUnkOuter=NULL, __in DWORD dwClsContext=CLSCTX_ALL)
    {
    CLSID sClsID;
    HRESULT hRes;

    NKT_ASSERT(lpPtr == NULL);
    hRes = ::CLSIDFromProgID(szProgID, &sClsID);
    if (SUCCEEDED(hRes))
      hRes = ::CoCreateInstance(sClsID, lpUnkOuter, dwClsContext, __uuidof(T), (void**)&lpPtr);
    return hRes;
    };

  HRESULT CoCreateInstance(__in HINSTANCE hDll, __in REFCLSID rClsID, __in LPUNKNOWN lpUnkOuter=NULL)
    {
    typedef HRESULT (__stdcall *lpfnDllGetClassObject)(REFCLSID rClsID, REFIID rIID, LPVOID* lplpVoid);
    HRESULT hRes;
    lpfnDllGetClassObject fnDllGetClassObject;
    IClassFactory *lpFactory;

    NKT_ASSERT(lpPtr == NULL);
    if (hDll == NULL)
      return REGDB_E_KEYMISSING;
    fnDllGetClassObject = (lpfnDllGetClassObject)slGetProcAddressWide(hDll, L"DllGetClassObject");
    if (fnDllGetClassObject == NULL)
      return REGDB_E_KEYMISSING;
    hRes = fnDllGetClassObject(rClsID, IID_IClassFactory, (LPVOID*)&lpFactory);
    if (FAILED(hRes))
      return hRes;
    hRes = lpFactory->CreateInstance(lpUnkOuter, __uuidof(T), (void**)&lpPtr);
    lpFactory->Release();
    return hRes;
    };

  template <class Q>
  HRESULT QueryInterface(__out Q** lplpQ) const
    {
    NKT_ASSERT(lplpQ!=NULL && *lplpQ==NULL);
    return lpPtr->QueryInterface(__uuidof(Q), (void**)lplpQ);
    };

protected:
  T* lpPtr;
};

//-----------------------------------------------------------

template <class T>
class TNktComPtr : public TNktComPtrBase<T>
{
public:
  TNktComPtr() : TNktComPtrBase<T>()
    {
    return;
    };

  TNktComPtr(__in T* _lpPtr) : TNktComPtrBase<T>(_lpPtr)
    {
    return;
    };

  TNktComPtr(__in const TNktComPtrBase<T>& _lpPtr) : TNktComPtrBase<T>(_lpPtr.lpPtr)
    {
    return;
    };

  T* operator=(__in T* _lpPtr)
    {
    if (lpPtr != _lpPtr)
    {
      if (_lpPtr != NULL)
        _lpPtr->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = _lpPtr;
    }
    return lpPtr;
    };

  template <typename Q>
  T* operator=(__in const TNktComPtr<Q>& cPtr)
    {
    if (IsEqualObject(cPtr.lpPtr) == FALSE)
    {
      T *lpTemp;

      lpTemp = NULL;
      if (cPtr.lpPtr != NULL)
        cPtr.lpPtr->QueryInterface(__uuidof(T), (void**)&lpTemp);
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = lpTemp;
    }
    return lpPtr;
    };

  T* operator=(__in const TNktComPtr<T>& cPtr)
    {
    if (this != &cPtr)
    {
      if (cPtr.lpPtr != NULL)
        cPtr.lpPtr->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = cPtr.lpPtr;
    }
    return lpPtr;
    };

  TNktComPtr<T>* Clone()
    {
    TNktComPtr<T> *lpNewObj = NKT_MEMMGR_NEW TNktComPtr<T>;
    if (lpNewObj == NULL)
      return NULL;
    lpNewObj->lpPtr = lpPtr;
    if (lpPtr != NULL)
      lpPtr->AddRef();
    slSetLastError(SLERR_NoError);
    return lpNewObj;
    };

  BOOL Clone(__inout TNktComPtr<T> &cTarget)
    {
    if (this != &cTarget)
    {
      if (cTarget.lpPtr != NULL)
        cTarget.lpPtr->Release();
      cTarget.lpPtr = lpPtr;
      if (lpPtr != NULL)
        lpPtr->AddRef();
    }
    slSetLastError(SLERR_NoError);
    return TRUE;
    };
};

//-----------------------------------------------------------
//specialization for IDispatch

class CNktComPtrDispathImpl
{
protected:
  CNktComPtrDispathImpl(__inout IDispatch **lplpPtr);

public:
  __inline HRESULT Invoke(__in DISPID dwDispID, __in UINT nFlags, __in VARTYPE vtRet,
                          __out LPVOID lpRetPtr, __in LPBYTE lpParameters, ...)
    {
    va_list argptr;
    HRESULT hRes;

    va_start(argptr, lpParameters);
    hRes = NktComPtrInvokeHelperV(*lplpPtr, dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
    va_end(argptr);
    return hRes;
    };

  __inline HRESULT InvokeV(__in DISPID dwDispID, __in UINT nFlags, __in VARTYPE vtRet,
                           __out LPVOID lpRetPtr, __in LPBYTE lpParameters, __in va_list argptr)
    {
    return NktComPtrInvokeHelperV(*lplpPtr, dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
    };

  HRESULT InvokeByName(__in_z LPCSTR szName, __in UINT nFlags, __in VARTYPE vtRet, __out LPVOID lpRetPtr,
                       __in LPBYTE lpParameters, ...);
  HRESULT InvokeByName(__in_z LPCWSTR szName, __in UINT nFlags, __in VARTYPE vtRet, __out LPVOID lpRetPtr,
                       __in LPBYTE lpParameters, ...);
  HRESULT InvokeByNameV(__in_z LPCSTR szName, __in UINT nFlags, __in VARTYPE vtRet, __out LPVOID lpRetPtr,
                        __in LPBYTE lpParameters, __in va_list argptr);
  HRESULT InvokeByNameV(__in_z LPCWSTR szName, __in UINT nFlags, __in VARTYPE vtRet, __out LPVOID lpRetPtr,
                        __in LPBYTE lpParameters, __in va_list argptr);

  HRESULT GetProperty(__in DISPID dwDispID, __in VARTYPE vtProp, __out LPVOID lpRetPtr) const;
  HRESULT GetPropertyByName(__in_z LPCSTR szName, __in VARTYPE vtProp, __out LPVOID lpRetPtr) const;
  HRESULT GetPropertyByName(__in_z LPCWSTR szName, __in VARTYPE vtProp, __out LPVOID lpRetPtr) const;

  HRESULT PutProperty(__in DISPID dwDispID, __in VARTYPE vtProp, ...);
  HRESULT PutPropertyByName(__in_z LPCSTR szName, __in VARTYPE vtProp, ...);
  HRESULT PutPropertyByName(__in_z LPCWSTR szName, __in VARTYPE vtProp, ...);

  HRESULT GetIdOfName(__in_z LPCSTR szName, __out DISPID *lpDispId) const;
  HRESULT GetIdOfName(__in_z LPCWSTR szName, __out DISPID *lpDispId) const;

protected:
  IDispatch **lplpPtr;
};

template <>
class TNktComPtr<IDispatch> : public TNktComPtrBase<IDispatch>, public CNktComPtrDispathImpl
{
public:
  TNktComPtr() : TNktComPtrBase<IDispatch>(), CNktComPtrDispathImpl(&lpPtr)
    {
    return;
    };

  TNktComPtr(__in IDispatch* lpDisp) : TNktComPtrBase<IDispatch>(lpDisp), CNktComPtrDispathImpl(&lpPtr)
    {
    return;
    };

  __inline operator LPDISPATCH()
    {
    return lpPtr;
    };

  IDispatch* operator=(__in IDispatch* lpDisp) throw()
    {
    if (lpPtr != lpDisp)
    {
      if (lpDisp != NULL)
        lpDisp->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = lpDisp;
    }
    return lpPtr;
    };

  IDispatch* operator=(__in const TNktComPtr<IDispatch>& cDisp) throw()
    {
    if (this != &cDisp)
    {
      if (cDisp.lpPtr != NULL)
        cDisp.lpPtr->AddRef();
      if (lpPtr != NULL)
        lpPtr->Release();
      lpPtr = cDisp.lpPtr;
    }
    return lpPtr;
    };
};

//-----------------------------------------------------------

class CNktComVariant : public CNktMemMgrObj
{
public:
  CNktComVariant();
  ~CNktComVariant();

  VOID Reset();

  operator VARIANT()
    {
    return sVt;
    };

  VARIANT* operator&()
    {
    return &sVt;
    };

  HRESULT operator=(__in const CNktComVariant &cPtr);
  HRESULT operator=(__in VARIANT _vt);

public:
  VARIANT sVt;
};

//-----------------------------------------------------------

class CNktComBStr : public CNktMemMgrObj
{
public:
  CNktComBStr();
  ~CNktComBStr();

  VOID Reset();

  HRESULT Set(__in_nz_opt LPCSTR szSrc, __in SIZE_T nSrcLen=NKT_SIZE_T_MAX);
  HRESULT Set(__in_nz_opt LPCWSTR szSrc, __in SIZE_T nSrcLen=NKT_SIZE_T_MAX);

  SIZE_T GetLength();

  VOID Attach(__in BSTR bstr);
  BSTR Detach();

  operator BSTR()
    {
    return bstr;
    };

  HRESULT operator=(__in const CNktComBStr &cPtr);
  HRESULT operator=(__in BSTR _bstr);

  //The assert on operator& usually indicates a bug.  If this is really
  //what is needed, however, take the address of the bstr member explicitly.
  BSTR* operator&()
    {
    NKT_ASSERT(bstr == NULL);
    return &bstr;
    };

private:
  BSTR bstr;
};

//-----------------------------------------------------------

class CNktComDll : public CNktMemMgrObj
{
public:
  CNktComDll();
  ~CNktComDll();

  HRESULT Initialize(__in DWORD dwCoInit);

protected:
  HRESULT hRes;
};

//-----------------------------------------------------------

