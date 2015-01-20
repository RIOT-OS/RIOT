/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_lis3dh LIS3DH accelerometer
 * @ingroup     drivers
 * @brief       Device driver for the LIS3DH accelerometer
 * @{
 *
 * @file
 * @brief       Device driver interface for the LIS3DH accelerometer
 *
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se
 */

#ifndef LIS3DH_H_
#define LIS3DH_H_

#include <stdint.h>

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The WHO_AM_I register should contain this value in order to correctly
 * identify the chip.
 */
#define LIS3DH_WHO_AM_I_RESPONSE (0b00110011)

/**
 * @brief LIS3DH hardware register addresses
 */
typedef enum {
    LIS3DH_REG_STATUS_AUX = 0x07,
    LIS3DH_REG_OUT_AUX_ADC1_L = 0x08,
    LIS3DH_REG_OUT_AUX_ADC1_H = 0x09,
    LIS3DH_REG_OUT_AUX_ADC2_L = 0x0A,
    LIS3DH_REG_OUT_AUX_ADC2_H = 0x0B,
    LIS3DH_REG_OUT_AUX_ADC3_L = 0x0C,
    LIS3DH_REG_OUT_AUX_ADC3_H = 0x0D,
    LIS3DH_REG_INT_COUNTER_REG = 0x0E,
    LIS3DH_REG_WHO_AM_I = 0x0F,
    LIS3DH_REG_TEMP_CFG_REG = 0x1F,
    LIS3DH_REG_CTRL_REG1 = 0x20,
    LIS3DH_REG_CTRL_REG2 = 0x21,
    LIS3DH_REG_CTRL_REG3 = 0x22,
    LIS3DH_REG_CTRL_REG4 = 0x23,
    LIS3DH_REG_CTRL_REG5 = 0x24,
    LIS3DH_REG_CTRL_REG6 = 0x25,
    LIS3DH_REG_REFERENCE = 0x26,
    LIS3DH_REG_STATUS_REG = 0x27,
    LIS3DH_REG_OUT_X_L = 0x28,
    LIS3DH_REG_OUT_X_H = 0x29,
    LIS3DH_REG_OUT_Y_L = 0x2A,
    LIS3DH_REG_OUT_Y_H = 0x2B,
    LIS3DH_REG_OUT_Z_L = 0x2C,
    LIS3DH_REG_OUT_Z_H = 0x2D,
    LIS3DH_REG_FIFO_CTRL_REG = 0x2E,
    LIS3DH_REG_FIFO_SRC_REG = 0x2F,
    LIS3DH_REG_INT1_CFG = 0x30,
    LIS3DH_REG_INT1_SOURCE = 0x31,
    LIS3DH_REG_INT1_THS = 0x32,
    LIS3DH_REG_INT1_DURATION = 0x33,
    LIS3DH_REG_CLICK_CFG = 0x38,
    LIS3DH_REG_CLICK_SRC = 0x39,
    LIS3DH_REG_CLICK_THS = 0x3A,
    LIS3DH_REG_TIME_LIMIT = 0x3B,
    LIS3DH_REG_TIME_LATENCY = 0x3C,
    LIS3DH_REG_TIME_WINDOW = 0x3D,
} lis3dh_reg_t;

/*
 * Bit offsets within the individual registers
 * source: LIS3DH datasheet
 */

/**
 * @name TEMP_CFG_REG bitfield macros
 */
/** @{ */


/**
 * @brief ADC enable
 *
 * Default value: 0
 *
 * 0: ADC disabled; 1: ADC enabled
 */
#define LIS3DH_TEMP_CFG_REG_ADC_PD_MASK (1 << 7)

/**
 * @brief Temperature sensor (T) enable.
 *
 * Default value: 0
 *
 * 0: T disabled; 1: T enabled
 */
#define LIS3DH_TEMP_CFG_REG_TEMP_EN_MASK (1 << 6)

/** @} */

/**
 * @name CTRL_REG1 bitfield macros
 */
/** @{ */

/**
 * @name Output data rate selection bitfield macros
 *
 * Default value: 0000
 *
 * 0000: Power down; Others: Refer to data sheet
 *
 * @see LIS3DH data sheet Table 25, “Data rate configuration”
 */
/** @{ */
#define LIS3DH_CTRL_REG1_ODR_SHIFT (4)
#define LIS3DH_CTRL_REG1_ODR3_MASK (1 << (LIS3DH_CTRL_REG1_ODR_SHIFT + 3))
#define LIS3DH_CTRL_REG1_ODR2_MASK (1 << (LIS3DH_CTRL_REG1_ODR_SHIFT + 2))
#define LIS3DH_CTRL_REG1_ODR1_MASK (1 << (LIS3DH_CTRL_REG1_ODR_SHIFT + 1))
#define LIS3DH_CTRL_REG1_ODR0_MASK (1 << LIS3DH_CTRL_REG1_ODR_SHIFT)

#define LIS3DH_CTRL_REG1_ODR_MASK  (LIS3DH_CTRL_REG1_ODR3_MASK | \
                                    LIS3DH_CTRL_REG1_ODR2_MASK | \
                                    LIS3DH_CTRL_REG1_ODR1_MASK | \
                                    LIS3DH_CTRL_REG1_ODR0_MASK)

/** @} */

/**
 * @brief Low power mode enable.
 *
 * Default value: 0
 *
 *  0. normal mode
 *  1. low power mode
 */
#define LIS3DH_CTRL_REG1_LPEN_MASK (1 << 3)

/** @brief Z enable bit offset */
#define LIS3DH_CTRL_REG1_ZEN_SHIFT (2)
/**
 * @brief Z axis enable.
 *
 * Default value: 1
 *
 *  0. Z axis disabled
 *  1. Z axis enabled
 */
#define LIS3DH_CTRL_REG1_ZEN_MASK (1 << LIS3DH_CTRL_REG1_ZEN_SHIFT)

/** @brief Y enable bit offset */
#define LIS3DH_CTRL_REG1_YEN_SHIFT (1)
/**
 * @brief Y axis enable.
 *
 * Default value: 1
 *
 *  0. Y axis disabled
 *  1. Y axis enabled
 */
#define LIS3DH_CTRL_REG1_YEN_MASK (1 << LIS3DH_CTRL_REG1_YEN_SHIFT)

/** @brief X enable bit offset */
#define LIS3DH_CTRL_REG1_XEN_SHIFT (0)
/**
 * @brief X axis enable.
 *
 * Default value: 1
 *
 *  0. X axis disabled
 *  1. X axis enabled
 */
#define LIS3DH_CTRL_REG1_XEN_MASK (1 << LIS3DH_CTRL_REG1_XEN_SHIFT)

/** @brief XYZ enable bitfield offset */
#define LIS3DH_CTRL_REG1_XYZEN_SHIFT (0)
/** @brief X, Y, Z enable bitfield mask */
#define LIS3DH_CTRL_REG1_XYZEN_MASK (LIS3DH_CTRL_REG1_XEN_MASK | \
                                     LIS3DH_CTRL_REG1_YEN_MASK | LIS3DH_CTRL_REG1_ZEN_MASK)
/** @} */
/**
 * @name Axis selection macros
 *
 * Use these when calling lis3dh_set_axes()
 */
/** @{ */
#define LIS3DH_AXES_X (LIS3DH_CTRL_REG1_XEN_MASK) /**< enable X axis */
#define LIS3DH_AXES_Y (LIS3DH_CTRL_REG1_YEN_MASK) /**< enable Y axis */
#define LIS3DH_AXES_Z (LIS3DH_CTRL_REG1_ZEN_MASK) /**< enable Z axis */
/**
 * @brief Convenience macro for enabling all axes.
 */
#define LIS3DH_AXES_XYZ (LIS3DH_CTRL_REG1_XYZEN_MASK)
/** @} */

/**
 * @name CTRL_REG2 bitfield macros
 */
/** @{ */
/**
 * @name High pass filter mode selection
 *
 * Default value: 00
 *
 * @see Refer to Table 29, "High pass filter mode configuration"
 */
/** @{ */
#define LIS3DH_CTRL_REG2_HPM1_MASK (1 << 7)
#define LIS3DH_CTRL_REG2_HPM0_MASK (1 << 6)
/** @} */

/**
 * @name High pass filter cut off frequency selection
 */
/** @{ */
#define LIS3DH_CTRL_REG2_HPCF2_MASK (1 << 5)
#define LIS3DH_CTRL_REG2_HPCF1_MASK (1 << 4)
/** @} */

/**
 * @brief Filtered data selection
 *
 * Default value: 0
 *
 *  0. internal filter bypassed
 *  1. data from internal filter sent to output register and FIFO
 */
#define LIS3DH_CTRL_REG2_FDS_MASK  (1 << 3)

/**
 * @brief High pass filter enabled for CLICK function.
 *
 *  0. filter bypassed
 *  1. filter enabled@
 */
#define LIS3DH_CTRL_REG2_HPCLICK_MASK  (1 << 2)

/**
 * @name High pass filter enabled for AOI function on interrupt 2
 *
 *  0. filter bypassed
 *  1. filter enabled
 */
/** @{ */
#define LIS3DH_CTRL_REG2_HPIS2_MASK  (1 << 1)
#define LIS3DH_CTRL_REG2_HPIS1_MASK  (1 << 0)
/** @} */
/** @} */

/**
 * @name CTRL_REG3 bitfield macros
 */
/** @{ */
/**
 * @brief CLICK interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_CLICK_MASK  (1 << 7)

/**
 * @brief AOI1 interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_AOI1_MASK    (1 << 6)
/**
 * @brief AOI2 interrupt on INT1.
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_AOI2_MASK    (1 << 5)
/**
 * @brief DRDY1 interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_DRDY1_MASK   (1 << 4)
/**
 * @brief DRDY2 interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_DRDY2_MASK   (1 << 3)
/**
 * @brief FIFO Watermark interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_WTM_MASK     (1 << 2)
/**
 * @brief FIFO Overrun interrupt on INT1
 *
 * Default value 0.
 *
 *  0. Disable
 *  1. Enable
 */
#define LIS3DH_CTRL_REG3_I1_OVERRUN_MASK (1 << 1)

/** @} */

/**
 * @name CTRL_REG4 bitfield macros
 */
/** @{ */

/**
 * @name Block data update.
 *
 * Default value: 0
 *
 *  0. continuous update
 *  1. output registers not updated until MSB and LSB reading
 */
/** @{ */
/** @brief BDU bit mask */
#define LIS3DH_CTRL_REG4_BDU_MASK (1 << 7)
/** @brief BDU enable */
#define LIS3DH_CTRL_REG4_BDU_ENABLE (LIS3DH_CTRL_REG4_BDU_MASK)
/** @brief BDU disable */
#define LIS3DH_CTRL_REG4_BDU_DISABLE (0)
/** @} */

/**
 * @name Big/little endian data selection
 *
 * Default value 0.
 *
 *  0. Data LSB @ lower address
 *  1. Data MSB @ lower address
 */
/** @{ */
/** @brief BLE bit mask */
#define LIS3DH_CTRL_REG4_BLE_MASK (1 << 6)

/** @brief BLE little endian mode */
#define LIS3DH_CTRL_REG4_BLE_LITTLE_ENDIAN (0)
/** @brief BLE big endian mode */
#define LIS3DH_CTRL_REG4_BLE_BIG_ENDIAN (LIS3DH_CTRL_REG4_BLE_MASK)
/** @} */

/**
 * @name Full scale selection.
 *
 * Default value: 00
 *
 *  - 00: +/- 2G
 *  - 01: +/- 4G
 *  - 10: +/- 8G
 *  - 11: +/- 16G
 */
/** @{ */
#define LIS3DH_CTRL_REG4_FS1_MASK (1 << 5)
#define LIS3DH_CTRL_REG4_FS0_MASK (1 << 4)
#define LIS3DH_CTRL_REG4_FS_MASK (LIS3DH_CTRL_REG4_FS1_MASK | LIS3DH_CTRL_REG4_FS0_MASK)
/** @} */

/**
 * @brief Scale parameters
 *
 * Use these names when calling lis3dh_set_scale()
 */
typedef enum {
    LIS3DH_SCALE_2G  = (0), /**< Scale: +/- 2G */
    LIS3DH_SCALE_4G  = (LIS3DH_CTRL_REG4_FS0_MASK), /**< Scale: +/- 4G */
    LIS3DH_SCALE_8G  = (LIS3DH_CTRL_REG4_FS1_MASK), /**< Scale: +/- 8G */
    /** Scale: +/- 16G */
    LIS3DH_SCALE_16G = (LIS3DH_CTRL_REG4_FS1_MASK | LIS3DH_CTRL_REG4_FS0_MASK)
} lis3dh_scale_t;

/**
 * @brief High resolution output mode
 *
 * Default value: 0
 *
 *  0. High resolution disable
 *  1. High resolution enable
 */
#define LIS3DH_CTRL_REG4_HR_MASK  (1 << 3)

/**
 * @name Self test enable
 *
 * Default value: 00
 *
 *  - 00: Self test disabled
 *  - Other: See Table 34
 *
 * @see Table 34
 */
/** @{ */
#define LIS3DH_CTRL_REG4_ST1_MASK (1 << 2)
#define LIS3DH_CTRL_REG4_ST0_MASK (1 << 1)
/** @} */

/**
 * @brief SPI serial interface mode selection
 *
 * Default value: 0
 *
 *  0. 4-wire interface
 *  1. 3-wire interface
 */
#define LIS3DH_CTRL_REG4_SIM_MASK (1 << 0)

/**
 * @brief Reboot memory content
 *
 * Default value: 0
 *
 *  0. normal mode
 *  1. reboot memory content
 */
#define LIS3DH_CTRL_REG5_REBOOT_MASK (1 << 7)

/**
 * @brief FIFO enable
 *
 * Default value: 0
 *
 *  0. FIFO disable
 *  1. FIFO enable
 */
#define LIS3DH_CTRL_REG5_FIFO_EN_MASK (1 << 6)

/**
 * @brief Latch interrupt request on INT1
 *
 * Latch interrupt request on INT1_SRC register, with INT1_SRC register
 * cleared by reading INT1_SRC itself.
 *
 * Default value: 0
 *
 *  0. interrupt request not latched
 *  1. interrupt request latched
 */
#define LIS3DH_CTRL_REG5_LIR_I1_MASK  (1 << 3)

/**
 * @brief 4D enable
 *
 * 4D detection is enabled on INT1 when 6D bit on INT1_CFG is set to 1.
 */
#define LIS3DH_CTRL_REG5_D4D_I1_MASK  (1 << 2)

/** @} */

/**
 * @name STATUS_REG bitfield macros
 */
/** @{ */
/**
 * @brief X, Y or Z axis data overrun
 *
 * Default value: 0
 *
 *  0. no overrun has occurred
 *  1. a new set of data has overwritten the previous ones
 */
#define LIS3DH_STATUS_REG_ZYXOR_MASK  (1 << 7)

/**
 * @brief Z axis data overrun
 *
 * Default value: 0
 *
 *  0. no overrun has occurred
 *  1. a new data for the Z-axis has overwritten the previous one
 */
#define LIS3DH_STATUS_REG_ZOR_MASK  (1 << 6)

/**
 * @brief Y axis data overrun
 *
 * Default value: 0
 *
 *  0. no overrun has occurred
 *  1. a new data for the Y-axis has overwritten the previous one
 */
#define LIS3DH_STATUS_REG_YOR_MASK  (1 << 5)

/**
 * @brief X axis data overrun
 *
 * Default value: 0
 *
 *  0. no overrun has occurred
 *  1. a new data for the X-axis has overwritten the previous one
 */
#define LIS3DH_STATUS_REG_XOR_MASK  (1 << 4)

/**
 * @brief X, Y or Z axis new data available
 *
 * Default value: 0
 *
 *  0. a new set of data is not yet available
 *  1. a new set of data is available
 */
#define LIS3DH_STATUS_REG_ZYXDA_MASK (1 << 3)

/**
 * @brief Z axis new data available
 *
 * Default value: 0
 *
 *  0. a new data for the Z-axis is not yet available
 *  1. a new data for the Z-axis is available
 */
#define LIS3DH_STATUS_REG_ZDA_MASK  (1 << 2)

/**
 * @brief Y axis new data available
 *
 * Default value: 0
 *
 *  0. a new data for the Y-axis is not yet available
 *  1. a new data for the Y-axis is available
 */
#define LIS3DH_STATUS_REG_YDA_MASK  (1 << 1)

/**
 * @brief X axis new data available
 *
 * Default value: 0
 *
 *  0. a new data for the X-axis is not yet available
 *  1. a new data for the X-axis is available
 */
#define LIS3DH_STATUS_REG_XDA_MASK  (1 << 0)

/** @} */

/**
 * @name FIFO_CTRL_REG bitfield macros
 */
/** @{ */
#define LIS3DH_FIFO_CTRL_REG_FM_SHIFT  (6)
#define LIS3DH_FIFO_CTRL_REG_FM1_MASK  (1 << 7)
#define LIS3DH_FIFO_CTRL_REG_FM0_MASK  (1 << 6)
#define LIS3DH_FIFO_CTRL_REG_FM_MASK   (LIS3DH_FIFO_CTRL_REG_FM1_MASK | \
                                        LIS3DH_FIFO_CTRL_REG_FM0_MASK)

#define LIS3DH_FIFO_CTRL_REG_TR_MASK   (1 << 5)
#define LIS3DH_FIFO_CTRL_REG_FTH4_MASK (1 << 4)
#define LIS3DH_FIFO_CTRL_REG_FTH3_MASK (1 << 3)
#define LIS3DH_FIFO_CTRL_REG_FTH2_MASK (1 << 2)
#define LIS3DH_FIFO_CTRL_REG_FTH1_MASK (1 << 1)
#define LIS3DH_FIFO_CTRL_REG_FTH0_MASK (1 << 0)
#define LIS3DH_FIFO_CTRL_REG_FTH_SHIFT  (0)
#define LIS3DH_FIFO_CTRL_REG_FTH_MASK \
  (LIS3DH_FIFO_CTRL_REG_FTH0_MASK | \
   LIS3DH_FIFO_CTRL_REG_FTH1_MASK | \
   LIS3DH_FIFO_CTRL_REG_FTH2_MASK | \
   LIS3DH_FIFO_CTRL_REG_FTH3_MASK | \
   LIS3DH_FIFO_CTRL_REG_FTH4_MASK)
/** @} */

/**
 * @name FIFO_SRC_REG bitfield macros
 */
/** @{ */
#define LIS3DH_FIFO_SRC_REG_WTM_MASK (1 << 7)
#define LIS3DH_FIFO_SRC_REG_OVRN_FIFO_MASK (1 << 6)
#define LIS3DH_FIFO_SRC_REG_EMPTY_MASK (1 << 5)
#define LIS3DH_FIFO_SRC_REG_FSS4_MASK (1 << 4)
#define LIS3DH_FIFO_SRC_REG_FSS3_MASK (1 << 3)
#define LIS3DH_FIFO_SRC_REG_FSS2_MASK (1 << 2)
#define LIS3DH_FIFO_SRC_REG_FSS1_MASK (1 << 1)
#define LIS3DH_FIFO_SRC_REG_FSS0_MASK (1 << 0)
#define LIS3DH_FIFO_SRC_REG_FSS_SHIFT  (0)
#define LIS3DH_FIFO_SRC_REG_FSS_MASK \
  (LIS3DH_FIFO_SRC_REG_FSS0_MASK | \
   LIS3DH_FIFO_SRC_REG_FSS1_MASK | \
   LIS3DH_FIFO_SRC_REG_FSS2_MASK | \
   LIS3DH_FIFO_SRC_REG_FSS3_MASK | \
   LIS3DH_FIFO_SRC_REG_FSS4_MASK)
/** @} */

/**
 * @name Register address bitfield macros
 */
/** @{ */

/**
 * Write to register
 */
#define LIS3DH_SPI_WRITE_MASK (0 << 7)
/**
 * The READ bit must be set when reading
 */
#define LIS3DH_SPI_READ_MASK (1 << 7)
/**
 * Multi byte transfers must assert this bit when writing the address.
 */
#define LIS3DH_SPI_MULTI_MASK (1 << 6)
/**
 * Opposite of LIS3DH_SPI_MULTI_MASK.
 */
#define LIS3DH_SPI_SINGLE_MASK (0 << 6)
/**
 * Mask of the address bits in the address byte during transfers.
 */
#define LIS3DH_SPI_ADDRESS_MASK (0x3F)

/** @} */

/**
 * Length of scalar measurement data in bytes.
 */
#define LIS3DH_ADC_DATA_SIZE (2)


/**
 * @brief Allowed values for the Output Data Rate of the sensor.
 *
 * Use these when calling lis3dh_set_odr(odr).
 */
typedef enum {
    LIS3DH_ODR_POWERDOWN = (0x00 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_1Hz       = (0x01 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_10Hz      = (0x02 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_25Hz      = (0x03 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_50Hz      = (0x04 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_100Hz     = (0x05 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_200Hz     = (0x06 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_400Hz     = (0x07 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_LP1600Hz  = (0x08 << LIS3DH_CTRL_REG1_ODR_SHIFT),
    LIS3DH_ODR_NP1250Hz_LP5000HZ = (0x09 << LIS3DH_CTRL_REG1_ODR_SHIFT)
} lis3dh_odr_t;

/**
 * @brief Allowed FIFO modes.
 *
 * Used when calling lis3dh_set_fifo_mode()
 */
typedef enum {
    /** FIFO mode: Bypass */
    LIS3DH_FIFO_MODE_BYPASS = (0x00 << LIS3DH_FIFO_CTRL_REG_FM_SHIFT),
    /** FIFO mode: FIFO */
    LIS3DH_FIFO_MODE_FIFO = (0x01 << LIS3DH_FIFO_CTRL_REG_FM_SHIFT),
    /** FIFO mode: Stream */
    LIS3DH_FIFO_MODE_STREAM = (0x02 << LIS3DH_FIFO_CTRL_REG_FM_SHIFT),
    /** FIFO mode: Stream to FIFO */
    LIS3DH_FIFO_MODE_STREAM_TO_FIFO = (0x03 << LIS3DH_FIFO_CTRL_REG_FM_SHIFT)
} lis3dh_fifo_mode_t;

/**
 * @brief Device descriptor for LIS3DH sensors
 */
typedef struct {
    spi_t spi;              /**< SPI device the sensor is connected to */
    gpio_t cs;              /**< Chip select pin */
    gpio_t int1;            /**< INT1 pin */
    gpio_t int2;            /**< INT2 (DRDY) pin */
    int16_t scale;          /**< Current scale setting of the sensor */
} lis3dh_t;

/**
 * @brief Result vector for accelerometer measurement
 */
typedef struct __attribute__((packed))
{
    int16_t acc_x;          /**< Acceleration in the X direction in milli-G */
    int16_t acc_y;          /**< Acceleration in the Y direction in milli-G */
    int16_t acc_z;          /**< Acceleration in the Z direction in milli-G */
}
lis3dh_data_t;


/**
 * @brief Initialize a LIS3DH sensor instance
 *
 * @param[in]  dev          Device descriptor of sensor to initialize
 * @param[in]  spi          SPI bus the accelerometer is connected to
 * @param[in]  cs_pin       GPIO connected to the chip select pin of the accelerometer
 * @param[in]  int1_pin     GPIO connected to the INT1 pin of the accelerometer
 * @param[in]  int2_pin     GPIO connected to the INT2 pin of the accelerometer
 * @param[in]  scale        Initial scale setting of the sensor
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_init(lis3dh_t *dev, spi_t spi, gpio_t cs_pin, gpio_t int1_pin, gpio_t int2_pin, lis3dh_scale_t scale);

/**
 * @brief Read 3D acceleration data from the accelerometer
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[out] acc_data     Accelerometer data output buffer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_read_xyz(const lis3dh_t *dev, lis3dh_data_t *acc_data);

/**
 * @brief Read auxiliary ADC channel 1 data from the accelerometer
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[out] out          The value of ADC1 (OUT_1_{L,H}) will be written to this buffer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_read_aux_adc1(const lis3dh_t *dev, int16_t *out);

/**
 * @brief Read auxiliary ADC channel 2 data from the accelerometer
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[out] out          The value of ADC2 (OUT_2_{L,H}) will be written to this buffer
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_read_aux_adc2(const lis3dh_t *dev, int16_t *out);

/**
 * @brief Read auxiliary ADC channel 3 data from the accelerometer
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[out] out          The value of ADC3 (OUT_3_{L,H}) will be written to this buffer
 *
 * @note The internal temperature sensor is connected to the third channel on
 *       the auxiliary ADC when the TEMP_EN bit of TEMP_CFG_REG is set.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_read_aux_adc3(const lis3dh_t *dev, int16_t *out);

/**
 * @brief Turn on/off power to the auxiliary ADC in LIS3DH.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[in]  enable       Power state of the auxiliary ADC
 * @param[in]  temperature  If not zero, switch the ADC mux so that a
 *                          temperature reading is available on OUT_3_L, OUT_3_H.
 *
 * @note This ADC is only used for the temperature reading and the external ADC
 *       pins. The accelerometer ADC is turned on by lis3dh_set_odr().
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_aux_adc(lis3dh_t *dev, const uint8_t enable, const uint8_t temperature);

/**
 * @brief Enable/disable accelerometer axes.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[in]  axes         An OR-ed combination of LIS3DH_AXES_X,
 *                          LIS3DH_AXES_Y, LIS3DH_AXES_Z.
 *
 * @note The macro LIS3DH_AXES_XYZ is a convenience shortcut to enable all axes.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_axes(lis3dh_t *dev, const uint8_t axes);

/**
 * @brief Set the FIFO mode.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[in]  mode         The chosen FIFO mode.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_fifo_mode(lis3dh_t *dev, const lis3dh_fifo_mode_t mode);

/**
 * @brief Enable/disable the FIFO.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[in]  enable       If zero, disable the FIFO, otherwise enables the FIFO.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_fifo(lis3dh_t *dev, const uint8_t enable);

/**
 * Set the output data rate of the sensor.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param[in]  odr          Chosen output data rate.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_odr(lis3dh_t *dev, const lis3dh_odr_t odr);

/**
 * @brief Set the full scale range of the sensor.
 *
 * Valid values for scale is 2, 4, 8, 16 and represents the full range of the
 * sensor.
 *
 * @param[in]  dev          Device descriptor of sensor
 * @param  scale            The chosen sensitivity scale.
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int lis3dh_set_scale(lis3dh_t *dev, const lis3dh_scale_t scale);

#ifdef __cplusplus
}
#endif

#endif /* LIS3DH_H_ */
/** @} */
