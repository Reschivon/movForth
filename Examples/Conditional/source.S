	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$1, %edi
	callq	.Lconditional
	popq	%rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lconditional,@function
.Lconditional:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	testl	%edi, %edi
	movl	$42, %eax
	movl	$32, %ebx
	cmovel	%eax, %ebx
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	popq	%rbx
	retq
.Lfunc_end1:
	.size	.Lconditional, .Lfunc_end1-.Lconditional
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4


	.section	".note.GNU-stack","",@progbits
