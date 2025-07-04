/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Default SDK configuration for the ESP32-C3 SoC bootloader
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_ESP32H2_REV_MIN_0

#ifndef CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ
#  define CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ 96
#endif

#define CONFIG_XTAL_FREQ 32

#define CONFIG_ESP_DEBUG_OCDAWARE 1

#define CONFIG_BOOTLOADER_OFFSET_IN_FLASH 0x0
#define CONFIG_EFUSE_MAX_BLK_LEN 256
#define CONFIG_IDF_FIRMWARE_CHIP_ID 0x0010
#define CONFIG_MMU_PAGE_SIZE 0x8000

#define CONFIG_ESP_CONSOLE_SECONDARY_USB_SERIAL_JTAG 1

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
