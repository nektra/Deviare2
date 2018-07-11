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

#include "DynamicNtApi.h"
#include <NktHookLib.h>

using namespace NktHookLib::Internals;
using namespace NktHookLibHelpers;

//-----------------------------------------------------------

static HRESULT GetPeb(__out LPBYTE *lplpPeb, __in HANDLE hProcess, __in SIZE_T nPlatformBits);
static HRESULT CheckImageType(__out NKT_HK_IMAGE_NT_HEADER *lpNtHdr, __in HANDLE hProcess, __in LPBYTE lpBaseAddr,
                              __out_opt LPBYTE *lplpNtHdrAddr=NULL);
static HRESULT GetFirstLdrEntry32(__out NKT_HK_LDRENTRY32 *lpEntry32, __in LPBYTE lpPeb, __in HANDLE hProcess);
static HRESULT GetNextLdrEntry32(__inout NKT_HK_LDRENTRY32 *lpEntry32);
#if defined(_M_X64)
static HRESULT GetFirstLdrEntry64(__out NKT_HK_LDRENTRY64 *lpEntry64, __in LPBYTE lpPeb, __in HANDLE hProcess);
static HRESULT GetNextLdrEntry64(__inout NKT_HK_LDRENTRY64 *lpEntry64);
#endif //_M_X64
static BOOL RemoteCompareUnicodeString32(__in HANDLE hProcess, __in NKT_HK_UNICODE_STRING32 *lpRemoteStr,
                                         __in LPCWSTR szLocalStrW, __in SIZE_T nStrLen);
#if defined(_M_X64)
static BOOL RemoteCompareUnicodeString64(__in HANDLE hProcess, __in NKT_HK_UNICODE_STRING64 *lpRemoteStr,
                                         __in LPCWSTR szLocalStrW, __in SIZE_T nStrLen);
#endif //_M_X64
static BOOL RemoteStrNICmpW(__in HANDLE hProcess, __in LPCWSTR szRemoteStrW, __in LPCWSTR szLocalStrW,
                            __in SIZE_T nStrLen);
static BOOL StrICmpA(__in LPCSTR sA_1, __in LPCSTR sA_2);
static BOOL StrNICmpW(__in LPCWSTR sW_1, __in LPCWSTR sW_2, __in SIZE_T nLen);
static DWORD ToNumberA(__in LPCSTR sA);
static LPVOID FindDllInApiSetCheck(__in HANDLE hProcess, __in SIZE_T nPlatformBits, __in LPVOID lpImportingDllBase,
                                   __in LPCWSTR szDllToFindW);

//-----------------------------------------------------------

namespace NktHookLib {
namespace Internals {

HINSTANCE GetRemoteModuleBaseAddress(__in HANDLE hProcess, __in_z LPCWSTR szDllNameW, __in BOOL bScanMappedImages)
{
  SIZE_T k, nPlatformBits, nDllNameLen;
  LPBYTE lpPeb;
  DWORD dwTries, dwType;
  LARGE_INTEGER liTime;
  union {
    NKT_HK_LDRENTRY32 sLdrEntry32;
#if defined(_M_X64)
    NKT_HK_LDRENTRY64 sLdrEntry64;
#endif //_M_X64
  };
  struct {
    NKT_HK_UNICODE_STRING us;
    WCHAR chDataW[2048];
  } usImportingDll;
  MEMORY_BASIC_INFORMATION sMbi;
  LPBYTE lpBaseAddr, lpCurrAddress;
  BOOL bContinue;
  NKT_HK_IMAGE_NT_HEADER sNtHdr;
  HRESULT hRes;

  if (szDllNameW == NULL || szDllNameW[0] == 0)
    return NULL;
  for (nDllNameLen=0; szDllNameW[nDllNameLen]!=0; nDllNameLen++);
  switch (GetProcessPlatform(hProcess))
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      nPlatformBits = 32;
      break;
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
      nPlatformBits = 64;
      break;
#endif //_M_X64
    default:
      return NULL;
  }
  if (FAILED(GetPeb(&lpPeb, hProcess, nPlatformBits)))
    return NULL;
  for (dwTries=10; dwTries>0; dwTries--)
  {
    switch (nPlatformBits)
    {
      case 32:
        hRes = GetFirstLdrEntry32(&sLdrEntry32, lpPeb, hProcess);
        while (hRes == S_OK)
        {
          if (sLdrEntry32.sEntry.DllBase != 0 &&
              RemoteCompareUnicodeString32(hProcess, &(sLdrEntry32.sEntry.BaseDllName), szDllNameW, nDllNameLen) != FALSE)
            return (HINSTANCE)(SIZE_T)(sLdrEntry32.sEntry.DllBase);
          hRes = GetNextLdrEntry32(&sLdrEntry32);
        }
        if (hRes == S_FALSE)
          hRes = S_OK;
        break;

#if defined(_M_X64)
      case 64:
        hRes = GetFirstLdrEntry64(&sLdrEntry64, lpPeb, hProcess);
        while (hRes == S_OK)
        {
          if (sLdrEntry64.sEntry.DllBase != 0 &&
              RemoteCompareUnicodeString64(hProcess, &(sLdrEntry64.sEntry.BaseDllName), szDllNameW, nDllNameLen) != FALSE)
            return (HINSTANCE)(sLdrEntry64.sEntry.DllBase);
          hRes = GetNextLdrEntry64(&sLdrEntry64);
        }
        if (hRes == S_FALSE)
          hRes = S_OK;
        break;
#endif //_M_X64

      default:
        return NULL;
    }
    if (hRes != E_FAIL)
      break;
    if (dwTries > 1)
    {
      liTime.QuadPart = -1000;
      NktNtDelayExecution(FALSE, &liTime);
    }
  }
  //try scanning mapped image files
  if (FAILED(hRes) && bScanMappedImages != FALSE)
  {
    lpCurrAddress = NULL;
    bContinue = TRUE;
    while (bContinue != FALSE)
    {
      if (!NT_SUCCESS(NktNtQueryVirtualMemory(hProcess, lpCurrAddress, MyMemoryBasicInformation, &sMbi,
                                              sizeof(sMbi), &k)))
        break;
      if (sMbi.Type == MEM_MAPPED || sMbi.Type == MEM_IMAGE)
      {
        dwType = sMbi.Type;
        lpBaseAddr = (LPBYTE)(sMbi.AllocationBase);
        do
        {
          lpCurrAddress += sMbi.RegionSize;
          if (!NT_SUCCESS(NktNtQueryVirtualMemory(hProcess, lpCurrAddress, MyMemoryBasicInformation, &sMbi,
                                                  sizeof(sMbi), &k)))
          {
            bContinue = FALSE;
            break;
          }
        }
        while (lpBaseAddr == (LPBYTE)(sMbi.AllocationBase));
        //check images
        if (dwType == MEM_IMAGE)
        {
          hRes = CheckImageType(&sNtHdr, hProcess, lpBaseAddr);
          if (FAILED(hRes) ||
              (nPlatformBits == 32 && hRes == (HRESULT)64) ||
              (nPlatformBits == 64 && hRes == (HRESULT)32))
            continue;
        }
        if (!NT_SUCCESS(NktNtQueryVirtualMemory(hProcess, (PVOID)lpBaseAddr, MyMemorySectionName,
                                                &usImportingDll, sizeof(usImportingDll), &k)))
          continue;
        //find last slash
        k = (SIZE_T)(usImportingDll.us.Length) / sizeof(WCHAR);
        while (k > 0 && usImportingDll.us.Buffer[k-1] != L'\\')
          k--;
        usImportingDll.us.Buffer += k;
        usImportingDll.us.Length -= (USHORT)k * (USHORT)sizeof(WCHAR);
        if (nDllNameLen*sizeof(WCHAR) == (SIZE_T)(usImportingDll.us.Length) &&
            StrNICmpW(szDllNameW, usImportingDll.us.Buffer, nDllNameLen) != FALSE)
          return (HINSTANCE)lpBaseAddr;
      }
      else
      {
        //skip block
        lpCurrAddress += sMbi.RegionSize;
      }
    }
  }
  return NULL;
}

LPVOID GetRemoteProcedureAddress(__in HANDLE hProcess, __in LPVOID lpDllBase, __in_z LPCSTR szFuncNameA)
{
  LPBYTE lpBaseAddr, lpExpDir, lpFuncAddr;
  LPVOID lpFwdDllBase;
  SIZE_T i, nDotPos, nLen, nExpDirSize, nPlatformBits;
  IMAGE_EXPORT_DIRECTORY sExpDir;
  CHAR szBufA[1024];
  WCHAR szBufW[1024+8];
  DWORD dw, dwTemp32, *lpdwAddressOfFunctions, *lpdwAddressOfNames;
  WORD wTemp16, *lpwAddressOfNameOrdinals;
  NKT_HK_IMAGE_NT_HEADER sNtLdr;
  HRESULT hRes;

  if (lpDllBase == NULL || szFuncNameA == NULL || szFuncNameA[0] == 0)
    return NULL;
  switch (GetProcessPlatform(hProcess))
  {
    case NKTHOOKLIB_ProcessPlatformX86:
      nPlatformBits = 32;
      break;
#if defined(_M_X64)
    case NKTHOOKLIB_ProcessPlatformX64:
      nPlatformBits = 64;
      break;
#endif //_M_X64
    default:
      return NULL;
  }
  //check image type
  lpBaseAddr = (LPBYTE)lpDllBase;
  hRes = CheckImageType(&sNtLdr, hProcess, lpBaseAddr);
  if (FAILED(hRes))
    return NULL;
  if (nPlatformBits != (SIZE_T)hRes)
    return NULL;
  //export directory address
  switch (nPlatformBits)
  {
    case 32:
      if (sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
          sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
        return NULL; //empty or no export table in module
      lpExpDir = lpBaseAddr + (SIZE_T)(sNtLdr.u32.OptionalHeader.
                                       DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      nExpDirSize = (SIZE_T)(sNtLdr.u32.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
      break;
#if defined(_M_X64)
    case 64:
      if (sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
          sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
        return NULL; //empty or no export table in module
      lpExpDir = lpBaseAddr + (SIZE_T)(sNtLdr.u64.OptionalHeader.
                                       DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
      nExpDirSize = (SIZE_T)(sNtLdr.u64.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
      break;
#endif //_M_X64
    default:
      return NULL;
  }
  if (ReadMem(hProcess, &sExpDir, lpExpDir, sizeof(sExpDir)) != sizeof(sExpDir))
    return NULL;
  //get addresses
  lpdwAddressOfFunctions = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfFunctions));
  lpdwAddressOfNames = (LPDWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNames));
  lpwAddressOfNameOrdinals = (LPWORD)(lpBaseAddr + (SIZE_T)(sExpDir.AddressOfNameOrdinals));
  //is by ordinal?
  if (szFuncNameA[0] == '#')
  {
    dwTemp32 = ToNumberA(szFuncNameA+1);
    if (dwTemp32 < sExpDir.Base || dwTemp32 > sExpDir.NumberOfFunctions)
      return NULL;
  }
  else
  {
    //find by name
    for (dwTemp32=0; dwTemp32<sExpDir.NumberOfNames; dwTemp32++)
    {
      //get the ordinal
      if (ReadMem(hProcess, &wTemp16, lpwAddressOfNameOrdinals+dwTemp32, sizeof(WORD)) != sizeof(WORD) ||
          ReadMem(hProcess, &dw, lpdwAddressOfNames+dwTemp32, sizeof(DWORD)) != sizeof(DWORD) ||
          dw == 0)
        continue;
      //get the name
      nLen = ReadMem(hProcess, szBufA, lpBaseAddr+(SIZE_T)dw, 1023);
      if (nLen == 0)
        continue;
      szBufA[nLen] = 0;
      if (StrICmpA(szBufA, szFuncNameA) != FALSE)
        break;
    }
    if (dwTemp32 >= sExpDir.NumberOfNames)
      return NULL;
    dwTemp32 = (DWORD)wTemp16;
  }
  //get address of function
  if (ReadMem(hProcess, &dw, lpdwAddressOfFunctions+dwTemp32, sizeof(DWORD)) != sizeof(DWORD) ||
      dw == 0)
    return NULL;
  lpFuncAddr = lpBaseAddr + (SIZE_T)dw;
  if (lpFuncAddr >= lpExpDir && lpFuncAddr < lpExpDir+nExpDirSize)
  {
    //read forwarded function
    nLen = ReadMem(hProcess, szBufA, lpFuncAddr, 1023);
    if (nLen == 0)
      return NULL;
    szBufA[nLen] = 0;
    for (nDotPos=0; nDotPos<nLen && szBufA[nDotPos]!='.'; nDotPos++);
    for (i=nDotPos; i>0; i--)
      szBufW[i-1] = (WCHAR)(UCHAR)szBufA[i-1];
    szBufW[nDotPos] = L'.';
    szBufW[nDotPos+1] = L'd';
    szBufW[nDotPos+2] = szBufW[nDotPos+3] = L'l';
    szBufW[nDotPos+4] = 0;
    lpFwdDllBase = GetRemoteModuleBaseAddress(hProcess, szBufW, FALSE);
    if (lpFwdDllBase == NULL) //check if we are dealing with an apiset
      lpFwdDllBase = FindDllInApiSetCheck(hProcess, nPlatformBits, lpDllBase, szBufW);
    if (lpFwdDllBase == NULL || lpFwdDllBase == lpDllBase)
      return NULL;
    lpFuncAddr = (LPBYTE)GetRemoteProcedureAddress(hProcess, lpFwdDllBase, &szBufA[nDotPos+1]);
  }
  //done
  return lpFuncAddr;
}

} //Internals
} //NktHookLib

//-----------------------------------------------------------

static HRESULT GetPeb(__out LPBYTE *lplpPeb, __in HANDLE hProcess, __in SIZE_T nPlatformBits)
{
#if defined(_M_IX86)
  NktHookLib::Internals::NKT_HK_PROCESS_BASIC_INFORMATION32 sPbi32;
#elif defined(_M_X64)
  NktHookLib::Internals::NKT_HK_PROCESS_BASIC_INFORMATION64 sPbi64;
  ULONGLONG nPeb32;
#endif
  HRESULT hRes;
  ULONG k;

  if (hProcess == NKTHOOKLIB_CurrentProcess)
  {
#if defined(_M_IX86)
    *lplpPeb = (LPBYTE)__readfsdword(0x30); //get PEB from the TIB
#elif defined(_M_X64)
    LPBYTE lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
    *lplpPeb = *((LPBYTE*)(lpPtr+0x60));
#else
  #error Unsupported platform
#endif
  }
  else
  {
    *lplpPeb = NULL;
    switch (nPlatformBits)
    {
      case 32:
#if defined(_M_IX86)
        hRes = NktNtQueryInformationProcess(hProcess, (PROCESSINFOCLASS)MyProcessBasicInformation, &sPbi32,
                                            sizeof(sPbi32), &k);
        if (FAILED(hRes))
          return hRes;
        *lplpPeb = (LPBYTE)(sPbi32.PebBaseAddress);
#elif defined(_M_X64)
        hRes = NktNtQueryInformationProcess(hProcess, (PROCESSINFOCLASS)MyProcessWow64Information, &nPeb32,
                                            sizeof(nPeb32), &k);
        if (FAILED(hRes))
          return hRes;
        *lplpPeb = (LPBYTE)nPeb32;
#endif
      break;

#if defined(_M_X64)
      case 64:
        hRes = NktNtQueryInformationProcess(hProcess, (PROCESSINFOCLASS)MyProcessBasicInformation, &sPbi64,
                                            sizeof(sPbi64), &k);
        if (FAILED(hRes))
          return hRes;
        *lplpPeb = (LPBYTE)(sPbi64.PebBaseAddress);
        break;
#endif

      default:
        return E_INVALIDARG;
    }
  }
  return S_OK;
}

static HRESULT CheckImageType(__out NKT_HK_IMAGE_NT_HEADER *lpNtHdr, __in HANDLE hProcess, __in LPBYTE lpBaseAddr,
                              __out_opt LPBYTE *lplpNtHdrAddr)
{
  LPBYTE lpNtHdrSrc;
  WORD wTemp16;
  DWORD dwTemp32;

  if (lpNtHdr == NULL || hProcess == NULL || lpBaseAddr == NULL)
    return E_INVALIDARG;
  //----
  if (ReadMem(hProcess, &wTemp16, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic),
              sizeof(wTemp16)) != sizeof(wTemp16))
    return E_FAIL;
  if (wTemp16 != IMAGE_DOS_SIGNATURE)
    return E_FAIL;
  //get header offset
  if (ReadMem(hProcess, &dwTemp32, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew),
              sizeof(dwTemp32)) != sizeof(dwTemp32))
    return E_FAIL;
  lpNtHdrSrc = lpBaseAddr + (SIZE_T)dwTemp32;
  if (lplpNtHdrAddr != NULL)
    *lplpNtHdrAddr = lpNtHdrSrc;
  //check image type
  if (ReadMem(hProcess, &wTemp16, lpNtHdrSrc + FIELD_OFFSET(IMAGE_NT_HEADERS32, FileHeader.Machine),
              sizeof(wTemp16)) != sizeof(wTemp16))
    return E_FAIL;
  switch (wTemp16)
  {
    case IMAGE_FILE_MACHINE_I386:
      if (ReadMem(hProcess, lpNtHdr, lpNtHdrSrc, sizeof(lpNtHdr->u32)) != sizeof(lpNtHdr->u32))
        return E_FAIL;
      //check signature
      if (lpNtHdr->u32.Signature != IMAGE_NT_SIGNATURE)
        return E_FAIL;
      return (HRESULT)32;

    case IMAGE_FILE_MACHINE_AMD64:
      if (ReadMem(hProcess, lpNtHdr, lpNtHdrSrc, sizeof(lpNtHdr->u64)) != sizeof(lpNtHdr->u64))
        return E_FAIL;
      //check signature
      if (lpNtHdr->u64.Signature != IMAGE_NT_SIGNATURE)
        return E_FAIL;
      return (HRESULT)64;
  }
  return E_FAIL;
}

static HRESULT GetFirstLdrEntry32(__out NKT_HK_LDRENTRY32 *lpEntry32, __in LPBYTE lpPeb, __in HANDLE hProcess)
{
  LPBYTE lpPtr;
  DWORD dwTemp32;
  BYTE nTemp8;

  if (lpEntry32 == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  lpEntry32->hProcess = hProcess;
  //nPeb32+12 => pointer to PEB_LDR_DATA32
  if (ReadMem(hProcess, &dwTemp32, lpPeb+0x0C, sizeof(dwTemp32)) != sizeof(dwTemp32) ||
      dwTemp32 == 0)
    return E_FAIL;
  //check PEB_LDR_DATA32.Initialize flag
  if (ReadMem(hProcess, &nTemp8, (LPBYTE)(SIZE_T)dwTemp32+0x04, sizeof(nTemp8)) != sizeof(nTemp8))
    return E_FAIL;
  if (nTemp8 == 0)
    return S_FALSE;
  //get PEB_LDR_DATA32.InLoadOrderModuleList.Flink
  lpEntry32->nFirstLink = dwTemp32+0x0C;
  if (ReadMem(hProcess, &(lpEntry32->nCurrLink), (LPBYTE)(SIZE_T)(lpEntry32->nFirstLink),
              sizeof(lpEntry32->nCurrLink)) != sizeof(lpEntry32->nCurrLink))
    return E_FAIL;
  if (lpEntry32->nFirstLink == lpEntry32->nCurrLink)
    return S_FALSE;
  //read first entry
  lpPtr = (LPBYTE)(SIZE_T)(lpEntry32->nCurrLink) - FIELD_OFFSET(NKT_HK_LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
  if (ReadMem(lpEntry32->hProcess, &(lpEntry32->sEntry), lpPtr,
              sizeof(lpEntry32->sEntry)) != sizeof(lpEntry32->sEntry))
    return E_FAIL;
  lpEntry32->nCurrLink = lpEntry32->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

static HRESULT GetNextLdrEntry32(__inout NKT_HK_LDRENTRY32 *lpEntry32)
{
  LPBYTE lpPtr;

  if (lpEntry32 == NULL)
    return E_INVALIDARG;
  if (lpEntry32->nFirstLink == lpEntry32->nCurrLink)
    return S_FALSE;
  lpPtr = (LPBYTE)(SIZE_T)(lpEntry32->nCurrLink) - FIELD_OFFSET(NKT_HK_LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
  if (ReadMem(lpEntry32->hProcess, &(lpEntry32->sEntry), lpPtr,
              sizeof(lpEntry32->sEntry)) != sizeof(lpEntry32->sEntry))
    return E_FAIL;
  lpEntry32->nCurrLink = lpEntry32->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

#if defined(_M_X64)
HRESULT GetBaseAddress64(__out LPBYTE *lplpBaseAddress, __in LPBYTE lpPeb, __in HANDLE hProcess)
{
  ULONGLONG qwTemp64;

  if (lplpBaseAddress == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  //nPeb64+10 => pointer to IMAGE BASE ADDRESS
  if (ReadMem(hProcess, &qwTemp64, lpPeb+0x10, sizeof(qwTemp64)) != sizeof(qwTemp64) ||
      qwTemp64 == 0)
    return E_FAIL;
  *lplpBaseAddress = (LPBYTE)qwTemp64;
  return S_OK;
}

HRESULT GetFirstLdrEntry64(__out NKT_HK_LDRENTRY64 *lpEntry64, __in LPBYTE lpPeb, __in HANDLE hProcess)
{
  LPBYTE lpPtr;
  ULONGLONG qwTemp64;
  BYTE nTemp8;

  if (lpEntry64 == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  lpEntry64->hProcess = hProcess;
  //nPeb32+24 => pointer to PEB_LDR_DATA64
  if (ReadMem(hProcess, &qwTemp64, lpPeb+0x18, sizeof(qwTemp64)) != sizeof(qwTemp64) ||
      qwTemp64 == 0)
    return E_FAIL;
  //check PEB_LDR_DATA64.Initialize flag
  if (ReadMem(hProcess, &nTemp8, (LPBYTE)qwTemp64+0x04, sizeof(nTemp8)) != sizeof(nTemp8))
    return E_FAIL;
  if (nTemp8 == 0)
    return S_FALSE;
  //get PEB_LDR_DATA64.InLoadOrderModuleList.Flink
  lpEntry64->nFirstLink = qwTemp64+0x10;
  if (ReadMem(hProcess, &(lpEntry64->nCurrLink), (LPBYTE)(lpEntry64->nFirstLink),
              sizeof(lpEntry64->nCurrLink)) != sizeof(lpEntry64->nCurrLink))
    return E_FAIL;
  if (lpEntry64->nFirstLink == lpEntry64->nCurrLink)
    return S_FALSE;
  //read first entry
  lpPtr = (LPBYTE)(lpEntry64->nCurrLink) - FIELD_OFFSET(NKT_HK_LDR_DATA_TABLE_ENTRY64, InLoadOrderLinks);
  if (ReadMem(lpEntry64->hProcess, &(lpEntry64->sEntry), lpPtr,
              sizeof(lpEntry64->sEntry)) != sizeof(lpEntry64->sEntry))
    return E_FAIL;
  lpEntry64->nCurrLink = lpEntry64->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

HRESULT GetNextLdrEntry64(__inout NKT_HK_LDRENTRY64 *lpEntry64)
{
  LPBYTE lpPtr;

  if (lpEntry64 == NULL)
    return E_INVALIDARG;
  if (lpEntry64->nFirstLink == lpEntry64->nCurrLink)
    return S_FALSE;
  lpPtr = (LPBYTE)(lpEntry64->nCurrLink) - FIELD_OFFSET(NKT_HK_LDR_DATA_TABLE_ENTRY64, InLoadOrderLinks);
  if (ReadMem(lpEntry64->hProcess, &(lpEntry64->sEntry), lpPtr,
              sizeof(lpEntry64->sEntry)) != sizeof(lpEntry64->sEntry))
    return E_FAIL;
  lpEntry64->nCurrLink = lpEntry64->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}
#endif //_M_X64

//fix by Boris Gjenero (https://github.com/dreamlayers)
static BOOL RemoteCompareUnicodeString32(__in HANDLE hProcess, __in NKT_HK_UNICODE_STRING32 *lpRemoteStr,
                                         __in LPCWSTR szLocalStrW, __in SIZE_T nStrLen)
{
  if ((SIZE_T)(lpRemoteStr->Length) != nStrLen*sizeof(WCHAR))
    return FALSE;
  return RemoteStrNICmpW(hProcess, (LPCWSTR)(SIZE_T)(lpRemoteStr->Buffer), szLocalStrW, nStrLen);
}

#if defined(_M_X64)
//fix by Boris Gjenero (https://github.com/dreamlayers)
static BOOL RemoteCompareUnicodeString64(__in HANDLE hProcess, __in NKT_HK_UNICODE_STRING64 *lpRemoteStr,
                                         __in LPCWSTR szLocalStrW, __in SIZE_T nStrLen)
{
  if ((SIZE_T)(lpRemoteStr->Length) != nStrLen*sizeof(WCHAR))
    return FALSE;
  return RemoteStrNICmpW(hProcess, (LPCWSTR)(lpRemoteStr->Buffer), szLocalStrW, nStrLen);
}
#endif //_M_X64

static BOOL RemoteStrNICmpW(__in HANDLE hProcess, __in LPCWSTR szRemoteStrW, __in LPCWSTR szLocalStrW,
                            __in SIZE_T nStrLen)
{
  WCHAR aBufW[128];
  SIZE_T i;

  if (szRemoteStrW == NULL)
    return FALSE;
  while (nStrLen > 0)
  {
    i = (nStrLen > 128) ? 128 : nStrLen;
    if (ReadMem(hProcess, aBufW, (LPVOID)szRemoteStrW, i*sizeof(WCHAR)) != i*sizeof(WCHAR) ||
        StrNICmpW(szLocalStrW, aBufW, i) == FALSE)
      return FALSE;
    szRemoteStrW += i;
    szLocalStrW += i;
    nStrLen -= i;
  }
  return TRUE;
}

static BOOL StrICmpA(__in LPCSTR sA_1, __in LPCSTR sA_2)
{
  CHAR chA_1, chA_2;

  while (sA_1[0] != 0 && sA_2[0] != 0)
  {
    chA_1 = sA_1[0];
    chA_2 = sA_2[0];
    if (chA_1 >= 'a' && chA_1 <= 'z')
      chA_1 -= ('a' - 'A');
    if (chA_2 >= 'a' && chA_2 <= 'z')
      chA_2 -= ('a' - 'A');
    if (chA_1 != chA_2)
      break;
    sA_1++;
    sA_2++;
  }
  return (sA_1[0] == 0 && sA_2[0] == 0) ? TRUE : FALSE;
}

static BOOL StrNICmpW(__in LPCWSTR sW_1, __in LPCWSTR sW_2, __in SIZE_T nLen)
{
  WCHAR chW_1, chW_2;

  while (nLen > 0)
  {
    chW_1 = sW_1[0];
    chW_2 = sW_2[0];
    if (chW_1 >= L'a' && chW_1 <= L'z')
      chW_1 -= (L'a' - L'A');
    if (chW_2 >= L'a' && chW_2 <= L'z')
      chW_2 -= (L'a' - L'A');
    if (chW_1 != chW_2)
      return FALSE;
    sW_1++;
    sW_2++;
    nLen--;
  }
  return TRUE;
}

static DWORD ToNumberA(__in LPCSTR sA)
{
  DWORD dwRes;

  dwRes = 0;
  while (sA[0] >= '0' || sA[0] <= '9')
  {
    if (dwRes >= 0xFFFFFFFF/0xA)
      return 0;
    dwRes = dwRes*10 + (DWORD)(sA[0] - '0');
    sA++;
  }
  return dwRes;
}

static LPVOID FindDllInApiSetCheck(__in HANDLE hProcess, __in SIZE_T nPlatformBits, __in LPVOID lpImportingDllBase,
                                   __in LPCWSTR szDllToFindW)
{
  LPBYTE lpPeb, lpApiMapSet, lpPtr;
  DWORD dwDllToFindLen, dwDllToFindLenInBytes, dwTemp32;
  struct {
    NKT_HK_UNICODE_STRING us;
    WCHAR chDataW[2048];
  } usImportingDll;
#if defined(_M_X64)
  ULONGLONG qwTemp64;
#endif //_M_X64
  SIZE_T nRetLen;
  DWORD dwOsVerMajor, dwOsVerMinor;

  //check OS version
  if (NktHookLibHelpers::GetOsVersion(&dwOsVerMajor, &dwOsVerMinor) == FALSE)
    return NULL;
  if (dwOsVerMajor < 6 || (dwOsVerMajor == 6 && dwOsVerMinor < 1))
    return NULL; //only on Win7 or later
  //query importing dll name
  if (!NT_SUCCESS(NktNtQueryVirtualMemory(hProcess, (PVOID)lpImportingDllBase, MyMemorySectionName, &usImportingDll,
                                          sizeof(usImportingDll), &nRetLen)))
    return NULL;
  //find last slash
  dwTemp32 = (DWORD)(usImportingDll.us.Length) / (DWORD)sizeof(WCHAR);
  while (dwTemp32 > 0 && usImportingDll.us.Buffer[dwTemp32-1] != L'\\')
    dwTemp32--;
  usImportingDll.us.Buffer += dwTemp32;
  usImportingDll.us.Length -= (USHORT)dwTemp32 * (USHORT)sizeof(WCHAR);
  if (usImportingDll.us.Length == 0)
    return NULL;
  //check length and "api-" prefix of dll to find
  for (dwDllToFindLen=0; szDllToFindW[dwDllToFindLen]!=0; dwDllToFindLen++);
  if (dwDllToFindLen <= 4)
    return NULL;
  if (!((((szDllToFindW[0] == L'a' || szDllToFindW[0] == L'A') &&
          (szDllToFindW[1] == L'p' || szDllToFindW[1] == L'P') &&
          (szDllToFindW[2] == L'i' || szDllToFindW[2] == L'I')) ||
         ((szDllToFindW[0] == L'e' || szDllToFindW[0] == L'E') &&
          (szDllToFindW[1] == L'x' || szDllToFindW[1] == L'X') &&
          (szDllToFindW[2] == L't' || szDllToFindW[2] == L'T'))) &&
        szDllToFindW[3] == L'-'))
    return NULL;
  //remove dll extension if any
  for (dwTemp32=dwDllToFindLen; dwTemp32>4; dwTemp32--)
  {
    if (szDllToFindW[dwTemp32-1] == L'.')
      break;
  }
  if (dwTemp32 > 4)
    dwDllToFindLen = dwTemp32-1;
  szDllToFindW += 4;
  dwDllToFindLen -= 4; //substract "api-" length
  if (dwDllToFindLen == 0)
    return NULL;
  dwDllToFindLenInBytes = dwDllToFindLen * (DWORD)sizeof(WCHAR);
  //get peb
  if (FAILED(GetPeb(&lpPeb, hProcess, nPlatformBits)))
    return NULL;
  //get pointer to apiset map
  switch (nPlatformBits)
  {
    case 32:
      if (ReadMem(hProcess, &dwTemp32, lpPeb+0x38, sizeof(dwTemp32)) != sizeof(dwTemp32))
        return NULL;
      lpApiMapSet = (LPBYTE)(SIZE_T)dwTemp32;
      break;
#if defined(_M_X64)
    case 64:
      if (ReadMem(hProcess, &qwTemp64, lpPeb+0x68, sizeof(qwTemp64)) != sizeof(qwTemp64))
        return NULL;
      lpApiMapSet = (LPBYTE)qwTemp64;
      break;
#endif //_M_X64
    default:
      return NULL;
  }
  //get apimapset version
  if (lpApiMapSet == NULL ||
      ReadMem(hProcess, &dwTemp32, lpApiMapSet, sizeof(DWORD)) != sizeof(DWORD))
    return NULL;
  switch (dwTemp32)
  {
    case 2: //found in Win7 & Win8
      {
      NKT_HK_APIMAPSET_HEADER_V2 sHdr;
      NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V2 sNamespaceEntry, *lpCurrNamespaceEntry;
      NKT_HK_APIMAPSET_HOST_HEADER_V2 sHostHdr;
      NKT_HK_APIMAPSET_HOST_ENTRY_V2 sHostEntry, sDefHostEntry, *lpCurrHostEntry;

      //read header entry
      if (ReadMem(hProcess, &sHdr, lpApiMapSet, sizeof(sHdr)) != sizeof(sHdr))
        return NULL;
      lpCurrNamespaceEntry = (NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V2*)(lpApiMapSet + sizeof(NKT_HK_APIMAPSET_HEADER_V2));
      for (; sHdr.dwEntriesCount>0; sHdr.dwEntriesCount--,lpCurrNamespaceEntry++)
      {
        if (ReadMem(hProcess, &sNamespaceEntry, lpCurrNamespaceEntry, sizeof(sNamespaceEntry)) == FALSE)
          return NULL;
        //check if it is the dll we are looking for
        if (sNamespaceEntry.dwNameLength == dwDllToFindLenInBytes &&
            RemoteStrNICmpW(hProcess, (LPCWSTR)(lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwNameOffset)), szDllToFindW,
                            (SIZE_T)dwDllToFindLen) != FALSE)
        {
          sDefHostEntry.dwLengthRealName = 0xFFFFFFFFUL;
          //scan host entries
          lpPtr = lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwHostModulesOffset);
          if (ReadMem(hProcess, &sHostHdr, lpPtr, sizeof(sHostHdr)) != sizeof(sHostHdr))
            return NULL;
          lpCurrHostEntry = (NKT_HK_APIMAPSET_HOST_ENTRY_V2*)(lpPtr + sizeof(sHostHdr));
          for (dwTemp32=0; dwTemp32<sHostHdr.dwCount; dwTemp32++)
          {
            if (ReadMem(hProcess, &sHostEntry, &lpCurrHostEntry[dwTemp32], sizeof(sHostEntry)) != sizeof(sHostEntry))
              return NULL;
            if (sHostEntry.dwLength == 0 || sHostEntry.dwNameOffset == 0)
              continue;
            if (sHostEntry.dwLengthRealName == (DWORD)(usImportingDll.us.Length) && sHostEntry.dwNameOffsetRealName > 0)
            {
              //check for importing dll name match
              lpPtr = lpApiMapSet + (SIZE_T)(sHostEntry.dwNameOffsetRealName);
              if (RemoteStrNICmpW(hProcess, (LPCWSTR)lpPtr, usImportingDll.us.Buffer,
                                  (SIZE_T)(usImportingDll.us.Length) / sizeof(WCHAR)) != FALSE)
              {
gotIt_V2:       //retrieve dll name
                lpPtr = lpApiMapSet + (SIZE_T)(sHostEntry.dwNameOffset);
                if (sHostEntry.dwLength >= 2040)
                  return NULL; //name too long
                if (ReadMem(hProcess, usImportingDll.chDataW, lpPtr,
                                               (SIZE_T)(sHostEntry.dwLength)) != (SIZE_T)(sHostEntry.dwLength))
                  return NULL;
                usImportingDll.chDataW[sHostEntry.dwLength / (DWORD)sizeof(WCHAR)] = 0;
                return NktHookLib::Internals::GetRemoteModuleBaseAddress(hProcess, usImportingDll.chDataW, FALSE);
              }
            }
            else if (sHostEntry.dwLengthRealName == 0 && sDefHostEntry.dwLengthRealName == 0xFFFFFFFFUL)
            {
              MemCopy(&sDefHostEntry, &sHostEntry, sizeof(sHostEntry));
            }
          }
          //have a default?
          if (sDefHostEntry.dwLengthRealName != 0xFFFFFFFFUL)
          {
            MemCopy(&sHostEntry, &sDefHostEntry, sizeof(sHostEntry));
            goto gotIt_V2;
          }
        }
      }
      }
      break;

    case 4: //found in Win8.1 & Win10
      {
      NKT_HK_APIMAPSET_HEADER_V4 sHdr;
      NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V4 sNamespaceEntry, *lpCurrNamespaceEntry;
      NKT_HK_APIMAPSET_HOST_HEADER_V4 sHostHdr;
      NKT_HK_APIMAPSET_HOST_ENTRY_V4 sHostEntry, sDefHostEntry, *lpCurrHostEntry;

      if (ReadMem(hProcess, &sHdr, lpApiMapSet, sizeof(sHdr)) != sizeof(sHdr))
        return NULL;
      lpCurrNamespaceEntry = (NKT_HK_APIMAPSET_NAMESPACE_ENTRY_V4*)(lpApiMapSet + sizeof(NKT_HK_APIMAPSET_HEADER_V4));
      for (; sHdr.dwEntriesCount>0; sHdr.dwEntriesCount--,lpCurrNamespaceEntry++)
      {
        if (ReadMem(hProcess, &sNamespaceEntry, lpCurrNamespaceEntry,
                    sizeof(sNamespaceEntry)) != sizeof(sNamespaceEntry))
          return NULL;
        //check if it is the dll we are looking for
        if ((sNamespaceEntry.dwNameLength1 == dwDllToFindLenInBytes &&
             RemoteStrNICmpW(hProcess, (LPCWSTR)(lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwNameOffset1)),
                             szDllToFindW, (SIZE_T)dwDllToFindLen) != FALSE) ||
            (sNamespaceEntry.dwNameLength2 == dwDllToFindLenInBytes &&
             RemoteStrNICmpW(hProcess, (LPCWSTR)(lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwNameOffset2)),
                             szDllToFindW, (SIZE_T)dwDllToFindLen) != FALSE))
        {
          sDefHostEntry.dwLengthRealName = 0xFFFFFFFFUL;
          //scan host entries
          lpPtr = lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwHostModulesOffset);
          if (ReadMem(hProcess, &sHostHdr, lpPtr, sizeof(sHostHdr)) != sizeof(sHostHdr))
            return NULL;
          lpCurrHostEntry = (NKT_HK_APIMAPSET_HOST_ENTRY_V4*)(lpPtr + sizeof(sHostHdr));
          for (dwTemp32=0; dwTemp32<sHostHdr.dwCount; dwTemp32++)
          {
            if (ReadMem(hProcess, &sHostEntry, &lpCurrHostEntry[dwTemp32], sizeof(sHostEntry)) != sizeof(sHostEntry))
              return NULL;
            if (sHostEntry.dwLength == 0 || sHostEntry.dwNameOffset == 0)
              continue;
            if (sHostEntry.dwLengthRealName == (DWORD)(usImportingDll.us.Length) && sHostEntry.dwNameOffsetRealName > 0)
            {
              //check for importing dll name match
              lpPtr = lpApiMapSet + (SIZE_T)(sHostEntry.dwNameOffsetRealName);
              if (RemoteStrNICmpW(hProcess, (LPCWSTR)lpPtr, usImportingDll.us.Buffer,
                                  (SIZE_T)(usImportingDll.us.Length) / sizeof(WCHAR)) != FALSE)
              {
gotIt_V4:       //retrieve dll name
                lpPtr = lpApiMapSet + (SIZE_T)(sHostEntry.dwNameOffset);
                if (sHostEntry.dwLength >= 2040)
                  return NULL; //name too long
                if (ReadMem(hProcess, usImportingDll.chDataW, lpPtr,
                            (SIZE_T)(sHostEntry.dwLength)) != (SIZE_T)(sHostEntry.dwLength))
                  return NULL;
                usImportingDll.chDataW[sHostEntry.dwLength / (DWORD)sizeof(WCHAR)] = 0;
                return NktHookLib::Internals::GetRemoteModuleBaseAddress(hProcess, usImportingDll.chDataW, FALSE);
              }
            }
            else if (sHostEntry.dwLengthRealName == 0 && sDefHostEntry.dwLengthRealName == 0xFFFFFFFFUL)
            {
              MemCopy(&sDefHostEntry, &sHostEntry, sizeof(sHostEntry));
            }
          }
          //have a default?
          if (sDefHostEntry.dwLengthRealName != 0xFFFFFFFFUL)
          {
            MemCopy(&sHostEntry, &sDefHostEntry, sizeof(sHostEntry));
            goto gotIt_V4;
          }
        }
      }
      }
      break;
  }
  return NULL;
}
