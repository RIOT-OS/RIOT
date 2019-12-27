/*
 * Copyright (C) 2013, 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger1@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

.text

.globl _native_sig_leave_tramp
_native_sig_leave_tramp:
    pushf

    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %rsp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    movabs $_native_saved_program_counter, %rax
    push %rax

    movabs $_native_isr_ctx, %rax
    push %rax

    movabs $_native_cur_ctx, %rax
    push %rax

    call swapcontext

    add $16, %rsp

    call irq_enable

    movabs $_native_in_isr, %rax
    movq $0x0, (%rax)
    
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rsp
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax

    popf

    ret

.globl _native_sig_leave_handler
_native_sig_leave_handler:
    movabs $_native_in_isr, %rax
    movq $0x0, (%rax)

    movabs $_native_saved_program_counter, %rax
    movq (%rax), %rax

    jmp %rax