/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */


#include "kernel_defines.h"
#include "cpu_conf_common.h"
#include "cc2538.h"
#include "cc2538_gpio.h"
#include "cc2538_uart.h"
#include "cc2538_gptimer.h"
#include "cc2538_soc_adc.h"
#include "cc2538_ssi.h"
#include "cc2538_sys_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   PERIPH_COUNT_IRQn
#define CPU_FLASH_BASE                  FLASH_BASE
#define CPU_HAS_BITBAND                 (1)
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE          (2048U)
/* Last page holds the Customer Configuration Area (CCA), this holds
   the Bootloader Backdoor Configuration, Application Entry Point,
   flashpage lock bits. For safety disable writing to that page by
   default */
#ifndef FLASHPAGE_CC2538_USE_CCA_PAGE
#define FLASHPAGE_CC2538_USE_CCA_PAGE   (0)
#endif
#if (IS_ACTIVE(MODULE_RIOTBOOT) || FLASHPAGE_CC2538_USE_CCA_PAGE)
#define FLASHPAGE_NUMOF         ((CC2538_FLASHSIZE / FLASHPAGE_SIZE))
#else
#define FLASHPAGE_NUMOF         ((CC2538_FLASHSIZE / FLASHPAGE_SIZE) -1)
#endif
#define FLASH_ERASE_STATE       (0x1)

/* The minimum block size which can be written is 4B. However, the erase
 * block is always FLASHPAGE_SIZE.
 */
#define FLASHPAGE_WRITE_BLOCK_SIZE      (4U)
/* Writing should be always 4 bytes aligned */
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
