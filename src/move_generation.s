	.file	"move_generation.c"
	.text
	.p2align 4
	.globl	print_board
	.type	print_board, @function
print_board:
.LFB39:
	.cfi_startproc
	endbr64
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	movq	%rsi, %r14
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	movl	$1, %r13d
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movl	$8, %ebp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	.p2align 4,,10
	.p2align 3
.L2:
	leal	-8(%rbp), %ebx
	jmp	.L6
	.p2align 4,,10
	.p2align 3
.L13:
	movl	$87, %edi
	call	putc@PLT
.L4:
	addl	$1, %ebx
	cmpl	%ebp, %ebx
	je	.L11
.L6:
	movq	%r13, %rax
	movl	%ebx, %ecx
	movq	stdout(%rip), %rsi
	salq	%cl, %rax
	testq	%rax, %r12
	jne	.L12
	testq	%rax, %r14
	jne	.L13
	movl	$42, %edi
	addl	$1, %ebx
	call	putc@PLT
	cmpl	%ebp, %ebx
	jne	.L6
.L11:
	movq	stdout(%rip), %rsi
	movl	$10, %edi
	leal	8(%rbx), %ebp
	call	putc@PLT
	cmpl	$64, %ebx
	jne	.L2
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
	.p2align 4,,10
	.p2align 3
.L12:
	.cfi_restore_state
	movl	$66, %edi
	call	putc@PLT
	jmp	.L4
	.cfi_endproc
.LFE39:
	.size	print_board, .-print_board
	.p2align 4
	.globl	square_mask
	.type	square_mask, @function
square_mask:
.LFB40:
	.cfi_startproc
	endbr64
	movzbl	%dil, %edi
	movzbl	%sil, %esi
	movl	$1, %eax
	leal	(%rsi,%rdi,8), %ecx
	salq	%cl, %rax
	ret
	.cfi_endproc
.LFE40:
	.size	square_mask, .-square_mask
	.p2align 4
	.globl	signed_shift
	.type	signed_shift, @function
signed_shift:
.LFB41:
	.cfi_startproc
	endbr64
	movl	%esi, %ecx
	movq	%rdi, %rax
	salq	%cl, %rax
	movl	%esi, %ecx
	negl	%ecx
	shrq	%cl, %rdi
	testl	%esi, %esi
	cmovle	%rdi, %rax
	ret
	.cfi_endproc
.LFE41:
	.size	signed_shift, .-signed_shift
	.p2align 4
	.globl	orthogonal_mask
	.type	orthogonal_mask, @function
orthogonal_mask:
.LFB42:
	.cfi_startproc
	endbr64
	movzbl	%dil, %edi
	movl	$126, %eax
	movabsq	$282578800148736, %rdx
	leal	0(,%rdi,8), %ecx
	salq	%cl, %rax
	movl	%esi, %ecx
	salq	%cl, %rdx
	orq	%rdx, %rax
	ret
	.cfi_endproc
.LFE42:
	.size	orthogonal_mask, .-orthogonal_mask
	.p2align 4
	.globl	diagonal_mask
	.type	diagonal_mask, @function
diagonal_mask:
.LFB43:
	.cfi_startproc
	endbr64
	movl	%edi, %edx
	addl	$1, %edi
	leal	1(%rsi), %r8d
	pushq	%r12
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
	cmpb	$6, %dil
	pushq	%rbp
	.cfi_def_cfa_offset 24
	.cfi_offset 6, -24
	setle	%bpl
	cmpb	$6, %r8b
	pushq	%rbx
	.cfi_def_cfa_offset 32
	.cfi_offset 3, -32
	setle	%r11b
	testb	%bpl, %bpl
	je	.L32
	testb	%r11b, %r11b
	je	.L32
	movl	%r8d, %r10d
	movl	%edi, %r9d
	xorl	%eax, %eax
	movl	$1, %r12d
	.p2align 4,,10
	.p2align 3
.L21:
	movzbl	%r9b, %ebx
	movzbl	%r10b, %ecx
	addl	$1, %r9d
	addl	$1, %r10d
	leal	(%rcx,%rbx,8), %ecx
	movq	%r12, %rbx
	salq	%cl, %rbx
	orq	%rbx, %rax
	cmpb	$6, %r9b
	jg	.L20
	cmpb	$6, %r10b
	jle	.L21
.L20:
	subl	$1, %esi
	testb	%sil, %sil
	setg	%r10b
	jle	.L23
	testb	%bpl, %bpl
	je	.L23
	movl	%esi, %r9d
	movl	$1, %ebp
	.p2align 4,,10
	.p2align 3
.L24:
	movzbl	%dil, %ebx
	movsbl	%r9b, %ecx
	addl	$1, %edi
	subl	$1, %r9d
	leal	(%rcx,%rbx,8), %ecx
	movq	%rbp, %rbx
	salq	%cl, %rbx
	orq	%rbx, %rax
	cmpb	$6, %dil
	jg	.L23
	testb	%r9b, %r9b
	jg	.L24
.L23:
	subl	$1, %edx
	testb	%dl, %dl
	setg	%bl
	jle	.L19
	testb	%r11b, %r11b
	je	.L26
	movl	%edx, %edi
	movl	$1, %r9d
	.p2align 4,,10
	.p2align 3
.L27:
	leal	(%r8,%rdi,8), %ecx
	movq	%r9, %r11
	subl	$1, %edi
	addl	$1, %r8d
	salq	%cl, %r11
	orq	%r11, %rax
	testb	%dil, %dil
	jle	.L26
	cmpb	$6, %r8b
	jle	.L27
.L26:
	testb	%bl, %bl
	je	.L19
	testb	%r10b, %r10b
	je	.L19
	movl	$1, %edi
	.p2align 4,,10
	.p2align 3
.L30:
	leal	(%rsi,%rdx,8), %ecx
	movq	%rdi, %rbx
	subl	$1, %edx
	subl	$1, %esi
	salq	%cl, %rbx
	orq	%rbx, %rax
	testb	%dl, %dl
	jle	.L19
	testb	%sil, %sil
	jg	.L30
.L19:
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%rbp
	.cfi_def_cfa_offset 16
	popq	%r12
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L32:
	.cfi_restore_state
	xorl	%eax, %eax
	jmp	.L20
	.cfi_endproc
.LFE43:
	.size	diagonal_mask, .-diagonal_mask
	.p2align 4
	.globl	generalized_ray_flip
	.type	generalized_ray_flip, @function
generalized_ray_flip:
.LFB44:
	.cfi_startproc
	endbr64
	movq	%rsi, %r10
	movl	%r8d, %esi
	movq	%rcx, %r11
	andq	%rcx, %rdi
	xorl	%r9d, %r9d
	negl	%esi
	jmp	.L75
	.p2align 4,,10
	.p2align 3
.L78:
	orq	%rax, %r9
.L75:
	movq	%rdx, %rax
	movl	%r8d, %ecx
	salq	%cl, %rax
	movl	%esi, %ecx
	shrq	%cl, %rdx
	testl	%r8d, %r8d
	cmovle	%rdx, %rax
	movq	%rdi, %rdx
	andq	%rax, %rdx
	jne	.L78
	andq	%r10, %r11
	testq	%rax, %r11
	cmove	%rdx, %r9
	movq	%r9, %rax
	ret
	.cfi_endproc
.LFE44:
	.size	generalized_ray_flip, .-generalized_ray_flip
	.p2align 4
	.globl	flip_north
	.type	flip_north, @function
flip_north:
.LFB45:
	.cfi_startproc
	endbr64
	shrq	$8, %rdx
	movq	%rdi, %rcx
	xorl	%eax, %eax
	andq	%rdx, %rcx
	je	.L81
	.p2align 4,,10
	.p2align 3
.L82:
	shrq	$8, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L82
.L81:
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE45:
	.size	flip_north, .-flip_north
	.p2align 4
	.globl	flip_south
	.type	flip_south, @function
flip_south:
.LFB46:
	.cfi_startproc
	endbr64
	salq	$8, %rdx
	movq	%rdi, %rcx
	xorl	%eax, %eax
	andq	%rdx, %rcx
	je	.L87
	.p2align 4,,10
	.p2align 3
.L88:
	salq	$8, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L88
.L87:
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE46:
	.size	flip_south, .-flip_south
	.p2align 4
	.globl	flip_east
	.type	flip_east, @function
flip_east:
.LFB47:
	.cfi_startproc
	endbr64
	movabsq	$-72340172838076674, %rax
	addq	%rdx, %rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L93
	.p2align 4,,10
	.p2align 3
.L94:
	orq	%rdx, %rax
	leaq	(%rcx,%rcx), %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L94
.L93:
	andq	%rdx, %rsi
	movabsq	$-72340172838076674, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE47:
	.size	flip_east, .-flip_east
	.p2align 4
	.globl	flip_west
	.type	flip_west, @function
flip_west:
.LFB48:
	.cfi_startproc
	endbr64
	movabsq	$9187201950435737471, %rax
	shrq	%rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L99
	.p2align 4,,10
	.p2align 3
.L100:
	shrq	%rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L100
.L99:
	andq	%rdx, %rsi
	movabsq	$9187201950435737471, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE48:
	.size	flip_west, .-flip_west
	.p2align 4
	.globl	flip_northeast
	.type	flip_northeast, @function
flip_northeast:
.LFB49:
	.cfi_startproc
	endbr64
	movabsq	$-72340172838076674, %rax
	shrq	$7, %rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L105
	.p2align 4,,10
	.p2align 3
.L106:
	shrq	$7, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L106
.L105:
	andq	%rdx, %rsi
	movabsq	$-72340172838076674, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE49:
	.size	flip_northeast, .-flip_northeast
	.p2align 4
	.globl	flip_northwest
	.type	flip_northwest, @function
flip_northwest:
.LFB50:
	.cfi_startproc
	endbr64
	movabsq	$9187201950435737471, %rax
	shrq	$9, %rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L111
	.p2align 4,,10
	.p2align 3
.L112:
	shrq	$9, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L112
.L111:
	andq	%rdx, %rsi
	movabsq	$9187201950435737471, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE50:
	.size	flip_northwest, .-flip_northwest
	.p2align 4
	.globl	flip_southeast
	.type	flip_southeast, @function
flip_southeast:
.LFB51:
	.cfi_startproc
	endbr64
	movabsq	$-72340172838076674, %rax
	salq	$9, %rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L117
	.p2align 4,,10
	.p2align 3
.L118:
	salq	$9, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L118
.L117:
	andq	%rdx, %rsi
	movabsq	$-72340172838076674, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE51:
	.size	flip_southeast, .-flip_southeast
	.p2align 4
	.globl	flip_southwest
	.type	flip_southwest, @function
flip_southwest:
.LFB52:
	.cfi_startproc
	endbr64
	movabsq	$9187201950435737471, %rax
	salq	$7, %rdx
	andq	%rax, %rdi
	xorl	%eax, %eax
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	je	.L123
	.p2align 4,,10
	.p2align 3
.L124:
	salq	$7, %rcx
	orq	%rdx, %rax
	movq	%rcx, %rdx
	movq	%rdi, %rcx
	andq	%rdx, %rcx
	jne	.L124
.L123:
	andq	%rdx, %rsi
	movabsq	$9187201950435737471, %rdx
	testq	%rdx, %rsi
	movl	$0, %edx
	cmove	%rdx, %rax
	ret
	.cfi_endproc
.LFE52:
	.size	flip_southwest, .-flip_southwest
	.p2align 4
	.globl	flip_all
	.type	flip_all, @function
flip_all:
.LFB53:
	.cfi_startproc
	endbr64
	movq	%rdi, %rax
	movq	%rdx, %rcx
	movq	%rsi, %rdi
	xorl	%r8d, %r8d
	shrq	$8, %rcx
	movq	%rax, %rsi
	andq	%rcx, %rsi
	je	.L129
	.p2align 4,,10
	.p2align 3
.L130:
	orq	%rcx, %r8
	movq	%rsi, %rcx
	movq	%rax, %rsi
	shrq	$8, %rcx
	andq	%rcx, %rsi
	jne	.L130
.L129:
	testq	%rcx, %rdi
	movl	$0, %ecx
	movq	%rax, %r9
	cmove	%rcx, %r8
	movq	%rdx, %rcx
	xorl	%esi, %esi
	salq	$8, %rcx
	andq	%rcx, %r9
	je	.L132
	.p2align 4,,10
	.p2align 3
.L133:
	orq	%rcx, %rsi
	movq	%r9, %rcx
	movq	%rax, %r9
	salq	$8, %rcx
	andq	%rcx, %r9
	jne	.L133
.L132:
	orq	%r8, %rsi
	testq	%rcx, %rdi
	movabsq	$-72340172838076674, %rcx
	cmovne	%rsi, %r8
	andq	%rax, %rcx
	leaq	(%rdx,%rdx), %rsi
	xorl	%r9d, %r9d
	movq	%rcx, %r10
	andq	%rsi, %r10
	je	.L135
	.p2align 4,,10
	.p2align 3
.L136:
	orq	%rsi, %r9
	leaq	(%r10,%r10), %rsi
	movq	%rcx, %r10
	andq	%rsi, %r10
	jne	.L136
.L135:
	movabsq	$-72340172838076674, %r10
	andq	%rdi, %rsi
	orq	%r8, %r9
	testq	%r10, %rsi
	movabsq	$9187201950435737471, %rsi
	cmovne	%r9, %r8
	andq	%rsi, %rax
	movq	%rdx, %rsi
	xorl	%r9d, %r9d
	shrq	%rsi
	movq	%rax, %r10
	andq	%rsi, %r10
	je	.L138
	.p2align 4,,10
	.p2align 3
.L139:
	orq	%rsi, %r9
	movq	%r10, %rsi
	movq	%rax, %r10
	shrq	%rsi
	andq	%rsi, %r10
	jne	.L139
.L138:
	movabsq	$9187201950435737471, %r10
	andq	%rdi, %rsi
	orq	%r8, %r9
	testq	%r10, %rsi
	movq	%rdx, %rsi
	movq	%rcx, %r10
	cmovne	%r9, %r8
	shrq	$7, %rsi
	xorl	%r9d, %r9d
	andq	%rsi, %r10
	je	.L141
	.p2align 4,,10
	.p2align 3
.L142:
	orq	%rsi, %r9
	movq	%r10, %rsi
	shrq	$7, %rsi
	movq	%rsi, %r10
	andq	%rcx, %r10
	jne	.L142
.L141:
	movabsq	$-72340172838076674, %r10
	andq	%rdi, %rsi
	orq	%r8, %r9
	testq	%r10, %rsi
	movq	%rdx, %rsi
	movq	%rax, %r10
	cmovne	%r9, %r8
	shrq	$9, %rsi
	xorl	%r9d, %r9d
	andq	%rsi, %r10
	je	.L144
	.p2align 4,,10
	.p2align 3
.L145:
	orq	%rsi, %r9
	movq	%r10, %rsi
	shrq	$9, %rsi
	movq	%rsi, %r10
	andq	%rax, %r10
	jne	.L145
.L144:
	movabsq	$9187201950435737471, %r10
	andq	%rdi, %rsi
	orq	%r8, %r9
	testq	%r10, %rsi
	movq	%rdx, %rsi
	movq	%rcx, %r10
	cmovne	%r9, %r8
	salq	$9, %rsi
	xorl	%r9d, %r9d
	andq	%rsi, %r10
	je	.L147
	.p2align 4,,10
	.p2align 3
.L148:
	orq	%rsi, %r9
	movq	%r10, %rsi
	salq	$9, %rsi
	movq	%rsi, %r10
	andq	%rcx, %r10
	jne	.L148
.L147:
	movabsq	$-72340172838076674, %rcx
	andq	%rdi, %rsi
	orq	%r8, %r9
	testq	%rcx, %rsi
	cmovne	%r9, %r8
	salq	$7, %rdx
	xorl	%ecx, %ecx
	movq	%rdx, %rsi
	andq	%rax, %rsi
	je	.L150
	.p2align 4,,10
	.p2align 3
.L151:
	orq	%rdx, %rcx
	movq	%rsi, %rdx
	salq	$7, %rdx
	movq	%rdx, %rsi
	andq	%rax, %rsi
	jne	.L151
.L150:
	movabsq	$9187201950435737471, %rax
	andq	%rdx, %rdi
	orq	%r8, %rcx
	testq	%rax, %rdi
	cmovne	%rcx, %r8
	movq	%r8, %rax
	ret
	.cfi_endproc
.LFE53:
	.size	flip_all, .-flip_all
	.p2align 4
	.globl	magic_hash
	.type	magic_hash, @function
magic_hash:
.LFB54:
	.cfi_startproc
	endbr64
	imulq	(%rdx), %rdi
	movzbl	24(%rdx), %ecx
	imulq	8(%rdx), %rsi
	xorq	%rsi, %rdi
	movq	%rdi, %rax
	shrq	%cl, %rax
	ret
	.cfi_endproc
.LFE54:
	.size	magic_hash, .-magic_hash
	.p2align 4
	.globl	fill_table
	.type	fill_table, @function
fill_table:
.LFB55:
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
	movq	%rdi, %r12
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
	.cfi_def_cfa_offset 64
	movq	16(%rdx), %r14
	testq	%r14, %r14
	je	.L185
	movl	%esi, %eax
	movq	%rdx, %r15
	movzbl	%sil, %edx
	movl	$1, %r13d
	movzbl	%ah, %eax
	movzbl	%al, %eax
	leal	(%rax,%rdx,8), %ecx
	salq	%cl, %r13
.L191:
	movq	%r14, %rbp
	xorq	$-1, %rbp
	je	.L186
	movzbl	24(%r15), %eax
	movq	%rbp, %r11
	movl	%eax, 4(%rsp)
	jmp	.L190
	.p2align 4,,10
	.p2align 3
.L187:
	cmpq	%rax, %rdx
	jne	.L192
	subq	$1, %r11
	andq	%rbp, %r11
	je	.L186
.L190:
	movq	(%r15), %rbx
	movq	8(%r15), %rax
	movq	%r13, %rdx
	movq	%r11, %rsi
	movzbl	4(%rsp), %ecx
	movq	%r14, %rdi
	imulq	%r11, %rax
	imulq	%r14, %rbx
	xorq	%rax, %rbx
	shrq	%cl, %rbx
	call	flip_all
	leaq	(%r12,%rbx,8), %rcx
	movq	(%rcx), %rdx
	cmpq	$-1, %rdx
	jne	.L187
	subq	$1, %r11
	movq	%rax, (%rcx)
	andq	%rbp, %r11
	jne	.L190
.L186:
	leaq	-1(%r14), %rdi
	andq	16(%r15), %rdi
	movq	%rdi, %r14
	jne	.L191
.L185:
	addq	$8, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	movq	%r12, %rax
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
	.p2align 4,,10
	.p2align 3
.L192:
	.cfi_restore_state
	addq	$8, %rsp
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
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
.LFE55:
	.size	fill_table, .-fill_table
	.p2align 4
	.globl	clear_table
	.type	clear_table, @function
clear_table:
.LFB56:
	.cfi_startproc
	endbr64
	testq	%rsi, %rsi
	je	.L202
	leaq	0(,%rsi,8), %rdx
	movl	$255, %esi
	jmp	memset@PLT
	.p2align 4,,10
	.p2align 3
.L202:
	ret
	.cfi_endproc
.LFE56:
	.size	clear_table, .-clear_table
	.globl	d_magic_values
	.section	.rodata
	.align 32
	.type	d_magic_values, @object
	.size	d_magic_values, 512
d_magic_values:
	.quad	74776051777760
	.quad	2378052370216525826
	.quad	594512542810784771
	.quad	612918651586609168
	.quad	146437631511627792
	.quad	318863749163044
	.quad	19338211080028192
	.quad	4631961304935039104
	.quad	2346661296633565192
	.quad	6926536503923703875
	.quad	-4034926147417406848
	.quad	1441152984615617156
	.quad	2308096089011519497
	.quad	568482425209092
	.quad	5084212700913824
	.quad	18023332611895360
	.quad	2595481997267730560
	.quad	-9217138901124034016
	.quad	576601559603937792
	.quad	72630440686520322
	.quad	289356294316673280
	.quad	2814749971324936
	.quad	1200671865995460
	.quad	72761419069919232
	.quad	288520647230423120
	.quad	1238495957730001600
	.quad	181340345030971524
	.quad	9051179736645640
	.quad	145685357813760
	.quad	36662150087049536
	.quad	4540985174397952
	.quad	1158023247158204672
	.quad	6919791102090322208
	.quad	-9222804362435229696
	.quad	288234778495420464
	.quad	-9223090285925890032
	.quad	-3602808780710870528
	.quad	1134773896610336
	.quad	2918895564366283776
	.quad	5071639385621780
	.quad	2453345253328027916
	.quad	1152996271800549520
	.quad	4683816185605652608
	.quad	-9185080384758988764
	.quad	576812598310228002
	.quad	4629722551877107840
	.quad	576884133572116757
	.quad	37154706841686024
	.quad	916997059658240
	.quad	-9223345575559168000
	.quad	74889955335802883
	.quad	-9221083935591361720
	.quad	292751585212244034
	.quad	40534596760174594
	.quad	72761290774380673
	.quad	4503758608269888
	.quad	2342180219844305536
	.quad	-8935141379381657248
	.quad	18014494074676517
	.quad	2306476624275056193
	.quad	18695996908544
	.quad	5637210576
	.quad	31595634923020805
	.quad	54043269113316704
	.globl	o_magic_values
	.align 32
	.type	o_magic_values, @object
	.size	o_magic_values, 512
o_magic_values:
	.quad	36029347848544536
	.quad	1170936040556331021
	.quad	72084119760143504
	.quad	2449975875410602048
	.quad	144132788986315040
	.quad	72061992135131656
	.quad	288232854381658256
	.quad	4755802375816609828
	.quad	1157565842796319780
	.quad	432627114366746624
	.quad	289672935558619280
	.quad	18594940683027329
	.quad	576601799029694465
	.quad	2324033372533359232
	.quad	-8619748940704612096
	.quad	-9214224099028490496
	.quad	5210700502999762064
	.quad	18017216008560640
	.quad	3460523766787641344
	.quad	1125934538817796
	.quad	4549779251077136
	.quad	162131785877590790
	.quad	180425734957826336
	.quad	-8623819341252516596
	.quad	18618101260140544
	.quad	-9218727523644817404
	.quad	1460371348843938324
	.quad	2341876352456069120
	.quad	297294758602082305
	.quad	504685183534965768
	.quad	288810919364952322
	.quad	36175214601617664
	.quad	144959907244806176
	.quad	1234004164965105672
	.quad	1104780722208
	.quad	-4591331840886829032
	.quad	-8492662958390247320
	.quad	-9223371469916995294
	.quad	9007371665801745
	.quad	-9214364148224227133
	.quad	577657026056847360
	.quad	9104162986147840
	.quad	3949450330542594
	.quad	4467854540802
	.quad	4612864850182864898
	.quad	2458969803180803072
	.quad	577023703464845318
	.quad	1442850147875749894
	.quad	5201798325896019976
	.quad	54043219421446272
	.quad	585476886214475808
	.quad	612524820811219712
	.quad	9007347431704656
	.quad	2739881821356490768
	.quad	749866938909330440
	.quad	-9142297894995344640
	.quad	90107454221781506
	.quad	1190093931308793986
	.quad	2305869690087411777
	.quad	4922583943513899041
	.quad	34695405574
	.quad	1444178389627043841
	.quad	282043254704132
	.quad	6918796219091264530
	.globl	diagonal_table
	.bss
	.align 32
	.type	diagonal_table, @object
	.size	diagonal_table, 262144
diagonal_table:
	.zero	262144
	.globl	orthogonal_table
	.align 32
	.type	orthogonal_table, @object
	.size	orthogonal_table, 2097152
orthogonal_table:
	.zero	2097152
	.globl	diagonal_magics
	.align 32
	.type	diagonal_magics, @object
	.size	diagonal_magics, 2048
diagonal_magics:
	.zero	2048
	.globl	orthogonal_magics
	.align 32
	.type	orthogonal_magics, @object
	.size	orthogonal_magics, 2048
orthogonal_magics:
	.zero	2048
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
