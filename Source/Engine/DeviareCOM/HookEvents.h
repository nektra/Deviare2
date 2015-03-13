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

//-----------------------------------------------------------

template <class T>
class ATL_NO_VTABLE CNktHookEventsImpl : public IDelayableDynamicConnectionPoint<T, DNktHookEvents>
{
public:
  HRESULT Fire_OnStateChanged(__in INktHook *lpHook, __in INktProcess *lpProc, __in eNktHookState nNewState,
                              __in eNktHookState nOldState)
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
          hRes = FireCommon(cConnDisp, dispidNktHookEventsOnStateChanged, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
        }
        else
        {
          hRes = FireWithDelay(cConnDisp, dispidNktHookEventsOnStateChanged, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
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
        hRes = FireCommon(cConnDisp, dispidNktHookEventsOnFunctionCalled, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
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
        hRes = FireCommon(cConnDisp, dispidNktHookEventsOnHookOverwritten, aVarParams, NKT_DV_ARRAYLEN(aVarParams));
      }
    }
    return S_OK;
    };
};
