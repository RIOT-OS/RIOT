/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       BLE Eddystone beacon example using Skald
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"

#include "net/skald/eddystone.h"

/* example of an Eddystone URI:
 * - namespace (ASCII): 'supercool!'
 * - instance  (ASCII): `_RIOT_` */
#define URI_NAMESPACE   { 0x73, 0x75, 0x70, 0x65, 0x72, \
                          0x63, 0x6f, 0x6f, 0x6c, 0x21 }
#define URI_INSTANCE    { 0x5f, 0x52, 0x49, 0x4f, 0x54, 0x5f }

/* advertise this short URL, points to https://www.riot-os.org */
#define URL             "bit.ly/2Ep11dm"
/* calibrated TX power value */
#define TX_PWR          (0U)
#define ADV_ITVL_MS     1000U

/* allocate two advertising contexts, one for Eddystone-URL and one for
 * Eddystone-URI */
static skald_ctx_t _ctx_uid;
static skald_ctx_t _ctx_url;

int main(void)
{
    LOG_INFO("Skald and the tail of Eddystone\n");

    /* advertise the defined URI */
    skald_eddystone_uid_t uid = { URI_NAMESPACE, URI_INSTANCE };
    skald_eddystone_uid_adv(&_ctx_uid, &uid, TX_PWR, ADV_ITVL_MS);

    /* also advertise the defined short-URL */
    skald_eddystone_url_adv(&_ctx_url, EDDYSTONE_URL_HTTPS, URL, TX_PWR,
                            ADV_ITVL_MS);

    return 0;
}
