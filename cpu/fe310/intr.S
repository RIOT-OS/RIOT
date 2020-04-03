/*
 * Copyright (C) 2017, 2019 JP Bonn, Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "vendor/encoding.h"
#include "context_frame.h"

  .section      .text.entry
  .align 2
  .global trap_entry

trap_entry:
    /* Save registers to stack */
    addi sp, sp, -CONTEXT_FRAME_SIZE

    sw s0, s0_OFFSET(sp)
    sw s1, s1_OFFSET(sp)
    sw s2, s2_OFFSET(sp)
    sw s3, s3_OFFSET(sp)
    sw s4, s4_OFFSET(sp)
    sw s5, s5_OFFSET(sp)
    sw s6, s6_OFFSET(sp)
    sw s7, s7_OFFSET(sp)
    sw s8, s8_OFFSET(sp)
    sw s9, s9_OFFSET(sp)
    sw s10, s10_OFFSET(sp)
    sw s11, s11_OFFSET(sp)
    sw ra, ra_OFFSET(sp)
    sw t0, t0_OFFSET(sp)
    sw t1, t1_OFFSET(sp)
    sw t2, t2_OFFSET(sp)
    sw t3, t3_OFFSET(sp)
    sw t4, t4_OFFSET(sp)
    sw t5, t5_OFFSET(sp)
    sw t6, t6_OFFSET(sp)
    sw a0, a0_OFFSET(sp)
    sw a1, a1_OFFSET(sp)
    sw a2, a2_OFFSET(sp)
    sw a3, a3_OFFSET(sp)
    sw a4, a4_OFFSET(sp)
    sw a5, a5_OFFSET(sp)
    sw a6, a6_OFFSET(sp)
    sw a7, a7_OFFSET(sp)


    /* Get the interrupt cause, PC, and address */
    csrr a0, mcause
    csrr a1, mepc
    csrr a2, mtval

    /* Save return PC in stack frame */
    sw a1, pc_OFFSET(sp)

    /*  Get the active thread (could be NULL) */
    lw tp, sched_active_thread
    beqz tp, null_thread

    /* Save stack pointer of current thread */
    sw sp, SP_OFFSET_IN_THREAD(tp)

null_thread:
    /* Switch to ISR stack.  Interrupts are not nested so use fixed
     *  starting address and just abandon stack when finished. */
    la  sp, _sp

    /*  Call handle_trap with MCAUSE and MEPC register value as args */
    call handle_trap

    /*  Get the active thread (guaranteed to be non NULL) */
    lw tp, sched_active_thread

    /*  Load the thread SP of scheduled thread */
    lw sp, SP_OFFSET_IN_THREAD(tp)

    /*  Set return PC */
    lw a1, pc_OFFSET(sp)
    csrw mepc, a1

    /* Restore registers from stack */
    lw s0, s0_OFFSET(sp)
    lw s1, s1_OFFSET(sp)
    lw s2, s2_OFFSET(sp)
    lw s3, s3_OFFSET(sp)
    lw s4, s4_OFFSET(sp)
    lw s5, s5_OFFSET(sp)
    lw s6, s6_OFFSET(sp)
    lw s7, s7_OFFSET(sp)
    lw s8, s8_OFFSET(sp)
    lw s9, s9_OFFSET(sp)
    lw s10, s10_OFFSET(sp)
    lw s11, s11_OFFSET(sp)
    lw ra, ra_OFFSET(sp)
    lw t0, t0_OFFSET(sp)
    lw t1, t1_OFFSET(sp)
    lw t2, t2_OFFSET(sp)
    lw t3, t3_OFFSET(sp)
    lw t4, t4_OFFSET(sp)
    lw t5, t5_OFFSET(sp)
    lw t6, t6_OFFSET(sp)
    lw a0, a0_OFFSET(sp)
    lw a1, a1_OFFSET(sp)
    lw a2, a2_OFFSET(sp)
    lw a3, a3_OFFSET(sp)
    lw a4, a4_OFFSET(sp)
    lw a5, a5_OFFSET(sp)
    lw a6, a6_OFFSET(sp)
    lw a7, a7_OFFSET(sp)

    addi sp, sp, CONTEXT_FRAME_SIZE
    mret
