/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_runstate
 * @{
 *
 * @file
 * @brief       System run state implementation
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include "runstate.h"

#define ENABLE_DEBUG      (0)
#include "debug.h"

void runstate_set(runstate_t state)
{
    DEBUG("Now entering system run state 0x%x\n", state);

    _runstate = state;
}
