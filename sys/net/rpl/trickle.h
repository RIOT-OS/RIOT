/**
 * Trickle constants and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    trickle.h
 * @brief   Trickle
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <vtimer.h>
#include <thread.h>

#define TRICKLE_TIMER_STACKSIZE 3072
#define TRICKLE_INTERVAL_STACKSIZE 3072
#define DAO_DELAY_STACKSIZE 3072
#define RT_STACKSIZE 512

void reset_trickletimer(void);
void init_trickle(void);
void start_trickle(uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void trickle_increment_counter(void);
void trickle_timer_over(void);
void trickle_interval_over(void);
void delay_dao(void);
void dao_delay_over(void);
void dao_ack_received(void);
void rt_timer_over(void);
