section .text
global asm_HashROL

asm_HashROL:
    push rcx 
    push rdx

    xor     eax, eax                ; start hash value = 0
    xor     ecx, ecx                ; symbol counter
.loop:
    mov     dl, byte [rdi + rcx]    ; dl = current symbol

    cmp     dl, 0                   ; reached end of line?
    je      .finish_hashing         ; if reached, finish hashing
    
    rol     eax, 1                  ; rotate left (shift = 1), 11001101 -> 10011011
    xor     al, dl                  ; xor last hash byte with current symbol

    inc     ecx                     ; increase counter
    jmp     .loop                   ; continue

.finish_hashing:
    pop rdx 
    pop rcx
    ret  