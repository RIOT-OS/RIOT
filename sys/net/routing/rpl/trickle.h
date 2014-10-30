/**
 * Trickle constants and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 * @file    trickle.h
 * @brief   Trickle
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include "vtimer.h"
#include "thread.h"

#define TRICKLE_TIMER_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define TRICKLE_INTERVAL_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define DAO_DELAY_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define RT_STACKSIZE (KERNEL_CONF_STACKSIZE_DEFAULT)

void reset_trickletimer(void);
void init_trickle(void);
void start_trickle(uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void trickle_increment_counter(void);
void delay_dao(void);
void dao_ack_received(void);
