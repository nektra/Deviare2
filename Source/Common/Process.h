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

class CNktDvProcess;
class CNktDvModulesEnumerator;
class CNktDvProcessesList;
class CNktDvInternalProcessesList;
class CNktDvProcessMemory;
#include "EngBaseObj.h"
#include "ModulesEnum.h"
#include <tlhelp32.h>

//-----------------------------------------------------------

class CNktDvProcessHandlesMgr;

class CNktDvProcess : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  CNktDvProcess();
  virtual ~CNktDvProcess();

  static HRESULT CNktDvProcess::CreateFromPID(__deref_out CNktDvProcess **lplpProc, __in DWORD dwPid);

  virtual DWORD GetProcessId() const
    {
    return dwProcessID;
    };

  virtual DWORD GetParentProcessId() const
    {
    return dwParentProcessID;
    };

  virtual SIZE_T GetPlatformBits() const
    {
    return nPlatformBits;
    };

  virtual SIZE_T GetThreadsCount() const
    {
    return nThreadsCount;
    };

  virtual LONG GetPriorityBaseClass() const
    {
    return nPriClassBase;
    };

  virtual LPWSTR GetPath() const
    {
    return const_cast<CNktStringW&>(cStrPathW);
    };

  virtual LPWSTR GetFileName() const
    {
    return GetPath() + nNameOffsetInPathStr;
    };

  virtual LPWSTR GetUserName() const
    {
    return (cStrUserNameW.IsEmpty() == FALSE) ? const_cast<CNktStringW&>(cStrUserNameW) : L"SYSTEM";
    };

  virtual LPWSTR GetDomainName() const
    {
    return const_cast<CNktStringW&>(cStrRefDomainW);
    };

  virtual LPWSTR GetLogonSid() const
    {
    return const_cast<CNktStringW&>(cStrSidW);
    };

  virtual HRESULT IsActive(__in DWORD dwWaitTimeMs=1000);

  virtual HRESULT Terminate(__in DWORD dwExitCode=0);

  virtual HRESULT GetHandle(__out HANDLE *lphProc, __in DWORD dwDesiredAccess);

  virtual HRESULT GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem);

  static HRESULT GetModulesEnumerator(__deref_out CNktDvModulesEnumerator **lplpEnum, __in DWORD dwPid);
  virtual HRESULT GetModulesEnumeratorV(__deref_out CNktDvModulesEnumerator **lplpEnum);
  static VOID MarkModulesEnumeratorAsDirty(__in DWORD dwPid, __in HINSTANCE hDll);
  virtual VOID MarkModulesEnumeratorAsDirtyV(__in HINSTANCE hDll);

  virtual HRESULT FindModuleByName(__deref_out CNktDvModule **lplpMod, __in_z LPCWSTR szDllNameW);
  virtual HRESULT FindModuleByAddress(__deref_out CNktDvModule **lplpMod, __in_opt LPVOID lpAddr,
                                      __in CNktDvModule::eSearchMode nSearchMode);

  virtual HRESULT FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                             __in_z LPCWSTR szDllFuncW);
  virtual HRESULT FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                __in_opt LPVOID lpAddr,
                                                __in CNktDvModule::eSearchMode nSearchMode);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

  //--------

  static HRESULT Create(__deref_out CNktDvProcess **lplpProc, __in_z LPCWSTR szImagePathW,
                        __in BOOL bSuspended=FALSE, __out_opt LPHANDLE lphContinueEvent=NULL);
  static HRESULT CreateWithLogon(__deref_out CNktDvProcess **lplpProc, __in_z LPCWSTR szImagePathW,
                                 __in_z LPCWSTR szUserNameW, __in_z LPCWSTR szPasswordW,
                                 __in BOOL bSuspended=FALSE, __out_opt LPHANDLE lphContinueEvent=NULL);
  static HRESULT CreateWithToken(__deref_out CNktDvProcess **lplpProc, __in_z LPCWSTR szImagePathW,
                                 __in_z HANDLE hToken, __in BOOL bSuspended=FALSE,
                                 __out_opt LPHANDLE lphContinueEvent=NULL);
  static HRESULT Terminate(__in DWORD dwPid, __in DWORD dwExitCode);
  static HRESULT FindProcessId(__in_z LPCWSTR szName, __out LPDWORD lpdwPid);

  static VOID FlushCache();

protected:
  friend class CNktDvInternalProcessesList;
  friend class CNktDvEngObjMgr;

  virtual HRESULT InitInfo(__in HANDLE hProcess, __in_z_opt LPCWSTR szExeNameW);

protected:
  DWORD dwProcessID;
  DWORD dwParentProcessID;
  SIZE_T nPlatformBits;
  SIZE_T nThreadsCount;
  SIZE_T nCpuUsage;
  LONG nPriClassBase;
  CNktStringW cStrPathW;
  SIZE_T nNameOffsetInPathStr;
  CNktStringW cStrUserNameW;
  CNktStringW cStrRefDomainW;
  CNktStringW cStrSidW;
  DWORD dwInternalFlags;
};

//-----------------------------------------------------------

