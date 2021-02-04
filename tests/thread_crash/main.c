/*
 * Copyright (C) 2021 Inria
 *               2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for testing the thread_crash module
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "ps.h"

#ifndef FORBIDDEN_ADDRESS
/* Many platforms do not allow writes to address 0x00000000 */
#define FORBIDDEN_ADDRESS (0x00000000)
#endif /* !defined(FORBIDDEN_ADDRESS) */
#ifndef INVALID_INSTRUCTION
/* Random garbage may crash the program as well. */
#define INVALID_INSTRUCTION __asm__ volatile (".short 0xdead, 0xbeef, 0xcafe, 0xbabe\n")
#endif /* !defined(INVALID_INSTRUCTION) */

static char _stack[THREAD_STACKSIZE_MAIN];

void *_crashing_thread(void *arg)
{
    (void)arg;
    puts("Crashing this thread now");
    *((volatile int *) FORBIDDEN_ADDRESS) = 12345;
    volatile int readback = *((volatile int *) FORBIDDEN_ADDRESS);
    (void)readback;
    INVALID_INSTRUCTION;
    /* Nothing worked, let's just try to fail an assertion */
    assert(false);
}

int main(void)
{
    puts("THREAD_CONFIG_KILL_ON_CRASH test application");
    puts("Spawning second thread to crash");
    thread_create(_stack,
                  sizeof(_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST | THREAD_CONFIG_KILL_ON_CRASH,
                  _crashing_thread,
                  NULL,
                  "crashing");
    thread_yield();

    ps();

    return 0;
}
