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

class CNktDvModule;
#include "EngBaseObj.h"
#include "ExportedFunctionsEnum.h"
#include "ExportedFunction.h"
#include "Tools.h"
#include "FnvHash.h"

//-----------------------------------------------------------

class CNktDvModule : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  typedef enum {
    smExactMatch=0, smFindContaining, smGetNearest
  } eSearchMode;

  CNktDvModule();
  virtual ~CNktDvModule();

  virtual DWORD GetProcessId() const
    {
    return dwProcessId;
    };

  virtual SIZE_T GetPlatformBits() const
    {
    return nPlatformBits;
    };

  virtual LPBYTE GetBaseAddress() const
    {
    return lpBaseAddress;
    };

  virtual SIZE_T GetSize() const
    {
    return nBaseSize;
    };

  virtual LPWSTR GetName() const
    {
    return GetFileName();
    };

  virtual LPWSTR GetPath() const
    {
    return const_cast<CNktStringW&>(cStrPathW);
    };

  virtual LPWSTR GetFileName() const
    {
    return GetPath() + nNameOffsetInPathStr;
    };

  virtual HMODULE GetHandle() const
    {
    return hModule;
    };

  virtual HRESULT GetExportedFunctionsEnumerator(__deref_out CNktDvExportedFunctionsEnumerator **lplpEnum);

  virtual HRESULT FindExportedFunctionByOrdinal(__deref_out CNktDvExportedFunction **lplpFunc,
                                                __in SIZE_T nOrdinal);
  virtual HRESULT FindExportedFunctionByName(__deref_out CNktDvExportedFunction **lplpFunc,
                                             __in_nz_opt LPCWSTR szFunctionNameW,
                                             __in SIZE_T nFunctionNameLen=NKT_SIZE_T_MAX);
  virtual HRESULT FindExportedFunctionByAddress(__deref_out CNktDvExportedFunction **lplpFunc,
                                                __in_opt LPVOID lpAddr, __in BOOL bSearchNearest=TRUE);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

  static VOID FlushCache();

private:
  friend class CNktDvModulesEnumerator;
  friend class CNktDvExportedFunctionsEnumerator;

private:
  DWORD dwProcessId;
  SIZE_T nPlatformBits;
  Fnv64_t nKey;
  LPBYTE lpBaseAddress;
  SIZE_T nBaseSize;
  HMODULE hModule;
  CNktStringW cStrPathW;
  SIZE_T nNameOffsetInPathStr;
  TNktComPtr<CNktDvExportedFunctionsEnumerator> cExpFuncEnum;
};

//-----------------------------------------------------------

