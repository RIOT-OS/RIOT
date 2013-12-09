/******************************************************************************
 * Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

#ifndef __LTC4150_ARCH_H
#define __LTC4150_ARCH_H

/**
 * @defgroup    ltc4150 LTC4150 Coulomb Counter
 * @ingroup     drivers
 * @{
 */

/**
 * @file
 * @brief       LTC4150 Coulomb Counter
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Heiko Will
 * @version     $Revision: 1203 $
 *
 * @note        $Id: ltc4150_arch.h 1203 2009-07-07 10:23:18Z baar $
 */

#define _GFH (double)32.631375
#define _R_SENSE (double)0.330
#define SUPPLY_VOLTAGE  (5)

/** board specific ltc4150 interrupt disable */
void ltc4150_disable_int(void);
/** board specific ltc4150 interrupt enable */
void ltc4150_enable_int(void);
/** board specific synchronization of ltc4150 */
void ltc4150_sync_blocking(void);
/** board specific ltc4150 initialization */
void ltc4150_arch_init(void);

/**
 * ltc4150 interrupt handler,
 * shall be called on ltc4150 interrupt,
 * implemented in driver
 */
void ltc4150_interrupt(void);

/** * @} */
#endif /* __LTC4150_ARCH_H */
