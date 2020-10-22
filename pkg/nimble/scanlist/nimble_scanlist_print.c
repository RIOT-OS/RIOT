/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nimble_scanlist
 * @{
 *
 * @file
 * @brief       Print functions for printing a scanlist or selected entries to
 *              STDIO
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "net/bluetil/ad.h"
#include "nimble_scanlist.h"
#include "nimble/hci_common.h"

static void _print_addr(const ble_addr_t *addr)
{
    printf("%02x", (int)addr->val[5]);
    for (int i = 4; i >= 0; i--) {
        printf(":%02x", addr->val[i]);
    }
    switch (addr->type) {
        case BLE_ADDR_PUBLIC:       printf(" (PUBLIC)");   break;
        case BLE_ADDR_RANDOM:       printf(" (RANDOM)");   break;
        case BLE_ADDR_PUBLIC_ID:    printf(" (PUB_ID)");   break;
        case BLE_ADDR_RANDOM_ID:    printf(" (RAND_ID)");  break;
        default:                    printf(" (UNKNOWN)");  break;
    }
}

static void _print_type(uint8_t type)
{
    switch (type) {
        case BLE_HCI_ADV_TYPE_ADV_IND:
            printf(" [IND]");
            break;
        case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_HD:
            printf(" [DIRECT_IND_HD]");
            break;
        case BLE_HCI_ADV_TYPE_ADV_SCAN_IND:
            printf(" [SCAN_IND]");
            break;
        case BLE_HCI_ADV_TYPE_ADV_NONCONN_IND:
            printf(" [NONCONN_IND]");
            break;
        case BLE_HCI_ADV_TYPE_ADV_DIRECT_IND_LD:
            printf(" [DIRECT_IND_LD]");
            break;
        default:
            printf(" [INVALID]");
            break;
    }
}

void nimble_scanlist_print(void)
{
    unsigned i = 0;

    nimble_scanlist_entry_t *e = nimble_scanlist_get_next(NULL);
    while (e) {
        printf("[%2u] ", i++);
        nimble_scanlist_print_entry(e);
        e = nimble_scanlist_get_next(e);
    }
}

void nimble_scanlist_print_entry(nimble_scanlist_entry_t *e)
{
    assert(e);

    /* try to find a device name */
    char name[(BLE_ADV_PDU_LEN + 1)] = { 0 };
    bluetil_ad_t ad = BLUETIL_AD_INIT(e->ad, e->ad_len, e->ad_len);
    int res = bluetil_ad_find_str(&ad, BLE_GAP_AD_NAME, name, sizeof(name));
    if (res != BLUETIL_AD_OK) {
        res = bluetil_ad_find_str(&ad, BLE_GAP_AD_NAME_SHORT, name, sizeof(name));
    }
    if (res != BLUETIL_AD_OK) {
        strncpy(name, "undefined", sizeof(name));
    }

    _print_addr(&e->addr);
    _print_type(e->type);
    unsigned adv_int = ((e->last_update - e->first_update) / e->adv_msg_cnt);
    printf(" \"%s\", adv_msg_cnt: %u, adv_int: %uus, last_rssi: %i\n",
           name, (unsigned)e->adv_msg_cnt, adv_int, (int)e->last_rssi);
}
