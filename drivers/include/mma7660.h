/*
 * Copyright (C) 2016 University of California, Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MMA7660_H
#define MMA7660_H

/**
 * @defgroup    drivers_mma7660 MMA7660 Accelerometer
 * @ingroup     drivers_sensors
 * @brief       Driver for the Freescale MMA7660 3-Axis accelerometer.
 *              This driver only implements basic functionality.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the MMA7660 accelerometer driver.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 */

#include <stdint.h>
#include <stdbool.h>
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Return codes
 */
enum {
    MMA7660_OK,               /**< all ok */
    MMA7660_I2C_ERR,          /**< i2c bus initialization error */
    MMA7660_I2C_READ_ERR,     /**< i2c bus cannot be read */
    MMA7660_I2C_WRITE_ERR,    /**< i2c bus cannot be written */
    MMA7660_READ_ERR,         /**< error when reading counts */
};

/**
 * @brief   Parameters for an MMA7660 device
 */
typedef struct
{
    i2c_t i2c;                /**< the I2C handle */
    uint8_t addr;             /**< the device I2C address */
    uint8_t amsr;             /**< active mode sample rate */
    uint8_t awsr;             /**< auto wake sample rate */
    uint8_t filt;             /**< filter samples */
} mma7660_params_t;

/**
 * @brief   Device descriptor for an MMA7660 device
 */
typedef struct
{
    mma7660_params_t params;  /**< initialization parameters */
} mma7660_t;

/**
 * @brief   Data type for the result data
 */
typedef struct {
    int16_t x;                  /**< acceleration in X direction */
    int16_t y;                  /**< acceleration in Y direction */
    int16_t z;                  /**< acceleration in Z direction */
} mma7660_data_t;

/**
 * @name    MMA7660 constants
 * @{
 */
#define MODE_ACTIVE_SHIFT             (0U)
#define MODE_AUTOWAKE_SHIFT           (3U)
#define MODE_AUTOSLEEP_SHIFT          (4U)
#define MODE_PRESCALE_SHIFT           (5U)
#define MODE_INTERRUPT_DEFAULT        (0x40) /* Active low, push-pull */

#define MMA7660_INTSOURCE_NONE        (0x00)
#define MMA7660_INTSOURCE_FB          (0x01)
#define MMA7660_INTSOURCE_UDLR        (0x02)
#define MMA7660_INTSOURCE_TAP         (0x04)
#define MMA7660_INTSOURCE_AUTOSLEEP   (0x08)
#define MMA7660_INTSOURCE_MEASURE     (0x10)
#define MMA7660_INTSOURCE_SHX         (0x20)
#define MMA7660_INTSOURCE_SHY         (0x40)
#define MMA7660_INTSOURCE_SHZ         (0x80)

#define MMA7660_SR_AMPD               (0x00)
#define MMA7660_SR_AM64               (0x01)
#define MMA7660_SR_AM32               (0x02)
#define MMA7660_SR_AM16               (0x03)
#define MMA7660_SR_AM8                (0x04)
#define MMA7660_SR_AM4                (0x05)
#define MMA7660_SR_AM2                (0x06)
#define MMA7660_SR_AM1                (0x07)
#define MMA7660_SR_AW32               (0x00)
#define MMA7660_SR_AW16               (0x08)
#define MMA7660_SR_AW8                (0x10)
#define MMA7660_SR_AW1                (0x18)
#define MMA7660_PDET_X                (0x20)
#define MMA7660_PDET_Y                (0x40)
#define MMA7660_PDET_Z                (0x80)

#define MMA7660_ADDR                  (0x4C)

/* This is actually 46.9 but the sensor is not accurate enough
 * for that to matter
 */
#define MMA7660_MG_PER_COUNT          (47U)
/** @} */

/**
 * @brief   Initialize an MMA7660 device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        device configuration parameters
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_ERR on i2c bus initialization error
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_init(mma7660_t *dev, const mma7660_params_t *params);

/**
 * @brief   Set the mode register
 *
 * @param[in] dev           device descriptor
 * @param[in] active        0=sleep 1=active
 * @param[in] autowake      see datasheet
 * @param[in] autosleep     see datasheet
 * @param[in] prescale      main clock prescalar
 *
 * See page 17 of http://www.nxp.com/files/sensors/doc/data_sheet/MMA7660FC.pdf
 * for information about the parameters
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_set_mode(const mma7660_t *dev, uint8_t active,
                     uint8_t autowake, uint8_t autosleep, uint8_t prescale);

/**
 * @brief   Read the tilt register
 *
 * @param[in] dev           device descriptor
 * @param[out] res          tilt register contents
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_READ_ERR on i2c bus read error
 */
int mma7660_read_tilt(const mma7660_t *dev, uint8_t *res);

/**
 * @brief   Write the sleep count register
 *
 * @param[in] dev           device descriptor
 * @param[in] sleep         sleep count
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_write_sleep_count(const mma7660_t *dev, uint8_t sleep);

/**
 * @brief   Configure the interrupt sources
 *
 * @param[in] dev           device descriptor
 * @param[in] isource_flags interrupt source flags
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_config_interrupts(const mma7660_t *dev, uint8_t isource_flags);

/**
 * @brief   Configure the sample rate
 *
 * @param[in] dev           device descriptor
 * @param[in] amsr          active mode sample rate (pg 18 of DS)
 * @param[in] awsr          auto wake sample rate (pg 19 of DS)
 * @param[in] filt          filter samples (pg 19 of DS)
 *
 * See datasheet http://www.nxp.com/files/sensors/doc/data_sheet/MMA7660FC.pdf
 * for details about the parameters
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_config_samplerate(const mma7660_t *dev, uint8_t amsr, uint8_t awsr, uint8_t filt);

/**
 * @brief   Configure the tap detection
 *
 * @param[in] dev           device descriptor
 * @param[in] pdth          pulse detection
 * @param[in] enabled_axes  enabled axes
 *
 * See page 21 of http://www.nxp.com/files/sensors/doc/data_sheet/MMA7660FC.pdf
 * for details about the parameters
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_config_pdet(const mma7660_t *dev, uint8_t pdth, uint8_t enabled_axes);

/**
 * @brief   Configure the tap detection debounce count
 *
 * @param[in] dev           device descriptor
 * @param[in] pd            pulse debounce
 *
 * See page 21 of http://www.nxp.com/files/sensors/doc/data_sheet/MMA7660FC.pdf
 * for details about the debouncer
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_WRITE_ERR on i2c bus write error
 */
int mma7660_config_pd(const mma7660_t *dev, uint8_t pd) ;

/**
 * @brief   Read the acceleration counts converted to mG
 *
 * @param[in] dev           device descriptor
 * @param[out] data         the acceleration data
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_READ_ERR on i2c bus read error
 */
int mma7660_read(const mma7660_t *dev, mma7660_data_t *data);

/**
 * @brief   Read the acceleration counts (unconverted)
 *
 * @param[in] dev           device descriptor
 * @param[out] x            the X axis value
 * @param[out] y            the Y axis value
 * @param[out] z            the Z axis value
 *
 * See page 28 of http://www.nxp.com/files/sensors/doc/data_sheet/MMA7660FC.pdf
 * for conversion of acceleration counts to angles or G forces.
 *
 * @return                  MMA7660_OK on success
 * @return                 -MMA7660_I2C_READ_ERR on i2c bus read error
 * @return                 -MMA7660_READ_ERR on general read error
 */
int mma7660_read_counts(const mma7660_t *dev, int8_t *x, int8_t *y, int8_t *z);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MMA7660_H */
