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
#include "StructPEOptionalHeader.h"

//-----------------------------------------------------------
// CNktStructPEOptionalHeaderImpl

HRESULT CNktStructPEOptionalHeaderImpl::get_Magic(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMagic;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MajorLinkerVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMajorLinkerVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MinorLinkerVersion(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMinorLinkerVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfCode(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfCode;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfInitializedData(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfInitializedData;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfUninitializedData(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfUninitializedData;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_AddressOfEntryPoint(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nAddressOfEntryPoint;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_BaseOfCode(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nBaseOfCode;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_ImageBase(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nImageBase;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SectionAlignment(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSectionAlignment;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_FileAlignment(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nFileAlignment;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MajorOperatingSystemVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMajorOperatingSystemVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MinorOperatingSystemVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMinorOperatingSystemVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MajorImageVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMajorImageVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MinorImageVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMinorImageVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MajorSubsystemVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMajorSubsystemVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_MinorSubsystemVersion(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMinorSubsystemVersion;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_Win32VersionValue(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nWin32VersionValue;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfImage(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfImage;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfHeaders(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfHeaders;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_CheckSum(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nCheckSum;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_Subsystem(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSubsystem;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_DllCharacteristics(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nDllCharacteristics;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfStackReserve(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfStackReserve;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfStackCommit(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfStackCommit;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfHeapReserve(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfHeapReserve;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_SizeOfHeapCommit(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfHeapCommit;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_LoaderFlags(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nLoaderFlags;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_NumberOfRvaAndSizes(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nNumberOfRvaAndSizes;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_DataDirectoryStart(__in LONG index,
                                                               __deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)NKT_DV_ARRAYLEN(aDataDirectory))
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = aDataDirectory[index].nStart;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::get_DataDirectoryEnd(__in LONG index,
                                                             __deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)NKT_DV_ARRAYLEN(aDataDirectory))
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = aDataDirectory[index].nEnd;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEOptionalHeaderImpl::Initialize(__in NKT_DV_IMAGE_NT_HEADER *lpNtHdr,
                                                   __in SIZE_T nPlatformBits)
{
  SIZE_T i;

  switch (nPlatformBits)
  {
    case 32:
      nMagic = (short)(lpNtHdr->u32.OptionalHeader.Magic);
      nMajorLinkerVersion = (short)(lpNtHdr->u32.OptionalHeader.MajorLinkerVersion);
      nMinorLinkerVersion = (short)(lpNtHdr->u32.OptionalHeader.MinorLinkerVersion);
      nSizeOfCode = (LONG)(lpNtHdr->u32.OptionalHeader.SizeOfCode);
      nSizeOfInitializedData = (LONG)(lpNtHdr->u32.OptionalHeader.SizeOfInitializedData);
      nSizeOfUninitializedData = (LONG)(lpNtHdr->u32.OptionalHeader.SizeOfUninitializedData);
      nAddressOfEntryPoint = (LONG)(lpNtHdr->u32.OptionalHeader.AddressOfEntryPoint);
      nBaseOfCode = (LONG)(lpNtHdr->u32.OptionalHeader.BaseOfCode);
      nImageBase = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.ImageBase);
      nSectionAlignment = (LONG)(lpNtHdr->u32.OptionalHeader.SectionAlignment);
      nFileAlignment = (LONG)(lpNtHdr->u32.OptionalHeader.FileAlignment);
      nMajorOperatingSystemVersion = (short)(lpNtHdr->u32.OptionalHeader.MajorOperatingSystemVersion);
      nMinorOperatingSystemVersion = (short)(lpNtHdr->u32.OptionalHeader.MinorOperatingSystemVersion);
      nMajorImageVersion = (short)(lpNtHdr->u32.OptionalHeader.MajorImageVersion);
      nMinorImageVersion = (short)(lpNtHdr->u32.OptionalHeader.MinorImageVersion);
      nMajorSubsystemVersion = (short)(lpNtHdr->u32.OptionalHeader.MajorSubsystemVersion);
      nMinorSubsystemVersion = (short)(lpNtHdr->u32.OptionalHeader.MinorSubsystemVersion);
      nWin32VersionValue = (LONG)(lpNtHdr->u32.OptionalHeader.Win32VersionValue);
      nSizeOfImage = (LONG)(lpNtHdr->u32.OptionalHeader.SizeOfImage);
      nSizeOfHeaders = (LONG)(lpNtHdr->u32.OptionalHeader.SizeOfHeaders);
      nCheckSum = (LONG)(lpNtHdr->u32.OptionalHeader.CheckSum);
      nSubsystem = (short)(lpNtHdr->u32.OptionalHeader.Subsystem);
      nDllCharacteristics = (short)(lpNtHdr->u32.OptionalHeader.DllCharacteristics);
      nSizeOfStackReserve = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.SizeOfStackReserve);
      nSizeOfStackCommit = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.SizeOfStackCommit);
      nSizeOfHeapReserve = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.SizeOfHeapReserve);
      nSizeOfHeapCommit = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.SizeOfHeapCommit);
      nLoaderFlags = (LONG)(lpNtHdr->u32.OptionalHeader.LoaderFlags);
      nNumberOfRvaAndSizes = (LONG)(lpNtHdr->u32.OptionalHeader.NumberOfRvaAndSizes);
      for (i=0; i<16; i++)
      {
        aDataDirectory[i].nStart = (my_ssize_t)(my_size_t)(lpNtHdr->u32.OptionalHeader.DataDirectory[i].
                                                           VirtualAddress);
        aDataDirectory[i].nEnd = aDataDirectory[i].nStart + (my_ssize_t)(my_size_t)
                                                    (lpNtHdr->u32.OptionalHeader.DataDirectory[i].Size);
      }
      return S_OK;

#ifdef _M_X64
    case 64:
      nMagic = (short)(lpNtHdr->u64.OptionalHeader.Magic);
      nMajorLinkerVersion = (short)(lpNtHdr->u64.OptionalHeader.MajorLinkerVersion);
      nMinorLinkerVersion = (short)(lpNtHdr->u64.OptionalHeader.MinorLinkerVersion);
      nSizeOfCode = (LONG)(lpNtHdr->u64.OptionalHeader.SizeOfCode);
      nSizeOfInitializedData = (LONG)(lpNtHdr->u64.OptionalHeader.SizeOfInitializedData);
      nSizeOfUninitializedData = (LONG)(lpNtHdr->u64.OptionalHeader.SizeOfUninitializedData);
      nAddressOfEntryPoint = (LONG)(lpNtHdr->u64.OptionalHeader.AddressOfEntryPoint);
      nBaseOfCode = (LONG)(lpNtHdr->u64.OptionalHeader.BaseOfCode);
      nImageBase = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.ImageBase);
      nSectionAlignment = (LONG)(lpNtHdr->u64.OptionalHeader.SectionAlignment);
      nFileAlignment = (LONG)(lpNtHdr->u64.OptionalHeader.FileAlignment);
      nMajorOperatingSystemVersion = (short)(lpNtHdr->u64.OptionalHeader.MajorOperatingSystemVersion);
      nMinorOperatingSystemVersion = (short)(lpNtHdr->u64.OptionalHeader.MinorOperatingSystemVersion);
      nMajorImageVersion = (short)(lpNtHdr->u64.OptionalHeader.MajorImageVersion);
      nMinorImageVersion = (short)(lpNtHdr->u64.OptionalHeader.MinorImageVersion);
      nMajorSubsystemVersion = (short)(lpNtHdr->u64.OptionalHeader.MajorSubsystemVersion);
      nMinorSubsystemVersion = (short)(lpNtHdr->u64.OptionalHeader.MinorSubsystemVersion);
      nWin32VersionValue = (LONG)(lpNtHdr->u64.OptionalHeader.Win32VersionValue);
      nSizeOfImage = (LONG)(lpNtHdr->u64.OptionalHeader.SizeOfImage);
      nSizeOfHeaders = (LONG)(lpNtHdr->u64.OptionalHeader.SizeOfHeaders);
      nCheckSum = (LONG)(lpNtHdr->u64.OptionalHeader.CheckSum);
      nSubsystem = (short)(lpNtHdr->u64.OptionalHeader.Subsystem);
      nDllCharacteristics = (short)(lpNtHdr->u64.OptionalHeader.DllCharacteristics);
      nSizeOfStackReserve = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.SizeOfStackReserve);
      nSizeOfStackCommit = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.SizeOfStackCommit);
      nSizeOfHeapReserve = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.SizeOfHeapReserve);
      nSizeOfHeapCommit = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.SizeOfHeapCommit);
      nLoaderFlags = (LONG)(lpNtHdr->u64.OptionalHeader.LoaderFlags);
      nNumberOfRvaAndSizes = (LONG)(lpNtHdr->u64.OptionalHeader.NumberOfRvaAndSizes);
      for (i=0; i<16; i++)
      {
        aDataDirectory[i].nStart = (my_ssize_t)(lpNtHdr->u64.OptionalHeader.DataDirectory[i].VirtualAddress);
        aDataDirectory[i].nEnd = aDataDirectory[i].nStart +
                                 (my_ssize_t)(lpNtHdr->u64.OptionalHeader.DataDirectory[i].Size);
      }
      return S_OK;
#endif //_M_X64
  }
  return E_FAIL;
}
