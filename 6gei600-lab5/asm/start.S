.section .text.boot
.align 7
.globl _start
_start:
    ldr x0, =_stack_top
    mov sp, x0

    ldr x1, =__bss_start
    ldr x2, =__bss_end
1:
    cmp x1, x2
    b.hs 2f
    str xzr, [x1], #8
    b 1b
2:

    mrs x0, CurrentEL
    and x0, x0, #0xC
    lsr x0, x0, #2

    cmp x0, #2
    b.ne skip_el2_setup

    ldr x0, =_stack_top
    msr SP_EL1, x0

    mov x0, #(1 << 31)
    msr HCR_EL2, x0

    mov x0, #0x5
    msr SPSR_EL2, x0

    adr x0, el1_entry
    msr ELR_EL2, x0

    eret

skip_el2_setup:
el1_entry:
    ldr x0, =vectors
    msr VBAR_EL1, x0
    isb

    msr DAIFClr, #2

    bl main

hang:
    wfe
    b hang

