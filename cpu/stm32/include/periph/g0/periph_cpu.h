/*
 * Copyright (C) 2020 Inria
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
 * @brief           STM32G0 CPU specific definitions for internal peripheral handling
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef PERIPH_G0_PERIPH_CPU_H
#define PERIPH_G0_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

/**
 * @name   Constants for internal VBAT ADC line
 * @{
 */
#define VBAT_ADC_RES        ADC_RES_12BIT
#define VBAT_ADC_MAX        4095
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   TIM6, DAC and LPTIM1 share the same interrupt
 */
#if defined(CPU_LINE_STM32G0B1xx) || defined(CPU_LINE_STM32G081xx) || \
    defined(CPU_LINE_STM32G071xx) || defined(CPU_LINE_STM32G0C1xx)
#define TIM6_DAC_LPTIM1_SHARED_IRQ
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_G0_PERIPH_CPU_H */
/** @} */
