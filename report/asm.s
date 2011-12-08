; Start of outer cycle: executed N times
;	4 stores, 1 load, 7 total
	movsd	(%rcx), %xmm8
	movq	%r8, %rax
	movsd	8(%rcx), %xmm7
	movsd	16(%rcx), %xmm6
	movq	%r10, (%rsp)
	movsd	(%rsp), %xmm5
	movapd	%xmm5, %xmm4

; Start of inner cycle: N*N
;	6 stores, 3 loads, 34 total
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

; end of inner cycle
;	3 stores, 6 loads, 26 total
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
; End of outer cycle

; Final cycle: N times
;	3 stores, 3 loads, 10 total
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
