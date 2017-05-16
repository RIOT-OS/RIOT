/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  JosÃ© Ignacio Alamos <jialamos@uc.cl>
 */

#include <stdint.h>

#include "msg.h"
#include "openthread/platform/alarm.h"
#include "openthread/platform/misc.h"
#include "ot.h"
#include "thread.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void otPlatReset(otInstance *aInstance)
{
    (void)aInstance;
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    (void)aInstance;
    return kPlatResetReason_PowerOn;
}
/** @} */
