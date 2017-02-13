/*
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SHT11BOARD_H_
#define SHT11BOARD_H_

/**
 * @ingroup     atmega256rfr2
 * @{
 */

/**
 * @file
 * @brief        SHT11 Device Driver
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @version     $Revision$
 *
 * @note        $Id$
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/*  serial clock line low */
#define SHT11_SCK_LOW   PORTD &= ~(1<<PD0);
/*  serial clock line high */
#define SHT11_SCK_HIGH  PORTD |= (1<<PD0);
/*  read serial I/O */
#define SHT11_DATA      ((PIND & (1<<PD1)) == 0)
/*  serial I/O line low */
#define SHT11_DATA_LOW  (PORTD &= ~(1<<PD1));
/*  serial I/O line high */
#define SHT11_DATA_HIGH (PORTD |= (1<<PD1));
/*  serial I/O as input */
#define SHT11_DATA_IN   (DDRD &= ~(1<<PD1))
/*  serial I/O as output */
#define SHT11_DATA_OUT  (DDRD |= (1<<PD1))

#define SHT11_INIT      DDRD |= (1<<PD0); DDRE |= (1<<PE4); PORTE &= ~(1<<PE4);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SHT11BOARD_H_ */
