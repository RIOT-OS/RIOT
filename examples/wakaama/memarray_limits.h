/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Header for Wakaama example client memarray pre-allocation limits.
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 */


#ifndef MEMARRAY_LIMITS_H
#define MEMARRAY_LIMITS_H

#include "memarray.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum number of 16 byte blocks. */
#define MAX_16BYTE_BLOCKS (25U)
/** Maximum number of 32 byte blocks. */
#define MAX_32BYTE_BLOCKS (15U)
/** Maximum number of 64 byte blocks. */
#define MAX_64BYTE_BLOCKS (20U)
/** Maximum number of 192 byte blocks. */
#define MAX_192BYTE_BLOCKS (10U)
/** Maximum number of 512 byte blocks. */
#define MAX_512BYTE_BLOCKS (1U)

/**
 * @brief  Initializes memarray blocks for the lwm2m example client.
 */
void lwm2m_memarray_init(void);

/**
 * @brief  Prints allocation and de-allocation statistics for
 *         memarray. Requires debugging to be enabled on platform.c.
 */
void lwm2m_memarray_print_stats(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MEMARRAY_LIMITS_H */
