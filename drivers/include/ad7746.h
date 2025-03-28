/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef AD7746_H
#define AD7746_H

/**
 * @defgroup   drivers_ad7746 AD7746 Capacitance-to-digital converter driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      I2C Capacitance-to-digital converter with temperature and voltage
 *             sensors. The devices has two main channels: capacitance channel
 *             and voltage / temperature channel.
 *
 *             The capacitance channel can measure from two different inputs
 *             (CIN1 and CIN2), selected using @ref ad7746_set_cap_ch_input.
 *
 *             The voltage / temperature channel can measure from five different
 *             sources: Voltage from VIN pins, internal VCC, internal
 *             temperature sensor and external temperature sensor
 *             (see datasheet for proper setup). The mode of this channel can be
 *             set using @ref ad7746_set_vt_ch_mode or will directly be set
 *             when trying to read from a specific source which is not the
 *             currently selected one. This mode change may lead to data not
 *             being available right away (in which case the reading function
 *             will return @ref AD7746_NODATA error code).
 *
 * @note       Constantly switching between modes in the voltage / temperature
 *             channel may lead to invalid data, for what it seems to be a
 *             device limitation.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief      AD7746 Capacitance-to-digital converter with temperature
 *             sensor driver
 *
 * @author     Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

/**
 * @brief 0 fF capacitance code
 */
#define AD7746_ZERO_SCALE_CODE  (0x800000LL)

/**
 * @brief Interval voltage reference expressed in mV
 */
#define AD7746_INTERNAL_VREF    (1170)

/**
 * @brief Maximum value that can be configured into the DACs.
 */
#define AD7746_DAC_MAX          (0x7F)

/**
 * @brief Channel numbers for reading
 */
enum {
    AD7746_READ_CAP_CH = 0, /**< read capacitive channel */
    AD7746_READ_VT_CH  = 1  /**< read voltage / temperature channel */
};

/**
 * @brief   Named return values
 */
enum {
    AD7746_OK          =  0,       /**< everything was fine */
    AD7746_NOI2C       = -1,       /**< I2C communication failed */
    AD7746_NODEV       = -2,       /**< no AD7746 device found on the bus */
    AD7746_NODATA      = -3        /**< no data available */
};

/**
 * @brief Voltage / Temperature channel sample rates
 */
typedef enum {
    AD7746_VT_SR_498 = 0, /**< 49.8 Hz */
    AD7746_VT_SR_312 = 1, /**< 31.2 Hz */
    AD7746_VT_SR_161 = 2, /**< 16.1 Hz */
    AD7746_VT_SR_082 = 3  /**<  8.2 Hz */
} ad7746_vt_sample_rate_t;

/**
 * @brief Voltage / Temperature channel modes
 */
typedef enum {
    AD7746_VT_MD_DIS   = -1, /**< channel disabled */
    AD7746_VT_MD_TEMP  = 0,  /**< internal temperature sensor */
    AD7746_VT_MD_ETEMP = 1,  /**< external temperature sensor (see datasheet) */
    AD7746_VT_MD_VDD   = 2,  /**< Vdd voltage monitor */
    AD7746_VT_MD_VIN   = 3   /**< external voltage input (Vin) */
} ad7746_vt_mode_t;

/**
 * @brief Capacitance channel input
 */
typedef enum {
    AD7746_CAP_IN_1 = 0, /**< CIN1 input */
    AD7746_CAP_IN_2      /**< CIN2 input */
} ad7746_cap_input_t;

/**
 * @brief Capacitive channel sample rate
 */
typedef enum {
    AD7746_CAP_SR_909 = 0, /**< 90.9 Hz */
    AD7746_CAP_SR_838 = 1, /**< 83.8 Hz */
    AD7746_CAP_SR_500 = 2, /**< 50.0 Hz */
    AD7746_CAP_SR_263 = 3, /**< 26.3 Hz */
    AD7746_CAP_SR_161 = 4, /**< 16.1 Hz */
    AD7746_CAP_SR_130 = 5, /**< 13.0 Hz */
    AD7746_CAP_SR_109 = 6, /**< 10.9 Hz */
    AD7746_CAP_SR_091 = 7  /**<  9.1 Hz */
} ad7746_cap_sample_rate_t;

/**
 * @brief Excitation signal output configuration
 */
typedef enum {
    AD7746_EXC_A  = 0x06,  /**< enable only exc A output */
    AD7746_EXC_B  = 0x09,  /**< enable only exc B output */
    AD7746_EXC_AB = 0x0A   /**< enable exc A and B outputs */
} ad7746_exc_config_t;

/**
 * @brief   AD7746 params
 */
typedef struct ad7746_params {
    i2c_t i2c;                                /**< i2c device */
    uint8_t addr;                             /**< i2c address */
    uint8_t dac_a_cap;                        /**< DAC A capacitance */
    uint8_t dac_b_cap;                        /**< DAC B capacitance */
    ad7746_exc_config_t exc_config;           /**< excitation signal config */
    ad7746_cap_sample_rate_t cap_sample_rate; /**< capacitance sample rate */
    ad7746_vt_sample_rate_t vt_sample_rate;   /**< voltage/temp sample rate */
    ad7746_vt_mode_t vt_mode;                 /**< mode of the voltage/temp ch */
    ad7746_cap_input_t cap_input;             /**< selected capacitance input*/
} ad7746_params_t;

/**
 * @brief   AD7746 device descriptor
 */
typedef struct ad7746 {
    ad7746_params_t params; /**< device driver configuration */
} ad7746_t;

/**
 * @brief   Initializes an AD7746 device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return AD7746_OK on success
 * @return AD7746_NODEV if no device is found on the bus
 * @return AD7746_NOI2C if other error occurs
 */
int ad7746_init(ad7746_t *dev, const ad7746_params_t *params);

/**
 * @brief Reads the capacitance from the input 1 (CIN1). Returns the value in
 *        fF.
 *
 * @note If the currently selected input does not match @ref AD7746_CAP_IN_1 it
 *       will be changed to it. This may cause data not to be available right
 *       away. The time until new data is available will depend on the
 *       @ref ad7746_params_t::cap_sample_rate "sample rate" of the channel.
 *
 * @param[in, out] dev  device descriptor
 * @param[out] value    read value in fF
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_capacitance_1(ad7746_t *dev, int *value);

/**
 * @brief Reads the capacitance from the input 2 (CIN2). Returns the value in
 *        fF.
 *
 * @note If the currently selected input does not match @ref AD7746_CAP_IN_2 it
 *       will be changed to it. This may cause data not to be available right
 *       away. The time until new data is available will depend on the
 *       @ref ad7746_params_t::cap_sample_rate "sample rate" of the channel.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in fF
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_capacitance_2(ad7746_t *dev, int *value);

/**
 * @brief Reads the voltage from the external voltage input (VIN). Returns the
 *        value in mV.
 *
 * @note If the current mode of the voltage / temperature channel does not match
 *       @ref AD7746_VT_MD_VIN it will be changed to this mode, causing data not
 *       to be available right away. The time until new data is available will
 *       depend on the @ref ad7746_params_t::vt_sample_rate "sample rate" of the
 *       channel.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in mV
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_voltage_in(ad7746_t *dev, int *value);

/**
 * @brief Reads the voltage from the VDD pin. Returns the value in mV.
 *
 * @note If the current mode of the voltage / temperature channel does not match
 *       @ref AD7746_VT_MD_VDD it will be changed to this mode, causing data not
 *       to be available right away. The time until new data is available will
 *       depend on the @ref ad7746_params_t::vt_sample_rate "sample rate" of the
 *       channel.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in mV
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_voltage_vdd(ad7746_t *dev, int *value);

/**
 * @brief Reads the temperature from the internal sensor.
 *
 * @note If the current mode of the voltage / temperature channel does not match
 *       @ref AD7746_VT_MD_TEMP it will be changed to this mode, causing data
 *       not to be available right away. The time until new data is available
 *       will depend on the @ref ad7746_params_t::vt_sample_rate "sample rate"
 *       of the channel.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in celsius
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_temperature_int(ad7746_t *dev, int *value);

/**
 * @brief Reads the temperature from the external sensor (see datasheet).
 *
 * @note If the current mode of the voltage / temperature channel does not match
 *       @ref AD7746_VT_MD_ETEMP it will be changed to this mode, causing data
 *       not to be available right away. The time until new data is available
 *       will depend on the @ref ad7746_params_t::vt_sample_rate "sample rate"
 *       of the channel.
 *
 * @param[in, out] dev device descriptor
 * @param[out] value read value in celsius
 *
 * @return AD7746_OK on success
 * @return AD7746_NODATA if there is no data available in the channel
 * @return AD7746_I2C if other error occurs
 */
int ad7746_read_temperature_ext(ad7746_t *dev, int *value);

/**
 * @brief   Sets the current input for the capacitive measurement.If not
            configured manually, the driver sets the correct mode within
            the dedicated read function.
 *
 * @param[in] dev      device descriptor
 * @param[in] input  selected input - 0 for CIN1, 1 for CIN2
 *
 * @return AD7746_OK on success
 * @return AD7746_NOI2C on error
 */
int ad7746_set_cap_ch_input(const ad7746_t *dev, ad7746_cap_input_t input);

/**
 * @brief   Sets the mode for the voltage / temperature channel and updates the
 *          descriptor with the new configuration. If not configured manually,
            the driver sets the correct mode within the dedicated read function.
 *
 * @param[in, out]  dev   device descriptor
 * @param[in] mode  mode to which the channel has to be set
 *
 * @return AD7746_OK on success
 * @return AD7746_NOI2C on error
 */
int ad7746_set_vt_ch_mode(ad7746_t *dev, ad7746_vt_mode_t mode);

/**
 * @brief   Sets the sample rate for the voltage / temperature channel
 *
 * @param[in] dev   device descriptor
 * @param[in] sr    sample rate
 *
 * @return AD7746_OK on success
 * @return AD7746_NOI2C on error
 */
int ad7746_set_vt_sr(const ad7746_t *dev, ad7746_vt_sample_rate_t sr);

/**
 * @brief   Sets the sample rate for the capacitance channel
 *
 * @param[in] dev   device descriptor
 * @param[in] sr    sample rate
 *
 * @return AD7746_OK on success
 * @return AD7746_NOI2C on error
 */
int ad7746_set_cap_sr(const ad7746_t *dev, ad7746_cap_sample_rate_t sr);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AD7746_H */
