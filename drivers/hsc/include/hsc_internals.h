/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hsc
 * @brief       Internal addresses, constants for the HSC sensor.
 * @{
 *
 * @file
 * @brief       Internal addresses, constants for the HSC sensor.
 *
 * @author      Quang Pham <phhr_quang@live.com>
 */

#ifndef HSC_INTERNALS_H
#define HSC_INTERNALS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    HSC I2C Packet Readout
 * @{
 */
#define HSC_PRESSURE_DATA_LENGTH    (2)      /**<
        Pressure is stored in the first 2 bytes of data */
#define HSC_FULL_DATA_LENGTH        (4)      /**<
        Pressure + temperature data is 4 bytes long */
#define HSC_STATUS_MASK             (0xc0)   /**<
        Bit mask for the status bits in the first byte */
#define HSC_PRESSURE_MASK           (0x3fff) /**<
        Bit mask for the pressure data */
#define HSC_TEMPERATURE_SHIFT       (5)      /**<
        Temperature data is left adjusted within the word */
/** @} */

/**
 * @name   Status and error return codes
 * @{
 */
enum {
    HSC_STATUS_OK           = 0x00,
    /**
     * stale data: data that has already been fetched since the last measurement
     * cycle, or data fetched before the first measurement has been completed.
     */
    HSC_STATUS_STALE_DATA   = 0x40,
    HSC_STATUS_COMMAND_MODE = 0x80,
    HSC_STATUS_DIAGNOSTIC   = 0xc0,
};
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HSC_INTERNALS_H */
