/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SHT1X_BOARD_H
#define SHT1X_BOARD_H

/**
 * @ingroup     boards_common_msba2
 * @{
 */

/**
 * @file
 * @brief       LPC2387 SHT11 Device Driver
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @version     $Revision$
 *
 * @note        $Id$
 */

#include "cpu.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  serial clock line low */
#define SHT1X_SCK_LOW   FIO1CLR = BIT25;
/*  serial clock line high */
#define SHT1X_SCK_HIGH  FIO1SET = BIT25;
/*  read serial I/O */
#define SHT1X_DATA      ((FIO1PIN & BIT26) != 0)
/*  serial I/O line low */
#define SHT1X_DATA_LOW  (FIO1CLR = BIT26);
/*  serial I/O line high */
#define SHT1X_DATA_HIGH (FIO1SET = BIT26);
/*  serial I/O as input */
#define SHT1X_DATA_IN   (FIO1DIR &= ~BIT26)
/*  serial I/O as output */
#define SHT1X_DATA_OUT  (FIO1DIR |= BIT26)

#define SHT1X_INIT      FIO1DIR |= BIT25; PINSEL3 &= ~(BIT14|BIT15 | BIT16|BIT17);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SHT1X_BOARD_H */
