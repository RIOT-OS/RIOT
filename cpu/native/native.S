/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief Generates platform-dependent symbol name
 */
#define SYMBOL(name) name

/**
 * @brief Defines new global symbol
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
    stmdb     sp!, {r0}
    LOADMEM   r0, SYMBOL(_native_user_fptr)
    stmdb     sp!, {r0-r12}
    stmdb     sp!, {lr}

    /* exchange r0 and _native_user_fptr */
    ldr       r0, [sp,#56]
    ldr       r1, [sp,#4 ]
    str       r0, [sp,#4 ]
    str       r1, [sp,#56]

    /* call swapcontext(_native_current_context, _native_isr_context) */
    LOADMEM   r0, SYMBOL(_native_current_context)
    LOADMEM   r1, SYMBOL(_native_isr_context)
    bl        SYMBOL(swapcontext)

    /* reeanble interrupts */
    bl        SYMBOL(irq_enable)

    /* _native_in_isr = 0 */
    eor       r0, r0, r0
    STOREMEM  r0, r1, SYMBOL(_native_in_isr)

    /* restore registers, jump to (saved) _native_user_fptr */
    ldmia     sp!, {lr}
    ldmia     sp!, {r0-r12}
    ldmia     sp!, {pc}

GLOBAL_SYMBOL _native_isr_leave
    stmdb     sp!, {r0}
    LOADMEM   r0, SYMBOL(_native_user_fptr)
    stmdb     sp!, {r0-r12}
    stmdb     sp!, {lr}

    /* exchange r0 and _native_user_fptr */
    ldr       r0, [sp,#56]
    ldr       r1, [sp,#4 ]
    str       r0, [sp,#4 ]
    str       r1, [sp,#56]

    /* _native_in_isr = 0 */
    eor       r0, r0, r0
    STOREMEM  r0, r1, SYMBOL(_native_in_isr)
    ldmia     sp!, {lr}
    ldmia     sp!, {r0-r12}
    ldmia     sp!, {pc}

#elif defined(__x86_64__)

GLOBAL_SYMBOL _native_sig_leave_tramp
    /* Push (relative) return address onto stack */
    pushq   SYMBOL(_native_user_fptr)(%rip)
    /* Push RFLAGS register onto stack */
    pushfq
    /* Preserve general-purpose registers */
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

    /* Push swapcontext arguments onto stack (relative) */
    mov     SYMBOL(_native_isr_context)(%rip), %rsi
    mov     SYMBOL(_native_current_context)(%rip), %rdi
    /* call swapcontext(_native_current_context (RDI), _native_isr_context (RSI)) */
    call    SYMBOL(swapcontext)

    /* reeanble interrupts */
    call    SYMBOL(irq_enable)

    /* _native_in_isr = 0 */
    movl    $0x0, SYMBOL(_native_in_isr)(%rip)

    /* Restore general-purpose registers */
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
    /* Restore RFLAGS register*/
    popfq

    /* Pop and jump to _native_user_fptr */
    ret

GLOBAL_SYMBOL _native_isr_leave
    /* Push (relative) return address onto stack */
    pushq   SYMBOL(_native_user_fptr)(%rip)

    /* _native_in_isr = 0 */
    movl    $0x0, SYMBOL(_native_in_isr)(%rip)

    /* Pop and jump to _native_user_fptr */
    ret

GLOBAL_SYMBOL _start_task_func64
    /* Use r14 and r15 as scratch registers. */
    /* r8, r9 / r12 are used by glibc / libplatform on arm64 */
    /* This is the 64-bit function argument. RIOT threads can only receive one argument. */
    /* System V ABI says: first argument in RDI.
     * See: https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
     * > %rdi - used to pass 1st argument to functions */
    mov     %r15, %rdi
    /* Call user thread func. */
    jmp     *%r14

#elif defined(__i386__)

GLOBAL_SYMBOL _native_sig_leave_tramp
    /* Push return address onto stack */
    pushl   SYMBOL(_native_user_fptr)
    /* Push eflags register onto stack */
    pushfl
    /* Push all general-purpose registers */
    pushal

    /* Push swapcontext arguments onto stack */
    pushl   SYMBOL(_native_isr_context)
    pushl   SYMBOL(_native_current_context)
    /* call swapcontext(_native_current_context, _native_isr_context) */
    call    SYMBOL(swapcontext)
    /* Remove swapcontext arguments from stack */
    addl    $8, %esp

    /* reeanble interrupts */
    call    SYMBOL(irq_enable)

    /* _native_in_isr = 0 */
    movl    $0x0, SYMBOL(_native_in_isr)

    /* Pop all general-purpose registers */
    popal
    /* Pop eflags register */
    popfl

    /* Pop and jump to _native_user_fptr */
    ret

GLOBAL_SYMBOL _native_isr_leave
    /* Push return address onto stack */
    pushl   SYMBOL(_native_user_fptr)

    /* _native_in_isr = 0 */
    movl    $0x0, SYMBOL(_native_in_isr)

    /* Pop and jump to _native_user_fptr*/
    ret

#else
# error "Unsupported architecture"
#endif

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
