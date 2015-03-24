/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     board_cy8ckit-050
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Cypress CY8CKIT-050 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "board.h"

void board_init(void)
{
	/* initialize the CPU */
    cpu_init();
}
