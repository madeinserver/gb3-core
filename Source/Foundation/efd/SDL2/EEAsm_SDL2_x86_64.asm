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

%ifdef EE_PLATFORM_WIN32
%define arg1 rcx
%define arg2 rdx
%define arg3 r8
%elif EE_PLATFORM_LINUX
%define arg1 rdi
%define arg2 rsi
%define arg3 rdx
%else
%error "Unsupported platform!"
%endif

_ee_sincos_asm:
    push rbp
    mov rsp, rbp
    sub rsp, 8

    movsd qword [rsp+8], xmm0
    fld qword [rsp+8]
    fsincos
    fstp qword [arg2]
    fstp qword [arg3]

    mov rbp, rsp
    pop rbp
    leave
    ret

_ee_cpuid_support:
    push rbx
    
    xor rax, rax
    cpuid ; eax -> out

    pop rbx
    ret

_ee_cpuid_extfamilymodel:
    push rbx

    mov eax, 1
    cpuid ; eax -> out
    
    pop rbx
    ret

_ee_cpuid_maxcorephys:
    push rbx

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
    mov rax, 1 ; just one core

multi_core1:
    pop rbx
    ret

_ee_cpuid_querycachetype:
    push rbx

	mov r8, arg1
    xor eax, eax
    cpuid
    cmp eax, 4 ; check if cpuid supports leaf 4
    jl multi_core2 ; Single core
    mov eax, 4
    mov ecx, r8d ; start with index = 0; Leaf 4 reports
    cpuid ; at least one valid cache level
    ; result is in eax

multi_core2:
    pop rbx
    ret

_ee_cpuid_genuineintel:
    push rbx

	mov r8, arg1

    xor eax, eax
    cpuid
    
    mov dword [r8], ebx
    mov dword [r8+8], edx
    mov dword [r8+16], ecx

    pop rbx
    ret

_ee_cpuid_maskwidth:
    mov rax, [arg1]
    mov rcx, 0
    mov [arg2], rcx
    dec rax
    bsr cx, ax
    jz next
    inc cx
    mov [arg2], rcx

next:
    ret

_ee_cpuid_apicid:
    push rbx

    mov eax, 1
    cpuid
    mov eax, ebx

    pop rbx
    ret

_ee_cpuid1:
    push rbx

    mov eax, 1
    cpuid
    mov eax, edx

    pop rbx
    ret
