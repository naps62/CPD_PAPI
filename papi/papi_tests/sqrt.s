	.file	"sqrt.c"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"%lf\n"
	.text
	.p2align 4,,15
.globl main
	.type	main, @function
main:
.LFB25:
	.cfi_startproc
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movsd	x(%rip), %xmm1
;sqrtsd	%xmm1, %xmm0
	ucomisd	%xmm0, %xmm0
	jp	.L5
.L2:
	movsd	%xmm0, x(%rip)
	movl	$.LC0, %esi
	movl	$1, %edi
	movl	$1, %eax
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	jmp	__printf_chk
.L5:
	.cfi_restore_state
	movapd	%xmm1, %xmm0
	call	sqrt
	jmp	.L2
	.cfi_endproc
.LFE25:
	.size	main, .-main
.globl x
	.data
	.align 8
	.type	x, @object
	.size	x, 8
x:
	.long	536870912
	.long	1108729137
	.ident	"GCC: (Ubuntu/Linaro 4.5.2-8ubuntu4) 4.5.2"
	.section	.note.GNU-stack,"",@progbits
