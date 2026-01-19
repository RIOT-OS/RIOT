/*
 * Copyright (C) 2023 LIG
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lis2de12 lis2de12
 * @ingroup     drivers_sensors
 * @brief       driver for the lis2de12 accelerometer
 *
 * @{
 *
 * @file
 *
 * @author      Léo <leo.cordier@etu.esisar.grenoble-inp.fr>
 */

#ifndef LIS2DE12_H
#define LIS2DE12_H

/* Add header includes here */
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Available scale values
 */
typedef enum {
    LIS2DE12_SCALE_2G  = 0x0,       /**< +- 2g */
    LIS2DE12_SCALE_4G  = 0x1,       /**< +- 4g */
    LIS2DE12_SCALE_8G  = 0x2,       /**< +- 8g */
    LIS2DE12_SCALE_16G = 0x3,       /**< +- 16g */
} lis2de12_scale_t;

/**
 * @brief   Available sampling rates
 *
 */
typedef enum {
    LIS2DE12_RATE_1HZ   = 0x1,      /**< sample with 1Hz @ all resolutions */
    LIS2DE12_RATE_10HZ  = 0x2,      /**< sample with 10Hz @ all resolutions */
    LIS2DE12_RATE_25HZ  = 0x3,      /**< sample with 25Hz @ all resolutions */
    LIS2DE12_RATE_50HZ  = 0x4,      /**< sample with 50Hz @ all resolutions */
    LIS2DE12_RATE_100HZ = 0x5,      /**< sample with 100Hz @ all resolutions */
    LIS2DE12_RATE_200HZ = 0x6,      /**< sample with 200Hz @ all resolutions */
    LIS2DE12_RATE_400HZ = 0x7,      /**< sample with 400Hz @ all resolutions */
    LIS2DE12_RATE_1620HZ = 0x8,     /**< sample with 1620HZ @ 8-bit */
    LIS2DE12_RATE_VERYHIGH = 0x9,   /**< sample with 1344Hz @ High resolution or \
                                        5376Hz @ 8-bit */
} lis2de12_rate_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    /* add initialization params here */
    i2c_t i2c;                      /**< I2C bus the device is connected to */
    uint8_t addr;                   /**< device address on the I2C bus */
} lis2de12_params_t;

/**
 * @brief   LIS2DE12 FIFO data struct
 */
typedef union {
    struct {
        int16_t x;                      /**< X data in mili-g */
        int16_t y;                      /**< Y data in mili-g */
        int16_t z;                      /**< Z data in mili-g */
    } axis;                             /**< named axis access */
    int16_t data[3];                    /**< x, y, z data in mili-g */
} lis2de12_fifo_data_t;

/**
 * @brief   LIS2DE12 FIFO modes
 */
typedef enum {
    LIS2DE12_FIFO_MODE_BYPASS = 0,      /**< default mode, FIFO is bypassed */
    LIS2DE12_FIFO_MODE_FIFOMODE,        /**< normal FIFO mode, stops if FIFO is full */
    LIS2DE12_FIFO_MODE_STREAM,          /**< Stream mode, oldest values get overwritten */
    LIS2DE12_FIFO_MODE_STREAMtoFIFO,    /**< Stream mode and on interrupt jumps to FIFO mode */
} lis2de12_fifo_mode_t;

/**
 * @brief   LIS2DE12 FIFO config values
 */
typedef struct {
    lis2de12_fifo_mode_t FIFO_mode; /**< set FIFO mode */
    uint8_t FIFO_watermark:5;       /**< set the FIFO watermark level */
    bool FIFO_set_INT2;             /**< sets the FIFO interrupt to INT2, otherwise INT1 */
} lis2de12_fifo_t;


/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    /** Device initialization parameters */
    lis2de12_scale_t scale;         /**< sampling sensitivity used */
    lis2de12_rate_t rate;           /**< sampling rate used */

    const lis2de12_params_t *params;
} lis2de12_t;

/**
 * @brief   Status and error return codes
 */
enum {
    LIS2DE12_OK    =  0,            /**< everything was fine */
    LIS2DE12_NOBUS = -1,            /**< bus interface error */
    LIS2DE12_NODEV = -2,            /**< unable to talk to device */
    LIS2DE12_NOINT = -3,            /**< wrong interrupt line (has to be LIS2DE12_INT1
                                        or LIS2DE12_INT2) */
    LIS2DE12_NODATA= -4,            /**< no data available */
};

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int lis2de12_init(lis2de12_t *dev, const lis2de12_params_t *params);

/**
 * @brief   Read acceleration data from the given device
 *
 * @param[in]  dev      device descriptor
 * @param[out] data     acceleration data in mili-g
 *
 * @return  LIS2DE12_OK on success
 * @return  LIS2DE12_NOBUS on bus error
 */
int lis2de12_read(const lis2de12_t *dev, lis2de12_fifo_data_t *data);


int lis2de12_clear_data(const lis2de12_t *dev);

int lis2de12_set_datarate(const lis2de12_t *dev, lis2de12_rate_t rate);
uint16_t lis2de12_get_datarate(const lis2de12_t *dev);

int lis2de12_set_scale(lis2de12_t *dev, lis2de12_scale_t scale);
lis2de12_scale_t lis2de12_get_scale(lis2de12_t *dev);

int lis2de12_set_fifo(const lis2de12_t *dev, const lis2de12_fifo_t *config);

/**
 * @brief   Read temperature data from the given device
 *
 * @note    The temperature sensor is not calibrated.
 *          Temperature values are only relative to a device specific
 *          reference.
 *
 * @param[in]  dev      device descriptor
 * @param[out] temp     temperature data in centi-°C
 *
 * @return  LIS2DE12_OK on success
 * @return  LIS2DE12_NOBUS on bus error
 */
int lis2de12_read_temperature(const lis2de12_t *dev, int16_t *temp);

#ifdef __cplusplus
}
#endif

#endif /* LIS2DE12_H */
/** @} */
