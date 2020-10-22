/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  drivers_pn532
 *
 * @{
 * @file
 * @brief   PN532 driver
 *
 * @author  Víctor Ariño <victor.arino@triagnosys.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "xtimer.h"
#include "mutex.h"
#include "pn532.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"

#define ENABLE_DEBUG                0
#include "debug.h"

#define PN532_I2C_ADDRESS           (0x24)

/* Commands */
#define CMD_FIRMWARE_VERSION        (0x02)
#define CMD_READ_REG                (0x06)
#define CMD_WRITE_REG               (0x08)
#define CMD_READ_GPIO               (0x0c)
#define CMD_WRITE_GPIO              (0x0E)
#define CMD_SAM_CONFIG              (0x14)
#define CMD_RF_CONFIG               (0x32)
#define CMD_DATA_EXCHANGE           (0x40)
#define CMD_DESELECT                (0x44)
#define CMD_LIST_PASSIVE            (0x4a)
#define CMD_RELEASE                 (0x52)

/* Mifare specific commands */
#define MIFARE_CMD_READ             (0x30)
#define MIFARE_CMD_WRITE            (0xA0)

/* RF register settings */
#define RF_CONFIG_MAX_RETRIES       (0x05)

/* Buffer operations */
#define BUFF_CMD_START              (6)
#define BUFF_DATA_START             (BUFF_CMD_START + 1)
#define RAPDU_DATA_BEGIN            (1)
#define RAPDU_MAX_DATA_LEN          (CONFIG_PN532_BUFFER_LEN - BUFF_DATA_START - 5)
#define CAPDU_MAX_DATA_LEN          (CONFIG_PN532_BUFFER_LEN - BUFF_DATA_START - 1)

/* Constants and magic numbers */
#define MIFARE_CLASSIC_BLOCK_SIZE   (16)
#define RESET_TOGGLE_SLEEP          (400000)
#define RESET_BACKOFF               (10000)
#define HOST_TO_PN532               (0xD4)
#define PN532_TO_HOST               (0xD5)
#define SPI_DATA_WRITE              (0x80)
#define SPI_STATUS_READING          (0x40)
#define SPI_DATA_READ               (0xC0)
#define SPI_WRITE_DELAY_US          (2000)

/* SPI bus parameters */
#define SPI_MODE                    (SPI_MODE_0)
#define SPI_CLK                     (SPI_CLK_1MHZ)

/* Length for passive listings */
#define LIST_PASSIVE_LEN_14443(num)   (num * 20)

#if IS_ACTIVE(ENABLE_DEBUG)
#define PRINTBUFF printbuff
static void printbuff(uint8_t *buff, unsigned len)
{
    while (len) {
        len--;
        printf("%02x ", *buff++);
    }
    printf("\n");
}
#else
#define PRINTBUFF(...)
#endif

static void _nfc_event(void *dev)
{
    mutex_unlock(&((pn532_t *)dev)->trap);
}

void pn532_reset(const pn532_t *dev)
{
    assert(dev != NULL);

    DEBUG("pn532: reset\n");
    gpio_clear(dev->conf->reset);
    xtimer_usleep(RESET_TOGGLE_SLEEP);
    gpio_set(dev->conf->reset);
    xtimer_usleep(RESET_BACKOFF);
}

int pn532_init(pn532_t *dev, const pn532_params_t *params, pn532_mode_t mode)
{
    assert(dev != NULL);

    dev->conf = params;

    gpio_init_int(dev->conf->irq, GPIO_IN_PU, GPIO_FALLING,
                  _nfc_event, (void *)dev);

    gpio_init(dev->conf->reset, GPIO_OUT);
    gpio_set(dev->conf->reset);
    dev->mode = mode;
    if (mode == PN532_SPI) {
#ifdef PN532_SUPPORT_SPI
        /* we handle the CS line manually... */
        gpio_init(dev->conf->nss, GPIO_OUT);
        gpio_set(dev->conf->nss);
#endif
    }

    pn532_reset(dev);

    mutex_init(&dev->trap);
    mutex_lock(&dev->trap);

    return 0;
}

static uint8_t chksum(uint8_t *b, unsigned len)
{
    uint8_t c = 0x00;

    while (len--) {
        c -= *b++;
    }
    return c;
}

#ifdef PN532_SUPPORT_SPI
static void reverse(char *buff, unsigned len)
{
    while (len--) {
        buff[len] = (buff[len] & 0xF0) >> 4 | (buff[len] & 0x0F) << 4;
        buff[len] = (buff[len] & 0xCC) >> 2 | (buff[len] & 0x33) << 2;
        buff[len] = (buff[len] & 0xAA) >> 1 | (buff[len] & 0x55) << 1;
    }
}
#endif

static int _write(const pn532_t *dev, uint8_t *buff, unsigned len)
{
    int ret = -1;

    (void)buff;
    (void)len;

    switch (dev->mode) {
#ifdef PN532_SUPPORT_I2C
    case PN532_I2C:
        i2c_acquire(dev->conf->i2c);
        ret = i2c_write_bytes(dev->conf->i2c, PN532_I2C_ADDRESS, buff, len, 0);
        if (ret == 0) {
            ret = (int)len;
        }
        i2c_release(dev->conf->i2c);
        break;
#endif
#ifdef PN532_SUPPORT_SPI
    case PN532_SPI:
        spi_acquire(dev->conf->spi, SPI_CS_UNDEF, SPI_MODE, SPI_CLK);
        gpio_clear(dev->conf->nss);
        xtimer_usleep(SPI_WRITE_DELAY_US);
        reverse(buff, len);
        spi_transfer_byte(dev->conf->spi, SPI_CS_UNDEF, true, SPI_DATA_WRITE);
        spi_transfer_bytes(dev->conf->spi, SPI_CS_UNDEF, true, buff, NULL, len);
        gpio_set(dev->conf->nss);
        spi_release(dev->conf->spi);
        ret = (int)len;
        break;
#endif
    default:
        DEBUG("pn532: invalid mode (%i)!\n", dev->mode);
    }
    DEBUG("pn532: -> ");
    PRINTBUFF(buff, len);
    return ret;
}

static int _read(const pn532_t *dev, uint8_t *buff, unsigned len)
{
    int ret = -1;

    (void)buff;
    (void)len;

    switch (dev->mode) {
#ifdef PN532_SUPPORT_I2C
    case PN532_I2C:
        i2c_acquire(dev->conf->i2c);
        /* len+1 for RDY after read is accepted */
        ret = i2c_read_bytes(dev->conf->i2c, PN532_I2C_ADDRESS, buff, len + 1, 0);
        if (ret == 0) {
            ret = (int)len + 1;
        }
        i2c_release(dev->conf->i2c);
        break;
#endif
#ifdef PN532_SUPPORT_SPI
    case PN532_SPI:
        spi_acquire(dev->conf->spi, SPI_CS_UNDEF, SPI_MODE, SPI_CLK);
        gpio_clear(dev->conf->nss);
        spi_transfer_byte(dev->conf->spi, SPI_CS_UNDEF, true, SPI_DATA_READ);
        spi_transfer_bytes(dev->conf->spi, SPI_CS_UNDEF, true, NULL, &buff[1], len);
        gpio_set(dev->conf->nss);
        spi_release(dev->conf->spi);

        buff[0] = 0x80;
        reverse(buff, len);
        ret = (int)len + 1;
        break;
#endif
    default:
        DEBUG("pn532: invalid mode (%i)!\n", dev->mode);
    }
    if (ret > 0) {
        DEBUG("pn532: <- ");
        PRINTBUFF(buff, len);
    }
    return ret;
}

static int send_cmd(const pn532_t *dev, uint8_t *buff, unsigned len)
{
    unsigned pos;
    uint8_t checksum;

    buff[0] = 0x00;
    buff[1] = 0x00;
    buff[2] = 0xFF;

    len += 1;
    if (len < 0xff) {
        buff[3] = (char)len;
        buff[4] = 0x00 - buff[3];
        pos = 5;

    }
    else {
        buff[3] = 0xff;
        buff[4] = 0xff;
        buff[5] = (len >> 8) & 0xff;
        buff[6] = (len) & 0xff;
        buff[7] = 0x00 - buff[5] - buff[6];
        pos = 8;
    }

    buff[pos] = HOST_TO_PN532;
    checksum = chksum(&buff[pos], len);

    len += pos;
    buff[len++] = checksum;
    buff[len++] = 0x00;

    return _write(dev, buff, len);
}

static void wait_ready(pn532_t *dev)
{
    mutex_lock(&dev->trap);
}

/* Returns >0 payload len (or <0 received len but not as expected) */
static int read_command(const pn532_t *dev, uint8_t *buff, unsigned len, int expected_cmd)
{
    int r;
    unsigned j, fi, lp, lc;

    /* apply framing overhead */
    len += 8;
    if (len >= 0xff) {
        len += 3;
    }

    r = _read(dev, buff, len);

    /* Validate frame structure and CRCs
     *
     * Note that all offsets are shifted by one since the first byte is always
     * 0x01. */
    if ((r < (int)len) || (buff[1] != 0x00) || (buff[2] != 0x00) || (buff[3] != 0xFF)) {
        return -r;
    }

    if (buff[4] == 0xff && buff[5] == 0xff) {
        /* extended frame */
        lp = buff[6] << 8 | buff[7];
        lc = (buff[6] + buff[7] + buff[8]) & 0xff;
        fi = 9;

    }
    else {
        /* normal frame */
        lp = buff[4];
        lc = (buff[4] + buff[5]) & 0xff;
        fi = 6;
    }

    if (lc != 0 || lp >= 265 || buff[fi] != PN532_TO_HOST) {
        return -r;
    }

    if (lp == 0) {
        return -r;
    }

    if (chksum(&buff[fi], lp) != buff[fi + lp]) {
        return -r;
    }

    if (buff[fi + 1] != expected_cmd) {
        return -r;
    }

    /* Move the meaningful data to the beginning of the buffer */
    /* start copying after command byte */
    for (j = 0, fi += 2, lp -= 2; j < lp; fi++, j++) {
        buff[j] = buff[fi];
    }

    DEBUG("pn532: in cmd ");
    PRINTBUFF(buff, lp);

    return lp;
}

/* Returns 0 if OK, <0 otherwise */
static int send_check_ack(pn532_t *dev, uint8_t *buff, unsigned len)
{
    if (send_cmd(dev, buff, len) > 0) {
        static char ack[] = { 0x00, 0x00, 0xff, 0x00, 0xff, 0x00 };

        wait_ready(dev);
        if (_read(dev, buff, sizeof(ack)) != sizeof(ack) + 1) {
            return -2;
        }

        if (memcmp(&buff[1], ack, sizeof(ack)) != 0) {
            return -3;
        }

        wait_ready(dev);
        return 0;
    }

    return -1;
}

/* sendl: send length, recvl: receive payload length */
static int send_rcv(pn532_t *dev, uint8_t *buff, unsigned sendl, unsigned recvl)
{
    assert(dev != NULL);

    int expected_cmd = buff[BUFF_CMD_START] + 1;

    if (send_check_ack(dev, buff, sendl + 1)) {
        return 0;
    }

    recvl += 1; /* cmd response */
    return read_command(dev, buff, recvl, expected_cmd);
}

int pn532_fw_version(pn532_t *dev, uint32_t *fw_ver)
{
    unsigned ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START] = CMD_FIRMWARE_VERSION;

    if (send_rcv(dev, buff, 0, 4) == 4) {
        *fw_ver =  ((uint32_t)buff[0] << 24);   /* ic version */
        *fw_ver += ((uint32_t)buff[1] << 16);   /* fw ver */
        *fw_ver += ((uint32_t)buff[2] << 8);    /* fw rev */
        *fw_ver += (buff[3]);                   /* feature support */
        ret = 0;
    }

    return ret;
}

int pn532_read_reg(pn532_t *dev, char *out, unsigned addr)
{
    int ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_READ_REG;
    buff[BUFF_DATA_START    ] = (addr >> 8) & 0xff;
    buff[BUFF_DATA_START + 1] = addr & 0xff;

    if (send_rcv(dev, buff, 2, 1) == 1) {
        *out = buff[0];
        ret = 0;
    }

    return ret;
}

int pn532_write_reg(pn532_t *dev, unsigned addr, char val)
{
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_WRITE_REG;
    buff[BUFF_DATA_START    ] = (addr >> 8) & 0xff;
    buff[BUFF_DATA_START + 1] = addr & 0xff;
    buff[BUFF_DATA_START + 2] = val;

    return send_rcv(dev, buff, 3, 0);
}

static int _rf_configure(pn532_t *dev, uint8_t *buff, unsigned cfg_item, char *config,
                         unsigned cfg_len)
{
    buff[BUFF_CMD_START ] = CMD_RF_CONFIG;
    buff[BUFF_DATA_START] = cfg_item;
    for (unsigned i = 1; i <= cfg_len; i++) {
        buff[BUFF_DATA_START + i] = *config++;
    }

    return send_rcv(dev, buff, cfg_len + 1, 0);
}

static int _set_act_retries(pn532_t *dev, uint8_t *buff, unsigned max_retries)
{
    char rtrcfg[] = { 0xff, 0x01, max_retries & 0xff };

    return _rf_configure(dev, buff, RF_CONFIG_MAX_RETRIES, rtrcfg, sizeof(rtrcfg));
}

int pn532_sam_configuration(pn532_t *dev, pn532_sam_conf_mode_t mode, unsigned timeout)
{
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_SAM_CONFIG;
    buff[BUFF_DATA_START    ] = (char)mode;
    buff[BUFF_DATA_START + 1] = (char)(timeout / 50);
    buff[BUFF_DATA_START + 2] = 0x01;

    return send_rcv(dev, buff, 3, 0);
}

static int _list_passive_targets(pn532_t *dev, uint8_t *buff, pn532_target_t target,
                                 unsigned max, unsigned recvl)
{
    buff[BUFF_CMD_START] = CMD_LIST_PASSIVE;
    buff[BUFF_DATA_START] = (char) max;
    buff[BUFF_DATA_START + 1] = (char)target;

    /* requested len depends on expected target num and type */
    return send_rcv(dev, buff, 2, recvl);
}

int pn532_get_passive_iso14443a(pn532_t *dev, nfc_iso14443a_t *out,
                                unsigned max_retries)
{
    int ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    if (_set_act_retries(dev, buff, max_retries) == 0) {
        ret = _list_passive_targets(dev, buff, PN532_BR_106_ISO_14443_A, 1,
                                    LIST_PASSIVE_LEN_14443(1));
    }

    if (ret > 0 && buff[0] > 0) {
        out->target = buff[1];
        out->sns_res = (buff[2] << 8) | buff[3];
        out->sel_res = buff[4];
        out->id_len  = buff[5];
        out->type = ISO14443A_UNKNOWN;

        for (int i = 0; i < out->id_len; i++) {
            out->id[i] = buff[6 + i];
        }

        /* try to find out the type */
        if (out->id_len == 4) {
            out->type = ISO14443A_MIFARE;
        }
        else if (out->id_len == 7) {
            /* In the case of type 4, the first byte of RATS is the length
             * of RATS including the length itself (6+7) */
            if (buff[13] == ret - 13) {
                out->type = ISO14443A_TYPE4;
            }
        }
        ret = 0;
    }
    else {
        ret = -1;
    }

    return ret;
}

void pn532_deselect_passive(pn532_t *dev, unsigned target_id)
{
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START ] = CMD_DESELECT;
    buff[BUFF_DATA_START] = target_id;

    send_rcv(dev, buff, 1, 1);
}

void pn532_release_passive(pn532_t *dev, unsigned target_id)
{
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START ] = CMD_RELEASE;
    buff[BUFF_DATA_START] = target_id;

    send_rcv(dev, buff, 1, 1);
}

int pn532_mifareclassic_authenticate(pn532_t *dev, nfc_iso14443a_t *card,
                                     pn532_mifare_key_t keyid, char *key, unsigned block)
{
    int ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_DATA_EXCHANGE;
    buff[BUFF_DATA_START    ] = card->target;
    buff[BUFF_DATA_START + 1] = keyid;
    buff[BUFF_DATA_START + 2] = block; /* current block */

    /*
     * The card ID directly follows the key in the buffer
     * The key consists of 6 bytes and starts at offset 3
     */
    for (int i = 0; i < 6; i++) {
        buff[BUFF_DATA_START + 3 + i] = key[i];
    }

    for (int i = 0; i < card->id_len; i++) {
        buff[BUFF_DATA_START + 9 + i] = card->id[i];
    }

    ret = send_rcv(dev, buff, 9 + card->id_len, 1);
    if (ret == 1) {
        ret = buff[0];
        card->auth = 1;
    }

    return ret;
}

int pn532_mifareclassic_write(pn532_t *dev, char *idata, nfc_iso14443a_t *card,
                              unsigned block)
{
    int ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    if (card->auth) {

        buff[BUFF_CMD_START     ] = CMD_DATA_EXCHANGE;
        buff[BUFF_DATA_START    ] = card->target;
        buff[BUFF_DATA_START + 1] = MIFARE_CMD_WRITE;
        buff[BUFF_DATA_START + 2] = block; /* current block */
        memcpy(&buff[BUFF_DATA_START + 3], idata, MIFARE_CLASSIC_BLOCK_SIZE);

        if (send_rcv(dev, buff, 19, 1) == 1) {
            ret = buff[0];
        }

    }
    return ret;
}

static int pn532_mifare_read(pn532_t *dev, char *odata, nfc_iso14443a_t *card,
                             unsigned block, unsigned len)
{
    int ret = -1;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_DATA_EXCHANGE;
    buff[BUFF_DATA_START    ] = card->target;
    buff[BUFF_DATA_START + 1] = MIFARE_CMD_READ;
    buff[BUFF_DATA_START + 2] = block; /* current block */

    if (send_rcv(dev, buff, 3, len + 1) == (int)(len + 1)) {
        memcpy(odata, &buff[1], len);
        ret = 0;
    }

    return ret;
}

int pn532_mifareclassic_read(pn532_t *dev, char *odata, nfc_iso14443a_t *card,
                             unsigned block)
{
    if (card->auth) {
        return pn532_mifare_read(dev, odata, card, block, MIFARE_CLASSIC_BLOCK_SIZE);
    }
    else {
        return -1;
    }
}

int pn532_mifareulight_read(pn532_t *dev, char *odata, nfc_iso14443a_t *card,
                            unsigned page)
{
    return pn532_mifare_read(dev, odata, card, page, 32);
}

static int send_rcv_apdu(pn532_t *dev, uint8_t *buff, unsigned slen, unsigned rlen)
{
    int ret;

    rlen += 3;
    if ((rlen >= RAPDU_MAX_DATA_LEN) || (slen >= CAPDU_MAX_DATA_LEN)) {
        return -1;
    }

    ret = send_rcv(dev, buff, slen, rlen);
    if ((ret == (int)rlen) && (buff[0] == 0x00)) {
        ret = (buff[rlen - 2] << 8) | buff[rlen - 1];
        if (ret == (int)0x9000) {
            ret = 0;
        }
    }

    return ret;
}

int pn532_iso14443a_4_activate(pn532_t *dev, nfc_iso14443a_t *card)
{
    int ret;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    /* select app ndef tag */
    buff[BUFF_CMD_START      ] = CMD_DATA_EXCHANGE;
    buff[BUFF_DATA_START     ] = card->target;
    buff[BUFF_DATA_START +  1] = 0x00;
    buff[BUFF_DATA_START +  2] = 0xa4;
    buff[BUFF_DATA_START +  3] = 0x04;
    buff[BUFF_DATA_START +  4] = 0x00;
    buff[BUFF_DATA_START +  5] = 0x07;
    buff[BUFF_DATA_START +  6] = 0xD2;
    buff[BUFF_DATA_START +  7] = 0x76;
    buff[BUFF_DATA_START +  8] = 0x00;
    buff[BUFF_DATA_START +  9] = 0x00;
    buff[BUFF_DATA_START + 10] = 0x85;
    buff[BUFF_DATA_START + 11] = 0x01;
    buff[BUFF_DATA_START + 12] = 0x01;
    buff[BUFF_DATA_START + 13] = 0x00;

    DEBUG("pn532: select app\n");
    ret = send_rcv_apdu(dev, buff, 14, 0);

    /* select ndef file */
    buff[BUFF_CMD_START     ] = CMD_DATA_EXCHANGE;
    buff[BUFF_DATA_START    ] = card->target;
    buff[BUFF_DATA_START + 1] = 0x00;
    buff[BUFF_DATA_START + 2] = 0xa4;
    buff[BUFF_DATA_START + 3] = 0x00;
    buff[BUFF_DATA_START + 4] = 0x0c;
    buff[BUFF_DATA_START + 5] = 0x02;
    buff[BUFF_DATA_START + 6] = 0x00;
    buff[BUFF_DATA_START + 7] = 0x01;

    if (ret == 0) {
        DEBUG("pn532: select file\n");
        ret = send_rcv_apdu(dev, buff, 8, 0);
    }

    return ret;
}

int pn532_iso14443a_4_read(pn532_t *dev, char *odata, nfc_iso14443a_t *card,
                           unsigned offset, char len)
{
    int ret;
    uint8_t buff[CONFIG_PN532_BUFFER_LEN];

    buff[BUFF_CMD_START     ] = CMD_DATA_EXCHANGE;
    buff[BUFF_DATA_START    ] = card->target;
    buff[BUFF_DATA_START + 1] = 0x00;
    buff[BUFF_DATA_START + 2] = 0xb0;
    buff[BUFF_DATA_START + 3] = (offset >> 8) & 0xff;
    buff[BUFF_DATA_START + 4] = offset & 0xff;
    buff[BUFF_DATA_START + 5] = len;

    ret = send_rcv_apdu(dev, buff, 6, len);
    if (ret == 0) {
        memcpy(odata, &buff[RAPDU_DATA_BEGIN], len);
    }

    return ret;
}
