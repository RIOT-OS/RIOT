/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 * Copyright (C) 2025 carl-tud
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* arm64e/ARMv8.3 */
#if defined(__ARM_FEATURE_PAC_DEFAULT) || defined(__ARM_FEATURE_PAUTH)
# define NATIVE_HAS_PTRAUTH_RETURNS 1
#elif defined(__has_feature)
# if __has_feature(ptrauth_returns)
#  define NATIVE_HAS_PTRAUTH_RETURNS 1
# endif
#endif

#if !defined(NATIVE_HAS_PTRAUTH_RETURNS)
#  define NATIVE_HAS_PTRAUTH_RETURNS 0
#endif

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

#elif defined(__arm64__) || defined(__aarch64__)

.macro ARM64_RETURN_WITH_PTRAUTH
# if NATIVE_HAS_PTRAUTH_RETURNS
    /* arm64e/ARMv8.3 */
    retab
# else
    ret
# endif
.endm

GLOBAL_SYMBOL _native_sig_leave_tramp
    sub      sp, sp, #240
    /* Preserve general-purpose registers */
    stp      x0, x1, [sp]
    stp      x2, x3, [sp, #0x10]
    stp      x4, x5, [sp, #0x20]
    stp      x6, x7, [sp, #0x30]
    stp      x8, x9, [sp, #0x40]
    stp      x10, x11, [sp, #0x50]
    stp      x12, x13, [sp, #0x60]
    stp      x14, x15, [sp, #0x70]
    stp      x16, x17, [sp, #0x80]
    stp      x18, x19, [sp, #0x90]
    stp      x20, x21, [sp, #0xA0]
    stp      x22, x23, [sp, #0xB0]
    stp      x24, x25, [sp, #0xC0]
    stp      x26, x27, [sp, #0xD0]
    stp      x28, x29, [sp, #0xE0]
    /* Don't need to preserve x30 (= LR), we modify LR below */

    /* call swapcontext(_native_current_context, _native_isr_context) */
    LOADMEM  x0, SYMBOL(_native_current_context)
    LOADMEM  x1, SYMBOL(_native_isr_context)
    /* branch and link (set LR to @here0, branch to swapcontext) */
    bl       SYMBOL(swapcontext)

    /* @here0: */
    /* reenable interrupts */
    bl       SYMBOL(irq_enable)

    /* _native_in_isr = 0
     *
     * _native_in_isr is an int, not a full word-width pointer.
     * Must not write 64-bit value to &_native_in_isr as this would also overwrite whatever
     * comes after _native_in_isr in memory.
     *
     * w0 (rather than x0) is the 32-bit version we need to use. */
    mov      w0, #0x0
    STOREMEM w0, x1, SYMBOL(_native_in_isr)

    /* restore registers, jump to (saved) _native_user_fptr */
    ldp      x0, x1, [sp]
    ldp      x2, x3, [sp, #0x10]
    ldp      x4, x5, [sp, #0x20]
    ldp      x6, x7, [sp, #0x30]
    ldp      x8, x9, [sp, #0x40]
    ldp      x10, x11, [sp, #0x50]
    ldp      x12, x13, [sp, #0x60]
    ldp      x14, x15, [sp, #0x70]
    ldp      x16, x18, [sp, #0x80]
    ldp      x18, x19, [sp, #0x90]
    ldp      x20, x21, [sp, #0xA0]
    ldp      x22, x23, [sp, #0xB0]
    ldp      x24, x25, [sp, #0xC0]
    ldp      x26, x27, [sp, #0xD0]
    ldp      x28, x29, [sp, #0xE0]
    /* Don't need to restore x30 (= LR), we modify LR below */
    add      sp, sp, #240

    /* return to _native_user_fptr. See comments below. */
    LOADMEM  lr, SYMBOL(_native_user_fptr)
    ARM64_RETURN_WITH_PTRAUTH

GLOBAL_SYMBOL _native_isr_leave
    /* Save registers we use to reset _native_in_isr. */
    sub      sp, sp, #16
    stp      x0, x1, [sp]

    /* _native_in_isr = 0
     *
     * _native_in_isr is an int, not a full word-width pointer.
     * Must not write 64-bit value to &_native_in_isr as this would also overwrite whatever
     * comes after _native_in_isr in memory.
     *
     * w0 (rather than x0) is the 32-bit version we need to use. */
    mov      w0, #0x0
    STOREMEM w0, x1, SYMBOL(_native_in_isr)

    /* Restore registers used above. */
    ldp      x0, x1, [sp]
    add      sp, sp, #16

    /* Return to _native_user_fptr.
     * This is the return address of the call to getcontext in userspace. We captured that
     * that before in the buffer where the user thread stack also lives (_native_user_context()).
     *
     * Modifying LR is fine as the context is applied with setcontext and we manipulate LR directly
     * in util/ucontext.h. (With makecontext that would work properly due to _ctx_start).
     */
    LOADMEM  lr, SYMBOL(_native_user_fptr)

    /* In _isr_switch_to_user, we set _native_user_fptr to the presigned value of the [return
     * address in the call to getcontext (that was used to capture the machine state when
     * switching to kernel space (ISR))]. That return address was stored in LR and has been
     * signed by getcontext.
     *
     * ret/retab will read LR and set PC to that value.
     * PC isn't mutable on arm64 (not a register) */
    ARM64_RETURN_WITH_PTRAUTH

GLOBAL_SYMBOL _start_task_func64
    /* Use x27 and x28 as scratch registers. */
    /* x19 and x20 are used by glibc and libplatform on arm64 */
    /* This is the 64-bit function argument. RIOT threads can only receive one argument. */
    mov     x0, x28
    /* Branch there. */
    br      x27

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
    /* r8, r9 / r12 are used by glibc / libplatform on x86_64 */
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
