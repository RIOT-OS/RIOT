/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
#ifndef ISR_STACKSIZE
#  define ISR_STACKSIZE               (2048)
#endif
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
