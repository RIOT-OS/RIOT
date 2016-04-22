/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc26x0
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Leon M. George <leon@georgemail.eu>
 *
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#include "cc26x0.h"

#include "cc26x0_aux.h"
#include "cc26x0_ccfg.h"
#include "cc26x0_fcfg.h"
#include "cc26x0_gpio.h"
#include "cc26x0_gpt.h"
#include "cc26x0_i2c.h"
#include "cc26x0_ioc.h"
#include "cc26x0_prcm.h"
#include "cc26x0_uart.h"
#include "cc26x0_vims.h"
#include "cc26x0_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   IRQN_COUNT
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
