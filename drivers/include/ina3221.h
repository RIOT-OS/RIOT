/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ina3221 INA3221 current/power monitor
 * @ingroup     drivers_sensors
 * @brief       Device driver for Texas Instruments INA3221
 *              three-channel ,high-side current and bus voltage
 *              monitor
 * @{
 *
 * @file
 * @brief       Device driver interface for Texas Instruments INA3221
 *              three-channel ,high-side current and bus voltage
 *              monitor.
 *
 *              The alert pins WRN, CRT, TC, PV are open drain
 *              output pins, so they should be pulled high with
 *              internal or external pull-ups. They pull low, if
 *              the alert condition becomes true. If they become
 *              pulled low, the configured input pins in
 *              @see ina3221_params_t will read a low flank and
 *              the related interrupt callback will be executed,
 *              if configured with @see ina3221_enable_alert.
 *              WRN condition:
 *              Any channel´s measured shunt
 *              voltage drop was higher than the configured value in
 *              INA3221_REG_CHx_WARN_ALERT_LIMIT,
 *              CRT condition:
 *              Any channel´s measured shunt
 *              voltage drop was higher than the configured value in
 *              INA3221_REG_CHx_CRIT_ALERT_LIMIT,
 *              or the sum of all channel´s voltage drops was higher
 *              than the configured value in
 *              INA3221_REG_SHUNT_VOLTAGE_SUM_LIMIT. Note that the
 *              sum of voltage drops only makes sense, if all
 *              channels are using the same shunt resistance value!
 *              TC condition:
 *              After a 1.2-V level is detected on channel 1, if the
 *              INA3221 does not detecta 1.2-V value or greater on
 *              the bus voltage measurement following four complete
 *              cycles of all three channels, the timing control
 *              (TC)alert pin pulls low.
 *              PV condition:
 *              If any channel´s measured bus voltage was not within
 *              the configured values in INA3221_REG_PV_LOWER_LIMIT
 *              and INA3221_REG_PV_UPPER_LIMIT, the PV pin is pulled
 *              low.
 *
 *              For all enabled channels (1 <= i <= INA3221_NUM_CH),
 *              the time to complete a full measurement cycle is:
 *
 *              num_samples * (shunt_conv_time_ch_i + bus_voltage_conv_time_ch_i)
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef INA3221_H
#define INA3221_H

#include <stdint.h>

#include "periph/gpio.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INA3221_NUM_CH                  (3)         /**< Number of channels */

#define INA3221_NUM_ALERTS              (4)         /**< Number of alerts */

/**
 * @brief INA3221 alert callback for alert pins
 * CRT, WRN, TC, PV
 */
typedef void (*ina3221_alert_cb_t)(void *arg);

/**
 * @brief Return codes
 */
typedef enum ina3221_ret_val {
    INA3221_OK,                                     /**< Everything is fine */
    INA3221_I2C_ERROR,                              /**< I2C bus acquirenment failed */
    INA3221_BAD_MANUF_ID,                           /**< Read device manufacturer ID failed */
    INA3221_BAD_DIE_ID,                             /**< Read device DIE ID failed */
    INA3221_RESET_FAILED,                           /**< Device reset failed */
    INA3221_CONFIG_FAILED                           /**< Device configuration failed */
} ina3221_ret_val_t;

/**
 * @brief I2C device addresses
 */
typedef enum ina3221_dev_addr {
    INA3221_ADDR_00 = 0x40,                         /**< A0 connected to GND */
    INA3221_ADDR_01 = 0x41,                         /**< A0 connected to VS  */
    INA3221_ADDR_10 = 0x42,                         /**< A0 connected to SDA */
    INA3221_ADDR_11 = 0x43,                         /**< A0 connected to SCL */
} ina3221_dav_addr_t;

/**
 * @brief Channel flags
 */
typedef enum ina3221_channel {
    INA3221_CH1 = (1 << 0),                         /**< Channel 1 */
    INA3221_CH2 = (1 << 1),                         /**< Channel 2 */
    INA3221_CH3 = (1 << 2)                          /**< Channel 3 */
} ina3221_channel_t;

/**
 * @brief Alert indices
 */
typedef enum ina3221_alert {
    INA3221_ALERT_WRN   = 0,                        /**< Warning alert */
    INA3221_ALERT_CRT   = 1,                        /**< Critical alert */
    INA3221_ALERT_TC    = 2,                        /**< Timing control */
    INA3221_ALERT_PV    = 3                         /**< Power valid alert */
} ina3221_alert_t;

/**
 * @brief Device operation modes
 */
typedef enum ina3221_mode {
    INA3221_MODE_POWER_DOWN             = 0x0000,   /**< Sleep mode */
    INA3221_MODE_TRIGGER_SHUNT_ONLY     = 0x0001,   /**< Trigger shunt voltage measurement only */
    INA3221_MODE_TRIGGER_BUS_ONLY       = 0x0002,   /**< Trigger bus voltage measurement only */
    INA3221_MODE_TRIGGER_SHUNT_BUS      = 0x0003,   /**< Trigger shunt and bus voltage measurement */
    INA3221_MODE_POWER_DOWN_            = 0x0004,   /**< Also power down (see datasheet)*/
    INA3221_MODE_CONTINUOUS_SHUNT_ONLY  = 0x0005,   /**< Continuous shunt voltage measurement only*/
    INA3221_MODE_CONTINUOUS_BUS_ONLY    = 0x0006,   /**< Continuous bus voltage measurement only*/
    INA3221_MODE_CONTINUOUS_SHUNT_BUS   = 0x0007    /**< Continuous shunt and bus voltage measurement */
} ina3221_mode_t;
#define INA3221_MODE_MASK               (0x0007)    /**< Device operation mode bit mask */

/**
 * @brief ADC shunt voltage conversion times
 */
typedef enum ina3221_conv_time_shunt_adc {
    INA3221_CONV_TIME_SADC_140US    = 0x0000,       /**< 140 us */
    INA3221_CONV_TIME_SADC_204US    = 0x0008,       /**< 204 us */
    INA3221_CONV_TIME_SADC_332US    = 0x0010,       /**< 332 us */
    INA3221_CONV_TIME_SADC_588US    = 0x0018,       /**< 588 us */
    INA3221_CONV_TIME_SADC_1100US   = 0x0020,       /**< 1100 us */
    INA3221_CONV_TIME_SADC_2116US   = 0x0028,       /**< 2116 us */
    INA3221_CONV_TIME_SADC_4156US   = 0x0030,       /**< 4156 us */
    INA3221_CONV_TIME_SADC_8244US   = 0x0038        /**< 8244 us */
} ina3221_conv_time_shunt_adc_t;
#define INA3221_CONV_TIME_SADC_MASK    (0x0038)     /**< ADC shunt voltage conversion times bit mask*/

/**
 * @brief ADC bus voltage conversion times
 */
typedef enum ina3221_conv_time_bus_adc {
    INA3221_CONV_TIME_BADC_140US    = 0x0000,       /**< 140 us */
    INA3221_CONV_TIME_BADC_204US    = 0x0040,       /**< 204 us */
    INA3221_CONV_TIME_BADC_332US    = 0x0080,       /**< 332 us */
    INA3221_CONV_TIME_BADC_588US    = 0x00C0,       /**< 588 us */
    INA3221_CONV_TIME_BADC_1100US   = 0x0100,       /**< 1100 us */
    INA3221_CONV_TIME_BADC_2116US   = 0x0140,       /**< 2116 us */
    INA3221_CONV_TIME_BADC_4156US   = 0x0180,       /**< 4156 us */
    INA3221_CONV_TIME_BADC_8244US   = 0x01C0        /**< 8244 us */
} ina3221_conv_time_bus_adc_t;
#define INA3221_CONV_TIME_BADC_MASK    (0x01C0)     /**< ADC bus voltage conversion times bit mask*/

/**
 * @brief Number of samples to calculate average shunt/bus voltage value
 */
typedef enum ina3221_num_samples {
    INA3221_NUM_SAMPLES_1       = 0x0000,           /**< 1 */
    INA3221_NUM_SAMPLES_4       = 0x0200,           /**< 4 */
    INA3221_NUM_SAMPLES_16      = 0x0400,           /**< 16 */
    INA3221_NUM_SAMPLES_64      = 0x0600,           /**< 64 */
    INA3221_NUM_SAMPLES_128     = 0x0800,           /**< 128 */
    INA3221_NUM_SAMPLES_256     = 0x0A00,           /**< 256 */
    INA3221_NUM_SAMPLES_512     = 0x0C00,           /**< 512 */
    INA3221_NUM_SAMPLES_1024    = 0x0E00            /**< 1024 */
} ina3221_num_samples_t;
#define INA3221_NUM_SAMPLES_MASK       (0x0E00)     /**< Number of samples to calculate average shunt/bus voltage value bit mask */

/**
 * @brief Enable channel flags
 */
typedef enum ina3221_enable_ch {
    INA3221_ENABLE_CH1  = 0x4000,                   /**< Enable channel 1 */
    INA3221_ENABLE_CH2  = 0x2000,                   /**< Enable channel 2 */
    INA3221_ENABLE_CH3  = 0x1000                    /**< Enable channel 3 */
} ina3221_enable_ch_t;
#define INA3221_ENABLE_CH_MASK          (INA3221_ENABLE_CH1    \
                                         | INA3221_ENABLE_CH2  \
                                         | INA3221_ENABLE_CH3)  /**< Enable channel bit mask */

/**
 * @brief   Channel state enabled/disabled
 */
typedef enum ina3221_channel_state {
    INA3221_CH_DISABLE,                             /**< Channel disabled */
    INA3221_CH_ENABLE                               /**< Channel enabled */
} ina3221_channel_state_t;

/**
 * @brief Enable shunt voltage sum calculation channel flags
 */
typedef enum ina3221_enable_sum_ch {
    INA3221_ENABLE_SUM_CH1  = 0x4000,               /**< Enable sum channel 1 */
    INA3221_ENABLE_SUM_CH2  = 0x2000,               /**< Enable sum channel 2 */
    INA3221_ENABLE_SUM_CH3  = 0x1000                /**< Enable sum channel 3 */
} ina3221_enable_sum_ch_t;
#define INA3221_ENABLE_SUM_CH_MASK     (INA3221_ENABLE_SUM_CH1     \
                                        | INA3221_ENABLE_SUM_CH2   \
                                        | INA3221_ENABLE_SUM_CH3)  /**< Enable shunt voltage sum calculation channel bit mask */

/**
 * @brief Enable latch flags
 */
typedef enum ina3221_enable_latch {
    INA3221_ENABLE_WARN_LATCH   = 0x0800,           /**< Enable warning alert latch */
    INA3221_ENABLE_CRIT_LATCH   = 0x0400            /**< Enable critical alert latch */
} ina3221_enable_latch_t;
#define INA3221_ENABLE_LATCH_MASK      (INA3221_ENABLE_WARN_LATCH  \
                                        | INA3221_ENABLE_CRIT_LATCH)  /**< Enable latch bit mask */
/**
 * @brief INA3221 device parameters
 */
typedef struct {
    i2c_t i2c;                                      /**< I2C bus */
    uint8_t addr;                                   /**< I2C address */
    union {
        struct {
            gpio_t pin_warn;                        /**< Warning alert pin, becomes configured as input*/
            gpio_t pin_crit;                        /**< Critical alert pin, becomes configured as input*/
            gpio_t pin_tc;                          /**< Timing control alert pin, becomes configured as input*/
            gpio_t pin_pv;                          /**< Power valid alert pin, becomes configured as input*/
        } pins;                                     /**< Struct of alert pins */
        struct {
            gpio_t alert_pins[INA3221_NUM_ALERTS];  /**< Array to access alert pins via an index */
        } apins;                                    /**< Struct of array of alert pins */
    } upins;                                        /**< Union of alert pins as array and as single members */
    /*
        For each pin 1 bit states whether to use internal pull up resistors.
        d = don´t care
        [d][d][d][d][pu PV][pu TC][pu CRT][pu WRN]
     */
    uint8_t gpio_config;                            /**< Configure pull up resistors for gpio pins */
    uint16_t config;                                /**< Configuration */
    uint16_t rshunt_mohm[INA3221_NUM_CH];           /**< Shunt resistances im mOhm for each channel */
} ina3221_params_t;

/* (  INA3221_ENABLE_CH_1
 | INA3221_ENABLE_CH_2
 | INA3221_ENABLE_CH_2
 | INA3221_NUM_SAMPLES_1
 | INA3221_CONV_TIME_BADC_1100US
 | INA3221_CONV_TIME_SADC_1100US
 | INA3221_MODE_CONTINUOUS_SHUNT_BUS ) */
#define INA3221_DEFCONFIG               (0x7127)    /**< Default configuration register value */

#define INA3221_RESET                   (0x8000)    /**< Reset configuration register value*/

#define INA3221_FLAG_CRIT_ALERT_CH1     (0x0200)    /**< Critical alert channel 1 flag */
#define INA3221_FLAG_CRIT_ALERT_CH2     (0x0100)    /**< Critical alert channel 2 flag */
#define INA3221_FLAG_CRIT_ALERT_CH3     (0x0080)    /**< Critical alert channel 3 flag */

#define INA3221_FLAG_SHUNT_SUM_ALERT    (0x0040)    /**< Alert channel sum flag */

#define INA3221_FLAG_WARN_ALERT_CH1     (0x0020)    /**< Warning alert channel 1 flag */
#define INA3221_FLAG_WARN_ALERT_CH2     (0x0010)    /**< Warning alert channel 2 flag */
#define INA3221_FLAG_WARN_ALERT_CH3     (0x0008)    /**< Warning alert channel 3 flag */

#define INA3221_FLAG_POWER_VALID        (0x0004)    /**< Power valid flag */

#define INA3221_FLAG_TIMING_CONTROL     (0x0002)    /**< Timing Control flag */

#define INA3221_FLAG_CONV_READY         (0x0001)    /**< Conversion ready flag */

#define INA3221_FLAGS_MASK              (INA3221_FLAG_CRIT_ALERT_CH1       \
                                         | INA3221_FLAG_CRIT_ALERT_CH2     \
                                         | INA3221_FLAG_CRIT_ALERT_CH3     \
                                         | INA3221_FLAG_SHUNT_SUM_ALERT    \
                                         | INA3221_FLAG_WARN_ALERT_CH1     \
                                         | INA3221_FLAG_WARN_ALERT_CH2     \
                                         | INA3221_FLAG_WARN_ALERT_CH3     \
                                         | INA3221_FLAG_POWER_VALID        \
                                         | INA3221_FLAG_TIMING_CONTROL     \
                                         | INA3221_FLAG_CONV_READY)      /**< Flags bit mask*/

/**
 * @brief INA3221 device handle struct
 */
typedef struct {
    ina3221_params_t params;                      /**< Device parameters */
#if defined(MODULE_INA3221_ALERTS) || defined(DOXYGEN)
    union {
        struct {
            ina3221_alert_cb_t warn_cb;             /**< Warning alert callback: executed when WRN is pulled low */
            ina3221_alert_cb_t crit_cb;             /**< Critical alert callback: executed when CRT is pulled low */
            ina3221_alert_cb_t tc_cb;               /**< Timing control alert callback: executed when TC is pulled low */
            ina3221_alert_cb_t pv_cb;               /**< Power Valid  alert callback: executed when PV is pulled low */
        };
        struct {
            ina3221_alert_cb_t alert_callbacks[INA3221_NUM_ALERTS];  /**< Array to access alert callbacks via an index */
        };
    };
    union {
        struct {
            void *arg_warn_cb;                      /**< Argument for Warning alert callback */
            void *arg_crit_cb;                      /**< Argument for Critical alert callback */
            void *arg_tc_cb;                        /**< Argument for Timing control alert callback */
            void *arg_pv_cb;                        /**< Argument for Power Valid  alert callback */
        };
        struct {
            void *alert_callback_arguments[INA3221_NUM_ALERTS]; /**< Array to access alert callback arguments via an index */
        };
    };
#endif /* MODULE_INA3221_ALERTS */
} ina3221_t;

/**
 * @brief Reset device, i.e. apply default configuration
 *
 * @param[in,out]   dev Device handle
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      -INA3221_RESET_FAILED, if device could not be reset
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_reset(ina3221_t *dev);

/**
 * @brief Initialize device
 *
 * @param[out]      dev Device handle
 * @param[in]       params Device parameters
 *
 * @return      INA3221_OK, on success
 * @return      -EINVAL, if NULL pointer was passed
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      -INA3221_BAD_MANUF_ID, if manufacturer ID does not match
 * @return      -INA3221_DIE_ID, if DIE ID does not match
 * @return      -INA3221_RESET_FAILED, if reset failed
 * @return      -INA3221_CONFIG_FAILED, if configuration could not be applied
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_init(ina3221_t *dev, const ina3221_params_t *params);

#if defined(MODULE_INA3221_ALERTS) || defined(DOXYGEN)
/**
 * @brief Enable alert callback and argument for alert @p alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] alert         Alert index
 * @param[in] cb            Alert callback
 * @param[in] arg           Alert callback argument
 *
 * @return INA3221_OK, on success
 * @return -ERANGE, if @p alert was out of bounds
 * @return -ENUTSUP, if alert pin was initialized with GPIO_UNDEF
 */
int _ina3221_enable_alert(ina3221_t *dev, ina3221_alert_t alert,
                          ina3221_alert_cb_t cb, void *arg);

/**
 * @brief   Wrapper around @see _ina3221_enable_alert, to
 *          enable warning alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] cb            Alert callback
 * @param[in] arg           Alert callback argument
 *
 * @return @see _ina3221_enable_alert
 */
static inline int ina3221_enable_warning_alert(ina3221_t *dev,
                                               ina3221_alert_cb_t cb, void *arg)
{
    return _ina3221_enable_alert(dev, INA3221_ALERT_WRN, cb, arg);
}

/**
 * @brief   Wrapper around @see _ina3221_enable_alert, to
 *          enable critical alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] cb            Alert callback
 * @param[in] arg           Alert callback argument
 *
 * @return @see _ina3221_enable_alert
 */
static inline int ina3221_enable_critical_alert(ina3221_t *dev,
                                                ina3221_alert_cb_t cb,
                                                void *arg)
{
    return _ina3221_enable_alert(dev, INA3221_ALERT_CRT, cb, arg);
}

/**
 * @brief   Wrapper around @see _ina3221_enable_alert, to
 *          enable timing control alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] cb            Alert callback
 * @param[in] arg           Alert callback argument
 *
 * @return @see _ina3221_enable_alert
 */
static inline int ina3221_enable_timing_control_alert(ina3221_t *dev,
                                                      ina3221_alert_cb_t cb,
                                                      void *arg)
{
    return _ina3221_enable_alert(dev, INA3221_ALERT_TC, cb, arg);
}

/**
 * @brief   Wrapper around @see _ina3221_enable_alert, to
 *          enable power valid alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] cb            Alert callback
 * @param[in] arg           Alert callback argument
 *
 * @return @see _ina3221_enable_alert
 */
static inline int ina3221_enable_power_valid_alert(ina3221_t *dev,
                                                   ina3221_alert_cb_t cb,
                                                   void *arg)
{
    return _ina3221_enable_alert(dev, INA3221_ALERT_PV, cb, arg);
}

/**
 * @brief Disable alert callback and argument for alert @p alert
 *
 * @param[in,out] dev       Device handle
 * @param[in] alert         Alert index
 *
 * @return INA3221_OK, on success
 * @return -ERANGE, if @p alert was out of bounds
 * @return -ENUTSUP, if alert pin was initialized with GPIO_UNDEF
 */
int _ina3221_disable_alert(ina3221_t *dev, ina3221_alert_t alert);

/**
 * @brief   Wrapper around @see _ina3221_disable_alert, to
 *          disable warning alert
 *
 * @param[in,out] dev       Device handle
 *
 * @return @see _ina3221_disable_alert
 */
static inline int ina3221_disable_warning_alert(ina3221_t *dev)
{
    return _ina3221_disable_alert(dev, INA3221_ALERT_WRN);
}

/**
 * @brief   Wrapper around @see _ina3221_disable_alert, to
 *          disable critical alert
 *
 * @param[in,out] dev       Device handle
 *
 * @return @see _ina3221_disable_alert
 */
static inline int ina3221_disable_critical_alert(ina3221_t *dev)
{
    return _ina3221_disable_alert(dev, INA3221_ALERT_CRT);
}

/**
 * @brief   Wrapper around @see _ina3221_disable_alert, to
 *          disable timing control alert
 *
 * @param[in,out] dev       Device handle
 *
 * @return @see _ina3221_disable_alert
 */
static inline int ina3221_disable_timing_control_alert(ina3221_t *dev)
{
    return _ina3221_disable_alert(dev, INA3221_ALERT_TC);
}

/**
 * @brief   Wrapper around @see _ina3221_disable_alert, to
 *          disable power valid alert
 *
 * @param[in,out] dev       Device handle
 *
 * @return @see _ina3221_disable_alert
 */
static inline int ina3221_disable_power_valid_alert(ina3221_t *dev)
{
    return _ina3221_disable_alert(dev, INA3221_ALERT_PV);
}

#endif /* MODULE_INA3221_ALERTS */

/**
 * @brief Write configuration register value to configuration register
 *
 * @param[in,out]   dev Device handle
 * @param[in]       cfg Configuration register value in host byte order
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int _ina3221_set_config(ina3221_t *dev, uint16_t cfg);

/**
 * @brief Wrapper around @see _ina3221_set_config
 *
 * @param[in, out]  dev Device handle
 * @param[in]       chs Channel enable flags @see ina3221_enable_ch_t
 * @param[in]       ns Number of samples
 * @param[in]       ctbadc Conversion time for bus voltage ADC
 * @param[in]       ctsadc Conversion time for shunt voltage ADC
 * @param[in]       mode Device operation mode
 *
 * @return      @see _ina3221_set_config
 */
static inline int ina3221_set_config(ina3221_t *dev,
                                     uint16_t chs,
                                     ina3221_num_samples_t ns,
                                     ina3221_conv_time_bus_adc_t ctbadc,
                                     ina3221_conv_time_shunt_adc_t ctsadc,
                                     ina3221_mode_t mode)
{
    return _ina3221_set_config(dev, chs | ns | ctbadc | ctsadc | mode);
}

/**
 * @brief Read currently saved configuration register value
 *
 * @param[in]       dev Device handle
 * @param[out]      cfg Pointer to configuration register output value in host byte order
 *
 * @return      INA3221_OK
 */
int _ina3221_get_config(const ina3221_t *dev, uint16_t *cfg);

/**
 * @brief   Wrapper around @see _ina3221_get_config
 *
 * @param[in]      dev Device handle
 * @param[out]     chs Pointer to enabled channels variable @see ina3221_enable_ch_t
 * @param[out]     ns Pointer to number of samples variable
 * @param[out]     ctbadc Pointer to conversion time bus adc variable
 * @param[out]     ctsadc Pointer to conversion time shunt adc variable
 * @param[out]     mode Pointer to measurement mode variable
 *
 * @return      @see _ina3221_get_config
 */
static inline int ina3221_get_config(const ina3221_t *dev,
                                     uint16_t *chs,
                                     ina3221_num_samples_t *ns,
                                     ina3221_conv_time_bus_adc_t *ctbadc,
                                     ina3221_conv_time_shunt_adc_t *ctsadc,
                                     ina3221_mode_t *mode)
{
    uint16_t cfg = 0;
    int ret = _ina3221_get_config(dev, &cfg);

    *chs = cfg & INA3221_ENABLE_CH_MASK;
    *ns = (ina3221_num_samples_t)(cfg & INA3221_NUM_SAMPLES_MASK);
    *ctbadc = (ina3221_conv_time_bus_adc_t)(cfg & INA3221_CONV_TIME_BADC_MASK);
    *ctsadc = (ina3221_conv_time_shunt_adc_t)(cfg & INA3221_CONV_TIME_SADC_MASK);
    *mode = (ina3221_mode_t)(cfg & INA3221_MODE_MASK);
    return ret;
}

/**
 * @brief Enable channels
 *
 * @param[in,out]   dev Device handle
 * @param[in]       ech Channel enable flags @see ina3221_enable_ch_t
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int _ina3221_set_enable_channel(ina3221_t *dev, uint16_t ech);

/**
 * @brief   Wrapper around @see _ina3221_set_enable_channel
 *
 * @param[in, out]  dev Device handle
 * @param[in]       ch1 Channel 1 state
 * @param[in]       ch2 Channel 2 state
 * @param[in]       ch3 Channel 3 state
 *
 * @return @see _ina3221_set_enable_channel
 */
static inline int ina3221_set_channel_state(ina3221_t *dev,
                                            ina3221_channel_state_t ch1,
                                            ina3221_channel_state_t ch2,
                                            ina3221_channel_state_t ch3)
{
    return _ina3221_set_enable_channel(dev,
                                       (ch1 ? INA3221_ENABLE_CH1 : 0) |
                                       (ch2 ? INA3221_ENABLE_CH2 : 0) |
                                       (ch3 ? INA3221_ENABLE_CH3 : 0));
}

/**
 * @brief Read which channels are currently enabled
 *
 * @param[in]       dev Device handle
 * @param[out]      ech Pointer to enabled channels output variable @see ina3221_enable_ch_t
 *
 * @return      Number of enabled channels
 */
int _ina3221_get_enable_channel(const ina3221_t *dev, uint16_t *ech);

/**
 * @brief   Wrapper around _ina3221_get_enable_channel
 *
 * @param[in]   dev Device handle
 * @param[out]  ch1 Pointer to channel 1 state variable
 * @param[out]  ch2 Pointer to channel 2 state variable
 * @param[out]  ch3 Pointer to channel 3 state variable
 *
 * @return      @see _ina3221_get_enable_channel
 */
static inline int ina3221_get_channel_state(const ina3221_t *dev,
                                            ina3221_channel_state_t *ch1,
                                            ina3221_channel_state_t *ch2,
                                            ina3221_channel_state_t *ch3)
{
    uint16_t ech = 0;
    int ret = _ina3221_get_enable_channel(dev, &ech);

    *ch1 = (ech & INA3221_ENABLE_CH1) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    *ch2 = (ech & INA3221_ENABLE_CH2) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    *ch3 = (ech & INA3221_ENABLE_CH3) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    return ret;
}

/**
 * @brief Update number of samples and write to configuration register
 *
 * @param[in, out]  dev Device handle
 * @param[in]       ns Number of samples
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_set_num_samples(ina3221_t *dev, ina3221_num_samples_t ns);

/**
 * @brief Read currently applied number of samples
 *
 * @param[in]       dev Device handle
 * @param[out]      ns Pointer to number of samples output variable
 *
 * @return  INA3221_OK
 */
int ina3221_get_num_samples(const ina3221_t *dev, ina3221_num_samples_t *ns);

/**
 * @brief Update conversion time of bus voltage ADC and write to configuration register
 *
 * @param[in,out]   dev Device handle
 * @param[in]       ctb Bus voltage conversion time
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_set_conv_time_bus_adc(ina3221_t *dev,
                                  ina3221_conv_time_bus_adc_t ctb);

/**
 * @brief Read currently applied bus voltage ADC conversion time
 *
 * @param[in]       dev Device handle
 * @param[out]      ctb Pointer to bus voltage ADC conversion time output variable
 *
 * @return      INA3221_OK
 */
int ina3221_get_conv_time_bus_adc(const ina3221_t *dev,
                                  ina3221_conv_time_bus_adc_t *ctb);

/**
 * @brief Update conversion time of shunt voltage ADC and write to configuration register
 *
 * @param[in,out]   dev Device handle
 * @param[in]       cts Shunt voltage conversion time value
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_set_conv_time_shunt_adc(ina3221_t *dev,
                                    ina3221_conv_time_shunt_adc_t cts);

/**
 * @brief Read currently applied shunt voltage ADC conversion time
 *
 * @param[in]       dev Device handle
 * @param[out]      cts Pointer to shunt voltage ADC conversion time output variable
 *
 * @return      INA3221_OK
 */
int ina3221_get_conv_time_shunt_adc(const ina3221_t *dev,
                                    ina3221_conv_time_shunt_adc_t *cts);

/**
 * @brief Update device operation mode
 *
 * @param[in,out]   dev Device handle
 * @param[in]       mode Operation mode value in host byte order
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_set_mode(ina3221_t *dev, ina3221_mode_t mode);

/**
 * @brief Read currently applied device operation mode
 *
 * @param[in]       dev Device handle
 * @param[out]      mode Pointer to device operation mode output variable
 *
 * @return      INA3221_OK
 */
int ina3221_get_mode(const ina3221_t *dev, ina3221_mode_t *mode);

/**
 * @brief Enable channels for shunt voltage sum calculation
 *
 * @param[in]       dev Device handle
 * @param[in]       esch Enable channel shunt voltage sum flags in host byte order @see ina3221_enable_sum_ch_t
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int _ina3221_set_enable_sum_channel(const ina3221_t *dev,
                                    uint16_t esch);

/**
 * @brief   Wrapper around @see _ina3221_set_enable_sum_channel
 *
 * @param[in]       dev Device handle
 * @param[in]       ch1 Channel 1 state for sum voltage drop calculation
 * @param[in]       ch2 Channel 2 state for sum voltage drop calculation
 * @param[in]       ch3 Channel 3 state for sum voltage drop calculation
 *
 * @return      @see _ina3221_set_enable_sum_channel
 */
static inline int ina3221_set_enable_sum_channel(const ina3221_t *dev,
                                                 ina3221_channel_state_t ch1,
                                                 ina3221_channel_state_t ch2,
                                                 ina3221_channel_state_t ch3)
{
    return _ina3221_set_enable_sum_channel(dev,
                                           (ch1 ? INA3221_ENABLE_SUM_CH1 : 0) |
                                           (ch2 ? INA3221_ENABLE_SUM_CH2 : 0) |
                                           (ch3 ? INA3221_ENABLE_SUM_CH3 : 0));
}

/**
 * @brief Read enabled channels for shunt voltage sum calculation
 *
 * @param[in]       dev Device handle
 * @param[out]      esch Pointer to enabled channels for shunt voltage sum calculation output variable @see ina3221_enable_sum_ch_t
 *
 * @return      Number of enabled channels for shunt voltage sum calculation, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int _ina3221_get_enable_sum_channel(const ina3221_t *dev,
                                    uint16_t *esch);

/**
 * @brief   Wrapper for @see _ina3221_get_enable_sum_channel
 *
 * @param[in]   dev Device handle
 * @param[out]  ch1 Pointer to channel 1 state variable for sum voltage drop calculation
 * @param[out]  ch2 Pointer to channel 2 state variable for sum voltage drop calculation
 * @param[out]  ch3 Pointer to channel 3 state variable for sum voltage drop calculation
 *
 * @return      @see _ina3221_get_enable_sum_channel
 */
static inline int ina3221_get_enable_sum_channel(const ina3221_t *dev,
                                                 ina3221_channel_state_t *ch1,
                                                 ina3221_channel_state_t *ch2,
                                                 ina3221_channel_state_t *ch3)
{
    uint16_t esch = 0;
    int ret = _ina3221_get_enable_sum_channel(dev, &esch);

    *ch1 =
        (esch &
         INA3221_ENABLE_SUM_CH1) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    *ch2 =
        (esch &
         INA3221_ENABLE_SUM_CH2) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    *ch3 =
        (esch &
         INA3221_ENABLE_SUM_CH3) ? INA3221_CH_ENABLE : INA3221_CH_DISABLE;
    return ret;
}

/**
 * @brief Enable latches for critical/warning alert pins
 *
 * @param[in]       dev Device handle
 * @param[in]       latch Enable latch flags
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 * @return      @see i2c_write_regs
 */
int ina3221_set_latch(const ina3221_t *dev, ina3221_enable_latch_t latch);

/**
 * @brief Read enabled latches for critical/warning alert pins
 *
 * @param[in]       dev Device handle
 * @param[in]       latch Pointer to latch flags output variable
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_latch(const ina3221_t *dev, ina3221_enable_latch_t *latch);

/**
 * @brief Set critical shunt voltage alert limit to @p in_uv for each channel in @p ch
 *
 * The function exits on the first channel that could not be updated.
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[in]       in_uv Critical shunt voltage limit in uv
 *
 * @return      Number of channels whose critical shunt voltage alert limit could be set, if any
 * @return      -ERANGE, if @p in_uv was not in [INA3221_MIN_SHUNT_UV; INA3221_MAX_SHUNT_UV]
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int ina3221_set_crit_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t in_uv);

/**
 * @brief Read critical shunt voltage alert limit for each channel in @p ch
 *
 * The function exits on the first channel whose critical alert limit could not be read.
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[out]      out_uv Output array of shunt voltage alert limits in uV
 *
 * @return      Number of channels whose critical shunt voltage alert limits could be read, if any
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_crit_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *out_uv);

/**
 * @brief Set warning shunt voltage alert limit to @p in_uv for each channel in @p ch
 *
 * The function exits on the first channel that could not be updated.
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[in]       in_uv Warning shunt voltage limit in uV in host byte order
 *
 * @return      Number of channels whose warning shunt voltage alert limit could be set, if any
 * @return      -ERANGE, if @p in_uv was not in [INA3221_MIN_SHUNT_UV; INA3221_MAX_SHUNT_UV]
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int ina3221_set_warn_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t in_uv);

/**
 * @brief Read warning shunt voltage alert limit for each channel in @p ch
 *
 * The function exits on the first channel whose warning alert limit could not be read.
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[out]      out_uv Output array of shunt voltage alert limits in uV
 *
 * @return      Number of channels whose warning shunt voltage alert limits could be read, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_warn_alert_limit(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *out_uv);

/**
 * @brief Set shunt voltage sum alert limit to @p in_uv
 *
 * @param[in]       dev Device handle
 * @param[in]       in_uv shunt voltage sum limit in uv
 *
 * @return      INA3221_OK, on success
 * @return      -ERANGE, if @p in_uv was not in [INA3221_MIN_SHUNT_SUM_UV; INA3221_MAX_SHUNT_SUM_UV]
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int ina3221_set_shunt_voltage_sum_alert_limit(const ina3221_t *dev,
                                              int32_t in_uv);

/**
 * @brief Read shunt voltage sum alert limit
 *
 * @param[in]       dev Device handle
 * @param[out]      out_uv Pointer to sum voltage sum alert limit output variable
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_shunt_voltage_sum_alert_limit(const ina3221_t *dev,
                                              int32_t *out_uv);

/**
 * @brief Set bus voltage power valid upper limit to @p in_mv
 *
 * @param[in]       dev Device handle
 * @param[in]       in_mv bus voltage power valid upper limit in mv
 *
 * @return      INA3221_OK, on success
 * @return      -ERANGE, if @p in_mv was not in [INA3221_MIN_BUS_MV; INA3221_MAX_BUS_MV]
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int ina3221_set_power_valid_upper_limit(const ina3221_t *dev, int32_t in_mv);

/**
 * @brief Read bus voltage power valid upper limit
 *
 * @param[in]       dev Device handle
 * @param[out]      out_mv Pointer to bus voltage power valid upper limit output variable
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_power_valid_upper_limit(const ina3221_t *dev, int32_t *out_mv);

/**
 * @brief Set bus voltage power valid lower limit to @p in_mv
 *
 * @param[in]       dev Device handle
 * @param[in]       in_mv bus voltage power valid lower limit in mV
 *
 * @return      INA3221_OK, on success
 * @return      -ERANGE, if @p in_mv was not in [INA3221_MIN_BUS_MV; INA3221_MAX_BUS_MV]
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_write_regs
 */
int ina3221_set_power_valid_lower_limit(const ina3221_t *dev, int32_t in_mv);

/**
 * @brief Read bus voltage power valid lower limit
 *
 * @param[in]       dev Device handle
 * @param[out]      out_mv Pointer to bus voltage power valid lower limit output variable
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_get_power_valid_lower_limit(const ina3221_t *dev, int32_t *out_mv);

/**
 * @brief Read status flags
 *
 * @param[in]       dev Device handle
 * @param[out]      flags Pointer to flags output variable
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_read_flags(const ina3221_t *dev, uint16_t *flags);

/**
 * @brief Read sum of shunt voltages
 *
 * @param[in]       dev Device handle
 * @param[out]      out_uv Pointer to shunt voltage sum output variable
 * @param[out]      flags Pointer to flags output variable, may be NULL
 *
 * @return      INA3221_OK, on success
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_read_shunt_sum_uv(const ina3221_t *dev, int32_t *out_uv,
                              uint16_t *flags);

/**
 * @brief Read shunt voltages for each channel in @p ch
 *
 * The function exits on the first channel whose shunt voltage could not be read
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[out]      out_uv Array of output shunt voltage values in uV
 * @param[out]      flags Pointer to flags output variable, may be NULL
 *
 * @return      Number of channels whose shunt voltages could be read, if any
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_read_shunt_uv(const ina3221_t *dev, ina3221_channel_t ch,
                          int32_t *out_uv, uint16_t *flags);

/**
 * @brief Read bus voltages for each channel in @p ch
 *
 * The function exits on the first channel whose bus voltage could not be read
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[out]      out_mv Array of output bus voltage values in mV
 * @param[out]      flags Pointer to flags output variable, may be NULL
 *
 * @return      Number of channels whose bus voltages could be read, if any
 * @return      -INA3221_I2C_ERROR, if I2C bus acquirement failed
 * @return      @see i2c_read_regs
 */
int ina3221_read_bus_mv(const ina3221_t *dev, ina3221_channel_t ch,
                        int16_t *out_mv, uint16_t *flags);

/**
 * @brief Calculate current for each channel in @p ch
 *
 * @param[in]       dev Device handle
 * @param[in]       ch Channel flags
 * @param[in]       in_uv Array of input shunt voltage values
 * @param[out]      out_ma Array of output current values
 *
 * @return      Number of clculated current values
 */
int ina3221_calculate_current_ua(const ina3221_t *dev, ina3221_channel_t ch,
                                 int32_t *in_uv, int32_t *out_ma);

/**
 * @brief Calculate power from bus voltage and current values
 *
 * @param[in]       in_mv Array of input bus voltage values in mV
 * @param[in]       in_ua Array of input current values in uA
 * @param[in]       num Number of values in @p in_mv
 * @param[out]      out_mw Array of output power values in uW
 *
 * @return      Number of calculated power values, on success
 * @return      -ERANGE, if @p num is greater than INA3221_NUM_CH
 */
int ina3221_calculate_power_uw(int16_t *in_mv, int32_t *in_ua, uint8_t num,
                               int32_t *out_mw);

/**
 * @brief Align @p in_res to the number of channels
 *        For example: @p ch = (INA3221_CH1 | INA3221_CH3)
 *                     @p in_res = {value_ch1, value_ch3}, then
 *                     @p out_res will be {value_ch1, 0, value_ch3}
 *
 * @param[in]       ch Channel flags
 * @param[in]       in_res Output of e.g. @see ina3221_calculate_current_ua
 * @param[out]      out_res Channel aligned result
 * @param[in]       res_val_size Size of a value in @p in_res in bytes
 */
void ina3221_ch_align(ina3221_channel_t ch, const void *in_res, void *out_res,
                      size_t res_val_size);

/**
 * @brief Set operation mode to INA3221_MODE_TRIGGER_SHUNT_ONLY to trigger shunt voltage measurement
 *
 * @param       dev Device handle
 */
#define INA3221_TRIGGER_SHUNT(dev) \
    ina3221_set_mode(dev, INA3221_MODE_TRIGGER_SHUNT_ONLY)

/**
 * @brief Set operation mode to INA3221_MODE_TRIGGER_BUS_ONLY to trigger bus voltage measurement
 *
 * @param       dev Device handle
 */
#define INA3221_TRIGGER_BUS(dev)   \
    ina3221_set_mode(dev, INA3221_MODE_TRIGGER_BUS_ONLY)

/**
 * @brief Set operation mode to INA3221_MODE_TRIGGER_SHUNT_BUS to trigger shunt and bus voltage measurement
 *
 * @param       dev Device handle
 */
#define INA3221_TRIGGER_SHUNT_AND_BUS(dev) \
    ina3221_set_mode(dev, INA3221_MODE_TRIGGER_SHUNT_BUS)

#ifdef __cplusplus
}
#endif

#endif /* INA3221_H */
/** @} */
