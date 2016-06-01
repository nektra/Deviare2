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

#include "EngBaseObj.h"
#include <objbase.h>
#include "Debug.h"
class CNktDvParam;

//-----------------------------------------------------------

class CNktDvProcessMemory : public TNktDvEngBaseObjWithLock<CNktFastMutex>,
                            private TNktLnkLstNode<CNktDvProcessMemory>
{
public:
  typedef enum {
    protExecute      = 0x01,
    protRead         = 0x02,
    protWrite        = 0x04,
    protIsWriteCopy  = 0x08,
    protGuard        = 0x10,
    protNoCache      = 0x20,
    protWriteCombine = 0x40
  } eProtection;

protected:
  CNktDvProcessMemory(__in DWORD dwPid);
public:
  virtual ~CNktDvProcessMemory();

  static CNktDvProcessMemory* CreateForPID(__in DWORD dwPid);

  virtual DWORD GetProcessId() const
    {
    return dwPid;
    };

  virtual ULONG __stdcall Release(); //override

  virtual HRESULT GetProtection(__out eProtection *lpnProtection, __in LPCVOID lpRemotePtr,
                                __in BOOL bAllocated=TRUE);

  virtual HRESULT Read(__out LPVOID lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nSize,
                       __deref_out_opt SIZE_T *lpnReaded=NULL);
  virtual HRESULT Write(__in LPVOID lpRemoteDest, __in LPCVOID lpLocalSrc, __in SIZE_T nSize,
                        __deref_out_opt SIZE_T *lpnWritten=NULL);
  virtual HRESULT WriteProtected(__in LPVOID lpRemoteDest, __in LPCVOID lpLocalSrc, __in SIZE_T nSize,
                                 __deref_out_opt SIZE_T *lpnWritten=NULL);

  virtual HRESULT ReadWord(__out LPWORD lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                           __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(WORD), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(WORD);
    return hRes;
    };

  virtual HRESULT ReadDWord(__out LPDWORD lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                            __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(DWORD), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(DWORD);
    return hRes;
    };

  virtual HRESULT ReadQWord(__out PULONGLONG lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                            __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(ULONGLONG), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(ULONGLONG);
    return hRes;
    };

  virtual HRESULT ReadFloat(__out float *lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                            __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(float), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(float);
    return hRes;
    };

  virtual HRESULT ReadDouble(__out double *lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(double), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(double);
    return hRes;
    };

  virtual HRESULT ReadSizeT(__out PSIZE_T lpLocalDest, __in LPCVOID lpRemoteSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnReaded=NULL)
    {
    HRESULT hRes;

    hRes = Read(lpLocalDest, lpRemoteSrc, nCount*sizeof(SIZE_T), lpnReaded);
    if (lpnReaded != NULL)
      *lpnReaded /= sizeof(SIZE_T);
    return hRes;
    };

  virtual HRESULT WriteWord(__in LPVOID lpRemoteDest, __in LPWORD lpLocalSrc, __in SIZE_T nCount,
                            __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(WORD), lpnWritten);
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(WORD);
    return hRes;
    };
  virtual HRESULT WriteWord(__in LPVOID lpRemoteDest, __in WORD nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(WORD));
    };

  virtual HRESULT WriteDWord(__in LPVOID lpRemoteDest, __in LPDWORD lpLocalSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(DWORD));
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(DWORD);
    return hRes;
    };
  virtual HRESULT WriteDWord(__in LPVOID lpRemoteDest, __in DWORD nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(DWORD));
    };

  virtual HRESULT WriteQWord(__in LPVOID lpRemoteDest, __in PULONGLONG lpLocalSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(ULONGLONG));
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(ULONGLONG);
    return hRes;
    };
  virtual HRESULT WriteQWord(__in LPVOID lpRemoteDest, __in ULONGLONG nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(ULONGLONG));
    };

  virtual HRESULT WriteFloat(__in LPVOID lpRemoteDest, __in float *lpLocalSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(float));
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(float);
    return hRes;
    };
  virtual HRESULT WriteFloat(__in LPVOID lpRemoteDest, __in float nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(float));
    };

  virtual HRESULT WriteDouble(__in LPVOID lpRemoteDest, __in double *lpLocalSrc, __in SIZE_T nCount,
                              __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(double));
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(double);
    return hRes;
    };
  virtual HRESULT WriteDouble(__in LPVOID lpRemoteDest, __in double nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(double));
    };

  virtual HRESULT WriteSizeT(__in LPVOID lpRemoteDest, __in PSIZE_T lpLocalSrc, __in SIZE_T nCount,
                             __deref_out_opt SIZE_T *lpnWritten=NULL)
    {
    HRESULT hRes;

    hRes = WriteProtected(lpRemoteDest, lpLocalSrc, nCount*sizeof(SIZE_T));
    if (lpnWritten != NULL)
      *lpnWritten /= sizeof(SIZE_T);
    return hRes;
    };
  virtual HRESULT WriteSizeT(__in LPVOID lpRemoteDest, __in SIZE_T nValue)
    {
    return WriteProtected(lpRemoteDest, &nValue, sizeof(SIZE_T));
    };

  virtual HRESULT ReadStringA(__deref_out LPSTR* lpszDestA, __in LPCVOID lpRemoteSrc,
                              __in SIZE_T nMaxChars=NKT_SIZE_T_MAX);
  virtual HRESULT ReadStringW(__deref_out LPWSTR* lpszDestW, __in LPCVOID lpRemoteSrc,
                              __in SIZE_T nMaxChars=NKT_SIZE_T_MAX);

  virtual HRESULT WriteStringA(__in LPVOID lpRemoteDest, __in_nz_opt LPCSTR szSrcA,
                               __in SIZE_T nSrcLen=NKT_SIZE_T_MAX, __in BOOL bWriteNulTerminator=TRUE);
  virtual HRESULT WriteStringW(__in LPVOID lpRemoteDest, __in_nz_opt LPCWSTR szSrcW,
                               __in SIZE_T nSrcLen=NKT_SIZE_T_MAX, __in BOOL bWriteNulTerminator=TRUE);

  virtual HRESULT GetStringLengthA(__in LPVOID lpRemoteDest, __out SIZE_T *lpnChars);
  virtual HRESULT GetStringLengthW(__in LPVOID lpRemoteDest, __out SIZE_T *lpnChars);

  virtual HRESULT AllocMem(__deref_out LPVOID *lplpRemoteDest, __in SIZE_T nSize, __in BOOL bExecuteFlag);
  virtual HRESULT FreeMem(__in LPVOID lpRemoteDest);

  virtual HRESULT ToString(__inout CNktStringW &cStrDest);

  static VOID FreeString(__in LPVOID lpString);

  virtual HRESULT BuildParam(__deref_out CNktDvParam **lplpParam, __in SIZE_T nAddress,
                             __in_z LPCWSTR szTypeW);

  virtual HANDLE GetReadAccessHandle();
  virtual HANDLE GetWriteAccessHandle();

private:
  DWORD dwPid;
  HANDLE volatile hProc[2];
  BOOL bWriteAccessChecked, bIsLocal;
};

//-----------------------------------------------------------

