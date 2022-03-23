/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       LoRa 2.4 GHz test range.
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"

#include "xtimer.h"
#include "random.h"

#include "periph/pm.h"

#include "ral_sx1280.h"

#include "lorawan_crypto.h"

#include "sx1280_cmd.h"

// FOR FUTURE LoRaWAN "simple" ABP context
#define FRAME_MAX_SIZE                          (256U)

#define MHDR_DATAUP_CNF                         (0x80)
#define FCTRL_NONE                              (0x00)

#ifndef FPORT
#define FPORT                                   (10U)
#endif
#define FCNT                                    (0U)
#define HEADER_SIZE                             (9U)
#define MIC_SIZE                                sizeof(uint32_t)


#ifndef RECEIVE_WINDOWS_IN_SECONDS
#define RECEIVE_WINDOWS_IN_SECONDS              (5U)
#endif

#ifndef RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS
#define RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS      (20U)
#endif

#ifndef DEVADDR
#define DEVADDR                                 (0xfc00ac10)
#endif

static uint8_t APPSKEY[] = {
#ifndef APPSKEY
    0x01, 0x02, 0x03, 0x04,
    0xca, 0xfe, 0xba, 0xbe,
    0x01, 0x02, 0x03, 0x04,
    0xca, 0xfe, 0xba, 0xbe
#else
    APPSKEY
#endif
};

static uint8_t NWKSKEY[] = {
#ifndef NWKSKEY
    0xca, 0xfe, 0xba, 0xbe,
    0x01, 0x02, 0x03, 0x04,
    0xca, 0xfe, 0xba, 0xbe,
    0x01, 0x02, 0x03, 0x04
#else
    APPSKEY
#endif
};

static uint32_t fcnt = FCNT;

static uint8_t frame[FRAME_MAX_SIZE];

static uint8_t frm_payload[FRAME_MAX_SIZE];

static void printf_ba(const uint8_t *ba, size_t len)
{
    for (unsigned int i = 0; i < len; i++) {
        printf("%02x", ba[i]);
    }
}

static void reboot_after_delay(uint32_t delay)
{
    puts("rebooting ...");
    xtimer_sleep(delay);
    pm_reboot();
}

/*
 * @brief Set the central frequencies of the LoRa channel
 */
int sx1280_channel_set(uint32_t freq_in_hz)
{

    if ((ISM2400_LOWER_FREQUENCY + (getBW(params.bw) / 2) >= freq_in_hz)
        && (freq_in_hz <= ISM2400_HIGHER_FREQUENCY - (getBW(params.bw) / 2))) {
#ifdef CPU_ESP32
        printf("[Error] setup: out-of-range frequency value given for bw=%d\n",
               getBW(params.bw));
#else
        printf("[Error] setup: out-of-range frequency value given for bw=%ld\n",
               getBW(params.bw));
#endif

        return -1;
    }
    params.freq_in_hz = freq_in_hz;

#ifdef CPU_ESP32
    printf("New channel set to %u Hz\n", params.freq_in_hz);
#else
    printf("New channel set to %lu Hz\n", params.freq_in_hz);
#endif
    return 0;
}


/*
 * @brief Set the IQ of the next LoRa communications
 */
void sx1280_invertiq_set(bool invert_iq_is_on)
{
    params.invert_iq_is_on = invert_iq_is_on;
}


// TODO add FOpt
static void build_lorawan_frame(uint8_t *frame, uint8_t frame_len, uint8_t mhdr,
                                uint32_t devaddr, uint32_t fcnt, uint8_t fctrl, uint8_t fport,
                                uint8_t *frm_payload, uint8_t frm_payload_len, uint8_t *nwkskey,
                                uint8_t *appskey)
{

    memset(frame, 0, frame_len);
    frame[0] = mhdr;
    memcpy(frame + 1, (void *)&devaddr, sizeof(devaddr));
    frame[5] = fctrl;
    frame[6] = fcnt & 0xff;
    frame[7] = (fcnt & 0xff00) >> 8;
    // No Fopt
    frame[8] = fport;
    memcpy(frame + HEADER_SIZE, frm_payload, frm_payload_len);

#ifdef 0
    if (appskey != NULL) {
        // TODO encrypt payload + 9 with appskey
    }

    if (nwkskey != NULL) {
        // add MIC with nwkskey
        uint32_t mic;
        const size_t mic_idx = frame_len - MIC_SIZE;
        // TODO lorawan_cmac_calculate_mic(frame, mic_idx, nwkskey, devaddr, 0, fcnt, &mic);

        printf("mic_idx=%d, ", mic_idx);
        printf("mic=");
        printf_ba((uint8_t *)(&mic), MIC_SIZE);
        printf("\n");

        memcpy((uint8_t *)(frame + mic_idx), &mic, MIC_SIZE);
    }
#endif
}


/*
 * @brief Range test by sending periodically LoRaWAN-like packets
 */
int sx1280_rangetest_cmd(int argc, char **argv)
{
    if (argc != 7) {
        puts(
            "usage: rangetest <number of packet> <packet size> <delay in seconds> <channel1_in_khz> <channel2_in_khz> <channel3_in_khz>");
        return -1;
    }

    uint32_t count = atoi(argv[1]);
    uint32_t payload_len = atoi(argv[2]);
    uint32_t delay = atoi(argv[3]);

    // TODO add assert on payload_len > HEADER_SIZE+MIC_SIZE

    uint32_t devaddr = DEVADDR;

    printf("starting range test\n");

#ifdef CPU_ESP32
    printf("devaddr : 0x%x\n", devaddr);
#else
    printf("devaddr : 0x%lx\n", devaddr);
#endif


    printf("appskey : ");
    printf_ba(APPSKEY, ARRAY_SIZE(APPSKEY));
    printf("\n");

    printf("nwkskey : ");
    printf_ba(NWKSKEY, ARRAY_SIZE(NWKSKEY));
    printf("\n");


    for (uint32_t i = 0; i < count; i++) {

        // set a new channel
        uint32_t channel = ((uint32_t)atoi(argv[4 + i % 3])) * 1000U;
        sx1280_channel_set(channel);

        // prepare the LoRaWAN frame payload
        uint8_t frm_payload_len = payload_len - (HEADER_SIZE + MIC_SIZE);
        for (uint8_t j = 0; j < frm_payload_len; j++) {
            frm_payload[j] = (random_uint32() % ('Z' - 'A' + 1)) + 'A';
        }

        printf("frm_payload=");
        printf_ba(frm_payload, frm_payload_len);
        printf("\n");

        // build the LoRaWAN frame
        build_lorawan_frame(frame, payload_len, MHDR_DATAUP_CNF, devaddr, fcnt,
                            FCTRL_NONE, FPORT, frm_payload, frm_payload_len,
                            NWKSKEY, APPSKEY);

        // Set the IQ to normal for uplink
        sx1280_invertiq_set(false);

        // prepare the LoRaWAN frame
#ifdef CPU_ESP32
        printf("sending : channel=%u iq=%d fcnt=%u frame=", channel, params.invert_iq_is_on,
               fcnt);
#else
        printf("sending : channel=%lu iq=%d fcnt=%lu frame=", channel, params.invert_iq_is_on,
               fcnt);
#endif

        printf_ba(frame, payload_len);
        printf("\n");

        params.pld_len_in_bytes = payload_len;
        ral_status_t res = ral_sx1280_setup_tx_lora(&ral_default_cfg, &params);
        if (res != RAL_STATUS_OK) {
            printf("ral_sx1280_setup_tx_lora ERROR %d\n", res);
            reboot_after_delay(10);
            return -1;
        }
        else {
            ral_sx1280_set_pkt_payload(&ral_default_cfg, (uint8_t *)frame,
                                       payload_len);
            res = ral_sx1280_set_tx(&ral_default_cfg);
            if (res != RAL_STATUS_OK) {
                printf("ral_sx1280_set_tx ERROR %d\n", res);
                reboot_after_delay(10);
                return -1;
            }

            xtimer_sleep(RECEIVE_WINDOWS_IN_SECONDS);

            // Set the IQ to inverted for downlink
            sx1280_invertiq_set(true);

            // listen downlink on receive windows
#ifdef CPU_ESP32
            printf("listen : channel=%u iq=%d duration=%d seconds ...\n", channel,
                   params.invert_iq_is_on, RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS);
#else
            printf("listen : channel=%lu iq=%d duration=%d seconds ...\n", channel,
                   params.invert_iq_is_on, RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS);
#endif


            params.pld_len_in_bytes = 255;
            ral_status_t res = ral_sx1280_setup_rx_lora(&ral_default_cfg, &params);
            if (res != RAL_STATUS_OK) {
                printf("ral_sx1280_setup_rx_lora ERROR %d\n", res);
            }
            else {
                res =
                    ral_sx1280_set_rx(&ral_default_cfg, RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS * 1000);
                if (res != RAL_STATUS_OK) {
                    printf("ral_sx1280_set_rx ERROR %d\n", res);
                }
            }

            xtimer_sleep(RECEIVE_WINDOWS_TIMEOUT_IN_SECONDS);

            // sleep before next frame sending
            if (i < count) {
#ifdef CPU_ESP32
                printf("sleep %d seconds before next frame\n", delay);
#else
                printf("sleep %ld seconds before next frame\n", delay);
#endif

                xtimer_sleep(delay);
            }

            fcnt++;

        }
    }
    return 0;
}
