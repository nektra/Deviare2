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

#ifndef _NKT_HOOKLIB_RELOCATABLECODE_H
#define _NKT_HOOKLIB_RELOCATABLECODE_H

#include <NktHookLib.h>

namespace NktHookLib {
namespace Internals {
namespace RelocatableCode {

//-----------------------------------------------------------

typedef struct {
  SIZE_T nOffset_GetModuleBaseAddress;
  SIZE_T nOffset_GetProcedureAddress;
} GETMODULEANDPROCADDR_DATA;

//-----------------------------------------------------------

SIZE_T GetModuleAndProcAddr_GetSize(__in LONG nPlatform);
LPBYTE GetModuleAndProcAddr_GetCode(__in LONG nPlatform, __out GETMODULEANDPROCADDR_DATA &sAddresses);

SIZE_T InjectDllInNewProcess_GetSize(__in LONG nPlatform);
LPBYTE InjectDllInNewProcess_GetCode(__in LONG nPlatform);

SIZE_T InjectDllInRunningProcess_GetSize(__in LONG nPlatform);
LPBYTE InjectDllInRunningProcess_GetCode(__in LONG nPlatform);

SIZE_T WaitForEventAtStartup_GetSize(__in LONG nPlatform);
LPBYTE WaitForEventAtStartup_GetCode(__in LONG nPlatform);

//-----------------------------------------------------------

} //RelocatableCode
} //Internals
} //NktHookLib

#endif //_NKT_HOOKLIB_RELOCATABLECODE_H
