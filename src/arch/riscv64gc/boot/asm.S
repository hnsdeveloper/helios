# Boot for Risc-V 
.option norvc

.section .data.low

 
.section .text.low

.global _start
_start:

.option push
.option norelax
	la			gp, _lglobal_pointer
.option pop
	# No address translation for now
	csrw		satp, x0
		
	# Zeroes the BSS section
	la 			t0, _lbss_begin
	la			t1, _lbss_end
	bgeu		t0, t1, 2f
1:
	sd			x0, (t0)
	addi		t0, t0, 8
	bltu		t0, t1, 1b
2:	
	# Store the stack frame, so we can unwind whenever the kernel panics!
	add			fp, x0, x0
	la			sp, _lstack_end

	addi		sp, sp, -32
	# Store argc and argv
	sd			a0, 0(sp)
	sd			a1, 8(sp)

	# Store framepointer (none) and return address
	sd			x0, 16(sp)
	la			t4, 3f
	sd			t4, 24(sp)

	addi		sp, sp, -32
	addi		a2, sp, 0
	addi		a3, sp, 8
	addi		a4, sp, 16
	addi 		a5, sp, 24
3:
	# Calls bootmain, so we start using C code. a0 and a1 should contain argc and argv
	call		bootmain

4:
	# Arguments for main function
	ld			a2, 0(sp)
	ld			a3, 8(sp)
	ld			a1, 16(sp)
	ld			a4, 24(sp)
	addi		sp, sp, 32

	# The page table should be in a0, but we need it for argc and argv
	add			t0, x0, a2
	ld			a0, 0(sp)
	
	# Now we manipulate the page table pointer
	srli 		t0, t0, 12
    li 			t1, 0x9000000000000000
    or 			t0, t0, t1

	# We also manipulate the sstatus register
    addi 		t2, x0, 1
    slli 		t2, t2, 18
    csrrs 		t2, sstatus, t2

	la			gp, _global_pointer
	la			t5, _bss_begin
	la			t6, _bss_end
	add			fp, x0, x0
	la			sp, _stack_end
	sfence.vma 	x0, x0
    csrrw 		t0, satp, t0
	
	# Zeroes the BSS
5:
	sd			x0, (t5)
	addi		t5, t5, 8
	bltu		t5, t6, 1b
6:	
	la			ra, _main
	jr			ra
	