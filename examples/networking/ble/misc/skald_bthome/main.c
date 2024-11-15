/*
 * SPDX-FileCopyrightText: 2024 Martine S. Lenders
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       BLE BTHome example using Skald
 *
 * @author      Martine S. Lenders <mail@martine-lenders.eu>
 *
 * @}
 */

#include <stdio.h>

#include "ztimer.h"

#include "net/skald/bthome.h"

#ifndef CONFIG_BTHOME_SAUL_REG_DEVS
#  define CONFIG_BTHOME_SAUL_REG_DEVS   (16U)
#endif

#ifndef BTHOME_ADV_INTERVAL
#  define BTHOME_ADV_INTERVAL           (60000U)
#endif

static skald_bthome_ctx_t _ctx;

int main(void)
{
    ztimer_sleep(ZTIMER_MSEC, 2000);
    printf("Skald and the tale of Harald's home\n");

    if (skald_bthome_init(&_ctx, NULL, BTHOME_NAME, 0) < 0) {
        return 1;
    }
    if (skald_bthome_add_uint24_measurement(&_ctx, BTHOME_ID_TEMPERATURE_FACTOR_0_01, 2500) < 0) {
        return 1;
    }
    if (skald_bthome_add_int16_measurement(&_ctx, BTHOME_ID_HUMIDITY_FACTOR_0_01, 5055) < 0) {
        printf("H %u\n", _ctx.skald.pkt.len);
        return 1;
    }
    skald_bthome_advertise(&_ctx, BTHOME_ADV_INTERVAL);
    return 0;
}
