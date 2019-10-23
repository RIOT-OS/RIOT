/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_puf_sram SRAM PUF
 * @ingroup     sys
 *
 * @brief       SRAM based physically unclonable function (PUF)
 * @experimental
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

#include "hashes.h"
#include "thread.h"

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
