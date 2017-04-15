/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include "cc2538.h"
#include "cc2538_gpio.h"
#include "cc2538_uart.h"
#include "cc2538_gptimer.h"
#include "cc2538_ioc.h"
#include "cc2538_soc_adc.h"
#include "cc2538_ssi.h"
#include "cc2538_rfcore.h"
#include "cc2538_sys_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   PERIPH_COUNT_IRQn
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CPU_CONF_H */
/** @} */
