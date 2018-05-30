/*
 * Copyright (C) 2017 JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "vendor/encoding.h"
#include "context_frame.h"

/* from platform.h TODO:fix this hard code.... */
CLINT_CTRL_ADDR = 0x02000000

  .section      .text.entry
  .align 2
  .global trap_entry

trap_entry:
    /*
     * Save all regs on the currently active stack.
     * This coule be the active thread's stack,
     * or if no thread is active, it is saved on ISR stack
     * (if initial startup) or on the deactivated threads
     * stack (in the case of thread exit).  In the latter
     * two cases the stack is just abandoned.
     */
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
    sw tp, tp_OFFSET(sp)
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


    /* Get the interrupt cause */
    csrr a0, mcause

    /* Save active thread stack pointer in a callee save register */
    mv s1, sp

    /* Switch to ISR stack.  Interrupts are not nested so use fixed
     *  starting address and just abandon stack when finished. */
    la  sp, _sp
    addi sp, sp, -4

    /*  Is it a software interrupt? */
    li t0, 0x80000003
    beq a0, t0, context_switch

    /*  Call handle_trap with MCAUSE register value as arg */
    jal handle_trap

    /*  See if a context switch was requested by the ISR */
    lw a0, sched_context_switch_request
    bnez a0, context_switch

    /*  Restore active thread stack pointer */
    mv sp, s1

    /* Restore remaining registers */
trap_exit:
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
    lw tp, tp_OFFSET(sp)
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


 context_switch:
    /*  clear the software interrupt */
    li t0, CLINT_CTRL_ADDR
    sw zero, (t0)

    /*  save the active thread's PC prior to interrupt on the stack */
    csrr a0, mepc
    sw a0, pc_OFFSET(s1)

    /*  get the active thread - it may be 0 if none currently active */
    lw t0, sched_active_thread
    /*  was there a previously running thread? */
    beqz t0, no_sp_save
    /*  if so, save the thread's SP in the _thread structure */
    sw s1,SP_OFFSET_IN_THREAD(t0)

no_sp_save:
    /*  all current thread state is saved - schedule a new thread */
    call sched_run
    lw tp, sched_active_thread

    /*  set the threads SP from the newly scheduled thread
     *  and abandon ISR stack. */
    lw sp, SP_OFFSET_IN_THREAD(tp)

    /*  restore the PC */
    lw a0, pc_OFFSET(sp)
    csrw mepc, a0

    j trap_exit
