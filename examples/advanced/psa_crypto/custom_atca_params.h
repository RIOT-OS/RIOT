/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example custom atca_params.h file to use multiple ATECC608A
 *              secure elements as backends for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */
#include "cryptoauthlib.h"
#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSA_ATCA_LOCATION_DEV0  (PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT)
#define ATCA_PARAM_I2C_DEV0     (I2C_DEV(0))    /*!< Change this to the bus you want to use */
#define ATCA_PARAM_ADDR_DEV0    (0xC0)          /*!< Change this to your first device's address */
#define ATCA_DEVTYPE_DEV0       (ATECC608A)
#define ATCA_RX_RETRIES         (20)

#ifdef MULTIPLE_SE
#define PSA_ATCA_LOCATION_DEV1  (PSA_KEY_LOCATION_SE_MIN)
#define ATCA_PARAM_I2C_DEV1     (I2C_DEV(0))    /*!< Change this to the bus you want to use */
#define ATCA_PARAM_ADDR_DEV1    (0xCC)          /*!< Change this to your second device's address */
#define ATCA_DEVTYPE_DEV1       (ATECC608A)

#define ATCA_PARAMS                     {   .atca_loc = PSA_ATCA_LOCATION_DEV0,\
                                            .cfg = {\
                                                .iface_type = ATCA_I2C_IFACE, \
                                                .devtype = ATCA_DEVTYPE_DEV0, \
                                                .atcai2c.address = ATCA_PARAM_ADDR_DEV0, \
                                                .atcai2c.bus = ATCA_PARAM_I2C_DEV0, \
                                                .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                                .wake_delay = 1500, \
                                                .rx_retries = ATCA_RX_RETRIES } \
                                        }, \
                                        {   .atca_loc = PSA_ATCA_LOCATION_DEV1,\
                                            .cfg = {\
                                                .iface_type = ATCA_I2C_IFACE, \
                                                .devtype = ATCA_DEVTYPE_DEV1, \
                                                .atcai2c.address = ATCA_PARAM_ADDR_DEV1, \
                                                .atcai2c.bus = ATCA_PARAM_I2C_DEV1, \
                                                .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                                .wake_delay = 1500, \
                                                .rx_retries = ATCA_RX_RETRIES } \
                                        }
#else
#define ATCA_PARAMS                     {   .atca_loc = PSA_ATCA_LOCATION_DEV0,\
                                            .cfg = {\
                                                .iface_type = ATCA_I2C_IFACE, \
                                                .devtype = ATCA_DEVTYPE_DEV0, \
                                                .atcai2c.address = ATCA_PARAM_ADDR_DEV0, \
                                                .atcai2c.bus = ATCA_PARAM_I2C_DEV0, \
                                                .atcai2c.baud = -1, /**< Not used in RIOT */ \
                                                .wake_delay = 1500, \
                                                .rx_retries = ATCA_RX_RETRIES } \
                                        }
#endif /* MULTIPLE_SE */

#define ATCA_SLOTS_DEV0 { \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_AES, 0, 0 }, \
                                    { PSA_KEY_TYPE_HMAC, 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { 0, 1, 1 }, \
                                    { 0, 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { 0, 0, 0 }, \
                                    { 0, 0, 0 }}

#ifdef MULTIPLE_SE
#define ATCA_SLOTS_DEV1 { \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_AES, 0, 0 }, \
                                    { PSA_KEY_TYPE_HMAC, 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { 0, 1, 1 }, \
                                    { 0, 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1), 0, 0 }, \
                                    { 0, 0, 0 }, \
                                    { 0, 0, 0 }}
#endif

#ifdef MULTIPLE_SE
#define ATCA_CONFIG_LIST        { ATCA_SLOTS_DEV0 }, \
                                { ATCA_SLOTS_DEV1 }
#else
#define ATCA_CONFIG_LIST        { ATCA_SLOTS_DEV0 }
#endif

#ifdef __cplusplus
}
#endif

/** @} */
