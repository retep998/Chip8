.data
extern Memory:dq
extern Vars:dq
extern Pixels:db
V dq 0, 0
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
    mov r12, offset [V] ; Registers
    mov r13, 0
    mov r14, 0
    mov r15, 0
    jmp logloop
    align 8h
incc:
    mov qword ptr [r8], rcx
    jmp logloop
    align 10h
logloop:
    inc rcx
    test cx, cx
    jz incc
    movzx rax, word ptr [rsi]
    add rsi, 2h
    xchg ah, al
    mov rbx, rax
    shr bx, 0ch
    and ax, 0fffh
    jmp qword ptr [r10 + rbx * 8h]
    align 8h
i0xxx:
    test ah, ah
    jnz unkno
    mov bx, ax
    shr bx, 4h
    and ax, 0fh
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
i3xxx:
    mov bx, ax
    shr bx, 8h
    cmp byte ptr [r12 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i4xxx:
    mov bx, ax
    shr bx, 8h
    cmp byte ptr [r12 + rbx], al
    je logloop
    add rsi, 2h
    jmp logloop
    align 8h
i5xxx:
    mov bx, ax
    shr ax, 4h
    and ax, 0fh
    shr bx, 8h
    mov al, byte ptr [r12 + rax]
    cmp byte ptr [r12 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i6xxx:
    mov bx, ax
    shr bx, 8h
    mov byte ptr [r12 + rbx], al
    jmp logloop
    align 8h
i7xxx:
    mov bx, ax
    shr bx, 8h
    add byte ptr [r12 + rbx], al
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
    mov bx, 8h
    mov rdx, 80h
    xorps xmm0, xmm0
clearloop:
    movdqa xmmword ptr [rax + 00h], xmm0
    movdqa xmmword ptr [rax + 10h], xmm0
    movdqa xmmword ptr [rax + 20h], xmm0
    movdqa xmmword ptr [rax + 30h], xmm0
    movdqa xmmword ptr [rax + 40h], xmm0
    movdqa xmmword ptr [rax + 50h], xmm0
    movdqa xmmword ptr [rax + 60h], xmm0
    movdqa xmmword ptr [rax + 70h], xmm0
    add rax, rdx
    dec bx
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
    mov byte ptr [r8 + 0ah], 0h
    jmp logloop
    align 8h
i00ff:
    mov byte ptr [r8 + 0ah], 1h
    jmp logloop
    align 8h
unkno:
    int 3
    jmp logloop
    align 8h
jumpt:
    dq i0xxx, i1xxx, i2xxx, i3xxx, i4xxx, i5xxx, i6xxx, i7xxx, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno ; inxxx
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00cx, unkno, i00ex, i00fx ; i00nx
    dq i00e0, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00ee, unkno ; i00en
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00fb, i00fc, unkno, i00fe, i00ff ; i00fn
LogicLoop endp
end