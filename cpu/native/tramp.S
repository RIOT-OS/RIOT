/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#define SYMBOL(name) name

/**
 * Defines new global symbol
 */
.macro GLOBAL_SYMBOL name
    .globl SYMBOL(\name)
    SYMBOL(\name):
.endm

/**
 * @brief Loads address immediate and then memory contents into register
 * @param register Register to load memory contents in
 * @param symbol Symbol whose memory contents to load
 */
.macro LOADMEM register, symbol
    ldr     \register, =\symbol
    ldr     \register, [\register]
.endm

/**
 * @brief Write register contents into memory
 * @param value Register containing value to write
 * @param register Helper register to use for loading address
 * @param symbol Symbol pointing to memory location to write to
 */
.macro STOREMEM value, register, symbol
    ldr     \register, =\symbol
    str     \value, [\register]
.endm

.text

#if defined(__arm__)

GLOBAL_SYMBOL _native_sig_leave_tramp
    /* save _native_user_fptr and registers */
    stmdb   sp!, {r0}
    ldr     r0, =SYMBOL(_native_user_fptr)
    ldr     r0, [r0]
    stmdb   sp!, {r0-r12}
    stmdb   sp!, {lr}

    /* exchange r0 and _native_user_fptr */
    ldr     r0, [sp,#56]
    ldr     r1, [sp,#4 ]
    str     r0, [sp,#4 ]
    str     r1, [sp,#56]

    /* call swapcontext ( _native_current_context, _native_isr_context ) */
    ldr     r2, =SYMBOL(_native_current_context)
    ldr     r0, [r2]
    ldr     r2, =SYMBOL(_native_isr_context)
    ldr     r1, [r2]
    bl      SYMBOL(swapcontext)

    /* reeanble interrupts */
    bl      SYMBOL(irq_enable)

    /* _native_in_isr = 0 */
    eor     r0, r0, r0
    ldr     r2, =SYMBOL(_native_in_isr)
    str     r0, [r2]

    /* restore registers, jump to (saved) _native_user_fptr */
    ldmia   sp!, {lr}
    ldmia   sp!, {r0-r12}
    ldmia   sp!, {pc}

GLOBAL_SYMBOL _native_isr_leave
    stmdb   sp!, {r0}
    ldr     r0, =SYMBOL(_native_user_fptr)
    ldr     r0, [r0]
    stmdb   sp!, {r0-r12}
    stmdb   sp!, {lr}

    /* exchange r0 and _native_user_fptr */
    ldr     r0, [sp,#56]
    ldr     r1, [sp,#4 ]
    str     r0, [sp,#4 ]
    str     r1, [sp,#56]

    /* _native_in_isr = 0 */
    eor     r0, r0, r0
    ldr     r1, =SYMBOL(_native_in_isr)
    str     r0, [r1]
    ldmia   sp!, {lr}
    ldmia   sp!, {r0-r12}
    ldmia   sp!, {pc}

#elif defined(__x86_64__)

GLOBAL_SYMBOL _native_sig_leave_tramp
    pushq   SYMBOL(_native_user_fptr)(%rip)
    pushfq

    pushq   %rax
    pushq   %rcx
    pushq   %rdx
    pushq   %rbx
    pushq   %rbp
    pushq   %rsi
    pushq   %rdi
    pushq   %r8
    pushq   %r9
    pushq   %r10
    pushq   %r11
    pushq   %r12
    pushq   %r13
    pushq   %r14
    pushq   %r15

    mov     SYMBOL(_native_isr_context)(%rip), %rsi
    mov     SYMBOL(_native_current_context)(%rip), %rdi

    call    SYMBOL(swapcontext)

    call    SYMBOL(irq_enable)

    movl    $0x0, SYMBOL(_native_in_isr)(%rip)
    popq    %r15
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %r11
    popq    %r10
    popq    %r9
    popq    %r8
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    popq    %rdx
    popq    %rcx
    popq    %rax

    popfq
    ret

GLOBAL_SYMBOL _native_isr_leave
    pushq   SYMBOL(_native_user_fptr)(%rip)
    movl    $0x0, SYMBOL(_native_in_isr)(%rip)
    ret

#elif defined(__i386__)

GLOBAL_SYMBOL _native_sig_leave_tramp
    pushl   SYMBOL(_native_user_fptr)
    pushfl
    pushal

    pushl   SYMBOL(_native_isr_context)
    pushl   SYMBOL(_native_current_context)
    call    SYMBOL(swapcontext)
    addl    $8, %esp

    call    SYMBOL(irq_enable)

    movl    $0x0, SYMBOL(_native_in_isr)
    popal
    popfl

    ret

GLOBAL_SYMBOL _native_isr_leave
    pushl   SYMBOL(_native_user_fptr)
    movl    $0x0, SYMBOL(_native_in_isr)
    ret

#else
# error "Unsupported architecture"
#endif

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
