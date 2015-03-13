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
#include "ProcessMemory.h"
#include "Param.h"

//-----------------------------------------------------------
// CNktProcessMemoryImpl

HRESULT CNktProcessMemoryImpl::GetProtection(__in my_ssize_t remoteAddr, __in VARIANT_BOOL allocated,
                                             __deref_out eNktProtection *pVal)
{
  CNktDvProcessMemory::eProtection nProt;
  HRESULT hRes;

  if (pVal == NULL || remoteAddr == 0)
  {
    if (pVal != NULL)
      *pVal = (eNktProtection)0;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //get protection
    hRes = lpIntObj->GetProtection(&nProt, (LPVOID)remoteAddr, Bool_From_VariantBool(allocated));
  }
  else
  {
    nProt = (CNktDvProcessMemory::eProtection)0;
  }
  *pVal = (eNktProtection)nProt;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::ReadMem(__in my_ssize_t localAddr, __in my_ssize_t remoteAddr,
                                       __in my_ssize_t bytes, __deref_out my_ssize_t *pReaded)
{
  HRESULT hRes;
  SIZE_T nReaded;

  if (pReaded == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  nReaded = 0;
  if (bytes == 0)
  {
    hRes = S_OK;
  }
  else if (localAddr == 0 || remoteAddr == 0)
  {
    hRes = E_POINTER;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //read memory
      hRes = lpIntObj->Read((LPVOID)localAddr, (LPVOID)remoteAddr, (SIZE_T)bytes, &nReaded);
    }
  }
  *pReaded = (my_ssize_t)nReaded;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::WriteMem(__in my_ssize_t remoteAddr, __in my_ssize_t localAddr,
                                        __in my_ssize_t bytes, __deref_out my_ssize_t *pWritten)
{
  SIZE_T nWritten;
  HRESULT hRes;

  if (pWritten == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  nWritten = 0;
  if (bytes == 0)
  {
    hRes = S_OK;
  }
  else if (localAddr == 0 || remoteAddr == 0)
  {
    hRes = E_POINTER;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //write memory
      hRes = lpIntObj->WriteProtected((LPVOID)remoteAddr, (LPVOID)localAddr, (SIZE_T)bytes, &nWritten);
    }
  }
  *pWritten = (my_ssize_t)nWritten;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::Read(__in my_ssize_t remoteAddr, __in eNktDboFundamentalType type,
                                    __deref_out VARIANT *pVal)
{
  HRESULT hRes;
  my_ssize_t nReaded;

  if (pVal == NULL || remoteAddr == 0)
  {
    if (pVal != NULL)
    {
      ::VariantInit(pVal);
      pVal->vt = VT_ERROR;
      pVal->scode = E_POINTER;
    }
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //read
  ::VariantInit(pVal);
  switch (type)
  {
    case ftSignedByte:
    case ftAnsiChar:
    case ftUnsignedByte:
      pVal->vt = (type != ftUnsignedByte) ? VT_I1 : VT_UI1;
      hRes = ReadMem((my_ssize_t)&(pVal->bVal), remoteAddr, sizeof(unsigned char), &nReaded);
      if (FAILED(hRes))
        pVal->bVal = 0;
      break;

    case ftSignedWord:
    case ftUnsignedWord:
    case ftWideChar:
      pVal->vt = (type == ftSignedWord) ? VT_I2 : VT_UI2;
      hRes = ReadMem((my_ssize_t)&(pVal->iVal), remoteAddr, sizeof(unsigned short), &nReaded);
      if (FAILED(hRes))
        pVal->iVal = 0;
      break;

    case ftSignedDoubleWord:
    case ftUnsignedDoubleWord:
      pVal->vt = (type == ftSignedDoubleWord) ? VT_I4 : VT_UI4;
      hRes = ReadMem((my_ssize_t)&(pVal->lVal), remoteAddr, sizeof(unsigned long), &nReaded);
      if (FAILED(hRes))
        pVal->lVal = 0;
      break;

    case ftSignedQuadWord:
    case ftUnsignedQuadWord:
      pVal->vt = (type == ftSignedQuadWord) ? VT_I8 : VT_UI8;
      hRes = ReadMem((my_ssize_t)&(pVal->llVal), remoteAddr, sizeof(unsigned __int64), &nReaded);
      if (FAILED(hRes))
        pVal->llVal = 0;
      break;

    case ftFloat:
      pVal->vt = VT_R4;
      hRes = ReadMem((my_ssize_t)&(pVal->fltVal), remoteAddr, sizeof(float), &nReaded);
      if (FAILED(hRes))
        pVal->fltVal = 0.0f;
      break;

    case ftDouble:
    case ftLongDouble:
      pVal->vt = VT_R8;
      hRes = ReadMem((my_ssize_t)&(pVal->dblVal), remoteAddr, sizeof(double), &nReaded);
      if (FAILED(hRes))
        pVal->dblVal = 0.0;
      break;

    default:
      pVal->vt = VT_ERROR;
      pVal->scode = hRes = E_INVALIDARG;
      break;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::ReadArray(__in my_ssize_t remoteAddr, __in eNktDboFundamentalType type,
                                         __in LONG count, __deref_out VARIANT *pVal)
{
  SAFEARRAYBOUND sNewBounds;
  my_ssize_t nReaded = 0;
  HRESULT hRes;

  if (pVal != NULL)
  {
    ::VariantInit(pVal);
    pVal->vt = VT_ERROR;
  }
  if (pVal == NULL || remoteAddr == 0)
  {
    if (pVal != NULL)
      pVal->scode = E_POINTER;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  if (count < 0)
  {
    pVal->scode = E_INVALIDARG;
    return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  //create array
  switch (type)
  {
    case ftSignedByte:
    case ftAnsiChar:
    case ftUnsignedByte:
      pVal->vt = VT_ARRAY | ((type != ftUnsignedByte) ? VT_I1 : VT_UI1);
      break;

    case ftSignedWord:
    case ftUnsignedWord:
    case ftWideChar:
      pVal->vt = VT_ARRAY | ((type == ftSignedWord) ? VT_I2 : VT_UI2);
      break;

    case ftSignedDoubleWord:
    case ftUnsignedDoubleWord:
      pVal->vt = VT_ARRAY | ((type == ftSignedDoubleWord) ? VT_I4 : VT_UI4);
      break;

    case ftSignedQuadWord:
    case ftUnsignedQuadWord:
      pVal->vt = VT_ARRAY | ((type == ftSignedQuadWord) ? VT_I8 : VT_UI8);
      break;

    case ftFloat:
      pVal->vt = VT_ARRAY | VT_R4;
      break;

    case ftDouble:
    case ftLongDouble:
      pVal->vt = VT_ARRAY | VT_R8;
      break;

    default:
      pVal->scode = E_INVALIDARG;
      return SetupErrorInfoAndReturn(E_INVALIDARG);
  }
  pVal->parray = ::SafeArrayCreateVector(pVal->vt & (~VT_ARRAY), 0, (ULONG)count);
  if (pVal->parray == NULL)
  {
    pVal->scode = E_OUTOFMEMORY;
    return SetupErrorInfoAndReturn(E_OUTOFMEMORY);
  }
  //read data
  switch (type)
  {
    case ftSignedByte:
    case ftAnsiChar:
    case ftUnsignedByte:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr,
                     (SIZE_T)count * sizeof(unsigned char), &nReaded);
      nReaded /= sizeof(unsigned char);
      break;

    case ftSignedWord:
    case ftUnsignedWord:
    case ftWideChar:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr,
                     (SIZE_T)count * sizeof(unsigned short), &nReaded);
      nReaded /= sizeof(unsigned short);
      break;

    case ftSignedDoubleWord:
    case ftUnsignedDoubleWord:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr,
                     (SIZE_T)count * sizeof(unsigned long), &nReaded);
      nReaded /= sizeof(unsigned long);
      break;

    case ftSignedQuadWord:
    case ftUnsignedQuadWord:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr,
                     (SIZE_T)count * sizeof(unsigned __int64), &nReaded);
      nReaded /= sizeof(unsigned __int64);
      break;

    case ftFloat:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr, (SIZE_T)count * sizeof(float),
                     &nReaded);
      nReaded /= sizeof(float);
      break;

    case ftDouble:
    case ftLongDouble:
      hRes = ReadMem((my_ssize_t)&(pVal->parray->pvData), remoteAddr, (SIZE_T)count * sizeof(double),
                     &nReaded);
      nReaded /= sizeof(double);
      break;
  }
  if (hRes == NKT_DVERR_PartialCopy || hRes == E_ACCESSDENIED)
  {
    sNewBounds.lLbound = 0;
    sNewBounds.cElements = (ULONG)((my_size_t)nReaded);
    hRes = ::SafeArrayRedim(pVal->parray, &sNewBounds);
    if (FAILED(hRes))
      goto err_dodestroy;
  }
  else if (FAILED(hRes))
  {
err_dodestroy:
    ::SafeArrayDestroy(pVal->parray);
    pVal->vt = VT_ERROR;
    pVal->scode = hRes;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::Write(__in my_ssize_t remoteAddr, __in VARIANT var)
{
  LPVOID lpData;
  SIZE_T nItemsCount;
  VARIANT *lpVar;
  HRESULT hRes;
  my_ssize_t nWritten;

  if (remoteAddr == 0)
    return SetupErrorInfoAndReturn(E_POINTER);
  //write
  lpVar = RealVariant_From_Variant(&var);
  if (lpVar == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  switch (lpVar->vt)
  {
    case VT_I1:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->cVal), sizeof(char), &nWritten);
      break;
    case VT_UI1:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->bVal), sizeof(unsigned char), &nWritten);
      break;
    case VT_I2:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->iVal), sizeof(short), &nWritten);
      break;
    case VT_UI2:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->uiVal), sizeof(unsigned short), &nWritten);
      break;
    case VT_I4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->lVal), sizeof(LONG), &nWritten);
      break;
    case VT_UI4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->ulVal), sizeof(ULONG), &nWritten);
      break;
    case VT_INT:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->intVal), sizeof(LONG), &nWritten);
      break;
    case VT_UINT:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->uintVal), sizeof(ULONG), &nWritten);
      break;
    case VT_I8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->llVal), sizeof(LONGLONG), &nWritten);
      break;
    case VT_UI8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->ullVal), sizeof(ULONGLONG), &nWritten);
      break;
    case VT_R4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->fltVal), sizeof(float), &nWritten);
      break;
    case VT_R8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)&(lpVar->dblVal), sizeof(double), &nWritten);
      break;

    case VT_BYREF|VT_I1:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pcVal), sizeof(char), &nWritten);
      break;
    case VT_BYREF|VT_UI1:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pbVal), sizeof(unsigned char), &nWritten);
      break;
    case VT_BYREF|VT_I2:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->piVal), sizeof(short), &nWritten);
      break;
    case VT_BYREF|VT_UI2:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->puiVal), sizeof(unsigned short), &nWritten);
      break;
    case VT_BYREF|VT_I4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->plVal), sizeof(LONG), &nWritten);
      break;
    case VT_BYREF|VT_UI4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pulVal), sizeof(ULONG), &nWritten);
      break;
    case VT_BYREF|VT_INT:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pintVal), sizeof(LONG), &nWritten);
      break;
    case VT_BYREF|VT_UINT:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->puintVal), sizeof(ULONG), &nWritten);
      break;
    case VT_BYREF|VT_I8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pllVal), sizeof(LONGLONG), &nWritten);
      break;
    case VT_BYREF|VT_UI8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pullVal), sizeof(ULONGLONG), &nWritten);
      break;
    case VT_BYREF|VT_R4:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pfltVal), sizeof(float), &nWritten);
      break;
    case VT_BYREF|VT_R8:
      hRes = WriteMem(remoteAddr, (my_ssize_t)(lpVar->pdblVal), sizeof(double), &nWritten);
      break;

    case VT_ARRAY|VT_I1:
    case VT_ARRAY|VT_UI1:
    case VT_ARRAY|VT_I2:
    case VT_ARRAY|VT_UI2:
    case VT_ARRAY|VT_I4:
    case VT_ARRAY|VT_UI4:
    case VT_ARRAY|VT_INT:
    case VT_ARRAY|VT_UINT:
    case VT_ARRAY|VT_I8:
    case VT_ARRAY|VT_UI8:
    case VT_ARRAY|VT_R4:
    case VT_ARRAY|VT_R8:
      if (lpVar->parray == NULL)
      {
        hRes = E_POINTER;
        break;
      }
      if (lpVar->parray->cDims != 1)
      {
        hRes = E_INVALIDARG;
        break;
      }
      nItemsCount = (SIZE_T)(lpVar->parray->rgsabound[0].cElements);
      //get array data
      hRes = ::SafeArrayAccessData(lpVar->parray, (LPVOID*)&lpData);
      if (FAILED(hRes))
        break;
      switch (lpVar->vt)
      {
        case VT_ARRAY|VT_I1:
        case VT_ARRAY|VT_UI1:
          nItemsCount *= sizeof(char);
          break;
        case VT_ARRAY|VT_I2:
        case VT_ARRAY|VT_UI2:
          nItemsCount *= sizeof(short);
          break;
        case VT_ARRAY|VT_I4:
        case VT_ARRAY|VT_INT:
        case VT_ARRAY|VT_UI4:
        case VT_ARRAY|VT_UINT:
          nItemsCount *= sizeof(LONG);
          break;
        case VT_ARRAY|VT_I8:
        case VT_ARRAY|VT_UI8:
          nItemsCount *= sizeof(LONGLONG);
          break;
        case VT_ARRAY|VT_R4:
          nItemsCount *= sizeof(float);
          break;
        case VT_ARRAY|VT_R8:
          nItemsCount *= sizeof(double);
          break;
      }
      hRes = WriteMem(remoteAddr, (my_ssize_t)lpData, nItemsCount, &nWritten);
      ::SafeArrayUnaccessData(lpVar->parray);
      break;

    default:
      hRes = E_INVALIDARG;
      break;
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::ReadString(__in my_ssize_t remoteAddr, __in VARIANT_BOOL isAnsi, __deref_out BSTR *pVal)
{
  union {
    LPVOID lpStr;
    LPSTR szDestA;
    LPWSTR szDestW;
  };
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  lpStr = NULL;
  if (remoteAddr == 0)
  {
    hRes = E_POINTER;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //read string
      if (isAnsi != VARIANT_FALSE)
        hRes = lpIntObj->ReadStringA(&szDestA, (LPVOID)remoteAddr);
      else
        hRes = lpIntObj->ReadStringW(&szDestW, (LPVOID)remoteAddr);
    }
    if (SUCCEEDED(hRes))
    {
      if (isAnsi != VARIANT_FALSE)
        hRes = cComBStr.Set(szDestA);
      else
        hRes = cComBStr.Set(szDestW);
    }
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  CNktDvProcessMemory::FreeString(lpStr);
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::ReadStringN(__in my_ssize_t remoteAddr, __in VARIANT_BOOL isAnsi,
                                           __in LONG maxChars, __deref_out BSTR *pVal)
{
  union {
    LPVOID lpStr;
    LPSTR szDestA;
    LPWSTR szDestW;
  };
  CNktComBStr cComBStr;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  lpStr = NULL;
  if (remoteAddr == 0)
  {
    hRes = E_POINTER;
  }
  else if (maxChars < 0)
  {
    hRes = E_INVALIDARG;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //read string
      if (isAnsi != VARIANT_FALSE)
        hRes = lpIntObj->ReadStringA(&szDestA, (LPVOID)remoteAddr, (SIZE_T)maxChars);
      else
        hRes = lpIntObj->ReadStringW(&szDestW, (LPVOID)remoteAddr, (SIZE_T)maxChars);
    }
    if (SUCCEEDED(hRes))
    {
      if (isAnsi != VARIANT_FALSE)
        hRes = cComBStr.Set(szDestA);
      else
        hRes = cComBStr.Set(szDestW);
    }
  }
  if (FAILED(hRes) && IsFatalHResult(hRes) == FALSE)
  {
    if (FAILED(cComBStr.Set(L"")))
      hRes = E_OUTOFMEMORY;
  }
  CNktDvProcessMemory::FreeString(lpStr);
  *pVal = cComBStr.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::WriteString(__in my_ssize_t remoteAddr, __in BSTR str,
                                           __in VARIANT_BOOL asAnsi, __in VARIANT_BOOL writeNul)
{
  HRESULT hRes;

  if (remoteAddr == NULL || str == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    if (asAnsi != VARIANT_FALSE)
    {
      TNktAutoFreePtr<CHAR> sA;

      sA.Attach(CNktDvTools::Wide2Ansi(str));
      if (sA == NULL)
      {
        hRes = E_OUTOFMEMORY;
      }
      else
      {
        //write ansi string
        hRes = lpIntObj->WriteStringA((LPVOID)remoteAddr, sA, NKT_SIZE_T_MAX,
                                     Bool_From_VariantBool(writeNul));
      }
    }
    else
    {
      //write wide string
      hRes = lpIntObj->WriteStringW((LPVOID)remoteAddr, str, NKT_SIZE_T_MAX,
                                   Bool_From_VariantBool(writeNul));
    }
  }
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::StringLength(__in my_ssize_t remoteAddr, __in VARIANT_BOOL asAnsi,
                                            __deref_out LONG *pVal)
{
  SIZE_T nChars;
  HRESULT hRes;

  if (pVal == NULL)
    return SetupErrorInfoAndReturn(E_POINTER);
  nChars = 0;
  if (remoteAddr == 0)
  {
    hRes = E_POINTER;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //read string length
      if (asAnsi != VARIANT_FALSE)
        hRes = lpIntObj->GetStringLengthA((LPVOID)remoteAddr, &nChars);
      else
        hRes = lpIntObj->GetStringLengthW((LPVOID)remoteAddr, &nChars);
    }
    if (nChars > LONG_MAX)
      nChars = LONG_MAX;
  }
  *pVal = (SUCCEEDED(hRes)) ? (LONG)nChars : 0;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::get_CharVal(__in my_ssize_t remoteAddr, __deref_out signed char *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(char), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_CharVal(__in my_ssize_t remoteAddr, __in signed char newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_ByteVal(__in my_ssize_t remoteAddr, __deref_out unsigned char *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(char), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_ByteVal(__in my_ssize_t remoteAddr, __in unsigned char newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_ShortVal(__in my_ssize_t remoteAddr, __deref_out short *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(short), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_ShortVal(__in my_ssize_t remoteAddr, __in short newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_UShortVal(__in my_ssize_t remoteAddr, __deref_out unsigned short *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(short), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_UShortVal(__in my_ssize_t remoteAddr, __in unsigned short newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_LongVal(__in my_ssize_t remoteAddr, __deref_out long *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(LONG), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_LongVal(__in my_ssize_t remoteAddr, __in long newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_ULongVal(__in my_ssize_t remoteAddr, __deref_out unsigned long *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(LONG), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_ULongVal(__in my_ssize_t remoteAddr, __in unsigned long newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_LongLongVal(__in my_ssize_t remoteAddr, __deref_out __int64 *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(LONGLONG), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_LongLongVal(__in my_ssize_t remoteAddr, __in __int64 newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_ULongLongVal(__in my_ssize_t remoteAddr, __deref_out unsigned __int64 *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(LONGLONG), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_ULongLongVal(__in my_ssize_t remoteAddr, __in unsigned __int64 newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_SSizeTVal(__in my_ssize_t remoteAddr, __deref_out my_ssize_t *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(my_ssize_t), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_SSizeTVal(__in my_ssize_t remoteAddr, __in my_ssize_t newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_SizeTVal(__in my_ssize_t remoteAddr, __deref_out my_size_t *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(my_size_t), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_SizeTVal(__in my_ssize_t remoteAddr, __in my_size_t newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_FloatVal(__in my_ssize_t remoteAddr, __deref_out float *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(float), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_FloatVal(__in my_ssize_t remoteAddr, __in float newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::get_DoubleVal(__in my_ssize_t remoteAddr, __deref_out double *pVal)
{
  my_ssize_t nReaded;

  return ReadMem((my_ssize_t)pVal, remoteAddr, sizeof(double), &nReaded);
}

HRESULT CNktProcessMemoryImpl::put_DoubleVal(__in my_ssize_t remoteAddr, __in double newValue)
{
  my_ssize_t nWritten;

  return WriteMem(remoteAddr, (my_ssize_t)&newValue, sizeof(newValue), &nWritten);
}

HRESULT CNktProcessMemoryImpl::BuildParam(__in my_ssize_t remoteAddr,__in BSTR type,
                                          __deref_out INktParam **ppParam)
{
  TNktComPtr<CNktDvParam> cParam;
  TNktComPtr<INktParam> cIParam;
  HRESULT hRes;

  if (ppParam == NULL || remoteAddr == 0)
  {
    if (ppParam != NULL)
      *ppParam = NULL;
    return SetupErrorInfoAndReturn(E_POINTER);
  }
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //build parameter
    hRes = lpIntObj->BuildParam(&cParam, (SIZE_T)remoteAddr, type);
  }
  if (SUCCEEDED(hRes))
  {
    //get outer interface
    hRes = GetComLinkHelper<CNktParamImpl,INktParam,CNktDvParam>(&cIParam, cParam,
                                                                 InterfaceItemGetCookies());
  }
  //done (don't create dummy interface on error)
  *ppParam = cIParam.Detach();
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::AllocMem(__in my_ssize_t bytes, __in VARIANT_BOOL executeFlag,
                                        __deref_out my_ssize_t *pVal)
{
  LPVOID lpRemoteDest;
  HRESULT hRes;

  if (bytes == 0)
    bytes = 1; //act like malloc
  if (bytes < 0)
  {
    hRes = E_INVALIDARG;
  }
  //check internal object
  else
  {
    get_InterfaceError(&hRes);
    if (SUCCEEDED(hRes))
    {
      //allocate remote memory
      hRes = lpIntObj->AllocMem(&lpRemoteDest, (SIZE_T)bytes, Bool_From_VariantBool(executeFlag));
    }
  }
  *pVal = (SUCCEEDED(hRes)) ? (my_ssize_t)lpRemoteDest : 0;
  return SetupErrorInfoAndReturn(hRes);
}

HRESULT CNktProcessMemoryImpl::FreeMem(__in my_ssize_t remoteAddr)
{
  HRESULT hRes;

  if (remoteAddr == 0)
    return SetupErrorInfoAndReturn(E_POINTER);
  //check internal object
  get_InterfaceError(&hRes);
  if (SUCCEEDED(hRes))
  {
    //free remote memory
    hRes = lpIntObj->FreeMem((LPVOID)remoteAddr);
  }
  return SetupErrorInfoAndReturn(hRes);
}
