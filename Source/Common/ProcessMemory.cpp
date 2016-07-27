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

#include "ProcessMemory.h"
#include "ProcessHandle.h"
#include "Tools.h"
#include "Param.h"

//-----------------------------------------------------------

#ifdef NKT_ENABLE_MEMORY_TRACKING
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif //NKT_ENABLE_MEMORY_TRACKING

//-----------------------------------------------------------

#define NKT_ACCESS_READ	 (PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_OPERATION)
#define NKT_ACCESS_WRITE	(NKT_ACCESS_READ|PROCESS_VM_WRITE)

//-----------------------------------------------------------

static LONG volatile nListLock = 0;
static TNktLnkLst<CNktDvProcessMemory> cProcMemList;

//-----------------------------------------------------------

CNktDvProcessMemory::CNktDvProcessMemory(__in DWORD _dwPid) : 
                               TNktDvEngBaseObjWithLock<CNktFastMutex>(TRUE),
                               TNktLnkLstNode<CNktDvProcessMemory>()
{
  dwPid = _dwPid;
  bIsLocal = (dwPid == ::GetCurrentProcessId()) ? TRUE : FALSE;
  hProc[0] = hProc[1] = NULL;
  bWriteAccessChecked = FALSE;
  return;
}

CNktDvProcessMemory::~CNktDvProcessMemory()
{
  cProcMemList.Remove(this);
  //----
  if (hProc[1] != NULL)
    ::CloseHandle(hProc[1]);
  if (hProc[0] != NULL)
    ::CloseHandle(hProc[0]);
  return;
}

CNktDvProcessMemory* CNktDvProcessMemory::CreateForPID(__in DWORD dwPid)
{
  CNktSimpleLockNonReentrant cListLock(&nListLock);
  TNktLnkLst<CNktDvProcessMemory>::Iterator it;
  CNktDvProcessMemory *lpProcMem;

  //check if already exists
  for (lpProcMem=it.Begin(cProcMemList); lpProcMem!=NULL; lpProcMem=it.Next())
  {
    if (lpProcMem->dwPid == dwPid)
    {
      lpProcMem->AddRef();
      return lpProcMem;
    }
  }
  //create a new one
  lpProcMem = NKT_MEMMGR_NEW CNktDvProcessMemory(dwPid);
  if (lpProcMem == NULL)
    return NULL;
  //add to list
  cProcMemList.PushTail(lpProcMem);
  //done
  return lpProcMem;
}

ULONG CNktDvProcessMemory::Release()
{
  CNktSimpleLockNonReentrant cListLock(&nListLock);

  return CNktDvObject::Release();
}

HRESULT CNktDvProcessMemory::GetProtection(__out eProtection *lpnProtection, __in LPCVOID lpRemotePtr,
                                           __in BOOL bAllocated)
{
  MEMORY_BASIC_INFORMATION sMbi;
  HANDLE hTempProc;
  DWORD dwProt;
  int nNktProt;

  if (lpnProtection == NULL)
    return E_POINTER;
  *lpnProtection = (eProtection)0;
  //get process handle
  hTempProc = GetReadAccessHandle();
  if (hTempProc == NULL)
    return E_ACCESSDENIED;
  //query info
  if (::VirtualQueryEx(hTempProc, lpRemotePtr, &sMbi, sizeof(sMbi)) == FALSE)
    return E_ACCESSDENIED;
  //convert
  nNktProt = 0;
  dwProt = (bAllocated == FALSE) ? sMbi.Protect : sMbi.AllocationProtect;
  if ((dwProt & PAGE_EXECUTE_READWRITE) != 0)
    nNktProt = protExecute | protRead | protWrite;
  else if ((dwProt & PAGE_EXECUTE_WRITECOPY) != 0)
    nNktProt = protExecute | protRead | protWrite | protIsWriteCopy;
  else if ((dwProt & PAGE_EXECUTE_READ) != 0)
    nNktProt = protExecute | protRead;
  else if ((dwProt & PAGE_READWRITE) != 0)
    nNktProt = protRead | protWrite;
  else if ((dwProt & PAGE_WRITECOPY) != 0)
    nNktProt = protRead | protWrite | protIsWriteCopy;
  if ((dwProt & PAGE_EXECUTE) != 0)
    nNktProt = protExecute;
  else if ((dwProt & PAGE_READONLY) != 0)
    nNktProt = protRead;
  // these are modifiers
  if ((dwProt & PAGE_GUARD) != 0)
    nNktProt |= protGuard;
  if ((dwProt & PAGE_NOCACHE) != 0)
    nNktProt |= protNoCache;
  if ((dwProt & PAGE_WRITECOMBINE) != 0)
    nNktProt |= protWriteCombine;
  *lpnProtection = (eProtection)nNktProt;
  return S_OK;
}

HRESULT CNktDvProcessMemory::Read(__out LPVOID lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nSize,
                                  __deref_out_opt SIZE_T *lpnReaded)
{
  HRESULT hRes;
  SIZE_T nReaded;
  HANDLE hTempProc;

  if (lpnReaded != NULL)
    *lpnReaded = 0;
  //check parameters
  if (nSize == 0)
    return S_OK;
  if (lpLocalDest == NULL || lpRemoteSrc == NULL)
    return E_POINTER;
  //do read
  nReaded = 0;
  if (bIsLocal == FALSE)
  {
    //get process handle
    hTempProc = GetReadAccessHandle();
    if (hTempProc != NULL)
    {
      hRes = (::ReadProcessMemory(hTempProc, lpRemoteSrc, lpLocalDest, nSize, &nReaded) != FALSE) ?
                    S_OK : NKT_HRESULT_FROM_LASTERROR();
    }
    else
    {
      hRes = E_ACCESSDENIED;
    }
  }
  else
  {
    nReaded = nktTryMemCopy(lpLocalDest, lpRemoteSrc, nSize);
    hRes = (nSize == nReaded) ? S_OK : E_ACCESSDENIED;
  }
  if (nReaded > 0 && nSize != nReaded)
    hRes = NKT_DVERR_PartialCopy;
  if (FAILED(hRes) && hRes != E_ACCESSDENIED && hRes != NKT_DVERR_PartialCopy)
    hRes = E_FAIL;
  if (lpnReaded != NULL)
    *lpnReaded = nReaded;
  return hRes;
}

HRESULT CNktDvProcessMemory::Write(__in LPVOID lpRemoteDest, __in LPCVOID lpLocalSrc, __in SIZE_T nSize,
                                   __deref_out_opt SIZE_T *lpnWritten)
{
  SIZE_T nWritten;
  HANDLE hTempProc;
  HRESULT hRes;

  if (lpnWritten != NULL)
    *lpnWritten = NULL;
  //check parameters
  if (nSize == 0)
    return S_OK;
  if (lpRemoteDest == NULL || lpLocalSrc == NULL)
    return E_POINTER;
  //do write
  if (bIsLocal == FALSE)
  {
    nWritten = 0;
    //get process handle
    hTempProc = GetWriteAccessHandle();
    if (hTempProc == NULL)
      return E_ACCESSDENIED;
    hRes = (::WriteProcessMemory(hTempProc, lpRemoteDest, lpLocalSrc, nSize, &nWritten) != FALSE) ?
                  S_OK : NKT_HRESULT_FROM_LASTERROR();
  }
  else
  {
    nWritten = nktTryMemCopy(lpRemoteDest, lpLocalSrc, nSize);
    hRes = (nSize == nWritten) ? S_OK : E_ACCESSDENIED;
  }
  if (nWritten > 0 && nSize != nWritten)
    hRes = NKT_DVERR_PartialCopy;
  if (FAILED(hRes) && hRes != E_ACCESSDENIED && hRes != NKT_DVERR_PartialCopy)
    hRes = E_FAIL;
  if (lpnWritten != NULL)
    *lpnWritten = nWritten;
  return hRes;
}

HRESULT CNktDvProcessMemory::WriteProtected(__in LPVOID lpRemoteDest, __in LPCVOID lpLocalSrc,
                                            __in SIZE_T nSize, __deref_out_opt SIZE_T *lpnWritten)
{
  LPBYTE s, d;
  SIZE_T nToCopy, nWritten, nTotalWritten;
  DWORD dwOrigProt, dwNewProt, dwDummy;
  MEMORY_BASIC_INFORMATION sMbi;
  HANDLE hTempProc;
  HRESULT hRes;

  if (lpnWritten != NULL)
    *lpnWritten = NULL;
  //check parameters
  if (nSize == 0)
    return S_OK;
  if (lpRemoteDest == NULL || lpLocalSrc == NULL)
    return E_POINTER;
  //get process handle
  hTempProc = GetWriteAccessHandle();
  if (hTempProc == NULL)
    return E_ACCESSDENIED;
  //do write
  hRes = S_OK;
  nTotalWritten = 0;
  {
    CNktAutoFastMutex cLock(&GetLockObject());

    d = (LPBYTE)lpRemoteDest;
    s = (LPBYTE)lpLocalSrc;
    while (nSize > 0)
    {
      //check current protection
      if (::VirtualQueryEx(hTempProc, d, &sMbi, sizeof(sMbi)) == FALSE)
      {
        hRes = E_FAIL;
        break;
      }
      //calculate available writable bytes in region
      nToCopy = 0;
      if (d >= (LPBYTE)(sMbi.BaseAddress))
      {
        nToCopy = d - (LPBYTE)(sMbi.BaseAddress);
        nToCopy = (nToCopy < sMbi.RegionSize) ? (sMbi.RegionSize - nToCopy) : 0;
      }
      NKT_ASSERT(nToCopy > 0);
      if (nToCopy == 0)
      {
        hRes = E_FAIL; //should not happen
        break;
      }
      //calculate new desired access based on the current one
      if ((sMbi.Protect & PAGE_GUARD) != 0)
      {
        hRes = E_FAIL; //if a page is guarded, do not touch
        break;
      }
      dwNewProt = sMbi.Protect & 0xFF;
      switch (dwNewProt)
      {
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY:
        case PAGE_READWRITE:
        case PAGE_WRITECOPY:
          break;
        case PAGE_EXECUTE_READ:
          dwNewProt = PAGE_EXECUTE_READWRITE;
          break;
        default:
          dwNewProt = PAGE_READWRITE;
          break;
      }
      dwNewProt |= (sMbi.Protect & 0xFFFFFF00); //add the rest of the flags
      //if access protection needs a change, do it
      if (dwNewProt != sMbi.Protect)
      {
        dwOrigProt = 0;
        if (::VirtualProtectEx(hTempProc, sMbi.BaseAddress, sMbi.RegionSize, dwNewProt, &dwOrigProt) == FALSE)
        {
          hRes = E_ACCESSDENIED;
          break;
        }
      }
      //do partial copy
      if (nToCopy > nSize)
        nToCopy = nSize;
      nWritten = 0;
      if (bIsLocal == FALSE)
      {
        hRes = (::WriteProcessMemory(hTempProc, d, s, nToCopy, &nWritten) != FALSE) ?
                      S_OK : NKT_HRESULT_FROM_LASTERROR();
      }
      else
      {
        nWritten = nktTryMemCopy(d, s, nToCopy);
        hRes = (nToCopy == nWritten) ? S_OK : E_ACCESSDENIED;
      }
      nTotalWritten += nWritten;
      //restore original access
      if (dwNewProt != sMbi.Protect)
        ::VirtualProtectEx(hTempProc, sMbi.BaseAddress, sMbi.RegionSize, dwOrigProt, &dwDummy);
      //check copy operation
      if (FAILED(hRes) || nToCopy != nWritten)
        break;
      //advance buffer
      s += nWritten;
      d += nWritten;
      nSize -= nWritten;
    }
  }
  if (nTotalWritten > 0 && nSize > 0)
    hRes = NKT_DVERR_PartialCopy;
  if (FAILED(hRes) && hRes != E_ACCESSDENIED && hRes != NKT_DVERR_PartialCopy)
    hRes = E_FAIL;
  if (lpnWritten != NULL)
    *lpnWritten = nTotalWritten;
  return hRes;
}

HRESULT CNktDvProcessMemory::ReadStringA(__deref_out LPSTR* lpszDestA, __in LPCVOID lpRemoteSrc,
                                         __in SIZE_T nMaxChars)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  LPSTR sA;
  SIZE_T nCurrLen, nCurrSize;

  if (lpszDestA == NULL)
    return E_POINTER;
  *lpszDestA = NULL;
  if (lpRemoteSrc == NULL)
    return E_POINTER;
  //initial alloc
  nCurrSize = 256;
  *lpszDestA = (LPSTR)nktMemMalloc(nCurrSize*sizeof(CHAR));
  if (*lpszDestA == NULL)
    return E_OUTOFMEMORY;
  nCurrLen = 0;
  for (hRes=S_OK;;)
  {
    //check buffer size
    if (nCurrLen >= nCurrSize)
    {
      if ((nCurrSize<<1) < nCurrSize)
      {
        hRes = E_OUTOFMEMORY;
        break;
      }
      nCurrSize <<= 1;
      sA = (LPSTR)nktMemRealloc(*lpszDestA, nCurrSize*sizeof(CHAR));
      if (sA == NULL)
      {
        hRes = E_OUTOFMEMORY;
        break;
      }
      *lpszDestA = sA;
    }
    //check max chars
    if (nMaxChars == 0)
    {
      (*lpszDestA)[nCurrLen] = 0;
      break;
    }
    else if (nMaxChars != NKT_SIZE_T_MAX)
      nMaxChars--;
    //read a char
    hRes = Read((*lpszDestA)+nCurrLen, (LPSTR)lpRemoteSrc+nCurrLen, sizeof(CHAR));
    if (FAILED(hRes))
    {
      (*lpszDestA)[nCurrLen] = 0;
      break;
    }
    if ((*lpszDestA)[nCurrLen] == 0)
      break;
    nCurrLen++;
  }
  if (hRes == E_OUTOFMEMORY)
  {
    nktMemFree(*lpszDestA);
    *lpszDestA = NULL;
  }
  return hRes;
}

HRESULT CNktDvProcessMemory::ReadStringW(__deref_out LPWSTR* lpszDestW, __in LPCVOID lpRemoteSrc,
                                         __in SIZE_T nMaxChars)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  LPWSTR sW;
  SIZE_T nCurrLen, nCurrSize;

  if (lpszDestW == NULL)
    return E_POINTER;
  *lpszDestW = NULL;
  if (lpRemoteSrc == NULL)
    return E_POINTER;
  //initial alloc
  nCurrSize = 256;
  *lpszDestW = (LPWSTR)nktMemMalloc(nCurrSize*sizeof(WCHAR));
  if (*lpszDestW == NULL)
    return E_OUTOFMEMORY;
  nCurrLen = 0;
  for (hRes=S_OK;;)
  {
    //check buffer size
    if (nCurrLen >= nCurrSize)
    {
      if ((nCurrSize<<1) < nCurrSize)
      {
        hRes = E_OUTOFMEMORY;
        break;
      }
      nCurrSize <<= 1;
      sW = (LPWSTR)nktMemRealloc(*lpszDestW, nCurrSize*sizeof(WCHAR));
      if (sW == NULL)
      {
        hRes = E_OUTOFMEMORY;
        break;
      }
      *lpszDestW = sW;
    }
    //check max chars
    if (nMaxChars == 0)
    {
      (*lpszDestW)[nCurrLen] = 0;
      break;
    }
    else if (nMaxChars != NKT_SIZE_T_MAX)
      nMaxChars--;
    //read a char
    hRes = Read((*lpszDestW)+nCurrLen, (LPWSTR)lpRemoteSrc+nCurrLen, sizeof(WCHAR));
    if (FAILED(hRes))
    {
      (*lpszDestW)[nCurrLen] = 0;
      break;
    }
    if ((*lpszDestW)[nCurrLen] == 0)
      break;
    nCurrLen++;
  }
  if (hRes == E_OUTOFMEMORY)
  {
    nktMemFree(*lpszDestW);
    *lpszDestW = NULL;
  }
  return hRes;
}

HRESULT CNktDvProcessMemory::WriteStringA(__in LPVOID lpRemoteDest, __in_nz_opt LPCSTR szSrcA,
                                          __in SIZE_T nSrcLen, __in BOOL bWriteNulTerminator)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  CHAR chA;

  if (lpRemoteDest == NULL || szSrcA == NULL)
    return E_POINTER;
  if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = strlen(szSrcA);
  if (nSrcLen > 0)
  {
    hRes = WriteProtected((LPSTR)lpRemoteDest, szSrcA, nSrcLen*sizeof(CHAR));
    if (FAILED(hRes))
      return hRes;
  }
  if (bWriteNulTerminator != FALSE)
  {
    chA = 0;
    hRes = WriteProtected((LPSTR)lpRemoteDest+nSrcLen,  &chA, sizeof(CHAR));
    if (FAILED(hRes))
      return hRes;
  }
  return S_OK;
}

HRESULT CNktDvProcessMemory::WriteStringW(__in LPVOID lpRemoteDest, __in_nz_opt LPCWSTR szSrcW,
                                          __in SIZE_T nSrcLen, __in BOOL bWriteNulTerminator)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  WCHAR chW;

  if (lpRemoteDest == NULL || szSrcW == NULL)
    return E_POINTER;
  if (nSrcLen == NKT_SIZE_T_MAX)
    nSrcLen = wcslen(szSrcW);
  if (nSrcLen > 0)
  {
    hRes = WriteProtected((LPWSTR)lpRemoteDest, szSrcW, nSrcLen*sizeof(WCHAR));
    if (FAILED(hRes))
      return hRes;
  }
  if (bWriteNulTerminator != FALSE)
  {
    chW = 0;
    hRes = WriteProtected((LPWSTR)lpRemoteDest+nSrcLen,  &chW, sizeof(WCHAR));
    if (FAILED(hRes))
      return hRes;
  }
  return S_OK;
}

HRESULT CNktDvProcessMemory::GetStringLengthA(__in LPVOID lpRemoteDest, __out SIZE_T *lpnChars)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  CHAR chA;

  if (lpnChars == NULL)
    return E_POINTER;
  *lpnChars = 0;
  if (lpRemoteDest == NULL)
    return E_POINTER;
  for (;;)
  {
    hRes = Read(&chA, (LPSTR)lpRemoteDest+(*lpnChars), sizeof(CHAR));
    if (FAILED(hRes))
      return hRes;
    if (chA == 0)
      break;
    (*lpnChars)++;
  }
  return S_OK;
}

HRESULT CNktDvProcessMemory::GetStringLengthW(__in LPVOID lpRemoteDest, __out SIZE_T *lpnChars)
{
  CNktAutoFastMutex cLock(&GetLockObject());
  HRESULT hRes;
  WCHAR chW;

  if (lpnChars == NULL)
    return E_POINTER;
  *lpnChars = 0;
  if (lpRemoteDest == NULL)
    return E_POINTER;
  for (;;)
  {
    hRes = Read(&chW, (LPWSTR)lpRemoteDest+(*lpnChars), sizeof(WCHAR));
    if (FAILED(hRes))
      return hRes;
    if (chW == 0)
      break;
    (*lpnChars)++;
  }
  return S_OK;
}

HRESULT CNktDvProcessMemory::AllocMem(__deref_out LPVOID *lplpRemoteDest, __in SIZE_T nSize,
                                      __in BOOL bExecuteFlag)
{
  HANDLE hTempProc;
  DWORD dwProtFlags;

  if (lplpRemoteDest == NULL)
    return E_POINTER;
  *lplpRemoteDest = 0;
  if (nSize == 0)
    return E_INVALIDARG;
  //get process handle
  hTempProc = GetWriteAccessHandle();
  if (hTempProc == NULL)
    return E_ACCESSDENIED;
  //convert protection flags
  dwProtFlags = (bExecuteFlag != FALSE) ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
  //do allocation
  *lplpRemoteDest = ::VirtualAllocEx(hTempProc, NULL, nSize, MEM_RESERVE|MEM_COMMIT, dwProtFlags);
  return ((*lplpRemoteDest) != NULL) ? S_OK : E_FAIL;
}

HRESULT CNktDvProcessMemory::FreeMem(__in LPVOID lpRemoteDest)
{
  if (lpRemoteDest != NULL)
  {
    HANDLE hTempProc;

    //get process handle
    hTempProc = GetWriteAccessHandle();
    if (hTempProc == NULL)
      return E_ACCESSDENIED;
    //free memory
    if (::VirtualFreeEx(hTempProc, lpRemoteDest, 0, MEM_RELEASE) == FALSE)
      return E_FAIL;
  }
  return S_OK;
}

HRESULT CNktDvProcessMemory::ToString(__inout CNktStringW &cStrDest)
{
  if (cStrDest.Format(L"CNktDvProcessMemory [PID=%lu]", dwPid) == FALSE)
    return E_OUTOFMEMORY;
  return S_OK;
}

VOID CNktDvProcessMemory::FreeString(__in LPVOID lpString)
{
  nktMemFree(lpString);
  return;
}

HRESULT CNktDvProcessMemory::BuildParam(__deref_out CNktDvParam **lplpParam, __in SIZE_T nAddress,
                                        __in_z LPCWSTR szTypeW)
{
  TNktComPtr<CNktDvParam> cTempParam;
  SIZE_T nProcBits;
  HANDLE hTempProc;
  HRESULT hRes;

  if (lplpParam == NULL)
    return E_POINTER;
  *lplpParam = NULL;
  //get process handle
  hTempProc = GetReadAccessHandle();
  if (hTempProc == NULL)
    return E_ACCESSDENIED;
  nProcBits = (nktDvDynApis_Is64BitProcess(hTempProc) != FALSE) ? 64 : 32;
#if defined _M_IX86
  if (nProcBits > 32)
    return NKT_DVERR_NotSupported;
#endif //_M_IX86
  cTempParam.Attach(NKT_MEMMGR_NEW CNktDvParam(nProcBits));
  if (cTempParam == NULL)
    return E_OUTOFMEMORY;
  hRes = cTempParam->User_Init(this, (LPVOID)nAddress, szTypeW);
  if (SUCCEEDED(hRes))
    *lplpParam = cTempParam.Detach();
  return hRes;
}

HANDLE CNktDvProcessMemory::GetReadAccessHandle()
{
  if (hProc[0] == NULL)
  {
    CNktAutoFastMutex cLock(&GetLockObject());
    TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;

    if (hProc[0] == NULL)
    {
      if (SUCCEEDED(CNktDvProcessHandlesMgr::Get(&cPhMgr)))
        hProc[0] = cPhMgr->GetHandle(dwPid, NKT_ACCESS_READ);
    }
  }
  return hProc[0];
}

HANDLE CNktDvProcessMemory::GetWriteAccessHandle()
{
  if (hProc[1] == NULL)
  {
    CNktAutoFastMutex cLock(&GetLockObject());
    TNktComPtr<CNktDvProcessHandlesMgr> cPhMgr;

    if (hProc[1] == NULL && bWriteAccessChecked == FALSE)
    {
      bWriteAccessChecked = TRUE;
      if (SUCCEEDED(CNktDvProcessHandlesMgr::Get(&cPhMgr)))
        hProc[1] = cPhMgr->GetHandle(dwPid, NKT_ACCESS_WRITE);
    }
  }
  return hProc[1];
}
