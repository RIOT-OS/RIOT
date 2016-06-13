/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for SIM900 module
 *
 * @author  José Ignacio Alamos   <jialamos@uc.cl>
 */

#ifdef MODULE_SIM900

#include "sim900.h"
#include "periph/uart.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/**
 * @brief   Define stack parameters for the PPP layer thread
 * @{
 */
#define SIM900_STACKSIZE           (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#define SIM900_PRIO                (THREAD_PRIORITY_MAIN - 4)

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _pppdev_stack[SIM900_STACKSIZE];
static gnrc_pppdev_t _gnrc_sim900;

static uint8_t _inbuf[2048];
static sim900_t dev;

void auto_init_sim900(void)
{
    DEBUG("auto_init_sim900(): initializing device...\n");

    sim900_params_t params;
    params.uart = 1;
    params.buf = _inbuf;
    params.buf_len = sizeof(_inbuf);
    sim900_setup(&dev, &params);

    gnrc_ppp_setup(&_gnrc_sim900, (pppdev_t *) &dev);
    kernel_pid_t pid = gnrc_pppdev_init(_pppdev_stack, sizeof(_pppdev_stack), SIM900_PRIO, "gnrc_sim900", &_gnrc_sim900);
    (void) pid;
}

#else
typedef int dont_be_pedantic;
#endif
