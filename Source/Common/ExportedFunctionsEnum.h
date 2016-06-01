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

class CNktDvExportedFunctionsEnumerator;
class CNktDvModulesEnumerator;
#include "EngBaseObj.h"
#include "Module.h"
#include "ExportedFunction.h"
#include "EnumeratorBase.h"
#include "ArrayList.h"
#include "Database.h"

//-----------------------------------------------------------

class CNktDvExportedFunctionsEnumerator : public TNktDvEnumerator<CNktDvExportedFunction,4096>
{
public:
  CNktDvExportedFunctionsEnumerator() : TNktDvEnumerator<CNktDvExportedFunction,4096>()
    {
    return;
    };

  virtual HRESULT GetItemByOrdinal(__deref_out CNktDvExportedFunction** lplpFunc, __in SIZE_T nOrdinal);
  virtual HRESULT GetItemByName(__deref_out CNktDvExportedFunction** lplpFunc, __in_nz_opt LPCWSTR szNameW,
                                __in SIZE_T nNameLen=NKT_SIZE_T_MAX);
  virtual HRESULT GetItemByAddress(__deref_out CNktDvExportedFunction **lplpFunc, __in_opt LPVOID lpAddr,
                                   __in BOOL bSearchNearest);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

protected:
  friend class CNktDvModule;

  virtual HRESULT Initialize(__in CNktDvModule *lpModule, __in CNktDvModulesEnumerator *lpModEnum);

  virtual CNktDvExportedFunctionsEnumerator* Clone();

private:
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  TNktAutoFreePtr<SIZE_T> cNameIndexList;
  TNktAutoFreePtr<SIZE_T> cAddrIndexList;
};

//-----------------------------------------------------------

