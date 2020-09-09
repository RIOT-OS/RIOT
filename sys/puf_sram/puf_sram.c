/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_puf_sram
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#include "puf_sram.h"

/* Allocation of the PUF seed variable */
PUF_SRAM_ATTRIBUTES uint32_t puf_sram_seed;

/* Allocation of the PUF seed state */
PUF_SRAM_ATTRIBUTES uint32_t puf_sram_state;

/* Allocation of the PUF soft reset conter*/
PUF_SRAM_ATTRIBUTES uint32_t puf_sram_softreset_cnt;

/* Allocation of the memory marker */
PUF_SRAM_ATTRIBUTES uint32_t puf_sram_marker;

void puf_sram_init(const uint8_t *ram, size_t len)
{
    /* generates a new seed value if power cycle was detected */
    if (!puf_sram_softreset()) {
        puf_sram_generate(ram, len);
    }
}
void puf_sram_generate(const uint8_t *ram, size_t len)
{
    /* build hash from start-up pattern */
    puf_sram_seed = dek_hash(ram, len);
    /* write marker to a defined section for subsequent reset detection */
    puf_sram_marker = PUF_SRAM_MARKER;
    /* setting state to 0 means seed was generated from SRAM pattern */
    puf_sram_state = 0;
    /* reset counter of detected soft resets */
    puf_sram_softreset_cnt = 0;
}

bool puf_sram_softreset(void)
{
    if (puf_sram_marker != PUF_SRAM_MARKER) {
        puf_sram_state = 2;
        return 0;
    }
    puf_sram_state = 1;

    /* increment number of detected soft resets */
    puf_sram_softreset_cnt++;

    /* generate alterntive seed value */
    puf_sram_seed ^= puf_sram_softreset_cnt;
    puf_sram_seed = dek_hash((uint8_t *)&puf_sram_seed, sizeof(puf_sram_seed));
    return 1;
}
