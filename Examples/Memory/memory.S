	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movl	$16, %edi
	callq	malloc@PLT
	movq	%rax, %rbx
	movq	$99, (%rax)
	movq	$100, 8(%rax)
	movl	$.L__unnamed_1, %edi
	movl	$99, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	leaq	8(%rsp), %rdx
	movl	$1, %edi
	movq	%rbx, %rsi
	callq	".Lget-index"
	movq	8(%rsp), %rsi
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	%rbx, %rdi
	callq	free@PLT
	addq	$16, %rsp
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.p2align	4, 0x90
	.type	".Lget-index",@function
".Lget-index":
	.cfi_startproc
	movq	(%rsi,%rdi,8), %rax
	movq	%rax, (%rdx)
	retq
.Lfunc_end1:
	.size	".Lget-index", .Lfunc_end1-".Lget-index"
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4

	.section	".note.GNU-stack","",@progbits
