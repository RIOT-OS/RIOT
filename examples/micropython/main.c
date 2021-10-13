/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       micropython example application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "thread.h"

#include "micropython.h"
#include "py/stackctrl.h"
#include "lib/utils/pyexec.h"

#include "blob/boot.py.h"

static char mp_heap[MP_RIOT_HEAPSIZE];

int main(void)
{
    int coldboot = 1;

    /* let MicroPython know the top of this thread's stack */
    uint32_t stack_dummy;
    mp_stack_set_top((char*)&stack_dummy);

    /* Make MicroPython's stack limit somewhat smaller than actual stack limit */
    mp_stack_set_limit(THREAD_STACKSIZE_MAIN - MP_STACK_SAFEAREA);

    while (1) {
        /* configure MicroPython's heap */
        mp_riot_init(mp_heap, sizeof(mp_heap));

        /* execute boot.py
         *
         * MicroPython's test suite gets confused by extra output, so only do
         * this the first time after the node boots up, not on following soft
         * reboots.
         */
        if (coldboot) {
            puts("-- Executing boot.py");
            mp_do_str((const char *)boot_py, boot_py_len);
            puts("-- boot.py exited. Starting REPL..");
            coldboot = 0;
        }

        /* loop over REPL input */
        while (1) {
            if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
                if (pyexec_raw_repl() != 0) {
                    break;
                }
            } else {
                if (pyexec_friendly_repl() != 0) {
                    break;
                }
            }
        }
        puts("soft reboot");
    }

    return 0;
}
