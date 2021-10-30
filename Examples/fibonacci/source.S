	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %rbp, -16
	movl	$1, %eax
	movl	$-1000000, %ebp
	movl	$1, %ebx
	.p2align	4, 0x90
.LBB0_1:
	movl	%eax, %r14d
	addl	%r14d, %ebx
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	movl	%r14d, %esi
	callq	printf
	incl	%ebp
	movl	%ebx, %eax
	movl	%r14d, %ebx
	jne	.LBB0_1
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4


	.section	".note.GNU-stack","",@progbits
