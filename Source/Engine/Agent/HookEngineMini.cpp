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

#include "HookEngineInternals.h"
#include "HookEngineCallDataEntry.h"
#include "HookEngineMiscHelpers.h"
#include "ThreadSuspend.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#pragma optimize("", off) //force optimizations off because a VS2008 SP1 compiler bug at CRASH HERE line.
#pragma warning(disable : 4748)
HRESULT nktDvHookEng_MiniHookInstall(__in LPVOID lpAddress2Hook, __in LPVOID lpNewFunction,
                                     __out LPVOID *lplpCallOriginal, __out LONG volatile **lplpnDisableHook)
{
  CNktDvThreadSuspend cThreadSuspender;
  CNktDvThreadSuspend::IP_RANGE aIpRange;
  BYTE aOriginalStub[HOOKENG_MAX_STUB_SIZE], aNewStub[HOOKENG_MAX_STUB_SIZE], aModifiedStub[8];
  SIZE_T nOriginalStubSize, nNewStubSize, nInjCodeAndDataSize;
  LPBYTE lpInjCodeAndData;
  HINSTANCE hProcDll;
  DWORD dw;
  HRESULT hRes;
#if defined _M_X64
  ULONGLONG nMin, nMax;
  MEMORY_BASIC_INFORMATION sMbi;
#endif

  if (lpAddress2Hook == NULL || lplpCallOriginal == NULL || lplpnDisableHook == NULL)
    return E_POINTER;
  *lplpCallOriginal = NULL;
  *lplpnDisableHook = NULL;
  if ((::GetVersion() & 0x80000000) != 0)
    return E_NOTIMPL; //reject win9x
  lpAddress2Hook = HookEng_SkipJumpInstructions((LPBYTE)lpAddress2Hook);
  //try to find which dll belongs to the hook address and increment its reference count
  hRes = HookEng_FindDllAndIncrementUsageCount(&hProcDll, lpAddress2Hook);
  if (FAILED(hRes) && hRes != NKT_DVERR_NotFound)
    return hRes;
  //read original stub
  hRes = HookEng_CreateNewStub2(aOriginalStub, nOriginalStubSize, aNewStub, nNewStubSize, lpAddress2Hook, TRUE);
  if (FAILED(hRes))
    return hRes;
  //calculate inject code size
  //allocate memory for inject code
#if defined _M_IX86
  nInjCodeAndDataSize = 0x2A + nNewStubSize;
  lpInjCodeAndData = (LPBYTE)::VirtualAlloc(NULL, (nInjCodeAndDataSize + (SIZE_T)63) & (~((SIZE_T)63)),
                                            MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (lpInjCodeAndData == NULL)
    return E_OUTOFMEMORY;
#elif defined _M_X64
  nInjCodeAndDataSize = 0x41 + nNewStubSize;
  nMin = nMax = NKT_PTR_2_ULONGLONG(lpAddress2Hook) & (~65535ui64);
  if (nMin > 0x40000000ui64)
    nMin -= 0x40000000ui64;
  else
    nMin = 0ui64;
  if (nMax < 0xFFFFFFFFFFFFFFFFui64-0x40000000ui64)
    nMax += 0x40000000ui64;
  else
    nMax = 0xFFFFFFFFFFFFFFFFui64;
  while (nMin < nMax)
  {
    nktMemSet(&sMbi, 0, sizeof(sMbi));
    ::VirtualQuery((LPCVOID)nMin, &sMbi, sizeof(sMbi));
    if (sMbi.State == MEM_FREE)
    {
      lpInjCodeAndData = (LPBYTE)::VirtualAlloc((LPVOID)nMin, nInjCodeAndDataSize, MEM_RESERVE|MEM_COMMIT,
                                                PAGE_EXECUTE_READWRITE);
      if (lpInjCodeAndData != NULL)
        break;
    }
    nMin += 65536;
  }
  if (nMin >= nMax)
    return E_FAIL;
#endif
  //setup code
  *lplpnDisableHook = (LONG volatile *)lpInjCodeAndData;
#if defined _M_IX86
  memset(lpInjCodeAndData, 0x00, 8);                                               //flags location
  memset(lpInjCodeAndData+0x08, 0x90, 8);                                          //NOPs for hotpatching double hooks
  lpInjCodeAndData[0x10] = 0x50;                                                   //push  eax
  lpInjCodeAndData[0x11] = 0xB8;                                                   //mov   eax, ADDR lpInjCode
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x12)) = (ULONG)lpInjCodeAndData;
  lpInjCodeAndData[0x16] = 0xF7;                                                   //test  dword ptr [eax], 00000001h
  lpInjCodeAndData[0x17] = 0x00;
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x18)) = 0x00000001;
  lpInjCodeAndData[0x1C] = 0x75;                                                   //jne   @@1 ;if disabled/uninst
  lpInjCodeAndData[0x1D] = 0x06;
  lpInjCodeAndData[0x1E] = 0x58;                                                   //pop   eax
  lpInjCodeAndData[0x1F] = 0xE9;                                                   //jmp   hooked proc
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x20)) = (ULONG)lpNewFunction - (ULONG)lpInjCodeAndData - 0x24;
  lpInjCodeAndData[0x24] = 0x58;                                                   //@@1: pop   eax
  *lplpCallOriginal = lpInjCodeAndData + 0x25; // <<<<==== CRASH HERE. See note above. mov [edx?????], eax
  memcpy(lpInjCodeAndData+0x25, aNewStub, nNewStubSize); //new stub
  lpInjCodeAndData[0x25+nNewStubSize] = 0xE9;                         //jmp original proc after stub
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x26+nNewStubSize)) = (ULONG)lpAddress2Hook + (ULONG)nOriginalStubSize -
                                                                  (ULONG)(lpInjCodeAndData+0x2A+nNewStubSize);
#elif defined _M_X64
  memset(lpInjCodeAndData, 0x00, 8);                                               //flags location
  memset(lpInjCodeAndData+0x08, 0x90, 8);                                          //NOPs for hotpatching double hooks
  lpInjCodeAndData[0x10] = 0x50;                                                   //push  eax
  lpInjCodeAndData[0x11] = 0x48;                                                   //mov   eax, ADDR lpInjCode
  lpInjCodeAndData[0x12] = 0xB8;
  *((ULONGLONG NKT_UNALIGNED*)(lpInjCodeAndData+0x13)) = (ULONGLONG)lpInjCodeAndData;
  lpInjCodeAndData[0x1B] = 0xF7;                                                   //test  dword ptr [rax], 00000001h
  lpInjCodeAndData[0x1C] = 0x00;
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x1D)) = 0x00000001;
  lpInjCodeAndData[0x21] = 0x75;                                                   //jne   @@1 ;if disabled/uninst
  lpInjCodeAndData[0x22] = 0x0F;
  lpInjCodeAndData[0x23] = 0x58;                                                   //pop   rax
  lpInjCodeAndData[0x24] = 0xFF;                                                   //jmp   hooked proc
  lpInjCodeAndData[0x25] = 0x25;
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x26)) = 0;
  *((ULONGLONG NKT_UNALIGNED*)(lpInjCodeAndData+0x2A)) = (ULONGLONG)lpNewFunction;
  lpInjCodeAndData[0x32] = 0x58;                                                   //@@1: pop   rax
  *lplpCallOriginal = lpInjCodeAndData+0x33;
  memcpy(lpInjCodeAndData+0x33, aNewStub, nNewStubSize); //new stub
  lpInjCodeAndData[0x33+nNewStubSize] = 0xFF;                         //jmp original proc after stub
  lpInjCodeAndData[0x34+nNewStubSize] = 0x25;
  *((ULONG NKT_UNALIGNED*)(lpInjCodeAndData+0x35+nNewStubSize)) = 0;
  *((ULONGLONG NKT_UNALIGNED*)(lpInjCodeAndData+0x39+nNewStubSize)) = (ULONGLONG)lpAddress2Hook + nOriginalStubSize;
#endif //_M_X64
  //suspend threads
  aIpRange.nStart = (SIZE_T)lpAddress2Hook;
  aIpRange.nEnd = aIpRange.nStart + HOOKENG_JUMP_TO_HOOK_SIZE;
  hRes = cThreadSuspender.SuspendAll(&aIpRange, 1);
  if (FAILED(hRes))
  {
    ::VirtualFree(lpInjCodeAndData, 0, MEM_RELEASE);
    return hRes;
  }
  //replace original proc with a jump
  dw = (DWORD)(((SIZE_T)lpInjCodeAndData + 8) - ((SIZE_T)lpAddress2Hook+5));
  nktMemCopy(aModifiedStub, aOriginalStub, 8);
  aModifiedStub[0] = 0xE9; //JMP
  aModifiedStub[1] = (BYTE)( dw        & 0xFF);
  aModifiedStub[2] = (BYTE)((dw >>  8) & 0xFF);
  aModifiedStub[3] = (BYTE)((dw >> 16) & 0xFF);
  aModifiedStub[4] = (BYTE)((dw >> 24) & 0xFF);
  dw = 0;
  if (::VirtualProtect(lpAddress2Hook, HOOKENG_JUMP_TO_HOOK_SIZE, PAGE_EXECUTE_READWRITE, &dw) == FALSE &&
      ::VirtualProtect(lpAddress2Hook, HOOKENG_JUMP_TO_HOOK_SIZE, PAGE_EXECUTE_WRITECOPY, &dw) == FALSE)
  {
    //set the system function to PAGE_EXECUTE_READWRITE
    hRes = NKT_HRESULT_FROM_LASTERROR();
    ::VirtualFree(lpInjCodeAndData, 0, MEM_RELEASE);
    return hRes;
  }
  nktMemCopy(lpAddress2Hook, aModifiedStub, HOOKENG_JUMP_TO_HOOK_SIZE);
  ::VirtualProtect(lpAddress2Hook, HOOKENG_JUMP_TO_HOOK_SIZE, dw, &dw);
  //flush instruction cache
  ::FlushInstructionCache(::GetCurrentProcess(), lpAddress2Hook, 32);
  //resume threads
  cThreadSuspender.ResumeAll();
  return S_OK;
}
#pragma warning(default : 4748)
#pragma optimize("", on)
