#include <syscall-trap.h>

.weak syscall_trap
.type syscall_trap,@function

syscall_trap:
	.cfi_startproc
	endbr64
	syscall
	retq $128
	.cfi_endproc
	.size syscall_trap,.-syscall_trap
