/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

.text

#ifdef __MACH__
.globl __native_sig_leave_tramp
__native_sig_leave_tramp:
    pushl __native_saved_eip
    pushfl
    pushal

    pushl __native_isr_ctx
    pushl __native_cur_ctx
    call _swapcontext
    addl $8, %esp

    call _enableIRQ

    movl $0x0, __native_in_isr
    popal
    popfl

    ret
#elif __arm__

.globl _native_sig_leave_tramp
_native_sig_leave_tramp:
    /* save _native_saved_eip and registers */
    stmdb   sp!, {r0}
    ldr     r0, =_native_saved_eip
    ldr     r0, [r0]
    stmdb   sp!, {r0-r12}
    stmdb   sp!, {lr}

    /* exchange r0 and _native_saved_eip */
    ldr     r0, [sp,#56]
    ldr     r1, [sp,#4 ]
    str     r0, [sp,#4 ]
    str     r1, [sp,#56]

    /* call swapcontext ( _native_cur_ctx, _native_isr_ctx ) */
    ldr     r2, =_native_cur_ctx
    ldr     r0, [r2]
    ldr     r2, =_native_isr_ctx
    ldr     r1, [r2]
    bl      swapcontext

    /* reeanble interrupts */
    bl      enableIRQ

    /* _native_in_isr = 0 */
    eor     r0, r0, r0
    ldr     r2, =_native_in_isr
    str     r0, [r2]

    /* restore registers, jump to (saved) _native_saved_eip */
    ldmia   sp!, {lr}
    ldmia   sp!, {r0-r12}
    ldmia   sp!, {pc}

#else
.globl _native_sig_leave_tramp

_native_sig_leave_tramp:
    pushl _native_saved_eip
    pushfl
    pushal

    pushl _native_isr_ctx
    pushl _native_cur_ctx
    call swapcontext
    addl $8, %esp

    call enableIRQ

    movl $0x0, _native_in_isr
    popal
    popfl

    ret
#endif
