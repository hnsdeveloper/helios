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
	csrw		satp, zero
	
	# If the thread is not thread 0, go to 3f
	#csrr		t0, mhartid
	#bnez		t0, 3f

	# No trap handling for now
	#csrrs		x0, mtvec, x0
	#csrrs		x0, mideleg, x0
	#csrrs		x0, medeleg, x0
	
	# Zeroes the BSS section
	la 			t0, _bss_start
	la			t1, _bss_end
	bgeu		t0, t1, 2f
1:
	sd			x0, (t0)
	addi		t0, t0, 8
	bltu		t0, t1, 1b
2:	
	# Store the stack frame, so we can unwind whenever the kernel panics!
	add			fp, x0, x0
	la			sp, _stack_end

	addi		sp, sp, -16
	sd			x0, 0(sp)
	la			t4, 3f
	sd			t4, 8(sp)

	# Calls bootmain, so we start using C code. a0 and a1 should contain argc and argv
	call		bootmain
3:
	wfi
	j	3b

.global _kernel_start
_kernel_start:

	srli		a0, a0, 12		#	Storing page table address and setting up mode		
	li			t5, 0x9
	slli		t5, t5, 60
	or			a0, a0, t5
	csrrs		a0, satp, a0

	add			a0, x0, a1		#	Move argc and argv to a0 and a1 so they are there
	add			a1, x0, a2		# 	when we call kernel_main

	csrrs		x0, medeleg, x0		# 	all interrupts handled at M-MODE for now
	csrrs		x0, mideleg, x0		# 	all interrupts handled at M-MODE for now

	la 			t3, m_mode_trap_vector		# Setting machine mode trap vector
	csrrs		t3, mtvec, t3				

	addi		t0, x0, -1
	csrrs		t0, pmpaddr0, t0

	li			t0, 0b11000
	csrrs		t0, pmpcfg0, t0

	li			t0, (0b1 << 11) | (0 << 7) | (1 << 5) | (0 << 3) | (1 << 1)
	csrrs		t0, mstatus, t0

	li			t3, (1 << 3) | (1 << 7) | (1 << 11)
	csrrs		t3, mie, t3

	

	#la			ra, kernel_main

	# Loads kernel main to mepc 
	la  		t0, kernel_main
	csrrs 		t0, mepc, t0

	mret

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

