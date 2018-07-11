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

#define ViewShare 1
#if defined(_M_X64) || defined(_M_IA64) || defined(_M_AMD64)
  #define NKT_UNALIGNED __unaligned
#else
  #define NKT_UNALIGNED
#endif

using namespace NktHookLib::Internals;

//-----------------------------------------------------------

static SIZE_T GenerateNtSysCall(__in LPVOID lpDest, __in LPBYTE lpFileFuncAddr, __in SIZE_T nPlatformBits,
                                __in LPBYTE lpData, __in IMAGE_SECTION_HEADER *lpFileImgSect, __in SIZE_T nSecCount);
static DWORD HelperConvertVAToRaw(__in DWORD dwVirtAddr, __in IMAGE_SECTION_HEADER *lpImgSect, __in SIZE_T nSecCount);
static BOOL IsNtOrZw(__in_z LPCSTR szApiA);
static BOOL StrCompareNoCaseA(__in_z LPCSTR szStr1, __in_z LPCSTR szStr2);

//-----------------------------------------------------------

namespace NktHookLibHelpers {

DWORD BuildNtSysCalls(__in LPSYSCALLDEF lpDefs, __in SIZE_T nDefsCount, __in SIZE_T nPlatform,
                      __out_opt LPVOID lpCode, __out SIZE_T *lpnCodeSize)

{
  static const UNICODE_STRING usSystem32NtDll = { 60, 60, L"\\SystemRoot\\System32\\ntdll.dll" };
  static const UNICODE_STRING usSysWow64NtDll = { 60, 60, L"\\SystemRoot\\SysWow64\\ntdll.dll" };
  OBJECT_ATTRIBUTES sObjAttr;
  IO_STATUS_BLOCK sIoStatusBlock;
  LARGE_INTEGER liOffset;
  SIZE_T nViewSize;
  LONG nNtStatus;
  HANDLE hFile, hFileMap;
  union {
    LPVOID lpNtHdr;
    IMAGE_NT_HEADERS32 *lpNtHdr32;
    IMAGE_NT_HEADERS64 *lpNtHdr64;
  };
  LPBYTE lpData, lpFileFuncAddr;
  SIZE_T i, k, nCodeSize, nSecCount, nDestSize, nPlatformBits;
  IMAGE_SECTION_HEADER *lpFileImgSect;
  IMAGE_DATA_DIRECTORY *lpFileExportsDir;
  IMAGE_EXPORT_DIRECTORY *lpExpDir;
  DWORD dwRawAddr, *lpdwAddressOfFunctions, *lpdwAddressOfNames;
  WORD wOrd, *lpwAddressOfNameOrdinals;
  LPSTR sA;

  if (lpnCodeSize != NULL)
    *lpnCodeSize = 0;
  if (lpDefs == NULL || nDefsCount == 0 || lpnCodeSize == NULL)
    return ERROR_INVALID_PARAMETER;
  for (i=0; i<nDefsCount; i++)
    lpDefs[i].nOffset = (SIZE_T)-1;
  switch (nPlatform)
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      nPlatformBits = 32;
      break;
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
      nPlatformBits = 64;
      break;
#endif //_M_X64
      break;
    default:
      return ERROR_INVALID_PARAMETER;
  }
  hFile = hFileMap = NULL;
  lpData = NULL;
  nDestSize = 0;
  //open and map ntdll. NOTE: We are calling NtCreateFile so FS redirection is disabled.
  switch (GetProcessorArchitecture())
  {
    case PROCESSOR_ARCHITECTURE_INTEL:
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX86:
          InitializeObjectAttributes(&sObjAttr, (PUNICODE_STRING)&usSystem32NtDll, OBJ_CASE_INSENSITIVE, NULL, NULL);
          break;
        default:
          return ERROR_INVALID_PARAMETER;
      }
      break;

    case PROCESSOR_ARCHITECTURE_AMD64:
      switch (nPlatform)
      {
        case NKTHOOKLIB_ProcessPlatformX86:
          InitializeObjectAttributes(&sObjAttr, (PUNICODE_STRING)&usSysWow64NtDll, OBJ_CASE_INSENSITIVE, NULL, NULL);
          break;
#if defined(_M_X64)
        case NKTHOOKLIB_ProcessPlatformX64:
          InitializeObjectAttributes(&sObjAttr, (PUNICODE_STRING)&usSystem32NtDll, OBJ_CASE_INSENSITIVE, NULL, NULL);
          break;
#endif //_M_X64
        default:
          return ERROR_INVALID_PARAMETER;
      }
      break;

    default:
      //GetProcessorArchitecture failing?
      return ERROR_INVALID_FUNCTION;
  }
  nNtStatus = NktNtCreateFile(&hFile, GENERIC_READ|SYNCHRONIZE|FILE_READ_ATTRIBUTES, &sObjAttr, &sIoStatusBlock,
                              NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ|FILE_SHARE_DELETE, FILE_OPEN,
                              FILE_SYNCHRONOUS_IO_NONALERT|FILE_NON_DIRECTORY_FILE, NULL, 0);
  if (NT_SUCCESS(nNtStatus))
  {
    InitializeObjectAttributes(&sObjAttr, NULL, 0, NULL, NULL);
    nNtStatus = NktNtCreateSection(&hFileMap, STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|SECTION_MAP_READ, &sObjAttr,
                                   NULL, PAGE_READONLY, SEC_COMMIT, hFile);
  }
  if (NT_SUCCESS(nNtStatus))
  {
    liOffset.QuadPart = 0;
    nViewSize = 0;
    nNtStatus = NktNtMapViewOfSection(hFileMap, NKTHOOKLIB_CurrentProcess, (PVOID*)&lpData, 0, 0, &liOffset,
                                      &nViewSize, ViewShare, 0, PAGE_READONLY);
  }
  //scan exports and generate code
  if (NT_SUCCESS(nNtStatus))
  {
    if (*((WORD UNALIGNED *)(lpData+FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic))) == IMAGE_DOS_SIGNATURE)
    {
      //get header offset
      lpNtHdr = lpData + (SIZE_T)*((DWORD UNALIGNED *)(lpData+FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew)));
      switch (lpNtHdr32->FileHeader.Machine)
      {
        case IMAGE_FILE_MACHINE_I386:
          if (nPlatform != NKTHOOKLIB_ProcessPlatformX86 || lpNtHdr32->Signature != IMAGE_NT_SIGNATURE)
            nNtStatus = STATUS_INVALID_PARAMETER;
          break;
        case IMAGE_FILE_MACHINE_AMD64:
          if (nPlatform != NKTHOOKLIB_ProcessPlatformX64 || lpNtHdr64->Signature != IMAGE_NT_SIGNATURE)
            nNtStatus = STATUS_INVALID_PARAMETER;
          break;
        default:
          nNtStatus = STATUS_INVALID_PARAMETER;
          break;
      }
    }
    else
    {
      nNtStatus = STATUS_UNSUCCESSFUL;
    }
  }
  if (NT_SUCCESS(nNtStatus))
  {
    switch (nPlatform)
    {
      case NKTHOOKLIB_ProcessPlatformX86:
        nSecCount = (SIZE_T)(lpNtHdr32->FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpNtHdr32+1);
        lpFileExportsDir = &(lpNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#if defined(_M_X64)
      case NKTHOOKLIB_ProcessPlatformX64:
        nSecCount = (SIZE_T)(lpNtHdr64->FileHeader.NumberOfSections);
        lpFileImgSect = (IMAGE_SECTION_HEADER*)(lpNtHdr64+1);
        lpFileExportsDir = &(lpNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
        break;
#endif //_M_X64
    }
    //check exports
    if (lpFileExportsDir->VirtualAddress != 0 && lpFileExportsDir->Size > 0)
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
          for (k=0; k<nDefsCount; k++)
          {
            if (lpDefs[k].nOffset == (SIZE_T)-1)
            {
              if (StrCompareNoCaseA(sA, lpDefs[k].szNtApiNameA) != FALSE)
                break;
              //try nt/zw
              if (IsNtOrZw(lpDefs[k].szNtApiNameA) != FALSE &&
                  IsNtOrZw(sA) != FALSE &&
                  StrCompareNoCaseA(sA+2, lpDefs[k].szNtApiNameA+2) != FALSE)
                break;
            }
          }
          if (k < nDefsCount)
          {
            wOrd = lpwAddressOfNameOrdinals[i];
            dwRawAddr = HelperConvertVAToRaw(lpdwAddressOfFunctions[wOrd], lpFileImgSect, nSecCount);
            lpFileFuncAddr = lpData + (SIZE_T)dwRawAddr;
            if (lpFileFuncAddr >= (LPBYTE)lpExpDir &&
                lpFileFuncAddr < (LPBYTE)lpExpDir+(SIZE_T)(lpFileExportsDir->Size))
              continue; //a forwarded function => ignore
            //create new stub
            lpDefs[k].nOffset = nDestSize;
            nCodeSize = GenerateNtSysCall((lpCode != NULL) ? (LPBYTE)lpCode+nDestSize : NULL, lpFileFuncAddr,
                                          nPlatformBits, lpData, lpFileImgSect, nSecCount);
            if (nCodeSize == 0)
            {
              nNtStatus = STATUS_UNSUCCESSFUL;
              break;
            }
            nDestSize += nCodeSize;
          }
        }
      }
    }
  }
  //free resources
  if (lpData != NULL)
    NktNtUnmapViewOfSection(NKTHOOKLIB_CurrentProcess, (PVOID)lpData);
  if (hFileMap != NULL)
    NktNtClose(hFileMap);
  if (hFile != NULL)
    NktNtClose(hFile);
  //check if all requested entrypoints were found
  if (NT_SUCCESS(nNtStatus))
  {
    for (k=0; k<nDefsCount && lpDefs[k].nOffset!=(SIZE_T)-1; k++);
    if (k < nDefsCount)
      nNtStatus = STATUS_ENTRYPOINT_NOT_FOUND;
  }
  //almost done
  if (!NT_SUCCESS(nNtStatus))
    return NktRtlNtStatusToDosError(nNtStatus);
  *lpnCodeSize = nDestSize;
  return NOERROR;
}

} //namespace NktHookLibHelpers

//-----------------------------------------------------------

static SIZE_T GenerateNtSysCall(__in LPVOID lpDest, __in LPBYTE lpFileFuncAddr, __in SIZE_T nPlatformBits,
                                __in LPBYTE lpData, __in IMAGE_SECTION_HEADER *lpFileImgSect, __in SIZE_T nSecCount)
{
  SIZE_T k, nSrcOfs, nInstrLen, nCurrSize, nExtraSize, nDestSize, nMainCodeSize;
  DWORD dwRawAddr, dwOsVerMajor, dwOsVerMinor, dwOsVerBuildNumber;
  LPBYTE lpSrc, d, lpStub;

  //check OS version
  if (NktHookLibHelpers::GetOsVersion(&dwOsVerMajor, &dwOsVerMinor, &dwOsVerBuildNumber) == FALSE)
    dwOsVerMajor = dwOsVerMinor = dwOsVerBuildNumber = 0;
  //stage 1: scan for a return
  nSrcOfs = nCurrSize = nExtraSize = 0;
  while (nCurrSize < 128)
  {
    if (lpFileFuncAddr[nSrcOfs] == 0xC2)
    {
      nSrcOfs += 3;
      nCurrSize += 3;
      break;
    }
    if (lpFileFuncAddr[nSrcOfs] == 0xC3)
    {
      //handle special case for Windows 10 x64 Anniversary to Fall Creators Update
      if (nPlatformBits == 64 && dwOsVerMajor >= 10 &&
          lpFileFuncAddr[nSrcOfs + 1] == 0xCD && lpFileFuncAddr[nSrcOfs + 2] == 0x2E)
      {
        nSrcOfs++;
        nCurrSize++;
      }
      else
      {
        nSrcOfs++;
        nCurrSize++;
        break;
      }
    }
    //handle special case for Windows 10 x86
    if (nPlatformBits == 32 && dwOsVerMajor >= 10 && lpFileFuncAddr[nSrcOfs] == 0xBA &&
        lpFileFuncAddr[nSrcOfs+5] == 0xFF && lpFileFuncAddr[nSrcOfs+6] == 0xD2)
    {
      //mov EDX, OFFSET _Wow64SystemServiceCall@0 / call EDX
      dwRawAddr = HelperConvertVAToRaw(*((ULONG NKT_UNALIGNED*)(lpFileFuncAddr+nSrcOfs+1)), lpFileImgSect, nSecCount);
      lpSrc = lpData + dwRawAddr;
      nSrcOfs += 7;
      nCurrSize += 5;
      k = nDestSize = 0;
      while (nDestSize < 128)
      {
        if (lpSrc[k] == 0xFF && lpSrc[k+1] == 0xE2) //jmp EDX
        {
          nDestSize += 2;
          break;
        }
        if (lpSrc[k] == 0xEA && lpSrc[k+5] == 0x33 && lpSrc[k+6] == 0x00) //jmp 0x33:OFFSET
        {
          k += 7;
          nDestSize += 12;
          continue;
        }
        nInstrLen = NktHookLibHelpers::GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
        k += nInstrLen;
        nDestSize += nInstrLen;
      }
      //add to extra size
      if (nDestSize >= 128)
        return NULL; //unsupported size
      nExtraSize += nDestSize;
      continue;
    }
    if (lpFileFuncAddr[nCurrSize] == 0xE8)
    {
      //near call found, calculate size too
      lpSrc = lpFileFuncAddr+nSrcOfs+5 + (SSIZE_T) *((LONG NKT_UNALIGNED*)(lpFileFuncAddr+nSrcOfs+1));
      nSrcOfs += 5;
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
        nInstrLen = NktHookLibHelpers::GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
        k += nInstrLen;
      }
      //add to extra size
      if (k >= 128)
        return 0; //unsupported size
      nExtraSize += k;
      continue;
    }
    //no special behavior
    nInstrLen = NktHookLibHelpers::GetInstructionLength(lpFileFuncAddr+nCurrSize, 128, (BYTE)nPlatformBits, NULL);
    nSrcOfs += nInstrLen;
    nCurrSize += nInstrLen;
  }
  if (nCurrSize+nExtraSize > 256)
    return 0; //unsupported size
  nMainCodeSize = nCurrSize;
  if (lpDest != NULL)
  {
    //stage 2: copy
    lpStub = (LPBYTE)lpDest;
    nSrcOfs = nCurrSize = nExtraSize = 0;
    while (nCurrSize < 128)
    {
      if (lpFileFuncAddr[nSrcOfs] == 0xC2)
      {
        NktHookLibHelpers::MemCopy(lpStub+nCurrSize, lpFileFuncAddr+nSrcOfs, 3);
        nSrcOfs += 3;
        nCurrSize += 3;
        break;
      }
      if (lpFileFuncAddr[nSrcOfs] == 0xC3)
      {
        //handle special case for Windows 10 x64 anniversary
        if (nPlatformBits == 64 && dwOsVerMajor >= 10 &&
            lpFileFuncAddr[nSrcOfs + 1] == 0xCD && lpFileFuncAddr[nSrcOfs + 2] == 0x2E)
        {
          lpStub[nCurrSize++] = lpFileFuncAddr[nSrcOfs++];
        }
        else
        {
          lpStub[nCurrSize++] = lpFileFuncAddr[nSrcOfs++];
          break;
        }
      }
      //handle special case for Windows 10 x86
      if (nPlatformBits == 32 && dwOsVerMajor >= 10 && lpFileFuncAddr[nSrcOfs] == 0xBA &&
          lpFileFuncAddr[nSrcOfs+5] == 0xFF && lpFileFuncAddr[nSrcOfs+6] == 0xD2)
      {
        //mov EDX, OFFSET _Wow64SystemServiceCall@0 / call EDX
        dwRawAddr = HelperConvertVAToRaw(*((ULONG NKT_UNALIGNED*)(lpFileFuncAddr+nSrcOfs+1)), lpFileImgSect, nSecCount);
        lpStub[nCurrSize] = 0xE8;
        *((ULONG NKT_UNALIGNED*)(lpStub+nCurrSize+1)) = (ULONG)(nMainCodeSize+nExtraSize - (nCurrSize+5));
        lpSrc = lpData + dwRawAddr;
        nSrcOfs += 7;
        nCurrSize += 5;
        d = lpStub+nMainCodeSize+nExtraSize;
        k = nDestSize = 0;
        while (nDestSize < 128)
        {
          if (lpSrc[k] == 0xFF && lpSrc[k+1] == 0xE2) //jmp EDX
          {
            NktHookLibHelpers::MemCopy(d+nDestSize, lpSrc+k, 2);
            nDestSize += 2;
            break;
          }
          if (lpSrc[k] == 0xEA && lpSrc[k+5] == 0x33 && lpSrc[k+6] == 0x00) //jmp 0x33:OFFSET
          {
            d[nDestSize] = 0x6A; d[nDestSize+1] = 0x33;                     //push 0x0033
            d[nDestSize+2] = 0xE8;                                          //call +0
            d[nDestSize+3] = d[nDestSize+4] =
              d[nDestSize+5] = d[nDestSize+6] = 0x00;
            d[nDestSize+7] = 0x83;  d[nDestSize+8] = 0x05;                  //add DWORD PTR [esp], 5
            d[nDestSize+9] = 0x24;  d[nDestSize+10] = 0x04;
            d[nDestSize+11] = 0xCB;                                         //retf
            k += 7;
            nDestSize += 12;
            continue;
          }
          nInstrLen = NktHookLibHelpers::GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
          NktHookLibHelpers::MemCopy(d+nDestSize, lpSrc+k, nInstrLen);
          k += nInstrLen;
          nDestSize += nInstrLen;
        }
        //add to extra size
        NKT_ASSERT(nDestSize < 128);
        nExtraSize += nDestSize;
        continue;
      }
      if (lpFileFuncAddr[nSrcOfs] == 0xE8)
      {
        //near call found, relocate
        lpStub[nSrcOfs] = 0xE8;
        *((ULONG NKT_UNALIGNED*)(lpStub+nSrcOfs+1)) = (ULONG)(nMainCodeSize+nExtraSize - (nSrcOfs+5));
        lpSrc = lpFileFuncAddr+nSrcOfs+5 + (SSIZE_T) *((LONG NKT_UNALIGNED*)(lpFileFuncAddr+nSrcOfs+1));
        nSrcOfs += 5;
        nCurrSize += 5;
        d = lpStub+nMainCodeSize+nExtraSize;
        k = 0;
        while (k < 128)
        {
          if (lpSrc[k] == 0xC2)
          {
            NktHookLibHelpers::MemCopy(d+k, lpSrc+k, 3);
            k += 3;
            break;
          }
          if (lpSrc[k] == 0xC3)
          {
            d[k] = lpSrc[k];
            k++;
            break;
          }
          nInstrLen = NktHookLibHelpers::GetInstructionLength(lpSrc+k, 128, (BYTE)nPlatformBits, NULL);
          NktHookLibHelpers::MemCopy(d+k, lpSrc+k, nInstrLen);
          k += nInstrLen;
        }
        //add to extra size
        NKT_ASSERT(k < 128);
        nExtraSize += k;
        continue;
      }
      nInstrLen = NktHookLibHelpers::GetInstructionLength(lpFileFuncAddr+nSrcOfs, 128, (BYTE)nPlatformBits, NULL);
      NktHookLibHelpers::MemCopy(lpStub+nCurrSize, lpFileFuncAddr+nSrcOfs, nInstrLen);
      nSrcOfs += nInstrLen;
      nCurrSize += nInstrLen;
    }
    NKT_ASSERT(nMainCodeSize == nCurrSize);
  }
  return nMainCodeSize+nExtraSize;
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

static BOOL IsNtOrZw(__in_z LPCSTR szApiA)
{
  if ((szApiA[0] == 'n' || szApiA[0] == 'N') &&
      (szApiA[1] == 't' || szApiA[1] == 'T'))
    return TRUE;
  if ((szApiA[0] == 'z' || szApiA[0] == 'Z') &&
      (szApiA[1] == 'w' || szApiA[1] == 'W'))
    return TRUE;
  return FALSE;
}

static BOOL StrCompareNoCaseA(__in_z LPCSTR szStr1, __in_z LPCSTR szStr2)
{
  STRING s[2];
  
  s[0].Buffer = (PCHAR)szStr1;
  for (s[0].Length=0; szStr1[s[0].Length]!=0; s[0].Length++);
  s[0].MaximumLength = s[0].Length;
  s[1].Buffer = (PCHAR)szStr2;
  for (s[1].Length=0; szStr1[s[1].Length]!=0; s[1].Length++);
  s[1].MaximumLength = s[1].Length;
  return (NktRtlCompareString(&s[0], &s[1], TRUE) == 0) ? TRUE : FALSE;
}
