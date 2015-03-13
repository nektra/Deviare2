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
#include "resource.h"
#if defined _M_IX86
  #include "DeviareCOM_i.h"
#elif defined _M_X64
  #include "DeviareCOM64_i.h"
#else
  #error Unsupported platform
#endif
#include "DllMain.h"
#include "DllDataX.h"

//-----------------------------------------------------------

CDeviareCOMModule _AtlModule;
static HINSTANCE hDllInst = NULL;
static LONG volatile nDetaching = 0;

//-----------------------------------------------------------

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
      hDllInst = hInstance;
      break;
    case DLL_PROCESS_DETACH:
      NktInterlockedExchange(&nDetaching, 1);
      break;
    case DLL_THREAD_DETACH:
      nktMemMgrOnThreadExit();
      break;
  }
#ifdef _MERGE_PROXYSTUB
  if (!PrxDllMain(hInstance, dwReason, lpReserved))
    return FALSE;
#endif
  return _AtlModule.DllMain(dwReason, lpReserved);
}

//-----------------------------------------------------------

extern "C"
{

HINSTANCE GetDllInstanceHandle()
{
  return hDllInst;
};

BOOL IsDllDetaching()
{
  return (nDetaching != 0) ? TRUE : FALSE;
};

}; //extern "C"
