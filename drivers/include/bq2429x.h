/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BQ2429X_H
#define BQ2429X_H

/**
 * @defgroup    drivers_bq2429x BQ2429x
 * @ingroup     drivers_power
 * @brief       Device driver interface for the BQ2429x Single-Cell USB Charger
 *
 * BQ2429x series power management ICs by Texas Instruments are controllable by
 * I2C, allowing to change charge parameters in the runtime.
 *
 * For more information, see the datasheets:
 * * [BQ24295](https://www.ti.com/lit/ds/symlink/bq24295.pdf)
 * * [BQ24296](https://www.ti.com/lit/ds/symlink/bq24296.pdf)
 * * [BQ24297](https://www.ti.com/lit/ds/symlink/bq24297.pdf)
 * * [BQ24298](https://www.ti.com/lit/ds/symlink/bq24298.pdf)
 * * [BQ24292I](https://www.ti.com/lit/ds/symlink/bq24292i.pdf)
 * * [BQ24296M](https://www.ti.com/lit/ds/symlink/bq24296m.pdf)
 *
 * It can be used for USB OTG to power other devices, see the
 * @ref bq2429x_enable_otg and @ref bq2429x_disable_otg functions,
 * an additional pin @ref bq2429x_params_t::otg_pin can be set to
 * control it also by hardware and these functions will take care of
 * setting it.
 *
 * To enable/disable charge the functions @ref bq2429x_enable_charge
 * and @ref bq2429x_disable_charge can be used, and as with the OTG
 * an additional pin @ref bq2429x_params_t::ce_pin can be set to
 * control it also by hardware and these functions will take care of
 * setting it.
 *
 * When a change happens on the FAULT registers of the device the
 * BQ2429x device generates an interrupt to inform when that happens, this
 * functionality has to be enabled with the `bq2429x_int` module and can be used
 * with the @ref bq2429x_init_int functions.
 *
 * @{
 *
 * @file
 * @brief       Device driver interface for the BQ2429x Single-Cell USB Charger
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include <stdint.h>
#include <stdbool.h>

#include "kernel_defines.h"
#include "periph/i2c.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Driver return values
 */
enum {
    BQ2429X_OK              = 0,        /**< Everything is ok */
    BQ2429X_ERR_I2C         = -1,       /**< I2C error */
    BQ2429X_ERR_GPIO        = -2,       /**< GPIO initialization error */
};

/**
 * @brief   Used VBUS type.
 */
typedef enum {
    BQ2429X_VBUS_NONE   = 0,            /**< No input source detected */
    BQ2429X_VBUS_USB_CHARGER,           /**< USB charger input */
    BQ2429X_VBUS_AP_CHARGER,            /**< Adapter port charge input */
    BQ2429X_VBUS_OTG,                   /**< VBUS is used for OTG */
} bq2429x_vbus_stat_t;

/**
 * @brief   Battery charging status
 */
typedef enum {
    BQ2429X_CHARGE_NONE = 0,            /**< The device is not charging a
                                             battery */
    BQ2429X_CHARGE_PRE,                 /**< Pre-charge state */
    BQ2429X_CHARGE_FAST,                /**< Fast charging */
    BQ2429X_CHARGE_DONE,                /**< Charging done */
} bq2429x_chrg_stat_t;

/**
 * @brief   Device status
 */
typedef struct {
    bq2429x_vbus_stat_t vbus;   /**< VBUS status */
    bq2429x_chrg_stat_t chrg;   /**< Charge status */
    /**
     * @brief   DPM status
     * @details false = Not in DPM.
     *          true = VINDPM or IINDMP.
     */
    bool dpm;
    /**
     * @brief   Power good status
     * @details false = Not Power Good
     *          true  = Power Good
     */
    bool pg;
    /**
     * @brief   Thermal regulation
     * @details false = Normal
     *          true  = In thermal regulation
     */
    bool therm;
    /**
     * @brief   VSYSMIN regulation
     * @details false = Not in VSYSMIN regulation (BAT voltage > VSYSMIN)
     *          true  = In VSYSMIN regulation (BAT voltage < VSYSMIN)
     */
    bool vsys;
} bq2429x_status_t;

/**
 * @brief   Charge fault values
 */
typedef enum {
    BQ2429x_CHRG_FAULT_NORMAL = 0,              /**< No fault, normal */
    BQ2429x_CHRG_FAULT_INPUT,                   /**< Input fault (OVP or bad
                                                     source) */
    BQ2429x_CHRG_FAULT_THERMAL_SHUTDOWN,        /**< Thermal shutdown */
    BQ2429x_CHRG_FAULT_CHARGE_TIMER_EXPIRATION, /**< Charge timer expiration */
} bq2429x_chrg_fault_t;

/**
 * @brief   Device faults
 */
typedef struct {
    /**
     * @brief   Watchdog fault.
     * @details false = Normal.
     *          true  = Watchdog timer expiration.
     */
    bool watchdog;
    /**
     * @brief   OTG fault.
     * @details false = Normal.
     *          true  = VBUS overloaded in OTG, or VBUS OVP, or battery is too
     *          low.
     */
    bool otg;
    /**
     * @brief   Charge fault.
     */
    bq2429x_chrg_fault_t chrg;
    /**
     * @brief   Battery fault.
     */
    bool bat;
    /**
     * @brief   NTC fault (1).
     * @details false = Normal.
     *          true  = Hot note.
     */
    bool ntc_fault_1;
    /**
     * @brief   NTC fault (0).
     * @details false = Normal.
     *          true  = Cold note.
     */
    bool ntc_fault_0;
} bq2429x_fault_t;

/**
 * @brief   Input Voltage Limit (VLIM).
 */
typedef enum {
    BQ2429X_VLIM_3880 = 0,              /**< 3880 mV */
    BQ2429X_VLIM_3960,                  /**< 3960 mV */
    BQ2429X_VLIM_4040,                  /**< 4040 mV */
    BQ2429X_VLIM_4120,                  /**< 4120 mV */
    BQ2429X_VLIM_4200,                  /**< 4200 mV */
    BQ2429X_VLIM_4280,                  /**< 4280 mV */
    BQ2429X_VLIM_4360,                  /**< 4360 mV */
    BQ2429X_VLIM_4440,                  /**< 4440 mV */
    BQ2429X_VLIM_4520,                  /**< 4520 mV */
    BQ2429X_VLIM_4600,                  /**< 4600 mV */
    BQ2429X_VLIM_4680,                  /**< 4680 mV */
    BQ2429X_VLIM_4760,                  /**< 4760 mV */
    BQ2429X_VLIM_4840,                  /**< 4840 mV */
    BQ2429X_VLIM_4920,                  /**< 4920 mV */
    BQ2429X_VLIM_5000,                  /**< 5000 mV */
    BQ2429X_VLIM_5080,                  /**< 5080 mV */
} bq2429x_input_voltage_limit_t;

/**
 * @brief   Input Current Limit (ILIM).
 */
typedef enum {
    BQ2429X_ILIM_100 = 0,               /**< 100 mA */
    BQ2429X_ILIM_150,                   /**< 150 mA */
    BQ2429X_ILIM_500,                   /**< 500 mA */
    BQ2429X_ILIM_900,                   /**< 900 mA */
    BQ2429X_ILIM_1000,                  /**< 1000 mA */
    BQ2429X_ILIM_1500,                  /**< 1500 mA */
    BQ2429X_ILIM_2000,                  /**< 2000 mA */
    BQ2429X_ILIM_3000,                  /**< 3000 mA */
} bq2429x_input_current_limit_t;

/**
 * @brief   Charge Current (ICHG)
 */
typedef enum {
    BQ2429X_ICHG_512    = 0,            /**< 512 mA */
    BQ2429X_ICHG_1024   = 8,            /**< 1024 mA */
    BQ2429X_ICHG_2048   = 24,           /**< 2048 mA */
    BQ2429X_ICHG_3008   = 39,           /**< 4032 mA */
    BQ2429X_ICHG_4032   = 55,           /**< 4544 mA */
    BQ2429X_ICHG_4544   = 63,
} bq2429x_charge_current_t;

/**
 * @brief   Charge Voltage Limit (VREG).
 */
typedef enum {
    BQ2429X_VREG_3504   = 0,            /**< 3504 mV */
    BQ2429X_VREG_3600   = 6,            /**< 3600 mV */
    BQ2429X_VREG_3808   = 19,           /**< 3808 mV */
    BQ2429X_VREG_3904   = 25,           /**< 3904 mV */
    BQ2429X_VREG_4000   = 31,           /**< 4000 mV */
    BQ2429X_VREG_4112   = 38,           /**< 4112 mV */
    BQ2429X_VREG_4208   = 44,           /**< 4208 mV */
    BQ2429X_VREG_4304   = 50,           /**< 4304 mV */
    BQ2429X_VREG_4352   = 53,           /**< 4352 mV */
    BQ2429X_VREG_4400   = 56,           /**< 4400 mV */
} bq2429x_charge_voltage_limit_t;

/**
 * @brief   BQ2429x device parameters
 */
typedef struct {
    i2c_t i2c;                              /**< I2C device */
#if IS_USED(MODULE_BQ2429X_INT)
    gpio_t int_pin;                         /**< Interrupt pin */
#endif
    gpio_t ce_pin;                          /**< Charge Enable pin (optional) */
    gpio_t otg_pin;                         /**< OTG Enable pin (optional) */
    bq2429x_input_voltage_limit_t vlim;     /**< Voltage limit */
    bq2429x_input_current_limit_t ilim;     /**< Current limit */
    bq2429x_charge_current_t ichg;          /**< Charge current limit */
    bq2429x_charge_voltage_limit_t vreg;    /**< Charge voltage limit */
} bq2429x_params_t;

/**
 * @brief   BQ2429x device descriptor
 */
typedef struct {
    bq2429x_params_t params;            /**< Device parameters */
} bq2429x_t;

/**
 * @brief   Initialize device
 *
 * @pre @p dev != NULL && @p params != NULL
 *
 * @param[out]  dev     Device descriptor.
 * @param[in]   params  Device parameters.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 * @return BQ2429X_ERR_GPIO on GPIO initialization failure (CE or OTG pin
 *         failed).
 */
int bq2429x_init(bq2429x_t *dev, const bq2429x_params_t *params);

#if IS_USED(MODULE_BQ2429X_INT) || DOXYGEN
/**
 * @brief   Callback function for BQ2429x interrupts.
 */
typedef void (* bq2429x_int_cb_t)(void *);

/**
 * @brief   Initialize interrupt support for the device.
 *
 * @pre @p dev != NULL && @p cb != NULL
 *
 * The callback @p cb is called in an ISR context, so keep in mind that heavy
 * work shouldn't be done there.
 *
 * @note @ref bq2429x_init MUST have been called before!
 *
 * @param[in]   dev     Device descriptor.
 * @param[in]   cb      Callback called on interrupt.
 * @param[in]   arg     Argument to be passed when the callback is called.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_GPIO on GPIO initialization failure.
 */
int bq2429x_init_int(bq2429x_t *dev, bq2429x_int_cb_t cb, void *arg);
#endif /* IS_USED(MODULE_BQ2429X_INT) || DOXYGEN */

/**
 * @brief   Get device status.
 *
 * @pre @p dev != NULL && @p status != NULL
 *
 * @param[in]   dev         Device descriptor.
 * @param[out]  status      Pointer where device status will be stored.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C error.
 */
int bq2429x_get_status(const bq2429x_t *dev, bq2429x_status_t *status);

/**
 * @brief   Get device faults.
 *
 * @pre @p dev != NULL && @p fault != NULL
 *
 * @param[in]   dev         Device descriptor.
 * @param[out]  fault       Pointer where device faults will be stored.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C error.
 */
int bq2429x_get_fault(const bq2429x_t *dev, bq2429x_fault_t *fault);

/**
 * @brief   Enable OTG.
 *
 * This allows powering USB devices from the same port, i.e. to provide power
 * or charge other devices.
 *
 * @note This will disable charging of the battery, if previously
 *       @ref bq2429x_enable_charge was called, to enable charge
 *       again only disable OTG mode with @ref bq2429x_disable_otg.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev Device descriptor.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_enable_otg(const bq2429x_t *dev);

/**
 * @brief   Disable OTG.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev Device descriptor.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_disable_otg(const bq2429x_t *dev);

/**
 * @brief   Enable battery charging.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev Device descriptor.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_enable_charge(const bq2429x_t *dev);

/**
 * @brief   Disable battery charging.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev Device descriptor.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_disable_charge(const bq2429x_t *dev);

/**
 * @brief   Set Input Voltage Limit.
 *
 * @note This is a limit on the lower bound of the voltage, for example,
 * if the VLIM is set to @ref BQ2429X_VLIM_5000 (5.0V) and the supply
 * voltage is lower than it (4.8V), it will result in a fault and no
 * power will be delivered through the output pin (SYS pin).
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[in]   vlim    Voltage limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_set_vlim(bq2429x_t *dev, bq2429x_input_voltage_limit_t vlim);

/**
 * @brief   Get Input Voltage Limit.
 *
 * @pre @p dev != NULL && @p vlim != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[out]  vlim    Voltage limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_get_vlim(const bq2429x_t *dev, bq2429x_input_voltage_limit_t *vlim);

/**
 * @brief   Set Input Current Limit.
 *
 * Sets the maximum current limit, this limit is also limited by hardware by
 * setting a resistor to the VLIM pin which sets (by hardware) the upper limit.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[in]   ilim    Current limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_set_ilim(bq2429x_t *dev, bq2429x_input_current_limit_t ilim);

/**
 * @brief   Get Input Current Limit.
 *
 * @pre @p dev != NULL && @p ilim != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[out]  ilim    Current limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_get_ilim(const bq2429x_t *dev, bq2429x_input_current_limit_t *ilim);

/**
 * @brief   Set Charge Current.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[in]   ichg    Charge current.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_set_ichg(bq2429x_t *dev, bq2429x_charge_current_t ichg);

/**
 * @brief   Get Charge Current.
 *
 * @pre @p dev != NULL && @p ichg != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[out]  ichg    Charge current.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_get_ichg(const bq2429x_t *dev, bq2429x_charge_current_t *ichg);

/**
 * @brief   Set Charge Voltage Limit.
 *
 * @pre @p dev != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[in]   vreg    Voltage limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_set_vreg(bq2429x_t *dev, bq2429x_charge_voltage_limit_t vreg);

/**
 * @brief   Get Charge Voltage Limit.
 *
 * @pre @p dev != NULL && @p vreg != NULL
 *
 * @param[in]   dev     Device descriptor.
 * @param[out]  vreg    Voltage limit.
 *
 * @return BQ2429X_OK on success.
 * @return BQ2429X_ERR_I2C on I2C failure.
 */
int bq2429x_get_vreg(const bq2429x_t *dev,
                     bq2429x_charge_voltage_limit_t *vreg);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BQ2429X_H */
