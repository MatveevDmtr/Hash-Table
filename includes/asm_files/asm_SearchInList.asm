
section .text
extern avx_strcmp
global asm_SearchInList

asm_SearchInList:
    pop r15

    push rdx            ; avx word
    push rsi            ; word
    push rdi            ; list

    push rcx            ; save regs
    push r9
    ;push rax

    mov r9, [rdi]         ; r9 = list->head

    mov rcx, [rdi+0x8]    ; counter = list size
    ;inc rcx
.loop_list:
    mov rdi, [r9+0x10]
    mov rsi, rdx
    call avx_strcmp     ; cmp words

    test eax, eax       ; equal?
    jne .word_found
    mov r9, [r9+0x8]
    loop .loop_list

.word_not_found:
    mov rax, 1
    ; found!
    jmp .finish_search

.word_found:
    mov rax, 0
    ; not found

.finish_search:
    ;pop rax
    pop r9
    pop rcx             ; save regs

    pop rdi             ; list
    pop rsi             ; word
    pop rdx             ; avx word

    push r15
    ret 