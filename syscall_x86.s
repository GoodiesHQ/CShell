.intel_syntax noprefix
.text
.globl cshell1, cshell2, cshell3, cshell4, cshell5, cshell6

cshell1:
cshell2:
cshell3:
cshell4:
cshell5:
cshell6:
	push ebp
	mov ebp, esp
	mov eax, [ebp+0x08]
	mov ebx, [ebp+0x0c]
	mov ecx, [ebp+0x10]
	mov edx, [ebp+0x14]
	mov esi, [ebp+0x18]
	mov edi, [ebp+0x1c]
	push ebp
	mov ebp, [ebp+0x20]
	int 0x80
	pop ebp
	mov esp, ebp
	pop ebp
	ret
