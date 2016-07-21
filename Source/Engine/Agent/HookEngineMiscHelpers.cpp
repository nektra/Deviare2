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

#include "Agent.h"
#include "HookEngineMiscHelpers.h"
#include "..\..\Common\ModulesEnum.h"

//-----------------------------------------------------------

namespace MiscHelpers {

LPBYTE SkipJumpInstructions(__in LPBYTE lpPtr)
{
  LPBYTE d;
  ULONG nOfs;

  try
  {
    for (;;)
    {
      if (lpPtr[0] == 0xE9)
      {
        nktMemCopy(&nOfs, lpPtr+1, sizeof(LONG));
        lpPtr += nOfs+5;
      }
      else if (lpPtr[0] == 0xEB)
      {
        nOfs = (LONG)*((signed char*)(lpPtr+1));
        lpPtr += nOfs+2;
      }
      else if ((lpPtr[0]==0xFF && lpPtr[1]==0x25) ||
               (lpPtr[0]==0x48 && lpPtr[1]==0xFF && lpPtr[2]==0x25))
      {
        if (lpPtr[0] == 0x48)
          lpPtr++;
        nOfs = ((ULONG)(*(lpPtr + 2))      ) | ((ULONG)(*(lpPtr + 3)) <<  8) |
               ((ULONG)(*(lpPtr + 4)) << 16) | ((ULONG)(*(lpPtr + 5)) << 24);
#if defined _M_IX86
        d = (LPBYTE)((ULONG_PTR)nOfs);
        lpPtr = (LPBYTE)(((ULONG)(*(d    ))      ) | ((ULONG)(*(d + 1)) <<  8) |
                         ((ULONG)(*(d + 2)) << 16) | ((ULONG)(*(d + 3)) << 24));
#elif defined _M_X64
        d = lpPtr + (ULONG_PTR)nOfs + 6;
        lpPtr = (LPBYTE)(((ULONG_PTR)(*(d    ))      ) | ((ULONG_PTR)(*(d + 1)) <<  8) |
                         ((ULONG_PTR)(*(d + 2)) << 16) | ((ULONG_PTR)(*(d + 3)) << 24) |
                         ((ULONG_PTR)(*(d + 4)) << 32) | ((ULONG_PTR)(*(d + 5)) << 40) |
                         ((ULONG_PTR)(*(d + 6)) << 48) | ((ULONG_PTR)(*(d + 7)) << 56));
#else
  #error Unsupported platform
#endif
      }
      else
        break;
    }
  }
  catch (...)
  { }
  return lpPtr;
}

HRESULT FindDll(__out HINSTANCE *lphDll, __in LPVOID lpAddress)
{
  TNktArrayList<HMODULE> aModulesList;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, nCount;
  DWORD dwProcessId;
  HANDLE hProcess;
  HRESULT hRes;

  NKT_ASSERT(lphDll != NULL);
  *lphDll = NULL;
  dwProcessId = ::GetCurrentProcessId();
  hProcess = ::GetCurrentProcess();
  //try to find the module containing the address
  hRes = CNktDvTools::GetModules(hProcess, sizeof(SIZE_T)<<3, aModulesList);
  if (FAILED(hRes))
    return hRes;
  nCount = aModulesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProcess, aModulesList[i], sModInfo);
    if (hRes == E_FAIL)
      continue;
    if (FAILED(hRes))
      return hRes;
    if ((LPBYTE)lpAddress >= sModInfo.lpBaseAddr &&
        (LPBYTE)lpAddress < sModInfo.lpBaseAddr + sModInfo.nBaseSize)
    {
      *lphDll = (HINSTANCE)aModulesList[i];
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT FindDllAndIncrementUsageCount(__in LPVOID lpAddress)
{
  TNktArrayList<HMODULE> aModulesList;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, nCount;
  DWORD dwProcessId;
  HANDLE hProcess;
  HRESULT hRes;

  dwProcessId = ::GetCurrentProcessId();
  hProcess = ::GetCurrentProcess();
  //try to find the module containing the address
  hRes = CNktDvTools::GetModules(hProcess, sizeof(SIZE_T)<<3, aModulesList);
  if (FAILED(hRes))
    return hRes;
  nCount = aModulesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProcess, aModulesList[i], sModInfo);
    if (hRes == E_FAIL)
      continue;
    if (FAILED(hRes))
      return hRes;
    if ((LPBYTE)lpAddress >= sModInfo.lpBaseAddr &&
        (LPBYTE)lpAddress < sModInfo.lpBaseAddr + sModInfo.nBaseSize)
    {
      ::LoadLibraryW((LPWSTR)sModInfo.cStrExePathW);
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

} //namespace MiscHelpers
