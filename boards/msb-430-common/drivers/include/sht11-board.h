/*
 * Copyright 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SHT11BOARD_H_
#define SHT11BOARD_H_

/**
 * @ingroup     msb_430h
 * @{
 */

/**
 * @file
 * @brief       SHT11 Device Driver Configuration For MSB-430 Platform
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, RIOT
 *
 */
#include <msp430x16x.h>
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SCK  = P3B5
 * DATA = P3B4
 */

#define SHT11_SCK_LOW   P3OUT &= ~(BIT5);     /**< serial clock line low */
#define SHT11_SCK_HIGH  P3OUT |= BIT5;      /**< serial clock line high */
#define SHT11_DATA      (P3IN & BIT5)      /**< read serial I/O */
#define SHT11_DATA_LOW  P3OUT &= ~(BIT5);   /**< serial I/O line low */
#define SHT11_DATA_HIGH P3OUT |= BIT5;      /**< serial I/O line high */
#define SHT11_DATA_IN   P3DIR &= ~(BIT5);      /**< serial I/O as input */
#define SHT11_DATA_OUT  P3DIR |= BIT5;      /**< serial I/O as output */
#define SHT11_INIT      P3DIR |= BIT5;      /* FIO1DIR |= BIT25; PINSEL3 &= ~(BIT14|BIT15 | BIT16|BIT17); */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SHT11BOARD_H_ */
