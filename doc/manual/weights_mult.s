mint_weights_mult:
.LFB25:
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
	movq	(%rbx), %rcx
	movslq	%r13d, %rdx
	movslq	%r15d, %r15
	cmpl	%r14d, %r13d
	movq	(%rcx,%rdx,8), %rbx
	movq	(%r12,%r15,8), %rcx
	jge	.L21
	leaq	(%rcx,%rdx,4), %rcx
	leal	-1(%rax), %edx
	movq	8(%rbp), %rdi
	leaq	4(,%rdx,4), %rsi
	.p2align 4,,10
	.p2align 3
.L25:
	testl	%eax, %eax
	jle	.L23
	movss	(%rcx), %xmm1
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L24:
	movss	(%rbx,%rdx), %xmm0
	mulss	(%rdi,%rdx), %xmm0
	addq	$4, %rdx
	cmpq	%rsi, %rdx
	addss	%xmm0, %xmm1
	movss	%xmm1, (%rcx)
	jne	.L24
	addq	%rsi, %rbx
.L23:
	addl	$1, %r13d
	cmpl	%r13d, %r14d
	jle	.L21
	addq	$4, %rcx
	jmp	.L25
.L21:
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
