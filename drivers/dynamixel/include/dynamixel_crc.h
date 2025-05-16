/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
