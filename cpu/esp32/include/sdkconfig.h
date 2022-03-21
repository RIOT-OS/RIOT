/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       SDK configuration used by ESP-IDF for ESP32x SoCs
 *
 * The SDK configuration can be partially overridden by application-specific
 * board configuration.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

/*
 * The SoC capability definitions are often included indirectly in the
 * ESP-IDF files, although all ESP-IDF files require them. Since not all
 * ESP-IDF header files are included in RIOT, the SoC capability definitions
 * are unknown if they are only indirectly included. Therefore, the SoC
 * capabilities are included in this file and are thus available to all
 * ESP-IDF files. This avoids to update vendor code.
 */
#include "soc/soc_caps.h"

/**
 * @brief   SDK version number
 *
 * Determined with `git describe --tags` in `$ESP32_SDK_DIR`
 */
#if !defined(IDF_VER) || DOXYGEN
#include "esp_idf_ver.h"
#endif

/**
 * @brief   Include ESP32x family specific SDK configuration
 */
#if defined(CPU_FAM_ESP32)
#include "sdkconfig_esp32.h"
#else
#error "ESP32x family implementation missing"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* SDKCONFIG_H */
/** @} */
