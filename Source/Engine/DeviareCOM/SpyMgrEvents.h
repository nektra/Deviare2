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

#include "DelayableDynamicConnectionPoint.h"
#include "idl_csharp_support.h"

//-----------------------------------------------------------

template <class T>
class ATL_NO_VTABLE CNktSpyMgrEventsImpl : public IDelayableDynamicConnectionPoint<T, DNktSpyMgrEvents>
{
public:
  HRESULT Fire_OnCustomDllLoad(__in INktProcess *lpProc, __in LONG nActionId, __in LONG hActionResult)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[3];

        aVarParams[2] = lpProc;
        aVarParams[1] = nActionId;
        aVarParams[0] = hActionResult;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCustomDllLoad, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnCustomDllUnload(__in INktProcess *lpProc, __in LONG nActionId, __in LONG hActionResult)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[3];

        aVarParams[2] = lpProc;
        aVarParams[1] = nActionId;
        aVarParams[0] = hActionResult;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCustomDllUnload, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnCustomApiCall(__in INktProcess *lpProc, __in LONG nActionId, __in LONG hActionResult,
                               __in LONG nApiResult)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[4];

        aVarParams[3] = lpProc;
        aVarParams[2] = nActionId;
        aVarParams[1] = hActionResult;
        aVarParams[0] = nApiResult;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCustomApiCall, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnHookStateChanged(__in INktHook *lpHook, __in INktProcess *lpProc,
                                  __in eNktHookState nNewState, __in eNktHookState nOldState)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes, hThreadRes;
    int i;

    //this event may be fired from an sta thread
    hThreadRes = IsMultiThreadApartment();
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[4];

        aVarParams[3] = lpHook;
        aVarParams[2] = lpProc;
        aVarParams[1] = (LONG)nNewState;
        aVarParams[0] = (LONG)nOldState;
        if (hThreadRes != S_FALSE)
        {
          hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnHookStateChanged, aVarParams,
                            NKT_DV_ARRAYLEN(aVarParams));
        }
        else
        {
          hRes = FireWithDelay(cConnDisp, dispidNktSpyMgrEventsOnHookStateChanged, aVarParams,
                               NKT_DV_ARRAYLEN(aVarParams));
        }
        if (hRes == E_OUTOFMEMORY)
          return hRes;
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnFunctionCalled(__in INktHook *lpHook, __in INktProcess *lpProc, __in INktHookCallInfo *lpCallInfo)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[3];

        aVarParams[2] = lpHook;
        aVarParams[1] = lpProc;
        aVarParams[0] = lpCallInfo;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnFunctionCalled, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnCreateProcessCall(__in INktProcess *lpProc, __in LONG dwChildPid, __in LONG dwMainThreadId,
                                   __in BOOL bIs64BitProcess, __in BOOL bCanHookNow)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[5];

        aVarParams[4] = lpProc;
        aVarParams[3] = dwChildPid;
        aVarParams[2] = dwMainThreadId;
        aVarParams[1].vt = VT_BOOL;
        aVarParams[1].boolVal = (bIs64BitProcess != FALSE) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
        aVarParams[0].vt = VT_BOOL;
        aVarParams[0].boolVal = (bCanHookNow != FALSE) ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCreateProcessCall, aVarParams,
                          NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnLoadLibraryCall(__in INktProcess *lpProc, __in LPCWSTR szLibraryNameW, __in VARIANT vtModule)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[3];

        aVarParams[2] = lpProc;
        aVarParams[1] = szLibraryNameW;
        if (aVarParams[1].vt != VT_BSTR)
        {
          //no exception is thrown because _ATL_NO_VARIANT_THROW is defined in stdafx.h
          return E_OUTOFMEMORY;
        }
        hRes = aVarParams[0].Copy(&vtModule);
        if (FAILED(hRes))
          return hRes;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnLoadLibraryCall, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnLoadLibraryCall2(__in INktProcess *lpProc, __in LPCWSTR szLibraryNameW, __in my_ssize_t nModule)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[3];

        aVarParams[2] = lpProc;
        aVarParams[1] = szLibraryNameW;
        if (aVarParams[1].vt != VT_BSTR)
        {
          //no exception is thrown because _ATL_NO_VARIANT_THROW is defined in stdafx.h
          return E_OUTOFMEMORY;
        }
#if defined _M_IX86
        aVarParams[0].vt = VT_INT;
        aVarParams[0].intVal = (INT)nModule;
#elif defined _M_X64
        aVarParams[0].vt = VT_I8;
        aVarParams[0].llVal = (LONGLONG)nModule;
#endif
        /*
        aVarParams[0].vt = VT_VOID;
        aVarParams[0].byref = (PVOID)nModule;
        */
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnLoadLibraryCall2, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnFreeLibraryCall(__in INktProcess *lpProc, __in VARIANT vtModule)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[2];

        aVarParams[1] = lpProc;
        hRes = aVarParams[0].Copy(&vtModule);
        if (FAILED(hRes))
          return hRes;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnFreeLibraryCall, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnFreeLibraryCall2(__in INktProcess *lpProc, __in my_ssize_t nModule)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[2];

        aVarParams[1] = lpProc;
#if defined _M_IX86
        aVarParams[0].vt = VT_INT;
        aVarParams[0].intVal = (INT)nModule;
#elif defined _M_X64
        aVarParams[0].vt = VT_I8;
        aVarParams[0].llVal = (LONGLONG)nModule;
#endif
        /*
        aVarParams[0].vt = VT_VOID;
        aVarParams[0].byref = (PVOID)nModule;
        */
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnFreeLibraryCall2, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnProcessStarted(__in INktProcess *lpProc)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[1];

        aVarParams[0] = lpProc;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnProcessStarted, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnProcessTerminated(__in INktProcess *lpProc)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[1];

        aVarParams[0] = lpProc;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnProcessTerminated, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnAgentLoad(__in INktProcess *lpProc, __in LONG hErrorCode)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes, hThreadRes;
    int i;

    //this event may be fired from an sta thread
    hThreadRes = IsMultiThreadApartment();
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[2];

        aVarParams[1] = lpProc;
        aVarParams[0] = (LONG)hErrorCode;
        if (hThreadRes != S_FALSE)
          hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnAgentLoad, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
        else
          hRes = FireWithDelay(cConnDisp, dispidNktSpyMgrEventsOnAgentLoad, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
        if (hRes == E_OUTOFMEMORY)
          return hRes;
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnAgentUnload(__in INktProcess *lpProc)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[1];

        aVarParams[0] = lpProc;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnAgentUnload, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnCustomMessage(__in INktProcess *lpProc, __in VARIANT vtMsgCode, __in VARIANT vtMsgParam,
                               __inout VARIANT *vtRetVal)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[4];

        aVarParams[3] = lpProc;
        hRes = aVarParams[2].Copy(&vtMsgCode);
        if (SUCCEEDED(hRes))
          hRes = aVarParams[1].Copy(&vtMsgParam);
        if (FAILED(hRes))
          return hRes;
        aVarParams[0].vt = VT_BYREF | VT_VARIANT;
        aVarParams[0].pvarVal = vtRetVal;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCustomMessage, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnCustomMessage2(__in INktProcess *lpProc, __in my_ssize_t nMsgCode, __in my_ssize_t nMsgParam,
                                __inout my_ssize_t *nRetVal)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[4];

        aVarParams[3] = lpProc;
#if defined _M_IX86
        aVarParams[2].vt = VT_INT;
        aVarParams[2].intVal = (INT)nMsgCode;
        aVarParams[1].vt = VT_INT;
        aVarParams[1].intVal = (INT)nMsgParam;
        aVarParams[0].vt = VT_INT|VT_BYREF;
        aVarParams[0].pintVal = (INT*)nRetVal;
#elif defined _M_X64
        aVarParams[2].vt = VT_I8;
        aVarParams[2].llVal = (LONGLONG)nMsgCode;
        aVarParams[1].vt = VT_I8;
        aVarParams[1].llVal = (LONGLONG)nMsgParam;
        aVarParams[0].vt = VT_I8|VT_BYREF;
        aVarParams[0].pllVal = (LONGLONG*)nRetVal;
#endif
        /*
        aVarParams[2].vt = VT_VOID;
        aVarParams[2].byref = (PVOID)nMsgCode;
        aVarParams[1].vt = VT_VOID;
        aVarParams[1].byref = (PVOID)nMsgParam;
        aVarParams[0].vt = VT_VOID;
        aVarParams[0].byref = (PVOID)(*nRetVal);
        */
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnCustomMessage2, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
        /*
        if (SUCCEEDED(hRes))
          *nRetVal = (my_ssize_t)(aVarParams[0].byref); //TO-CHECK
        */
      }
    }
    return S_OK;
    };

  HRESULT Fire_OnHookOverwritten(__in INktHook *lpHook, __in INktProcess *lpProc)
    {
    TNktComPtr<IDispatch> cConnDisp;
    HRESULT hRes;
    int i;

    //this event is always called from a secondary thread
    for (i=0; ; i++)
    {
      cConnDisp.Release();
      hRes = GetConnAt(&cConnDisp, i);
      if (hRes == E_INVALIDARG)
        break;
      if (SUCCEEDED(hRes))
      {
        CComVariant aVarParams[2];

        aVarParams[1] = lpHook;
        aVarParams[0] = lpProc;
        hRes = FireCommon(cConnDisp, dispidNktSpyMgrEventsOnHookOverwritten, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };
};
