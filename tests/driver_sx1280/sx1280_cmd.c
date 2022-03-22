/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       shell commands for using the SX1280 as a LoRa 2.4 GHz modem.
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

#include "sx1280_cmd.h"

// TODO add a mutex
static bool _SX1280_WaitingRx = false;

static ral_status_t sx1280_init_status;

#ifdef MULTITECH_ISM2400_PARAMS

ral_params_lora_t params = {
    .freq_in_hz = MULTITECH_ISM2400_CHANNEL_1,
    .sf = MULTITECH_ISM2400_SF,
    .bw = MULTITECH_ISM2400_BANDWITH,
    .cr = MULTITECH_ISM2400_CR,
    .pbl_len_in_symb = MULTITECH_ISM2400_LEN_IN_SYMB,
    .sync_word = MULTITECH_ISM2400_SYNCWORD_PUBLIC,
    .pld_is_fix = false,
    .crc_is_on = true,
    .invert_iq_is_on = false,
    .symb_nb_timeout = 0,               //! Rx only parameters - useless anyway
    .pwr_in_dbm = SX1280_MAX_TXPOWER,   //! Tx only parameters
};
#else


ral_params_lora_t params = { .freq_in_hz = 2483500000,
                             .sf = RAL_LORA_SF9, .bw = RAL_LORA_BW_200_KHZ,
                             .cr = RAL_LORA_CR_LI_4_5,
                             .pbl_len_in_symb = 12,
                                                                                //.sync_word = 0x12,
                             .sync_word = 0x21, .pld_is_fix = false, .crc_is_on = true,
                             .invert_iq_is_on = false, .symb_nb_timeout = 0,    //! Rx only parameters - useless anyway
                             .pwr_in_dbm = 4,                                   //! Tx only parameters
};

#endif


/*
 * @brief Get the frequency in Hz from the driver lora definition
 */
uint32_t getBW(ral_lora_bw_t lora_bw)
{
    switch (lora_bw) {
    case RAL_LORA_BW_007_KHZ:
        return 7000;
    case RAL_LORA_BW_010_KHZ:
        return 10000;
    case RAL_LORA_BW_015_KHZ:
        return 15000;
    case RAL_LORA_BW_020_KHZ:
        return 20000;
    case RAL_LORA_BW_031_KHZ:
        return 31000;
    case RAL_LORA_BW_041_KHZ:
        return 41000;
    case RAL_LORA_BW_062_KHZ:
        return 62000;
    case RAL_LORA_BW_125_KHZ:
        return 125000;
    case RAL_LORA_BW_200_KHZ:
        return 200000;
    case RAL_LORA_BW_250_KHZ:
        return 250000;
    case RAL_LORA_BW_400_KHZ:
        return 400000;
    case RAL_LORA_BW_500_KHZ:
        return 500000;
    case RAL_LORA_BW_800_KHZ:
        return 800000;
    case RAL_LORA_BW_1600_KHZ:
        return 1600000;
    default:
        return 0;
    }
}


/*
 * @brief Get/Set the central frequencies of the LoRa channel
 */
int sx1280_channel_cmd(int argc, char **argv)
{
    if (argc < 2) {
        puts("usage: channel <get|set>");
        return -1;
    }

    if (strstr(argv[1], "get") != NULL) {
#ifdef CPU_ESP32
        printf("Channel: %u\n", params.freq_in_hz);
#else
        printf("Channel: %lu\n", params.freq_in_hz);
#endif

        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: channel set <channel>");
            return -1;
        }
        uint32_t freq_in_hz = strtoull(argv[2], NULL, 10);
        if ((ISM2400_LOWER_FREQUENCY + (getBW(params.bw) / 2) >= freq_in_hz)
            && (freq_in_hz
                <= ISM2400_HIGHER_FREQUENCY - (getBW(params.bw) / 2))) {
#ifdef CPU_ESP32
            printf(
                "[Error] setup: out-of-range frequency value given for bw=%d\n",
                getBW(params.bw));
#else
            printf(
                "[Error] setup: out-of-range frequency value given for bw=%ld\n",
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
    }
    else {
        puts("usage: channel <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the TX power of the next TX LoRa communications
 */
int sx1280_txpower_cmd(int argc, char **argv)
{

    if (argc < 2) {
        puts("usage: txpower <get|set>");
        return -1;
    }
    if (strstr(argv[1], "get") != NULL) {
        printf("txpower: %d\n", params.pwr_in_dbm);
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: txpower set <length>");
            return -1;
        }
        // TODO check power 1 .. 13
        params.pwr_in_dbm = atoi(argv[2]);
        printf("New TX power set to %d\n", params.pwr_in_dbm);
    }
    else {
        puts("usage: txpower <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the sync word of the next LoRa communications
 */
int sx1280_syncword_cmd(int argc, char **argv)
{

    if (argc < 2) {
        puts("usage: syncword <get|set>");
        return -1;
    }
    if (strstr(argv[1], "get") != NULL) {
        printf("syncword : 0x%2x\n", params.sync_word);
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: syncword set <value>");
            return -1;
        }
        uint8_t syncword = atoi(argv[2]);
        params.sync_word = syncword;
        printf("New syncword set to 0x%2x\n", params.sync_word);
    }
    else {
        puts("usage: syncword <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the preamble length of the next LoRa communications
 */
int sx1280_preamble_cmd(int argc, char **argv)
{

    if (argc < 2) {
        puts("usage: preamble <get|set>");
        return -1;
    }
    if (strstr(argv[1], "get") != NULL) {
        printf("Preamble length: %d\n", params.pbl_len_in_symb);
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: preamble set <length>");
            return -1;
        }
        uint8_t preamble = atoi(argv[2]);
        // TODO check value range
//        if (preamble < 6 || preamble > 10) {
//            puts("[Error] setup: invalid preamble value given");
//            return -1;
//        }
        params.pbl_len_in_symb = preamble;
        printf("New preamble length set to %d\n", params.pbl_len_in_symb);
    }
    else {
        puts("usage: preamble <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the payload size of the next LoRa communications
 */
int sx1280_payload_cmd(int argc, char **argv)
{

    if (argc < 2) {
        puts("usage: payload <get|set>");
        return -1;
    }

    if (strstr(argv[1], "get") != NULL) {

        printf("Payload: %s\n", params.pld_is_fix ? "fix" : "var");
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: payload set <fix/var>");
            return -1;
        }
        if (strcmp(argv[2], "fix") == 0) {
            params.pld_is_fix = true;
        }
        else if (strcmp(argv[2], "var") == 0) {
            params.pld_is_fix = false;
        }
        else {
            puts("usage: payload set <fix/var>");
            return -1;
        }
        printf("New Payload: %s\n", params.pld_is_fix ? "fix" : "var");
    }
    else {
        puts("usage: payload <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the IQ of the next LoRa communications
 */
int sx1280_invertiq_cmd(int argc, char **argv)
{

    if (argc < 2) {
        puts("usage: invert_iq <get|set>");
        return -1;
    }

    if (strstr(argv[1], "get") != NULL) {

        printf("InvertIQ: %s\n",
               params.invert_iq_is_on ? "inverted" : "normal");
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: invert_iq set <normal/inverted>");
            return -1;
        }
        if (strcmp(argv[2], "normal") == 0) {
            params.invert_iq_is_on = false;
        }
        else if (strcmp(argv[2], "inverted") == 0) {
            params.invert_iq_is_on = true;
        }
        else {
            puts("usage: invert_iq set <normal/inverted>");
            return -1;
        }
        printf("New InvertIQ: %s\n",
               params.invert_iq_is_on ? "inverted" : "normal");
    }
    else {
        puts("usage: invert_iq <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Get/Set the CRC of the next LoRa communications
 */
int sx1280_crc_cmd(int argc, char **argv)
{

    (void)argc;
    (void)argv;

    if (argc < 2) {
        puts("usage: crc <get|set>");
        return -1;
    }

    if (strstr(argv[1], "get") != NULL) {

        printf("CRC: %s\n", params.crc_is_on ? "on" : "off");
        return 0;
    }

    if (strstr(argv[1], "set") != NULL) {
        if (argc < 3) {
            puts("usage: crc set <on/off>");
            return -1;
        }
        if (strcmp(argv[2], "on") == 0) {
            params.crc_is_on = true;
        }
        else if (strcmp(argv[2], "off") == 0) {
            params.crc_is_on = false;
        }
        else {
            puts("usage: crc set <on/off>");
            return -1;
        }
        printf("New CRC: %s\n", params.crc_is_on ? "on" : "off");
    }
    else {
        puts("usage: crc <get|set>");
        return -1;
    }

    return 0;
}

/*
 * @brief Setup the bandwidth, the spreading factor and the code rate of the next LoRa communications
 */
int sx1280_setup_cmd(int argc, char **argv)
{
    if (argc == 1) {
#ifdef CPU_ESP32
        printf("Setup: sf=%d bw=%dHz cr=%d/8\n", params.sf, getBW(params.bw),
               params.cr);
#else
        printf("Setup: sf=%d bw=%ld Hz cr=%d/8\n", params.sf, getBW(params.bw),
               params.cr);
#endif

        return 0;
    }

    if (argc < 4) {
        puts("usage: setup"
             "<bandwidth (200, 400, 800, 1600)> "
             "<spreading factor (7..12)> "
             "<code rate (5..8)> "
             "[li]");
        return -1;
    }

    /* Check bandwidth value */
    int bw = atoi(argv[1]);
    uint8_t lora_bw;

    switch (bw) {
    case 200:
        puts("setup: setting 200KHz bandwidth");
        lora_bw = RAL_LORA_BW_200_KHZ;
        break;

    case 400:
        puts("setup: setting 400KHz bandwidth");
        lora_bw = RAL_LORA_BW_400_KHZ;
        break;

    case 800:
        puts("setup: setting 800KHz bandwidth");
        lora_bw = RAL_LORA_BW_800_KHZ;
        break;

    case 1600:
        puts("setup: setting 1600KHz bandwidth");
        lora_bw = RAL_LORA_BW_1600_KHZ;
        break;

    default:
        puts("[Error] setup: invalid bandwidth value given, "
             "only 200, 400, 800 or 1600 allowed.");
        return -1;
    }

    /* Check spreading factor value */
    uint8_t lora_sf = atoi(argv[2]);

    if (lora_sf < 7 || lora_sf > 12) {
        puts("[Error] setup: invalid spreading factor value given");
        return -1;
    }

    /* Check coding rate value */
    int cr = atoi(argv[3]);
    bool li = argc > 4 && strcmp(argv[4], "li") == 0;

    if ((!li && (cr < 5 || cr > 8)) || (li && (cr < 5 || cr > 7))) {
        puts("[Error ]setup: invalid coding rate value given");
        return -1;
    }
    // TODO utiliser les defines
    uint8_t lora_cr = (uint8_t)(cr - 5);

    if (li) {
        lora_cr = (uint8_t)(cr - 1);
    }

    params.bw = lora_bw;
    params.sf = lora_sf;
    params.cr = lora_cr;

    puts("[Info] setup: configuration set with success");

    return 0;
}

//static int sx1280_tx_cmd(int argc, char **argv)
//{
//    if (argc != 2) {
//        puts("usage: tx <msg>");
//        return -1;
//    }
//
//    char* msg = argv[1];
//
//    params.pld_len_in_bytes = strlen(msg)+1;
//	ral_status_t res = ral_sx1280_setup_tx_lora(&ral_default_cfg, &params );
//    if (res != RAL_STATUS_OK) {
//        printf("ral_sx1280_setup_tx_lora ERROR %d\n", res);
//    } else {
//        ral_sx1280_set_pkt_payload(&ral_default_cfg, (uint8_t *)&msg, strlen(msg)+1 );
//        res = ral_sx1280_set_tx(&ral_default_cfg);
//        if (res != RAL_STATUS_OK) {
//            printf("ral_sx1280_set_tx ERROR %d\n", res);
//        }
//    }
//
//    return 0;
//}

/*
 * @brief Send packets to the SX1280 radio
 */
int sx1280_send_cmd(int argc, char **argv)
{
    if (argc < 2) {
        puts("usage: send <number of packet> <payload>");
        puts(
            "Options: if payload is empty, will send a random payload of size 16");
        return -1;
    }

    char *payload = NULL;
    int count = atoi(argv[1]);

    if (argc == 2) { //random mode
        payload = NULL;
    }
    else {
        payload = argv[2];
    }
    char random_payload[17] = { 0 };

    for (int i = 1; i <= count; i++) {
        char *to_send = payload;
        if (payload == NULL) {
            for (int j = 0; j < 16; j++) {
                random_payload[j] = (random_uint32() % 26) + 'A';
            }
            to_send = random_payload;
        }

        printf("sending \"%s\" payload (%u bytes)   (%d/%d)\n", to_send,
               (unsigned)strlen(to_send) + 1, i, count);

        params.pld_len_in_bytes = (unsigned)strlen(to_send) + 1;
        ral_status_t res = ral_sx1280_setup_tx_lora(&ral_default_cfg, &params);
        if (res != RAL_STATUS_OK) {
            printf("ral_sx1280_setup_tx_lora ERROR %d\n", res);
        }
        else {
            ral_sx1280_set_pkt_payload(&ral_default_cfg, (uint8_t *)to_send,
                                       (unsigned)strlen(to_send) + 1);
            res = ral_sx1280_set_tx(&ral_default_cfg);
            if (res != RAL_STATUS_OK) {
                printf("ral_sx1280_set_tx ERROR %d\n", res);
            }
        }
        if (i < count) {
            xtimer_usleep(DELAY_BETWEEN_TX);
        }
    }

    return 0;
}


// TODO int sx1280_sendhex_cmd(int argc, char **argv)

/*
 * @brief Listen the SX1280 radio for count packets during timeout_in_sec seconds for each packet
 *
 * @param timeout_in_sec
 * @param count
 */
int sx1280_listen_cmd(int argc, char **argv)
{
    uint32_t timeout;
    uint32_t count = 1;

    if (argc == 3) {
        timeout = atoi(argv[1]);
        count = atoi(argv[2]);
    }
    else if (argc == 2) {
        timeout = atoi(argv[1]);
    }
    else if (argc == 1) {
        timeout = 0xFFFFFFFF;
    }
    else {
        puts("usage: listen <timeout_in_sec> <count>");
        return -1;
    }

    for (uint32_t c = 0; c < count; c++) {
#ifdef CPU_ESP32
        printf("c=%d\n", c);
#else
        printf("c=%ld\n", c);
#endif

        params.pld_len_in_bytes = 255;
        ral_status_t res = ral_sx1280_setup_rx_lora(&ral_default_cfg, &params);
        if (res != RAL_STATUS_OK) {
            printf("ral_sx1280_setup_rx_lora ERROR %d\n", res);
        }
        else {
            _SX1280_WaitingRx = true;
            res = ral_sx1280_set_rx(&ral_default_cfg, timeout * 1000);
            if (res != RAL_STATUS_OK) {
                printf("ral_sx1280_set_rx ERROR %d\n", res);
                _SX1280_WaitingRx = false;
            }
            else {
                while (_SX1280_WaitingRx) {
                    xtimer_usleep(DELAY_FOR_RX);
                }

            }
        }



    }

    return 0;
}


/*
 * @brief handler when Tx is down
 */
static void _SX1280_OnTxDone(void)
{
    printf("SX1280 OnTxDone\n");
}

/*
 * @brief handler when Rx is down
 */
static void _SX1280_OnRxDone(void)
{
    static uint8_t buffer[64];

    printf("SX1280 OnRxDone\n");

    uint16_t size = 0;
    ral_status_t res = ral_sx1280_get_pkt_payload(&ral_default_cfg, buffer, 63,
                                                  &size);

    if (res != RAL_STATUS_OK) {
        printf("ral_sx1280_get_pkt_payload ERROR %d\n", res);
    }
    else {
        printf("Rx(%u): ", size);
        for (unsigned int i = 0; i < size; i++) {
            printf("%02x", buffer[i]);
        }
        printf("\n");

        ral_rx_pkt_status_lora_t pkt_status;
        res = ral_sx1280_get_lora_pkt_status(&ral_default_cfg, &pkt_status);
        if (res != RAL_STATUS_OK) {
            printf("ral_sx1280_get_lora_pkt_status ERROR %d\n", res);
        }
        else {
            printf(" SNR=%d dBm / RSSI=%d dBm\n", pkt_status.snr_pkt_in_db,
                   pkt_status.rssi_pkt_in_dbm);
        }
    }
    _SX1280_WaitingRx = false;

}

/*
 * @brief handler when the Rx timeout exceeded
 */
static void _SX1280_OnRxTimeout(void)
{
    printf("SX1280 OnRxTimeout\n");
    _SX1280_WaitingRx = false;

}

/*
 * @brief handler when a error occurs during the Rx
 */
static void _SX1280_OnRxError(void)
{
    printf("SX1280 OnRxError\n");
    _SX1280_WaitingRx = false;

}

static void sx1280_handler_cb(void *arg)
{
    (void)arg;  // unused param
    ral_sx1280_proces_lora_irqs(&ral_default_cfg);
}

/*
 * @brief Initialize the SX1280 driver
 */
int sx1280_init(void)
{
    _SX1280_WaitingRx = false;
    static const radio_callbacks_t rcb = { .txDone = _SX1280_OnTxDone, .rxDone =
                                               _SX1280_OnRxDone, .rxHeaderDone = NULL, .rxTimeout =
                                               _SX1280_OnRxTimeout, .rxError = _SX1280_OnRxError, };

    sx1280_init_status = ral_sx1280_init_all(&ral_default_cfg,
                                             sx1280_handler_cb, &rcb);

    if (sx1280_init_status != RAL_STATUS_OK) {
        printf("sx1280_init_status ERROR %d\n", sx1280_init_status);
        return -1;
    }
    return 0;
}

/*
 * @brief Initialize the SX1280 driver and reboot if the init failed
 */
void sx1280_init_and_reboot_on_failure(void)
{
    int ret = sx1280_init();

    if (ret != 0) {
        puts("rebooting ...");
        xtimer_sleep(SECONDS_IF_INIT_FAILED);
        pm_reboot();
    }
}
