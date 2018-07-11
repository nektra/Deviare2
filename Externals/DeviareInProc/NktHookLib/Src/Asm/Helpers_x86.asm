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

.386
.model flat, stdcall
.code

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


IMAGE_DOS_SIGNATURE        EQU 5A4Dh     ;MZ
IMAGE_NT_SIGNATURE         EQU 00004550h ;PE00
IMAGE_FILE_MACHINE_AMD64   EQU 8664h

CONTEXT STRUCT
    ;Control flags
    ContextFlags  DWORD ?
    ;Debug registers
    _Dr0          DWORD ?
    _Dr1          DWORD ?
    _Dr2          DWORD ?
    _Dr3          DWORD ?
    _Dr6          DWORD ?
    _Dr7          DWORD ?
    ;Float registres
    FloatSave     BYTE 112 DUP (?)
    ;Segment Registers and processor flags
    SegGs         DWORD ?
    SegFs         DWORD ?
    SegEs         DWORD ?
    SegDs         DWORD ?
    ;Integer registers
    _Edi          DWORD ?
    _Esi          DWORD ?
    _Ebx          DWORD ?
    _Edx          DWORD ?
    _Ecx          DWORD ?
    _Eax          DWORD ?
    _Ebp          DWORD ?
    _Eip          DWORD ?
    SegCs         DWORD ?
    EFlags        DWORD ?
    _Esp          DWORD ?
    SegSs         DWORD ?
    ;The rest is not used
CONTEXT ENDS

EXCEPTION_RECORD STRUCT
    ExceptionCode         DWORD ?
    ExceptionFlags        DWORD ?
    ExceptionRecord       DWORD ?
    ExceptionAddress      DWORD ?
    NumberParameters      DWORD ?
    __unusedAlignment     DWORD ?
    ExceptionInformation  DWORD 15 DUP(?)
EXCEPTION_RECORD ENDS

MYSEH STRUCT
    PrevLink        DWORD ?
    CurrentHandler  DWORD ?
    SafeOffset      DWORD ?
    PrevEsp         DWORD ?
    PrevEbp         DWORD ?
MYSEH ENDS

UNICODE_STRING64 STRUCT 8
    _Length       WORD  ?
    MaximumLength WORD  ?
    Buffer        QWORD ?
UNICODE_STRING64 ENDS

LIST_ENTRY64 STRUCT
    Flink QWORD ?
    Blink QWORD ?
LIST_ENTRY64 ENDS

MODULE_ENTRY64 STRUCT
    InLoadOrderLinks           LIST_ENTRY64 <>
    InMemoryOrderLinks         LIST_ENTRY64 <>
    InInitializationOrderLinks LIST_ENTRY64 <>
    DllBase                    QWORD ?
    EntryPoint                 QWORD ?
    SizeOfImage                QWORD ?
    FullDllName                UNICODE_STRING64 <>
    BaseDllName                UNICODE_STRING64 <>
    Flags                      DWORD ?
    LoadCount                  WORD  ?
    ;structure continues but it is not needed
MODULE_ENTRY64 ENDS

IMAGE_DATA_DIRECTORY STRUCT
    VirtualAddress DWORD ?
    _Size          DWORD ?
IMAGE_DATA_DIRECTORY ENDS

IMAGE_DOS_HEADER STRUCT
    e_magic    WORD  ?
    e_cblp     WORD  ?
    e_cp       WORD  ?
    e_crlc     WORD  ?
    e_cparhdr  WORD  ?
    e_minalloc WORD  ?
    e_maxalloc WORD  ?
    e_ss       WORD  ?
    e_sp       WORD  ?
    e_csum     WORD  ?
    e_ip       WORD  ?
    e_cs       WORD  ?
    e_lfarlc   WORD  ?
    e_ovno     WORD  ?
    e_res      DW 4 DUP (<?>)
    e_oemid    WORD  ?
    e_oeminfo  WORD  ?
    e_res2     DW 10 DUP (<?>)
    e_lfanew   DWORD ?
IMAGE_DOS_HEADER ENDS

IMAGE_FILE_HEADER STRUCT
    Machine              WORD  ?
    NumberOfSections     WORD  ?
    TimeDateStamp        DWORD ?
    PointerToSymbolTable DWORD ?
    NumberOfSymbols      DWORD ?
    SizeOfOptionalHeader WORD  ?
    Characteristics      WORD  ?
IMAGE_FILE_HEADER ENDS

IMAGE_OPTIONAL_HEADER64 STRUCT 
    Magic                       WORD  ?
    MajorLinkerVersion          BYTE  ?
    MinorLinkerVersion          BYTE  ?
    SizeOfCode                  DWORD ?
    SizeOfInitializedData       DWORD ?
    SizeOfUninitializedData     DWORD ?
    AddressOfEntryPoint         DWORD ?
    BaseOfCode                  DWORD ?
    ImageBase                   QWORD ?
    SectionAlignment            DWORD ?
    FileAlignment               DWORD ?
    MajorOperatingSystemVersion WORD  ?
    MinorOperatingSystemVersion WORD  ?
    MajorImageVersion           WORD  ?
    MinorImageVersion           WORD  ?
    MajorSubsystemVersion       WORD  ?
    MinorSubsystemVersion       WORD  ?
    Win32VersionValue           DWORD ?
    SizeOfImage                 DWORD ?
    SizeOfHeaders               DWORD ?
    CheckSum                    DWORD ?
    Subsystem                   WORD  ?
    DllCharacteristics          WORD  ?
    SizeOfStackReserve          QWORD ?
    SizeOfStackCommit           QWORD ?
    SizeOfHeapReserve           QWORD ?
    SizeOfHeapCommit            QWORD ?
    LoaderFlags                 DWORD ?
    NumberOfRvaAndSizes         DWORD ?
    ExportDataDirectory         IMAGE_DATA_DIRECTORY <>
    ExtraDataDirectory          IMAGE_DATA_DIRECTORY 15 DUP (<>)
IMAGE_OPTIONAL_HEADER64 ENDS

IMAGE_NT_HEADERS64 STRUCT 
    Signature      DWORD ?
    FileHeader     IMAGE_FILE_HEADER <>
    OptionalHeader IMAGE_OPTIONAL_HEADER64 <>
IMAGE_NT_HEADERS64 ENDS

IMAGE_EXPORT_DIRECTORY STRUCT
    Characteristics       DWORD ?
    TimeDateStamp         DWORD ?
    MajorVersion          WORD  ?
    MinorVersion          WORD  ?
    _Name                 DWORD ?
    Base                  DWORD ?
    NumberOfFunctions     DWORD ?
    NumberOfNames         DWORD ?
    AddressOfFunctions    DWORD ?
    AddressOfNames        DWORD ?
    AddressOfNameOrdinals DWORD ?
IMAGE_EXPORT_DIRECTORY ENDS

;---------------------------------------------------------------------------------

REX_W EQU 48h

EnterX64 MACRO
    DB 6Ah, 33h                            ;push 0x33
    DB 0E8h, 0, 0, 0, 0                    ;call $+5
    DB 83h, 4h, 24h, 5h                     ;add  DWORD PTR [esp], 5
    DB 0CBh                                ;retf
    ENDM

ExitX64 MACRO
    DB 0E8h, 0, 0, 0, 0                    ;call $+5
    DB 0C7h, 44h, 24h, 4h, 23h, 0, 0, 0    ;mov  DWORD PTR [rsp+4], 0x23
    DB 83h, 4h, 24h, 0Dh                   ;add  QWORD PTR [rsp], 13
    DB 0CBh                                ;retf
    ENDM

;---------------------------------------------------------------------------------

PUBLIC NktHookLib_TryMemCopy

NktHookLib_TryMemCopy_SEH PROTO C, :DWORD,:DWORD,:DWORD,:DWORD
.SAFESEH NktHookLib_TryMemCopy_SEH

ALIGN 4
;SIZE_T __stdcall NktHookLib_TryMemCopy(__in LPVOID lpDest, __in LPVOID lpSrc, __in SIZE_T nCount);
NktHookLib_TryMemCopy PROC STDCALL USES ebx ecx esi edi, lpDest:DWORD, lpSrc:DWORD, nCount:DWORD
_lpDest$ = 8
_lpSrc$ = 12
_nCount$ = 16

    ASSUME FS:NOTHING
    push offset NktHookLib_TryMemCopy_SEH
    push fs:[0h]
    mov  fs:[0h], esp ;install SEH

    mov  ebx, DWORD PTR _nCount$[ebp]
    mov  esi, DWORD PTR _lpSrc$[ebp]
    mov  edi, DWORD PTR _lpDest$[ebp]
    mov  ecx, ebx
    test edi, 7 ;destination aligned?
    jne  @slowPath
    test esi, 7 ;source aligned?
    jne  @slowPath
    cmp  ecx, 4
    jbe  @slowPath
@@:
    mov  eax, DWORD PTR [esi]
    add  esi, 4
    mov  DWORD PTR [edi], eax
    add  edi, 4
    sub  ecx, 4
    cmp  ecx, 4
    jae  @B
@slowPath:
    test ecx, ecx
    je   NktHookLib_TryMemCopy_AfterCopy
    mov  al, BYTE PTR [esi]
    inc  esi
    mov  BYTE PTR [edi], al
    inc  edi
    dec  ecx
    jmp  @slowPath

NktHookLib_TryMemCopy_AfterCopy::
    pop  fs:[0h] ;uninstall SEH
    pop  eax     ;
    mov  eax, ebx
    sub  eax, ecx
    ret
NktHookLib_TryMemCopy ENDP

NktHookLib_TryMemCopy_SEH PROC C USES ecx, pExcept:DWORD, pFrame:DWORD, pContext:DWORD, pDispatch:DWORD
    mov  ecx, pContext
    lea  eax, OFFSET NktHookLib_TryMemCopy_AfterCopy
    mov  [ecx].CONTEXT._Eip, eax
    xor  eax, eax ;ExceptionContinueExecution
    ret
NktHookLib_TryMemCopy_SEH ENDP

;---------------------------------------------------------------------------------

PUBLIC NktHookLib_TryCallOneParam

NktHookLib_TryCallOneParam_SEH PROTO C, :DWORD,:DWORD,:DWORD,:DWORD
.SAFESEH NktHookLib_TryCallOneParam_SEH

ALIGN 4
;SIZE_T __stdcall NktHookLib_TryCallOneParam(__in LPVOID lpFunc, __in SIZE_T nParam1, __in BOOL bIsCDecl);
NktHookLib_TryCallOneParam PROC STDCALL lpFunc:DWORD, nParam1:DWORD, bIsCDecl:DWORD
.safeseh NktHookLib_TryCallOneParam_SEH
LOCAL seh:MYSEH

    ;install SEH
    ASSUME FS:NOTHING
    push fs:[0h]
    pop  seh.PrevLink
    mov  seh.CurrentHandler, OFFSET NktHookLib_TryCallOneParam_SEH
    mov  seh.SafeOffset, OFFSET NktHookLib_TryCallOneParam_AfterCall
    lea  eax, seh
    mov  fs:[0], eax
    mov  seh.PrevEsp, esp
    mov  seh.PrevEbp, ebp
    ;do call
    mov  eax, nParam1
    push eax
    mov  eax, lpFunc
    cmp  bIsCDecl, 0
    jne  @isCDecl
    call eax
    jmp  NktHookLib_TryCallOneParam_AfterCall
@isCDecl:
    call eax
    add  esp, 4h
NktHookLib_TryCallOneParam_AfterCall::
    ;uninstall SEH
    push seh.PrevLink
    pop  fs:[0]
    ret
NktHookLib_TryCallOneParam ENDP

NktHookLib_TryCallOneParam_SEH PROC C USES edx, pExcept:DWORD, pFrame:DWORD, pContext:DWORD, pDispatch:DWORD
;@@:
;jmp @B
    mov  edx, pExcept
    ASSUME EDX:ptr EXCEPTION_RECORD
    test [edx].ExceptionFlags, EXCEPTION_UNWINDING
    je   @F
    mov  eax, 1
    jmp  done
@@: mov  edx, pFrame
    ASSUME EDX:ptr MYSEH
    mov  eax, pContext
    ASSUME EAX:ptr CONTEXT
    push [edx].SafeOffset
    pop  [eax]._Eip
    push [edx].PrevEsp
    pop  [eax]._Esp
    push [edx].PrevEbp
    pop  [eax]._Ebp
    xor  eax, eax ;ExceptionContinueExecution
done:
    ret
NktHookLib_TryCallOneParam_SEH ENDP

;---------------------------------------------------------------------------------

PUBLIC NktHookLib_InitializeWow64

ALIGN 4
;DWORD __stdcall NktHookLib_InitializeWow64()
NktHookLib_InitializeWow64 PROC
    push ebx
    push ecx
    push edx
    push esi
    push ebp
    mov  ebp, esp
    and  esp, 0FFFFFFF0h
    EnterX64

ASSUME GS:NOTHING
    DB   65h, REX_W, 0A1h, 30h,0,0,0,0,0,0,0  ;mov rax, QWORD PTR gs:[30h]  ;TEB
ASSUME GS:ERROR
    DB   REX_W
    mov  eax, DWORD PTR [eax+60h] ;PEB
    DB   REX_W
    mov  eax, DWORD PTR [eax+18h] ;peb64+24 => pointer to PEB_LDR_DATA64

    DB   REX_W
    test eax, eax
    je   @@not_found

    cmp  DWORD PTR [eax+4], 0h ;check PEB_LDR_DATA64.Initialize flag
    je   @@not_found

    DB   REX_W
    mov  esi, eax
    DB   REX_W
    add  esi, 10h ;esi has the first link (PEB_LDR_DATA64.InLoadOrderModuleList.Flink)
    DB   REX_W
    mov  ebx, DWORD PTR [esi]
@@loop:
    DB   REX_W
    cmp  ebx, esi
    je   @@not_found
    ;check if this is the entry we are looking for...
    DB   REX_W
    movzx ecx, WORD PTR [ebx].MODULE_ENTRY64.BaseDllName._Length
    DB   REX_W
    cmp  ecx, 12h
    jne  @@next
    DB   REX_W
    mov  eax, DWORD PTR [ebx].MODULE_ENTRY64.BaseDllName.Buffer
    DB   REX_W
    test eax, eax
    je   @@next
    ;check name
    mov  ecx, DWORD PTR [eax]
    or   ecx, 00200020h ;convert to lowercase
    cmp  ecx, 0074006Eh ; "nt"?
    jne  @@next
    mov  ecx, DWORD PTR [eax+4]
    or   ecx, 00200020h ;convert to lowercase
    cmp  ecx, 006C0064h ; "dl"?
    jne  @@next
    mov  ecx, DWORD PTR [eax+8]
    or   ecx, 00000020h ;convert to lowercase
    cmp  ecx, 002E006Ch ; "l."?
    jne  @@next
    mov  ecx, DWORD PTR [eax+12]
    or   ecx, 00000020h ;convert to lowercase
    cmp  ecx, 006C0064h ; "dl"?
    jne  @@next
    mov  cx, WORD PTR [eax+16]
    or   cx, 0020h ;convert to lowercase
    cmp  cx, 006Ch ; "l"?
    jne  @@next
    ;got it
    DB   REX_W
    mov  edx, DWORD PTR MODULE_ENTRY64.DllBase[ebx] ;RDX has base address
    ;check dos header magic
    cmp  WORD PTR IMAGE_DOS_HEADER.e_magic[edx], IMAGE_DOS_SIGNATURE
    jne  @@next
    ;get header offset
    DB   REX_W
    xor  eax, eax
    mov  eax, DWORD PTR IMAGE_DOS_HEADER.e_lfanew[edx]
    DB   REX_W
    mov  ecx, edx
    DB   REX_W
    add  ecx, eax ;RCX now points to NtHeader address
    ;check if we are asked to store NtHeader address
    DB   REX_W
    test ecx, ecx
    je   @@next
    ;check image type
    cmp  WORD PTR [ecx].IMAGE_NT_HEADERS64.FileHeader.Machine, IMAGE_FILE_MACHINE_AMD64
    jne  @@next
    ;check magic
    cmp  WORD PTR [ecx].IMAGE_NT_HEADERS64.Signature, IMAGE_NT_SIGNATURE
    je   @@found

@@next:
    DB   REX_W
    mov  ebx, DWORD PTR [ebx].MODULE_ENTRY64.InLoadOrderLinks.Flink ;go to the next entry
    jmp  @@loop
@@not_found:
    xor  eax, eax
@@found:
    ;at this point RCX contains the PE header of 64-bit ntdll.dll and RDX the base address
    DB   REX_W
    mov  ecx, edx

GetX64Proc MACRO _apiName, _apiAddress
    DB   REX_W
    xor  edx, edx
    mov  edx, OFFSET _apiName
    call GetProcedureAddress64
    DB   REX_W
    xor  edx, edx
    mov  edx, OFFSET _apiAddress
    DB   REX_W
    mov  DWORD PTR [edx], eax
    ENDM

    GetX64Proc apiNtQueryInformationProcess, NktHookLib_fnNtQueryInformationProcess64
    GetX64Proc apiNtQueryInformationThread,  NktHookLib_fnNtQueryInformationThread64
    GetX64Proc apiNtSetInformationThread,    NktHookLib_fnNtSetInformationThread64
    GetX64Proc apiNtCreateThreadEx,          NktHookLib_fnNtCreateThreadEx64
    GetX64Proc apiNtGetContextThread,        NktHookLib_fnNtGetContextThread64
    GetX64Proc apiNtSetContextThread,        NktHookLib_fnNtSetContextThread64
    GetX64Proc apiNtAllocateVirtualMemory,   NktHookLib_fnNtAllocateVirtualMemory64
    GetX64Proc apiNtFreeVirtualMemory,       NktHookLib_fnNtFreeVirtualMemory64
    GetX64Proc apiNtFlushVirtualMemory,      NktHookLib_fnNtFlushVirtualMemory64
    GetX64Proc apiNtReadVirtualMemory,       NktHookLib_fnNtReadVirtualMemory64
    GetX64Proc apiNtWriteVirtualMemory,      NktHookLib_fnNtWriteVirtualMemory64
    GetX64Proc apiNtProtectVirtualMemory,    NktHookLib_fnNtProtectVirtualMemory64
    GetX64Proc apiNtFlushInstructionCache,   NktHookLib_fnNtFlushInstructionCache64
    GetX64Proc apiNtQueueApcThread,          NktHookLib_fnNtQueueApcThread64
    ;all was ok
    DB   REX_W
    xor  eax, eax
    DB   REX_W, 0FFh, 0C0h ;inc rax

@@end:
    ExitX64

    mov  esp, ebp
    pop  ebp
    pop  esi
    pop  edx
    pop  ecx
    pop  ebx
    ret
NktHookLib_InitializeWow64 ENDP

ALIGN 4
apiNtQueryInformationProcess:
    DB "NtQueryInformationProcess", 0
apiNtQueryInformationThread:
    DB "NtQueryInformationThread", 0
apiNtSetInformationThread:
    DB "NtSetInformationThread", 0
apiNtCreateThreadEx:
    DB "NtCreateThreadEx", 0
apiNtGetContextThread:
    DB "NtGetContextThread", 0
apiNtSetContextThread:
    DB "NtSetContextThread", 0
apiNtDuplicateObject:
    DB "NtDuplicateObject", 0
apiNtAllocateVirtualMemory:
    DB "NtAllocateVirtualMemory", 0
apiNtFreeVirtualMemory:
    DB "NtFreeVirtualMemory", 0
apiNtFlushVirtualMemory:
    DB "NtFlushVirtualMemory", 0
apiNtReadVirtualMemory:
    DB "NtReadVirtualMemory", 0
apiNtWriteVirtualMemory:
    DB "NtWriteVirtualMemory", 0
apiNtProtectVirtualMemory:
    DB "NtProtectVirtualMemory", 0
apiNtFlushInstructionCache:
    DB "NtFlushInstructionCache", 0
apiNtQueueApcThread:
    DB "NtQueueApcThread", 0
apiNtClose:
    DB "NtClose", 0

ALIGN 4
;LPVOID __stdcall GetProcedureAddress64(LPVOID lpDllBase, LPCSTR szFuncNameA)
GetProcedureAddress64 PROC
    push ecx
    push edx
    DB   41h, 50h ;push R8
    DB   41h, 51h ;push R9
    DB   41h, 52h ;push R10
    DB   41h, 53h ;push R11
    push esi
    push edi

    ;get header offset
    DB   REX_W
    xor  eax, eax
    mov  eax, DWORD PTR [ecx].IMAGE_DOS_HEADER.e_lfanew
    DB   REX_W+4
    mov  edx, ecx ;mov R10, rcx
    DB   REX_W+4
    add  edx, eax ;R10 now points to NtHeader address

    ;check export data directory
    DB   41h
    cmp  DWORD PTR IMAGE_NT_HEADERS64.OptionalHeader.ExportDataDirectory._Size[edx], 0
    je   @@not_found
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR IMAGE_NT_HEADERS64.OptionalHeader.ExportDataDirectory.VirtualAddress[edx]
    DB   REX_W
    test eax, eax
    je   @@not_found
    DB   REX_W
    add  eax, ecx
    DB   REX_W+4
    mov  edx, eax ;mov R10, rax ;R10 now points to IMAGE_EXPORT_DIRECTORY

    ;get the number of names
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR IMAGE_EXPORT_DIRECTORY.NumberOfNames[edx]
    DB   REX_W+4
    mov  ecx, eax ;R9 has the names count

    ;get the AddressOfNames
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR IMAGE_EXPORT_DIRECTORY.AddressOfNames[edx]
    DB   REX_W+4
    mov  ebx, eax
    DB   REX_W+4
    add  ebx, ecx ;R11 has the address of names

    ;main loop
    DB   REX_W
    xor  esi, esi ;ESI is the counter
@@loop:
    DB   REX_W+1
    cmp  esi, ecx ; RSI>=R9
    jae  @@not_found
    ;get exported name
    push ecx
    push edx
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR [ebx] ;mov eax, DWORD PTR [rdx]
    DB   REX_W
    add  ecx, eax
    call SimpleStrICmpA
    pop  edx
    pop  ecx
    DB   REX_W
    test eax, eax
    je   @@next
    ;got the function
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR IMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals[edx]
    DB   REX_W
    add  eax, ecx
    DB   REX_W
    shl  esi, 1
    DB   REX_W
    add  eax, esi ;add rax, rsi
    DB   REX_W
    xor  edx, edx
    mov  dx, WORD PTR [eax] ;get the ordinal of this function
    DB   REX_W
    xor  eax, eax
    DB   41h
    mov  eax, DWORD PTR IMAGE_EXPORT_DIRECTORY.AddressOfFunctions[edx]
    DB   REX_W
    add  eax, ecx
    DB   REX_W
    shl  edx, 2
    DB   REX_W
    add  edx, eax
    ;get the function address
    DB   REX_W
    xor  eax, eax
    mov  eax, DWORD PTR [edx]
    DB   REX_W
    add  eax, ecx
    jmp  @@found
@@next:
    DB   REX_W+1
    add  ebx, 4 ;R11 has the address of names
    DB   REX_W, 0FFh, 0C6h ;inc RSI
    jmp  @@loop
@@not_found:
    DB   REX_W
    xor  eax, eax
@@found:
    pop  edi
    pop  esi
    DB   41h, 5Bh ;pop R11
    DB   41h, 5Ah ;pop R10
    DB   41h, 59h ;pop R9
    DB   41h, 58h ;pop R8
    pop  edx
    pop  ecx
    ret
GetProcedureAddress64 ENDP

SimpleStrICmpA PROC
    ;get string1 and check for null
    DB   REX_W
    test ecx, ecx
    je   @@mismatch
    ;get string2 and check for null
    DB   REX_W
    test edx, edx
    je   @@mismatch
@@loop:
    ;compare letter
    mov  al, BYTE PTR [ecx]
    mov  ah, BYTE PTR [edx]
    ;check letters between A-Z and a-z
    cmp  al, 41h
    jb   @F
    cmp  al, 5Ah
    ja   @F
    or   al, 20h
@@: cmp  ah, 41h
    jb   @F
    cmp  ah, 5Ah
    ja   @F
    or   ah, 20h
@@: cmp  al, ah
    jne  @@mismatch
    cmp  al, 0
    je   @F
    DB   REX_W, 0FFh, 0C1h ;inc rcx
    DB   REX_W, 0FFh, 0C2h ;inc rdx
    jmp  @@loop
@@: ;match
    DB   REX_W
    xor  eax, eax
    DB   REX_W, 0FFh, 0C0h ;inc rax
    ret
@@mismatch:
    DB   REX_W
    xor  eax, eax
    ret
SimpleStrICmpA ENDP

;---------------------------------------------------------------------------------

PUBLIC NktHookLib_CallWow64

ALIGN 4
NktHookLib_CallWow64 PROC STDCALL api:QWORD,p1:QWORD,p2:QWORD,p3:QWORD,p4:QWORD,p5:QWORD,
                                  p6:QWORD,p7:QWORD,p8:QWORD,p9:QWORD,p10:QWORD,p11:QWORD
_api$ = 8
_param1$ = 16
_param2$ = 24
_param3$ = 32
_param4$ = 40
_param5$ = 48
_param6$ = 56
_param7$ = 64
_param8$ = 72
_param9$ = 80
_param10$ = 88
_param11$ = 96

    and  esp, 0FFFFFFF0h           ;align stack to 16 bytes

    sub  esp, 12                   ;to maintain 16-byte alignment with the following pushes
    push ebx
    push ecx
    push edx
    push esi
    push edi

    EnterX64

    DB   41h, 52h                  ;push R10
    DB   41h, 53h                  ;push R11
    DB   41h, 54h                  ;push R12
    DB   41h, 55h                  ;push R13
    DB   41h, 56h                  ;push R14
    DB   41h, 57h                  ;push R15
    push ebx                       ;push RBX
    push ebp                       ;push RBP
    push esi                       ;push RSI
    push edi                       ;push RDI
    DB   REX_W                     ;sub rsp, 0x60
    sub  esp, 60h

    DB   REX_W                     ;mov  rcx, QWORD PTR _param1$[rbp]
    mov  ecx, DWORD PTR _param1$[ebp]
    DB   REX_W                     ;mov  rdx, QWORD PTR _param2$[rbp]
    mov  edx, DWORD PTR _param2$[ebp]
    DB   REX_W+4                   ;mov  r8, QWORD PTR _param3$[rbp]
    mov  eax, DWORD PTR _param3$[ebp]
    DB   REX_W+4                   ;mov  r9, QWORD PTR _param4$[rbp]
    mov  ecx, DWORD PTR _param4$[ebp]
    DB   REX_W                     ;mov  rax, QWORD PTR _param5$[rbp]
    mov  eax, DWORD PTR _param5$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x20], rax
    mov  DWORD PTR [esp+20h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param6$[rbp]
    mov  eax, DWORD PTR _param6$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x28], rax
    mov  DWORD PTR [esp+28h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param7$[rbp]
    mov  eax, DWORD PTR _param7$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x30], rax
    mov  DWORD PTR [esp+30h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param8$[rbp]
    mov  eax, DWORD PTR _param8$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x38], rax
    mov  DWORD PTR [esp+38h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param9$[rbp]
    mov  eax, DWORD PTR _param9$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x40], rax
    mov  DWORD PTR [esp+40h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param10$[rbp]
    mov  eax, DWORD PTR _param10$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x48], rax
    mov  DWORD PTR [esp+48h], eax
    DB   REX_W                     ;mov  rax, QWORD PTR _param11$[rbp]
    mov  eax, DWORD PTR _param11$[ebp]
    DB   REX_W                     ;mov  QWORD PTR [rsp+0x50], rax
    mov  DWORD PTR [esp+50h], eax

    call DWORD PTR _api$[ebp]      ;call QWORD _api$[rbp]

    DB   REX_W                     ;add rsp, 0x60
    add  esp, 60h
    pop  edi                       ;pop RDI
    pop  esi                       ;pop RSI
    pop  ebp                       ;pop RBP
    pop  ebx                       ;pop RBX
    DB   41h, 5Fh                  ;pop R15
    DB   41h, 5Eh                  ;pop R14
    DB   41h, 5Dh                  ;pop R13
    DB   41h, 5Ch                  ;pop R12
    DB   41h, 5Bh                  ;pop R11
    DB   41h, 5Ah                  ;pop R10

    ExitX64

    pop  edi                       ;restore registers
    pop  esi                       ;the 'leave' at the epilog will restore stack pointer
    pop  edx                       ;and original alignment
    pop  ecx
    pop  ebx

    ret
NktHookLib_CallWow64 ENDP


;NktHookLib_QueryBasicInfoProcess64 PROC USES ecx edx esi edi, Process:DWORD, param1:DWORD
;_Process$ = 8
;_ProcessInfo$ = 12
;xxx
;LOCAL pbi64:PROCESS_BASIC_INFORMATION64

 ;   lea  eax, seh.CurrentHandler
  ;  mov  ecx, DWORD PTR _Process$[ebp]

  ;  and  esp, 0FFFFFFF0h
   ; EnterX64

;		and  ecx
;		mov  edx, 0 ;ProcessBasicInformation
;		NtQueryInformationProcess(__in HANDLE Process, __in PROCESSINFOCLASS ProcessInfoClass,
;                                             __out_opt PVOID ProcessInfo, __in ULONG ProcessInfoLength,
;                                             __out_opt PULONG ReturnLength);





  ;  mov  esi, DWORD PTR _ProcessInfo$[ebp]


 ;   ret
;NktHookLib_QueryBasicInfoProcess64 ENDP

;---------------------------------------------------------------------------------

_TEXT ENDS

;---------------------------------------------------------------------------------

.data
_DATA SEGMENT 'data'
PUBLIC NktHookLib_fnNtQueryInformationProcess64
PUBLIC NktHookLib_fnNtQueryInformationThread64
PUBLIC NktHookLib_fnNtSetInformationThread64
PUBLIC NktHookLib_fnNtCreateThreadEx64
PUBLIC NktHookLib_fnNtGetContextThread64
PUBLIC NktHookLib_fnNtSetContextThread64
PUBLIC NktHookLib_fnNtAllocateVirtualMemory64
PUBLIC NktHookLib_fnNtFreeVirtualMemory64
PUBLIC NktHookLib_fnNtFlushVirtualMemory64
PUBLIC NktHookLib_fnNtReadVirtualMemory64
PUBLIC NktHookLib_fnNtWriteVirtualMemory64
PUBLIC NktHookLib_fnNtProtectVirtualMemory64
PUBLIC NktHookLib_fnNtFlushInstructionCache64
PUBLIC NktHookLib_fnNtQueueApcThread64

NktHookLib_fnNtQueryInformationProcess64 DQ 0
NktHookLib_fnNtQueryInformationThread64  DQ 0
NktHookLib_fnNtSetInformationThread64    DQ 0
NktHookLib_fnNtCreateThreadEx64          DQ 0
NktHookLib_fnNtGetContextThread64        DQ 0
NktHookLib_fnNtSetContextThread64        DQ 0
NktHookLib_fnNtAllocateVirtualMemory64   DQ 0
NktHookLib_fnNtFreeVirtualMemory64       DQ 0
NktHookLib_fnNtFlushVirtualMemory64      DQ 0
NktHookLib_fnNtReadVirtualMemory64       DQ 0
NktHookLib_fnNtWriteVirtualMemory64      DQ 0
NktHookLib_fnNtProtectVirtualMemory64    DQ 0
NktHookLib_fnNtFlushInstructionCache64   DQ 0
NktHookLib_fnNtQueueApcThread64          DQ 0

_DATA ENDS

END
