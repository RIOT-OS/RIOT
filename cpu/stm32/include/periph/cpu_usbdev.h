/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32_usbdev
 * @{
 *
 * @file
 * @brief           USB CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#include <stdint.h>

#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USBDEV buffers must be word aligned because of DMA restrictions
 */
#define USBDEV_CPU_DMA_ALIGNMENT       (4)

/**
 * @brief USBDEV buffer instantiation requirement
 */
#define USBDEV_CPU_DMA_REQUIREMENTS    __attribute__((aligned(USBDEV_CPU_DMA_ALIGNMENT)))

/**
 * @name Flags used in stm32_usbdev_fs_config_t
 * @{
 */
/**
 * @brief Indicates that logic levels are inverted compared to a GPIO connected
 *        directly via a 1.5 kOhm resistor to the USB D+ signal.
 */
#define STM32_USBDEV_FS_CONFIG_FLAG_DISCONN_INVERTED 0x01
/** @} */

/**
 * @brief stm32 USB device FS configuration
 */
typedef struct {
    uintptr_t base_addr;            /**< USB peripheral base address */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
    gpio_t disconn;                 /**< GPIO if used for USB disconnect */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t apb;                    /**< APB bus */
    uint8_t flags;                  /**< Configuration flags */
} stm32_usbdev_fs_config_t;

#ifdef __cplusplus
}
#endif

/** @} */
