/*
 * SPDX-FileCopyrightText: 2017-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Setting up an iBeacon using Skald
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"

#include "net/skald/ibeacon.h"

/* configure the iBeacon */
#define UUID            { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, \
                          0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, }
#define MAJOR           (0x0023)
#define MINOR           (0x0017)
#define TXPOWER         (0U)
#define ADV_ITVL_MS     1000U

/* allocate a single advertising context */
static skald_ctx_t _ctx;

int main(void)
{
    LOG_INFO("Skald and the tail of the iBeacon\n");

    /* this will configure the iBeacon and start advertising it */
    skald_uuid_t uuid = { UUID };
    skald_ibeacon_advertise(&_ctx, &uuid, MAJOR, MINOR, TXPOWER, ADV_ITVL_MS);

    return 0;
}
