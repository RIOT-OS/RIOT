/**
 * 6lowpan border router prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowborder.h
 * @brief   header for 6lowpan border router
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

/* 6LoWPAN Border Router header file */

#ifndef _SIXLOWPAN_BORDER_H
#define _SIXLOWPAN_BORDER_H

#include <stdint.h>

#include "mutex.h"
#include "transceiver.h"

#include "ip.h"
#include "semaphore.h"

extern ipv6_addr_t *abr_addr;

kernel_pid_t border_get_serial_reader(void);

uint8_t *get_serial_out_buffer(int offset);
uint8_t *get_serial_in_buffer(int offset);

void border_process_lowpan(void);

#endif /* _SIXLOWPAN_BORDER_H*/
