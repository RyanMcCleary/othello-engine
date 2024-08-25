	.file	"find_magics.c"
	.text
	.p2align 4
	.globl	random_uint64
	.type	random_uint64, @function
random_uint64:
.LFB39:
	.cfi_startproc
	endbr64
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	call	rand@PLT
	movl	%eax, %ebx
	call	rand@PLT
	movzwl	%bx, %ebx
	movl	%eax, %r12d
	call	rand@PLT
	salq	$16, %r12
	movl	%eax, %ebp
	andl	$4294901760, %r12d
	call	rand@PLT
	salq	$32, %rbp
	movabsq	$281470681743360, %rdx
	salq	$48, %rax
	andq	%rdx, %rbp
	orq	%rbx, %rax
	popq	%rbx
	.cfi_def_cfa_offset 24
	orq	%r12, %rax
	orq	%rbp, %rax
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE39:
	.size	random_uint64, .-random_uint64
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Entered find_magic()"
	.text
	.p2align 4
	.globl	find_magic
	.type	find_magic, @function
find_magic:
.LFB40:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	movl	%edx, %r14d
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	movq	%rcx, %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdi, %r12
	leaq	.LC0(%rip), %rdi
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	movq	%rsi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	movl	%r8d, %ebx
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	puts@PLT
	movl	$64, %eax
	movq	%r13, 16(%rbp)
	movl	%ebx, %ecx
	subl	%ebx, %eax
	movl	$1, %r13d
	sall	%cl, %r13d
	movb	%al, 24(%rbp)
	movslq	%r13d, %r13
	leaq	0(,%r13,8), %r15
	.p2align 4,,10
	.p2align 3
.L6:
	testq	%r13, %r13
	je	.L5
	movq	%r15, %rdx
	movl	$255, %esi
	movq	%r12, %rdi
	call	memset@PLT
.L5:
	call	random_uint64
	movq	%rax, %rbx
	call	random_uint64
	andq	%rax, %rbx
	call	random_uint64
	andq	%rax, %rbx
	movq	%rbx, 0(%rbp)
	call	random_uint64
	movq	%rax, %rbx
	call	random_uint64
	andq	%rax, %rbx
	call	random_uint64
	movq	%rbp, %rdx
	movl	%r14d, %esi
	movq	%r12, %rdi
	andq	%rax, %rbx
	movq	%rbx, 8(%rbp)
	call	fill_table@PLT
	testq	%rax, %rax
	je	.L6
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
.LFE40:
	.size	find_magic, .-find_magic
	.p2align 4
	.globl	find_orthogonal_magics
	.type	find_orthogonal_magics, @function
find_orthogonal_magics:
.LFB41:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	leaq	-32768(%rsp), %r11
	.cfi_def_cfa 11, 32824
.LPSRL0:
	subq	$4096, %rsp
	orq	$0, (%rsp)
	cmpq	%r11, %rsp
	jne	.LPSRL0
	.cfi_def_cfa_register 7
	subq	$40, %rsp
	.cfi_def_cfa_offset 32864
	movq	%fs:40, %rax
	movq	%rax, 32792(%rsp)
	xorl	%eax, %eax
	movq	%rdi, 8(%rsp)
	leaq	16(%rsp), %r13
	movq	$0, (%rsp)
	.p2align 4,,10
	.p2align 3
.L14:
	movq	(%rsp), %rax
	movq	8(%rsp), %rbx
	xorl	%r14d, %r14d
	movl	%eax, %r12d
	movl	%eax, %ebp
	.p2align 4,,10
	.p2align 3
.L15:
	movb	%r12b, %r15b
	movl	%r14d, %edx
	movl	%r14d, %esi
	movl	%ebp, %edi
	movl	%r15d, %eax
	addl	$1, %r14d
	movb	%dl, %ah
	movl	%eax, %r15d
	call	orthogonal_mask@PLT
	movq	%rbx, %rsi
	movl	%r15d, %edx
	movq	%r13, %rdi
	movq	%rax, %rcx
	movl	$12, %r8d
	addq	$32, %rbx
	call	find_magic
	cmpl	$8, %r14d
	jne	.L15
	addq	$1, (%rsp)
	movq	(%rsp), %rax
	addq	$256, 8(%rsp)
	cmpq	$8, %rax
	jne	.L14
	movq	32792(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L20
	addq	$32808, %rsp
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
.L20:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE41:
	.size	find_orthogonal_magics, .-find_orthogonal_magics
	.p2align 4
	.globl	find_diagonal_magics
	.type	find_diagonal_magics, @function
find_diagonal_magics:
.LFB42:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$4096, %rsp
	.cfi_def_cfa_offset 4152
	orq	$0, (%rsp)
	subq	$40, %rsp
	.cfi_def_cfa_offset 4192
	movq	%fs:40, %rax
	movq	%rax, 4120(%rsp)
	xorl	%eax, %eax
	movq	%rdi, 8(%rsp)
	leaq	16(%rsp), %r13
	movq	$0, (%rsp)
	.p2align 4,,10
	.p2align 3
.L22:
	movq	(%rsp), %rax
	movq	8(%rsp), %rbx
	xorl	%r14d, %r14d
	movl	%eax, %r12d
	movl	%eax, %ebp
	.p2align 4,,10
	.p2align 3
.L23:
	movb	%r12b, %r15b
	movl	%r14d, %edx
	movl	%r14d, %esi
	movl	%ebp, %edi
	movl	%r15d, %eax
	addl	$1, %r14d
	movb	%dl, %ah
	movl	%eax, %r15d
	call	diagonal_mask@PLT
	movq	%rbx, %rsi
	movl	%r15d, %edx
	movq	%r13, %rdi
	movq	%rax, %rcx
	movl	$9, %r8d
	addq	$32, %rbx
	call	find_magic
	cmpl	$8, %r14d
	jne	.L23
	addq	$1, (%rsp)
	movq	(%rsp), %rax
	addq	$256, 8(%rsp)
	cmpq	$8, %rax
	jne	.L22
	movq	4120(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L28
	addq	$4136, %rsp
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
.L28:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE42:
	.size	find_diagonal_magics, .-find_diagonal_magics
	.section	.rodata.str1.1
.LC1:
	.string	"const bitboard %s[64] = {\n"
.LC2:
	.string	"   "
.LC3:
	.string	" C64(0x%016lx),"
.LC4:
	.string	" C64(0x%016lx)};\n"
	.text
	.p2align 4
	.globl	print_magic_initializer
	.type	print_magic_initializer, @function
print_magic_initializer:
.LFB43:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movq	%rdi, %rdx
	movq	%rsi, %r15
	movl	$1, %edi
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	leaq	.LC1(%rip), %rsi
	xorl	%eax, %eax
	leaq	2048(%r15), %r14
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	leaq	.LC2(%rip), %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	leaq	.LC3(%rip), %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	leaq	128(%r15), %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	call	__printf_chk@PLT
	.p2align 4,,10
	.p2align 3
.L31:
	movq	%r13, %rsi
	movl	$1, %edi
	leaq	-128(%rbp), %rbx
	xorl	%eax, %eax
	call	__printf_chk@PLT
.L30:
	movq	(%rbx), %rdx
	movq	%r12, %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	addq	$32, %rbx
	call	__printf_chk@PLT
	cmpq	%rbp, %rbx
	jne	.L30
	movq	stdout(%rip), %rsi
	movl	$10, %edi
	leaq	128(%rbx), %rbp
	call	putc@PLT
	cmpq	%r14, %rbp
	jne	.L31
	movq	%r13, %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	call	__printf_chk@PLT
	movq	%r12, %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	movq	1920(%r15), %rdx
	call	__printf_chk@PLT
	movq	%r12, %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	movq	1952(%r15), %rdx
	call	__printf_chk@PLT
	movq	%r12, %rsi
	movl	$1, %edi
	xorl	%eax, %eax
	movq	1984(%r15), %rdx
	call	__printf_chk@PLT
	movq	2016(%r15), %rdx
	addq	$8, %rsp
	.cfi_def_cfa_offset 56
	movl	$1, %edi
	popq	%rbx
	.cfi_def_cfa_offset 48
	leaq	.LC4(%rip), %rsi
	popq	%rbp
	.cfi_def_cfa_offset 40
	xorl	%eax, %eax
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	jmp	__printf_chk@PLT
	.cfi_endproc
.LFE43:
	.size	print_magic_initializer, .-print_magic_initializer
	.section	.rodata.str1.1
.LC5:
	.string	"d_magic_values"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB44:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	subq	$4096, %rsp
	.cfi_def_cfa_offset 4112
	orq	$0, (%rsp)
	subq	$16, %rsp
	.cfi_def_cfa_offset 4128
	xorl	%edi, %edi
	movq	%fs:40, %rax
	movq	%rax, 4104(%rsp)
	xorl	%eax, %eax
	leaq	2048(%rsp), %rbp
	call	time@PLT
	movl	%eax, %edi
	call	srand@PLT
	movq	%rsp, %rdi
	call	find_orthogonal_magics
	movq	%rbp, %rdi
	call	find_diagonal_magics
	movq	%rbp, %rsi
	leaq	.LC5(%rip), %rdi
	call	print_magic_initializer
	movq	4104(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L38
	addq	$4112, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 16
	xorl	%eax, %eax
	popq	%rbp
	.cfi_def_cfa_offset 8
	ret
.L38:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE44:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
