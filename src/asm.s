; Start of outer cycle: executed N times
;	3 stores (32 + 32 + 32 + 32), 1 load (64)
	movsd	(%rcx), %xmm8
	movsd	8(%rcx), %xmm7
	movsd	16(%rcx), %xmm6
	movq	%r10, (%rsp)
	movsd	(%rsp), %xmm5

; Start of inner cycle: N*N
;	6 stores, 3 loads
	movsd	(%rax), %xmm2
	movsd	8(%rax), %xmm1
	movsd	16(%rax), %xmm0
	movq	%xmm3, 8(%rsp)
	movq	8(%rsp), %rdx
	movq	%rdx, (%rsp)
	movq	%rdx, sqrt_val(%rip)
	movsd	(%rsp), %xmm3
	movsd	48(%rax), %xmm0

; end of inner cycle
;	3 stores, 6 loads
	movsd	24(%rcx), %xmm2
	movsd	32(%rcx), %xmm1
	movsd	40(%rcx), %xmm0
	movsd	%xmm1, 32(%rcx)
	movsd	%xmm5, 24(%rcx)
	movsd	%xmm8, 56(%rcx)
	movsd	%xmm7, 64(%rcx)
	movsd	%xmm4, 40(%rcx)
	movsd	%xmm6, 72(%rcx)
; End of outer cycle

; Final cycle: N times
;	3 stores, 3 loads
	movq	56(%r8), %rax
	movq	%rax, (%r8)
	movq	64(%r8), %rax
	movq	%rax, 8(%r8)
	movq	72(%r8), %rax
	movq	%rax, 16(%r8)
