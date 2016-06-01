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

class CNktDvParam;
class CNktDvParamsEnumerator;
#include "EngBaseObj.h"
#include "Database.h"
#include "ParamsEnum.h"
#include "ProcessMemory.h"
#include "TransportCommon.h"
#include "MiscEnums.h"

//-----------------------------------------------------------

class CNktDvParam : public TNktDvEngBaseObjWithLock<CNktFastMutex>
{
public:
  typedef enum {
    stNone=0, stAnsi, stWide
  } eStringType;

  CNktDvParam(__in SIZE_T nPlatformBits);
  virtual ~CNktDvParam();

  virtual LPVOID GetAddress() const;

  virtual CNktDvDbObjectNoRef::eClass GetClass() const;

  virtual CNktDvDbObjectNoRef::eFundamentalType GetBasicType() const;

  virtual LPWSTR GetName() const;
  virtual LPWSTR GetTypeName() const;
  virtual LPWSTR GetDeclaration() const;
  virtual int GetFlags() const;

  virtual BOOL IsPointer() const;
  virtual BOOL IsArray() const;
  virtual SIZE_T GetArrayMaxHint() const;

  virtual eStringType GetStringType() const;
  virtual BOOL IsAnsiString() const
    {
    return (GetStringType() == CNktDvParam::stAnsi) ? TRUE : FALSE;
    };
  virtual BOOL IsWideString() const
    {
    return (GetStringType() == CNktDvParam::stWide) ? TRUE : FALSE;
    };

  virtual HRESULT IsIntResourceString(__out unsigned short *lpnId=NULL) const; //ret: S_OK, S_FALSE or err

  virtual HRESULT IsGuidString(__inout CNktStringW &cStrDest) const;

  virtual HRESULT GetEvaluated(__deref_out CNktDvParam **lplpParam, __in SIZE_T nArrayIndex=0);
  virtual HRESULT GetFullEvaluated(__deref_out CNktDvParam **lplpParam);

  virtual SIZE_T GetFieldsCount() const;
  virtual HRESULT GetField(__deref_out CNktDvParam **lplpParam, __in SIZE_T nIndex);
  virtual HRESULT GetField(__deref_out CNktDvParam **lplpParam, __in LPCWSTR szFieldNameW);
  virtual HRESULT GetFields(__deref_out CNktDvParamsEnumerator **lplpParamEnum);

  virtual HRESULT GetSByte(__out char *lpValue, __in SIZE_T nIndex=0)
    {
    return GetByte((unsigned char*)lpValue, nIndex);
    };
  virtual HRESULT SetSByte(__in char nValue, __in SIZE_T nIndex=0)
    {
    return SetByte((unsigned char)nValue, nIndex);
    };
  virtual HRESULT GetByte(__out unsigned char *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetByte(__in unsigned char nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetSWord(__out short *lpValue, __in SIZE_T nIndex=0)
    {
    return GetWord((unsigned short*)lpValue, nIndex);
    };
  virtual HRESULT SetSWord(__in short nValue, __in SIZE_T nIndex=0)
    {
    return SetWord((unsigned short)nValue, nIndex);
    };
  virtual HRESULT GetWord(__out unsigned short *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetWord(__in unsigned short nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetSDWord(__out long *lpValue, __in SIZE_T nIndex=0)
    {
    return GetDWord((unsigned long*)lpValue, nIndex);
    };
  virtual HRESULT SetSDWord(__in long nValue, __in SIZE_T nIndex=0)
    {
    return SetDWord((unsigned long)nValue, nIndex);
    };
  virtual HRESULT GetDWord(__out unsigned long *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetDWord(__in unsigned long nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetSQWord(__out __int64 *lpValue, __in SIZE_T nIndex=0)
    {
    return GetQWord((unsigned __int64*)lpValue, nIndex);
    };
  virtual HRESULT SetSQWord(__in __int64 nValue, __in SIZE_T nIndex=0)
    {
    return SetQWord((unsigned __int64)nValue, nIndex);
    };
  virtual HRESULT GetQWord(__out unsigned __int64 *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetQWord(__in unsigned __int64 nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetFloat(__out float *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetFloat(__in float nValue, __in SIZE_T nIndex=0);
  virtual HRESULT GetDouble(__out double *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetDouble(__in double nValue, __in SIZE_T nIndex=0);
  virtual HRESULT GetLongDouble(__out long double *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetLongDouble(__in long double nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetAnsiString(__out LPSTR *lpszValue, __in SIZE_T nStart=0,
                                __in SIZE_T nMaxLength=NKT_SIZE_T_MAX);
  virtual HRESULT SetAnsiString(__in LPSTR szValue, __in SIZE_T nIndex=0, __in BOOL bIncludeEOS=TRUE);
  virtual HRESULT GetWideString(__out LPWSTR *lpszValue, __in SIZE_T nStart=0,
                                __in SIZE_T nMaxLength=NKT_SIZE_T_MAX);
  virtual HRESULT SetWideString(__in LPWSTR szValue, __in SIZE_T nIndex=0, __in BOOL bIncludeEOS=TRUE);
  virtual HRESULT GetAnsiStringLength(__out SIZE_T *lpnLength, SIZE_T nIndex=0);
  virtual HRESULT GetWideStringLength(__out SIZE_T *lpnLength, SIZE_T nIndex=0);
  virtual VOID FreeString(__inout LPVOID szString);

  virtual HRESULT GetSizeT(__out SIZE_T *lpValue, __in SIZE_T nIndex=0);
  virtual HRESULT SetSizeT(__in SIZE_T nValue, __in SIZE_T nIndex=0);

  virtual HRESULT GetPointer(__out LPVOID *lplpValue);
  virtual HRESULT SetPointer(__in LPVOID lpValue);
  virtual HRESULT IsNullPointer(__out LPBOOL lpbIsNull);

  virtual HRESULT GetProcessMemory(__deref_out CNktDvProcessMemory **lplpProcMem);

  virtual HRESULT CastTo(__deref_out CNktDvParam **lplpParam, __in_z LPCWSTR szNewTypeW);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

private:
  friend class CNktDvFunctionParamsCache;
  friend class CDvAgentMgr;
  friend class CNktDvSpyMgr;
  friend class CNktDvHookCallInfo;
  friend class CNktHookCallInfoPluginImpl;
  friend class CNktDvProcessMemory;

  typedef enum {
    eEnvUserDefined=0, eEnvSpyMgr, eEnvAgent
  } eEnvironment;

  class CCallStateContainer : public TNktDvEngBaseObjWithLock<CNktFastMutex>
  {
  public:
    CCallStateContainer(__in LPNKT_DV_CURRENT_CALL_STATE32 _lpCallState32) :
                                   TNktDvEngBaseObjWithLock<CNktFastMutex>(FALSE)
      {
      lpCallState32 = _lpCallState32;
      NKT_ASSERT(lpCallState32 != NULL);
      lpCallState64 = NULL;
      NktInterlockedExchange(&nValid, (lpCallState32 != NULL) ? 1 : 0);
      return;
      };

    CCallStateContainer(__in LPNKT_DV_CURRENT_CALL_STATE64 _lpCallState64) :
                                   TNktDvEngBaseObjWithLock<CNktFastMutex>(FALSE)
      {
      lpCallState64 = _lpCallState64;
      lpCallState32 = NULL;
      NktInterlockedExchange(&nValid, (lpCallState64 != NULL) ? 1 : 0);
      return;
      };

    virtual VOID Invalidate()
      {
      NktInterlockedExchange(&nValid, 0);
      return;
      };

    virtual BOOL IsValid() const
      {
      return (nValid != 0) ? TRUE : FALSE;
      };

    virtual LPNKT_DV_CURRENT_CALL_STATE32 Get32() const
      {
      return (nValid != 0) ? lpCallState32 : NULL;
      };

    virtual LPNKT_DV_CURRENT_CALL_STATE64 Get64() const
      {
      return (nValid != 0) ? lpCallState64 : NULL;
      };

    virtual LPVOID GetAddressForRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister)
      {
      if (nValid == 0)
        return NULL;
      if (lpCallState32 != NULL)
      {
        switch (nRegister)
        {
          case ENktDvMiscEnums::asmRegEax:
          case ENktDvMiscEnums::asmRegEaxEdxPair:
            return &(lpCallState32->sAsmRegisters.sInteger.Eax);
          case ENktDvMiscEnums::asmRegEbx:
            return &(lpCallState32->sAsmRegisters.sInteger.Ebx);
          case ENktDvMiscEnums::asmRegEcx:
            return &(lpCallState32->sAsmRegisters.sInteger.Ecx);
          case ENktDvMiscEnums::asmRegEdx:
            return &(lpCallState32->sAsmRegisters.sInteger.Edx);
          case ENktDvMiscEnums::asmRegEsp:
            return &(lpCallState32->sAsmRegisters.sInteger.Esp);
          case ENktDvMiscEnums::asmRegEbp:
            return &(lpCallState32->sAsmRegisters.sInteger.Ebp);
          case ENktDvMiscEnums::asmRegEip:
            return &(lpCallState32->sAsmRegisters.sInteger.Eip);
          case ENktDvMiscEnums::asmRegEsi:
            return &(lpCallState32->sAsmRegisters.sInteger.Esi);
          case ENktDvMiscEnums::asmRegEdi:
            return &(lpCallState32->sAsmRegisters.sInteger.Edi);
          case ENktDvMiscEnums::asmRegSt0:
            return &(lpCallState32->sAsmRegisters.sFloating.St0);

        }
      }
      else if (lpCallState64 != NULL)
      {
        switch (nRegister)
        {
          case ENktDvMiscEnums::asmRegRax:
            return &(lpCallState64->sAsmRegisters.sInteger.Rax);
          case ENktDvMiscEnums::asmRegRbx:
            return &(lpCallState64->sAsmRegisters.sInteger.Rbx);
          case ENktDvMiscEnums::asmRegRcx:
            return &(lpCallState64->sAsmRegisters.sInteger.Rcx);
          case ENktDvMiscEnums::asmRegRdx:
            return &(lpCallState64->sAsmRegisters.sInteger.Rdx);
          case ENktDvMiscEnums::asmRegR8:
            return &(lpCallState64->sAsmRegisters.sInteger.R8);
          case ENktDvMiscEnums::asmRegR9:
            return &(lpCallState64->sAsmRegisters.sInteger.R9);
          case ENktDvMiscEnums::asmRegR10:
            return &(lpCallState64->sAsmRegisters.sInteger.R10);
          case ENktDvMiscEnums::asmRegR11:
            return &(lpCallState64->sAsmRegisters.sInteger.R11);
          case ENktDvMiscEnums::asmRegRsp:
            return &(lpCallState64->sAsmRegisters.sInteger.Rsp);
          case ENktDvMiscEnums::asmRegRbp:
            return &(lpCallState64->sAsmRegisters.sInteger.Rbp);
          case ENktDvMiscEnums::asmRegRip:
            return &(lpCallState64->sAsmRegisters.sInteger.Rip);
          case ENktDvMiscEnums::asmRegRsi:
            return &(lpCallState64->sAsmRegisters.sInteger.Rsi);
          case ENktDvMiscEnums::asmRegRdi:
            return &(lpCallState64->sAsmRegisters.sInteger.Rdi);
          case ENktDvMiscEnums::asmRegXmm0:
            return &(lpCallState64->sAsmRegisters.sFloating.Xmm0);
          case ENktDvMiscEnums::asmRegXmm1:
            return &(lpCallState64->sAsmRegisters.sFloating.Xmm1);
          case ENktDvMiscEnums::asmRegXmm2:
            return &(lpCallState64->sAsmRegisters.sFloating.Xmm2);
          case ENktDvMiscEnums::asmRegXmm3:
            return &(lpCallState64->sAsmRegisters.sFloating.Xmm3);
        }
      }
      return NULL;
      };

    virtual BOOL IsFloatingRegister(__in ENktDvMiscEnums::eNktDvRegister nRegister) const
      {
      switch (nRegister)
      {
        case ENktDvMiscEnums::asmRegSt0:
        case ENktDvMiscEnums::asmRegXmm0:
        case ENktDvMiscEnums::asmRegXmm1:
        case ENktDvMiscEnums::asmRegXmm2:
        case ENktDvMiscEnums::asmRegXmm3:
          return TRUE;
      }
      return FALSE;
      };

  private:
    LPNKT_DV_CURRENT_CALL_STATE32 lpCallState32;
    LPNKT_DV_CURRENT_CALL_STATE64 lpCallState64;
    LONG volatile nValid;
  };

  virtual VOID SpyMgr_Init(__in ENktDvMiscEnums::eNktDvRegister nAsmRegisterIndex, __in LPVOID lpAddress,
                           __in CCallStateContainer *lpCallStateCont);
  virtual HRESULT SpyMgr_Init4Custom(__in CNktDvProcessMemory *lpProcMem, __in int nFundType,
                                     __in LPWSTR szNameW, __in SIZE_T nNameLen, __in LPVOID lpSrc,
                                     __in SIZE_T nSrcSize, __in CCallStateContainer *lpCallStateCont);
  virtual VOID Agent_Init();
  virtual HRESULT Agent_Init4Custom(__in CNktDvProcessMemory *lpProcMem, __in int nFundType,
                                    __in LPCWSTR szNameW, __in LPVOID lpSrc, __in SIZE_T nSrcSize);

  virtual HRESULT User_Init(__in CNktDvProcessMemory *lpProcMem, __in LPVOID lpAddress,
                            __in_z LPCWSTR szTypeW);

  virtual VOID Init2();

  static BOOL AreFundamentalTypeCompatible(__in CNktDvDbObjectNoRef::eFundamentalType nDestFundType,
                                           __in CNktDvDbObjectNoRef::eFundamentalType nSrcFundType);

  virtual HRESULT GetValueCommon(__out LPVOID lpValue, __in SIZE_T nIndex,
                                 __in CNktDvDbObjectNoRef::eFundamentalType nDestFundType);
  virtual HRESULT SetValueCommon(__in LPVOID lpValue, __in SIZE_T nIndex,
                                 __in CNktDvDbObjectNoRef::eFundamentalType nSrcFundType);

  virtual HRESULT GetStringCommon(__out LPVOID *lpszValue, __out SIZE_T *lpnLength, __in SIZE_T nStart,
                                  __in SIZE_T nMaxLength, __in BOOL bIsWide);
  virtual HRESULT SetStringCommon(__in LPVOID szValue, __in SIZE_T nStart, __in BOOL bIncludeEOS,
                                  __in BOOL bIsWide);

  virtual HRESULT ReadMemoryBytes(__out LPVOID lpDest, __in LPCVOID lpSrc, __in SIZE_T nCount,
                                  __in BOOL bForceLocal);
  virtual HRESULT WriteMemoryBytes(__out LPVOID lpDest, __in LPCVOID lpSrc, __in SIZE_T nCount);

  virtual HRESULT ReadPointer(__out LPBYTE *lplpDest, __in LPVOID lpAddress);

  virtual CNktDvParam* Clone();

  LPWSTR szNameW;     //this are only pointers to database strings
  LPWSTR szDeclW;     //don't free them, except szNameW that may
  LPWSTR szTypeNameW; //point to sCustomValue.cStrNameW on custom params
  int nFlags;
  LPVOID lpAddress;
  SIZE_T nOriginalStackValue;
  ENktDvMiscEnums::eNktDvRegister nInternalAsmRegisterIndex; //for internal use when assigning addresses
  BOOL bReadOnly;
  eEnvironment nEnvironment;
  CNktDvDbObjectNoRef *lpDbObj, *lpDbParentObj;
  TNktComPtr<CNktDvProcessMemory> cProcMem;
  struct tagCustomValue {
    CNktDvDbObjectNoRef::eFundamentalType nFundType;
    CNktStringW cStrNameW;
    TNktAutoFreePtr<BYTE> cLongValue;
    BYTE aShortValue[16];
    SIZE_T nSize;
  } sCustomValue;
  struct {
    SIZE_T nSize;
    SIZE_T nShiftLeft;
    ULONGLONG nMask;
  } sFundamentalExtra;
  //----
  TNktComPtr<CCallStateContainer> cCallStateCont;
  SIZE_T nPlatformBits;
};

//-----------------------------------------------------------

