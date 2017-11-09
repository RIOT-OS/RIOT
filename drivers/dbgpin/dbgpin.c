/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_dbgpin
 * @{
 *
 * @file
 * @brief       Initialization of configured direct controlled debug pins
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "dbgpin.h"
#include "periph/gpio.h"

void dbgpin_init(void)
{
#ifdef DBGPIN0
    gpio_init(DBGPIN0, GPIO_OUT);
#endif
#ifdef DBGPIN1
    gpio_init(DBGPIN1, GPIO_OUT);
#endif
}
