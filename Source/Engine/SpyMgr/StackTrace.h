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

#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\ExportedFunction.h"
#include "..\..\Common\Module.h"
#include "..\..\Common\ModulesEnum.h"
#include "..\..\Common\ParamsEnum.h"

//-----------------------------------------------------------

class CNktDvStackTrace : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  CNktDvStackTrace();
  virtual ~CNktDvStackTrace();

  virtual HRESULT GetNearestFunction(__deref_out CNktDvExportedFunction **lplpFunc, __in SIZE_T nDepth);

  virtual HRESULT GetModule(__deref_out CNktDvModule **lplpMod, __in SIZE_T nDepth);

  virtual SIZE_T GetOffset(__in SIZE_T nDepth) const;

  virtual LPVOID GetAddress(__in SIZE_T nDepth) const;

  virtual LPWSTR GetNearestSymbol(__in SIZE_T nDepth) const; //returns NULL on error

  virtual HRESULT GetParams(__deref_out CNktDvParamsEnumerator **lplpParamEnum,
                            __deref_out CNktDvParam **lplpResult, __in SIZE_T nDepth);

private:
  friend class CNktDvHookCallInfo;
  friend class CNktHookCallInfoPluginImpl;

  virtual HRESULT InitModEnum();
  virtual HRESULT InitMod(__in SIZE_T nDepth);
  virtual HRESULT InitExpFunc(__in SIZE_T nDepth);

  SIZE_T nStackTrace[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  TNktComPtr<CNktDvProcess> cProc;
  TNktComPtr<CNktDvModulesEnumerator> cModEnum;
  TNktComPtr<CNktDvModule> cMod[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  HRESULT volatile hModInit[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  TNktComPtr<CNktDvExportedFunction> cExpFunc[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  HRESULT volatile hExpFuncInit[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  LPWSTR volatile szSymbolW[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
};

//-----------------------------------------------------------

