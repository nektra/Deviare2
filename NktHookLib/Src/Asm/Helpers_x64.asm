 ;
 ; Copyright (C) 2010-2015 Nektra S.A., Buenos Aires, Argentina.
 ; All rights reserved. Contact: http://www.nektra.com
 ;
 ;
 ; This file is part of Deviare In-Proc
 ;
 ;
 ; Commercial License Usage
 ; ------------------------
 ; Licensees holding valid commercial Deviare In-Proc licenses may use this
 ; file in accordance with the commercial license agreement provided with the
 ; Software or, alternatively, in accordance with the terms contained in
 ; a written agreement between you and Nektra.  For licensing terms and
 ; conditions see http://www.nektra.com/licensing/.  For further information
 ; use the contact form at http://www.nektra.com/contact/.
 ;
 ;
 ; GNU General Public License Usage
 ; --------------------------------
 ; Alternatively, this file may be used under the terms of the GNU
 ; General Public License version 3.0 as published by the Free Software
 ; Foundation and appearing in the file LICENSE.GPL included in the
 ; packaging of this file.  Please review the following information to
 ; ensure the GNU General Public License version 3.0 requirements will be
 ; met: http://www.gnu.org/copyleft/gpl.html.
 ;
 ;

_TEXT SEGMENT

;---------------------------------------------------------------------------------

EXCEPTION_NONCONTINUABLE   EQU 01h
EXCEPTION_UNWINDING        EQU 02h
EXCEPTION_EXIT_UNWIND      EQU 04h
EXCEPTION_STACK_INVALID    EQU 08h
EXCEPTION_NESTED_CALL      EQU 10h
EXCEPTION_TARGET_UNWIND    EQU 20h
EXCEPTION_COLLIDED_UNWIND  EQU 40h
EXCEPTION_UNWIND           EQU 66h

CONTEXT STRUCT
    ;Register parameter home addresses
    P1Home        QWORD ?
    P2Home        QWORD ?
    P3Home        QWORD ?
    P4Home        QWORD ?
    P5Home        QWORD ?
    P6Home        QWORD ?
    ;Control flags
    ContextFlags  DWORD ?
    MxCsr         DWORD ?
    ;Segment Registers and processor flags
    SegCs         WORD ?
    SegDs         WORD ?
    SegEs         WORD ?
    SegFs         WORD ?
    SegGs         WORD ?
    SegSs         WORD ?
    EFlags        DWORD ?
    ;Debug registers
    _Dr0          QWORD ?
    _Dr1          QWORD ?
    _Dr2          QWORD ?
    _Dr3          QWORD ?
    _Dr6          QWORD ?
    _Dr7          QWORD ?
    ;Integer registers
    _Rax          QWORD ?
    _Rcx          QWORD ?
    _Rdx          QWORD ?
    _Rbx          QWORD ?
    _Rsp          QWORD ?
    _Rbp          QWORD ?
    _Rsi          QWORD ?
    _Rdi          QWORD ?
    _R8           QWORD ?
    _R9           QWORD ?
    _R10          QWORD ?
    _R11          QWORD ?
    _R12          QWORD ?
    _R13          QWORD ?
    _R14          QWORD ?
    _R15          QWORD ?
    ;Program counter
    _Rip          QWORD ?
    ;The rest is not used
CONTEXT ENDS

EXCEPTION_RECORD STRUCT
    ExceptionCode         DWORD ?
    ExceptionFlags        DWORD ?
    ExceptionRecord       QWORD ?
    ExceptionAddress      QWORD ?
    NumberParameters      DWORD ?
    __unusedAlignment     DWORD ?
    ExceptionInformation  QWORD 15 DUP(?)
EXCEPTION_RECORD ENDS

MYSEH STRUCT
    SafeOffset         QWORD ?
    PrevRsp            QWORD ?
    PrevRbp            QWORD ?
    __unusedAlignment  QWORD ?
MYSEH ENDS

;---------------------------------------------------------------------------------

PUBLIC NktHookLib_TryMemCopy

NktHookLib_TryMemCopy_SEH PROTO

ALIGN 16
;SIZE_T __stdcall NktHookLib_TryMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount);
NktHookLib_TryMemCopy PROC FRAME :NktHookLib_TryMemCopy_SEH
    sub  rsp, 28h
.allocstack 28h
.endprolog

    mov  r9, r8 ;copy original count
NktHookLib_TryMemCopy_GuardTop::
    test rcx, 7 ;destination aligned?
    jne  @slowPath
    test rdx, 7 ;source aligned?
    jne  @slowPath
    cmp  r8, 8
    jbe  @slowPath
@@:
    mov  rax, QWORD PTR [rdx]
    add  rdx, 8
    mov  QWORD PTR [rcx], rax
    add  rcx, 8
    sub  r8, 8
    cmp  r8, 8
    jae  @B
@slowPath:
    test r8, r8
    je   NktHookLib_TryMemCopy_AfterCopy
    mov  al, BYTE PTR [rdx]
    inc  rdx
    mov  BYTE PTR [rcx], al
    inc  rcx
    dec  r8
    jmp  @slowPath
NktHookLib_TryMemCopy_GuardBottom::
NktHookLib_TryMemCopy_AfterCopy::
    mov  rax, r9
    sub  rax, r8
    add  rsp, 28h
    ret
NktHookLib_TryMemCopy ENDP

ALIGN 16
NktHookLib_TryMemCopy_SEH PROC
    ;check if we are inside guarded code range
    lea  rax, NktHookLib_TryMemCopy_GuardTop
    cmp  [rcx].EXCEPTION_RECORD.ExceptionAddress, rax
    jb   @unhandled
    lea  rax, NktHookLib_TryMemCopy_GuardBottom
    cmp  [rcx].EXCEPTION_RECORD.ExceptionAddress, rax ; are we within the end range?
    jae  @unhandled
    lea  rax, NktHookLib_TryMemCopy_AfterCopy
    mov  [r8].CONTEXT._Rip, rax ; change the current location
    xor  rax, rax ;ExceptionContinueExecution
    jmp  @done
@unhandled:
    mov  rax, 1 ;ExceptionContinueSearch
@done:
    ret
NktHookLib_TryMemCopy_SEH ENDP

;---------------------------------------------------------------------------------

NktHookLib_TryCallOneParam_SEH PROTO

ALIGN 16
;SIZE_T __stdcall NktHookLib_TryCallOneParam(__in LPVOID lpFunc, __in SIZE_T nParam1, __in BOOL bIsCDecl);
NktHookLib_TryCallOneParam PROC FRAME :NktHookLib_TryCallOneParam_SEH
   db   48h ;REX prefix to enable hot-patching
   push rbp
.pushreg rbp
   mov  rbp, rsp
.setframe rbp, 0
   sub  rsp, 40h+sizeof MYSEH+20h
.allocstack 40h+sizeof MYSEH+20h
.endprolog
seh EQU <[rbp - sizeof MYSEH].MYSEH>

    ;setup SEH
    mov  rax, OFFSET NktHookLib_TryCallOneParam_AfterCall
    mov  seh.SafeOffset, rax
    mov  seh.PrevRsp, rsp
    mov  seh.PrevRbp, rbp
    ;do call
    mov  rax, rcx
    mov  rcx, rdx
    call rax
NktHookLib_TryCallOneParam_AfterCall::
    mov  rsp, rbp
    pop  rbp
    ret
NktHookLib_TryCallOneParam ENDP

ALIGN 16
NktHookLib_TryCallOneParam_SEH PROC
seh EQU <[rdx - sizeof MYSEH].MYSEH>

    push seh.SafeOffset
    pop  [r8].CONTEXT._Rip
    push seh.PrevRsp
    pop  [r8].CONTEXT._Rsp
    push seh.PrevRbp
    pop  [r8].CONTEXT._Rbp
    xor  rax, rax ;ExceptionContinueExecution
    ret
NktHookLib_TryCallOneParam_SEH ENDP

_TEXT ENDS

END
