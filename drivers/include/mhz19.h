/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MHZ19_H
#define MHZ19_H

/**
 * @defgroup    drivers_mhz19 MH-Z19 CO2 sensor
 * @ingroup     drivers_sensors
 *
 * @brief       MH-Z19 CO2 sensor driver
 *
 * ## Description
 *
 * The MH-Z19 is a CO2 sensor. Measurements are provided in parts per million
 * (ppm) over UART and PWM. The ppm value ranges from 0 (theoretically) to
 * 2000 or 5000, depending on the sensor settings.
 *
 * In UART mode, additional commands are available to configure the sensor.
 *
 * Note that the sensor requires considerable time before accurate measurements
 * are provided.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MH-Z19 CO2 sensor driver.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Christian Manal <manal@uni-bremen.de>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include <stdbool.h>

#include "saul.h"

#ifdef MODULE_MHZ19_UART
#include "periph/uart.h"
#include "mhz19_internals.h"
#include "mutex.h"
#endif /* MODULE_MHZ19_UART */

#ifdef MODULE_MHZ19_PWM
#include "periph/gpio.h"
#endif /* MODULE_MHZ19_PWM */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Status and error return codes
 */
enum {
    MHZ19_OK            =  0,       /**< everything was fine */
    MHZ19_ERR_INIT      = -1,       /**< error initializing the device */
    MHZ19_ERR_TIMEOUT   = -2,       /**< timeout on retrieving sensor data */
    MHZ19_ERR_CHECKSUM  = -3,       /**< checksum failure on received data */
};

#ifdef MODULE_MHZ19_UART
/**
 * @brief   Device initialization parameters
 */
typedef struct {
    uart_t uart;                /**< UART device that sensor is connected to */
    bool auto_calibration;      /** enable or disable auto calibration */
} mhz19_params_t;

/**
 * @brief   Device descriptor for a MH-Z19 device
 */
typedef struct {
    const mhz19_params_t *params;   /**< device parameters */
    mutex_t mutex;                  /**< protect against simulaneous access */
    mutex_t sync;                   /**< transfer complete or timeout sync */
    uint8_t idx;                    /**< rx buffer index */
    uint8_t rxmem[MHZ19_BUF_SIZE];  /**< rx buffer */
} mhz19_t;
#endif /* MODULE_MHZ19_UART */

#ifdef MODULE_MHZ19_PWM
/**
 * @brief  Device initialization parameters
 */
typedef struct {
    gpio_t pin;     /**< Pin the sensor is connected to */
} mhz19_params_t;

/**
 * @brief  Device descriptor for a mhz19 driver
 */
typedef struct {
    gpio_t pin;     /**< Pin the sensor is connected to */
} mhz19_t;
#endif /* MODULE_MHZ19_PWM */

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
 * @return                  MHZ19_ERR_INIT
 */
int mhz19_init(mhz19_t *dev, const mhz19_params_t *params);

/**
 * @brief   Get measured CO2 ppm value
 *
 * @param[in] dev           device descriptor
 * @param[out] ppm          int16_t buffer where CO2 measurement will be
 *                          written to, in ppm
 *
 * @return                  MHZ19_OK
 * @return                  MHZ19_ERR_TIMEOUT
 * @return                  MHZ19_ERR_CHECKSUM
 */
int mhz19_get_ppm(mhz19_t *dev, int16_t *ppm);

#ifdef MODULE_MHZ19_UART
/**
 * @brief   Enable or disable auto base calibration
 *
 * @param[in] dev           device descriptor
 * @param[in] enable        true to enable, false to disable
 */
void mhz19_set_auto_calibration(mhz19_t *dev, bool enable);

/**
 * @brief   Calibrate zero point
 *
 * @param[in] dev           device descriptor
 */
void mhz19_calibrate_zero_point(mhz19_t *dev);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MHZ19_H */
