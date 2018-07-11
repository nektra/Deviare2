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

BuildFrame MACRO LocalsSize:REQ,CallerExtra:REQ,
                 SaveReg1,SaveReg2,SaveReg3,SaveReg4,SaveReg5,SaveReg6,SaveReg7,
                 SaveReg8,SaveReg9,SaveReg10,SaveReg11,SaveReg12,SaveReg13,SaveReg14
    push rbp
    SavedRegsCount=1
IFNB <SaveReg1>
    push SaveReg1
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg2>
    push SaveReg2
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg3>
    push SaveReg3
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg4>
    push SaveReg4
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg5>
    push SaveReg5
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg6>
    push SaveReg6
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg7>
    push SaveReg7
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg8>
    push SaveReg8
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg9>
    push SaveReg9
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg10>
    push SaveReg10
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg11>
    push SaveReg11
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg12>
    push SaveReg12
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg13>
    push SaveReg13
    SavedRegsCount=SavedRegsCount+1
ENDIF
IFNB <SaveReg14>
    push SaveReg14
    SavedRegsCount=SavedRegsCount+1
ENDIF

    SavedRegsPadding = ((SavedRegsCount AND 1) XOR 1) * 8
    LocalsPadding = 16 - (LocalsSize AND 15)
    IF (LocalsPadding GE 16)
        LocalsPadding = 0
    ENDIF
    CallerExtraPadding = (CallerExtra AND 1) * 8
    StackSize = LocalsSize + LocalsPadding + CallerExtra * 8 + CallerExtraPadding + SavedRegsPadding
    lea  rbp, [rsp - LocalsSize - LocalsPadding]
    sub  rsp, StackSize + 32
    OffsetHome = StackSize + 32 + SavedRegsCount*8 + 8
    ENDM

RemoveFrame MACRO SaveReg1,SaveReg2,SaveReg3,SaveReg4,SaveReg5,SaveReg6,SaveReg7,
                  SaveReg8,SaveReg9,SaveReg10,SaveReg11,SaveReg12,SaveReg13,SaveReg14

    add  rsp, StackSize + 32
IFNB <SaveReg14>
    pop  SaveReg14
ENDIF
IFNB <SaveReg13>
    pop  SaveReg13
ENDIF
IFNB <SaveReg12>
    pop  SaveReg12
ENDIF
IFNB <SaveReg11>
    pop  SaveReg11
ENDIF
IFNB <SaveReg10>
    pop  SaveReg10
ENDIF
IFNB <SaveReg9>
    pop  SaveReg9
ENDIF
IFNB <SaveReg8>
    pop  SaveReg8
ENDIF
IFNB <SaveReg7>
    pop  SaveReg7
ENDIF
IFNB <SaveReg6>
    pop  SaveReg6
ENDIF
IFNB <SaveReg5>
    pop  SaveReg5
ENDIF
IFNB <SaveReg4>
    pop  SaveReg4
ENDIF
IFNB <SaveReg3>
    pop  SaveReg3
ENDIF
IFNB <SaveReg2>
    pop  SaveReg2
ENDIF
IFNB <SaveReg1>
    pop  SaveReg1
ENDIF
    pop  rbp
    ENDM

;---------------------------------------------------------------------------------

IMAGE_DOS_SIGNATURE            EQU    5A4Dh     ;MZ
IMAGE_NT_SIGNATURE             EQU    00004550h ;PE00
IMAGE_NT_OPTIONAL_HDR64_MAGIC  EQU    20Bh
IMAGE_FILE_MACHINE_AMD64       EQU    8664h

ERROR_MOD_NOT_FOUND            EQU    126
ERROR_PROC_NOT_FOUND           EQU    127

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

;NOTE: Getting rid of the "error A2006: undefined symbol : rip" of MASM x64
GetPtr MACRO _reg:REQ, symbol:REQ, ofs:REQ
    DB   0E8h, 0, 0, 0, 0 ;call 0
    sub  QWORD PTR [rsp], OFFSET $ - (symbol + ofs)
    pop  _reg
ENDM

;---------------------------------------------------------------------------------

PUBLIC GETMODULEANDPROCADDR_SECTION_START
PUBLIC GETMODULEANDPROCADDR_SECTION_END

ALIGN 8
GETMODULEANDPROCADDR_SECTION_START:

ALIGN 8
;BOOL __stdcall SimpleStrNICmpW(LPCWSTR string1, LPCWSTR string2, SIZE_T len)
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

ALIGN 8
;BOOL __stdcall SimpleStrICmpA(LPCSTR string1, LPCSTR string2)
SimpleStrICmpA PROC
    push rbx
    xor  rax, rax
    ;get string1 and check for null
    test rcx, rcx
    je   @@done
    ;get string2 and check for null
    test rdx, rdx
    je   @@done
@@loop:
    ;compare letter
    mov  bl, BYTE PTR [rcx]
    mov  bh, BYTE PTR [rdx]
    inc  rcx
    inc  rdx
    ;check letters between A-Z and a-z
    cmp  bl, 41h
    jb   @F
    cmp  bl, 5Ah
    ja   @F
    or   bl, 20h
@@: cmp  bh, 41h
    jb   @F
    cmp  bh, 5Ah
    ja   @F
    or   bh, 20h
@@: cmp  bl, bh
    jne  @@done
    cmp  bl, 0
    jne  @@loop
    ;match
    inc  rax
@@done:
    pop  rbx
    ret
SimpleStrICmpA ENDP

ALIGN 8
;BOOL __stdcall CheckImageType(LPVOID lpBase, LPVOID *lplpNtHdr)
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

ALIGN 8
;LPVOID __stdcall GetModuleBaseAddress(LPCWSTR szDllNameW)
GetModuleBaseAddress PROC

    BuildFrame 0, 0, rbx, r10

_szDllNameW$ = OffsetHome

    mov  QWORD PTR _szDllNameW$[rsp], rcx ;save 1st parameter for later use

    mov  rax, QWORD PTR gs:[30h]  ;TEB
    mov  rax, QWORD PTR [rax+60h] ;PEB
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
    mov  rdx, QWORD PTR _szDllNameW$[rsp]
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
    RemoveFrame rbx, r10
    ret
GetModuleBaseAddress ENDP

ALIGN 8
;LPVOID __stdcall GetProcedureAddress(LPVOID lpDllBase, LPCSTR szFuncNameA)
GetProcedureAddress PROC

    BuildFrame 18h, 0, r13

_lpDllBase$ = OffsetHome
_szFuncNameA$ = OffsetHome + 8
_lpNtHdr$ = 0
_nNamesCount$ = 8
_lpAddrOfNames$ = 16

    mov  QWORD PTR _lpDllBase$[rsp], rcx ;save 1st parameter for later use
    mov  QWORD PTR _szFuncNameA$[rsp], rdx ;save 2nd parameter for later use

    ;check szFuncNameA for null
    test rdx, rdx
    je   @@not_found
    ;get module base address and check for null
    test rcx, rcx
    je   @@not_found
    ;get nt header
    lea  rdx, QWORD PTR _lpNtHdr$[rbp]
    call CheckImageType
    test rax, rax
    je   @@not_found
    ;check export data directory
    mov  rax, QWORD PTR _lpNtHdr$[rbp]
    cmp  DWORD PTR [rax].IMAGE_NT_HEADERS64.OptionalHeader.DataDirectory[0]._Size, 0
    je   @@not_found
    xor  r8, r8
    mov  r8d, DWORD PTR [rax].IMAGE_NT_HEADERS64.OptionalHeader.DataDirectory[0].VirtualAddress
    test r8d, r8d
    je   @@not_found
    add  r8, QWORD PTR _lpDllBase$[rsp]
    ;get the number of names
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.NumberOfNames
    mov  QWORD PTR _nNamesCount$[rbp], rax
    ;get the AddressOfNames
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfNames
    add  rax, QWORD PTR _lpDllBase$[rsp]
    mov  QWORD PTR _lpAddrOfNames$[rbp], rax
    ;main loop
    xor  r13, r13
@@loop:
    cmp  r13, QWORD PTR _nNamesCount$[rbp]
    jae  @@not_found
    ;get exported name
    mov  rdx, QWORD PTR _szFuncNameA$[rsp]
    mov  rax, QWORD PTR _lpAddrOfNames$[rbp]
    xor  rcx, rcx
    mov  ecx, DWORD PTR [rax]
    add  rcx, QWORD PTR _lpDllBase$[rsp]
    call SimpleStrICmpA
    test rax, rax
    je   @@next
    ;got the function
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfNameOrdinals
    add  rax, QWORD PTR _lpDllBase$[rsp]
    shl  r13, 1
    add  rax, r13
    xor  rcx, rcx
    mov  cx, WORD PTR [rax] ;get the ordinal of this function
    xor  rax, rax
    mov  eax, DWORD PTR [r8].IMAGE_EXPORT_DIRECTORY.AddressOfFunctions
    add  rax, QWORD PTR _lpDllBase$[rsp]
    shl  rcx, 2
    add  rcx, rax
    ;get the function address
    xor  rax, rax
    mov  eax, DWORD PTR [rcx]
    add  rax, QWORD PTR _lpDllBase$[rsp]
    jmp  @@found
@@next:
    add  QWORD PTR _lpAddrOfNames$[rbp], 4
    inc  r13
    jmp  @@loop
@@not_found:
    xor  rax, rax
@@found:
    RemoveFrame r13
    ret
GetProcedureAddress ENDP

GETMODULEANDPROCADDR_SECTION_END:

;---------------------------------------------------------------------------------

PUBLIC INJECTDLLINNEWPROCESS_SECTION_START
PUBLIC INJECTDLLINNEWPROCESS_SECTION_END

ALIGN 8
INJECTDLLINNEWPROCESS_SECTION_START:

ALIGN 8
InjectDllInNewProcess PROC
_GETPROCADDR_1                    EQU 0
_GETMODBASEADDR_1                 EQU 8
_DLLNAME_1                        EQU 16
_INITFUNCTION_1                   EQU 24
_ORIGINAL_ENTRYPOINT_1            EQU 32
_CHECKPOINTEVENT_1                EQU 40
_INITFUNCTIONPARAMSPTR_1          EQU 48
_INITFUNCTIONPARAMSSIZE_1         EQU 56

_ntdll_hinst$ = 0
_ntterminateprocessAddr$ = 8
_ntcloseAddr$ = 16
_ntseteventAddr$ = 24
_kernel32_hinst$ = 32
_loadlibrarywAddr$ = 40
_freelibraryAddr$ = 48
_injectdll_hinst$ = 56
_initfunctionAddr$ = 64

    db   8 DUP (0h)                                                  ;offset 0: address of GetProcedureAddress
    db   8 DUP (0h)                                                  ;offset 8: address of GetModuleBaseAddress
    db   8 DUP (0h)                                                  ;offset 16: pointer to dll name
    db   8 DUP (0h)                                                  ;offset 24: pointer to initialize function
    db   8 DUP (0h)                                                  ;offset 32: original entrypoint
    db   8 DUP (0h)                                                  ;offset 40: checkpoint event
    db   8 DUP (0h)                                                  ;offset 48: pointer to initialize function params
    db   8 DUP (0h)                                                  ;offset 56: length of initialize function params
    jmp @@start

@@ntdll_dll:
    dw   'n','t','d','l','l','.','d','l','l', 0
@@ntterminateprocess:
    db   'NtTerminateProcess', 0
@@ntclose:
    db   'NtClose', 0
@@ntsetevent:
    db   'NtSetEvent', 0
@@kernel32_dll:
    dw   'k','e','r','n','e','l','3','2','.','d','l','l', 0
@@loadlibraryw:
    db   'LoadLibraryW', 0
@@freelibrary:
    db   'FreeLibrary', 0

@@start:
    BuildFrame 50h, 0, rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;get ntdll.dll base address
    GetPtr rcx, @@ntdll_dll, 0
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _GETMODBASEADDR_1
    call QWORD PTR [rax]
    mov  QWORD PTR _ntdll_hinst$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_modnotfound_helper, 0
    jmp  @@hardfail

@@: ;get kernel32.dll base address
    GetPtr rcx, @@kernel32_dll, 0
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _GETMODBASEADDR_1
    call QWORD PTR [rax]
    mov  QWORD PTR _kernel32_hinst$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_modnotfound_helper, 0
    jmp  @@hardfail

@@: GetPtr rbx, INJECTDLLINNEWPROCESS_SECTION_START, _GETPROCADDR_1

    ;get address of NtTerminateProcess
    GetPtr rdx, @@ntterminateprocess, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _ntterminateprocessAddr$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_procnotfound_helper, 0
    jmp  @@hardfail

@@: ;get address of NtClose
    GetPtr rdx, @@ntclose, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _ntcloseAddr$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_procnotfound_helper, 0
    jmp  @@hardfail

@@: ;get address of NtSetEvent
    GetPtr rdx, @@ntsetevent, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _ntseteventAddr$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_procnotfound_helper, 0
    jmp  @@hardfail

@@: ;get address of LoadLibraryW
    GetPtr rdx, @@loadlibraryw, 0
    mov  rcx, QWORD PTR _kernel32_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _loadlibrarywAddr$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_procnotfound_helper, 0
    jmp  @@hardfail

@@: ;get address of FreeLibrary
    GetPtr rdx, @@freelibrary, 0
    mov  rcx, QWORD PTR _kernel32_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _freelibraryAddr$[rbp], rax
    test rax, rax
    jne  @F
    GetPtr rax, @@hardfail_procnotfound_helper, 0
    jmp  @@hardfail

@@: ;load library
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _DLLNAME_1
    mov rcx, QWORD PTR [rax]
    call QWORD PTR _loadlibrarywAddr$[rbp]
    mov  QWORD PTR _injectdll_hinst$[rbp], rax
    test rax, rax
    jne  @F
    ;get last error
    mov  rax, QWORD PTR gs:[30h]
    mov  eax, DWORD PTR [rax+68h]
    jmp  @@fail

@@: ;call init function if provided
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _INITFUNCTION_1
    mov  rdx, QWORD PTR [rax]
    test rdx, rdx
    je   @@done

    ;get init function address
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _GETPROCADDR_1
    call QWORD PTR [rax]
    test rax, rax
    jne  @F

    ;free library if init function was not found
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    call QWORD PTR _freelibraryAddr$[rbp]
    mov  eax, ERROR_PROC_NOT_FOUND
    jmp  @@fail

@@: ;get init function parameters (becasue x64 calling convention, if values are null and init function
    ;does not expects any parameters, they will be discarded)
    GetPtr rcx, INJECTDLLINNEWPROCESS_SECTION_START, _INITFUNCTIONPARAMSPTR_1
    GetPtr rdx, INJECTDLLINNEWPROCESS_SECTION_START, _INITFUNCTIONPARAMSSIZE_1
    mov  rcx, QWORD PTR [rcx]
    mov  rdx, QWORD PTR [rdx]
    ;call init function
    call rax
    test rax, rax
    je   @@done
    ;if init function returns an error, first free library
    push rax ;save error code
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    call QWORD PTR _freelibraryAddr$[rbp]
    pop  rax ;restore error code
    jmp  @@fail

@@done:
    ;set checkpoint event
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _CHECKPOINTEVENT_1
    mov  rbx, QWORD PTR [rax]
    test rbx, rbx
    je   @F
    xor  rdx, rdx
    mov  rcx, rbx
    call QWORD PTR _ntseteventAddr$[rbp]
    ;close checkpoint event
    mov  rcx, rbx
    call QWORD PTR _ntcloseAddr$[rbp]

@@: RemoveFrame rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;jmp to original address
    push rax
    push rax
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _ORIGINAL_ENTRYPOINT_1
    mov  rax, QWORD PTR [rax]
    mov  QWORD PTR [rsp+8], rax
    pop  rax
    ret

@@fail:
    ;on error, terminate process
    mov  rdx, rax
    xor  rcx, rcx
    dec  rcx
    call QWORD PTR _ntterminateprocessAddr$[rbp]
    int  3

@@hardfail:
    ;replace RCX (entrypoint) value with our code
    mov QWORD PTR [rsp+StackSize + 32 + 11 * 8], rax

    RemoveFrame rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;jmp to original address
    push rax
    push rax
    GetPtr rax, INJECTDLLINNEWPROCESS_SECTION_START, _ORIGINAL_ENTRYPOINT_1
    mov  rax, QWORD PTR [rax]
    mov  QWORD PTR [rsp+8], rax
    pop  rax
    ret

@@hardfail_modnotfound_helper:
    mov  eax, ERROR_MOD_NOT_FOUND
    ret

@@hardfail_procnotfound_helper:
    mov  eax, ERROR_PROC_NOT_FOUND
    ret
InjectDllInNewProcess ENDP

INJECTDLLINNEWPROCESS_SECTION_END:

;---------------------------------------------------------------------------------

PUBLIC INJECTDLLINRUNNINGPROCESS_SECTION_START
PUBLIC INJECTDLLINRUNNINGPROCESS_SECTION_END

ALIGN 8
INJECTDLLINRUNNINGPROCESS_SECTION_START:

ALIGN 8
InjectDllInRunningProcess PROC
_GETPROCADDR_2                    EQU 0
_GETMODBASEADDR_2                 EQU 8
_DLLNAME_2                        EQU 16
_INITFUNCTION_2                   EQU 24
_CONTINUEEVENT_2                  EQU 32
_INITFUNCTIONPARAMSPTR_2          EQU 40
_INITFUNCTIONPARAMSSIZE_2         EQU 48

_ntdll_hinst$ = 0
_ntseteventAddr$ = 8
_kernel32_hinst$ = 16
_loadlibrarywAddr$ = 24
_freelibraryAddr$ = 32
_injectdll_hinst$ = 40
_initfunctionAddr$ = 48

    db   8 DUP (0h)                                                  ;offset 0: address of GetProcedureAddress
    db   8 DUP (0h)                                                  ;offset 8: address of GetModuleBaseAddress
    db   8 DUP (0h)                                                  ;offset 16: pointer to dll name
    db   8 DUP (0h)                                                  ;offset 24: pointer to initialize function
    db   8 DUP (0h)                                                  ;offset 32: continue event handle
    db   8 DUP (0h)                                                  ;offset 40: pointer to initialize function params
    db   8 DUP (0h)                                                  ;offset 48: length of initialize function params
    jmp @@start

@@ntdll_dll:
    dw   'n','t','d','l','l','.','d','l','l', 0
@@ntsetevent:
    db   'NtSetEvent', 0
@@kernel32_dll:
    dw   'k','e','r','n','e','l','3','2','.','d','l','l', 0
@@loadlibraryw:
    db   'LoadLibraryW', 0
@@freelibrary:
    db   'FreeLibrary', 0

@@start:
    BuildFrame 38h, 1, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;get ntdll.dll base address
    GetPtr rcx, @@ntdll_dll, 0
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _GETMODBASEADDR_2
    call QWORD PTR [rax]
    mov  QWORD PTR _ntdll_hinst$[rbp], rax
    test rax, rax
    jne  @F
    mov  eax, ERROR_MOD_NOT_FOUND
    jmp  @@exit

@@: ;get kernel32.dll base address
    GetPtr rcx, @@kernel32_dll, 0
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _GETMODBASEADDR_2
    call QWORD PTR [rax]
    mov  QWORD PTR _kernel32_hinst$[rbp], rax
    test rax, rax
    jne  @F
    mov  eax, ERROR_MOD_NOT_FOUND
    jmp  @@exit

@@: GetPtr rbx, INJECTDLLINRUNNINGPROCESS_SECTION_START, _GETPROCADDR_2

    ;get address of NtSetEvent
    GetPtr rdx, @@ntsetevent, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _ntseteventAddr$[rbp], rax
    test rax, rax
    jne  @F
    mov  eax, ERROR_PROC_NOT_FOUND
    jmp  @@exit

@@: ;get address of LoadLibraryW
    GetPtr rdx, @@loadlibraryw, 0
    mov  rcx, QWORD PTR _kernel32_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _loadlibrarywAddr$[rbp], rax
    test rax, rax
    jne  @F
    mov  eax, ERROR_PROC_NOT_FOUND
    jmp  @@exit

@@: ;get address of FreeLibrary
    GetPtr rdx, @@freelibrary, 0
    mov  rcx, QWORD PTR _kernel32_hinst$[rbp]
    call QWORD PTR [rbx]
    mov  QWORD PTR _freelibraryAddr$[rbp], rax
    test rax, rax
    jne  @F
    mov  eax, ERROR_PROC_NOT_FOUND
    jmp  @@exit

@@: ;load library
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _DLLNAME_2
    mov rcx, QWORD PTR [rax]
    call QWORD PTR _loadlibrarywAddr$[rbp]
    mov  QWORD PTR _injectdll_hinst$[rbp], rax
    test rax, rax
    jne  @F
    ;get last error
    mov  rax, QWORD PTR gs:[30h]
    mov  eax, DWORD PTR [rax+68h]
    jmp  @@exit

@@: ;call init function if provided
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _INITFUNCTION_2
    mov  rdx, QWORD PTR [rax]
    test rdx, rdx
    je   @@done

    ;get init function address
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _GETPROCADDR_2
    call QWORD PTR [rax]
    test rax, rax
    jne  @F

    ;free library if init function was not found
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    call QWORD PTR _freelibraryAddr$[rbp]
    mov  eax, ERROR_PROC_NOT_FOUND
    jmp  @@exit

@@: ;get init function parameters (becasue x64 calling convention, if values are null and init function
    ;does not expects any parameters, they will be discarded)
    GetPtr rcx, INJECTDLLINRUNNINGPROCESS_SECTION_START, _INITFUNCTIONPARAMSPTR_2
    GetPtr rdx, INJECTDLLINRUNNINGPROCESS_SECTION_START, _INITFUNCTIONPARAMSSIZE_2
    mov  rcx, QWORD PTR [rcx]
    mov  rdx, QWORD PTR [rdx]
    ;call init function
    call rax
    test rax, rax
    je   @@done
    ;if init function returns an error, first free library
    push rax ;save error code
    mov  rcx, QWORD PTR _injectdll_hinst$[rbp]
    call QWORD PTR _freelibraryAddr$[rbp]
    pop  rax ;restore error code
    jmp  @@exit

@@done:
    ;set continue event
    GetPtr rax, INJECTDLLINRUNNINGPROCESS_SECTION_START, _CONTINUEEVENT_2
    mov  rbx, QWORD PTR [rax]
    test rbx, rbx
    je   @F
    xor  rdx, rdx
    mov  rcx, rbx
    call QWORD PTR _ntseteventAddr$[rbp]

@@: ;no error
    xor  eax, eax

@@exit:
    RemoveFrame rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;return and exit thread
    ret
InjectDllInRunningProcess ENDP

INJECTDLLINRUNNINGPROCESS_SECTION_END:

;---------------------------------------------------------------------------------

PUBLIC WAITFOREVENTATSTARTUP_SECTION_START
PUBLIC WAITFOREVENTATSTARTUP_SECTION_END

ALIGN 8
WAITFOREVENTATSTARTUP_SECTION_START:

ALIGN 8
;VOID __cdecl WaitForEventAtStartup(LPVOID lpApcArgument1, LPVOID lpApcArgument2, LPVOID lpApcArgument3)
WaitForEventAtStartup PROC
_GETPROCADDR_3                    EQU 0
_GETMODBASEADDR_3                 EQU 8
_LOADERTHREAD_3                   EQU 16
_CONTINUEEVENT_3                  EQU 24
_CONTROLLERPROC_3                 EQU 32

_ntdll_hinst$ = 0
_ntcloseAddr$ = 8
_ntresumethreadAddr$ = 16
_ntwaitformultipleobjectsAddr$ = 24

    db   8 DUP (0h)                                                  ;offset 0: address of GetProcedureAddress
    db   8 DUP (0h)                                                  ;offset 8: address of GetModuleBaseAddress
    db   8 DUP (0h)                                                  ;offset 16: loader thread handle
    db   8 DUP (0h)                                                  ;offset 24: continue event handle
    db   8 DUP (0h)                                                  ;offset 32: controller process handle
    jmp  @@start

@@ntdll_dll:
    dw   'n','t','d','l','l','.','d','l','l', 0
@@ntclose:
    db   'NtClose', 0
@@ntwaitformultipleobjects:
    db   'NtWaitForMultipleObjects', 0
@@ntresumethread:
    db   'NtResumeThread', 0

@@start:
    BuildFrame 20h, 1, rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi

    ;get ntdll.dll base address
    GetPtr rcx, @@ntdll_dll, 0
    GetPtr rax, WAITFOREVENTATSTARTUP_SECTION_START, _GETMODBASEADDR_3
    call QWORD PTR [rax]
    test rax, rax
    je   @@done
    mov  QWORD PTR _ntdll_hinst$[rbp], rax

    GetPtr rbx, WAITFOREVENTATSTARTUP_SECTION_START, _GETPROCADDR_3

    ;get address of NtClose
    GetPtr rdx, @@ntclose, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    test rax, rax
    je   @@done
    mov  QWORD PTR _ntcloseAddr$[rbp], rax

    ;get address of NtWaitForMultipleObjects
    GetPtr rdx, @@ntwaitformultipleobjects, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    test rax, rax
    je   @@done
    mov  QWORD PTR _ntwaitformultipleobjectsAddr$[rbp], rax

    ;get address of NtResumeThread
    GetPtr rdx, @@ntresumethread, 0
    mov  rcx, QWORD PTR _ntdll_hinst$[rbp]
    call QWORD PTR [rbx]
    test rax, rax
    je   @@done
    mov  QWORD PTR _ntresumethreadAddr$[rbp], rax

    ;resume secondary thread
    GetPtr rbx, WAITFOREVENTATSTARTUP_SECTION_START, _LOADERTHREAD_3
    xor  rdx, rdx
    mov  rcx, QWORD PTR [rbx]
    test rcx, rcx
    je   @F
    call QWORD PTR _ntresumethreadAddr$[rbp]

@@: ;wait for continue event or controller process termination
    mov  rcx, 2
    GetPtr rdx, WAITFOREVENTATSTARTUP_SECTION_START, _CONTINUEEVENT_3
    mov  r8, 1 ;WaitAnyObject
    xor  r9, r9 ;FALSE
    mov  QWORD PTR [rsp+20h], 0
    call QWORD PTR _ntwaitformultipleobjectsAddr$[rbp]

    ;close continue event
    GetPtr rax, WAITFOREVENTATSTARTUP_SECTION_START, _CONTINUEEVENT_3
    mov  rcx, QWORD PTR [rax]
    call QWORD PTR _ntcloseAddr$[rbp]

    ;close controller process
    GetPtr rax, WAITFOREVENTATSTARTUP_SECTION_START, _CONTROLLERPROC_3
    mov  rcx, QWORD PTR [rax]
    call QWORD PTR _ntcloseAddr$[rbp]

@@done:
    RemoveFrame rax, rbx, rcx, rdx, r8, r9, r10, r11, r12, r13, r14, r15, rsi, rdi
    ret
WaitForEventAtStartup ENDP

WAITFOREVENTATSTARTUP_SECTION_END:

;---------------------------------------------------------------------------------

_TEXT ENDS

END
