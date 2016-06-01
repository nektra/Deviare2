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

#include "ParamsEnum.h"

//-----------------------------------------------------------

class CNktDvFunctionParamsCache : public CNktFastMutex
{
public:
  class FUNCTION_PARAMS : public CNktDvObject
  {
  public:
    FUNCTION_PARAMS() : CNktDvObject()
      {
      return;
      };

  public:
    TNktComPtr<CNktDvParamsEnumerator> cParameters;
    TNktComPtr<CNktDvParam> cResult;
  };

  CNktDvFunctionParamsCache();
  virtual ~CNktDvFunctionParamsCache();

  virtual HRESULT Initialize(__in CNktDvDbObjectNoRef *lpDbObjFunc, __in CNktDvProcessMemory *lpProcMem,
                             __in SIZE_T nSizeOfSize);

  static SIZE_T GetStackUsage(__in CNktDvDbObjectNoRef *lpDbObjFunc);
#if defined _M_IX86
  static HRESULT PreCall_AssignAddresses(__inout FUNCTION_PARAMS *lpItem,
                                         __in CNktDvDbObjectNoRef *lpDbObjFunc,
                                         __in LPNKT_DV_ASMREGISTERS32 lpRegisters);
#elif defined _M_X64
  static HRESULT PreCall_AssignAddresses(__inout FUNCTION_PARAMS *lpItem,
                                         __in CNktDvDbObjectNoRef *lpDbObjFunc,
                                         __in LPNKT_DV_ASMREGISTERS64 lpRegisters);
#endif
  static VOID PreCall_ReassignAddressesAndSaveOriginalStackValues(__inout FUNCTION_PARAMS *lpItem);
#if defined _M_IX86
  static VOID PostCall_RestoreOriginalStackValues(__inout FUNCTION_PARAMS *lpItem,
                                                  __in LPNKT_DV_ASMREGISTERS32 lpRegisters);
#elif defined _M_X64
  static VOID PostCall_RestoreOriginalStackValues(__inout FUNCTION_PARAMS *lpItem,
                                                  __in LPNKT_DV_ASMREGISTERS64 lpRegisters);
#endif

  virtual FUNCTION_PARAMS* GetFuncParam();

private:
  TNktComPtr<FUNCTION_PARAMS> cBaseFuncParam;
};

//-----------------------------------------------------------

