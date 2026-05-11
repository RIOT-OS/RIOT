/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the radiolib package
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 *
 * @}
 */

#include "board.h"
#include <stdio.h>
#include "radiolib_riotos.h"
#include <RadioLib.h>

/* Build with -DRADIOLIB_TEST_TX to transmit, otherwise receive */

static RiotHal hal(SPI_DEV(1), SPI_CLK_5MHZ);

/**
 * SX1276 has the following connections:
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

int main(void)
{
    printf("[SX1276] Initializing ... ");
    int state = radio.begin();
    if (state != RADIOLIB_ERR_NONE) {
        printf("failed, code %d\n", state);
        return -1;
    }
    puts("success!");

#ifdef RADIOLIB_TEST_TX
    int count = 0;
    while (1) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Hello RIOT! #%d", count++);

        printf("[SX1276] Transmitting: %s ... ", buf);
        state = radio.transmit((uint8_t *)buf, strlen(buf));

        if (state == RADIOLIB_ERR_NONE) {
            printf("done, datarate: %.2f bps\n", radio.getDataRate());
        } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
            puts("packet too long!");
        } else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
            puts("TX timeout!");
        } else {
            printf("failed, code %d\n", state);
        }

        hal.delay(1000);
    }

#else /* receive */
    while (true) {
        printf("[SX1276] Waiting for packet ... ");

        uint8_t buf[256];
        size_t len = sizeof(buf);
        state = radio.receive(buf, len);

        if (state == RADIOLIB_ERR_NONE) {
            size_t rxlen = radio.getPacketLength();
            if (rxlen >= len){
                rxlen = len - 1 ;
            }
            buf[rxlen] = '\0';
            puts("success!");
            printf("[SX1276] Data:  %s\n", (char *)buf);
            printf("[SX1276] RSSI:  %.2f dBm\n", radio.getRSSI());
            printf("[SX1276] SNR:   %.2f dB\n", radio.getSNR());
            printf("[SX1276] Freq error: %.2f Hz\n", radio.getFrequencyError());
        } else if (state == RADIOLIB_ERR_RX_TIMEOUT) {
            puts("timeout");
        } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
            puts("CRC error!");
        } else {
            printf("failed, code %d\n", state);
        }
    }
#endif

    return 0;
}
