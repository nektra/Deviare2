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

#include "Agent.h"
#include "HookEngineInternals.h"

//-----------------------------------------------------------

static SIZE_T CheckHookedEntry(__in SIZE_T nInstrPointer, __in TNktLnkLst<CNktDvHookCallDataBase> *lpInUseStack);

//-----------------------------------------------------------

#if defined _M_IX86
VOID CNktDvHookEngine::GetStackTrace(__out SIZE_T *lpnOutput, __in SIZE_T nCount, __in SIZE_T nSkipCount,
                                     __in ULONG Esp, __in ULONG Ebp, __in SIZE_T nStackLimitLow,
                                     __in SIZE_T nStackLimitHigh, __inout CNktDvTlsData *lpTlsData)
{
  TNktLnkLst<CNktDvHookCallDataBase> *lpInUseStack;
  LPDWORD lpdwFrame;
  SIZE_T k;
  BYTE temp8;

  lpInUseStack = (lpTlsData != NULL) ? &(lpTlsData->sHookCallData.cInUseStack) : NULL;
  if (nStackLimitLow == 0 && nStackLimitHigh == 0)
    GetStackLimits(NULL, nStackLimitLow, nStackLimitHigh);
  lpdwFrame = (LPDWORD)Ebp;
  k = 0;
  __try
  {
    //first item is pointer by esp register
    if (nCount > 0)
    {
      if (nSkipCount == 0)
      {
        lpnOutput[k] = CheckHookedEntry((SIZE_T)*((LPDWORD)Esp), lpInUseStack);
        //NKT_ASSERT((lpnOutput[k] & 0xFFFF) != 0x010E); //<<== CAN LEAD INTO FALSE POSITIVES
        //try to read byte at lpnOutput[k], if that "eip" is invalid, an exception will raise and loop will end
        temp8 = *((LPBYTE)lpnOutput[k]);
        k++;
        nCount--;
      }
      else
      {
        nSkipCount--;
      }
    }
    //get the rest
    while (nCount > 0)
    {
      if (((SIZE_T)lpdwFrame & 0x03) != 0)
        break; //unaligned
      if ((SIZE_T)lpdwFrame < nStackLimitLow || (SIZE_T)lpdwFrame >= nStackLimitHigh)
        break;
      if (nSkipCount > 0)
      {
        nSkipCount--;
      }
      else
      {
        lpnOutput[k] = CheckHookedEntry((SIZE_T)lpdwFrame[1], lpInUseStack);
        //NKT_ASSERT((lpnOutput[k] & 0xFFFF) != 0x010E); //<<== CAN LEAD INTO FALSE POSITIVES
        //try to read byte at lpnOutput[k], if that "eip" is invalid, an exception will raise and loop will end
        temp8 = *((LPBYTE)lpnOutput[k]);
        k++;
        nCount--;
      }
      lpdwFrame = (LPDWORD)(lpdwFrame[0]);
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  { }
  while (nCount > 0)
  {
    lpnOutput[k++] = 0;
    nCount--;
  }
  return;
}

#elif defined _M_X64

VOID CNktDvHookEngine::GetStackTrace(__out SIZE_T *lpnOutput, __in SIZE_T nCount, __in SIZE_T nSkipCount,
                                     __in ULONGLONG Rip, __in ULONGLONG Rsp, __in SIZE_T nStackLimitLow,
                                     __in SIZE_T nStackLimitHigh, __inout CNktDvTlsData *lpTlsData)
{
  TNktLnkLst<CNktDvHookCallDataBase> *lpInUseStack;
  CONTEXT sCtx;
  NKT_DV_UNWIND_HISTORY_TABLE sUnwindHistTbl;
  PRUNTIME_FUNCTION lpFunc;
  PVOID lpHandlerData;
  ULONG64 nEstablisherFrame, nImageBase;
  BYTE temp8;
  SIZE_T nRetrieved;

  lpInUseStack = (lpTlsData != NULL) ? &(lpTlsData->sHookCallData.cInUseStack) : NULL;
  if (nStackLimitLow == 0 && nStackLimitHigh == 0)
    CNktDvHookEngine::GetStackLimits(NULL, nStackLimitLow, nStackLimitHigh);
  nRetrieved = 0;
  if (SUCCEEDED(nktDvDynApis_RtlCaptureContext(&sCtx)))
  {
    nktMemSet(&sUnwindHistTbl, 0, sizeof(sUnwindHistTbl));
    sUnwindHistTbl.Search = UNWIND_HISTORY_TABLE_GLOBAL;
    sUnwindHistTbl.LowAddress = (DWORD64)-1;
    sCtx.Rip = Rip;
    sCtx.Rsp = Rsp;
    __try
    {
      while (nRetrieved < nCount)
      {
        if ((sCtx.Rsp & 0x07) != 0)
          break; //unaligned
        if ((SIZE_T)(sCtx.Rsp) < nStackLimitLow || (SIZE_T)(sCtx.Rsp) >= nStackLimitHigh)
          break;
        if (FAILED(nktDvDynApis_RtlLookupFunctionEntry(&lpFunc, sCtx.Rip, &nImageBase, NULL/*&sUnwindHistTbl*/)))
          break;
        if (lpFunc == NULL)
        {
          //break;
          sCtx.Rip = *((ULONGLONG*)(sCtx.Rsp));
          sCtx.Rsp += 8;
        }
        else
        {
          if (FAILED(nktDvDynApis_RtlVirtualUnwind(NULL, NKT_DV_UNW_FLAG_NHANDLER, nImageBase, sCtx.Rip, lpFunc,
                                                   &sCtx, &lpHandlerData, &nEstablisherFrame, NULL)))
            break;
          if ((SIZE_T)nEstablisherFrame < nStackLimitLow || (SIZE_T)nEstablisherFrame >= nStackLimitHigh)
            break;
        }
        if (sCtx.Rip == 0)
          break;
        if (nSkipCount > 0)
        {
          nSkipCount--;
        }
        else
        {
          sCtx.Rip = lpnOutput[nRetrieved] = CheckHookedEntry((SIZE_T)(sCtx.Rip), lpInUseStack);
          temp8 = *((LPBYTE)lpnOutput[nRetrieved]);
          nRetrieved++;
        }
      }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    { }
  }
  nktMemSet(lpnOutput+nRetrieved, 0, (nCount-nRetrieved)*sizeof(SIZE_T));
  return;
}

#else
  #error Unsupported platform
#endif

VOID CNktDvHookEngine::GetStackLimits(__in HANDLE hThread, __out SIZE_T &nLow, __out SIZE_T &nHigh)
{
  NKT_DV_THREAD_BASIC_INFORMATION sBasicInfo;
  NT_TIB *lpTIB;
  SIZE_T k;

  nLow = 0;
  nHigh = NKT_SIZE_T_MAX;
  if (hThread == NULL)
  {
#if defined _M_IX86
    lpTIB = (NT_TIB*)__readfsdword(0x18);
#elif defined _M_X64
    lpTIB = (NT_TIB*)__readgsqword(0x30);
#endif
  }
  else
  {
    //using documented(?) slow way
    lpTIB = NULL;
    //get TEB address
    if (SUCCEEDED(nktDvDynApis_NtQueryInformationThread(hThread, NKT_DV_ThreadBasicInformation, &sBasicInfo,
                                                        sizeof(sBasicInfo), NULL)))
      lpTIB = (NT_TIB*)(sBasicInfo.TebBaseAddress);
  }
  if (lpTIB != NULL)
  {
    __try
    {
      nHigh = (SIZE_T)(lpTIB->StackBase);
      nLow = (SIZE_T)(lpTIB->StackLimit);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
      nLow = 0;
      nHigh = NKT_SIZE_T_MAX;
    }
  }
  if (nLow > nHigh)
  {
    k = nLow;  nLow = nHigh;  nHigh = k;
  }
  return;
}

static SIZE_T CheckHookedEntry(__in SIZE_T nInstrPointer, __in TNktLnkLst<CNktDvHookCallDataBase> *lpInUseStack)
{
  TNktLnkLst<CNktDvHookCallDataBase>::Iterator it;
  CHookEngCallDataEntry *lpCallDataEntry;
  BOOL bRetry;

  if (lpInUseStack != NULL)
  {
    do
    {
      bRetry = FALSE;
      for (lpCallDataEntry=(CHookEngCallDataEntry*)it.Begin(*lpInUseStack); lpCallDataEntry!=NULL;
           lpCallDataEntry=(CHookEngCallDataEntry*)it.Next())
      {
        if (nInstrPointer == lpCallDataEntry->nAfterCallAddr)
        {
          nInstrPointer = lpCallDataEntry->nReturnAddr;
          bRetry = TRUE;
        }
      }
    }
    while (bRetry != FALSE);
  }
  return nInstrPointer;
}
