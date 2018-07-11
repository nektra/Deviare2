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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <NktHookLib.h>

#define DISALLOW_REENTRANCY

//-----------------------------------------------------------

typedef int (WINAPI *lpfnMessageBoxW)(__in_opt HWND hWnd, __in_opt LPCWSTR lpText, __in_opt LPCWSTR lpCaption, __in UINT uType);
static int WINAPI Hooked_MessageBoxW(__in_opt HWND hWnd, __in_opt LPCWSTR lpText, __in_opt LPCWSTR lpCaption, __in UINT uType);

static struct {
  SIZE_T nHookId;
  lpfnMessageBoxW fnMessageBoxW;
} sMessageBoxW_Hook = { 0, NULL };

//-----------------------------------------------------------

int WinMainCRTStartup()
//int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPTSTR lpCmdLine, __in int nShowCmd)
{
  CNktHookLib cHookMgr;
  HINSTANCE hUser32Dll;
  LPVOID fnOrigMessageBoxW;
  DWORD dwOsErr;

  cHookMgr.SetEnableDebugOutput(TRUE);

  hUser32Dll = NktHookLibHelpers::GetModuleBaseAddress(L"user32.dll");
  if (hUser32Dll == NULL) {
    ::MessageBoxW(0, L"Error: Cannot get handle of user32.dll", L"HookTest", MB_OK|MB_ICONERROR);
    return 0;
  }
  fnOrigMessageBoxW = NktHookLibHelpers::GetProcedureAddress(hUser32Dll, "MessageBoxW");
  if (fnOrigMessageBoxW == NULL) {
    ::MessageBoxW(0, L"Error: Cannot get address of MessageBoxW", L"HookTest", MB_OK|MB_ICONERROR);
    return 0;
  }

  dwOsErr = cHookMgr.Hook(&(sMessageBoxW_Hook.nHookId), (LPVOID*)&(sMessageBoxW_Hook.fnMessageBoxW),
                          fnOrigMessageBoxW, Hooked_MessageBoxW,
#ifdef DISALLOW_REENTRANCY
                          NKTHOOKLIB_DisallowReentrancy
#else //DISALLOW_REENTRANCY
                          0
#endif //DISALLOW_REENTRANCY
                          );

  ::MessageBoxW(0, L"This should be hooked", L"HookTest", MB_OK);
  //dwOsErr = cHookMgr.EnableHook(dwHookId_MessageBoxW, FALSE);
  dwOsErr = cHookMgr.Unhook(sMessageBoxW_Hook.nHookId);

  ::MessageBoxW(0, L"This should NOT be hooked", L"HookTest", MB_OK);

  return 0;
}

static int WINAPI Hooked_MessageBoxW(__in_opt HWND hWnd, __in_opt LPCWSTR lpText, __in_opt LPCWSTR lpCaption, __in UINT uType)
{
#ifdef DISALLOW_REENTRANCY
  //NOTE: When a hook is created with DISALLOW_REENTRANCY, then we can call the original function directly.
  //      The stub will detected the call comes from the same thread and redirect it to the original function.
  return ::MessageBoxW(hWnd, lpText, L"HOOKED!!!", uType);
#else //DISALLOW_REENTRANCY
  //NOTE: If the hook is NOT created with the DISALLOW_REENTRANCY flag, then we must call the returned function pointer.
  return sMessageBoxW_Hook.fnMessageBoxW(hWnd, lpText, L"HOOKED!!!", uType);
#endif //DISALLOW_REENTRANCY
}

//NOTE: The code below was added because we are linking without default VC runtime libraries in order to show
//      that NktHookLib does not depend on the VC runtime nor Kernel dlls.
//      Visual C's default setting is to create a SAFESEH compatible image executable. NktHookLib is compiled using
//      safeseh too.
//      Normally you application will be linked against the VC runtime libraries. If so, do not add the code below.
#if defined(_M_IX86)
  #ifdef __cplusplus
  extern "C" {
  #endif //__cplusplus
  extern PVOID __safe_se_handler_table[];
  extern BYTE  __safe_se_handler_count;

  IMAGE_LOAD_CONFIG_DIRECTORY32 _load_config_used = {
      sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32),
      0,    0,    0,    0,    0,    0,    0,    0,
      0,    0,    0,    0,    0,    0,    0,    0,
      0,
      (DWORD)__safe_se_handler_table,
      (DWORD) &__safe_se_handler_count
  };

  #ifdef __cplusplus
  }
  #endif //__cplusplus
#endif //_M_IX86
