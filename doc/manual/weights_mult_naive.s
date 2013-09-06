mint_weights_mult_naive:
.LFB26:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	movl	%r8d, %r14d
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	pushq	%r13
	.cfi_def_cfa_offset 32
	movl	%ecx, %r13d
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	movq	%rdx, %r12
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	movq	%rsi, %rbp
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movq	%rdi, %rbx
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	mint_weights_get_target
	movq	%rbx, %rdi
	movl	%eax, %r15d
	call	mint_weights_cols
	cmpl	%r14d, %r13d
	jae	.L27
	leal	-1(%rax), %edx
	mov	%r15d, %r15d
	leaq	(%r12,%r15,8), %r9
	leaq	4(,%rdx,4), %r8
	.p2align 4,,10
	.p2align 3
.L29:
	testl	%eax, %eax
	je	.L31
	mov	%r13d, %edx
	movq	(%rbx), %rsi
	leaq	0(,%rdx,4), %rcx
	addq	(%r9), %rcx
	movq	(%rsi,%rdx,8), %rdi
	movq	8(%rbp), %rsi
	xorl	%edx, %edx
	movss	(%rcx), %xmm1
	.p2align 4,,10
	.p2align 3
.L30:
	movss	(%rdi,%rdx), %xmm0
	mulss	(%rsi,%rdx), %xmm0
	addq	$4, %rdx
	cmpq	%r8, %rdx
	addss	%xmm0, %xmm1
	movss	%xmm1, (%rcx)
	jne	.L30
.L31:
	addl	$1, %r13d
	cmpl	%r14d, %r13d
	jb	.L29
.L27:
	addq	$8, %rsp
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
