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

#include "StdAfx.h"
#include "resource.h"
#include "dllmain.h"
#include "dlldatax.h"

//-----------------------------------------------------------

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
  HRESULT hRes = PrxDllCanUnloadNow();
  if (hRes != S_OK)
    return hRes;
#endif //_MERGE_PROXYSTUB
  return _AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
  if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
    return S_OK;
#endif //_MERGE_PROXYSTUB
  return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
  // registers object, typelib and all interfaces in typelib
  HRESULT hRes = _AtlModule.DllRegisterServer();
#ifdef _MERGE_PROXYSTUB
  if (FAILED(hRes))
    return hRes;
  hRes = PrxDllRegisterServer();
  if (hRes == E_NOINTERFACE) //patch because all interfaces are local
    hRes = S_OK;
#endif //_MERGE_PROXYSTUB
  return hRes;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
  HRESULT hRes = _AtlModule.DllUnregisterServer();
#ifdef _MERGE_PROXYSTUB
  if (FAILED(hRes))
    return hRes;
  hRes = PrxDllRegisterServer();
  if (SUCCEEDED(hRes))
    hRes = PrxDllUnregisterServer();
  else if (hRes == E_NOINTERFACE) //patch because all interfaces are local
    hRes = S_OK;
#endif //_MERGE_PROXYSTUB
  return hRes;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
  HRESULT hRes = E_FAIL;
  static const wchar_t szUserSwitch[] = L"user";

  if (pszCmdLine != NULL)
  {
    if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    {
      AtlSetPerUserRegistration(true);
    }
  }
  if (bInstall)
  {
    hRes = DllRegisterServer();
    if (FAILED(hRes))
    {
       DllUnregisterServer();
    }
  }
  else
  {
    hRes = DllUnregisterServer();
  }
  return hRes;
}
