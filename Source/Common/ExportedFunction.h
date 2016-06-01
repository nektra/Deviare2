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

class CNktDvExportedFunction;
#include "EngBaseObj.h"
#include "ExportedFunctionsEnum.h"
#include "Database.h"

//-----------------------------------------------------------

class CNktDvExportedFunction : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  CNktDvExportedFunction();
  virtual ~CNktDvExportedFunction();

  virtual SIZE_T GetOrdinal() const
    {
    return nOrdinal;
    };

  virtual LPWSTR GetName() const
    {
    return const_cast<CNktStringW&>(cStrNameW);
    };

  virtual LPWSTR GetModuleName() const
    {
    return const_cast<CNktStringW&>(cStrModuleNameW);
    };

  virtual LPWSTR GetCompleteName() const
    {
    return const_cast<CNktStringW&>(cStrCompleteNameW);
    };

  virtual BOOL IsForwarded() const
    {
    return bIsForwarded;
    };

  virtual LPWSTR GetForwardedApiName() const
    {
    return const_cast<CNktStringW&>(cStrForwardedApiNameW);
    };

  virtual LPWSTR GetForwardedModuleName() const
    {
    return const_cast<CNktStringW&>(cStrForwardedModuleNameW);
    };

  virtual LPWSTR GetForwardedCompleteName() const
    {
    return const_cast<CNktStringW&>(cStrForwardedCompleteNameW);
    };

  virtual SIZE_T GetAddrBase() const
    {
    return nAddrBase;
    };

  virtual SIZE_T GetAddrOffset() const
    {
    return nAddrOffset;
    };

  virtual SIZE_T GetAddr() const
    {
    return GetAddrBase() + GetAddrOffset();
    };

  virtual BOOL HasDeclaration() const
    {
    return (GetFunctionEntry() != NULL) ? TRUE : FALSE;
    };

  virtual LPWSTR GetDeclaration() const
    {
    return (GetFunctionEntry() != NULL) ? (GetFunctionEntry()->GetDeclaration()) : L"";
    };

  virtual CNktDvDbObjectNoRef::eCallingConvention GetCallingConvention() const;

  virtual CNktDvDbObjectNoRef* GetFunctionEntry() const;

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  friend class CNktDvExportedFunctionsEnumerator;

  TNktComPtr<CNktDvEngDatabase> cDvDB;
  SIZE_T nOrdinal;
  CNktStringW cStrNameW, cStrModuleNameW, cStrCompleteNameW;
  BOOL bIsForwarded, bForwardResolved;
  CNktStringW cStrForwardedApiNameW, cStrForwardedModuleNameW, cStrForwardedCompleteNameW;
  SIZE_T nAddrBase, nAddrOffset;
  CNktDvDbObjectNoRef *lpDbObj;
  LONG volatile nDbObjInitialized;
};

//-----------------------------------------------------------

