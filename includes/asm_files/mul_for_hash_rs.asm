section .data

;--------------------------------------------------
section .rodata

;------------------------------------------------

section .text

global Mul378551

Mul378551:
	;mov rdi, format_str
	;mov rsi, -191932321
	;mov rdx, example_str

	pop r15					; saving return address
																					
	push rdi

    mov rbx, rdi

    mov rax, rdi
    ;mov cl, 18
    shl rax, 18
    add rbx, rax

    mov rax, rdi
    ;mov cl, 16
    shl rax, 16
    add rbx, rax

    mov rax, rdi
    ;mov cl, 15
    shl rax, 15
    add rbx, rax

    mov rax, rdi
    ;mov cl, 14
    shl rax, 14
    add rbx, rax

    mov rax, rdi
    ;mov cl, 10
    shl rax, 10
    add rbx, rax

    mov rax, rdi
    ;mov cl, 9
    shl rax, 9
    add rbx, rax

    mov rax, rdi
    ;mov cl, 7
    shl rax, 7
    add rbx, rax

    mov rax, rdi
    ;mov cl, 5
    shl rax, 5
    add rbx, rax

    mov rax, rdi
    ;mov cl, 4
    shl rax, 4
    add rbx, rax

    mov rax, rdi
    ;mov cl, 2
    shl rax, 2
    add rbx, rax


    mov rax, rdi
    ;mov cl, 1
    shl rax, 1
    add rbx, rax

    mov rbx, rax

	pop rdi 				; balancing stack by deleting arg from it

	push r15				; push ret address

	ret