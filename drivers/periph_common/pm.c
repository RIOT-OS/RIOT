/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Platform-independent power management fallback code
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef PROVIDES_PM_OFF
void pm_off(void)
{
    irq_disable();
    while(1) {};
}
#endif

#ifndef PROVIDES_PM_SET_LOWEST
void pm_set_lowest(void)
{
    /* don't do anything here */
}
#endif
