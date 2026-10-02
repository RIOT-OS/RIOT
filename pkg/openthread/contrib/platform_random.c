/*
 * SPDX-FileCopyrightText: 2017 Fundacion Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
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
