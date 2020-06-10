/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief           STM32F7 CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef PERIPH_F7_PERIPH_CPU_H
#define PERIPH_F7_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x1FF00000)

#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F7_PERIPH_CPU_H */
/** @} */
