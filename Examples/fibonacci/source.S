	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	subq	$264, %rsp
	.cfi_def_cfa_offset 272
	leaq	8(%rsp), %rdx
	leaq	12(%rsp), %rcx
	movl	$1, %edi
	movl	$1, %esi
	callq	.Lfib
	movl	12(%rsp), %edi
	movl	8(%rsp), %esi
	leaq	16(%rsp), %rdx
	leaq	20(%rsp), %rcx
	callq	.Lfib
	movl	20(%rsp), %edi
	movl	16(%rsp), %esi
	leaq	24(%rsp), %rdx
	leaq	28(%rsp), %rcx
	callq	.Lfib
	movl	28(%rsp), %edi
	movl	24(%rsp), %esi
	leaq	32(%rsp), %rdx
	leaq	36(%rsp), %rcx
	callq	.Lfib
	movl	36(%rsp), %edi
	movl	32(%rsp), %esi
	leaq	40(%rsp), %rdx
	leaq	44(%rsp), %rcx
	callq	.Lfib
	movl	44(%rsp), %edi
	movl	40(%rsp), %esi
	leaq	48(%rsp), %rdx
	leaq	52(%rsp), %rcx
	callq	.Lfib
	movl	52(%rsp), %edi
	movl	48(%rsp), %esi
	leaq	56(%rsp), %rdx
	leaq	60(%rsp), %rcx
	callq	.Lfib
	movl	60(%rsp), %edi
	movl	56(%rsp), %esi
	leaq	64(%rsp), %rdx
	leaq	68(%rsp), %rcx
	callq	.Lfib
	movl	68(%rsp), %edi
	movl	64(%rsp), %esi
	leaq	72(%rsp), %rdx
	leaq	76(%rsp), %rcx
	callq	.Lfib
	movl	76(%rsp), %edi
	movl	72(%rsp), %esi
	leaq	80(%rsp), %rdx
	leaq	84(%rsp), %rcx
	callq	.Lfib
	movl	84(%rsp), %edi
	movl	80(%rsp), %esi
	leaq	88(%rsp), %rdx
	leaq	92(%rsp), %rcx
	callq	.Lfib
	movl	92(%rsp), %edi
	movl	88(%rsp), %esi
	leaq	96(%rsp), %rdx
	leaq	100(%rsp), %rcx
	callq	.Lfib
	movl	100(%rsp), %edi
	movl	96(%rsp), %esi
	leaq	104(%rsp), %rdx
	leaq	108(%rsp), %rcx
	callq	.Lfib
	movl	108(%rsp), %edi
	movl	104(%rsp), %esi
	leaq	112(%rsp), %rdx
	leaq	116(%rsp), %rcx
	callq	.Lfib
	movl	116(%rsp), %edi
	movl	112(%rsp), %esi
	leaq	120(%rsp), %rdx
	leaq	124(%rsp), %rcx
	callq	.Lfib
	movl	124(%rsp), %edi
	movl	120(%rsp), %esi
	leaq	128(%rsp), %rdx
	leaq	132(%rsp), %rcx
	callq	.Lfib
	movl	132(%rsp), %edi
	movl	128(%rsp), %esi
	leaq	136(%rsp), %rdx
	leaq	140(%rsp), %rcx
	callq	.Lfib
	movl	140(%rsp), %edi
	movl	136(%rsp), %esi
	leaq	144(%rsp), %rdx
	leaq	148(%rsp), %rcx
	callq	.Lfib
	movl	148(%rsp), %edi
	movl	144(%rsp), %esi
	leaq	152(%rsp), %rdx
	leaq	156(%rsp), %rcx
	callq	.Lfib
	movl	156(%rsp), %edi
	movl	152(%rsp), %esi
	leaq	160(%rsp), %rdx
	leaq	164(%rsp), %rcx
	callq	.Lfib
	movl	164(%rsp), %edi
	movl	160(%rsp), %esi
	leaq	168(%rsp), %rdx
	leaq	172(%rsp), %rcx
	callq	.Lfib
	movl	172(%rsp), %edi
	movl	168(%rsp), %esi
	leaq	176(%rsp), %rdx
	leaq	180(%rsp), %rcx
	callq	.Lfib
	movl	180(%rsp), %edi
	movl	176(%rsp), %esi
	leaq	184(%rsp), %rdx
	leaq	188(%rsp), %rcx
	callq	.Lfib
	movl	188(%rsp), %edi
	movl	184(%rsp), %esi
	leaq	192(%rsp), %rdx
	leaq	196(%rsp), %rcx
	callq	.Lfib
	movl	196(%rsp), %edi
	movl	192(%rsp), %esi
	leaq	200(%rsp), %rdx
	leaq	204(%rsp), %rcx
	callq	.Lfib
	movl	204(%rsp), %edi
	movl	200(%rsp), %esi
	leaq	208(%rsp), %rdx
	leaq	212(%rsp), %rcx
	callq	.Lfib
	movl	212(%rsp), %edi
	movl	208(%rsp), %esi
	leaq	216(%rsp), %rdx
	leaq	220(%rsp), %rcx
	callq	.Lfib
	movl	220(%rsp), %edi
	movl	216(%rsp), %esi
	leaq	224(%rsp), %rdx
	leaq	228(%rsp), %rcx
	callq	.Lfib
	movl	228(%rsp), %edi
	movl	224(%rsp), %esi
	leaq	232(%rsp), %rdx
	leaq	236(%rsp), %rcx
	callq	.Lfib
	movl	236(%rsp), %edi
	movl	232(%rsp), %esi
	leaq	240(%rsp), %rdx
	leaq	244(%rsp), %rcx
	callq	.Lfib
	movl	244(%rsp), %edi
	movl	240(%rsp), %esi
	leaq	248(%rsp), %rdx
	leaq	252(%rsp), %rcx
	callq	.Lfib
	movl	252(%rsp), %edi
	movl	248(%rsp), %esi
	leaq	256(%rsp), %rdx
	leaq	260(%rsp), %rcx
	callq	.Lfib
	addq	$264, %rsp
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lfib,@function
.Lfib:
	.cfi_startproc
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	%edi, (%rdx)
	addl	%edi, %esi
	movl	%esi, (%rcx)
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf
	popq	%rax
	retq
.Lfunc_end1:
	.size	.Lfib, .Lfunc_end1-.Lfib
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4


	.section	".note.GNU-stack","",@progbits
