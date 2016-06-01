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

#include "HookEngineInternals.h"
#include "..\..\Common\FunctionParamsCache.h"
#include "..\..\Common\DynamicAPIs.h"
class CHookEntry;

//-----------------------------------------------------------

class CHookEngCallDataEntry : public CNktDvHookCallDataBase
{
public:
  class CTiming
  {
  public:
    __inline VOID Initialize(__in LARGE_INTEGER &liTimerFreq)
      {
      FILETIME ftExitTime, ftKernelTime, ftUserTime;
      ULARGE_INTEGER liTime, liTemp;

      if (liTimerFreq.QuadPart != 0)
      {
        ::QueryPerformanceCounter((LARGE_INTEGER*)&liTime);
        if (liTime.QuadPart < 0x20C49BA5E353F7ui64)
          nTimeMs = double(liTime.QuadPart * 1000ui64) / double(liTimerFreq.QuadPart);
        else
          nTimeMs = double(liTime.QuadPart / liTimerFreq.QuadPart) * 1000.0;
        //nTimeMs = double(liTime.QuadPart) / (double(liTimerFreq.QuadPart) / 1000.0);
      }
      else
      {
        nTimeMs = (double)::GetTickCount();
      }
      //--------
      ::GetThreadTimes(::GetCurrentThread(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
      liTemp.LowPart = ftKernelTime.dwLowDateTime;
      liTemp.HighPart = ftKernelTime.dwHighDateTime;
      nKernelMs = double(liTemp.QuadPart) * 0.0001;
      liTemp.LowPart = ftUserTime.dwLowDateTime;
      liTemp.HighPart = ftUserTime.dwHighDateTime;
      nUserMs = double(liTemp.QuadPart) * 0.0001;
      //--------
      nktDvDynApis_QueryThreadCycleTime(::GetCurrentThread(), &nCpuClockCycles);
      return;
      };

    CTiming& CTiming::operator=(const CTiming &rhs)
      {
      if (this != &rhs)
      {
        ftCreationTime = rhs.ftCreationTime;
        nTimeMs = rhs.nTimeMs;
        nKernelMs = rhs.nKernelMs;
        nUserMs = rhs.nUserMs;
        nCpuClockCycles = rhs.nCpuClockCycles;
      }
      return *this;
      };

    static __inline double TimeDiff(__in double nEndMs, __in double nStartMs)
      {
      return (nEndMs > nStartMs) ? (nEndMs-nStartMs) : 0.0;
      };

  public:
    FILETIME ftCreationTime;
    double nTimeMs, nKernelMs, nUserMs;
    ULONG64 nCpuClockCycles;
  };

    /*
  static __inline double GetTimeDiff(__in FUNCTIONTIMING &uFT_End, __in FUNCTIONTIMING &uFT_Start,
                                     __in LARGE_INTEGER &liTimerFreq)
    {
    if (liTimerFreq.QuadPart != 0)
    {
      if (uFT_End.liTime.QuadPart >= uFT_Start.liTime.QuadPart)
        return double(uFT_End.liTime.QuadPart - uFT_Start.liTime.QuadPart) /
               (double(liTimerFreq.QuadPart) / 1000.0);
      return double((~(uFT_Start.liTime.QuadPart)) + uFT_End.liTime.QuadPart + 1ui64) /
             (double(liTimerFreq.QuadPart) / 1000.0);
    }
    if (uFT_End.dwTime >= uFT_Start.dwTime)
      return (double)(uFT_End.dwTime - uFT_Start.dwTime);
    return (double)((~uFT_Start.dwTime) + uFT_End.dwTime + 1UL);
    };
    */

public:
  LONG volatile nId;
  LONG nCookie;
  SIZE_T nReturnAddr, nAfterCallAddr;
  SIZE_T nStackTrace[NKT_DV_TMSG_ONHOOKCALLED_StackTraceDepth];
  CHookEntry *lpHookEntry;
  LONG nCallDataItemId;
#if defined _M_IX86
  NKT_DV_CURRENT_CALL_STATE32 sCallState;
#elif defined _M_X64
  NKT_DV_CURRENT_CALL_STATE64 sCallState;
#endif //_M_X64
  CTiming cFunctionTiming[2];
  DWORD dwChainDepth;
  double nCurrentTimeMs, nChildsElapsedTimeMs, nChildOverHeadMs;
  TNktComPtr<CNktDvFunctionParamsCache::FUNCTION_PARAMS> cFuncParams;
  BYTE aIntercallCustomData[NKT_DV_HOOKENG_IntercallCustomDataSize];
};

//-----------------------------------------------------------

