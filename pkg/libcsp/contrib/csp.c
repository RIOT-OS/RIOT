/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP RIOT adaptation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <errno.h>

#include "csp/csp.h"
#include "csp/csp_id.h"
#include "csp_conn.h"
#include "csp_qfifo.h"

#include "csp_riot.h"
#include "thread.h"

/**
 * @brief   Priority used for CSP routing thread
 */
#ifndef CSP_ROUTE_PRIO
#define CSP_ROUTE_PRIO                   (THREAD_PRIORITY_MAIN - 3)
#endif

/**
 * @brief   Stacksize
 */
#ifndef CSP_ROUTE_STACKSIZE
#define CSP_ROUTE_STACKSIZE              (THREAD_STACKSIZE_DEFAULT)
#endif

static char _stack_csp_route[CSP_ROUTE_STACKSIZE];

static void *_csp_route_thread(void *arg)
{
    (void)arg;
    while (1) {
        csp_route_work();
    }
    return NULL;
}

csp_conf_t csp_conf = {
    .version = 2,
    .address = 0,
    .hostname = RIOT_BOARD,
    .model = RIOT_VERSION,
    .revision = "",
    .conn_dfl_so = CSP_O_NONE,
    .dedup = CSP_DEDUP_OFF
};

void libcsp_set_dedup(enum csp_dedup_types type)
{
    csp_conf.dedup = type;
}

enum csp_dedup_types libcsp_get_dedup(void)
{
    return csp_conf.dedup;
}

uint8_t libcsp_get_csp_version(void)
{
    return csp_conf.version;
}

int libcsp_set_csp_version(uint8_t version)
{
    if (version == 2 || version == 1) {
        csp_conf.version = version;
    }
    return -EINVAL;
}

void libcsp_init(void)
{
    csp_buffer_init();
    csp_conn_init();
    csp_qfifo_init();

    thread_create(_stack_csp_route, sizeof(_stack_csp_route),
                  CSP_ROUTE_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _csp_route_thread, NULL,
                  "csp_route");
}
