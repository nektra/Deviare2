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

#include "MemoryManagerPool.h"
#include <malloc.h>
#include <crtdbg.h>
#include <intrin.h>
#include "WaitableObjects.h"
#include "Debug.h"
#include "NtInternals.h"
#include "RegistrySettings.h"
#include <new.h>

#undef new

#pragma intrinsic(_BitScanReverse)

//-----------------------------------------------------------

#define SUPERCHUNK_SIZE                ((SIZE_T)(2*1048576))
#define BINS_COUNT                                        12
#if defined _M_IX86
  #define TAG_MARK                              0xDE324F1AUL
#elif defined _M_X64
  #define TAG_MARK                    0xDE324F1A8492B311ui64
#else
  #error Unsupported platform
#endif

#ifdef _DEBUG
  #define DO_SANITY_CHECK
#endif //_DEBUG

//-----------------------------------------------------------

typedef struct tagDLLIST_ITEM {
  struct tagDLLIST_ITEM *lpNext, *lpPrev;
} DLLIST_ITEM, *LPDLLIST_ITEM;

//-----------------------------------------------------------

class CNktMemMgrSuperChunk : protected CNktFastMutex
{
public:
  explicit CNktMemMgrSuperChunk();
  virtual ~CNktMemMgrSuperChunk();

  LPVOID Alloc();
  VOID Free(__in LPVOID lpPtr);

  virtual VOID Cleanup();

private:
  typedef struct tagSUPERCHUNK {
    DLLIST_ITEM sLink;
    DWORD dwLastInUseTime;
    DWORD dwFreeCount;
    SIZE_T* lpFirstFreeBlock;
    LPBYTE lpBlock;
  } SUPERCHUNK, *LPSUPERCHUNK;

  HANDLE hHeap;
  DLLIST_ITEM sFreeList, sInUseList, sFullList;

  VOID Trim(__in DWORD dwTime);
#ifdef DO_SANITY_CHECK
  VOID SanityCheck(__in LPSUPERCHUNK lpSuperChunk);
#endif //DO_SANITY_CHECK
};

//-----------------------------------------------------------

class CNktMemMgrBin : protected CNktFastMutex
{
public:
  explicit CNktMemMgrBin(__in DWORD dwBlockSize);
  virtual ~CNktMemMgrBin();

  static LPVOID Alloc(__in SIZE_T nSize);
  static VOID Free(__in LPVOID lpPtr);
  static SIZE_T GetSize(__in LPVOID lpPtr);

  virtual LPVOID SmallHeapAlloc();
  virtual VOID SmallHeapFree(__in LPVOID lpPtr);
  virtual SIZE_T SmallHeapGetSize()
    {
    return (SIZE_T)dwBlockSize;
    };

  virtual VOID Cleanup();

private:
  typedef struct tagHEADER {
    union {
      SIZE_T nTag;
      ULONGLONG _pad0;
    };
    DWORD dwLastInUseTime;
    DWORD dwFreeCount;
    DLLIST_ITEM sLink;
    CNktMemMgrBin *lpBinOwner;
    SIZE_T* lpFirstFreeBlock;
  } HEADER, *LPHEADER;

  typedef struct tagCHUNK {
    HEADER sHeader;
    BYTE aBuffer[0x10000-sizeof(HEADER)];
  } CHUNK, *LPCHUNK;

  DWORD dwBlockSize, dwBlocksPerChunk;
  DWORD dwCheckFreqCounter;
  DLLIST_ITEM sFreeList, sInUseList, sFullList;

  virtual VOID Trim(__in DWORD dwTime);
#ifdef DO_SANITY_CHECK
  virtual VOID SanityCheck(__in LPCHUNK lpChunk);
#endif //DO_SANITY_CHECK
};

#define CHUNK_FROM_DLLIST(ptr) (LPCHUNK)((LPBYTE)ptr - FIELD_OFFSET(CHUNK, sHeader.sLink))
#define SUPERCHUNK_FROM_DLLIST(ptr) (LPSUPERCHUNK)((LPBYTE)ptr - FIELD_OFFSET(SUPERCHUNK, sLink))

//-----------------------------------------------------------

static LONG volatile nBinMtx = 0;
static CNktMemMgrBin* lpBins[BINS_COUNT] = { 0 };
static BYTE aBinMem[BINS_COUNT][sizeof(CNktMemMgrBin)];
static CNktMemMgrSuperChunk* lpSuperChunkMgr = NULL;
static BYTE aSuperChunkMem[sizeof(CNktMemMgrSuperChunk)];

//-----------------------------------------------------------

static VOID DlListInsertHead(__inout LPDLLIST_ITEM lpList, __inout LPDLLIST_ITEM lpItem);
static VOID DlListInsertTail(__inout LPDLLIST_ITEM lpList, __inout LPDLLIST_ITEM lpItem);
static VOID DlListRemove(__inout LPDLLIST_ITEM lpItem);
static SIZE_T IndexFromSize(__in SIZE_T nSize);

//-----------------------------------------------------------

namespace memmgrpool
{

LPVOID Malloc(__in SIZE_T nSize)
{
  return CNktMemMgrBin::Alloc(nSize);
}

LPVOID Realloc(__in LPVOID lpOld, __in SIZE_T nNewSize)
{
  LPVOID lpNewPtr;
  SIZE_T nOrigSize;

  if (lpOld == NULL)
    return Malloc(nNewSize);
  if (nNewSize == 0)
  {
    Free(lpOld);
    return NULL;
  }
  //get info from original memory
  nOrigSize = BlockSize(lpOld);
  //reallocate
  lpNewPtr = Malloc(nNewSize);
  if (lpNewPtr != NULL)
  {
    nktMemCopy(lpNewPtr, lpOld, (nOrigSize < nNewSize) ? nOrigSize : nNewSize);
    Free(lpOld);
  }
  return lpNewPtr;
}

VOID Free(__in LPVOID lpPtr)
{
  CNktMemMgrBin::Free(lpPtr);
  return;
}

SIZE_T BlockSize(__in LPVOID lpPtr)
{
  return CNktMemMgrBin::GetSize(lpPtr);
}

VOID OnThreadExit()
{
  return;
}

VOID OnProcessExit()
{
  BOOL bShutdownInProgress;

  nktDvDynApis_RtlDllShutdownInProgress(&bShutdownInProgress);
  if (bShutdownInProgress == FALSE)
  {
    CNktSimpleLockNonReentrant cLock(&nBinMtx);
    SIZE_T i;

    for (i=0; i<BINS_COUNT; i++)
    {
      if (lpBins[i] != NULL)
      {
        lpBins[i]->~CNktMemMgrBin();
        lpBins[i] = NULL;
      }
    }
    if (lpSuperChunkMgr != NULL)
    {
      lpSuperChunkMgr->~CNktMemMgrSuperChunk();
      lpSuperChunkMgr = NULL;
    }
  }
  return;
}

};

//-----------------------------------------------------------

static VOID DlListInsertHead(__inout LPDLLIST_ITEM lpList, __inout LPDLLIST_ITEM lpItem)
{
  NKT_ASSERT(lpItem->lpNext == lpItem && lpItem->lpPrev == lpItem);
  lpItem->lpNext = lpList->lpNext;
  lpItem->lpPrev = lpList;
  lpList->lpNext->lpPrev = lpItem;
  lpList->lpNext = lpItem;
  return;
}

static VOID DlListInsertTail(__inout LPDLLIST_ITEM lpList, __inout LPDLLIST_ITEM lpItem)
{
  NKT_ASSERT(lpItem->lpNext == lpItem && lpItem->lpPrev == lpItem);
  lpItem->lpNext = lpList;
  lpItem->lpPrev = lpList->lpPrev;
  lpList->lpPrev->lpNext = lpItem;
  lpList->lpPrev = lpItem;
  return;
}

static VOID DlListRemove(__inout LPDLLIST_ITEM lpItem)
{
  lpItem->lpPrev->lpNext = lpItem->lpNext;
  lpItem->lpNext->lpPrev = lpItem->lpPrev;
  lpItem->lpNext = lpItem->lpPrev = lpItem;
  return;
}

static SIZE_T IndexFromSize(__in SIZE_T nSize)
{
  unsigned long _ndx;

  if (nSize >= 32768)
    return BINS_COUNT;
  if (nSize < 8)
    nSize = 8;
  _BitScanReverse(&_ndx, (unsigned long)nSize);
  if ((nSize & (nSize-1)) != 0)
    _ndx++;
  return _ndx-3;
}

//-----------------------------------------------------------

CNktMemMgrSuperChunk::CNktMemMgrSuperChunk() : CNktFastMutex()
{
  NKT_ASSERT((SUPERCHUNK_SIZE & 0xFFFF) == 0);
  hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
  sFreeList.lpNext = sFreeList.lpPrev = &sFreeList;
  sInUseList.lpNext = sInUseList.lpPrev = &sInUseList;
  sFullList.lpNext = sFullList.lpPrev = &sFullList;
  return;
}

CNktMemMgrSuperChunk::~CNktMemMgrSuperChunk()
{
  Cleanup();
  if (hHeap != NULL)
    ::HeapDestroy(hHeap);
  return;
}

LPVOID CNktMemMgrSuperChunk::Alloc()
{
  CNktAutoFastMutex cLock(this);
  LPSUPERCHUNK lpSuperChunk;
  SIZE_T *lpPtr;
  DWORD dw;

  if (hHeap == NULL)
    return NULL;
  if (sInUseList.lpNext == &sInUseList)
  {
    //check if we can get a chunk from the free list
    if (sFreeList.lpNext != &sFreeList)
    {
      lpSuperChunk = SUPERCHUNK_FROM_DLLIST(sFreeList.lpNext);
      DlListRemove(&(lpSuperChunk->sLink));
      DlListInsertHead(&sInUseList, &(lpSuperChunk->sLink));
    }
    else
    {
      //else create a new one
      lpSuperChunk = (LPSUPERCHUNK)::HeapAlloc(hHeap, 0, sizeof(SUPERCHUNK));
      if (lpSuperChunk == NULL)
        return NULL;
      nktMemSet(lpSuperChunk, 0, sizeof(SUPERCHUNK));
      lpSuperChunk->lpBlock = (LPBYTE)::VirtualAlloc(NULL, SUPERCHUNK_SIZE, MEM_RESERVE|MEM_COMMIT|
                                                     MEM_TOP_DOWN, PAGE_READWRITE);
      if (lpSuperChunk->lpBlock == NULL)
      {
        ::HeapFree(hHeap, 0, lpSuperChunk);
        return NULL;
      }
      lpSuperChunk->dwFreeCount = SUPERCHUNK_SIZE / 0x10000;
      lpSuperChunk->sLink.lpNext = lpSuperChunk->sLink.lpPrev = &(lpSuperChunk->sLink);
      lpPtr = lpSuperChunk->lpFirstFreeBlock = (SIZE_T*)(lpSuperChunk->lpBlock);
      //init buffers internal links
      for (dw=lpSuperChunk->dwFreeCount; dw>1; dw--)
      {
        *lpPtr = (SIZE_T)lpPtr + 0x10000;
        lpPtr = (SIZE_T*)(*lpPtr);
      }
      *lpPtr = 0; //last is null
      //insert block into available list
      DlListInsertHead(&sInUseList, &(lpSuperChunk->sLink));
    }
  }
  //get a chunk from the available list
  NKT_ASSERT(sInUseList.lpNext != &sInUseList);
  lpSuperChunk = SUPERCHUNK_FROM_DLLIST(sInUseList.lpNext);
  //get the block to return
  lpPtr = lpSuperChunk->lpFirstFreeBlock;
  //decrement free count
  (lpSuperChunk->dwFreeCount)--;
  //set the pointer to the new block
  lpSuperChunk->lpFirstFreeBlock = (SIZE_T*)(*lpPtr);
  if (lpSuperChunk->lpFirstFreeBlock == NULL)
  {
    NKT_ASSERT(lpSuperChunk->dwFreeCount == 0);
    //if chunk is full, move it to the full list
    DlListRemove(&(lpSuperChunk->sLink));
    DlListInsertHead(&sFullList, &(lpSuperChunk->sLink));
  }
#ifdef DO_SANITY_CHECK
  SanityCheck(lpSuperChunk);
#endif //DO_SANITY_CHECK
  //done
  return lpPtr;
}

VOID CNktMemMgrSuperChunk::Free(__in LPVOID lpPtr)
{
  if (lpPtr != NULL)
  {
    CNktAutoFastMutex cLock(this);
    LPSUPERCHUNK lpSuperChunk;
    LPDLLIST_ITEM lpItem;
    BOOL bFullList;

    NKT_ASSERT(((SIZE_T)lpPtr & 0xFFFF) == 0); //ensure it is 64k aligned
    //find to which chunk "lpPtr" belongs to
    bFullList = FALSE;
    for (lpItem=sInUseList.lpNext; lpItem!=&sInUseList; lpItem=lpItem->lpNext)
    {
      lpSuperChunk = SUPERCHUNK_FROM_DLLIST(lpItem);
      if ((LPBYTE)lpPtr >= lpSuperChunk->lpBlock &&
          (LPBYTE)lpPtr < lpSuperChunk->lpBlock+SUPERCHUNK_SIZE)
        break;
    }
    if (lpItem == &sInUseList)
    {
      bFullList = TRUE;
      for (lpItem=sFullList.lpNext; lpItem!=&sFullList; lpItem=lpItem->lpNext)
      {
        lpSuperChunk = SUPERCHUNK_FROM_DLLIST(lpItem);
        if ((LPBYTE)lpPtr >= lpSuperChunk->lpBlock &&
            (LPBYTE)lpPtr < lpSuperChunk->lpBlock+SUPERCHUNK_SIZE)
          break;
      }
      if (lpItem == &sFullList)
      {
        NKT_ASSERT(FALSE);
        return;
      }
    }
    //add to the free block list
    *((SIZE_T*)lpPtr) = (SIZE_T)(lpSuperChunk->lpFirstFreeBlock);
    lpSuperChunk->lpFirstFreeBlock = (SIZE_T*)lpPtr;
    //increment free count
    (lpSuperChunk->dwFreeCount)++;
    if (lpSuperChunk->dwFreeCount >= (SUPERCHUNK_SIZE/0x10000))
    {
      //chunk is completely free so move to the free list
      DlListRemove(&(lpSuperChunk->sLink));
      DlListInsertHead(&sFreeList, &(lpSuperChunk->sLink));
    }
    else if (bFullList != FALSE)//lpSuperChunk->dwFreeCount > (SUPERCHUNK_SIZE/0x10000)/4)
    {
      //the chunk now has 1/4 of its blocks free so move it to the in-use list
      DlListRemove(&(lpSuperChunk->sLink));
      DlListInsertHead(&sInUseList, &(lpSuperChunk->sLink));
    }
    //set last used time
    lpSuperChunk->dwLastInUseTime = ::GetTickCount();
    Trim((lpSuperChunk->dwLastInUseTime != 0) ? (lpSuperChunk->dwLastInUseTime) : 0);
#ifdef DO_SANITY_CHECK
    SanityCheck(lpSuperChunk);
#endif //DO_SANITY_CHECK
  }
  return;
}

VOID CNktMemMgrSuperChunk::Cleanup()
{
  LPSUPERCHUNK lpSuperChunk;

  while (sFullList.lpNext != &sFullList)
  {
    lpSuperChunk = SUPERCHUNK_FROM_DLLIST(sFullList.lpNext);
    DlListRemove(&(lpSuperChunk->sLink));
    ::VirtualFree(lpSuperChunk->lpBlock, 0, MEM_RELEASE);
    ::HeapFree(hHeap, 0, lpSuperChunk);
  }
  while (sInUseList.lpNext != &sInUseList)
  {
    lpSuperChunk = SUPERCHUNK_FROM_DLLIST(sInUseList.lpNext);
    DlListRemove(&(lpSuperChunk->sLink));
    ::VirtualFree(lpSuperChunk->lpBlock, 0, MEM_RELEASE);
    ::HeapFree(hHeap, 0, lpSuperChunk);
  }
  while (sFreeList.lpNext != &sFreeList)
  {
    lpSuperChunk = SUPERCHUNK_FROM_DLLIST(sFreeList.lpNext);
    DlListRemove(&(lpSuperChunk->sLink));
    ::VirtualFree(lpSuperChunk->lpBlock, 0, MEM_RELEASE);
    ::HeapFree(hHeap, 0, lpSuperChunk);
  }
  return;
}

VOID CNktMemMgrSuperChunk::Trim(__in DWORD dwTime)
{
  LPDLLIST_ITEM lpItem, lpPrevItem;
  LPSUPERCHUNK lpSuperChunk;
  DWORD dw, dwMaxAge;

  dwMaxAge = CNktDvRegistrySettings::GetMemMgrPoolMaxAge();
  for (lpItem=sFreeList.lpPrev; lpItem!=&sFreeList; lpItem=lpPrevItem)
  {
    lpPrevItem = lpItem->lpPrev;
    lpSuperChunk = SUPERCHUNK_FROM_DLLIST(lpItem);
    if (dwTime >= lpSuperChunk->dwLastInUseTime)
      dw = dwTime - lpSuperChunk->dwLastInUseTime;
    else
      dw = dwMaxAge; //force release on time wrap
    if (dw >= dwMaxAge)
    {
      //remove this item
      DlListRemove(&(lpSuperChunk->sLink));
      ::VirtualFree(lpSuperChunk->lpBlock, 0, MEM_RELEASE);
      ::HeapFree(hHeap, 0, lpSuperChunk);
    }
  }
  return;
}

#ifdef DO_SANITY_CHECK
VOID CNktMemMgrSuperChunk::SanityCheck(__in LPSUPERCHUNK lpSuperChunk)
{
  DWORD dwFreeCount;
  SIZE_T *lpPtr;

  dwFreeCount = 0;
  lpPtr = lpSuperChunk->lpFirstFreeBlock;
  while (lpPtr != NULL)
  {
    dwFreeCount++;
    NKT_ASSERT((LPBYTE)lpPtr >= lpSuperChunk->lpBlock);
    NKT_ASSERT((LPBYTE)lpPtr < lpSuperChunk->lpBlock + SUPERCHUNK_SIZE);
    lpPtr = (SIZE_T*)(*lpPtr);
  }
  NKT_ASSERT(dwFreeCount == lpSuperChunk->dwFreeCount);
  NKT_ASSERT(dwFreeCount <= (SUPERCHUNK_SIZE/0x10000));
  return;
}
#endif //DO_SANITY_CHECK

//-----------------------------------------------------------

CNktMemMgrBin::CNktMemMgrBin(__in DWORD _dwBlockSize) : CNktFastMutex()
{
  NKT_ASSERT(_dwBlockSize>=8 && _dwBlockSize<=16384);
  NKT_ASSERT(sizeof(CHUNK) == 0x10000);
  dwBlockSize = _dwBlockSize;
  dwBlocksPerChunk = (DWORD)(0x10000-sizeof(HEADER)) / dwBlockSize;
  dwCheckFreqCounter = 0;
  sFreeList.lpNext = sFreeList.lpPrev = &sFreeList;
  sInUseList.lpNext = sInUseList.lpPrev = &sInUseList;
  sFullList.lpNext = sFullList.lpPrev = &sFullList;
  return;
}

CNktMemMgrBin::~CNktMemMgrBin()
{
  Cleanup();
  return;
}

LPVOID CNktMemMgrBin::Alloc(__in SIZE_T nSize)
{
  LPVOID p;
  LPHEADER lpPtr;
  SIZE_T nBinIdx;

  nBinIdx = IndexFromSize(nSize);
  if (nBinIdx < BINS_COUNT)
  {
    if (lpBins[nBinIdx] == NULL)
    {
      CNktSimpleLockNonReentrant cLock(&nBinMtx);
      CNktMemMgrBin *lpTempBin;

      if (lpBins[nBinIdx] == NULL)
      {
        lpTempBin = new (aBinMem[nBinIdx]) CNktMemMgrBin(1UL << (3+nBinIdx));
        _ReadWriteBarrier();
        lpBins[nBinIdx] = lpTempBin;
        _ReadWriteBarrier();
      }
    }
    p = lpBins[nBinIdx]->SmallHeapAlloc();
  }
  else
  {
    lpPtr = (LPHEADER)::VirtualAlloc(NULL, nSize+sizeof(HEADER), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (lpPtr != NULL)
    {
      NKT_ASSERT(((SIZE_T)lpPtr & 0xFFFF) == 0); //ensure it is 64k aligned
      nktMemSet(lpPtr, 0, sizeof(HEADER));
      lpPtr->nTag = TAG_MARK ^ (SIZE_T)lpPtr;
      lpPtr++;
    }
    p = lpPtr;
  }
  return p;
}

VOID CNktMemMgrBin::Free(__in LPVOID lpPtr)
{
  if (lpPtr != NULL)
  {
    LPHEADER lpHdr;

    lpHdr = (LPHEADER)((SIZE_T)lpPtr & (~0xFFFF));
    NKT_ASSERT(lpHdr->nTag == (TAG_MARK ^ (SIZE_T)lpHdr));
    if (lpHdr->lpBinOwner != NULL)
    {
      lpHdr->lpBinOwner->SmallHeapFree(lpPtr);
    }
    else
    {
      ::VirtualFree(lpHdr, 0, MEM_RELEASE);
    }
  }
  return;
}

SIZE_T CNktMemMgrBin::GetSize(__in LPVOID lpPtr)
{
  LPHEADER lpHdr;
  MEMORY_BASIC_INFORMATION sMbi;
  SIZE_T nSize;

  nSize = 0;
  if (lpPtr != NULL)
  {
    lpHdr = (LPHEADER)((SIZE_T)lpPtr & (~0xFFFF));
    NKT_ASSERT(lpHdr->nTag == (TAG_MARK ^ (SIZE_T)lpHdr));
    if (lpHdr->lpBinOwner != NULL)
    {
      nSize = lpHdr->lpBinOwner->SmallHeapGetSize();
    }
    else
    {
      if (::VirtualQuery(lpHdr, &sMbi, sizeof(sMbi)) >= FIELD_OFFSET(MEMORY_BASIC_INFORMATION, State))
      {
        NKT_ASSERT(sMbi.RegionSize >= sizeof(HEADER));
        nSize = (sMbi.RegionSize >= sizeof(HEADER)) ? (sMbi.RegionSize-sizeof(HEADER)) : 0;
      }
    }
  }
  return nSize;
}

LPVOID CNktMemMgrBin::SmallHeapAlloc()
{
  CNktAutoFastMutex cLock(this);
  LPCHUNK lpChunk;
  SIZE_T *lpPtr;
  DWORD dw;

  if (sInUseList.lpNext == &sInUseList)
  {
    //check if we can get a chunk from the free list
    if (sFreeList.lpNext != &sFreeList)
    {
      lpChunk = CHUNK_FROM_DLLIST(sFreeList.lpNext);
      DlListRemove(&(lpChunk->sHeader.sLink));
      DlListInsertHead(&sInUseList, &(lpChunk->sHeader.sLink));
    }
    else
    {
      //else create a new one
      if (lpSuperChunkMgr == NULL)
      {
        CNktSimpleLockNonReentrant cLock(&nBinMtx);
        CNktMemMgrSuperChunk *lpTempSuperChunk;

        if (lpSuperChunkMgr == NULL)
        {
          lpTempSuperChunk = new (aSuperChunkMem) CNktMemMgrSuperChunk();
          _ReadWriteBarrier();
          lpSuperChunkMgr = lpTempSuperChunk;
          _ReadWriteBarrier();
        }
      }
      lpChunk = (LPCHUNK)(lpSuperChunkMgr->Alloc());
      if (lpChunk == NULL)
        return NULL;
      NKT_ASSERT(((SIZE_T)lpChunk & 0xFFFF) == 0); //ensure it is 64k aligned
      nktMemSet(&(lpChunk->sHeader), 0, sizeof(lpChunk->sHeader));
      lpChunk->sHeader.dwFreeCount = dwBlocksPerChunk;
      lpChunk->sHeader.nTag = TAG_MARK ^ (SIZE_T)&(lpChunk->sHeader);
      lpChunk->sHeader.sLink.lpNext = lpChunk->sHeader.sLink.lpPrev = &(lpChunk->sHeader.sLink);
      lpChunk->sHeader.lpBinOwner = this;
      lpPtr = lpChunk->sHeader.lpFirstFreeBlock = (SIZE_T*)(lpChunk->aBuffer);
      //init buffers internal links
      for (dw=lpChunk->sHeader.dwFreeCount; dw>1; dw--)
      {
        *lpPtr = (SIZE_T)lpPtr + (SIZE_T)dwBlockSize;
        lpPtr = (SIZE_T*)(*lpPtr);
      }
      *lpPtr = 0; //last is null
      //insert block into available list
      DlListInsertHead(&sInUseList, &(lpChunk->sHeader.sLink));
    }
  }
  //get a chunk from the available list
  NKT_ASSERT(sInUseList.lpNext != &sInUseList);
  lpChunk = CHUNK_FROM_DLLIST(sInUseList.lpNext);
  //get the block to return
  lpPtr = lpChunk->sHeader.lpFirstFreeBlock;
  //decrement free count
  (lpChunk->sHeader.dwFreeCount)--;
  //set the pointer to the new block
  lpChunk->sHeader.lpFirstFreeBlock = (SIZE_T*)(*lpPtr);
  if (lpChunk->sHeader.lpFirstFreeBlock == NULL)
  {
    NKT_ASSERT(lpChunk->sHeader.dwFreeCount == 0);
    //if chunk is full, move it to the full list
    DlListRemove(&(lpChunk->sHeader.sLink));
    DlListInsertHead(&sFullList, &(lpChunk->sHeader.sLink));
  }
#ifdef DO_SANITY_CHECK
  SanityCheck(lpChunk);
#endif //DO_SANITY_CHECK
  //done
  return lpPtr;
}

VOID CNktMemMgrBin::SmallHeapFree(__in LPVOID lpPtr)
{
  if (lpPtr != NULL)
  {
    CNktAutoFastMutex cLock(this);
    LPCHUNK lpChunk;

    lpChunk = (LPCHUNK)((SIZE_T)lpPtr & (~0xFFFF));
    NKT_ASSERT((LPBYTE)lpPtr >= (LPBYTE)&(lpChunk->aBuffer));
    NKT_ASSERT((LPBYTE)lpPtr < (LPBYTE)&(lpChunk->aBuffer)+sizeof(lpChunk->aBuffer));
    NKT_ASSERT((((SIZE_T)lpPtr - (SIZE_T)&(lpChunk->aBuffer)) % (SIZE_T)dwBlockSize) == 0);
    //add to the free block list
    *((SIZE_T*)lpPtr) = (SIZE_T)(lpChunk->sHeader.lpFirstFreeBlock);
    lpChunk->sHeader.lpFirstFreeBlock = (SIZE_T*)lpPtr;
    //increment free count
    (lpChunk->sHeader.dwFreeCount)++;
    if (lpChunk->sHeader.dwFreeCount >= dwBlocksPerChunk)
    {
      //chunk is completely free so move to the free list
      DlListRemove(&(lpChunk->sHeader.sLink));
      DlListInsertHead(&sFreeList, &(lpChunk->sHeader.sLink));
    }
    else if (lpChunk->sHeader.dwFreeCount > dwBlocksPerChunk/4)
    {
      //the chunk now has 1/4 of its blocks free so move it to the in-use list
      DlListRemove(&(lpChunk->sHeader.sLink));
      DlListInsertHead(&sInUseList, &(lpChunk->sHeader.sLink));
    }
    //set last used time
    lpChunk->sHeader.dwLastInUseTime = ::GetTickCount();
    if ((++dwCheckFreqCounter) >= CNktDvRegistrySettings::GetMemMgrPoolCheckFrequency())
    {
      dwCheckFreqCounter = 0;
      Trim((lpChunk->sHeader.dwLastInUseTime != 0) ? (lpChunk->sHeader.dwLastInUseTime) : 0);
    }
#ifdef DO_SANITY_CHECK
    SanityCheck(lpChunk);
#endif //DO_SANITY_CHECK
  }
  return;
}

VOID CNktMemMgrBin::Cleanup()
{
  LPCHUNK lpChunk;

  while (sFullList.lpNext != &sFullList)
  {
    lpChunk = CHUNK_FROM_DLLIST(sFullList.lpNext);
    DlListRemove(&(lpChunk->sHeader.sLink));
    NKT_ASSERT(lpSuperChunkMgr != NULL);
    lpSuperChunkMgr->Free(lpChunk);
  }
  while (sInUseList.lpNext != &sInUseList)
  {
    lpChunk = CHUNK_FROM_DLLIST(sInUseList.lpNext);
    DlListRemove(&(lpChunk->sHeader.sLink));
    NKT_ASSERT(lpSuperChunkMgr != NULL);
    lpSuperChunkMgr->Free(lpChunk);
  }
  while (sFreeList.lpNext != &sFreeList)
  {
    lpChunk = CHUNK_FROM_DLLIST(sFreeList.lpNext);
    DlListRemove(&(lpChunk->sHeader.sLink));
    NKT_ASSERT(lpSuperChunkMgr != NULL);
    lpSuperChunkMgr->Free(lpChunk);
  }
  return;
}

VOID CNktMemMgrBin::Trim(__in DWORD dwTime)
{
  LPDLLIST_ITEM lpItem, lpPrevItem;
  LPCHUNK lpChunk;
  DWORD dw, dwMaxAge;

  dwMaxAge = CNktDvRegistrySettings::GetMemMgrPoolMaxAge();
  for (lpItem=sFreeList.lpPrev; lpItem!=&sFreeList; lpItem=lpPrevItem)
  {
    lpPrevItem = lpItem->lpPrev;
    lpChunk = CHUNK_FROM_DLLIST(lpItem);
    if (dwTime >= lpChunk->sHeader.dwLastInUseTime)
      dw = dwTime - lpChunk->sHeader.dwLastInUseTime;
    else
      dw = dwMaxAge; //force release on time wrap
    if (dw >= dwMaxAge)
    {
      //remove this item
      DlListRemove(&(lpChunk->sHeader.sLink));
      NKT_ASSERT(lpSuperChunkMgr != NULL);
      lpSuperChunkMgr->Free(lpChunk);
    }
  }
  return;
}

#ifdef DO_SANITY_CHECK
VOID CNktMemMgrBin::SanityCheck(__in LPCHUNK lpChunk)
{
  DWORD dwFreeCount;
  SIZE_T *lpPtr;

  NKT_ASSERT(lpChunk->sHeader.nTag == (TAG_MARK ^ (SIZE_T)(&(lpChunk->sHeader))));
  dwFreeCount = 0;
  lpPtr = lpChunk->sHeader.lpFirstFreeBlock;
  while (lpPtr != NULL)
  {
    dwFreeCount++;
    NKT_ASSERT((LPBYTE)lpPtr >= lpChunk->aBuffer);
    NKT_ASSERT((LPBYTE)lpPtr < lpChunk->aBuffer + sizeof(lpChunk->aBuffer));
    lpPtr = (SIZE_T*)(*lpPtr);
  }
  NKT_ASSERT(dwFreeCount == lpChunk->sHeader.dwFreeCount);
  NKT_ASSERT(dwFreeCount <= dwBlocksPerChunk);
  return;
}
#endif //DO_SANITY_CHECK
