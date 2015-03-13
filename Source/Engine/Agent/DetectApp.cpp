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

#include "DetectApp.h"
#include "..\..\Common\WaitableObjects.h"
#include "..\..\Common\Tools.h"

#pragma comment(lib, "version.lib")

//-----------------------------------------------------------

static LONG volatile nMutex = 0;
static LONG volatile nApp = -1;

//-----------------------------------------------------------

static DWORD GetFileVersion(__in_z LPCWSTR szFileNameW);

//-----------------------------------------------------------

eApplication DetectApplication()
{
  if (nApp == (eApplication)-1)
  {
    CNktSimpleLockNonReentrant cLock(&nMutex);
    CNktStringW cStrExeFileNameW;
    LPWSTR sW;
    DWORD dwVer;
    LONG _nApp;
    HRESULT hRes;

    if (nApp == (eApplication)-1)
    {
      _nApp = (LONG)appOther;
      hRes = CNktDvTools::GetExecutableFileName(cStrExeFileNameW, ::GetCurrentProcess(), NULL);
      if (SUCCEEDED(hRes))
      {
        sW = (LPWSTR)cStrExeFileNameW;
        while (sW > (LPWSTR)cStrExeFileNameW && *(sW-1) != L'\\')
          sW--;
        if (_wcsicmp(sW, L"iexplore.exe") == 0)
        {
          dwVer = GetFileVersion((LPWSTR)cStrExeFileNameW);
          if ((dwVer  >> 16) == 8)
            _nApp = (LONG)appInternetExplorer8;
        }
      }
      NktInterlockedExchange(&nApp, _nApp);
    }
  }
  return (eApplication)nApp;
}

BOOL IsRunningElevated()
{
  TOKEN_ELEVATION_TYPE sTet;
  DWORD dwRet;
  HANDLE hToken;

  dwRet = ::GetVersion();
  if ((DWORD)(LOBYTE(LOWORD(dwRet))) < 6)
    return TRUE; //pre-Vista => always elevated
  if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken) == FALSE)
    return FALSE; //assume not elevated on error
  dwRet = 0;
  if ( ::GetTokenInformation(hToken, TokenElevationType, &sTet, sizeof(sTet), &dwRet) == FALSE)
  {
    ::CloseHandle(hToken);
    return FALSE; //assume not elevated on error
  }
  ::CloseHandle(hToken);
  return (sTet == TokenElevationTypeFull) ? TRUE : FALSE;
}

//-----------------------------------------------------------

static DWORD GetFileVersion(__in_z LPCWSTR szFileNameW)
{
  BYTE aBuf[3072], *lpBuf;
  VS_FIXEDFILEINFO *lpFfi;
  UINT nLen;
  DWORD dw, dwSize, dwVer;

  dwSize = ::GetFileVersionInfoSizeW(szFileNameW, &dw);
  if (dwSize == 0)
    return 0;
  if (dwSize > NKT_DV_ARRAYLEN(aBuf))
  {
    lpBuf = (LPBYTE)nktMemMalloc((SIZE_T)dwSize);
    if (lpBuf == NULL)
      return 0;
  }
  else
  {
    lpBuf = aBuf;
  }
  dwVer = 0;
  if (::GetFileVersionInfoW(szFileNameW, 0, dwSize, lpBuf) != FALSE &&
      ::VerQueryValueW(lpBuf, L"\\", (LPVOID*)&lpFfi, &nLen) == FALSE ||
      nLen < sizeof(VS_FIXEDFILEINFO) || lpFfi->dwSignature != 0xFEEF04BD)
  {
    dwVer = lpFfi->dwProductVersionMS;
  }
  if (lpBuf != aBuf)
    nktMemFree(lpBuf);
  return dwVer;
}
