/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dynamixel
 *
 * @{
 *
 * @file
 * @brief       Interface definition for Dynamixel crc
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 */

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t dynamixel_crc_update(uint16_t crc_accum, const uint8_t *buffer, size_t size);

#ifdef __cplusplus
}
#endif

/** @} */
