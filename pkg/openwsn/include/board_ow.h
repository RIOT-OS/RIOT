/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 *
 * @}
 */

#ifndef __BOARD_OW_H
#define __BOARD_OW_H

#include "board_info.h"
#include "toolchain_defs.h"

typedef enum {
    DO_NOT_KICK_SCHEDULER,
    KICK_SCHEDULER,
} kick_scheduler_t;

void board_init_ow(void);
void board_sleep(void);
void board_reset(void);

#endif
