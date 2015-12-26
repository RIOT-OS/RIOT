/*
 * Copyright 2008-2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ltc4150 LTC4150
 * @ingroup     drivers_sensors
 * @brief       Driver for the Linear Technology LTC4150 Coulomb Counter
 * @{
 *
 * @file
 * @brief       LTC4150 Coulomb Counter
 *
 * @author      Heiko Will <heiko.will@fu-berlin.de>
 */

#ifndef LTC4150_ARCH_H
#define LTC4150_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Constants used by the driver
 * @{
 */
#define _GFH (double)32.631375
#define _R_SENSE (double)0.330
#define SUPPLY_VOLTAGE  (5)
/** @} */

/**
 * @brief   Board specific ltc4150 interrupt disable
 **/
void ltc4150_disable_int(void);

/**
 * @brief   Board specific ltc4150 interrupt enable
 **/
void ltc4150_enable_int(void);

/**
 * @brief   Board specific synchronization of ltc4150
 **/
void ltc4150_sync_blocking(void);

/**
 * @brief   Board specific ltc4150 initialization
 **/
void ltc4150_arch_init(void);

/**
 * @brief   Ltc4150 interrupt handler
 *
 * This handler shall be called on ltc4150 interrupt, it is implemented in the
 * driver.
 */
void ltc4150_interrupt(void);

#ifdef __cplusplus
}
#endif

/** * @} */
#endif /* LTC4150_ARCH_H */
