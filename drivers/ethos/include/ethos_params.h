/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup drivers_ethos
 * @{
 *
 * @file
 * @brief       Default configuration for the ethos device driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include "board.h"
#include "ethos.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ETHOS_PARAMS
#define ETHOS_PARAMS        { .uart = ETHOS_UART,   \
                              .baudrate = ETHOS_BAUDRATE }
#endif

/**
 * @brief   ethos configuration
 *
 * The first element in this array will be used to multiplex stdio if
 * `stdio_ethos` is included.
 */
static const ethos_params_t ethos_params[] = {
    ETHOS_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
