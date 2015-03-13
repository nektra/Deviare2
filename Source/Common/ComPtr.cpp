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

#include "ComPtr.h"
#include <ocidl.h>
#include <crtdbg.h>

//-----------------------------------------------------------

#ifndef NKT_VTS_MFCBYREF
  #define NKT_VTS_MFCBYREF 0x40
#endif //NKT_VTS_MFCBYREF

//-----------------------------------------------------------

static LPWSTR Ansi2Wide(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen);

//-----------------------------------------------------------

HRESULT NktComPtrAdvise(__in IUnknown* lpUnkCP, __in IUnknown* lpUnk, __in const IID& sIid,
                        __out LPDWORD lpdwCookie)
{
  TNktComPtrBase<IConnectionPointContainer> sCPC;
  TNktComPtrBase<IConnectionPoint> sCP;
  HRESULT hRes;

  if (lpUnkCP == NULL)
    return E_INVALIDARG;
  hRes = lpUnkCP->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&sCPC);
  if (SUCCEEDED(hRes))
    hRes = sCPC->FindConnectionPoint(sIid, &sCP);
  if (SUCCEEDED(hRes))
    hRes = sCP->Advise(lpUnk, lpdwCookie);
  return hRes;
}

HRESULT NktComPtrUnadvise(__in IUnknown *lpUnkCP, __in const IID& sIid, __in DWORD dwCookie)
{
  TNktComPtrBase<IConnectionPointContainer> sCPC;
  TNktComPtrBase<IConnectionPoint> sCP;
  HRESULT hRes;

  if (lpUnkCP == NULL)
    return E_INVALIDARG;
  hRes = lpUnkCP->QueryInterface(__uuidof(IConnectionPointContainer), (void**)&sCPC);
  if (SUCCEEDED(hRes))
    hRes = sCPC->FindConnectionPoint(sIid, &sCP);
  if (SUCCEEDED(hRes))
    hRes = sCP->Unadvise(dwCookie);
  return hRes;
}

HRESULT NktComPtrInvokeHelper(__in IDispatch *lpDisp, __in DISPID dwDispID, __in UINT nFlags,
                              __in VARTYPE vtRet, __out LPVOID lpRetPtr, __in LPBYTE lpParameters, ...)
{
  HRESULT hRes;
  va_list argptr;

  va_start(argptr, lpParameters);
  hRes = NktComPtrInvokeHelperV(lpDisp, dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
  va_end(argptr);
  return hRes;
}

HRESULT NktComPtrInvokeHelperV(__in IDispatch *lpDisp, __in DISPID dwDispID, __in UINT nFlags,
                               __in VARTYPE vtRet, __out LPVOID lpRetPtr, __in LPBYTE lpParameters,
                               __in va_list argptr)
{
  DISPPARAMS sDispParams;
  DISPID nDispIdNamed;
  LPBYTE lpCurrParam;
  LPCOLESTR lpOleStr;
  VARIANT sVarResult, *lpVarResult, *lpArg;
  LPSTR lpSzA;
  UINT nArgErr;
  EXCEPINFO sExcepInfo;
  HRESULT hRes;

  if (lpDisp == NULL)
    return E_FAIL;
  nktMemSet(&sDispParams, 0, sizeof(sDispParams));
  // determine number of arguments
  if (lpParameters != NULL)
    sDispParams.cArgs = (UINT)strlen((LPCSTR)lpParameters);
  nDispIdNamed = DISPID_PROPERTYPUT;
  if ((nFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF)) != 0)
  {
    if (sDispParams.cArgs == 0)
      return E_INVALIDARG;
    sDispParams.cNamedArgs = 1;
    sDispParams.rgdispidNamedArgs = &nDispIdNamed;
  }
  //allocate memory for all VARIANT parameters
  if (sDispParams.cArgs != 0)
  {
    sDispParams.rgvarg = (VARIANTARG*)nktMemMalloc((size_t)sDispParams.cArgs * sizeof(VARIANTARG));
    if (sDispParams.rgvarg == NULL)
      return E_OUTOFMEMORY;
    nktMemSet(sDispParams.rgvarg, 0, (size_t)sDispParams.cArgs * sizeof(VARIANTARG));
    // get ready to walk vararg list
    lpCurrParam = (LPBYTE)lpParameters;
    lpArg = sDispParams.rgvarg + (sDispParams.cArgs - 1); // params go in opposite order
    while (*lpCurrParam != 0)
    {
      NKT_ASSERT(lpArg >= sDispParams.rgvarg);
      lpArg->vt = (VARTYPE)(*lpCurrParam); // set the variant type
      if ((lpArg->vt & NKT_VTS_BYREF) != 0)
      {
        lpArg->vt &= ~NKT_VTS_BYREF;
        lpArg->vt |= VT_BYREF;
      }
      switch (lpArg->vt)
      {
        case VT_UI1:
          lpArg->bVal = va_arg(argptr, BYTE);
          break;
        case VT_UI2:
          lpArg->uiVal = va_arg(argptr, USHORT);
          break;
        case VT_UI4:
          lpArg->ulVal = va_arg(argptr, ULONG);
          break;
        case VT_UI8:
          lpArg->ullVal = va_arg(argptr, ULONGLONG);
          break;
        case VT_I1:
          lpArg->cVal = va_arg(argptr, CHAR);
          break;
        case VT_I2:
          lpArg->iVal = va_arg(argptr, SHORT);
          break;
        case VT_I4:
          lpArg->lVal = va_arg(argptr, LONG);
          break;
        case VT_I8:
          lpArg->llVal = va_arg(argptr, LONGLONG);
          break;
        case VT_R4:
          lpArg->fltVal = va_arg(argptr, float);
          break;
        case VT_R8:
          lpArg->dblVal = va_arg(argptr, double);
          break;
        case VT_DATE:
          lpArg->date = va_arg(argptr, DATE);
          break;
        case VT_CY: //CY is always passed by ref
          lpArg->cyVal = *va_arg(argptr, CY*);
          break;
        case VT_BSTR:
          lpOleStr = va_arg(argptr, LPOLESTR);
          lpArg->bstrVal = NULL;
          if (lpOleStr != NULL)
          {
            lpArg->bstrVal = ::SysAllocString(lpOleStr);
            if (lpArg->bstrVal == NULL)
            {
              nktMemFree(sDispParams.rgvarg);
              return E_OUTOFMEMORY;
            }
          }
          break;
        case VT_BSTRA:
          lpSzA = va_arg(argptr, LPSTR);
          lpArg->vt = VT_BSTR;
          lpArg->bstrVal = NULL;
          if (lpSzA != NULL)
          {
            lpOleStr = Ansi2Wide(lpSzA, NKT_SIZE_T_MAX);
            if (lpOleStr == NULL)
            {
              nktMemFree(sDispParams.rgvarg);
              return E_OUTOFMEMORY;
            }
            lpArg->bstrVal = ::SysAllocString(lpOleStr);
            nktMemFree((void*)lpOleStr);
            if (lpArg->bstrVal == NULL)
            {
              nktMemFree(sDispParams.rgvarg);
              return E_OUTOFMEMORY;
            }
          }
          break;
        case VT_DISPATCH:
          lpArg->pdispVal = va_arg(argptr, LPDISPATCH);
          break;
        case VT_ERROR:
          lpArg->scode = va_arg(argptr, SCODE);
          break;
        case VT_BOOL:
          lpArg->boolVal = va_arg(argptr, VARIANT_BOOL);
          break;
        case VT_VARIANT: //VARIANT is always passed by ref
          *lpArg = *va_arg(argptr, VARIANT*);
          break;
        case VT_UNKNOWN:
          lpArg->punkVal = va_arg(argptr, LPUNKNOWN);
          break;
        case VT_UI1|VT_BYREF:
          lpArg->pbVal = va_arg(argptr, LPBYTE);
          break;
        case VT_UI2|VT_BYREF:
          lpArg->puiVal = va_arg(argptr, USHORT*);
          break;
        case VT_UI4|VT_BYREF:
          lpArg->pulVal = va_arg(argptr, ULONG*);
          break;
        case VT_UI8|VT_BYREF:
          lpArg->pullVal = va_arg(argptr, ULONGLONG*);
          break;
        case VT_I1|VT_BYREF:
          lpArg->pcVal = va_arg(argptr, CHAR*);
          break;
        case VT_I2|VT_BYREF:
          lpArg->piVal = va_arg(argptr, SHORT*);
          break;
        case VT_I4|VT_BYREF:
          lpArg->plVal = va_arg(argptr, LONG*);
          break;
        case VT_I8|VT_BYREF:
          lpArg->pllVal = va_arg(argptr, LONGLONG*);
          break;
        case VT_R4|VT_BYREF:
          lpArg->pfltVal = va_arg(argptr, float*);
          break;
        case VT_R8|VT_BYREF:
          lpArg->pdblVal = va_arg(argptr, double*);
          break;
        case VT_DATE|VT_BYREF:
          lpArg->pdate = va_arg(argptr, DATE*);
          break;
        case VT_CY|VT_BYREF:
          lpArg->pcyVal = va_arg(argptr, CY*);
          break;
        case VT_BSTR|VT_BYREF:
          lpArg->pbstrVal = va_arg(argptr, BSTR*);
          break;
        case VT_DISPATCH|VT_BYREF:
          lpArg->ppdispVal = va_arg(argptr, LPDISPATCH*);
          break;
        case VT_ERROR|VT_BYREF:
          lpArg->pscode = va_arg(argptr, SCODE*);
          break;
        case VT_BOOL|VT_BYREF:
          lpArg->pboolVal = va_arg(argptr, VARIANT_BOOL*);
          break;
        case VT_VARIANT|VT_BYREF:
          lpArg->pvarVal = va_arg(argptr, VARIANT*);
          break;
        case VT_UNKNOWN|VT_BYREF:
          lpArg->ppunkVal = va_arg(argptr, LPUNKNOWN*);
          break;
        default:
          NKT_ASSERT(FALSE);  // unknown type!
          return E_INVALIDARG;
      }
      lpArg--;
      lpCurrParam++;
    }
  }
  // initialize return value
  lpVarResult = NULL;
  nktMemSet(&sVarResult, 0, sizeof(sVarResult));
  if (vtRet != VT_EMPTY)
    lpVarResult = &sVarResult;
  nArgErr = (UINT)-1;  // initialize to invalid arg
  nktMemSet(&sExcepInfo, 0, sizeof(sExcepInfo));
  // make the call
  hRes = lpDisp->Invoke(dwDispID, IID_NULL, 0, (WORD)(nFlags & 0xFFFF), &sDispParams, lpVarResult,
                        &sExcepInfo, &nArgErr);
  // cleanup any arguments that need cleanup
  if (sDispParams.cArgs != 0)
  {
    lpArg = sDispParams.rgvarg + sDispParams.cArgs - 1;
    lpCurrParam = (LPBYTE)lpParameters;
    while (*lpCurrParam != 0)
    {
      switch ((VARTYPE)(*lpCurrParam))
      {
        case VT_BSTRA:
        case VT_BSTR:
          ::VariantClear(lpArg);
          break;
      }
      lpArg--;
      lpCurrParam++;
    }
    nktMemFree(sDispParams.rgvarg);
  }
  //check for failure
  if (FAILED(hRes))
  {
    ::VariantClear(&sVarResult);
    if (hRes == DISP_E_EXCEPTION)
    {
      //exception error code
      // make sure sExcepInfo is filled in
      if (sExcepInfo.pfnDeferredFillIn != NULL)
        sExcepInfo.pfnDeferredFillIn(&sExcepInfo);
      ::SysFreeString(sExcepInfo.bstrSource);
      ::SysFreeString(sExcepInfo.bstrDescription);
      ::SysFreeString(sExcepInfo.bstrHelpFile);
      hRes = (HRESULT)sExcepInfo.scode;
    }
  }
  //convert return value if needed
  if (vtRet != VT_EMPTY)
  {
    if (vtRet != VT_VARIANT)
    {
      HRESULT hRes2;

      hRes2 = ::VariantChangeType(&sVarResult, &sVarResult, 0, vtRet);
      if (FAILED(hRes2))
      {
        ::VariantClear(&sVarResult);
        hRes = hRes2;
      }
#ifdef _DEBUG
      else
      {
        NKT_ASSERT(vtRet == sVarResult.vt);
      }
#endif //_DEBUG
    }
    // copy return value into return spot!
    switch (vtRet)
    {
      case VT_UI1:
        *((LPBYTE)lpRetPtr) = sVarResult.bVal;
        break;
      case VT_UI2:
        *((USHORT*)lpRetPtr) = sVarResult.uiVal;
        break;
      case VT_UI4:
        *((ULONG*)lpRetPtr) = sVarResult.ulVal;
        break;
      case VT_UI8:
        *((ULONGLONG*)lpRetPtr) = sVarResult.ullVal;
        break;
      case VT_I1:
        *((CHAR*)lpRetPtr) = sVarResult.cVal;
        break;
      case VT_I2:
        *((SHORT*)lpRetPtr) = sVarResult.iVal;
        break;
      case VT_I4:
        *((LONG*)lpRetPtr) = sVarResult.lVal;
        break;
      case VT_I8:
        *((LONGLONG*)lpRetPtr) = sVarResult.llVal;
        break;
      case VT_R4:
        *((float*)lpRetPtr) = sVarResult.fltVal;
        break;
      case VT_R8:
        *((double*)lpRetPtr) = sVarResult.dblVal;
        break;
      case VT_DATE:
        *((DATE*)lpRetPtr) = sVarResult.date;
        break;
      case VT_CY:
        *((CY*)lpRetPtr) = sVarResult.cyVal;
        break;
      case VT_BSTR:
        *((BSTR*)lpRetPtr) = sVarResult.bstrVal;
        break;
      case VT_DISPATCH:
        *((LPDISPATCH*)lpRetPtr) = sVarResult.pdispVal;
        break;
      case VT_ERROR:
        *((SCODE*)lpRetPtr) = sVarResult.scode;
        break;
      case VT_BOOL:
        *((LPBOOL)lpRetPtr) = (sVarResult.boolVal != 0) ? TRUE : FALSE;
        break;
      case VT_VARIANT:
        *((VARIANT*)lpRetPtr) = sVarResult;
        break;
      case VT_UNKNOWN:
        *((LPUNKNOWN*)lpRetPtr) = sVarResult.punkVal;
        break;
      default:
        ::VariantClear(&sVarResult);
        hRes = E_INVALIDARG; // invalid return type specified
        break;
    }
  }
  return hRes;
}

//-----------------------------------------------------------

CNktComPtrDispathImpl::CNktComPtrDispathImpl(__inout IDispatch **_lplpPtr)
{
  NKT_ASSERT(_lplpPtr != NULL);
  lplpPtr = _lplpPtr;
  return;
}

HRESULT CNktComPtrDispathImpl::InvokeByName(__in_z LPCSTR szName, __in UINT nFlags, __in VARTYPE vtRet,
                                            __out LPVOID lpRetPtr, __in LPBYTE lpParameters, ...)
{
  va_list argptr;
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
  {
    va_start(argptr, lpParameters);
    hRes = InvokeV(dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
    va_end(argptr);
  }
  return hRes;
}

HRESULT CNktComPtrDispathImpl::InvokeByName(__in_z LPCWSTR szName, __in UINT nFlags, __in VARTYPE vtRet,
                                            __out LPVOID lpRetPtr, __in LPBYTE lpParameters, ...)
{
  va_list argptr;
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
  {
    va_start(argptr, lpParameters);
    hRes = InvokeV(dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
    va_end(argptr);
  }
  return hRes;
}

HRESULT CNktComPtrDispathImpl::InvokeByNameV(__in_z LPCSTR szName, __in UINT nFlags, __in VARTYPE vtRet,
                                             __out LPVOID lpRetPtr, __in LPBYTE lpParameters,
                                             __in va_list argptr)
{
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
    hRes = InvokeV(dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::InvokeByNameV(__in_z LPCWSTR szName, __in UINT nFlags, __in VARTYPE vtRet,
                                             __out LPVOID lpRetPtr, __in LPBYTE lpParameters,
                                             __in va_list argptr)
{
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
    hRes = InvokeV(dwDispID, nFlags, vtRet, lpRetPtr, lpParameters, argptr);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::GetProperty(__in DISPID dwDispID, __in VARTYPE vtProp,
                                           __out LPVOID lpRetPtr) const
{
  return NktComPtrInvokeHelper(*lplpPtr, dwDispID, DISPATCH_PROPERTYGET, vtProp, lpRetPtr, NULL);
}

HRESULT CNktComPtrDispathImpl::GetPropertyByName(__in_z LPCSTR szName, __in VARTYPE vtProp,
                                                 __out LPVOID lpRetPtr) const
{
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
    hRes = GetProperty(dwDispID, vtProp, lpRetPtr);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::GetPropertyByName(__in_z LPCWSTR szName, __in VARTYPE vtProp,
                                                 __out LPVOID lpRetPtr) const
{
  DISPID dwDispID;
  HRESULT hRes;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
    hRes = GetProperty(dwDispID, vtProp, lpRetPtr);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::PutProperty(__in DISPID dwDispID, __in VARTYPE vtProp, ...)
{
  va_list argptr;
  BYTE aParams[2];
  UINT nFlags;
  HRESULT hRes;

  va_start(argptr, vtProp);
  if ((vtProp & VT_BYREF) != 0)
  {
    vtProp &= ~VT_BYREF;
    vtProp |= NKT_VTS_BYREF;
  }
  aParams[0] = (BYTE)vtProp;
  aParams[1] = 0;
  nFlags = (vtProp == VT_DISPATCH) ? DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT;
  hRes = NktComPtrInvokeHelperV(*lplpPtr, dwDispID, nFlags, VT_EMPTY, NULL, aParams, argptr);
  va_end(argptr);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::PutPropertyByName(__in_z LPCSTR szName, __in VARTYPE vtProp, ...)
{
  DISPID dwDispID;
  HRESULT hRes;
  va_list argptr;
  BYTE aParams[2];
  UINT nFlags;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
  {
    va_start(argptr, vtProp);
    if ((vtProp & VT_BYREF) != 0)
    {
      vtProp &= ~VT_BYREF;
      vtProp |= NKT_VTS_BYREF;
    }
    aParams[0] = (BYTE)vtProp;
    aParams[1] = 0;
    nFlags = (vtProp == VT_DISPATCH) ? DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT;
    hRes = NktComPtrInvokeHelper(*lplpPtr, dwDispID, nFlags, VT_EMPTY, NULL, aParams, argptr);
    va_end(argptr);
  }
  return hRes;
}

HRESULT CNktComPtrDispathImpl::PutPropertyByName(__in_z LPCWSTR szName, __in VARTYPE vtProp, ...)
{
  DISPID dwDispID;
  HRESULT hRes;
  va_list argptr;
  BYTE aParams[2];
  UINT nFlags;

  hRes = GetIdOfName(szName, &dwDispID);
  if (SUCCEEDED(hRes))
  {
    va_start(argptr, vtProp);
    if ((vtProp & VT_BYREF) != 0)
    {
      vtProp &= ~VT_BYREF;
      vtProp |= NKT_VTS_BYREF;
    }
    aParams[0] = (BYTE)vtProp;
    aParams[1] = 0;
    nFlags = (vtProp == VT_DISPATCH) ? DISPATCH_PROPERTYPUTREF : DISPATCH_PROPERTYPUT;
    hRes = NktComPtrInvokeHelperV(*lplpPtr, dwDispID, nFlags, VT_EMPTY, NULL, aParams, argptr);
    va_end(argptr);
  }
  return hRes;
}

HRESULT CNktComPtrDispathImpl::GetIdOfName(__in_z LPCSTR szName, __out DISPID *lpDispId) const
{
  HRESULT hRes;
  LPWSTR sW;

  if (*lplpPtr == NULL)
    return E_FAIL;
  if (szName == NULL)
    return E_INVALIDARG;
  sW = Ansi2Wide(szName, NKT_SIZE_T_MAX);
  if (sW == NULL)
    return E_OUTOFMEMORY;
  hRes = (*lplpPtr)->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&sW), 1, 0, lpDispId);
  nktMemFree(sW);
  return hRes;
}

HRESULT CNktComPtrDispathImpl::GetIdOfName(__in_z LPCWSTR szName, __out DISPID *lpDispId) const
{
  if (*lplpPtr == NULL)
    return E_FAIL;
  if (szName == NULL)
    return E_INVALIDARG;
  return (*lplpPtr)->GetIDsOfNames(IID_NULL, const_cast<LPOLESTR*>(&szName), 1, 0, lpDispId);
}

//-----------------------------------------------------------

CNktComVariant::CNktComVariant() : CNktMemMgrObj()
{
  ::VariantInit(&sVt);
  return;
}

CNktComVariant::~CNktComVariant()
{
  ::VariantClear(&sVt);
  return;
}

VOID CNktComVariant::Reset()
{
  ::VariantClear(&sVt);
  return;
}

HRESULT CNktComVariant::operator=(__in const CNktComVariant &cPtr)
{
  return operator=(cPtr.sVt);
}

HRESULT CNktComVariant::operator=(__in VARIANT _vt)
{
  ::VariantClear(&sVt);
  return ::VariantCopy((VARIANTARG*)&sVt, (VARIANTARG*)&_vt);
}

//-----------------------------------------------------------

CNktComBStr::CNktComBStr() : CNktMemMgrObj()
{
  bstr = NULL;
  return;
}

CNktComBStr::~CNktComBStr()
{
  if (bstr != NULL)
    ::SysFreeString(bstr);
  return;
}

VOID CNktComBStr::Reset()
{
  if (bstr != NULL)
    ::SysFreeString(bstr);
  bstr = NULL;
  return;
}

HRESULT CNktComBStr::Set(__in_nz_opt LPCSTR szSrc, __in SIZE_T nSrcLen)
{
  LPWSTR sW;

  Reset();
  if (szSrc == NULL)
  {
    if (nSrcLen==0 || nSrcLen==NKT_SIZE_T_MAX)
      return S_OK;
    return E_INVALIDARG;
  }
  if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = strlen(szSrc);
  sW = Ansi2Wide(szSrc, nSrcLen);
  if (sW == NULL)
    return E_OUTOFMEMORY;
  bstr = ::SysAllocString(sW);
  nktMemFree(sW);
  return (bstr != NULL) ? S_OK : E_OUTOFMEMORY;
}

HRESULT CNktComBStr::Set(__in_nz_opt LPCWSTR szSrc, __in SIZE_T nSrcLen)
{
  Reset();
  if (szSrc == NULL)
  {
    if (nSrcLen != 0 && nSrcLen != NKT_SIZE_T_MAX)
      return E_INVALIDARG;
    nSrcLen = 0;
  }
  else if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = wcslen(szSrc);
  if (nSrcLen > 0)
    bstr = ::SysAllocStringLen(szSrc, (UINT)nSrcLen);
  else
    bstr = ::SysAllocString(L"");
  return (bstr != NULL) ? S_OK : E_OUTOFMEMORY;
}

SIZE_T CNktComBStr::GetLength()
{
  return (bstr != NULL) ? ::SysStringLen(bstr) : 0;
}

VOID CNktComBStr::Attach(BSTR _bstr)
{
  Reset();
  bstr = _bstr;
  return;
}

BSTR CNktComBStr::Detach()
{
  BSTR bstr_ret;

  bstr_ret = bstr;
  bstr = NULL;
  return bstr_ret;
}

HRESULT CNktComBStr::operator=(const CNktComBStr &cPtr)
{
  return operator=(cPtr.bstr);
}

HRESULT CNktComBStr::operator=(BSTR _bstr)
{
  Reset();
  if (_bstr != NULL)
  {
    SIZE_T nSrcLen;

    nSrcLen = (SIZE_T)::SysStringLen(_bstr);
    bstr = ::SysAllocStringLen((LPOLESTR)_bstr, (UINT)nSrcLen);
    if (bstr == NULL)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

//-----------------------------------------------------------

CNktComDll::CNktComDll() : CNktMemMgrObj()
{
  hRes = CO_E_NOTINITIALIZED;
  return;
}

CNktComDll::~CNktComDll()
{
  if (SUCCEEDED(hRes))
  {
    ::CoUninitialize();
    hRes = CO_E_NOTINITIALIZED;
  }
  return;
}

HRESULT CNktComDll::Initialize(__in DWORD dwCoInit)
{
  if (FAILED(hRes))
    hRes = ::CoInitializeEx(NULL, dwCoInit);
  return hRes;
}

//-----------------------------------------------------------

static LPWSTR Ansi2Wide(__in_nz_opt LPCSTR szSrcA, __in SIZE_T nSrcLen)
{
  SIZE_T nDestLen;
  LPWSTR szDestW;

  if (szSrcA == NULL)
    szSrcA = "";
  if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = strlen(szSrcA);
  nDestLen = (size_t)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, NULL, 0);
  szDestW = (LPWSTR)nktMemMalloc((nDestLen+1)*sizeof(WCHAR));
  if (szDestW == NULL)
    return NULL;
  nDestLen = (size_t)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSrcA, (int)nSrcLen, szDestW, (int)(nDestLen+1));
  szDestW[nDestLen] = 0;
  return szDestW;
}
