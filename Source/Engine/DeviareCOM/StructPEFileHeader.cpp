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
#include "StructPEFileHeader.h"

//-----------------------------------------------------------
// CNktStructPEFileHeaderImpl

HRESULT CNktStructPEFileHeaderImpl::get_Machine(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nMachine;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_NumberOfSections(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nNumberOfSections;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_TimeDateStamp(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nTimeDateStamp;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_PointerToSymbolTable(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nPointerToSymbolTable;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_NumberOfSymbols(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nNumberOfSymbols;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_SizeOfOptionalHeader(__deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nSizeOfOptionalHeader;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::get_Characteristics(__deref_out short *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = nCharacteristics;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPEFileHeaderImpl::Initialize(__in NKT_DV_IMAGE_NT_HEADER *lpNtHdr,
                                               __in SIZE_T nPlatformBits)
{
  switch (nPlatformBits)
  {
    case 32:
      nMachine = (short)(lpNtHdr->u32.FileHeader.Machine);
      nNumberOfSections = (LONG)(lpNtHdr->u32.FileHeader.NumberOfSections);
      nTimeDateStamp = (LONG)(lpNtHdr->u32.FileHeader.TimeDateStamp);
      nPointerToSymbolTable = (LONG)(lpNtHdr->u32.FileHeader.PointerToSymbolTable);
      nNumberOfSymbols = (LONG)(lpNtHdr->u32.FileHeader.NumberOfSymbols);
      nSizeOfOptionalHeader = (my_ssize_t)(lpNtHdr->u32.FileHeader.SizeOfOptionalHeader);
      return S_OK;

#ifdef _M_X64
    case 64:
      nMachine = (short)(lpNtHdr->u64.FileHeader.Machine);
      nNumberOfSections = (LONG)(lpNtHdr->u64.FileHeader.NumberOfSections);
      nTimeDateStamp = (LONG)(lpNtHdr->u64.FileHeader.TimeDateStamp);
      nPointerToSymbolTable = (LONG)(lpNtHdr->u64.FileHeader.PointerToSymbolTable);
      nNumberOfSymbols = (LONG)(lpNtHdr->u64.FileHeader.NumberOfSymbols);
      nSizeOfOptionalHeader = (my_ssize_t)(lpNtHdr->u64.FileHeader.SizeOfOptionalHeader);
      return S_OK;
#endif //_M_X64
  }
  return E_FAIL;
}
