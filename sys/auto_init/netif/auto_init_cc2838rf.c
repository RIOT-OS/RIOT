/*
 * Copyright (C) 2015
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for nx_cc2538rf network interfaces
 *
 * @author  Anon Mall <anon.mall@dai-labor.de>
 */

#ifdef CPU_CC2538

#include "board.h"
#include "net/gnrc/nomac.h"
#include "net/gnrc.h"

#include "cc2538rf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */

#define CC2538RF_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define CC2538RF_MAC_PRIO          (THREAD_PRIORITY_MAIN - 4)
//#define CC2538RF_NUM (sizeof(cc2538rf_params)/sizeof(cc2538rf_params[0]))

static char _nomac_stacks[CC2538RF_MAC_STACKSIZE];
static cc2538rf_t cc2538rf_dev;

void auto_init_cc2538rf(void)
{
    DEBUG("auto init cc2538rf: doing auto init\n");


    cc2538rf_init(&cc2538rf_dev);

     gnrc_nomac_init(_nomac_stacks,
                    CC2538RF_MAC_STACKSIZE, CC2538RF_MAC_PRIO,
                    "cc2538rf", (gnrc_netdev_t *)&cc2538rf_dev);
}
#else
typedef int dont_be_pedantic;
#endif /* CPU_2358 */

/** @} */
