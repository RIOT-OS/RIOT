/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_transport FIDO2 CTAP transport
 * @ingroup     fido2_ctap
 * @brief       CTAP transport layer
 *
 * @{
 *
 * @file
 * @brief       CTAP transport layer defines and function declarations
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
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
 * @brief CTAP transport thread priority
 */
#ifndef CTAP_TRANSPORT_PRIO
#define CTAP_TRANSPORT_PRIO     (THREAD_PRIORITY_MAIN - 5)
#endif
/**
 * @brief Initialize ctap_transport layer and fido2_ctap
 */
void fido2_ctap_transport_init(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* FIDO2_CTAP_TRANSPORT_CTAP_TRANSPORT_H */
