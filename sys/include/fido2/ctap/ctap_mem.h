/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     fido2_ctap
 * @brief       FIDO2 CTAP flash memory helper
 *
 * @{
 *
 * @file
 * @brief       Definition for CTAP2 flash memory helper functions
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_CTAP_MEM_H
#define FIDO2_CTAP_CTAP_MEM_H

#include <stdint.h>

#include "fido2/ctap.h"
#include "periph/flashpage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Write to flash memory and verify the written page against the given data
 *
 * @param[in] page      page to write to
 * @param[in] offset    offset into the page
 * @param[in] data      data to write and compare against
 * @param[in] len       length of data
 *
 * @return CTAP status code
 */
int fido2_ctap_mem_write_and_verify(int page, int offset, const void *data, size_t len);

/**
 * @brief Read page into buf
 *
 * @param[in] page       page to read from
 * @param[in] buf        memory to write the page to
 *
 */
void fido2_ctap_mem_read(int page, void *buf);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_CTAP_MEM_H */
/** @} */
