/*
 * SPDX-FileCopyrightText: 2017 Fundacion Inria Chile
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "openthread/error.h"
#include "openthread/instance.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void otPlatSettingsInit(otInstance *aInstance)
{
    (void)aInstance;
}

otError otPlatSettingsBeginChange(otInstance *aInstance)
{
    (void)aInstance;
    return OT_ERROR_NONE;
}

otError otPlatSettingsCommitChange(otInstance *aInstance)
{
    DEBUG("openthread: otPlatSettingsCommitChange\n");
    (void)aInstance;
    return OT_ERROR_NONE;
}

otError otPlatSettingsAbandonChange(otInstance *aInstance)
{
    (void)aInstance;
    return OT_ERROR_NONE;
}

otError otPlatSettingsGet(otInstance *aInstance, uint16_t aKey, int aIndex, uint8_t *aValue,
                          uint16_t *aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aIndex;
    (void)aValue;

    DEBUG("openthread: otPlatSettingsGet\n");
    *aValueLength = 0;
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue,
                          uint16_t aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aValue;
    (void)aValueLength;
    return OT_ERROR_NONE;
}

otError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue,
                          uint16_t aValueLength)
{
    (void)aInstance;
    (void)aKey;
    (void)aValue;
    (void)aValueLength;
    return OT_ERROR_NONE;
}

otError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    (void)aInstance;
    (void)aKey;
    (void)aIndex;
    return OT_ERROR_NONE;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
    (void)aInstance;
}
