/*
 * Copyright (C) 2019 Koen Zandberg
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_suit
 * @brief       SUIT conditions
 *
 * @{
 *
 * @brief       SUIT conditions API
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef SUIT_CONDITIONS_H
#define SUIT_CONDITIONS_H

#include <stddef.h>
#include <stdint.h>

#include "uuid.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The SUIT vendor ID source
 *
 * The basis of the UUID must be the vendor domain, please change this when
 * using this module in a product
 */
#ifndef SUIT_VENDOR_DOMAIN
#define SUIT_VENDOR_DOMAIN  "riot-os.org" /**< Device vendor domain */
#endif

/**
 * @brief The SUIT class ID source
 *
 * By default the RIOT_VERSION define is used for this
 */
#ifndef SUIT_CLASS_ID
#define SUIT_CLASS_ID  RIOT_BOARD
#endif

/**
 * @brief SUIT conditionals
 */
enum {
    SUIT_COND_VENDOR_ID     = 1,    /**< Vendor ID match conditional */
    SUIT_COND_CLASS_ID      = 2,    /**< Class ID match conditional */
    SUIT_COND_DEV_ID        = 3,    /**< Device ID match conditional */
    SUIT_COND_BEST_BEFORE   = 4,    /**< Best before conditional */
};

/**
 * @brief SUIT condition parameters
 */
typedef struct {
    uuid_t vendor;  /**< Vendor url as UUID                  */
    uuid_t class;   /**< Device class UUID                   */
    uuid_t device;  /**< Device specific information as UUID */
} suit_condition_params_t;

/**
 * @brief Initialize boot-time conditions for SUIT manifests
 *
 * This initializes the device-based conditions for validating manifest
 * preconditions
 *
 * Vendor url as UUID:   UUID5(DNS_PREFIX, SUIT_VENDOR_DOMAIN)
 * Device class UUID:    UUID5(vendor, SUIT_CLASS_ID)
 * Device specific UUID: UUID5(vendor, Device ID)
 */
void suit_init_conditions(void);

/**
 * @brief Retrieve the generated vendor ID
 *
 * @returns     The vendor ID as UUID
 */
uuid_t *suit_get_vendor_id(void);

/**
 * @brief Retrieve the generated class ID
 *
 * @returns     The class ID as UUID
 */
uuid_t *suit_get_class_id(void);

/**
 * @brief Retrieve the generated device ID
 *
 * @returns     The device ID as UUID
 */
uuid_t *suit_get_device_id(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SUIT_CONDITIONS_H */
