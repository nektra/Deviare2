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

#ifndef _NKT_THREADSUSPEND_H
#define _NKT_THREADSUSPEND_H

#include <NktHookLib.h>

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

class CNktThreadSuspend
{
public:
  typedef struct {
    SIZE_T nStart, nEnd;
  } IP_RANGE;

  CNktThreadSuspend();
  ~CNktThreadSuspend();

  DWORD SuspendAll(__in DWORD dwPid, __in IP_RANGE *lpRanges, __in SIZE_T nRangesCount);
  VOID ResumeAll();

  BOOL CheckIfThreadIsInRange(__in SIZE_T nStart, __in SIZE_T nEnd);

  class CAutoResume
  {
  public:
    CAutoResume(__in CNktThreadSuspend *_lpThreadSuspend)
      {
      lpThreadSuspend = _lpThreadSuspend;
      return;
      };

    ~CAutoResume()
      {
      lpThreadSuspend->ResumeAll();
      return;
      };

  private:
    CNktThreadSuspend *lpThreadSuspend;
  };

private:
  typedef struct {
    DWORD dwTid;
    HANDLE hThread;
    SIZE_T nCurrIP;
  } THREAD_ITEM, *LPTHREAD_ITEM;

  DWORD EnumProcessThreads(__in DWORD dwPid, __in HANDLE hProcess, __out SIZE_T *lpnEnumMethod,
                           __out LPDWORD lpdwSessionId);
  BOOL GrowCheckProcessThreadsMem();
  DWORD CheckProcessThreads(__in DWORD dwPid, __in SIZE_T nEnumMethod, __in DWORD dwSessionId);
  BOOL GetProcessSessionId(__in HANDLE hProcess, __out LPDWORD lpdwSessionId);

  DWORD IsCurrentProcessLowIntegrity(__out BOOL *lpbProcessIsLow);

private:
  struct {
    LPTHREAD_ITEM lpList;
    SIZE_T nCount;
  } sSuspendedTids;
  //----
  struct {
    LPBYTE lpMem;
    SIZE_T nSize;
  } sCheckThreads;
};

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_THREADSUSPEND_H
