/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gps
 * @brief       Driver for NMEA 0183 GPS devices
 * @{
 *
 * @file
 * @brief       Interface definition for NMEA 0183 device driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GPS_SERIAL_H
#define GPS_SERIAL_H

#include <stdint.h>

#include "isrpipe.h"
#include "periph/uart.h"
#include "gps.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GPS_SERIAL_BUFSIZE
#define GPS_SERIAL_BUFSIZE  (512U)      /**< buffer for NMEA sentences */
#endif

/**
 * @brief gps_serial parameter struct
 */
typedef struct {
    uart_t uart;        /**< UART device the GPS module is connected to */
    uint32_t baudrate;  /**< baudrate of GPS module */
} gps_serial_params_t;

/**
 * @brief gps_serial context struct
 * @extends gps_t
 */
typedef struct {
    gps_t super;                    /**< parent struct */
    char buf[GPS_SERIAL_BUFSIZE];   /**< GPS input buffer */
    isrpipe_t isrpipe;              /**< isrpipe object used for serial IO */
} gps_serial_t;

/**
 * @brief   GPS serial initialization function
 *
 * @param[in]   dev     device struct to initialize
 * @param[in]   params  pointer to parameter struct
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int gps_serial_init(gps_serial_t *dev, const gps_serial_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* GPS_SERIAL_H */
