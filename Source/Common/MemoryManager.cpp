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

#include "MemoryManager.h"
#include "MemoryManagerTracker.h"
#include "MemoryManagerPool.h"

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef SL_THIS_FILE
  static char SL_THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

static void __cdecl MEMMGR_Finalizer(); //in section .CRT$XTV, for execution after MEMTRACKER_Finalizer

NKTMEMMGR_PRAGMASECTION(".CRT$XTV500");
#pragma data_seg(".CRT$XTV500")
static __declspec(allocate(".CRT$XTV500")) void (__cdecl *aMEMMGR_Finalizers[])(void) = {
  &MEMMGR_Finalizer
};
#pragma data_seg()

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING

void* __cdecl _nktMemMallocWithTrack(__in size_t nSize, __in_z_opt const char *szFileName,
                                     __in int nLineNumber)
{
  LPBYTE p;

#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (nktMemTracker() == NULL)
    return NULL;
  nSize = ((nSize+sizeof(SIZE_T)-1) & (~(sizeof(SIZE_T)-1))) + NKT_MEMORY_TRACKING_BLOCK_SIZE +
          sizeof(SIZE_T);
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  nSize += 2*sizeof(SIZE_T);
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  //allocate memory
  p = (LPBYTE)(memmgrpool::Malloc(nSize));
  if (p != NULL)
  {
    //the real allocated size may be greater than the requested size
    nSize = memmgrpool::BlockSize(p);
  }
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (p != NULL)
  {
    nktMemTracker()->AddTrace(p, nSize-(NKT_MEMORY_TRACKING_BLOCK_SIZE+sizeof(SIZE_T)), szFileName,
                              nLineNumber);
    p += NKT_MEMORY_TRACKING_BLOCK_SIZE;
  }
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (p != NULL)
  {
    ((SIZE_T*)p)[0] = (SIZE_T)szFileName;
    ((SIZE_T*)p)[1] = (SIZE_T)nLineNumber;
    p += 2*sizeof(SIZE_T);
  }
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  return p;
}

void* __cdecl _nktMemReallocWithTrack(__in void *lpMemory, __in size_t nNewSize,
                                      __in_z_opt const char *szFileName, __in int nLineNumber)
{
  LPBYTE p, lpOld;

  if (nNewSize == 0)
  {
    _nktMemFreeWithTrack(lpMemory);
    return NULL;
  }
  lpOld = (LPBYTE)lpMemory;
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (nktMemTracker() == NULL)
    return NULL;
  nNewSize = ((nNewSize+sizeof(SIZE_T)-1) & (~(sizeof(SIZE_T)-1))) + NKT_MEMORY_TRACKING_BLOCK_SIZE +
             sizeof(SIZE_T);
  if (lpOld != NULL)
  {
    lpOld -= NKT_MEMORY_TRACKING_BLOCK_SIZE;
    nktMemTracker()->RemoveTrace(lpOld);
  }
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  nNewSize += 2*sizeof(SIZE_T);
  if (lpOld != NULL)
    lpOld -= 2*sizeof(SIZE_T);
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  //reallocate memory
  p = (LPBYTE)memmgrpool::Realloc(lpOld, nNewSize);
  if (p != NULL)
  {
    //the real allocated size may be greater than the requested size
    nNewSize = memmgrpool::BlockSize(p);
  }
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (p != NULL)
  {
    nktMemTracker()->AddTrace(p, nNewSize-(NKT_MEMORY_TRACKING_BLOCK_SIZE+sizeof(SIZE_T)), szFileName,
                              nLineNumber);
    p += NKT_MEMORY_TRACKING_BLOCK_SIZE;
  }
  else
  {
    nktMemTracker()->AddTrace(lpOld, ((CNktMemMgrTracker::LPMEMTRACE_BLOCK)lpOld)->nBlockSize,
                              ((CNktMemMgrTracker::LPMEMTRACE_BLOCK)lpOld)->szFileName,
                              (int)((CNktMemMgrTracker::LPMEMTRACE_BLOCK)lpOld)->nLineNumber);
  }
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  if (p != NULL)
  {
    ((SIZE_T*)p)[0] = (SIZE_T)szFileName;
    ((SIZE_T*)p)[1] = (SIZE_T)nLineNumber;
    p += 2*sizeof(SIZE_T);
  }
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  return p;
}

void __cdecl _nktMemFreeWithTrack(__inout void *lpMemory)
{
  LPBYTE p;

  if (lpMemory == NULL)
    return;
  p = (LPBYTE)lpMemory;
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  p -= NKT_MEMORY_TRACKING_BLOCK_SIZE;
  if (nktMemTrackerNoCreate() != NULL)
    nktMemTracker()->RemoveTrace(p);
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  p -= 2*sizeof(SIZE_T);
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  //free memory
  memmgrpool::Free(p);
  return;
}

size_t _nktMemSizeWithTrack(__in void *lpMemory)
{
  LPBYTE p;
  SIZE_T nSize;

  if (lpMemory == NULL)
    return 0;
  p = (LPBYTE)lpMemory;
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  p -= NKT_MEMORY_TRACKING_BLOCK_SIZE;
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  p -= 2*sizeof(SIZE_T);
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  //get memory size
  nSize = memmgrpool::BlockSize(p);
  //tracking
#ifndef NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  nSize = (nSize >= NKT_MEMORY_TRACKING_BLOCK_SIZE) ? (nSize-NKT_MEMORY_TRACKING_BLOCK_SIZE) : 0;
#else //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  nSize = (nSize >= (2*sizeof(SIZE_T))) ? (nSize-(2*sizeof(SIZE_T))) : 0;
#endif //NKT_ENABLE_MEMORY_TRACKING_ONLY_POINTERS
  return nSize;
}

#endif //NKT_ENABLE_MEMORY_TRACKING

void* __cdecl _nktMemMalloc(__in size_t nSize)
{
  //allocate memory
  return memmgrpool::Malloc(nSize);
}

void* __cdecl _nktMemRealloc(__in void *lpMemory, __in size_t nNewSize)
{
  //reallocate memory
  return memmgrpool::Realloc(lpMemory, nNewSize);
}

void __cdecl _nktMemFree(__inout void *lpMemory)
{
  //free memory
  memmgrpool::Free(lpMemory);
  return;
}

size_t __cdecl _nktMemSize(__in void *lpMemory)
{
  //get memory size
  return memmgrpool::BlockSize(lpMemory);
}

void nktMemMgrOnThreadExit()
{
  memmgrpool::OnThreadExit();
  return;
}

static void __cdecl MEMMGR_Finalizer()
{
  memmgrpool::OnProcessExit();
  return;
}

//-----------------------------------------------------------
//-----------------------------------------------------------

#define XISALIGNED(x)  ((((SIZE_T)(x)) & (sizeof(SIZE_T)-1)) == 0)

void* nktMemSet(__inout void *lpDest, __in SIZE_T nValue, __in SIZE_T nCount)
{
  LPBYTE d;

  d = (LPBYTE)lpDest;
  if (XISALIGNED(d))
  {
    nValue &= (SIZE_T)(0xFF);
    nValue |= (nValue << 8);
    nValue |= (nValue << 16);
#if defined _M_X64
    nValue |= (nValue << 32);
#endif //_M_X64
    while (nCount >= sizeof(SIZE_T))
    {
      *((SIZE_T*)d) = nValue;
      d += sizeof(SIZE_T);
      nCount -= sizeof(SIZE_T);
    }
  }
  while ((nCount--) > 0)
    *d++ = (BYTE)nValue;
  return lpDest;
}

void* nktMemCopy(__inout void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  LPBYTE s, d;

  s = (LPBYTE)lpSrc;
  d = (LPBYTE)lpDest;
  if (XISALIGNED(s) && XISALIGNED(d))
  {
    while (nCount >= sizeof(SIZE_T))
    {
      *((SIZE_T*)d) = *((SIZE_T*)s);
      s += sizeof(SIZE_T);
      d += sizeof(SIZE_T);
      nCount -= sizeof(SIZE_T);
    }
  }
  while ((nCount--) > 0)
    *d++ = *s++;
  return lpDest;
}

void* nktMemMove(__inout void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  LPBYTE s, d;

  s = (LPBYTE)lpSrc;
  d = (LPBYTE)lpDest;
  if (d<=s || d>=(s+nCount))
  {
    //dest is before source or non-overlapping buffers
    //copy from lower to higher addresses
    if (d+sizeof(SIZE_T)<=s && XISALIGNED(s) && XISALIGNED(d))
    {
      while (nCount >= sizeof(SIZE_T))
      {
        *((SIZE_T*)d) = *((SIZE_T*)s);
        s += sizeof(SIZE_T);
        d += sizeof(SIZE_T);
        nCount -= sizeof(SIZE_T);
      }
    }
    while ((nCount--) > 0)
      *d++ = *s++;
  }
  else
  {
    //dest is past source or overlapping buffers
    //copy from higher to lower addresses
    if (nCount>=sizeof(SIZE_T) && XISALIGNED(s) && XISALIGNED(d))
    {
      s += nCount;
      d += nCount;
      while (nCount>0 && (!XISALIGNED(nCount)))
      {
        --s;
        --d;
        *d = *s;
        nCount--;
      }
      _ASSERT(XISALIGNED(s) && XISALIGNED(d));
      while (nCount > 0)
      {
        s -= sizeof(SIZE_T);
        d -= sizeof(SIZE_T);
        *((SIZE_T*)d) = *((SIZE_T*)s);
        nCount -= sizeof(SIZE_T);
      }
    }
    else
    {
      s += nCount;
      d += nCount;
      while (nCount > 0)
      {
        --s;
        --d;
        *d = *s;
        nCount--;
      }
    }
  }
  return lpDest;
}

int nktMemCompare(__in const void *lpBuf1, __in const void *lpBuf2, __in SIZE_T nCount)
{
  LPBYTE b1, b2;

  if (nCount == 0)
    return 0;
  b1 = (LPBYTE)lpBuf1;
  b2 = (LPBYTE)lpBuf2;
  if (XISALIGNED(b1) && XISALIGNED(b2))
  {
    while (nCount > sizeof(SIZE_T))
    {
      if (*((SIZE_T*)b1) != *((SIZE_T*)b2))
        break;
      b1 += sizeof(SIZE_T);
      b2 += sizeof(SIZE_T);
      nCount -= sizeof(SIZE_T);
    }
  }
  while ((--nCount)>0 && *b1==*b2)
  {
    b1++;
    b2++;
  }
  return ((int)(unsigned int)(*b1)) - ((int)(unsigned int)(*b2));
}

void* nktMemCharAnsi(__in const void *lpBuffer, __in CHAR cChar, __in SIZE_T nCount)
{
  LPSTR s;

  s = (LPSTR)lpBuffer;
  while (nCount > 0)
  {
    if (*s == cChar)
      return s;
    s++;
    nCount--;
  }
  return NULL;
}

void* nktMemCharWide(__in const void *lpBuffer, __in WCHAR cChar, __in SIZE_T nCount)
{
  LPWSTR s;

  s = (LPWSTR)lpBuffer;
  while (nCount > 0)
  {
    if (*s == cChar)
      return s;
    s++;
    nCount--;
  }
  return NULL;
}

SIZE_T nktTryMemCopy(__out void *lpDest, __in const void *lpSrc, __in SIZE_T nCount)
{
  LPBYTE s, d;
  SIZE_T nOrigCount;

  s = (LPBYTE)lpSrc;
  d = (LPBYTE)lpDest;
  nOrigCount = nCount;
  __try
  {
    if (XISALIGNED(s) && XISALIGNED(d))
    {
      while (nCount >= sizeof(SIZE_T))
      {
        *((SIZE_T*)d) = *((SIZE_T*)s);
        s += sizeof(SIZE_T);
        d += sizeof(SIZE_T);
        nCount -= sizeof(SIZE_T);
      }
    }
    while (nCount > 0)
    {
      *d++ = *s++;
      nCount--;
    }
  }
  __except (EXCEPTION_EXECUTE_HANDLER)
  { }
  return nOrigCount-nCount;
}
