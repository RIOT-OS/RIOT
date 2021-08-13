/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the PN532 NFC reader
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 *
 * @}
 */

#include "board.h"

#include "pn532.h"
#include "pn532_params.h"
#include "xtimer.h"

#define LOG_LEVEL LOG_INFO
#include "log.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static void printbuff(char *buff, unsigned len)
{
    while (len) {
        len--;
        printf("%02x ", *buff++);
    }
    puts("");
}

int main(void)
{
    static char data[16];
    static nfc_iso14443a_t card;
    static pn532_t pn532;
    unsigned len;
    int ret;

    pn532_mode_t mode = IS_ACTIVE(MODULE_PN532_I2C) ? PN532_I2C : PN532_SPI;
    ret = pn532_init(&pn532, &pn532_conf[0], mode);

    if (ret != 0) {
        LOG_INFO("init error %d\n", ret);
    }

    xtimer_usleep(200000);
    LOG_INFO("awake\n");

    uint32_t fwver;
    pn532_fw_version(&pn532, &fwver);
    LOG_INFO("ver %d.%d\n", (unsigned)PN532_FW_VERSION(fwver), (unsigned)PN532_FW_REVISION(fwver));

    ret = pn532_sam_configuration(&pn532, PN532_SAM_NORMAL, 1000);
    LOG_INFO("set sam %d\n", ret);

    while (1) {
        /* Delay not to be always polling the interface */
        xtimer_usleep(250000UL);

        ret = pn532_get_passive_iso14443a(&pn532, &card, 0x50);
        if (ret < 0) {
            LOG_DEBUG("no card\n");
            continue;
        }

        if (card.type == ISO14443A_TYPE4) {
            if (pn532_iso14443a_4_activate(&pn532, &card) != 0) {
                LOG_ERROR("act\n");
                continue;

            }
            else if (pn532_iso14443a_4_read(&pn532, data, &card, 0x00, 2) != 0) {
                LOG_ERROR("len\n");
                continue;
            }

            len = PN532_ISO14443A_4_LEN_FROM_BUFFER(data);
            len = MIN(len, sizeof(data));

            if (pn532_iso14443a_4_read(&pn532, data, &card, 0x02, len) != 0) {
                LOG_ERROR("read\n");
                continue;
            }

            LOG_INFO("dumping card contents (%d bytes)\n", len);
            printbuff(data, len);
            pn532_release_passive(&pn532, card.target);

        }
        else if (card.type == ISO14443A_MIFARE) {
            char key[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
            char data[32];

            for (int i = 0; i < 64; i++) {
                LOG_INFO("sector %02d, block %02d | ", i / 4, i);
                if ((i & 0x03) == 0) {
                    ret = pn532_mifareclassic_authenticate(&pn532, &card,
                                                           PN532_MIFARE_KEY_A, key, i);
                    if (ret != 0) {
                        LOG_ERROR("auth\n");
                        break;
                    }
                }

                ret = pn532_mifareclassic_read(&pn532, data, &card, i);
                if (ret == 0) {
                    printbuff(data, 16);
                }
                else {
                    LOG_ERROR("read\n");
                    break;
                }
            }

        }
        else {
            LOG_ERROR("unknown\n");
        }
    }

    return 0;
}
