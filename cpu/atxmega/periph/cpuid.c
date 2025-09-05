/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "periph_cpu.h"
#include "cpu_nvm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void cpuid_get(void *id)
{
    uint8_t *addr = id;

    addr[0x0] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM0));
    addr[0x1] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM1));
    addr[0x2] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM2));
    addr[0x3] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM3));
    addr[0x4] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM4));
    addr[0x5] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(LOTNUM5));

    addr[0x6] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(WAFNUM));

    addr[0x7] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(COORDX0));
    addr[0x8] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(COORDX1));
    addr[0x9] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(COORDY0));
    addr[0xa] = nvm_read_production_signature_row(
        nvm_get_production_signature_row_offset(COORDY1));

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        DEBUG("CPUID: ");
        for (uint8_t i = 0; i < CPUID_LEN; i++) {
            DEBUG("%02x ", addr[i]);
        }
        DEBUG("\n");
    }
}
