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

#pragma once

#include "EngBaseObj.h"

//-----------------------------------------------------------

class CNktDvProcessHandlesMgr : public CNktDvObject, private CNktFastMutex, private CNktThread
{
public:
  CNktDvProcessHandlesMgr();
  virtual ~CNktDvProcessHandlesMgr();

  static HRESULT Get(__deref_out CNktDvProcessHandlesMgr **lplpPhMgr);

  virtual ULONG __stdcall Release(); //override

  static HANDLE CreateHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess);

  virtual HANDLE GetHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess,
                           __in BOOL bInheritable=FALSE);
  virtual HRESULT AddHandle(__in DWORD dwProcessId, __in HANDLE hProc, __in DWORD dwCurrentAccess);
  virtual VOID RemoveHandle(__in DWORD dwProcessId);

private:
  typedef struct {
    DWORD dwProcId;
    DWORD dwCurrAccess;
    HANDLE hProc;
  } ITEM;

  class ITEM_Remove
  {
  public:
    virtual VOID OnItemFinalize(__inout ITEM& item)
      {
      if (item.hProc != NULL)
        ::CloseHandle(item.hProc);
      return;
      };
  };

  HRESULT Initialize();
  VOID Finalize();

  virtual HANDLE InternalGetHandle(__in DWORD dwProcessId, __in DWORD dwDesiredAccess);

  VOID ThreadProc();

  SIZE_T Find(__in DWORD dwProcessId);
  VOID RemoveInvalid();

  static int __cdecl InsertById(__in void *, __in ITEM *lpElem1, __in ITEM *lpElem2);
  static int __cdecl SearchById(__in void *lpContext, __in const void *lpKey, __in const void *elem);

  TNktArrayList4Structs<ITEM,32,ITEM_Remove> sHandlesList;
};

//-----------------------------------------------------------

