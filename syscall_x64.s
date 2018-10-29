.intel_syntax noprefix
.text
.globl cshell1, cshell2, cshell3, cshell4, cshell5, cshell6

cshell1:
cshell2:
cshell3:
cshell4:
cshell5:
cshell6:
	//; create a stack frame
	push rbp
	mov rbp, rsp

	//; move all parameters to set up a syscall
	mov rax, rdi
	mov rdi, rsi
	mov rsi, rdx
	mov rdx, rcx
	mov r10, r8
	mov r8,  r9
	mov r9, [rbp + 0x10]

	//; execute the syscall
	syscall

	//; destroy the stack frame
	mov rsp, rbp
	pop rbp

	ret
