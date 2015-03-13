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

#include "StdAfx.h"
#include "Tools.h"
#include "PdbFunctionSymbol.h"

//-----------------------------------------------------------
// CNktToolsImpl

HRESULT CNktToolsImpl::ExtractIcon(__in BSTR executablePath, __in LONG index,
                                   __deref_out IPicture **ppIcon)
{
  HRESULT hRes;

  if (ppIcon == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_POINTER);
  *ppIcon = NULL;
  if (executablePath == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_POINTER);
  if (executablePath[0] == 0 || index < 0)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_INVALIDARG);
  hRes = CNktDvTools::_ExtractIcon(ppIcon, executablePath, index);
  return ::SetupErrorInfoAndReturn(IID_INktTools, hRes);
}

HRESULT CNktToolsImpl::GetFileNameFromHandle(__in my_ssize_t _handle, __in VARIANT procOrId,
                                             __deref_out BSTR *pVal)
{
  CNktStringW cStrFileNameW;
  TNktComPtr<CNktDvProcess> cProc;
  VARIANT *lpVar;
  DWORD dwPid;
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_POINTER);
  if (_handle == NULL || (HANDLE)_handle == INVALID_HANDLE_VALUE)
  {
    *pVal = NULL;
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_INVALIDARG);
  }
  //check procOrId param
  lpVar = RealVariant_From_Variant(&procOrId);
  if (lpVar->vt == VT_EMPTY || lpVar->vt == VT_NULL)
  {
    dwPid = 0;
    hRes = S_OK;
  }
  else
  {
    hRes = ProcId_From_Variant(&cProc, &dwPid, lpVar);
    if (SUCCEEDED(hRes))
    {
      if (dwPid == 0 && cProc != NULL)
        dwPid = cProc->GetProcessId();
    }
  }
  if (SUCCEEDED(hRes))
    hRes = CNktDvTools::GetFileNameFromHandle(cStrFileNameW, (HANDLE)_handle, dwPid);
  if (SUCCEEDED(hRes))
    hRes = cComBStr.Set((LPWSTR)cStrFileNameW);
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  *pVal = cComBStr.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktTools, hRes);
}

HRESULT CNktToolsImpl::LocateFunctionSymbolInPdb(__in BSTR fileName, __in BSTR functionName,
                                                 __in BSTR symbolServerPath, __in BSTR localCachePath,
                                                 __deref_out INktPdbFunctionSymbol **ppSymbol)
{
  TNktComPtr<CNktDvPdbFunctionSymbol> cSymbol;
  TNktComPtr<INktPdbFunctionSymbol> cISymbol;
  HRESULT hRes;

  if (ppSymbol == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_POINTER);
  *ppSymbol = NULL;
  if (fileName == NULL || functionName == NULL)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_POINTER);
  if (fileName[0] == 0 || functionName[0] == 0)
    return ::SetupErrorInfoAndReturn(IID_INktTools, E_INVALIDARG);
  hRes = CNktDvPdbFunctionSymbol::Create(fileName, functionName, symbolServerPath, localCachePath,
                                         &cSymbol);
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktPdbFunctionSymbolImpl,INktPdbFunctionSymbol,
                            CNktDvPdbFunctionSymbol>(&cISymbol, cSymbol, InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppSymbol = cISymbol.Detach();
  return ::SetupErrorInfoAndReturn(IID_INktTools, hRes);
}
