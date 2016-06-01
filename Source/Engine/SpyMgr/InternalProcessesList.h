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

#include "..\..\Common\EngBaseObj.h"
#include "..\..\Common\ProcessesEnum.h"
#include "..\..\Common\ShutdownMutex.h"
#include "..\..\Common\LinkedList.h"
class CNktDvInternalProcessesListCallback;

//-----------------------------------------------------------

class CNktDvInternalProcessesList : protected CNktThread, private CNktFastMutex
{
public:
  CNktDvInternalProcessesList(__in CNktDvInternalProcessesListCallback *lpCallback);
  virtual ~CNktDvInternalProcessesList();

  virtual HRESULT Initialize();
  virtual VOID Finalize();

  virtual HRESULT Refresh(__in BOOL bNotify);
  virtual HRESULT GetEnumerator(__out CNktDvProcessesEnumerator **lplpEnum);

private:
  class CProcessItem : public CNktMemMgrObj, public TNktLnkLstNode<CProcessItem>
  {
  public:
    CProcessItem() : CNktMemMgrObj(), TNktLnkLstNode<CProcessItem>()
      {
      bRemoveMark = FALSE;
      return;
      };
    virtual ~CProcessItem()
      {
      RemoveNode();
      return;
      };

  public:
    TNktComPtr<CNktDvProcess> cProc;
    BOOL bRemoveMark;
  };

  virtual VOID ThreadProc();
  virtual HRESULT AddItem(__inout TNktLnkLst<CProcessItem> &cNewProcList, __in LPPROCESSENTRY32W lpEntryW);
  virtual VOID ExecNotification(__inout TNktLnkLst<CProcessItem> &cAddedProcList,
                                __inout TNktLnkLst<CProcessItem> &cRemovedProcList);

  TNktLnkLst<CProcessItem> cProcessList;
  //----
  CNktDvLightShutdownMutex cShutdownMtx;
  CNktDvInternalProcessesListCallback *lpCallback;
};

//-----------------------------------------------------------

class CNktDvInternalProcessesListCallback
{
public:
  virtual VOID IPL_OnProcessAdded(__in CNktDvProcess *lpProc)=0;
  virtual VOID IPL_OnProcessRemoved(__in CNktDvProcess *lpProc)=0;
  virtual VOID IPL_WorkerThreadStarted()=0;
  virtual VOID IPL_WorkerThreadEnded()=0;
};

//-----------------------------------------------------------

