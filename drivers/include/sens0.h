/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_sens0 Sens0 simple sensor driver
 * @ingroup     drivers_sensors
 * @brief       Sens0 provides a generic interface for simple digital sensors
 *
 * The sens0 driver provides an easy way to interface with sensors when the
 * application only requires a basic measurement functionality. The sensor
 * configuration consists of a hardware specification, @ref sens0_specs_t;
 * connection parameters, @ref sens0_i2c_params_t, or TODO sens0_spi_params_t.
 *
 * The hardware specifications are device specific and describes the
 * initialization procedure, as well as the location and format of the sensor
 * output registers. All information required for the hardware specification
 * should be available in the data sheet for the sensor.
 *
 * The connection parameters are application or board specific and will be found
 * in the electric schematic for the board in combination with the sensor data
 * sheet. The connection information contains which bus to use, which CS line
 * (SPI connected sensors) or device address (I2C connected sensors) to use for
 * accessing the device.
 *
 * A custom conversion function can be supplied for when the output format is
 * not linear, or otherwise incompatible with the default conversion function.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the Sens0 driver.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/**
 * @defgroup    drivers_sens0_specs Sens0 supported devices
 * @ingroup     drivers_sens0
 * @brief       Sens0 hardware specifications for interfacing with devices.
 */

#ifndef SENS0_H
#define SENS0_H

#include <stdint.h>
#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sensor output format flags
 */
typedef enum {
    /** Set this flag if raw values are in big endian byte order on the device */
    SENS0_BIG_ENDIAN = (1 <<  0),
    /** This flag ensures that the sign bit is preserved correctly in the default conversion */
    SENS0_SIGNED     = (1 <<  1),
} sens0_output_flags_t;

/**
 * @brief   Register write description
 *
 * This is used in the initialization procedure to specify where and what to
 * write to the device registers.
 */
typedef struct {
    uint8_t  reg;   /**< Register address */
    uint8_t  value; /**< Register value */
} sens0_reg_t;

/**
 * @brief   Register bits description
 *
 * This data type is used to specify only specific bits of an 8 bit register.
 */
typedef struct {
    uint8_t  reg;   /**< Register address */
    uint8_t  mask;  /**< Bit mask used to select specific bits */
    uint8_t  value; /**< Register value */
} sens0_reg_masked_t;

/**
 * @brief   Sensor output specification
 *
 * This struct describes where to find values in the hardware registers
 */
typedef struct sens0_output {
    uint8_t flags;     /**< Conversion flags, see @ref sens0_output_flags_t */
    /**
     * @brief   Number of values
     */
    uint8_t nelem;
    /**
     * @brief   Length of each value
     *
     * It is not defined what happens if the size is > 4
     */
    uint8_t elem_size;
    /**
     * @brief   Hardware register address
     */
    uint8_t reg;
    /**
     * @brief   Optional custom function to convert the raw data to @ref phydat_t
     *
     * @param[in]       specs   pointer to the specification
     * @param[out]      data    pointer to output buffer
     * @param[in]       raw     pointer to raw input buffer from sensor
     *
     * @return          number of elements converted on success
     * @return          <0 on failure
     */
    int (*conv_func)(const struct sens0_output *specs, phydat_t *data, const uint8_t *raw);
    /**
     * @brief   Scaling fraction numerator
     *
     * Used by the default conversion function when `conv_func == NULL`
     */
    int32_t conv_frac_num;
    /**
     * @brief   Scaling fraction denominator
     *
     * Used by the default conversion function when `conv_func == NULL`
     */
    int32_t conv_frac_den;
    /**
     * @brief   Output value unit, see @ref phydat_t
     */
    uint8_t unit;
    /**
     * @brief   Output value scale exponent, see @ref phydat_t
     */
    uint8_t scale;
} sens0_output_t;

/**
 * @brief   Device hardware specifications
 */
typedef struct {
    /**
     * @brief   Device identification register address
     *
     * Sometimes also called a WHO_AM_I register.
     * During sens0_init, this register at address id.reg will be read and the
     * contents will be compared against id.value, to ensure that the driver is
     * talking to the correct device. If the value is not the expected value, the
     * initialization will abort and an error code will be returned.
     *
     * The actual value is combined with id.mask to perform a check of only the
     * masked bits:
     *
     *     if ((actual_value & id.mask) != id.value) { return error... }
     */
    sens0_reg_masked_t id;
    /**
     * @brief   Number of reset registers to write
     */
    uint8_t  reset_regs_nelem;
    /**
     * @brief   reset write sequence
     *
     * The register values specified in this vector will be written, in order,
     * to the device by @ref sens0_i2c_init
     */
    const sens0_reg_t *reset_regs;
    /**
     * @brief   Reset delay
     *
     * The initialization will sleep after the reset sequence has completed to
     * let the hardware complete the operation.
     */
    uint32_t reset_delay;
    /**
     * @brief   Number of initialization registers to write
     */
    uint8_t  init_regs_nelem;
    /**
     * @brief   initialization write sequence
     *
     * The register values specified in this vector will be written, in order,
     * to the device by @ref sens0_i2c_init
     */
    const sens0_reg_t *init_regs;
    /**
     * @brief   sensor measurement request register
     *
     * The masked bits will be written to initiate a measurement.
     *
     * Set measurement_req.mask = 0x00 if the device does not need explicit
     * measurement requests.
     */
    sens0_reg_masked_t measurement_req;
    /**
     * @brief   Time in microseconds to wait after a measurement request before
     * reading the result register
     */
    uint32_t measurement_delay;
    /**
     * @brief   Number of outputs
     */
    uint8_t  outputs_nelem;
    /**
     * @brief   Output descriptions
     */
    const sens0_output_t *outputs;
    /**
     * @brief   Power state register address
     */
    uint8_t  power_reg;
    /**
     * @brief   Bit mask for standby mode setting in power reg
     */
    uint8_t  power_mask;
    /**
     * @brief   Value for putting the device in 'active' state
     */
    uint8_t  power_value_active;
    /**
     * @brief   Value for putting the device in 'standby' state
     */
    uint8_t  power_value_standby;
} sens0_specs_t;

/* These functions are prefixed sens0_i2c_, but are located here to avoid
 * dependency on periph/i2c.h for specs implementations. The specs must be
 * compilable without any particular hardware interface configured or enabled */

/**
 * @brief   Read value from the first output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 0
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output0(const void *dev, phydat_t *res);

/**
 * @brief   Read value from the second output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 1
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output1(const void *dev, phydat_t *res);

/**
 * @brief   Read value from the third output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 2
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output2(const void *dev, phydat_t *res);

#ifdef __cplusplus
}
#endif

#endif /* SENS0_H */
/** @} */
