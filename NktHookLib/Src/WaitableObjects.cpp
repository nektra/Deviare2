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

#include "WaitableObjects.h"

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

static LONG volatile nProcessorsCount = 0;

//-----------------------------------------------------------

BOOL NktIsMultiProcessor()
{
  if (nProcessorsCount == 0)
  {
    SYSTEM_BASIC_INFORMATION sBasicInfo;
    LONG nTemp;
    NTSTATUS nNtStatus;

    nNtStatus = NktRtlGetNativeSystemInformation(SystemBasicInformation, &sBasicInfo, sizeof(sBasicInfo), NULL);
    if (nNtStatus == STATUS_NOT_IMPLEMENTED)
      nNtStatus = NktNtQuerySystemInformation(SystemBasicInformation, &sBasicInfo, sizeof(sBasicInfo), NULL);
    nTemp = 1;
    if (NT_SUCCESS(nNtStatus) && sBasicInfo.NumberOfProcessors > 1)
      nTemp = (LONG)(sBasicInfo.NumberOfProcessors);
    _InterlockedExchange(&nProcessorsCount, nTemp);
  }
  return (nProcessorsCount > 1) ? TRUE : FALSE;
}

VOID NktYieldProcessor()
{
  if (NktIsMultiProcessor() == FALSE)
  {
    NktNtYieldExecution();
  }
  else
  {
    LARGE_INTEGER sLI;

    sLI.QuadPart = -10000;
    NktNtDelayExecution(FALSE, &sLI);
  }
  return;
}

//-----------------------------------------------------------

CNktEvent::CNktEvent() : CNktNtHeapBaseObj()
{
  hEvent = NULL;
  return;
}

CNktEvent::~CNktEvent()
{
  Destroy();
  return;
}

BOOL CNktEvent::Create(__in BOOL bManualReset, __in BOOL bInitialState, __in_z_opt LPCWSTR szName)
{
  OBJECT_ATTRIBUTES sObjAttr;
  UNICODE_STRING sName;
  NTSTATUS nNtStatus;

  Destroy();
  if (szName != NULL)
  {
    sName.Buffer = (PWSTR)szName;
    for (sName.Length=0; sName.Length<32767 && szName[sName.Length]!=0; sName.Length++);
    sName.Length <<= 1;
    sName.MaximumLength = sName.Length;
  }
  InitializeObjectAttributes(&sObjAttr, (szName != NULL) ? &sName : NULL, 0, NULL, NULL);
  nNtStatus = NktNtCreateEvent(&hEvent, EVENT_ALL_ACCESS, &sObjAttr, (bManualReset != FALSE) ? 0 : 1, bInitialState);
  if (!NT_SUCCESS(nNtStatus))
  {
    hEvent = NULL;
    return FALSE;
  }
  return TRUE;
}

BOOL CNktEvent::Open(__in_z_opt LPCWSTR szName)
{
  OBJECT_ATTRIBUTES sObjAttr;
  UNICODE_STRING sName;
  NTSTATUS nNtStatus;

  Destroy();
  if (szName != NULL)
  {
    sName.Buffer = (PWSTR)szName;
    for (sName.Length=0; sName.Length<32767 && szName[sName.Length]!=0; sName.Length++);
    sName.Length <<= 1;
    sName.MaximumLength = sName.Length;
  }
  InitializeObjectAttributes(&sObjAttr, (szName != NULL) ? &sName : NULL, 0, NULL, NULL);
  nNtStatus = NktNtOpenEvent(&hEvent, EVENT_ALL_ACCESS, &sObjAttr);
  if (!NT_SUCCESS(nNtStatus))
  {
    hEvent = NULL;
    return FALSE;
  }
  return TRUE;
}

VOID CNktEvent::Destroy()
{
  if (hEvent != NULL)
  {
    NktNtClose(hEvent);
    hEvent = NULL;
  }
  return;
}

BOOL CNktEvent::Wait(__in DWORD dwTimeout)
{
  NTSTATUS nNtStatus;
  LARGE_INTEGER sLI, *lpLI;

  if (hEvent == NULL)
    return TRUE;
  if (dwTimeout != INFINITE)
  {
    sLI.QuadPart = UInt32x32To64(dwTimeout, -10000);
    lpLI = &sLI;
  }
  else
  {
    lpLI = NULL;
  }
  nNtStatus = NktNtWaitForSingleObject(hEvent, FALSE, lpLI);
  return (nNtStatus == WAIT_OBJECT_0) ? TRUE : FALSE;
}

BOOL CNktEvent::Reset()
{
  NTSTATUS nNtStatus;

  if (hEvent == NULL)
    return FALSE;
  nNtStatus = NktNtResetEvent(hEvent, NULL);
  return (NT_SUCCESS(nNtStatus)) ? TRUE : FALSE;
}

BOOL CNktEvent::Set()
{
  NTSTATUS nNtStatus;

  if (hEvent == NULL)
    return FALSE;
  nNtStatus = NktNtSetEvent(hEvent, NULL);
  return (NT_SUCCESS(nNtStatus)) ? TRUE : FALSE;
}

VOID CNktEvent::Attach(__in HANDLE _hEvent)
{
  Destroy();
  hEvent = _hEvent;
  return;
}

HANDLE CNktEvent::Detach()
{
  HANDLE hTempEvent = hEvent;
  hEvent = NULL;
  return hTempEvent;
}

//-----------------------------------------------------------

} //Internals
} //NktHookLib
