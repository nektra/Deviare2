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

#include <NktHookLib.h>

extern "C" {
  #include "stdint.h"
  #include "amalgamation_defines.h"
  #include "source\libudis86\types.h"
  #include "source\libudis86\extern.h"
  #include "tables\itab.h"
};

//-----------------------------------------------------------

namespace NktHookLibHelpers {

SIZE_T GetInstructionLength(__in LPVOID lpAddr, __in SIZE_T nSize, __in BYTE nPlatformBits,
                            __out_opt BOOL *lpbIsMemOp, __out_z_opt LPSTR szBufA, __in SIZE_T nBufLen)
{
  SIZE_T nLen;
  ud_t ud_obj;
  ud_operand_t *lpUdOp;

  if (lpbIsMemOp != NULL)
    *lpbIsMemOp = FALSE;
  NktHookLib_ud_init(&ud_obj);
  NktHookLib_ud_set_mode(&ud_obj, (uint8_t)nPlatformBits);
  NktHookLib_ud_set_input_buffer(&ud_obj, (LPBYTE)lpAddr, nSize);
  if (szBufA != NULL && nBufLen > 0)
  {
    NktHookLib_ud_set_syntax(&ud_obj, UD_SYN_INTEL);
    NktHookLib_ud_set_asm_buffer(&ud_obj, szBufA, nBufLen);
  }
  else
  {
    NktHookLib_ud_set_syntax(&ud_obj, NULL);
  }
  nLen = (SIZE_T)NktHookLib_ud_disassemble(&ud_obj);
  if (nLen > 0 && lpbIsMemOp != NULL)
  {
    lpUdOp = const_cast<ud_operand_t *>(NktHookLib_ud_insn_opr(&ud_obj, 0));
    if (lpUdOp != NULL && lpUdOp->type == UD_OP_MEM)
      *lpbIsMemOp = TRUE;
    lpUdOp = const_cast<ud_operand_t *>(NktHookLib_ud_insn_opr(&ud_obj, 1));
    if (lpUdOp != NULL && lpUdOp->type == UD_OP_MEM)
      *lpbIsMemOp = TRUE;
  }
  return nLen;
}

} //NktHookLibHelpers
