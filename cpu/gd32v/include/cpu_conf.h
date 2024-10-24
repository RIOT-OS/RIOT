/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#include "vendor/gd32vf103_core.h"
#include "cpu_conf_common.h"

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define CLIC_NUM_INTERRUPTS  (ECLIC_NUM_INTERRUPTS)
#define CLIC_BASE_ADDR       (ECLIC_CTRL_ADDR)
#define CPU_DEFAULT_IRQ_PRIO (0xFF)

/**
 * @brief   The gd32v doesn't use the MIE CSR, but uses the CLIC instead
 */
#define HAVE_CSR_MIE                    (0)

/**
 * @name Flashpage settings
 * @{
 */
#define FLASHPAGE_SIZE                  (1024U)
#define FLASHPAGE_NUMOF                 (128U)
#define FLASHPAGE_WRITE_BLOCK_SIZE      (2U)
#define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)
#define CPU_FLASH_BASE                  0x08000000
/** @} */

/**
 * @brief   Possible values of the `SWJ_CFG` field in the AFIO->PCF0 register
 */
typedef enum {
     /**
      * @brief  Full JTAG interface (reset value)
      */
    SWJ_CFG_FULL_JTAG    = 0,
    /**
     * @brief   JTAG enabled, but NJTRST disabled and pin PB4 usable as GPIO
     */
    SWJ_CFG_NO_NJTRST   = 1U << AFIO_PCF0_SWJ_CFG_Pos,
    /**
     * @brief   JTAG disabled, all debug pins usable as GPIOs
     */
    SWJ_CFG_NO_JTAG     = 4U << AFIO_PCF0_SWJ_CFG_Pos,
} afio_pcf0_swj_cfg_t;

#ifndef CONFIG_AFIO_PCF0_SWJ_CFG
/**
 * @brief   By default, enable JTAG but disable NJTRST
 *
 * This default makes PB4 usable as GPIO while still being able to debug and
 * flash via JTAG.
 */
#define CONFIG_AFIO_PCF0_SWJ_CFG    SWJ_CFG_NO_NJTRST
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CPU_CONF_H */
