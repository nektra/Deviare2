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

#include "StdAfx.h"
#include "HookLib.h"
#include "HookInfo.h"

//-----------------------------------------------------------

STDMETHODIMP CNktHookInfoImpl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = { &IID_INktHookInfo, NULL };
  SIZE_T i;

  for (i=0; arr[i]!=NULL; i++)
  {
    if (InlineIsEqualGUID(*arr[i], riid))
      return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP CNktHookInfoImpl::get_Id(__out LONG *hookId)
{
  if (hookId == NULL)
    return E_POINTER;
  *hookId = (LONG)(ULONG)(sInfo.nHookId);
  return S_OK;
}

STDMETHODIMP CNktHookInfoImpl::get_OrigProcAddr(__out my_ssize_t *procAddr)
{
  if (procAddr == NULL)
    return E_POINTER;
  *procAddr = (my_ssize_t)(sInfo.lpProcToHook);
  return S_OK;
}

STDMETHODIMP CNktHookInfoImpl::put_OrigProcAddr(__in my_ssize_t procAddr)
{
  sInfo.lpProcToHook = (LPVOID)(SIZE_T)procAddr;
  return S_OK;
}

STDMETHODIMP CNktHookInfoImpl::get_NewProcAddr(__out my_ssize_t *procAddr)
{
  if (procAddr == NULL)
    return E_POINTER;
  *procAddr = (my_ssize_t)(sInfo.lpNewProcAddr);
  return S_OK;
}

STDMETHODIMP CNktHookInfoImpl::put_NewProcAddr(__in my_ssize_t procAddr)
{
  sInfo.lpNewProcAddr = (LPVOID)(SIZE_T)procAddr;
  return S_OK;
}

STDMETHODIMP CNktHookInfoImpl::get_CallOriginalAddr(__out my_ssize_t *procAddr)
{
  if (procAddr == NULL)
    return E_POINTER;
  *procAddr = (my_ssize_t)(sInfo.lpCallOriginal);
  return S_OK;
}
