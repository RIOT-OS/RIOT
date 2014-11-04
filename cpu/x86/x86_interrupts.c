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
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       Interrupt entry points for x86.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_interrupts.h"
#include "x86_memory.h"
#include "x86_ports.h"
#include "x86_registers.h"
#include "x86_threading.h"

#include <attributes.h>
#include <cpu.h>
#include <sched.h>
#include <thread.h>
#include <ucontext.h>

#include <stdint.h>
#include <stdio.h>

#define ASM_FUN_ATTRIBUTES \
    __attribute__((noinline)) \
    __attribute__((no_instrument_function)) \
    __attribute__((optimize("Os", "omit-frame-pointer")))

static const char *const exception_name[X86_MAX_INTERRUPTS] = {
    [0x00] = "Divide by zero",
    [0x01] = "Debug",
    [0x02] = "Non-maskable Interrupt",
    [0x03] = "Breakpoint",
    [0x04] = "Overflow",
    [0x05] = "Bound range",
    [0x06] = "Invalid Opcode",
    [0x07] = "Device not available",
    [0x08] = "Double Fault",
    [0x09] = NULL,
    [0x0a] = NULL,
    [0x0b] = NULL,
    [0x0c] = NULL,
    [0x0d] = "General Protection Fault",
    [0x0e] = "Page Fault",
    [0x0f] = NULL,
    [0x10] = "x87 Floating-Point Exception",
    [0x11] = "Alignment Check",
    [0x12] = "Machine Check",
    [0x13] = NULL,
    [0x14] = NULL,
    [0x15] = NULL,
    [0x16] = NULL,
    [0x17] = NULL,
    [0x18] = NULL,
    [0x19] = NULL,
    [0x1a] = NULL,
    [0x1b] = NULL,
    [0x1c] = NULL,
    [0x1d] = NULL,
    [0x1e] = NULL,
    [0x1f] = NULL,
    [0x20] = "PIC PIT",
    [0x21] = "PIC KBC1",
    [0x22] = NULL,
    [0x23] = "PIC RS232 2/4",
    [0x24] = "PIC RS232 1/3",
    [0x25] = "PIC LPT2",
    [0x26] = "PIC floppy",
    [0x27] = "PIC LPT1",
    [0x28] = "PIC RTC",
    [0x29] = "PIC #9",
    [0x2a] = "PIC ATA4",
    [0x2b] = "PIC ATA3",
    [0x2c] = "PIC KBC2",
    [0x2d] = NULL,
    [0x2e] = "PIC ATA1",
    [0x2f] = "PIC ATA2",
};

uint8_t x86_current_interrupt;
unsigned long x86_current_interrupt_error_code;
struct x86_pushad x86_interrupted_ctx;
char x86_interrupt_handler_stack[2048];

static x86_intr_handler_t x86_registered_interrupts[X86_MAX_INTERRUPTS];

static void print_stacktrace(unsigned long bp, unsigned long ip)
{
    puts("<stack trace>");
    printf("  %08lx\n", ip);
    for (unsigned max_depth = 0; max_depth < 30; ++max_depth) {
        uint64_t pte = x86_get_pte(bp) & x86_get_pte(bp + 4);
        if (!(pte & PT_P) || !(pte & PT_PWT)) {
            puts("  ???");
            break;
        }

        unsigned long *bp_ = (void *) bp;
        ip = bp_[1];
        if (ip == 0) {
            break;
        }
        printf("  %08lx\n", ip);
        bp = bp_[0];
        if (bp == 0) {
            break;
        }
    }
    puts("</stack trace>");
}

void x86_print_registers(struct x86_pushad *orig_ctx, unsigned long error_code)
{
    unsigned long *sp = (void *) orig_ctx->sp; /* ip, cs, flags */
    printf("EAX=%08lx  ECX=%08lx  EDX=%08lx  EBX=%08lx\n", orig_ctx->ax, orig_ctx->cx, orig_ctx->dx, orig_ctx->bx);
    printf("ESP=%08lx  EBP=%08lx  ESI=%08lx  EDI=%08lx\n", orig_ctx->sp, orig_ctx->bp, orig_ctx->si, orig_ctx->di);
    printf("Error code=%08lx\n", error_code);
    printf("CR0=%08x  CR2=%08x  CR3=%08x  CR4=%08x\n", cr0_read(), cr2_read(), cr3_read(), cr4_read());
    printf("EIP=%04lx:%08lx  EFLAGS=%08lx\n", sp[1], sp[0], sp[2]);

    print_stacktrace(orig_ctx->bp, sp[0]);
}

static void intr_handler_default(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code)
{
    printf("Unhandled interrupt 0x%02x (%s)\n", intr_num, exception_name[intr_num]);
    x86_print_registers(orig_ctx, error_code);
    puts("Halting.");
    x86_hlt();
}

void x86_int_exit(void) NORETURN;

static void continue_after_intr(void)
{
    ucontext_t *ctx = (ucontext_t *) sched_active_thread->sp;
    x86_interrupted_ctx = ctx->uc_context.registers;
    asm volatile (
        "push %0\n"      /* flags */
        "push $0x0008\n" /* cs */
        "push %1\n"      /* ip */
        "jmp x86_int_exit"
        :: "g"(ctx->__intr.flags), "g"(ctx->__intr.ip)
    );
    __builtin_unreachable();
}

static unsigned in_intr_handler = 0, old_intr;
void x86_int_handler(void)
{
    switch (in_intr_handler++) {
        case 0:
            break;
        case 1:
            printf("Interrupt 0x%02x (%s) while handling 0x%02x (%s)\n",
                   x86_current_interrupt, exception_name[x86_current_interrupt],
                   old_intr, exception_name[old_intr]);
            x86_print_registers(&x86_interrupted_ctx, x86_current_interrupt_error_code);
            puts("Halting.");
            x86_hlt();
        default:
            x86_hlt();
    }
    old_intr = x86_current_interrupt;

    bool old_in_isr = x86_in_isr;
    x86_in_isr = true;

    x86_intr_handler_t intr_handler = x86_registered_interrupts[x86_current_interrupt];
    (intr_handler ? intr_handler : intr_handler_default)(x86_current_interrupt, &x86_interrupted_ctx, x86_current_interrupt_error_code);

    --in_intr_handler;

    unsigned long *sp = (void *) x86_interrupted_ctx.sp; /* ip, cs, flags */
    if (!sched_context_switch_request || !(sp[2] & X86_IF)) {
        x86_in_isr = old_in_isr;
        return;
    }

    ucontext_t *ctx = (ucontext_t *) sched_active_thread->sp;
    ctx->uc_context.registers = x86_interrupted_ctx;
    ctx->uc_stack.ss_sp = x86_interrupt_handler_stack;
    ctx->uc_stack.ss_size = sizeof x86_interrupt_handler_stack;
    asm volatile ("pushf; pop %0" : "=g"(ctx->uc_context.flags));
    ctx->uc_context.ip = (void *) (uintptr_t) &continue_after_intr;
    ctx->__intr.ip = sp[0];
    ctx->__intr.flags = sp[2];

    thread_yield_higher();
    __builtin_unreachable();
}

void ASM_FUN_ATTRIBUTES NORETURN x86_int_entry(void)
{
    asm volatile ("mov %eax, (4*0 + x86_interrupted_ctx)");
    asm volatile ("mov %ecx, (4*1 + x86_interrupted_ctx)");
    asm volatile ("mov %edx, (4*2 + x86_interrupted_ctx)");
    asm volatile ("mov %ebx, (4*3 + x86_interrupted_ctx)");
    asm volatile ("mov %ebp, (4*5 + x86_interrupted_ctx)");
    asm volatile ("mov %esi, (4*6 + x86_interrupted_ctx)");
    asm volatile ("mov %edi, (4*7 + x86_interrupted_ctx)");

    asm volatile ("jnc 1f");
    asm volatile ("  mov (%esp), %eax");
    asm volatile ("  add $4, %esp");
    asm volatile ("  jmp 2f");
    asm volatile ("1:");
    asm volatile ("  xor %eax, %eax");
    asm volatile ("2:");
    asm volatile ("  mov %eax, x86_current_interrupt_error_code");

    asm volatile ("mov %esp, (4*4 + x86_interrupted_ctx)");
    asm volatile ("mov %0, %%esp" :: "g"(&x86_interrupt_handler_stack[sizeof x86_interrupt_handler_stack]));
    asm volatile ("call x86_int_handler");
    asm volatile ("jmp x86_int_exit");
    __builtin_unreachable();
}

void ASM_FUN_ATTRIBUTES NORETURN x86_int_exit(void)
{
    asm volatile ("mov (4*0 + x86_interrupted_ctx), %eax");
    asm volatile ("mov (4*1 + x86_interrupted_ctx), %ecx");
    asm volatile ("mov (4*2 + x86_interrupted_ctx), %edx");
    asm volatile ("mov (4*3 + x86_interrupted_ctx), %ebx");
    asm volatile ("mov (4*5 + x86_interrupted_ctx), %ebp");
    asm volatile ("mov (4*6 + x86_interrupted_ctx), %esi");
    asm volatile ("mov (4*7 + x86_interrupted_ctx), %edi");
    asm volatile ("mov (4*4 + x86_interrupted_ctx), %esp");

    asm volatile ("iret");
    __builtin_unreachable();
}

#define DECLARE_INT(NUM, HAS_ERROR_CODE, MNEMONIC) \
    static void ASM_FUN_ATTRIBUTES NORETURN x86_int_entry_##NUM##h(void) \
    { \
        asm volatile ("movb %0, x86_current_interrupt" :: "n"(0x##NUM)); \
        if ((HAS_ERROR_CODE)) { \
            asm volatile ("stc"); \
        } \
        else { \
            asm volatile ("clc"); \
        }\
        asm volatile ("jmp x86_int_entry"); \
        __builtin_unreachable(); \
    }
DECLARE_INT(00, 0, "#DE")
DECLARE_INT(01, 0, "#DB")
DECLARE_INT(02, 0, "NMI")
DECLARE_INT(03, 0, "#BP")
DECLARE_INT(04, 0, "#OF")
DECLARE_INT(05, 0, "#BR")
DECLARE_INT(06, 0, "#UD")
DECLARE_INT(07, 0, "#NM")
DECLARE_INT(08, 1, "#DF")
DECLARE_INT(0d, 1, "#GP")
DECLARE_INT(0e, 1, "#PF")
DECLARE_INT(10, 0, "#MF")
DECLARE_INT(11, 1, "#AC")
DECLARE_INT(12, 0, "#MC")
DECLARE_INT(20, 0, "PIC PIT")
DECLARE_INT(21, 1, "PIC KBC1")
DECLARE_INT(23, 0, "PIC RS232 2/4")
DECLARE_INT(24, 0, "PIC RS232 1/3")
DECLARE_INT(25, 0, "PIC LPT2")
DECLARE_INT(26, 0, "PIC floppy")
DECLARE_INT(27, 0, "PIC LPT1")
DECLARE_INT(28, 0, "PIC RPC")
DECLARE_INT(29, 0, "PIC #9")
DECLARE_INT(2a, 0, "PIC ATA4")
DECLARE_INT(2b, 0, "PIC ATA3")
DECLARE_INT(2c, 0, "PIC KBC2")
DECLARE_INT(2e, 0, "PIC ATA1")
DECLARE_INT(2f, 0, "PIC ATA2")

static struct idt_desc X86_IDT_ENTRIES[X86_MAX_INTERRUPTS];
static struct idtr_t idtr = {
    .limit = sizeof X86_IDT_ENTRIES,
    .base = &X86_IDT_ENTRIES[0],
};

#define INTR_TEST_REG_AX (0xAA00AA00ul)
#define INTR_TEST_REG_DX (0x00DD00DDul)
#define INTR_TEST_REG_CX (0xC0C0C0C0ul)
#define INTR_TEST_REG_BX (0x0B0B0B0Bul)
#define INTR_TEST_REG_SI (0x00666600ul)
#define INTR_TEST_REG_DI (0x33000033ul)

static void intr_handler_test_int_bp(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code)
{
    (void) error_code;
    if (intr_num != X86_INT_BP ||
        orig_ctx->ax != INTR_TEST_REG_AX ||
        orig_ctx->dx != INTR_TEST_REG_DX ||
        orig_ctx->cx != INTR_TEST_REG_CX ||
        orig_ctx->bx != INTR_TEST_REG_BX ||
        orig_ctx->si != INTR_TEST_REG_SI ||
        orig_ctx->di != INTR_TEST_REG_DI
    ) {
        puts("Interrupt handler test failed (int 3, capture registers).");
        x86_hlt();
    }
    orig_ctx->ax ^= -1;
    orig_ctx->dx ^= -2;
    orig_ctx->cx ^= -3;
    orig_ctx->bx ^= -4;
    orig_ctx->si ^= -5;
    orig_ctx->di ^= -6;
}

static void test_int_bp(void)
{
    x86_registered_interrupts[X86_INT_BP] = intr_handler_test_int_bp;

    unsigned long ax = INTR_TEST_REG_AX;
    unsigned long dx = INTR_TEST_REG_DX;
    unsigned long cx = INTR_TEST_REG_CX;
    unsigned long bx = INTR_TEST_REG_BX;
    unsigned long si;
    unsigned long di;
    unsigned long eflags_before, eflags_after;
    asm volatile (
        "mov %8, %%esi\n"
        "mov %9, %%edi\n"
        "pushf; pop %6\n"
        "int3\n"
        "pushf; pop %7\n"
        "mov %%esi, %4\n"
        "mov %%edi, %5\n"
        : "+a"(ax), "+d"(dx), "+c"(cx), "+b"(bx), "=g"(si), "=g"(di), "=g"(eflags_before), "=g"(eflags_after)
        : "n"(INTR_TEST_REG_SI), "n"(INTR_TEST_REG_DI)
        : "esi", "edi"
    );
    if (ax != (INTR_TEST_REG_AX ^ -1) ||
        dx != (INTR_TEST_REG_DX ^ -2) ||
        cx != (INTR_TEST_REG_CX ^ -3) ||
        bx != (INTR_TEST_REG_BX ^ -4) ||
        si != (INTR_TEST_REG_SI ^ -5) ||
        di != (INTR_TEST_REG_DI ^ -6) ||
        /* ignore EFLAGS.TF, hurts debugging */
        ((eflags_before != eflags_after) && ((eflags_before ^ eflags_after) != X86_TF))
    ) {
        puts("Interrupt handler test failed (int 3, return code).");
        x86_hlt();
    }

    x86_registered_interrupts[X86_INT_BP] = NULL;
}

static inline void __attribute__((always_inline)) set_idt_desc(void (*fun_)(void), unsigned num, unsigned pl)
{
    uintptr_t fun = (uintptr_t) fun_;
    X86_IDT_ENTRIES[num] = (struct idt_desc) {
        .offset_1 = fun & 0xffff,
        .selector = 8,
        .zero = 0,
        .type_attr = 14 | (0 << 4) | (pl << 5) | (1 << 7),
        .offset_2 = fun >> 16,
    };
}

#define SET_IDT_DESC(NUM, HAS_ERROR_CODE, MNEMONIC, PL) \
        (set_idt_desc(x86_int_entry_##NUM##h, 0x##NUM, PL));

static void load_interrupt_descriptor_table(void)
{
    SET_IDT_DESC(00, 0, "#DE", 0)
    SET_IDT_DESC(01, 0, "#DB", 0)
    SET_IDT_DESC(02, 0, "NMI", 0)
    SET_IDT_DESC(03, 0, "#BP", 3)
    SET_IDT_DESC(04, 0, "#OF", 0)
    SET_IDT_DESC(05, 0, "#BR", 0)
    SET_IDT_DESC(06, 0, "#UD", 0)
    SET_IDT_DESC(07, 0, "#NM", 0)
    SET_IDT_DESC(08, 1, "#DF", 0)
    SET_IDT_DESC(0d, 1, "#GP", 0)
    SET_IDT_DESC(0e, 1, "#PF", 0)
    SET_IDT_DESC(10, 0, "#MF", 0)
    SET_IDT_DESC(11, 1, "#AC", 0)
    SET_IDT_DESC(12, 0, "#MC", 0)
    SET_IDT_DESC(20, 0, "PIC PIT", 0)
    SET_IDT_DESC(21, 1, "PIC KBC1", 0)
    SET_IDT_DESC(23, 0, "PIC RS232 2/4", 0)
    SET_IDT_DESC(24, 0, "PIC RS232 1/3", 0)
    SET_IDT_DESC(25, 0, "PIC LPT2", 0)
    SET_IDT_DESC(26, 0, "PIC floppy", 0)
    SET_IDT_DESC(27, 0, "PIC LPT1", 0)
    SET_IDT_DESC(28, 0, "PIC RPC", 0)
    SET_IDT_DESC(29, 0, "PIC #9", 0)
    SET_IDT_DESC(2a, 0, "PIC ATA4", 0)
    SET_IDT_DESC(2b, 0, "PIC ATA3", 0)
    SET_IDT_DESC(2c, 0, "PIC KBC2", 0)
    SET_IDT_DESC(2e, 0, "PIC ATA1", 0)
    SET_IDT_DESC(2f, 0, "PIC ATA2", 0)

    asm volatile ("lidt %0" :: "m"(idtr));
}

void x86_init_interrupts(void)
{
    load_interrupt_descriptor_table();
    test_int_bp();
    puts("Interrupt handling initialized");
}

void x86_interrupt_handler_set(unsigned num, x86_intr_handler_t handler)
{
    x86_registered_interrupts[num] = handler;
}
