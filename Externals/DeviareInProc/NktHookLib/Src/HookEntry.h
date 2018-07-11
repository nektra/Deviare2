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

#ifndef _NKT_HOOKLIB_HOOKENTRY_H
#define _NKT_HOOKLIB_HOOKENTRY_H

#include <NktHookLib.h>
#include "LinkedList.h"
#include "ProcessEntry.h"
#include "NtHeapBaseObj.h"

namespace NktHookLib {
namespace Internals {

//-----------------------------------------------------------

#define HOOKENG_MAX_ORIGINAL_STUB_SIZE                    64
#define HOOKENG_MAX_STUB_SIZE                             64

//-----------------------------------------------------------

class CHookEntry : public TNktLnkLstNode<CHookEntry>, public CNktNtHeapBaseObj
{
public:
  CHookEntry(__in CProcessesHandles::CEntry *lpProcEntry, __in DWORD dwFlags);
  ~CHookEntry();

  LPBYTE SkipJumpInstructions(__in LPBYTE lpPtr);

  DWORD CreateStub(__in BOOL bOutputDebug);

  SIZE_T GetJumpToHookBytes()
    {
    if ((dwFlags & NKTHOOKLIB_UseAbsoluteIndirectJumps) != 0)
      return 6;
    return 5;
    };

private:
  friend class CNktHookLib;

  VOID ZeroFields();

  SIZE_T nId;
  LPBYTE lpOrigProc;
  LPBYTE lpNewProc;
  LPBYTE lpCall2Orig;
  LPBYTE lpInjCode;
  LPBYTE lpInjData;
  SIZE_T nInjCodeAndDataSize;
  BYTE aOriginalStub[HOOKENG_MAX_ORIGINAL_STUB_SIZE], aNewStub[HOOKENG_MAX_STUB_SIZE];
  BYTE aJumpStub[8];
  SIZE_T nOriginalStubSize, nNewStubSize;
  SIZE_T nInstalledCode;
  DWORD dwFlags;
  CProcessesHandles::CEntryPtr cProcEntry;
};

//-----------------------------------------------------------

} //Internals
} //NktHookLib

#endif //_NKT_HOOKLIB_HOOKENTRY_H
