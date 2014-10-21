/*
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_redbee-econotag Redbee Econotag
 * @ingroup     boards
 * @brief       Support for the Redbee Econotag board
 * @{
 *
 * @file
 * @brief       Basic definitions for the Redbee Econotag board
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef REDBEE_ECONOTAG_BOARD_H
#define REDBEE_ECONOTAG_BOARD_H

#include <stdint.h>

#define F_CPU   (24000000)              ///< CPU target speed in Hz

#define CTUNE       0xb
#define IBIAS       0x1f
#define FTUNE       0x7

typedef uint8_t radio_packet_length_t;

#endif
