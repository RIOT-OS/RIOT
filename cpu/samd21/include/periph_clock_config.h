/*
 * Copyright (C) 2017 Daniel Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd21
 * @{
 *
 * @file
 * @brief           CPU specific definitions for generic clock generators
 *
 * @author          Daniel Evans <photonthunder@gmail.com>
 */

#ifndef PERIPH_CLOCK_CONFIG_H
#define PERIPH_CLOCK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief generic clock generator 1
 * setup 1 MHz clock on generic clock generator 1
 */
void setup_gen1_1MHz(void);

/**
 * @brief is gen2 xosc32 enabled
 *
 * @return true if it is enabled
 * @return false if it is not enabled
 */
bool is_enabled_gen2_xosc32(void);

/**
 * @brief   generic clock generator 2
 *
 * @param[in] standby    Run gen2 in standby mode
 */
void setup_gen2_xosc32(bool standby);

/**
 * @brief generic clock generator 3
 * setup 32 kHz ULP internal clock on generic clock generator 3
 */
void setup_gen3_ULP32k(void);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CLOCK_CONFIG_H */
/** @} */
