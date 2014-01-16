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
    asm volatile ("pushf\n" :: "a"(ucp));
    asm volatile ("pop 4*2(%eax)\n");

    asm volatile ("mov %eax, 4*3(%eax)\n");
    asm volatile ("mov %ecx, 4*4(%eax)\n");
    asm volatile ("mov %edx, 4*5(%eax)\n");
    asm volatile ("mov %ebx, 4*6(%eax)\n");
 /* asm volatile ("mov %esp, 4*7(%eax)\n"); omitted */
    asm volatile ("mov %ebp, 4*8(%eax)\n");
    asm volatile ("mov %esi, 4*9(%eax)\n");
    asm volatile ("mov %edi, 4*10(%eax)\n");

    asm volatile ("lea 4(%esp), %edx\n");
    asm volatile ("mov %edx, 4*0(%eax)\n");
    asm volatile ("xor %edx, %edx\n");
    asm volatile ("mov %edx, 4*1(%eax)\n");

    asm volatile ("mov (%esp), %edx\n");
    asm volatile ("mov %edx, 4*11(%eax)\n");

    return 0;
}

int __attribute__((optimize("omit-frame-pointer"), no_instrument_function)) setcontext(const ucontext_t *ucp)
{
    asm volatile ("1:\n" :: "a"(ucp));

 /* asm volatile ("mov 4*3(%eax), %eax\n");, omitted */
    asm volatile ("mov 4*4(%eax), %ecx\n");
 /* asm volatile ("mov 4*5(%eax), %edx\n");, omitted */
    asm volatile ("mov 4*6(%eax), %ebx\n");
 /* asm volatile ("mov 4*7(%eax), %esp\n");, omitted */
    asm volatile ("mov 4*8(%eax), %ebp\n");
    asm volatile ("mov 4*9(%eax), %esi\n");
    asm volatile ("mov 4*10(%eax), %edi\n");

    asm volatile ("mov 4*0(%eax), %esp\n");
    asm volatile ("add 4*1(%eax), %esp\n");

    asm volatile ("mov 4*11(%eax), %edx\n");
    asm volatile ("mov %eax, %ebx\n");

    asm volatile ("push 4*2(%eax)\n");
    asm volatile ("popf\n");

    asm volatile ("call *%edx\n");

    asm volatile ("mov 4*12(%ebx), %eax\n");
    asm volatile ("jmp 1b\n");
    __builtin_unreachable();
}

static void __attribute__((optimize("omit-frame-pointer"), noreturn, no_instrument_function)) makecontext_entrypoint(void)
{
    /* ebx = ucp, ecx = argc, ebp = arg[0], esi = arg[1], edi = arg[2] */
    asm volatile ("mov 4*3(%ebx), %eax\n"); /* eax = func */

    asm volatile ("jecxz 0f\n");
    asm volatile ("cmpb $1, %cl; je 1f\n");
    asm volatile ("cmpb $2, %cl; je 2f\n");
    asm volatile ("cmpb $3, %cl; je 3f\n");
    asm volatile ("cmpb $4, %cl; je 4f\n");

    asm volatile ("   mov 4*7(%ebx), %edx; push %edx\n");
    asm volatile ("4: mov 4*5(%ebx), %edx; push %edx\n");
    asm volatile ("3: push %edi\n");
    asm volatile ("2: push %esi\n");
    asm volatile ("1: push %ebp\n");
    asm volatile ("0: call *%eax\n"); /* call func(...), preserves ebx */

    asm volatile ("mov 4*12(%ebx), %eax\n");
    asm volatile ("push %eax\n");
    asm volatile ("call setcontext\n");
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
