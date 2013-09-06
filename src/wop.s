	.file	"wop.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"wop.c"
.LC2:
	.string	"%s (%s:%d): "
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC3:
	.string	"rows argument missing (parameter 0)"
	.section	.rodata.str1.1
.LC5:
	.string	"rows argument negative"
	.text
	.p2align 4,,15
.globl mint_weights_init_rows
	.type	mint_weights_init_rows, @function
mint_weights_init_rows:
.LFB21:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$1, %eax
	movss	(%rcx), %xmm0
	movss	.LC0(%rip), %xmm1
	ucomiss	%xmm1, %xmm0
	setp	%dl
	cmove	%edx, %eax
	testb	%al, %al
	je	.L6
	ucomiss	.LC4(%rip), %xmm0
	jbe	.L7
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4179, %edx
	movl	$.LC2, %esi
	movl	$11, %r8d
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$35, %edx
	movl	$1, %esi
	movl	$.LC3, %edi
.L4:
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
.L7:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4179, %edx
	movl	$.LC2, %esi
	movl	$12, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$22, %edx
	movl	$1, %esi
	movl	$.LC5, %edi
	jmp	.L4
	.cfi_endproc
.LFE21:
	.size	mint_weights_init_rows, .-mint_weights_init_rows
	.section	.rodata.str1.8
	.align 8
.LC6:
	.string	"cols argument missing (parameter 0)"
	.section	.rodata.str1.1
.LC7:
	.string	"cols argument negative"
	.text
	.p2align 4,,15
.globl mint_weights_init_cols
	.type	mint_weights_init_cols, @function
mint_weights_init_cols:
.LFB22:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$1, %eax
	movss	(%rcx), %xmm0
	movss	.LC0(%rip), %xmm1
	ucomiss	%xmm1, %xmm0
	setp	%dl
	cmove	%edx, %eax
	testb	%al, %al
	je	.L12
	ucomiss	.LC4(%rip), %xmm0
	jbe	.L13
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L12:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4186, %edx
	movl	$.LC2, %esi
	movl	$17, %r8d
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$35, %edx
	movl	$1, %esi
	movl	$.LC6, %edi
.L11:
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
.L13:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4186, %edx
	movl	$.LC2, %esi
	movl	$18, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$22, %edx
	movl	$1, %esi
	movl	$.LC7, %edi
	jmp	.L11
	.cfi_endproc
.LFE22:
	.size	mint_weights_init_cols, .-mint_weights_init_cols
	.section	.rodata.str1.8
	.align 8
.LC8:
	.string	"states argument missing (parameter 0)"
	.section	.rodata.str1.1
.LC9:
	.string	"states argument negative"
	.text
	.p2align 4,,15
.globl mint_weights_init_states
	.type	mint_weights_init_states, @function
mint_weights_init_states:
.LFB23:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$1, %eax
	movss	(%rcx), %xmm0
	movss	.LC0(%rip), %xmm1
	ucomiss	%xmm1, %xmm0
	setp	%dl
	cmove	%edx, %eax
	testb	%al, %al
	je	.L18
	ucomiss	.LC4(%rip), %xmm0
	jb	.L19
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L18:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4193, %edx
	movl	$.LC2, %esi
	movl	$23, %r8d
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$37, %edx
	movl	$1, %esi
	movl	$.LC8, %edi
.L17:
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
.L19:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4193, %edx
	movl	$.LC2, %esi
	movl	$24, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$24, %edx
	movl	$1, %esi
	movl	$.LC9, %edi
	jmp	.L17
	.cfi_endproc
.LFE23:
	.size	mint_weights_init_states, .-mint_weights_init_states
	.p2align 4,,15
.globl mint_weights_init_sparse
	.type	mint_weights_init_sparse, @function
mint_weights_init_sparse:
.LFB24:
	.cfi_startproc
	rep
	ret
	.cfi_endproc
.LFE24:
	.size	mint_weights_init_sparse, .-mint_weights_init_sparse
	.p2align 4,,15
.globl mint_weights_mult
	.type	mint_weights_mult, @function
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
.LFE25:
	.size	mint_weights_mult, .-mint_weights_mult
	.p2align 4,,15
.globl mint_weights_mult_naive
	.type	mint_weights_mult_naive, @function
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
.LFE26:
	.size	mint_weights_mult_naive, .-mint_weights_mult_naive
	.p2align 4,,15
.globl mint_weights_mult_sparse
	.type	mint_weights_mult_sparse, @function
mint_weights_mult_sparse:
.LFB27:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	pushq	%r14
	.cfi_def_cfa_offset 24
	movl	%ecx, %r14d
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	movq	%rdx, %r12
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	pushq	%rbp
	.cfi_def_cfa_offset 48
	movq	%rsi, %rbp
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movq	%rdi, %rbx
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movl	%r8d, 20(%rsp)
	call	mint_weights_get_target
	cmpl	20(%rsp), %r14d
	movq	8(%rbp), %r13
	jae	.L33
	mov	%eax, %eax
	leaq	(%r12,%rax,8), %r12
	movq	%r12, 24(%rsp)
	.p2align 4,,10
	.p2align 3
.L37:
	movq	24(%rsp), %rax
	movl	%r14d, %esi
	movq	%rbx, %rdi
	mov	%r14d, %r15d
	movq	(%rax), %rax
	movq	%rax, 8(%rsp)
	call	mint_weights_colind
	movq	%rax, %rbp
	movq	(%rbx), %rax
	movl	%r14d, %esi
	movq	%rbx, %rdi
	movq	(%rax,%r15,8), %r12
	call	mint_weights_rowlen
	testl	%eax, %eax
	je	.L35
	movq	8(%rsp), %rdx
	subl	$1, %eax
	leaq	4(,%rax,4), %rsi
	xorl	%eax, %eax
	leaq	(%rdx,%r15,4), %rcx
	movss	(%rcx), %xmm1
	.p2align 4,,10
	.p2align 3
.L36:
	mov	0(%rbp,%rax), %edx
	movss	(%r12,%rax), %xmm0
	addq	$4, %rax
	cmpq	%rsi, %rax
	mulss	0(%r13,%rdx,4), %xmm0
	addss	%xmm0, %xmm1
	movss	%xmm1, (%rcx)
	jne	.L36
.L35:
	addl	$1, %r14d
	cmpl	20(%rsp), %r14d
	jb	.L37
.L33:
	addq	$40, %rsp
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
.LFE27:
	.size	mint_weights_mult_sparse, .-mint_weights_mult_sparse
	.section	.rodata.str1.1
.LC10:
	.string	"rmin, rmax out fo range"
	.text
	.p2align 4,,15
.globl mint_weights_hebbian
	.type	mint_weights_hebbian, @function
mint_weights_hebbian:
.LFB28:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	movq	%r9, %r15
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	movl	%r8d, %r14d
	.cfi_offset 14, -24
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
	subq	$24, %rsp
	.cfi_def_cfa_offset 80
	call	mint_weights_rows
	movq	%rbx, %rdi
	movl	%eax, 12(%rsp)
	call	mint_weights_cols
	cmpl	12(%rsp), %r14d
	jg	.L41
	movl	%r13d, %edx
	shrl	$31, %edx
	testb	%dl, %dl
	jne	.L41
	cmpl	%r14d, %r13d
	jge	.L40
	movslq	%r13d, %r8
	leaq	0(,%r8,4), %r9
	salq	$3, %r8
	.p2align 4,,10
	.p2align 3
.L43:
	testl	%eax, %eax
	jle	.L46
	cmpq	%r12, %rbp
	setne	%r10b
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L45:
	cmpl	%edx, %r13d
	je	.L44
	testb	%r10b, %r10b
	je	.L44
	movq	8(%rbp), %rdi
	movq	(%rbx), %rsi
	movss	(%rdi,%rcx), %xmm0
	movq	8(%r12), %rdi
	subss	4(%r15), %xmm0
	movq	(%rsi,%r8), %rsi
	movss	(%rdi,%r9), %xmm1
	subss	8(%r15), %xmm1
	leaq	(%rcx,%rsi), %rsi
	movss	(%rsi), %xmm2
	mulss	%xmm1, %xmm0
	movss	12(%r15), %xmm1
	mulss	%xmm2, %xmm1
	mulss	(%r15), %xmm0
	subss	%xmm1, %xmm0
	addss	%xmm2, %xmm0
	movss	%xmm0, (%rsi)
.L44:
	addl	$1, %edx
	addq	$4, %rcx
	cmpl	%edx, %eax
	jne	.L45
.L46:
	addl	$1, %r13d
	addq	$4, %r9
	addq	$8, %r8
	cmpl	%r13d, %r14d
	jg	.L43
.L40:
	addq	$24, %rsp
	.cfi_remember_state
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
.L41:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$88, %r8d
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4273, %edx
	movl	$.LC2, %esi
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$23, %edx
	movl	$1, %esi
	movl	$.LC10, %edi
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
	.cfi_endproc
.LFE28:
	.size	mint_weights_hebbian, .-mint_weights_hebbian
	.section	.rodata.str1.1
.LC11:
	.string	"rmin, rmax out of range"
	.text
	.p2align 4,,15
.globl mint_weights_delta
	.type	mint_weights_delta, @function
mint_weights_delta:
.LFB29:
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
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movss	(%r9), %xmm1
	movss	4(%r9), %xmm0
	cvttss2si	%xmm0, %r15d
	movss	%xmm1, (%rsp)
	call	mint_weights_rows
	movq	%rbx, %rdi
	movl	%eax, 28(%rsp)
	call	mint_weights_cols
	cmpl	28(%rsp), %r14d
	movss	(%rsp), %xmm1
	jg	.L49
	movl	%r13d, %edx
	shrl	$31, %edx
	testb	%dl, %dl
	jne	.L49
	cmpl	%r14d, %r13d
	jge	.L48
	subl	%r13d, %r14d
	movslq	%r13d, %rdx
	movslq	%r15d, %r15
	leal	-1(%r14), %ecx
	leaq	0(,%rdx,8), %r10
	leaq	0(,%rdx,4), %r11
	leaq	(%r12,%r15,8), %r15
	leaq	1(%rdx,%rcx), %r13
	leal	-1(%rax), %edx
	leaq	4(,%rdx,4), %r9
	salq	$3, %r13
	.p2align 4,,10
	.p2align 3
.L51:
	testl	%eax, %eax
	jle	.L53
	movq	(%rbx), %rdx
	movq	%r11, %r8
	movq	8(%r12), %rdi
	addq	(%r15), %r8
	movq	8(%rbp), %rsi
	movq	(%rdx,%r10), %rcx
	addq	%r11, %rdi
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L52:
	movss	(%r8), %xmm0
	subss	(%rdi), %xmm0
	mulss	%xmm1, %xmm0
	mulss	(%rsi,%rdx), %xmm0
	addss	(%rcx,%rdx), %xmm0
	movss	%xmm0, (%rcx,%rdx)
	addq	$4, %rdx
	cmpq	%r9, %rdx
	jne	.L52
.L53:
	addq	$8, %r10
	addq	$4, %r11
	cmpq	%r13, %r10
	jne	.L51
.L48:
	addq	$40, %rsp
	.cfi_remember_state
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
.L49:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$106, %r8d
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4294, %edx
	movl	$.LC2, %esi
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$23, %edx
	movl	$1, %esi
	movl	$.LC11, %edi
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
	.cfi_endproc
.LFE29:
	.size	mint_weights_delta, .-mint_weights_delta
	.section	.rodata.str1.1
.LC12:
	.string	"counter"
	.section	.rodata.str1.8
	.align 8
.LC13:
	.string	"no counter defined for pre-synaptic nodes"
	.align 8
.LC14:
	.string	"no counter defined for post-synaptic nodes"
	.text
	.p2align 4,,15
.globl mint_weights_stdp
	.type	mint_weights_stdp, @function
mint_weights_stdp:
.LFB30:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%r13
	.cfi_def_cfa_offset 24
	movl	%r8d, %r13d
	.cfi_offset 13, -24
	.cfi_offset 14, -16
	pushq	%r12
	.cfi_def_cfa_offset 32
	movl	%ecx, %r12d
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	movq	%rdx, %rbp
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	movq	%rdi, %rbx
	.cfi_offset 3, -48
	movq	%rsi, %rdi
	call	mint_nodes_get_ops
	movl	$1, %edx
	movl	$.LC12, %esi
	movq	%rax, %rdi
	movq	%rax, %r14
	call	mint_ops_find
	testl	%eax, %eax
	js	.L65
	movl	%eax, %esi
	movq	%r14, %rdi
	call	mint_ops_get
	movl	$1, %esi
	movq	%rax, %rdi
	call	mint_op_get_param
	movq	%rbp, %rdi
	call	mint_nodes_get_ops
	movl	$1, %edx
	movl	$.LC12, %esi
	movq	%rax, %rdi
	movq	%rax, %rbp
	call	mint_ops_find
	testl	%eax, %eax
	js	.L66
	movl	%eax, %esi
	movq	%rbp, %rdi
	call	mint_ops_get
	movl	$1, %esi
	movq	%rax, %rdi
	call	mint_op_get_param
	movq	%rbx, %rdi
	call	mint_weights_rows
	cmpl	%eax, %r13d
	jg	.L62
	movl	%r12d, %eax
	shrl	$31, %eax
	testb	%al, %al
	jne	.L62
	movq	%rbx, %rdi
	call	mint_weights_is_sparse
	cmpl	%r13d, %r12d
	jge	.L55
	.p2align 4,,10
	.p2align 3
.L61:
	movl	%r12d, %esi
	movq	%rbx, %rdi
	call	mint_weights_rowlen
	movl	%r12d, %esi
	movq	%rbx, %rdi
	addl	$1, %r12d
	call	mint_weights_colind
	cmpl	%r12d, %r13d
	jg	.L61
.L55:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L65:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4328, %edx
	movl	$.LC2, %esi
	movl	$130, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$41, %edx
	movl	$1, %esi
	movl	$.LC13, %edi
.L64:
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
.L62:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4328, %edx
	movl	$.LC2, %esi
	movl	$139, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$23, %edx
	movl	$1, %esi
	movl	$.LC11, %edi
	jmp	.L64
.L66:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4328, %edx
	movl	$.LC2, %esi
	movl	$135, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$42, %edx
	movl	$1, %esi
	movl	$.LC14, %edi
	jmp	.L64
	.cfi_endproc
.LFE30:
	.size	mint_weights_stdp, .-mint_weights_stdp
	.p2align 4,,15
.globl mint_weights_init_random_sparse
	.type	mint_weights_init_random_sparse, @function
mint_weights_init_random_sparse:
.LFB31:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	movl	%esi, %r15d
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	movq	%r8, %r12
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	pushq	%rbp
	.cfi_def_cfa_offset 48
	movq	%rcx, %rbp
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	subq	$72, %rsp
	.cfi_def_cfa_offset 128
	movq	%rdi, 40(%rsp)
	movl	%edx, 52(%rsp)
	.cfi_offset 3, -56
	call	mint_weights_cols
	cmpl	52(%rsp), %r15d
	movl	%eax, 36(%rsp)
	jge	.L67
	leaq	8(%rbp), %rax
	movq	%rax, 56(%rsp)
	.p2align 4,,10
	.p2align 3
.L71:
	movq	56(%rsp), %rcx
	movl	36(%rsp), %edi
	movss	(%rcx), %xmm0
	call	mint_random_binomial
	movslq	%eax, %r13
	movl	%eax, %ebx
	leaq	0(,%r13,4), %rdi
	call	malloc
	testl	%ebx, %ebx
	movq	%rax, %r14
	jle	.L69
	leaq	4(%rbp), %rax
	movq	%r14, %rdx
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L70:
	movq	24(%rsp), %rcx
	movss	0(%rbp), %xmm0
	movss	(%rcx), %xmm1
	movl	%eax, 8(%rsp)
	movq	%rdx, 16(%rsp)
	call	*%r12
	movl	8(%rsp), %eax
	movq	16(%rsp), %rdx
	addl	$1, %eax
	movss	%xmm0, (%rdx)
	addq	$4, %rdx
	cmpl	%eax, %ebx
	jg	.L70
.L69:
	leaq	0(,%r13,8), %rdi
	call	malloc
	movl	36(%rsp), %ecx
	movq	%rax, %rdi
	xorl	%edx, %edx
	movl	$1, %r8d
	movl	%ebx, %esi
	movq	%rax, %r13
	call	mint_random_subset
	movq	40(%rsp), %rdi
	xorl	%r9d, %r9d
	movl	%r15d, %esi
	movq	%r13, %r8
	movq	%r14, %rcx
	movl	%ebx, %edx
	addl	$1, %r15d
	call	mint_weights_set_row
	movq	%r14, %rdi
	call	free
	movq	%r13, %rdi
	call	free
	cmpl	%r15d, 52(%rsp)
	jg	.L71
.L67:
	addq	$72, %rsp
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
.LFE31:
	.size	mint_weights_init_random_sparse, .-mint_weights_init_random_sparse
	.p2align 4,,15
.globl mint_weights_init_random_dense
	.type	mint_weights_init_random_dense, @function
mint_weights_init_random_dense:
.LFB32:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	movq	%r8, %r15
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	movq	%rcx, %r13
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	pushq	%r12
	.cfi_def_cfa_offset 40
	movl	%edx, %r12d
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	movl	%esi, %ebp
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movq	%rdi, %rbx
	.cfi_offset 3, -56
	subq	$56, %rsp
	.cfi_def_cfa_offset 112
	call	mint_weights_cols
	movq	%rbx, %rdi
	movl	%eax, %r14d
	call	mint_weights_rows
	cmpl	%eax, %r12d
	ja	.L84
	movl	%ebp, %eax
	shrl	$31, %eax
	testb	%al, %al
	jne	.L84
	cmpl	%ebp, %r12d
	movl	%ebp, 36(%rsp)
	jbe	.L74
	.p2align 4,,10
	.p2align 3
.L86:
	testl	%r14d, %r14d
	je	.L83
	leaq	8(%r13), %rax
	movq	$0, 16(%rsp)
	xorl	%ebp, %ebp
	movq	%rax, 24(%rsp)
	mov	36(%rsp), %eax
	salq	$3, %rax
	movq	%rax, 40(%rsp)
	jmp	.L81
	.p2align 4,,10
	.p2align 3
.L79:
	addl	$1, %ebp
	addq	$4, 16(%rsp)
	cmpl	%ebp, %r14d
	jbe	.L83
.L81:
	call	mint_random
	movq	24(%rsp), %rax
	movss	(%rax), %xmm1
	ucomiss	%xmm0, %xmm1
	jbe	.L79
	movq	(%rbx), %rax
	movq	40(%rsp), %rdx
	addl	$1, %ebp
	movss	4(%r13), %xmm1
	movss	0(%r13), %xmm0
	movq	(%rax,%rdx), %rax
	addq	16(%rsp), %rax
	movq	%rax, 8(%rsp)
	call	*%r15
	movq	8(%rsp), %rax
	movss	%xmm0, (%rax)
	addq	$4, 16(%rsp)
	cmpl	%ebp, %r14d
	ja	.L81
.L83:
	addl	$1, 36(%rsp)
	cmpl	36(%rsp), %r12d
	ja	.L86
.L74:
	addq	$56, %rsp
	.cfi_remember_state
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
.L84:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$191, %r8d
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4369, %edx
	movl	$.LC2, %esi
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$23, %edx
	movl	$1, %esi
	movl	$.LC11, %edi
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
	.cfi_endproc
.LFE32:
	.size	mint_weights_init_random_dense, .-mint_weights_init_random_dense
	.p2align 4,,15
.globl mint_weights_init_random
	.type	mint_weights_init_random, @function
mint_weights_init_random:
.LFB33:
	.cfi_startproc
	movq	%rbx, -32(%rsp)
	movq	%rbp, -24(%rsp)
	movq	%rdi, %rbx
	.cfi_offset 6, -32
	.cfi_offset 3, -40
	movq	%r12, -16(%rsp)
	movq	%r13, -8(%rsp)
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	%esi, %ebp
	movl	%edx, %r12d
	.cfi_offset 13, -16
	.cfi_offset 12, -24
	movq	%rcx, %r13
	call	mint_weights_is_sparse
	testl	%eax, %eax
	jne	.L91
	movq	%r13, %rcx
	movl	%r12d, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	movq	16(%rsp), %rbp
	movq	8(%rsp), %rbx
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	movl	$mint_random_uniform, %r8d
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	jmp	mint_weights_init_random_dense
	.p2align 4,,10
	.p2align 3
.L91:
	.cfi_restore_state
	movq	%r13, %rcx
	movl	%r12d, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	movq	16(%rsp), %rbp
	movq	8(%rsp), %rbx
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	movl	$mint_random_uniform, %r8d
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	jmp	mint_weights_init_random_sparse
	.cfi_endproc
.LFE33:
	.size	mint_weights_init_random, .-mint_weights_init_random
	.p2align 4,,15
.globl mint_weights_init_normal
	.type	mint_weights_init_normal, @function
mint_weights_init_normal:
.LFB34:
	.cfi_startproc
	movq	%rbx, -32(%rsp)
	movq	%rbp, -24(%rsp)
	movq	%rdi, %rbx
	.cfi_offset 6, -32
	.cfi_offset 3, -40
	movq	%r12, -16(%rsp)
	movq	%r13, -8(%rsp)
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	%esi, %ebp
	movl	%edx, %r12d
	.cfi_offset 13, -16
	.cfi_offset 12, -24
	movq	%rcx, %r13
	call	mint_weights_is_sparse
	testl	%eax, %eax
	jne	.L94
	movq	%r13, %rcx
	movl	%r12d, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	movq	16(%rsp), %rbp
	movq	8(%rsp), %rbx
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	movl	$mint_random_normal, %r8d
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	jmp	mint_weights_init_random_dense
	.p2align 4,,10
	.p2align 3
.L94:
	.cfi_restore_state
	movq	%r13, %rcx
	movl	%r12d, %edx
	movl	%ebp, %esi
	movq	%rbx, %rdi
	movq	16(%rsp), %rbp
	movq	8(%rsp), %rbx
	movq	24(%rsp), %r12
	movq	32(%rsp), %r13
	movl	$mint_random_normal, %r8d
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	jmp	mint_weights_init_random_sparse
	.cfi_endproc
.LFE34:
	.size	mint_weights_init_normal, .-mint_weights_init_normal
	.section	.rodata.str1.1
.LC15:
	.string	"from size != to size"
	.text
	.p2align 4,,15
.globl mint_weights_init_diagonal
	.type	mint_weights_init_diagonal, @function
mint_weights_init_diagonal:
.LFB35:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	pushq	%r13
	.cfi_def_cfa_offset 24
	movl	%esi, %r13d
	.cfi_offset 13, -24
	.cfi_offset 14, -16
	pushq	%r12
	.cfi_def_cfa_offset 32
	movq	%rcx, %r12
	.cfi_offset 12, -32
	pushq	%rbp
	.cfi_def_cfa_offset 40
	movl	%edx, %ebp
	.cfi_offset 6, -40
	pushq	%rbx
	.cfi_def_cfa_offset 48
	movq	%rdi, %rbx
	.cfi_offset 3, -48
	call	mint_weights_rows
	movq	%rbx, %rdi
	movl	%eax, %r14d
	call	mint_weights_cols
	cmpl	%eax, %r14d
	je	.L101
	jmp	.L103
	.p2align 4,,10
	.p2align 3
.L99:
	movl	%r13d, %ecx
	movl	%r13d, %edx
	xorl	%esi, %esi
	movss	(%r12), %xmm0
	movq	%rbx, %rdi
	addl	$1, %r13d
	call	mint_weights_set
.L101:
	cmpl	%ebp, %r13d
	jb	.L99
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
.L103:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	$225, %r8d
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4397, %edx
	movl	$.LC2, %esi
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$20, %edx
	movl	$1, %esi
	movl	$.LC15, %edi
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
	.cfi_endproc
.LFE35:
	.size	mint_weights_init_diagonal, .-mint_weights_init_diagonal
	.p2align 4,,15
.globl mint_weights_init_target
	.type	mint_weights_init_target, @function
mint_weights_init_target:
.LFB36:
	.cfi_startproc
	movss	(%rcx), %xmm1
	cvttss2siq	%xmm1, %rsi
	jmp	mint_weights_set_target
	.cfi_endproc
.LFE36:
	.size	mint_weights_init_target, .-mint_weights_init_target
	.section	.rodata.str1.8
	.align 8
.LC16:
	.string	"from and to nodes must be the same!"
	.section	.rodata.str1.1
.LC17:
	.string	"rows"
	.section	.rodata.str1.8
	.align 8
.LC19:
	.string	"distance must be >0 (parameter 2)"
	.text
	.p2align 4,,15
.globl mint_weights_lateral
	.type	mint_weights_lateral, @function
mint_weights_lateral:
.LFB37:
	.cfi_startproc
	pushq	%r15
	.cfi_def_cfa_offset 16
	xorps	%xmm0, %xmm0
	pushq	%r14
	.cfi_def_cfa_offset 24
	pushq	%r13
	.cfi_def_cfa_offset 32
	pushq	%r12
	.cfi_def_cfa_offset 40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	movq	%rsi, %rbx
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	subq	$88, %rsp
	.cfi_def_cfa_offset 144
	ucomiss	12(%r9), %xmm0
	movq	%rdi, 48(%rsp)
	movq	%r9, 72(%rsp)
	jp	.L105
	jne	.L105
	cmpq	%rdx, %rsi
	jne	.L132
	movq	%rsi, %rdi
	.p2align 4,,5
	call	mint_nodes_get_ops
	movl	$8, %edx
	movl	$.LC17, %esi
	movq	%rax, %rdi
	movq	%rax, %rbp
	call	mint_ops_find
	testl	%eax, %eax
	movl	$1, 60(%rsp)
	js	.L108
	movl	%eax, %esi
	movq	%rbp, %rdi
	call	mint_ops_get
	xorl	%esi, %esi
	movq	%rax, %rdi
	call	mint_op_get_param
	cvttss2si	%xmm0, %eax
	movl	%eax, 60(%rsp)
.L108:
	movq	%rbx, %rdi
	call	mint_nodes_size
	movl	%eax, %edx
	movss	.LC18(%rip), %xmm1
	sarl	$31, %edx
	xorps	%xmm4, %xmm4
	idivl	60(%rsp)
	movl	%eax, %r12d
	movq	72(%rsp), %rax
	movss	4(%rax), %xmm2
	addss	%xmm1, %xmm2
	movss	(%rax), %xmm0
	movss	8(%rax), %xmm3
	ucomiss	%xmm4, %xmm2
	jbe	.L133
	movaps	%xmm0, %xmm4
	cmpl	$0, 60(%rsp)
	subss	%xmm3, %xmm4
	movaps	%xmm1, %xmm3
	subss	%xmm2, %xmm3
	divss	%xmm3, %xmm4
	subss	%xmm4, %xmm0
	movss	%xmm4, 40(%rsp)
	movss	%xmm0, 44(%rsp)
	jle	.L110
	movl	$0, 68(%rsp)
	movl	$0, 64(%rsp)
.L111:
	xorl	%r13d, %r13d
	testl	%r12d, %r12d
	jle	.L120
.L126:
	movl	68(%rsp), %eax
	xorl	%r15d, %r15d
	xorl	%r14d, %r14d
	addl	%r13d, %eax
	movl	%eax, 36(%rsp)
	.p2align 4,,10
	.p2align 3
.L121:
	movl	64(%rsp), %ebp
	xorl	%ebx, %ebx
	subl	%r14d, %ebp
	imull	%ebp, %ebp
	jmp	.L117
	.p2align 4,,10
	.p2align 3
.L114:
	addl	$1, %ebx
	cmpl	%ebx, %r12d
	jle	.L134
.L117:
	movl	%r13d, %eax
	subl	%ebx, %eax
	imull	%eax, %eax
	addl	%ebp, %eax
	cvtsi2sd	%eax, %xmm3
	sqrtsd	%xmm3, %xmm0
	ucomisd	%xmm0, %xmm0
	jp	.L135
.L112:
	unpcklpd	%xmm0, %xmm0
	cvtpd2ps	%xmm0, %xmm0
	ucomiss	%xmm1, %xmm0
	jb	.L114
	ucomiss	%xmm0, %xmm2
	jb	.L114
	mulss	40(%rsp), %xmm0
	movl	36(%rsp), %ecx
	leal	(%rbx,%r15), %edx
	movq	48(%rsp), %rdi
	xorl	%esi, %esi
	addl	$1, %ebx
	movss	%xmm1, (%rsp)
	movss	%xmm2, 16(%rsp)
	addss	44(%rsp), %xmm0
	call	mint_weights_set
	cmpl	%ebx, %r12d
	movss	16(%rsp), %xmm2
	movss	(%rsp), %xmm1
	jg	.L117
	.p2align 4,,10
	.p2align 3
.L134:
	addl	$1, %r14d
	addl	%r12d, %r15d
	cmpl	%r14d, 60(%rsp)
	jg	.L121
	addl	$1, %r13d
	cmpl	%r13d, %r12d
	jg	.L126
.L120:
	addl	$1, 64(%rsp)
	addl	%r12d, 68(%rsp)
	movl	64(%rsp), %eax
	cmpl	%eax, 60(%rsp)
	jg	.L111
.L110:
	movq	72(%rsp), %rax
	movl	$0x3f800000, 12(%rax)
.L105:
	addq	$88, %rsp
	.cfi_remember_state
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
.L135:
	.cfi_restore_state
	movapd	%xmm3, %xmm0
	movss	%xmm1, (%rsp)
	movss	%xmm2, 16(%rsp)
	call	sqrt
	movss	16(%rsp), %xmm2
	movss	(%rsp), %xmm1
	jmp	.L112
.L132:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4431, %edx
	movl	$.LC2, %esi
	movl	$244, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$35, %edx
	movl	$1, %esi
	movl	$.LC16, %edi
.L131:
	call	fwrite
	movq	stderr(%rip), %rsi
	movl	$10, %edi
	call	fputc
	call	abort
.L133:
	movq	stderr(%rip), %rdi
	movl	$.LC1, %ecx
	movl	$__FUNCTION__.4431, %edx
	movl	$.LC2, %esi
	movl	$260, %r8d
	xorl	%eax, %eax
	call	fprintf
	movq	stderr(%rip), %rcx
	movl	$33, %edx
	movl	$1, %esi
	movl	$.LC19, %edi
	jmp	.L131
	.cfi_endproc
.LFE37:
	.size	mint_weights_lateral, .-mint_weights_lateral
	.section	.rodata
	.align 16
	.type	__FUNCTION__.4431, @object
	.size	__FUNCTION__.4431, 21
__FUNCTION__.4431:
	.string	"mint_weights_lateral"
	.align 16
	.type	__FUNCTION__.4397, @object
	.size	__FUNCTION__.4397, 27
__FUNCTION__.4397:
	.string	"mint_weights_init_diagonal"
	.align 16
	.type	__FUNCTION__.4369, @object
	.size	__FUNCTION__.4369, 31
__FUNCTION__.4369:
	.string	"mint_weights_init_random_dense"
	.align 16
	.type	__FUNCTION__.4328, @object
	.size	__FUNCTION__.4328, 18
__FUNCTION__.4328:
	.string	"mint_weights_stdp"
	.align 16
	.type	__FUNCTION__.4294, @object
	.size	__FUNCTION__.4294, 19
__FUNCTION__.4294:
	.string	"mint_weights_delta"
	.align 16
	.type	__FUNCTION__.4273, @object
	.size	__FUNCTION__.4273, 21
__FUNCTION__.4273:
	.string	"mint_weights_hebbian"
	.align 16
	.type	__FUNCTION__.4193, @object
	.size	__FUNCTION__.4193, 25
__FUNCTION__.4193:
	.string	"mint_weights_init_states"
	.align 16
	.type	__FUNCTION__.4186, @object
	.size	__FUNCTION__.4186, 23
__FUNCTION__.4186:
	.string	"mint_weights_init_cols"
	.align 16
	.type	__FUNCTION__.4179, @object
	.size	__FUNCTION__.4179, 23
__FUNCTION__.4179:
	.string	"mint_weights_init_rows"
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC0:
	.long	3212836864
	.align 4
.LC4:
	.long	0
	.align 4
.LC18:
	.long	1065353216
	.ident	"GCC: (GNU) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
