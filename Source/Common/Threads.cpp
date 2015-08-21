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

#include "Threads.h"
#include <process.h>

//-----------------------------------------------------------

#define THREAD_INTERNAL_FLAG_BEINGRELEASED        0x00000001
#ifndef DWORD_MAX
  #define DWORD_MAX 0xFFFFFFFFUL
#endif //!DWORD_MAX
#define X_COWAIT_DISPATCH_CALLS            8
#define X_COWAIT_DISPATCH_WINDOW_MESSAGES  0x10

//-----------------------------------------------------------

typedef struct {
  CNktThread *obj;
  LONG volatile *lpnThreadStartedOK;
  LONG volatile *lpnThreadHold;
  HANDLE hThreadEndedOK;
  LPDWORD lpdwInternalFlags;
} MXTHREAD_PARAM;

#define MS_VC_EXCEPTION                           0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

//-----------------------------------------------------------

static unsigned int __stdcall CommonThreadProc(__inout LPVOID lpParameter);

//-----------------------------------------------------------

CNktThread::CNktThread() : CNktMemMgrObj()
{
  nPriority = THREAD_PRIORITY_NORMAL;
  hThread = NULL;
  dwThreadId = 0;
  dwInternalFlags = 0;
  return;
}

CNktThread::~CNktThread()
{
  //this assertion may hit if app exits because it kills all threads except the main
  //one and IsRunning does only a quick test. just ignore in that case
  //NKT_ASSERT(IsRunning() == FALSE);
  return;
}

BOOL CNktThread::Start(__in BOOL bSuspended)
{
  MXTHREAD_PARAM sParam;
  LONG volatile nThreadStartedOK;
  unsigned int tid;

  if (hThread != NULL)
  {
    if (cThreadEndedOK.Wait(0) == FALSE)
      return TRUE;
    Stop(0);
  }
  if (cKillEvent.Create(TRUE, FALSE) == FALSE)
    return FALSE;
  if (cThreadEndedOK.Create(TRUE, FALSE) == FALSE)
  {
    cKillEvent.Destroy();
    return FALSE;
  }
  dwInternalFlags &= ~THREAD_INTERNAL_FLAG_BEINGRELEASED;
  nThreadStartedOK = 0;
  sParam.obj = this;
  sParam.hThreadEndedOK = cThreadEndedOK.GetEventHandle();
  sParam.lpnThreadStartedOK = &nThreadStartedOK;
  sParam.lpnThreadHold = NULL;
  sParam.lpdwInternalFlags = &dwInternalFlags;
  hThread = (HANDLE)_beginthreadex(NULL, 0, &CommonThreadProc, &sParam, 0, &tid);
  if (hThread == NULL)
  {
    cThreadEndedOK.Destroy();
    cKillEvent.Destroy();
    return FALSE;
  }
  dwThreadId = (DWORD)tid;
  ::SetThreadPriority(hThread, nPriority);
  //wait for thread start
  while (sParam.lpnThreadHold == NULL)
    ::Sleep(1);
  if (bSuspended != FALSE)
  {
    if (::SuspendThread(hThread) == (DWORD)-1)
    {
      ::TerminateThread(hThread, 0);
      hThread = NULL;
      cThreadEndedOK.Destroy();
      cKillEvent.Destroy();
      return FALSE;
    }
  }
  //allow continue
  NktInterlockedExchange(sParam.lpnThreadHold, 1);
  return TRUE;
}

BOOL CNktThread::Stop(__in DWORD dwTimeout)
{
  HANDLE hEvents[2];
  SIZE_T dwRetCode;
  DWORD dwExitCode;

  if (hThread != NULL)
  {
    if (dwThreadId == ::GetCurrentThreadId())
      return StopAsync();
    if ((dwInternalFlags & THREAD_INTERNAL_FLAG_BEINGRELEASED) == 0)
    {
      if (cThreadEndedOK.Wait(0) == FALSE)
      {
        cKillEvent.Set();
        hEvents[0] = hThread;
        hEvents[1] = cThreadEndedOK.GetEventHandle();
        for (;;)
        {
          //dwRetCode = WaitForMultipleObjects(2, hEvents, FALSE, (nTimeout > 100) ? 100 : dwTimeout);
          dwRetCode = ::WaitForSingleObject(hThread, (dwTimeout > 100) ? 100 : dwTimeout);
          //dwRetCode = CoWaitAndDispatchMessages((dwTimeout > 100) ? 100 : dwTimeout, 1, &hThread);
          if (dwRetCode == WAIT_TIMEOUT)
          {
            ::GetExitCodeThread(hThread, &dwExitCode);
            if (dwExitCode != STILL_ACTIVE)
              dwRetCode = WAIT_OBJECT_0;
          }
          if (dwRetCode != WAIT_TIMEOUT)
            break;
          if (dwTimeout != INFINITE)
          {
            if (dwTimeout > 100)
              dwTimeout -= 100;
            else
              break;
          }
        }
      }
      else
      {
        //if i'm here, it may be that C runtime is still freeing some stuff
        //so i wait 2 seconds to give him a chance to free all the stuff
        dwRetCode = ::WaitForSingleObject(hThread, 2000);
      }
      if (dwRetCode != WAIT_OBJECT_0 && dwRetCode != (WAIT_OBJECT_0+1))
        ::TerminateThread(hThread, 0);
    }
    ::CloseHandle(hThread);
    hThread = NULL;
  }
  dwThreadId = 0;
  cThreadEndedOK.Destroy();
  cKillEvent.Destroy();
  dwInternalFlags &= ~THREAD_INTERNAL_FLAG_BEINGRELEASED;
  return TRUE;
}

BOOL CNktThread::StopAsync()
{
  if (hThread != NULL)
  {
    if (cThreadEndedOK.Wait(0) == FALSE)
      cKillEvent.Set();
  }
  return TRUE;
}

BOOL CNktThread::Pause()
{
  if (hThread == NULL)
    return FALSE;
  if (::SuspendThread(hThread) == (DWORD)-1)
    return FALSE;
  return TRUE;
}

BOOL CNktThread::Resume()
{
  if (hThread == NULL)
    return FALSE;
  if (::ResumeThread(hThread) == (DWORD)-1)
    return FALSE;
  return TRUE;
}

BOOL CNktThread::Wait(__in DWORD dwTimeout, __in DWORD dwEventCount, __in LPHANDLE lphEventList,
                      __out LPDWORD lpdwHitEvent)
{
  HANDLE hEvents[50];
  DWORD i, dwRetCode;

  if (dwEventCount > 48)
    return FALSE;
  if (dwEventCount>0 && lphEventList==NULL)
    return FALSE;
  if (hThread == NULL)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  hEvents[0] = hThread;
  hEvents[1] = cThreadEndedOK.GetEventHandle();
  for (i=0; i<dwEventCount; i++)
    hEvents[2+i] = lphEventList[i];
  dwRetCode = ::WaitForMultipleObjects(2+dwEventCount, hEvents, FALSE, dwTimeout);
  if (dwRetCode == WAIT_FAILED)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = DWORD_MAX;
    return TRUE;
  }
  if (dwRetCode==WAIT_OBJECT_0 || dwRetCode==(WAIT_OBJECT_0+1))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  if (dwRetCode>=WAIT_OBJECT_0+2 && dwRetCode<=(WAIT_OBJECT_0+1+dwEventCount))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = dwRetCode-(WAIT_OBJECT_0+1);
    return TRUE;
  }
  if (lpdwHitEvent != NULL)
    *lpdwHitEvent = 0;
  return FALSE;
}

BOOL CNktThread::MsgWait(__in DWORD dwTimeout, __in DWORD dwEventCount, __in LPHANDLE lphEventList,
                         __out LPDWORD lpdwHitEvent)
{
  HANDLE hEvents[50];
  DWORD i, dwRetCode;

  if (dwEventCount > 48)
    return FALSE;
  if (dwEventCount>0 && lphEventList==NULL)
    return FALSE;
  if (hThread == NULL)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  hEvents[0] = hThread;
  hEvents[1] = cThreadEndedOK.GetEventHandle();
  for (i=0; i<dwEventCount; i++)
    hEvents[2+i] = lphEventList[i];
  dwRetCode = MsgWaitAndDispatchMessages(dwTimeout, 2+dwEventCount, hEvents);
  if (dwRetCode == WAIT_FAILED)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = DWORD_MAX;
    return TRUE;
  }
  if (dwRetCode==WAIT_OBJECT_0 || dwRetCode==(WAIT_OBJECT_0+1))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  if (dwRetCode>=WAIT_OBJECT_0+2 && dwRetCode<=(WAIT_OBJECT_0+1+dwEventCount))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = dwRetCode-(WAIT_OBJECT_0+1);
    return TRUE;
  }
  if (lpdwHitEvent != NULL)
    *lpdwHitEvent = 0;
  return FALSE;
}

BOOL CNktThread::CoWait(__in DWORD dwTimeout, __in DWORD dwEventCount, __in LPHANDLE lphEventList,
                        __out LPDWORD lpdwHitEvent)
{
  HANDLE hEvents[50];
  DWORD i, dwRetCode;

  if (dwEventCount > 48)
    return FALSE;
  if (dwEventCount>0 && lphEventList==NULL)
    return FALSE;
  if (hThread == NULL)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  hEvents[0] = hThread;
  hEvents[1] = cThreadEndedOK.GetEventHandle();
  for (i=0; i<dwEventCount; i++)
    hEvents[2+i] = lphEventList[i];
  dwRetCode = CoWaitAndDispatchMessages(dwTimeout, 2+dwEventCount, hEvents);
  if (dwRetCode == WAIT_FAILED)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = DWORD_MAX;
    return TRUE;
  }
  if (dwRetCode==WAIT_OBJECT_0 || dwRetCode==(WAIT_OBJECT_0+1))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  if (dwRetCode>=WAIT_OBJECT_0+2 && dwRetCode<=(WAIT_OBJECT_0+1+dwEventCount))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = dwRetCode-(WAIT_OBJECT_0+1);
    return TRUE;
  }
  if (lpdwHitEvent != NULL)
    *lpdwHitEvent = 0;
  return FALSE;
}

DWORD CNktThread::MsgWaitAndDispatchMessages(__in DWORD dwTimeout, __in DWORD dwEventsCount, __in LPHANDLE lpHandles)
{
  MSG sMsg;
  DWORD dwRetCode;
  BOOL bUnicode, bRet;
  struct {
    BOOL bHasQuit;
    WPARAM wParam;
  } sQuitMsg;

  memset(&sQuitMsg, 0, sizeof(sQuitMsg));
  while (1)
  {
    dwRetCode = ::MsgWaitForMultipleObjectsEx(dwEventsCount, lpHandles, dwTimeout, QS_ALLINPUT|QS_ALLPOSTMESSAGE,
                                              MWMO_INPUTAVAILABLE|MWMO_ALERTABLE);
    if (dwRetCode == WAIT_IO_COMPLETION)
      continue;
    if (dwRetCode != WAIT_OBJECT_0+dwEventsCount)
      break;
    bRet = FALSE;
    while (bRet == FALSE && ::PeekMessageW(&sMsg, NULL, 0, 0, PM_NOREMOVE) != FALSE)
    {
      bUnicode = ::IsWindowUnicode(sMsg.hwnd);
      if (bUnicode != FALSE)
        bRet = ::GetMessageW(&sMsg, NULL, 0, 0);
      else
        bRet = ::GetMessageA(&sMsg, NULL, 0, 0);
      if (bRet > 0)
      {
        if (sMsg.message == WM_QUIT)
        {
          sQuitMsg.bHasQuit = TRUE;
          sQuitMsg.wParam = sMsg.wParam;
        }
        else
        {
          if (bUnicode != FALSE)
            bRet = ::IsDialogMessageW(sMsg.hwnd, &sMsg);
          else
            bRet = ::IsDialogMessageA(sMsg.hwnd, &sMsg);
          if (bRet == FALSE)
          {
            ::TranslateMessage(&sMsg);
            if (bUnicode != FALSE)
              ::DispatchMessageW(&sMsg);
            else
              ::DispatchMessageA(&sMsg);
          }
        }
      }
      dwRetCode = ::WaitForMultipleObjects(dwEventsCount, lpHandles, FALSE, 0);
      bRet = (dwRetCode>=WAIT_OBJECT_0 && dwRetCode<WAIT_OBJECT_0+dwEventsCount) ? TRUE : FALSE;
    }
    if (bRet != FALSE)
      break;
  }
  if (sQuitMsg.bHasQuit != FALSE)
  {
    ::PostMessageW(NULL, WM_NULL, 0, 0);
    ::PostQuitMessage((int)sQuitMsg.wParam);
  }
  return dwRetCode;
}

DWORD CNktThread::CoWaitAndDispatchMessages(__in DWORD dwTimeout, __in DWORD dwEventsCount, __in LPHANDLE lpHandles)
{
  HRESULT hRes;
  DWORD dwIndex;

restart:
  hRes = ::CoWaitForMultipleHandles(COWAIT_ALERTABLE/*|X_COWAIT_DISPATCH_CALLS|X_COWAIT_DISPATCH_WINDOW_MESSAGES*/,
                                    dwTimeout, (ULONG)dwEventsCount, lpHandles, &dwIndex);
  if (hRes == RPC_S_CALLPENDING)
  {
    ::SetLastError(WAIT_TIMEOUT);
    return WAIT_TIMEOUT;
  }
  if (hRes == S_OK)
  {
    if (dwIndex == WAIT_IO_COMPLETION)
      goto restart;
    return dwIndex;
  }
  ::PostMessageW(NULL, WM_NULL, 0, 0);
  ::SetLastError((DWORD)hRes & 0x0000FFFF);
  return WAIT_FAILED;
}

BOOL CNktThread::IsRunning()
{
  if (hThread == NULL)
    return FALSE;
  if (cThreadEndedOK.Wait(0) != FALSE)
    return FALSE;
  return TRUE;
}

BOOL CNktThread::CheckForAbort(__in DWORD dwTimeout, __in DWORD dwEventCount,
                               __out_opt LPHANDLE lphEventList, __out_opt LPDWORD lpdwHitEvent)
{
  HANDLE hEvents[51];
  DWORD i, dwRetCode;

  if (hThread == NULL)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return FALSE;
  }
  if (dwEventCount > 48)
    return FALSE;
  if (dwEventCount > 0 && lphEventList == NULL)
    return FALSE;
  hEvents[0] = hThread;
  hEvents[1] = cThreadEndedOK.GetEventHandle();
  hEvents[2] = cKillEvent.GetEventHandle();
  for (i=0; i<dwEventCount; i++)
    hEvents[3+i] = lphEventList[i];
  dwRetCode = ::WaitForMultipleObjects(3+dwEventCount, hEvents, FALSE, dwTimeout);
  if (dwRetCode == WAIT_FAILED)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = DWORD_MAX;
    return TRUE;
  }
  if (dwRetCode >= WAIT_OBJECT_0 && dwRetCode <= (WAIT_OBJECT_0+2))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  if (dwRetCode >= WAIT_OBJECT_0+3 && dwRetCode <= (WAIT_OBJECT_0+2+dwEventCount))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = dwRetCode-(WAIT_OBJECT_0+2);
  }
  else
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
  }
  return FALSE;
}

BOOL CNktThread::MsgCheckForAbort(__in DWORD dwTimeout, __in DWORD dwEventCount,
                                  __out_opt LPHANDLE lphEventList, __out_opt LPDWORD lpdwHitEvent)
{
  HANDLE hEvents[51];
  DWORD i, dwRetCode;

  if (hThread == NULL)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return FALSE;
  }
  if (dwEventCount > 48)
    return FALSE;
  if (dwEventCount > 0 && lphEventList == NULL)
    return FALSE;
  hEvents[0] = hThread;
  hEvents[1] = cThreadEndedOK.GetEventHandle();
  hEvents[2] = cKillEvent.GetEventHandle();
  for (i=0; i<dwEventCount; i++)
    hEvents[3+i] = lphEventList[i];
  dwRetCode = ::MsgWaitForMultipleObjects(3+dwEventCount, hEvents, FALSE, dwTimeout, QS_ALLINPUT);
  if (dwRetCode == WAIT_FAILED)
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = DWORD_MAX;
    return TRUE;
  }
  if (dwRetCode >= WAIT_OBJECT_0 && dwRetCode <= (WAIT_OBJECT_0+2))
  {
    if (lpdwHitEvent != NULL)
      *lpdwHitEvent = 0;
    return TRUE;
  }
  if (lpdwHitEvent != NULL)
  {
    if (dwRetCode >= WAIT_OBJECT_0+3 && dwRetCode <= (WAIT_OBJECT_0+2+dwEventCount))
      *lpdwHitEvent = dwRetCode-(WAIT_OBJECT_0+2);
    else
      *lpdwHitEvent = 0;
  }
  return FALSE;
}

VOID CNktThread::SetThreadName(__in DWORD dwThreadId, __in_z_opt LPCSTR szName)
{
  //NOTE: Temporary commented because registers are not saved correctly when called inside a C#/XAML application
  /*
  THREADNAME_INFO sInfo;

  ::Sleep(10);
  sInfo.dwType = 0x1000;
  sInfo.szName = (szName != NULL) ? szName : "";
  sInfo.dwThreadID = (DWORD)dwThreadId;
  sInfo.dwFlags = 0;
#pragma warning(disable: 6312 6322)
  __try
  {
    ::RaiseException(MS_VC_EXCEPTION, 0, sizeof(sInfo)/sizeof(DWORD), (ULONG_PTR*)&sInfo);
  }
  __except (EXCEPTION_CONTINUE_EXECUTION)
  { }
#pragma warning(default: 6312 6322)
  */
  return;
}

VOID CNktThread::SetThreadName(__in_z_opt LPCSTR szName)
{
  if (hThread != NULL)
    SetThreadName(dwThreadId, szName);
  return;
}

BOOL CNktThread::SetPriority(__in int _nPriority)
{
  if (_nPriority != THREAD_PRIORITY_TIME_CRITICAL &&
      _nPriority != THREAD_PRIORITY_HIGHEST &&
      _nPriority != THREAD_PRIORITY_ABOVE_NORMAL &&
      _nPriority != THREAD_PRIORITY_NORMAL &&
      _nPriority != THREAD_PRIORITY_BELOW_NORMAL &&
      _nPriority != THREAD_PRIORITY_LOWEST &&
      _nPriority != THREAD_PRIORITY_IDLE)
    return FALSE;
  if (hThread != NULL)
  {
    if (::SetThreadPriority(hThread, _nPriority) == FALSE)
      return FALSE;
  }
  nPriority = _nPriority;
  return TRUE;
}

int CNktThread::GetPriority()
{
  return nPriority;
}

//-----------------------------------------------------------

CNktWorkerThread::CNktWorkerThread(__in LPNKT_WRKTHREAD_ROUTINE _lpStartRoutine, __in LPVOID _lpParam) :
                                   CNktThread()
{
  lpStartRoutine = _lpStartRoutine;
  lpParam = _lpParam;
  return;
}

CNktWorkerThread::~CNktWorkerThread()
{
  return;
}

BOOL CNktWorkerThread::SetRoutine(__in LPNKT_WRKTHREAD_ROUTINE _lpStartRoutine, __in LPVOID _lpParam)
{
  lpStartRoutine = _lpStartRoutine;
  lpParam = _lpParam;
  return TRUE;
}

VOID CNktWorkerThread::ThreadProc()
{
  NKT_ASSERT(lpStartRoutine != NULL);
  lpStartRoutine(this, lpParam);
  return;
}

//-----------------------------------------------------------

static unsigned int __stdcall CommonThreadProc(__inout LPVOID lpParameter)
{
  HANDLE hThreadEndedOK;
  CNktThread *lpObj;
  LONG volatile nThreadHold;

  hThreadEndedOK = ((MXTHREAD_PARAM*)lpParameter)->hThreadEndedOK;
  lpObj = ((MXTHREAD_PARAM*)lpParameter)->obj;
  nThreadHold = 0;
  ((MXTHREAD_PARAM*)lpParameter)->lpnThreadHold = &nThreadHold;
  //hold
  while (nThreadHold == 0)
    ::Sleep(1);
  //run thread proc
  lpObj->ThreadProc();
  //signal thread end
  ::SetEvent(hThreadEndedOK);
  nktMemMgrOnThreadExit();
  return 0;
}
