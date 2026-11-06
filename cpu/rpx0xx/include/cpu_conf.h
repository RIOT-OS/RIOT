/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
*/

#include <stdint.h>

#include "macros/units.h"
#include "cpu_conf_common.h"
#include "RP2040.h"

/* register field definitions (*_LSB, *_BITS, *_VALUE_*) from the Pico SDK */
#include "hardware/platform_defs.h"
#include "hardware/regs/adc.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/pads_qspi.h"
#include "hardware/regs/pio.h"
#include "hardware/regs/pll.h"
#include "hardware/regs/pwm.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/rosc.h"
#include "hardware/regs/spi.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/watchdog.h"
#include "hardware/regs/xosc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_FLASH_BASE                  ROM_START_ADDR
#define CPU_IRQ_NUMOF                   (32U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
