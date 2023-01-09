/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_esp32_esp_idf_api ESP-IDF Interface API
 * @ingroup         cpu_esp32
 * @brief           ESP-IDF Interface API
 *
 * This module implements an interface for ESP-IDF types and functions that are
 * required by RIOT-OS but cannot be included directly due to name conflicts.
 *
 * For this purpose, the header files of this module declare all the types and
 * functions that are required from the ESP-IDF, but without using the ESP-IDF
 * header files with conflicting names. The implementation of the module then
 * uses the ESP-IDF. In most cases, simple wrapper functions are sufficient.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */
