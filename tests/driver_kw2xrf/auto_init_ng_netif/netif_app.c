/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
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
 * @brief       Test application for KW2xRF network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 *
 * @}
 */

#include <stdio.h>

#include "kernel.h"
#include "kw2xrf.h"
#include "net/ng_nomac.h"
#include "net/ng_netbase.h"

/* make sure the SPI port and the needed GPIO pins are defined */
#ifndef KWRF_SPI
#error "SPI not defined"
#endif
#ifndef KWRF_CS
#error "Chip select pin not defined"
#endif
#ifndef KWRF_INT
#error "Interrupt pin not defined"
#endif
#ifndef KWRF_SPI_SPEED
#define KWRF_SPI_SPEED          (SPI_SPEED_10MHZ)
#endif

/**
 * @brief   MAC layer stack configuration
 * @{
 */
#define STACKSIZE               (THREAD_STACKSIZE_MAIN)
#define PRIO                    (0)
/** @} */

/**
 * @brief   Allocate the KW2XRF device descriptor
 */
static kw2xrf_t dev;

/**
 * @brief   Stack for the nomac thread
 */
static char nomac_stack[STACKSIZE];


void auto_init_ng_netif(void)
{
    kernel_pid_t iface;
    int res;

    /* initialize the KW2XRF device */
    printf("Initializing the KW2XRF radio at SPI_%i... \n", KWRF_SPI);
    res = kw2xrf_init(&dev, KWRF_SPI, KWRF_SPI_SPEED,
                            KWRF_CS, KWRF_INT);
    if (res < 0) {
        puts("Error initializing KW2XRF radio device");
        return;
    }

    /* start MAC layer */
    puts("Starting the NOMAC layer on top of the driver");
    iface = ng_nomac_init(nomac_stack, sizeof(nomac_stack), PRIO, "kw2xrf",
                          (ng_netdev_t *)(&dev));
    if (iface <= KERNEL_PID_UNDEF) {
        puts("Error initializing MAC layer");
        return;
    }

}
