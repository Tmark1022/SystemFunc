	.file	"test1.c"
	.text
	.globl	func
	.type	func, @function
func:
.LFB63:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, -4(%rsp)
	movl	%esi, -8(%rsp)
#APP
# 14 "test1.c" 1
	movl -4(%rsp), %eax
	movl -8(%rsp), %ebx
	movl %eax, -8(%rsp)
	movl %ebx, -4(%rsp)
	
# 0 "" 2
#NO_APP
	movl	-4(%rsp), %eax
	subl	-8(%rsp), %eax
	popq	%rbx
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE63:
	.size	func, .-func
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Result: %d, %d, %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB64:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$200, %esi
	movl	$10, %edi
	call	func
	movl	%eax, %r8d
	movl	$200, %ecx
	movl	$10, %edx
	leaq	.LC0(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	__printf_chk@PLT
	movl	$0, %eax
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE64:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 8.4.0-1ubuntu1~18.04) 8.4.0"
	.section	.note.GNU-stack,"",@progbits
