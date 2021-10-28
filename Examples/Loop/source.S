	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset %rbx, -16
	movl	$12, %ebx
	.p2align	4, 0x90
.LBB0_1:
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	movl	%ebx, %esi
	callq	printf
	decl	%ebx
	jne	.LBB0_1
	popq	%rbx
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
