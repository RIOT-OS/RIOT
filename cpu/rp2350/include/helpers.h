#pragma once

#include "RP2350.h"

#define ATOMIC_XOR_WRITE 0x1000
#define ATOMIC_BITMASK_SET_WRITE 0x2000
#define ATOMIC_BITMASK_CLEAR_WRITE 0x3000

/**
 * @brief Perform an atomic XOR write to a register
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Value to be XORed with the register
 */
static inline void atomic_xor(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_XOR_WRITE) = val;
}

/**
 * @brief Set bits in a register atomically
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Bit mask of bits to set
 */
static inline void atomic_set(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_SET_WRITE) = val;
}

/**
 * @brief Clear bits in a register atomically
 *
 * @param[in] reg   Pointer to the target register
 * @param[in] val   Bit mask of bits to clear
 */
static inline void atomic_clear(volatile uint32_t *reg, uint32_t val)
{
    *(volatile uint32_t *)((uintptr_t)reg | ATOMIC_BITMASK_CLEAR_WRITE) = val;
}

static inline void reset_component(uint32_t reset_value, uint32_t reset_done_value)
{
    atomic_clear(&RESETS->RESET, reset_value);
    while(~RESETS->RESET_DONE & reset_done_value) {
        // Wait for the reset to complete
    }
}
