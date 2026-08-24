/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ads131m02
 * @{
 *
 * @file
 * @brief       Register map and types for the ADS131M02 ADC
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  ID Register (Address 0x00)
 * Reset Value: 0x2200
 */
#define ADS131M02_REG_ID                0x00U
/**
 * @brief  Channel count (Bits [11:8])
 *
 * 0010: 2 channels
 */
#define ADS131M02_ID_CHAN_CNT_MASK      (BIT11 | BIT10 | BIT9 | BIT8)
/**
 * @brief  Revision ID (Bits [7:0])
 */
#define ADS131M02_ID_REVID_MASK         (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

/**
 * @brief  STATUS Register (Address 0x01)
 * Reset Value: 0x0503
 */
#define ADS131M02_REG_STATUS            0x01U
/**
 * @brief  SPI interface lock status
 *
 * 0: Interface is unlocked (default)
 * 1: Interface is locked
 */
#define ADS131M02_STATUS_LOCK_MASK      BIT15
/**
 * @brief  ADC resynchronization indicator
 *
 * 0: No resynchronization (default)
 * 1: Resynchronization occurred
 */
#define ADS131M02_STATUS_F_RESYNC_MASK  BIT14

/**
 * @brief  Register map CRC fault indicator
 *
 * 0: No change in the register map CRC (default)
 * 1: Register map CRC changed
 */
#define ADS131M02_STATUS_REG_MAP_MASK   BIT13
/**
 * @brief  SPI input CRC error indicator
 *
 * 0: No CRC error (default)
 * 1: Input CRC error occurred
 */
#define ADS131M02_STATUS_CRC_ERR_MASK   BIT12
/**
 * @brief  CRC type
 *
 * 0: 16 bit CCITT (default)
 * 1: 16 bit ANSI
 */
#define ADS131M02_STATUS_CRC_TYPE_MASK  BIT11
/**
 * @brief  Reset status
 *
 * 0: Not reset
 * 1: Reset occurred (default)
 */
#define ADS131M02_STATUS_RESET_MASK     BIT10
/**
 * @brief  Data word length
 *
 * 00: 16-bit
 * 01: 24-bit (default)
 * 10: 32-bit zero-padded
 * 11: 32-bit sign-extended
 */
#define ADS131M02_STATUS_WLENGTH_MASK   (BIT9 | BIT8)
/**
 * @brief  Channel 1 ADC data ready indicator
 *
 * 0: Channel 1 data is not new
 * 1: Channel 1 data is new
 */
#define ADS131M02_STATUS_DRDY1_MASK     BIT1
/**
 * @brief  Channel 0 ADC data ready indicator
 *
 * 0: Channel 0 data is not new
 * 1: Channel 0 data is new
 */
#define ADS131M02_STATUS_DRDY0_MASK     BIT0

/**
 * @brief  MODE Register
 * Reset Value: 0x0100
 */
#define ADS131M02_REG_MODE              0x02U
/**
 * @brief  Register map CRC enable (Bit 13)
 *
 * 0: Register CRC disabled (default)
 * 1: Register CRC enabled
 */
#define ADS131M02_MODE_REG_CRC_EN_MASK  BIT13
/**
 * @brief   SPI input CRC enable
 *
 * 0: Disabled (default)
 * 1: Enabled
 */
#define ADS131M02_MODE_RX_CRC_EN_MASK   BIT12
/**
 * @brief  SPI input and output, register map CRC type
 *
 * 0: 16-bit CCITT (default)
 * 1: 16-bit ANSI
 */
#define ADS131M02_MODE_CRC_TYPE_MASK    BIT11
/**
 * @brief  Reset - Write 0b to clear this bit in the STATUS register
 *
 * 0: No reset
 * 1: Reset occurred (default by definition)
 */
#define ADS131M02_MODE_RESET_MASK       BIT10
/**
 * @name  Data word length selection
 * @{
 *
 * 00: 16-bit
 * 01: 24-bit (default)
 * 10: 32-bit zero-padded
 * 11: 32-bit sign-extended
 */
#define ADS131M02_MODE_WLENGTH_MASK     (BIT9 | BIT8)
#define ADS131M02_MODE_WLENGTH_16BIT    (0x0U << 8)
#define ADS131M02_MODE_WLENGTH_24BIT    (0x1U << 8)
#define ADS131M02_MODE_WLENGTH_32BIT_ZP (0x2U << 8)
#define ADS131M02_MODE_WLENGTH_32BIT_SE (0x3U << 8)

typedef enum {
    ads131m02_wlength_16bit = ADS131M02_MODE_WLENGTH_16BIT,
    ads131m02_wlength_24bit = ADS131M02_MODE_WLENGTH_24BIT,
    ads131m02_wlength_32bit_zp = ADS131M02_MODE_WLENGTH_32BIT_ZP,
    ads131m02_wlength_32bit_se = ADS131M02_MODE_WLENGTH_32BIT_SE,
} ads131m02_wlength_t;
/** @} */

/**
 * @brief  SPI timeout enable
 *
 * 0: Disabled (default)
 * 1: Enabled
 */
#define ADS131M02_MODE_TIMEOUT_MASK     BIT4
/**
 * @name  DRDY pin signal source selection
 * @{
 *
 * 00: Most lagging enabled channel (default)
 * 01: Logic OR of all the enabled channels
 * 10: Most leading enabled channel
 * 11: Most leading enabled channel
 */
#define ADS131M02_MODE_DRDY_SEL_MASK    (BIT3 | BIT2)
#define ADS131M02_MODE_DRDY_SEL_ALL     (0x0U << 2)
#define ADS131M02_MODE_DRDY_SEL_ANY     (0x1U << 2)

typedef enum {
    ads131m02_drdy_sel_all = ADS131M02_MODE_DRDY_SEL_ALL,
    ads131m02_drdy_sel_any = ADS131M02_MODE_DRDY_SEL_ANY,
} ads131m02_drdy_sel_t;
/** @} */

/**
 * @brief  DRDY pin state when conversion data are not available
 *
 * 0: Logic high (default)
 * 1: High impedance
 */
#define ADS131M02_MODE_DRDY_HiZ_MASK    BIT1
/**
 * @brief  DRDY signal format when conversion data are available
 *
 * 0: Logic low (default)
 * 1: Low pulse with a fixed duration
 */
#define ADS131M02_MODE_DRDY_FMT_MASK    BIT0

/**
 * @brief  CLOCK Register (Address 0x03)
 * Reset Value: 0x030E
 */
#define ADS131M02_REG_CLOCK             0x03U
/**
 * @brief  Channel 1 ADC enable
 *
 * 0: Disabled
 * 1: Enabled (default)
 */
#define ADS131M02_CLOCK_CH1_EN_MASK     BIT9
/**
 * @brief  Channel 0 ADC enable
 *
 * 0: Disabled
 * 1: Enabled (default)
 */
#define ADS131M02_CLOCK_CH0_EN_MASK     BIT8
/**
 * @brief   Modulator oversampling ratio 64 selection (TurboMode)
 *
 * 0: OSR set by Bit 4:2 , i.e. OSR[2:0]
 * 1: OSR of 64 is selected
 */
#define ADS131M02_CLOCK_TBM_MASK        BIT5
/**
 * @name  Modulator oversampling ratio selection
 * @{
 *
 * The OSR determines the amount of averaging of the modulator output in the digital filter and therefore also
 * the filter bandwidth.
 *
 * OSR = fMOD / fDATA
 *     = fCLK / 2 / fDATA
 *
 * 000: 128
 * 001: 256
 * 010: 512
 * 011: 1024 (default)
 * 100: 2048
 * 101: 4096
 * 110: 8192
 * 111: 16384
 */
#define ADS131M02_CLOCK_OSR_MASK        (BIT4 | BIT3 | BIT2)
#define ADS131M02_CLOCK_OSR_128         (0x0U << 2)
#define ADS131M02_CLOCK_OSR_256         (0x1U << 2)
#define ADS131M02_CLOCK_OSR_512         (0x2U << 2)
#define ADS131M02_CLOCK_OSR_1024        (0x3U << 2)
#define ADS131M02_CLOCK_OSR_2048        (0x4U << 2)
#define ADS131M02_CLOCK_OSR_4096        (0x5U << 2)
#define ADS131M02_CLOCK_OSR_8192        (0x6U << 2)
#define ADS131M02_CLOCK_OSR_16384       (0x7U << 2)

typedef enum {
    ads131m02_osr_128 = ADS131M02_CLOCK_OSR_128,
    ads131m02_osr_256 = ADS131M02_CLOCK_OSR_256,
    ads131m02_osr_512 = ADS131M02_CLOCK_OSR_512,
    ads131m02_osr_1024 = ADS131M02_CLOCK_OSR_1024,
    ads131m02_osr_2048 = ADS131M02_CLOCK_OSR_2048,
    ads131m02_osr_4096 = ADS131M02_CLOCK_OSR_4096,
    ads131m02_osr_8192 = ADS131M02_CLOCK_OSR_8192,
    ads131m02_osr_16384 = ADS131M02_CLOCK_OSR_16384,
} ads131m02_osr_t;
/** @}  */

/**
 * @name  Power mode selection
 * @{
 *
 * 00: Very low power
 * 01: Low power
 * 10: High resolution (default)
 * 11: High resolution
 */
#define ADS131M02_CLOCK_PWR_MASK        (BIT1 | BIT0)
#define ADS131M02_CLOCK_PWR_VERY_LOW    (0x0U)
#define ADS131M02_CLOCK_PWR_LOW         (0x1U)
#define ADS131M02_CLOCK_PWR_HIGH_RES    (0x2U)

typedef enum {
    ads131m02_pwr_very_low = ADS131M02_CLOCK_PWR_VERY_LOW,
    ads131m02_pwr_low = ADS131M02_CLOCK_PWR_LOW,
    ads131m02_pwr_high_res = ADS131M02_CLOCK_PWR_HIGH_RES,
} ads131m02_pwr_t;
/** @}  */

/**
 * @brief  GAIN1 Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_GAIN              0x04U
/**
 * @name  Channel 1 PGA gain selection
 * @{
 *
 * FSR = ±1.2 V / Gain
 *
 * 000: 1 (default)
 * 001: 2
 * 010: 4
 * 011: 8
 * 100: 16
 * 101: 32
 * 110: 64
 * 111: 128
 */
#define ADS131M02_GAIN_PGAGAIN1_MASK    (BIT6 | BIT5 | BIT4)
#define ADS131M02_GAIN_SHIFT(ch)        ((ch) * 4)
#define ADS131M02_GAIN_PGAGAIN1_1       (0x0U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_2       (0x1U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_4       (0x2U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_8       (0x3U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_16      (0x4U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_32      (0x5U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_64      (0x6U << ADS131M02_GAIN_SHIFT(1))
#define ADS131M02_GAIN_PGAGAIN1_128     (0x7U << ADS131M02_GAIN_SHIFT(1))
/** @} */

/**
 * @name  Channel 0 PGA gain selection
 * @{
 *
 * 000: 1 (default)
 * 001: 2
 * 010: 4
 * 011: 8
 * 100: 16
 * 101: 32
 * 110: 64
 * 111: 128
 */
#define ADS131M02_GAIN_PGAGAIN0_MASK    (BIT2 | BIT1 | BIT0)
#define ADS131M02_GAIN_SHIFT(ch)        ((ch) * 4)
#define ADS131M02_GAIN_PGAGAIN0_1       (0x0U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_2       (0x1U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_4       (0x2U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_8       (0x3U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_16      (0x4U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_32      (0x5U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_64      (0x6U << ADS131M02_GAIN_SHIFT(0))
#define ADS131M02_GAIN_PGAGAIN0_128     (0x7U << ADS131M02_GAIN_SHIFT(0))

#define ADS131M02_GAIN(div)             ((div) ? __builtin_ctz(div) : 0)

typedef enum {
    ads131m02_gain_1 = ADS131M02_GAIN_PGAGAIN0_1,
    ads131m02_gain_2 = ADS131M02_GAIN_PGAGAIN0_2,
    ads131m02_gain_4 = ADS131M02_GAIN_PGAGAIN0_4,
    ads131m02_gain_8 = ADS131M02_GAIN_PGAGAIN0_8,
    ads131m02_gain_16 = ADS131M02_GAIN_PGAGAIN0_16,
    ads131m02_gain_32 = ADS131M02_GAIN_PGAGAIN0_32,
    ads131m02_gain_64 = ADS131M02_GAIN_PGAGAIN0_64,
    ads131m02_gain_128 = ADS131M02_GAIN_PGAGAIN0_128,
} ads131m02_gain_t;
/** @} */

/**
 * @brief  CFG Register (Address 0x06)
 * Reset Value: 0x0600
 */
#define ADS131M02_REG_CFG               0x06U
/**
 * @brief  Global chop delay selection
 * Delay in modulator clock periods before measurement begins
 *
 * 0000: 2
 * 0001: 4
 * 0010: 8
 * 0011: 16 (default)
 * 0100: 32
 * 0101: 64
 * 0110: 128
 * 0111: 256
 * 1000: 512
 * 1001: 1024
 * 1010: 2048
 * 1011: 4096
 * 1100: 8192
 * 1101: 16384
 * 1110: 32768
 * 1111: 65536
 */
#define ADS131M02_CFG_GC_DLY_MASK       (BIT12 | BIT11 | BIT10 | BIT9)
/**
 * @brief  Global chop enable
 *
 * 0: Disabled (default)
 * 1: Enabled
 */
#define ADS131M02_CFG_GC_EN_MASK        BIT8
/**
 * @brief   Current-detect channel selection
 * Channels required to trigger current-detect
 *
 * 0: Any channel (default)
 * 1: All channels
 */
#define ADS131M02_CFG_CD_ALL_MASK       BIT7
/**
 * @name  Number of current-detect exceeded thresholds selection
 * @{
 *
 * Number of current-detect exceeded thresholds to trigger a detection
 *
 * 000: 1 (default)
 * 001: 2
 * 010: 4
 * 011: 8
 * 100: 16
 * 101: 32
 * 110: 64
 * 111: 128
 */
#define ADS131M02_CFG_CD_NUM_MASK       (BIT6 | BIT5 | BIT4)
#define ADS131M02_CFG_CD_NUM_1          (0x0U << 4)
#define ADS131M02_CFG_CD_NUM_2          (0x1U << 4)
#define ADS131M02_CFG_CD_NUM_4          (0x2U << 4)
#define ADS131M02_CFG_CD_NUM_8          (0x3U << 4)
#define ADS131M02_CFG_CD_NUM_16         (0x4U << 4)
#define ADS131M02_CFG_CD_NUM_32         (0x5U << 4)
#define ADS131M02_CFG_CD_NUM_64         (0x6U << 4)
#define ADS131M02_CFG_CD_NUM_128        (0x7U << 4)
/** @} */

/**
 * @name  Current-detect measurement length selection
 * @{
 *
 * 000: 128 conversion periods (default)
 * 001: 256
 * 010: 512
 * 011: 768
 * 100: 1280
 * 101: 1792
 * 110: 2560
 * 111: 3584
 */
#define ADS131M02_CFG_CD_LEN_MASK       (BIT3 | BIT2 | BIT1)
#define ADS131M02_CFG_CD_LEN_128        (0x0U << 1)
#define ADS131M02_CFG_CD_LEN_256        (0x1U << 1)
#define ADS131M02_CFG_CD_LEN_512        (0x2U << 1)
#define ADS131M02_CFG_CD_LEN_768        (0x3U << 1)
#define ADS131M02_CFG_CD_LEN_1280       (0x4U << 1)
#define ADS131M02_CFG_CD_LEN_1792       (0x5U << 1)
#define ADS131M02_CFG_CD_LEN_2560       (0x6U << 1)
#define ADS131M02_CFG_CD_LEN_3584       (0x7U << 1)
/** @} */

/**
 * @brief  Current-detect mode enable
 *
 * 0: Disabled (default)
 * 1: Enabled
 */
#define ADS131M02_CFG_CD_EN_MASK        BIT0

/**
 * @brief  THRREG_MSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_THRREG_MSB        0x07U
/**
 * @brief  Current-detect threshold MSB bits [23:8]
 */
#define ADS131M02_THRREG_MSB_MASK       (0xFFFFU)

/**
 * @brief  THRREG_LSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_THRREG_LSB        0x08U
/**
 * @brief  Current-detect threshold LSB bits [7:0]
 */
#define ADS131M02_THRREG_LSB_CD_TH_MASK (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
/**
 * @name  DC block filter setting, see Table 8-4for details
 * @{
 *
 * Value of coefficient a
 *
 * 0000: DC block filter disabled
 * 0001: 1/4
 * 0010: 1/8
 * 0011: 1/16
 * 0100: 1/32
 * 0101: 1/64
 * 0110: 1/128
 * 0111: 1/256
 * 1000: 1/512
 * 1001: 1/1024
 * 1010: 1/2048
 * 1011: 1/4096
 * 1100: 1/8192
 * 1101: 1/16384
 * 1110: 1/32768
 * 1111: 1/65536
 */
#define ADS131M02_THRREG_LSB_DCBLOCK_MASK       (BIT3 | BIT2 | BIT1 | BIT0)
#define ADS131M02_THRREG_LSB_DCBLOCK_DISABLED   (0x0U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_4        (0x1U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_8        (0x2U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_16       (0x3U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_32       (0x4U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_64       (0x5U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_128      (0x6U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_256      (0x7U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_512      (0x8U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_1024     (0x9U)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_2048     (0xAU)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_4096     (0xBU)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_8192     (0xCU)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_16384    (0xDU)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_32768    (0xEU)
#define ADS131M02_THRREG_LSB_DCBLOCK_1_65536    (0xFU)
/** @} */

/**
 * @brief  CH0_CFG Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH0_CFG           0x09U
/**
 * @brief  Channel 0 phase compensation delay
 * 10-bit two's complement value.
 */
#define ADS131M02_CH0_CFG_PHASE0_MASK   (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6)
/**
 * @brief  Channel 0 DC block filter disable
 *
 * 0: Controlled by DCBLOCK[3:0] (default)
 * 1: Disabled for this channel
 */
#define ADS131M02_CH0_CFG_DCBLK0_DIS0_MASK      BIT2
/**
 * @name  Channel 0 input multiplexer
 * @{
 *
 * The input multiplexer allows the following inputs to be connected to the ADC channel:
 * The analog input pins corresponding to the given channel
 * AGND, which is helpful for offset calibration
 * Positive DC test signal
 * Negative DC test signal
 *
 * 00: AIN0P and AIN0N connected directly to ADC (default)
 * 01: ADC inputs shorted to (AIN0P + AIN0N) / 2
 * 10: Positive DC test signal
 * 11: Negative DC test signal
 */
#define ADS131M02_CH0_CFG_MUX_MASK              (BIT1 | BIT0)
#define ADS131M02_CH0_CFG_MUX_AIN0P_AIN0N       (0x0U)
#define ADS131M02_CH0_CFG_MUX_SHORTED           (0x1U)
#define ADS131M02_CH0_CFG_MUX_POS_DC            (0x2U)
#define ADS131M02_CH0_CFG_MUX_NEG_DC            (0x3U)
/** @} */

/**
 * @brief  CH0_OCAL_MSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH0_OCAL_MSB              0x0AU
/**
 * @brief  Channel 0 offset calibration MSB bits [23:8]
 */
#define ADS131M02_CH0_OCAL_MSB_MASK             (0xFFFFU)

/**
 * @brief  CH0_OCAL0_LSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH0_OCAL0_LSB             0x0BU
/**
 * @brief  Channel 0 offset calibration LSB bits [7:0]
 */
#define ADS131M02_CH0_OCAL0_LSB_MASK            (0xFF00U)

/**
 * @brief  CH0_GCAL_MSB Register
 * Reset Value: 0x8000
 */
#define ADS131M02_REG_CH0_GCAL_MSB              0x0CU
/**
 * @brief  Channel 0 gain calibration MSB bits [23:8]
 */
#define ADS131M02_CH0_GCAL_MSB_MASK             (0xFFFFU)

/**
 * @brief  CH0_GCAL_LSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH0_GCAL_LSB              0x0DU
/**
 * @brief  Channel 0 gain calibration LSB bits [7:0]
 */
#define ADS131M02_CH0_GCAL_LSB_MASK             (0xFF00U)

/**
 * @brief  CH1_CFG Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH1_CFG                   0x0EU
/**
 * @brief  Channel 1 phase compensation delay
 * 10-bit two's complement value.
 */
#define ADS131M02_CH1_CFG_PHASE1_MASK           (BIT15 | BIT14 | BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6)
/**
 * @brief  Channel 1 DC block filter disable
 *
 * 0: Controlled by DCBLOCK[3:0] (default
 * 1: Disabled for this channel
 */
#define ADS131M02_CH1_CFG_DCBLK1_DIS0_MASK      BIT2
/**
 * @name  Channel 1 input multiplexer selection
 * @{
 *
 * 00: AIN1P and AIN1N connected directly to ADC (default)
 * 01: ADC inputs shorted to (AIN1P + AIN1N) / 2
 * 10: Positive DC test signal
 * 11: Negative DC test signal
 */
#define ADS131M02_CH1_CFG_MUX_MASK              (BIT1 | BIT0)
#define ADS131M02_CH1_CFG_MUX_AIN1P_AIN1N       (0x0U)
#define ADS131M02_CH1_CFG_MUX_SHORTED           (0x1U)
#define ADS131M02_CH1_CFG_MUX_POS_DC            (0x2U)
#define ADS131M02_CH1_CFG_MUX_NEG_DC            (0x3U)

typedef enum {
    ads131m02_mux_ain0p_ain0n = ADS131M02_CH0_CFG_MUX_AIN0P_AIN0N,
    ads131m02_mux_shorted = ADS131M02_CH0_CFG_MUX_SHORTED,
    ads131m02_mux_pos_dc = ADS131M02_CH0_CFG_MUX_POS_DC,
    ads131m02_mux_neg_dc = ADS131M02_CH0_CFG_MUX_NEG_DC,
} ads131m02_mux_t;
/** @} */

/**
 * @brief  CH1_OCAL1_MSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH1_OCAL_MSB              0x0FU
/**
 * @brief  Channel 1 offset calibration MSB bits [23:8]
 */
#define ADS131M02_CH1_OCAL1_MSB_MASK            (0xFFFFU)

/**
 * @brief  CH1_OCAL1_LSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH1_OCAL_LSB              0x10U
/**
 * @brief  Channel 1 offset calibration LSB bits [7:0]
 */
#define ADS131M02_CH1_OCAL1_LSB_MASK           (0xFF00U)

/**
 * @brief  CH1_GCAL_MSB Register
 * Reset Value: 0x8000
 */
#define ADS131M02_REG_CH1_GCAL_MSB              0x11U
/**
 * @brief  Channel 1 gain calibration MSB bits [23:8]
 */
#define ADS131M02_CH1_GCAL1_MSB_MASK            (0xFFFFU)

/**
 * @brief  CH1_GCAL_LSB Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_CH1_GCAL_LSB              0x12U
/**
 * @brief  Channel 1 gain calibration LSB bits [7:0]
 */
#define ADS131M02_CH1_GCAL1_LSB_MASK            (0xFF00U)

/**
 * @brief  REGMAP_CRC Register
 * Reset Value: 0x0000
 */
#define ADS131M02_REG_REGMAP_CRC                0x3EU
/**
 * @brief  Register map CRC value
 */
#define ADS131M02_REGMAP_CRC_MASK               (0xFFFFU)

#ifdef __cplusplus
}
#endif

/** @} */
