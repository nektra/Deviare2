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

#include "StdAfx.h"
#include "..\..\SupportLibs\UDis86\GetInstructionLength.h"

//-----------------------------------------------------------

#pragma pack(1)
typedef struct {
  CHAR szApiNameA[128];
  DWORD dwOffset;
} CODE3_BUILDORIGINALNTCALL_API_ITEM;

typedef struct {
  DWORD dwPlatformBits;
  DWORD dwItemCount;
  DWORD dwDestSize;
  CODE3_BUILDORIGINALNTCALL_API_ITEM aItems[1];
} CODE3_BUILDORIGINALNTCALL_API;
#pragma pack()

//-----------------------------------------------------------

static SIZE_T BuildOriginalNtCall(__in LPVOID lpDest, __in LPBYTE lpFileFuncAddr, __in SIZE_T nPlatformBits);
static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount);

//-----------------------------------------------------------

HRESULT Internal1_Code3(__in SIZE_T nSubCodeA, __in SIZE_T nSubCodeB)
{
  CODE3_BUILDORIGINALNTCALL_API *lpHdr = (CODE3_BUILDORIGINALNTCALL_API*)nSubCodeA;
  LPBYTE *lplpDest = (LPBYTE*)nSubCodeB;
  WCHAR szBufW[4096];
  HANDLE hFile, hFileMap;
  union {
    LPVOID lpNtHdr;
    IMAGE_NT_HEADERS32 *lpNtHdr32;
    IMAGE_NT_HEADERS64 *lpNtHdr64;
  };
  LPBYTE lpData, lpDest, lpFileFuncAddr, lpCurrDest;
  SIZE_T i, k, nCodeSize, nSecCount, nCurrDestSize;
  IMAGE_SECTION_HEADER *lpFileImgSect;
  IMAGE_DATA_DIRECTORY *lpFileExportsDir;
  IMAGE_EXPORT_DIRECTORY *lpExpDir;
  DWORD dwRawAddr, *lpdwAddressOfFunctions, *lpdwAddressOfNames;
  WORD wOrd, *lpwAddressOfNameOrdinals;
  LPSTR sA;
  HRESULT hRes;

  if (lpHdr == NULL || lplpDest == NULL)
    return E_POINTER;
  lpHdr->dwDestSize = 0;
  *lplpDest = NULL;
  hFile = hFileMap = NULL;
  lpData = NULL;
  //----
  for (i=0; i<(SIZE_T)(lpHdr->dwItemCount); i++)
    lpHdr->aItems[i].dwOffset = 0xFFFFFFFFUL;
  nCurrDestSize = (SIZE_T)(lpHdr->dwItemCount) * 256;
  lpCurrDest = lpDest = (LPBYTE)::GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, nCurrDestSize);
  if (lpDest == NULL)
    return E_OUTOFMEMORY;
  //map ntdll
  i = ::GetWindowsDirectoryW(szBufW, 4000);
  if (i > 0 && szBufW[i-1] != L'\\')
    szBufW[i++] = L'\\';
  szBufW[i] = 0;
  switch (lpHdr->dwPlatformBits)
  {
#if defined _M_IX86
    case 32:
      wcscat_s(szBufW, L"\\System32\\ntdll.dll");
      break;
#elif defined _M_X64
    case 32:
      wcscat_s(szBufW, L"\\SysWow64\\ntdll.dll");
      break;
    case 64:
      wcscat_s(szBufW, L"\\System32\\ntdll.dll");
      break;
#endif
    default:
      _ASSERT(FALSE);
      ::GlobalFree(lpDest);
      return E_FAIL;
  }
  hFile = ::CreateFileW(szBufW, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
  hRes = (hFile != NULL && hFile != INVALID_HANDLE_VALUE) ? S_OK : NKT_HRESULT_FROM_LASTERROR();
  if (SUCCEEDED(hRes))
  {
    hFileMap = ::CreateFileMappingW(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMap == NULL || hFileMap == INVALID_HANDLE_VALUE)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (SUCCEEDED(hRes))
  {
    lpData = (LPBYTE)::MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
    if (lpData == NULL)
      hRes = NKT_HRESULT_FROM_LASTERROR();
  }
  if (SUCCEEDED(hRes))
  {
    if (*((WORD UNALIGNED *)(lpData+FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic))) == IMAGE_DOS_SIGNATURE)
    {
      //get header offset
      lpNtHdr = lpData + (SIZE_T)*((DWORD UNALIGNED *)(lpData+FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew)));
      switch (lpNtHdr32->FileHeader.Machine)
      {
        case IMAGE_FILE_MACHINE_I386:
          if (lpHdr->dwPlatformBits != 32 || lpNtHdr32->Signature != IMAGE_NT_SIGNATURE)
            hRes = E_FAIL;
          break;
        case IMAGE_FILE_MACHINE_AMD64:
          if (lpHdr->dwPlatformBits != 64 || lpNtHdr64->Signature != IMAGE_NT_SIGNATURE)
            hRes = E_FAIL;
          break;
        default:
          hRes = E_FAIL;
          break;
      }
    }
    else
    {
      hRes = E_FAIL;
    }
  }
  if (SUCCEEDED(hRes))
  {
    switch (lpHdr->dwPlatformBits)
    {
      case 32:
        nSecCount = (SIZE_T)(lpNtHdr32->FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpNtHdr32+1);
        lpFileExportsDir = &(lpNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#if defined _M_X64
      case 64:
        nSecCount = (SIZE_T)(lpNtHdr64->FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpNtHdr64+1);
        lpFileExportsDir = &(lpNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#endif //_M_X64
    }
    //check exports
    if (SUCCEEDED(hRes) && lpFileExportsDir->VirtualAddress != 0 && lpFileExportsDir->Size > 0)
    {
      dwRawAddr = HelperConvertVAToRaw(lpFileExportsDir->VirtualAddress, lpFileImgSect, nSecCount);
      if (dwRawAddr != 0)
      {
        lpExpDir = (IMAGE_EXPORT_DIRECTORY*)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfFunctions, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpdwAddressOfFunctions = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNames, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpdwAddressOfNames = (LPDWORD)(lpData + (SIZE_T)dwRawAddr);
        dwRawAddr = HelperConvertVAToRaw(lpExpDir->AddressOfNameOrdinals, lpFileImgSect, nSecCount);
      }
      if (dwRawAddr != 0)
      {
        lpwAddressOfNameOrdinals = (LPWORD)(lpData + (SIZE_T)dwRawAddr);
        for (i=0; i<lpExpDir->NumberOfNames; i++)
        {
          dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfNames[i], lpFileImgSect, nSecCount);
          if (dwRawAddr == 0)
            continue;
          sA = (LPSTR)(lpData + (SIZE_T)dwRawAddr);
          for (k=0; k<(SIZE_T)(lpHdr->dwItemCount); k++)
          {
            if (lpHdr->aItems[k].dwOffset == 0xFFFFFFFFUL)
            {
              if (_stricmp(sA, lpHdr->aItems[k].szApiNameA) == 0)
                break;
              //try nt/zw
              if (_strnicmp(lpHdr->aItems[k].szApiNameA, "nt", 2) == 0 &&
                  _strnicmp(sA, "zw", 2) == 0 &&
                  _stricmp(sA+2, lpHdr->aItems[k].szApiNameA+2) == 0)
                break;
            }
          }
          if (k < (SIZE_T)(lpHdr->dwItemCount))
          {
            wOrd = lpwAddressOfNameOrdinals[i];
            dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfFunctions[wOrd], lpFileImgSect, nSecCount);
            lpFileFuncAddr = lpData + (SIZE_T)dwRawAddr;
            if (lpFileFuncAddr >= (LPBYTE)lpExpDir &&
                lpFileFuncAddr < (LPBYTE)lpExpDir+(SIZE_T)(lpFileExportsDir->Size))
              continue; //a forwarded function => ignore
            //create new stub
            lpHdr->aItems[k].dwOffset = (DWORD)(lpCurrDest-lpDest);
            nCodeSize = BuildOriginalNtCall(lpCurrDest, lpFileFuncAddr, lpHdr->dwPlatformBits);
            if (nCodeSize == 0)
            {
              hRes = E_FAIL;
              break;
            }
            lpCurrDest += nCodeSize;
          }
        }
      }
    }
  }
  if (lpData != NULL)
    ::UnmapViewOfFile(lpData);
  if (hFileMap != NULL && hFileMap != INVALID_HANDLE_VALUE)
    ::CloseHandle(hFileMap);
  if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    ::CloseHandle(hFile);
  if (SUCCEEDED(hRes))
  {
    for (k=0; k<(SIZE_T)(lpHdr->dwItemCount) && lpHdr->aItems[k].dwOffset!=0xFFFFFFFFUL; k++);
    if (k < (SIZE_T)(lpHdr->dwItemCount))
      hRes = HRESULT_FROM_NT(STATUS_ENTRYPOINT_NOT_FOUND);
  }
  if (SUCCEEDED(hRes))
  {
    *lplpDest = lpDest;
    lpHdr->dwDestSize= (DWORD)(SIZE_T)(lpCurrDest-lpDest);
  }
  else
  {
    ::GlobalFree(lpDest);
  }
  return hRes;
}

static SIZE_T BuildOriginalNtCall(__in LPVOID lpDest, __in LPBYTE lpFileFuncAddr, __in SIZE_T nPlatformBits)
{
  SIZE_T i, k, nInstrLen, nCurrSize, nExtraSize;
  LPBYTE lpSrc, d, lpStub;

  //stage 1: scan for a return
  nCurrSize = nExtraSize = 0;
  while (nCurrSize < 128)
  {
    if (lpFileFuncAddr[nCurrSize] == 0xC2)
    {
      nCurrSize += 3;
      break;
    }
    if (lpFileFuncAddr[nCurrSize] == 0xC3)
    {
      nCurrSize++;
      break;
    }
    if (lpFileFuncAddr[nCurrSize] == 0xE8)
    {
      //near call found, calculate size too
      lpSrc = lpFileFuncAddr+nCurrSize+5 + (SSIZE_T) *((LONG NKT_UNALIGNED*)(lpFileFuncAddr+nCurrSize+1));
      nCurrSize += 5;
      k = 0;
      while (k < 128)
      {
        if (lpSrc[k] == 0xC2)
        {
          k += 3;
          break;
        }
        if (lpSrc[k] == 0xC3)
        {
          k++;
          break;
        }
        nInstrLen = GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
        k += nInstrLen;
      }
      //add to extra size
      if (k >= 128)
        return 0; //unsupported size
      nExtraSize += k;
    }
    else
    {
      nInstrLen = GetInstructionLength(lpFileFuncAddr+nCurrSize, 128, (BYTE)nPlatformBits, NULL);
      nCurrSize += nInstrLen;
    }
  }
  if (nCurrSize >= 128)
    return 0; //unsupported size
  if (lpDest != NULL)
  {
    //stage 2: copy
    lpStub = (LPBYTE)lpDest;
    i = nExtraSize = 0;
    while (i < 128)
    {
      if (lpFileFuncAddr[i] == 0xC2)
      {
        memcpy(lpStub+i, lpFileFuncAddr+i, 3);
        i += 3;
        break;
      }
      if (lpFileFuncAddr[i] == 0xC3)
      {
        lpStub[i] = lpFileFuncAddr[i];
        i++;
        break;
      }
      if (lpFileFuncAddr[i] == 0xE8)
      {
        //near call found, relocate
        lpStub[i] = 0xE8;
        *((ULONG NKT_UNALIGNED*)(lpStub+i+1)) = (ULONG)(nCurrSize+nExtraSize - (i+5));
        lpSrc = lpFileFuncAddr+i+5 + (SSIZE_T)*((LONG NKT_UNALIGNED*)(lpFileFuncAddr+i+1));
        i += 5;
        d = lpStub+nCurrSize+nExtraSize;
        k = 0;
        while (k < 128)
        {
          if (lpSrc[k] == 0xC2)
          {
            memcpy(d+k, lpSrc+k, 3);
            k += 3;
            break;
          }
          if (lpSrc[k] == 0xC3)
          {
            d[k] = lpSrc[k];
            k++;
            break;
          }
          nInstrLen = GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
          memcpy(d+k, lpSrc+k, nInstrLen);
          k += nInstrLen;
        }
        //add to extra size
        _ASSERT(k < 128);
        nExtraSize += k;
      }
      else
      {
        nInstrLen = GetInstructionLength(lpFileFuncAddr+i, 128, (BYTE)nPlatformBits, NULL);
        memcpy(lpStub+i, lpFileFuncAddr+i, nInstrLen);
        i += nInstrLen;
      }
    }
    _ASSERT(i == nCurrSize);
  }
  return nCurrSize+nExtraSize;
}

static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount)
{
  SIZE_T i;

  for (i=0; i<nSecCount; i++)
  {
    if (dwVirtAddr >= lpImgSect[i].VirtualAddress &&
        dwVirtAddr < lpImgSect[i].VirtualAddress + lpImgSect[i].Misc.VirtualSize)
      return dwVirtAddr - lpImgSect[i].VirtualAddress + lpImgSect[i].PointerToRawData;
  }
  return 0;
}
