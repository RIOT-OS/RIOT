/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards
 * @{
 *
 * @file
 * @brief       Generic board functions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"
#include "kernel_defines.h"

/*
 * Allow overwriting board_init if common implementation doesn't work.
 * If at link time another implementation of board_init() not marked as weak
 * ((a.k.a. a strong symbol) is present, it will be linked in instead.
 */
void __attribute__((weak)) board_init(void)
{}
