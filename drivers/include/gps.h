/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_gps GPS
 * @ingroup     drivers
 * @brief       API for accessing GPS modules
 * @{
 *
 * @file
 * @brief       Interface definition for GPS
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GPS_H
#define GPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gps_driver gps_driver_t; /**< gps_driver_t forward declaration */

/**
 * @brief base GPS context structure
 */
typedef struct {
    const gps_driver_t *driver;         /**< pointer to gps driver structure */
} gps_t;

/**
 * @brief enum holding possible GPS power saving modes
 */
typedef enum {
    GPS_OFF,                            /**< GPS hardware turned off */
    GPS_SLEEP,                          /**< GPS hardware in sleep mode */
    GPS_POWERSAVE,                      /**< GPS in low power mode */
    GPS_ON,                             /**< GPS active */
} gps_powermode_t;

/**
 * @brief Structure for GPS backend function pointers
 */
struct gps_driver {
    /** ptr to get_loc implementation */
    int(*get_loc)(gps_t *dev, char *lat, char *lon, uint32_t wait_fix);

    /** ptr to power implementation */
    int(*power)(gps_t *dev, gps_powermode_t mode);
};

/**
 * @brief   Get GPS location
 *
 * Gets the current location from GPS. Waits at most @p maxwait usec until a
 * location fix has been acquired.
 * Location will be stored in decimal degree (DD.DDDDDD) format as text.
 * Both @p and @p lon can be NULL. In that case, the corresponding value will
 * not be stored.
 *
 * @note If not NULL, both @p lat and @p lon must point to buffers holding the
 *       result (10 bytes)!
 *
 * @note allow for enough time to read out the fix. E.g., serial NMEA0183
 *       devices report every second.
 *
 * @param[in]   gpsdev  GPS device to operate on
 * @param[in]   lat     buffer for storing latitude
 * @param[in]   lon     buffer for storing lontitide
 * @param[in]   maxwait maximum wait time in usec
 *
 * @returns     0 on success
 * @returns     -ETIMEOUT on timeout
 * @returns     <0 on other errors
 */
static inline int gps_get_loc(gps_t *gpsdev, char *lat, char *lon, uint32_t maxwait)
{
    return gpsdev->driver->get_loc(gpsdev, lat, lon, maxwait);
}

/**
 * @brief Set power mode of GPS device
 *
 * @param[in]   gpsdev  GPS device to operate on
 * @param[in]   mode    low-power mode to set
 *
 * @returns     0 on success
 * @return      <0 on error
 */
static inline int gps_power(gps_t *gpsdev, gps_powermode_t mode)
{
    return gpsdev->driver->power(gpsdev, mode);
}

/**
 * @brief   parse raw GPS GLL sentence
 *
 * Used internally by backend drivers.
 *
 * @param[in]   buf buffer containing sentence
 * @param[in]   lat buffer to store latitude
 * @param[in]   lon buffer to store lontitude
 *
 * @returns 0 on success
 * @returns <0 on error
 */
int gps_parse_gll(const uint8_t *buf, char *lat, char *lon);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* GPS_H */
