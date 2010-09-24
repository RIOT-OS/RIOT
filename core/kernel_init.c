/**
 * platform-independent kernel initialization
 *
 * Copyright (C) 2010 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU General Public
 * License. See the file LICENSE in the top level directory for more details.
 *
 * @ingroup kernel
 * @{
 * @file
 * @author Kaspar Schleiser <kaspar.schleiser@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "tcb.h"
#include "kernel.h"
#include "kernel_intern.h"
#include "scheduler.h"
#include "flags.h"
#include "cpu.h"
#include "lpm.h"
#include "thread.h"
#include "hwtimer.h"

#ifdef MODULE_AUTO_INIT
#include <auto_init.h>
#endif

#define ENABLE_DEBUG
#include "debug.h"

volatile tcb *fk_threads[MAXTHREADS];
volatile tcb *fk_thread;
volatile int lpm_prevent_sleep = 0;

extern void main(void);
extern void fk_switch_context_exit(void);

void fk_idle(void) {
    while(1) {
        if (lpm_prevent_sleep) {
            lpm_set(LPM_IDLE);
        }
        else {
            lpm_set(LPM_IDLE);
//            lpm_set(LPM_SLEEP);          
//            lpm_set(LPM_POWERDOWN);
        }
    }
}

const char *main_name = "main";
const char *idle_name = "idle";

#ifdef MODULE_AUTO_INIT
#define MAIN_FUNC auto_init
#else
#define MAIN_FUNC main
#endif

void kernel_init(void)
{
    dINT();
    printf("kernel_init(): This is µkleos!\n");
    
    scheduler_init();

    if (thread_create(KERNEL_CONF_STACKSIZE_IDLE, PRIORITY_IDLE, CREATE_WOUT_YIELD | CREATE_STACKTEST, fk_idle, idle_name) < 0) {
        printf("kernel_init(): error creating idle task.\n");
    }

    if (thread_create(KERNEL_CONF_STACKSIZE_MAIN, PRIORITY_MAIN, CREATE_WOUT_YIELD | CREATE_STACKTEST, MAIN_FUNC, main_name) < 0) {
        printf("kernel_init(): error creating main task.\n");
    }

    printf("kernel_init(): jumping into first task...\n");
   
    fk_switch_context_exit();
}

