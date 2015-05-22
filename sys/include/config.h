/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file
 * @brief       Kernel configuration interface
 *
 * @author      unknown
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#ifdef FEATURE_TRANSCEIVER
#include "radio/types.h"
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#define CONFIG_KEY      (0x1701)    /**< key to identify configuration             */
#define CONFIG_NAME_LEN (10)        /**< length of name for configuration in bytes */

/**
 * @brief Memory for configuration defined externally.
 */
extern char configmem[];

/**
 * @brief  Stores configuration data of the node.
 */
typedef struct {
    uint16_t id;                    /**< unique node identifier          */
#ifdef FEATURE_TRANSCEIVER
    radio_address_t radio_address;  /**< address for radio communication */
    uint8_t radio_channel;          /**< current frequency               */
    uint16_t radio_pan_id;          /**< PAN id for radio communication  */
#endif
    char name[CONFIG_NAME_LEN];     /**< name of the node                */
} config_t;

/**
 * @brief  Element to store in flashrom.
 */
typedef struct {
    uint16_t magic_key;         /**< validity check           */
    config_t config;            /**< the node's configuration */
} configmem_t;

/**
 * @brief Variable sysconfig defined externally
 */
extern config_t sysconfig;

/**
 * @brief  Write configuration back to flashrom.
 *
 * @return 1 on success, 0 otherwise
 */
uint8_t config_save(void);

/**
 * @brief Read configuration from flashrom and stores it to sysconfig
 *
 * @note If no configuration is present within flashrom a new configuration will be created
 */
void config_load(void);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
/** @} */
