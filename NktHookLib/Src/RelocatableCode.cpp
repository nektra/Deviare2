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

#include "RelocatableCode.h"
static
#include <RelocatableCode_x86.inl>
static
#include <RelocatableCode_x64.inl>

namespace NktHookLib {
namespace Internals {
namespace RelocatableCode {

//-----------------------------------------------------------

SIZE_T GetModuleAndProcAddr_GetSize(__in LONG nPlatform)
{
  switch (nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return RelocatableCodeX86_GETMODULEANDPROCADDR_SECTION_END_OFFSET -
             RelocatableCodeX86_GETMODULEANDPROCADDR_SECTION_START_OFFSET;

    case NKTHOOKLIB_ProcessPlatformX64:
      return RelocatableCodeX64_GETMODULEANDPROCADDR_SECTION_END_OFFSET -
             RelocatableCodeX64_GETMODULEANDPROCADDR_SECTION_START_OFFSET;
  }
  return 0;
}

LPBYTE GetModuleAndProcAddr_GetCode(__in LONG nPlatform, __out GETMODULEANDPROCADDR_DATA &sAddresses)
{
  switch ( nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      sAddresses.nOffset_GetModuleBaseAddress = RelocatableCodeX86_GetModuleBaseAddress_OFFSET;
      sAddresses.nOffset_GetProcedureAddress = RelocatableCodeX86_GetProcedureAddress_OFFSET;
      return &RelocatableCodeX86[RelocatableCodeX86_GETMODULEANDPROCADDR_SECTION_START_OFFSET];

    case NKTHOOKLIB_ProcessPlatformX64:
      sAddresses.nOffset_GetModuleBaseAddress = RelocatableCodeX64_GetModuleBaseAddress_OFFSET;
      sAddresses.nOffset_GetProcedureAddress = RelocatableCodeX64_GetProcedureAddress_OFFSET;
      return &RelocatableCodeX64[RelocatableCodeX64_GETMODULEANDPROCADDR_SECTION_START_OFFSET];
  }
  sAddresses.nOffset_GetModuleBaseAddress = sAddresses.nOffset_GetProcedureAddress = 0;
  return 0;
}

//----------------

SIZE_T InjectDllInNewProcess_GetSize(__in LONG nPlatform)
{
  switch (nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return RelocatableCodeX86_INJECTDLLINNEWPROCESS_SECTION_END_OFFSET -
             RelocatableCodeX86_INJECTDLLINNEWPROCESS_SECTION_START_OFFSET;

    case NKTHOOKLIB_ProcessPlatformX64:
      return RelocatableCodeX64_INJECTDLLINNEWPROCESS_SECTION_END_OFFSET -
             RelocatableCodeX64_INJECTDLLINNEWPROCESS_SECTION_START_OFFSET;
  }
  return 0;
}

LPBYTE InjectDllInNewProcess_GetCode(__in LONG nPlatform)
{
  switch ( nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return &RelocatableCodeX86[RelocatableCodeX86_INJECTDLLINNEWPROCESS_SECTION_START_OFFSET];

    case NKTHOOKLIB_ProcessPlatformX64:
      return &RelocatableCodeX64[RelocatableCodeX64_INJECTDLLINNEWPROCESS_SECTION_START_OFFSET];
  }
  return 0;
}

//----------------

SIZE_T InjectDllInRunningProcess_GetSize(__in LONG nPlatform)
{
  switch (nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return RelocatableCodeX86_INJECTDLLINRUNNINGPROCESS_SECTION_END_OFFSET -
             RelocatableCodeX86_INJECTDLLINRUNNINGPROCESS_SECTION_START_OFFSET;

    case NKTHOOKLIB_ProcessPlatformX64:
      return RelocatableCodeX64_INJECTDLLINRUNNINGPROCESS_SECTION_END_OFFSET -
             RelocatableCodeX64_INJECTDLLINRUNNINGPROCESS_SECTION_START_OFFSET;
  }
  return 0;
}

LPBYTE InjectDllInRunningProcess_GetCode(__in LONG nPlatform)
{
  switch ( nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return &RelocatableCodeX86[RelocatableCodeX86_INJECTDLLINRUNNINGPROCESS_SECTION_START_OFFSET];

    case NKTHOOKLIB_ProcessPlatformX64:
      return &RelocatableCodeX64[RelocatableCodeX64_INJECTDLLINRUNNINGPROCESS_SECTION_START_OFFSET];
  }
  return 0;
}

//----------------

SIZE_T WaitForEventAtStartup_GetSize(__in LONG nPlatform)
{
  switch (nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return RelocatableCodeX86_WAITFOREVENTATSTARTUP_SECTION_END_OFFSET -
             RelocatableCodeX86_WAITFOREVENTATSTARTUP_SECTION_START_OFFSET;

    case NKTHOOKLIB_ProcessPlatformX64:
      return RelocatableCodeX64_WAITFOREVENTATSTARTUP_SECTION_END_OFFSET -
             RelocatableCodeX64_WAITFOREVENTATSTARTUP_SECTION_START_OFFSET;
  }
  return 0;
}

LPBYTE WaitForEventAtStartup_GetCode(__in LONG nPlatform)
{
  switch ( nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      return &RelocatableCodeX86[RelocatableCodeX86_WAITFOREVENTATSTARTUP_SECTION_START_OFFSET];

    case NKTHOOKLIB_ProcessPlatformX64:
      return &RelocatableCodeX64[RelocatableCodeX64_WAITFOREVENTATSTARTUP_SECTION_START_OFFSET];
  }
  return 0;
}

//-----------------------------------------------------------

} //RelocatableCode
} //Internals
} //NktHookLib
