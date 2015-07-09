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

#include "FunctionParamsCache.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

CNktDvFunctionParamsCache::CNktDvFunctionParamsCache() : CNktFastMutex()
{
  return;
}

CNktDvFunctionParamsCache::~CNktDvFunctionParamsCache()
{
  return;
}

HRESULT CNktDvFunctionParamsCache::Initialize(__in CNktDvDbObjectNoRef *lpDbObjFunc,
                                              __in CNktDvProcessMemory *lpProcMem,
                                              __in SIZE_T nPlatformBits)
{
  TNktComPtr<CNktDvEngDatabase> cDvDB;
  TNktComPtr<CNktDvParam> cTempParam;
  CNktDvDbObjectNoRef *lpDbObjTemp;
  SIZE_T i, nCount;
  HRESULT hRes;

  hRes = CNktDvEngDatabase::Get(&cDvDB, NULL, nPlatformBits);
  if (FAILED(hRes))
    return hRes;
  //create the base FUNCTION_PARAMS object for doing fast initialization of parameters and
  //results in each api call
  cBaseFuncParam.Attach(NKT_MEMMGR_NEW FUNCTION_PARAMS);
  if (cBaseFuncParam == NULL)
    return E_OUTOFMEMORY;
  cBaseFuncParam->cParameters.Attach(NKT_MEMMGR_NEW CNktDvParamsEnumerator);
  cBaseFuncParam->cResult.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
  if (cBaseFuncParam->cParameters == NULL || cBaseFuncParam->cResult == NULL)
    return E_OUTOFMEMORY;
  //----
  if (lpDbObjFunc != NULL)
  {
    NKT_ASSERT(lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsFunction ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsFunctionType ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassConstructor ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassDestructor ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassOperatorMethod ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassMethod ||
               lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassConverter);
    NKT_ASSERT(lpProcMem != NULL);
    //process function parameters
    nCount = lpDbObjFunc->GetItemsCount();
    for (i=0; i<nCount; i++)
    {
      cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nPlatformBits));
      if (cTempParam == NULL)
        return E_OUTOFMEMORY;
      cTempParam->lpDbParentObj = lpDbObjFunc;
      cTempParam->lpDbObj = lpDbObjFunc->GetItem(i);
      NKT_ASSERT(cTempParam->lpDbObj != NULL);
      cTempParam->nFlags = lpDbObjFunc->GetItemFlags(i);
      cTempParam->cProcMem = lpProcMem;
      cTempParam->szNameW = lpDbObjFunc->GetItemName(i);
      NKT_ASSERT(cTempParam->szNameW != NULL);
      cTempParam->szTypeNameW = cTempParam->lpDbObj->GetName();
      NKT_ASSERT(cTempParam->szTypeNameW != NULL);
      cTempParam->szDeclW = lpDbObjFunc->GetItemDeclaration(i);
      if (cTempParam->szDeclW == NULL)
        return E_OUTOFMEMORY;
      hRes = cBaseFuncParam->cParameters->Add(cTempParam);
      if (FAILED(hRes))
        return hRes;
      cTempParam.Release();
    }
    //process function result
    cBaseFuncParam->cResult->lpDbParentObj = lpDbObjFunc;
    cBaseFuncParam->cResult->lpDbObj = lpDbObjFunc->GetFunctionReturn();
    cBaseFuncParam->cResult->nFlags = lpDbObjFunc->GetFunctionReturnFlags();
    cBaseFuncParam->cResult->szTypeNameW = cBaseFuncParam->cResult->lpDbObj->GetName();
    NKT_ASSERT(cBaseFuncParam->cResult->szTypeNameW != NULL);
    cBaseFuncParam->cResult->szDeclW = lpDbObjFunc->GetFunctionReturnDeclaration();
    if (cBaseFuncParam->cResult->szDeclW == NULL)
      return E_OUTOFMEMORY;
    cBaseFuncParam->cResult->cProcMem = lpProcMem;
  }
  else
  {
    cBaseFuncParam->cResult->lpDbParentObj = lpDbObjFunc;
#if defined _M_IX86
    lpDbObjTemp = cDvDB->FindObjectByName(L"unsigned long");
#elif defined _M_X64
    lpDbObjTemp = cDvDB->FindObjectByName(L"unsigned long long");
#endif
    if (lpDbObjTemp == NULL)
      return E_FAIL;
    cBaseFuncParam->cResult->lpDbObj = lpDbObjTemp;
    cBaseFuncParam->cResult->nFlags = lpDbObjTemp->GetFlags();
    cBaseFuncParam->cResult->szDeclW = lpDbObjTemp->GetDeclaration();
    if (cBaseFuncParam->cResult->szDeclW == NULL)
      return E_OUTOFMEMORY;
    cBaseFuncParam->cResult->cProcMem = lpProcMem;
  }
  return S_OK;
}

SIZE_T CNktDvFunctionParamsCache::GetStackUsage(__in CNktDvDbObjectNoRef *lpDbObjFunc)
{
#if defined _M_IX86
  CNktDvDbObjectNoRef::eCallingConvention nCallConv;
#elif defined _M_X64
  SIZE_T k;
#endif //_M_IX86
  SIZE_T i, nCount, nArgSize, nStackUsage, nTempFlags;
  CNktDvDbObjectNoRef *lpDbObj;

  NKT_ASSERT(lpDbObjFunc != NULL);
  NKT_ASSERT(lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsFunction ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsFunctionType ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassConstructor ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassDestructor ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassOperatorMethod ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassMethod ||
              lpDbObjFunc->GetClass() == CNktDvDbObjectNoRef::clsClassConverter);
#if defined _M_IX86
  nCallConv = lpDbObjFunc->GetFuncCallingConvention();
#endif //_M_IX86
  nTempFlags = 0; //multiuse flags
  nStackUsage = 0;
  //process return param first (can change stack pointer)
  lpDbObj = lpDbObjFunc->GetFunctionReturn();
  NKT_ASSERT(lpDbObj != NULL);
  //----
  nArgSize = (lpDbObj->GetSize() + 7) >> 3;

#if defined _M_IX86

  switch (nCallConv)
  {
    case CNktDvDbObjectNoRef::ccFastCall:
      //check if we are returning a struct/union
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
          lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
      {
        if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasConstructor|
                                    CNktDvDbObject::suflgHasDestructor|CNktDvDbObject::suflgHasVirtual|
                                    CNktDvDbObject::suflgHasInheritance)) != 0)
        {
          //if struct is has virtual/inh/etc., a pointer to it is passed via ecx so mark ecx as used
          nTempFlags |= 0x0001;
          break;
        }
      }
      //fall into default

    default:
      //CNktDbObject::ccCDecl, CNktDbObject::ccStdCall or CNktDvDbObjectNoRef::ccThisCall
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
          (lpDbObj->GetFundamentalType() == CNktDvDbObject::ftFloat ||
           lpDbObj->GetFundamentalType() == CNktDvDbObject::ftDouble ||
           lpDbObj->GetFundamentalType() == CNktDvDbObject::ftLongDouble))
      {
        //floating point results are returned in fpu register
        break;
      }
      if (nArgSize > 8)
      {
        //else if object size is greather than 8 bytes, a pointer to it is passed via stack
        nStackUsage += 4;
        break;
      }
      break;
  }

#elif defined _M_X64

  //check if we are returning a struct/union
  if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
      lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
  {
    if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasConstructor|CNktDvDbObject::suflgHasDestructor|
                                CNktDvDbObject::suflgHasVirtual|CNktDvDbObject::suflgHasInheritance)) != 0)
    {
      //if struct is has virtual/inh/etc., it is passed via rcx so mark rcx as used
aa1_result_assign1:
      nTempFlags = 1;
      
      goto aa1_after_result_assign;
    }
  }
  k = lpDbObj->GetSize();
  if (k > 0 && ((k & ((~k)+1)) != k))
  {
    //if size is not a power of 2, also a pointer is passed via rcx like previous case
    goto aa1_result_assign1;
  }
  if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
      (lpDbObj->GetFundamentalType() == CNktDvDbObject::ftFloat ||
       lpDbObj->GetFundamentalType() == CNktDvDbObject::ftDouble ||
       lpDbObj->GetFundamentalType() == CNktDvDbObject::ftLongDouble))
  {
    goto aa1_after_result_assign;
  }
  if (nArgSize > 8)
  {
    //else if struct size is greather than 8 bytes, it is passed via stack
    //a pointer to the struct is returned in eax but we address the struct itself
    nStackUsage += (nArgSize + 7) & (~7); //qword align
    goto aa1_after_result_assign;
  }
aa1_after_result_assign: ;

#endif

  //process parameters
  nCount = lpDbObjFunc->GetItemsCount();
  for (i=0; i<nCount; i++)
  {
    lpDbObj = lpDbObjFunc->GetItem(i);
    NKT_ASSERT(lpDbObj != NULL);
    //----
    nArgSize = (lpDbObj->GetSize() + 7) >> 3;

#if defined _M_IX86

    switch (nCallConv)
    {
      case CNktDvDbObjectNoRef::ccThisCall:
        if ((nTempFlags & 0x0001) != 1)
        {
          //first parameter is passed to ecx
          nTempFlags |= 0x0001;
          break;
        }
        //fall into default
        goto gsu_default1;

      case CNktDvDbObjectNoRef::ccFastCall:
        if (nArgSize <= 4 && (nTempFlags & 0x0003) != 3)
        {
          if ((nTempFlags & 0x0001) == 0)
            nTempFlags |= 0x0001;
          else
            nTempFlags |= 0x0002;
          break;
        }
        //fall into default

      default:
gsu_default1:
        nStackUsage += (nArgSize + 3) & (~3); //dword align
        break;
    }

#elif defined _M_X64

    //check if we are passing a struct/union
    if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
        lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
    {
      if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasVirtual|
                                  CNktDvDbObject::suflgHasInheritance)) != 0)
      {
        //if struct is has virtual/inh/etc., it is passed via available register
aa1_param_assign1:
        if (nTempFlags >= 4)
        {
          //reference to object stored in the stack
          nStackUsage += 8;
        }
        goto aa1_after_param_assign;
      }
    }
    k = lpDbObj->GetSize();
    if (k > 0 && ((k & ((~k)+1)) != k))
    {
      //if size is not a power of 2, also a pointer is passed via rcx/rdx/r8/r9 like previous case
      goto aa1_param_assign1;
    }
    if (nArgSize > 8)
    {
      //if object is greather than 8 bytes, the same applies
      goto aa1_param_assign1;
    }
    if (nTempFlags >= 4)
    {
      //passed in the stack
      nStackUsage += 8; //qword align (it is safe to add 8 here)
    }
aa1_after_param_assign:
    nTempFlags++;

#endif
  }
  //done
  return nStackUsage;
}

#if defined _M_IX86

HRESULT CNktDvFunctionParamsCache::PreCall_AssignAddresses(__inout FUNCTION_PARAMS *lpItem,
                                                           __in CNktDvDbObjectNoRef *lpDbObjFunc,
                                                           __in LPNKT_DV_ASMREGISTERS32 lpRegisters)
{
  TNktComPtr<CNktDvParam> cTempParam;
  CNktDvDbObjectNoRef::eCallingConvention nCallConv;
  SIZE_T i, nCount, nArgSize, nTempFlags;
  DWORD nSP;
  HRESULT hRes;
  CNktDvDbObjectNoRef *lpDbObj;

  NKT_ASSERT(lpItem != NULL);
  nCallConv = (lpDbObjFunc != NULL) ? (lpDbObjFunc->GetFuncCallingConvention()) :
                                      CNktDvDbObjectNoRef::ccNA;
  nSP = lpRegisters->sInteger.Esp + 4; //skip return address
  nTempFlags = 0; //multiuse flags
  //process return param first (can change stack pointer)
  if (lpDbObjFunc != NULL)
  {
    lpDbObj = lpDbObjFunc->GetFunctionReturn();
    NKT_ASSERT(lpDbObj != NULL);
    //----
    nArgSize = (lpDbObj->GetSize() + 7) >> 3;
    switch (nCallConv)
    {
      case CNktDvDbObjectNoRef::ccFastCall:
        //check if we are returning a struct/union
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
            lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
        {
          if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasConstructor|
                                      CNktDvDbObject::suflgHasDestructor|CNktDvDbObject::suflgHasVirtual|
                                      CNktDvDbObject::suflgHasInheritance)) != 0)
          {
            //if struct is has virtual/inh/etc., it is passed via ecx so mark ecx as used
            nTempFlags |= 0x0001;
            //ecx contains the address of the structure itself (don't pass pointer-to)
            lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
            lpItem->cResult->lpAddress = (LPVOID)(lpRegisters->sInteger.Ecx);
            break;
          }
        }
        //fall into default

      default:
        //CNktDbObject::ccCDecl, CNktDbObject::ccStdCall or CNktDvDbObjectNoRef::ccThisCall
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
            (lpDbObj->GetFundamentalType() == CNktDvDbObject::ftFloat ||
             lpDbObj->GetFundamentalType() == CNktDvDbObject::ftDouble ||
             lpDbObj->GetFundamentalType() == CNktDvDbObject::ftLongDouble))
        {
          //floating point results are returned in fpu register
          lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegSt0;
          lpItem->cResult->lpAddress = &(lpRegisters->sFloating.St0);
        }
        else if (nArgSize > 8)
        {
          //else if struct size is greather than 8 bytes, it is passed via stack
          //a pointer to the struct is returned in eax but we address the struct itself
          lpItem->cResult->lpAddress = (LPVOID)nSP;
          lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
          nSP += (nArgSize + 3) & (~3); //dword align
        }
        else if (nArgSize <= 4)
        {
          //if less or equal than 4 bytes, it is returned in eax
          lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEax;
          lpItem->cResult->lpAddress = &(lpRegisters->sInteger.Eax);
          break;
        }
        else
        {
          //else returned in eax/edx pair
          lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEaxEdxPair;
          lpItem->cResult->lpAddress = &(lpRegisters->sInteger.Eax);
        }
        break;
    }
  }
  else
  {
    //if no function object is provided assume a result that is stored in the eax/rax register
    lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEax;
    lpItem->cResult->lpAddress = &(lpRegisters->sInteger.Eax);
  }
  lpItem->cResult->Agent_Init();
  //process parameters
  if (lpDbObjFunc != NULL)
  {
    nCount = lpItem->cParameters->GetCount();
    for (i=0; i<nCount; i++)
    {
      cTempParam.Release();
      hRes = lpItem->cParameters->GetAt(i, &cTempParam);
      if (FAILED(hRes))
        return hRes;
      lpDbObj = lpDbObjFunc->GetItem(i);
      NKT_ASSERT(lpDbObj != NULL);
      //----
      nArgSize = (lpDbObj->GetSize() + 7) >> 3;
      switch (nCallConv)
      {
        case CNktDvDbObjectNoRef::ccThisCall:
          //first parameter is passed via ecx
          if ((nTempFlags & 0x0001) == 0)
          {
            cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEcx;
            cTempParam->lpAddress = &(lpRegisters->sInteger.Ecx);
            nTempFlags |= 0x0001;
            break;
          }
          goto pcaa_default1;

        case CNktDvDbObjectNoRef::ccFastCall:
          if (nArgSize <= 4 && (nTempFlags & 0x0003) != 3)
          {
            if ((nTempFlags & 0x0001) == 0)
            {
              cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEcx;
              cTempParam->lpAddress = &(lpRegisters->sInteger.Ecx);
              nTempFlags |= 0x0001;
            }
            else
            {
              cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegEdx;
              cTempParam->lpAddress = &(lpRegisters->sInteger.Edx);
              nTempFlags |= 0x0002;
            }
            break;
          }
          //fall into default

        default: //CNktDbObject::ccCDecl or case CNktDbObject::ccStdCall:
pcaa_default1:
          cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
          cTempParam->lpAddress = (LPVOID)nSP;
          nSP += (nArgSize + 3) & (~3); //dword align
          break;
      }
      cTempParam->Agent_Init();
    }
  }
  //done
  return S_OK;
}

#elif defined _M_X64

HRESULT CNktDvFunctionParamsCache::PreCall_AssignAddresses(__inout FUNCTION_PARAMS *lpItem,
                                                           __in CNktDvDbObjectNoRef *lpDbObjFunc,
                                                           __in LPNKT_DV_ASMREGISTERS64 lpRegisters)
{
  TNktComPtr<CNktDvParam> cTempParam;
  static const ENktDvMiscEnums::eNktDvRegister aRegsIntEnum[4] = {
    ENktDvMiscEnums::asmRegRcx, ENktDvMiscEnums::asmRegRdx,
    ENktDvMiscEnums::asmRegR8, ENktDvMiscEnums::asmRegR9
  };
  static const ENktDvMiscEnums::eNktDvRegister aRegsFltEnum[4] = {
    ENktDvMiscEnums::asmRegXmm0, ENktDvMiscEnums::asmRegXmm1,
    ENktDvMiscEnums::asmRegXmm2, ENktDvMiscEnums::asmRegXmm3
  };
  DWORD64 *lpRegsIntPtrs[4];
  double *lpRegsFltPtrs[4];
  SIZE_T i, k, nCount, nArgSize, nTempFlags;
  DWORD64 nSP;
  HRESULT hRes;
  CNktDvDbObjectNoRef *lpDbObj;

  NKT_ASSERT(lpItem != NULL);
  nSP = lpRegisters->sInteger.Rsp + 40; //skip return address plus shadow space
  lpRegsIntPtrs[0] = &(lpRegisters->sInteger.Rcx);
  lpRegsIntPtrs[1] = &(lpRegisters->sInteger.Rdx);
  lpRegsIntPtrs[2] = &(lpRegisters->sInteger.R8);
  lpRegsIntPtrs[3] = &(lpRegisters->sInteger.R9);
  lpRegsFltPtrs[0] = &(lpRegisters->sFloating.Xmm0);
  lpRegsFltPtrs[1] = &(lpRegisters->sFloating.Xmm1);
  lpRegsFltPtrs[2] = &(lpRegisters->sFloating.Xmm2);
  lpRegsFltPtrs[3] = &(lpRegisters->sFloating.Xmm3);
  nTempFlags = 0; //multiuse flags
  //process return param first (can change stack pointer)
  if (lpDbObjFunc != NULL)
  {
    lpDbObj = lpDbObjFunc->GetFunctionReturn();
    NKT_ASSERT(lpDbObj != NULL);
    //----
    nArgSize = (lpDbObj->GetSize() + 7) >> 3;
    //check if we are returning a struct/union
    if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
        lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
    {
      if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasConstructor|CNktDvDbObject::suflgHasDestructor|
        CNktDvDbObject::suflgHasVirtual|CNktDvDbObject::suflgHasInheritance)) != 0)
      {
        //if struct is has virtual/inh/etc., it is passed via rcx so mark rcx as used
aa1_result_assign1:
        nTempFlags = 1;
        //rcx contains the address of the structure itself (don't pass pointer-to)
        lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegRcx;
        lpItem->cResult->lpAddress = (LPVOID)(lpRegisters->sInteger.Rcx);
        goto aa1_after_result_assign;
      }
    }
    k = lpDbObj->GetSize();
    if (k > 0 && ((k & ((~k)+1)) != k))
    {
      //if size is not a power of 2, also a pointer is passed via rcx like previous case
      goto aa1_result_assign1;
    }
    if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
        (lpDbObj->GetFundamentalType() == CNktDvDbObject::ftFloat ||
        lpDbObj->GetFundamentalType() == CNktDvDbObject::ftDouble ||
        lpDbObj->GetFundamentalType() == CNktDvDbObject::ftLongDouble))
    {
      //floating point results are returned in xmm0 register
      lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegXmm0;
      lpItem->cResult->lpAddress = &(lpRegisters->sFloating.Xmm0);
      goto aa1_after_result_assign;
    }
    if (nArgSize > 8)
    {
      //else if struct size is greather than 8 bytes, it is passed via stack
      //a pointer to the struct is returned in eax but we address the struct itself
      lpItem->cResult->lpAddress = (LPVOID)nSP;
      lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
      nSP += (nArgSize + 7) & (~7); //qword align
      goto aa1_after_result_assign;
    }
    else
    {
      //else returned in rax
      lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegRax;
      lpItem->cResult->lpAddress = &(lpRegisters->sInteger.Rax);
    }
aa1_after_result_assign: ;
  }
  else
  {
    //if no function object is provided assume a result that is stored in the eax/rax register
    lpItem->cResult->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegRax;
    lpItem->cResult->lpAddress = &(lpRegisters->sInteger.Rax);
  }
  lpItem->cResult->Agent_Init();
  //process parameters
  if (lpDbObjFunc != NULL)
  {
    nCount = lpItem->cParameters->GetCount();
    for (i=0; i<nCount; i++)
    {
      cTempParam.Release();
      hRes = lpItem->cParameters->GetAt(i, &cTempParam);
      if (FAILED(hRes))
        return hRes;
      lpDbObj = lpDbObjFunc->GetItem(i);
      NKT_ASSERT(lpDbObj != NULL);
      //----
      nArgSize = (lpDbObj->GetSize() + 7) >> 3;
      //check if we are passing a struct/union
      if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsStruct ||
          lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsUnion)
      {
        if ((lpDbObj->GetFlags() & (CNktDvDbObject::suflgHasVirtual|
                                    CNktDvDbObject::suflgHasInheritance)) != 0)
        {
          //if struct is has virtual/inh/etc., it is passed via available register
aa1_param_assign1:
          if (nTempFlags < 4)
          {
            //rcx/rdx/r8/r9 contains the address of the structure itself (don't pass pointer-to)
            cTempParam->nInternalAsmRegisterIndex = aRegsIntEnum[nTempFlags];
            cTempParam->lpAddress = lpRegsIntPtrs[nTempFlags];
          }
          else
          {
            //reference to object stored in the stack
            cTempParam->lpAddress = (LPVOID)nSP;
            cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
            nSP += 8;
          }
          goto aa1_after_param_assign;
        }
      }
      k = lpDbObj->GetSize();
      if (k > 0 && ((k & ((~k)+1)) != k))
      {
        //if size is not a power of 2, also a pointer is passed via rcx/rdx/r8/r9 like previous case
        goto aa1_param_assign1;
      }
      if (nArgSize > 8)
      {
        //if object is greather than 8 bytes, the same applies
        goto aa1_param_assign1;
      }
      if (nTempFlags < 4)
      {
        if (lpDbObj->GetClass() == CNktDvDbObjectNoRef::clsFundamental &&
            (lpDbObj->GetFundamentalType() == CNktDvDbObject::ftFloat ||
            lpDbObj->GetFundamentalType() == CNktDvDbObject::ftDouble ||
            lpDbObj->GetFundamentalType() == CNktDvDbObject::ftLongDouble))
        {
          //floating point params are sent in xmm0-3 registers
          cTempParam->nInternalAsmRegisterIndex = aRegsFltEnum[nTempFlags];
          cTempParam->lpAddress = lpRegsFltPtrs[nTempFlags];
        }
        else
        {
          //passed in integer register
          cTempParam->nInternalAsmRegisterIndex = aRegsIntEnum[nTempFlags];
          cTempParam->lpAddress = lpRegsIntPtrs[nTempFlags];
        }
      }
      else
      {
        //passed in the stack
        cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
        cTempParam->lpAddress = (LPVOID)nSP;
        nSP += 8; //qword align (it is safe to add 8 here)
      }
aa1_after_param_assign:
      nTempFlags++;
      cTempParam->Agent_Init();
    }
  }
  //done
  return S_OK;
}

#endif

VOID CNktDvFunctionParamsCache::PreCall_ReassignAddressesAndSaveOriginalStackValues(
                                    __inout FUNCTION_PARAMS *lpItem)
{
  TNktComPtr<CNktDvParam> cTempParam;
  SIZE_T i, nCount;
  HRESULT hRes;

  NKT_ASSERT(lpItem != NULL);
  //process result
  if (lpItem->cResult->nInternalAsmRegisterIndex == ENktDvMiscEnums::asmRegNone)
    lpItem->cResult->nOriginalStackValue = *((SIZE_T*)(lpItem->cResult->lpAddress));
  //process parameters
  nCount = lpItem->cParameters->GetCount();
  for (i=0; i<nCount; i++)
  {
    cTempParam.Release();
    hRes = lpItem->cParameters->GetAt(i, &cTempParam);
    NKT_ASSERT(SUCCEEDED(hRes));
    //----
    if (cTempParam->nInternalAsmRegisterIndex == ENktDvMiscEnums::asmRegNone)
      cTempParam->nOriginalStackValue = *((SIZE_T*)(cTempParam->lpAddress));
  }
  //done
  return;
}

#if defined _M_IX86

VOID CNktDvFunctionParamsCache::PostCall_RestoreOriginalStackValues(__inout FUNCTION_PARAMS *lpItem,
                                                                 __in LPNKT_DV_ASMREGISTERS32 lpRegisters)
{
  TNktComPtr<CNktDvParam> cTempParam;
  SIZE_T i, nCount;
  HRESULT hRes;

  NKT_ASSERT(lpItem != NULL);
  //process result
  switch (lpItem->cResult->nInternalAsmRegisterIndex)
  {
    case ENktDvMiscEnums::asmRegNone:
      *((SIZE_T*)(lpItem->cResult->lpAddress)) = lpItem->cResult->nOriginalStackValue;
      break;
  }
  //process parameters
  nCount = lpItem->cParameters->GetCount();
  for (i=0; i<nCount; i++)
  {
    cTempParam.Release();
    hRes = lpItem->cParameters->GetAt(i, &cTempParam);
    NKT_ASSERT(SUCCEEDED(hRes));
    //----
    switch (cTempParam->nInternalAsmRegisterIndex)
    {
      case ENktDvMiscEnums::asmRegNone:
        *((SIZE_T*)(cTempParam->lpAddress)) = cTempParam->nOriginalStackValue;
        break;
      case ENktDvMiscEnums::asmRegEcx:
        cTempParam->lpAddress = &(lpRegisters->sInteger.Ecx);
        break;
      case ENktDvMiscEnums::asmRegEdx:
        cTempParam->lpAddress = &(lpRegisters->sInteger.Edx);
        break;
    }
    ////in the after-call parameters always points to backup'ed memory locations
    //cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  }
  //done
  return;
}

#elif defined _M_X64

VOID CNktDvFunctionParamsCache::PostCall_RestoreOriginalStackValues(__inout FUNCTION_PARAMS *lpItem,
                                                                 __in LPNKT_DV_ASMREGISTERS64 lpRegisters)
{
  TNktComPtr<CNktDvParam> cTempParam;
  SIZE_T i, nCount;
  HRESULT hRes;

  NKT_ASSERT(lpItem != NULL);
  //process result
  switch (lpItem->cResult->nInternalAsmRegisterIndex)
  {
    case ENktDvMiscEnums::asmRegNone:
      *((SIZE_T*)(lpItem->cResult->lpAddress)) = lpItem->cResult->nOriginalStackValue;
      break;
  }
  //process parameters
  nCount = lpItem->cParameters->GetCount();
  for (i=0; i<nCount; i++)
  {
    cTempParam.Release();
    hRes = lpItem->cParameters->GetAt(i, &cTempParam);
    NKT_ASSERT(SUCCEEDED(hRes));
    //----
    switch (cTempParam->nInternalAsmRegisterIndex)
    {
      case ENktDvMiscEnums::asmRegNone:
        *((SIZE_T*)(cTempParam->lpAddress)) = cTempParam->nOriginalStackValue;
        break;
      case ENktDvMiscEnums::asmRegRcx:
        cTempParam->lpAddress = &(lpRegisters->sInteger.Rcx);
        break;
      case ENktDvMiscEnums::asmRegRdx:
        cTempParam->lpAddress = &(lpRegisters->sInteger.Rdx);
        break;
      case ENktDvMiscEnums::asmRegR8:
        cTempParam->lpAddress = &(lpRegisters->sInteger.R8);
        break;
      case ENktDvMiscEnums::asmRegR9:
        cTempParam->lpAddress = &(lpRegisters->sInteger.R9);
        break;
      case ENktDvMiscEnums::asmRegXmm0:
        cTempParam->lpAddress = &(lpRegisters->sFloating.Xmm0);
        break;
      case ENktDvMiscEnums::asmRegXmm1:
        cTempParam->lpAddress = &(lpRegisters->sFloating.Xmm1);
        break;
      case ENktDvMiscEnums::asmRegXmm2:
        cTempParam->lpAddress = &(lpRegisters->sFloating.Xmm2);
        break;
      case ENktDvMiscEnums::asmRegXmm3:
        cTempParam->lpAddress = &(lpRegisters->sFloating.Xmm3);
        break;
    }
    ////in the after-call parameters always points to backup'ed memory locations
    //cTempParam->nInternalAsmRegisterIndex = ENktDvMiscEnums::asmRegNone;
  }
  //done
  return;
}

#endif

CNktDvFunctionParamsCache::FUNCTION_PARAMS* CNktDvFunctionParamsCache::GetFuncParam()
{
  CNktAutoFastMutex cLock(this);
  TNktComPtr<FUNCTION_PARAMS> cNewItem;
  TNktComPtr<CNktDvParam> cParam, cParamCopy;
  SIZE_T i, nCount;

  cNewItem.Attach(NKT_MEMMGR_NEW FUNCTION_PARAMS);
  if (cNewItem == NULL)
    return NULL;
  cNewItem->cParameters.Attach(NKT_MEMMGR_NEW CNktDvParamsEnumerator);
  if (cNewItem->cParameters == NULL)
    return NULL;
  nCount = cBaseFuncParam->cParameters->GetCount();
  for (i=0; i<nCount; i++)
  {
    cParam.Release();
    if (FAILED(cBaseFuncParam->cParameters->GetAt(i, &cParam)))
      return NULL;
    cParamCopy.Attach(cParam->Clone());
    if (cParamCopy == NULL)
      return NULL;
    if (FAILED(cNewItem->cParameters->Add(cParamCopy)))
      return NULL;
  }
  cNewItem->cResult.Attach(cBaseFuncParam->cResult->Clone());
  if (cNewItem->cResult == NULL)
    return NULL;
  return cNewItem.Detach();
}
