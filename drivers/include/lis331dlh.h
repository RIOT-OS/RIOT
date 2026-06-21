/*
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_lis331dlh LIS331DLH accelerometer
 * @ingroup     drivers_sensors
 * @brief       Device driver for the ST LIS331DLH 3-axis accelerometer
 * @{
 *
 * @file
 * @brief       Interface definition for the LIS331DLH accelerometer
 */

#include <stdint.h>

#include "periph/i2c.h"
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LIS331DLH_ADDR_DEFAULT   (0x18)

typedef enum {
    LIS331DLH_SCALE_2G = 0x00,
    LIS331DLH_SCALE_4G = 0x10,
    LIS331DLH_SCALE_8G = 0x30,
} lis331dlh_scale_t;

typedef enum {
    LIS331DLH_RATE_50HZ   = 0x00,
    LIS331DLH_RATE_100HZ  = 0x08,
    LIS331DLH_RATE_400HZ  = 0x10,
    LIS331DLH_RATE_1000HZ = 0x18,
} lis331dlh_rate_t;

typedef struct {
    i2c_t i2c;
    uint8_t addr;
    lis331dlh_scale_t scale;
    lis331dlh_rate_t rate;
} lis331dlh_params_t;

typedef struct {
    const lis331dlh_params_t *params;
} lis331dlh_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lis331dlh_data_t;

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params);

int lis331dlh_read_xyz(const lis331dlh_t *dev, lis331dlh_data_t *data);

extern const saul_driver_t lis331dlh_saul_driver;

#ifdef __cplusplus
}
#endif

/** @} */
