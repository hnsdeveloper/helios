.section .rodata
kernel_panic_str:
.asciz "Kernel panic!!!! Register status:\r\n"

.section .text
.global _print_registers
_print_registers:
1:

# Store return address
sd a0, 16(sp)  

# So now we store the registers
# albeit x0 is always 0, it is stored as a "sanity check", so to say
sd   x0,  24(x2);
sd   x1,  32(x2);
#sd  x2,  40(x2);   # x2 is sp, which we are using now
sd   x3,  48(x2);
sd   x4,  56(x2);
sd   x5,  64(x2);
sd   x6,  72(x2);
sd   x7,  80(x2);
sd   x8,  88(x2);
sd   x9,  96(x2);
ld   x10, 0(sp)      # Restores a0 so we follow the sequence     
sd   x10, 104(x2)     
sd   x11, 112(x2)
sd   x12, 120(x2)
sd   x13, 128(x2)
sd   x14, 136(x2)
sd   x15, 144(x2)
sd   x16, 152(x2)
sd   x17, 160(x2)
sd   x18, 168(x2)
sd   x19, 176(x2)
sd   x20, 184(x2)
sd   x21, 192(x2)
sd   x22, 200(x2)
sd   x23, 208(x2)
sd   x24, 216(x2)
sd   x25, 224(x2)
sd   x26, 232(x2)
sd   x27, 240(x2)
sd   x28, 248(x2)
sd   x29, 256(x2)
sd   x30, 264(x2)
sd   x31, 272(x2)

# Now we store the original sp
add  x10, x0, x2
addi x10, x10, 304
sd   x10, 40(x2)

# Now we store PC
ld   x10, 8(sp)
sd   x10, 280(x2)

# Lets print the prologue
la   x10, kernel_panic_str
call panic_message_print

# Then print the registers
add  a0, x0, sp
addi a0, a0, 24
call print_registers

# Now we can resume execution as if nothing happened :D
ld x1, 32(x2)
ld a0, 16(sp)
addi sp, sp, 288
jr a0


.global _die
_die:
wfi
j _die