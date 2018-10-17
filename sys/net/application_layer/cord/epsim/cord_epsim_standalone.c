/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_epsim
 * @{
 *
 * @file
 * @brief       Standalone extension for the simple RD registration endpoint
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"
#include "thread.h"
#include "xtimer.h"
#include "net/cord/epsim.h"
#include "net/cord/config.h"

#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)
#define TNAME               "cord_epsim"

static char _stack[STACKSIZE];

static void *reg_runner(void *arg)
{
    (void)arg;

    /* wait some seconds to give the address configuration some time to settle */
    xtimer_sleep(CORD_STARTUP_DELAY);

    while (1) {
        if (cord_epsim_register() != CORD_EPSIM_OK) {
            /* if this fails once, it will always fail, so we might as well
             * quit now */
            LOG_ERROR("[cord_epsim] error: unable to send registration\n");
            break;
        }
        xtimer_sleep(CORD_UPDATE_INTERVAL);
    }

    return NULL;
}

#ifdef MODULE_CORD_EPSIM_STANDALONE
void cord_epsim_run(void)
{
    thread_create(_stack, sizeof(_stack), PRIO, 0, reg_runner, NULL, TNAME);
}
#endif
