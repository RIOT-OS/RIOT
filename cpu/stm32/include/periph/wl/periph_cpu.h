/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @brief           STM32WL CPU specific definitions for internal peripheral handling
 *
 * @author          Akshai M <akshai.m@fu-berlin.de>
 *
 */

#ifndef PERIPH_WL_PERIPH_CPU_H
#define PERIPH_WL_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606 ( Table 143 : System memory)
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_WL_PERIPH_CPU_H */
/** @} */
