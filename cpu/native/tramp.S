/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

.text

#ifdef __arm__

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
    bl      irq_enable

    /* _native_in_isr = 0 */
    eor     r0, r0, r0
    ldr     r2, =_native_in_isr
    str     r0, [r2]

    /* restore registers, jump to (saved) _native_saved_eip */
    ldmia   sp!, {lr}
    ldmia   sp!, {r0-r12}
    ldmia   sp!, {pc}

.globl _native_sig_leave_handler
_native_sig_leave_handler:
    stmdb sp!, {r0}
    ldr r0, =_native_saved_eip
    ldr r0, [r0]
    stmdb sp!, {r0-r12}
    stmdb sp!, {lr}
    /* exchange r0 and _native_saved_eip */
    ldr     r0, [sp,#56]
    ldr     r1, [sp,#4 ]
    str     r0, [sp,#4 ]
    str     r1, [sp,#56]
    /* _native_in_isr = 0 */
    eor     r0, r0, r0
    ldr     r1, =_native_in_isr
    str     r0, [r1]
    ldmia sp!, {lr}
    ldmia sp!, {r0-r12}
    ldmia sp!, {pc}

#else
.globl _native_sig_leave_tramp

#ifdef __x86_64__
_native_sig_leave_tramp:
    pushq _native_saved_eip(%rip)
    pushfq

    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    mov _native_isr_ctx(%rip), %rsi
    mov _native_cur_ctx(%rip), %rdi

    call swapcontext

    call irq_enable

    movl $0x0, _native_in_isr(%rip)
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    popfq
    ret
#else
_native_sig_leave_tramp:
    pushl _native_saved_eip
    pushfl
    pushal

    pushl _native_isr_ctx
    pushl _native_cur_ctx
    call swapcontext
    addl $8, %esp

    call irq_enable

    movl $0x0, _native_in_isr
    popal
    popfl

    ret
#endif

.globl _native_sig_leave_handler
_native_sig_leave_handler:
#ifdef __x86_64__
    pushq _native_saved_eip(%rip)
    movl $0x0, _native_in_isr(%rip)
#else
    pushl _native_saved_eip
    movl $0x0, _native_in_isr
#endif
    ret
#endif

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
