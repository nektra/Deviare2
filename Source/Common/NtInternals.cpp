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

#include "NtInternals.h"
#include "AutoPtr.h"
#include "DynamicAPIs.h"

//-----------------------------------------------------------

#define X_LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY        0x00000002
#define X_LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_NOT_ACQUIRED 2

//-----------------------------------------------------------

static __inline BOOL ReadProcMem(__in HANDLE hProc, __out LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nDataSize);
static HRESULT CompareRemoteTextW(__in HANDLE hProcess, __in LPWSTR szRemoteTextW, __in LPCWSTR szTextW,
                                  __in DWORD dwChars);

//-----------------------------------------------------------

HRESULT nktDvNtGetPeb(__out LPBYTE *lplpPeb, __in HANDLE hProcess, __in SIZE_T nPlatformBits)
{
#if defined _M_IX86
  NKT_DV_PROCESS_BASIC_INFORMATION32 sPbi32;
#elif defined _M_X64
  NKT_DV_PROCESS_BASIC_INFORMATION64 sPbi64;
  ULONGLONG nPeb32;
#endif
  HRESULT hRes;
  ULONG k;

  if (lplpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  *lplpPeb = NULL;
  switch (nPlatformBits)
  {
    case 32:
#if defined _M_IX86
      hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessBasicInformation, &sPbi32,
                                                    sizeof(sPbi32), &k);
      if (FAILED(hRes))
        return hRes;
      *lplpPeb = (LPBYTE)(sPbi32.PebBaseAddress);
#elif defined _M_X64
      hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessWow64Information, &nPeb32,
                                                    sizeof(nPeb32), &k);
      if (FAILED(hRes))
        return hRes;
      *lplpPeb = (LPBYTE)nPeb32;
#endif
      break;

#if defined _M_X64
    case 64:
      hRes = nktDvDynApis_NtQueryInformationProcess(hProcess, NKT_DV_ProcessBasicInformation, &sPbi64,
                                                    sizeof(sPbi64), &k);
      if (FAILED(hRes))
        return hRes;
      *lplpPeb = (LPBYTE)(sPbi64.PebBaseAddress);
      break;
#endif

    default:
      return E_INVALIDARG;
  }
  return S_OK;
}

HRESULT nktDvNtCheckImageType(__out NKT_DV_IMAGE_NT_HEADER *lpNtHdr,
                              __in HANDLE hProcess, __in LPBYTE lpBaseAddr,
                              __out_opt LPBYTE *lplpNtHdrAddr)
{
  LPBYTE lpNtHdrSrc;
  WORD wTemp16;
  DWORD dwTemp32;

  if (lpNtHdr == NULL || hProcess == NULL || lpBaseAddr == NULL)
    return E_INVALIDARG;
  //----
  if (ReadProcMem(hProcess, &wTemp16, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_magic), sizeof(wTemp16)) == FALSE)
    return E_FAIL;
  if (wTemp16 != IMAGE_DOS_SIGNATURE)
    return E_FAIL;
  //get header offset
  if (ReadProcMem(hProcess, &dwTemp32, lpBaseAddr + FIELD_OFFSET(IMAGE_DOS_HEADER, e_lfanew),
                  sizeof(dwTemp32)) == FALSE)
    return E_FAIL;
  lpNtHdrSrc = lpBaseAddr + (SIZE_T)dwTemp32;
  if (lplpNtHdrAddr != NULL)
    *lplpNtHdrAddr = lpNtHdrSrc;
  //check image type
  if (ReadProcMem(hProcess, &wTemp16, lpNtHdrSrc + FIELD_OFFSET(IMAGE_NT_HEADERS32, FileHeader.Machine),
                  sizeof(wTemp16)) == FALSE)
    return E_FAIL;
  switch (wTemp16)
  {
    case IMAGE_FILE_MACHINE_I386:
      if (ReadProcMem(hProcess, lpNtHdr, lpNtHdrSrc, sizeof(lpNtHdr->u32)) == FALSE)
        return E_FAIL;
      //check signature
      if (lpNtHdr->u32.Signature != IMAGE_NT_SIGNATURE)
        return E_FAIL;
      return (HRESULT)32;

    case IMAGE_FILE_MACHINE_AMD64:
      if (ReadProcMem(hProcess, lpNtHdr, lpNtHdrSrc, sizeof(lpNtHdr->u64)) == FALSE)
        return E_FAIL;
      //check signature
      if (lpNtHdr->u64.Signature != IMAGE_NT_SIGNATURE)
        return E_FAIL;
      return (HRESULT)64;
  }
  return E_FAIL;
}

HRESULT nktDvNtGetBaseAddress32(__out LPBYTE *lplpBaseAddress, __in LPBYTE lpPeb, __in HANDLE hProcess)
{
  DWORD dwTemp32;

  if (lplpBaseAddress == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  //nPeb32+8 => pointer to IMAGE BASE ADDRESS
  if (ReadProcMem(hProcess, &dwTemp32, lpPeb+0x08, sizeof(dwTemp32)) == FALSE ||
      dwTemp32 == 0)
    return E_FAIL;
  *lplpBaseAddress = (LPBYTE)((ULONG_PTR)dwTemp32);
  return S_OK;
}

HRESULT nktDvNtGetFirstLdrEntry32(__out NKT_DV_LDRENTRY32 *lpEntry32, __in LPBYTE lpPeb,
                                  __in HANDLE hProcess)
{
  LPBYTE lpPtr;
  DWORD dwTemp32;
  BYTE nTemp8;

  if (lpEntry32 == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  lpEntry32->hProcess = hProcess;
  //nPeb32+12 => pointer to PEB_LDR_DATA32
  if (ReadProcMem(hProcess, &dwTemp32, lpPeb+0x0C, sizeof(dwTemp32)) == FALSE ||
      dwTemp32 == 0)
    return E_FAIL;
  //check PEB_LDR_DATA32.Initialize flag
  if (ReadProcMem(hProcess, &nTemp8, (LPBYTE)((ULONG_PTR)dwTemp32)+0x04, sizeof(nTemp8)) == FALSE)
    return E_FAIL;
  if (nTemp8 == 0)
    return NKT_DVERR_NotFound;
  //get PEB_LDR_DATA32.InLoadOrderModuleList.Flink
  lpEntry32->nFirstLink = dwTemp32+0x0C;
  if (ReadProcMem(hProcess, &(lpEntry32->nCurrLink), (LPBYTE)((ULONG_PTR)(lpEntry32->nFirstLink)),
                  sizeof(lpEntry32->nCurrLink)) == FALSE)
    return E_FAIL;
  if (lpEntry32->nFirstLink == lpEntry32->nCurrLink)
    return NKT_DVERR_NotFound;
  //read first entry
  lpPtr = (LPBYTE)((ULONG_PTR)(lpEntry32->nCurrLink)) - FIELD_OFFSET(NKT_DV_LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
  if (ReadProcMem(lpEntry32->hProcess, &(lpEntry32->sEntry), lpPtr, sizeof(lpEntry32->sEntry)) == FALSE)
    return E_FAIL;
  lpEntry32->nCurrLink = lpEntry32->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

HRESULT nktDvNtGetNextLdrEntry32(__inout NKT_DV_LDRENTRY32 *lpEntry32)
{
  LPBYTE lpPtr;

  if (lpEntry32 == NULL)
    return E_INVALIDARG;
  if (lpEntry32->nFirstLink == lpEntry32->nCurrLink)
    return NKT_DVERR_NotFound;
  lpPtr = (LPBYTE)((ULONG_PTR)(lpEntry32->nCurrLink)) - FIELD_OFFSET(NKT_DV_LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);
  if (ReadProcMem(lpEntry32->hProcess, &(lpEntry32->sEntry), lpPtr, sizeof(lpEntry32->sEntry)) == FALSE)
    return E_FAIL;
  lpEntry32->nCurrLink = lpEntry32->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

#if defined _M_X64
HRESULT nktDvNtGetBaseAddress64(__out LPBYTE *lplpBaseAddress, __in LPBYTE lpPeb, __in HANDLE hProcess)
{
  ULONGLONG qwTemp64;

  if (lplpBaseAddress == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  //nPeb64+10 => pointer to IMAGE BASE ADDRESS
  if (ReadProcMem(hProcess, &qwTemp64, lpPeb+0x10, sizeof(qwTemp64)) == FALSE ||
      qwTemp64 == 0)
    return E_FAIL;
  *lplpBaseAddress = (LPBYTE)qwTemp64;
  return S_OK;
}

HRESULT nktDvNtGetFirstLdrEntry64(__out NKT_DV_LDRENTRY64 *lpEntry64, __in LPBYTE lpPeb,
                                  __in HANDLE hProcess)
{
  LPBYTE lpPtr;
  ULONGLONG qwTemp64;
  BYTE nTemp8;

  if (lpEntry64 == NULL || lpPeb == NULL || hProcess == NULL)
    return E_INVALIDARG;
  lpEntry64->hProcess = hProcess;
  //nPeb32+24 => pointer to PEB_LDR_DATA64
  if (ReadProcMem(hProcess, &qwTemp64, lpPeb+0x18, sizeof(qwTemp64)) == FALSE ||
      qwTemp64 == 0)
    return E_FAIL;
  //check PEB_LDR_DATA64.Initialize flag
  if (ReadProcMem(hProcess, &nTemp8, (LPBYTE)qwTemp64+0x04, sizeof(nTemp8)) == FALSE)
    return E_FAIL;
  if (nTemp8 == 0)
    return NKT_DVERR_NotFound;
  //get PEB_LDR_DATA64.InLoadOrderModuleList.Flink
  lpEntry64->nFirstLink = qwTemp64+0x10;
  if (ReadProcMem(hProcess, &(lpEntry64->nCurrLink), (LPBYTE)(lpEntry64->nFirstLink),
                  sizeof(lpEntry64->nCurrLink)) == FALSE)
    return E_FAIL;
  if (lpEntry64->nFirstLink == lpEntry64->nCurrLink)
    return NKT_DVERR_NotFound;
  //read first entry
  lpPtr = (LPBYTE)(lpEntry64->nCurrLink) - FIELD_OFFSET(NKT_DV_LDR_DATA_TABLE_ENTRY64, InLoadOrderLinks);
  if (ReadProcMem(lpEntry64->hProcess, &(lpEntry64->sEntry), lpPtr, sizeof(lpEntry64->sEntry)) == FALSE)
    return E_FAIL;
  lpEntry64->nCurrLink = lpEntry64->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}

HRESULT nktDvNtGetNextLdrEntry64(__inout NKT_DV_LDRENTRY64 *lpEntry64)
{
  LPBYTE lpPtr;

  if (lpEntry64 == NULL)
    return E_INVALIDARG;
  if (lpEntry64->nFirstLink == lpEntry64->nCurrLink)
    return NKT_DVERR_NotFound;
  lpPtr = (LPBYTE)(lpEntry64->nCurrLink) - FIELD_OFFSET(NKT_DV_LDR_DATA_TABLE_ENTRY64, InLoadOrderLinks);
  if (ReadProcMem(lpEntry64->hProcess, &(lpEntry64->sEntry), lpPtr, sizeof(lpEntry64->sEntry)) == FALSE)
    return E_FAIL;
  lpEntry64->nCurrLink = lpEntry64->sEntry.InLoadOrderLinks.Flink;
  return S_OK;
}
#endif //_M_X64

// http://www.geoffchappell.com/studies/windows/win32/apisetschema/index.htm
HRESULT nktDvNtFindApiMapSet(__out CNktStringW &cStrReplDllW, __in HANDLE hProcess,
                             __in SIZE_T nPlatformBits, __in LPCWSTR szDllToFindW,
                             __in LPCWSTR szImportingDllW)
{
  DWORD dwOsVer;
  LPBYTE lpPeb, lpApiMapSet, lpPtr;
  DWORD dwTemp32, dwDllToFindLen, dwDllToFindLenInBytes, dwImportingDllLen, dwImportingDllLenInBytes;
#if defined _M_X64
  ULONGLONG qwTemp64;
#endif //_M_X64
  SIZE_T k;
  HRESULT hRes;

  cStrReplDllW.Empty();
  if (hProcess == NULL || szDllToFindW == NULL || szImportingDllW == NULL)
    return E_INVALIDARG;
  dwImportingDllLen = (DWORD)wcslen(szImportingDllW);
  if (dwImportingDllLen == 0)
    return E_INVALIDARG;
  dwImportingDllLenInBytes = dwImportingDllLen * (DWORD)sizeof(WCHAR);
  //----
  dwOsVer = ::GetVersion();
  if ((dwOsVer & 0x80000000) != 0)
    return E_NOTIMPL; //reject win9x
  if ((LOBYTE(LOWORD(dwOsVer))) < 6)
    return NKT_DVERR_NotFound; //reject pre-winvista
  if ((LOBYTE(LOWORD(dwOsVer))) == 6 && (HIBYTE(LOWORD(dwOsVer))) < 1)
    return NKT_DVERR_NotFound; //reject pre-win7
  //remove dll extension if any
  dwDllToFindLen = (DWORD)wcslen(szDllToFindW);
  for (dwTemp32=dwDllToFindLen; dwTemp32 > 0 && szDllToFindW[dwTemp32-1] != L'.'; dwTemp32--);
  if (dwTemp32 > 0)
    dwDllToFindLen = dwTemp32-1;
  //check dll name prefix
  if (dwDllToFindLen <= 4)
    return NKT_DVERR_NotFound;
  if (_wcsnicmp(szDllToFindW, L"api-", 4) != 0 && _wcsnicmp(szDllToFindW, L"ext-", 4) != 0)
    return NKT_DVERR_NotFound;
  //strip prefix
  szDllToFindW += 4;
  dwDllToFindLen -= 4;
  dwDllToFindLenInBytes = dwDllToFindLen * (DWORD)sizeof(WCHAR);
  //get peb
  hRes = nktDvNtGetPeb(&lpPeb, hProcess, nPlatformBits);
  if (FAILED(hRes))
    return hRes;
  //get pointer to apiset map
  switch (nPlatformBits)
  {
    case 32:
      if (ReadProcMem(hProcess, &dwTemp32, lpPeb+0x38, sizeof(dwTemp32)) == FALSE)
        return E_FAIL;
      lpApiMapSet = (LPBYTE)((ULONG_PTR)dwTemp32);
      break;
#if defined _M_X64
    case 64:
      if (ReadProcMem(hProcess, &qwTemp64, lpPeb+0x68, sizeof(qwTemp64)) == FALSE)
        return E_FAIL;
      lpApiMapSet = (LPBYTE)qwTemp64;
      break;
#endif //_M_X64
    default:
      return E_FAIL;
  }
  if (lpApiMapSet == NULL)
    return NKT_DVERR_NotFound;
  if (ReadProcMem(hProcess, &dwTemp32, lpApiMapSet, sizeof(dwTemp32)) == FALSE)
    return E_FAIL;
  switch (dwTemp32)
  {
    case 2: //found in Win7 & Win8
      {
      NKT_DV_APIMAPSET_HEADER_V2 sHdr;
      NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V2 sNamespaceEntry, *lpCurrNamespaceEntry;
      NKT_DV_APIMAPSET_HOST_HEADER_V2 sHostHdr;
      NKT_DV_APIMAPSET_HOST_ENTRY_V2 sHostEntry, sDefHostEntry, *lpCurrHostEntry;

      //read header entry
      if (ReadProcMem(hProcess, &sHdr, lpApiMapSet, sizeof(sHdr)) == FALSE)
        return E_FAIL;
      lpCurrNamespaceEntry = (NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V2*)(lpApiMapSet + sizeof(NKT_DV_APIMAPSET_HEADER_V2));
      for (; sHdr.dwEntriesCount>0; sHdr.dwEntriesCount--,lpCurrNamespaceEntry++)
      {
        if (ReadProcMem(hProcess, &sNamespaceEntry, lpCurrNamespaceEntry, sizeof(sNamespaceEntry)) == FALSE)
          return E_FAIL;
        //check if it is the dll we are looking for
        hRes = S_FALSE;
        if (sNamespaceEntry.dwNameLength == dwDllToFindLenInBytes)
        {
          hRes = CompareRemoteTextW(hProcess, (LPWSTR)(lpApiMapSet+(SIZE_T)(sNamespaceEntry.dwNameOffset)),
                                    szDllToFindW, dwDllToFindLen);
          if (FAILED(hRes))
            return hRes;
        }
        if (hRes == S_OK)
        {
          sDefHostEntry.dwLengthRealName = 0xFFFFFFFFUL;
          //scan host entries
          lpPtr = lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwHostModulesOffset);
          if (ReadProcMem(hProcess, &sHostHdr, lpPtr, sizeof(sHostHdr)) == FALSE)
            return E_FAIL;
          lpCurrHostEntry = (NKT_DV_APIMAPSET_HOST_ENTRY_V2*)(lpPtr + sizeof(sHostHdr));
          for (dwTemp32=0; dwTemp32<sHostHdr.dwCount; dwTemp32++)
          {
            if (ReadProcMem(hProcess, &sHostEntry, &lpCurrHostEntry[dwTemp32], sizeof(sHostEntry)) == FALSE)
              return E_FAIL;
            if (sHostEntry.dwLength == 0 || sHostEntry.dwNameOffset == 0)
              continue;
            if (sHostEntry.dwLengthRealName == dwImportingDllLenInBytes && sHostEntry.dwNameOffsetRealName > 0)
            {
              //check for importing dll name match
              hRes = CompareRemoteTextW(hProcess, (LPWSTR)(lpApiMapSet+(SIZE_T)(sHostEntry.dwNameOffsetRealName)),
                                        szImportingDllW, dwImportingDllLen);
              if (FAILED(hRes))
                return hRes;
              if (hRes == S_OK)
              {
gotIt_V2:       //retrieve dll name
                sHostEntry.dwLength &= (~1);
                k = (SIZE_T)(sHostEntry.dwLength) / sizeof(WCHAR);
                if (cStrReplDllW.EnsureBuffer(k) == FALSE)
                  return E_OUTOFMEMORY;
                if (ReadProcMem(hProcess, (LPWSTR)cStrReplDllW, (LPWSTR)(lpApiMapSet+(SIZE_T)(sHostEntry.dwNameOffset)),
                                (SIZE_T)(sHostEntry.dwLength)) == FALSE)
                  return E_FAIL;
                ((LPWSTR)cStrReplDllW)[k] = 0;
                cStrReplDllW.Refresh();
                return S_OK;
              }
            }
            else if (sHostEntry.dwLengthRealName == 0 && sDefHostEntry.dwLengthRealName == 0xFFFFFFFFUL)
            {
              nktMemCopy(&sDefHostEntry, &sHostEntry, sizeof(sHostEntry));
            }
          }
          //have a default?
          if (sDefHostEntry.dwLengthRealName != 0xFFFFFFFFUL)
          {
            nktMemCopy(&sHostEntry, &sDefHostEntry, sizeof(sHostEntry));
            goto gotIt_V2;
          }
        }
      }
      }
      break;

    case 4: //found in Win8.1
      {
      NKT_DV_APIMAPSET_HEADER_V4 sHdr;
      NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V4 sNamespaceEntry, *lpCurrNamespaceEntry;
      NKT_DV_APIMAPSET_HOST_HEADER_V4 sHostHdr;
      NKT_DV_APIMAPSET_HOST_ENTRY_V4 sHostEntry, sDefHostEntry, *lpCurrHostEntry;

      if (ReadProcMem(hProcess, &sHdr, lpApiMapSet, sizeof(sHdr)) == FALSE)
        return E_FAIL;
      lpCurrNamespaceEntry = (NKT_DV_APIMAPSET_NAMESPACE_ENTRY_V4*)(lpApiMapSet + sizeof(NKT_DV_APIMAPSET_HEADER_V4));
      for (; sHdr.dwEntriesCount>0; sHdr.dwEntriesCount--,lpCurrNamespaceEntry++)
      {
        if (ReadProcMem(hProcess, &sNamespaceEntry, lpCurrNamespaceEntry, sizeof(sNamespaceEntry)) == FALSE)
          return E_FAIL;
        //check if it is the dll we are looking for
        hRes = S_FALSE;
        if (sNamespaceEntry.dwNameLength1 == dwDllToFindLenInBytes)
        {
          hRes = CompareRemoteTextW(hProcess, (LPWSTR)(lpApiMapSet+(SIZE_T)(sNamespaceEntry.dwNameOffset1)),
                                    szDllToFindW, dwDllToFindLen);
          if (FAILED(hRes))
            return hRes;
        }
        if (hRes == S_FALSE && sNamespaceEntry.dwNameLength2 == dwDllToFindLenInBytes)
        {
          hRes = CompareRemoteTextW(hProcess, (LPWSTR)(lpApiMapSet+(SIZE_T)(sNamespaceEntry.dwNameOffset2)),
                                    szDllToFindW, dwDllToFindLen);
          if (FAILED(hRes))
            return hRes;
        }
        if (hRes == S_OK)
        {
          sDefHostEntry.dwLengthRealName = 0xFFFFFFFFUL;
          //scan host entries
          lpPtr = lpApiMapSet + (SIZE_T)(sNamespaceEntry.dwHostModulesOffset);
          if (ReadProcMem(hProcess, &sHostHdr, lpPtr, sizeof(sHostHdr)) == FALSE)
            return E_FAIL;
          lpCurrHostEntry = (NKT_DV_APIMAPSET_HOST_ENTRY_V4*)(lpPtr + sizeof(sHostHdr));
          for (dwTemp32=0; dwTemp32<sHostHdr.dwCount; dwTemp32++)
          {
            if (ReadProcMem(hProcess, &sHostEntry, &lpCurrHostEntry[dwTemp32], sizeof(sHostEntry)) == FALSE)
              return E_FAIL;
            if (sHostEntry.dwLength == 0 || sHostEntry.dwNameOffset == 0)
              continue;
            if (sHostEntry.dwLengthRealName == dwImportingDllLenInBytes && sHostEntry.dwNameOffsetRealName > 0)
            {
              //check for importing dll name match
              hRes = CompareRemoteTextW(hProcess, (LPWSTR)(lpApiMapSet+(SIZE_T)(sHostEntry.dwNameOffsetRealName)),
                                        szImportingDllW, dwImportingDllLen);
              if (FAILED(hRes))
                return hRes;
              if (hRes == S_OK)
              {
gotIt_V4:       //retrieve dll name
                sHostEntry.dwLength &= (~1);
                k = (SIZE_T)(sHostEntry.dwLength) / sizeof(WCHAR);
                if (cStrReplDllW.EnsureBuffer(k) == FALSE)
                  return E_OUTOFMEMORY;
                if (ReadProcMem(hProcess, (LPWSTR)cStrReplDllW, (LPWSTR)(lpApiMapSet+(SIZE_T)(sHostEntry.dwNameOffset)),
                                (SIZE_T)(sHostEntry.dwLength)) == FALSE)
                  return E_FAIL;
                ((LPWSTR)cStrReplDllW)[k] = 0;
                cStrReplDllW.Refresh();
                return S_OK;
              }
            }
            else if (sHostEntry.dwLengthRealName == 0 && sDefHostEntry.dwLengthRealName == 0xFFFFFFFFUL)
            {
              nktMemCopy(&sDefHostEntry, &sHostEntry, sizeof(sHostEntry));
            }
          }
          //have a default?
          if (sDefHostEntry.dwLengthRealName != 0xFFFFFFFFUL)
          {
            nktMemCopy(&sHostEntry, &sDefHostEntry, sizeof(sHostEntry));
            goto gotIt_V4;
          }
        }
      }
      }
      break;
  }
  return NKT_DVERR_NotFound;
}

HRESULT nktDvNtGetFileNameFromHandle(__out CNktStringW &cStrNameW, __in HANDLE hFile)
{
  TNktAutoFreePtr<NKT_DV_OBJECT_NAME_INFORMATION> cObjNameBuf;
  ULONG nObjNameBufLen, nRetLen;
  HRESULT hRes;

  cStrNameW.Empty();
  if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
    return E_INVALIDARG;
  for (nObjNameBufLen=2048;;)
  {
    cObjNameBuf.Reset();
    cObjNameBuf.Attach((NKT_DV_OBJECT_NAME_INFORMATION*)nktMemMalloc(nObjNameBufLen));
    if (cObjNameBuf == NULL)
      return E_OUTOFMEMORY;
    hRes = nktDvDynApis_NtQueryObject(hFile, NKT_DV_ObjectNameInformation, cObjNameBuf.Get(),
                                      nObjNameBufLen, &nRetLen);
    if (hRes != NKT_DV_NTSTATUS_BUFFER_TOO_SMALL && hRes != NKT_DV_NTSTATUS_INFO_LENGTH_MISMATCH)
      break;
    nObjNameBufLen += 2048;
    if (nObjNameBufLen < nRetLen)
      nObjNameBufLen = nRetLen;
  }
  if (SUCCEEDED(hRes))
  {
    if (cStrNameW.CopyN(cObjNameBuf->Name.Buffer, cObjNameBuf->Name.Length/sizeof(WCHAR)) == FALSE)
      hRes = E_OUTOFMEMORY;
  }
  return hRes;
}

HRESULT nktDvNtGetKeyNameFromHandle(__out CNktStringW &cStrNameW, __in HKEY hKey)
{
  return nktDvNtGetFileNameFromHandle(cStrNameW, (HANDLE)hKey);
}

HRESULT nktDvNtLockLoader(__out PVOID *lplpCookie, __in BOOL bOnlyTry)
{
  PRTL_CRITICAL_SECTION lpCS;
  ULONG nDisp;
  HRESULT hRes;

  if (lplpCookie == NULL)
    return E_POINTER;
  *lplpCookie = NULL;
  hRes = nktDvDynApis_LdrLockLoaderLock((bOnlyTry != FALSE) ? X_LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY : 0,
                                         &nDisp, lplpCookie);
  switch (hRes)
  {
    case S_OK:
      if (bOnlyTry != FALSE && nDisp == X_LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_NOT_ACQUIRED)
        hRes = S_FALSE;
      break;

    case E_NOTIMPL:
      lpCS = nktDvNtGetLoaderLock();
      hRes = nktDvDynApis_RtlEnterCriticalSection(lpCS, bOnlyTry);
      break;
  }
  return hRes;
}

HRESULT nktDvNtUnlockLoader(__in PVOID lpCookie)
{
  PRTL_CRITICAL_SECTION lpCS;
  HRESULT hRes;

  if (lpCookie == NULL)
    return E_POINTER;
  hRes = nktDvDynApis_LdrUnlockLoaderLock(0, lpCookie);
  if (hRes == E_NOTIMPL)
  {
    lpCS = nktDvNtGetLoaderLock();
    hRes = nktDvDynApis_RtlLeaveCriticalSection(lpCS);
  }
  return hRes;
}

PRTL_CRITICAL_SECTION nktDvNtGetLoaderLock()
{
#if defined _M_IX86
  static LONG volatile nLoaderLock = 0;
#elif defined _M_X64
  static __int64 volatile nLoaderLock = 0;
#else
  #error Unsupported platform
#endif

  if (nLoaderLock == 0)
  {
    LPBYTE lpPtr;

    //loader lock will not vary its address when process is running
#if defined _M_IX86
    lpPtr = (LPBYTE)__readfsdword(0x30); //get PEB from the TIB
    lpPtr = *((LPBYTE*)(lpPtr+0xA0));    //get loader lock pointer
    _InterlockedExchange(&nLoaderLock, (LONG)lpPtr);
#elif defined _M_X64
    lpPtr = (LPBYTE)__readgsqword(0x30); //get TEB
    lpPtr = *((LPBYTE*)(lpPtr+0x60));
    lpPtr = *((LPBYTE*)(lpPtr+0x110));
    _InterlockedExchange64(&nLoaderLock, (__int64)lpPtr);
#else
  #error Unsupported platform
#endif
  }
  return (PRTL_CRITICAL_SECTION)nLoaderLock;
}

BOOL nktDvNtIsLoaderLoaderLockHeld(__in BOOL bAnyThread)
{
  PRTL_CRITICAL_SECTION lpCS;

  lpCS = nktDvNtGetLoaderLock();
  return (lpCS->OwningThread != NULL &&
          (bAnyThread != FALSE || lpCS->OwningThread == (HANDLE)((ULONG_PTR)::GetCurrentThreadId()))) ? TRUE : FALSE;
}

//-----------------------------------------------------------

static __inline BOOL ReadProcMem(__in HANDLE hProc, __out LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nDataSize)
{
  SIZE_T nReaded;

  return (::ReadProcessMemory(hProc, lpSrc, lpDest, nDataSize, &nReaded) != FALSE &&
          nReaded == nDataSize) ? TRUE : FALSE;
}

static HRESULT CompareRemoteTextW(__in HANDLE hProcess, __in LPWSTR szRemoteTextW, __in LPCWSTR szTextW,
                                  __in DWORD dwChars)
{
  WCHAR szTempW[128];
  SIZE_T nThisRoundChars;

  while (dwChars > 0)
  {
    if ((nThisRoundChars = (SIZE_T)dwChars) > NKT_DV_ARRAYLEN(szTempW))
      nThisRoundChars = NKT_DV_ARRAYLEN(szTempW);
    if (ReadProcMem(hProcess, szTempW, szRemoteTextW, nThisRoundChars*sizeof(WCHAR)) == FALSE)
      return E_FAIL;
    if (_wcsnicmp(szTextW, szTempW, nThisRoundChars) != 0)
      break;
    szTextW += nThisRoundChars;
    szRemoteTextW += nThisRoundChars;
    dwChars -= (DWORD)nThisRoundChars;
  }
  return (dwChars == 0) ? S_OK : S_FALSE;
}
