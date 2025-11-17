/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Check if the ZBOSS package builds and the ZDO starts.
 *
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 *
 */

#include <stdio.h>
#include <stdbool.h>

#include "ztimer.h"

#include "zb_common.h"
#include "zb_aps.h"

static bool zboss_start_successful;

void zb_data_indication(zb_uint8_t param)
{
    zb_buf_t *buf = ZB_BUF_FROM_REF(param);
    zb_apsde_data_indication_t *ind = ZB_GET_BUF_PARAM(buf,
                                                    zb_apsde_data_indication_t);

    switch (ind->clusterid) {
    case 0x0019: /* zdo start */
        zboss_start_successful = true;
        /* don't free buf */
        break;

    default:
        zb_free_buf(buf);
    }
}

int main(void)
{
    /* wait for ZDO to start */
    ztimer_sleep(ZTIMER_SEC, 1);;

    if (zboss_start_successful) {
        puts("SUCCESS: ZBOSS started");
    } else {
        puts("FAILED: ZBOSS start not successful");
    }
    return 0;
}
