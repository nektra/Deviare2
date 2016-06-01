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
class CNktDvPdbSymbolManager;

//-----------------------------------------------------------

class CNktDvPdbFunctionSymbol : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  CNktDvPdbFunctionSymbol();
  virtual ~CNktDvPdbFunctionSymbol();

  static HRESULT Create(__in_z LPCWSTR szDllNameW, __in_z LPCWSTR szFunctionNameW,
                        __in_z LPCWSTR szSymbolServerPathW, __in_z LPCWSTR szLocalCachePathW,
                        __deref_out CNktDvPdbFunctionSymbol **lplpSymbol);

  virtual SIZE_T GetAddressOffset() const
    {
    return nOffset;
    };

  virtual SIZE_T GetPlatformBits() const
    {
    return nPlatformBits;
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

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  SIZE_T nOffset, nPlatformBits;
  CNktStringW cStrNameW, cStrModuleNameW, cStrCompleteNameW;
  TNktComPtr<CNktDvPdbSymbolManager> cSymMgr;
};

//-----------------------------------------------------------

