	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	.cfi_offset %r15, -16
	movl	$1, %eax
	movq	$-40, %r14
	movl	$1, %ebx
	.p2align	4, 0x90
.LBB0_1:
	movq	%rax, %r15
	addq	%rax, %rbx
	movl	$.L__unnamed_1, %edi
	movq	%rbx, %rsi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	%rbx, %rax
	movq	%r15, %rbx
	incq	%r14
	jne	.LBB0_1
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
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
