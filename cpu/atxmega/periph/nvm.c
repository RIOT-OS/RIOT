/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level NVM driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */
#include <avr/io.h>

#include "cpu_nvm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Read one byte using the Load Program Memory (LPM) instruction
 * @internal
 *
 * This function sets the specified NVM_CMD, reads one byte using at the
 * specified byte address with the LPM instruction. NVM_CMD is restored after
 * use.
 *
 * @note This will disable interrupts during execution.
 *
 * @param nvm_cmd NVM command to load before running LPM
 * @param address Byte offset into the signature row
 */
static inline uint8_t _nvm_read_byte(uint8_t nvm_cmd, uint16_t address)
{
    uint8_t result;

    __asm__ volatile (
        "in  __tmp_reg__, __SREG__ \n\t"
        "cli                       \n\t"
        "lds __zero_reg__, %3      \n\t"
        "sts %3, %1                \n\t"
        "lpm %0, %a2               \n\t"
        "sts %3, __zero_reg__      \n\t"
        "clr __zero_reg__          \n\t"
        "out __SREG__, __tmp_reg__ \n\t"
        :
        "=&r" (result)
        :
        "r" (nvm_cmd), "e" (address), "m" (NVM_CMD)
        : /* no clobbers */
    );

    return result;
}

/**
 * @brief Read one byte from the production signature row
 *
 * This function reads one byte from the production signature row of the device
 * at the given address.
 *
 * @note The execution can take some time. It is recommended not call this
 * inside an interrupt service routine.
 *
 * @param address Byte offset into the signature row
 */
uint8_t nvm_read_production_signature_row(uint8_t address)
{
    return _nvm_read_byte(NVM_CMD_READ_CALIB_ROW_gc, address);
}
