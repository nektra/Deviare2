 ;
 ; Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 ; All rights reserved. Contact: http://www.nektra.com
 ;
 ;
 ; This file is part of Deviare
 ;
 ;
 ; Commercial License Usage
 ; ------------------------
 ; Licensees holding valid commercial Deviare licenses may use this file
 ; in accordance with the commercial license agreement provided with the
 ; Software or, alternatively, in accordance with the terms contained in
 ; a written agreement between you and Nektra.  For licensing terms and
 ; conditions see http://www.nektra.com/licensing/. Use the contact form
 ; at http://www.nektra.com/contact/ for further information.
 ;
 ;
 ; GNU General Public License Usage
 ; --------------------------------
 ; Alternatively, this file may be used under the terms of the GNU General
 ; Public License version 3.0 as published by the Free Software Foundation
 ; and appearing in the file LICENSE.GPL included in the packaging of this
 ; file.  Please visit http://www.gnu.org/copyleft/gpl.html and review the
 ; information to ensure the GNU General Public License version 3.0
 ; requirements will be met.
 ;
 ;

;See: http://www.japheth.de/

.code

_TEXT SEGMENT

;---------------------------------------------------------------------------------

PUBLIC NktDvSuperHook_x64

USAGE_COUNT                     equ 0FFDDFFDDFFDDFF01h
HOOK_ENGINE_PTR                 equ 0FFDDFFDDFFDDFF02h
HOOK_ENTRY_PTR                  equ 0FFDDFFDDFFDDFF03h
PRECALL_ADDR                    equ 0FFDDFFDDFFDDFF04h
POSTCALL_ADDR                   equ 0FFDDFFDDFFDDFF05h
JMP_TO_ORIGINAL                 equ 0FFDDFFDDFFDDFF06h
CONTINUE_AFTER_CALL_MARK        equ 0FFDDFFDDFFDDFF07h
AFTER_CALL_STACK_PRESERVE_SIZE  equ 0FFDDFFDDFFDDFF08h
AFTER_CALL_STACK_PRESERVE_SIZE2 equ 0FFDDFFDDFFDDFF09h

ALIGN 8
NktDvSuperHook_x64:
    sub     rsp, 200h + 28h                ;locals + shadow space + return address. Size should be
                                           ;    0x####8h always to mantain 16-byte alignment
    mov     qword ptr [rsp+20h], rax       ;save rax

    mov     rax, USAGE_COUNT
    lock    inc qword ptr [rax] ;increment usage count

    mov     qword ptr [rsp+28h], rbx       ;save the rest of the registers
    mov     qword ptr [rsp+30h], rcx
    mov     qword ptr [rsp+38h], rdx
    mov     qword ptr [rsp+40h], rbp
    mov     qword ptr [rsp+48h], rsi
    mov     qword ptr [rsp+50h], rdi
    mov     qword ptr [rsp+58h], r8
    mov     qword ptr [rsp+60h], r9
    mov     qword ptr [rsp+68h], r10
    mov     qword ptr [rsp+70h], r11
    mov     qword ptr [rsp+78h], r12
    mov     qword ptr [rsp+80h], r13
    mov     qword ptr [rsp+88h], r14
    mov     qword ptr [rsp+90h], r15
    movupd  xmmword ptr [rsp+0a0h], xmm0
    movupd  xmmword ptr [rsp+0b0h], xmm1
    movupd  xmmword ptr [rsp+0c0h], xmm2
    movupd  xmmword ptr [rsp+0d0h], xmm3
    movupd  xmmword ptr [rsp+0e0h], xmm4
    movupd  xmmword ptr [rsp+0f0h], xmm5
    movupd  xmmword ptr [rsp+100h], xmm6
    movupd  xmmword ptr [rsp+110h], xmm7
    movupd  xmmword ptr [rsp+120h], xmm8
    movupd  xmmword ptr [rsp+130h], xmm9
    movupd  xmmword ptr [rsp+140h], xmm10
    movupd  xmmword ptr [rsp+150h], xmm11
    movupd  xmmword ptr [rsp+160h], xmm12
    movupd  xmmword ptr [rsp+170h], xmm13
    movupd  xmmword ptr [rsp+180h], xmm14
    movupd  xmmword ptr [rsp+190h], xmm15

    ;call precall routine
    mov     rcx, HOOK_ENGINE_PTR
    mov     rdx, HOOK_ENTRY_PTR
    mov     r8, rsp
    mov     rax, PRECALL_ADDR
    call    rax
    test    rax, rax
    je      @dont_process_this
    test    rax, 80000000h                 ;skip bit?
    je      @normal_call

    and     rax, 7FFFFFFFh                 ;rax=stack bytes to add

    movupd  xmm15, xmmword ptr [rsp+190h]  ;restore the registers
    movupd  xmm14, xmmword ptr [rsp+180h]
    movupd  xmm13, xmmword ptr [rsp+170h]
    movupd  xmm12, xmmword ptr [rsp+160h]
    movupd  xmm11, xmmword ptr [rsp+150h]
    movupd  xmm10, xmmword ptr [rsp+140h]
    movupd  xmm9, xmmword ptr [rsp+130h]
    movupd  xmm8, xmmword ptr [rsp+120h]
    movupd  xmm7, xmmword ptr [rsp+110h]
    movupd  xmm6, xmmword ptr [rsp+100h]
    movupd  xmm5, xmmword ptr [rsp+0f0h]
    movupd  xmm4, xmmword ptr [rsp+0e0h]
    movupd  xmm3, xmmword ptr [rsp+0d0h]
    movupd  xmm2, xmmword ptr [rsp+0c0h]
    movupd  xmm1, xmmword ptr [rsp+0b0h]
    movupd  xmm0, xmmword ptr [rsp+0a0h]
    mov     r15, qword ptr [rsp+90h]
    mov     r14, qword ptr [rsp+88h]
    mov     r13, qword ptr [rsp+80h]
    mov     r12, qword ptr [rsp+78h]
    mov     r11, qword ptr [rsp+70h]
    mov     r10, qword ptr [rsp+68h]
    mov     r9, qword ptr [rsp+60h]
    mov     r8, qword ptr [rsp+58h]
    mov     rdi, qword ptr [rsp+50h]
    mov     rsi, qword ptr [rsp+48h]
    mov     rbp, qword ptr [rsp+40h]
    mov     rdx, qword ptr [rsp+38h]
    mov     rcx, qword ptr [rsp+30h]
    mov     rbx, qword ptr [rsp+28h]

    mov     rax, USAGE_COUNT
    lock    sub qword ptr [rax], 1h        ;decrement usage count
    mov     rax, qword ptr [rsp+20h]       ;restore eax register

    ;in x64 calling convention the caller always cleans the stack so remaining rax is ignored
    add     rsp, 200h + 28h
    ret

@normal_call:
    movupd  xmm15, xmmword ptr [rsp+190h]  ;restore the registers
    movupd  xmm14, xmmword ptr [rsp+180h]
    movupd  xmm13, xmmword ptr [rsp+170h]
    movupd  xmm12, xmmword ptr [rsp+160h]
    movupd  xmm11, xmmword ptr [rsp+150h]
    movupd  xmm10, xmmword ptr [rsp+140h]
    movupd  xmm9, xmmword ptr [rsp+130h]
    movupd  xmm8, xmmword ptr [rsp+120h]
    movupd  xmm7, xmmword ptr [rsp+110h]
    movupd  xmm6, xmmword ptr [rsp+100h]
    movupd  xmm5, xmmword ptr [rsp+0f0h]
    movupd  xmm4, xmmword ptr [rsp+0e0h]
    movupd  xmm3, xmmword ptr [rsp+0d0h]
    movupd  xmm2, xmmword ptr [rsp+0c0h]
    movupd  xmm1, xmmword ptr [rsp+0b0h]
    movupd  xmm0, xmmword ptr [rsp+0a0h]
    mov     r15, qword ptr [rsp+90h]
    mov     r14, qword ptr [rsp+88h]
    mov     r13, qword ptr [rsp+80h]
    mov     r12, qword ptr [rsp+78h]
    mov     r11, qword ptr [rsp+70h]
    mov     r10, qword ptr [rsp+68h]
    mov     r9, qword ptr [rsp+60h]
    mov     r8, qword ptr [rsp+58h]
    mov     rdi, qword ptr [rsp+50h]
    mov     rsi, qword ptr [rsp+48h]
    mov     rbp, qword ptr [rsp+40h]
    mov     rdx, qword ptr [rsp+38h]
    mov     rcx, qword ptr [rsp+30h]
    mov     rbx, qword ptr [rsp+28h]
    mov     rax, qword ptr [rsp+20h]
    add     rsp, 200h + 28h

    push    rax
    push    rax
    mov     rax, JMP_TO_ORIGINAL
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rsp+8], rax
    pop     rax
    ret

    DQ      CONTINUE_AFTER_CALL_MARK
    ;here is the return address when the call to the original function returns
    push    rax                            ;push a dummy value that will be modified with the real return value
    sub     rsp, 200h + 28h
    mov     qword ptr [rsp+20h], rax       ;save all the registers
    mov     qword ptr [rsp+28h], rbx
    mov     qword ptr [rsp+30h], rcx
    mov     qword ptr [rsp+38h], rdx
    mov     qword ptr [rsp+40h], rbp
    mov     qword ptr [rsp+48h], rsi
    mov     qword ptr [rsp+50h], rdi
    mov     qword ptr [rsp+58h], r8
    mov     qword ptr [rsp+60h], r9
    mov     qword ptr [rsp+68h], r10
    mov     qword ptr [rsp+70h], r11
    mov     qword ptr [rsp+78h], r12
    mov     qword ptr [rsp+80h], r13
    mov     qword ptr [rsp+88h], r14
    mov     qword ptr [rsp+90h], r15
    movupd  xmmword ptr [rsp+0a0h], xmm0
    movupd  xmmword ptr [rsp+0b0h], xmm1
    movupd  xmmword ptr [rsp+0c0h], xmm2
    movupd  xmmword ptr [rsp+0d0h], xmm3
    movupd  xmmword ptr [rsp+0e0h], xmm4
    movupd  xmmword ptr [rsp+0f0h], xmm5
    movupd  xmmword ptr [rsp+100h], xmm6
    movupd  xmmword ptr [rsp+110h], xmm7
    movupd  xmmword ptr [rsp+120h], xmm8
    movupd  xmmword ptr [rsp+130h], xmm9
    movupd  xmmword ptr [rsp+140h], xmm10
    movupd  xmmword ptr [rsp+150h], xmm11
    movupd  xmmword ptr [rsp+160h], xmm12
    movupd  xmmword ptr [rsp+170h], xmm13
    movupd  xmmword ptr [rsp+180h], xmm14
    movupd  xmmword ptr [rsp+190h], xmm15

    ;call postcall routine
    mov     rcx, HOOK_ENGINE_PTR
    mov     rdx, HOOK_ENTRY_PTR
    mov     r8, rsp
    mov     rax, POSTCALL_ADDR
    call    rax
    mov     qword ptr [rsp+200h+28h], rax   ;replace return value

    movupd  xmm15, xmmword ptr [rsp+190h]   ;restore the registers
    movupd  xmm14, xmmword ptr [rsp+180h]
    movupd  xmm13, xmmword ptr [rsp+170h]
    movupd  xmm12, xmmword ptr [rsp+160h]
    movupd  xmm11, xmmword ptr [rsp+150h]
    movupd  xmm10, xmmword ptr [rsp+140h]
    movupd  xmm9, xmmword ptr [rsp+130h]
    movupd  xmm8, xmmword ptr [rsp+120h]
    movupd  xmm7, xmmword ptr [rsp+110h]
    movupd  xmm6, xmmword ptr [rsp+100h]
    movupd  xmm5, xmmword ptr [rsp+0f0h]
    movupd  xmm4, xmmword ptr [rsp+0e0h]
    movupd  xmm3, xmmword ptr [rsp+0d0h]
    movupd  xmm2, xmmword ptr [rsp+0c0h]
    movupd  xmm1, xmmword ptr [rsp+0b0h]
    movupd  xmm0, xmmword ptr [rsp+0a0h]
    mov     r15, qword ptr [rsp+90h]
    mov     r14, qword ptr [rsp+88h]
    mov     r13, qword ptr [rsp+80h]
    mov     r12, qword ptr [rsp+78h]
    mov     r11, qword ptr [rsp+70h]
    mov     r10, qword ptr [rsp+68h]
    mov     r9, qword ptr [rsp+60h]
    mov     r8, qword ptr [rsp+58h]
    mov     rdi, qword ptr [rsp+50h]
    mov     rsi, qword ptr [rsp+48h]
    mov     rbp, qword ptr [rsp+40h]
    mov     rdx, qword ptr [rsp+38h]
    mov     rcx, qword ptr [rsp+30h]
    mov     rbx, qword ptr [rsp+28h]

    mov     rax, USAGE_COUNT
    lock    sub qword ptr [rax], 1h         ;decrement usage count

    mov     rax, qword ptr [rsp+20h]
    add     rsp, 200h + 28h
    ret

@dont_process_this:
    movupd  xmm15, xmmword ptr [rsp+190h]   ;restore the registers
    movupd  xmm14, xmmword ptr [rsp+180h]
    movupd  xmm13, xmmword ptr [rsp+170h]
    movupd  xmm12, xmmword ptr [rsp+160h]
    movupd  xmm11, xmmword ptr [rsp+150h]
    movupd  xmm10, xmmword ptr [rsp+140h]
    movupd  xmm9, xmmword ptr [rsp+130h]
    movupd  xmm8, xmmword ptr [rsp+120h]
    movupd  xmm7, xmmword ptr [rsp+110h]
    movupd  xmm6, xmmword ptr [rsp+100h]
    movupd  xmm5, xmmword ptr [rsp+0f0h]
    movupd  xmm4, xmmword ptr [rsp+0e0h]
    movupd  xmm3, xmmword ptr [rsp+0d0h]
    movupd  xmm2, xmmword ptr [rsp+0c0h]
    movupd  xmm1, xmmword ptr [rsp+0b0h]
    movupd  xmm0, xmmword ptr [rsp+0a0h]
    mov     r15, qword ptr [rsp+90h]
    mov     r14, qword ptr [rsp+88h]
    mov     r13, qword ptr [rsp+80h]
    mov     r12, qword ptr [rsp+78h]
    mov     r11, qword ptr [rsp+70h]
    mov     r10, qword ptr [rsp+68h]
    mov     r9, qword ptr [rsp+60h]
    mov     r8, qword ptr [rsp+58h]
    mov     rdi, qword ptr [rsp+50h]
    mov     rsi, qword ptr [rsp+48h]
    mov     rbp, qword ptr [rsp+40h]
    mov     rdx, qword ptr [rsp+38h]
    mov     rcx, qword ptr [rsp+30h]
    mov     rbx, qword ptr [rsp+28h]

    mov     rax, USAGE_COUNT
    lock    sub qword ptr [rax], 1h        ;decrement usage count

    mov     rax, qword ptr [rsp+20h]
    add     rsp, 200h + 28h

    push    rax
    push    rax
    mov     rax, JMP_TO_ORIGINAL
    mov     rax, QWORD PTR [rax]
    mov     QWORD PTR [rsp+8], rax
    pop     rax
    ret

    DQ      0FFDDFFDDFFDDFFFFh

;---------------------------------------------------------------------------------
;---------------------------------------------------------------------------------

PUBLIC Internal_GetStackTrace_X64_ASM1

;static VOID Internal_GetStackTrace_X64_ASM(__out SIZE_T *lpnRetrieved, __out SIZE_T *lpnOutput, __in SIZE_T nCount,
;                                           __in SIZE_T nSkipCount, __in CONTEXT *lpCtx, __in SIZE_T nStackLimitLow,
;                                           __in SIZE_T nStackLimitHigh, __in LPVOID lpInUseStack,
;                                           __in SIZE_T nAddressOfTargetRIP, __in SIZE_T nAddressOfTargetRIP);

ALIGN 8
Internal_GetStackTrace_X64_ASM1:
    mov     rax, [rsp]
    ret

_TEXT ENDS

END
