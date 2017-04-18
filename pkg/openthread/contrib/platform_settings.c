/*
 * Copyright (C) Fundación Inria Chile 2017
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for interacting with OpenThread
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 */

#include "assert.h"

#define ENABLE_DEBUG (1)
#include "debug.h"
#include "openthread/types.h"

// settings API
void otPlatSettingsInit(otInstance *aInstance)
{
}

ThreadError otPlatSettingsBeginChange(otInstance *aInstance)
{
    (void)aInstance;
    return kThreadError_None;
}

ThreadError otPlatSettingsCommitChange(otInstance *aInstance)
{
    DEBUG("openthread: otPlatSettingsCommitChange");
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
    DEBUG("openthread: otPlatSettingsGet");
    *aValueLength = 0;
    return kThreadError_NotFound;
}

ThreadError otPlatSettingsSet(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    return kThreadError_None;
}

ThreadError otPlatSettingsAdd(otInstance *aInstance, uint16_t aKey, const uint8_t *aValue, uint16_t aValueLength)
{
    return kThreadError_None;
}

ThreadError otPlatSettingsDelete(otInstance *aInstance, uint16_t aKey, int aIndex)
{
    return kThreadError_None;
}

void otPlatSettingsWipe(otInstance *aInstance)
{
}

