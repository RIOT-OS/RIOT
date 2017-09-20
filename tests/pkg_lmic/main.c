/*
 * Copyright (c) 2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for LMIC LoRaMAC package
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "periph/gpio.h"

#include "lmic.h"
#include "lmic/hal.h"
#include "lmic/params.h"
#include "hal.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

const unsigned TX_INTERVAL = 1;

static uint8_t mydata[] = "Hello, world!";
static osjob_t sendjob;

/* Radio device configuration */
const lmic_pinmap lmic_pins = {
    .nss = LMIC_PARAM_PIN_NSS,
    .rxtx = LMIC_PARAM_PIN_RXTX,
    .rst = LMIC_PARAM_PIN_RESET,
    .dio = {
        LMIC_PARAM_PIN_DIO0,
        LMIC_PARAM_PIN_DIO0,
        LMIC_PARAM_PIN_DIO0
    },
};


/* LoRaWAN required parameters */

/* unique device ID (LSBF) */
static u1_t DEVEUI[8]  = LMIC_PARAM_DEVEUI;

/* application router ID (LSBF) */
static u1_t APPEUI[8]  = LMIC_PARAM_APPEUI;

/* application AES key (derived from device EUI) */
static u1_t APPKEY[16] = LMIC_PARAM_APPKEY;

static u4_t DEVADDR = LMIC_PARAM_DEVADDR;

/* LoRaWAN NwkSKey, network session key */
static u1_t NWKSKEY[16] = LMIC_PARAM_NWKSKEY;

/* LoRaWAN AppSKey, application session key */
static u1_t APPSKEY[16] = LMIC_PARAM_APPSKEY;

/* utility functions */
void do_send(osjob_t* j) {
    DEBUG("[LMIC] test: do send\n");
    /* Check if there is not a current TX/RX job running */
    if (LMIC.opmode & OP_TXRXPEND) {
        puts("[LMIC] test: OP_TXRXPEND, not sending");
    }
    else {
        /* Prepare upstream data transmission at the next possible time. */
        LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
        puts("[LMIC] test: Packet queued");
    }
    /* Next TX is scheduled after TX_COMPLETE event. */
}

/* provide application router ID (8 bytes, LSBF) */
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

/* provide device ID (8 bytes, LSBF) */
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

/* provide device key (16 bytes) */
void os_getDevKey (u1_t* buf) {
    memcpy(buf, APPKEY, 16);
}


int main (void) {
    /* LMIC init */
    os_init();

    /* Reset the MAC state.
       Session and pending data transfers will be discarded. */
    LMIC_reset();

#ifdef LMIC_JOIN_ABP
    (void) APPKEY;
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);

    LMIC_setDrTxpow(DR_SF12,2);
    LMIC_setLinkCheckMode(0);
    // printf("%i\n", LMIC.dn2Dr);
#else
    (void) DEVADDR;
    (void) NWKSKEY;
    (void) APPSKEY;
#endif
    DEBUG("[LMIC] test: initialized\n");

    /* Start job */
    do_send(&sendjob);
    os_runloop();
    return 0;
}


/* LMIC EVENT CALLBACK */
void onEvent (ev_t ev) {
    printf("[LMIC] test: ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            puts("EV_SCAN_TIMEOUT");
            break;

        case EV_BEACON_FOUND:
            puts("EV_BEACON_FOUND");
            break;

        case EV_BEACON_MISSED:
            puts("EV_BEACON_MISSED");
            break;

        case EV_BEACON_TRACKED:
            puts("EV_BEACON_TRACKED");
            break;

        case EV_JOINING:
            puts("EV_JOINING");
            break;

        case EV_JOINED:
            puts("EV_JOINED");

            /* Disable link check validation. */
            LMIC_setLinkCheckMode(0);
            break;

        case EV_RFU1:
            puts("EV_RFU1");
            break;

        case EV_JOIN_FAILED:
            puts("EV_JOIN_FAILED");
            break;

        case EV_REJOIN_FAILED:
            puts("EV_REJOIN_FAILED");
            break;

        case EV_LOST_TSYNC:
            puts("EV_LOST_TSYNC");
            break;

        case EV_RESET:
            puts("EV_RESET");
            break;

        case EV_RXCOMPLETE:
            /* data received in ping slot */
            puts("EV_RXCOMPLETE");
            break;

        case EV_LINK_DEAD:
            puts("EV_LINK_DEAD");
            break;

        case EV_LINK_ALIVE:
            puts("EV_LINK_ALIVE");
            break;

        case EV_TXCOMPLETE:
            puts("EV_TXCOMPLETE (includes waiting for RX windows)");
            if (LMIC.txrxFlags & TXRX_ACK)
                puts("Received ack");
            if (LMIC.dataLen) {
                printf("Received ");
                printf("%i ", (int) LMIC.dataLen);
                puts(" bytes of payload");
            }

            /* Schedule next transmission */
            os_setTimedCallback(&sendjob,
                                os_getTime() + sec2osticks(TX_INTERVAL),
                                do_send);
            break;

      default:
          break;
    }
}
