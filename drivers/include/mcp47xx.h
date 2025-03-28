/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MCP47XX_H
#define MCP47XX_H

/**
 * @defgroup    drivers_mcp47xx MCP47xx DAC with I2C interface
 * @ingroup     drivers_saul
 * @ingroup     drivers_actuators
 * @brief       Device driver for Microchip MCP47xx DAC with I2C interface
 *
 * ## Overview
 *
 * The driver supports the different Microchip MCP47xx DAC variants.
 *
 * Expander | Channels | Resolution
 * :--------|:--------:|:----------:
 * MCP4706  | 1        |  8-bit
 * MCP4716  | 1        | 10-bit
 * MCP4725  | 1        | 12-bit
 * MCP4726  | 1        | 12-bit
 * MCP4728  | 4        | 12-bit
 *
 * @note The following features of MCP47xx DAC devices are not supported at the
 * moment:
 * - configuring and reading address bits to/from EEPROM
 * - writing DAC channel output values to the EEPROM
 * - setting the UDAC bit and using the LDAC pin for MCP4728
 *
 * ## Usage
 *
 * Multiple MCP47xx DAC devices and different variants can be used at the
 * same time.
 *
 * The driver interface is kept as compatible as possible with the peripheral
 * DAC interface. The only differences are that
 *
 * - functions have the prefix `mcp47xx_` and
 * - functions require an additional parameter, the pointer to the MCP47xx
 *   device of type #mcp47xx_t.
 *
 * Please refer the test application in `tests/drivers/mcp47xx` for an example
 * on how to use the driver.
 *
 * ## SAUL Capabilities
 *
 * The driver provides SAUL capabilities that are compatible with SAUL
 * actuators of type #SAUL_ACT_DIMMER.
 * Each MCP47xx channel can be mapped directly to SAUL by defining an
 * according entry in \c MCP47XX_SAUL_DAC_PARAMS. Please refer file
 * `$RIOTBASE/drivers/mcp47xx/include/mcp47xx_params.h` for an example.
 *
 *      mcp47xx_saul_dac_params_t mcp47xx_saul_dac_params[] = {
 *          {
 *              .name = "DAC00",
 *              .dev = 0,
 *              .channel = 0,
 *              .initial = 32768,
 *          },
 *      };
 *
 * For each DAC channel that should be used with SAUL, an entry with a name,
 * the device, the channel, and the initial value has to be defined as shown
 * above.
 *
 * ## Using Multiple Devices
 *
 * It is possible to used multiple devices and different variants of MCP47xx
 * DAC devices at the same time. The application has to configure all
 * devices by defining the configuration parameter set `mcp47xx_params`
 * of type #mcp47xx_params_t. As an example, the default configuration for
 * one MCP4725 device is defined in `drivers/mcp47xx/mcp47xx_params.h`.
 *
 * The application can override it by placing a file `mcp47xx_params.h` in
 * the application directory `$(APPDIR)`. For example, the definition of
 * the configuration parameter set for the two devices (one MCP4725 and one
 * MCP4728) could looks like:
 *
 *      static const mcp47xx_params_t mcp47xx_params[] = {
 *          {
 *              .variant = MCP4725,
 *              .dev = I2C_DEV(0),
 *              .addr = MCP47XX_BASE_ADDR + 2,
 *              .gain = MCP47XX_GAIN_1X,
 *              .vref = MCP47XX_VDD,
 *              .pd_mode = MCP47XX_PD_LARGE,
 *          },
 *          {
 *              .variant = MCP4728,
 *              .dev = I2C_DEV(0),
 *              .addr = MCP47XX_BASE_ADDR + 3,
 *              .gain = MCP47XX_GAIN_2X,
 *              .vref = MCP47XX_VREF_INT,
 *              .pd_mode = MCP47XX_PD_LARGE,
 *          },
 *      };
 *
 * @{
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>

#include "kernel_defines.h"
#include "periph/dac.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

/**
 * @name    MCP47xx I2C slave addresses
 *
 * MCP47xx I2C slave addresses are defined as an offset to a base address,
 * which depends on the expander used. The address offset is in the range
 * of 0 to 7.
 * @{
 */
#define MCP47XX_BASE_ADDR   (0x60)  /**< MCP47xx I2C slave base address.
                                         Addresses are then in the range from
                                         0x60 to 0x67 */
/** @} */

/**
 * @name    MCP47xx Channel number
 * @{
 */
#define MCP4706_CHN_NUM     (1)  /**< MCP4706 has 1 channel */
#define MCP4716_CHN_NUM     (1)  /**< MCP4716 has 1 channel */
#define MCP4725_CHN_NUM     (1)  /**< MCP4725 has 1 channel */
#define MCP4726_CHN_NUM     (1)  /**< MCP4726 has 1 channel */
#define MCP4728_CHN_NUM     (4)  /**< MCP4728 has 4 channels */
#define MCP47XX_CHN_NUM_MAX (4)  /**< maximum number of channels */
/** @} */

/**
 * @brief   MCP47xx driver error codes */
typedef enum {
    MCP47XX_OK,                 /**< success */
    MCP47XX_ERROR_I2C,          /**< I2C communication error */
    MCP47XX_ERROR_NOT_AVAIL,    /**< device not available */
} mcp47xx_error_codes_t;

/**
 * @brief   Supported MCP47xx variants
 *
 * It is used in configuration parameters to specify the MCP47xx expander
 * used by device.
 */
typedef enum {
    MCP4706,    /**< 1 channel 8-bit DAC */
    MCP4716,    /**< 1 channel 10-bit DAC */
    MCP4725,    /**< 1 channel 12-bit DAC */
    MCP4726,    /**< 1 channel 12-bit DAC */
    MCP4728,    /**< 4 channel 12-bit DAC */
} mcp47xx_variant_t;

/**
 * @brief   MCP47xx gain configuration type
 *
 * @note Gains are not supported by MCP4725.
 */
typedef enum {
    MCP47XX_GAIN_1X = 0,  /**< Gain is 1.0, supported by all MCP47xx variants */
    MCP47XX_GAIN_2X = 1,  /**< Gain is 2.0, not supported by MCP4725 */
} mcp47xx_gain_t;

/**
 * @brief   MCP47xx V_REF configuration type
 *
 * @note Different MCP47xx variants allow different V_REF configurations
 */
typedef enum {
    MCP47XX_VREF_VDD = 0,   /**< V_REF = V_DD, supported by all MCP47xx */
    MCP47XX_VREF_INT = 1,   /**< V_REF = internal (2.048 V), MCP4728 only  */
    MCP47XX_VREF_PIN = 2,   /**< V_REF = VREF pin not buffered, MCP47x6 only */
    MCP47XX_VREF_BUF = 3,   /**< V_REF = VREF pin buffered, MCP47x6 only */
} mcp47xx_vref_t;

/**
 * @brief   MCP47xx Power-down mode selection type
 *
 * Defines the possible power-down modes used for MCP47xx device configuration.
 * The mode is used by function #mcp47xx_dac_poweroff to set the DAC into the
 * configured power-down mode.
 *
 * @note #MCP47XX_NORMAL cannot be configured as power-down mode.
 */
typedef enum {
    MCP47XX_NORMAL = 0,     /**< Normal mode */
    MCP47XX_PD_SMALL = 1,   /**< Power down, small resistor 1 kOhm */
    MCP47XX_PD_MEDIUM = 2,  /**< Power down, medium resistor,
                                 125 kOhm for MCP47x6, 100 kOhm otherwise */
    MCP47XX_PD_LARGE = 3,   /**< Power down, large resistor,
                                 640 kOhm for MCP47x6, 125 kOhm otherwise */
} mcp47xx_pd_mode_t;

/**
 * @brief   MCP47xx device configuration parameters
 */
typedef struct {

    i2c_t    dev;       /**< I2C device */
    uint16_t addr;      /**< I2C slave address MCP47XX_BASE_ADDR + [0...7] */

    mcp47xx_variant_t   variant;    /**< used variant of MCP47xx */
    mcp47xx_gain_t      gain;       /**< Gain selection */
    mcp47xx_vref_t      vref;       /**< Voltage reference selection */
    mcp47xx_pd_mode_t   pd_mode;    /**< Power-down mode selection */

} mcp47xx_params_t;

/**
 * @brief   MCP47xx device data structure type
 */
typedef struct {
    mcp47xx_params_t params;              /**< device configuration parameters */
    uint16_t values[MCP47XX_CHN_NUM_MAX]; /**< contains the last values set
                                               for persistence when device is
                                               powered off */
} mcp47xx_t;

#if IS_USED(MODULE_SAUL) || DOXYGEN
/**
 * @brief   MCP47xx configuration structure for mapping DAC channels to SAUL
 */
typedef struct {
    const char *name;   /**< name of the MCP47xx device */
    unsigned int dev;   /**< index of the MCP47xx device */
    uint8_t channel;    /**< channel of the MCP47xx device */
    uint16_t initial;   /**< initial value */
} mcp47xx_saul_dac_params_t;
#endif

/**
 * @brief   Initialize the MCP47xx DAC
 *
 * All expander pins are set to be input and are pulled up.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   params  configuration parameters, see #mcp47xx_params_t
 *
 * @retval  MCP47XX_OK      on success
 * @retval  MCP47XX_ERROR_* a negative error code on error,
 *                           see #mcp47xx_error_codes_t
 */
int mcp47xx_init(mcp47xx_t *dev, const mcp47xx_params_t *params);

/**
 * @brief   Initialize a MCP47xx DAC channel
 *
 * After the initialization, the DAC channel is active and its output is set
 * to 0.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   chn     channel to initialize
 *
 * @retval  MCP47XX_OK          on success
 * @retval  MCP47XX_ERROR_*     a negative error code on error,
 *                              see #mcp47xx_error_codes_t
 */
int mcp47xx_dac_init(mcp47xx_t *dev, uint8_t chn);

/**
 * @brief   Write a value to a MCP47xx DAC channel
 *
 * The value is always given as 16-bit value and is internally scaled to the
 * actual resolution that the DAC unit provides, e.g., 12-bit. So to get the
 * maximum output voltage, this function has to be called with value set
 * to 65535 (UINT16_MAX).
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   chn     channel to set
 * @param[in]   value   value to set line to
 *
 * @retval  none
 */
void mcp47xx_dac_set(mcp47xx_t *dev, uint8_t chn, uint16_t value);

/**
 * @brief   Get the current value of a MCP47xx DAC channel
 *
 * The value is always given as 16-bit value and is internally scaled to the
 * actual resolution that the DAC unit provides, e.g., 12-bit.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   chn     channel to set
 * @param[out]  value   value to set line to
 *
 * @retval  none
 */
void mcp47xx_dac_get(mcp47xx_t *dev, uint8_t chn, uint16_t *value);

/**
 * @brief   Enables the MCP47xx DAC device
 *
 * MCP47xx is enabled and the output is set to the last set value.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   chn     channel to power on
 * @retval      none
 */
void mcp47xx_dac_poweron(mcp47xx_t *dev, uint8_t chn);

/**
 * @brief   Disables the MCP47xx DAC device
 *
 * MCP47xx is switched to the power-down mode configured by the configuration
 * parameter mcp47xx_params_t::pd_mode. V_OUT is loaded with the configured
 * resistor to ground. Most of the channel circuits are powered off.
 *
 * @note If #MCP47XX_NORMAL is used as power-down mode, the DAC can't be
 * powerd off.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @param[in]   chn     channel to power on
 * @retval      none
 */
void mcp47xx_dac_poweroff(mcp47xx_t *dev, uint8_t chn);

/**
 * @brief   Returns the number of channels of MCP47xx DAC device
 *
 * This function returns the number of channels of the device MCP47xx DAC
 * device.
 *
 * @param[in]   dev     descriptor of the MCP47xx DAC device
 * @retval      number of channels on success or 0 on error
 */
uint8_t mcp47xx_dac_channels(mcp47xx_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MCP47XX_H */
