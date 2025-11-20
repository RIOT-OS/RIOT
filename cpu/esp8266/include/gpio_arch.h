/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Architecture specific GPIO functions for ESP8266
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "gpio_arch_common.h"

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/** Map of GPIO pin numbers to IOMUX pin numbers */
extern const uint8_t _gpio_to_iomux[];
/** Map of IOMUX pin numbers to GPIO pin numbers */
extern const uint8_t _iomux_to_gpio[];

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
