/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * Based on bplib's example implementation in [bplib]/app/src/bpcat_cla.c
 */
#include "bplib_cla_udp.h"
#include "bplib_init.h"

#include <inttypes.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static void* cla_udp_out(bplib_cla_udp_t * cla)
{
    BPLib_Status_t egress_status;
    int rc;
    uint8_t buffer[BPLIB_CLA_UDP_BUFLEN] = {0};
    size_t out_size;

    while (cla->running && bplib_instance_data.running) {
        egress_status = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, cla->contact_id,
            buffer, &out_size, BPLIB_CLA_UDP_BUFLEN, BPLIB_CLA_UDP_TIMEOUT);
        if (egress_status == BPLIB_SUCCESS) {
            rc = sock_udp_send(&cla->sock, buffer, out_size, NULL);
            if (rc < 0 && rc != -EHOSTUNREACH) {
                DEBUG("sock_udp_send() failed: %i\n", rc);
                return NULL;
            }
        }
        else if (egress_status != BPLIB_CLA_TIMEOUT) {
            DEBUG("Error egressing, RC=%"PRId32"\n", egress_status);
        }
    }
    return NULL;
}

static void* cla_udp_in(bplib_cla_udp_t * cla)
{
    uint8_t buffer[BPLIB_CLA_UDP_BUFLEN] = {0};
    BPLib_Status_t bp_status;

    while (cla->running && bplib_instance_data.running) {

        int rv = sock_udp_recv(&cla->sock, buffer, BPLIB_CLA_UDP_BUFLEN, BPLIB_CLA_UDP_TIMEOUT, NULL);

        if (rv > 0) {
            bp_status = BPLib_CLA_Ingress(&bplib_instance_data.BPLibInst, cla->contact_id, buffer, rv, 0);
            if (bp_status != BPLIB_SUCCESS) {
                DEBUG("BPLib_CLA_Ingress Fail RC=%"PRId32"\n", bp_status);
            }
        }
        else if (rv < 0 && rv != -ETIMEDOUT) {
            DEBUG("sock_udp_recv() failed");
            return NULL;
        }
    }

    return NULL;
}

int bplib_cla_udp_start(bplib_cla_udp_t* cla, uint32_t contact_id) {
    if (cla == NULL || cla->running == 1) {
        return -EINVAL;
    }

    int rc;
    sock_udp_ep_t local_ep;
    sock_udp_ep_t remote_ep;

    if (ipv6_addr_from_str((ipv6_addr_t*) &remote_ep.addr.ipv6,
        bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr) == NULL)
    {
        DEBUG("Failed to convert IP address [%s]\n",
            bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr);
        return -EINVAL;
    }
    remote_ep.family = AF_INET6;
    remote_ep.port = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutPort;

    if (ipv6_addr_from_str((ipv6_addr_t*) &local_ep.addr.ipv6,
        bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInAddr) == NULL)
    {
        DEBUG("Failed to convert IP address [%s]\n",
            bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInAddr);
        return -EINVAL;
    }
    local_ep.family = AF_INET6;
    local_ep.port = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInPort;

    cla->contact_id = contact_id;

    rc = sock_udp_create(&cla->sock, &local_ep, &remote_ep, 0);
    if (rc < 0) {
        DEBUG("Failed to open socket: %i, %s\n", rc, strerror(rc));
        return rc;
    }

    /* This should be true before because it is the threads exit condition */
    cla->running = 1;

    rc = thread_create(cla->stack_rx, BPLIB_CLA_UDP_RX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 2, 0, (void* (*)(void*)) cla_udp_out,
        cla, "bplib-cla-udp-rx");
    if (rc < 0) {
        cla->running = 0;
        return rc;
    }

    rc = thread_create(cla->stack_tx, BPLIB_CLA_UDP_TX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 2, 0, (void* (*)(void*)) cla_udp_in,
        cla, "bplib-cla-udp-tx");
    if (rc < 0) {
        cla->running = 0;
        return rc;
    }

    return 0;
}

void bplib_cla_udp_stop(bplib_cla_udp_t* cla) {
    if (cla == NULL || cla->running == 0) {
        return;
    }

    cla->running = 0;

    sock_udp_close(&cla->sock);
}
