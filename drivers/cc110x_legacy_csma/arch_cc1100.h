/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     LPC2387
 * @brief       CC1100 LPC2387 dependend functions
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1775 $
 *
 * @note        $Id: arch_cc1100.h 1775 2010-01-26 09:37:03Z hillebra $
 */

#ifndef ARCH_CC1100_H
#define ARCH_CC1100_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t cc110x_txrx(uint8_t c);

void cc110x_gdo0_enable(void);
void cc110x_gdo0_disable(void);
void cc110x_gdo2_enable(void);
void cc110x_gdo2_disable(void);
void cc110x_init_interrupts(void);

void cc110x_before_send(void);
void cc110x_after_send(void);

#ifdef __cplusplus
}
#endif

#endif /* ARCH_CC1100_H */
