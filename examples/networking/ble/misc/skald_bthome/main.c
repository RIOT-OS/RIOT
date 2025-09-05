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

#include <errno.h>
#include <stdio.h>

#include "saul_reg.h"
#include "ztimer.h"

#include "net/skald/bthome.h"

#ifndef CONFIG_BTHOME_SAUL_REG_DEVS
#  define CONFIG_BTHOME_SAUL_REG_DEVS   (16U)
#endif

#ifndef BTHOME_ADV_INTERVAL
#  define BTHOME_ADV_INTERVAL           (60000U)
#endif

static skald_bthome_ctx_t _ctx;
static skald_bthome_saul_t _saul_devs[CONFIG_BTHOME_SAUL_REG_DEVS];

static int _add_text(skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{
    static const char info[] = "RIOT";

    (void)data;
    (void)idx;
    return skald_bthome_add_measurement(ctx, obj_id, info, strlen(info));
}

int main(void)
{
    saul_reg_t *dev = saul_reg;
    unsigned i = 0;
    int res;

    ztimer_sleep(ZTIMER_MSEC, 2000);
    printf("Skald and the tale of Harald's home\n");

    _ctx.skald.update_pkt = NULL;
    _ctx.devs = NULL;
    _ctx.skald.pkt.len = 0;
    if (skald_bthome_init(&_ctx, NULL, BTHOME_NAME, 0) < 0) {
        return 1;
    }
    if (!saul_reg) {
        puts("Hark! The board does not know SAUL. :-(");
        return 1;
    }
    while (dev && (i < CONFIG_BTHOME_SAUL_REG_DEVS)) {
        _saul_devs[i].saul = *dev;  /* copy registry entry */
        _saul_devs[i].saul.next = NULL;
        printf("Adding %s (%s) to BTHome.\n", dev->name, saul_class_to_str(dev->driver->type));
        if ((res = skald_bthome_saul_add(&_ctx, &_saul_devs[i])) < 0) {
            errno = -res;
            perror("Unable to add sensor to BTHome");
            dev = dev->next;
            continue;
        };
        i++;
        dev = dev->next;
    }
    assert(!saul_reg || _ctx.devs);
    if (i < CONFIG_BTHOME_SAUL_REG_DEVS) {
        memset(&_saul_devs[i].saul, 0, sizeof(_saul_devs[i].saul));
        _saul_devs[i].obj_id = BTHOME_ID_TEXT;
        _saul_devs[i].flags = SKALD_BTHOME_SAUL_FLAGS_CUSTOM;
        _saul_devs[i].add_measurement = _add_text;
        if ((res = skald_bthome_saul_add(&_ctx, &_saul_devs[i])) < 0) {
            errno = -res;
            perror("Unable to add text info to BTHome");
        };
        i++;
    }
    skald_bthome_advertise(&_ctx, BTHOME_ADV_INTERVAL);
    return 0;
}
