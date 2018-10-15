/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_rdcli_simple
 * @{
 *
 * @file
 * @brief       Standalone extension for the simple RD registration client
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"
#include "thread.h"
#include "xtimer.h"
#include "net/rdcli_config.h"
#include "net/rdcli_simple.h"

#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)
#define TNAME               "rdcli_simple"

static char _stack[STACKSIZE];

static void *reg_runner(void *arg)
{
    (void)arg;

    /* wait some seconds to give the address configuration some time to settle */
    xtimer_sleep(RDCLI_STARTUP_DELAY);

    while (1) {
        if (rdcli_simple_register() != RDCLI_SIMPLE_OK) {
            /* if this fails once, it will always fail, so we might as well
             * quit now */
            LOG_ERROR("[rdcli_simple] error: unable to send registration\n");
            break;
        }
        xtimer_sleep(RDCLI_UPDATE_INTERVAL);
    }

    return NULL;
}

void rdcli_simple_run(void)
{
    thread_create(_stack, sizeof(_stack), PRIO, 0, reg_runner, NULL, TNAME);
}
