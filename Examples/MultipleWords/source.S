	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$42, %edi
	callq	.Lprint
	movl	$42, %edi
	callq	.Lprint
	movl	$42, %edi
	callq	.Lprint
	popq	%rax
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lprint,@function
.Lprint:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, %ecx
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	movl	%ecx, %esi
	callq	printf
	popq	%rax
	retq
.Lfunc_end1:
	.size	.Lprint, .Lfunc_end1-.Lprint
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4


	.section	".note.GNU-stack","",@progbits
