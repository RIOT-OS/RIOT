/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Xtensa ASM code specific configuration options for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtensa ASM code specific default stack sizes
 * @{
 */
#define ISR_STACKSIZE                 (2048)
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
