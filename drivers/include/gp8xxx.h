/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_gp8xxx GP8xxx Guestgood GP8xxx I2C DACs
 * @ingroup     drivers_actuators
 * @ingroup     drivers_saul
 * @brief       Driver for the Guestgood GP8xxx I2C DACs
 *
 * The Guestgood GP8xxx I2C DACs are a series of digital-to-analog voltage
 * (VDAC) and current (IDAC) converters. The driver supports a range of
 * converters, each with different number of channels and output ranges.
 *
 * The supported DACs are:
 *
 * * GP8503: 2-channel 12-bit VDAC with 0-2.5/VCC V output range
 * * GP8512: 1-channel 15-bit VDAC with 0-2.5/VCC V output range
 * * GP8211S: 1-channel 15-bit VDAC with 0-5/10 V output range
 * * GP8403: 2-channel 12-bit VDAC with 0-5/10 V output range
 * * GP8413: 2-channel 15-bit VDAC with 0-5/10 V output range
 * * GP8302: 1-channel 12-bit IDAC with 0-25 mA output range
 *
 * Storing the output settings into internal memory is not supported by this
 * driver, because the driver will always initialize to a known state.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "periph/i2c.h"

#include "gp8xxx_info.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compile time macros to enable/disable features
 *
 * VDAC or IDAC support is only enabled if a module is selected that supports
 * it. A minor code size increase is expected when both a VDAC and IDAC module
 * are selected.
 */
#if MODULE_GP8503 || MODULE_GP8512 || MODULE_GP8403 || MODULE_GP8413 || \
    MODULE_GP8211S
#  define GP8XXX_HAS_VDAC 1
#endif
#if MODULE_GP8302
#  define GP8XXX_HAS_IDAC 1
#endif

/**
 * @brief   Channel values
 */
typedef enum {
    GP8XXX_CHANNEL_0    = 0,    /**< Channel 0 */
    GP8XXX_CHANNEL_1    = 1,    /**< Channel 1 */
    GP8XXX_CHANNEL_ALL  = 2     /**< All channels */
} gp8xxx_channel_t;

/**
 * @brief   Output range values
 */
typedef enum {
    GP8XXX_OUTPUT_RANGE_2_5V    = 2500,     /**< 0-2.5 V output range */
    GP8XXX_OUTPUT_RANGE_VCC     = 3300,     /**< 0-VCC V output range (VCC fixed to 3V3) */
    GP8XXX_OUTPUT_RANGE_5V      = 5000,     /**< 0-5 V output range */
    GP8XXX_OUTPUT_RANGE_10V     = 10000,    /**< 0-10 V output range */
    GP8XXX_OUTPUT_RANGE_25_MA   = 25000,    /**< 0-25 mA output range */
} gp8xxx_output_range_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C bus the converter is connected to */
    uint8_t address;                /**< converter address */
#if GP8XXX_HAS_VDAC || DOXYGEN
    gp8xxx_output_range_t range;    /**< default output range */
#endif /* GP8XXX_HAS_VDAC || DOXYGEN */
    const gp8xxx_info_t *info;      /**< device information */
} gp8xxx_params_t;

/**
 * @brief   SAUL device initialization parameters
 */
typedef struct {
    char *name;                     /**< name of the device and channel */
    uint8_t dev;                    /**< device index */
    gp8xxx_channel_t channel;       /**< channel of the device */
} gp8xxx_saul_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    gp8xxx_params_t params;         /**< device parameters */
    gp8xxx_output_range_t range;    /**< current output range */
#if GP8XXX_HAS_IDAC || DOXYGEN
    uint16_t calibration_4ma;       /**< calibration value for 4 mA */
    uint16_t calibration_20ma;      /**< calibration value for 20 mA */
#endif /* GP8XXX_HAS_IDAC || DOXYGEN */
} gp8xxx_t;

/**
 * @brief   SAUL device descriptor for the driver
 *
 * A SAUL registration needs a reference to the device descriptor AND the
 * channel it represents. This structure combines both.
 */
typedef struct {
    gp8xxx_t *dev;                  /**< device descriptor */
    gp8xxx_channel_t channel;       /**< channel of the device */
} gp8xxx_saul_t;

/**
 * @brief   Initialize the given device
 *
 * The driver will initialize will initialize all channels to a known state.
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 *
 * @retval                  0 on success
 * @retval                  -ENODEV if no device is found
 */
int gp8xxx_init(gp8xxx_t *dev, const gp8xxx_params_t *params);

/**
 * @brief   Set the DAC value
 *
 * @param[in] dev           Device descriptor of the driver
 * @param[in] channel       Channel to set
 * @param[in] value         Value to set (between 0 and maximum resolution)
 *
 * @retval                  0 on success
 * @retval                  -ERANGE if value is out of range
 * @retval                  -EIO on I2C error
 */
int gp8xxx_set_dac(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t value);

#if GP8XXX_HAS_VDAC || DOXYGEN
/**
 * @brief   Set the output range
 *
 * @param[in] dev           Device descriptor of the driver
 * @param[in] value         Output range to set
 *
 * @retval                  0 on success
 * @retval                  -EIO on I2C error
 */
int gp8xxx_set_voltage_range(gp8xxx_t *dev, gp8xxx_output_range_t value);

/**
 * @brief   Set the voltage
 *
 * @param[in] dev           Device descriptor of the driver
 * @param[in] channel       Channel to set
 * @param[in] voltage       Voltage to set (in mV)
 *
 * @retval                  0 on success
 * @retval                  -ERANGE if value is out of range
 * @retval                  -EIO on I2C error
 */
int gp8xxx_set_voltage(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t voltage);
#endif /* GP8XXX_HAS_VDAC || DOXYGEN */

#if GP8XXX_HAS_IDAC || DOXYGEN
/**
 * @brief   Set the current calibration values
 *
 * When both calibration values are set, the driver can more precisely set the
 * current between 4 mA and 20 mA. If both values are set to 0, the driver will
 * default to a linear interpolation between 4 mA and 20 mA.
 *
 * The value of @p calibration_4ma must be smaller than @p calibration_20ma
 * and both values must be between 0 and the maximum resolution of the DAC.
 *
 * @param[in] dev                   Device descriptor of the driver
 * @param[in] calibration_4ma       ADC value for 4 mA
 * @param[in] calibration_20ma      ADC value for 20 mA
 */
void gp8xxx_set_current_calibration(gp8xxx_t *dev, uint16_t calibration_4ma,
                                    uint16_t calibration_20ma);

/**
 * @brief   Set the current
 *
 * @param[in] dev           Device descriptor of the driver
 * @param[in] channel       Channel to set
 * @param[in] current       Current to set (in uA)
 *
 * @retval                  0 on success
 * @retval                  -ERANGE if value is out of range
 * @retval                  -EIO on I2C error
 */
int gp8xxx_set_current(gp8xxx_t *dev, gp8xxx_channel_t channel, uint16_t current);
#endif /* GP8XXX_HAS_IDAC || DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */
