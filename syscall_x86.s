.intel_syntax noprefix
.text
.globl cshell1, cshell2, cshell3, cshell4, cshell5, cshell6

cshell1:
cshell2:
cshell3:
cshell4:
cshell5:
cshell6:
	//; create a stack frame to preserve previous base pointer
	push ebp
	mov ebp, esp

	//; move all possible arguments into gp registers
	mov eax, [ebp+0x08]
	mov ebx, [ebp+0x0c]
	mov ecx, [ebp+0x10]
	mov edx, [ebp+0x14]
	mov esi, [ebp+0x18]
	mov edi, [ebp+0x1c]

	//; save current ebp
	push ebp
	mov ebp, [ebp+0x20]

	//; kernel int
	int 0x80

	//; restore ebp and destroy the stack frame
	pop ebp
	mov esp, ebp
	pop ebp
	ret
