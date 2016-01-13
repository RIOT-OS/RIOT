/*
 * Copyright 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ltc4150 LTC4150 Coulomb Counter
 * @ingroup     drivers_sensors
 * @brief       Device driver for LTC4150 coulomb counters
 *
 * @deprecated  This driver should be ported to the peripheral driver interface
 *              (@ref drivers_periph)
 *
 * @{
 *
 * @file
 * @brief       Driver interface for the LTC4150 driver
 *
 * @author      Heiko Will <heiko.will@fu-berlin.de>
 */


#ifndef LTC4150_H
#define LTC4150_H

#include "ltc4150_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the counter
 */
void ltc4150_init(void);

/**
 * @brief   Start a measurement
 */
void ltc4150_start(void);

/**
 * @brief   End the ongoing measurement
 */
void ltc4150_stop(void);

/**
 * @brief   Get the current electrical current
 *
 * @return  electrical current in mA
 */
double ltc4150_get_current_mA(void);

/**
 * @brief   Get the total power used since @p ltc4150_start was called
 *
 * @return  power used in mAh
 */
double ltc4150_get_total_mAh(void);

/**
 * @brief   Get the total energy used since @p ltc4150_start was called
 *
 * @return  energy used in Joule
 */
double ltc4150_get_total_Joule(void);

/**
 * @brief   Get the average electrical current sine @p ltc4150_start was called
 *
 * @return  average current in mA
 */
double ltc4150_get_avg_mA(void);

/**
 * @brief   Get the time the current measurement is going on
 *
 * @return  time
 */
int ltc4150_get_interval(void);

/**
 * @brief   Get the number of samples taken
 *
 * @return  number of samples in the current interval
 */
long ltc4150_get_intcount(void);

#ifdef __cplusplus
}
#endif

#endif /* LTC4150_H */
/** @} */
