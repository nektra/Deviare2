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

;See: http://www.japheth.de/ and http://www.ntcore.com/files/vista_x64.htm

.code

_TEXT SEGMENT

;---------------------------------------------------------------------------------

INIT:
    jmp   @@RealInit

INCLUDE GetProcAddress_x64.inc.asm

INIT_DATA STRUCT 8
    lpszDllNameW     QWORD  ?
    lpInitData       QWORD  ?
    hServerProcToken QWORD  ?
INIT_DATA ENDS

;---------------------------------------------------------------------------------

@@RealInit:
lpInitData$ = 70h+28h + 8h
hNtDll$ = 20h
fnLdrLoadDll$ = 28h
fnLdrUnloadDll$ = 30h
fnNtSetInformationThread$ = 38h
hMod$ = 40h
nExitCode$ = 48h
fnInitialize$ = 50h
nTemp$ = 58h
sDllNameUS$ = 60h  ;sizeof(UNICODE_STRING64)=16

    mov  QWORD PTR [rsp+8h], rcx ;save 1st parameter for later use
    sub  rsp, 70h + 28h             ;locals + shadow space + return address. Size should be 0x####8h always to mantain 16-byte alignment

    ;zero some variables
    xor  rax, rax
    mov  QWORD PTR fnNtSetInformationThread$[rsp], rax
    ;find needed apis
    GetDataPtr @@aNtDllDll
    mov  rcx, rax
    call GetModuleBaseAddress
    mov  QWORD PTR hNtDll$[rsp], rax
    test rax, rax
    je   @@api_not_found
    GetDataPtr @@aLdrLoadDll
    mov  rdx, rax
    mov  rcx, QWORD PTR hNtDll$[rsp]
    call GetProcedureAddress
    mov  QWORD PTR fnLdrLoadDll$[rsp], rax
    test rax, rax
    je   @@api_not_found
    GetDataPtr @@aLdrUnloadDll
    mov  rdx, rax
    mov  rcx, QWORD PTR hNtDll$[rsp]
    call GetProcedureAddress
    mov  QWORD PTR fnLdrUnloadDll$[rsp], rax
    test rax, rax
    je   @@api_not_found
    GetDataPtr @@aNtSetInformationThread
    mov  rdx, rax
    mov  rcx, QWORD PTR hNtDll$[rsp]
    call GetProcedureAddress
    mov  QWORD PTR fnNtSetInformationThread$[rsp], rax
    test rax, rax
    jne  @@check_and_load_agent
@@api_not_found:
    mov  DWORD PTR nExitCode$[rsp], 8007007Fh       ;HRESULT(ERROR_PROC_NOT_FOUND)
    jmp  @@exit

@@check_and_load_agent:
    ;impersonate as server thread
    mov  rax, QWORD PTR lpInitData$[rsp]
    lea  r8, QWORD PTR [rax].INIT_DATA.hServerProcToken ;addr of token
    cmp  QWORD PTR [r8], 0
    je   @F
    mov  rcx, 0FFFFFFFFFFFFFFFEh                        ;current thread
    mov  rdx, 5                                         ;ThreadImpersonationToken
    mov  r9, 8                                          ;sizeof(HANDLE)
    call QWORD PTR fnNtSetInformationThread$[rsp]
    test eax, 80000000h
    je   @F
    or   eax, 10000000h                                 ;FACILITY_NT_BIT
    mov  DWORD PTR nExitCode$[rsp], eax                 ;HRESULT_FROM_NT
    jmp  @@exit

@@: ;check if library is loaded
    mov  rax, QWORD PTR lpInitData$[rsp]
    mov  rcx, QWORD PTR [rax].INIT_DATA.lpszDllNameW
    call GetModuleBaseAddress
    test rax, rax
    je   @F
    mov  DWORD PTR nExitCode$[rsp], 800704DFh       ;HRESULT(ERROR_ALREADY_INITIALIZED)
    jmp  @@exit

@@: ;build UNICODE_STRING
    mov  rax, QWORD PTR lpInitData$[rsp]
    mov  rcx, QWORD PTR [rax].INIT_DATA.lpszDllNameW
    lea  rax, QWORD PTR sDllNameUS$[rsp]
    mov  QWORD PTR [rax].UNICODE_STRING64.Buffer, rcx
    xor  rdx, rdx
@@: cmp  WORD PTR [rcx+rdx], 0
    je   @@got_lpszDllNameW_len
    add  rdx, 2
    jmp  @b
@@got_lpszDllNameW_len:
    mov  WORD PTR [rax].UNICODE_STRING64._Length, dx
    mov  WORD PTR [rax].UNICODE_STRING64.MaximumLength, dx

    ;load library
    xor  rcx, rcx ;PathToFile
    xor  rdx, rdx ;Flags
    lea  r8, QWORD PTR sDllNameUS$[rsp] ;ModuleFileName
    lea  r9, QWORD PTR hMod$[rsp] ;ModuleHandle
    call QWORD PTR fnLdrLoadDll$[rsp]
    test eax, 80000000h
    je   @F ;dll loaded
    or   eax, 10000000h                                 ;FACILITY_NT_BIT
    mov  DWORD PTR nExitCode$[rsp], eax                 ;HRESULT_FROM_NT
    jmp  @@exit

@@: ;find Initialize function address
    GetDataPtr @@aInitialize
    mov  rdx, rax
    mov  rcx, QWORD PTR hMod$[rsp]
    call GetProcedureAddress
    mov  QWORD PTR fnInitialize$[rsp], rax
    test rax, rax
    jne  @F
@@proc_not_found:
    mov  DWORD PTR nExitCode$[rsp], 8007007Fh       ;HRESULT(ERROR_PROC_NOT_FOUND)
    jmp  @@exit_free_dll

@@: ;call initialize
    mov  rax, QWORD PTR lpInitData$[rsp]
    mov  rcx, QWORD PTR [rax].INIT_DATA.lpInitData
    call QWORD PTR fnInitialize$[rsp]     ;call dll's Initialize function (eax will have the return code)
    mov  DWORD PTR nExitCode$[rsp], eax
    test eax, eax
    je   @@exit

@@exit_free_dll:
    mov  rcx, QWORD PTR hMod$[rsp]
    call QWORD PTR fnLdrUnloadDll$[rsp]

@@exit:
    ;remove thread token if provided
    xor  rax, rax
    cmp  QWORD PTR fnNtSetInformationThread$[rsp], rax
    je   @F
    mov  rax, QWORD PTR lpInitData$[rsp]
    lea  r8, QWORD PTR [rax].INIT_DATA.hServerProcToken ;addr of token
    xor  rax, rax
    cmp  QWORD PTR [r8], rax
    je   @F
    lea  r8, QWORD PTR nTemp$[rsp]
    mov  QWORD PTR [r8], rax
    mov  rcx, 0FFFFFFFFFFFFFFFEh                        ;current thread
    mov  rdx, 5                                         ;ThreadImpersonationToken
    mov  r9, 8                                          ;sizeof(HANDLE)
    call QWORD PTR fnNtSetInformationThread$[rsp]

@@: ;done
    xor  rax, rax
    mov  eax, DWORD PTR nExitCode$[rsp]
    add  rsp, 70h + 28h
    ret

ALIGN 8
@@aNtDllDll:
    DB 'n', 0, 't', 0, 'd', 0, 'l', 0, 'l', 0, '.', 0, 'd', 0, 'l', 0, 'l', 0, 0, 0
@@aLdrLoadDll:
    DB "LdrLoadDll", 0
@@aLdrUnloadDll:
    DB "LdrUnloadDll", 0
@@aNtSetInformationThread:
    DB "NtSetInformationThread", 0
@@aInitialize:
    DB "Initialize", 0

;---------------------------------------------------------------------------------

_TEXT ENDS

END
