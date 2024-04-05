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
 * @brief           Shared CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "cpu.h"
#include "macros/units.h"

#if defined(CPU_FAM_STM32F0)
#include "periph/f0/periph_cpu.h"
#elif defined(CPU_FAM_STM32F1)
#include "periph/f1/periph_cpu.h"
#elif defined(CPU_FAM_STM32F2)
#include "periph/f2/periph_cpu.h"
#elif defined(CPU_FAM_STM32F3)
#include "periph/f3/periph_cpu.h"
#elif defined(CPU_FAM_STM32F4)
#include "periph/f4/periph_cpu.h"
#elif defined(CPU_FAM_STM32F7)
#include "periph/f7/periph_cpu.h"
#elif defined(CPU_FAM_STM32G0)
#include "periph/g0/periph_cpu.h"
#elif defined(CPU_FAM_STM32C0)
#include "periph/c0/periph_cpu.h"
#elif defined(CPU_FAM_STM32G4)
#include "periph/g4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L0)
#include "periph/l0/periph_cpu.h"
#elif defined(CPU_FAM_STM32L1)
#include "periph/l1/periph_cpu.h"
#elif defined(CPU_FAM_STM32L4)
#include "periph/l4/periph_cpu.h"
#elif defined(CPU_FAM_STM32L5)
#include "periph/l5/periph_cpu.h"
#elif defined(CPU_FAM_STM32U5)
#include "periph/u5/periph_cpu.h"
#elif defined(CPU_FAM_STM32WB)
#include "periph/wb/periph_cpu.h"
#elif defined(CPU_FAM_STM32WL)
#include "periph/wl/periph_cpu.h"
#endif

#include "periph/cpu_backup_ram.h"
#include "periph/cpu_common.h"
#include "periph/cpu_dma.h"
#include "periph/cpu_eth.h"
#include "periph/cpu_fmc.h"
#include "periph/cpu_gpio.h"
#include "periph/cpu_gpio_ll.h"
#include "periph/cpu_i2c.h"
#include "periph/cpu_ltdc.h"
#include "periph/cpu_pm.h"
#include "periph/cpu_pwm.h"
#include "periph/cpu_qdec.h"
#include "periph/cpu_sdmmc.h"
#include "periph/cpu_spi.h"
#include "periph/cpu_timer.h"
#include "periph/cpu_uart.h"
#include "periph/cpu_usbdev.h"
#include "periph/cpu_vbat.h"
#include "periph/cpu_wdt.h"

#ifdef MODULE_PERIPH_CAN
#ifdef MODULE_FDCAN
#include "fdcandev_stm32.h"
#else
#include "candev_stm32.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
#if !defined(CPU_FAM_STM32F0) && !defined(CPU_FAM_STM32L0) && \
    !defined(CPU_FAM_STM32L1) && !defined(CPU_FAM_STM32WL)
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
#endif
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @name    PTP clock configuration
 * @{
 */
#define HAVE_PTP_CLOCK_READ         1   /**< Native implementation available */
#define HAVE_PTP_CLOCK_SET          1   /**< Native implementation available */
#define HAVE_PTP_TIMER_SET_ABSOLUTE 1   /**< Native implementation available */
/** @} */

#if !DOXYGEN    /* hide implementation details */
/**
 * @name    USB device definitions
 * @{
 */
/* Detect the IP version based on the available register define */
#if defined(USB_OTG_GCCFG_NOVBUSSENS)
#define STM32_USB_OTG_CID_1x        /**< USB OTG FS version 0x00001200 */
#elif defined(USB_OTG_GCCFG_VBDEN)
#define STM32_USB_OTG_CID_2x        /**< USB OTG FS version 0x00002000 */
#elif defined(USB)
#define STM32_USB_FS_CID_1x         /**< USB FS version 0x00001200 */
#endif

/**
 * @brief Number of endpoints available with the OTG FS peripheral
 *        including the control endpoint
 */
#if defined(USB_OTG_FS_MAX_IN_ENDPOINTS)
#define STM32_USB_OTG_FS_NUM_EP     (USB_OTG_FS_MAX_IN_ENDPOINTS)
#elif defined(STM32_USB_OTG_CID_1x)
#define STM32_USB_OTG_FS_NUM_EP     (4)    /**< OTG FS with 4 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_FS_NUM_EP     (6)    /**< OTG FS with 6 endpoints */
#endif

/**
 * @brief Number of endpoints available with the OTG HS peripheral
 *        including the control endpoint
 */
#if defined(USB_OTG_HS_MAX_IN_ENDPOINTS)
#define STM32_USB_OTG_HS_NUM_EP     (USB_OTG_HS_MAX_IN_ENDPOINTS)
#elif defined(STM32_USB_OTG_CID_1x)
#define STM32_USB_OTG_HS_NUM_EP     (6)     /**< OTG HS with 6 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_HS_NUM_EP     (9)     /**< OTG HS with 9 endpoints */
#endif

/**
 * @brief Number of IN/OUT endpoints including EP0 as used by USBUS
 *
 * @note USBUS allows only one definition of the number of available EPs, which
 *       is then used for all devices. To be able to use all EPs for devices
 *       with more EPs, the largest possible number of available EPs for
 *       several USB devices is defined here. The driver has to ensure that the
 *       number of allocated EPs does not exceed the number of available EPs if
 *       a device has less EPs.
 */
#if defined(MODULE_PERIPH_USBDEV_HS) && defined(STM32_USB_OTG_HS_NUM_EP)
#define USBDEV_NUM_ENDPOINTS            STM32_USB_OTG_HS_NUM_EP
#elif defined(STM32_USB_OTG_FS_NUM_EP)
#define USBDEV_NUM_ENDPOINTS            STM32_USB_OTG_FS_NUM_EP
#else
#define USBDEV_NUM_ENDPOINTS            8
#endif

/* unify names across STM32 families */
#ifdef SPI_CR1_CPHA_Msk
#  define STM32_SPI_CPHA_Msk            SPI_CR1_CPHA_Msk
#endif
#ifdef SPI_CFG2_CPHA_Msk
#  define STM32_SPI_CPHA_Msk            SPI_CFG2_CPHA_Msk
#endif
#ifdef SPI_CR1_CPOL_Msk
#  define STM32_SPI_CPOL_Msk            SPI_CR1_CPOL_Msk
#endif
#ifdef SPI_CFG2_CPOL_Msk
#  define STM32_SPI_CPOL_Msk            SPI_CFG2_CPOL_Msk
#endif

/**
 * @name   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0,                                         /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = STM32_SPI_CPHA_Msk,                        /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = STM32_SPI_CPOL_Msk,                        /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = STM32_SPI_CPOL_Msk | STM32_SPI_CPHA_Msk,   /**< CPOL=1, CPHA=0 */
} spi_mode_t;
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
