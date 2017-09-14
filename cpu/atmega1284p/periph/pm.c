/*
 * Copyright (C) 2017 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega1284p
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
#include <avr/wdt.h>
#include "irq.h"

void pm_reboot(void) {
  irq_disable();
  wdt_enable(WDTO_250MS);
  while(1);
}
