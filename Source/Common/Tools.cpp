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

#include "Tools.h"
#include <olectl.h>
#include <sddl.h>
#include <psapi.h>
#include <ShellAPI.h>
#include <process.h>
#include <intrin.h>
#include "AutoPtr.h"
#include "WaitableObjects.h"
#include "NtInternals.h"
#include "ProcessHandle.h"

//-----------------------------------------------------------

#define GETMODULESLIST_RETRIES                            10
#define GETMODULESLIST_UNDOCUMENTED_RETRIES               10
#define X_GRANULARITY                                  65536

//-----------------------------------------------------------

//#define TOOLS_HELPER_THREAD_WAIT_TIME               INFINITE //only used fo debugging purposes
#define TOOLS_HELPER_THREAD_WAIT_TIME                   1000

#define THTDATA_CODE_GetFileNameFromHandle                 1

typedef struct {
  HANDLE hFile;
  DWORD dwPid;
  CNktStringW *lpStrFileNameW;
} THTDATA_GETFILENAMEFROMHANDLE;

//-----------------------------------------------------------

static struct {
  LONG volatile nMtx;
  HANDLE hThread;
  HANDLE hEvents[3]; //1-Quit / 2-Start / 3-job done
  LONG volatile nCode;
  LPVOID volatile lpUserData;
  LONG volatile hRes;
} sHelperThread = { 0 };

//-----------------------------------------------------------

static __inline BOOL ReadProcMem(__in HANDLE hProc, __out LPVOID lpDest, __in LPVOID lpSrc,
                                 __in SIZE_T nDataSize);

static HRESULT ToolsHelperThread_Exec(__in LONG nCode, __inout LPVOID lpUserData);
static HRESULT ToolsHelperThread_Start();
static VOID ToolsHelperThread_Stop(__in BOOL bWait);
static unsigned int __stdcall ToolsHelperThreadProc(__inout LPVOID lpParameter);

static __inline BOOL IsBufferSmallError(__in HRESULT hRes)
{
  return (hRes == HRESULT_FROM_NT(NKT_DV_NTSTATUS_INFO_LENGTH_MISMATCH) ||
          hRes == HRESULT_FROM_NT(NKT_DV_NTSTATUS_BUFFER_TOO_SMALL) ||
          hRes == NKT_HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) ||
          hRes == NKT_HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW)) ? TRUE : FALSE;
}

//-----------------------------------------------------------

VOID CNktDvTools::NotifyError(__in_z LPCWSTR szFormatW, ...)
{
  WCHAR szBufW[1024];
  va_list ap;

  va_start(ap, szFormatW);
  _vsnwprintf_s(szBufW, sizeof(szBufW)/sizeof(szBufW[0]), _TRUNCATE, szFormatW, ap);
  va_end(ap);
  NKT_DEBUGPRINTLNA(Nektra::dlError, ("%lu) Deviare: %S\n", ::GetTickCount(), szBufW));
  return;
}

VOID CNktDvTools::NotifyWarning(__in_z LPCWSTR szFormatW, ...)
{
  WCHAR szBufW[1024];
  va_list ap;

  va_start(ap, szFormatW);
  _vsnwprintf_s(szBufW, sizeof(szBufW)/sizeof(szBufW[0]), _TRUNCATE, szFormatW, ap);
  va_end(ap);
  NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("Deviare: %S\n", ::GetTickCount(), szBufW));
  return;
}

HICON CNktDvTools::_ExtractIcon(__in_z LPCWSTR szExecutablePathW, __in int nIndex)
{
  HICON hIcon[2];

  if (szExecutablePathW == NULL || szExecutablePathW[0] == 0)
    return NULL;
  hIcon[0] = hIcon[1] = NULL;
  ::ExtractIconExW(szExecutablePathW, nIndex, &hIcon[0], &hIcon[1], 1);
  if (hIcon[0] != NULL)
  {
    if (hIcon[1] != NULL)
      ::DestroyIcon(hIcon[1]);
    return hIcon[0];
  }
  if (hIcon[0] != NULL)
    ::DestroyIcon(hIcon[0]);
  return hIcon[1];
}

HRESULT CNktDvTools::_ExtractIcon(__deref_out IPicture** lplpIcon, __in_z LPCWSTR szExecutablePathW,
                                  __in int nIndex)
{
  PICTDESC sPicDesc;
  HICON hIcon;

  if (lplpIcon == NULL)
    return E_POINTER;
  *lplpIcon = NULL;
  hIcon = CNktDvTools::_ExtractIcon(szExecutablePathW, nIndex);
  if (hIcon == NULL)
    return E_FAIL;
  nktMemSet(&sPicDesc, 0, sizeof(sPicDesc));
  sPicDesc.picType = PICTYPE_ICON;
  sPicDesc.cbSizeofstruct = sizeof(PICTDESC);
  sPicDesc.icon.hicon = hIcon;
  return ::OleCreatePictureIndirect(&sPicDesc, IID_IPicture, FALSE, (LPVOID*)lplpIcon);
}

HRESULT CNktDvTools::GetProcessUserName(__inout CNktStringW &cStrDestW, __in DWORD dwPid)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;
  HANDLE hProc;

  cStrDestW.Empty();
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
  if (hProc == NULL)
    return E_ACCESSDENIED;
  hRes = CNktDvTools::GetProcessUserName(cStrDestW, hProc);
  ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvTools::GetProcessUserName(__inout CNktStringW &cStrDestW, __in HANDLE hProcess)
{
  HANDLE hToken;
  DWORD dwOsErr, dwRet, dwPass, dwNameLen, dwRefDomainNameLen;
  TOKEN_USER *lpTokUser;
  LPWSTR szUserNameW, szRefDomainW;
  SID_NAME_USE sSnu;

  szUserNameW = szRefDomainW = NULL;
  hToken = NULL;
  lpTokUser = NULL;
  cStrDestW.Empty();
  //open process's token to get security information
  if (::OpenProcessToken(hProcess, TOKEN_QUERY|TOKEN_READ, &hToken) == FALSE)
  {
    dwOsErr = ::GetLastError();
    goto gpun_end;
  }
  //get user information from token
  dwRet = sizeof(TOKEN_USER);
  for (dwPass=1; dwPass<=2; dwPass++)
  {
    lpTokUser = (TOKEN_USER*)nktMemMalloc((SIZE_T)dwRet);
    if (lpTokUser == NULL)
    {
      dwOsErr = ERROR_OUTOFMEMORY;
      goto gpun_end;
    }
    if (::GetTokenInformation(hToken, TokenUser, lpTokUser, dwRet, &dwRet) != FALSE)
    {
      dwNameLen = dwRefDomainNameLen = 256;
      for (;;)
      {
        szUserNameW = (LPWSTR)nktMemMalloc((SIZE_T)dwNameLen * sizeof(WCHAR));
        szRefDomainW = (LPWSTR)nktMemMalloc((SIZE_T)dwRefDomainNameLen * sizeof(WCHAR));
        if (szUserNameW == NULL || szRefDomainW == NULL)
        {
          dwOsErr = ERROR_OUTOFMEMORY;
          goto gpun_end;
        }
        if (::LookupAccountSidW(NULL, lpTokUser->User.Sid, szUserNameW, &dwNameLen,
                                szRefDomainW, &dwRefDomainNameLen, &sSnu) != FALSE)
        {
          dwPass = 3; //force loop end
          break;
        }
        dwOsErr = ::GetLastError();
        if (dwOsErr != ERROR_INSUFFICIENT_BUFFER && dwOsErr != ERROR_BUFFER_OVERFLOW)
          goto gpun_end;
        nktMemFree(szRefDomainW);
        szRefDomainW = NULL;
        nktMemFree(szUserNameW);
        szUserNameW = NULL;
      }
    }
    nktMemFree(lpTokUser);
    lpTokUser = NULL;
  }
  if (szUserNameW != NULL && szUserNameW[0] != 0 && szRefDomainW != NULL && szRefDomainW[0] != 0)
  {
    if (cStrDestW.Copy(szRefDomainW) == FALSE ||
        cStrDestW.Concat(L"\\") == FALSE)
    {
      dwOsErr = ERROR_OUTOFMEMORY;
      goto gpun_end;
    }
  }
  if (cStrDestW.Concat((szUserNameW!=NULL && szUserNameW[0]!=0) ? szUserNameW : L"SYSTEM") == FALSE)
    dwOsErr = ERROR_OUTOFMEMORY;
  else
    dwOsErr = NOERROR;
gpun_end:
  if (dwOsErr != NOERROR)
    cStrDestW.Empty();
  if (szRefDomainW != NULL)
    nktMemFree(szRefDomainW);
  if (szUserNameW != NULL)
    nktMemFree(szUserNameW);
  if (lpTokUser != NULL)
    nktMemFree(lpTokUser);
  if (hToken != NULL)
    ::CloseHandle(hToken);
  return NKT_HRESULT_FROM_WIN32(dwOsErr);
}

HRESULT CNktDvTools::GetLogonSid(__inout CNktStringW &cStrDestW, __in DWORD dwPid)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;
  HANDLE hProc;

  cStrDestW.Empty();
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
  if (hProc == NULL)
    return E_ACCESSDENIED;
  hRes = CNktDvTools::GetLogonSid(cStrDestW, hProc);
  ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvTools::GetLogonSid(__inout CNktStringW &cStrDestW, __in HANDLE hProcess)
{
  HANDLE hToken;
  DWORD dwOsErr, dwRet, dwPass, dwIndex;
  TOKEN_GROUPS *lpTokGroups;
  LPWSTR szSidW;

  szSidW = NULL;
  hToken = NULL;
  cStrDestW.Empty();
  //open process's token to get security information
  if (::OpenProcessToken(hProcess, TOKEN_QUERY|TOKEN_READ, &hToken) == FALSE)
  {
    dwOsErr = ::GetLastError();
    goto gls_end;
  }
  //get logon sid from token
  dwRet = sizeof(TOKEN_GROUPS);
  for (dwPass=1; dwPass<=2; dwPass++)
  {
    lpTokGroups = (TOKEN_GROUPS*)nktMemMalloc((SIZE_T)dwRet);
    if (lpTokGroups == NULL)
    {
      dwOsErr = ERROR_OUTOFMEMORY;
      goto gls_end;
    }
    if (::GetTokenInformation(hToken, TokenGroups, lpTokGroups, dwRet, &dwRet) != FALSE)
    {
      for (dwIndex=0; dwIndex<lpTokGroups->GroupCount; dwIndex++)
      {
        if ((lpTokGroups->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID) ==  SE_GROUP_LOGON_ID)
        {
          if (::ConvertSidToStringSidW(lpTokGroups->Groups[dwIndex].Sid, &szSidW) != FALSE)
          {
            dwPass = 3; //force loop end
            break;
          }
          szSidW = NULL;
        }
      }
    }
    nktMemFree(lpTokGroups);
    lpTokGroups = NULL;
  }
  dwOsErr = NOERROR;
  if (szSidW != NULL && szSidW[0] != 0)
  {
    if (cStrDestW.Copy((szSidW!=NULL && szSidW[0]!=0) ? szSidW : L"") == FALSE)
      dwOsErr = ERROR_OUTOFMEMORY;
  }
gls_end:
  if (dwOsErr != NOERROR)
    cStrDestW.Empty();
  if (szSidW != NULL)
    ::LocalFree(szSidW);
  if (hToken != NULL)
    ::CloseHandle(hToken);
  return NKT_HRESULT_FROM_WIN32(dwOsErr);
}

HRESULT CNktDvTools::GetExecutableFileName(__inout CNktStringW &cStrDestW, __in DWORD dwPid,
                                           __in HMODULE hModule)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;
  HANDLE hProc;

  cStrDestW.Empty();
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
  if (hProc == NULL)
    return E_ACCESSDENIED;
  hRes = CNktDvTools::GetExecutableFileName(cStrDestW, hProc, hModule);
  ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvTools::GetExecutableFileName(__inout CNktStringW &cStrDestW, __in HANDLE hProcess, __in HMODULE hModule)
{
  TNktAutoFreePtr<BYTE> aBuf;
  BYTE aTempBuf[4096];
  DWORD dwBufSize, dwReqBuf;
  SIZE_T nReqBuf;
  HRESULT hRes;
  union {
    LPBYTE lpPtr;
    LPWSTR sW;
    NKT_DV_UNICODE_STRING *lpUStr;
  } u;

  //retrieve process executable full file name
  hRes = E_FAIL;
  for (dwBufSize=NKT_DV_ARRAYLEN(aTempBuf); dwBufSize<=65536*2; dwBufSize+=4096)
  {
    if (dwBufSize <= NKT_DV_ARRAYLEN(aTempBuf))
    {
      u.lpPtr = aTempBuf;
    }
    else
    {
      aBuf.Reset();
      aBuf.Attach(u.lpPtr = (LPBYTE)nktMemMalloc(dwBufSize));
      if (aBuf == NULL)
      {
        hRes = E_OUTOFMEMORY;
        break;
      }
    }
    if (hModule == NULL)
    {
      //first try
      hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessImageFileName, u.lpPtr,
                                                    dwBufSize, &dwReqBuf);
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.CopyN(u.lpUStr->Buffer, u.lpUStr->Length/sizeof(WCHAR)) != FALSE)
          hRes = Device2DosForm(cStrDestW);
        else
          hRes = E_OUTOFMEMORY;
        if (SUCCEEDED(hRes))
          break;
      }
      //second try
      hRes = nktDvDynApis_QueryFullProcessImageName(hProcess, u.sW, dwBufSize/sizeof(WCHAR));
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.Copy(u.sW) == FALSE)
          hRes = E_OUTOFMEMORY;
        else
          break;
      }
      //third try
      hRes = nktDvDynApis_GetProcessImageFileNameW(hProcess, u.sW, dwBufSize/sizeof(WCHAR));
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.Copy(u.sW) != FALSE)
          hRes = Device2DosForm(cStrDestW);
        else
          hRes = E_OUTOFMEMORY;
        if (SUCCEEDED(hRes))
          break;
      }
    }
    //try another method for modules
    if (hModule != NULL)
    {
      hRes = nktDvDynApis_NtQueryVirtualMemory(hProcess, (PVOID)hModule, NKT_DV_MemorySectionName,
                                               u.lpPtr, (SIZE_T)dwBufSize, &nReqBuf);
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.CopyN(u.lpUStr->Buffer, u.lpUStr->Length/sizeof(WCHAR)) != FALSE)
          hRes = Device2DosForm(cStrDestW);
        else
          hRes = E_OUTOFMEMORY;
        if (SUCCEEDED(hRes))
          break;
      }
    }
    //another one
    if (hProcess == ::GetCurrentProcess()) {
      hRes = nktDvDynApis_GetModuleFileNameW(hModule, u.sW, dwBufSize / sizeof(WCHAR));
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.Copy(u.sW) == FALSE)
          hRes = E_OUTOFMEMORY;
        break;
      }
    }
    else {
      hRes = nktDvDynApis_GetModuleFileNameExW(hProcess, hModule, u.sW, dwBufSize / sizeof(WCHAR));
      if (IsBufferSmallError(hRes) != FALSE)
        continue;
      if (SUCCEEDED(hRes))
      {
        if (cStrDestW.Copy(u.sW) == FALSE)
          hRes = E_OUTOFMEMORY;
        break;
      }
    }
    //no more
    break;
  }
  if (FAILED(hRes))
    cStrDestW.Empty();
  return hRes;
}

HRESULT CNktDvTools::Device2DosForm(__inout CNktStringW &cStrPathW)
{
  WCHAR szTempW[512], szNameW[1024], szDriveW[3], *s, *p;
  SIZE_T nNameLen;
  HRESULT hRes;

  s = (LPWSTR)cStrPathW;
  //try network shares first
  if (_wcsnicmp(s, L"\\Device\\MUP\\", 12) == 0)
  {
    cStrPathW.Delete(1, 10);
    return S_OK;
  }
  if (_wcsnicmp(s, L"\\Device\\LanmanRedirector\\", 25) == 0)
  {
    cStrPathW.Delete(1, 23);
    return S_OK;
  }
  if (memcmp(s, L"\\\\?\\", 8) == 0 || memcmp(s, L"\\??\\", 8) == 0)
    cStrPathW.Delete(0, 4);
  //translate path in device form to drive letters
  hRes = S_OK;
  szDriveW[1] = L':';
  szDriveW[2] = szTempW[0] = 0;
  if (::GetLogicalDriveStringsW(NKT_DV_ARRAYLEN(szTempW)-1, szTempW) != FALSE)
  {
    for (p=szTempW; *p!=0; p++)
    {
      szDriveW[0] = *p;
      while (*p != 0)
        p++; //advance to next item
      //----
      if (::QueryDosDeviceW(szDriveW, szNameW, NKT_DV_ARRAYLEN(szNameW)) != FALSE)
      {
        nNameLen = wcslen(szNameW);
        if (nNameLen < NKT_DV_ARRAYLEN(szNameW) &&
            _wcsnicmp(s, szNameW, nNameLen) == 0 && s[nNameLen] == L'\\')
        {
          cStrPathW.Delete(0, nNameLen);
          if (cStrPathW.Insert(szDriveW, 0) == FALSE)
            hRes = E_OUTOFMEMORY;
          break;
        }
      }
      else {
        hRes = NKT_HRESULT_FROM_LASTERROR();
      }
    }
  }
  return hRes;
}

HRESULT CNktDvTools::GetRealFilePath(__inout CNktStringW &cStrFileNameW)
{
  HANDLE hFile;
  HRESULT hRes;

  if (cStrFileNameW.IsEmpty() == FALSE)
  {
    hFile = ::CreateFileW((LPWSTR)cStrFileNameW, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
      hRes = nktDvNtGetFileNameFromHandle(cStrFileNameW, hFile);
      ::CloseHandle(hFile);
    }
    else
    {
      hRes = NKT_HRESULT_FROM_LASTERROR();
    }
    if (SUCCEEDED(hRes))
      hRes = CNktDvTools::Device2DosForm(cStrFileNameW);
  }
  else
  {
    hRes = S_OK;
  }
  return hRes;
}

HRESULT CNktDvTools::SetProcessPrivilege(__in DWORD dwPid, __in_z LPCWSTR szPrivilegeW, __in BOOL bEnable)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;
  HANDLE hProc;

  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_SET_INFORMATION);
  if (hProc == NULL)
    return E_ACCESSDENIED;
  hRes = CNktDvTools::SetProcessPrivilege(hProc, szPrivilegeW, bEnable);
  ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvTools::SetProcessPrivilege(__in HANDLE hProcess, __in_z LPCWSTR szPrivilegeW,
                                         __in BOOL bEnable)
{
  HANDLE hToken;
  LUID sLuid;
  HRESULT hRes;
  TOKEN_PRIVILEGES sTokPriv;

  if (::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken) == FALSE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    goto spp_end;
  }
  if (::LookupPrivilegeValueW(NULL, szPrivilegeW, &sLuid) == FALSE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    goto spp_end;
  }
  //set privilege
  sTokPriv.PrivilegeCount = 1;
  sTokPriv.Privileges[0].Luid = sLuid;
  sTokPriv.Privileges[0].Attributes = (bEnable != FALSE) ? SE_PRIVILEGE_ENABLED : 0;
  if (::AdjustTokenPrivileges(hToken, FALSE, &sTokPriv, sizeof(TOKEN_PRIVILEGES), NULL, NULL) == FALSE)
  {
    hRes = NKT_HRESULT_FROM_LASTERROR();
    if (FAILED(hRes))
      goto spp_end;
  }
  else
  {
    hRes = S_OK;
  }
spp_end:
  if (FAILED(hRes))
    NotifyError(L"Unable to set process privilege. (%08X)", hRes);
  if (hToken != NULL)
    ::CloseHandle(hToken);
  return hRes;
}

HRESULT CNktDvTools::GetProcessPrivilege(__out LPBOOL lpbEnabled, __in DWORD dwPid,
                                         __in_z LPCWSTR szPrivilegeW)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HRESULT hRes;
  HANDLE hProc;

  if (lpbEnabled == NULL)
    return E_POINTER;
  *lpbEnabled = FALSE;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProc = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION);
  if (hProc == NULL)
    return E_ACCESSDENIED;
  hRes = CNktDvTools::GetProcessPrivilege(lpbEnabled, hProc, szPrivilegeW);
  ::CloseHandle(hProc);
  return hRes;
}

HRESULT CNktDvTools::GetProcessPrivilege(__out LPBOOL lpbEnabled, __in HANDLE hProcess,
                                         __in LPCWSTR szPrivilegeW)
{
  HANDLE hToken;
  LUID sLuid;
  TOKEN_PRIVILEGES *lpTokPriv;
  DWORD dwOsErr, dwRet, dwPass, dwIndex;

  if (lpbEnabled == NULL)
    return E_POINTER;
  *lpbEnabled = FALSE;
  //open process's token to get security information
  if (::OpenProcessToken(hProcess, TOKEN_READ|TOKEN_QUERY, &hToken) == FALSE)
  {
    dwOsErr = ::GetLastError();
    goto gpp_end;
  }
  if (::LookupPrivilegeValueW(NULL, szPrivilegeW, &sLuid) == FALSE)
  {
    dwOsErr = ::GetLastError();
    goto gpp_end;
  }
  //get user information from token
  dwRet = sizeof(TOKEN_PRIVILEGES);
  for (dwPass=1; dwPass<=2; dwPass++)
  {
    lpTokPriv = (TOKEN_PRIVILEGES*)nktMemMalloc((SIZE_T)dwRet);
    if (lpTokPriv == NULL)
    {
      dwOsErr = ERROR_OUTOFMEMORY;
      goto gpp_end;
    }
    if (::GetTokenInformation(hToken, TokenPrivileges, lpTokPriv, dwRet, &dwRet) != FALSE)
    {
      for (dwIndex=0; dwIndex<lpTokPriv->PrivilegeCount; dwIndex++)
      {
        if (memcmp(&(lpTokPriv->Privileges[dwIndex].Luid), &sLuid, sizeof(sLuid)) == 0)
        {
          if ((lpTokPriv->Privileges[dwIndex].Attributes & (SE_PRIVILEGE_ENABLED|SE_PRIVILEGE_ENABLED_BY_DEFAULT)) != 0)
            *lpbEnabled = TRUE;
          break;
        }
      }
      dwPass = 3; //force loop end
      break;
    }
    dwOsErr = ::GetLastError();
    if (dwOsErr != ERROR_INSUFFICIENT_BUFFER && dwOsErr != ERROR_BUFFER_OVERFLOW)
      goto gpp_end;
    nktMemFree(lpTokPriv);
    lpTokPriv = NULL;
  }
gpp_end:
  if (dwOsErr != NOERROR)
    NotifyError(L"Unable to get process privilege. (%08X)", NKT_HRESULT_FROM_WIN32(dwOsErr));
  if (lpTokPriv != NULL)
    nktMemFree(lpTokPriv);
  if (hToken != NULL)
    ::CloseHandle(hToken);
  return NKT_HRESULT_FROM_WIN32(dwOsErr);
}

HRESULT CNktDvTools::SplitFileNameAndPath(__inout CNktStringW &cStrNameW, __inout CNktStringW &cStrPathW,
                                          __in_z LPCWSTR szFullNameW)
{
  LPCWSTR sW;

  cStrNameW.Empty();
  cStrPathW.Empty();
  if (szFullNameW == NULL)
    return E_POINTER;
  //find last separator
  for (sW=szFullNameW; *sW!=0; *sW++);
  for (; sW>szFullNameW && *(sW-1)!=L'\\' && *(sW-1)!=L'/'; *sW--);
  //extract name
  if (cStrNameW.Copy(sW) == FALSE)
    return E_OUTOFMEMORY;
  //extract path
  if (sW > szFullNameW)
  {
    if (cStrPathW.CopyN(szFullNameW, (SIZE_T)(sW-szFullNameW)) == FALSE)
      return E_OUTOFMEMORY;
    //convert backslashes
    for (LPWSTR sW2=(LPWSTR)cStrPathW; *sW2!=0; sW2++)
    {
      if (*sW2 == L'/')
        *sW2 = L'\\';
    }
  }
  return S_OK;
}

HRESULT CNktDvTools::SplitUserNameAndDomain(__inout CNktStringW &cStrNameW,
                                            __inout CNktStringW &cStrDomainW, __in_z LPCWSTR szFullNameW)
{
  LPCWSTR sW;

  cStrNameW.Empty();
  cStrDomainW.Empty();
  if (szFullNameW == NULL)
    return E_POINTER;
  //find last separator
  for (sW=szFullNameW; *sW!=0; *sW++);
  for (; sW>szFullNameW && *(sW-1)!=L'\\'; *sW--);
  //extract name
  if (cStrNameW.Copy(sW) == FALSE)
    return E_OUTOFMEMORY;
  //extract path
  for (; sW>szFullNameW && *(sW-1)==L'\\'; *sW--);
  if (sW > szFullNameW)
  {
    if (cStrDomainW.CopyN(szFullNameW, (SIZE_T)(sW-szFullNameW)) == FALSE)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

LPWSTR CNktDvTools::stristrW(__in_z LPCWSTR szStringW, __in_z LPCWSTR szToSearchW)
{
  LPWSTR cp, s1, s2;

  if (szStringW == NULL || szToSearchW == NULL)
    return NULL;
  if (*szToSearchW == 0)
    return (LPWSTR)szStringW;
  cp = (LPWSTR)szStringW;
  while (*cp != 0)
  {
    s1 = cp;
    s2 = (LPWSTR)szToSearchW;
    while (*s1 != 0 && *s2 != 0 &&
           ::CharLowerW((LPWSTR)(USHORT)(*s1)) == ::CharLowerW((LPWSTR)(USHORT)(*s2)))
    {
      s1++;
      s2++;
    }
    if (*s2 == 0)
      return cp;
    cp++;
  }
  return NULL;
}

HRESULT CNktDvTools::wstr2ulong(__out ULONG *lpnValue, __in_z LPCWSTR szStringW)
{
  ULONG k;

  NKT_ASSERT(lpnValue != NULL);
  NKT_ASSERT(szStringW != NULL);
  *lpnValue = 0;
  while (*szStringW != 0)
  {
    if (szStringW[0]<L'0' || szStringW[0]>L'9')
      return E_INVALIDARG;
    k = (*lpnValue) * 10;
    if (k < (*lpnValue))
      return NKT_DVERR_ArithmeticOverflow;
    *lpnValue = k + (ULONG)(szStringW[0] - L'0');
    if ((*lpnValue) < k)
      return NKT_DVERR_ArithmeticOverflow;
    szStringW++;
  }
  return S_OK;
}

LPSTR CNktDvTools::Wide2Ansi(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen)
{
  SIZE_T nDestLen;
  LPSTR szDestA;

  if (szSrcW == NULL)
    szSrcW = L"";
  if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = wcslen(szSrcW);
  nDestLen = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, szSrcW, (int)nSrcLen, NULL, 0, NULL, NULL);
  szDestA = (LPSTR)nktMemMalloc((nDestLen+1)*sizeof(CHAR));
  if (szDestA == NULL)
    return NULL;
  nDestLen = (SIZE_T)::WideCharToMultiByte(CP_ACP, 0, szSrcW, (int)nSrcLen, szDestA, (int)(nDestLen+1),
                                           NULL, NULL);
  szDestA[nDestLen] = 0;
  return szDestA;
}

VOID CNktDvTools::DoEvents(__in BOOL bKeepQuitMsg)
{
  MSG sMsg;

  while (::PeekMessage(&sMsg, NULL, 0, 0, PM_NOREMOVE) != FALSE)
  {
    if (bKeepQuitMsg != FALSE && sMsg.message == WM_QUIT)
      break;
    ::PeekMessage(&sMsg, NULL, 0, 0, PM_REMOVE);
    ::TranslateMessage(&sMsg);
    ::DispatchMessage(&sMsg);
  }
  return;
}

VOID CNktDvTools::DoMessageLoop()
{
  MSG sMsg;

  while (::GetMessage( &sMsg, NULL, 0, 0 ) != FALSE)
  {
    ::TranslateMessage(&sMsg);
    ::DispatchMessage(&sMsg);
  }
  return;
}

BOOL CNktDvTools::Win32ErrToString(__inout CNktStringW &cStrDestW, __in DWORD dwError)
{
  LPWSTR lpBufW;
  BOOL bRes;

  cStrDestW.Empty();
  lpBufW = NULL;
  ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpBufW, 0, NULL);
  if (lpBufW == NULL)
    return FALSE;
  bRes = cStrDestW.Copy(lpBufW);
  ::LocalFree(lpBufW);
  return bRes;
}

HRESULT CNktDvTools::ParseFunctionName(__inout FUNCTION_PARSE_INFO &sInfo,
                                       __in_nz_opt LPCWSTR szFunctionNameW, __in SIZE_T nFunctionNameLen)
{
  SIZE_T i, k;

  if (szFunctionNameW == NULL)
    return E_POINTER;
  if (nFunctionNameLen == NKT_SIZE_T_MAX)
    nFunctionNameLen = wcslen(szFunctionNameW);
  if (nFunctionNameLen == 0)
    return E_FAIL;
  nktMemSet(&sInfo, 0, sizeof(sInfo));
  //parse dll name
  while (sInfo.nDllNameLen < nFunctionNameLen && szFunctionNameW[sInfo.nDllNameLen] != L'!')
    sInfo.nDllNameLen++;
  if (sInfo.nDllNameLen == 0 && szFunctionNameW[0] == L'!')
    return E_INVALIDARG;
  //if no separator was found, assume that only an api was provided
  if (sInfo.nDllNameLen >= nFunctionNameLen)
    sInfo.nApiNameStart = sInfo.nDllNameLen = 0;
  else
    sInfo.nApiNameStart = sInfo.nDllNameLen + 1; //else skip separator
  //parse api name
  sInfo.nApiOrdinal = NKT_SIZE_T_MAX;
  sInfo.nApiNameLen = 0;
  if (sInfo.nApiNameStart < nFunctionNameLen)
  {
    if (szFunctionNameW[sInfo.nApiNameStart] == L'#')
    {
      if ((i=sInfo.nApiNameStart+1) >= nFunctionNameLen)
        return E_INVALIDARG;
      for (sInfo.nApiOrdinal=0; i<nFunctionNameLen; i++)
      {
        if (szFunctionNameW[i] < L'0' || szFunctionNameW[i] > L'9')
          return E_INVALIDARG;
        k = sInfo.nApiOrdinal * 10;
        if (k < sInfo.nApiOrdinal)
          return E_INVALIDARG;
        sInfo.nApiOrdinal = k + (SIZE_T)(szFunctionNameW[i] - L'0');
        if (sInfo.nApiOrdinal < k || sInfo.nApiOrdinal > 0x00FFFFFF)
          return E_INVALIDARG;
      }
      sInfo.nApiNameLen = i - sInfo.nApiNameStart;
    }
    else
    {
      sInfo.nApiNameLen = nFunctionNameLen - sInfo.nApiNameStart;
    }
  }
  return (sInfo.nApiNameLen != 0) ? S_OK : E_INVALIDARG;
}

SIZE_T CNktDvTools::GetProcessorsCount()
{
  SYSTEM_INFO sInfo;

  ::GetSystemInfo(&sInfo);
  if (sInfo.dwNumberOfProcessors < 1)
    return 1;
  return (SIZE_T)sInfo.dwNumberOfProcessors;
}

HRESULT CNktDvTools::SuspendAfterCreateProcessW(__out LPHANDLE lphReadyExecutionEvent,
                                                __out LPHANDLE lphContinueExecutionEvent,
                                                __in HANDLE hEngineProc, __in HANDLE hSuspendedProc,
                                                __in HANDLE hSuspendedMainThread)
{
  BYTE aTempBuf[256], *lpRemoteCode, *lpRemoteData;
  CNktEvent cContinueEvent, cReadyEvent;
  HANDLE hContinueEventCopy, hReadyEventCopy, hEngineProcCopy;
  SIZE_T nOrigIP, nSetEventAddr, nWaitMultObjAddr, nCloseHandleAddr, nWritten;
  HMODULE hKernel32DLL;
  SIZE_T k, m, nNewEntryPointOfs, nNewStubOfs;
  DWORD dwTemp32;
  BOOL bIs64BitProcess;
  CONTEXT sThreadCtx;
#if defined _M_X64
  WOW64_CONTEXT sWow64ThreadCtx;
#endif //_M_X64
  HRESULT hRes;

  if (lphReadyExecutionEvent == NULL || lphContinueExecutionEvent == NULL)
    return E_POINTER;
  *lphReadyExecutionEvent = NULL;
  *lphContinueExecutionEvent = NULL;
  NKT_DEBUGPRINTLNA(Nektra::dlTools, ("%lu) Tools[SuspendAfterCreateProcessW]: ProcHnd=%08X, ThreadHnd=%08X",
                    ::GetTickCount(), hSuspendedProc, hSuspendedMainThread));
  //get suspended thread execution context
  bIs64BitProcess = nktDvDynApis_Is64BitProcess(hSuspendedProc);
#if defined _M_IX86
  if (bIs64BitProcess != FALSE)
    return E_FAIL;
  nktMemSet(&sThreadCtx, 0, sizeof(sThreadCtx));
  sThreadCtx.ContextFlags = CONTEXT_FULL;
  if (::GetThreadContext(hSuspendedMainThread, &sThreadCtx) == FALSE)
    return E_FAIL;
  nOrigIP = sThreadCtx.Eip;
#elif defined _M_X64
  if (bIs64BitProcess == FALSE)
  {
    sWow64ThreadCtx.ContextFlags = WOW64_CONTEXT_FULL;
    hRes = nktDvDynApis_Wow64GetThreadContext(hSuspendedMainThread, &sWow64ThreadCtx);
    if (FAILED(hRes))
      return hRes;
    nOrigIP = sWow64ThreadCtx.Eip;
  }
  else
  {
    sThreadCtx.ContextFlags = CONTEXT_FULL;
    if (::GetThreadContext(hSuspendedMainThread, &sThreadCtx) == FALSE)
      return E_FAIL;
    nOrigIP = sThreadCtx.Rip;
  }
#endif
  //get some API addresses
#if defined _M_IX86
  hKernel32DLL = ::GetModuleHandleW(L"kernel32.dll");
  if (hKernel32DLL == NULL)
    return E_FAIL;
  nSetEventAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "SetEvent", TRUE);
  nWaitMultObjAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "WaitForMultipleObjects", TRUE);
  nCloseHandleAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "CloseHandle", TRUE);
#elif defined _M_X64
  if (bIs64BitProcess == FALSE)
  {
    //we are targetting a x86 process from a x64 one, different approach must be done
    //because the target process is suspended, the module entry list in the peb is
    //not initialized, so we will scan the process memory regions until we find some
    //useful data
    LPBYTE lpCurrAddress, lpBaseAddr, lpExpDir, lpFuncAddr;
    NKT_DV_IMAGE_NT_HEADER sNtHdr;
    MEMORY_BASIC_INFORMATION sMbi;
    BOOL bContinue;
    LPDWORD lpdwAddressOfFunctions, lpdwAddressOfNames;
    LPWORD lpwAddressOfNameOrdinals;
    SIZE_T i, nBlockSize, nExpDirSize;
    IMAGE_EXPORT_DIRECTORY sExpDir;
    DWORD dwTemp32;
    WORD wTemp16;
    CHAR szTempNameA[32];
    int nIndex;

    lpCurrAddress = NULL;
    for (bContinue=TRUE; bContinue!=FALSE;)
    {
      nSetEventAddr = nWaitMultObjAddr = nCloseHandleAddr = 0;
      //----
      if (::VirtualQueryEx(hSuspendedProc, lpCurrAddress, &sMbi, sizeof(sMbi)) == 0)
      {
        bContinue = FALSE;
        continue;
      }
      if (sMbi.Type == MEM_MAPPED || sMbi.Type == MEM_IMAGE)
      {
        lpBaseAddr = (LPBYTE)(sMbi.AllocationBase);
        nBlockSize = 0;
        do
        {
          nBlockSize += sMbi.RegionSize;
          lpCurrAddress += sMbi.RegionSize;
          if (::VirtualQueryEx(hSuspendedProc, lpCurrAddress, &sMbi, sizeof(sMbi)) == 0)
          {
            bContinue = FALSE;
            break;
          }
        }
        while (lpBaseAddr == (LPBYTE)(sMbi.AllocationBase));
        //check if there is a library here
        hRes = nktDvNtCheckImageType(&sNtHdr, hSuspendedProc, lpBaseAddr);
        if (hRes != (HRESULT)32)
          continue;
        //found a 32-bit dll, now try to find needed exports
        if (sNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
            sNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
          continue; //empty or no export table in module
        lpExpDir = lpBaseAddr + (SIZE_T)(sNtHdr.u32.OptionalHeader.DataDirectory[
                                            IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        nExpDirSize = (SIZE_T)(sNtHdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
        //get export directory info
        if (ReadProcMem(hSuspendedProc, &sExpDir, lpExpDir, sizeof(sExpDir)) == FALSE)
          continue;
        lpdwAddressOfFunctions = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfFunctions));
        lpdwAddressOfNames = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNames));
        lpwAddressOfNameOrdinals = (LPWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNameOrdinals));
        //process the exported names
        for (i=0; i<(SIZE_T)(sExpDir.NumberOfNames); i++)
        {
          //get the name address
          if (ReadProcMem(hSuspendedProc, &dwTemp32, lpdwAddressOfNames+i, sizeof(dwTemp32)) == FALSE ||
              dwTemp32 == NULL)
            continue;
          //get up to NKT_DV_ARRAYLEN(szTempNameA) chars of the name
          if (ReadProcMem(hSuspendedProc, szTempNameA, lpBaseAddr+(SIZE_T)dwTemp32,
                          NKT_DV_ARRAYLEN(szTempNameA)) == FALSE)
            continue;
          if (strcmp(szTempNameA, "NtSetEvent") == 0)
            nIndex = 0;
          else if (strcmp(szTempNameA, "NtWaitForMultipleObjects") == 0)
            nIndex = 1;
          else if (strcmp(szTempNameA, "NtClose") == 0)
            nIndex = 2;
          else
            continue;
          //get the ordinal
          if (ReadProcMem(hSuspendedProc, &wTemp16, lpwAddressOfNameOrdinals+i, sizeof(wTemp16)) == FALSE)
            continue;
          if ((DWORD)wTemp16 >= sExpDir.NumberOfFunctions)
            continue;
          //get address of function
          if (ReadProcMem(hSuspendedProc, &dwTemp32, lpdwAddressOfFunctions+(SIZE_T)wTemp16,
                          sizeof(DWORD)) == FALSE ||
              dwTemp32 == 0)
          {
            continue;
          }
          //retrieve function address
          lpFuncAddr = lpBaseAddr + (SIZE_T)dwTemp32;
          if (lpFuncAddr >= lpExpDir && lpFuncAddr < lpExpDir+nExpDirSize)
          {
            //it is a forward declaration, ignore
            continue;
          }
          //got a function
          switch (nIndex)
          {
            case 0:
              nSetEventAddr = (SIZE_T)lpFuncAddr;
              break;
            case 1:
              nWaitMultObjAddr = (SIZE_T)lpFuncAddr;
              break;
            case 2:
              nCloseHandleAddr = (SIZE_T)lpFuncAddr;
              break;
          }
        }
        if (nSetEventAddr != 0 && nWaitMultObjAddr != 0 && nCloseHandleAddr != 0)
          break; //found needed ntdll.dll exported functions
      }
      else
      {
        //skip block
        lpCurrAddress += sMbi.RegionSize;
      }
    }
  }
  else
  {
    hKernel32DLL = ::GetModuleHandleW(L"kernel32.dll");
    if (hKernel32DLL == NULL)
      return E_FAIL;
    nSetEventAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "SetEvent", TRUE);
    nWaitMultObjAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "WaitForMultipleObjects", TRUE);
    nCloseHandleAddr = (SIZE_T)GetApiAddress(hKernel32DLL, "CloseHandle", TRUE);
  }
#endif
  if (nSetEventAddr == 0 || nWaitMultObjAddr == 0 || nCloseHandleAddr == 0)
    return E_FAIL;
  //create "ready" and "continue" events
  if (cReadyEvent.Create(NULL, FALSE) == FALSE || cContinueEvent.Create(NULL, FALSE) == FALSE)
    return E_OUTOFMEMORY;
  hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), cReadyEvent.GetEventHandle(), hSuspendedProc,
                                      &hReadyEventCopy, 0, FALSE, DUPLICATE_SAME_ACCESS);
  if (SUCCEEDED(hRes))
  {
    hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), cContinueEvent.GetEventHandle(), hSuspendedProc,
                                        &hContinueEventCopy, 0, FALSE, DUPLICATE_SAME_ACCESS);
  }
  if (SUCCEEDED(hRes))
  {
    hRes = nktDvDynApis_DuplicateHandle(::GetCurrentProcess(), hEngineProc, hSuspendedProc, &hEngineProcCopy,
                                        0, FALSE, DUPLICATE_SAME_ACCESS);
  }
  if (FAILED(hRes))
    return hRes;
  //create remote data. NOTE: We are wasting memory by allocating a 4K (page size) block and only use 4/8 bytes :(
  lpRemoteData = (LPBYTE)::VirtualAllocEx(hSuspendedProc, NULL, 4096, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  if (lpRemoteData == NULL)
    return E_OUTOFMEMORY;
  //create remote code
  lpRemoteCode = (LPBYTE)::VirtualAllocEx(hSuspendedProc, NULL, 4096, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (lpRemoteCode == NULL)
  {
    ::VirtualFreeEx(hSuspendedProc, lpRemoteData, 0, MEM_RELEASE);
    return E_OUTOFMEMORY;
  }
  //write new stub
  m = 0;
  if (bIs64BitProcess == FALSE)
  {
#if defined _M_IX86
    //data
    nktMemCopy(aTempBuf+m, &hEngineProcCopy, 4);  m+=4;
    nktMemCopy(aTempBuf+m, &hContinueEventCopy, 4);  m+=4;
    //new entrypoint starts here
    nNewEntryPointOfs = m;
    //push READY EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF SetEvent
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nSetEventAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push READY EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push INFINITE
    aTempBuf[m++] = 0x68;  nktMemSet(aTempBuf+m, 0xFF, 4);  m+=4;
    //push FALSE
    aTempBuf[m++] = 0x68;  nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    //push address of array of ENGINE PROC HANDLE and CONTINUE EVENT HANDLE
    k = (SIZE_T)lpRemoteCode;
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //push 2
    k = 2;
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //mov eax, ADDRESS OF WaitForMultipleObjects
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nWaitMultObjAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push CONTINUE EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hContinueEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push ENGINE PROC HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hEngineProcCopy, 4);  m+=4;
    //mov eax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //jmp dword ptr [lpRemoteData] (absolute)
    dwTemp32 = (DWORD)lpRemoteData;
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x25;  nktMemCopy(aTempBuf+m, &dwTemp32, 4);  m+=4;
    //NEW STUB
    //at this point eax has the original entry point, store it in lpRemoteCode using interlocked
    nNewStubOfs = m;
    //push ecx
    aTempBuf[m++] = 0x51;
    //mov ecx, lpRemoteData
    k = (SIZE_T)lpRemoteData;
    aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //lock xchg eax, [ecx]
    aTempBuf[m++] = 0xF0;  aTempBuf[m++] = 0x87;  aTempBuf[m++] = 0x01;
    //mov eax, NEW ENTRY POINT
    k = (SIZE_T)lpRemoteCode + nNewEntryPointOfs;
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //pop ecx
    aTempBuf[m++] = 0x59;
    //jmp to original eip
    k = (SIZE_T)lpRemoteCode + (m+6);
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x25;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    nktMemCopy(aTempBuf+m, &nOrigIP, 4);  m+=4;
#elif defined _M_X64
    //data
    nktMemCopy(aTempBuf+m, &hEngineProcCopy, 4);  m+=4;
    nktMemCopy(aTempBuf+m, &hContinueEventCopy, 4);  m+=4;
    //new entrypoint starts here
    nNewEntryPointOfs = m;
    //push NULL
    aTempBuf[m++] = 0x68;  nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    //push READY EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF NtSetEvent
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nSetEventAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push READY EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF NtClose
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push NULL
    aTempBuf[m++] = 0x68;  nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    //push FALSE
    aTempBuf[m++] = 0x68;  nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    //push WaitAnyObject
    dwTemp32 = 1;
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &dwTemp32, 4);  m+=4;
    //push address of array of ENGINE PROC HANDLE and CONTINUE EVENT HANDLE
    k = (SIZE_T)lpRemoteCode;
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //push 2
    dwTemp32 = 2;
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &dwTemp32, 4);  m+=4;
    //mov eax, ADDRESS OF NtWaitForMultipleObjects
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nWaitMultObjAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push CONTINUE EVENT HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hContinueEventCopy, 4);  m+=4;
    //mov eax, ADDRESS OF NtClose
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //push ENGINE PROC HANDLE
    aTempBuf[m++] = 0x68;  nktMemCopy(aTempBuf+m, &hEngineProcCopy, 4);  m+=4;
    //mov eax, ADDRESS OF NtClose
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 4);  m+=4;
    //call eax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //jmp dword ptr [lpRemoteData] (absolute)
    dwTemp32 = (DWORD)((ULONG_PTR)lpRemoteData);
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x25;  nktMemCopy(aTempBuf+m, &dwTemp32, 4);  m+=4;
    //NEW STUB
    //at this point eax has the original entry point, store it in lpRemoteCode using interlocked
    nNewStubOfs = m;
    //push ecx
    aTempBuf[m++] = 0x51;
    //mov ecx, lpRemoteData
    k = (SIZE_T)lpRemoteData;
    aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //lock xchg eax, [ecx]
    aTempBuf[m++] = 0xF0;  aTempBuf[m++] = 0x87;  aTempBuf[m++] = 0x01;
    //mov eax, NEW ENTRY POINT
    k = (SIZE_T)lpRemoteCode + nNewEntryPointOfs;
    aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    //pop ecx
    aTempBuf[m++] = 0x59;
    //jmp to original eip
    k = (SIZE_T)lpRemoteCode + (m+6);
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x25;  nktMemCopy(aTempBuf+m, &k, 4);  m+=4;
    nktMemCopy(aTempBuf+m, &nOrigIP, 4);  m+=4;
#endif
  }
  else
  {
    //data
    nktMemCopy(aTempBuf+m, &hEngineProcCopy, 8);  m+=8;
    nktMemCopy(aTempBuf+m, &hContinueEventCopy, 8);  m+=8;
    //new entrypoint starts here
    nNewEntryPointOfs = m;
    //sub rsp, 48h
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0x83;  aTempBuf[m++] = 0xEC;  aTempBuf[m++] = 0x48;
    //mov rcx, READY EVENT HANDLE
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 8);  m+=8;
    //mov rax, ADDRESS OF SetEvent
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nSetEventAddr, 8);  m+=8;
    //call rax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //mov rcx, READY EVENT HANDLE
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &hReadyEventCopy, 8);  m+=8;
    //mov rax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 8);  m+=8;
    //call rax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //mov rcx, 2
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  aTempBuf[m++] = 0x02;
    nktMemSet(aTempBuf+m, 0x00, 7); m+=7;
    //mov rdx, address of array of ENGINE PROC HANDLE and CONTINUE EVENT HANDLE
    k = (SIZE_T)lpRemoteCode;
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xBA;  nktMemCopy(aTempBuf+m, &k, 8);  m+=8;
    //mov r8, FALSE
    aTempBuf[m++] = 0x49;  aTempBuf[m++] = 0xB8;  nktMemSet(aTempBuf+m, 0x00, 8);  m+=8;
    //mov r9, INFINITE
    aTempBuf[m++] = 0x49;  aTempBuf[m++] = 0xB9;  nktMemSet(aTempBuf+m, 0xFF, 4);  m+=4;
    nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    //mov rax, ADDRESS OF WaitForMultipleObjects
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nWaitMultObjAddr, 8);  m+=8;
    //call rax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //mov rcx, CONTINUE EVENT HANDLE
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &hContinueEventCopy, 8);  m+=8;
    //mov rax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 8);  m+=8;
    //call rax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //mov rcx, ENGINE PROC HANDLE
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &hEngineProcCopy, 8);  m+=8;
    //mov rax, ADDRESS OF CloseHandle
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &nCloseHandleAddr, 8);  m+=8;
    //call rax
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0xD0;
    //add rsp, 48h
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0x83;  aTempBuf[m++] = 0xC4;  aTempBuf[m++] = 0x48;
    //mov rax, lpRemoteData
    k = (SIZE_T)lpRemoteData;
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf + m, &k, 8);  m += 8;
    //push QWORD PTR [rax]
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x30;
    //ret
    aTempBuf[m++] = 0xC3;
    //NEW STUB
    //at this point rcx has the original entry point, store it in lpRemoteCode using interlocked
    nNewStubOfs = m;
    //push rax
    aTempBuf[m++] = 0x50;
    //mov rax, lpRemoteData
    k = (SIZE_T)lpRemoteData;
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB8;  nktMemCopy(aTempBuf+m, &k, 8);  m+=8;
    //lock xchg rcx, [rax]
    aTempBuf[m++] = 0xF0;  aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0x87;  aTempBuf[m++] = 0x08;
    //mov rcx, NEW ENTRY POINT
    k = (SIZE_T)lpRemoteCode + nNewEntryPointOfs;
    aTempBuf[m++] = 0x48;  aTempBuf[m++] = 0xB9;  nktMemCopy(aTempBuf+m, &k, 8);  m+=8;
    //pop rax
    aTempBuf[m++] = 0x58;
    //jmp to original rip
    aTempBuf[m++] = 0xFF;  aTempBuf[m++] = 0x25;  nktMemSet(aTempBuf+m, 0x00, 4);  m+=4;
    nktMemCopy(aTempBuf+m, &nOrigIP, 8);  m+=8;
  }
  //write memory
  if (::WriteProcessMemory(hSuspendedProc, lpRemoteCode, aTempBuf, m, &nWritten) == FALSE ||
      nWritten != m)
  {
    ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
    ::VirtualFreeEx(hSuspendedProc, lpRemoteData, 0, MEM_RELEASE);
    return E_FAIL;
  }
  //flush cache and change page protection
  ::FlushInstructionCache(hSuspendedProc, lpRemoteCode, 4096);
  dwTemp32 = 0;
  ::VirtualProtectEx(hSuspendedProc, lpRemoteCode, 4096, PAGE_EXECUTE_READ, &dwTemp32);
  //set new entry point address
#if defined _M_IX86
  sThreadCtx.Eip = (DWORD)lpRemoteCode + nNewStubOfs;
  if (::SetThreadContext(hSuspendedMainThread, &sThreadCtx) == FALSE)
  {
    ::VirtualFreeEx(hSuspendedProc, lpRemoteData, 0, MEM_RELEASE);
    ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
    return E_FAIL;
  }
#elif defined _M_X64
  if (bIs64BitProcess == FALSE)
  {
    sWow64ThreadCtx.Eip = (DWORD)((ULONG_PTR)(lpRemoteCode + nNewStubOfs));
    hRes = nktDvDynApis_Wow64SetThreadContext(hSuspendedMainThread, &sWow64ThreadCtx);
    if (FAILED(hRes))
    {
      ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
      ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
      return hRes;
    }
  }
  else
  {
    sThreadCtx.Rip = (DWORD64)(lpRemoteCode + nNewStubOfs);
    if (::SetThreadContext(hSuspendedMainThread, &sThreadCtx) == FALSE)
    {
      ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
      ::VirtualFreeEx(hSuspendedProc, lpRemoteCode, 0, MEM_RELEASE);
      return E_FAIL;
    }
  }
#endif
  NKT_DEBUGPRINTLNA(Nektra::dlTools, ("%lu) Tools[SuspendAfterCreateProcessW]: RemoteCode=%IXh",
                     ::GetTickCount(), lpRemoteCode));
  //done
  *lphReadyExecutionEvent = cReadyEvent.Detach();
  *lphContinueExecutionEvent = cContinueEvent.Detach();
  return S_OK;
}

HRESULT CNktDvTools::GetModules(__in HANDLE hProcess, __in SIZE_T nPlatformBits,
                                __inout TNktArrayList<HMODULE> &aModList)
{
  DWORD dwTries, dwNeeded;
  HRESULT hRes;
  union {
    NKT_DV_LDRENTRY32 sLdrEntry32;
    NKT_DV_LDRENTRY64 sLdrEntry64;
  };
#if defined _M_X64
  DWORD dwTemp32;
#endif //_M_X64
  LPBYTE lpPeb;

  aModList.RemoveAllElements();
  if (hProcess == NULL)
    return E_INVALIDARG;
#if defined _M_IX86
  if (nPlatformBits != 32)
    return E_INVALIDARG;
#elif defined _M_X64
  switch (nPlatformBits)
  {
    case 32:
      dwTemp32 = LIST_MODULES_32BIT;
      break;
    case 64:
      dwTemp32 = LIST_MODULES_64BIT;
      break;
    default:
      return E_INVALIDARG;
  }
#else
  #error Unsupported platform
#endif
  for (dwNeeded=128,dwTries=GETMODULESLIST_RETRIES; dwTries>0; dwTries--)
  {
    if (dwNeeded < 128)
      dwNeeded = 128;
    if (aModList.SetSize((SIZE_T)dwNeeded) == FALSE)
      return E_OUTOFMEMORY;
#if defined _M_IX86
    hRes = nktDvDynApis_EnumProcessModules(hProcess, aModList.GetBuffer(),
                                           dwNeeded * (DWORD)sizeof(HMODULE), &dwNeeded);
#elif defined _M_X64
    hRes = nktDvDynApis_EnumProcessModulesEx(hProcess, aModList.GetBuffer(),
                                             dwNeeded * (DWORD)sizeof(HMODULE), &dwNeeded, dwTemp32);
#endif
    if (SUCCEEDED(hRes))
    {
      dwNeeded /= (DWORD)sizeof(HMODULE);
      if ((SIZE_T)dwNeeded > aModList.GetSize())
      {
        dwTries++;
        continue;
      }
      break;
    }
    if (hRes == E_NOTIMPL)
      goto undocumented_method;
    if (hRes != NKT_HRESULT_FROM_WIN32(ERROR_PARTIAL_COPY) &&
        hRes != NKT_HRESULT_FROM_WIN32(ERROR_BAD_LENGTH))
      return hRes;
    ::Sleep(10);
  }
  if (dwTries == 0)
    goto undocumented_method; //try undocumented
  if (aModList.SetCount((SIZE_T)dwNeeded) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;

undocumented_method:
  for (dwTries=GETMODULESLIST_UNDOCUMENTED_RETRIES; dwTries>0; dwTries--)
  {
    aModList.RemoveAllElements();
    switch (nPlatformBits)
    {
      case 32:
        hRes = nktDvNtGetPeb(&lpPeb, hProcess, 32);
        if (SUCCEEDED(hRes) && lpPeb != NULL)
        {
          hRes = nktDvNtGetFirstLdrEntry32(&sLdrEntry32, lpPeb, hProcess);
          while (SUCCEEDED(hRes))
          {
            if (sLdrEntry32.sEntry.DllBase != 0)
            {
              if (aModList.AddElement((HMODULE)((ULONG_PTR)(sLdrEntry32.sEntry.DllBase))) == FALSE)
              {
                hRes = E_OUTOFMEMORY;
                break;
              }
            }
            hRes = nktDvNtGetNextLdrEntry32(&sLdrEntry32);
          }
          if (hRes == NKT_DVERR_NotFound)
            hRes = S_OK;
        }
        break;

#if defined _M_X64
      case 64:
        hRes = nktDvNtGetPeb(&lpPeb, hProcess, 64);
        if (SUCCEEDED(hRes) && lpPeb != NULL)
        {
          hRes = nktDvNtGetFirstLdrEntry64(&sLdrEntry64, lpPeb, hProcess);
          while (SUCCEEDED(hRes))
          {
            if (sLdrEntry64.sEntry.DllBase != 0)
            {
              if (aModList.AddElement((HMODULE)(sLdrEntry64.sEntry.DllBase)) == FALSE)
              {
                hRes = E_OUTOFMEMORY;
                break;
              }
            }
            hRes = nktDvNtGetNextLdrEntry64(&sLdrEntry64);
          }
          if (hRes == NKT_DVERR_NotFound)
            hRes = S_OK;
        }
        break;
#endif //_M_X64

      default:
        return E_FAIL;
    }
    if (hRes != E_FAIL)
      break;
    if (dwTries > 1)
      ::Sleep(10);
    else
      hRes = E_FAIL;
  }
  return hRes;
}

HRESULT CNktDvTools::GetMappedFiles(__in DWORD dwProcessId, __in HANDLE hProcess, __in SIZE_T nPlatformBits,
                                    __in HMODULE *lpModList, __in SIZE_T nModListCount,
                                    __inout TNktArrayListWithDelete<MODULE_INFO*> &aMapList)
{
  MEMORY_BASIC_INFORMATION sMbi;
  LPBYTE lpBaseAddr, lpCurrAddress;
  BOOL bContinue;
  SIZE_T i, nBlockSize;
  LPWSTR sW;
  TNktAutoDeletePtr<MODULE_INFO> sModInfo;
  NKT_DV_IMAGE_NT_HEADER sNtHdr;
  HRESULT hRes;
  DWORD dwType;

  lpCurrAddress = NULL;
  bContinue = TRUE;
  while (bContinue != FALSE)
  {
    //----
    if (::VirtualQueryEx(hProcess, lpCurrAddress, &sMbi, sizeof(sMbi)) == 0)
      break;
    if (sMbi.Type == MEM_MAPPED || sMbi.Type == MEM_IMAGE)
    {
      dwType = sMbi.Type;
      lpBaseAddr = (LPBYTE)(sMbi.AllocationBase);
      nBlockSize = 0;
      do
      {
        nBlockSize += sMbi.RegionSize;
        lpCurrAddress += sMbi.RegionSize;
        if (::VirtualQueryEx(hProcess, lpCurrAddress, &sMbi, sizeof(sMbi)) == 0)
        {
          bContinue = FALSE;
          break;
        }
      }
      while (lpBaseAddr == (LPBYTE)(sMbi.AllocationBase));
      //skip modules in hModules list
      for (i=0; i<nModListCount; i++)
      {
        if (lpBaseAddr == (LPBYTE)(lpModList[i]))
          break;
      }
      if (i < nModListCount)
        continue;
      //check images
      if (dwType == MEM_IMAGE)
      {
        hRes = nktDvNtCheckImageType(&sNtHdr, hProcess, lpBaseAddr);
        if ((nPlatformBits == 32 && hRes == (HRESULT)64) ||
            (nPlatformBits == 64 && hRes == (HRESULT)32))
          continue;
      }
      sModInfo.Attach(new MODULE_INFO);
      if (sModInfo == NULL)
        return E_OUTOFMEMORY;
      sModInfo->dwProcessId = dwProcessId;
      sModInfo->hModule = (HMODULE)lpBaseAddr;
      sModInfo->lpBaseAddr = lpBaseAddr;
      sModInfo->nBaseSize = nBlockSize;
      sModInfo->nPlatformBits = (dwType == MEM_IMAGE) ? nPlatformBits : 0;
      //get module file name
      hRes = GetExecutableFileName(sModInfo->cStrExePathW, hProcess, sModInfo->hModule);
      if (FAILED(hRes))
      {
        if (hRes != E_FAIL && hRes != E_HANDLE)
          return hRes;
        continue;
      }
      if (sModInfo->cStrExePathW.IsEmpty() != FALSE)
        continue;
      //get base name
      sW = (LPWSTR)(sModInfo->cStrExePathW);
      sModInfo->nOffsetToModuleName = wcslen(sW);
      while (sModInfo->nOffsetToModuleName > 0 && sW[sModInfo->nOffsetToModuleName-1] != L'\\' &&
             sW[sModInfo->nOffsetToModuleName-1] != L'/')
         sModInfo->nOffsetToModuleName--;
      //generate key
      sModInfo->nKey = fnv_64a_buf((LPWSTR)(sModInfo->cStrExePathW), sModInfo->cStrExePathW.GetLength()*sizeof(WCHAR),
                                   FNV1A_64_INIT);
      sModInfo->nKey = fnv_64a_buf(&(sModInfo->hModule), sizeof(sModInfo->hModule), sModInfo->nKey);
      if (dwType == MEM_IMAGE)
      {
        switch (nPlatformBits)
        {
          case 32:
            sModInfo->nKey = fnv_64a_buf(&(sNtHdr.u32), sizeof(sNtHdr.u32), sModInfo->nKey);
            break;
          case 64:
            sModInfo->nKey = fnv_64a_buf(&(sNtHdr.u64), sizeof(sNtHdr.u64), sModInfo->nKey);
            break;
        }
      }
      else
      {
        sModInfo->nKey = fnv_64a_buf(&dwProcessId, sizeof(dwProcessId), FNV1A_64_INIT);
        sModInfo->nKey = fnv_64a_buf(&nBlockSize, sizeof(nBlockSize), sModInfo->nKey);
      }
      //add to list
      if (aMapList.AddElement(sModInfo.Get()) == FALSE)
        return E_OUTOFMEMORY;
      sModInfo.Detach();
    }
    else
    {
      //skip block
      lpCurrAddress += sMbi.RegionSize;
    }
  }
  return S_OK;
}

HRESULT CNktDvTools::GetModuleInfo(__in DWORD dwProcessId, __in HANDLE hProcess, __in HMODULE hModule,
                                   __inout MODULE_INFO &sModInfo)
{
  MEMORY_BASIC_INFORMATION sMbi;
  NKT_DV_IMAGE_NT_HEADER sNtHdr;
  LPBYTE lpPeb;
  union {
    NKT_DV_LDRENTRY32 sLdrEntry32;
    NKT_DV_LDRENTRY64 sLdrEntry64;
  };
  SIZE_T nProcPlatformBits;
  LPBYTE lpCurrAddress;
  DWORD dwType;
  LPWSTR sW;
  HRESULT hRes;

  sModInfo.dwProcessId = dwProcessId;
  sModInfo.hModule = hModule;
  sModInfo.lpBaseAddr = (LPBYTE)hModule;
  sModInfo.nBaseSize = 0;
  sModInfo.cStrExePathW.Empty();
  sModInfo.nOffsetToModuleName = 0;
  sModInfo.nPlatformBits = 0;
  sModInfo.nKey = 0;
  if (hProcess == NULL || hModule == NULL)
    return E_INVALIDARG;
  if (::VirtualQueryEx(hProcess, sModInfo.lpBaseAddr, &sMbi, sizeof(sMbi)) == 0)
    return NKT_HRESULT_FROM_LASTERROR();
  switch (dwType = sMbi.Type)
  {
    case MEM_IMAGE:
      hRes = nktDvNtCheckImageType(&sNtHdr, hProcess, sModInfo.lpBaseAddr);
      if (FAILED(hRes))
      {
        NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) GetModuleInfo[CheckImgType]: Pid=%04X, hMod=%IXh / hRes=%08X",
                                              ::GetTickCount(), dwProcessId, hModule, hRes));
        return hRes;
      }
      sModInfo.nPlatformBits = (SIZE_T)hRes;
      //check if it is a real mapped image
      nProcPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
      switch (nProcPlatformBits)
      {
        case 32:
          hRes = nktDvNtGetPeb(&lpPeb, hProcess, 32);
          if (SUCCEEDED(hRes) && lpPeb != NULL)
          {
            hRes = nktDvNtGetFirstLdrEntry32(&sLdrEntry32, lpPeb, hProcess);
            while (SUCCEEDED(hRes))
            {
              if ((LPBYTE)((ULONG_PTR)(sLdrEntry32.sEntry.DllBase)) == sModInfo.lpBaseAddr)
                break;
              hRes = nktDvNtGetNextLdrEntry32(&sLdrEntry32);
            }
          }
          break;

#if defined _M_X64
        case 64:
          hRes = nktDvNtGetPeb(&lpPeb, hProcess, 64);
          if (SUCCEEDED(hRes) && lpPeb != NULL)
          {
            hRes = nktDvNtGetFirstLdrEntry64(&sLdrEntry64, lpPeb, hProcess);
            while (SUCCEEDED(hRes))
            {
              if ((LPBYTE)(sLdrEntry64.sEntry.DllBase) == sModInfo.lpBaseAddr)
                break;
              hRes = nktDvNtGetNextLdrEntry64(&sLdrEntry64);
            }
          }
          break;
#endif //_M_X64

        default:
          hRes = E_FAIL;
          break;
      }
      if (hRes == NKT_DVERR_NotFound)
      {
        dwType = MEM_MAPPED;
        sModInfo.nPlatformBits = 0;
        goto _isMemMapped;
      }
      if (FAILED(hRes))
      {
        NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) GetModuleInfo[Check4RealDll]: Pid=%04X, hMod=%IXh / "
                          "hRes=%08X", ::GetTickCount(), dwProcessId, hModule, hRes));
        return hRes;
      }
      break;

    case MEM_MAPPED:
_isMemMapped:
      lpCurrAddress = sModInfo.lpBaseAddr;
      do
      {
        sModInfo.nBaseSize += sMbi.RegionSize;
        lpCurrAddress += sMbi.RegionSize;
        if (::VirtualQueryEx(hProcess, lpCurrAddress, &sMbi, sizeof(sMbi)) == 0)
          break;
      }
      while (sModInfo.lpBaseAddr == (LPBYTE)(sMbi.AllocationBase));
      break;

    default:
      return E_FAIL;
  }
  //get module file name
  hRes = GetExecutableFileName(sModInfo.cStrExePathW, hProcess, sModInfo.hModule);
  if (FAILED(hRes))
  {
    NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) GetModuleInfo[GetExeName]: Pid=%04X, hMod=%IXh / "
                      "hRes=%08X", ::GetTickCount(), dwProcessId, hModule, hRes));
    return (hRes != E_HANDLE) ? hRes : E_FAIL;
  }
  if (sModInfo.cStrExePathW.IsEmpty() != FALSE)
    return E_FAIL;
  //get base name
  sW = (LPWSTR)(sModInfo.cStrExePathW);
  sModInfo.nOffsetToModuleName = wcslen(sW);
  while (sModInfo.nOffsetToModuleName > 0 && sW[sModInfo.nOffsetToModuleName-1] != L'\\' &&
         sW[sModInfo.nOffsetToModuleName-1] != L'/')
    sModInfo.nOffsetToModuleName--;
  //generate key
  sModInfo.nKey = fnv_64a_buf((LPWSTR)(sModInfo.cStrExePathW), sModInfo.cStrExePathW.GetLength()*sizeof(WCHAR),
                              FNV1A_64_INIT);
  sModInfo.nKey = fnv_64a_buf(&(sModInfo.hModule), sizeof(sModInfo.hModule), sModInfo.nKey);
  if (dwType == MEM_IMAGE)
  {
    switch (sModInfo.nPlatformBits)
    {
      case 32:
        sModInfo.nKey = fnv_64a_buf(&(sNtHdr.u32), sizeof(sNtHdr.u32), sModInfo.nKey);
        //get size of image
        sModInfo.nBaseSize = (SIZE_T)(sNtHdr.u32.OptionalHeader.SizeOfImage);
        break;
#if defined _M_X64
      case 64:
        sModInfo.nKey = fnv_64a_buf(&(sNtHdr.u64), sizeof(sNtHdr.u64), sModInfo.nKey);
        //get size of image
        sModInfo.nBaseSize = (SIZE_T)(sNtHdr.u32.OptionalHeader.SizeOfImage);
        break;
#endif //_M_X64
      default:
        NKT_DEBUGPRINTLNA(Nektra::dlWarning, ("%lu) GetModuleInfo[Platform]: Pid=%04X, hMod=%IXh / "
                                              "hRes=%08X", ::GetTickCount(), dwProcessId, hModule, hRes));
        return E_FAIL;
    }
  }
  else
  {
    sModInfo.nKey = fnv_64a_buf(&dwProcessId, sizeof(dwProcessId), FNV1A_64_INIT);
    sModInfo.nKey = fnv_64a_buf(&(sModInfo.nBaseSize), sizeof(sModInfo.nBaseSize), sModInfo.nKey);
  }
  return S_OK;
}

HRESULT CNktDvTools::GetProcAddressW(__out LPVOID *lplpProc, __in HINSTANCE hDll, __in LPCWSTR szProcNameW)
{
  LPSTR szProcNameA;

  if (lplpProc == NULL)
    return E_POINTER;
  *lplpProc = NULL;
  szProcNameA = CNktDvTools::Wide2Ansi(szProcNameW, NKT_SIZE_T_MAX);
  if (szProcNameA == NULL)
    return E_OUTOFMEMORY;
  *lplpProc = ::GetProcAddress(hDll, szProcNameA);
  nktMemFree(szProcNameA);
  return ((*lplpProc) != NULL) ? S_OK : NKT_DVERR_NotFound;
}

HRESULT CNktDvTools::GetSystem32Path(__inout CNktStringW &cStrPathW)
{
  LPWSTR sW;
  SIZE_T nLen, nBufSize;

  for (nBufSize=4096; nBufSize<409600; nBufSize+=4096)
  {
    if (cStrPathW.EnsureBuffer(nBufSize) == FALSE)
      return E_OUTOFMEMORY;
    nLen = (SIZE_T)::GetSystemDirectory(cStrPathW, (UINT)nBufSize);
    if (nLen < nBufSize-4)
      break;
  }
  if (nBufSize >= 409600)
    return E_OUTOFMEMORY;
  sW = (LPWSTR)cStrPathW;
  if (nLen > 0 && sW[nLen-1] != L'\\' && sW[nLen-1] != L'/')
    sW[nLen++] = L'\\';
  sW[nLen] = 0;
  cStrPathW.Refresh();
  return FixFolderStr(cStrPathW);
}

HRESULT CNktDvTools::GetModuleFullName(__inout CNktStringW &cStrPathW, __in HINSTANCE hDll)
{
  SIZE_T i, nLen, nBufSize;
  LPWSTR sW;

  for (nBufSize=4096; nBufSize<409600; nBufSize+=4096)
  {
    if (cStrPathW.EnsureBuffer(nBufSize) == FALSE)
      return E_OUTOFMEMORY;
    nLen = (SIZE_T)::GetModuleFileNameW(hDll, cStrPathW, (DWORD)nBufSize);
    if (nLen < nBufSize-4)
      break;
  }
  if (nBufSize >= 409600)
    return E_OUTOFMEMORY;
  sW = (LPWSTR)cStrPathW;
  for (i=0; i<nLen; i++)
  {
    if (sW[i] == L'/')
      sW[i] = L'\\';
  }
  sW[nLen] = 0;
  cStrPathW.Refresh();
  return S_OK;
}

HRESULT CNktDvTools::GetModulePath(__inout CNktStringW &cStrPathW, __in HINSTANCE hDll)
{
  HRESULT hRes;
  LPWSTR sW;
  SIZE_T nLen;

  hRes = GetModuleFullName(cStrPathW, hDll);
  if (SUCCEEDED(hRes))
  {
    sW = (LPWSTR)cStrPathW;
    nLen = cStrPathW.GetLength();
    while (nLen > 0 && sW[nLen-1] != L'\\')
      nLen--;
    sW[nLen] = 0;
    cStrPathW.Refresh();
  }
  return hRes;
}

HRESULT CNktDvTools::FixFolderStr(__inout CNktStringW &cStrPathW)
{
  SIZE_T i, nLen;
  LPWSTR sW;

  nLen = cStrPathW.GetLength();
  if (nLen > 0)
  {
    sW = (LPWSTR)cStrPathW;
    for (i=0; i<nLen; i++)
    {
      if (sW[i] == L'/')
        sW[i] = L'\\';
    }
    if (nLen > 0 && sW[nLen-1] != L'\\')
    {
      if (cStrPathW.Concat(L"\\") == FALSE)
        return E_OUTOFMEMORY;
    }
  }
  return S_OK;
}

HRESULT CNktDvTools::GetImageNtHeaders(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in DWORD dwPid,
                                       __in LPBYTE lpBaseAddr)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HANDLE hProcess;
  LPBYTE lpPeb;
  HRESULT hRes;
  SIZE_T nPlatformBits;

  if (dwPid == 0)
    return E_INVALIDARG;
  if (lpNtHdr == NULL)
    return E_POINTER;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProcess = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
  if (hProcess == NULL)
    return E_ACCESSDENIED;
  nPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
  //find process base address
  if (lpBaseAddr == NULL)
  {
    hRes = nktDvNtGetPeb(&lpPeb, hProcess, nPlatformBits);
    if (FAILED(hRes))
    {
      ::CloseHandle(hProcess);
      return hRes;
    }
    if (lpPeb == NULL)
    {
      ::CloseHandle(hProcess);
      return E_FAIL;
    }
    switch (nPlatformBits)
    {
      case 32:
        hRes = nktDvNtGetBaseAddress32(&lpBaseAddr, lpPeb, hProcess);
        break;
#if defined _M_X64
      case 64:
        hRes = nktDvNtGetBaseAddress64(&lpBaseAddr, lpPeb, hProcess);
        break;
#endif //_M_X64
      default:
        ::CloseHandle(hProcess);
        return E_FAIL;
    }
  }
  //check image type
  hRes = nktDvNtCheckImageType(lpNtHdr, hProcess, lpBaseAddr, NULL);
  ::CloseHandle(hProcess);
  if (FAILED(hRes))
    return hRes;
  switch (nPlatformBits)
  {
    case 32:
      if ((SIZE_T)hRes != 32)
        return E_FAIL; //should be the same
      break;
#if defined _M_X64
    case 64:
      if ((SIZE_T)hRes != 32 && (SIZE_T)hRes != 64)
        return E_FAIL;
      break;
#endif //_M_X64
  }
  return (HRESULT)nPlatformBits;
}

HRESULT CNktDvTools::GetImageNtHeaders(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in HANDLE hProcess,
                                       __in LPBYTE lpBaseAddr)
{
  DWORD dwPid;
  HRESULT hRes;

  hRes = nktDvDynApis_GetProcessId(hProcess, &dwPid);
  if (FAILED(hRes))
    return hRes;
  return GetImageNtHeaders(lpNtHdr, dwPid, lpBaseAddr);
}

HRESULT CNktDvTools::GetImageSections(__out_opt PROCESSSECTION **lplpSections, __in DWORD dwPid,
                                      __in LPBYTE lpBaseAddr)
{
  TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
  HANDLE hProcess;
  LPBYTE lpPeb, lpNtHdrAddr;
  HRESULT hRes;
  SIZE_T i, nPlatformBits, nSecCount;
  NKT_DV_IMAGE_NT_HEADER sNtHdr;
  IMAGE_SECTION_HEADER *lpImgSectHdr, sImgSectHdrCopy;

  if (dwPid == 0)
    return E_INVALIDARG;
  if (lplpSections != NULL)
    *lplpSections = NULL;
  hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
  if (FAILED(hRes))
    return hRes;
  hProcess = cPhMgr->GetHandle(dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
  if (hProcess == NULL)
    return E_ACCESSDENIED;
  nPlatformBits = (nktDvDynApis_Is64BitProcess(hProcess) != FALSE) ? 64 : 32;
  //find process base address
  if (lpBaseAddr == NULL)
  {
    hRes = nktDvNtGetPeb(&lpPeb, hProcess, nPlatformBits);
    if (FAILED(hRes))
    {
      ::CloseHandle(hProcess);
      return hRes;
    }
    if (lpPeb == NULL)
    {
      ::CloseHandle(hProcess);
      return E_FAIL;
    }
    switch (nPlatformBits)
    {
      case 32:
        hRes = nktDvNtGetBaseAddress32(&lpBaseAddr, lpPeb, hProcess);
        break;
#if defined _M_X64
      case 64:
        hRes = nktDvNtGetBaseAddress64(&lpBaseAddr, lpPeb, hProcess);
        break;
#endif //_M_X64
      default:
        ::CloseHandle(hProcess);
        return E_FAIL;
    }
  }
  //check image type
  hRes = nktDvNtCheckImageType(&sNtHdr, hProcess, lpBaseAddr, &lpNtHdrAddr);
  if (FAILED(hRes))
  {
    ::CloseHandle(hProcess);
    return hRes;
  }
  switch (nPlatformBits)
  {
    case 32:
      if ((SIZE_T)hRes != 32)
      {
        ::CloseHandle(hProcess);
        return E_FAIL; //should be the same
      }
      nSecCount = (SIZE_T)(sNtHdr.u32.FileHeader.NumberOfSections);
      lpImgSectHdr = (IMAGE_SECTION_HEADER*)(lpNtHdrAddr + sizeof(sNtHdr.u32));
      break;
#if defined _M_X64
    case 64:
      if ((SIZE_T)hRes != 32 && (SIZE_T)hRes != 64)
      {
        ::CloseHandle(hProcess);
        return E_FAIL;
      }
      nSecCount = (SIZE_T)(sNtHdr.u64.FileHeader.NumberOfSections);
      lpImgSectHdr = (IMAGE_SECTION_HEADER*)(lpNtHdrAddr + sizeof(sNtHdr.u64));
      break;
#endif //_M_X64
  }
  if (nSecCount > 0 && lplpSections != NULL)
  {
    *lplpSections = (PROCESSSECTION*)nktMemMalloc(nSecCount*sizeof(PROCESSSECTION));
    if (*lplpSections == NULL)
    {
      ::CloseHandle(hProcess);
      return E_OUTOFMEMORY;
    }
    for (i=0; i<nSecCount; i++)
    {
      if (ReadProcMem(hProcess, &sImgSectHdrCopy, lpImgSectHdr+i, sizeof(sImgSectHdrCopy)) == FALSE)
      {
        nktMemFree(*lplpSections);
        *lplpSections = NULL;
        ::CloseHandle(hProcess);
        return E_ACCESSDENIED;
      }
      nktMemCopy((*lplpSections)[i].szName, sImgSectHdrCopy.Name, IMAGE_SIZEOF_SHORT_NAME);
      (*lplpSections)[i].dwCharacteristics = sImgSectHdrCopy.Characteristics;
      (*lplpSections)[i].nSectionStart = (SIZE_T)lpBaseAddr + (SIZE_T)sImgSectHdrCopy.VirtualAddress;
      (*lplpSections)[i].nSectionEnd = (*lplpSections)[i].nSectionStart +
                                       (SIZE_T)sImgSectHdrCopy.Misc.VirtualSize;
    }
  }
  ::CloseHandle(hProcess);
  return (HRESULT)nSecCount;
}

HRESULT CNktDvTools::GetImageSections(__out_opt PROCESSSECTION **lplpSections, __in HANDLE hProcess,
                                        __in LPBYTE lpBaseAddr)
{
  DWORD dwPid;
  HRESULT hRes;

  hRes = nktDvDynApis_GetProcessId(hProcess, &dwPid);
  if (FAILED(hRes))
    return hRes;
  return GetImageSections(lplpSections, dwPid, lpBaseAddr);
}

SIZE_T CNktDvTools::GetModuleUsageCount(__in HINSTANCE hDll)
{
  SIZE_T nRes;
  PVOID lpCookie;
  NKT_DV_LDR_DATA_TABLE_ENTRY *lpEntry;
  HRESULT hRes;
  BOOL bLocked;

  if (hDll == NULL)
    return 0;
  nRes = NKT_SIZE_T_MAX; //assume fixed dll
  bLocked = (SUCCEEDED(nktDvNtLockLoader(&lpCookie))) ? TRUE : FALSE;
  hRes = nktDvDynApis_LdrFindEntryForAddress((PVOID)hDll, (PVOID*)&lpEntry);
  if (hRes == S_OK && lpEntry->LoadCount != 0xFFFF)
    nRes = lpEntry->LoadCount;
  if (bLocked != FALSE)
    nktDvNtUnlockLoader(lpCookie);
  return nRes;
}

SIZE_T CNktDvTools::GetModuleSize(__in HINSTANCE hDll)
{
  IMAGE_DATA_DIRECTORY sDataDirEntry;
#if defined _M_IX86
  IMAGE_NT_HEADERS32 *lpNtHdr;
#elif defined _M_X64
  IMAGE_NT_HEADERS64 *lpNtHdr;
#else
  #error Unsupported platform
#endif
  SIZE_T i, j, nImageSize;
  DWORD dwTemp32;
  WORD wTemp16;
  LPBYTE lpBaseAddr;

  lpBaseAddr = (LPBYTE)hDll;
  if (lpBaseAddr == NULL)
    return 0;
  //check dos signature
  nktMemCopy(&wTemp16, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic), sizeof(WORD));
  if (wTemp16 != IMAGE_DOS_SIGNATURE)
    return 0;
  //get header offset
  nktMemCopy(&dwTemp32, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew), sizeof(DWORD));
#if defined _M_IX86
  lpNtHdr = (IMAGE_NT_HEADERS32*)(lpBaseAddr + (SIZE_T)dwTemp32);
#elif defined _M_X64
  lpNtHdr = (IMAGE_NT_HEADERS64*)(lpBaseAddr + (SIZE_T)dwTemp32);
#endif
  nImageSize = (SIZE_T)(lpNtHdr->OptionalHeader.SizeOfImage);
  for (i=0; i<IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
  {
    nktMemCopy(&sDataDirEntry, &(lpNtHdr->OptionalHeader.DataDirectory[i]), sizeof(sDataDirEntry));
    if (sDataDirEntry.VirtualAddress != NULL && sDataDirEntry.Size != 0)
    {
      j = (SIZE_T)sDataDirEntry.VirtualAddress + (SIZE_T)sDataDirEntry.Size;
      if (j > nImageSize)
        nImageSize = j;
    }
  }
  return nImageSize;
}

LPVOID CNktDvTools::GetApiAddress(__in HINSTANCE hDll, __in_z LPCSTR szAddressA, __in BOOL bIgnoreShims)
{
  LPVOID lpAddr;

  if (bIgnoreShims == FALSE)
  {
    lpAddr = ::GetProcAddress(hDll, szAddressA);
  }
  else
  {
    if (FAILED(nktDvDynApis_LdrGetProcedureAddress((HMODULE)hDll, szAddressA, &lpAddr)))
      lpAddr = NULL;
  }
  return lpAddr;
}

HRESULT CNktDvTools::GetFileNameFromHandle(__inout CNktStringW &cStrFileNameW, __in HANDLE hFile,
                                           __in DWORD dwPid)
{
  THTDATA_GETFILENAMEFROMHANDLE sData;

  cStrFileNameW.Empty();
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
    return E_INVALIDARG;
  sData.hFile = hFile;
  sData.dwPid = dwPid;
  sData.lpStrFileNameW = &cStrFileNameW;
  return ToolsHelperThread_Exec(THTDATA_CODE_GetFileNameFromHandle, &sData);
}

DWORD CNktDvTools::GetThreadId(__in HANDLE hThread)
{
  NKT_DV_THREAD_BASIC_INFORMATION sTbi;
  HRESULT hRes;

  if (hThread == NULL)
    return 0;
  hRes = nktDvDynApis_NtQueryInformationThread(hThread, NKT_DV_ThreadBasicInformation, &sTbi, sizeof(sTbi), NULL);
  if (FAILED(hRes))
    return 0;
  return (DWORD)(sTbi.ClientId.UniqueThread);
}

MANDATORY_LEVEL CNktDvTools::GetProcessIntegrityLevel(__in DWORD dwPid)
{
  HANDLE hProc, hToken;
  MANDATORY_LEVEL nLevel;
  PTOKEN_MANDATORY_LABEL lpBuf;
  DWORD dwLen, dwIntegrityLevel;

  nLevel = MandatoryLevelUntrusted;
  if (dwPid == 0)
    return nLevel;
  hProc = ::OpenProcess(MAXIMUM_ALLOWED, FALSE, dwPid);
  if (hProc != NULL)
  {
    if (::OpenProcessToken(hProc, MAXIMUM_ALLOWED, &hToken) != FALSE)
    {
      dwLen = 0;
      ::GetTokenInformation(hToken, TokenIntegrityLevel, NULL, 0, &dwLen);
      if (dwLen > 0)
      {
        lpBuf = (PTOKEN_MANDATORY_LABEL)malloc(dwLen);
        if (lpBuf != NULL)
        {
          if (::GetTokenInformation(hToken, TokenIntegrityLevel, lpBuf, dwLen, &dwLen) != FALSE)
          {
            dwIntegrityLevel = *GetSidSubAuthority(lpBuf->Label.Sid,
                                                   (DWORD)(UCHAR)(*GetSidSubAuthorityCount(lpBuf->Label.Sid)-1));
            if (dwIntegrityLevel < SECURITY_MANDATORY_LOW_RID)
              nLevel = MandatoryLevelUntrusted;
            else if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
              nLevel = MandatoryLevelLow;
            else if (dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
              nLevel = MandatoryLevelMedium;
            else if (dwIntegrityLevel < SECURITY_MANDATORY_SYSTEM_RID)
              nLevel = MandatoryLevelHigh;
            else if (dwIntegrityLevel < SECURITY_MANDATORY_PROTECTED_PROCESS_RID)
              nLevel = MandatoryLevelSystem;
            else
              nLevel = MandatoryLevelSecureProcess;
          }
          free(lpBuf);
        }
      }
      ::CloseHandle(hToken);
    }
    ::CloseHandle(hProc);
  }
  return nLevel;
}

//-----------------------------------------------------------

static __inline BOOL ReadProcMem(__in HANDLE hProc, __out LPVOID lpDest, __in LPVOID lpSrc,
                                 __in SIZE_T nDataSize)
{
  SIZE_T nReaded;

  return (::ReadProcessMemory(hProc, lpSrc, lpDest, nDataSize, &nReaded) != FALSE &&
          nReaded == nDataSize) ? TRUE : FALSE;
}

static HRESULT ToolsHelperThread_Exec(__in LONG nCode, __inout LPVOID lpUserData)
{
  CNktSimpleLockNonReentrant cLock(&(sHelperThread.nMtx));
  HRESULT hRes;

  hRes = ToolsHelperThread_Start();
  if (FAILED(hRes))
    return hRes;
  NktInterlockedExchange(&(sHelperThread.nCode), nCode);
  InterlockedExchangePointer(&(sHelperThread.lpUserData), lpUserData);
  NktInterlockedExchange(&(sHelperThread.hRes), E_FAIL);
  if (::ResetEvent(sHelperThread.hEvents[2]) == FALSE ||
      ::SetEvent(sHelperThread.hEvents[1]) == FALSE)
    return NKT_HRESULT_FROM_LASTERROR();
  if (::WaitForSingleObject(sHelperThread.hEvents[2], TOOLS_HELPER_THREAD_WAIT_TIME) != WAIT_OBJECT_0)
  {
    ToolsHelperThread_Stop(FALSE);
    return E_FAIL;
  }
  return sHelperThread.hRes;
}

static HRESULT ToolsHelperThread_Start()
{
  unsigned int tid;
  SIZE_T i;

  for (i=0; i<NKT_DV_ARRAYLEN(sHelperThread.hEvents); i++)
  {
    if (sHelperThread.hEvents[i] == NULL)
    {
      sHelperThread.hEvents[i] = ::CreateEventW(NULL, (i != 1) ? TRUE : FALSE, FALSE, NULL);
      if (sHelperThread.hEvents[i] == NULL)
        return E_OUTOFMEMORY;
    }
  }
  if (sHelperThread.hThread == NULL)
  {
    sHelperThread.hThread = (HANDLE)_beginthreadex(NULL, 0, &ToolsHelperThreadProc, NULL, 0, &tid);
    if (sHelperThread.hThread == NULL)
      return E_OUTOFMEMORY;
  }
  return S_OK;
}

static VOID ToolsHelperThread_Stop(__in BOOL bWait)
{
  typedef LONG (__stdcall *lpfnRtlFreeUserThreadStack)(__in HANDLE procHandle, __in HANDLE threadHandle);
  lpfnRtlFreeUserThreadStack fnRtlFreeUserThreadStack;
  OSVERSIONINFO sOvi;
  HINSTANCE hNtDll;
  SIZE_T i;

  if (sHelperThread.hThread != NULL)
  {
    ::SetEvent(sHelperThread.hEvents[0]);
    if (bWait == FALSE || ::WaitForSingleObject(sHelperThread.hThread, 1000) != WAIT_OBJECT_0)
    {
      //terminate thread if didn't end successfully
      ::SuspendThread(sHelperThread.hThread);
      //on pre-vista call "RtlFreeUserThreadStack" to avoid leaks when terminating threads
      nktMemSet(&sOvi, 0, sizeof(sOvi));
      sOvi.dwOSVersionInfoSize = sizeof(sOvi);
      if (::GetVersionEx(&sOvi) != FALSE && sOvi.dwPlatformId == VER_PLATFORM_WIN32_NT && sOvi.dwMajorVersion < 6)
      {
        hNtDll = ::GetModuleHandleW(L"ntdll.dll");
        if (hNtDll != NULL)
        {
          fnRtlFreeUserThreadStack = (lpfnRtlFreeUserThreadStack)::GetProcAddress(hNtDll, "RtlFreeUserThreadStack");
          if (fnRtlFreeUserThreadStack != NULL)
            fnRtlFreeUserThreadStack(::GetCurrentProcess(), sHelperThread.hThread);
        }
      }
      //finish him!
      ::TerminateThread(sHelperThread.hThread, 0);
    }
    ::CloseHandle(sHelperThread.hThread);
    sHelperThread.hThread = NULL;
  }
  //destroy events
  for (i=0; i<NKT_DV_ARRAYLEN(sHelperThread.hEvents); i++)
  {
    if (sHelperThread.hEvents[i] != NULL)
    {
      ::CloseHandle(sHelperThread.hEvents[i]);
      sHelperThread.hEvents[i] = NULL;
    }
  }
  return;
}

static unsigned int __stdcall ToolsHelperThreadProc(__inout LPVOID lpParameter)
{
  DWORD dw;

  while (1)
  {
    dw = ::WaitForMultipleObjects(2, sHelperThread.hEvents, FALSE, INFINITE);
    if (dw == WAIT_OBJECT_0)
      break;
    if (dw == WAIT_OBJECT_0+1)
    {
      switch (sHelperThread.nCode)
      {
        case THTDATA_CODE_GetFileNameFromHandle:
          {
          THTDATA_GETFILENAMEFROMHANDLE *lpData;
          TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;
          HANDLE hFile, hProc;
          HRESULT hRes;

          lpData = (THTDATA_GETFILENAMEFROMHANDLE*)(sHelperThread.lpUserData);
          if (lpData->dwPid != 0 && lpData->dwPid != ::GetCurrentProcessId())
          {
            hFile = NULL;
            hRes = CNktDvProcessHandlesMgr::Get(&cPhMgr);
            if (SUCCEEDED(hRes))
            {
              hProc = cPhMgr->GetHandle(lpData->dwPid, PROCESS_QUERY_INFORMATION|PROCESS_VM_READ);
              if (hProc != NULL)
              {
                hRes = nktDvDynApis_DuplicateHandle(hProc, lpData->hFile, ::GetCurrentProcess(), &hFile, 0, FALSE,
                                                    DUPLICATE_SAME_ACCESS);
                if (FAILED(hRes))
                  hFile = NULL;
                ::CloseHandle(hProc);
              }
              else
              {
                hRes = E_ACCESSDENIED;
              }
            }
          }
          else
          {
            hFile = lpData->hFile;
            hRes = S_OK;
          }
          if (SUCCEEDED(hRes))
          {
            hRes = nktDvNtGetFileNameFromHandle(*(lpData->lpStrFileNameW), hFile);
            if (SUCCEEDED(hRes))
              hRes = CNktDvTools::Device2DosForm(*(lpData->lpStrFileNameW));
          }
          if (hFile != NULL && hFile != lpData->hFile)
            ::CloseHandle(hFile);
          NktInterlockedExchange(&(sHelperThread.hRes), hRes);
          }
          break;
      }
      ::SetEvent(sHelperThread.hEvents[2]);
    }
  }
  return 0;
}
