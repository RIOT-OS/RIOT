/*
 * Copyright (C) 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           RISC-V PMP configuration options
 *
 * RISCV implementations using this peripheral must define the `PMP_REGION_COUNT`
 * `PMP_REGION_COUNT` must be 16 or 64.
 *
 * @author          Bennet Blischke
 */

#ifndef PMP_H
#define PMP_H

#include <assert.h>
#include <stdint.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Region definitions per use-case, depending on the priority and the 
 * number of available regions on a given system
 * @{
 */
#if PMP_REGION_COUNT < 4
#error "You do not have enough PMP entries to run RIOT"
#endif
#define PMP_REGION_STACK_GUARD (PMP_REGION_COUNT - 4) /**< Used for the PMP_STACK_GUARD module */
/* One region left free to account for silicon bugs in the hifive1b / fe310 */
#define PMP_REGION_NOEXEC_RAM (PMP_REGION_COUNT - 2) /**< Used for the PMP_NOEXEC_RAM module */
#define PMP_REGION_ALLOW_ALL (PMP_REGION_COUNT - 1) /**< Used as a catch-all fall back */
/** @} */

/**
 * @name    Bit masks for the PMP configuration register
 * @{
 */
#define PMP_NONE    0x00    /**< No access allowed at all */
#define PMP_R       0x01    /**< Allow read access */
#define PMP_W       0x02    /**< Allow write access */
#define PMP_X       0x04    /**< Allow execution */

#define PMP_A       0x18    /**< Addressing mode mask */
#define PMP_OFF     0x00    /**< Disable this pmp entry */
#define PMP_TOR     0x08    /**< Top-of-range addressing mode */
#define PMP_NA4     0x10    /**< Naturally aligned four-byte region */
#define PMP_NAPOT   0x18    /**< Naturally aligned power-of-two region, ≥8 bytes */

#define PMP_L       0x80    /**< Lock; read-only config & applies to machine-mode */
/** @} */

/**
 * @brief   Create a NAPOT formatted address
 *
 * @param   addr        Base address, must be aligned to the size of the region
 * @param   size        Size of the region in bytes
 */
static inline uint32_t make_napot(uint32_t addr, uint32_t size)
{
    assert(addr % size == 0);
    return addr | ((size - 1) >> 1);
}

/**
 * @brief A NAPOT formatted address that encodes the entire 34-Bit address space
 */
#define PMP_NAPOT_ENTIRE_ADDRESS_SPACE UINT32_MAX

/**
 * @brief   Writes a complete pmpcfg register
 *
 * @param   reg_num     Register number
 * @param   value       Value to write
 */
void write_pmpcfg(uint8_t reg_num, uint32_t value);

/**
 * @brief Read a complete pmpcfg register
 *
 * @param[in] reg_num   Register number
 *
 * @return Contents of the specified register
 */
uint32_t read_pmpcfg(uint8_t reg_num);

/**
 * @brief   Writes a complete pmpaddr register
 *
 * @param   reg_num     Register number
 * @param   value       Value to write
 */
void write_pmpaddr(uint8_t reg_num, uint32_t value);

/**
 * @brief Read a complete pmpaddr register
 *
 * @param[in] reg_num   Register number
 *
 * @return Contents of the specified register
 */
uint32_t read_pmpaddr(uint8_t reg_num);

/**
 * @brief Read a single pmpcfg sub-register
 *
 * @param[in] entry     Sub-register number
 *
 * @return Contents of the specified sub-register
 */
uint8_t get_pmpcfg(uint8_t entry);

/**
 * @brief   Set's a single pmpcfg sub-register
 *
 * @param   entry       Sub-register number
 * @param   value       Value to write
 */
void set_pmpcfg(uint8_t entry, uint8_t value);

/**
 * @brief   Prints a single pmpcfg sub-register human readable
 *
 * @param   entry     Register number to print
 */
void print_pmpcfg(uint8_t entry);

#ifdef __cplusplus
}
#endif

#endif /* PMP_H */
/** @} */
