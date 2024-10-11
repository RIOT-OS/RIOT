/*
 * Copyright (C) 2020 Puhang Ding
 *               2020 Jan Schlichter
 *               2020 Nishchay Agrawal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_scd30
 * @{
 * @file
 * @brief       Internal constants, configuration commands for SCD30 sensor
 *
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 */

#ifndef SCD30_INTERNAL_H
#define SCD30_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SCD30 CRC Function and start value for CRC
 * @{
 */
#define SCD30_CRC_FUNC                  0x31    /**< CRC Function for CRC-8
                          calculation and verification */
#define SCD30_CRC_START_VAL             0xFF    /**< Start value for CRC-8
                          calculation */
/** @} */

/**
 * @name    SCD30 Internal Constants
 * @{
 */
#define SCD30_DEF_PRESSURE              0x03f5  /**< Ambient Pressure:
                          1013.25 mBar */

#define SCD30_READ_WRITE_SLEEP_US       (4 * US_PER_MS)
#define SCD30_RESET_SLEEP_US            (25 * US_PER_MS)
#define SCD30_DATA_RDY_TIMEOUT          (1 * US_PER_SEC)

#define SCD30_MIN_INTERVAL              2
#define SCD30_MAX_INTERVAL              1800

#define SCD30_MIN_PRESSURE_COMP         700
#define SCD30_MAX_PRESSURE_COMP         1400
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SCD30_INTERNAL_H */
