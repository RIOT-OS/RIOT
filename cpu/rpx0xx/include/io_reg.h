/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_rpx0xx
 * @{
 *
 * @file
 * @brief           RP2040 atomic register access macros
 * @details         This allows individual fields of a control register to be
 *                  modified without performing a read-modify-write sequence.
 *                  See section "2.1.2. Atomic Register Access" in
 *                  https://datasheets.raspberrypi.org/rpx0xx/rpx0xx-datasheet.pdf
 *
 * @warning         The Single-cycle IO block (SIO), which contains the GPIO, does not support
 *                  atomic access using these aliases.
 *
 * @author          Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef IO_REG_H
#define IO_REG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Bit to be set if an atomic XOR operation shall be done
 */
#define REG_ATOMIC_XOR_BIT      (0x1000U)

/**
 * @brief   Bit to be set if an atomic set operation shall be done
 */
#define REG_ATOMIC_SET_BIT      (0x2000U)

/**
 * @brief   Bits to be set if an atomic clear operation shall be done
 */
#define REG_ATOMIC_CLEAR_BIT    (0x3000U)

/**
 * @brief   The operation to be performed to the register at address @p reg
 *          will be an atomic XOR of the bits of the right-hand-side operand
 */
#define REG_ATOMIC_XOR(reg)     ((volatile uint32_t *)(((uintptr_t)(reg)) | REG_ATOMIC_XOR_BIT))

/**
 * @brief   The operation to be performed to the register at address @p reg
 *          will be an atomic set of the bits of the right-hand-side operand
 */
#define REG_ATOMIC_SET(reg)     ((volatile uint32_t *)(((uintptr_t)(reg)) | REG_ATOMIC_SET_BIT))

/**
 * @brief   The operation to be performed to the register at address @p reg
 *          will be an atomic clear of the bits of the right-hand-side operand
 */
#define REG_ATOMIC_CLEAR(reg)   ((volatile uint32_t *)(((uintptr_t)(reg)) | REG_ATOMIC_CLEAR_BIT))

/**
 * @brief   Performed an atomic XOR of the set bits in @p op
 *          with the bits in the register at address @p reg
 *
 * @param   reg     Register address
 * @param   mask    Mask of bits to be XORed
 */
static inline void io_reg_atomic_xor(volatile uint32_t *reg, uint32_t mask)
{
    *REG_ATOMIC_XOR(reg) = mask;
}

/**
 * @brief   Set the bits in the register at address @p reg as given by
 *          the set bits in operand @p op
 *
 * @param   reg     Register address
 * @param   mask    Mask of bits to be set
 */
static inline void io_reg_atomic_set(volatile uint32_t *reg, uint32_t mask)
{
    *REG_ATOMIC_SET(reg) = mask;
}

/**
 * @brief   Clear the bits in the register at address @p reg as given by
 *          the set bits in operand @p op
 *
 * @param   reg     Register address
 * @param   mask    Mask of bits to be cleared
 */
static inline void io_reg_atomic_clear(volatile uint32_t *reg, uint32_t mask)
{
    *REG_ATOMIC_CLEAR(reg) = mask;
}

/**
 * @brief   Updates part of an I/O register without corrupting its contents
 *
 * @param   reg     Register address
 * @param   value   bits to set in the register (e.g. the new value shifted to the right position)
 * @param   mask    bits to clear in the register (e.g. bitmask indicating the part to update with
 *                  one bits, and everything else with zero bits)
 *
 * @details         The RP2040 only correctly implements 32 bit writes to I/O register. When
 *                  updating parts of an I/O register using the structs in the CMSIS header, the
 *                  C compiler might implement those updates e.g. using 8-bit writes, especially
 *                  when the part to update is 8-bit in size and byte-aligned. The RP2040 will then
 *                  sneakily corrupt the I/O register, rather than triggering a hard fault. This
 *                  hard to debug silent corruption issue is *NOT* a bug, but an intentional
 *                  feature, obviously.
 * @pre             `value & (~mask) == 0`
 *
 * Example use:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * io_reg_write_dont_corrupt(&CLOCKS->CLK_SYS_CTRL.reg, source << CLOCKS_CLK_SYS_CTRL_SRC_Pos,
 *                           CLOCKS_CLK_SYS_CTRL_SRC_Msk);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static inline void io_reg_write_dont_corrupt(volatile uint32_t *reg, uint32_t value, uint32_t mask)
{
    *reg = (*reg & (~mask)) | value;
}

#ifdef __cplusplus
}
#endif

#endif /* IO_REG_H */
/** @} */
