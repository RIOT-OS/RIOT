/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @brief       CTAP transport layer defines and function declarations
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
 * @brief CTAP supported transport prototocols
 */
typedef enum {
    USB,
    NFC,
    BLE
} ctap_transport_type_t;

/**
 * @brief Initialize ctap_transport layer and fido2_ctap
 */
void fido2_ctap_transport_init(void);

/**
 * @brief Try timeout long to read data from specified transport layer
 *
 * @param[in] type              transport layer to read from
 * @param[in] buffer            buffer for data
 * @param[in] size              size of buffer
 * @param[in] timeout           timeout in microseconds
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_transport_read_timeout(ctap_transport_type_t type, void *buffer,
                                      size_t size, uint32_t timeout);

/**
 * @brief Write to specified transport layer
 *
 * @param[in] type              transport layer to read from
 * @param[in] buffer            data buffer
 * @param[in] size              size of buffer
 *
 * @return @ref ctap_status_codes_t
 */
int fido2_ctap_transport_write(ctap_transport_type_t type, const void *buffer,
                               size_t size);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_TRANSPORT_CTAP_TRANSPORT_H */
/** @} */
