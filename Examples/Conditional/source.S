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
	pushq	%rax
	.cfi_def_cfa_offset 16
	testl	%edi, %edi
	je	.LBB1_2
	movq	%rsp, %rdi
	leaq	4(%rsp), %rsi
	callq	.Lgen32
	jmp	.LBB1_3
.LBB1_2:
	movq	%rsp, %rdi
	leaq	4(%rsp), %rsi
	callq	.Lgen42
.LBB1_3:
	movl	4(%rsp), %edi
	movl	%edi, %esi
	callq	.Lprint
	movl	(%rsp), %edi
	movl	%edi, %esi
	callq	.Lprint
	popq	%rax
	retq
.Lfunc_end1:
	.size	.Lconditional, .Lfunc_end1-.Lconditional
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lgen32,@function
.Lgen32:
	.cfi_startproc
	movl	$32, (%rdi)
	movl	$32, (%rsi)
	retq
.Lfunc_end2:
	.size	.Lgen32, .Lfunc_end2-.Lgen32
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lgen42,@function
.Lgen42:
	.cfi_startproc
	movl	$42, (%rdi)
	movl	$42, (%rsi)
	retq
.Lfunc_end3:
	.size	.Lgen42, .Lfunc_end3-.Lgen42
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
.Lfunc_end4:
	.size	.Lprint, .Lfunc_end4-.Lprint
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4


	.section	".note.GNU-stack","",@progbits
