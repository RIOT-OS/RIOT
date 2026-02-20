/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking 
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at 
 *
 * http://www.apache.org/licenses/LICENSE-2.0 
 *
 * Unless required by applicable law or agreed to in writing, software distributed under 
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF 
 * ANY KIND, either express or implied. See the License for the specific language 
 * governing permissions and limitations under the License. The copyright notice to be 
 * included in the software is as follows: 
 *
 * Copyright 2025 United States Government as represented by the Administrator of the 
 * National Aeronautics and Space Administration. All Rights Reserved.
 *
 */
#include "bplib_cla_udp.h"
#include "bplib_init.h"

#include <inttypes.h>

#define CLA_TIMEOUT              10000

static void* cla_udp_out(bplib_cla_udp_t * cla)
{
    BPLib_Status_t EgressStatus;
    int rc;
    uint8_t buffer[BPLIB_CLA_UDP_BUFLEN] = {0};
    size_t OutSize;

    while(cla->running && bplib_instance_data.running)
    {
        EgressStatus = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, cla->contact_id,
            buffer, &OutSize, BPLIB_CLA_UDP_BUFLEN, CLA_TIMEOUT);
        if (EgressStatus == BPLIB_SUCCESS)
        {
            rc = sock_udp_send(&cla->sock, buffer, OutSize, NULL);
            if (rc < 0 && rc != -EHOSTUNREACH)
            {
                printf("sock_udp_send() failed: %i\n", rc);
                return NULL;
            }
        }
        else if (EgressStatus != BPLIB_CLA_TIMEOUT)
        {
            printf("Error egressing, RC=%"PRId32"\n", EgressStatus);
        }
    }
    return NULL;
}

static void* cla_udp_in(bplib_cla_udp_t * cla)
{
    uint8_t buffer[BPLIB_CLA_UDP_BUFLEN] = {0};
    BPLib_Status_t BpStatus;

    while(cla->running && bplib_instance_data.running)
    {

        int rv = sock_udp_recv(&cla->sock, buffer, BPLIB_CLA_UDP_BUFLEN, CLA_TIMEOUT, NULL);

        if (rv > 0)
        {
            BpStatus = BPLib_CLA_Ingress(&bplib_instance_data.BPLibInst, cla->contact_id, buffer, rv, 0);
            if (BpStatus != BPLIB_SUCCESS)
            {
                printf("BPLib_CLA_Ingress Fail RC=%"PRId32"\n", BpStatus);
            }
        }
        else if (rv < 0 && rv != -ETIMEDOUT)
        {
            printf("sock_udp_recv() failed");
            return NULL;
        }
    }

    return NULL;
}


int bplib_cla_udp_start(bplib_cla_udp_t* cla, uint32_t contact_id) {
    if (cla == NULL || cla->running == 1) return -EINVAL;

    int rc;
    sock_udp_ep_t local_ep;
    sock_udp_ep_t remote_ep;

    if (ipv6_addr_from_str((ipv6_addr_t*) &remote_ep.addr.ipv6,
        bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr) == NULL)
    {
        printf("Failed to convert IP address [%s]\n",
            bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr);
        return -EINVAL;
    }
    remote_ep.family = AF_INET6;
    remote_ep.port = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutPort;

    if (ipv6_addr_from_str((ipv6_addr_t*) &local_ep.addr.ipv6,
        bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInAddr) == NULL)
    {
        printf("Failed to convert IP address [%s]\n",
            bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInAddr);
        return -EINVAL;
    }
    local_ep.family = AF_INET6;
    local_ep.port = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInPort;

    cla->contact_id = contact_id;

    rc = sock_udp_create(&cla->sock, &local_ep, &remote_ep, 0);
    if (rc < 0) {
        printf("Failed to open socket: %i, %s\n", rc, strerror(rc));
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
    if (cla == NULL || cla->running == 0) return;

    cla->running = 0;

    sock_udp_close(&cla->sock);
}