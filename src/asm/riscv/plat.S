# Definition of some functions used throughout the code

.section .text
# Simple popcount
.global _popcount

_popcount:
add     t0, zero, a0
addi    t1, zero, 64
add     t2, zero, zero
addi    t3, zero, 0x1
addi    a0, zero, 0

loop_begin:
and     t4, t0, t3
bne     t4, t3, loop_end
addi    a0, a0, 1

loop_end:
srli    t0, t0, 1
addi    t2, t2, 1
bltu    t2, t1, loop_begin

ret

.global _die
_die:
    wfi
    j _die    
