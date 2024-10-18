/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *               2017 Inria
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
 * @brief           STM32L0 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef PERIPH_L0_PERIPH_CPU_H
#define PERIPH_L0_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x1FF00000)

/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = (0x3 << 3),     /**< ADC resolution: 6 bit */
    ADC_RES_8BIT  = (0x2 << 3),     /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = (0x1 << 3),     /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (0x0 << 3),     /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = (0xfe),         /**< not applicable */
    ADC_RES_16BIT = (0xff)          /**< not applicable */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @name    EEPROM configuration
 * https://www.st.com/en/microcontrollers-microprocessors/stm32l0-series.html#products
 * @{
 */
#define EEPROM_START_ADDR          (0x08080000)
#if defined(CPU_LINE_STM32L073xx) || defined(CPU_LINE_STM32L072xx)
#define EEPROM_SIZE                (6144U)  /* 6kB */
#elif defined(CPU_LINE_STM32L053xx) || defined(CPU_LINE_STM32L052xx)
#define EEPROM_SIZE                (2048U)  /* 2kB */
#elif defined(CPU_LINE_STM32L031xx)
#define EEPROM_SIZE                (1024U)  /* 1kB */
#elif defined(CPU_LINE_STM32L010xB) || defined(CPU_LINE_STM32L011x3) || \
      defined(CPU_LINE_STM32L011x4) || defined(CPU_LINE_STM32L021x4) || \
      defined(CPU_MODEL_STM32L011K4)
#define EEPROM_SIZE                (512U)   /* 512B */
#elif defined(CPU_LINE_STM32L010x6) || defined(CPU_LINE_STM32L010x8)
#define EEPROM_SIZE                (256U)   /* 256B */
#elif defined(CPU_LINE_STM32L010x4)
#define EEPROM_SIZE                (128U)   /* 128B */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_L0_PERIPH_CPU_H */
