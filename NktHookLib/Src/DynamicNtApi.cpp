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

#include <NktHookLib.h>
#include "DynamicNtApi.h"
#include <intrin.h>

#if defined(_M_IX86)
  #pragma intrinsic (_InterlockedExchange)
#elif defined(_M_X64)
  #pragma intrinsic (_InterlockedExchangePointer)
#endif

//-----------------------------------------------------------

static LONG volatile nInitialized = 0;
extern "C" {
  extern void* volatile NktHookLib_fn_vsnprintf;
  void* volatile NktHookLib_fn_vsnwprintf = NULL;
  void* volatile NktHookLib_fn_DbgPrint = NULL;
};
namespace NktHookLibHelpers {

extern lpfnInternalApiResolver fnInternalApiResolver;
extern LPVOID lpUserParam;

} //namespace NktHookLibHelpers

//-----------------------------------------------------------

static VOID InitializeInternals();
static LPVOID ResolveNtDllApi(__inout LPVOID &_hNtDll, __in_z LPCSTR szApiNameA);

//-----------------------------------------------------------

namespace NktHookLib {
namespace Internals {

#define NKT_PARSE_NTAPI_NTSTATUS(name, parameters, _notused)  \
  typedef NTSTATUS (__stdcall *lpfn_##name)parameters;        \
  lpfn_##name volatile NktHookLib_fn_##name = NULL;
#define NKT_PARSE_NTAPI_VOID(name, parameters, _notused)      \
  typedef VOID (__stdcall *lpfn_##name)parameters;            \
  lpfn_##name volatile NktHookLib_fn_##name = NULL;
#define NKT_PARSE_NTAPI_PVOID(name, parameters, _notused)     \
  typedef PVOID (__stdcall *lpfn_##name)parameters;           \
  lpfn_##name volatile NktHookLib_fn_##name = NULL;
#define NKT_PARSE_NTAPI_BOOLEAN(name, parameters, _notused)   \
  typedef BOOLEAN (__stdcall *lpfn_##name)parameters;         \
  lpfn_##name volatile NktHookLib_fn_##name = NULL;
#define NKT_PARSE_NTAPI_ULONG(name, parameters, _notused)     \
  typedef ULONG (__stdcall *lpfn_##name)parameters;           \
  lpfn_##name volatile NktHookLib_fn_##name = NULL;
#include "NtApiDeclarations.h"
#undef NKT_PARSE_NTAPI_NTSTATUS
#undef NKT_PARSE_NTAPI_VOID
#undef NKT_PARSE_NTAPI_PVOID
#undef NKT_PARSE_NTAPI_BOOLEAN
#undef NKT_PARSE_NTAPI_ULONG

#define NKT_PARSE_NTAPI_NTSTATUS(name, parameters, paramCall) \
NTSTATUS __stdcall Nkt##name parameters                       \
{                                                             \
  if (nInitialized == 0)                                      \
    InitializeInternals();                                    \
  if (NktHookLib_fn_##name == NULL)                           \
    return STATUS_NOT_IMPLEMENTED;                            \
  return NktHookLib_fn_##name paramCall;                      \
}
#define NKT_PARSE_NTAPI_VOID(name, parameters, paramCall)     \
VOID __stdcall Nkt##name parameters                           \
{                                                             \
  if (nInitialized == 0)                                      \
    InitializeInternals();                                    \
  if (NktHookLib_fn_##name != NULL)                           \
    NktHookLib_fn_##name paramCall;                           \
  return;                                                     \
}
#define NKT_PARSE_NTAPI_PVOID(name, parameters, paramCall)    \
PVOID __stdcall Nkt##name parameters                          \
{                                                             \
  if (nInitialized == 0)                                      \
    InitializeInternals();                                    \
  if (NktHookLib_fn_##name == NULL)                           \
    return NULL;                                              \
  return NktHookLib_fn_##name paramCall;                      \
}
#define NKT_PARSE_NTAPI_BOOLEAN(name, parameters, paramCall)  \
BOOLEAN __stdcall Nkt##name parameters                        \
{                                                             \
  if (nInitialized == 0)                                      \
    InitializeInternals();                                    \
  if (NktHookLib_fn_##name == NULL)                           \
    return FALSE;                                             \
  return NktHookLib_fn_##name paramCall;                      \
}
#define NKT_PARSE_NTAPI_ULONG(name, parameters, paramCall)    \
ULONG __stdcall Nkt##name parameters                          \
{                                                             \
  if (nInitialized == 0)                                      \
    InitializeInternals();                                    \
  if (NktHookLib_fn_##name == NULL)                           \
    return 0;                                                 \
  return NktHookLib_fn_##name paramCall;                      \
}
#include "NtApiDeclarations.h"
#undef NKT_PARSE_NTAPI_NTSTATUS
#undef NKT_PARSE_NTAPI_VOID
#undef NKT_PARSE_NTAPI_PVOID
#undef NKT_PARSE_NTAPI_BOOLEAN
#undef NKT_PARSE_NTAPI_ULONG

} //Internals
} //NktHookLib

//-----------------------------------------------------------

static VOID InitializeInternals()
{
  LPVOID _hNtDll, fn;

  _hNtDll = NULL;
#if defined(_M_IX86)
  #define NKT_PARSE_NTAPI_NTSTATUS(name, _notused, _notused2)                                     \
    fn = ResolveNtDllApi(_hNtDll, # name);                                                        \
    _InterlockedExchange((long volatile*)&(NktHookLib::Internals::NktHookLib_fn_##name), (long)fn);
#elif defined(_M_X64)
  #define NKT_PARSE_NTAPI_NTSTATUS(name, _notused, _notused2)                                       \
    fn = ResolveNtDllApi(_hNtDll, # name);                                                          \
    _InterlockedExchangePointer((void* volatile*)&(NktHookLib::Internals::NktHookLib_fn_##name), fn);
#endif
#define NKT_PARSE_NTAPI_VOID NKT_PARSE_NTAPI_NTSTATUS
#define NKT_PARSE_NTAPI_PVOID NKT_PARSE_NTAPI_NTSTATUS
#define NKT_PARSE_NTAPI_BOOLEAN NKT_PARSE_NTAPI_NTSTATUS
#define NKT_PARSE_NTAPI_ULONG NKT_PARSE_NTAPI_NTSTATUS
#include "NtApiDeclarations.h"
#undef NKT_PARSE_NTAPI_NTSTATUS
#undef NKT_PARSE_NTAPI_VOID
#undef NKT_PARSE_NTAPI_PVOID
#undef NKT_PARSE_NTAPI_BOOLEAN
#undef NKT_PARSE_NTAPI_ULONG
  //----
  fn = ResolveNtDllApi(_hNtDll, "_vsnprintf");
#if defined(_M_IX86)
  _InterlockedExchange((long volatile*)&NktHookLib_fn_vsnprintf, (long)fn);
#elif defined(_M_X64)
  _InterlockedExchangePointer((void* volatile*)&NktHookLib_fn_vsnprintf, fn);
#endif
  fn = ResolveNtDllApi(_hNtDll, "_vsnwprintf");
#if defined(_M_IX86)
  _InterlockedExchange((long volatile*)&NktHookLib_fn_vsnwprintf, (long)fn);
#elif defined(_M_X64)
  _InterlockedExchangePointer((void* volatile*)&NktHookLib_fn_vsnwprintf, fn);
#endif
  fn = ResolveNtDllApi(_hNtDll, "DbgPrint");
#if defined(_M_IX86)
  _InterlockedExchange((long volatile*)&NktHookLib_fn_DbgPrint, (long)fn);
#elif defined(_M_X64)
  _InterlockedExchangePointer((void* volatile*)&NktHookLib_fn_DbgPrint, fn);
#endif
  //done
  _InterlockedExchange(&nInitialized, 1);
  return;
}

static LPVOID ResolveNtDllApi(__inout LPVOID &_hNtDll, __in_z LPCSTR szApiNameA)
{
  LPVOID fn;

  if (NktHookLibHelpers::fnInternalApiResolver != NULL)
  {
    fn = NktHookLibHelpers::fnInternalApiResolver(szApiNameA, NktHookLibHelpers::lpUserParam);
    if (fn != NULL)
      return fn;
  }
  if (_hNtDll == NULL)
  {
    //Set ScanMappedImages to FALSE to avoid recursion
   _hNtDll = NktHookLib::Internals::GetRemoteModuleBaseAddress(NKTHOOKLIB_CurrentProcess, L"ntdll.dll", FALSE);
  }
  if (_hNtDll == NULL)
    return NULL;
  return NktHookLib::Internals::GetRemoteProcedureAddress(NKTHOOKLIB_CurrentProcess, _hNtDll, szApiNameA);
}
