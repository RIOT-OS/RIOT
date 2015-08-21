/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 *
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       CC3200 MCU definitions
 *
 * @author      Attilio Dona' <@attiliodona>
 * @}
 *
 */

#ifndef CPU_INCLUDE_CC3200_H_
#define CPU_INCLUDE_CC3200_H_

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"

#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin.h"
#include "driverlib/gpio.h"
#include "driverlib/prcm.h"

#include "board.h"

#define __ASM            __asm
#define __STATIC_INLINE  static inline

#define SW_TIMERS_EXAUSTED 111

#endif /* CPU_INCLUDE_CC3200_H_ */
