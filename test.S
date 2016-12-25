.text
.global test
test:
	pushl %eax
	pushl %ebx
	movl %eax,%ebx
	movl %ebx,handle
	pusha
	pushf
	call copy
	popf
	popa
	popl %ebx
	popl %eax
	jmp *next_call_dlopen_addr
	

	
