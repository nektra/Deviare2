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

#pragma once

#include "EngDefines.h"
#include <objbase.h>
#include <oaidl.h>
#include <ocidl.h>
#include <comip.h>
#include <ole2.h>
#include "Debug.h"
#include "StringLiteW.h"
#include "ArrayList.h"
#include "FnvHash.h"
#include "NtInternals.h"

//-----------------------------------------------------------

class CNktDvTools
{
public:
  typedef struct {
    DWORD dwProcessId;
    HMODULE hModule;
    LPBYTE lpBaseAddr;
    SIZE_T nBaseSize;
    CNktStringW cStrExePathW;
    SIZE_T nOffsetToModuleName;
    SIZE_T nPlatformBits;
    Fnv64_t nKey;
  } MODULE_INFO;

  typedef struct {
    SIZE_T nDllNameLen; //always begin at char 0, if len=0 => no dll name was provided
    SIZE_T nApiNameStart, nApiNameLen; // if len=0 => no api name was provided
    SIZE_T nApiOrdinal; //NKT_SIZE_T_MAX if api name is NOT an ordinal
  } FUNCTION_PARSE_INFO;

  typedef struct {
    char szName[IMAGE_SIZEOF_SHORT_NAME];
    SIZE_T nSectionStart;
    SIZE_T nSectionEnd;
    DWORD dwCharacteristics;
  } PROCESSSECTION;

  static VOID NotifyError(__in_z LPCWSTR szFormatW, ...);
  static VOID NotifyWarning(__in_z LPCWSTR szFormatW, ...);

  static HICON _ExtractIcon(__in_z LPCWSTR szExecutablePathW, __in int nIndex=0);
  static HRESULT _ExtractIcon(__deref_out IPicture** lplpIcon, __in_z LPCWSTR szExecutablePathW,
                              __in int nIndex=0);

  static HRESULT GetProcessUserName(__inout CNktStringW &cStrDestW, __in DWORD dwPid);
  static HRESULT GetProcessUserName(__inout CNktStringW &cStrDestW, __in HANDLE hProcess);

  static HRESULT GetLogonSid(__inout CNktStringW &cStrDestW, __in DWORD dwPid);
  static HRESULT GetLogonSid(__inout CNktStringW &cStrDestW, __in HANDLE hProcess);

  static HRESULT GetExecutableFileName(__inout CNktStringW &cStrDestW, __in DWORD dwPid,
                                       __in HMODULE hModule);
  static HRESULT GetExecutableFileName(__inout CNktStringW &cStrDestW, __in HANDLE hProcess,
                                       __in HMODULE hModule);

  static HRESULT Device2DosForm(__inout CNktStringW &cStrPathW);

  static HRESULT GetRealFilePath(__inout CNktStringW &cStrFileNameW);

  /**
  Enable privilege for the specified process. Returns TRUE on success.
  */
  static HRESULT SetProcessPrivilege(__in DWORD dwPid, __in_z LPCWSTR szPrivilegeW, __in BOOL bEnable);
  static HRESULT SetProcessPrivilege(__in HANDLE hProcess, __in_z LPCWSTR szPrivilegeW, __in BOOL bEnable);

  /**
  Test privilege for the specified process. Returns TRUE if enabled.
  */
  static HRESULT GetProcessPrivilege(__out LPBOOL lpbEnabled, __in DWORD dwPid, __in_z LPCWSTR szPrivilegeW);
  static HRESULT GetProcessPrivilege(__out LPBOOL lpbEnabled, __in HANDLE hProcess,
                                     __in_z LPCWSTR szPrivilegeW);

  static HRESULT SplitFileNameAndPath(__inout CNktStringW &cStrNameW, __inout CNktStringW &cStrPathW,
                                      __in_z LPCWSTR szFullNameW);
  static HRESULT SplitUserNameAndDomain(__inout CNktStringW &cStrNameW, __inout CNktStringW &cStrDomainW,
                                        __in_z LPCWSTR szFullNameW);

  static LPWSTR stristrW(__in_z LPCWSTR szStringW, __in_z LPCWSTR szToSearchW);

  static HRESULT wstr2ulong(__out ULONG *lpnValue, __in_z LPCWSTR szStringW);

  static LPSTR Wide2Ansi(__in_nz_opt LPCWSTR szSrcW, __in SIZE_T nSrcLen=NKT_SIZE_T_MAX);

  /**
  Process all pending messages.<br>
  keepQuitMsg: if TRUE do not process WM_QUIT messages. Keep this messages to be processed by the message
  queue. If<br> this flag is not set and the WM_QUIT message is processed out from the message queue, the
  message loop will enter<br>in an infinite loop.
  */
  static VOID DoEvents(__in BOOL bKeepQuitMsg=TRUE);

  /**
  Create a message loop
  */
  static VOID DoMessageLoop();

  static BOOL Win32ErrToString(__inout CNktStringW &cStrDestW, __in DWORD dwError);

  static HRESULT ParseFunctionName(__inout FUNCTION_PARSE_INFO &sInfo, __in_nz_opt LPCWSTR szFunctionNameW,
                                   __in SIZE_T nFunctionNameLen=NKT_SIZE_T_MAX);

  static SIZE_T GetProcessorsCount();

  static HRESULT SuspendAfterCreateProcessW(__out LPHANDLE lphReadyExecutionEvent,
                                            __out LPHANDLE lphContinueExecutionEvent,
                                            __in HANDLE hEngineProc, __in HANDLE hSuspendedProc,
                                            __in HANDLE hSuspendedMainThread);

  static HRESULT GetModules(__in HANDLE hProcess, __in SIZE_T nPlatformBits,
                            __inout TNktArrayList<HMODULE> &aModList);

  static HRESULT GetMappedFiles(__in DWORD dwProcessId, __in HANDLE hProcess, __in SIZE_T nPlatformBits,
                                __in HMODULE *lpModList, __in SIZE_T nModListCount,
                                __inout TNktArrayListWithDelete<MODULE_INFO*> &aMapList);

  static HRESULT GetModuleInfo(__in DWORD dwProcessId, __in HANDLE hProcess, __in HMODULE hModule,
                               __inout MODULE_INFO &sModInfo);

  static HRESULT GetProcAddressW(__out LPVOID *lplpProc, __in HINSTANCE hDll, __in_z LPCWSTR szProcNameW);

  static HRESULT GetSystem32Path(__inout CNktStringW &cStrPathW);

  static HRESULT GetModuleFullName(__inout CNktStringW &cStrPathW, __in HINSTANCE hDll);

  static HRESULT GetModulePath(__inout CNktStringW &cStrPathW, __in HINSTANCE hDll);

  static HRESULT FixFolderStr(__inout CNktStringW &cStrPathW);

  static HRESULT GetImageNtHeaders(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in DWORD dwPid,
                                   __in LPBYTE lpBaseAddr);
  static HRESULT GetImageNtHeaders(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr, __in HANDLE hProcess,
                                   __in LPBYTE lpBaseAddr);

  static HRESULT GetImageSections(__out_opt PROCESSSECTION **lplpSections, __in DWORD dwPid,
                                  __in LPBYTE lpBaseAddr);
  static HRESULT GetImageSections(__out_opt PROCESSSECTION **lplpSections, __in HANDLE hProcess,
                                  __in LPBYTE lpBaseAddr);

  static SIZE_T GetModuleUsageCount(__in HINSTANCE hDll);
  static SIZE_T GetModuleSize(__in HINSTANCE hDll);

  static LPVOID GetApiAddress(__in HINSTANCE hDll, __in_z LPCSTR szAddressA, __in BOOL bIgnoreShims);

  static HRESULT GetFileNameFromHandle(__inout CNktStringW &cStrFileNameW, __in HANDLE hFile,
                                       __in DWORD dwPid);

  static DWORD GetThreadId(__in HANDLE hThread);

  static MANDATORY_LEVEL GetProcessIntegrityLevel(__in DWORD dwPid);
};

//-----------------------------------------------------------

