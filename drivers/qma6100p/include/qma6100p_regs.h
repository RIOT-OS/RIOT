/*
 * SPDX-FileCopyrightText: 2025 Baptiste Le Duc <baptiste.leduc@etik.com>
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
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 * @author      Léandre Le Duc <leandre.leduc38@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define BIT(n)                 (1UL << (n))                            /**< single-bit mask at position n */
#define GENMASK(h, l)          (((1UL << ((h) - (l) + 1)) - 1) << (l)) /**< contiguous bitmask from bit l to bit h */

#define FIELD_CLEAR(mask, reg) ((reg) &= ~(mask))                        /**< clear bits in reg covered by mask */
#define FIELD_PREP(mask, val)  (((val) << __builtin_ctz(mask)) & (mask)) /**< shift val into the field defined by mask */

/**
 * @brief Set a bitfield in reg to val, using mask to locate the field
 *
 * Clears the bits covered by mask in reg, then ORs in val shifted into
 * position (shift derived from the lowest set bit of mask)
 *
 * @param[in]     mask  bitmask of the target field
 * @param[in]     val   unshifted value to write into the field
 * @param[in,out] reg   register variable to modify
 */
#define FIELD_SET(mask, val, reg)       \
    do {                                \
        FIELD_CLEAR(mask, reg);         \
        (reg) |= FIELD_PREP(mask, val); \
    } while (0)

/**
 * @name    Register addresses (QST-PD-B002-22 Table 14 / section 9)
 * @{
 */
#define QMA6100P_REG_CHIP_ID             (0x00) /**< Chip ID */

/**
 * @name    Acceleration data registers (0x01–0x06)
 *
 * 14-bit two's complement, burst-readable
 * LSB[7:2]=data[5:0], LSB[0]=NEWDATA
 * MSB[7:0]=data[13:6], MSB[7]=sign
 * Pairs: DX=0x01-02, DY=0x03-04, DZ=0x05-06
 *
 * @{
 */
#define QMA6100P_REG_DX_LSB              (0x01) /**< X low: data[5:0] in bits[7:2] */
#define QMA6100P_REG_DX_MSB              (0x02) /**< X high: data[13:6] */
#define QMA6100P_REG_DY_LSB              (0x03) /**< Y low */
#define QMA6100P_REG_DY_MSB              (0x04) /**< Y high */
#define QMA6100P_REG_DZ_LSB              (0x05) /**< Z low */
#define QMA6100P_REG_DZ_MSB              (0x06) /**< Z high */
/** @} */

/**
 * @name    Control and configuration registers
 * @{
 */
#define QMA6100P_REG_RANGE               (0x0F) /**< Full-scale range + LPF/HPF select */
#define QMA6100P_REG_ODR                 (0x10) /**< Output data rate + NLPF */
#define QMA6100P_REG_PM                  (0x11) /**< Power management (MODE, MCLK) */
#define QMA6100P_REG_SW_RESET            (0x36) /**< Soft reset */
#define QMA6100P_REG_NVM                 (0x33) /**< Non Volatile Memory */
#define QMA6100P_REG_CHIP_STATE          (0x45) /**< Chip state */
#define QMA6100P_REG_ULPS                (0x46) /**< ULPS control register */
#define QMA6100P_REG_TST0_ANA            (0x4A) /**< Analog test register 0 */
/** @} */

/**
 * @name    Interrupt registers
 * @{
 */
#define QMA6100P_REG_INT_ST2             (0x0B) /**< Interrupt status 2 (FIFO/data) */
#define QMA6100P_REG_INT_EN1             (0x17) /**< Interrupt enable register */
#define QMA6100P_REG_INT1_MAP1           (0x1A) /**< INT1 map: no_mot/FIFO/data/q_tap/any_mot */
#define QMA6100P_REG_INT2_MAP1           (0x1C) /**< INT2 map: no_mot/FIFO/data/q_tap/any_mot */
#define QMA6100P_REG_INTPIN_CONF         (0x20) /**< INT pin config (OD/LVL) */
#define QMA6100P_REG_INT_CFG             (0x21) /**< Interrupt latch/shadow/I2C config */
/** @} */
/** @} */

/**
 * @name    CHIP_ID (0x00) masks — chip ID field
 * Bits[7:4] = 0x9 (fixed). Bits[3:0] factory-set, software must ignore
 * @{
 */
#define QMA6100P_CHIP_ID_MASK            (0xF0) /**< mask for the fixed upper nibble */
#define QMA6100P_CHIP_ID_VAL             (0x90) /**< expected upper nibble (0x9x), lower nibble is factory-set */
/** @} */

/**
 * @name    Data QMA6100P_REG_D(X|Y|Z)_LSB masks — new-data flag
 * @{
 */
#define QMA6100P_NEWDATA_FLAG_MASK       BIT(0) /**< 1 = channel updated since last read */
/** @} */

/**
 * @name    INT_ST2 (0x0B) masks — data-ready status
 * @{
 */
#define QMA6100P_INT_ST2_DATA_INT_MASK   BIT(4) /**< data-ready interrupt active */
/** @} */

/**
 * @name    RANGE (0x0F) masks — full-scale range and filter select
 * @{
 */
#define QMA6100P_RANGE_MASK              GENMASK(3, 0) /**< full-scale range bits[3:0] mask */
#define QMA6100P_RANGE_LPF_HPF_MASK      BIT(6)        /**< 0=LPF mode, 1=HPF mode (affects NLPF in ODR) */
/** @} */

/**
 * @name    ODR (0x10) masks — output data rate and filter
 * ODR[4:0] in bits[4:0], NLPF[2:0] in bits[7:5]
 *
 * NLPF oversampling ratio depends on RANGE[LPF_HPF]:
 * - LPF mode: 000=off, 100=×1, x01=×2, x10=×4, x11=×8
 * - HPF mode: 000=off, 001=ODR/10, 010=ODR/25, ...
 * @{
 */
#define QMA6100P_ODR_MASK                GENMASK(4, 0) /**< ODR bits[4:0] mask */
#define QMA6100P_NLPF_MASK               GENMASK(7, 5) /**< NLPF bits[7:5] mask */
/** @} */

/**
 * @name    PM (0x11) masks — power management
 * @{
 */
#define QMA6100P_PM_MODE_MASK            BIT(7)                           /**< active/standby mode bit mask */
#define QMA6100P_PM_MCLK_MASK            GENMASK(3, 0)                    /**< MCLK[3:0] */
/** @} */

/**
 * @name    INT_EN1 (0x17) masks — data-ready interrupt enable
 * @{
 */
#define QMA6100P_INT_EN1_DATA_MASK            BIT(4) /**< data-ready interrupt enable */
/** @} */

/**
 * @name    INT1_MAP1 (0x1A) / INT2_MAP1 (0x1C) masks — route data-ready to INT pin
 * @{
 */
#define QMA6100P_INT_MAP1_DATA_MASK           BIT(4) /**< route data-ready to INTx pin */
/** @} */

/**
 * @name    INTPIN_CONF (0x20) masks — INT pin electrical config
 * Default 0x05 = INT1 active-high push-pull, INT2 active-high push-pull
 * @{
 */
#define QMA6100P_INTPIN_INT1_LVL_MASK         BIT(0) /**< INT1 active level mask (@ref qma6100p_int_active_level_t) */
#define QMA6100P_INTPIN_INT1_OD_MASK          BIT(1) /**< INT1 output mode mask (@ref qma6100p_int_pin_mode_t) */
#define QMA6100P_INTPIN_INT2_LVL_MASK         BIT(2) /**< INT2 active level mask (@ref qma6100p_int_active_level_t) */
#define QMA6100P_INTPIN_INT2_OD_MASK          BIT(3) /**< INT2 output mode mask (@ref qma6100p_int_pin_mode_t) */
/** @} */

/**
 * @name    INT_CFG (0x21) masks — interrupt latch/shadow config
 * @{
 */
#define QMA6100P_INT_CFG_CLR_MASK        BIT(7) /**< INT_STATUS clear behavior mask (@ref qma6100p_int_clr_t) */
#define QMA6100P_INT_CFG_SHADOW_MASK     BIT(6) /**< data shadowing mode mask (@ref qma6100p_int_shadow_t) */
#define QMA6100P_INT_CFG_LATCH_MASK      BIT(0) /**< INT latch mode mask (@ref qma6100p_int_latch_t) */
/** @} */

/**
 * @name    SW_RESET (0x36) — soft reset
 * @{
 */
#define QMA6100P_SW_RESET_VAL            (0xB6) /**< trigger soft reset */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
