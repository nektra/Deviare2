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

#ifndef _DVENGINETEST_H
#define _DVENGINETEST_H

//-----------------------------------------------------------

#include <windows.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <statreg.h>
#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include "StringLiteW.h"
#include "WaitableObjects.h"
#include "ArrayList.h"

//-----------------------------------------------------------

#define NKT_DV_ARRAYLEN(x)        (sizeof(x)/sizeof(x[0]))

//-----------------------------------------------------------

typedef enum {
  ttTestHookExec=1, ttTestHookAttach, ttTestWndProcHook
} eTestType;

typedef struct {
  eTestType nTestType;
  struct {
    CNktStringW cStrExeFileNameW;
  } sExec;
  union {
    struct {
      SIZE_T nCount;
      DWORD dwPids[8];
    } sAttach;
    struct {
      SIZE_T nCount;
      HWND hWnds[8];
    } sWndProc;
  };
  CNktStringW cStrXmlFileNameW;
  CNktStringW cStrLogFileNameW;
  TNktArrayListWithFree<LPWSTR> aCustomHandlers;
  BOOL bBatchMode;
  BOOL bDontDisplayStack;
  BOOL bAsyncCallbacks;
  BOOL bCustomHandlerDebug;
} CMDLINE_PARAMS;

//-----------------------------------------------------------

extern CMDLINE_PARAMS sCmdLineParams;

BOOL IsExitKeyPressed();

VOID PressAnyKeyToContinue();

VOID LogPrintNoTick(__in LPCWSTR szFormatW, ...);
VOID LogPrint(__in LPCWSTR szFormatW, ...);
CNktFastMutex* LogGetMutex();

//-----------------------------------------------------------

#endif //_DVENGINETEST_H

