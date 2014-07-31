/*
 * Copyright (C) 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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

    call _eINT

    movl $0x0, __native_in_isr
    popal
    popfl

    ret
#else
.extern $_native_saved_eip
.extern $_native_isr_ctx
.extern $_native_cur_ctx
.extern $_native_in_isr

.globl _native_sig_leave_tramp

_native_sig_leave_tramp:
    pushl _native_saved_eip
    pushfl
    pushal

    pushl _native_isr_ctx
    pushl _native_cur_ctx
    call swapcontext
    addl $8, %esp

    call eINT

    movl $0x0, _native_in_isr
    popal
    popfl

    ret
#endif
