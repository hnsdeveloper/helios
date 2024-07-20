# Boot for Risc-V 
.option norvc

.section .data.low

.section .text.start

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
	la			ra, _start

	# Return address and frame pointer saved on the stack.
	addi 		sp, sp, -16
	sd			ra, 0(sp)
	sd			fp, 8(sp)

	addi		sp, sp, -128
	sd			a0, 0(sp)
	sd			a1, 8(sp)
	add			a2, sp, x0
	addi		a2, a2, 16	

3:
	# Calls bootmain, so we start using C code. a0 and a1 should contain argc and argv
	call		bootmain
4:
	add			a0, x0, sp
	addi		a0, a0, 16

	ld			t0, 24(a0)
	
	# Now we manipulate the page table pointer
	srli 		t0, t0, 12
    li 			t1, RISCV_MEM_MODE_MASK
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
	bltu		t5, t6, 5b
6:	
	la			ra, _main
	addi		sp, sp, -16
	sd			fp, 0(sp)
	sd			ra, 8(sp)
	jr 			ra
	
7:	
	wfi
	j 			7b
	
