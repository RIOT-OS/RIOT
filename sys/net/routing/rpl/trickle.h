/**
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     rpl
 * @{
 * @file        trickle.h
 * @brief       Trickle
 *
 * Header-file, which defines all Trickle constants and prototypes
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include "vtimer.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRICKLE_TIMER_STACKSIZE (1024+128)
#define TRICKLE_INTERVAL_STACKSIZE (512)
#define DAO_DELAY_STACKSIZE (1024+128)
#define RT_STACKSIZE (512)

void reset_trickletimer(void);
void init_trickle(void);
void start_trickle(uint8_t DIOINtMin, uint8_t DIOIntDoubl, uint8_t DIORedundancyConstatnt);
void trickle_increment_counter(void);
void delay_dao(void);
void dao_ack_received(void);

#ifdef __cplusplus
}
#endif
