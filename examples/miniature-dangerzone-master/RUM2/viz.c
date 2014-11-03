/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_demo
 * @{
 *
 * @file        viz.c
 * @brief       CeBIT 2014 demo application - router node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "viz.h"
#include "demo.h"
#include "events.h"


#ifdef VIZ_REMOTE
#include "udpif.h"

#define VIZ_ADDR        23
#define VIZ_PORT        APPLICATION_PORT
#endif

#ifdef VIZ_REMOTE
static uint8_t next_sequ = 0;
#endif

extern radio_address_t id;


void viz_udp_pkt(uint8_t src)
{
#ifdef VIZ_REMOTE
    char evt[3];
    evt[0] = DTA_RCVD;
    evt[1] = (uint8_t)src;
    evt[2] = (char)next_sequ++;
    udpif_send(VIZ_ADDR, VIZ_PORT, evt, 3);
#else
    printf("VIZ: UPD packet from %i\n", src);
    printf("fw %i %i %i\n", src, DTA_RCVD, id);
#endif
}

void viz_udp_snd(uint8_t dst)
{
    if (id == 32 || id == 33) {
        printf("VIZ: UDP send packet to %i\n", dst);
        printf("fw %i %i %i\n", id, DTA_RCVD, dst);
    }
}

void viz_parent_select(uint8_t parent)
{
#ifdef VIZ_REMOTE
    char evt[3];
    evt[0] = PARENT_SELECT;
    evt[1] = parent;
    evt[2] = (char)next_sequ++;
    udpif_send(VIZ_ADDR, VIZ_PORT, evt, 3);
#else
    printf("VIZ: RPL %i selected parent: %i\n", id, parent);
    // printf("fw %i %i %i\n", id, PARENT_SELECT, parent);
#endif
}

void viz_parent_deselect(uint8_t parent)
{
#ifdef VIZ_REMOTE
    char evt[3];
    evt[0] = PARENT_DELETE;
    evt[1] = parent;
    evt[2] = (char)next_sequ++;
    udpif_send(VIZ_ADDR, VIZ_PORT, evt, 3);
#else
    printf("VIZ: RPL %i deleted parent: %i\n", id, parent);
    // printf("fw %i %i %i\n", id, PARENT_DELETE, parent);
#endif
}

void viz_dio(uint8_t src)
{
#ifdef VIZ_REMOTE
    char evt[3];
    evt[0] = DIO_RCVD;
    evt[1] = (uint8_t)src;
    evt[2] = (char)next_sequ++;
    udpif_send(VIZ_ADDR, VIZ_PORT, evt, 3);
#else
    printf("VIZ: RPL dio from %i\n", src);
    printf("fw %i %i %i\n", id, DIO_RCVD, src);
#endif
}
