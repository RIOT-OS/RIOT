/*
 * Copyright (C) 2019-2021 Freie Universität Berlin
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

#include <stdio.h>
#include <assert.h>

#include "net/bluetil/ad.h"
#include "nimble_addr.h"
#include "nimble_scanlist.h"
#include "nimble/hci_common.h"

static const char *_phys[] = { "N/A", "1M", "2M", "CODED" };

static void _print_type(uint8_t type)
{
#if MYNEWT_VAL_BLE_EXT_ADV
    if (type & NIMBLE_SCANNER_EXT_ADV) {
        printf(" [EXT");
        if (type & BLE_HCI_ADV_CONN_MASK) {
            printf("-CONN");
        }
        if (type & BLE_HCI_ADV_SCAN_MASK) {
            printf("-SCAN");
        }
        if (type & BLE_HCI_ADV_DIRECT_MASK) {
            printf("-DIR");
        }
        if (type & BLE_HCI_ADV_SCAN_RSP_MASK) {
            printf("-SCANRSP");
        }
        printf("]");
        return;
    }
#endif

    switch (type) {
        case BLE_HCI_ADV_RPT_EVTYPE_ADV_IND:
            printf(" [IND]");
            break;
        case BLE_HCI_ADV_RPT_EVTYPE_DIR_IND:
            printf(" [DIRECT_IND]");
            break;
        case BLE_HCI_ADV_RPT_EVTYPE_SCAN_IND:
            printf(" [SCAN_IND]");
            break;
        case BLE_HCI_ADV_RPT_EVTYPE_NONCONN_IND:
            printf(" [NONCONN_IND]");
            break;
        case BLE_HCI_ADV_RPT_EVTYPE_SCAN_RSP:
            printf(" [SCAN_RSP]");
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

    nimble_addr_print(&e->addr);
    _print_type(e->type);
    printf(" phy:%s-%s", _phys[e->phy_pri], _phys[e->phy_sec]);
    unsigned adv_int = ((e->last_update - e->first_update) / e->adv_msg_cnt);
    printf(" \"%s\", adv_msg_cnt: %u, adv_int: %uus, last_rssi: %i\n",
           name, (unsigned)e->adv_msg_cnt, adv_int, (int)e->last_rssi);
}
