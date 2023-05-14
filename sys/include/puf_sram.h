/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     sys_puf_sram SRAM PUF
 * @ingroup      sys
 * @brief        SRAM based physically unclonable function (PUF)
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @warning      The SRAM based seed mechanism it not cryptographically secure in its current
                 state.
 *
 * # About
 *
 * Transistor variations of SRAM memory cells lead to different states
 * after device power-on. The startup state of multiple memory
 * blocks form a device-unique pattern plus additional noise ("weak PUF").
 * The noise is used to generate random numbers for PRNG seeding.
 *
 * # Preliminaries
 *
 * High entropy numbers can only be generated when the device starts from power-off (including
 * low-power modes that turn of the RAM partly)
 * and before the memory has been used. That's why the SRAM PUF procedure is implemented
 * even before kernel initialization.
 * Memory properties are hardware specific and can depend on environmental conditions.
 * Thus, they should be evaluated for each individual deployment. A basic
 * testing tool is provided in /RIOT/tests/sys/puf_sram.
 *
 * # Soft-reset detection
 *
 * In order to detect a software reboot without preceding power-off phase, a soft-reset
 * detection mechanism writes a marker memory @p PUF_SRAM_MARKER into SRAM. If the marker
 * is still present after a restart, a soft-reset is expected and the PUF procedure
 * is skipped.
 *
 * # Random Seed Generation
 *
 * Uninitialized memory pattern are compressed by the lightweight DEK hash function
 * to generate a high entropy 32-bit integer which can be used to seed a PRNG. This hash
 * function is not cryptographically secure and as such, adversaries might be able to
 * track parts of the initial SRAM response by analyzing PRNG sequences.
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#ifndef PUF_SRAM_H
#define PUF_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief SRAM length considered for seeding
 */
#ifndef SEED_RAM_LEN
#define SEED_RAM_LEN     (2048 / sizeof(uint32_t))
#endif

/**
 * @brief SRAM marker to detect reboot without power-off
 *
 * Source: https://www.random.org/bytes/
 */
#define PUF_SRAM_MARKER  (0xad3021ff)

/**
 * @brief Global seed variable, allocated in puf_sram.c
 */
extern uint32_t puf_sram_seed;

/**
 * @brief Global seed state, allocated in puf_sram.c
 *        0 means seed was generated from SRAM pattern,
          1 means missing power cycle detected,
          2 means power cycle detected. The state will most likely
            be overwritten with 0 in the next steps
 */
extern uint32_t puf_sram_state;

/**
 * @brief Counter variable allocated in puf_sram.c. It is incremented
          during each soft reset when no new PUF measurement was taken
          and it gets reset to zero after a power cycle was detected.
 */
extern uint32_t puf_sram_softreset_cnt;

/**
 * @brief checks source of reboot by @p puf_sram_softreset and conditionally
          calls @p puf_sram_generate
 *
 * @param[in] ram pointer to SRAM memory
 * @param[in] len length of the memory to consider
 *
 */
void puf_sram_init(const uint8_t *ram, size_t len);

/**
 * @brief builds hash from @p SEED_RAM_LEN bytes uninitialized SRAM, writes it
 *        to the global variable @p puf_sram_seed and returns the value
 *
 * @param[in] ram pointer to SRAM memory
 * @param[in] len length of the memory to consider
 */
void puf_sram_generate(const uint8_t *ram, size_t len);

/**
 * @brief checks for a memory marker to determine whether memory contains old data.
          Otherwise it assumes a reboot from power down mode
 *
 * @return    0 when reset with power cycle was detected
 * @return    1 when reset without power cycle was detected
 */
bool puf_sram_softreset(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* PUF_SRAM_H */
