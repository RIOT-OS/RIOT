/*
 * Copyright (C) 2017 OTA keys S.A.
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup   drivers_ads101x ADS101x/111x ADC device driver
 * @ingroup    drivers_sensors
 * @ingroup    drivers_saul
 * @brief      I2C Analog-to-Digital Converter device driver
 *
 * This driver works with ADS1013-5 and ADS1113-5.
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief      ADS101x/111x ADC device driver
 *
 * ADC and alert functionality are separated into two devices to
 * prevent wasteful representations on muxed devices.
 *
 * @author     Vincent Dupont <vincent@otakeys.com>
 * @author     Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef ADS101X_H
#define ADS101X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"

#if MODULE_ADC_NG || DOXYGEN
#include "adc_ng.h"
#endif

/**
 * @brief   ADS101x/111x default address
 *
 * Address pin tied to: GND (0x48), Vcc (0x49), SDA (0x50), SCL (0x51)
 */
#ifndef ADS101X_I2C_ADDRESS
#define ADS101X_I2C_ADDRESS    (0x48)
#endif

#if MODULE_ADC_NG || DOXYGEN

/**
 * @brief   Resolution supported by given ADS101x/ADS111x device
 */
#define ADS101X_RES(d)  (((d == ADS101X_DEV_ADS1113) || \
                          (d == ADS101X_DEV_ADS1114) || \
                          (d == ADS101X_DEV_ADS1115)) ? 16 : 12)

/**
 * @brief   Reference to the corresponding array of reference voltages
 */
#define ADS101X_REF_VOL(d)  (((d == ADS101X_DEV_ADS1013) || \
                              (d == ADS101X_DEV_ADS1113)) ? ads1013_adc_ng_refs \
                                                          : ads101x_adc_ng_refs)

/**
 * @brief   Number of channels supported by given ADS101x/ADS111x device
 */
#define ADS101X_CH_NUMOF(d) (((d == ADS101X_DEV_ADS1015) || \
                              (d == ADS101X_DEV_ADS1115)) ? 4 : 1)

/**
 * @brief   Array of reference voltages supported by ADS1013/ADS1113
 */
static const int16_t ads1013_adc_ng_refs[] = { 2048, 0 };

/**
 * @brief   Array of reference voltages supported by the other ADS101x/ADS111x devices 
 */
static const int16_t ads101x_adc_ng_refs[] = { 256, 512, 1024, 2048, 4096, 6144, 0 };

/**
 * @brief   ADS101x/111x device variants required by the Common ADC API
 *
 * ADS101x/111x device variant is required by the Common ADC API to have
 * information about the number of channels and the ADC resolution. The
 * ADS101x/111x device variant has to be specified in configuration
 * parameters.
 */
typedef enum {
    ADS101X_DEV_UNKOWN = 0, /**< unknown device variant */
    ADS101X_DEV_ADS1013,    /**< 1 channel, 12-bit ADC */
    ADS101X_DEV_ADS1014,    /**< 1 channel, 12-bit ADC */
    ADS101X_DEV_ADS1015,    /**< 4 channel, 12-bit ADC */
    ADS101X_DEV_ADS1113,    /**< 1 channel, 16-bit ADC */
    ADS101X_DEV_ADS1114,    /**< 1 channel, 16-bit ADC */
    ADS101X_DEV_ADS1115,    /**< 4 channel, 16-bit ADC */
} ads101x_device_t;
#endif

/**
 * @brief   Named return values
 */
enum {
    ADS101X_OK          =  0,       /**< everything was fine */
    ADS101X_NOI2C       = -1,       /**< I2C communication failed */
    ADS101X_NODEV       = -2,       /**< no ADS101X device found on the bus */
    ADS101X_NODATA      = -3        /**< no data available */
};

/**
 * @brief   ADS101x/111x params
 */
typedef struct ads101x_params {
    i2c_t i2c;              /**< i2c device */
    uint8_t addr;           /**< i2c address */
    uint8_t mux_gain;       /**< Mux and gain boolean settings */
#if MODULE_ADC_NG || DOXYGEN
    ads101x_device_t device;  /**< ADS101x/111x device variant */
#endif
} ads101x_params_t;

/**
 * @brief   ADS101x/111x alert params
 */
typedef struct ads101x_alert_params {
    i2c_t i2c;              /**< i2c device */
    uint8_t addr;           /**< i2c address */
    gpio_t alert_pin;       /**< alert pin (GPIO_UNDEF if not connected) */
    int16_t low_limit;      /**< alert low value */
    int16_t high_limit;     /**< alert high value */
} ads101x_alert_params_t;

/**
 * @brief   ADS101x/111x device descriptor
 */
typedef struct ads101x {
    ads101x_params_t params;    /**< device driver configuration */
#if MODULE_ADC_NG || DOXYGEN
    uint8_t res;                /**< supported resolution */
    uint8_t ch_numof;           /**< number of available channels */
#endif
} ads101x_t;

/**
 * @brief   ADS101x/111x alert callback
 */
typedef void (*ads101x_alert_cb_t)(void *);

/**
 * @brief   ADS101x/111x alert device descriptor
 */
typedef struct ads101x_alert {
    ads101x_alert_params_t params;    /**< device driver configuration */
    ads101x_alert_cb_t cb;            /**< alert callback */
    void *arg;                        /**< alert callback param */
} ads101x_alert_t;

/**
 * @brief   Initialize an ADS101x/111x ADC device (ADC only)
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int ads101x_init(ads101x_t *dev, const ads101x_params_t *params);

/**
 * @brief   Initialize an ADS101x/111x alert device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int ads101x_alert_init(ads101x_alert_t *dev,
                       const ads101x_alert_params_t *params);

/**
 * @brief   Set mux and gain
 *
 * Mux settings have no effect on ADS1013-4 and ADS1113-4.
 * Gain settings have no effect on ADS1013 and ADS1113.
 *
 * @param[in] dev       device descriptor
 * @param[in] mux_gain  mux and gain boolean values
 *
 * @return zero on successful read, non zero on error
 */
int ads101x_set_mux_gain(const ads101x_t *dev, uint8_t mux_gain);

/**
 * @brief   Read a raw ADC value
 *
 * @param[in] dev   device descriptor
 * @param[out] raw  read value
 *
 * @return zero on successful read, non zero on error
 */
int ads101x_read_raw(const ads101x_t *dev, int16_t *raw);

/**
 * @brief   Enable alert interrupt
 *
 * Alert settings have no effect on ADS1013 and ADS1113.
 *
 * @param[in] dev   device descriptor
 * @param[in] cb    callback called when the alert fires
 * @param[in] arg   callback argument
 *
 * @return zero on success, non zero on error
 */
int ads101x_enable_alert(ads101x_alert_t *dev,
                         ads101x_alert_cb_t cb, void *arg);

/**
 * @brief   Set the alert parameters
 *
 * Alert settings have no effect on ADS1013 and ADS1113.
 *
 * @param[in,out] dev      device descriptor
 * @param[in] low_limit    alert low limit
 * @param[in] high_limit   alert high limit
 *
 * @return zero on success, non zero on error
 */
int ads101x_set_alert_parameters(const ads101x_alert_t *dev,
                                 int16_t low_limit, int16_t high_limit);

#if MODULE_ADC_NG || DOXYGEN
/**
 * @name    ADC extension API functions
 *
 * These functions are defined for compatibility with the Common ADC API.
 * They are required to use ADS101x/111x devices as ADC extensions.
 *
 * @{
 */

/**
 * @brief   #adc_ng_init_t callback function for the Common ADC API
 *
 * @note This function is for use with the Common ADC API.
 *
 * This function is the implementation of of #adc_ng_init function of the
 * ADC extension API. Since ADS101x/ADS111x channels have not to be
 * initialized, this function will not do anything.
 * 
 * @see #adc_ng_init_t for details.
 */
int ads101x_adc_ng_init(void* handle, uint8_t chn, uint8_t res, uint8_t ref);

/**
 * @brief   #adc_ng_single_t callback function for the Common ADC API
 *
 * @note This function is for use with the Common ADC API.
 *
 * The function starts a single conversion and returns the result of the
 * conversion or -1 in case of error. The gain that is used for the conversion
 * is defined in the ads101x_params_t::mux_gain parameter. The conversion
 * takes about 8 ms.
 * 
 * @see #adc_ng_single_t for details.
 */
int ads101x_adc_ng_single(void *handle, int32_t *dest);

/** @} */
#endif /* MODULE_ADC_NG || DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* ADS101X_H */
/** @} */
