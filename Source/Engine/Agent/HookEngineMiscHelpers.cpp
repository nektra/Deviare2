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
#include "HookEngineMiscHelpers.h"
#include "..\..\Common\ModulesEnum.h"
#include "..\..\SupportLibs\UDis86\GetInstructionLength.h"

//-----------------------------------------------------------

static SIZE_T ProcessCALLs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);
static SIZE_T ProcessMOVs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);
static SIZE_T ProcessLEAs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);
static SIZE_T ProcessSPECIAL1s(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest);
static SIZE_T ProcessSPECIAL2s(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest);
static SIZE_T ProcessCMPXCHGandXADDs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                                     __out LPBYTE lpDest);
static SIZE_T ProcessPUSHandPOPs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                                 __out LPBYTE lpDest);
static SIZE_T ProcessJUMPs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);

//-----------------------------------------------------------

LPBYTE HookEng_SkipJumpInstructions(__in LPBYTE lpPtr)
{
  LONG nOfs;
#if defined _M_IX86
  LPBYTE d;
#endif

  try
  {
    for (;;)
    {
      if (lpPtr[0] == 0xE9)
      {
        nktMemCopy(&nOfs, lpPtr+1, sizeof(LONG));
        lpPtr += nOfs+5;
      }
      else if (lpPtr[0] == 0xEB)
      {
        nOfs = (LONG)*((signed char*)(lpPtr+1));
        lpPtr += nOfs+2;
      }
      else if ((lpPtr[0]==0xFF && lpPtr[1]==0x25) ||
               (lpPtr[0]==0x48 && lpPtr[1]==0xFF && lpPtr[2]==0x25))
      {
        if (lpPtr[0] == 0x48)
          lpPtr++;
#if defined _M_IX86
        d = (LPBYTE)HookEng_ReadUnalignedULong(lpPtr+2);
        lpPtr = (LPBYTE)HookEng_ReadUnalignedULong(d);
#elif defined _M_X64
        nOfs = (LONG)HookEng_ReadUnalignedULong(lpPtr+2);
        lpPtr = (LPBYTE)HookEng_ReadUnalignedULongLong(lpPtr+6+(LONGLONG)nOfs);
#else
  #error Unsupported platform
#endif
      }
      else
        break;
    }
  }
  catch (...)
  { }
  return lpPtr;
}

HRESULT HookEng_CreateNewStub(__inout CHookEntry *lpEntry, __in LPVOID lpProcToHook, __in BOOL bSkipJumps)
{
  return HookEng_CreateNewStub2(lpEntry->aOriginalStub, lpEntry->nOriginalStubSize, lpEntry->aNewStub,
                                lpEntry->nNewStubSize, lpProcToHook, bSkipJumps);
}

HRESULT HookEng_CreateNewStub2(__inout BYTE aOriginalStub[HOOKENG_MAX_ORIGINAL_STUB_SIZE],
                               __inout SIZE_T& nOriginalStubSize, __inout BYTE aNewStub[HOOKENG_MAX_STUB_SIZE],
                               __inout SIZE_T& nNewStubSize, __in LPVOID lpProcToHook, __in BOOL bSkipJumps)
{
  SIZE_T k, nSrcInstrLen, nDestInstrLen, nNextSrcIP;
  LPBYTE lpSrc, lpDest, s[2];
  BOOL b, bIsMemOp;

  nktMemSet(aOriginalStub, 0, HOOKENG_MAX_ORIGINAL_STUB_SIZE);
  nktMemSet(aNewStub, 0, HOOKENG_MAX_STUB_SIZE);
  for (k=HOOKENG_MAX_ORIGINAL_STUB_SIZE; k>0; k--)
  {
    try
    {
      nktMemCopy(aOriginalStub, lpProcToHook, k);
      b = TRUE;
    }
    catch (...)
    {
      b = FALSE;
    }
    if (b != FALSE)
      break;
  }
  if (k == 0)
    return E_FAIL;
  //calculate stub size and create the new one
  nOriginalStubSize = nNewStubSize = 0;
  while (nOriginalStubSize < HOOKENG_JUMP_TO_HOOK_SIZE)
  {
    lpSrc = aOriginalStub + nOriginalStubSize;
    lpDest = aNewStub + nNewStubSize;
    nSrcInstrLen = GetInstructionLength(lpSrc, HOOKENG_MAX_ORIGINAL_STUB_SIZE-nOriginalStubSize, sizeof(SIZE_T)<<3,
                                        &bIsMemOp);
    if (nSrcInstrLen == 0) //invalid opcode
      return E_FAIL;
    nDestInstrLen = nSrcInstrLen;
    //check special opcodes
    s[0] = (LPBYTE)lpProcToHook + nOriginalStubSize;
    s[1] = (bSkipJumps == FALSE) ? s[0] : HookEng_SkipJumpInstructions(s[0]);
    if (s[1] != s[0])
    {
      //we hit a jump in the middle of the stuff so we encode a jump
      //to the final position ...
#if defined _M_IX86
      //...convert to PUSH imm32/RET
      lpDest[0] = 0x68;
      HookEng_WriteUnalignedULong(lpDest+1, (ULONG)(s[1]));
      lpDest[5] = 0xC3; //ret
      nDestInstrLen = 6;
#elif defined _M_X64
      //jmp to original call (JMP QWORD PTR [RIP+0h])
      lpDest[0] = 0xFF;  lpDest[1] = 0x25; //
      HookEng_WriteUnalignedULong(lpDest+2, 0);
      HookEng_WriteUnalignedSizeT(lpDest+6, (ULONGLONG)(s[1]));
      nDestInstrLen = 14;
#endif
      //...and finalize the loop
      if (nOriginalStubSize + nSrcInstrLen < HOOKENG_JUMP_TO_HOOK_SIZE)
        nSrcInstrLen = HOOKENG_JUMP_TO_HOOK_SIZE - nOriginalStubSize;
    }
    else
    {
      nNextSrcIP = (SIZE_T)lpProcToHook+nOriginalStubSize+nSrcInstrLen;
      nDestInstrLen = ProcessCALLs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessMOVs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessLEAs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessSPECIAL1s(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessSPECIAL2s(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessCMPXCHGandXADDs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessPUSHandPOPs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessJUMPs(lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
      {
        nktMemCopy(lpDest, lpSrc, nSrcInstrLen);
        nDestInstrLen = nSrcInstrLen;
      }
    }
    nOriginalStubSize += nSrcInstrLen;
    nNewStubSize += nDestInstrLen;
  }
  return S_OK;
}

HRESULT HookEng_FindDll(__out HINSTANCE *lphDll, __in LPVOID lpAddress)
{
  TNktArrayList<HMODULE> aModulesList;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, nCount;
  DWORD dwProcessId;
  HANDLE hProcess;
  HRESULT hRes;

  NKT_ASSERT(lphDll != NULL);
  *lphDll = NULL;
  dwProcessId = ::GetCurrentProcessId();
  hProcess = ::GetCurrentProcess();
  //try to find the module containing the address
  hRes = CNktDvTools::GetModules(hProcess, sizeof(SIZE_T)<<3, aModulesList);
  if (FAILED(hRes))
    return hRes;
  nCount = aModulesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProcess, aModulesList[i], sModInfo);
    if (hRes == E_FAIL)
      continue;
    if (FAILED(hRes))
      return hRes;
    if ((LPBYTE)lpAddress >= sModInfo.lpBaseAddr &&
        (LPBYTE)lpAddress < sModInfo.lpBaseAddr + sModInfo.nBaseSize)
    {
      *lphDll = (HINSTANCE)aModulesList[i];
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

HRESULT HookEng_FindDllAndIncrementUsageCount(__out HINSTANCE *lphDll, __in LPVOID lpAddress)
{
  TNktArrayList<HMODULE> aModulesList;
  CNktDvTools::MODULE_INFO sModInfo;
  SIZE_T i, nCount;
  DWORD dwProcessId;
  HANDLE hProcess;
  HRESULT hRes;

  NKT_ASSERT(lphDll != NULL);
  *lphDll = NULL;
  dwProcessId = ::GetCurrentProcessId();
  hProcess = ::GetCurrentProcess();
  //try to find the module containing the address
  hRes = CNktDvTools::GetModules(hProcess, sizeof(SIZE_T)<<3, aModulesList);
  if (FAILED(hRes))
    return hRes;
  nCount = aModulesList.GetCount();
  for (i=0; i<nCount; i++)
  {
    hRes = CNktDvTools::GetModuleInfo(dwProcessId, hProcess, aModulesList[i], sModInfo);
    if (hRes == E_FAIL)
      continue;
    if (FAILED(hRes))
      return hRes;
    if ((LPBYTE)lpAddress >= sModInfo.lpBaseAddr &&
        (LPBYTE)lpAddress < sModInfo.lpBaseAddr + sModInfo.nBaseSize)
    {
      *lphDll = ::LoadLibraryW((LPWSTR)sModInfo.cStrExePathW);
      return S_OK;
    }
  }
  return NKT_DVERR_NotFound;
}

//-----------------------------------------------------------

static SIZE_T ProcessCALLs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
  ULONG ulTemp;
  SIZE_T k;

  switch (nSrcInstrLen)
  {
    case 5:
      if (lpSrc[0] == 0xE8)
      {
        //convert "relative 32-bit CALL" (CALL [mofs32]) [E8 xxyyzzww] into...
        //..."relative call to next instruction" (CALL [0h]) [E8 00000000]
        lpDest[0] = 0xE8;
        HookEng_WriteUnalignedULong(lpDest+1, 0);
#if defined _M_IX86
        //...increment return address (ADD DWORD PTR [esp], 4+5+1)
        lpDest[5] = 0x83;
        lpDest[6] = 0x04;
        lpDest[7] = 0x24;
        lpDest[8] = 4 + 5 + 1;
        //jmp to original call (PUSH imm32/RET)
        lpDest[9] = 0x68;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+1);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULong(lpDest+10, k);
        lpDest[14] = 0xC3; //ret
        return 15;
#elif defined _M_X64
        //...increment return address (ADD QWORD PTR [rsp], 5+14)
        lpDest[5] = 0x48;
        lpDest[6] = 0x83;
        lpDest[7] = 0x04;
        lpDest[8] = 0x24;
        lpDest[9] = 5 + 14;
        //jmp to original call (JMP QWORD PTR [RIP+0h])
        lpDest[10] = 0xFF;  lpDest[11] = 0x25;
        HookEng_WriteUnalignedULong(lpDest+12, 0);
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+1);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedSizeT(lpDest+16, k);
        return 24;
#endif
      }
      break;

    case 6:
#if defined _M_X64
      if (lpSrc[0] == 0xFF && lpSrc[1] == 0x15)
      {
        //convert "indirect 32-bit CALL" (CALL [mofs32]) [FF 15 xxyyzzww] into...
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
pj_setupfarcall:
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        //... PUSH RAX
        lpDest[0] = 0x50;
        //... MOV RAX, nDest
        lpDest[1] = 0x48;  lpDest[2] = 0xB8;
        HookEng_WriteUnalignedULongLong(lpDest+3, (ULONGLONG)k);
        //... MOV RAX, QWORD PTR [RAX]
        lpDest[11] = 0x48;  lpDest[12] = 0x8B;
        lpDest[13] = 0x00;
        //... MOV QWORD PTR [RSP-8], RAX
        lpDest[14] = 0x48;  lpDest[15] = 0x89;
        lpDest[16] = 0x44;  lpDest[17] = 0x24;
        lpDest[18] = 0xF8;
        //... POP RAX
        lpDest[19] = 0x58;
        //... CALL QWORD PTR [RSP-10h]
        lpDest[20] = 0xFF;  lpDest[21] = 0x54;
        lpDest[22] = 0x24;  lpDest[23] = 0xF0;
        return 24;
      }
#endif //_M_X64
      break;

    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0xFF && lpSrc[1] == 0x15)
      {
        //convert "indirect 64-bit CALL" (CALL [mofs64]) [48 FF 15 xxyyzzww] into...
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        goto pj_setupfarcall;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessMOVs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  BYTE nReg;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined _M_X64
      if (lpSrc[0] >= 0x88 && lpSrc[0] <= 0x8B && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert MOV [mofs32], r8 into... (88h)
        //        MOV [mofs32], r32 into... (89h)
        //        MOV r8, [mofs32] into... (8Ah)
        //        MOV r32, [mofs32] into... (8Bh)
        nReg = ((lpSrc[1] & 0x38) == 0) ? 3 : 0; //fix by tkoecker
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV [rax/rbx], r8; (88h)
        //...MOV [rax/rbx], r32; (89h)
        //...MOV r8, [rax/rbx]; (8Ah)
        //...MOV r32, [rax/rbx]; (8Bh)
        lpDest[11] = lpSrc[0];
        lpDest[12] = (lpSrc[1] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[13] = 0x58+nReg;
        return 14;
      }
#endif
      break;

    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0xC6 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert MOV BYTE PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV byte ptr [rax], imm8
        lpDest[11] = 0xC6;
        lpDest[12] = 0x00;
        lpDest[13] = lpSrc[6];
        //...POP eax
        lpDest[14] = 0x58;
        return 15;
      }
      //----
      if (lpSrc[0] == 0x66 && (lpSrc[1] == 0x89 || lpSrc[1] == 0x8B) && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert MOV [mofs32], r16 into... (89h)
        //        MOV r16, [mofs32] into... (8Bh)
        nReg = ((lpSrc[2] & 0x38) == 0) ? 3 : 0; //fix by tkoecker
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV [rax/rbx], r16; (89h)
        //...MOV r16, [rax/rbx]; (8Bh)
        lpDest[11] = lpSrc[0];
        lpDest[12] = lpSrc[1];
        lpDest[13] = (lpSrc[2] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[14] = 0x58+nReg;
        return 15;
      }
      //----
      if ((lpSrc[0] & 0xFB) == 0x48 && lpSrc[1] == 0x8B && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert MOV r64, [mofs32] into...
        nReg = (lpSrc[2] & 0x38);
        //...MOV r64, imm64
        lpDest[0] = 0x48 | ((lpSrc[0] & 0x04) >> 2); //REX bit
        lpDest[1] = 0xB8 | (nReg >> 3);
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+2, k);
        //...MOV r64, [r64]
        lpDest[10] = ((lpSrc[0] & 0x04) == 0) ? 0x48 : 0x4D; //REX bit
        lpDest[11] = lpSrc[1];
        lpDest[12] = nReg | (nReg >> 3);
        return 13;
      }
      if ((lpSrc[0] & 0xFB) == 0x48 && lpSrc[1] == 0x89 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert "relative 32-bit MOV" (MOV [mofs32], r64) [48/4C 89 00###101 xxyyzzww] into...
        //... save rax (PUSH rax)
        lpDest[0] = 0x50;
        //... "copy register to rax" [48/49 8B C1 11000###] (MOV rax, ###)
        lpDest[1] = 0x48 + ((lpSrc[0] & 0x04) >> 2); //REX bit
        lpDest[2] = 0x89;
        lpDest[3] = 0xC0 + ((lpSrc[2] & 0x38) >> 3);
        //..."inmmediate 64-bit MOV" (MOV [off64], rax) [48 A3 xxyyzzwwxxyyzzww]
        lpDest[4] = 0x48; //REX bit
        lpDest[5] = 0xA3;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+6, k);
        //... restore rax (POP rax)
        lpDest[14] = 0x58;
        return 15;
      }
#endif
      break;

    case 8:
#if defined _M_X64
      if ((lpSrc[0] == 0x66 || lpSrc[0] == 0xF3) && lpSrc[1] == 0x0F && lpSrc[2] == 0x6F &&
          (lpSrc[3] & 0xC7) == 0x05)
      {
        //convert "relative 32-bit MOVDQ(U|A)" (MOVDQU xmm0-7, xmmword ptr [mofs32])
        //   [66/F3 (44) 0F 6F 00###101 xxyyzzww] into...
        //... save rax (PUSH rax)
        lpDest[0] = 0x50;
        //..."inmmediate 64-bit MOV" (MOV rax, imm64) [48 B8 xxyyzzwwxxyyzzww]
        lpDest[1] = 0x48; //REX bit
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+4);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...and "relative 128-bit MOV" (MOVDQU xmm0-7, [rax]) [F3 0F 6F 00###000]
        lpDest[11] = 0xF3;  lpDest[12] = 0x0F;
        lpDest[13] = 0x6F;  lpDest[14] = (lpSrc[3] & 0x38);
        //... restore rax (POP rax)
        lpDest[15] = 0x58;
        return 16;
      }
#endif
      break;

    case 9:
#if defined _M_X64
      if (lpSrc[0] == 0x66 && lpSrc[1] == 0xC7 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert MOV WORD PTR [mofs32], imm16 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV word ptr [rax], imm16
        lpDest[11] = 0x66;
        lpDest[12] = 0xC7;
        lpDest[13] = 0x00;
        lpDest[14] = lpSrc[7];
        lpDest[15] = lpSrc[8];
        //...POP eax
        lpDest[16] = 0x58;
        return 17;
      }
      //----
      if ((lpSrc[0] == 0x66 || lpSrc[0] == 0xF3) && lpSrc[1] == 0x44 && lpSrc[2] == 0x0F &&
          lpSrc[3] == 0x6F && (lpSrc[4] & 0xC7) == 0x05)
      {
        //convert EXTENDED "relative 32-bit MOVDQ(U|A)" (MOVDQU xmm#, xmmword ptr [mofs32])
        //    [66/F3 (44) 0F 6F 00###101 xxyyzzww] into...
        //... save rax (PUSH rax)
        lpDest[0] = 0x50;
        //..."inmmediate 64-bit MOV" (MOV rax, imm64) [48 B8 xxyyzzwwxxyyzzww]
        lpDest[1] = 0x48; //REX bit
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+4);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...and "relative 128-bit MOV" (MOVDQU xmm0-7, [rax]) [F3 44 0F 6F 00###000]
        lpDest[11] = 0xF3;  lpDest[12] = 0x44;  lpDest[13] = 0x0F;
        lpDest[14] = 0x6F;  lpDest[15] = (lpSrc[4] & 0x38);
        //... restore rax (POP rax)
        lpDest[16] = 0x58;
        return 17;
      }
#endif
      break;

    case 10:
#if defined _M_X64
      if (lpSrc[0] == 0xC7 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert MOV DWORD PTR [mofs32], imm32 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV qword ptr [rax], imm32
        lpDest[11] = 0xC7;
        lpDest[12] = 0x00;
        lpDest[13] = lpSrc[6];
        lpDest[14] = lpSrc[7];
        lpDest[15] = lpSrc[8];
        lpDest[16] = lpSrc[9];
        //...POP eax
        lpDest[17] = 0x58;
        return 18;
      }
#endif
      break;

    case 11:
#if defined _M_X64
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0xC7 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert MOV QWORD PTR [mofs32], imm32 into....
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...MOV word ptr [rax], imm16
        lpDest[11] = 0x48;
        lpDest[12] = 0xC7;
        lpDest[13] = 0x00;
        lpDest[14] = lpSrc[7];
        lpDest[15] = lpSrc[8];
        lpDest[16] = lpSrc[9];
        lpDest[17] = lpSrc[10];
        //...POP eax
        lpDest[18] = 0x58;
        return 19;
      }
#endif
      break;
  }
  return 0;
}

static SIZE_T ProcessLEAs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined _M_X64
      if (lpSrc[0] == 0x8D && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert LEA r32, [mofs32] into...
        //...MOV r32, imm32
        lpDest[0] = 0xB8 + ((lpSrc[1] & 0x38) >> 3);
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULong(lpDest+1, (ULONG)k); //on x64 store only the lower bits
        return 5;
      }
#endif
      break;

    case 7:
#if defined _M_X64
      if ((lpSrc[0] & 0xFB) == 0x48 && lpSrc[1] == 0x8D && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert LEA r64, [mofs32] into...
        //...MOV r64, imm64
        lpDest[0] = 0x48 + ((lpSrc[0] & 0x04) >> 2); //REX bit
        lpDest[1] = 0xB8 + ((lpSrc[2] & 0x38) >> 3);
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+2, k);
        return 10;
      }
#endif
      break;
  }
  return 0;
}

static SIZE_T ProcessSPECIAL1s(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  SIZE_T k;
#endif
  SIZE_T nLockPrefix;

  if (lpSrc[0] == 0xF0)
  {
    nLockPrefix = 1;
    lpSrc++;
    nSrcInstrLen--;
  }
  else
  {
    nLockPrefix = 0;
  }
  switch (nSrcInstrLen)
  {
    case 6:
#if defined _M_X64
      if ((lpSrc[0] == 0xFF && (lpSrc[1] == 0x0D || lpSrc[1] == 0x05)) ||
          (lpSrc[0] == 0xF7 && (lpSrc[1] == 0x1D || lpSrc[1] == 0x15)))
      {
        //convert DEC/INC/NEG/NOT [mofs32] into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] DEC/INC/NEG/NOT [rax]
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = (lpSrc[1] & 0xF8);
        //...POP rax
        lpDest[13+nLockPrefix] = 0x58;
        return 14+nLockPrefix;
      }
#endif
      break;

    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0x48 &&
          ((lpSrc[1] == 0xFF && (lpSrc[2] == 0x0D || lpSrc[2] == 0x05)) ||
           (lpSrc[1] == 0xF7 && (lpSrc[2] == 0x1D || lpSrc[2] == 0x15))))
      {
        //convert DEC/INC/NEG/NOT [mofs32] into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] DEC/INC/NEG/NOT [rax]
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = 0x48;
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0xF8);
        //...POP rax
        lpDest[14+nLockPrefix] = 0x58;
        return 15+nLockPrefix;
      }
#endif
      break;
  }
  return 0;
}

static SIZE_T ProcessSPECIAL2s(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  SIZE_T k;
  BYTE nReg;
#endif
  SIZE_T nLockPrefix;

  if (lpSrc[0] == 0xF0)
  {
    nLockPrefix = 1;
    lpSrc++;
    nSrcInstrLen--;
  }
  else
  {
    nLockPrefix = 0;
  }
  switch(nSrcInstrLen)
  {
    case 6:
#if defined _M_X64
      if ((lpSrc[0] & 0xC4) == 0x00 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [mofs32], r8 or
        //        ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR r8, [mofs32] or
        //        ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], r32 or
        //        ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR r32, [mofs32] into...
        nReg = ((lpSrc[1] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax/rbx], r8 or
        //   [lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP r8, BYTE PTR [rax/rbx] or
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [rax/rbx], r32 or
        //   [lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP r32, DWORD PTR [rax/rbx]
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = (lpSrc[1] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[13+nLockPrefix] = 0x58+nReg;
        return 14+nLockPrefix;
      }
#endif
      break;

    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0x80 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax], imm8
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = (lpSrc[1] & 0xF8);
        lpDest[13+nLockPrefix] = lpSrc[6];
        //...POP rax
        lpDest[14+nLockPrefix] = 0x58;
        return 15+nLockPrefix;
      }
#endif
      //----
#if defined _M_X64
      if (lpSrc[0] == 0x83 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax], imm8
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = (lpSrc[1] & 0xF8);
        lpDest[13+nLockPrefix] = lpSrc[6];
        //...POP rax
        lpDest[14+nLockPrefix] = 0x58;
        return 15+nLockPrefix;
      }
#endif
      //----
#if defined _M_X64
      if (lpSrc[0] & 0x66 && (lpSrc[1] & 0xC4) == 0x00 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [mofs32], r16 or
        //        ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR r16, [mofs32] into...
        nReg = ((lpSrc[2] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [rax/rbx], r16 or
        //   [lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP r16, WORD PTR [rax/rbx]
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[14+nLockPrefix] = 0x58+nReg;
        return 15+nLockPrefix;
      }
#endif
      //----
#if defined _M_X64
      if (lpSrc[0] & 0x48 && (lpSrc[1] & 0xC4) == 0x00 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP QWORD PTR [mofs32], r64 or
        //        ADD/SUB/ADC/SBB/AND/OR/XOR/CMP QWORD PTR r64, [mofs32] into...
        nReg = ((lpSrc[2] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP QWORD PTR [rax/rbx], r64 or
        //   [lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP r64, QWORD PTR [rax/rbx]
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[14+nLockPrefix] = 0x58+nReg;
        return 15+nLockPrefix;
      }
#endif //_M_X64
      break;

    case 8:
#if defined _M_X64
      if (lpSrc[0] == 0x66 && lpSrc[1] == 0x83 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [rax], imm8
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0xF8);
        lpDest[14+nLockPrefix] = lpSrc[7];
        //...POP rax
        lpDest[15+nLockPrefix] = 0x58;
        return 16+nLockPrefix;
      }
      //----
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0x80 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax], imm8
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[1];
        lpDest[12+nLockPrefix] = (lpSrc[2] & 0xF8);
        lpDest[13+nLockPrefix] = lpSrc[7];
        //...POP rax
        lpDest[14+nLockPrefix] = 0x58;
        return 15+nLockPrefix;
      }
      //----
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0x83 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], imm8 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax], imm8
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[1];
        lpDest[12+nLockPrefix] = (lpSrc[2] & 0xF8);
        lpDest[13+nLockPrefix] = lpSrc[7];
        //...POP rax
        lpDest[14+nLockPrefix] = 0x58;
        return 15+nLockPrefix;
      }
#endif
      break;

    case 9:
#if defined _M_X64
      if (lpSrc[0] == 0x66 && lpSrc[1] == 0x81 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [mofs32], imm16 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [rax], imm16
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0xF8);
        lpDest[14+nLockPrefix] = lpSrc[7];
        lpDest[15+nLockPrefix] = lpSrc[8];
        //...POP rax
        lpDest[16+nLockPrefix] = 0x58;
        return 17+nLockPrefix;
      }
#endif
      break;

    case 10:
#if defined _M_X64
      if (lpSrc[0] == 0x81 && (lpSrc[1] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], imm32 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [rax], imm32
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = (lpSrc[1] & 0xF8);
        lpDest[13+nLockPrefix] = lpSrc[6];
        lpDest[14+nLockPrefix] = lpSrc[7];
        lpDest[15+nLockPrefix] = lpSrc[8];
        lpDest[16+nLockPrefix] = lpSrc[9];
        //...POP rax
        lpDest[17+nLockPrefix] = 0x58;
        return 18+nLockPrefix;
      }
#endif
      break;

    case 11:
#if defined _M_X64
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0x81 && (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP QWORD PTR [mofs32], imm32 into...
        //...PUSH rax
        lpDest[0] = 0x50;
        //...MOV rax, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] ADD/SUB/ADC/SBB/AND/OR/XOR WORD PTR [rax], imm16
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0xF8);
        lpDest[14+nLockPrefix] = lpSrc[7];
        lpDest[15+nLockPrefix] = lpSrc[8];
        lpDest[16+nLockPrefix] = lpSrc[9];
        lpDest[17+nLockPrefix] = lpSrc[10];
        //...POP rax
        lpDest[18+nLockPrefix] = 0x58;
        return 19+nLockPrefix;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessCMPXCHGandXADDs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                                     __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  SIZE_T k;
  BYTE nReg;
#endif
  SIZE_T nLockPrefix;

  if (lpSrc[0] == 0xF0)
  {
    nLockPrefix = 1;
    lpSrc++;
    nSrcInstrLen--;
  }
  else
  {
    nLockPrefix = 0;
  }
  switch (nSrcInstrLen)
  {
    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0x0F &&
          ((lpSrc[1] & 0xFE) == 0xB0 || (lpSrc[1] & 0xFE) == 0xC0) &&
          (lpSrc[2] & 0xC7) == 0x05)
      {
        //convert CMPXCHG BYTE PTR [mofs32], r8 or
        //        CMPXCHG DWORD PTR [mofs32], r32 or
        //        XADD BYTE PTR [mofs32], r8 or
        //        XADD DWORD PTR [mofs32], r32 into...
        nReg = ((lpSrc[2] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] CMPXCHG BYTE PTR [rax/rbx], r8 or
        //...[lock] CMPXCHG DWORD PTR [rax/rbx], r32
        //...[lock] XADD BYTE PTR [rax/rbx], r8 or
        //...[lock] XADD DWORD PTR [rax/rbx], r32
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = (lpSrc[2] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[14+nLockPrefix] = 0x58+nReg;
        return 15+nLockPrefix;
      }
#endif
      break;

    case 8:
#if defined _M_X64
      if (lpSrc[0] == 0x66 && lpSrc[1] == 0x0F &&
          (lpSrc[2] == 0xB1 || lpSrc[2] == 0xC1) &&
          (lpSrc[3] & 0xC7) == 0x05)
      {
        //convert CMPXCHG WORD PTR [mofs32], r16 or
        //        XADD WORD PTR [mofs32], r16 into...
        nReg = ((lpSrc[3] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+4);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] CMPXCHG WORD PTR [rax/rbx], r16 or
        //...[lock] XADD WORD PTR [rax/rbx], r16
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = lpSrc[2];
        lpDest[14+nLockPrefix] = (lpSrc[3] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[15+nLockPrefix] = 0x58+nReg;
        return 16+nLockPrefix;
      }
#endif
      //----
#if defined _M_X64
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0x0F &&
          (lpSrc[2] == 0xB1 || lpSrc[2] == 0xC1) &&
          (lpSrc[3] & 0xC7) == 0x05)
      {
        //convert CMPXCHG QWORD PTR [mofs32], r64 or
        //        XADD QWORD PTR [mofs32], r64 into...
        nReg = ((lpSrc[3] & 0x38) == 0) ? 3 : 0;
        //...PUSH rax/rbx
        lpDest[0] = 0x50+nReg;
        //...MOV rax/rbx, imm64
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8+nReg;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+4);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //...[lock] CMPXCHG QWORD PTR [rax/rbx], r64 or
        //...[lock] XADD QWORD PTR [rax/rbx], r64
        if (nLockPrefix != 0)
          lpDest[11] = 0xF0;
        lpDest[11+nLockPrefix] = lpSrc[0];
        lpDest[12+nLockPrefix] = lpSrc[1];
        lpDest[13+nLockPrefix] = lpSrc[2];
        lpDest[14+nLockPrefix] = (lpSrc[3] & 0x38) | nReg;
        //...POP rax/rbx
        lpDest[15+nLockPrefix] = 0x58+nReg;
        return 16+nLockPrefix;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessPUSHandPOPs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                                 __out LPBYTE lpDest)
{
#if defined _M_X64
  ULONG ulTemp;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined _M_X64
      if (lpSrc[0] == 0xFF && lpSrc[1] == 0x35)
      {
        //convert PUSH DWORD PTR [mofs32] into...
        //... PUSH rax
        lpDest[0] = 0x50;
        //... MOV rax, 1234567812345678h
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+2);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //... MOV rax, QWORD PTR [rax]
        lpDest[11] = 0x48;
        lpDest[12] = 0x8B;
        lpDest[13] = 0x00;
        //... XCHG QWORD PTR [rsp-8], rax
        lpDest[14] = 0x48;
        lpDest[15] = 0x87;
        lpDest[16] = 0x44;
        lpDest[17] = 0x24;
        lpDest[18] = 0xF8;
        return 19;
      }
      if (lpSrc[0] == 0x8F && lpSrc[1] == 0x05)
      {
        //convert POP DWORD PTR [mofs32] into...
        //... PUSH rax
        lpDest[0] = 0x50;
        //... PUSH rbx
        lpDest[1] = 0x53;
        //... MOV rax, 1234567812345678h
        lpDest[2] = 0x48;
        lpDest[3] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+4, k);
        //... MOV ebx, DWORD PTR [rsp+16]
        lpDest[12] = 0x8B;
        lpDest[13] = 0x5C;
        lpDest[14] = 0x24;
        lpDest[15] = 0x10;
        //... MOV DWORD PTR [rax], ebx
        lpDest[16] = 0x89;
        lpDest[17] = 0x18;
        //... POP rbx
        lpDest[18] = 0x5B;
        //... POP rax
        lpDest[19] = 0x58;
        //... ADD rsp, 8
        lpDest[20] = 0x48;
        lpDest[21] = 0x83;
        lpDest[22] = 0xC4;
        lpDest[23] = 0x08;
        return 24;
      }
#endif
      break;

    case 7:
#if defined _M_X64
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0xFF && lpSrc[2] == 0x35)
      {
        //convert PUSH QWORD PTR [mofs32] into...
        //... PUSH rax
        lpDest[0] = 0x50;
        //... MOV rax, 1234567812345678h
        lpDest[1] = 0x48;
        lpDest[2] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+3, k);
        //... XOR rax, rax
        lpDest[11] = 0x48;
        lpDest[12] = 0x33;
        lpDest[13] = 0xC0;
        //... MOV eax, DWORD PTR [rax]
        lpDest[14] = 0x8B;
        lpDest[15] = 0x00;
        //... XCHG QWORD PTR [rsp-8], rax
        lpDest[16] = 0x48;
        lpDest[17] = 0x87;
        lpDest[18] = 0x44;
        lpDest[19] = 0x24;
        lpDest[20] = 0xF8;
        return 21;
      }
      //----
      if (lpSrc[0] == 0x48 && lpSrc[1] == 0x8F && lpSrc[2] == 0x05)
      {
        //convert POP QWORD PTR [mofs32] into...
        //... PUSH rax
        lpDest[0] = 0x50;
        //... PUSH rbx
        lpDest[1] = 0x53;
        //... MOV rax, 1234567812345678h
        lpDest[2] = 0x48;
        lpDest[3] = 0xB8;
        ulTemp = HookEng_ReadUnalignedULong(lpSrc+3);
        k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
        HookEng_WriteUnalignedULongLong(lpDest+4, k);
        //... MOV rbx, QWORD PTR [rsp+16]
        lpDest[12] = 0x48;
        lpDest[13] = 0x8B;
        lpDest[14] = 0x5C;
        lpDest[15] = 0x24;
        lpDest[16] = 0x10;
        //... MOV QWORD PTR [rax], rbx
        lpDest[17] = 0x48;
        lpDest[18] = 0x89;
        lpDest[19] = 0x18;
        //... POP rbx
        lpDest[20] = 0x5B;
        //... POP rax
        lpDest[21] = 0x58;
        //... ADD rsp, 8
        lpDest[22] = 0x48;
        lpDest[23] = 0x83;
        lpDest[24] = 0xC4;
        lpDest[25] = 0x08;
        return 26;
      }
#endif
      break;
  }
  return 0;
}

static SIZE_T ProcessJUMPs(__in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
  LONG nOfs32;
  SIZE_T nDest;

  switch (nSrcInstrLen)
  {
    case 2:
      if (lpSrc[0] == 0xEB)
      {
        nOfs32 = (LONG)*((signed char*)(lpSrc+1));
        nDest = (SIZE_T)(lpSrc+nOfs32+2);
        //setup far jump
pj_setupfarjump:
#if defined _M_IX86
        lpDest[0] = 0xFF;
        lpDest[1] = 0x25;
        HookEng_WriteUnalignedULong(lpDest+2, 0);
        HookEng_WriteUnalignedULongLong(lpDest+6, nDest);
        return 14;
#elif defined _M_X64
        lpDest[0] = 0x48;
        lpDest[1] = 0xFF;
        lpDest[2] = 0x25;
        HookEng_WriteUnalignedULong(lpDest+3, 0);
        HookEng_WriteUnalignedULongLong(lpDest+7, nDest);
        return 15;
#endif
      }
      break;

    case 5:
      if (lpSrc[0] == 0xE9)
      {
        nktMemCopy(&nOfs32, lpSrc+1, sizeof(LONG));
        nDest = (SIZE_T)(lpSrc+nOfs32+5);
        goto pj_setupfarjump;
      }
      break;

    case 6:
      if (lpSrc[0]==0xFF && lpSrc[1]==0x25)
      {
#if defined _M_IX86
        nDest = (SIZE_T)HookEng_ReadUnalignedULong(lpSrc+2);
#elif defined _M_X64
        nOfs32 = (LONG)HookEng_ReadUnalignedULong(lpSrc+2);
        nDest = (SIZE_T)(nNextSrcIP+(LONGLONG)nOfs32);
#endif
pj_setupfarjump2:
#if defined _M_IX86
        //convert JMP DWORD PTR [nOfs32] into...
        //... PUSH EAX
        lpDest[0] = 0x50;
        //... MOV EAX, nDest
        lpDest[1] = 0xB8;
        HookEng_WriteUnalignedULong(lpDest+2, nDest);
        //... PUSH DWORD PTR [EAX]
        lpDest[6] = 0xFF;  lpDest[7] = 0x30;
        //... MOV EAX, DWORD PTR [ESP+4]
        lpDest[8] = 0x8B;  lpDest[9] = 0x44;
        lpDest[10] = 0x24;  lpDest[11] = 0x04;
        //... RET 4h
        lpDest[12] = 0xC2;  lpDest[13] = 0x04;  lpDest[14] = 0x00;
        return 15;
#elif defined _M_X64
        //convert JMP QWORD PTR [nOfs32] into...
        //... PUSH RAX
        lpDest[0] = 0x50;
        //... MOV RAX, nDest
        lpDest[1] = 0x48;  lpDest[2] = 0xB8;
        HookEng_WriteUnalignedULongLong(lpDest+3, nDest);
        //... PUSH QWORD PTR [RAX]
        lpDest[11] = 0xFF;  lpDest[12] = 0x30;
        //... MOV RAX, QWORD PTR [RSP+8]
        lpDest[13] = 0x48;  lpDest[14] = 0x8B;
        lpDest[15] = 0x44;  lpDest[16] = 0x24;
        lpDest[17] = 0x08;
        //... RET 8h
        lpDest[18] = 0xC2;  lpDest[19] = 0x08;  lpDest[20] = 0x00;
        return 21;
#endif
      }
      break;

    case 7:
      if (lpSrc[0]==0x48 && lpSrc[1]==0xFF && lpSrc[2]==0x25)
      {
#if defined _M_IX86
        nDest = (SIZE_T)HookEng_ReadUnalignedULong(lpSrc+3);
#elif defined _M_X64
        nOfs32 = (LONG)HookEng_ReadUnalignedULong(lpSrc+3);
        nDest = (SIZE_T)(nNextSrcIP+(LONGLONG)nOfs32);
#endif
        goto pj_setupfarjump2;
      }
      break;
  }
  return 0;
}
