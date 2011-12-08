	.file	"n_body.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"\nerror: %d\n"
	.text
	.p2align 4,,15
.globl handle_error
	.type	handle_error, @function
handle_error:
.LFB37:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	%edi, %ecx
	movq	stderr(%rip), %rdi
	movl	$.LC0, %edx
	movl	$1, %esi
	xorl	%eax, %eax
	call	__fprintf_chk
	movl	$1, %edi
	call	exit
	.cfi_endproc
.LFE37:
	.size	handle_error, .-handle_error
	.p2align 4,,15
.globl fast_sqrt
	.type	fast_sqrt, @function
fast_sqrt:
.LFB38:
	.cfi_startproc
	movsd	%xmm0, -8(%rsp)
	movq	-8(%rsp), %rax
	movabsq	$2303591209400008704, %rdx
	sarq	%rax
	addq	%rdx, %rax
	movq	%rax, -8(%rsp)
	movsd	-8(%rsp), %xmm0
	ret
	.cfi_endproc
.LFE38:
	.size	fast_sqrt, .-fast_sqrt
	.p2align 4,,15
.globl n_body
	.type	n_body, @function
n_body:
.LFB39:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	event_set(%rip), %edi
	call	PAPI_start
	testl	%eax, %eax
	jne	.L4
	movl	N(%rip), %eax
	testl	%eax, %eax
	jle	.L5
	movsd	.LC2(%rip), %xmm10
	movq	objects(%rip), %r8
	subl	$1, %eax
	movsd	.LC3(%rip), %xmm9
	mulsd	G(%rip), %xmm10
	leaq	(%rax,%rax,4), %rsi
	xorl	%r10d, %r10d
	leaq	80(%r8), %r9
	movq	%r8, %rcx
	movabsq	$2303591209400008704, %rdi
	salq	$4, %rsi
	leaq	(%r9,%rsi), %rsi
	.p2align 4,,10
	.p2align 3
.L6:
	movsd	(%rcx), %xmm8
	movq	%r8, %rax
	movsd	8(%rcx), %xmm7
	movsd	16(%rcx), %xmm6
	movq	%r10, (%rsp)
	movsd	(%rsp), %xmm5
	movapd	%xmm5, %xmm4
	.p2align 4,,10
	.p2align 3
.L7:
	movsd	(%rax), %xmm2
	movsd	8(%rax), %xmm1
	subsd	%xmm8, %xmm2
	movsd	16(%rax), %xmm0
	subsd	%xmm7, %xmm1
	subsd	%xmm6, %xmm0
	movapd	%xmm2, %xmm3
	movapd	%xmm1, %xmm11
	mulsd	%xmm2, %xmm3
	mulsd	%xmm1, %xmm11
	mulsd	%xmm0, %xmm0
	addsd	%xmm11, %xmm3
	addsd	%xmm0, %xmm3
	movq	%xmm3, 8(%rsp)
	movq	8(%rsp), %rdx
	sarq	%rdx
	addq	%rdi, %rdx
	movq	%rdx, (%rsp)
	movq	%rdx, sqrt_val(%rip)
	movsd	(%rsp), %xmm3
	movapd	%xmm3, %xmm0
	mulsd	%xmm3, %xmm0
	mulsd	%xmm3, %xmm0
	movapd	%xmm9, %xmm3
	divsd	%xmm0, %xmm3
	movsd	48(%rax), %xmm0
	addq	$80, %rax
	cmpq	%rsi, %rax
	mulsd	%xmm3, %xmm0
	mulsd	%xmm0, %xmm2
	mulsd	%xmm0, %xmm1
	addsd	%xmm2, %xmm5
	addsd	%xmm1, %xmm4
	jne	.L7
	movsd	24(%rcx), %xmm2
	movapd	%xmm5, %xmm0
	movsd	32(%rcx), %xmm1
	addsd	%xmm2, %xmm8
	movapd	%xmm4, %xmm3
	mulsd	%xmm10, %xmm0
	addsd	%xmm1, %xmm7
	mulsd	%xmm10, %xmm3
	addsd	%xmm4, %xmm1
	addsd	%xmm2, %xmm5
	addsd	%xmm0, %xmm8
	movsd	40(%rcx), %xmm0
	addsd	%xmm3, %xmm7
	movsd	%xmm1, 32(%rcx)
	addsd	%xmm0, %xmm6
	movsd	%xmm5, 24(%rcx)
	addsd	%xmm0, %xmm4
	movsd	%xmm8, 56(%rcx)
	movsd	%xmm7, 64(%rcx)
	addsd	%xmm3, %xmm6
	movsd	%xmm4, 40(%rcx)
	movsd	%xmm6, 72(%rcx)
	addq	$80, %rcx
	cmpq	%rsi, %rcx
	jne	.L6
	jmp	.L12
	.p2align 4,,10
	.p2align 3
.L15:
	addq	$80, %r9
.L12:
	movq	56(%r8), %rax
	cmpq	%rsi, %r9
	movq	%rax, (%r8)
	movq	64(%r8), %rax
	movq	%rax, 8(%r8)
	movq	72(%r8), %rax
	movq	%rax, 16(%r8)
	movq	%r9, %r8
	jne	.L15
.L5:
	movl	event_set(%rip), %edi
	movl	$event_value, %esi
	call	PAPI_stop
	testl	%eax, %eax
	jne	.L16
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L16:
	.cfi_restore_state
	movl	$7, %ecx
.L13:
	movq	stderr(%rip), %rdi
	movl	$.LC0, %edx
	movl	$1, %esi
	xorl	%eax, %eax
	call	__fprintf_chk
	movl	$1, %edi
	call	exit
.L4:
	movl	$6, %ecx
	jmp	.L13
	.cfi_endproc
.LFE39:
	.size	n_body, .-n_body
.globl G
	.data
	.align 16
	.type	G, @object
	.size	G, 8
G:
	.long	2593315660
	.long	1037195420
	.comm	N,4,16
	.comm	sqrt_val,8,16
	.comm	objects,8,16
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC2:
	.long	0
	.long	1071644672
	.align 8
.LC3:
	.long	0
	.long	1072693248
	.ident	"GCC: (Ubuntu/Linaro 4.5.2-8ubuntu4) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
