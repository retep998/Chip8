seg8 segment page execute

extern Memory:dq
Src     equ rsi
Dest    equ rdi
Mem     equ r8
Jump    equ r9
Count   equ r10
Rand    equ r11
CLoop   equ r12
V       equ Mem
VF      equ Mem + 0fh
Font    equ Mem + 10h
SFont   equ Mem + 60h
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
j00en   equ Jump + 100h
j00fn   equ Jump + 180h
j8xxn   equ Jump + 200h

LogicLoop proc
    mov Src, offset [Memory + 200h]
    mov Dest, offset [Memory]
    mov Mem, offset [Memory]
    mov Jump, offset [JumpTable]
    mov CLoop, offset [ChipLoop]
incc:
    mov qword ptr [Counter], Count
    rdtsc
    xor Rand, rax
    rol Rand, 1
    jmp incdone
    align 10h
ChipLoop:
    inc Count
    test Count, 0ffffh
    jz incc
incdone:
    movzx eax, word ptr [Src]
    add Src, 2h
    rol ax, 8
    mov ebx, eax
    shr ebx, 0ch
    and eax, 0fffh
    jmp qword ptr [jnxxx + rbx * 8h]
    align 10h
i0xxx:
    mov ebx, eax
    shr ebx, 4h
    and eax, 0fh
    jmp qword ptr [j00nx + rbx * 8h]
    align 10h
i1xxx:
    lea Src, qword ptr [Mem + rax]
    jmp CLoop
    align 10h
i2xxx:
    push Src
    lea Src, qword ptr [Mem + rax]
    jmp CLoop
    align 10h
i3xxx:
    mov ebx, eax
    shr ebx, 8h
    cmp byte ptr [V + rbx], al
    jne noskip3
    add Src, 2h
noskip3:
    jmp CLoop
    align 10h
i4xxx:
    mov ebx, eax
    shr ebx, 8h
    cmp byte ptr [V + rbx], al
    je noskip4
    add Src, 2h
noskip4:
    jmp CLoop
    align 10h
i5xxx:
    mov ebx, eax
    shr eax, 4h
    and eax, 0fh
    shr ebx, 8h
    movzx eax, byte ptr [V + rax]
    cmp byte ptr [V + rbx], al
    jne noskip5
    add Src, 2h
noskip5:
    jmp CLoop
    align 10h
i6xxx:
    mov ebx, eax
    shr ebx, 8h
    mov byte ptr [V + rbx], al
    jmp CLoop
    align 10h
i7xxx:
    mov ebx, eax
    shr ebx, 8h
    add byte ptr [V + rbx], al
    jmp CLoop
    align 10h
i8xxx:
    mov ebx, eax
    mov ecx, eax
    and ebx, 0fh
    shr ecx, 4h
    shr eax, 8h
    and ecx, 0fh
    jmp qword ptr [j8xxn + rbx * 8h]
    align 10h
i9xxx:
    mov ebx, eax
    shr eax, 4h
    and eax, 0fh
    shr ebx, 8h
    movzx eax, byte ptr [V + rax]
    cmp byte ptr [V + rbx], al
    je noskip9
    add Src, 2h
noskip9:
    jmp CLoop
    align 10h
iaxxx:
    lea Dest, qword ptr [Mem + rax]
    jmp CLoop
    align 10h
ibxxx:
    movzx ebx, byte ptr [V]
    add ebx, eax
    lea Src, qword ptr [Mem + rbx]
    jmp CLoop
    align 10h
icxxx:
    xor Rand, Count
    rol Rand, 1
    movzx ebx, ah
    mov rdx, Rand
    and edx, eax
    mov byte ptr [V + rbx], dl
    jmp CLoop
    align 10h
idxxx: ; TODO
    jmp CLoop
    align 10h
iexxx: ; TODO
    jmp CLoop
    align 10h
ifxxx: ; TODO
    jmp CLoop
    align 10h
i00cx:
    jmp CLoop ; Todo: Scroll x down
    align 10h
i00ex:
    jmp qword ptr [j00en + rax * 8h]
    align 10h
i00fx:
    jmp qword ptr [j00fn + rax * 8h]
    align 10h
i00e0:
    lea rax, qword ptr [Pixels]
    mov ebx, 2h
    cmp byte ptr [Large], 0
    jz sclear
    mov ebx, 8h
sclear:
    pxor xmm0, xmm0
    mov edx, 80h
clearl:
    movaps xmmword ptr [rax + 00h], xmm0
    movaps xmmword ptr [rax + 10h], xmm0
    movaps xmmword ptr [rax + 20h], xmm0
    movaps xmmword ptr [rax + 30h], xmm0
    movaps xmmword ptr [rax + 40h], xmm0
    movaps xmmword ptr [rax + 50h], xmm0
    movaps xmmword ptr [rax + 60h], xmm0
    movaps xmmword ptr [rax + 70h], xmm0
    add rax, rdx
    dec ebx
    jnz clearl
    jmp CLoop
    align 10h
i00ee:
    pop rsi
    jmp CLoop
    align 10h
i00fb:
    jmp CLoop ; Todo: Scroll 4 right
    align 10h
i00fc:
    jmp CLoop ; Todo: Scroll 4 left
    align 10h
i00fd:
    mov byte ptr [Over], 1h
waitexit:
    pause
    jmp waitexit
    align 10h
i00fe:
    mov byte ptr [Large], 0h
    jmp CLoop
    align 10h
i00ff:
    mov byte ptr [Large], 1h
    jmp CLoop
    align 10h
i8xx0:
    mov dl, byte ptr [V + rcx]
    mov byte ptr [V + rax], dl
    jmp CLoop
    align 10h
i8xx1:
    mov dl, byte ptr [V + rcx]
    or byte ptr [V + rax], dl
    jmp CLoop
    align 10h
i8xx2:
    mov dl, byte ptr [V + rcx]
    and byte ptr [V + rax], dl
    jmp CLoop
    align 10h
i8xx3:
    mov dl, byte ptr [V + rcx]
    xor byte ptr [V + rax], dl
    jmp CLoop
    align 10h
i8xx4:
    mov dl, byte ptr [V + rcx]
    add byte ptr [V + rax], dl
    setc byte ptr [VF]
    jmp CLoop
    align 10h
i8xx5:
    mov dl, byte ptr [V + rcx]
    sub byte ptr [V + rax], dl
    setnc byte ptr [VF]
    jmp CLoop
    align 10h
i8xx6:
    shr byte ptr [V + rax], 1
    setc byte ptr [VF]
    jmp CLoop
    align 10h
i8xx7:
    mov dl, byte ptr [V + rax]
    sub dl, byte ptr [V + rcx]
    mov byte ptr [V + rax], dl
    setnc byte ptr [VF]
    jmp CLoop
    align 10h
i8xxe:
    shl byte ptr [V + rax], 1
    setc byte ptr [VF]
    jmp CLoop
    align 10h
inone:
    int 3
    jmp CLoop
    align 10h
JumpTable:
    dq i0xxx, i1xxx, i2xxx, i3xxx, i4xxx, i5xxx, i6xxx, i7xxx, i8xxx, i9xxx, iaxxx, ibxxx, icxxx, idxxx, inone, inone ; inxxx
    dq inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00cx, inone, i00ex, i00fx ; i00nx
    dq i00e0, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00ee, inone ; i00en
    dq inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, inone, i00fb, i00fc, i00fd, i00fe, i00ff ; i00fn
    dq i8xx0, i8xx1, i8xx2, i8xx3, i8xx4, i8xx5, i8xx6, i8xx7, inone, inone, inone, inone, inone, inone, i8xxe, inone ; i8xxn
LogicLoop endp

end