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
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     Test the module "multitasking"
 *
 * @author    René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "multitasking.h"

static char other_stack[2][KERNEL_CONF_STACKSIZE_DEFAULT];

static void do_idle(void)
{
    for (volatile unsigned i = 0; i < (1 << 14); ++i) {
        i = i + 1;
        i = i - 1;
    }
}

static void *other_thread(void *arg)
{
    (void) arg;

    while (1) {
        puts((const char *) arg);
        do_idle();
    }

    return NULL;
}


int main(void)
{
#if 0
    /* Without multitasking only "main" will run. */
    multitasking_active(false);
#else
    /* No need to explicitly activate multitasking. It is active by default. */
#endif

    thread_create(other_stack[0], sizeof(other_stack[0]),
                  PRIORITY_MAIN, CREATE_WOUT_YIELD,
                  other_thread, "thread #1", "other_thread #1");

    thread_create(other_stack[1], sizeof(other_stack[1]),
                  PRIORITY_MAIN, CREATE_WOUT_YIELD,
                  other_thread, "thread #2", "other_thread #2");

    while (1) {
        puts("thread #0");
        do_idle();
    }

    return 0;
}
