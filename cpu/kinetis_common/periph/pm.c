/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Eistec AB
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @note    The current PM implementation is very much simplified down to only
 *          using the 'WAIT' mode. This implementation must be further expanded
 *          to make use of the available and more efficient (deep) sleep modes
 *          of the Kinetis CPUs.
 */
void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:
            cortexm_sleep(0);
            break;
    }
}
