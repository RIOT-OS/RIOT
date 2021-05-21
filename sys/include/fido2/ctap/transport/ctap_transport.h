/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_transport CTAP transport layer
 * @ingroup     fido2_ctap
 * @brief       CTAP transport layer
 *
 * @{
 *
 * @file
 * @brief       CTAP2 transport layer defines and function declarations
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_TRANSPORT_CTAP_TRANSPORT_H
#define FIDO2_CTAP_TRANSPORT_CTAP_TRANSPORT_H

#include <stdint.h>
#include "mutex.h"
#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CTAP supported transport prototocols
 *
 * @{
 */
#define CTAP_TRANSPORT_USB 0x1
#define CTAP_TRANSPORT_NFC 0x2
#define CTAP_TRANSPORT_BLE 0x3
/** @} */

/**
 * @brief CTAP_TRANSPORT thread stack size
 */
#define CTAP_TRANSPORT_STACKSIZE 16000

/**
 * @brief Initialize ctap_transport layer and fido2_ctap
 */
void fido2_ctap_transport_init(void);

/**
 * @brief Initialize needed RIOT transport layers
 *
 * @param[in] type              transport layer to initialize
 * @param[in] arg               optional arg
 * @param[in] size              size of optional arg
 *
 * @return CTAP status code
 */
int fido2_ctap_transport_create(uint8_t type, void *arg, size_t size);

/**
 * @brief Try timeout long to read data from specified transport layer
 *
 * @param[in] type              transport layer to read from
 * @param[in] buffer            buffer for data
 * @param[in] size              size of buffer
 * @param[in] timeout           timeout
 *
 * @return CTAP status code
 */
int fido2_ctap_transport_read_timeout(uint8_t type, void *buffer, size_t size,
                                      uint32_t timeout);

/**
 * @brief Write to specified transport layer
 *
 * @param[in] type              transport layer to read from
 * @param[in] buffer            data buffer
 * @param[in] size              size of buffer
 *
 * @return CTAP status code
 */
int fido2_ctap_transport_write(uint8_t type, const void *buffer, size_t size);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_TRANSPORT_CTAP_TRANSPORT_H */
/** @} */
