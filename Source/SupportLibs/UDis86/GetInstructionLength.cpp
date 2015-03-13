#include "GetInstructionLength.h"

extern "C" {
  #include "types.h"
  #include "extern.h"
  #include "itab.h"
};

//-----------------------------------------------------------

SIZE_T GetInstructionLength(__in LPVOID lpAddr, __in SIZE_T nSize, __in BYTE nPlatformBits,
                            __out_opt BOOL *lpbIsMemOp)
{
  SIZE_T nLen;
  ud_t ud_obj;
  ud_operand_t *lpUdOp;

  if (lpbIsMemOp != NULL)
    *lpbIsMemOp = FALSE;
  ud_init(&ud_obj);
  ud_set_mode(&ud_obj, (uint8_t)nPlatformBits);
  ud_set_input_buffer(&ud_obj, (LPBYTE)lpAddr, nSize);
  ud_set_syntax(&ud_obj, NULL);
  nLen = (SIZE_T)ud_disassemble(&ud_obj);
  if (nLen > 0 && lpbIsMemOp != NULL)
  {
    lpUdOp = const_cast<ud_operand_t *>(ud_insn_opr(&ud_obj, 0));
    if (lpUdOp != NULL && lpUdOp->type == UD_OP_MEM)
      *lpbIsMemOp = TRUE;
    lpUdOp = const_cast<ud_operand_t *>(ud_insn_opr(&ud_obj, 1));
    if (lpUdOp != NULL && lpUdOp->type == UD_OP_MEM)
      *lpbIsMemOp = TRUE;
  }
  return nLen;
}
