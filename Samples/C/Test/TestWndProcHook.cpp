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

#include "TestWndProcHook.h"

//-----------------------------------------------------------

void TestHookWndProc(__in Deviare2::INktSpyMgr *lpSpyMgr)
{
  CComPtr<Deviare2::INktHook> cHook;
  HRESULT hRes;
  SIZE_T i, k, nCount2;
  typedef struct {
    DWORD dwProcId;
    LPVOID lpWndProc;
  } WND_INFO;
  TNktArrayList<WND_INFO> aWndInfoList;
  CComVariant cVt;
  BSTR bstr;
  WND_INFO sWndInfo;
  long lTemp, nFlags;

  for (i=0; i<sCmdLineParams.sWndProc.nCount; i++)
  {
    if (::IsWindow(sCmdLineParams.sWndProc.hWnds[i]) == FALSE)
    {
      wprintf_s(L"Error: Cannot find desired window.\n");
      return;
    }
    ::GetWindowThreadProcessId(sCmdLineParams.sWndProc.hWnds[i], &(sWndInfo.dwProcId));
    LogPrintNoTick(L"Window %lu => process id: %Iu\n", (SIZE_T)(sCmdLineParams.sWndProc.hWnds[i]), sWndInfo.dwProcId);
    if (sWndInfo.dwProcId == 0)
    {
      wprintf_s(L"Error: Cannot find desired window's process.\n");
      return;
    }
    //lpWndProc = (LPVOID)::GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    sWndInfo.lpWndProc = (LPVOID)::GetClassLongPtr(sCmdLineParams.sWndProc.hWnds[i], GCLP_WNDPROC);
    LogPrintNoTick(L"Window %Iu => WndProc: 0x%p\n", (SIZE_T)(sCmdLineParams.sWndProc.hWnds[i]), sWndInfo.lpWndProc);
    if (sWndInfo.lpWndProc == NULL)
    {
      wprintf_s(L"Error: Cannot retrieve WndProc address.\n");
      return;
    }
    if (aWndInfoList.AddElement(sWndInfo) == FALSE)
    {
err_nomem:
      wprintf_s(L"Error: Not enough memory.\n");
      return;
    }
  }
  nFlags = (sCmdLineParams.bAsyncCallbacks != FALSE) ? Deviare2::flgAsyncCallbacks : 0;
  nFlags |= Deviare2::flgDontCheckAddress;
  for (i=0; i<sCmdLineParams.sWndProc.nCount; i++)
  {
    //create hook
    cHook.Release();
    bstr = ::SysAllocString(L"DefWindowProcW");
    if (bstr == NULL)
      goto err_nomem;
    hRes = lpSpyMgr->CreateHookForAddress((size_t)(aWndInfoList[i].lpWndProc), bstr, nFlags, &cHook);
    ::SysFreeString(bstr);
    LogPrintNoTick(L"CNktDvEngine::CreateHook (Window %Iu) => %08X\n", (SIZE_T)(sCmdLineParams.sWndProc.hWnds[i]),
                   hRes);
    //add custom handlers
    lTemp = Deviare2::flgChDontCallIfLoaderLocked;
    if (sCmdLineParams.bCustomHandlerDebug != FALSE)
      lTemp |= Deviare2::flgChDisableExceptionChecks;
    nCount2 = sCmdLineParams.aCustomHandlers.GetCount();
    for (k=0; k<nCount2 && SUCCEEDED(hRes); k++)
    {
      bstr = ::SysAllocString(sCmdLineParams.aCustomHandlers[k]);
      if (bstr == NULL)
        goto err_nomem;
      hRes = cHook->AddCustomHandler(bstr, lTemp, L"");
      ::SysFreeString(bstr);
    }
    //activate hooks
    if (SUCCEEDED(hRes))
    {
      cVt = (LONG)(aWndInfoList[i].dwProcId);
      hRes = cHook->Attach(cVt, VARIANT_TRUE);
      if (SUCCEEDED(hRes))
        hRes = cHook->Hook(VARIANT_TRUE);
      LogPrintNoTick(L"CNktDvHook::Hook (Window %Iu) => %08X\n", (SIZE_T)(sCmdLineParams.sWndProc.hWnds[i]), hRes);
    }
  }
  //wait for exit key
  while (IsExitKeyPressed() == FALSE)
    ::Sleep(100);
  return;
}
