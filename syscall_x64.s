.intel_syntax noprefix
.text
.globl cshell1, cshell2, cshell3, cshell4, cshell5, cshell6

cshell1:
cshell2:
cshell3:
cshell4:
cshell5:
cshell6:
	mov rax,rdi
	mov rdi,rsi
	mov rsi,rdx
	mov rdx,rcx
	mov r10,r8
	mov r8,r9
	mov r9, [rsp + 0x10]
	syscall
	ret
