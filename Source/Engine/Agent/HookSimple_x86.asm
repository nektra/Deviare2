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

.686p
.model flat, stdcall
.code

_TEXT SEGMENT

;---------------------------------------------------------------------------------

PUBLIC NktDvSuperHook_x86@0

USAGE_COUNT                     equ 0FFDDFF01h
HOOK_ENGINE_PTR                 equ 0FFDDFF02h
HOOK_ENTRY_PTR                  equ 0FFDDFF03h
PRECALL_ADDR                    equ 0FFDDFF04h
POSTCALL_ADDR                   equ 0FFDDFF05h
JMP_TO_ORIGINAL                 equ 0FFDDFF06h
CONTINUE_AFTER_CALL_MARK        equ 0FFDDFF07h
AFTER_CALL_STACK_PRESERVE_SIZE  equ 0FFDDFF08h
AFTER_CALL_STACK_PRESERVE_SIZE2 equ 0FFDDFF09h

ALIGN 4
NktDvSuperHook_x86@0:
    lock    inc dword ptr ds:[USAGE_COUNT] ;increment usage count

    push    eax                        ;save registers
    push    ebx
    push    ecx
    push    edx
    push    ebp
    push    esi
    push    edi

    mov     ebx, esp                   ;do stack alignment to 8 byte
    and     ebx, 7                     ;boundary for storing floating
    sub     esp, ebx                   ;point register
    xor     eax, eax
    push    eax                        ;on x86, push a quiet-nan double value
    mov     eax, 40700000h             ;stack and check for modifications later
    push    eax
    push    ebx                        ;store alignment offset & st(0) available bit

    ;call precall routine
    push    esp
    mov     eax, HOOK_ENTRY_PTR
    push    eax
    mov     eax, HOOK_ENGINE_PTR
    push    eax
    mov     eax, PRECALL_ADDR
    call    eax
    test    eax, eax
    je      @dont_process_this
    test    eax, 80000000h             ;skip bit?
    je      @normal_call

    ;skip call
    and     eax, 7FFFFFFFh             ;eax=stack bytes to add

    pop     ebx                        ;get alignment offset & st(0) available bit
    test    ebx, 80000000h             ;is st(0) available?
    je      @F
    fld     qword ptr [esp]            ;load st(0)
@@: add     esp, 8
    and     ebx, 7FFFFFFFh             ;restore stack aligment
    add     esp, ebx

    mov     edx, dword ptr [esp+1Ch]   ;move the return address 'skip bytes' forward
    mov     dword ptr [esp+1Ch+eax], edx

    pop     edi                        ;restore the registers except eax
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx

    add     eax, 4
    add     esp, eax
    neg     eax
    mov     eax, dword ptr [esp+eax]   ;restore eax register
    lock    dec dword ptr ds:[USAGE_COUNT]     ;decrement usage count
    ret

@normal_call:
    pop     ebx                        ;get alignment offset & ignore st(0) available bit
    add     esp, 8
    and     ebx, 7FFFFFFFh             ;restore stack aligment
    add     esp, ebx

    pop     edi                        ;restore the registers
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    DB      0FFh, 25h                  ;JMP DWORD PTR []
    DD      JMP_TO_ORIGINAL            ;RELATIVE address to call original

    DD      CONTINUE_AFTER_CALL_MARK

    ;here is the return address when the call to the original function returns

    sub     esp, AFTER_CALL_STACK_PRESERVE_SIZE
    push    eax                        ;push a dummy value that will be modified with the real return value

    push    eax                        ;save all the registers
    push    ebx
    push    ecx
    push    edx
    push    ebp
    push    esi
    push    edi

    mov     ebx, esp                   ;do stack alignment to 8 byte
    and     ebx, 7h                    ;boundary for storing floating
    sub     esp, ebx                   ;point register
    fxam                               ;check if st(0) has a value
    fnstsw  ax
    fwait
    and     ax, 4500h                  ;check C3, C2 & C0 bits of fpu status word
    cmp     ax, 4100h                  ;if C3=1, C2=0 & C0=1 => st(0) is empty
    jz      @F
    ;we have a value in the stack
    sub     esp, 8                     ;save st(0)
    fstp    qword ptr [esp]            ;
    or      ebx, 80000000h             ;set st(0) available bit
    jmp     @after_fp_save2
@@: xor     eax, eax
    push    eax                        ;push quiet-nan double value
    mov     eax, 40700000h
    push    eax
@after_fp_save2:
    push    ebx                        ;store alignment offset & st(0) available bit

    ;call postcall routine
    push    esp
    mov     eax, HOOK_ENTRY_PTR
    push    eax
    mov     eax, HOOK_ENGINE_PTR
    push    eax
    mov     eax, POSTCALL_ADDR
    call    eax

    pop     ebx                        ;get alignment offset & st(0) available bit
    test    ebx, 80000000h             ;is st(0) available?
    je      @F
    fld     qword ptr [esp]            ;load st(0)
@@: add     esp, 8
    and     ebx, 7FFFFFFFh             ;restore stack aligment
    add     esp, ebx

    mov     dword ptr [esp+1Ch], eax   ;replace return value (after floating-point to avoid stack aligment calculation)

    pop     edi                        ;restore the registers
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax
    lock    dec dword ptr ds:[USAGE_COUNT] ;decrement usage count
    DB      0C2h                       ;ret with immediate
    DD      AFTER_CALL_STACK_PRESERVE_SIZE2

@dont_process_this:
    pop     ebx                        ;get alignment offset & ignore st(0) available bit
    add     esp, 8
    and     ebx, 7FFFFFFFh             ;restore stack aligment
    add     esp, ebx

    pop     edi                        ;restore the registers
    pop     esi
    pop     ebp
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    lock    dec dword ptr ds:[USAGE_COUNT] ;decrement usage count

    DB      0FFh, 25h                  ;JMP DWORD PTR []
    DD      JMP_TO_ORIGINAL            ;RELATIVE address to call original

    DD      0FFDDFFFFh

_TEXT ENDS

;TO ADD:
;
;mov      RAX,  fixed-allocation-size
;call   __chkstk
;
;pushfq [4 bytes]
;fxsave/FXRSTOR dword ptr [4 bytes]
;stmxcsr/LDMXCSR dword ptr [512 bytes]  MXCSR

END
