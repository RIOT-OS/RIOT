/*
 * Copyright 2016, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <mips/cpu.h>
#include <mips/hal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "cpu.h"
#include "irq.h"
#include "cpu_conf.h"
#include "malloc.h"
#include "stdio_uart.h"

#define STACK_END_PAINT    (0xdeadc0de)
#define C0_STATUS_EXL      (2)
#define PADDING            (16)
#define MICROMIPS_ISA_MODE (1)
#define M32_SYSCALL        (0xC)
#define M32_SYSCALL_MASK   (0xfc00003f)

/*
 * note the major 16bits of a 32bit MicroMIPS opcode appear first in the
 * instruction stream
 */
#define MM_SYSCALL         (0x8B7C0000)
#define MM_SYSCALL_MASK    (0xfffffc00)

#ifdef MIPS_HARD_FLOAT
/* pointer to the current and old fpu context for lazy context switching */
static struct fp64ctx *currentfpctx;   /* fpu context of current running task */
static struct fp64ctx *oldfpctx;       /* fpu context of last task that executed fpu */
#endif

/*
 *    Stack Layout, note struct gpctx is defined in
 *    $MIPS_ELF_ROOT/mips-mti-elf/include/mips/hal.h
 *
 *    Top Of Stack
 *     ---------------
 *    |               |
 *    |  User stack   |
 *    |               |
 *     ---------------  <--- gpctx->sp
 *    |               |
 *    |    gpctx      |
 *    |               |
 *     ---------------
 *    |  16 byte pad  |
 *     ---------------   <--- thread_get_active()->sp
 */

char *thread_stack_init(thread_task_func_t task_func, void *arg,
                        void *stack_start, int stack_size)
{
    /* make sure it is aligned to 8 bytes this is a requirement of the O32 ABI */
    uintptr_t *p = (uintptr_t *)(((long)(stack_start) + stack_size) & ~7);
    uintptr_t *fp;

    /* paint */
    p--;
    *p-- = STACK_END_PAINT;

    /* prepare stack for __exception_restore() */
    fp = p;
    p -= sizeof(struct gpctx) / sizeof(unsigned int);

    struct gpctx *initial_ctx = (struct gpctx *)p;
    initial_ctx->a[0] = (reg_t)arg;
    initial_ctx->status = mips32_get_c0(C0_STATUS) | SR_IE; /* Enable interrupts */
    __asm volatile ("sw    $gp, 0(%0)" : : "r" (&initial_ctx->gp));
    initial_ctx->epc = (reg_t)task_func;
    initial_ctx->ra = (reg_t)sched_task_exit;
    initial_ctx->sp = (reg_t)fp;
    initial_ctx->link = (struct linkctx *)NULL;

#ifdef MIPS_MICROMIPS
    initial_ctx->epc |= MICROMIPS_ISA_MODE;
    initial_ctx->ra |= MICROMIPS_ISA_MODE;
#endif

#ifdef MIPS_HARD_FLOAT
    /*
     * Disable FPU so we get an exception on first use to allow
     * Lazy FPU context save and restore
     */
    initial_ctx->status &= ~SR_CU1;
    initial_ctx->status |= SR_FR; /*use double width FPU */
#endif
    /*
     * note the -4 (-16 bytes) as the toolchain exception handling code
     * adjusts the sp for alignment
     */
    p -= PADDING/sizeof(unsigned int);

    return (void *)p;
}

void thread_stack_print(void)
{
    uintptr_t *sp = (void *)thread_get_active()->sp;

    printf("Stack trace:\n");
    while (*sp != STACK_END_PAINT) {
        printf(" 0x%p: 0x%08lx\n", sp, *sp);
        sp++;
    }
}

extern void __exception_restore(void);
void cpu_switch_context_exit(void)
{
    unsigned int status = mips32_get_c0(C0_STATUS);

    /*
     * Set Exception level if we are not already running at it
     * the EXL mode depends on the bootloader.
     */

    if ((status & C0_STATUS_EXL) == 0) {
        mips32_set_c0(C0_STATUS, status | C0_STATUS_EXL);
    }

    sched_run();

    __asm volatile ("lw    $sp, 0(%0)" : : "r" (&thread_get_active()->sp));

    __exception_restore();

    UNREACHABLE();
}

struct linkctx* exctx_find(reg_t id, struct gpctx *gp)
{
    struct linkctx **ctx = (struct linkctx **)&gp->link;
    while (*ctx) {
        if ((*ctx)->id == id) {
            return *ctx;
        }
        ctx = &(*ctx)->next;
    }
    return NULL;
}

/* unaligned access helper */
static inline uint32_t
#ifndef __clang__
/* Clang does not support attribute optimize */
__attribute__((optimize("-O3")))
#endif
mem_rw(const void *vaddr)
{
    uint32_t v;
    memcpy(&v, vaddr, sizeof(v));
    return v;
}

#ifdef MIPS_DSP
extern int _dsp_save(struct dspctx *ctx);
extern int _dsp_load(struct dspctx *ctx);
#endif

/*
 * The official mips toolchain version 2016.05-03 needs this attribute.
 * Newer versions (>=2017.10-05) don't. Those started being based on gcc 6,
 * thus use that to guard the attribute.
 *
 * See https://github.com/RIOT-OS/RIOT/pull/11986.
 */
#if __GNUC__ <= 4
void __attribute__((nomips16))
#else
void
#endif

/* note return type from above #ifdef */
_mips_handle_exception(struct gpctx *ctx, int exception)
{
    unsigned int syscall_num = 0;
#ifdef MIPS_DSP
    struct dspctx dsp_ctx; /* intentionally allocated on current stack */
#endif

    switch (exception) {

        case EXC_SYS:
#ifdef MIPS_MICROMIPS
            /* note major 16bits of opcode is first in instruction stream */
            syscall_num =
                mem_rw((const void *)(ctx->epc & ~MICROMIPS_ISA_MODE))
                & 0x3FF;
#else
            syscall_num = (mem_rw((const void *)ctx->epc) >> 6) & 0xFFFF;
#endif

#ifdef MODULE_STDIO_UART
#include <mips/uhi_syscalls.h>
            /*
             * intercept UHI write syscalls (printf) which would normally
             * get routed to debug probe or bootloader handler and output
             * via a UART
             */

            if (syscall_num == __MIPS_UHI_SYSCALL_NUM) {
                if (ctx->t2[1] == __MIPS_UHI_WRITE &&
                    (ctx->a[0] == STDOUT_FILENO || ctx->a[0] == STDERR_FILENO)) {
                    uint32_t status = irq_disable();
                    stdio_write((void *)ctx->a[1], ctx->a[2]);
                    ctx->v[0] = ctx->a[2];
                    ctx->epc += 4; /* move PC past the syscall */
                    irq_restore(status);
                    return;
                }
                else if (ctx->t2[1] == __MIPS_UHI_READ && ctx->a[0] == STDIN_FILENO) {
                    ctx->v[0] = stdio_read((void *)ctx->a[1], ctx->a[2]);
                    ctx->epc += 4; /* move PC past the syscall */
                    return;
                }
                else if (ctx->t2[1] == __MIPS_UHI_FSTAT &&
                         (ctx->a[0] == STDOUT_FILENO || ctx->a[0] == STDIN_FILENO || ctx->a[0] == STDERR_FILENO)) {
                    /*
                     * Printf fstat's the stdout/stdin/stderr file so
                     * fill out a minimal struct stat.
                     */
                    struct stat *sbuf = (struct stat *)ctx->a[1];
                    memset(sbuf, 0, sizeof(struct stat));
                    sbuf->st_mode = S_IRUSR | S_IWUSR | S_IWGRP;
                    sbuf->st_blksize = BUFSIZ;
                    /* return 0 */
                    ctx->v[0] = 0;
                    ctx->epc += 4; /* move PC past the syscall */
                    return;
                }
            }
            else
#endif
            if (syscall_num == 2) {
                unsigned int return_instruction = 0;
                struct gpctx *new_ctx;
#ifdef MIPS_DSP
                struct dspctx *new_dspctx;
#endif
                /*
                 * Syscall 1 is reserved for UHI.
                 */

                /*
                 * save the stack pointer in the thread info
                 * note we want the saved value to include the
                 * saved off context and the 16 bytes padding.
                 * Note we cannot use the current sp value as
                 * the prologue of this function has adjusted it
                 */
                thread_t *t = thread_get_active();
                t->sp = (char *)(ctx->sp - sizeof(struct gpctx) - PADDING);

#ifdef MIPS_DSP
                _dsp_save(&dsp_ctx);
                _linkctx_append(ctx,&(dsp_ctx.link));
#endif

#ifdef MIPS_HARD_FLOAT
                if(currentfpctx) {
                    _linkctx_append(ctx,&(currentfpctx->fp.link));
                }
#endif

                sched_run();

                t = thread_get_active();
                new_ctx = (struct gpctx *)((unsigned int)t->sp + PADDING);

#ifdef MIPS_HARD_FLOAT
                currentfpctx = (struct fp64ctx *)exctx_find(LINKCTX_TYPE_FP64, new_ctx);
                if(!currentfpctx) {
                    /* check for half-width FPU ctx in-case hardware doesn't support double. */
                    currentfpctx = (struct fp64ctx *)exctx_find(LINKCTX_TYPE_FP32, new_ctx);
                }
#endif

#ifdef MIPS_DSP
                new_dspctx = (struct dspctx *)exctx_find(LINKCTX_TYPE_DSP, new_ctx);
                if (new_dspctx)
                    _dsp_load(new_dspctx);
#endif

#ifdef MIPS_MICROMIPS
                return_instruction =
                    mem_rw((const void *)(new_ctx->epc & ~MICROMIPS_ISA_MODE));
                if ((return_instruction & MM_SYSCALL_MASK) == MM_SYSCALL) { /* syscall */
                    new_ctx->epc += 4; /* move PC past the syscall */
                }
#else
                return_instruction = mem_rw((const void *)new_ctx->epc);
                if ((return_instruction & M32_SYSCALL_MASK) == M32_SYSCALL) { /* syscall */
                    new_ctx->epc += 4; /* move PC past the syscall */
                }
#endif

                /*
                 * The toolchain Exception restore code just wholesale copies the
                 * status register from the context back to the register losing
                 * any changes that may have occurred, 'status' is really global state
                 * You don't enable interrupts on one thread and not another...
                 * So we just copy the current status value into the saved value
                 * so nothing changes on the restore
                 */

                new_ctx->status = mips32_get_c0(C0_STATUS);

#ifdef MIPS_HARD_FLOAT
                /*
                 * Disable FPU so we get an exception on first use to allow
                 * Lazy FPU context save and restore
                 */
                new_ctx->status &= ~SR_CU1;
#endif

                __asm volatile ("lw    $sp, 0(%0)" : : "r" (&thread_get_active()->sp));

                /*
                 * Jump straight to the exception restore code
                 * if we return this functions prologue messes up
                 * the stack  pointer
                 */
                __exception_restore();

                UNREACHABLE();
            }
        break;
#ifdef MIPS_HARD_FLOAT
        case EXC_CPU:
        {
            int newly_allocd  = false;

            mips_bissr(SR_CU1);
            ctx->status |= SR_CU1;

            if (!currentfpctx) {
                currentfpctx = malloc(sizeof(struct fp64ctx));
                assert(currentfpctx);
                memset(currentfpctx,0,sizeof(struct fp64ctx));
                currentfpctx->fp.link.id = LINKCTX_TYPE_FP64;
                newly_allocd = true;
            }

            /* this means no one exec'd fpu since we last run */
            if (oldfpctx == currentfpctx) {
                return;
            }

            if (oldfpctx) {
                _fpctx_save(&oldfpctx->fp);
            }

            if (!newly_allocd) {
                _fpctx_load(&currentfpctx->fp);
            }

            /*
             * next fpu exception must save our context as it's not necessarily
             * the next context switch will cause fpu exception and it's very
             * hard for any future task to determine which was the last one
             * that performed fpu operations. so by saving this pointer now we
             * give this knowledge to that future task
             */
            oldfpctx = currentfpctx;

        return;
        }
#endif

            /* default: */
    }
    /* Pass all other exceptions through to the toolchain handler */
    __exception_handle(ctx, exception);
}
