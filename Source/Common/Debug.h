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

#include <windows.h>
#include <time.h>
#include <WTypes.h>
#include <crtdbg.h>

//#define NKT_RELEASE_DEBUG_MACROS
//#define NKT_FUNCTION_TIMING_ENABLED
#define NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY

//-----------------------------------------------------------

//NOTE: Wide functions were removed because, internally, it transforms wide char to ansi
//      and some times crashes.

namespace Nektra
{

typedef enum {
  dlFunctionTiming=0x00000001,
  //dlMemory=0x00000002, //deprecated
  dlMemoryTracker=0x00000004,
  dlTools=0x00000008,
  dlTransport=0x00000010,
  dlEngine=0x00000020,
  dlAgent=0x00000040,
  dlHookEngine=0x00000080,
  dlHookEnginePreCall=0x00000100,
  dlHookEnginePostCall=0x00000200,
  dlError=0x00000400,
  dlWarning=0x00000800,
  dlMemoryTrackerLevel2=0x00001000,
  dlInformation=0x00002000,
  dlObjectDump=0x20000000,
  dlWriteOutputToFile=0x40000000,
  dlEnableOutputOnXP=0x80000000
} eDebugLevel;

#ifdef _DEBUG
  #define DEFAULT_DBGLEVEL_MASK Nektra::dlEngine|Nektra::dlAgent|          \
                                Nektra::dlHookEngine|Nektra::dlMemoryTracker
#else //_DEBUG
  #define DEFAULT_DBGLEVEL_MASK 0
#endif //_DEBUG

/**
Set new debug level
*/
VOID SetDebugLevel(__in int nNewDebugLevelMask);
int GetDebugLevel();

BOOL CheckDebugLevel(__in int nMask);

/**
Print formatted message.
*/
VOID DebugPrintA(__in_z LPCSTR szFormatA, ...);
VOID DebugPrintV_A(__in_z LPCSTR szFormatA, __in va_list ap);

/**
Print formatted message line.
*/
VOID DebugPrintLnA(__in_z LPCSTR szFormatA, ...);
VOID DebugPrintLnV_A(__in_z LPCSTR szFormatA, __in va_list ap);

/**
Break.
*/
__inline VOID DebugBreak()
{
  __debugbreak();
  return;
}

} //namespace

//-----------------------------------------------------------

#if (defined(_DEBUG) || defined(NKT_RELEASE_DEBUG_MACROS))
  #define NKT_ASSERT(expr)          if (!(expr)) Nektra::DebugBreak()
#else //_DEBUG || NKT_RELEASE_DEBUG_MACROS
  #define NKT_ASSERT(expr)          ((void)0)
#endif //_DEBUG || NKT_RELEASE_DEBUG_MACROS

#if (defined(_DEBUG) || defined(NKT_RELEASE_DEBUG_MACROS) || defined(NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY))
  #define NKT_DEBUGPRINTA(mask,expr)       { if (Nektra::CheckDebugLevel(mask)) {   \
                                               Nektra::DebugPrintA expr;            \
                                           } }
  #define NKT_DEBUGPRINTLNA(mask,expr)     { if (Nektra::CheckDebugLevel(mask)) {   \
                                               Nektra::DebugPrintLnA expr;          \
                                           } }
#else //_DEBUG || NKT_RELEASE_DEBUG_MACROS || NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY
  #define NKT_DEBUGPRINTA(mask,expr)       ((void)0)
  #define NKT_DEBUGPRINTLNA(mask,expr)     ((void)0)
#endif //_DEBUG || NKT_RELEASE_DEBUG_MACROS || NKT_ENABLE_DEBUG_OUTPUT_BY_REGISTRY

//-----------------------------------------------------------

#ifdef NKT_FUNCTION_TIMING_ENABLED
  #define NKT_FUNCTION_TIMING(fnc) CNktFunctionTimingDebug _cFuncTiming(fnc);

  class CNktFunctionTimingDebug
  {
  public:
    CNktFunctionTimingDebug(__in_z LPCSTR szNameA);
    ~CNktFunctionTimingDebug();

  private:
    LPCSTR szNameA;
    LARGE_INTEGER liStart, liEnd, liFreq;
    DWORD dwEnterTime;
  };

#else //NKT_FUNCTION_TIMING_ENABLED
  #define NKT_FUNCTION_TIMING(fnc)
#endif //NKT_FUNCTION_TIMING_ENABLED

//-----------------------------------------------------------

