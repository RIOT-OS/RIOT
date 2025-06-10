/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include "RP2350.h"

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Helper functions for atomic register operations
 *
 * @author          Tom Hert <git@annsann.eu>
 */

/** Bit to be set for an atomic XOR operation */
#define ATOMIC_XOR_WRITE 0x1000u
/** Bit to be set for an atomic set operation */
#define ATOMIC_BITMASK_SET_WRITE 0x2000u
/** Bits to be set for an atomic clear operation */
#define ATOMIC_BITMASK_CLEAR_WRITE 0x3000u

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform an atomic XOR write to a register
 *
 * @param[in,out] reg   Pointer to the target register
 * @param[in] val       Value to be XORed with the register
 */
static inline void atomic_xor(volatile uint32_t *reg, uint32_t val) {
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_XOR_WRITE) = val;
}

/**
 * @brief Set bits in a register atomically
 *
 * @param[in,out] reg   Pointer to the target register
 * @param[in] val       Bit mask of bits to set
 */
static inline void atomic_set(volatile uint32_t *reg, uint32_t val) {
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_SET_WRITE) = val;
}

/**
 * @brief Clear bits in a register atomically
 *
 * @param[in,out] reg   Pointer to the target register
 * @param[in] val       Bit mask of bits to clear
 */
static inline void atomic_clear(volatile uint32_t *reg, uint32_t val) {
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_CLEAR_WRITE) = val;
}

/**
 * @brief Reset a component by clearing its reset bits and waiting for the reset to complete
 *
 * @param reset_value       Bit mask of the reset bits to clear
 * @param reset_done_value  Bit mask of the reset done bits to wait for
 */
static inline void reset_component(uint32_t reset_value,
    uint32_t reset_done_value) {
    atomic_clear(&RESETS->RESET, reset_value);
    while (~RESETS->RESET_DONE & reset_done_value) {
        /* Wait for the reset to complete */
    }
}

#ifdef __cplusplus
}
#endif

/** @} */
