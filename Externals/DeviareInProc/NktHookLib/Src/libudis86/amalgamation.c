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

#include "amalgamation_defines.h"

#include "stdint.h"
#include "source\libudis86\decode.h"
#include "source\libudis86\extern.h"
#include "tables\itab.h"
#include "source\libudis86\syn.h"
#include "source\libudis86\types.h"
#include "source\libudis86\udint.h"
#include <intrin.h>
#include <stdarg.h>

#pragma intrinsic(memset)

//-----------------------------------------------------------

typedef int (__cdecl *lpfn_vsnprintf)(_Out_cap_(_MaxCount) char * _DstBuf, _In_ size_t _MaxCount,
                                      _In_z_ _Printf_format_string_ const char * _Format, va_list _ArgList);

//-----------------------------------------------------------

void* volatile NktHookLib_fn_vsnprintf = NULL;

//-----------------------------------------------------------

int NktHookLib_vsnprintf(__out_z char *lpDest, __in size_t nMaxCount, __in_z const char *szFormatA,
                         __in va_list lpArgList);
int NktHookLib_sprintf(__out_z char *lpDest, __in_z const char *szFormatA, ...);
static void NktHookLib_memset(__out void *lpDest, __in int nVal, __in size_t nCount);

//-----------------------------------------------------------

#define vsnprintf  NktHookLib_vsnprintf
#define sprintf  NktHookLib_sprintf
#define memset  NktHookLib_memset

#include "source\libudis86\decode.c"
#include "tables\itab.c"

#define opr_cast    NktHookLib_ATT_opr_cast
#define gen_operand NktHookLib_ATT_gen_operand
#include "source\libudis86\syn-att.c"
#undef gen_operand
#undef opr_cast

#define opr_cast NktHookLib_INTEL_opr_cast
#define gen_operand NktHookLib_INTEL_gen_operand
#include "source\libudis86\syn-intel.c"
#undef gen_operand
#undef opr_cast

#include "source\libudis86\udis86.c"
#include "source\libudis86\syn.c"

#undef memset
#undef sprintf
#undef vsnprintf

//-----------------------------------------------------------

int NktHookLib_vsnprintf(__out_z char *lpDest, __in size_t nMaxCount, __in_z const char *szFormatA,
                         __in va_list lpArgList)
{
  if (lpDest != NULL && nMaxCount > 0)
    *lpDest = 0;
  if (NktHookLib_fn_vsnprintf == NULL)
    return 0;
  return ((lpfn_vsnprintf)NktHookLib_fn_vsnprintf)(lpDest, nMaxCount, szFormatA, lpArgList);
}

int NktHookLib_sprintf(__out_z char *lpDest, __in_z const char *szFormatA, ...)
{
  va_list argptr;
  int ret;

  va_start(argptr, szFormatA);
  ret = NktHookLib_vsnprintf(lpDest, 64, szFormatA, argptr);
  va_end(argptr);
  return ret;
}

#define XISALIGNED(x)  ((((size_t)(x)) & (sizeof(size_t)-1)) == 0)
static void NktHookLib_memset(__out void *lpDest, __in int nVal, __in size_t nCount)
{
  unsigned char *d;
  size_t n;

  d = (unsigned char*)lpDest;
  nVal &= 0xFF;
  if (XISALIGNED(d))
  {
    n = ((size_t)nVal) | (((size_t)nVal) << 8);
    n = n | (n << 16);
#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
    n = n | (n << 32);
#endif //_M_X64 || _M_IA64 || _M_AMD64
    while (nCount >= sizeof(size_t))
    {
      *((size_t*)d) = n;
      d += sizeof(size_t);
      nCount -= sizeof(size_t);
    }
  }
  //the following code is not fully optimized but avoid VC compiler to insert undesired "_memset" calls
  if (nCount > 0)
  {
    do
    {
      *d = (unsigned char)nVal;
    }
    while (--nCount > 0);
  }
  return;
}
#undef XISALIGNED

//-----------------------------------------------------------

//HACKS: Some functions in LibUDis86 uses 64-bit shift operations. On 32-bit, Visual C++ generates some dependency
//       on C Runtime by using '__allshl' and '__aullshr' helpers. We try to remove that dependency by creating our
//       replacements and telling the linker to use a weak symbol using an undocumented pragma directive.

#ifdef _M_IX86

__declspec(naked) void _allshl_default_nkthooklib()
{
  __asm
  {
    ; Handle shifts of 64 or more bits(all get 0)
    cmp     cl, 64
    jae     short ReturnZero
    ; Handle shifts of between 0 and 31 bits
    cmp     cl, 32
    jae     short MoreThanOrEqualTo32
    shld    edx, eax, cl
    shl     eax, cl
    ret

    ; Handle shifts of between 32 and 63 bits
MoreThanOrEqualTo32:
    mov     edx, eax
    xor     eax, eax
    and     cl, 31
    shl     edx, cl
    ret

    ; return 0 in edx:eax
ReturnZero:
    xor     eax, eax
    xor     edx, edx
    ret
  }
}

__declspec(naked) void _aullshr_default_nkthooklib()
{
  __asm
  {
    cmp     cl, 64
    jae     short ReturnZero
    ; Handle shifts of between 0 and 31 bits
    cmp     cl, 32
    jae     short MoreThanOrEqualTo32
    shrd    eax, edx, cl
    shr     edx, cl
    ret
    ; Handle shifts of between 32 and 63 bits
MoreThanOrEqualTo32:
    mov     eax, edx
    xor     edx, edx
    and     cl, 31
    shr     eax, cl
    ret
    ; return 0 in edx:eax
ReturnZero:
    xor     eax, eax
    xor     edx, edx
    ret
  }
}

#pragma comment(linker, "/alternatename:__allshl=__allshl_default_nkthooklib")
#pragma comment(linker, "/alternatename:__aullshr=__aullshr_default_nkthooklib")
const void* _dummy__allshl_default_nkthooklib = (void*)&_allshl_default_nkthooklib;
const void* _dummy__aullshr_default_nkthooklib = (void*)&_aullshr_default_nkthooklib;

#endif // _M_IX86
