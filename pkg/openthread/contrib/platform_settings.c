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
 * @brief       Implementation of OpenThread settings platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include "assert.h"
#include "openthread/types.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void otPlatSettingsInit(otInstance *aInstance)
{
    (void)aInstance;
}

ThreadError otPlatSettingsBeginChange(otInstance *aInstance)
{
    (void)aInstance;
    return kThreadError_None;
}

ThreadError otPlatSettingsCommitChange(otInstance *aInstance)
{
    DEBUG("openthread: otPlatSettingsCommitChange\n");
    (void)aInstance;
    return kThreadError_None;
}

ThreadError otPlatSettingsAbandonChange(otInstance *aInstance)
{
    (void)aInstance;
    return kThreadError_None;
}

ThreadError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue, uint16_t *aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aIndex;
    (void)aValue;

    DEBUG("openthread: otPlatSettingsGet\n");
    *aValueLength = 0;
    return kThreadError_NotImplemented;
}

ThreadError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aValue;
    (void)aValueLength;

    return kThreadError_None;
}

ThreadError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aValue;
    (void)aValueLength;

    return kThreadError_None;
}

ThreadError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    (void)aInstance;
    (void)aKey;
    (void)aIndex;

    return kThreadError_None;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    (void)aInstance;
}
