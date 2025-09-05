/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_tsl4531x
 * @brief       Internal addresses, registers, constants for the TSL4531x sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the TSL4531x sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Daniel Petry <danielpetry@cantab.net>
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * Derived from the internals.h file for the tsl2561 driver.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name TSL4531x internal registers
 * @{
 */
#define TSL4531X_CONTROL_REG             (0x0)      /**< sets power mode */
#define TSL4531X_CONFIGURATION_REG       (0x01)     /**< sets power mode */
#define TSL4531X_ALSDATA1_REG            (0x04)     /**< contains DATALOW */
#define TSL4531X_ALSDATA2_REG            (0x05)     /**< contains DATAHIGH */
#define TSL4531X_ID_REG                  (0x0A)     /**< contains part no above*/
/** @} */

/**
 * @name TSL4531x power modes
 * @{
 */
#define TSL4531X_MODE_POWER_DOWN         (0x00)
#define TSL4531X_MODE_RESERVED           (0x01)
#define TSL4531X_MODE_SINGLE_ADC_CYCLE   (0x02)     /**< Runs a single ADC cycle then
                                                       returns to power down. */
#define TSL4531X_MODE_NORMAL             (0x03)

/* PowerSave saves some power in full power mode. PowerSave skip turns this off.
 * Currently PowerSave skip is hard-coded to be on for simplicity, as it's just
 * an intermediate between normal mode and low-power mode. */
#define TSL4531X_PSAVESKIP_OFF           (0)
#define TSL4531X_PSAVESKIP_ON            (1)
/** @} */

/**
 * @name TSL4531x macros
 * @{
 */
/* Assemble the Command register */
#define TSL4531X_COMMAND(addr)   ((1 << 7) | (addr))

/* Assemble the Configuration register */
#define TSL4531X_CONFIG(psaveskip, tcntrl)  (((!!(psaveskip)) << 3) | (tcntrl))

/* Assemble the Control register */
#define TSL4531X_CONTROL(mode)  (mode)

#define TSL4531X_GET_PARTNO(id_reg)  ((id_reg) >> 4)

/* Data multiplier according to integration time.
 *
 * From the manual:
 *      MULTIPLIER = 1 for TCNTRL = 00 (Tint = 400 ms)
 *      MULTIPLIER = 2 for TCNTRL = 01 (Tint = 200 ms)
 *      MULTIPLIER = 4 for TCNTRL = 10 (Tint = 100 ms)
 */
#define MULTIPLY_DATA(data_raw, integration_time) ((data_raw) << (integration_time))

/* This allows either full power mode or power down mode. */
#define TSL4531X_POWER_MODE(mode) ((!mode) * (0x03))

/* The full integration time is a sum of:
 * 1. A worst-case time for each of the integration times, which according to
 *    the datasheet is 5% more than the nominal time
 * 2. 60, 30 or 15 ms, according to the integration time, if PowerSave is on;
 *    which is the power down period in between integration cycles in this mode.
 *    Note that in the current implementation, the "PowerSave skip" option is
 *    hard-coded to be on, as PowerSave only gives slightly less power
 *    consumption than normal mode.
 */
#define TSL4531X_GET_INTEGRATION_TIME_USEC(int_time_setting, psaveskip) \
    (uint32_t)1000 * (((0x04 >> int_time_setting) * 105) + ((!psaveskip) * (60 >> int_time_setting)))
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
