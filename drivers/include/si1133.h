/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_si1133 Si1133 UV Index/Ambient Light Sensor with I2C
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Device driver for the Si1133 sensor
 *
 * The Si1133 is a UV Index Sensor and Ambient Light Sensor with I2C digital
 * interface and programmable-event interrupt output.
 *
 * The I2C protocol implemented in this driver is most similar in registers and
 * commands to the Si115x family, like the SI1153, however the Si1133 supports
 * UV index while the Si115x doesn't.
 *
 * This driver provides @ref drivers_saul capabilities as well.
 * @{
 *
 * @file
 * @brief       Device driver interface for the Si1133 sensor
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#ifndef SI1133_H
#define SI1133_H

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver error return codes
 */
typedef enum {
    SI1133_OK           = 0,    /**< No error. */
    SI1133_ERR_PARAMS   = -1,   /**< Invalid parameters. */
    SI1133_ERR_I2C      = -2,   /**< I2C communication error. */
    SI1133_ERR_LOGIC    = -3,   /**< Device communication logic error. */
    SI1133_ERR_NODEV    = -4,   /**< No SI1133 device detected. */
    SI1133_ERR_OVERFLOW = -5,   /**< ADC overflow when sampling. */
} si1133_ret_code_t;

/**
 * @brief Sensor (photodiode combination) in the Si1133 package.
 */
typedef enum {
    SI1133_SENS_SMALL_IR    = 1u << 0,
    SI1133_SENS_MEDIUM_IR   = 1u << 1,
    SI1133_SENS_LARGE_IR    = 1u << 2,
    SI1133_SENS_WHITE       = 1u << 3,
    SI1133_SENS_LARGE_WHITE = 1u << 4,
    SI1133_SENS_UV          = 1u << 5,
    SI1133_SENS_DEEP_UV     = 1u << 6,
} si1133_sensor_t;

/**
 * @brief Channel configuration the Si1133 sensor.
 *
 * The sensor sampling in the Si1133 is done via Analog to Digital "channels"
 * that read from a given sensor (photodiode combination) and output a numeric
 * value. The A/D process has some configuration parameters that affect the
 * acquisition time, the power consumption and the quality of the result.
 *
 * The A/D time is controlled by the internal 21 MHz clock. The sampling
 * duration time is:
 *
 *   (1 << decimation) * (1 << hw_gain) * 512 / 21000000 s
 *
 * where @p decimation is a number between 0 and 3 and @p hw_gain" is between 0
 * and 11. The shortest A/D sampling time is therefore 24.4 us while the longest
 * is 400 ms. However, each sample is performed twice internally to cancel ADC
 * offset and there are processing and sampling start times specified in the
 * datasheet that increase the total sampling time. Increasing the sampling time
 * doesn't make the output value be larger.
 *
 * The @p hw_gain and @p decimation parameters are configured from a single
 * @p sample_time_log parameter in this struct, between 0 and 14, preferring the
 * "normal" decimation when possible.
 *
 * An additional software sampling and averaging is possible by selecting a
 * "sw_gain" value between 0 and 7. This will cause each A/D measurement to be
 * repeated for a total of (1 << sw_gain) and accumulated in software in the
 * 24-bit output. The output value will be affected by the sw_gain since it is
 * a sum of samples and not an average.
 */
typedef struct {
    uint8_t sample_time_log;    /**< Log2 of sampling time, 0 to 14. */
    uint8_t sw_gain;            /**< Software gain, 0 to 7. */
    si1133_sensor_t sensor;     /**< Sensor to sample. */
} si1133_channel_t;

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C bus the sensor is connected to */
    /**
     * @brief sensor address.
     * Note: it is possible to change the a sensor's I2C address after it has
     * been initialized to have multiple sensor on the same bus, but this is not
     * supported by this driver. However, two different addresses can be
     * selected by hardware.
     */
    uint8_t address;
} si1133_params_t;

/**
 * @brief Device descriptor for the Si1133 sensor
 */
typedef struct {
    /* Initialization parameters */
    i2c_t i2c_dev;                  /**< I2C bus the sensor is connected to */
    uint8_t address;                /**< sensor address */
    /* Internal members */
    uint8_t cmd_counter;            /**< Si1133 command counter */
    uint8_t num_channels;           /**< Number of configured channels. */
    si1133_channel_t channel[6];    /**< Channel configuration. */
} si1133_t;

/**
 * @brief Initialize the given Si1133 device
 *
 * @param[out] dev          Initialized device descriptor of Si1133 device
 * @param[in]  params       Initialization parameters
 *
 * @return                  A si1133_ret_code_t error or status code.
 */
si1133_ret_code_t si1133_init(si1133_t *dev, const si1133_params_t *params);

/**
 * @brief Configure the capture channels.
 *
 * The Si1133 has up to 6 "channels" that can be configured to capture from the
 * different sensors (photodiode combinations). See @ref si1133_channel_t for
 * a description of the channel configuration.
 *
 * @param[in] dev           Device descriptor of Si1133 device to read from
 * @param[in] channels      Array of @p num_channels channel configuration.
 * @param[in] num_channels  Number of configured channel passed in @p channels.
 *
 * @return                  A si1133_ret_code_t error or status code.
 */
si1133_ret_code_t si1133_configure_channels(si1133_t *dev,
                                            const si1133_channel_t *channels,
                                            uint32_t num_channels);

/**
 * @brief Convenience function to configure all capture channels.
 *
 * This function is a convenience function to configure one channel per selected
 * sensor in the @p sensor_mask, up to the maximum number of channels, setting
 * all channels to force-mode only with the same parameters. This is equivalent
 * to a call to @ref si1133_configure_channels with as many channels as bits
 * set in the @p sensor_mask.
 *
 * The channels are configured in increasing order of the @ref si1133_sensor_t
 * values.
 *
 * @param[in] dev              Device descriptor of Si1133 device to read from
 * @param[in] sensor_mask      Combination of up to 6 si1133_sensor_t values.
 * @param[in] sample_time_log  Log2 of sampling time, 0 to 14. See @ref
 *                             si1133_channel_t for details.
 * @param[in] sw_gain          Software gain, 0 to 7. See @ref si1133_channel_t
 *                             for details.
 */
si1133_ret_code_t si1133_easy_configure(si1133_t *dev,
                                        si1133_sensor_t sensor_mask,
                                        uint8_t sample_time_log,
                                        uint8_t sw_gain);

/**
 * @brief Perform a one-time blocking sample of the configured channels.
 *
 * Forces a one-time blocking sample of the sensors configured in the channels
 * and returns the read values as signed 24-bit integers, sign extended to
 * 32-bits. The number of channels sampled and configured sensor is set by the
 * last call to @ref si1133_configure_channels, however after sampling all of
 * them only up to the first @p num_channels values will be returned by this
 * function.
 *
 * In case of ADC overflow, for example because there's too much light for the
 * configured sensors the overflown sensor will read @p 0x7fffff and the
 * function will return @ref SI1133_ERR_OVERFLOW. In case of overflow, try
 * configuring a smaller sensor, for example @p SI1133_SENS_MEDIUM_IR instead of
 * @p SI1133_SENS_LARGE_IR, or reduce the @p sw_gain for the given sensor.
 *
 * @param[in] dev           Device descriptor of Si1133 device to read from
 * @param[out] values       Pointer to the output value buffer.
 * @param[in] num_channels  Maximum number of channel values to return.
 *
 * @return                  A si1133_ret_code_t error or status code.
 */
si1133_ret_code_t si1133_capture_sensors(si1133_t *dev, int32_t *values,
                                         uint32_t num_channels);

#ifdef __cplusplus
}
#endif

#endif /* SI1133_H */
/** @} */
