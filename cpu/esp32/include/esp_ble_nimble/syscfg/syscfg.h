/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32_esp_ble_nimble
 * @{
 *
 * @file
 * @brief       NimBLE configuration for ESP32x SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if !DOXYGEN

/* disable native transport mechanism and enable the custom transport mechanism */
#define MYNEWT_VAL_BLE_TRANSPORT_LL__custom (1)
#define MYNEWT_VAL_BLE_TRANSPORT_LL__native (0)

#include_next "syscfg/syscfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* !DOXYGEN */
/** @} */
