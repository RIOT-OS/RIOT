/*
 * Copyright (C) 2025 David Picard
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_max31865_constants MAX31865 constants
 * @ingroup     drivers_max31865
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      David Picard
 */

#ifdef __cplusplus
extern "C" {
#endif
/** Maximum number of bytes that can be read/written from/to the device */
#define MAX31865_DATA_SIZE                  (8)

/** Reference resistance (ohm) */
#define MAX31865_REF_RESISTANCE_DEFAULT     (220)

/** Default value of the high threshold register */
#define MAX31865_HTHRES_DEFAULT             (0xFFFF)

/** Default value of the low threshold register */
#define MAX31865_LTHRES_DEFAULT             (0x0000)

/**
 * @defgroup drivers_max31865_reg_addresses Register addresses
 * @ingroup drivers_max31865_constants
 * @{
 *
 * Register                | Read address  | Write address
 * ---                     | ---           | ---
 * Configuration           | 0x00          | 0x80
 * RTD MSB                 | 0x01          | N/A
 * RTD LSB                 | 0x02          | N/A
 * RTD high threshold MSB  | 0x03          | 0x83
 * RTD high threshold LSB  | 0x04          | 0x84
 * RTD low threshold MSB   | 0x05          | 0x85
 * RTD low threshold LSB   | 0x06          | 0x86
 * Fault                   | 0x07          | N/A
 */

#define MAX31865_ADDR_CFG_R                 (0x00)  /**< Configuration register, read address */
#define MAX31865_ADDR_CFG_W                 (0x80)  /**< Configuration register, write address */
#define MAX31865_ADDR_RTD_MSB               (0x01)  /**< RTD MSB, read-only */
#define MAX31865_ADDR_RTD_LSB               (0x02)  /**< RTD LSB, read-only */
#define MAX31865_ADDR_RTD_HTHRES_MSB_R      (0x03)  /**< RTD high threshold MSB, read address */
#define MAX31865_ADDR_RTD_HTHRES_MSB_W      (0x83)  /**< RTD high threshold MSB, write address */
#define MAX31865_ADDR_RTD_HTHRES_LSB_R      (0x04)  /**< RTD high threshold LSB, read address */
#define MAX31865_ADDR_RTD_HTHRES_LSB_W      (0x84)  /**< RTD high threshold LSB, write address */
#define MAX31865_ADDR_RTD_LTHRES_MSB_R      (0x05)  /**< RTD low threshold MSB, read address */
#define MAX31865_ADDR_RTD_LTHRES_MSB_W      (0x85)  /**< RTD low threshold MSB, write address */
#define MAX31865_ADDR_RTD_LTHRES_LSB_R      (0x06)  /**< RTD low threshold LSB, read address */
#define MAX31865_ADDR_RTD_LTHRES_LSB_W      (0x86)  /**< RTD low threshold LSB, write address */
#define MAX31865_ADDR_FAULT                 (0x07)  /**< Fault status register, read-only */
/** @} */

/**
 * @defgroup drivers_max31865_constants_regcfg Configuration register bits
 * @ingroup drivers_max31865_constants
 * @{
 */

/**
 * @brief Enable Vbias
 */
#define MAX31865_CFG_VBIAS_ON               (0b10000000)

/**
 * @brief Enable automatic conversion mode
 *
 * The conversion rate is the same as the selected filter mode,
 * \em i.e. either 50 or 60Hz.
 */
#define MAX31865_CFG_CONV_AUTO              (0b01000000)

/**
 * @brief Trigger a single conversion
 *
 * The automatic conversion mode must be off.
 * Single conversion time is 52ms in 60Hz or 62.5ms in 50Hz filter mode.
 */
#define MAX31865_CFG_1SHOT                  (0b00100000)

/**
 * @brief 3-wire resistor connection
 *
 * Clear the bit for 2- or 4-wire connections.
 */
#define MAX31865_CFG_3WIRE                  (0b00010000)

/**
 * @brief Clear fault condition
 */
#define MAX31865_CFG_CLEAR_FAULT            (0b00000010)

/**
 * @brief Filter out 50Hz if set, or 60Hz otherwise
 *
 * Clear the bit for 60Hz filter mode.
 */
#define MAX31865_CFG_FILTER_50HZ            (0b00000001)

/**
 * @brief Fault detection bit mask
 *
 * To check the status of the fault detection cycle,
 * mask the bits of the configuration register and
 * compare the value with #MAX31865_CFG_FLTDET_IDLE,
 * #MAX31865_CFG_FLTDET_AUTO_START, #MAX31865_CFG_FLTDET_MANU_START
 * or #MAX31865_CFG_FLTDET_MANU_START.
 */
#define MAX31865_CFG_FLTDET_MASK           (0b00001100)

/**
 * @brief No fault detection is running
 */
#define MAX31865_CFG_FLTDET_IDLE           (0b00000000)

/**
 * @brief Start a fault detection with an automatic delay (about 550µs)
 */
#define MAX31865_CFG_FLTDET_AUTO_START     (0b00000100)

/**
 * @brief Start a fault detection with a manually controlled delay
 */
#define MAX31865_CFG_FLTDET_MANU_START     (0b00001000)

/**
 * @brief Terminate fault detection with a manually controlled delay
 */
#define MAX31865_CFG_FLTDET_MANU_STOP      (0b00001100)

/** @} */   /* end of group drivers_max31865_constants_regcfg */

/**
 * @defgroup drivers_max31865_constants_regflt Fault register bits
 * In normal conditions, all bits are cleared.
 * Bits 0 and 1 are not used.
 *
 * During measurement, the reference resistor and the RTD are connected in
 * series.
 * During a fault detection cycle initiated by the master, the IC can open an
 * internal switch on the FORCE- pin.
 * - When the FORCE- switch is closed, current should flow through the reference
 *   resistor and V<sub>REFIN-</sub> is expected to be less than 0.85 &times; V<sub>BIAS</sub>.
 * - When the FORCE- switch is open, no current should flow through the reference
 *   resistor nor the RTD and both V<sub>REFIN-</sub> and V<sub>RTDIN-</sub> are
 *   expected to be greater than 0.85 &times; V<sub>BIAS</sub>.
 *
 * The table below sums up the fault conditions when the FORCE- switch is open.
 * The two left-most column reflect the values of the bits masked by
 * #MAX31865_FLT_REF_FO and #MAX31865_FLT_RTD_FO.
 *
 * V<sub>REFIN-</sub>   | V<sub>RTDIN-</sub>    | Fault
 * ---                  | ---                   | ---
 * 0                    | 0                     | no error
 * 0                    | 1                     | RTD disconnected from the RTD- pin
 * 1                    | 0                     | RTD- is shorted to VCC; very unlikely
 * 1                    | 1                     | RTD disconnected from the RTD+ pin or shorted
 *
 * In the last case, it can be asserted that the RTD terminals are shorted together if the bit
 * masked by #MAX31865_FLT_THRESLOW is set.
 *
 * @ingroup drivers_max31865_constants
 * @{
 */

/**
 * @brief Fault: RTD is greater than the high threshold
 */
#define MAX31865_FLT_THRESHIGH              (0b10000000)

/**
 * @brief Fault: RTD is less than the low threshold
 */
#define MAX31865_FLT_THRESLOW               (0b01000000)

/**
 * @brief Fault: V<sub>REFIN-</sub> > 0.85 &times; V<sub>BIAS</sub> when FORCE- is closed
 */
#define MAX31865_FLT_REF_FC                 (0b00100000)

/**
 * @brief Fault: V<sub>REFIN-</sub> < 0.85 &times; V<sub>BIAS</sub> when FORCE- is open
 */
#define MAX31865_FLT_REF_FO                 (0b00010000)

/**
 * @brief Fault: V<sub>RTDIN-</sub> < 0.85 &times; &times; V<sub>BIAS</sub> when FORCE- is open
 */
#define MAX31865_FLT_RTD_FO                 (0b00001000)

/**
 * @brief Fault: overvoltage or undervoltage condition
 *
 * Overvoltage or undervoltage condition on at least one of the
 * FORCE or RTD pins.
 */
#define MAX31865_FLT_VOLTAGE                (0b00000100)

/** @} */   /* end of group drivers_max31865_constants_regflt */

#ifdef __cplusplus
}
#endif

/** @} */
