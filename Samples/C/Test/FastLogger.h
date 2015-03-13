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

#ifndef _NKT_FAST_LOGGER_H
#define _NKT_FAST_LOGGER_H

//-----------------------------------------------------------

#include "EngineTest.h"
#include "WaitableObjects.h"

//-----------------------------------------------------------

class CNktDvFastLogger : private CNktFastMutex
{
public:
  CNktDvFastLogger();
  virtual ~CNktDvFastLogger();

  virtual HRESULT Initialize(__in LPCWSTR szLogFileNameW, __in BOOL bAppend=FALSE);
  virtual VOID Finalize();

  virtual HRESULT Log(__in LPCWSTR szFormatW, ...);
  virtual HRESULT LogV(__in LPCWSTR szFormatW, __in va_list argptr);

private:
  virtual VOID ThreadProc();
  virtual VOID WriteMessages(__in BOOL bFlush);

private:
  friend unsigned int __stdcall _ThreadProc(void*);

  typedef struct tagMSG_ITEM {
    struct tagMSG_ITEM *lpNext;
    DWORD dwLengthInBytes;
    WCHAR szMessageW[1];
  } MSG_ITEM, *LPMSG_ITEM;

  LPMSG_ITEM lpFirstMsg, lpLastMsg;
  CNktEvent cQuitEv, cWakeUpEv;
  HANDLE hThread, hFile;
};

//-----------------------------------------------------------

#endif //_NKT_FAST_LOGGER_H
