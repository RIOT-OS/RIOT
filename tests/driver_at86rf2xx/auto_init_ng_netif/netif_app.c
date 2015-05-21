/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "ng_at86rf2xx.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

/* make sure the SPI port and the needed GPIO pins are defined */
#ifndef ATRF_SPI
#error "SPI not defined"
#endif
#ifndef ATRF_CS
#error "Chip select pin not defined"
#endif
#ifndef ATRF_INT
#error "Interrupt pin not defined"
#endif
#ifndef ATRF_SLEEP
#error "Sleep pin not defined"
#endif
#ifndef ATRF_RESET
#error "Reset pin not defined"
#endif
#ifndef ATRF_SPI_SPEED
#define ATRF_SPI_SPEED          (SPI_SPEED_5MHZ)
#endif

/**
 * @brief   MAC layer stack configuration
 * @{
 */
#define STACKSIZE               (THREAD_STACKSIZE_MAIN)
#define PRIO                    (0)
/** @} */

/**
 * @brief   Allocate the AT86RF2xx device descriptor
 */
static ng_at86rf2xx_t dev;

/**
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[STACKSIZE];


void auto_init_ng_netif(void)
{
    kernel_pid_t iface;
    int res;

    /* initialize the AT86RF2xx device */
    printf("Initializing the AT86RF2xx radio at SPI_%i... \n", ATRF_SPI);
    res = ng_at86rf2xx_init(&dev, ATRF_SPI, ATRF_SPI_SPEED,
                            ATRF_CS, ATRF_INT,
                            ATRF_SLEEP, ATRF_RESET);
    if (res < 0) {
        puts("Error initializing AT86RF2xx radio device");
        return;
    }

    /* start MAC layer */
    puts("Starting the NOMAC layer on top of the driver");
    iface = ng_nomac_init(nomac_stack, sizeof(nomac_stack), PRIO, "at86rf2xx",
                          (ng_netdev_t *)(&dev));
    if (iface <= KERNEL_PID_UNDEF) {
        puts("Error initializing MAC layer");
        return;
    }

}
