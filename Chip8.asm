.data
extern Memory:dq
extern Vars:dq
extern Pixels:db
V dq 0, 0
.code
LogicLoop proc
    ; Seeding random numbers
    rdtsc
    xor r13, rax
    ; Variables
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    mov rsi, offset [Memory + 200h] ; Instruction pointer
    mov rdi, offset [Memory] ; Memory pointer
    ; Constants
    mov r8, offset [Vars] ; Various variables
    mov r9, offset [Memory] ; Base memory pointer
    mov r10, offset [jumpt] ; Jump table
    mov r11, offset [Pixels] ; The screen
    mov r12, offset [V] ; Registers
    xor r14, r14
    xor r15, r15
    jmp logloop
    align 8h
incc:
    mov qword ptr [r8], rcx
    rdtsc
    xor r13, rax
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
i3xxx:
    mov rbx, rax
    shr rbx, 8h
    cmp byte ptr [r12 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i4xxx:
    mov rbx, rax
    shr rbx, 8h
    cmp byte ptr [r12 + rbx], al
    je logloop
    add rsi, 2h
    jmp logloop
    align 8h
i5xxx:
    mov rbx, rax
    shr rax, 4h
    and rax, 0fh
    shr rbx, 8h
    mov al, byte ptr [r12 + rax]
    cmp byte ptr [r12 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i6xxx:
    mov rbx, rax
    shr rbx, 8h
    mov byte ptr [r12 + rbx], al
    jmp logloop
    align 8h
i7xxx:
    mov rbx, rax
    shr rbx, 8h
    add byte ptr [r12 + rbx], al
    jmp logloop
    align 8h
i8xxx:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    jmp qword ptr [r10 + rbx * 8h + 200h]
    align 8h
i9xxx:
    mov rbx, rax
    shr rax, 4h
    and rax, 0fh
    shr rbx, 8h
    mov al, byte ptr [r12 + rax]
    cmp byte ptr [r12 + rbx], al
    je logloop
    add rsi, 2h
    jmp logloop
    align 8h
iaxxx:
    lea rdi, qword ptr [r9 + rax]
    jmp logloop
    align 8h
ibxxx:
    movzx rbx, byte ptr [r12]
    add rbx, rax
    lea rsi, qword ptr [r9 + rbx]
    jmp logloop
    align 8h
icxxx:
    xor r13, rcx
    movzx ebx, ah
    mov rdx, r13
    and rdx, rax
    mov byte ptr [r12 + rbx], dl
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
    mov rbx, 8h
    mov rdx, 80h
    xorps xmm0, xmm0
    align 8h
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
i00fd:
    mov byte ptr [r8 + 0dh], 1h
waitexit:
    jmp waitexit
    align 8h
i00fe:
    mov byte ptr [r8 + 0ah], 0h
    jmp logloop
    align 8h
i00ff:
    mov byte ptr [r8 + 0ah], 1h
    jmp logloop
    align 8h
i8xx0:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rbx]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx1:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rbx]
    or byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx2:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rbx]
    and byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx3:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rbx]
    xor byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx4:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rax]
    mov bl, byte ptr [r12 + rbx]
    add dl, bl
    setc byte ptr [r12 + 0fh]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx5:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov dl, byte ptr [r12 + rax]
    mov bl, byte ptr [r12 + rbx]
    sub dl, bl
    setnc byte ptr [r12 + 0fh]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx6:
    shr rax, 4h
    mov dl, byte ptr [r12 + rax]
    shr dl, 1
    setc byte ptr [r12 + 0fh]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xx7:
    mov rbx, rax
    and rbx, 0fh
    shr rax, 4h
    mov bl, byte ptr [r12 + rax]
    mov dl, byte ptr [r12 + rbx]
    sub dl, bl
    setnc byte ptr [r12 + 0fh]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
i8xxe:
    shr rax, 4h
    mov dl, byte ptr [r12 + rax]
    shl dl, 1
    setc byte ptr [r12 + 0fh]
    mov byte ptr [r12 + rax], dl
    jmp logloop
    align 8h
unkno:
    int 3
    jmp logloop
    align 8h
jumpt:
    dq i0xxx, i1xxx, i2xxx, i3xxx, i4xxx, i5xxx, i6xxx, i7xxx, i8xxx, i9xxx, iaxxx, ibxxx, icxxx, unkno, unkno, unkno ; inxxx
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00cx, unkno, i00ex, i00fx ; i00nx
    dq i00e0, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00ee, unkno ; i00en
    dq unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, unkno, i00fb, i00fc, i00fd, i00fe, i00ff ; i00fn
    dq i8xx0, i8xx1, i8xx2, i8xx3, i8xx4, i8xx5, i8xx6, i8xx7, unkno, unkno, unkno, unkno, unkno, unkno, i8xxe, unkno ; i8xxn
LogicLoop endp
end