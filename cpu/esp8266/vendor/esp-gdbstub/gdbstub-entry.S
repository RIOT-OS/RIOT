/******************************************************************************
 * Copyright 2015 Espressif Systems
 *
 * Description: Assembly routines for the gdbstub
 *
 * License: ESPRESSIF MIT License
 *******************************************************************************/


#include "gdbstub-cfg.h"
#include "gdbstub-exc.h"

#include <xtensa/config/specreg.h>
#include <xtensa/config/core-isa.h>
#include <xtensa/corebits.h>

#include "xtensa/xtensa_context.h"

#define DEBUG_PC        (EPC + XCHAL_DEBUGLEVEL)
#define DEBUG_EXCSAVE   (EXCSAVE + XCHAL_DEBUGLEVEL)
#define DEBUG_PS        (EPS + XCHAL_DEBUGLEVEL)

.global gdbstub_regs
.global gdbstub_debug_exception_entry

#if GDBSTUB_USE_OWN_STACK
.global gdbstub_exceptionStack
#endif

    .text
    .literal_position

    .text
    .align  4
/**
 * @brief   Debugging exception routine; it's called by the debugging vector
 */
gdbstub_debug_exception_entry:

    /*
     * All registers except A2 are intact when we arrive here. The original
     * contents of A2 was save in the EXCSAVE2/DEBUG_EXCSAVE register by
     * _DebugExceptionVector stub. EPC2/DEBUG_PC register contains the
     * original PC and EPC2/DEBUG_PC the original PS register
     */

    /* Save all regs to standard exception frame structure XtExcFrame */
    movi    a2, gdbstub_regs

    s32i    a0, a2, XT_STK_A0   /* save A0 (return address) before we used it */
    s32i    a1, a2, XT_STK_A1   /* save A1 (stack pointer) */ 

    rsr     a0, DEBUG_PC        /* read original PC from EPC2 and save it */
    s32i    a0, a2, XT_STK_PC   

    rsr     a0, DEBUG_PS        /* read original PS from ESP2 and save it */    
    s32i    a0, a2, XT_STK_PS

    rsr     a0, DEBUG_EXCSAVE   /* read original A2 from EXCSAVE2 and save it */
    s32i    a0, a2, XT_STK_A2

    s32i    a3, a2, XT_STK_A3   /* save remaining A registers */
    s32i    a4, a2, XT_STK_A4
    s32i    a5, a2, XT_STK_A5
    s32i    a6, a2, XT_STK_A6
    s32i    a7, a2, XT_STK_A7
    s32i    a8, a2, XT_STK_A8
    s32i    a9, a2, XT_STK_A9
    s32i    a10, a2, XT_STK_A10
    s32i    a11, a2, XT_STK_A11
    s32i    a12, a2, XT_STK_A12
    s32i    a13, a2, XT_STK_A13
    s32i    a14, a2, XT_STK_A14
    s32i    a15, a2, XT_STK_A15

    rsr     a0, SAR             /* read SAR and save it */
    s32i    a0, a2, XT_STK_SAR

#if XCHAL_HAVE_LOOPS
    rsr     a0, lbeg
    s32i    a0, a2, XT_STK_LBEG
    rsr     a0, lend
    s32i    a0, a2, XT_STK_LEND
    rsr     a0, lcount
    s32i    a0, a2, XT_STK_LCOUNT
#endif

#ifdef XT_USE_SWPRI
    rsr     a0, vpri
    s32i    a0, a2, XT_STK_VPRI
#endif

#ifdef XT_USE_OVLY
    rsr     a0, ovly
    s32i    a0, a2, XT_STK_OVLY
#endif

    /* Save additional registers required for gdb_stub */
    movi    a2, gdbstub_regs + XtExcFrameSize
    rsr     a0, LITBASE
    s32i    a0, a2, XT_STK_LITBASE
    rsr     a0, 176
    s32i    a0, a2, XT_STK_SR176
    rsr     a0, 208
    s32i    a0, a2, XT_STK_SR208
    rsr     a0, DEBUGCAUSE
    s32i    a0, a2, XT_STK_REASON

#if GDBSTUB_USE_OWN_STACK
    /* Move to our own stack */
    movi a1, gdbstub_exceptionStack + GDBSTUB_STACK_SIZE - 4
#endif

    /*
     * If ICOUNT is -1, disable it by setting it to 0, otherwise we will
     * keep triggering on the same instruction.
     */
    rsr     a2, ICOUNT
    movi    a3, -1
    bne     a2, a3, noIcountReset
    movi    a3, 0
    wsr     a3, ICOUNT

noIcountReset:
    rsr     a2, ps
    addi    a2, a2, -PS_EXCM_MASK
    wsr     a2, ps
    rsync

    /* Call into the C code to do the actual handling. */
    call0    gdbstub_handle_debug_exception

DebugExceptionExit:

    rsr     a2, ps
    addi    a2, a2, PS_EXCM_MASK
    wsr     a2, ps
    rsync

    /* Restore registers from the gdbstub_regs struct. */

    movi    a2, gdbstub_regs + XtExcFrameSize

#if 0
    /* TODO: check whether it is really necessary to recover SR178 and SR208
     * Some versions of gcc do not understand instruction 'wsr <n>' where n is
     * the decimal number of the special register. A hand-assembled version of
     * instruction would have to be used instead.
     *
     *      .byte   0x00, <n>, 0x13
     *
     * However, writing to SR176 or SR208 leads to an IllegalInstruction
     * exception
     */

    l32i    a0, a2, XT_STK_SR208
    wsr     a0, 208
    l32i    a0, a2, XT_STK_SR176
    wsr     a0, 176
#endif

    l32i    a0, a2, XT_STK_LITBASE
    wsr     a0, LITBASE

    movi    a2, gdbstub_regs

#ifdef XT_USE_OVLY
    l32i    a0, a2, XT_STK_OVLY
    wsr     a0, ovly
#endif

#ifdef XT_USE_SWPRI
    l32i    a0, a2, XT_STK_VPRI
    wsr     a0, vpri
#endif

#if XCHAL_HAVE_LOOPS
    l32i    a0, a2, XT_STK_LCOUNT
    wsr     a0, lcount
    l32i    a0, a2, XT_STK_LEND
    wsr     a0, lend
    l32i    a0, a2, XT_STK_LBEG
    wsr     a0, lbeg
#endif

    l32i    a0, a2, XT_STK_SAR
    wsr     a0, sar
    l32i    a15, a2, XT_STK_A15
    l32i    a14, a2, XT_STK_A14
    l32i    a13, a2, XT_STK_A13
    l32i    a12, a2, XT_STK_A12
    l32i    a11, a2, XT_STK_A11
    l32i    a10, a2, XT_STK_A10
    l32i    a9, a2, XT_STK_A9
    l32i    a8, a2, XT_STK_A8
    l32i    a7, a2, XT_STK_A7
    l32i    a6, a2, XT_STK_A6
    l32i    a5, a2, XT_STK_A5
    l32i    a4, a2, XT_STK_A4
    l32i    a3, a2, XT_STK_A3

    l32i    a0, a2, XT_STK_A2   /* read original A2 and save it to EXCSAVE2 */
    wsr     a0, DEBUG_EXCSAVE
    l32i    a0, a2, XT_STK_PS   /* read original PS and save it to EPS2 */
    wsr     a0, DEBUG_PS
    l32i    a0, a2, XT_STK_PC   /* read original PC and save it to EPS2 */
    wsr     a0, DEBUG_PC

    l32i    a1, a2, XT_STK_A1   /* restore A1 (stack pointer) */
    l32i    a0, a2, XT_STK_A0   /* restore A0 (return address) */

    /* Read back vector-saved a2 value, put back address of this routine. */
    movi    a2, gdbstub_debug_exception_entry
    xsr     a2, DEBUG_EXCSAVE

    /* All done. Return to where we came from. */
    rfi     XCHAL_DEBUGLEVEL


    .global gdbstub_save_extra_sfrs_for_exception
    .align 4
/*
 * The Xtensa standard exception handlers does not save all the special
 * function register things. This bit of assembly fills the gdbstub_regs struct
 * with them.
 */
gdbstub_save_extra_sfrs_for_exception:

    /* a14-a15 are only saved by standard exception handlers for Windowed ABI */
    #ifdef __XTENSA_CALL0_ABI__
    movi    a2, gdbstub_regs
    s32i    a14, a2, XT_STK_A14
    s32i    a15, a2, XT_STK_A15
    #endif

    /* Save additional registers required for gdb_stub */
    movi    a2, gdbstub_regs + XtExcFrameSize
    rsr     a3, LITBASE
    s32i    a3, a2, XT_STK_LITBASE
    rsr     a3, 176
    s32i    a3, a2, XT_STK_SR176
    rsr     a3, 208
    s32i    a3, a2, XT_STK_SR208
    rsr     a3, EXCCAUSE
    s32i    a3, a2, XT_STK_REASON

    ret


    .global gdbstub_init_debug_entry
    .global _DebugExceptionVector
    .align    4
/*
 *  This puts the following 2 instructions into the debug exception vector:
 *        xsr    a2, DEBUG_EXCSAVE
 *        jx    a2
 */
gdbstub_init_debug_entry:

    movi    a2, _DebugExceptionVector
    movi    a3, 0xa061d220
    s32i    a3, a2, 0
    movi    a3, 0x00000002
    s32i    a3, a2, 4

    /* Tell the just-installed debug vector where to go. */
    movi    a2, gdbstub_debug_exception_entry
    wsr     a2, DEBUG_EXCSAVE

    ret


    .global gdbstub_icount_ena_single_step
    .align 4
/*
 * Set up ICOUNT register to step one single instruction
 */
gdbstub_icount_ena_single_step:
    movi    a3, XCHAL_DEBUGLEVEL /* Only count steps in non-debug mode */
    movi    a2, -2
    wsr     a3, ICOUNTLEVEL
    wsr     a2, ICOUNT
    isync
    ret

/*
 * The following routines all assume that only one breakpoint and watchpoint
 * is available, which is the case for the ESP8266 Xtensa core.
 */

    .global gdbstub_set_hw_breakpoint
    .align 4
/*
 * set an hw breakpoint
 * paramters: a2 = addr, a3 = len (unused here)
 */
gdbstub_set_hw_breakpoint:

call0 40000080
    rsr     a4, IBREAKENABLE
    bbsi    a4, 0, return_w_error
    wsr     a2, IBREAKA
    movi    a2, 1
    wsr     a2, IBREAKENABLE
    isync
    movi    a2, 1
    ret


    .global gdbstub_del_hw_breakpoint
    .align 4
/*
 * delete an hw breakpoint
 * paramters: a2 = addr
 */
gdbstub_del_hw_breakpoint:

    rsr     a5, IBREAKENABLE
    bbci    a5, 0, return_w_error
    rsr     a3, IBREAKA
    bne     a3, a2, return_w_error
    movi    a2,0
    wsr     a2, IBREAKENABLE
    isync
    movi    a2, 1
    ret


    .global gdbstub_set_hw_watchpoint
    .align 4
/*
 * set an hw breakpoint
 * paramters: a2 = addr, a3 = mask, a4 = type (1=read, 2=write, 3=access)
 */
gdbstub_set_hw_watchpoint:

    /* Check if any of the masked address bits are set. If so, that is an error. */
    movi    a5,0x0000003F
    xor     a5, a5, a3
    bany    a2, a5, return_w_error

    /* Check if watchpoint already is set */
    rsr     a5, DBREAKC
    movi    a6, 0xC0000000
    bany    a6, a5, return_w_error

    /* Set watchpoint */
    wsr     a2, DBREAKA

    /* Combine type and mask */
    movi    a6, 0x3F
    and     a3, a3, a6
    slli    a4, a4, 30
    or      a3, a3, a4
    wsr     a3, DBREAKC

    mov     a2, a3
    isync
    ret


    .global gdbstub_del_hw_watchpoint
    .align 4
/*
 * delete a hw breakpoint
 * paramters: a2 = addr
 */
gdbstub_del_hw_watchpoint:
    /* see if the address matches */
    rsr     a3, DBREAKA
    bne     a3, a2, return_w_error
    /* see if the bp actually is set */
    rsr     a3, DBREAKC
    movi    a2, 0xC0000000
    bnone   a3, a2, return_w_error
    /* Disable bp */
    movi    a2,0
    wsr     a2,DBREAKC
    movi    a2,1
    isync
    ret

return_w_error:
    movi    a2, 0
    ret


    .global gdbstub_do_break_breakpoint_addr
    .global gdbstub_do_break
    .align  4
/*
 * Breakpoint, with an attempt at a functional function prologue and epilogue...
 */
gdbstub_do_break:
    addi    a1, a1, -16
    s32i    a15, a1, 12
    mov     a15, a1

gdbstub_do_break_breakpoint_addr:
    break 0,0

    mov     a1, a15
    l32i    a15, a1, 12
    addi    a1, a1, 16
    ret
