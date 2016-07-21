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

#include <windows.h>
#include <stdio.h>

//IMPORTANT: Temporary fully disabled due to RtlCaptureStackBackTrace trying to acquire a SRW_LOCK previously
//           acquired by the dll loader in NTDLL when the loader call to NtProtectVirtualMemory.
#ifdef _DEBUG
  //#define NKT_ENABLE_MEMORY_TRACKING
  //#define NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
#else //_DEBUG
  //#define NKT_ENABLE_MEMORY_TRACKING
  //#define NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
#endif //_DEBUG

#if _MSC_FULL_VER >= 140050214
  #define NKTMEMMGR_PRAGMASECTION(sect_name) __pragma(section(sect_name,long,read))
#else //_MSC_FULL_VER >= 140050214
  #if defined _M_IX86
    #define NKTMEMMGR_PRAGMASECTION(sect_name) __pragma(section(sect_name,long,read,write)
  #elif defined _M_X64
    #define NKTMEMMGR_PRAGMASECTION(sect_name) __pragma(section(sect_name,long,read))
  #else
    #error Unsupported platform
  #endif
#endif //_MSC_FULL_VER >= 140050214

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING

void* __cdecl _nktMemMallocWithTrack(__in size_t nSize, __in_z_opt const char *szFileName,
                                     __in int nLineNumber);
void* __cdecl _nktMemReallocWithTrack(__in void *lpMemory, __in size_t nNewSize,
                                      __in_z_opt const char *szFileName, __in int nLineNumber);
void __cdecl _nktMemFreeWithTrack(__inout void *lpMemory);
size_t _nktMemSizeWithTrack(__in void *lpMemory);

#endif //NKT_ENABLE_MEMORY_TRACKING

void* __cdecl _nktMemMalloc(__in size_t nSize);
void* __cdecl _nktMemRealloc(__in void *lpMemory, __in size_t nNewSize);
void __cdecl _nktMemFree(__inout void *lpMemory);
size_t _nktMemSize(__in void *lpMemory);

//--------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #define nktMemMalloc(nSize)                    _nktMemMallocWithTrack(nSize, __FILE__, __LINE__)
  #define nktMemRealloc(lpMemory, nNewSize)                                                      \
                                   _nktMemReallocWithTrack(lpMemory, nNewSize, __FILE__, __LINE__)
  #define nktMemFree(lpMemory)                   _nktMemFreeWithTrack(lpMemory)
  #define nktMemSize(lpMemory)                   _nktMemSizeWithTrack(lpMemory)
#else //NKT_ENABLE_MEMORY_TRACKING
  #define nktMemMalloc(nSize)                    _nktMemMalloc(nSize)
  #define nktMemRealloc(lpMemory, nNewSize)      _nktMemRealloc(lpMemory, nNewSize)
  #define nktMemFree(lpMemory)                   _nktMemFree(lpMemory)
  #define nktMemSize(lpMemory)                   _nktMemSize(lpMemory)
#endif //NKT_ENABLE_MEMORY_TRACKING

void nktMemMgrTrim();
void nktMemMgrOnThreadExit();

//-----------------------------------------------------------

void* nktMemSet(__out void *lpDest, __in SIZE_T nValue, __in SIZE_T nCount);
void* nktMemCopy(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount);
void* nktMemMove(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount);
int nktMemCompare(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount);
void* nktMemCharAnsi(__in const void *lpBuffer, __in CHAR cChar, __in SIZE_T nCount);
void* nktMemCharWide(__in const void *lpBuffer, __in WCHAR cChar, __in SIZE_T nCount);
SIZE_T nktTryMemCopy(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount);

//-----------------------------------------------------------

#include "MemoryManagerObjects.h"

//-----------------------------------------------------------

