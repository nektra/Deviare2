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

#ifndef _NKT_DV_THREADSUSPEND_H
#define _NKT_DV_THREADSUSPEND_H

#include "..\..\Common\EngBaseObj.h"

//-----------------------------------------------------------

class CNktDvThreadSuspend : public CNktMemMgrObj
{
public:
  typedef struct {
    SIZE_T nStart, nEnd;
  } IP_RANGE;

  CNktDvThreadSuspend();
  virtual ~CNktDvThreadSuspend();

  virtual HRESULT SuspendAll(__in IP_RANGE *lpRanges, __in SIZE_T nRangesCount);
  virtual VOID ResumeAll();

  virtual BOOL CheckIfThreadIsInRange(__in SIZE_T nStart, __in SIZE_T nEnd);

  virtual HRESULT GetProcessThreads(__out LPDWORD *lplpdwTids, __out SIZE_T *lpnTidsCount);

private:
  typedef struct {
    DWORD dwTid;
    HANDLE hThread;
    SIZE_T nCurrIP;
  } THREAD_ITEM, *LPTHREAD_ITEM;

  virtual HRESULT EnumProcessThreads(__out SIZE_T *lpnEnumMethod, __out LPDWORD lpdwSessionId,
                                     __out LPDWORD *lplpdwTids, __out SIZE_T *lpnTidsCount);
  virtual BOOL GrowCheckProcessThreadsMem();
  virtual HRESULT CheckProcessThreads(__in SIZE_T nEnumMethod, __in DWORD dwSessionId);

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

  //----

  static BOOL GetCurrentProcessSessionId(__out LPDWORD lpdwSessionId);
};

//-----------------------------------------------------------

#endif //_NKT_DV_THREADSUSPEND_H
