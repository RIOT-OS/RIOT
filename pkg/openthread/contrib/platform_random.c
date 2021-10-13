/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread random platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include "openthread/platform/entropy.h"
#include "random.h"

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    random_bytes(aOutput, aOutputLength);
    return OT_ERROR_NONE;
}
