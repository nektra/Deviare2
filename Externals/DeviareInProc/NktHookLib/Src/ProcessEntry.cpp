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
#include "ProcessEntry.h"

#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#define MemoryBasicInformation                             0
#define ThreadImpersonationToken                           5

#define MY_SE_DEBUG_PRIVILEGE                             20

//-----------------------------------------------------------

CProcessesHandles::CProcessesHandles()
{
  _InterlockedExchange(&nRemoveKilledCounter, 0);
  return;
}

CProcessesHandles::~CProcessesHandles()
{
  CNktAutoFastMutex cLock(&cMtx);
  CEntry *lpEntry;

  while ((lpEntry=cEntries.PopHead()) != NULL)
    lpEntry->Release();
  return;
}

NTSTATUS CProcessesHandles::Get(__in DWORD dwPid, __out CProcessesHandles::CEntry **lplpEntry)
{
  CNktAutoFastMutex cLock(&cMtx);
  TNktLnkLst<CEntry>::Iterator it;
  CEntry *lpEntry;
  LONG nPlatform;
  HANDLE h;
  NTSTATUS nNtStatus;

  if (lplpEntry == NULL)
    return STATUS_INVALID_PARAMETER;
  *lplpEntry = NULL;
  if (dwPid == 0)
    return STATUS_INVALID_PARAMETER;
  RemoveKilledProcesses();
  for (lpEntry=it.Begin(cEntries); lpEntry!=NULL; lpEntry=it.Next())
  {
    if (lpEntry->dwPid == dwPid)
      break;
  }
  if (lpEntry == NULL)
  {
    if (dwPid == NktHookLibHelpers::GetCurrentProcessId())
    {
      h = NKTHOOKLIB_CurrentProcess;
    }
    else
    {
      nNtStatus = CreateHandle(dwPid, PROCESS_SUSPEND_RESUME|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_READ|
                              PROCESS_VM_WRITE, &h);
      if (!NT_SUCCESS(nNtStatus))
        return nNtStatus;
    }
    nPlatform = NktHookLibHelpers::GetProcessPlatform(h);
    if (!NT_SUCCESS(nPlatform))
    {
      if (h != NKTHOOKLIB_CurrentProcess)
        NktNtClose(h);
      return (NTSTATUS)nNtStatus;
    }
    lpEntry = new CEntry(dwPid, h, nPlatform);
    if (lpEntry == NULL)
    {
      if (h != NKTHOOKLIB_CurrentProcess)
        NktNtClose(h);
      return STATUS_NO_MEMORY;
    }
    cEntries.PushHead(lpEntry);
  }
  lpEntry->AddRef();
  *lplpEntry = lpEntry;
  return STATUS_SUCCESS;
}

NTSTATUS CProcessesHandles::CreateHandle(__in DWORD dwPid, __in DWORD dwDesiredAccess, __out HANDLE *lphProc)
{
  HANDLE hToken;
  TOKEN_PRIVILEGES sTokPriv;
  BOOL bRevertToSelf;
  OBJECT_ATTRIBUTES sObjAttr;
  NKT_HK_CLIENT_ID sClientId;
  NTSTATUS nNtStatus;

  if (lphProc == NULL)
    return STATUS_INVALID_PARAMETER;
  *lphProc = NULL;
  if (dwPid == 0)
    return STATUS_INVALID_PARAMETER;
  //try to enable SeDebugPrivilege
  bRevertToSelf = FALSE;
  nNtStatus = NktNtOpenThreadToken(NKTHOOKLIB_CurrentThread, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken);
  if (!NT_SUCCESS(nNtStatus))
  {
    hToken = NULL;
    nNtStatus = NktRtlImpersonateSelf(SecurityImpersonation);
    if (NT_SUCCESS(nNtStatus))
    {
      bRevertToSelf = TRUE;
      nNtStatus = NktNtOpenThreadToken(NKTHOOKLIB_CurrentThread, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken);
      if (!NT_SUCCESS(nNtStatus))
        hToken = NULL;
    }
  }
  NktHookLibHelpers::MemSet(&sTokPriv, 0, sizeof(sTokPriv));
  if (hToken != NULL)
  {
    sTokPriv.PrivilegeCount = 1;
    sTokPriv.Privileges[0].Luid.LowPart = MY_SE_DEBUG_PRIVILEGE;
    sTokPriv.Privileges[0].Luid.HighPart = 0;
    sTokPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    NktNtAdjustPrivilegesToken(hToken, 0, &sTokPriv, sizeof(sTokPriv), NULL, NULL);
  }
  //open process
  sClientId.UniqueProcess = (SIZE_T)(ULONG_PTR)(dwPid);
  sClientId.UniqueThread = 0;
  InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
  nNtStatus = NktNtOpenProcess(lphProc, dwDesiredAccess, &sObjAttr, &sClientId);
  if (!NT_SUCCESS(nNtStatus))
  {
    if ((dwDesiredAccess & (PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION)) == PROCESS_QUERY_INFORMATION)
    {
      dwDesiredAccess &= (~PROCESS_QUERY_INFORMATION);
      dwDesiredAccess |= PROCESS_QUERY_LIMITED_INFORMATION;
      nNtStatus = NktNtOpenProcess(lphProc, dwDesiredAccess, &sObjAttr, &sClientId);
    }
  }
  //restore privileges
  if (sTokPriv.PrivilegeCount > 0)
  {
    sTokPriv.Privileges[0].Attributes = 0;
    NktNtAdjustPrivilegesToken(hToken, 0, &sTokPriv, sizeof(sTokPriv), NULL, NULL);
  }
  if (bRevertToSelf != FALSE)
  {
    HANDLE hDummyToken = NULL;
    NktNtSetInformationThread(NKTHOOKLIB_CurrentThread, (THREADINFOCLASS)ThreadImpersonationToken, &hDummyToken,
                              sizeof(hDummyToken));
  }
  if (hToken != NULL)
    NktNtClose(hToken);
  //done
  return nNtStatus;
}

VOID CProcessesHandles::RemoveKilledProcesses()
{
  if ((_InterlockedIncrement(&nRemoveKilledCounter) & 0xFF) == 0xFF)
  {
    //TODO: Remove non-existant processes from list
  }
  return;
}

//-----------------------------------------------------------

CProcessesHandles::CEntry::CEntry(__in DWORD _dwPid, __in HANDLE _h, __in LONG _nPlatform) : TNktLnkLstNode<CEntry>(),
                                                                                             CNktNtHeapBaseObj()
{
  dwPid = _dwPid;
  h = _h;
  nPlatform = _nPlatform;
  _InterlockedExchange(&nRefCount, 1);
  _InterlockedExchange(&nStubAllocMutex, 0);
  return;
}

CProcessesHandles::CEntry::~CEntry()
{
  NKT_ASSERT(GetLinkedList() == NULL);
  if (h != NULL && h != INVALID_HANDLE_VALUE && h != NKTHOOKLIB_CurrentProcess)
    NktNtClose(h);
  return;
}

VOID CProcessesHandles::CEntry::AddRef()
{
  _InterlockedIncrement(&nRefCount);
  return;
}

VOID CProcessesHandles::CEntry::Release()
{
  if (_InterlockedDecrement(&nRefCount) == 0)
    delete this;
  return;
}

LONG CProcessesHandles::CEntry::GetCurrPlatform()
{
  LONG volatile nPlatform = (LONG)-1;

  if (nPlatform == (LONG)-1)
  {
    LONG _plat = NktHookLibHelpers::GetProcessPlatform(NKTHOOKLIB_CurrentProcess);
    if (NT_SUCCESS(_plat))
      _InterlockedExchange(&nPlatform, _plat);
  }
  return nPlatform;
}

LPBYTE CProcessesHandles::CEntry::AllocateMem(__in LPVOID lpRefAddr, __in SIZE_T nSlotSize, __in BOOL bIsData)
{
  CNktSimpleLockNonReentrant cLock(&nStubAllocMutex);
  TNktLnkLst<CMemBlock>::Iterator it;
  CMemBlock *lpBlock;
  LPBYTE lpPtr;
#if defined(_M_X64)
  ULONGLONG nMin, nMax;
#endif //_M_X64
  TNktLnkLst<CMemBlock> *lpBlocksList;

  NKT_ASSERT(nSlotSize > 0 && nSlotSize <= 65536);
  //round slot size to next power of two
  nSlotSize--;
  nSlotSize |= nSlotSize >> 1;
  nSlotSize |= nSlotSize >> 2;
  nSlotSize |= nSlotSize >> 4;
  nSlotSize |= nSlotSize >> 8;
  nSlotSize |= nSlotSize >> 16;
#if defined(_M_X64)
  nSlotSize |= nSlotSize >> 32;
#endif //_M_X64
  nSlotSize++;
  //calculate min/max address
#if defined(_M_X64)
  nMin = nMax = ((ULONGLONG)(SIZE_T)lpRefAddr) & (~65535ui64);
  if (nMin > 0x40000000ui64)
    nMin -= 0x40000000ui64;
  else
    nMin = 0ui64;
  if (nMax < 0xFFFFFFFFFFFFFFFFui64-0x40000000ui64)
    nMax += 0x40000000ui64;
  else
    nMax = 0xFFFFFFFFFFFFFFFFui64;
#endif //_M_X64
  //find a previously allocated block
  lpBlocksList = (bIsData == FALSE) ? &cCodeBlocksList : &cDataBlocksList;
  for (lpBlock=it.Begin(*lpBlocksList); lpBlock!=NULL; lpBlock=it.Next())
  {
    if (
#if defined(_M_X64)
        (ULONGLONG)(SIZE_T)(lpBlock->GetBaseAddress()) >= nMin &&
        (ULONGLONG)(SIZE_T)(lpBlock->GetBaseAddress()) < nMax &&
#endif //_M_X64
        lpBlock->GetSlotSize() == nSlotSize
       )
    {
      lpPtr = lpBlock->GetFreeSlot();
      if (lpPtr != NULL)
        return lpPtr;
    }
  }
  lpBlock = new CMemBlock(GetHandle(), nSlotSize, bIsData);
  if (lpBlock == NULL)
    return NULL;
  if (lpBlock->Initialize(
#if defined(_M_X64)
                          nMin, nMax
#endif //_M_X64
     ) == FALSE)
  {
    delete lpBlock;
    return NULL;
  }
  lpBlocksList->PushHead(lpBlock);
  return lpBlock->GetFreeSlot();
}

VOID CProcessesHandles::CEntry::FreeMem(__in LPVOID lpAddr, __in BOOL bIsData)
{
  CNktSimpleLockNonReentrant cLock(&nStubAllocMutex);
  TNktLnkLst<CMemBlock>::Iterator it;
  TNktLnkLst<CMemBlock> *lpBlocksList;
  CMemBlock *lpBlock;

  lpBlocksList = (bIsData == FALSE) ? &cCodeBlocksList : &cDataBlocksList;
  for (lpBlock=it.Begin(*lpBlocksList); lpBlock!=NULL; lpBlock=it.Next())
  {
    if (lpBlock->IsAddressInBlock(lpAddr) != FALSE)
    {
      lpBlock->ReleaseSlot(lpAddr);
      return;
    }
  }
  NKT_ASSERT(FALSE);
  return;
}

//-----------------------------------------------------------

CProcessesHandles::CEntry::CMemBlock::CMemBlock(__in HANDLE _hProc, __in SIZE_T _nSlotSize, __in BOOL _bIsData) :
                                      TNktLnkLstNode<CMemBlock>(), CNktNtHeapBaseObj()
{
  SIZE_T nFreeEntriesSize;

  NKT_ASSERT(_nSlotSize > 0 && _nSlotSize <= 65536);
  NKT_ASSERT((_nSlotSize && !(_nSlotSize & (_nSlotSize - 1))) != false); //_nSlotSize must be a power of 2
  hProc = _hProc;
  nSlotSize = _nSlotSize;
  bIsData = _bIsData;
  nFreeEntriesSize = ((65536 / nSlotSize) + 7) >> 3;
  lpFreeEntries = (LPBYTE)NktHookLibHelpers::MemAlloc(nFreeEntriesSize);
  if (lpFreeEntries != NULL)
    NktHookLibHelpers::MemSet(lpFreeEntries, 0xFF, nFreeEntriesSize);
  nFreeCount = 65536 / nSlotSize;
  lpBaseAddress = NULL;
  return;
}

CProcessesHandles::CEntry::CMemBlock::~CMemBlock()
{
  SIZE_T nSize;

  if (lpBaseAddress != NULL && nFreeCount >= (65536 / nSlotSize))
  {
    nSize = 0;
    NktNtFreeVirtualMemory(hProc, (PVOID*)&lpBaseAddress, &nSize, MEM_RELEASE);
  }
  if (lpFreeEntries != NULL)
    NktHookLibHelpers::MemFree(lpFreeEntries);
  return;
}

#if defined(_M_IX86)
BOOL CProcessesHandles::CEntry::CMemBlock::Initialize()
{
  SIZE_T nSize;
  NTSTATUS nNtStatus;

  if (lpFreeEntries == NULL)
    return FALSE;
  lpBaseAddress = NULL;
  nSize = 65536;
  nNtStatus = NktNtAllocateVirtualMemory(hProc, (PVOID*)&lpBaseAddress, 0, &nSize, MEM_RESERVE|MEM_COMMIT,
                                         (bIsData == FALSE) ? PAGE_EXECUTE_READ : PAGE_READWRITE);
  if (!NT_SUCCESS(nNtStatus))
    lpBaseAddress = NULL;
  return (lpBaseAddress != NULL) ? TRUE : FALSE;
}

#elif defined(_M_X64)
BOOL CProcessesHandles::CEntry::CMemBlock::Initialize(__in ULONGLONG nMin, __in ULONGLONG nMax)
{
  MEMORY_BASIC_INFORMATION sMbi;
  SIZE_T nSize, nResultLength;
  NTSTATUS nNtStatus;

  if (lpFreeEntries == NULL)
    return FALSE;
  if (nMin < 65536)
    nMin = 65536;
  while (nMin < nMax)
  {
    NktHookLibHelpers::MemSet(&sMbi, 0, sizeof(sMbi));
    nNtStatus = NktNtQueryVirtualMemory(hProc, (PVOID)nMin, MyMemoryBasicInformation, &sMbi, sizeof(sMbi),
                                        &nResultLength);
    if (NT_SUCCESS(nNtStatus) && sMbi.State == MEM_FREE)
    {
      lpBaseAddress = (LPBYTE)nMin;
      nSize = 65536;
      nNtStatus = NktNtAllocateVirtualMemory(hProc, (PVOID*)&lpBaseAddress, 0, &nSize, MEM_RESERVE|MEM_COMMIT,
                                             (bIsData == FALSE) ? PAGE_EXECUTE_READ : PAGE_READWRITE);

      if (NT_SUCCESS(nNtStatus))
        return TRUE;
    }
    nMin += 65536;
  }
  lpBaseAddress = NULL;
  return FALSE;
}
#endif

LPBYTE CProcessesHandles::CEntry::CMemBlock::GetFreeSlot()
{
  SIZE_T i, nIdx, nFreeEntriesSize;

  if (nFreeCount == 0)
    return NULL;
  nFreeEntriesSize = ((65536 / nSlotSize) + 7) >> 3;
  for (nIdx=0; nIdx<nFreeEntriesSize; nIdx++)
  {
    if (lpFreeEntries[nIdx] != 0)
      break;
  }
  NKT_ASSERT(nIdx < nFreeEntriesSize);
  for (i=0; i<8; i++)
  {
    if ((lpFreeEntries[nIdx] & (1<<i)) != 0)
      break;
  }
  NKT_ASSERT(i < 8);
  lpFreeEntries[nIdx] &= ~(1<<i);
  nFreeCount--;
  return lpBaseAddress + ((nIdx<<3) + i) * nSlotSize;
}

VOID CProcessesHandles::CEntry::CMemBlock::ReleaseSlot(__in LPVOID lpAddr)
{
  SIZE_T nOfs;
  BYTE nMask;

  NKT_ASSERT((SIZE_T)(LPBYTE)lpAddr >= (SIZE_T)lpBaseAddress);
  nOfs = (SIZE_T)(LPBYTE)lpAddr - (SIZE_T)lpBaseAddress;
  NKT_ASSERT((nOfs % nSlotSize) == 0);
  nOfs /= nSlotSize;
  NKT_ASSERT((nOfs>>3) < ((65536 / nSlotSize) + 7) >> 3);
  nMask = 1 << (nOfs & 7);
  nOfs >>= 3;
  NKT_ASSERT((lpFreeEntries[nOfs] & nMask) == 0);
  lpFreeEntries[nOfs] |= nMask;
  nFreeCount++;
  return;
}

BOOL CProcessesHandles::CEntry::CMemBlock::IsAddressInBlock(__in LPVOID lpAddr)
{
  return ((SIZE_T)lpAddr >= (SIZE_T)lpBaseAddress &&
          (SIZE_T)lpAddr < (SIZE_T)lpBaseAddress + 65536) ? TRUE : FALSE;
}

//-----------------------------------------------------------

} //Internals
} //NktHookLib
