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

#include "DevComActCtx.h"

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

typedef HANDLE (WINAPI *lpfnCreateActCtxW)(__in PCACTCTXW pActCtx);
typedef VOID (WINAPI *lpfnReleaseActCtx)(__inout HANDLE hActCtx);
typedef BOOL (WINAPI *lpfnActivateActCtx)(__inout_opt HANDLE hActCtx, __out ULONG_PTR *lpCookie);
typedef BOOL (WINAPI *lpfnDeactivateActCtx)(__in DWORD dwFlags, __in ULONG_PTR ulCookie);

//-----------------------------------------------------------

CNktDeviareComActCtx::CNktDeviareComActCtx() : CNktMemMgrObj()
{
  OSVERSIONINFO sOvi;

  hActCtx = NULL;
  nktMemSet(fn, 0, sizeof(fn));
  bIsXPorLater = FALSE;
  hKernel32Dll = NULL;
  //----
  nktMemSet(&sOvi, 0, sizeof(sOvi));
  sOvi.dwOSVersionInfoSize = sizeof(sOvi);
  if (::GetVersionEx(&sOvi) != FALSE &&
      sOvi.dwPlatformId == VER_PLATFORM_WIN32_NT &&
      (sOvi.dwMajorVersion > 5 ||
      (sOvi.dwMajorVersion == 5 && sOvi.dwMinorVersion >= 1)))
  {
    bIsXPorLater = TRUE;
    hKernel32Dll = ::LoadLibraryW(L"kernel32.dll");
    if (hKernel32Dll != NULL)
    {
      fn[0] = ::GetProcAddress(hKernel32Dll, "CreateActCtxW");
      fn[1] = ::GetProcAddress(hKernel32Dll, "ReleaseActCtx");
      fn[2] = ::GetProcAddress(hKernel32Dll, "ActivateActCtx");
      fn[3] = ::GetProcAddress(hKernel32Dll, "DeactivateActCtx");
      if (fn[0] == NULL || fn[1] == NULL || fn[2] == NULL || fn[3] == NULL)
      {
        ::FreeLibrary(hKernel32Dll);
        hKernel32Dll = NULL;
      }
    }
  }
  return;
}

CNktDeviareComActCtx::~CNktDeviareComActCtx()
{
  if (hKernel32Dll != NULL)
  {
    if (hActCtx != NULL)
      ((lpfnReleaseActCtx)fn[1])(hActCtx);
    ::FreeLibrary(hKernel32Dll);
  }
  return;
}

HRESULT CNktDeviareComActCtx::Initialize(__in HINSTANCE hDll, __in int nResource)
{
  ACTCTXW sActCtxW;
  WCHAR szModuleW[4096];
  HRESULT hRes;

  hRes = S_OK;
  if (bIsXPorLater != FALSE)
  {
    if (hKernel32Dll == NULL)
      return E_FAIL;
    ::GetModuleFileNameW(hDll, szModuleW, NKT_DV_ARRAYLEN(szModuleW));
    nktMemSet(&sActCtxW, 0, sizeof(ACTCTX));
    sActCtxW.cbSize = sizeof(sActCtxW);
    sActCtxW.dwFlags = ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID|ACTCTX_FLAG_HMODULE_VALID|ACTCTX_FLAG_RESOURCE_NAME_VALID;
    sActCtxW.lpSource = szModuleW;
    sActCtxW.lpAssemblyDirectory = szModuleW;
    sActCtxW.lpResourceName = MAKEINTRESOURCE(nResource);
    sActCtxW.hModule = hDll;
    hActCtx = ((lpfnCreateActCtxW)fn[0])(&sActCtxW);
    if (hActCtx == NULL || hActCtx == INVALID_HANDLE_VALUE)
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
      hActCtx = NULL;
    }
  }
  return hRes;
}

HRESULT CNktDeviareComActCtx::Activate(__out ULONG_PTR *lpnCookie)
{
  HRESULT hRes;

  if (lpnCookie == NULL)
    return E_POINTER;
  *lpnCookie = 0;
  hRes = S_OK;
  if (bIsXPorLater != FALSE)
  {
    if (hActCtx != NULL)
    {
      if (((lpfnActivateActCtx)fn[2])(hActCtx, lpnCookie) == FALSE)
      {
        *lpnCookie = 0;
        hRes = NKT_HRESULT_FROM_LASTERROR();
      }
    }
    else
    {
      hRes = E_FAIL;
    }
  }
  return hRes;
}

VOID CNktDeviareComActCtx::Deactivate(__in ULONG_PTR nCookie)
{
  if (bIsXPorLater != FALSE && hActCtx != NULL)
  {
    try
    {
      ((lpfnDeactivateActCtx)fn[3])(0, nCookie);
    }
    catch (...)
    { }
  }
  return;
}
