#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/i2c.h"
#include "periph/gpio.h"


/**
 * @brief Named return values
 */
enum {
    ADS1X1X_OK      = 0,    /**< Operation successful */
    ADS1X1X_NOI2C   = -1,   /**< I2C communication error */
    ADS1X1X_NODEV   = -2,   /**< No device found on the bus */
    ADS1X1X_NODATA  = -3,   /**< No data available */
};


/**
 * @brief   ADS101x/111x params
 */
typedef struct {
    i2c_t i2c;                          /**< I2C device */
    uint8_t addr;                       /**< I2C address */
    uint8_t mux_gain;                   /**< Input multiplexer configuration */
    uint8_t pga;                        /**< Programmable gain amplifier configuration */
    uint8_t mode;                       /**< Device mode */
    uint8_t dr;                         /**< Data rate configuration */
    uint8_t comp_mode;                  /**< Comparator mode */
    uint8_t comp_polarity;              /**< Comparator polarity */
    uint8_t comp_latch;                 /**< Comparator latch */
    uint8_t comp_queue;                 /**< Comparator queue */
} ads1x1x_params_t;

/**
 * @brief   ADS101x/111x alert params
 */
typedef struct ads1x1x_alert_params {
    i2c_t i2c;              /**< i2c device */
    uint8_t addr;           /**< i2c address */
    gpio_t alert_pin;       /**< alert pin (GPIO_UNDEF if not connected) */
    int16_t low_limit;      /**< alert low value */
    int16_t high_limit;     /**< alert high value */
} ads1x1x_alert_params_t;


/**
 * @brief   ADS101x/111x device descriptor
 */
typedef struct ads1x1x {
    ads1x1x_params_t params;    /**< device driver configuration */
} ads1x1x_t;


/**
 * @brief   ADS101x/111x alert callback
 */
typedef void (*ads1x1x_alert_cb_t)(void *);

/**
 * @brief   ADS101x/111x alert device descriptor
 */
typedef struct ads1x1x_alert {
    ads1x1x_alert_params_t params;      /**< device driver configuration */
    ads1x1x_alert_cb_t cb;              /**< alert callback */
    void *arg;                          /**< alert callback param */
} ads1x1x_alert_t;

/**
 * @brief   Initialize an ADS101x/111x ADC device (ADC only)
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int ads1x1x_init(ads1x1x_t *dev, const ads1x1x_params_t *params);

/**
 * @brief   Set parameters
 *
 * @param[in,out] dev      device descriptor
 * @param[in] params       device configuration
 *
 * @return zero on success, non zero on error
 */
int ads1x1x_set_parameters(ads1x1x_t *dev, const ads1x1x_params_t *params);

/**
 * @brief   Initialize an ADS101x/111x alert device
 *
 * @param[in,out] dev  device descriptor
 * @param[in] params   device configuration
 *
 * @return zero on successful initialization, non zero on error
 */
int ads1x1x_alert_init(ads1x1x_alert_t *dev,
                       const ads1x1x_alert_params_t *params);

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
int ads1x1x_set_mux_gain(ads1x1x_t *dev, uint8_t mux_gain);

/**
 * @brief   Read a raw ADC value
 *
 * @param[in] dev   device descriptor
 * @param[out] raw  read value
 *
 * @return zero on successful read, non zero on error
 */
int ads1x1x_read_raw(const ads1x1x_t *dev, int16_t *raw);

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
int ads1x1x_enable_alert(ads1x1x_alert_t *dev,
                         ads1x1x_alert_cb_t cb, void *arg);

/**
 * @brief   Set the alert parameters
 *
 * Alert settings have no effect on ADS1013 and ADS1113.
 *
 * @param[in,out] dev      device descriptor
 * @param[in] params       device configuration
 *
 * @return zero on success, non zero on error
 */
int ads1x1x_set_alert_parameters(ads1x1x_alert_t *dev, const ads1x1x_alert_params_t *params);
/**
 * @brief Converts the digital value from an ADS101x/111x device to millivolts.
 *
 * @param[in] dev    Device descriptor
 * @param[in] value  Raw value from the ADS101x/111x device
 *
 * @return Converted value in millivolts
 */
int ads1x1x_convert_to_mv(ads1x1x_t *dev, int16_t value);



#ifdef __cplusplus
}
#endif

/** @} */
