/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       gcoap example
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "net/gcoap.h"
#include "net/utils.h"
#include "od.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t req_count;  /**< Counts requests sent by CLI. */

/**
 * @brief   Registers the CoAP resources exposed in the example app
 *
 * Run this exactly one during startup.
 */
void server_init(void);

/**
 * @brief   Notifies all observers registered to /cli/stats - if any
 *
 * Call this whenever the count of successfully send client requests changes
 */
void notify_observers(void);

#ifdef __cplusplus
}
#endif

/** @} */
