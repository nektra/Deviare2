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

#define __UD_STANDALONE__
#undef UD_DEBUG

#define ud_itab                     NktHookLib_ud_itab
#define ud_lookup_table_list        NktHookLib_ud_lookup_table_list
#define ud_reg_tab                  NktHookLib_ud_reg_tab

#define ud_inp_init                 NktHookLib_ud_inp_init
#define ud_syn_rel_target           NktHookLib_ud_syn_rel_target
#define ud_asmprintf                NktHookLib_ud_asmprintf
#define ud_syn_print_addr           NktHookLib_ud_syn_print_addr
#define ud_syn_print_imm            NktHookLib_ud_syn_print_imm
#define ud_syn_print_mem_disp       NktHookLib_ud_syn_print_mem_disp
#define ud_init                     NktHookLib_ud_init
#define ud_set_mode                 NktHookLib_ud_set_mode
#define ud_set_pc                   NktHookLib_ud_set_pc
#define ud_set_input_hook           NktHookLib_ud_set_input_hook
#define ud_set_input_buffer         NktHookLib_ud_set_input_buffer
#define ud_set_input_file           NktHookLib_ud_set_input_file
#define ud_set_vendor               NktHookLib_ud_set_vendor
#define ud_set_syntax               NktHookLib_ud_set_syntax
#define ud_input_skip               NktHookLib_ud_input_skip
#define ud_input_end                NktHookLib_ud_input_end
#define ud_decode                   NktHookLib_ud_decode
#define ud_disassemble              NktHookLib_ud_disassemble
#define ud_translate_intel          NktHookLib_ud_translate_intel
#define ud_translate_att            NktHookLib_ud_translate_att
#define ud_insn_asm                 NktHookLib_ud_insn_asm
#define ud_insn_ptr                 NktHookLib_ud_insn_ptr
#define ud_insn_off                 NktHookLib_ud_insn_off
#define ud_insn_hex                 NktHookLib_ud_insn_hex
#define ud_insn_len                 NktHookLib_ud_insn_len
#define ud_insn_opr                 NktHookLib_ud_insn_opr
#define ud_opr_is_sreg              NktHookLib_ud_opr_is_sreg
#define ud_opr_is_gpr               NktHookLib_ud_opr_is_gpr
#define ud_mnemonic_code            NktHookLib_ud_mnemonic_code
#define ud_mnemonics_str            NktHookLib_ud_mnemonics_str
#define ud_insn_mnemonic            NktHookLib_ud_insn_mnemonic
#define ud_lookup_mnemonic          NktHookLib_ud_lookup_mnemonic
#define ud_set_user_opaque_data     NktHookLib_ud_set_user_opaque_data
#define ud_get_user_opaque_data     NktHookLib_ud_get_user_opaque_data
#define ud_insn_sext_imm            NktHookLib_ud_insn_sext_imm
#define ud_set_asm_buffer           NktHookLib_ud_set_asm_buffer
#define ud_set_sym_resolver         NktHookLib_ud_set_sym_resolver
