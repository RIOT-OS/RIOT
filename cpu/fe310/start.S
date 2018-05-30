/*
 * Copyright (C) 2017 JP Bonn, Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
    .section .init
    .globl _start
    .type _start,@function

_start:
    .cfi_startproc
    .cfi_undefined ra
.option push
.option norelax
    la gp, __global_pointer$
.option pop
    la sp, _sp


    /* Load data section */
    la a0, _data_lma
    la a1, _data
    la a2, _edata
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b
2:

    /* Clear bss section */
    la a0, __bss_start
    la a1, _end
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
2:

    /* Call global constructors */
    la a0, __libc_fini_array
    call atexit
    call __libc_init_array


    /* Initialize board and start kernel */
    call board_init
    call kernel_init

    /* Loop forever (should never get here) */
1:
    j 1b

    .cfi_endproc
