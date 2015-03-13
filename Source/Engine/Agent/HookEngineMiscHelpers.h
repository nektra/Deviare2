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

#ifndef _NKT_DV_HOOKENGINE_MISCHELPERS_H
#define _NKT_DV_HOOKENGINE_MISCHELPERS_H

#include "HookEngineInternals.h"

//-----------------------------------------------------------

LPBYTE HookEng_SkipJumpInstructions(__in LPBYTE lpPtr);
HRESULT HookEng_CreateNewStub(__inout CHookEntry *lpEntry, __in LPVOID lpProcToHook, __in BOOL bSkipJumps);
HRESULT HookEng_CreateNewStub2(__inout BYTE aOriginalStub[HOOKENG_MAX_ORIGINAL_STUB_SIZE],
                               __inout SIZE_T& nOriginalStubSize, __inout BYTE aNewStub[HOOKENG_MAX_STUB_SIZE],
                               __inout SIZE_T& nNewStubSize, __in LPVOID lpProcToHook, __in BOOL bSkipJumps);
HRESULT HookEng_FindDll(__out HINSTANCE *lphDll, __in LPVOID lpAddress);
HRESULT HookEng_FindDllAndIncrementUsageCount(__out HINSTANCE *lphDll, __in LPVOID lpAddress);

//-----------------------------------------------------------

static __inline ULONG HookEng_ReadUnalignedULong(__in LPVOID lpAddr)
{
  return ((ULONG)(*( (LPBYTE)lpAddr)   )      ) |
         ((ULONG)(*(((LPBYTE)lpAddr)+1)) <<  8) |
         ((ULONG)(*(((LPBYTE)lpAddr)+2)) << 16) |
         ((ULONG)(*(((LPBYTE)lpAddr)+3)) << 24);
}

static __inline VOID HookEng_WriteUnalignedULong(__in LPVOID lpAddr, __in ULONG nVal)
{
  *( (LPBYTE)lpAddr   ) = (BYTE)( nVal        & 0xFF);
  *(((LPBYTE)lpAddr)+1) = (BYTE)((nVal >>  8) & 0xFF);
  *(((LPBYTE)lpAddr)+2) = (BYTE)((nVal >> 16) & 0xFF);
  *(((LPBYTE)lpAddr)+3) = (BYTE)((nVal >> 24) & 0xFF);
  return;
}

static __inline ULONGLONG HookEng_ReadUnalignedULongLong(__in LPVOID lpAddr)
{
  return ((ULONGLONG)(*( (LPBYTE)lpAddr)   )      ) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+1)) <<  8) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+2)) << 16) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+3)) << 24) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+4)) << 32) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+5)) << 40) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+6)) << 48) |
         ((ULONGLONG)(*(((LPBYTE)lpAddr)+7)) << 56);
}

static __inline VOID HookEng_WriteUnalignedULongLong(__in LPVOID lpAddr, __in ULONGLONG nVal)
{
  *( (LPBYTE)lpAddr   ) = (BYTE)( nVal        & 0xFF);
  *(((LPBYTE)lpAddr)+1) = (BYTE)((nVal >>  8) & 0xFF);
  *(((LPBYTE)lpAddr)+2) = (BYTE)((nVal >> 16) & 0xFF);
  *(((LPBYTE)lpAddr)+3) = (BYTE)((nVal >> 24) & 0xFF);
  *(((LPBYTE)lpAddr)+4) = (BYTE)((nVal >> 32) & 0xFF);
  *(((LPBYTE)lpAddr)+5) = (BYTE)((nVal >> 40) & 0xFF);
  *(((LPBYTE)lpAddr)+6) = (BYTE)((nVal >> 48) & 0xFF);
  *(((LPBYTE)lpAddr)+7) = (BYTE)((nVal >> 56) & 0xFF);
  return;
}

#if defined _M_IX86
  #define HookEng_ReadUnalignedSizeT HookEng_ReadUnalignedULong
  #define HookEng_WriteUnalignedSizeT HookEng_WriteUnalignedULong
#elif defined _M_X64
  #define HookEng_ReadUnalignedSizeT HookEng_ReadUnalignedULongLong
  #define HookEng_WriteUnalignedSizeT HookEng_WriteUnalignedULongLong
#endif

//-----------------------------------------------------------

#endif //_NKT_DV_HOOKENGINE_MISCHELPERS_H
