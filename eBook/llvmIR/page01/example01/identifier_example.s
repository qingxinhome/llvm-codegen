	.text
	.file	"identifier_example.ll"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$10, 4(%rsp)
	movl	$.L.str, %edi
	callq	puts@PLT
	movl	$144, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	my_golbal,@object               # @my_golbal
	.data
	.globl	my_golbal
	.p2align	2, 0x0
my_golbal:
	.long	42                              # 0x2a
	.size	my_golbal, 4

	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Hello World"
	.size	.L.str, 12

	.section	".note.GNU-stack","",@progbits
