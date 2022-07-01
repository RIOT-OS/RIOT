/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_esp32_conf ESP32 compile configurations
 * @ingroup     cpu_esp32
 * @ingroup     config
 * @brief       Compile-time configuration macros for ESP32x SoCs
 * @{
 *
 * @file
 * @brief       Compile compile-time configurations that are common for all ESP32x SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

/**
 * @name   Stack size configuration
 * @{
 */
/** Extra thread stack size required if newlib-nano is not used */
#ifdef MODULE_NEWLIB_NANO
#define THREAD_EXTRA_STACKSIZE          (0)
#else
#define THREAD_EXTRA_STACKSIZE          (512)
#endif

/** Extra thread stack size if `printf` is used */
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1536)

#ifndef THREAD_STACKSIZE_DEFAULT
/** Default thread stack size */
#define THREAD_STACKSIZE_DEFAULT        (2048)
#endif

#ifndef THREAD_STACKSIZE_IDLE
/** Stack size for the idle thread */
#define THREAD_STACKSIZE_IDLE           (2048)
#endif

#ifndef ESP_WIFI_STACKSIZE
/** Stack size for the WiFi thread */
#define ESP_WIFI_STACKSIZE              (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE)
#endif
/** @} */

/** Buffer size used for printf functions (maximum length of formatted output). */
#define PRINTF_BUFSIZ 256

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".noinit")))

/* include ESP32x SoC specific compile time configurations */
#if defined(CPU_FAM_ESP32)
#include "cpu_conf_esp32.h"
#elif defined(CPU_FAM_ESP32C3)
#include "cpu_conf_esp32c3.h"
#else
#error "ESP32x family implementation missing"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
