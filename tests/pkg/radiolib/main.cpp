/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Board independent RadioLib PingPong test
 *
 * The radio is brought up by the package auto-init and used through the
 * global @ref radiolib_radio, so this application names no chip and no pins.
 *
 * Build as Ping sender (initiator): `make PING=1`
 * Build as Pong sender (responder): `make`
 * Enable debug output:      `make CFLAGS+=-DENABLE_DEBUG=1`
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include <stdio.h>
#include "ztimer.h"
#include "radiolib_riotos.h"
#include <RadioLib.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#define RECV_TIMEOUT_MS 5000

namespace {

bool send(const char *msg)
{
    DEBUG("[radiolib] Sending: %s....", msg);
    int state = radiolib_radio->transmit(msg);
    if (state == RADIOLIB_ERR_NONE) {
        DEBUG("done\n");
        return true;
    }
    else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        puts("packet too long!");
    }
    else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
        puts("TX timeout!");
    }
    else {
        printf("failed, code %d\n", state);
    }
    return false;
}

bool recv(uint32_t timeout_ms)
{
    uint8_t buf[256];

    int state = radiolib_radio->receive(buf, sizeof(buf) - 1, timeout_ms);
    if (state == RADIOLIB_ERR_NONE) {
        size_t len = radiolib_radio->getPacketLength();
        if (len > sizeof(buf) - 1) {
            len = sizeof(buf) - 1;
        }
        buf[len] = '\0';
        printf("[radiolib] Received: %s\n", (char *)buf);
        DEBUG("[radiolib] RSSI:      %.2f dBm\n", radiolib_radio->getRSSI());
        DEBUG("[radiolib] SNR:       %.2f dB\n", radiolib_radio->getSNR());
        return true;
    }
    else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
        DEBUG("[radiolib] RX timeout!\n");
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
        puts("[radiolib] CRC error!");
    }
    else if (state == RADIOLIB_ERR_LORA_HEADER_DAMAGED) {
        DEBUG("[radiolib] header damaged (noise/weak signal)\n");
    }
    else {
        printf("[radiolib] RX failed, code %d\n", state);
    }
    return false;
}

} /* namespace */

int main(void)
{
    if (radiolib_radio == NULL) {
        puts("[radiolib] radio auto-init failed");
        return -1;
    }
    puts("[radiolib] radio ready");

    while (true) {
#if defined(PING)
        if (!send("Ping")) {
            continue;
        }
        recv(RECV_TIMEOUT_MS);
        ztimer_sleep(ZTIMER_MSEC, 1000);
#else
        if (!recv(RECV_TIMEOUT_MS)) {
            continue;
        }
        send("Pong");
#endif
    }
}
