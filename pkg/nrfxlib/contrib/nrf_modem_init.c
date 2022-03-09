/*
 * Copyright (C) 2021-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Dylan Laduranty <dylan.laduranty@mesotic.com
 */

#include "kernel_defines.h"
#include <stdio.h>

#include "board.h"
#include "tlsf.h"
#include "ztimer.h"

#include "nrf_modem_os.h"
#include "nrf_modem_riot.h"
#include "nrf_modem_params.h"
#include "nrf_modem.h"
#include "nrf_modem_at.h"

#define ENABLE_DEBUG 0
#include "debug.h"

tlsf_t tlsf;

/* Declare static shared memory buffer for communication between
   nRF9160 MCU and its modem, enforce alignment */
static uint8_t tx[NRF_MODEM_SHMEM_SIZE]    __attribute__((aligned(32)));
static uint8_t rx[NRF_MODEM_SHMEM_SIZE]    __attribute__((aligned(32)));
static uint8_t ctrl[NRF_MODEM_SHMEM_SIZE]  __attribute__((aligned(32)));
#if (NRF_MODEM_TRACE_ENABLE == 1)
static uint8_t trace[NRF_MODEM_SHMEM_SIZE] __attribute__((aligned(32)));
#endif

static const nrf_modem_init_params_t nrf_modem_default_init_params = {
    .ipc_irq_prio = 6,
    .shmem.ctrl = {
        .base = (uint32_t)ctrl,
        .size = NRF_MODEM_SHMEM_SIZE,
    },
    .shmem.tx = {
        .base = (uint32_t)tx,
        .size = NRF_MODEM_SHMEM_SIZE,
    },
    .shmem.rx = {
        .base = (uint32_t)rx,
        .size = NRF_MODEM_SHMEM_SIZE,
    },
#if (NRF_MODEM_TRACE_ENABLE == 1)
    .shmem.trace = {
        .base = (uint32_t)trace,
        .size = NRF_MODEM_SHMEM_SIZE,
    },
#endif
};

void nrf_modem_bootstrap(void)
{
    int ret;

    /* Create a pool with TX buffer as nRF modem library need to malloc
       some data from it */
    tlsf = tlsf_create_with_pool(tx, NRF_MODEM_SHMEM_SIZE);
    /* Add some delay before initialize nRF modem lib */
    ztimer_sleep(ZTIMER_MSEC, 80);
    /* Initialize Modem librairy */
    ret = nrf_modem_init(&nrf_modem_default_init_params, NORMAL_MODE);

    if (ret != 0) {
        DEBUG_PUTS("nRF modem library init failed");
        return;
    }

    DEBUG_PUTS("nRF modem libray initialized");

    /* Setup modem with board default parameters */
    if (nrf_modem_setup() != 0) {
        DEBUG_PUTS("nrf_modem_setup failed, check your board parameters");
        /* Setup failed, shutdown modem */
        nrf_modem_shutdown();
        return;
    }

}

/** @} */