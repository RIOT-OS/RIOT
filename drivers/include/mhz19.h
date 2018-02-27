/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mhz19 MH-Z19 CO2 sensor
 * @ingroup     drivers_sensors
 *
 * @brief       MH-Z19 CO2 sensor driver
 *
 * ## Description
 *
 * The MH-Z19 is a C02 sensor. Measurements are provided in ppm over UART and
 * PWM. Only the UART interface is implemented in this driver. PPM range from
 * 0 (theoretically) to 2000 or 5000 depending on the sensor settings.
 *
 * @note The sensor requires considerable time before accurate measurements are
 * provided
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the mh-z19 CO2 sensor driver.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MHZ19_H
#define MHZ19_H

#include "periph/uart.h"
#include "saul.h"
#include "mhz19_internals.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status and error return codes
 */
enum {
    MHZ19_OK            =  0,       /**< everything was fine */
    MHZ19_ERR_UART      = -1,       /**< error initializing the UART bus */
    MHZ19_ERR_TIMEOUT   = -2,       /**< UART timeout on retrieving sensor data */
    MHZ19_ERR_CHECKSUM  = -3,       /**< Checksum failure on received data */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uart_t uart;        /**< UART device that sensor is connected to */
} mhz19_params_t;

/**
 * @brief   Device descriptor for a MH-Z19 device
 */
typedef struct {
    const mhz19_params_t *params;   /**< Device parameters */
    mutex_t mutex;                  /**< protect against simulaneous access */
    mutex_t sync;                   /**< Transfer complete or timeout sync */
    uint8_t idx;                    /**< rx buffer index */
    uint8_t rxmem[MHZ19_BUF_SIZE];  /**< rx buffer */
} mhz19_t;

/**
 * @brief   Export SAUL endpoint
 */
extern const saul_driver_t mhz19_ppm_saul_driver;

/**
 * @brief   Initialize a MH-Z19 device
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       MH-Z19 initialization struct
 *
 * @return                  MHZ19_OK
 * @return                  MHZ19_ERR_UART
 */
int mhz19_init(mhz19_t *dev, const mhz19_params_t *params);

/**
 * @brief   Get measured CO2 ppm value
 *
 * @param[in] dev           device descriptor
 * @param[out] ppm          int16_t buffer where CO2 measurement will be written to in ppm
 *
 * @return                  MHZ19_OK
 * @return                  MHZ19_ERR_TIMEOUT
 * @return                  MHZ19_ERR_CHECKSUM
 */
int mhz19_get_ppm(mhz19_t *dev, int16_t *ppm);

#ifdef __cplusplus
}
#endif

#endif /* MHZ19_H */
/** @} */
