.data
extern Memory:dq
extern Counter:dq
.code
LogicLoop proc
    ; Variables
    mov rax, 0
    mov rbx, 0
    mov rcx, 0
    mov rdx, 0 ; Sub Counter
    mov rsi, offset [Memory + 200h] ; Instruction pointer
    mov rdi, offset [Memory] ; Memory pointer
    ; Constants
    mov r8, offset [Counter] ; Counter
    mov r9, offset [Memory] ; Base memory pointer
    mov r10, offset [jumpt] ; Jump Table
    mov r11, 0
    mov r12, 0
    mov r13, 0
    mov r14, 0
    mov r15, 0
    jmp logloop
    align 10h
jumpt:
    dq i0xxx, i1xxx, i2xxx, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno ; nxxx
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00ex, unkno ; 00nx
    align 10h
incc:
    inc qword ptr [r8]
    jmp logloop
    align 10h
logloop:
    inc dx
    jz incc
    movzx rax, word ptr [rsi]
    add rsi, 2h
    xchg ah, al
    mov rbx, rax
    shr rbx, 0ch
    and rax, 0fffh
    jmp qword ptr [r10 + rbx * 8h]
    align 10h
i0xxx:
    mov rbx, rax
    shr rbx, 4h
    jmp qword ptr [r10 + rbx * 8h + 80h]
    align 10h
i1xxx:
    lea rsi, qword ptr [r9 + rax]
    jmp logloop
    align 10h
i2xxx:
    push rsi
    lea rsi, qword ptr [r9 + rax]
    jmp logloop
    align 10h
i00ex:
    jmp logloop ; Todo: Jump to proper spots
    align 10h
i00e0:
    jmp logloop ; Todo: Clear screen
    align 10h
i00ee:
    pop rsi
    jmp logloop
    align 10h
unkno:
    int 3
    jmp logloop
LogicLoop endp
end