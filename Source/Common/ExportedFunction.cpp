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

#include "ExportedFunction.h"

//-----------------------------------------------------------

CNktDvExportedFunction::CNktDvExportedFunction() : TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE)
{
  nOrdinal = 0;
  bIsForwarded = FALSE;
  nAddrBase = nAddrOffset = 0;
  lpDbObj = NULL;
  NktInterlockedExchange(&nDbObjInitialized, 0);
  return;
}

CNktDvExportedFunction::~CNktDvExportedFunction()
{
  return;
}

CNktDvDbObjectNoRef::eCallingConvention CNktDvExportedFunction::GetCallingConvention() const
{
  CNktDvDbObjectNoRef *_lpDbObj;

  _lpDbObj = GetFunctionEntry();
  return (_lpDbObj != NULL) ? (lpDbObj->GetFuncCallingConvention()) : CNktDvDbObjectNoRef::ccNA;
}

CNktDvDbObjectNoRef* CNktDvExportedFunction::GetFunctionEntry() const
{
  if (nDbObjInitialized == 0)
  {
    CNktDvExportedFunction *lpThis = const_cast<CNktDvExportedFunction*>(this);
    {
      CNktAutoFastMutex cLock(&(lpThis->GetLockObject()));

      if (nDbObjInitialized == 0)
      {
        //search the DbObject in the database the first time it is required
        lpThis->lpDbObj = (cDvDB != NULL) ? (cDvDB->FindFunctionByName(GetName())) : NULL;
        NktInterlockedExchange(&(lpThis->nDbObjInitialized), 1);
      }
    }
  }
  return lpDbObj;
}

HRESULT CNktDvExportedFunction::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvExportedFunction [%s @ %IXh]", GetName(), GetAddr()) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}
