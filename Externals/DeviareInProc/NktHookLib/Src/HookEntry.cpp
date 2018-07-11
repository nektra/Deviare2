/*
 * Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 * All rights reserved. Contact: http://www.nektra.com
 *
 *
 * This file is part of Deviare In-Proc
 *
 *
 * Commercial License Usage
 * ------------------------
 * Licensees holding valid commercial Deviare In-Proc licenses may use this
 * file in accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and Nektra.  For licensing terms and
 * conditions see http://www.nektra.com/licensing/.  For further information
 * use the contact form at http://www.nektra.com/contact/.
 *
 *
 * GNU General Public License Usage
 * --------------------------------
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl.html.
 *
 **/

#include "HookEntry.h"

namespace NktHookLib {
namespace Internals {

#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

//-----------------------------------------------------------

static SIZE_T ProcessCALLs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                           __out LPBYTE lpDest);
static SIZE_T ProcessMOVs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                          __out LPBYTE lpDest);
static SIZE_T ProcessLEAs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                          __out LPBYTE lpDest);
static SIZE_T ProcessSPECIAL1s(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest);
static SIZE_T ProcessSPECIAL2s(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest);
static SIZE_T ProcessCMPXCHGandXADDs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen,
                                     __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);
static SIZE_T ProcessPUSHandPOPs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen,
                                 __in SIZE_T nNextSrcIP, __out LPBYTE lpDest);
static SIZE_T ProcessJUMPs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                           __in HANDLE hProc, __out LPBYTE lpDest);

//-----------------------------------------------------------

CHookEntry::CHookEntry(__in CProcessesHandles::CEntry *lpProcEntry, __in DWORD _dwFlags) : TNktLnkLstNode<CHookEntry>(),
                                                                                           CNktNtHeapBaseObj()
{
  ZeroFields();
  cProcEntry = lpProcEntry;
  dwFlags = _dwFlags;
  return;
}

CHookEntry::~CHookEntry()
{
  if (lpInjCode != NULL)
    cProcEntry->FreeMem(lpInjCode, FALSE);
  if (lpInjData != NULL)
    cProcEntry->FreeMem(lpInjData, TRUE);
  return;
}

VOID CHookEntry::ZeroFields()
{
  nId = 0;
  lpOrigProc = NULL;
  lpNewProc = NULL;
  lpCall2Orig = NULL;
  lpInjCode = lpInjData = NULL;
  nInjCodeAndDataSize = 0;
  nOriginalStubSize = nNewStubSize = 0;
  nInstalledCode = 0;
  dwFlags = 0;
  return;
}

LPBYTE CHookEntry::SkipJumpInstructions(__in LPBYTE lpPtr)
{
  BYTE aTempBuf[16];
  SIZE_T nReaded;
  LONG nSize, nOfs;

  for (;;)
  {
    nReaded = NktHookLibHelpers::ReadMem(cProcEntry->GetHandle(), aTempBuf, lpPtr, sizeof(aTempBuf));
    if (nReaded >= 5 && aTempBuf[0] == 0xE9)
    {
      nOfs = *((LONG NKT_UNALIGNED*)(aTempBuf+1));
      lpPtr += nOfs+5;
    }
    else if (nReaded >= 2 && aTempBuf[0] == 0xEB)
    {
      nOfs = (LONG)*((signed char*)&aTempBuf[1]);
      lpPtr += nOfs+2;
    }
    else if ((nReaded >= 6 && aTempBuf[0]==0xFF && aTempBuf[1]==0x25) ||
             (nReaded >= 7 && aTempBuf[0]==0x48 && aTempBuf[1]==0xFF && aTempBuf[2]==0x25))
    {
      nSize = (aTempBuf[0] == 0x48) ? 3 : 2;
      nOfs = *((LONG NKT_UNALIGNED*)(aTempBuf+nSize));
      switch (cProcEntry->GetPlatform())
      {
        case NKTHOOKLIB_ProcessPlatformX86:
          if (NktHookLibHelpers::ReadMem(cProcEntry->GetHandle(), aTempBuf, (LPBYTE)((ULONG_PTR)(ULONG)nOfs),
                                         sizeof(ULONG)) != sizeof(ULONG))
            return NULL;
          lpPtr = (LPBYTE)(ULONG_PTR)*((ULONG NKT_UNALIGNED*)(aTempBuf));
          break;

#if defined(_M_X64)
        case NKTHOOKLIB_ProcessPlatformX64:
          if (NktHookLibHelpers::ReadMem(cProcEntry->GetHandle(), aTempBuf, lpPtr+(LONGLONG)(nSize+4)+(LONGLONG)nOfs,
                                         sizeof(ULONGLONG)) != sizeof(ULONGLONG))
            return NULL;
          lpPtr = (LPBYTE)*((ULONGLONG NKT_UNALIGNED*)(aTempBuf));
          break;
#endif //_M_X64

        default:
          return NULL;
      }
    }
    else
    {
      break;
    }
  }
  return lpPtr;
}

DWORD CHookEntry::CreateStub(__in BOOL bOutputDebug)
{
  SIZE_T k, nSrcInstrLen, nDestInstrLen, nNextSrcIP;
  LPBYTE lpSrc, lpDest, s[2];
  CHAR szBufA[32];
  BOOL bIsMemOp;
  LONG nPlatform;

  switch (nPlatform = cProcEntry->GetPlatform())
  {
    case NKTHOOKLIB_ProcessPlatformX86:
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
#endif //_M_X64
      break;
    default:
      return ERROR_NOT_SUPPORTED;
  }
  //---
  NktHookLibHelpers::MemSet(aOriginalStub, 0, HOOKENG_MAX_ORIGINAL_STUB_SIZE);
  NktHookLibHelpers::MemSet(aNewStub, 0, HOOKENG_MAX_STUB_SIZE);
  for (k=HOOKENG_MAX_ORIGINAL_STUB_SIZE; k>0; k--)
  {
    if (NktHookLibHelpers::ReadMem(cProcEntry->GetHandle(), aOriginalStub, lpOrigProc, k) == k)
      break;
  }
  if (k == 0)
    return ERROR_CAN_NOT_COMPLETE;
  //calculate stub size and create the new one
  nOriginalStubSize = nNewStubSize = 0;
  while (nOriginalStubSize < GetJumpToHookBytes())
  {
    lpSrc = aOriginalStub + nOriginalStubSize;
    lpDest = aNewStub + nNewStubSize;
    nSrcInstrLen = NktHookLibHelpers::GetInstructionLength(lpSrc, HOOKENG_MAX_ORIGINAL_STUB_SIZE-nOriginalStubSize,
                                        sizeof(SIZE_T)<<3, &bIsMemOp, (bOutputDebug != FALSE) ? szBufA : NULL,
                                        (bOutputDebug != FALSE) ? (sizeof(szBufA)/sizeof(szBufA[0])) : 0);
    if (nSrcInstrLen == 0)
      return ERROR_INVALID_DATA; //invalid opcode
    nDestInstrLen = nSrcInstrLen;
    //check special opcodes
    s[0] = (LPBYTE)lpOrigProc + nOriginalStubSize;
    s[1] = ((dwFlags & NKTHOOKLIB_DontSkipAnyJumps) != 0) ? s[0] : SkipJumpInstructions(s[0]);
    if (s[1] == NULL)
      s[1] = s[0];
    if (s[1] != s[0])
    {
      //we hit a jump in the middle of the stuff so we encode a jump to the final position ...
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX86:
          //...convert to PUSH imm32/RET
          lpDest[0] = 0x68;
          *((ULONG NKT_UNALIGNED*)(lpDest+1)) = (ULONG)(ULONG_PTR)(s[1]);
          lpDest[5] = 0xC3; //ret
          nDestInstrLen = 6;
          break;

#if defined(_M_X64)
        case NKTHOOKLIB_ProcessPlatformX64:
          //jmp to original call (JMP QWORD PTR [RIP+0h])
          lpDest[0] = 0x48;  lpDest[1] = 0xFF;  lpDest[2] = 0x25;
          *((ULONG NKT_UNALIGNED*)(lpDest+3)) = 0;
          *((ULONGLONG NKT_UNALIGNED*)(lpDest+7)) = (ULONGLONG)(s[1]);
          nDestInstrLen = 15;
          break;
#endif //_M_X64
      }
      //...and finalize the loop
      if (nOriginalStubSize + nSrcInstrLen < GetJumpToHookBytes())
        nSrcInstrLen = GetJumpToHookBytes() - nOriginalStubSize;
    }
    else
    {
      if (bOutputDebug != FALSE)
      {
        NktHookLibHelpers::DebugPrint("NktHookLib: Disassembly 0x%IX -> %s\r\n", (SIZE_T)lpOrigProc+nOriginalStubSize,
                                      szBufA);
      }
      nNextSrcIP = (SIZE_T)lpOrigProc + nOriginalStubSize + nSrcInstrLen;
      nDestInstrLen = ProcessCALLs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessMOVs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessLEAs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessSPECIAL1s(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessSPECIAL2s(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessCMPXCHGandXADDs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessPUSHandPOPs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, lpDest);
      if (nDestInstrLen == 0)
        nDestInstrLen = ProcessJUMPs(nPlatform, lpSrc, nSrcInstrLen, nNextSrcIP, cProcEntry->GetHandle(), lpDest);
      if (nDestInstrLen == 0)
      {
        NktHookLibHelpers::MemCopy(lpDest, lpSrc, nSrcInstrLen);
        nDestInstrLen = nSrcInstrLen;
      }
    }
    nOriginalStubSize += nSrcInstrLen;
    nNewStubSize += nDestInstrLen;
  }
  return ERROR_SUCCESS;
}

//-----------------------------------------------------------

static SIZE_T ProcessCALLs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                           __out LPBYTE lpDest)
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
        *((ULONG NKT_UNALIGNED*)(lpDest+1)) = 0;
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            //...increment return address (ADD DWORD PTR [esp], 4+5+1)
            lpDest[5] = 0x83;
            lpDest[6] = 0x04;
            lpDest[7] = 0x24;
            lpDest[8] = 4 + 5 + 1;
            //jmp to original call (PUSH imm32/RET)
            lpDest[9] = 0x68;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+1));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONG NKT_UNALIGNED*)(lpDest+10)) = (ULONG)(ULONG_PTR)k;
            lpDest[14] = 0xC3; //ret
            return 15;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            //...increment return address (ADD QWORD PTR [rsp], 5+15)
            lpDest[5] = 0x48;
            lpDest[6] = 0x83;
            lpDest[7] = 0x04;
            lpDest[8] = 0x24;
            lpDest[9] = 5 + 15;
            //jmp to original call (JMP QWORD PTR [RIP+0h])
            lpDest[10] = 0x48;  lpDest[11] = 0xFF;  lpDest[12] = 0x25;
            *((ULONG NKT_UNALIGNED*)(lpDest+13)) = 0;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+1));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+17)) = k;
            return 25;
#endif //_M_X64
        }
      }
      break;

    case 6:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0xFF && lpSrc[1] == 0x15)
          {
            //convert "indirect 32-bit CALL" (CALL [mofs32]) [FF 15 xxyyzzww] into...
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
pj_setupfarcall_x64:
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            //... PUSH RAX
            lpDest[0] = 0x50;
            //... MOV RAX, nDest
            lpDest[1] = 0x48;  lpDest[2] = 0xB8;
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = (ULONGLONG)k;
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
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x48 && lpSrc[1] == 0xFF && lpSrc[1] == 0x15)
          {
            //convert "indirect 64-bit CALL" (CALL [mofs64]) [48 FF 15 xxyyzzww] into...
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            goto pj_setupfarcall_x64;
          }
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessMOVs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                          __out LPBYTE lpDest)
{
#if defined(_M_X64)
  ULONG ulTemp;
  BYTE nReg;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0xC6 && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert MOV BYTE PTR [mofs32], imm8 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+2)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+6)) = k;
            //... restore rax (POP rax)
            lpDest[14] = 0x58;
            return 15;
          }
          break;
      }
#endif
      break;

    case 8:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+4));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
            //...and "relative 128-bit MOV" (MOVDQU xmm0-7, [rax]) [F3 0F 6F 00###000]
            lpDest[11] = 0xF3;  lpDest[12] = 0x0F;
            lpDest[13] = 0x6F;  lpDest[14] = (lpSrc[3] & 0x38);
            //... restore rax (POP rax)
            lpDest[15] = 0x58;
            return 16;
          }
          break;
      }
#endif //_M_X64
      break;

    case 9:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x66 && lpSrc[1] == 0xC7 && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert MOV WORD PTR [mofs32], imm16 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+4));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
            //...and "relative 128-bit MOV" (MOVDQU xmm0-7, [rax]) [F3 44 0F 6F 00###000]
            lpDest[11] = 0xF3;  lpDest[12] = 0x44;  lpDest[13] = 0x0F;
            lpDest[14] = 0x6F;  lpDest[15] = (lpSrc[4] & 0x38);
            //... restore rax (POP rax)
            lpDest[16] = 0x58;
            return 17;
          }
          break;
      }
#endif //_M_X64
      break;

    case 10:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0xC7 && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert MOV DWORD PTR [mofs32], imm32 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 11:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x48 && lpSrc[1] == 0xC7 && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert MOV QWORD PTR [mofs32], imm32 into....
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessLEAs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                          __out LPBYTE lpDest)
{
#if defined(_M_X64)
  ULONG ulTemp;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x8D && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert LEA r32, [mofs32] into...
            //...MOV r32, imm32
            lpDest[0] = 0xB8 + ((lpSrc[1] & 0x38) >> 3);
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONG NKT_UNALIGNED*)(lpDest+1)) = (ULONG)k; //on x64 store only the lower bits
            return 5;
          }
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if ((lpSrc[0] & 0xFB) == 0x48 && lpSrc[1] == 0x8D && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert LEA r64, [mofs32] into...
            //...MOV r64, imm64
            lpDest[0] = 0x48 + ((lpSrc[0] & 0x04) >> 2); //REX bit
            lpDest[1] = 0xB8 + ((lpSrc[2] & 0x38) >> 3);
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+2)) = k;
            return 10;
          }
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessSPECIAL1s(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest)
{
#if defined(_M_X64)
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
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if ((lpSrc[0] == 0xFF && (lpSrc[1] == 0x0D || lpSrc[1] == 0x05)) ||
              (lpSrc[0] == 0xF7 && (lpSrc[1] == 0x1D || lpSrc[1] == 0x15)))
          {
            //convert DEC/INC/NEG/NOT [mofs32] into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
            //...[lock] DEC/INC/NEG/NOT [rax]
            if (nLockPrefix != 0)
              lpDest[11] = 0xF0;
            lpDest[11+nLockPrefix] = lpSrc[0];
            lpDest[12+nLockPrefix] = (lpSrc[1] & 0xF8);
            //...POP rax
            lpDest[13+nLockPrefix] = 0x58;
            return 14+nLockPrefix;
          }
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessSPECIAL2s(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                               __out LPBYTE lpDest)
{
#if defined(_M_X64)
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
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x80 && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP BYTE PTR [mofs32], imm8 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          //----
          if (lpSrc[0] == 0x83 && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], imm8 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          //----
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          //----
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 8:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x66 && lpSrc[1] == 0x83 && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [mofs32], imm8 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 9:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x66 && lpSrc[1] == 0x81 && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP WORD PTR [mofs32], imm16 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 10:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x81 && (lpSrc[1] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP DWORD PTR [mofs32], imm32 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 11:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x48 && lpSrc[1] == 0x81 && (lpSrc[2] & 0xC7) == 0x05)
          {
            //convert ADD/SUB/ADC/SBB/AND/OR/XOR/CMP QWORD PTR [mofs32], imm32 into...
            //...PUSH rax
            lpDest[0] = 0x50;
            //...MOV rax, imm64
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessCMPXCHGandXADDs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen,
                                     __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined(_M_X64)
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
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 8:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+4));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          //----
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+4));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessPUSHandPOPs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen,
                                 __in SIZE_T nNextSrcIP, __out LPBYTE lpDest)
{
#if defined(_M_X64)
  ULONG ulTemp;
  SIZE_T k;
#endif

  switch (nSrcInstrLen)
  {
    case 6:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0xFF && lpSrc[1] == 0x35)
          {
            //convert PUSH DWORD PTR [mofs32] into...
            //... PUSH rax
            lpDest[0] = 0x50;
            //... MOV rax, 1234567812345678h
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+2));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
          //----
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+4)) = k;
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
          break;
      }
#endif //_M_X64
      break;

    case 7:
#if defined(_M_X64)
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX64:
          if (lpSrc[0] == 0x48 && lpSrc[1] == 0xFF && lpSrc[2] == 0x35)
          {
            //convert PUSH QWORD PTR [mofs32] into...
            //... PUSH rax
            lpDest[0] = 0x50;
            //... MOV rax, 1234567812345678h
            lpDest[1] = 0x48;
            lpDest[2] = 0xB8;
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = k;
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
            ulTemp = *((ULONG NKT_UNALIGNED*)(lpSrc+3));
            k = nNextSrcIP + (SSIZE_T)(LONG)ulTemp; //add displacement
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+4)) = k;
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
          break;
      }
#endif //_M_X64
      break;
  }
  return 0;
}

static SIZE_T ProcessJUMPs(__in LONG nPlatform, __in LPBYTE lpSrc, __in SIZE_T nSrcInstrLen, __in SIZE_T nNextSrcIP,
                           __in HANDLE hProc, __out LPBYTE lpDest)
{
  LONG nOfs32;
  BYTE nOpcodeInfo = 0;
  SIZE_T nDest;

  switch (nSrcInstrLen)
  {
    case 2:
      if (lpSrc[0] == 0xEB)
      {
        nOfs32 = (LONG)*((signed char*)(lpSrc+1));
        nDest = nNextSrcIP + (SSIZE_T)(LONG)nOfs32; //add displacement
        //setup far jump
pj_setupfarjump:
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            //...convert to PUSH imm32/RET
            lpDest[0] = 0x68;
            *((ULONG NKT_UNALIGNED*)(lpDest + 1)) = (ULONG)nDest;
            lpDest[5] = 0xC3; //ret
            return 6;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            //... convert to JMP QWORD PTR [RIP+0h]
            lpDest[0] = 0x48;
            lpDest[1] = 0xFF;
            lpDest[2] = 0x25;
            *((ULONG NKT_UNALIGNED*)(lpDest+3)) = 0;
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+7)) = (ULONGLONG)nDest;
            return 15;
#endif // _M_X64
        }
      }
      if (lpSrc[0] == 0xE3)
      {
        //calculate target of JECXZ/JRCXZ
        nOfs32 = (LONG)*((signed char*)(lpSrc + 1));
        nDest = nNextSrcIP + (SSIZE_T)(LONG)nOfs32; //add displacement
        //NOTE: because of the lack of a JECXNZ/JRCXNZ opcode we have to simulate a series of jumps
        //a) JECXZ $+2 (to skip next jump
        lpDest[0] = 0xE3;
        lpDest[1] = 0x02;
        //b) JMP $+n
        lpDest[2] = 0xEB;
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            lpDest[3] = 0x06;
            break;
#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            lpDest[3] = 0x0F;
            break;
#endif // _M_X64
        }
        //c) JMP far to real destination of original JECXZ/JRCXZ
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            lpDest[4] = 0x68;
            *((ULONG NKT_UNALIGNED*)(lpDest + 5)) = (ULONG)nDest;
            lpDest[9] = 0xC3; //ret
            return 10;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            lpDest[4] = 0x48;
            lpDest[5] = 0xFF;
            lpDest[6] = 0x25;
            *((ULONG NKT_UNALIGNED*)(lpDest + 7)) = 0;
            *((ULONGLONG NKT_UNALIGNED*)(lpDest + 11)) = nDest;
            return 19;
#endif // _M_X64
        }
      }
      if ((lpSrc[0] & 0xF0) == 0x70)
      {
        //conditional 8-bit jump
        nOfs32 = (LONG)*((signed char*)(lpSrc+1));
        nOpcodeInfo = lpSrc[0] & 0x0F;
pj_setupconditionaljump:
        nDest = nNextSrcIP + (SSIZE_T)(LONG)nOfs32; //add displacement
        //IMPORTANT: As stated on Intel manuals, we assume:
        //           1) 0F 8x (Jcc rel-32) maps to their rel-8 counterparts (opcodes 7x).
        //              I.e.: 0F 86 cd (JBE rel-32) => 76 cb (JBE rel-8)
        //           2) A Jcc rel-8 opcode has this format 0111xxxy, let z = !y, then opcode 0111xxxz maps to
        //              the opposite jump. I.e.: 01111010 (7A) is JP (jump if parity) so 01111011 (7B) is JNP
        //              (jump if not parity)
        //
        //NOTE: We need to replace a conditional jump with a series of jumps
        //a) build a short jump using the opposite rel-8 jump
        lpDest[0] = 0x70 | (nOpcodeInfo ^ 1);
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            lpDest[1] = 0x06;
            break;
#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            lpDest[1] = 0x0F;
            break;
#endif // _M_X64
        }
        //c) JMP far to real destination of original Jcc
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            lpDest[2] = 0x68;
            *((ULONG NKT_UNALIGNED*)(lpDest + 3)) = (ULONG)nDest;
            lpDest[7] = 0xC3; //ret
            return 8;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            lpDest[2] = 0x48;
            lpDest[3] = 0xFF;
            lpDest[4] = 0x25;
            *((ULONG NKT_UNALIGNED*)(lpDest + 5)) = 0;
            *((ULONGLONG NKT_UNALIGNED*)(lpDest + 9)) = nDest;
            return 17;
#endif // _M_X64
        }
      }
      break;

    case 5:
      if (lpSrc[0] == 0xE9)
      {
        nOfs32 = *((LONG NKT_UNALIGNED*)(lpSrc+1));
        nDest = (SIZE_T)(lpSrc+nOfs32+5);
        goto pj_setupfarjump;
      }
      break;

    case 6:
      if (lpSrc[0]==0xFF && lpSrc[1]==0x25)
      {
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            nDest = (SIZE_T)(*((ULONG NKT_UNALIGNED*)(lpSrc+2)));
pj_setupfarjump2_x86:
            //convert JMP DWORD PTR [nOfs32] into...
            //... PUSH EAX
            lpDest[0] = 0x50;
            //... MOV EAX, nDest
            lpDest[1] = 0xB8;
            *((ULONG NKT_UNALIGNED*)(lpDest+2)) = (ULONG)nDest;
            //... PUSH DWORD PTR [EAX]
            lpDest[6] = 0xFF;  lpDest[7] = 0x30;
            //... MOV EAX, DWORD PTR [ESP+4]
            lpDest[8] = 0x8B;  lpDest[9] = 0x44;
            lpDest[10] = 0x24;  lpDest[11] = 0x04;
            //... RET 4h
            lpDest[12] = 0xC2;  lpDest[13] = 0x04;  lpDest[14] = 0x00;
            return 15;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            nOfs32 = *((LONG NKT_UNALIGNED*)(lpSrc+2));
            nDest = (SIZE_T)(nNextSrcIP+(LONGLONG)nOfs32);
pj_setupfarjump2_x64:
            //convert JMP QWORD PTR [nOfs32] into...
            //... PUSH RAX
            lpDest[0] = 0x50;
            //... MOV RAX, nDest
            lpDest[1] = 0x48;  lpDest[2] = 0xB8;
            *((ULONGLONG NKT_UNALIGNED*)(lpDest+3)) = (ULONGLONG)nDest;
            //... PUSH QWORD PTR [RAX]
            lpDest[11] = 0xFF;  lpDest[12] = 0x30;
            //... MOV RAX, QWORD PTR [RSP+8]
            lpDest[13] = 0x48;  lpDest[14] = 0x8B;
            lpDest[15] = 0x44;  lpDest[16] = 0x24;
            lpDest[17] = 0x08;
            //... RET 8h
            lpDest[18] = 0xC2;  lpDest[19] = 0x08;  lpDest[20] = 0x00;
            return 21;
#endif //_M_X64
        }
      }
      if (lpSrc[0] == 0x0F && (lpSrc[1] & 0xF0) == 0x80)
      {
        //conditional jump with 32-bit offset
        nOfs32 = *((LONG NKT_UNALIGNED*)(lpSrc+2));
        nOpcodeInfo = lpSrc[1] & 0x0F;
        goto pj_setupconditionaljump;
      }
      break;

    case 7:
      if (lpSrc[0]==0x48 && lpSrc[1]==0xFF && lpSrc[2]==0x25)
      {
        switch (nPlatform)
        {
          case NKTHOOKLIB_ProcessPlatformX86:
            nDest = (SIZE_T)(*((ULONG NKT_UNALIGNED*)(lpSrc+3)));
            goto pj_setupfarjump2_x86;

#if defined(_M_X64)
          case NKTHOOKLIB_ProcessPlatformX64:
            nOfs32 = *((LONG NKT_UNALIGNED*)(lpSrc+3));
            nDest = (SIZE_T)(nNextSrcIP+(LONGLONG)nOfs32);
            goto pj_setupfarjump2_x64;
#endif //_M_X64
        }
      }
      break;
  }
  return 0;
}

//-----------------------------------------------------------

} //Internals
} //NktHookLib
