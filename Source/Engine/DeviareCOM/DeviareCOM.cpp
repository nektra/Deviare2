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
#include "ObjectBase.h"
#include "DllMain.h"
#include "DllDataX.h"
#include "ParamsEnum.h"
#include "Param.h"
#include "InternalCreate.h"
#include "HookCallInfoPlugin.h"
#include "HookInfo.h"
#include "Internals1.h"

//#define DUMPOBJECTS_CODE

//-----------------------------------------------------------

static HRESULT HookCallInfoPlugin_Create(__inout LPVOID *lplpPtr);
static HRESULT HookInfo_Create(__inout LPVOID *lplpPtr);
static DWORD WINAPI TP_Unloader(__in LPVOID lpParameter);

//-----------------------------------------------------------

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
  HRESULT hRes;

#ifdef _MERGE_PROXYSTUB
  hRes = PrxDllCanUnloadNow();
  if (hRes != S_OK)
    return hRes;
#endif //_MERGE_PROXYSTUB
  hRes = _AtlModule.DllCanUnloadNow();
  if (hRes == S_OK &&
      CNktDvSpyMgr::CanUnloadNow() == FALSE)
    hRes = S_FALSE;
  if (hRes != S_OK)
  {
#ifdef DUMPOBJECTS_CODE
    if ((::GetAsyncKeyState(VK_SHIFT) & 0x8001) == 0x8001)
      CNktDvSpyMgr::DumpObjects();
#endif //DUMPOBJECTS_CODE
    return hRes;
  }
  return S_OK;
}

// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
  HRESULT hRes;

  //check for special internal objects
  if (::IsEqualIID(rclsid, sInternalIid[0]) != FALSE &&
      ::IsEqualIID(riid, sInternalIid[0]) != FALSE)
  {
    return HookCallInfoPlugin_Create(ppv);
  }
  if (::IsEqualIID(rclsid, sInternalIid[1]) != FALSE &&
      ::IsEqualIID(riid, sInternalIid[1]) != FALSE)
  {
    return HookInfo_Create(ppv);
  }
  //normal path
#ifdef _MERGE_PROXYSTUB
  hRes = PrxDllGetClassObject(rclsid, riid, ppv);
  if (hRes == S_OK)
    return S_OK;
#endif
  hRes = _AtlModule.DllGetClassObject(rclsid, riid, ppv);
  return hRes;
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
  HRESULT hRes;

  // registers object, typelib and all interfaces in typelib
  hRes = _AtlModule.DllRegisterServer();
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
#endif
  return hRes;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine. 
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
  static const wchar_t szUserSwitch[] = L"user";
  HRESULT hRes;

  if (pszCmdLine != NULL)
  {
    if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
    {
      AtlSetPerUserRegistration(true);
    }
  }

  if (bInstall != FALSE)
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

//-----------------------------------------------------------

STDAPI GetLastErrorCode()
{
  //See SetupErrorInfo in ObjectBase.h
  CComPtr<IErrorInfo> cErrInfo;
  BSTR bStr;
  HRESULT hRes;

  if (FAILED(::GetErrorInfo(0, &cErrInfo)) || cErrInfo == NULL)
    return S_OK;
  if (FAILED(cErrInfo->GetDescription(&bStr)) || bStr == NULL)
    return S_OK;
  hRes = (HRESULT)(LONG)_wtol(bStr);
  ::SysFreeString(bStr);
  return hRes;
}

STDAPI Internal1(__in DWORD nCode, __in SIZE_T nSubCodeA, __in __in SIZE_T nSubCodeB)
{
  switch (nCode)
  {
    case 1:
      IInterfaceItem::InterfaceItem_InvalidateAll((DWORD)nSubCodeA, (DWORD)nSubCodeB);
      return S_OK;

    case 2:
      {
      DWORD dwTid;
      HANDLE hThread;

      hThread = ::CreateThread(NULL, 0, TP_Unloader, NULL, 0, &dwTid);
      if (hThread == NULL)
        return E_FAIL;
      ::CloseHandle(hThread);
      }
      return S_OK;

    case 3:
      return Internal1_Code3(nSubCodeA, nSubCodeB);
  }
  return E_INVALIDARG;
}

//-----------------------------------------------------------

static HRESULT HookCallInfoPlugin_Create(__inout LPVOID *lplpPtr)
{
  IDispatch *lpDisp;
  HRESULT hRes;

  if (lplpPtr == NULL || *lplpPtr == NULL)
    return E_POINTER;
  hRes = CNktHookCallInfoPluginImpl::InternalCreate(
                (DV_AGENTCOM_HOOKCALLINFOPLUGIN_CREATE_DATA*)(*lplpPtr), &lpDisp);
  *lplpPtr = (SUCCEEDED(hRes)) ? lpDisp : NULL;
  return hRes;
}

static HRESULT HookInfo_Create(__inout LPVOID *lplpPtr)
{
  IDispatch *lpDisp;
  HRESULT hRes;

  if (lplpPtr == NULL || *lplpPtr == NULL)
    return E_POINTER;
  hRes = CNktHookInfoImpl::InternalCreate((DV_AGENTCOM_HOOKINFO_CREATE_DATA*)(*lplpPtr), &lpDisp);
  *lplpPtr = (SUCCEEDED(hRes)) ? lpDisp : NULL;
  return hRes;
}

static DWORD WINAPI TP_Unloader(__in LPVOID lpParameter)
{
  IInterfaceItem::InterfaceItem_InvalidateAll(0, 0);
  while (DllCanUnloadNow() == S_FALSE && IsDllDetaching() == FALSE)
    ::Sleep(2000);
  ::FreeLibraryAndExitThread(GetDllInstanceHandle(), 0);
  //NOTE: No need to call "nktMemMgrOnThreadExit()" because DLL_THREAD_DETACH will handle it.
  return 0;
}
