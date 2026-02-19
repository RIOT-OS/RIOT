#pragma once

#include <stdint.h>

#include "thread_config.h"
#include "net/sock/udp.h"

#ifndef BPLIB_CLA_UDP_BUFLEN
#define BPLIB_CLA_UDP_BUFLEN 4096
#endif

#define BPLIB_CLA_UDP_RX_STACK_SIZE (THREAD_STACKSIZE_MEDIUM + BPLIB_CLA_UDP_BUFLEN)
#define BPLIB_CLA_UDP_TX_STACK_SIZE (THREAD_STACKSIZE_MEDIUM + BPLIB_CLA_UDP_BUFLEN)

typedef struct {
    volatile int running;
    char stack_tx[BPLIB_CLA_UDP_TX_STACK_SIZE];
    char stack_rx[BPLIB_CLA_UDP_RX_STACK_SIZE];

    uint32_t contact_id;
    
    sock_udp_t sock;
} bplib_cla_udp_t;



int bplib_cla_udp_start(bplib_cla_udp_t* cla, uint32_t contact_id);
void bplib_cla_udp_stop(bplib_cla_udp_t* cla);

