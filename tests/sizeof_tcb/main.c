/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief  Print the size of tcb_t.
 *
 * @author René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stddef.h>

#include "tcb.h"

#define P(NAME) printf("\t%-*s%4zu%4zu\n", 11, #NAME, sizeof(((tcb_t *) 0)->NAME), offsetof(tcb_t, NAME));

int main(void)
{
    puts("\tmember, sizeof, offsetof");

    printf("sizeof(tcb_t): %zu\n", sizeof(tcb_t));

    P(sp);
    P(status);
    P(pid);
    P(priority);
    P(rq_entry);
    P(wait_data);
    P(msg_waiters);
    P(msg_queue);
    P(msg_array);
    P(name);
    P(stack_start);

#ifdef DEVELHELP
    P(stack_size);
#endif

    puts("Done.");
    return 0;
}
