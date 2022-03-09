/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           USB CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_USBDEV_H
#define PERIPH_CPU_USBDEV_H

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
 * @brief USB OTG peripheral type.
 *
 * High speed peripheral is assumed to have DMA support available.
 *
 * @warning Only one of each type is supported at the moment, it is not
 * supported to have two FS type or two HS type peripherals enabled on a
 * single MCU.
 */
typedef enum {
    STM32_USB_OTG_FS = 0,   /**< Full speed peripheral */
    STM32_USB_OTG_HS = 1,   /**< High speed peripheral */
} stm32_usb_otg_fshs_type_t;

/**
 * @brief Type of USB OTG peripheral phy.
 *
 * The FS type only supports the built-in type, the HS phy can have either the
 * FS built-in phy enabled or the HS ULPI interface enabled.
 */
typedef enum {
    STM32_USB_OTG_PHY_BUILTIN,
    STM32_USB_OTG_PHY_ULPI,
} stm32_usb_otg_fshs_phy_t;

/**
 * @brief stm32 USB OTG configuration
 */
typedef struct {
    uintptr_t periph;               /**< USB peripheral base address */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    stm32_usb_otg_fshs_phy_t phy;   /**< Built-in or ULPI phy */
    stm32_usb_otg_fshs_type_t type; /**< FS or HS type */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t ahb;                    /**< AHB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
} stm32_usb_otg_fshs_config_t;

/**
 * @brief stm32 USB device FS configuration
 */
typedef struct {
    uintptr_t base_addr;            /**< USB peripheral base address */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t apb;                    /**< APB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
} stm32_usbdev_fs_config_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_USBDEV_H */
/** @} */
