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
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#include "pm_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* make sure we do not conflict with pm_layered */
#ifndef MODULE_PM_LAYERED
/* specific defines, if general define is set */
#ifdef NEEDS_PM_FALLBACK
#define NEEDS_PM_SET_LOWEST
#define NEEDS_PM_OFF
#endif

#if defined(NEEDS_PM_SET_LOWEST)
void pm_set_lowest(void) {

}
#endif

#if defined(NEEDS_PM_OFF)
void pm_off(void)
{
    irq_disable();
    pm_set_lowest();
    while(1) {};
}
#endif
#endif /* MODULE_PM_LAYERED */
