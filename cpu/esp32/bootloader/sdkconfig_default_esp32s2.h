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
 * @brief       Default SDK configuration for the ESP32-S2 SoC bootloader
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef SDKCONFIG_DEFAULT_ESP32S2_H
#define SDKCONFIG_DEFAULT_ESP32S2_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ 160
#endif

#define CONFIG_ESP32S2_DEBUG_OCDAWARE 1

#define CONFIG_BOOTLOADER_OFFSET_IN_FLASH 0x1000
#define CONFIG_EFUSE_MAX_BLK_LEN 256
#define CONFIG_IDF_FIRMWARE_CHIP_ID 0x0002

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
#endif /* SDKCONFIG_DEFAULT_ESP32S2_H */
