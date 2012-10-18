.data
extern Memory:dq
Mem     equ r8
Jump    equ r9
Count   equ r10
Countw  equ r10w
Rand    equ r11
Src     equ rsi
Dest    equ rdi
V       equ Mem
VF      equ Mem + 0fh
Font    equ Mem + 10h
SFont   equ Mem + 60h
Stack   equ Mem + 100h
Keys    equ Mem + 1d0h
Counter equ Mem + 1d8h
Delay   equ Mem + 1e0h
CDelay  equ Mem + 1e4h
Sound   equ Mem + 1e8h
CSound  equ Mem + 1ech
Large   equ Mem + 1f0h
Over    equ Mem + 1f1h
Pixels  equ Mem + 1000h
jnxxx   equ Jump
j00nx   equ Jump + 80h
.code
LogicLoop proc
    mov Mem, offset [Memory]
    mov Src, offset [Memory + 200h]
    mov Dest, offset [Memory]
    mov Jump, offset [JumpTable]
    rdtsc
    mov Rand, rax
    align 8h
incc:
    add qword ptr [Counter], 10000h
    rdtsc
    xor Count, rax
    jmp incdone
    align 8h
logloop:
    dec Countw
    jz incc
incdone:
    movzx rax, word ptr [Src]
    add Src, 2h
    xchg ah, al
    mov rbx, rax
    shr rbx, 0ch
    and rax, 0fffh
    jmp qword ptr [jnxxx + rbx * 8h]
    align 8h
i0xxx:
    test ah, ah
    jnz inone
    mov rbx, rax
    shr rbx, 4h
    and rax, 0fh
    jmp qword ptr [j00nx + rbx * 8h]
    align 8h
i1xxx:
    lea Src, qword ptr [Mem + rax]
    jmp logloop
    align 8h
i2xxx:
    push rsi
    lea rsi, qword ptr [Mem + rax]
    jmp logloop
    align 8h
i3xxx:
    mov rbx, rax
    shr rbx, 8h
    cmp byte ptr [r8 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i4xxx:
    mov rbx, rax
    shr rbx, 8h
    cmp byte ptr [r8 + rbx], al
    je logloop
    add rsi, 2h
    jmp logloop
    align 8h
i5xxx:
    mov rbx, rax
    shr rax, 4h
    and rax, 0fh
    shr rbx, 8h
    mov al, byte ptr [r8 + rax]
    cmp byte ptr [r8 + rbx], al
    jne logloop
    add rsi, 2h
    jmp logloop
    align 8h
i6xxx:
    mov rbx, rax
    shr rbx, 8h
    mov byte ptr [r8 + rbx], al
    jmp logloop
    align 8h
i7xxx:
    mov rbx, rax
    shr rbx, 8h
    add byte ptr [r8 + rbx], al
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
    xor r13, r14
    movzx ebx, ah
    mov rdx, r13
    and rdx, rax
    mov byte ptr [r12 + rbx], dl
    jmp logloop
    align 8h
idxxx: ;0xyn -> rcx = x, rdx = y, rax = n
    mov rcx, rax
    shr rcx, 8h
    movzx rcx, byte ptr [r12 + rcx]
    and rax, 0ffh
    mov rdx, rax
    shr rdx, 4h
    movzx rdx, byte ptr [r12 + rdx]
    and rax, 0fh
dloop:
    and rdx, 3fh
    dec rax
    jnz dloop
    jmp logloop ; Todo: Draw sprite
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
    pxor xmm0, xmm0
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
inone:
    int 3
    jmp logloop
    align 8h
JumpTable:
    dq i0xxx, i1xxx, i2xxx, i3xxx, i4xxx, i5xxx, i6xxx, i7xxx, i8xxx, i9xxx, iaxxx, ibxxx, icxxx, idxxx, inone, inone ; inxxx
    dq inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00cx, inone, i00ex, i00fx ; i00nx
    dq i00e0, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00ee, inone ; i00en
    dq inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00fb, i00fc, i00fd, i00fe, i00ff ; i00fn
    dq i8xx0, i8xx1, i8xx2, i8xx3, i8xx4, i8xx5, i8xx6, i8xx7, inone, inone, inone, inone, inone, inone, i8xxe, inone ; i8xxn
LogicLoop endp
end