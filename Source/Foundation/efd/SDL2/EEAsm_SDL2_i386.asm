; EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
;
; This software is supplied under the terms of a license agreement or
; nondisclosure agreement with Emergent Game Technologies and may not
; be copied or disclosed except in accordance with the terms of that
; agreement.
;
;      Copyright (c) 2022-2023 Arves100/Made In Server Developers.
;      Copyright (c) 1996-2009 Emergent Game Technologies.
;      All Rights Reserved.
;
; Emergent Game Technologies, Calabasas, CA 91302
; http://www.emergent.net

default rel

global _ee_sincos_asm
global _ee_cpuid_support
global _ee_cpuid_extfamilymodel
global _ee_cpuid_querycachetype
global _ee_cpuid_apicid
global _ee_cpuid_maskwidth
global _ee_cpuid1
global _ee_cpuid_genuineintel
global _ee_cpuid_maxcorephys

section .text

_ee_sincos_asm:
    push ebp
    mov ebp, esp

    fld dword [ebp+8]
    fsincos
    fstp dword [ebp+12]
    fstp dword [ebp+16]

    pop ebp
    ret

_ee_cpuid_support:
    push ebp
    push ebx
    
    xor eax, eax
    cpuid ; eax -> out

    pop ebx
    pop ebp
    ret

_ee_cpuid_extfamilymodel:
    push ebp
    push ebx

    mov eax, 1
    cpuid ; eax -> out
    
    pop ebx
    pop ebp
    ret

_ee_cpuid_maxcorephys:
    push ebp
    push ebx

    xor eax, eax
    cpuid
    cmp eax, 4 ; check if cpuid supports leaf 4
    jl single_core1 ; Single core
    mov eax, 4
    mov ecx, 0
    cpuid ; at least one valid cache level
    ; result is in eax
    jmp multi_core1

single_core1:
    mov eax, 1 ; just one core

multi_core1:
    pop ebx
    pop ebp
    ret

_ee_cpuid_querycachetype:
    push ebp
    mov ebp, esp
    push ebx

    xor eax, eax
    cpuid
    cmp eax, 4 ; check if cpuid supports leaf 4
    jl multi_core2 ; Single core
    mov eax, 4
    mov ecx, [ebp+8] ; start with index = 0; Leaf 4 reports
    cpuid ; at least one valid cache level
    ; result is in eax

multi_core2:
    pop ebx
    pop ebp
    ret

_ee_cpuid_genuineintel:
    push ebp
    mov ebp, esp
    push ebx

    xor eax, eax
    cpuid
    
    mov eax, [ebp+8]
    mov [eax], ebx
    mov [eax+4], edx
    mov [eax+8], ecx

    pop ebx
    pop ebp
    ret

_ee_cpuid_maskwidth:
    push ebp
    mov ebp, esp

    mov eax, [ebp+8]
    mov ecx, 0
    mov [ebp+12], ecx
    dec eax
    bsr cx, ax
    jz next
    inc cx
    mov [ebp+12], ecx

next:
    pop ebp
    ret

_ee_cpuid_apicid:
    push ebp
    push ebx

    mov eax, 1
    cpuid
    mov eax, ebx

    pop ebx
    pop ebp 
    ret

_ee_cpuid1:
    push ebp
    push ebx

    mov eax, 1
    cpuid
    mov eax, edx

    pop ebx
    pop ebp
    ret
