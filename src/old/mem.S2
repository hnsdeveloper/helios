#Definition of functions that handle memory management

.section .text

.global _enable_address_translation
_enable_address_translation:
    srli a0, a0, 12
    li a1, 0x9000000000000000
    or a0, a0, a1
    addi t1, x0, 1
    slli t1, t1, 18
    csrrs t1, sstatus, t1
    sfence.vma x0, x0
    csrrw a0, satp, a0
    ret

.global _disable_address_translation
_disable_address_translation:
    add a0, x0, x0
    sfence.vma x0, x0
    csrrw a0, satp, a0
    slli a0, a0, 12
    ret

.global _get_current_page_table
_get_current_page_table:
    add a0, x0, x0
    csrrw a0, satp, a0
    add a1, x0, a0
    csrrw a0, satp, a0
    slli a0, a1, 12
    ret


