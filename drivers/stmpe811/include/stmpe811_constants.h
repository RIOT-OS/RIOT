/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_stmpe811
 *
 * @{
 * @file
 * @brief       Constants for STMPE811
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "stmpe811.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STMPE811_I2C_ADDR_DEFAULT                   (0x41)      /**< Default I2C address */

#define STMPE811_CHIP_ID_VALUE                      (0x0811)    /**< Chip ID */

/**
 * @name    Registers
 * @{
 */
#define STMPE811_CHIP_ID                            (0x00)      /**< STMPE811 Device identification */
#define STMPE811_ID_VER                             (0x02)      /**< STMPE811 Revision number */
#define STMPE811_SYS_CTRL1                          (0x03)      /**< Reset control */
#define STMPE811_SYS_CTRL2                          (0x04)      /**< Clock control */
#define STMPE811_SPI_CFG                            (0x08)      /**< SPI interface configuration */
#define STMPE811_INT_CTRL                           (0x09)      /**< Interrupt control register */
#define STMPE811_INT_EN                             (0x0A)      /**< Interrupt enable register */
#define STMPE811_INT_STA                            (0x0B)      /**< Interrupt status register */
#define STMPE811_GPIO_EN                            (0x0C)      /**< GPIO interrupt enable register */
#define STMPE811_GPIO_INT_STA                       (0x0D)      /**< GPIO interrupt status register */
#define STMPE811_ADC_INT_EN                         (0x0E)      /**< ADC interrupt enable register */
#define STMPE811_ADC_INT_STA                        (0x0F)      /**< ADC interface status register */
#define STMPE811_GPIO_SET_PIN                       (0x10)      /**< GPIO set pin register */
#define STMPE811_GPIO_CLR_PIN                       (0x11)      /**< GPIO clear pin register */
#define STMPE811_MP_STA                             (0x12)      /**< GPIO monitor pin state register */
#define STMPE811_GPIO_DIR                           (0x13)      /**< GPIO direction register */
#define STMPE811_GPIO_ED                            (0x14)      /**< GPIO edge detect register */
#define STMPE811_GPIO_RE                            (0x15)      /**< GPIO rising edge register */
#define STMPE811_GPIO_FE                            (0x16)      /**< GPIO falling edge register */
#define STMPE811_GPIO_ALT_FUNCTION                  (0x17)      /**< Alternate function register */
#define STMPE811_ADC_CTRL1                          (0x20)      /**< ADC control */
#define STMPE811_ADC_CTRL2                          (0x21)      /**< ADC control */
#define STMPE811_ADC_CAPT                           (0x22)      /**< To initiate ADC data acquisition */
#define STMPE811_ADC_DATA_CHO                       (0x30)      /**< ADC channel 0 */
#define STMPE811_ADC_DATA_CH1                       (0x32)      /**< ADC channel 1 */
#define STMPE811_ADC_DATA_CH2                       (0x34)      /**< ADC channel 2 */
#define STMPE811_ADC_DATA_CH3                       (0x36)      /**< ADC channel 3 */
#define STMPE811_ADC_DATA_CH4                       (0x38)      /**< ADC channel 4 */
#define STMPE811_ADC_DATA_CH5                       (0x3A)      /**< ADC channel 5 */
#define STMPE811_ADC_DATA_CH6                       (0x3C)      /**< ADC channel 6 */
#define STMPE811_ADC_DATA_CH7                       (0x3E)      /**< ADC channel 7 */
#define STMPE811_TSC_CTRL                           (0x40)      /**< 4-wire tsc setup */
#define STMPE811_TSC_CFG                            (0x41)      /**< Tsc configuration */
#define STMPE811_WDW_TR_X                           (0x42)      /**< Window setup for top right X */
#define STMPE811_WDW_TR_Y                           (0x44)      /**< Window setup for top right Y */
#define STMPE811_WDW_BL_X                           (0x46)      /**< Window setup for bottom left X */
#define STMPE811_WDW_BL_Y                           (0x48)      /**< Window setup for bottom left Y */
#define STMPE811_FIFO_TH                            (0x4A)      /**< FIFO level to generate interrupt */
#define STMPE811_FIFO_CTRL_STA                      (0x4B)      /**< Current status of FIFO */
#define STMPE811_FIFO_SIZE                          (0x4C)      /**< Current filled level of FIFO */
#define STMPE811_TSC_DATA_X                         (0x4D)      /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_Y                         (0x4F)      /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_Z                         (0x51)      /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_XYZ                       (0x52)      /**< Data port for tsc data access */
#define STMPE811_TSC_DATA_INC                       (0x57)      /**< Data port for tsc auto-increment data access */
#define STMPE811_TSC_DATA_NON_INC                   (0xD7)      /**< Data port for tsc non auto-increment data access */
#define STMPE811_TSC_FRACTION_Z                     (0x56)      /**< Touchscreen controller FRACTION_Z */
#define STMPE811_TSC_DATA                           (0x57)      /**< Data port for tsc data access */
#define STMPE811_TSC_I_DRIVE                        (0x58)      /**< Touchscreen controller drivel */
#define STMPE811_TSC_SHIELD                         (0x59)      /**< Touchscreen controller shield */
#define STMPE811_TEMP_CTRL                          (0x60)      /**< Temperature sensor setup */
#define STMPE811_TEMP_DATA                          (0x61)      /**< Temperature data access port */
#define STMPE811_TEMP_TH                            (0x62)      /**< Threshold for temp controlled int */
/** @} */

/**
 * @name    SYS_CTRL1 register bitfields
 * @{
 */
#define STMPE811_SYS_CTRL1_HIBERNATE                (1 << 0)    /**< Hibernate the device*/
#define STMPE811_SYS_CTRL1_SOFT_RESET               (1 << 1)    /**< Trigger software reset */
/** @} */

/**
 * @name    SYS_CTRL2 register bitfields
 * @{
 */
#define STMPE811_SYS_CTRL2_ADC_OFF                  (1 << 0)    /**< Disable ADC */
#define STMPE811_SYS_CTRL2_TSC_OFF                  (1 << 1)    /**< Disable Touchscreen*/
#define STMPE811_SYS_CTRL2_GPIO_OFF                 (1 << 2)    /**< Disable GPIO */
#define STMPE811_SYS_CTRL2_TS_OFF                   (1 << 3)    /**< Disable Temperature sensor */
/** @} */

/**
 * @name    SPI_CFG register bitfields
 * @{
 */
#define STMPE811_SPI_CFG_SPI_CLK_MOD0               (1 << 0)    /**< SCAD/A0 pin during power-up reset */
#define STMPE811_SPI_CFG_SPI_CLK_MOD1               (1 << 1)    /**< SCAD/A0 pin during power-up reset */
#define STMPE811_SPI_CFG_AUTO_INCR                  (1 << 2)    /**< SPI transactions internal autoincrement */
/** @} */

/**
 * @name    INT_CTRL register bitfields
 * @{
 */
#define STMPE811_INT_CTRL_INT_POLARITY              (1 << 2)    /**< Configure interrupt polarity (falling or raising) */
#define STMPE811_INT_CTRL_INT_TYPE                  (1 << 1)    /**< Configure interrupt type (edge or level) */
#define STMPE811_INT_CTRL_GLOBAL_INT                (1 << 0)    /**< Enable global interrupt */
/** @} */

/**
 * @name    INT_EN register bitfields
 * @{
 */
#define STMPE811_INT_EN_TOUCH_DET                   (1 << 0)    /**< Enable touch detection interrupt */
#define STMPE811_INT_EN_FIFO_TH                     (1 << 1)    /**< Enable FIFO threshold interrupt */
#define STMPE811_INT_EN_FIFO_OFLOW                  (1 << 2)    /**< Enable FIFO overflow interrupt */
#define STMPE811_INT_EN_FIFO_FULL                   (1 << 3)    /**< Enable FIFO full interrupt */
#define STMPE811_INT_EN_FIFO_EMPTY                  (1 << 4)    /**< Enable FIFO empty interrupt */
#define STMPE811_INT_EN_TEMP_SENS                   (1 << 5)    /**< Enable temperature sensor interrupt */
#define STMPE811_INT_EN_ADC                         (1 << 6)    /**< Enable ADC interrupt */
#define STMPE811_INT_EN_GPIO                        (1 << 7)    /**< Enable GPIO interrupt */
/** @} */

/**
 * @name    ADC_CTRL1 register bitfields
 * @{
 */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_POS          (4)         /**< Sample time bits shift position */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_36           (0b000)     /**< Conversion time: 36 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_44           (0b001)     /**< Conversion time: 44 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_56           (0b010)     /**< Conversion time: 56 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_64           (0b011)     /**< Conversion time: 64 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_80           (0b100)     /**< Conversion time: 80 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_96           (0b101)     /**< Conversion time: 96 cycles */
#define STMPE811_ADC_CTRL1_SAMPLE_TIME_124          (0b110)     /**< Conversion time: 124 cycles */
#define STMPE811_ADC_CTRL1_MOD_12B                  (1 << 3)    /**< Enable 12 bit ADC (10bit if 0) */
/** @} */

/**
 * @name    ADC_CTRL2 register bitfields
 * @{
 */
#define STMPE811_ADC_CTRL2_FREQ_1_625MHZ            (0b00)      /**< ADC clock frequency 1.625MHz */
#define STMPE811_ADC_CTRL2_FREQ_3_25MHZ             (0b01)      /**< ADC clock frequency 3.25MHz */
#define STMPE811_ADC_CTRL2_FREQ_6_5MHZ              (0b10)      /**< ADC clock frequency 6.5MHz */
#define STMPE811_ADC_CTRL2_FREQ_6_5_2MHZ            (0b11)      /**< ADC clock frequency 6.5MHz */
/** @} */

/**
 * @name    TSC_CTRL register bitfields
 * @{
 */
#define STMPE811_TSC_CTRL_EN                        (1 << 0)    /**< Enable touchscreen */
#define STMPE811_TSC_CTRL_OPMOD_POS                 (1)         /**< Operating mode bit shift position */
#define STMPE811_TSC_CTRL_OPMOD_XYZ                 (0b000)     /**< X,Y,Z acquisition */
#define STMPE811_TSC_CTRL_OPMOD_XY_ONLY             (0b001)     /**< X,Y only acquisition */
#define STMPE811_TSC_CTRL_OPMOD_X_ONLY              (0b010)     /**< X only acquisition */
#define STMPE811_TSC_CTRL_OPMOD_Y_ONLY              (0b011)     /**< Y only acquisition */
#define STMPE811_TSC_CTRL_OPMOD_Z_ONLY              (0b100)     /**< Z only acquisition */
#define STMPE811_TSC_CTRL_TRACK_POS                 (4)         /**< Movement tracking index bit shift position */
#define STMPE811_TSC_CTRL_TRACK_NO                  (0b000)     /**< No window tracking */
#define STMPE811_TSC_CTRL_TRACK_4                   (0b001)     /**< Tracking index 4 */
#define STMPE811_TSC_CTRL_TRACK_8                   (0b010)     /**< Tracking index 8 */
#define STMPE811_TSC_CTRL_TRACK_16                  (0b011)     /**< Tracking index 16 */
#define STMPE811_TSC_CTRL_TRACK_32                  (0b100)     /**< Tracking index 32 */
#define STMPE811_TSC_CTRL_TRACK_64                  (0b101)     /**< Tracking index 64 */
#define STMPE811_TSC_CTRL_TRACK_92                  (0b110)     /**< Tracking index 92 */
#define STMPE811_TSC_CTRL_TRACK_127                 (0b111)     /**< Tracking index 127 */
#define STMPE811_TSC_CTRL_STA                       (1 << 7)    /**< Touchscreen status (1: touch detected, 0: no touch detected), read-only */
/** @} */

/**
 * @name    TSC_CFG register bitfields
 * @{
 */
#define STMPE811_TSC_CFG_AVE_CTRL_POS               (6)         /**< Average control bit shift position */
#define STMPE811_TSC_CFG_AVE_CTRL_1                 (0b00)      /**< Average control, 1 sample */
#define STMPE811_TSC_CFG_AVE_CTRL_2                 (0b01)      /**< Average control, 2 sample */
#define STMPE811_TSC_CFG_AVE_CTRL_4                 (0b10)      /**< Average control, 4 sample */
#define STMPE811_TSC_CFG_AVE_CTRL_8                 (0b11)      /**< Average control, 8 sample */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_POS        (3)         /**< Touch detection delay bit shift position */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_10US       (0b000)     /**< Touch detection 10us delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_50US       (0b001)     /**< Touch detection 50us delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_100US      (0b010)     /**< Touch detection 100us delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_500US      (0b011)     /**< Touch detection 500us delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_1MS        (0b100)     /**< Touch detection 1ms delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_5MS        (0b101)     /**< Touch detection 5ms delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_10MS       (0b110)     /**< Touch detection 10ms delay */
#define STMPE811_TSC_CFG_TOUCH_DET_DELAY_50MS       (0b111)     /**< Touch detection 50ms delay */
#define STMPE811_TSC_CFG_SETTLING_10US              (0b000)     /**< Settling time 10us */
#define STMPE811_TSC_CFG_SETTLING_100US             (0b001)     /**< Settling time 100us */
#define STMPE811_TSC_CFG_SETTLING_500US             (0b010)     /**< Settling time 500us */
#define STMPE811_TSC_CFG_SETTLING_1MS               (0b011)     /**< Settling time 1ms */
#define STMPE811_TSC_CFG_SETTLING_5MS               (0b100)     /**< Settling time 5ms */
#define STMPE811_TSC_CFG_SETTLING_10MS              (0b101)     /**< Settling time 10ms */
#define STMPE811_TSC_CFG_SETTLING_50MS              (0b110)     /**< Settling time 50ms */
#define STMPE811_TSC_CFG_SETTLING_100MS             (0b111)     /**< Settling time 100ms */
/** @} */

/**
 * @name    FIFO_CTRL_STA register bitfields
 * @{
 */
#define STMPE811_FIFO_CTRL_STA_RESET                (1 << 0)    /**< Reset FIFO */
/** @} */

/**
 * @name    TSC_FRACTION_Z register bitfields
 * @{
 */
#define STMPE811_TSC_FRACTION_Z_0_8                 (0b000)     /**< Fractional part is 0, whole part is 8 */
#define STMPE811_TSC_FRACTION_Z_1_7                 (0b001)     /**< Fractional part is 1, whole part is 7 */
#define STMPE811_TSC_FRACTION_Z_2_6                 (0b010)     /**< Fractional part is 2, whole part is 6 */
#define STMPE811_TSC_FRACTION_Z_3_5                 (0b011)     /**< Fractional part is 3, whole part is 5 */
#define STMPE811_TSC_FRACTION_Z_4_4                 (0b100)     /**< Fractional part is 4, whole part is 4 */
#define STMPE811_TSC_FRACTION_Z_5_3                 (0b101)     /**< Fractional part is 5, whole part is 3 */
#define STMPE811_TSC_FRACTION_Z_6_2                 (0b110)     /**< Fractional part is 6, whole part is 2 */
#define STMPE811_TSC_FRACTION_Z_7_1                 (0b111)     /**< Fractional part is 7, whole part is 1 */
/** @} */

/**
 * @name    TSC_I_DRIVE register bitfields
 * @{
 */
#define STMPE811_TSC_I_DRIVE_50MA                   (1 << 0)    /**< Enable 50mA drive current, 20mA if 0 */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
