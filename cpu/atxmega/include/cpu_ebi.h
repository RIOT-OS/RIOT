/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atxmega
 * @{
 *
 * @file
 * @brief           External Bus Interface API
 *
 * @author          Gerson Fernando Budke <nandojve@gmail.com>
 */

#include "periph_cpu.h"

#ifndef CPU_EBI_H
#define CPU_EBI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t hugemem_ptr_t;

#define HUGEMEM_NULL        0

void ebi_init(void);

/**
 * @brief Load byte register from external memory pointer
 *
 * @param  from     24-bit external memory address
 * @return uint8_t  8-bit value
 */
static inline uint8_t hugemem_read8(const hugemem_ptr_t from)
{
    uint8_t value;

    __asm__ volatile (
        /* place 24 bit address into r30, r31, RAMPZ
         * r31:r30     <- %B[from]:%A[from]
         * RAMPZ       <- %C[from]
         */
        "movw   r30,        %A[from]           \n\t"
        "out    %[rampz],   %C[from]           \n\t"
        /* read byte from 24 bit address
         * register    <- *(RAMPZ + Z)
         */
        "ld     %[dest],    Z                  \n\t"
        /* clear ramp */
        "out    %[rampz],   __zero_reg__       \n\t"
        : [dest]    "=r"(value)
        : [from]    "r"(from),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );

    return value;
}

/**
 * @brief Store byte register to external memory pointer
 *
 * @param  to       24-bit external memory address
 * @param  val      8-bit value
 */
static inline void hugemem_write8(hugemem_ptr_t to, uint8_t val)
{
    __asm__ volatile (
        /* place 24 bit address into r30, r31, RAMPZ
         * r31:r30     <- %B[from]:%A[from]
         * RAMPZ       <- %C[from]
         */
        "movw   r30,        %A[to]             \n\t"
        "out    %[rampz],   %C[to]             \n\t"
        /* write byte to 24 bit address
         * (RAMPZ + Z) <- register
         */
        "st     Z,          %[val]             \n\t"
        /* clear ramp */
        "out    %[rampz],   __zero_reg__       \n\t"
        :
        : [to]      "r"(to),
          [val]     "r"(val),
          [rampz]   "i"(&RAMPZ)
        : "r30", "r31"
    );
}

/**
 * @brief Load register pair from external memory pointer
 *
 * @param  from     24-bit external memory address
 * @return uint16_t 8-bit value
 */
uint16_t hugemem_read16(const hugemem_ptr_t from);

/**
 * @brief Store register pair to external memory pointer
 *
 * @param  to      24-bit external memory address
 * @param  val     16-bit value
 */
void hugemem_write16(hugemem_ptr_t to, uint16_t val);

/**
 * @brief Load two register pair from external memory pointer
 *
 * @param  from    24-bit external memory address
 * @return uint8_t 32-bit value
 */
uint32_t hugemem_read32(const hugemem_ptr_t from);

/**
 * @brief Store two register pair to external memory pointer
 *
 * @param  to      24-bit external memory address
 * @param  val     32-bit value
 */
void hugemem_write32(hugemem_ptr_t to, uint32_t val);

/**
 * @brief Read byte stream from external memory to internal memory
 *
 * @param to       intenal memory pointer
 * @param from     24-bit external memory pointer
 * @param size     number of bytes to read
 *
 * @note  The address range to copy from is within 64 kB boundary
 */
void hugemem_read_block(void *to, const hugemem_ptr_t from, size_t size);

/**
 * @brief Write byte stream from internal memory to external memory
 *
 * @param to       24-bit external memory pointer
 * @param from     intenal memory pointer
 * @param size     number of bytes to write
 *
 * @note  The address range to copy to is within 64 kB boundary
 */
void hugemem_write_block(hugemem_ptr_t to, const void *from, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* CPU_EBI_H */
/** @} */
