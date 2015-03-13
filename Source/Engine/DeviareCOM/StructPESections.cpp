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
#include "StructPESections.h"

//-----------------------------------------------------------
// CNktStructPESectionsImpl

HRESULT CNktStructPESectionsImpl::get_Count(__deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  //return data
  *pVal = (LONG)(aSectionsList.GetCount());
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPESectionsImpl::get_Name(__in LONG index, __deref_out BSTR *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)(aSectionsList.GetCount()))
  {
    *pVal = NULL;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = ::SysAllocString(aSectionsList[index].szNameW);
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), ((*pVal) != NULL) ? S_OK : E_OUTOFMEMORY);
}

HRESULT CNktStructPESectionsImpl::get_StartAddress(__in LONG index, __deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)(aSectionsList.GetCount()))
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = aSectionsList[index].nStartAddress;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPESectionsImpl::get_EndAddress(__in LONG index, __deref_out my_ssize_t *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)(aSectionsList.GetCount()))
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = aSectionsList[index].nEndAddress;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPESectionsImpl::get_Characteristics(__in LONG index, __deref_out LONG *pVal)
{
  //check parameters
  if (pVal == NULL)
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_POINTER);
  if (index < 0 || index >= (LONG)(aSectionsList.GetCount()))
  {
    *pVal = 0;
    return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), E_INVALIDARG);
  }
  //return data
  *pVal = aSectionsList[index].nCharacteristics;
  return ::SetupErrorInfoAndReturn(*(_tih.m_pguid), S_OK);
}

HRESULT CNktStructPESectionsImpl::Initialize(__in CNktDvTools::PROCESSSECTION *lpSections,
                                             __in SIZE_T nSecCount)
{
  SIZE_T i, nLen;
  CNktStringW cStrTempW;

  if (nSecCount > 0)
  {
    if (aSectionsList.SetCount(nSecCount) == FALSE)
      return E_OUTOFMEMORY;
    for (i=0; i<nSecCount; i++)
    {
      for (nLen=0; nLen < 8 && lpSections[i].szName[nLen] != 0; nLen++);
      if (cStrTempW.CopyN(lpSections[i].szName, nLen) == FALSE)
        return E_OUTOFMEMORY;
      nLen = cStrTempW.GetLength();
      if (nLen > 8)
        nLen = 8;
      memcpy(aSectionsList[i].szNameW, (LPWSTR)cStrTempW, nLen*sizeof(WCHAR));
      aSectionsList[i].szNameW[nLen] = 0;
      aSectionsList[i].nStartAddress = (my_ssize_t)(lpSections[i].nSectionStart);
      aSectionsList[i].nEndAddress = (my_ssize_t)(lpSections[i].nSectionEnd);
      aSectionsList[i].nCharacteristics = (LONG)(lpSections[i].dwCharacteristics);
    }
  }
  return S_OK;
}
