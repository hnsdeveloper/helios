# Boot for Risc-V 
.option norvc

.section .data
 
.global _kernel_page_addr
_kernel_page_addr:
.dword 0

.section .text.init

.global _start
_start:
.option push
.option norelax
	la		gp, _global_pointer
.option pop
	# No address translation for now
	csrw	satp, zero

	la 		t0, m_mode_trap_vector
	csrw	mtvec, t0
	
	# If the thread is not thread 0, go to 3f
	csrr	t0, mhartid
	bnez	t0, 3f
	
	# Zeroes the BSS section
	la 		t0, _bss_start
	la		t1, _bss_end
	bgeu	t0, t1, 2f
1:
	sd		x0, (t0)
	addi	t0, t0, 8
	bltu	t0, t1, 1b
2:	
	# Store the stack frame, so we can unwind whenever the kernel panics!
	add		fp, x0, x0
	la		sp, _stack_end

	addi	sp, sp, -16
	sd		x0, 0(sp)
	la		t4, 3f
	sd		t4, 8(sp)

	# Calls bootmain, so we start using C code. a0 and a1 should contain argc and argv
	call	bootmain
3:
	wfi
	j	3b

# Control registers, set the stack, mstatus, mepc,
# and mtvec to return to the main function.
# li		t5, 0xffff;
# csrw	medeleg, t5
# csrw	mideleg, t5


# We use mret here so that the mstatus register
# is properly updated.
#li		t0, (0b11 << 11) | (1 << 7) | (1 << 3)
#csrw	mstatus, t0
#la		t1, kmain
#csrw	mepc, t1
#la		t2, asm_trap_vector
#csrw	mtvec, t2
#li		t3, (1 << 3) | (1 << 7) | (1 << 11)
#csrw	mie, t3
#la		ra, 4f
#mret
#4:
#	wfi
#	j	4b

