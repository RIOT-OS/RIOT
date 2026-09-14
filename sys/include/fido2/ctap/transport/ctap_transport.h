/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
