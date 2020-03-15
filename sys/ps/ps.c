/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_ps
 * @{
 * @file
 * @brief   UNIX like ps command
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "fmt.h"
#include "fmt_table.h"
#include "kernel_types.h"
#include "sched.h"
#include "thread.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "schedstatistics.h"
#endif

#ifdef MODULE_TLSF_MALLOC
#include "tlsf.h"
#include "tlsf-malloc.h"
#endif

/* list of states copied from tcb.h */
static const char *state_names[] = {
    [STATUS_RUNNING] = "running",
    [STATUS_PENDING] = "pending",
    [STATUS_STOPPED] = "stopped",
    [STATUS_SLEEPING] = "sleeping",
    [STATUS_ZOMBIE] = "zombie",
    [STATUS_MUTEX_BLOCKED] = "bl mutex",
    [STATUS_RECEIVE_BLOCKED] = "bl rx",
    [STATUS_SEND_BLOCKED] = "bl send",
    [STATUS_REPLY_BLOCKED] = "bl reply",
    [STATUS_FLAG_BLOCKED_ANY] = "bl anyfl",
    [STATUS_FLAG_BLOCKED_ALL] = "bl allfl",
    [STATUS_MBOX_BLOCKED] = "bl mbox",
    [STATUS_COND_BLOCKED] = "bl cond",
};

static void _col_str(const char *s, size_t width)
{
    print_str(" ");
    print_str_fill(s, width, ' ');
    print_str(" |");
}

static void _col_u32_dec(uint32_t num, size_t width)
{
    print_str(" ");
    print_col_u32_dec(num, width);
    print_str(" |");
}

static void _col_ptr(void *ptr, size_t width)
{
    print_str(" ");
    while (width-- > 10) {
        print_str(" ");
    }
    print_str("0x");
    print_u32_hex((uintptr_t)ptr);
    print_str(" |");
}

#ifdef MODULE_SCHEDSTATISTICS
static _col_runtime(kernel_pid_t pid, uint64_t rt_sum)
{
    /* multiply with 100 for percentage and to avoid floats/doubles */
    uint64_t runtime_ticks = sched_pidlist[pid].runtime_ticks * 100;
    unsigned runtime_major = runtime_ticks / rt_sum;
    unsigned runtime_minor = ((runtime_ticks % rt_sum) * 1000) / rt_sum;
    print_str(" ");
    print_col_u32_dec(runtime_major, 2);
    print_str(".");
    print_u32_dec_zeros(runtime_minor, 3);
    print_str("% |");
}
#endif

/**
 * @brief Prints a list of running threads including stack usage to stdout.
 */
void ps(void)
{
    const char queued_name[] = {'_', 'Q'};
#ifdef DEVELHELP
    int overall_stacksz = 0, overall_used = 0;
#endif

    print_str("\t|");
    _col_str("pid", 3);
#ifdef DEVELHELP
    _col_str("name", 20);
#endif
    _col_str("state", 8);
    _col_str("Q", 1);
    _col_str("pri", 3);
#ifdef DEVELHELP
    _col_str("stack", 10);
    _col_str("stack used", 10);
    _col_str("base addr", 10);
    _col_str("current", 10);
#endif
#ifdef MODULE_SCHEDSTATISTICS
    _col_str("runtime", 8);
    _col_str("switches", 8);
#endif
    print_str("\n");

#if defined(DEVELHELP) && ISR_STACKSIZE
    int isr_usage = thread_isr_stack_usage();
    void *isr_start = thread_isr_stack_start();
    void *isr_sp = thread_isr_stack_pointer();
    print_str("\t|");
    _col_str("-", 3);
    _col_str("isr_stack", 20);
    _col_str("-", 8);
    _col_str("-", 1);
    _col_str("-", 3);
    _col_u32_dec(ISR_STACKSIZE, 10);
    _col_u32_dec(isr_usage, 10);
    _col_ptr(isr_start, 10);
    _col_ptr(isr_sp, 10);

    overall_stacksz += ISR_STACKSIZE;
    if (isr_usage > 0) {
        overall_used += isr_usage;
    }
#ifdef MODULE_SCHEDSTATISTICS
    _col_str("-", 8);
    _col_str("-", 8);
#endif
    print_str("\n");
#endif

#ifdef MODULE_SCHEDSTATISTICS
    uint64_t rt_sum = 0;
    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = (thread_t *)sched_threads[i];
        if (p != NULL) {
            rt_sum += sched_pidlist[i].runtime_ticks;
        }
    }
#endif /* MODULE_SCHEDSTATISTICS */

    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = (thread_t *)sched_threads[i];

        if (p != NULL) {
            thread_status_t state = p->status;                                     /* copy state */
            const char *sname = state_names[state];                                /* get state name */
            const char *queued = &queued_name[(int)(state >= STATUS_ON_RUNQUEUE)]; /* get queued flag */
#ifdef DEVELHELP
            int stacksz = p->stack_size;                                           /* get stack size */
            overall_stacksz += stacksz;
            stacksz -= thread_measure_stack_free(p->stack_start);
            overall_used += stacksz;
#endif
            print_str("\t|");
            _col_u32_dec(p->pid, 3);
#ifdef DEVELHELP
            _col_str(p->name, 20);
#endif
            _col_str(sname, 8);
            _col_str(queued, 1);
            _col_u32_dec(p->priority, 3);
#ifdef DEVELHELP
            _col_u32_dec(p->stack_size, 10);
            _col_u32_dec(stacksz, 10);
            _col_ptr(p->stack_start, 10);
            _col_ptr(p->sp, 10);
#endif
#ifdef MODULE_SCHEDSTATISTICS
            _col_runtime(i, rt_sum);
            _col_u32_dec(sched_pidlist[i].schedules, 8);
#endif
            print_str("\n");
        }
    }

#ifdef DEVELHELP
    print_str("\t|");
    _col_str("-", 3);
    _col_str("SUM", 20);
    _col_str("-", 8);
    _col_str("-", 1);
    _col_str("-", 3);
    _col_u32_dec(overall_stacksz, 10);
    _col_u32_dec(overall_used, 10);
    _col_str("-", 10);
    _col_str("-", 10);
#   ifdef MODULE_SCHEDSTATISTICS
    _col_str("10.000%", 8);
    _col_str("-", 8);
#   endif
#   ifdef MODULE_TLSF_MALLOC
    print_str("\nHeap usage:\n");
    tlsf_size_container_t sizes = { .free = 0, .used = 0 };
    tlsf_walk_pool(tlsf_get_pool(_tlsf_get_global_control()), tlsf_size_walker, &sizes);
    print_str("\tTotal free size: ");
    print_u32_dec(sizes.free);
    print_str("\n\tTotal used size: ");
    print_u32_dec(sizes.used);
    print_str("\n");
#   endif
#endif
}
