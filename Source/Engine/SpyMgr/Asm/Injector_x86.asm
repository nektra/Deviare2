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

.386
.model flat, stdcall
.code

_TEXT SEGMENT

;---------------------------------------------------------------------------------

INIT:
    jmp   @@RealInit

INCLUDE GetProcAddress_x86.inc.asm

INIT_DATA STRUCT 8
    lpszDllNameW     DWORD  ?
    lpInitData       DWORD  ?
    hServerProcToken DWORD  ?
INIT_DATA ENDS

;---------------------------------------------------------------------------------

@@RealInit:
lpInitData$ = 8h
hNtDll$ = -4h
fnLdrLoadDll$ = -8h
fnLdrUnloadDll$ = -0Ch
fnNtSetInformationThread$ = -10h
hMod$ = -14h
nExitCode$ = -18h
fnInitialize$ = -1Ch
nTemp$ = -20h
sDllNameUS$ = -28h ;sizeof(UNICODE_STRING32)=8

    push ebp
    mov  ebp, esp
    sub  esp, 40h ;locals

    ;zero some variables
    xor  eax, eax
    mov  DWORD PTR fnNtSetInformationThread$[ebp], eax
    ;find needed apis
    GetDataPtr eax, @@aNtDllDll
    push eax
    call GetModuleBaseAddress
    mov  DWORD PTR hNtDll$[ebp], eax
    test eax, eax
    je   @@api_not_found
    GetDataPtr eax, @@aLdrLoadDll
    push eax
    push DWORD PTR hNtDll$[ebp]
    call GetProcedureAddress
    mov  DWORD PTR fnLdrLoadDll$[ebp], eax
    test eax, eax
    je   @@api_not_found
    GetDataPtr eax, @@aLdrUnloadDll
    push eax
    push DWORD PTR hNtDll$[ebp]
    call GetProcedureAddress
    mov  DWORD PTR fnLdrUnloadDll$[ebp], eax
    test eax, eax
    je   @@api_not_found
    GetDataPtr eax, @@aNtSetInformationThread
    push eax
    push DWORD PTR hNtDll$[ebp]
    call GetProcedureAddress
    mov  DWORD PTR fnNtSetInformationThread$[ebp], eax
    test eax, eax
    jne  @@check_and_load_agent
@@api_not_found:
    mov  DWORD PTR nExitCode$[ebp], 8007007Fh       ;HRESULT(ERROR_PROC_NOT_FOUND)
    jmp  @@exit

@@check_and_load_agent:
    ;impersonate as server thread
    mov  eax, DWORD PTR lpInitData$[ebp]
    lea  eax, DWORD PTR [eax].INIT_DATA.hServerProcToken
    cmp  DWORD PTR [eax], 0  ;check if token was provided
    je   @F
    push 4                   ;sizeof(HANDLE)
    push eax                 ;addr of token
    push 5                   ;ThreadImpersonationToken
    push 0FFFFFFFEh          ;current thread
    call DWORD PTR fnNtSetInformationThread$[ebp]
    test eax, 80000000h
    je   @F
    or   eax, 10000000h                             ;FACILITY_NT_BIT
    mov  DWORD PTR nExitCode$[ebp], eax             ;HRESULT_FROM_NT
    jmp  @@exit

@@: ;check if library is loaded
    mov  eax, DWORD PTR lpInitData$[ebp]
    push DWORD PTR [eax].INIT_DATA.lpszDllNameW
    call GetModuleBaseAddress
    test eax, eax
    je   @F
    mov  DWORD PTR nExitCode$[ebp], 800704DFh       ;HRESULT(ERROR_ALREADY_INITIALIZED)
    jmp  @@exit

@@: ;build UNICODE_STRING
    push ebx
    push edi
    mov  eax, DWORD PTR lpInitData$[ebp]
    mov  edi, DWORD PTR [eax].INIT_DATA.lpszDllNameW
    lea  eax, DWORD PTR sDllNameUS$[ebp]
    mov  DWORD PTR [eax].UNICODE_STRING32.Buffer, edi
    xor  ebx, ebx
@@: cmp  WORD PTR [edi+ebx], 0
    je   @@got_lpszDllNameW_len
    add  ebx, 2
    jmp  @b
@@got_lpszDllNameW_len:
    mov  WORD PTR [eax].UNICODE_STRING32._Length, bx
    mov  WORD PTR [eax].UNICODE_STRING32.MaximumLength, bx
    pop  edi
    pop  ebx

    ;load library
    lea  eax, DWORD PTR hMod$[ebp]
    push eax ;ModuleHandle
    lea  eax, DWORD PTR sDllNameUS$[ebp]
    push eax ;ModuleFileName
    xor  eax, eax
    push eax ;Flags
    push eax ;PathToFile
    call DWORD PTR fnLdrLoadDll$[ebp]
    test eax, 80000000h
    je   @F ;dll loaded
    or   eax, 10000000h                             ;FACILITY_NT_BIT
    mov  DWORD PTR nExitCode$[ebp], eax             ;HRESULT_FROM_NT
    jmp  @@exit

@@: ;find Initialize function address
    GetDataPtr eax, @@aInitialize
    push eax
    push DWORD PTR hMod$[ebp]
    call GetProcedureAddress
    mov  DWORD PTR fnInitialize$[ebp], eax
    test eax, eax
    jne  @F
@@proc_not_found:
    mov  DWORD PTR nExitCode$[ebp], 8007007Fh       ;HRESULT(ERROR_PROC_NOT_FOUND)
    jmp  @@exit_free_dll

@@: ;call initialize
    mov  eax, DWORD PTR lpInitData$[ebp]
    push DWORD PTR [eax].INIT_DATA.lpInitData
    call DWORD PTR fnInitialize$[ebp]     ;call dll's Initialize function (eax will have the return code)
    mov  DWORD PTR nExitCode$[ebp], eax
    test eax, eax
    je   @@exit

@@exit_free_dll:
    push DWORD PTR hMod$[ebp]
    call DWORD PTR fnLdrUnloadDll$[ebp]

@@exit:
    ;remove thread token if provided
    cmp  DWORD PTR fnNtSetInformationThread$[ebp], 0
    je   @F
    mov  eax, DWORD PTR lpInitData$[ebp]
    lea  eax, DWORD PTR [eax].INIT_DATA.hServerProcToken
    cmp  DWORD PTR [eax], 0  ;check if token was provided
    je   @F
    push ebx
    xor  eax, eax
    lea  ebx, DWORD PTR nTemp$[ebp]
    mov  DWORD PTR [ebx], eax
    push 4                   ;sizeof(HANDLE)
    push ebx                 ;addr of token
    push 5                   ;ThreadImpersonationToken
    push 0FFFFFFFEh          ;current thread
    call DWORD PTR fnNtSetInformationThread$[ebp]
    pop ebx

@@: ;done
    mov  eax, DWORD PTR nExitCode$[ebp]
    add  esp, 40h
    pop  ebp
    ret  4h

ALIGN 4
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
