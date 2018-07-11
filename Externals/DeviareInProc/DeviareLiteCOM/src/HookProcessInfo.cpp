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
#include "HookProcessInfo.h"

#pragma warning(disable: 4302 4311)

//-----------------------------------------------------------

STDMETHODIMP CNktHookProcessInfoImpl::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] = { &IID_INktHookProcessInfo, NULL };
  SIZE_T i;

  for (i=0; arr[i]!=NULL; i++)
  {
    if (InlineIsEqualGUID(*arr[i], riid))
      return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP CNktHookProcessInfoImpl::get_ProcessHandle(__out my_ssize_t *procHandle)
{
  if (procHandle == NULL)
    return E_POINTER;
  *procHandle = (my_ssize_t)(sProcInfo.hProcess);
  return S_OK;
}

STDMETHODIMP CNktHookProcessInfoImpl::get_ThreadHandle(__out my_ssize_t *threadHandle)
{
  if (threadHandle == NULL)
    return E_POINTER;
  *threadHandle = (my_ssize_t)(sProcInfo.hThread);
  return S_OK;
}

STDMETHODIMP CNktHookProcessInfoImpl::get_ProcessId(__out LONG *pid)
{
  if (pid == NULL)
    return E_POINTER;
  *pid = (LONG)(sProcInfo.dwProcessId);
  return S_OK;
}

STDMETHODIMP CNktHookProcessInfoImpl::get_ThreadId(__out LONG *tid)
{
  if (tid == NULL)
    return E_POINTER;
  *tid = (LONG)(sProcInfo.dwThreadId);
  return S_OK;
}
