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

#include "MySpyMgr.h"

//-----------------------------------------------------------

CMySpyMgr::CMySpyMgr() : CSpyMgr()
{
  return;
}

CMySpyMgr::~CMySpyMgr()
{
  return;
}

HRESULT CMySpyMgr::OnCustomDllLoad(__in Deviare2::INktProcess *proc, __in long actionId,
                                   __in long actionResult)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"Custom DLL load [%lu]: Action:%Iu, hRes=0x%08X\n", pid, actionId, actionResult);
  return S_OK;
}

HRESULT CMySpyMgr::OnCustomDllUnload(__in Deviare2::INktProcess *proc, __in long actionId,
                                     __in long actionResult)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"Custom DLL unload [%lu]: Action:%Iu, hRes=0x%08X\n", pid, actionId, actionResult);
  return S_OK;
}

HRESULT CMySpyMgr::OnCustomApiCall(__in Deviare2::INktProcess *proc, __in long actionId,
                                   __in long actionResult, __in long apiResult)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"Custom API call [%lu]: Result:%lu, Action:%Iu, hRes=0x%08X\n", pid, apiResult, actionId,
           actionResult);
  return S_OK;
}

HRESULT CMySpyMgr::OnHookStateChanged(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                      __in Deviare2::eNktHookState newState,
                                      __in Deviare2::eNktHookState oldState)
{
  WCHAR szBufW[256], *sW;
  long pid, err;
  BSTR bstr;
  VARIANT vt;

  proc->get_Id(&pid);
  switch (newState)
  {
    case Deviare2::stActivating:
      sW = L"Activating";  break;
    case Deviare2::stActive:
      sW = L"Active";  break;
    case Deviare2::stDisabled:
      sW = L"Disabled";  break;
    case Deviare2::stRemoved:
      sW = L"Removing";  break;
    case Deviare2::stNotAvailable:
      sW = L"Not available";  break;
    case Deviare2::stError:
      ::VariantInit(&vt);
      vt.vt = VT_I4;
      vt.lVal = pid;
      Hook->LastError(vt, &err);
      ::VariantClear(&vt);
      wsprintf(szBufW, L"Error: %08X", err);
      sW = szBufW;  break;
    default:
      sW = L"Not found";  break;
  }
  Hook->get_FunctionName(&bstr);
  LogPrint(L"Hook state change [%lu]: %s -> %s\n", pid, (bstr != NULL) ? bstr : L"", sW);
  ::SysFreeString(bstr);
  return S_OK;
}

HRESULT CMySpyMgr::OnFunctionCalled(__in Deviare2::INktHook *Hook, __in Deviare2::INktProcess *proc,
                                    __in Deviare2::INktHookCallInfo *callInfo)
{
  CNktAutoFastMutex cLock(LogGetMutex());
  CComPtr<Deviare2::INktParamsEnum> cParameters;
  CComPtr<Deviare2::INktParam> cParam;
  CComPtr<Deviare2::INktStackTrace> cStTr;
  SIZE_T i, nCount;
  VARIANT_BOOL vbIsPreCall;
  long nPlatformBits;
  BSTR bstr;
  long pid, tid, cookie, lTemp;
  double elapsedTimeMs, childsElapsedTimeMs, kernelTimeMs, userTimeMs;
  unsigned __int64 cpuClockCycles;
  my_ssize_t ssTemp;

  proc->get_Id(&pid);
  callInfo->get_ThreadId(&tid);
  callInfo->get_Cookie(&cookie);
  callInfo->get_IsPreCall(&vbIsPreCall);
  Hook->get_FunctionName(&bstr);
  if (vbIsPreCall != VARIANT_FALSE)
  {
    callInfo->get_KernelModeTimeMs(&kernelTimeMs);
    callInfo->get_UserModeTimeMs(&userTimeMs);
    callInfo->get_CpuClockCycles(&cpuClockCycles);
    LogPrint(L"Hook called [%lu/%lu - %ld]: %s (PreCall)\n"
             L"     [KT:%.6lfms / UT:%.6lfms / CC:%I64u]\n",
             pid, tid, cookie,
             (bstr != NULL) ? bstr : L"",
             kernelTimeMs, userTimeMs, cpuClockCycles);
  }
  else
  {
    callInfo->get_ElapsedTimeMs(&elapsedTimeMs);
    callInfo->get_ChildsElapsedTimeMs(&childsElapsedTimeMs);
    callInfo->get_KernelModeTimeMs(&kernelTimeMs);
    callInfo->get_UserModeTimeMs(&userTimeMs);
    callInfo->get_CpuClockCycles(&cpuClockCycles);
    LogPrint(L"Hook called [%lu/%lu - %ld]: %s (%.3lfms / %.3lfms)\n"
             L"     [KT:%.6lfms / UT:%.6lfms / CC:%I64u]\n",
             pid, tid, cookie,
             (bstr != NULL) ? bstr : L"", elapsedTimeMs, childsElapsedTimeMs,
             kernelTimeMs, userTimeMs, cpuClockCycles);
  }
  ::SysFreeString(bstr);
  if (FAILED(proc->get_PlatformBits(&nPlatformBits)))
    nPlatformBits = 0;
  //show parameters
  //if (callInfo->IsPreCall() != FALSE)
  {
    if (sCmdLineParams.bAsyncCallbacks == FALSE &&
      SUCCEEDED(callInfo->Params(&cParameters)))
    {
      LogPrint(L"  Parameters:\n");
      nCount = (SUCCEEDED(cParameters->get_Count(&lTemp))) ? (SIZE_T)(ULONG)lTemp : 0;
      for (i=0; i<nCount; i++)
      {
        cParam.Release();
        if (SUCCEEDED(cParameters->GetAt((LONG)i, &cParam)))
        {
          PrintParamDetails(cParam, 14, 0, nPlatformBits);
        }
      }
      cParameters.Release();
    }
    if (SUCCEEDED(callInfo->CustomParams(&cParameters)))
    {
      LogPrint(L"  Custom parameters:\n");
      nCount = (SUCCEEDED(cParameters->get_Count(&lTemp))) ? (SIZE_T)(ULONG)lTemp : 0;
      for (i=0; i<nCount; i++)
      {
        cParam.Release();
        if (SUCCEEDED(cParameters->GetAt((LONG)i, &cParam)))
        {
          PrintParamDetails(cParam, 14, 0, nPlatformBits);
        }
      }
      cParameters.Release();
    }
  }
  //show result
  if (vbIsPreCall == VARIANT_FALSE)
  {
    cParam.Release();
    if (SUCCEEDED(callInfo->Result(&cParam)))
    {
      LogPrint(L"  Result:\n");
      PrintParamDetails(cParam, 14, 0, nPlatformBits);
    }
  }
  //show stack trace
  if (sCmdLineParams.bDontDisplayStack == FALSE && vbIsPreCall != VARIANT_FALSE)
  {
    if (SUCCEEDED(callInfo->StackTrace(&cStTr)))
    {
      LogPrint(L"  Stack trace:\n");
      for (i=0; i<4; i++)
      {
        bstr = NULL;
        cStTr->NearestSymbol((LONG)i, &bstr);
        if (bstr == NULL)
          break;
        if (bstr[0] == 0)
        {
          ::SysFreeString(bstr);
          break;
        }
        cStTr->Offset((LONG)i, &ssTemp);
        LogPrint(L"    %lu) %s + 0x%p\n", i+1, bstr, ssTemp);
        ::SysFreeString(bstr);
      }
    }
  }
  LogPrintNoTick(L"\n");
  return S_OK;
}

HRESULT CMySpyMgr::OnCreateProcessCall(__in Deviare2::INktProcess *proc, __in long childPid,
                                       __in long mainThreadId, __in VARIANT_BOOL is64BitProcess,
                                       __in VARIANT_BOOL canHookNow)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"CreateProcess [%lu]: Pid=%lu / Tid=%lu\n", pid, childPid, mainThreadId);
  return S_OK;
}

HRESULT CMySpyMgr::OnLoadLibraryCall(__in Deviare2::INktProcess *proc, __in BSTR dllName,
                                     __in VARIANT moduleHandle)
{
  long pid;
  SIZE_T nModH;

  proc->get_Id(&pid);
  switch (moduleHandle.vt)
  {
    case VT_I4:
      nModH = (SIZE_T)(my_ssize_t)(moduleHandle.lVal);
      break;
    case VT_UI4:
      nModH = (SIZE_T)(moduleHandle.ulVal);
      break;
    case VT_I8:
      nModH = (SIZE_T)(my_ssize_t)(moduleHandle.llVal);
      break;
    case VT_UI8:
      nModH = (SIZE_T)(moduleHandle.ullVal);
      break;
    default:
      nModH = 0;
      break;
  }
  LogPrint(L"LoadLibrary [%lu]: %s / Mod=%p\n", pid, dllName, moduleHandle);
  return S_OK;
}

HRESULT CMySpyMgr::OnFreeLibraryCall(__in Deviare2::INktProcess *proc, __in VARIANT moduleHandle)
{
  long pid;
  SIZE_T nModH;

  proc->get_Id(&pid);
  switch (moduleHandle.vt)
  {
    case VT_I4:
      nModH = (SIZE_T)(my_ssize_t)(moduleHandle.lVal);
      break;
    case VT_UI4:
      nModH = (SIZE_T)(moduleHandle.ulVal);
      break;
    case VT_I8:
      nModH = (SIZE_T)(my_ssize_t)(moduleHandle.llVal);
      break;
    case VT_UI8:
      nModH = (SIZE_T)(moduleHandle.ullVal);
      break;
    default:
      nModH = 0;
      break;
  }
  LogPrint(L"FreeLibrary [%lu]: Mod=%p\n", pid, nModH);
  return S_OK;
}

HRESULT CMySpyMgr::OnProcessStarted(__in Deviare2::INktProcess *proc)
{
  long pid;
  BSTR bstr;

  proc->get_Id(&pid);
  proc->get_Path(&bstr);
  LogPrint(L"New process [%lu]: %s\n", pid, (bstr != NULL) ? bstr : L"");
  ::SysFreeString(bstr);
  return S_OK;
}

HRESULT CMySpyMgr::OnProcessTerminated(__in Deviare2::INktProcess *proc)
{
  long pid;
  BSTR bstr;

  proc->get_Id(&pid);
  proc->get_Path(&bstr);
  LogPrint(L"Process terminated [%lu]: %s\n", pid, (bstr != NULL) ? bstr : L"");
  ::SysFreeString(bstr);
  return S_OK;
}

HRESULT CMySpyMgr::OnAgentLoad(__in Deviare2::INktProcess *proc, __in long errorCode)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"Agent load [%lu]: hRes=0x%08X\n", pid, errorCode);
  return S_OK;
}

HRESULT CMySpyMgr::OnAgentUnload(__in Deviare2::INktProcess *proc)
{
  long pid;

  proc->get_Id(&pid);
  LogPrint(L"Agent unload [%lu]\n", pid);
  return S_OK;
}

//-----------------------------------------------------------

VOID CMySpyMgr::PrintParamDetails(__in Deviare2::INktParam *lpParam, __in SIZE_T nCurrTabSize,
                                  __in SIZE_T nDepth, __in long nPlatformBits)
{
  CComPtr<Deviare2::INktParam> cTempParam;
  HRESULT hRes;
  SIZE_T i, nCount;
  union {
    char nInt8;
    unsigned char nUInt8;
    short nInt16;
    unsigned short nUInt16;
    long nInt32;
    unsigned long nUInt32;
    __int64 nInt64;
    unsigned __int64 nUInt64;
    float nFlt;
    double nDbl;
    long double nLngDbl;
    BSTR bstr;
  } uVal;
  CComBSTR cBstrTypeName, cBstrName;
  my_ssize_t ssAddr;
  Deviare2::eNktDboClass nClass;
  Deviare2::eNktDboFundamentalType nFundType;
  CComVariant cVt;
  VARIANT_BOOL vb;

  if (nDepth > 2)
    return;
  PrintSpaces(nCurrTabSize);
  //----
  lpParam->get_TypeName(&cBstrTypeName);
  lpParam->get_Name(&cBstrName);
  lpParam->get_Address(&ssAddr);
  lpParam->get_IntResourceString(&uVal.nInt32);
  if (uVal.nInt32 > 0)
  {
    LogPrintNoTick(L"%s %s [0x%p] \"%lu\" (resource-string)\n",
                   (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                   (cBstrName != NULL) ? (BSTR)cBstrName : NULL,
                   ssAddr, uVal.nInt32);
    return;
  }
  //----
  lpParam->get_IsAnsiString(&vb);
  if (vb != VARIANT_FALSE)
  {
    lpParam->ReadString(&uVal.bstr);
    LogPrintNoTick(L"%s %s [0x%p] \"%S\" (ansi-string)\n",
                   (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                   (cBstrName != NULL) ? (BSTR)cBstrName : NULL,
                   ssAddr, (uVal.bstr != NULL) ? uVal.bstr : L"");
    ::SysFreeString(uVal.bstr);
    return;
  }
  //----
  lpParam->get_IsWideString(&vb);
  if (vb != VARIANT_FALSE)
  {
    lpParam->ReadString(&uVal.bstr);
    LogPrintNoTick(L"%s %s [0x%p] \"%s\" (wide-string)\n",
                   (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                   (cBstrName != NULL) ? (BSTR)cBstrName : NULL,
                   ssAddr, (uVal.bstr != NULL) ? uVal.bstr : L"");
    ::SysFreeString(uVal.bstr);
    return;
  }
  //----
  lpParam->get_IsPointer(&vb);
  if (vb == VARIANT_FALSE)
    lpParam->get_IsArray(&vb);
  if (vb != VARIANT_FALSE)
  {
    lpParam->get_Declaration(&uVal.bstr);
    LogPrintNoTick(L"%s [0x%p]\n", (uVal.bstr != NULL) ? uVal.bstr : L"", ssAddr);
    ::SysFreeString(uVal.bstr);
    hRes = lpParam->Evaluate(&cTempParam);
    if (SUCCEEDED(hRes))
      PrintParamDetails(cTempParam, nCurrTabSize+2, nDepth+1, nPlatformBits);
    return;
  }
  //----
  lpParam->get_Class(&nClass);
  switch (nClass)
  {
    case Deviare2::clsFundamental:
      lpParam->get_BasicType(&nFundType);
      switch (nFundType)
      {
        case Deviare2::ftSignedByte:
          hRes = lpParam->get_CharVal(&uVal.nInt8);
          if (FAILED(hRes))
            uVal.nInt8 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%d\" (signed byte)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (int)(uVal.nInt8));
          break;
        case Deviare2::ftUnsignedByte:
          hRes = lpParam->get_ByteVal(&uVal.nUInt8);
          if (FAILED(hRes))
            uVal.nUInt8 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%u\" (unsigned byte)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (unsigned int)(uVal.nUInt8));
          break;
        case Deviare2::ftSignedWord:
          hRes = lpParam->get_ShortVal(&uVal.nInt16);
          if (FAILED(hRes))
            uVal.nInt16 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%d\" (signed word)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (int)(uVal.nInt16));
          break;
        case Deviare2::ftUnsignedWord:
          hRes = lpParam->get_UShortVal(&uVal.nUInt16);
          if (FAILED(hRes))
            uVal.nUInt16 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%u\" (unsigned word)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (unsigned int)(uVal.nUInt16));
          break;
        case Deviare2::ftSignedDoubleWord:
          hRes = lpParam->get_LongVal(&uVal.nInt32);
          if (FAILED(hRes))
            uVal.nInt32 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%d\" (signed dword)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (int)(uVal.nInt32));
          break;
        case Deviare2::ftUnsignedDoubleWord:
          hRes = lpParam->get_ULongVal(&uVal.nUInt32);
          if (FAILED(hRes))
            uVal.nUInt32 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%u\" (unsigned dword)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (unsigned int)(uVal.nUInt32));
          break;
        case Deviare2::ftSignedQuadWord:
          hRes = lpParam->get_LongLongVal(&uVal.nInt64);
          if (FAILED(hRes))
            uVal.nInt64 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%I64d\" (signed qword)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         uVal.nInt64);
          break;
        case Deviare2::ftUnsignedQuadWord:
          hRes = lpParam->get_ULongLongVal(&uVal.nUInt64);
          if (FAILED(hRes))
            uVal.nUInt64 = 0;
          LogPrintNoTick(L"%s %s [0x%p] \"%I64u\" (unsigned qword)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         uVal.nUInt64);
          break;
        case Deviare2::ftFloat:
          hRes = lpParam->get_FloatVal(&uVal.nFlt);
          if (FAILED(hRes))
            uVal.nFlt = 0.0f;
          LogPrintNoTick(L"%s %s [0x%p] \"%f\" (float)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         (double)(uVal.nFlt));
          break;
        case Deviare2::ftDouble:
          hRes = lpParam->get_DoubleVal(&uVal.nDbl);
          if (FAILED(hRes))
            uVal.nDbl = 0.0;
          LogPrintNoTick(L"%s %s [0x%p] \"%f\" (double)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         uVal.nDbl);
          break;
        case Deviare2::ftLongDouble:
          hRes = lpParam->get_DoubleVal(&uVal.nDbl);
          if (FAILED(hRes))
            uVal.nLngDbl = 0.0;
          LogPrintNoTick(L"%s %s [0x%p] \"%Lf\" (long double)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr,
                         uVal.nLngDbl);
          break;
        case Deviare2::ftAnsiChar:
          LogPrintNoTick(L"%s %s [0x%p] (ansi char)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr);
          break;
        case Deviare2::ftWideChar:
          LogPrintNoTick(L"%s %s [0x%p] (wide char)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr);
          break;
        default:
          {
          CComPtr<Deviare2::INktProcessMemory> cProcMem;
          SIZE_T nValue;
          ULONGLONG ull;
          DWORD dw;

          nValue = 0;
          if (ssAddr != 0)
          {
            hRes = lpParam->Memory(&cProcMem);
            if (SUCCEEDED(hRes))
            {
              switch (nPlatformBits)
              {
                case 32:
                  if (FAILED(cProcMem->get_ULongVal(ssAddr, &dw)))
                    dw = 0;
                  ssAddr = (SIZE_T)dw;
                  break;
                case 64:
                  if (FAILED(cProcMem->get_ULongLongVal(ssAddr, &ull)))
                    ull = 0;
                  ssAddr = (SIZE_T)ull;
                  break;
                default:
                  ssAddr = 0;
                  break;
              }
            }
          }
#if defined _M_IX86
          LogPrintNoTick(L"%s %s [0x%p] \"%u\" (unsupp. basic type)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, nValue, ssAddr);
#elif defined _M_X64
          LogPrintNoTick(L"%s %s [0x%p] \"%I64u\" (unsupp. basic type)\n",
                         (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                         (cBstrName != NULL) ? (BSTR)cBstrName : NULL, nValue, ssAddr);
#endif
          }
          break;
      }
      break;
    case Deviare2::clsStruct:
    case Deviare2::clsUnion:
      LogPrintNoTick(L"%s %s [0x%p] (struct/union)\n",
                     (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                     (cBstrName != NULL) ? (BSTR)cBstrName : NULL,
                     ssAddr);
      nCount = (SUCCEEDED(lpParam->get_FieldsCount(&(uVal.nInt32)))) ? (SIZE_T)(ULONG)uVal.nInt32 : 0;
      for (i=0; i<nCount; i++)
      {
        cTempParam.Release();
        cVt = (LONG)i;
        hRes = lpParam->Field(cVt, &cTempParam);
        if (SUCCEEDED(hRes))
          PrintParamDetails(cTempParam, nCurrTabSize+2, nDepth+1, nPlatformBits);
      }
      break;
    case Deviare2::clsEnumeration:
      LogPrintNoTick(L"%s %s [0x%p] (enum)\n",
                     (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                     (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr);
      break;
    case Deviare2::clsFunction:
      LogPrintNoTick(L"%s %s [0x%p] (function)\n",
                     (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                     (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr);
      break;
    case Deviare2::clsFunctionType:
      LogPrintNoTick(L"%s %s [0x%p] (function-type)\n",
                     (cBstrTypeName != NULL) ? (BSTR)cBstrTypeName : NULL,
                     (cBstrName != NULL) ? (BSTR)cBstrName : NULL, ssAddr);
      break;
  }
  return;
}

VOID CMySpyMgr::PrintSpaces(__in SIZE_T nCount)
{
  for (; nCount >= 8; nCount -= 8)
    LogPrintNoTick(L"        ");
  for (; nCount >= 4; nCount -= 4)
    LogPrintNoTick(L"    ");
  for (; nCount > 0; nCount--)
    LogPrintNoTick(L" ");
  return;
}
