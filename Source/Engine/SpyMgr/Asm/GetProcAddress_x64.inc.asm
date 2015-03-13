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

.code

_TEXT SEGMENT

;---------------------------------------------------------------------------------

GetDataPtr MACRO symbol:REQ
    DB 48h, 8Dh, 05h ; lea rax, [rip + OFFSET symbol - $ - 4]
    DD OFFSET symbol - $ - 4
ENDM

;---------------------------------------------------------------------------------

IMAGE_DOS_SIGNATURE            EQU    5A4Dh     ;MZ
IMAGE_NT_SIGNATURE             EQU    00004550h ;PE00
IMAGE_NT_OPTIONAL_HDR64_MAGIC  EQU    20Bh
IMAGE_FILE_MACHINE_AMD64       EQU    8664h

;---------------------------------------------------------------------------------

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
    DataDirectory               IMAGE_DATA_DIRECTORY 16 DUP (<>)
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

;TEST_GETPROCADDRESS_ASM:
;    sub  rsp, 20h + 28h             ;locals + shadow space + return address. Size should be 0x####8h always to mantain 16-byte alignment
;    GetDataPtr @@dllname
;    mov  rcx, rax
;    call GetModuleBaseAddress
;    cmp  rax, 0
;    je   @F
;    mov  rcx, rax
;    GetDataPtr @@apiname
;    mov  rdx, rax
;    call GetProcedureAddress
;@@: add  rsp, 20h + 28h             ;locals + shadow space + return address. Size should be 0x####8h always to mantain 16-byte alignment
;    ret
;
;@@dllname:
;    ;DB 'K', 0, 'e', 0, 'r', 0, 'n', 0, 'e', 0, 'l', 0, '3', 0, '2', 0, '.', 0, 'd', 0, 'l', 0, 'l', 0, 0, 0  
;    DB 'U', 0, 's', 0, 'e', 0, 'r', 0, '3', 0, '2', 0, '.', 0, 'd', 0, 'l', 0, 'l', 0, 0, 0  
;@@apiname:
;    ;DB "GetProcAddress", 0
;    DB "CreateWindowExW", 0

;---------------------------------------------------------------------------------

;BOOL SimpleStrNICmpW(LPWSTR string1, LPWSTR string2, DWORD dwStr1Len)
SimpleStrNICmpW PROC
    ;get string1 and check for null
    test rcx, rcx
    je   @@mismatch
    mov  r9, rcx
    ;get string2 and check for null
    test rdx, rdx
    je   @@mismatch
    ;get length and check for zero
    test r8, r8
    je   @@afterloop
@@loop:
    ;compare letter
    mov  ax, WORD PTR [r9]
    mov  cx, WORD PTR [rdx]
    cmp  cx, ax
    je   @@next
    ;check letters between A-Z and a-z
    cmp  ax, 41h
    jb   @@check2
    cmp  ax, 5Ah
    jbe  @@check2_test
@@check2:
    cmp  ax, 61h
    jb   @@mismatch
    cmp  ax, 7Ah
    ja   @@mismatch
@@check2_test:
    ;compare letter case insensitive
    or   ax, 20h
    or   cx, 20h
    cmp  cx, ax
    jne  @@mismatch
@@next:
    add  r9, 2
    add  rdx, 2
    dec  r8
    jne  @@loop
@@afterloop:
    cmp  WORD PTR [rdx], 0
    jne  @@mismatch
    xor  rax, rax
    inc  rax
    ret
@@mismatch:
    xor  rax, rax
    ret
SimpleStrNICmpW ENDP

;BOOL SimpleStrCmpA(LPSTR string1, LPSTR string2)
SimpleStrCmpA PROC
    ;get string1 and check for null
    test rcx, rcx
    je   @@mismatch
    ;get string2 and check for null
    test rdx, rdx
    je   @@mismatch
@@loop:
    ;compare letter
    mov  al, BYTE PTR [rcx]
    cmp  al, BYTE PTR [rdx]
    jne  @@mismatch
    cmp  al, 0
    je   @F
    inc  rcx
    inc  rdx
    jne  @@loop
@@: ;match
    xor  rax, rax
    inc  rax
    ret
@@mismatch:
    xor  rax, rax
    ret
SimpleStrCmpA ENDP

;LPVOID GetPEB()
GetPEB PROC
    mov  rax, QWORD PTR gs:[30h]
    mov  rax, QWORD PTR [rax+60h]
    ret
GetPEB ENDP

;LPVOID GetLoaderLockAddr()
GetLoaderLockAddr PROC
    call GetPEB
    mov  rax, QWORD PTR [rax+110h]
    ret
GetLoaderLockAddr ENDP

;BOOL CheckImageType(LPVOID lpBase, LPVOID *lplpNtHdr)
CheckImageType PROC
    xor  rax, rax
    ;get lpBase and check for null
    test rcx, rcx
    je   @@end
    ;check dos header magic
    cmp  WORD PTR [rcx].IMAGE_DOS_HEADER.e_magic, IMAGE_DOS_SIGNATURE
    jne  @@end
    ;get header offset
    xor  r8, r8
    mov  r8d, DWORD PTR [rcx].IMAGE_DOS_HEADER.e_lfanew
    add  rcx, r8 ;rcx now points to NtHeader address
    ;check if we are asked to store NtHeader address 
    test rdx, rdx
    je   @F
    mov  QWORD PTR [rdx], rcx ;save it
@@: ;check image type
    cmp  WORD PTR [rcx].IMAGE_NT_HEADERS64.FileHeader.Machine, IMAGE_FILE_MACHINE_AMD64
    jne  @@end
    ;check magic
    cmp  WORD PTR [rcx].IMAGE_NT_HEADERS64.Signature, IMAGE_NT_SIGNATURE
    jne  @@end
    inc  rax
@@end:
    ret
CheckImageType ENDP

;LPVOID GetModuleBaseAddress(LPWSTR szDllNameW)
GetModuleBaseAddress PROC
szDllNameW$ = 20h+28h + 8h

    mov  QWORD PTR [rsp+8h], rcx ;save 1st parameter for later use
    sub  rsp, 20h + 28h             ;locals + shadow space + return address. Size should be 0x####8h always to mantain 16-byte alignment
    call GetPEB
    mov  rax, QWORD PTR [rax+18h] ;peb64+24 => pointer to PEB_LDR_DATA64
    test rax, rax
    je   @@not_found
    cmp  DWORD PTR [rax+4], 0h ;check PEB_LDR_DATA64.Initialize flag
    je   @@not_found
    mov  r10, rax
    add  r10, 10h ;r10 has the first link (PEB_LDR_DATA64.InLoadOrderModuleList.Flink)
    mov  rbx, QWORD PTR [r10]
@@loop:
    cmp  rbx, r10
    je   @@not_found
    ;check if this is the entry we are looking for...
    movzx r8, WORD PTR [rbx].MODULE_ENTRY64.BaseDllName._Length
    test r8, r8
    je   @@next
    shr  r8, 1 ;divide by 2 because they are unicode chars
    mov  rcx, QWORD PTR [rbx].MODULE_ENTRY64.BaseDllName.Buffer
    test rcx, rcx
    je   @@next
    mov  rdx, QWORD PTR szDllNameW$[rsp]
    CALL SimpleStrNICmpW
    test rax, rax
    je   @@next
    ;got it
    mov  rcx, QWORD PTR [rbx].MODULE_ENTRY64.DllBase
    xor  rdx, rdx
    call CheckImageType
    test rax, rax
    je   @@next
    mov  rax, QWORD PTR [rbx].MODULE_ENTRY64.DllBase
    jmp  @@found
@@next:
    mov  rbx, QWORD PTR [rbx].MODULE_ENTRY64.InLoadOrderLinks.Flink ;go to the next entry
    jmp  @@loop
@@not_found:
    xor  rax, rax
@@found:
    add  rsp, 20h + 28h
    ret
GetModuleBaseAddress ENDP

;LPVOID GetProcedureAddress(LPVOID lpDllBase, LPSTR szFuncNameA)
GetProcedureAddress PROC
lpDllBase$ = 40h+28h + 8h
szFuncNameA$ = 40h+28h + 10h
_lpNtHdr$ = 32
_nNamesCount$ = 40
_lpAddrOfNames$ = 48

    mov  QWORD PTR [rsp+8h], rcx ;save 1st parameter for later use
    mov  QWORD PTR [rsp+10h], rdx ;save 2nd parameter for later use
    sub  rsp, 38h + 28h             ;locals + shadow space + return address. Size should be 0x####8h always to mantain 16-byte alignment
    push r13
    ;check szFuncNameA for null
    test rdx, rdx
    je   @@not_found
    ;get module base address and check for null
    test rcx, rcx
    je   @@not_found
    ;get nt header
    lea  rdx, QWORD PTR _lpNtHdr$[rsp]
    call CheckImageType
    test rax, rax
    je   @@not_found
    ;check export data directory
    mov  rax, QWORD PTR _lpNtHdr$[rsp]
    cmp  DWORD PTR [rax].IMAGE_NT_HEADERS64.OptionalHeader.DataDirectory[0]._Size, 0
    je   @@not_found
    xor  r8, r8
    mov  r8d, DWORD PTR [rax].IMAGE_NT_HEADERS64.OptionalHeader.DataDirectory[0].VirtualAddress
    test r8d, r8d
    je   @@not_found
    add  r8, QWORD PTR lpDllBase$[rsp]
    ;get the number of names
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.NumberOfNames
    mov  QWORD PTR _nNamesCount$[rsp], rax
    ;get the AddressOfNames
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfNames
    add  rax, QWORD PTR lpDllBase$[rsp]
    mov  QWORD PTR _lpAddrOfNames$[rsp], rax
    ;main loop
    xor  r13, r13
@@loop:
    cmp  r13, QWORD PTR _nNamesCount$[rsp]
    jae  @@not_found
    ;get exported name
    mov  rdx, QWORD PTR szFuncNameA$[rsp]
    mov  rax, QWORD PTR _lpAddrOfNames$[rsp]
    xor  rcx, rcx
    mov  ecx, DWORD PTR [rax]
    add  rcx, QWORD PTR lpDllBase$[rsp]
    call SimpleStrCmpA
    test rax, rax
    je   @@next
    ;got the function
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals
    add  rax, QWORD PTR lpDllBase$[rsp]
    shl  r13, 1
    add  rax, r13
    xor  rcx, rcx
    mov  cx, WORD PTR [rax] ;get the ordinal of this function
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfFunctions
    add  rax, QWORD PTR lpDllBase$[rsp]
    shl  rcx, 2
    add  rcx, rax
    ;get the function address
    xor  rax, rax
    mov  eax, DWORD PTR [rcx]
    add  rax, QWORD PTR lpDllBase$[rsp]
    jmp  @@found
@@next:
    add  QWORD PTR _lpAddrOfNames$[rsp], 4
    inc  r13
    jmp  @@loop
@@not_found:
    xor  rax, rax
@@found:
    pop  r13
    add  rsp, 38h + 28h
    ret
GetProcedureAddress ENDP

;---------------------------------------------------------------------------------

_TEXT ENDS
