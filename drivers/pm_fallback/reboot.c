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

#define ENABLE_DEBUG (0)
#include "debug.h"

void pm_reboot(void) { }
