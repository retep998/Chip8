.data
extern Memory:dq
extern Vars:dq
extern Pixels:db
.code
LogicLoop proc
    ; Variables
    mov rax, 0
    mov rbx, 0
    mov rcx, 0
    mov rdx, 0
    mov rsi, offset [Memory + 200h] ; Instruction pointer
    mov rdi, offset [Memory] ; Memory pointer
    ; Constants
    mov r8, offset [Vars] ; Various variables
    mov r9, offset [Memory] ; Base memory pointer
    mov r10, offset [jumpt] ; Jump table
    mov r11, offset [Pixels] ; The screen
    mov r12, 0
    mov r13, 0
    mov r14, 0
    mov r15, 0
    jmp logloop
    align 8h
incc:
    inc qword ptr [r8]
    jmp logloop
    align 8h
logloop:
    inc rcx
    test cx, cx
    jz incc
    movzx rax, word ptr [rsi]
    add rsi, 2h
    xchg ah, al
    mov rbx, rax
    shr rbx, 0ch
    and rax, 0fffh
    jmp qword ptr [r10 + rbx * 8h]
    align 8h
i0xxx:
    test ah, ah
    jnz unkno
    mov rbx, rax
    shr rbx, 4h
    and rax, 0fh
    jmp qword ptr [r10 + rbx * 8h + 80h]
    align 8h
i1xxx:
    lea rsi, qword ptr [r9 + rax]
    jmp logloop
    align 8h
i2xxx:
    push rsi
    lea rsi, qword ptr [r9 + rax]
    jmp logloop
    align 8h
i00cx:
    jmp logloop ; Todo: Scroll x down
    align 8h
i00ex:
    jmp qword ptr [r10 + rax * 8h + 100h]
    align 8h
i00fx:
    jmp qword ptr [r10 + rax * 8h + 180h]
    align 8h
i00e0:
    mov rax, r11
    mov rbx, 10h
    mov rdx, 0h
clearloop:
    mov qword ptr [rax + 00h], rdx
    mov qword ptr [rax + 08h], rdx
    mov qword ptr [rax + 10h], rdx
    mov qword ptr [rax + 18h], rdx
    mov qword ptr [rax + 20h], rdx
    mov qword ptr [rax + 28h], rdx
    mov qword ptr [rax + 30h], rdx
    mov qword ptr [rax + 38h], rdx
    add rax, 40h
    dec rbx
    jnz clearloop
    jmp logloop
    align 8h
i00ee:
    pop rsi
    jmp logloop
    align 8h
i00fb:
    jmp logloop ; Todo: Scroll 4 right
    align 8h
i00fc:
    jmp logloop ; Todo: Scroll 4 left
    align 8h
i00fe:
    jmp logloop ; Todo: Scroll 4 left
    align 8h
i00ff:
    jmp logloop ; Todo: Scroll 4 left
    align 8h
unkno:
    int 3
    jmp logloop
    align 8h
jumpt:
    dq i0xxx, i1xxx, i2xxx, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno ; inxxx
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00cx, unkno, i00ex, unkno ; i00nx
    dq i00e0, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00ee, unkno ; i00en
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00fb, i00fc, unkno, unkno, unkno ; i00fn
LogicLoop endp
end