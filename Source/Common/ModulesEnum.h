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

#include "EngBaseObj.h"
#include "Module.h"
#include "Process.h"
#include "EnumeratorBase.h"
#include "Tools.h"
#include "ArrayList.h"

//-----------------------------------------------------------

class CNktDvModulesEnumerator : public TNktDvEnumerator<CNktDvModule,384>
{
public:
  CNktDvModulesEnumerator() : TNktDvEnumerator<CNktDvModule,384>()
    {
    dwPid = 0;
    nPlatformBits = 0;
    NktInterlockedExchange(&(sDirtyModules.nDoCheck), 0);
    return;
    };

  virtual HRESULT GetItemByName(__deref_out CNktDvModule **lplpMod, __in_z LPCWSTR szDllNameW,
                                __in SIZE_T nDllNameLen=NKT_SIZE_T_MAX);
  virtual HRESULT GetItemByAddress(__deref_out CNktDvModule **lplpMod, __in_opt LPVOID lpAddr,
                                   __in CNktDvModule::eSearchMode nSearchMode);

  virtual HRESULT FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                             __in_z LPCWSTR szDllFuncW);
  virtual HRESULT FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                __in_opt LPVOID lpAddr,
                                                __in CNktDvModule::eSearchMode nSearchMode);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  friend class CInternalModEnumCache;

  virtual HRESULT Initialize(__in DWORD dwProcessId, __in SIZE_T nProcPlatformBits);
  virtual HRESULT AddItem(__in CNktDvTools::MODULE_INFO *lpModInfo);

  virtual DWORD GetPid()
    {
    return dwPid;
    };

  virtual SIZE_T GetPlatformBits()
    {
    return nPlatformBits;
    };

  virtual CNktDvModulesEnumerator* Clone();

  virtual HRESULT MarkModuleAsDirty(__in HINSTANCE hDll);
  virtual HRESULT CheckDirtyModules();

  virtual HRESULT RebuildSortedLists();

private:
  DWORD dwPid;
  SIZE_T nPlatformBits;
  TNktAutoFreePtr<SIZE_T> cNameIndexList;
  TNktAutoFreePtr<SIZE_T> cAddrIndexList;
  struct {
    LONG volatile nDoCheck;
    TNktArrayList<HINSTANCE> aList;
  } sDirtyModules;
};

//-----------------------------------------------------------

