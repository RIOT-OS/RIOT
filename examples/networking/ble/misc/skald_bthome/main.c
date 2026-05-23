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

static int _add_battery_binary(skald_bthome_ctx_t *ctx, uint8_t obj_id, phydat_t *data, uint8_t idx)
{

    return skald_bthome_add_uint8_measurement(ctx, obj_id, (data->val[idx] < 3500) ? 0x01 : 0x00);
}

int main(void)
{
    saul_reg_t *dev = saul_reg;
    unsigned i = 0;
    int res;

    ztimer_sleep(ZTIMER_MSEC, 2000);
    printf("Skald and the tale of Harald's home\n");

#ifdef ENCRYPTION_KEY
    int key_len;

    key_len = _get_encryption_key();

    if (key_len != SKALD_BTHOME_KEY_LEN) {
        printf(
            "Key should be of length 16 (32 chars), "
            "was \"%s\" (%u chars)\n",
            ENCRYPTION_KEY,
            strlen(ENCRYPTION_KEY)
        );
        return 1;
    }
#endif
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
        if ((dev->driver->type == SAUL_SENSE_VOLTAGE) && (strcmp(dev->name, "BAT") == 0)) {
            printf("Adding %s (%s) to BTHome.\n", dev->name, saul_class_to_str(dev->driver->type));
            _saul_devs[i].saul = *dev;  /* copy registry entry */
            _saul_devs[i].saul.next = NULL;
            _saul_devs[i].obj_id = BTHOME_ID_BATTERY_BINARY;
            _saul_devs[i].flags = SKALD_BTHOME_SAUL_FLAGS_CUSTOM;
            _saul_devs[i].add_measurement = _add_battery_binary;

            if ((res = skald_bthome_saul_add(&_ctx, &_saul_devs[i])) < 0) {
                printf("Unable to add sensor to BTHome (%d)\n", -res);
            };
            i++;
        }
        if ((dev->driver->type == SAUL_SENSE_BTN)
            || (dev->driver->type == SAUL_SENSE_ACCEL)
            || (dev->driver->type == SAUL_SENSE_GYRO)
            || (dev->driver->type == SAUL_SENSE_TEMP && (strcmp(dev->name, "NRF_TEMP") != 0))
        ) {
            printf("Skipping %s (%s).\n", dev->name, saul_class_to_str(dev->driver->type));
            dev = dev->next;
            continue;
        }
        _saul_devs[i].saul = *dev;  /* copy registry entry */
        _saul_devs[i].saul.next = NULL;
        printf("Adding %s (%s) to BTHome.\n", dev->name, saul_class_to_str(dev->driver->type));
        if ((res = skald_bthome_saul_add(&_ctx, &_saul_devs[i])) < 0) {
            printf("Unable to add sensor to BTHome (%d)\n", -res);
            dev = dev->next;
            continue;
        };
        i++;
        dev = dev->next;
    }
    assert(!saul_reg || _ctx.devs);
    skald_bthome_advertise(&_ctx, BTHOME_ADV_INTERVAL);
    return 0;
}
