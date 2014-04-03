/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
 * and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @ingroup     drivers_cc110x_ng
 * @{
 *
 * @file        cc110x-arch.h
 * @brief       CC1100 architecture dependent functions
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 */

#ifndef __CC1100_ARCH_H
#define __CC1100_ARCH_H

#include <stdint.h>

uint8_t cc110x_txrx(uint8_t c);

void cc110x_gdo0_enable(void);
void cc110x_gdo0_disable(void);
void cc110x_gdo2_enable(void);
void cc110x_gdo2_disable(void);
void cc110x_init_interrupts(void);

void cc110x_before_send(void);
void cc110x_after_send(void);

/** @} */
#endif /* __CC1100_ARCH_H */
