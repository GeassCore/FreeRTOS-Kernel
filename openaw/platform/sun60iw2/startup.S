/* E902 (RV32E) reset entry — linked at 0x4000a000 */
	.section .text.init, "ax", @progbits
	.align 2
	.globl _start
	.type _start, @function
_start:
	/* Interrupts off */
	csrci	mstatus, 0x8
	csrw	mie, zero

	/* GP / SP */
	.option push
	.option norelax
	la	gp, __global_pointer$
	.option pop
	la	sp, _stack_top

	/* Clear BSS */
	la	a0, _bss_start
	la	a1, _bss_end
1:
	bgeu	a0, a1, 2f
	sw	zero, 0(a0)
	addi	a0, a0, 4
	j	1b
2:
	jal	platform_early_init
	jal	main
3:
	j	3b
	.size _start, .-_start
