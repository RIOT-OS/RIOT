/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Setup of the LoRa device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sets up the first LoRa device of the board
 *
 * @return  the netdev of the device, not initialized
 */
netdev_t *lora_setup(void);

#ifdef __cplusplus
}
#endif

/** @} */
