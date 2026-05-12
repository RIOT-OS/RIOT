/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       RadioLib SX1276 PingPong test for b-l072z-lrwan1
 *
 * Build as Ping sender (initiator) : make PING=1
 * Build as Pong sender (responder) : make (default)
 * Enable debug output:      make CFLAGS+=-DENABLE_DEBUG=1
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include "board.h"
#include <stdio.h>
#include "radiolib_riotos.h"
#include <RadioLib.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static RiotHal hal(SPI_DEV(1), SPI_CLK_5MHZ);

/**
 * SX1276 connections on b-l072z-lrwan1:
 * NSS pin:   PA.15
 * DIO0 pin:  PB.4
 * RESET pin: PC.0
 * DIO1 pin:  PB.1
 */
static SX1276 radio = new Module(
    &hal,
    SX127X_PARAM_SPI_NSS,
    SX127X_PARAM_DIO0,
    SX127X_PARAM_RESET,
    SX127X_PARAM_DIO1
);

namespace {

bool send(const char *msg)
{
    DEBUG("[SX1276] Sending: %s....", msg);
    int state = radio.transmit(msg);
    if (state == RADIOLIB_ERR_NONE) {
        DEBUG("done\n");
        return true;
    } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        puts("packet too long!");
    } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
        puts("TX timeout!");
    } else {
        printf("failed, code %d\n", state);
    }
    return false;
}

bool recv(void)
{
    uint8_t buf[256];

    int state = radio.receive(buf, sizeof(buf) - 1);
    if (state == RADIOLIB_ERR_NONE) {
        size_t len = radio.getPacketLength();
        if (len > sizeof(buf) - 1) {
            len = sizeof(buf) - 1;
        }
        buf[len] = '\0';
        printf("[SX1276] Received: %s\n", (char *)buf);
        DEBUG("[SX1276] RSSI:      %.2f dBm\n", radio.getRSSI());
        DEBUG("[SX1276] SNR:       %.2f dB\n", radio.getSNR());
        DEBUG("[SX1276] Freq err:  %.2f Hz\n", radio.getFrequencyError());
        return true;
    } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        DEBUG("[SX1276] RX timeout!\n");
    } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        puts("[SX1276] CRC error!");
    } else {
        printf("[SX1276] RX failed, code %d\n", state);
    }
    return false;
}

} /* namespace */

int main(void)
{
    printf("[SX1276] Initializing ... ");
    int state = radio.begin();
    if (state != RADIOLIB_ERR_NONE) {
        printf("failed, code %d\n", state);
        return -1;
    }
    puts("success!");

    state = radio.setFrequency(868.0);
    if (state != RADIOLIB_ERR_NONE) {
        printf("[SX1276] setFrequency failed, code %d\n", state);
        return -1;
    }

    while (true) {
#if defined(PING)
        if (!send("Ping")) { continue; }
        recv();
        hal.delay(10000);
#else
        if (!recv()) { continue; }
        send("Pong");
#endif
    }
}
