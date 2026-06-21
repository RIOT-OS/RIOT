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

#ifdef __cplusplus
extern "C" {
#endif

#define LIS331DLH_ADDR_DEFAULT   (0x18)

typedef struct {
    i2c_t i2c;
    uint8_t addr;
} lis331dlh_params_t;

typedef struct {
    const lis331dlh_params_t *params;
} lis331dlh_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} lis331dlh_data_t;

enum {
    LIS331DLH_OK    =  0,
    LIS331DLH_ERROR = -1,
    LIS331DLH_NODEV = -2,
};

int lis331dlh_init(lis331dlh_t *dev, const lis331dlh_params_t *params);

int lis331dlh_read(const lis331dlh_t *dev, lis331dlh_data_t *data);

#ifdef __cplusplus
}
#endif

/** @} */
