/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86-multithreading
 * @{
 *
 * @file
 * @brief       Coroutine helper functions.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <ucontext.h>

#include <stdarg.h>

int __attribute__((optimize("omit-frame-pointer"), no_instrument_function)) getcontext(ucontext_t *ucp)
{
    __asm__ volatile ("pushf\n" :: "a"(ucp));
    __asm__ volatile ("pop 4*2(%eax)\n");

    __asm__ volatile ("mov %eax, 4*3(%eax)\n");
    __asm__ volatile ("mov %ecx, 4*4(%eax)\n");
    __asm__ volatile ("mov %edx, 4*5(%eax)\n");
    __asm__ volatile ("mov %ebx, 4*6(%eax)\n");
 /* __asm__ volatile ("mov %esp, 4*7(%eax)\n"); omitted */
    __asm__ volatile ("mov %ebp, 4*8(%eax)\n");
    __asm__ volatile ("mov %esi, 4*9(%eax)\n");
    __asm__ volatile ("mov %edi, 4*10(%eax)\n");

    __asm__ volatile ("lea 4(%esp), %edx\n");
    __asm__ volatile ("mov %edx, 4*0(%eax)\n");
    __asm__ volatile ("xor %edx, %edx\n");
    __asm__ volatile ("mov %edx, 4*1(%eax)\n");

    __asm__ volatile ("mov (%esp), %edx\n");
    __asm__ volatile ("mov %edx, 4*11(%eax)\n");

    return 0;
}

int __attribute__((optimize("omit-frame-pointer"), no_instrument_function)) setcontext(const ucontext_t *ucp)
{
    __asm__ volatile ("1:\n" :: "a"(ucp));

 /* __asm__ volatile ("mov 4*3(%eax), %eax\n");, omitted */
    __asm__ volatile ("mov 4*4(%eax), %ecx\n");
 /* __asm__ volatile ("mov 4*5(%eax), %edx\n");, omitted */
    __asm__ volatile ("mov 4*6(%eax), %ebx\n");
 /* __asm__ volatile ("mov 4*7(%eax), %esp\n");, omitted */
    __asm__ volatile ("mov 4*8(%eax), %ebp\n");
    __asm__ volatile ("mov 4*9(%eax), %esi\n");
    __asm__ volatile ("mov 4*10(%eax), %edi\n");

    __asm__ volatile ("mov 4*0(%eax), %esp\n");
    __asm__ volatile ("add 4*1(%eax), %esp\n");

    __asm__ volatile ("mov 4*11(%eax), %edx\n");
    __asm__ volatile ("mov %eax, %ebx\n");

    __asm__ volatile ("push 4*2(%eax)\n");
    __asm__ volatile ("popf\n");

    __asm__ volatile ("call *%edx\n");

    __asm__ volatile ("mov 4*12(%ebx), %eax\n");
    __asm__ volatile ("jmp 1b\n");
    __builtin_unreachable();
}

static void __attribute__((optimize("omit-frame-pointer"), noreturn, no_instrument_function)) makecontext_entrypoint(void)
{
    /* ebx = ucp, ecx = argc, ebp = arg[0], esi = arg[1], edi = arg[2] */
    __asm__ volatile ("mov 4*3(%ebx), %eax\n"); /* eax = func */

    __asm__ volatile ("jecxz 0f\n");
    __asm__ volatile ("cmpb $1, %cl; je 1f\n");
    __asm__ volatile ("cmpb $2, %cl; je 2f\n");
    __asm__ volatile ("cmpb $3, %cl; je 3f\n");
    __asm__ volatile ("cmpb $4, %cl; je 4f\n");

    __asm__ volatile ("   mov 4*7(%ebx), %edx; push %edx\n");
    __asm__ volatile ("4: mov 4*5(%ebx), %edx; push %edx\n");
    __asm__ volatile ("3: push %edi\n");
    __asm__ volatile ("2: push %esi\n");
    __asm__ volatile ("1: push %ebp\n");
    __asm__ volatile ("0: call *%eax\n"); /* call func(...), preserves ebx */

    __asm__ volatile ("mov 4*12(%ebx), %eax\n");
    __asm__ volatile ("push %eax\n");
    __asm__ volatile ("call setcontext\n");
    __builtin_unreachable();
}

void makecontext(ucontext_t *ucp, makecontext_fun_t func, int argc, ...)
{
    ucp->uc_context.ip = (void *) (unsigned long) &makecontext_entrypoint;
    ucp->uc_context.registers.bx = (unsigned long) ucp;

    ucp->uc_context.registers.ax = (unsigned long) func;
    ucp->uc_context.registers.cx = argc > 0 ? argc : 0;

    if (argc <= 0) {
        return;
    }

    unsigned long *arg_registers[5] = {
        &ucp->uc_context.registers.bp,
        &ucp->uc_context.registers.si,
        &ucp->uc_context.registers.di,
        &ucp->uc_context.registers.dx,
        &ucp->uc_context.registers.sp,
    };
    va_list ap;
    va_start(ap, argc);
    for (int i = 0; i < argc; ++i) {
        *arg_registers[i] = va_arg(ap, unsigned long);
    }
    va_end(ap);
}

int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
    /* Valid sentinel: an address in .text section is not valid for a stack pointer. */
    oucp->uc_context.registers.sp = (unsigned long) &swapcontext;
    getcontext(oucp);
    if (oucp->uc_context.registers.sp == (unsigned long) &swapcontext) {
        oucp->uc_context.registers.sp = 0;
        setcontext(ucp);
    }
    return 0;
}
