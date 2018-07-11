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

#include "StdAfx.h"
#include "DotNetCoreHooks.h"
#include <NktHookLib.h>
#include <intrin.h>

#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

//-----------------------------------------------------------

// CODE EXPLANATION: In order to .NET method hooks to work properly, some steps
//                   must be taken.
//
// When DeviareLiteCOM is initialized, some JIT compiler functions are hooked in
// order to inhibit the compiler from generating INLINE code. This will add some
// performance //limitation but inlined methods cannot be hooked.
//
// Also, if you see code at DeviareLiteInterop, you can find some static methods
// that are executed upon program startup. This MUST be done in this way so
// JIT compiler methods are hooked before program's main routine starts running
// to ensure almost all methods are NOT inlined.

//-----------------------------------------------------------

#define FRAMEWORK_V2                                       1
#define FRAMEWORK_V4                                       2

#define INDEX_CILJit_compileMethod                         0
#define INDEX_CEEInfo_getMethodAttribs                     1

#define CORINFO_FLG_DONT_INLINE                   0x10000000
#define CORINFO_FLG_DONT_INLINE_CALLER            0x20000000

//-----------------------------------------------------------

class CFastLock
{
public:
  CFastLock(__inout LONG volatile *_lpnLock)
    {
    lpnLock = _lpnLock;
    while (_InterlockedCompareExchange(lpnLock, 1, 0) != 0)
      ::Sleep(1);
    return;
    };

  ~CFastLock()
    {
    _InterlockedExchange(lpnLock, 0);
    return;
    };

private:
  LONG volatile *lpnLock;
};

//-----------------------------------------------------------

static LONG volatile nHooksMutex = 0;
static CNktHookLib *lpHooks = NULL;
static DWORD dwFrameworkVersion = 0;
static CNktHookLib::HOOK_INFO sHooks[2] = { 0 };

//-----------------------------------------------------------

//NOTE: Although source code says "compHnd" parameter is a pointer to the ICorJitInfo interface, depending on which
//      .NET Framework version is being used, and depending on which OS is .NET running, the method receives a
//      pointer to a "CEEJitInfo" object casted to a ICorJitInfo object or a ICorMethodInfo one.
//
//      I think because the same compiler is used to compile all the framework, offsets and inherited v-table handling
//      is done properly but I couldn't figure out a pattern to detect which interface type is being sent to the
//      method.
//
//      Because of this, I decided to use an unsafe method. If you have more information and want to share why this
//      is happening, help will be appreciated.
static DWORD GuessMethod(__in LPBYTE compHnd, __out LPBYTE *lplpPtr)
{
  LPBYTE *lpCEEJitInfo_VTable, *lpCEEInfo_VTable;
  LONG *lpnOffsets;
  SSIZE_T nDif;

  __try
  {
    lpCEEJitInfo_VTable = *((LPBYTE**)(compHnd));
    lpnOffsets = *((LONG**)(compHnd+sizeof(SIZE_T)));
    lpCEEInfo_VTable = *((LPBYTE**)(compHnd+sizeof(SIZE_T)+(SSIZE_T)lpnOffsets[1]));
    if (lpCEEInfo_VTable != NULL)
    {
      nDif = (SSIZE_T)lpCEEJitInfo_VTable - (SSIZE_T)lpCEEInfo_VTable;
      if (nDif >= -512 && nDif < 512)
      {
        *lplpPtr = lpCEEInfo_VTable[2];
        return 2;
      }
    }
  }
  __except(EXCEPTION_BREAKPOINT)
  { }

  *lplpPtr = lpCEEJitInfo_VTable[0];
  return 1;
}

static LPBYTE GuessCallTo_getMethodAttribsInternal(__in LPBYTE lpPtr)
{
  SIZE_T i;

  //find the call to getMethodAttribsInternal
  for (i=0; i<128; i++)
  {
#if defined(_M_IX86)
    if (lpPtr[i] == 0xFF && lpPtr[i+1] == 0x75 && lpPtr[i+2] == 0x08 && lpPtr[i+3] == 0xE8)
      return lpPtr + (i + 8 + *((LONG NKT_UNALIGNED*)(lpPtr+i+4)));
#elif defined(_M_X64)
    if (lpPtr[i] == 0x48 && lpPtr[i+1] == 0x8B && lpPtr[i+2] == 0xD7 && lpPtr[i+3] == 0xE8)
      return lpPtr + (LONGLONG)(i + 8 + *((LONG NKT_UNALIGNED*)(lpPtr+i+4)));
#endif
  }
  return NULL;
}

#if defined(_M_IX86)

static DWORD __stdcall CEEInfo_getMethodAttribs_V2(__in LPVOID lpThis, __in LPVOID ftn1, __in LPVOID ftn2)
{
  typedef DWORD (__stdcall *lpfn_CEEInfo_getMethodAttribs)(__in LPVOID lpThis, __in LPVOID ftn1, __in LPVOID ftn2);
  DWORD dw;

  dw = ((lpfn_CEEInfo_getMethodAttribs)(sHooks[INDEX_CEEInfo_getMethodAttribs].lpCallOriginal))(lpThis, ftn1, ftn2);
  dw |= CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
  return dw;
}

static __declspec(naked) DWORD CEEInfo_getMethodAttribsInternal_V2(__in LPVOID ftn)
{
  __asm {
    push ebp;
    mov ebp, esp;
    push DWORD PTR [ebp+0x0C];
    push DWORD PTR [ebp+0x08];
    lea eax, sHooks;
    mov eax, DWORD PTR [eax+0x1C];
    call eax;
    or eax, CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
    pop ebp;
    ret 4h;
  }
}

static DWORD __stdcall CEEInfo_getMethodAttribs_V4(__in LPVOID lpThis, __in LPVOID ftn)
{
  typedef DWORD (__stdcall *lpfn_CEEInfo_getMethodAttribs)(__in LPVOID lpThis, __in LPVOID ftn);
  DWORD dw;

  dw = ((lpfn_CEEInfo_getMethodAttribs)(sHooks[INDEX_CEEInfo_getMethodAttribs].lpCallOriginal))(lpThis, ftn);
  dw |= CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
  return dw;
}

static __declspec(naked) DWORD CEEInfo_getMethodAttribsInternal_V4(__in LPVOID ftn)
{
  __asm {
    push ebp;
    mov ebp, esp;
    push DWORD PTR [ebp+0x08];
    lea eax, sHooks;
    mov eax, DWORD PTR [eax+0x1C];
    call eax;
    or eax, CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
    pop ebp;
    ret 4h;
  }
}

#elif defined(_M_X64)

static DWORD __stdcall CEEInfo_getMethodAttribs_V2(__in LPVOID lpThis, __in LPVOID ftn1, __in LPVOID ftn2)
{
  typedef DWORD (__stdcall *lpfn_CEEInfo_getMethodAttribs)(__in LPVOID lpThis, __in LPVOID ftn1, __in LPVOID ftn2);
  DWORD dw;

  dw = ((lpfn_CEEInfo_getMethodAttribs)(sHooks[INDEX_CEEInfo_getMethodAttribs].lpCallOriginal))(lpThis, ftn1, ftn2);
  dw |= CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
  return dw;
}

static DWORD __stdcall CEEInfo_getMethodAttribs_V4(__in LPVOID lpThis, __in LPVOID ftn)
{
  typedef DWORD (__stdcall *lpfn_CEEInfo_getMethodAttribs)(__in LPVOID lpThis, __in LPVOID ftn);
  DWORD dw;

  dw = ((lpfn_CEEInfo_getMethodAttribs)(sHooks[INDEX_CEEInfo_getMethodAttribs].lpCallOriginal))(lpThis, ftn);
  dw |= CORINFO_FLG_DONT_INLINE|CORINFO_FLG_DONT_INLINE_CALLER;
  return dw;
}

#endif

static LONG __stdcall CILJit_compileMethod(__in LPVOID lpThis, __in LPVOID compHnd, __in LPVOID methodInfo,
                                           __in unsigned flags, __out BYTE **entryAddress,
                                           __out ULONG *nativeSizeOfCode)
{
  typedef LONG (__stdcall *lpfn_CILJit_compileMethod)(__in LPVOID lpThis, __in LPVOID compHnd, __in LPVOID methodInfo,
                                                      __in unsigned flags, __out BYTE **entryAddress,
                                                      __out ULONG *nativeSizeOfCode);

  {
    CFastLock cLock(&nHooksMutex);

    //See "GuessMethod" definition above for technical details.
    if (sHooks[INDEX_CEEInfo_getMethodAttribs].lpProcToHook == NULL && compHnd != NULL)
    {
      LPBYTE lpPtr, lpPtr2;
      HRESULT hRes;

      GuessMethod((LPBYTE)compHnd, &lpPtr);
      lpPtr2 = GuessCallTo_getMethodAttribsInternal(lpPtr);
      if (lpPtr2 == NULL)
      {
        sHooks[INDEX_CEEInfo_getMethodAttribs].lpProcToHook = lpPtr;
        switch (dwFrameworkVersion)
        {
          case FRAMEWORK_V2:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribs_V2;
            break;
          case FRAMEWORK_V4:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribs_V4;
            break;
        }
      }
      else
      {
        sHooks[INDEX_CEEInfo_getMethodAttribs].lpProcToHook = lpPtr2;
#if defined(_M_IX86)
        switch (dwFrameworkVersion)
        {
          case FRAMEWORK_V2:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribsInternal_V2;
            break;
          case FRAMEWORK_V4:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribsInternal_V4;
            break;
        }
#elif defined(_M_X64)
        switch (dwFrameworkVersion)
        {
          case FRAMEWORK_V2:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribs_V2;
            break;
          case FRAMEWORK_V4:
            sHooks[INDEX_CEEInfo_getMethodAttribs].lpNewProcAddr = &CEEInfo_getMethodAttribs_V4;
            break;
        }
#endif
      }
      hRes = HRESULT_FROM_WIN32(lpHooks->Hook(&sHooks[INDEX_CEEInfo_getMethodAttribs], 1));
      if (FAILED(hRes))
      {
        if (entryAddress != NULL)
          *entryAddress = NULL;
        if (nativeSizeOfCode != NULL)
          *nativeSizeOfCode = 0;
        return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NULL, 3);
      }
    }
  }
  return ((lpfn_CILJit_compileMethod)(sHooks[INDEX_CILJit_compileMethod].lpCallOriginal))(lpThis, compHnd, methodInfo,
                                                                                          flags, entryAddress,
                                                                                          nativeSizeOfCode);
}

//-----------------------------------------------------------
//-----------------------------------------------------------

namespace DotNetCoreHooks
{

HRESULT Initialize()
{
  typedef LPVOID (__stdcall *lpfn_getJit)();

  CFastLock cLock(&nHooksMutex);
  HINSTANCE hDll;
  lpfn_getJit fn_getJit;
  LPVOID lpJit;
  HRESULT hRes;

  if (lpHooks != NULL)
    return S_OK;
  //----
  hDll = ::GetModuleHandleW(L"clrjit.dll");
  if (hDll != NULL)
  {
    dwFrameworkVersion = FRAMEWORK_V4;
  }
  else
  {
    hDll = ::GetModuleHandleW(L"mscorjit.dll");
    if (hDll == NULL)
      return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    dwFrameworkVersion = FRAMEWORK_V2;
  }
  fn_getJit = (lpfn_getJit)::GetProcAddress(hDll, "getJit");
  if (fn_getJit == NULL)
    return ERROR_PROC_NOT_FOUND;
  lpJit = fn_getJit();
  if (lpJit == NULL)
    return E_FAIL;
  //create hook
  lpHooks = new CNktHookLib();
  if (lpHooks == NULL)
    return E_OUTOFMEMORY;
  sHooks[INDEX_CILJit_compileMethod].lpProcToHook = (LPVOID)((*((SIZE_T**)lpJit))[0]);
  sHooks[INDEX_CILJit_compileMethod].lpNewProcAddr = &CILJit_compileMethod;
  hRes = HRESULT_FROM_WIN32(lpHooks->Hook(&sHooks[INDEX_CILJit_compileMethod], 1));
  if (FAILED(hRes))
  {
    delete lpHooks;
    lpHooks = NULL;
    dwFrameworkVersion = 0;
    return hRes;
  }
  return S_OK;
}

VOID Finalize()
{
  CFastLock cLock(&nHooksMutex);

  if (lpHooks != NULL)
  {
    lpHooks->UnhookAll();
    delete lpHooks;
    lpHooks = NULL;
  }
  memset(sHooks, 0, sizeof(sHooks));
  dwFrameworkVersion = 0;
  return;
}

}; //namespace DotNetCoreHooks
