	.text
	.file	"MovForth"
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%r15
	.cfi_def_cfa_offset 24
	pushq	%r14
	.cfi_def_cfa_offset 32
	pushq	%r13
	.cfi_def_cfa_offset 40
	pushq	%r12
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	.cfi_offset %rbp, -16
	movq	%rsp, %rsi
	movl	$20000, %edi
	callq	.Lpopulate
	xorl	%ebp, %ebp
	movq	(%rsp), %r12
	leaq	8(%rsp), %r14
	leaq	16(%rsp), %r15
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_5:
	incq	%rbp
	cmpq	$19999, %rbp
	jge	.LBB0_6
.LBB0_1:
	xorl	%ebx, %ebx
	jmp	.LBB0_2
	.p2align	4, 0x90
.LBB0_4:
	movl	$19999, %eax
	subq	%rbp, %rax
	movq	%r13, %rbx
	cmpq	%rax, %r13
	jge	.LBB0_5
.LBB0_2:
	leaq	1(%rbx), %r13
	movq	%r13, %rdi
	movq	%r12, %rsi
	movq	%r14, %rdx
	callq	".Lget-value"
	movq	%rbx, %rdi
	movq	%r12, %rsi
	movq	%r15, %rdx
	callq	".Lget-value"
	movq	8(%rsp), %rax
	cmpq	16(%rsp), %rax
	jge	.LBB0_4
	leaq	1(%rbx), %rdi
	movq	%rbx, %rsi
	movq	%r12, %rdx
	callq	.Lswap
	jmp	.LBB0_4
.LBB0_6:
	movl	$20000, %edi
	movq	%r12, %rsi
	callq	".Lprint-array"
	movq	%r12, %rdi
	callq	free@PLT
	addq	$24, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%r12
	.cfi_def_cfa_offset 40
	popq	%r13
	.cfi_def_cfa_offset 32
	popq	%r14
	.cfi_def_cfa_offset 24
	popq	%r15
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lpopulate,@function
.Lpopulate:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	pushq	%rax
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -24
	.cfi_offset %r14, -16
	movq	%rsi, %r14
	movq	%rdi, %rbx
	leaq	(,%rdi,8), %rdi
	callq	malloc@PLT
	movl	$42, %esi
	xorl	%ecx, %ecx
	movq	%rax, %rdx
	.p2align	4, 0x90
.LBB1_1:
	imull	$1309, %esi, %esi
	addl	$13849, %esi
	movzwl	%si, %esi
	movq	%rsi, (%rdx)
	incq	%rcx
	addq	$8, %rdx
	cmpq	%rbx, %rcx
	jl	.LBB1_1
	movq	%rax, (%r14)
	addq	$8, %rsp
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	.Lpopulate, .Lfunc_end1-.Lpopulate
	.cfi_endproc

	.p2align	4, 0x90
	.type	".Lget-value",@function
".Lget-value":
	.cfi_startproc
	movq	(%rsi,%rdi,8), %rax
	movq	%rax, (%rdx)
	retq
.Lfunc_end2:
	.size	".Lget-value", .Lfunc_end2-".Lget-value"
	.cfi_endproc

	.p2align	4, 0x90
	.type	.Lswap,@function
.Lswap:
	.cfi_startproc
	movq	(%rdx,%rsi,8), %rax
	movq	(%rdx,%rdi,8), %rcx
	movq	%rcx, (%rdx,%rsi,8)
	movq	%rax, (%rdx,%rdi,8)
	retq
.Lfunc_end3:
	.size	.Lswap, .Lfunc_end3-.Lswap
	.cfi_endproc

	.p2align	4, 0x90
	.type	".Lprint-array",@function
".Lprint-array":
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
	movq	%rsi, %r15
	movq	%rdi, %r14
	xorl	%ebx, %ebx
	.p2align	4, 0x90
.LBB4_1:
	movq	(%r15), %rsi
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	incq	%rbx
	addq	$8, %r15
	cmpq	%r14, %rbx
	jl	.LBB4_1
	popq	%rbx
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end4:
	.size	".Lprint-array", .Lfunc_end4-".Lprint-array"
	.cfi_endproc

	.type	.L__unnamed_1,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4

	.section	".note.GNU-stack","",@progbits
