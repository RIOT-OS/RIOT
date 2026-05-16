/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc38@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_qma6100p
 * @{
 *
 * @file
 * @brief       Register definitions for the QST QMA6100P accelerometer
 *
 * @author      Baptiste Le Duc <baptiste.leduc38@gmail.com>
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses (QST-PD-B002-22 Table 14 / section 9)
 * @{
 */
#define QMA6100P_REG_CHIP_ID        (0x00) /**< §9.2  Chip ID */
/* §9.3 Acceleration data — 14-bit two's complement, burst-readable 0x01–0x06.
 * LSB reg: bits[7:2]=data[5:0], bit[0]=NEWDATA flag (1=updated since last read).
 * MSB reg: bits[7:0]=data[13:6], bit[7] is the sign bit. */
#define QMA6100P_REG_DX_LSB         (0x01) /**< X low: data[5:0] in bits[7:2] */
#define QMA6100P_REG_DX_MSB         (0x02) /**< X high: data[13:6] */
#define QMA6100P_REG_DY_LSB         (0x03) /**< Y low */
#define QMA6100P_REG_DY_MSB         (0x04) /**< Y high */
#define QMA6100P_REG_DZ_LSB         (0x05) /**< Z low */
#define QMA6100P_REG_DZ_MSB         (0x06) /**< Z high */
#define QMA6100P_REG_STEP_CNT1      (0x07) /**< §9.4  Step count [7:0] */
#define QMA6100P_REG_STEP_CNT2      (0x08) /**< §9.4  Step count [15:8] */
#define QMA6100P_REG_INT_ST0        (0x09) /**< §9.5  Interrupt status 0 (motion/step) */
#define QMA6100P_REG_INT_ST1        (0x0A) /**< §9.6  Interrupt status 1 (tap/raise/sig) */
#define QMA6100P_REG_INT_ST2        (0x0B) /**< §9.7  Interrupt status 2 (FIFO/data) */
#define QMA6100P_REG_INT_ST3        (0x0C) /**< §9.8  Interrupt status 3 (tap sign) */
#define QMA6100P_REG_STEP_CNT3      (0x0D) /**< §9.4  Step count [23:16] (RESV in upper bits) */
#define QMA6100P_REG_FIFO_ST        (0x0E) /**< §9.9  FIFO frame counter */
#define QMA6100P_REG_RANGE          (0x0F) /**< §9.10 Full-scale range + LPF/HPF select */
#define QMA6100P_REG_ODR            (0x10) /**< §9.11 Output data rate + NLPF */
#define QMA6100P_REG_PM             (0x11) /**< §9.12 Power management (MODE, MCLK) */
#define QMA6100P_REG_STEP_CONF0     (0x12) /**< §9.13 Step config: STEP_EN, STEP_SAMPLE_CNT */
#define QMA6100P_REG_STEP_CONF1     (0x13) /**< §9.14 Step config: STEP_CLR, STEP_PRECISION */
#define QMA6100P_REG_STEP_CONF2     (0x14) /**< §9.15 Step config: STEP_TIME_LOW */
#define QMA6100P_REG_STEP_CONF3     (0x15) /**< §9.16 Step config: STEP_TIME_UP */
#define QMA6100P_REG_INT_EN0        (0x16) /**< §9.17 Interrupt enable: tap/step/raise */
#define QMA6100P_REG_INT_EN1        (0x17) /**< §9.18 Interrupt enable: FIFO/data */
#define QMA6100P_REG_INT_EN2        (0x18) /**< §9.19 Interrupt enable: any_mot/no_mot */
#define QMA6100P_REG_INT1_MAP0      (0x19) /**< §9.20 INT1 map: tap/step/raise/sig_mot */
#define QMA6100P_REG_INT1_MAP1      (0x1A) /**< §9.21 INT1 map: no_mot/FIFO/data/q_tap/any_mot */
#define QMA6100P_REG_INT2_MAP0      (0x1B) /**< §9.22 INT2 map: tap/step/raise/sig_mot */
#define QMA6100P_REG_INT2_MAP1      (0x1C) /**< §9.23 INT2 map: no_mot/FIFO/data/q_tap/any_mot */
#define QMA6100P_REG_STEP_CFG0      (0x1D) /**< §9.24 Step config: STEP_INTERVAL, En_Rst_Dc */
#define QMA6100P_REG_STEP_CFG1      (0x1E) /**< §9.25 Step config: NLPF_STEP, TAP_QUIET_TH */
#define QMA6100P_REG_STEP_CFG2      (0x1F) /**< §9.26 Step config: START_CNT, COUNT_PEAK, P2P */
#define QMA6100P_REG_INTPIN_CONF    (0x20) /**< §9.27 INT pin config (OD/LVL/SPI3W) */
#define QMA6100P_REG_INT_CFG        (0x21) /**< §9.28 Interrupt latch/shadow/I2C config */
#define QMA6100P_REG_RAISE_CFG0     (0x22) /**< §9.29 Raise-to-wake config 0 */
#define QMA6100P_REG_RAISE_CFG1     (0x23) /**< §9.30 Raise-to-wake config 1 */
#define QMA6100P_REG_RAISE_CFG2     (0x24) /**< §9.31 Raise-to-wake config 2 */
#define QMA6100P_REG_RAISE_CFG3     (0x25) /**< §9.32 Raise-to-wake config 3 */
#define QMA6100P_REG_RAISE_CFG4     (0x26) /**< §9.33 Raise-to-wake config 4 */
#define QMA6100P_REG_OS_CUST        (0x27) /**< §9.34 OS custom X (0x27), Y (0x28), Z (0x29) */
#define QMA6100P_REG_TAP_CFG0       (0x2A) /**< §9.35 Tap config 0 */
#define QMA6100P_REG_TAP_CFG1       (0x2B) /**< §9.36 Tap config 1 */
#define QMA6100P_REG_MOT_CFG0       (0x2C) /**< §9.37 Motion config 0 (no_mot/any_mot dur) */
#define QMA6100P_REG_MOT_CFG1       (0x2D) /**< §9.38 Motion config 1 (no_mot threshold) */
#define QMA6100P_REG_MOT_CFG2       (0x2E) /**< §9.39 Motion config 2 (any_mot threshold) */
#define QMA6100P_REG_MOT_CFG3       (0x2F) /**< §9.40 Motion config 3 (LPF/SIG_MOT sel) */
#define QMA6100P_REG_RST_MOT_CFG    (0x30) /**< §9.41 Reset motion detectors */
#define QMA6100P_REG_FIFO_WM_LVL   (0x31) /**< §9.42 FIFO watermark level */
#define QMA6100P_REG_SELFTEST       (0x32) /**< §9.43 Self-test */
#define QMA6100P_REG_NVM            (0x33) /**< §9.44 NVM control */
#define QMA6100P_REG_Y_TH_YZ_SEL   (0x34) /**< §9.45 Y/YZ threshold + select */
#define QMA6100P_REG_RAISE_WK_PER  (0x35) /**< §9.46 Raise wake period */
#define QMA6100P_REG_SW_RESET       (0x36) /**< §9.47 Soft reset / NVM unlock */
#define QMA6100P_REG_FIFO_CFG0      (0x3E) /**< §9.48 FIFO configuration */
#define QMA6100P_REG_FIFO_DATA      (0x3F) /**< §9.49 FIFO data output (I2C burst) */
#define QMA6100P_REG_ULPS           (0x46) /**< §9.50 Ultra-low-power state config */
#define QMA6100P_REG_AFE_ANA        (0x56) /**< §9.52 AFE analog test */
#define QMA6100P_REG_TST1_ANA       (0x5F) /**< §9.53 Digital config / step peak */
/** @} */

/**
 * @name    CHIP_ID (0x00)
 * Bits[7:4] = 0x9 (fixed). Bits[3:0] factory-set, software must ignore.
 * @{
 */
#define QMA6100P_CHIP_ID_MASK       (0xF0)
#define QMA6100P_CHIP_ID_VAL        (0x90)
/** @} */

/**
 * @name    Data LSB registers (0x01 / 0x03 / 0x05)
 * @{
 */
#define QMA6100P_NEWDATA_FLAG       (1 << 0) /**< 1 = channel updated since last read */
/** @} */

/**
 * @name    INT_ST0 (0x09) — motion / step interrupt status
 * @{
 */
#define QMA6100P_INT_ST0_ANY_MOT_X  (1 << 0) /**< any_motion triggered by X axis */
#define QMA6100P_INT_ST0_ANY_MOT_Y  (1 << 1) /**< any_motion triggered by Y axis */
#define QMA6100P_INT_ST0_ANY_MOT_Z  (1 << 2) /**< any_motion triggered by Z axis */
#define QMA6100P_INT_ST0_ANY_MOT_SG (1 << 3) /**< sign of triggering signal (1=negative) */
/* bits[5:4] RESERVED */
#define QMA6100P_INT_ST0_STEP_FLAG  (1 << 6) /**< step detected */
#define QMA6100P_INT_ST0_NO_MOT     (1 << 7) /**< no_motion interrupt active */
/** @} */

/**
 * @name    INT_ST1 (0x0A) — tap / raise / significant motion status
 * @{
 */
#define QMA6100P_INT_ST1_SIG_MOT    (1 << 0) /**< significant motion active */
#define QMA6100P_INT_ST1_RAISE      (1 << 1) /**< raise-hand interrupt active */
#define QMA6100P_INT_ST1_HD         (1 << 2) /**< hand-down interrupt active */
#define QMA6100P_INT_ST1_STEP       (1 << 3) /**< step valid interrupt active */
#define QMA6100P_INT_ST1_T_TAP      (1 << 4) /**< triple tap active */
#define QMA6100P_INT_ST1_D_TAP      (1 << 5) /**< double tap active */
#define QMA6100P_INT_ST1_SIG_STEP   (1 << 6) /**< significant step active */
#define QMA6100P_INT_ST1_S_TAP      (1 << 7) /**< single tap active */
/** @} */

/**
 * @name    INT_ST2 (0x0B) — FIFO / data-ready / quad-tap status
 * @{
 */
#define QMA6100P_INT_ST2_Q_TAP      (1 << 0) /**< quad tap active */
#define QMA6100P_INT_ST2_EARIN_FLAG (1 << 1) /**< raise-hand (ear-in) flag */
/* bits[3:2] RESERVED */
#define QMA6100P_INT_ST2_DATA_INT   (1 << 4) /**< data-ready interrupt active */
#define QMA6100P_INT_ST2_FIFO_FULL  (1 << 5) /**< FIFO full */
#define QMA6100P_INT_ST2_FIFO_WM    (1 << 6) /**< FIFO watermark reached */
#define QMA6100P_INT_ST2_FIFO_OR    (1 << 7) /**< FIFO over-run */
/** @} */

/**
 * @name    INT_ST3 (0x0C) — tap sign
 * @{
 */
#define QMA6100P_INT_ST3_TAP_SIGN   (1 << 7) /**< 0=negative direction, 1=positive */
/** @} */

/**
 * @name    RANGE (0x0F) — full-scale range and filter select
 * @{
 */
#define QMA6100P_RANGE_MASK         (0x0F)
#define QMA6100P_RANGE_2G           (0x01) /**< ±2g  — sensitivity 4096 LSB/g */
#define QMA6100P_RANGE_4G           (0x02) /**< ±4g  — sensitivity 2048 LSB/g */
#define QMA6100P_RANGE_8G           (0x04) /**< ±8g  — sensitivity 1024 LSB/g */
#define QMA6100P_RANGE_16G          (0x08) /**< ±16g — sensitivity  512 LSB/g */
#define QMA6100P_RANGE_32G          (0x0F) /**< ±32g — sensitivity  256 LSB/g */
/* bits[5:4] RESERVED, must be 0 */
#define QMA6100P_RANGE_LPF_HPF      (1 << 6) /**< 0=LPF mode, 1=HPF mode (affects NLPF in ODR) */
/** @} */

/**
 * @name    ODR (0x10) — output data rate and filter
 * NLPF[2:0] in bits[7:5], ODR[4:0] in bits[4:0].
 * ODR values assume MCLK=51.2KHz (default after init sequence).
 * @{
 */
#define QMA6100P_ODR_MASK           (0x1F)
#define QMA6100P_ODR_1600HZ         (0x04)
#define QMA6100P_ODR_800HZ          (0x03)
#define QMA6100P_ODR_400HZ          (0x02)
#define QMA6100P_ODR_200HZ          (0x01)
#define QMA6100P_ODR_100HZ          (0x00) /**< default / fallback */
#define QMA6100P_ODR_50HZ           (0x05)
#define QMA6100P_ODR_25HZ           (0x06)
#define QMA6100P_ODR_12HZ5          (0x07)
#define QMA6100P_NLPF_SHIFT         (5)
#define QMA6100P_NLPF_MASK          (0x07 << QMA6100P_NLPF_SHIFT)
/* NLPF when RANGE[LPF_HPF]=0 (LPF mode): 000=off, 100=1, x01=2, x10=4, x11=8 */
/* NLPF when RANGE[LPF_HPF]=1 (HPF mode): 000=off, 001=ODR/10, 010=ODR/25, etc. */
/** @} */

/**
 * @name    PM (0x11) — power management
 * Default 0x00 after POR — device is in standby. Write MODE=1 to activate.
 * @{
 */
#define QMA6100P_PM_MODE            (1 << 7) /**< 0=standby (POR default), 1=active */
/* bit6 RESERVED */
#define QMA6100P_PM_SINC_SHIFT      (4)
#define QMA6100P_PM_SINC_MASK       (0x03 << QMA6100P_PM_SINC_SHIFT) /**< T_RSTB_SINC[1:0] */
#define QMA6100P_PM_SINC_3MCLK      (0x00 << QMA6100P_PM_SINC_SHIFT)
#define QMA6100P_PM_SINC_4MCLK      (0x01 << QMA6100P_PM_SINC_SHIFT)
#define QMA6100P_PM_SINC_6MCLK      (0x02 << QMA6100P_PM_SINC_SHIFT)
#define QMA6100P_PM_SINC_8MCLK      (0x03 << QMA6100P_PM_SINC_SHIFT)
#define QMA6100P_PM_MCLK_MASK       (0x0F)   /**< MCLK[3:0] */
#define QMA6100P_PM_MCLK_51K2       (0x04)   /**< 51.2 kHz — recommended default */
#define QMA6100P_PM_MCLK_25K6       (0x05)
#define QMA6100P_PM_MCLK_12K8       (0x06)
#define QMA6100P_PM_MCLK_6K4        (0x07)
/** @} */

/**
 * @name    STEP_CONF0 (0x12) — step counter enable + sample count
 * @{
 */
#define QMA6100P_STEP_CONF0_EN      (1 << 7) /**< 1=enable step counter */
#define QMA6100P_STEP_CONF0_CNT_MASK (0x7F)  /**< STEP_SAMPLE_CNT[6:0] (actual = value*8) */
/** @} */

/**
 * @name    STEP_CONF1 (0x13) — step counter clear + precision
 * @{
 */
#define QMA6100P_STEP_CONF1_CLR     (1 << 7) /**< 1=clear step count registers */
#define QMA6100P_STEP_CONF1_PREC_MASK (0x7F) /**< STEP_PRECISION[6:0] */
/** @} */

/**
 * @name    INT_EN0 (0x16) — tap / step / raise interrupt enable
 * @{
 */
#define QMA6100P_INT_EN0_Q_TAP      (1 << 0)
#define QMA6100P_INT_EN0_RAISE      (1 << 1)
#define QMA6100P_INT_EN0_HD         (1 << 2)
#define QMA6100P_INT_EN0_STEP       (1 << 3)
#define QMA6100P_INT_EN0_T_TAP      (1 << 4)
#define QMA6100P_INT_EN0_D_TAP      (1 << 5)
#define QMA6100P_INT_EN0_SIG_STEP   (1 << 6)
#define QMA6100P_INT_EN0_S_TAP      (1 << 7)
/** @} */

/**
 * @name    INT_EN1 (0x17) — FIFO / data-ready interrupt enable
 * @{
 */
/* bits[3:0] RESERVED */
#define QMA6100P_INT_EN1_DATA       (1 << 4) /**< data-ready interrupt */
#define QMA6100P_INT_EN1_FFULL      (1 << 5) /**< FIFO full interrupt */
#define QMA6100P_INT_EN1_FWM        (1 << 6) /**< FIFO watermark interrupt */
/** @} */

/**
 * @name    INT_EN2 (0x18) — any_motion / no_motion interrupt enable
 * @{
 */
#define QMA6100P_INT_EN2_ANY_MOT_X  (1 << 0)
#define QMA6100P_INT_EN2_ANY_MOT_Y  (1 << 1)
#define QMA6100P_INT_EN2_ANY_MOT_Z  (1 << 2)
/* bits[4:3] RESERVED */
#define QMA6100P_INT_EN2_NO_MOT_X   (1 << 5)
#define QMA6100P_INT_EN2_NO_MOT_Y   (1 << 6)
#define QMA6100P_INT_EN2_NO_MOT_Z   (1 << 7)
/** @} */

/**
 * @name    INT1_MAP0 (0x19) — route interrupts to INT1 pin
 * @{
 */
#define QMA6100P_INT1_MAP0_SIG_MOT  (1 << 0)
#define QMA6100P_INT1_MAP0_RAISE    (1 << 1)
#define QMA6100P_INT1_MAP0_HD       (1 << 2)
#define QMA6100P_INT1_MAP0_STEP     (1 << 3)
#define QMA6100P_INT1_MAP0_T_TAP    (1 << 4)
#define QMA6100P_INT1_MAP0_D_TAP    (1 << 5)
#define QMA6100P_INT1_MAP0_SIG_STEP (1 << 6)
#define QMA6100P_INT1_MAP0_S_TAP    (1 << 7)
/** @} */

/**
 * @name    INT1_MAP1 (0x1A)
 * @{
 */
#define QMA6100P_INT1_MAP1_ANY_MOT  (1 << 0)
#define QMA6100P_INT1_MAP1_Q_TAP    (1 << 1)
/* bits[3:2] RESERVED */
#define QMA6100P_INT1_MAP1_DATA     (1 << 4)
#define QMA6100P_INT1_MAP1_FFULL    (1 << 5)
#define QMA6100P_INT1_MAP1_FWM      (1 << 6)
#define QMA6100P_INT1_MAP1_NO_MOT   (1 << 7)
/** @} */

/**
 * @name    INT2_MAP0 (0x1B) — route interrupts to INT2 pin
 * @{
 */
#define QMA6100P_INT2_MAP0_SIG_MOT  (1 << 0)
#define QMA6100P_INT2_MAP0_RAISE    (1 << 1)
#define QMA6100P_INT2_MAP0_HD       (1 << 2)
#define QMA6100P_INT2_MAP0_STEP     (1 << 3)
#define QMA6100P_INT2_MAP0_T_TAP    (1 << 4)
#define QMA6100P_INT2_MAP0_D_TAP    (1 << 5)
#define QMA6100P_INT2_MAP0_SIG_STEP (1 << 6)
#define QMA6100P_INT2_MAP0_S_TAP    (1 << 7)
/** @} */

/**
 * @name    INT2_MAP1 (0x1C)
 * @{
 */
#define QMA6100P_INT2_MAP1_ANY_MOT  (1 << 0)
#define QMA6100P_INT2_MAP1_Q_TAP    (1 << 1)
/* bits[3:2] RESERVED */
#define QMA6100P_INT2_MAP1_DATA     (1 << 4)
#define QMA6100P_INT2_MAP1_FFULL    (1 << 5)
#define QMA6100P_INT2_MAP1_FWM      (1 << 6)
#define QMA6100P_INT2_MAP1_NO_MOT   (1 << 7)
/** @} */

/**
 * @name    INTPIN_CONF (0x20) — INT pin electrical config
 * Default 0x05 = INT1_LVL active-high push-pull, INT2_LVL active-high push-pull.
 * @{
 */
#define QMA6100P_INTPIN_INT1_LVL    (1 << 0) /**< 0=active-low, 1=active-high */
#define QMA6100P_INTPIN_INT1_OD     (1 << 1) /**< 0=push-pull, 1=open-drain */
#define QMA6100P_INTPIN_INT2_LVL    (1 << 2)
#define QMA6100P_INTPIN_INT2_OD     (1 << 3)
/* bit4: STEP_COUNT_PEAK[2] — MSB of peak field, rest in 0x1F */
#define QMA6100P_INTPIN_EN_SPI3W    (1 << 5) /**< 0=4W SPI, 1=3W SPI */
#define QMA6100P_INTPIN_DIS_IE_AD0  (1 << 6) /**< 1=disable AD0 input, fixes I2C addr to 0x12 */
#define QMA6100P_INTPIN_DIS_PU_SENB (1 << 7) /**< 1=disable pull-up on SENB */
/** @} */

/**
 * @name    INT_CFG (0x21) — interrupt latch / shadow / interface config
 * @{
 */
#define QMA6100P_INT_CFG_LATCH_INT      (1 << 0) /**< 0=non-latch, 1=latch mode */
#define QMA6100P_INT_CFG_LATCH_INT_STEP (1 << 1) /**< latch for step interrupt */
/* bit2 RESERVED */
#define QMA6100P_INT_CFG_DIS_PU_SDX    (1 << 3) /**< 1=disable pull-up on SDX */
#define QMA6100P_INT_CFG_PP_I3C        (1 << 4) /**< 0=open-drain SDX, 1=push-pull */
#define QMA6100P_INT_CFG_DIS_I2C       (1 << 5) /**< 1=disable I2C (SPI-only mode) */
#define QMA6100P_INT_CFG_SHADOW_DIS    (1 << 6) /**< 1=disable shadowing for accel data */
#define QMA6100P_INT_CFG_INT_RD_CLR    (1 << 7) /**< 1=any read 0x09-0x0D clears all ints */
/** @} */

/**
 * @name    FIFO_CFG0 (0x3E)
 * @{
 */
#define QMA6100P_FIFO_CFG0_EN_X         (1 << 0)
#define QMA6100P_FIFO_CFG0_EN_Y         (1 << 1)
#define QMA6100P_FIFO_CFG0_EN_Z         (1 << 2)
#define QMA6100P_FIFO_CFG0_MODE_SHIFT   (6)
#define QMA6100P_FIFO_CFG0_MODE_MASK    (0x03 << QMA6100P_FIFO_CFG0_MODE_SHIFT)
#define QMA6100P_FIFO_CFG0_MODE_BYPASS  (0x00 << QMA6100P_FIFO_CFG0_MODE_SHIFT)
#define QMA6100P_FIFO_CFG0_MODE_FIFO    (0x01 << QMA6100P_FIFO_CFG0_MODE_SHIFT)
#define QMA6100P_FIFO_CFG0_MODE_STREAM  (0x02 << QMA6100P_FIFO_CFG0_MODE_SHIFT)
/** @} */

/**
 * @name    SW_RESET (0x36)
 * @{
 */
#define QMA6100P_SW_RESET_VAL       (0xB6) /**< Trigger soft reset */
#define QMA6100P_NVM_UNLOCK_VAL     (0xB8) /**< Unlock NVM for programming */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
