.section .init

.option norvc

.type start, @function
.global start
start:
    .cfi_startproc

.option push
.option norelax
    la gp, global_pointer
.option pop

    /* reset satp */
    csrw satp, zero

    /* setup stack */
    la sp, stack_top

    /* clear the bss section */
    la t5, bss_start
    la t6, bss_end
bss_clear:
    sd zero, (t5)
    addi t5, t5, 8
    bltu t5, t6, bss_clear

    la t0, kmain
    csrw mepc, t0

    /* jump to kernel */
    tail kmain

    .cfi_endproc

.end