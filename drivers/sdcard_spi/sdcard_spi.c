/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sdcard_spi
 * @{
 *
 * @file
 * @brief       low level driver for accessing sd-cards via spi interface.
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */
#define ENABLE_DEBUG (0)
#include "debug.h"
#include "sdcard_spi_internal.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static inline void _select_card_spi(sdcard_spi_t *card);
static inline void _unselect_card_spi(sdcard_spi_t *card);
static inline char _wait_for_r1(sdcard_spi_t *card, int32_t max_retries);
static inline void _send_dummy_byte(sdcard_spi_t *card);
static inline bool _wait_for_not_busy(sdcard_spi_t *card, int32_t max_retries);
static inline bool _wait_for_token(sdcard_spi_t *card, char token, int32_t max_retries);
static sd_init_fsm_state_t _init_sd_fsm_step(sdcard_spi_t *card, sd_init_fsm_state_t state);
static sd_rw_response_t _read_cid(sdcard_spi_t *card);
static sd_rw_response_t _read_csd(sdcard_spi_t *card);
static sd_rw_response_t _read_data_packet(sdcard_spi_t *card, char token, char *data, int size);
static sd_rw_response_t _write_data_packet(sdcard_spi_t *card, char token, char *data, int size);

/* CRC-7 (polynomial: x^7 + x^3 + 1) LSB of CRC-7 in a 8-bit variable is always 1*/
static char _crc_7(const char *data, int n);

/* CRC-16 (CRC-CCITT) (polynomial: x^16 + x^12 + x^5 + x^1) */
static uint16_t _crc_16(const char *data, size_t n);

/* use this transfer method instead of _transfer_bytes to force the use of 0xFF as dummy bytes */
static inline int _transfer_bytes(sdcard_spi_t *card, char *out, char *in, unsigned int length);

/* uses bitbanging for spi communication which allows to enable pull-up on the miso pin for
greater card compatibility on platforms that don't have a hw pull up installed */
static inline int _sw_spi_rxtx_byte(sdcard_spi_t *card, char out, char *in);

/* wrapper for default spi_transfer_byte function */
static inline int _hw_spi_rxtx_byte(sdcard_spi_t *card, char out, char *in);

/* function pointer to switch to hw spi mode after init sequence */
static int (*_dyn_spi_rxtx_byte)(sdcard_spi_t *card, char out, char *in);

int sdcard_spi_init(sdcard_spi_t *card, const sdcard_spi_params_t *params)
{
    sd_init_fsm_state_t state = SD_INIT_START;
    memcpy(&card->params, params, sizeof(sdcard_spi_params_t));
    card->spi_clk = SD_CARD_SPI_SPEED_PREINIT;

    do {
        state = _init_sd_fsm_step(card, state);
    } while (state != SD_INIT_FINISH);

    if (card->card_type != SD_UNKNOWN) {
        card->init_done = true;
        return SDCARD_SPI_OK;
    }
    card->init_done = false;
    return SDCARD_SPI_INIT_ERROR;
}

static sd_init_fsm_state_t _init_sd_fsm_step(sdcard_spi_t *card, sd_init_fsm_state_t state)
{
    switch (state) {

        case SD_INIT_START:
            DEBUG("SD_INIT_START\n");

            if ((gpio_init(card->params.mosi, GPIO_OUT) == 0) &&
                (gpio_init(card->params.clk,  GPIO_OUT) == 0) &&
                (gpio_init(card->params.cs,   GPIO_OUT) == 0) &&
                (gpio_init(card->params.miso, GPIO_IN_PU) == 0) &&
                ( (card->params.power == GPIO_UNDEF) ||
                  (gpio_init(card->params.power, GPIO_OUT) == 0)) ) {

                DEBUG("gpio_init(): [OK]\n");
                xtimer_init();
                return SD_INIT_SPI_POWER_SEQ;
            }

            DEBUG("gpio_init(): [ERROR]\n");
            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_SPI_POWER_SEQ:
            DEBUG("SD_INIT_SPI_POWER_SEQ\n");

            if (card->params.power != GPIO_UNDEF) {
                gpio_write(card->params.power, card->params.power_act_high);
                xtimer_usleep(SD_CARD_WAIT_AFTER_POWER_UP_US);
            }

            gpio_set(card->params.mosi);
            gpio_set(card->params.cs);   /* unselect sdcard for power up sequence */

            /* powersequence: perform at least 74 clockcycles with mosi_pin being high
             * (same as sending dummy bytes with 0xFF) */
            for (int i = 0; i < SD_POWERSEQUENCE_CLOCK_COUNT; i += 1) {
                gpio_set(card->params.clk);
                xtimer_usleep(SD_CARD_PREINIT_CLOCK_PERIOD_US/2);
                gpio_clear(card->params.clk);
                xtimer_usleep(SD_CARD_PREINIT_CLOCK_PERIOD_US/2);
            }
            return SD_INIT_SEND_CMD0;

        case SD_INIT_SEND_CMD0:
            DEBUG("SD_INIT_SEND_CMD0\n");

            gpio_clear(card->params.mosi);

            /* use soft-spi to perform init command to allow use of internal pull-ups on miso */
            _dyn_spi_rxtx_byte = &_sw_spi_rxtx_byte;

            /* select sdcard for cmd0 */
            gpio_clear(card->params.cs);
            char cmd0_r1 = sdcard_spi_send_cmd(card, SD_CMD_0, SD_CMD_NO_ARG, INIT_CMD0_RETRY_CNT);
            gpio_set(card->params.cs);

            if (R1_VALID(cmd0_r1) && !R1_ERROR(cmd0_r1) && R1_IDLE_BIT_SET(cmd0_r1)) {
                DEBUG("CMD0: [OK]\n");

                /* give control over SPI pins back to HW SPI device */
                spi_init_pins(card->params.spi_dev);
                /* switch to HW SPI since SD card is now in real SPI mode */
                _dyn_spi_rxtx_byte = &_hw_spi_rxtx_byte;
                return SD_INIT_ENABLE_CRC;
            }

            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_ENABLE_CRC:
            DEBUG("SD_INIT_ENABLE_CRC\n");
            _select_card_spi(card);
            char r1 = sdcard_spi_send_cmd(card, SD_CMD_59, SD_CMD_59_ARG_EN, INIT_CMD_RETRY_CNT);
            _unselect_card_spi(card);

            if (R1_VALID(r1) && !R1_ERROR(r1)) {
                DEBUG("CMD59: [OK]\n");
                return SD_INIT_SEND_CMD8;
            }
            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_SEND_CMD8:
            DEBUG("SD_INIT_SEND_CMD8\n");
            _select_card_spi(card);
            int cmd8_arg = (SD_CMD_8_VHS_2_7_V_TO_3_6_V << 8) | SD_CMD_8_CHECK_PATTERN;
            char cmd8_r1 = sdcard_spi_send_cmd(card, SD_CMD_8, cmd8_arg, INIT_CMD_RETRY_CNT);

            if (R1_VALID(cmd8_r1) && !R1_ERROR(cmd8_r1)) {
                DEBUG("CMD8: [OK] --> reading remaining bytes for R7\n");

                char r7[4];

                if (_transfer_bytes(card, 0, &r7[0], sizeof(r7)) == sizeof(r7)) {
                    DEBUG("R7 response: 0x%02x 0x%02x 0x%02x 0x%02x\n", r7[0], r7[1], r7[2], r7[3]);
                    /* check if lower 12 bits (voltage range and check pattern) of response and arg
                       are equal to verify compatibility and communication is working properly */
                    if (((r7[2] & 0x0F) == ((cmd8_arg >> 8) & 0x0F)) &&
                        (r7[3] == (cmd8_arg & 0xFF))) {
                        DEBUG("CMD8: [R7 MATCH]\n");
                        return SD_INIT_SEND_ACMD41_HCS;
                    }

                    DEBUG("CMD8: [R7 MISMATCH]\n");
                    _unselect_card_spi(card);
                    return SD_INIT_CARD_UNKNOWN;;
                }

                DEBUG("CMD8: _transfer_bytes (R7): [ERROR]\n");
                return SD_INIT_CARD_UNKNOWN;
            }

            DEBUG("CMD8: [ERROR / NO RESPONSE]\n");
            return SD_INIT_SEND_ACMD41;

        case SD_INIT_CARD_UNKNOWN:
            DEBUG("SD_INIT_CARD_UNKNOWN\n");
            card->card_type = SD_UNKNOWN;
            return SD_INIT_FINISH;

        case SD_INIT_SEND_ACMD41_HCS:
            DEBUG("SD_INIT_SEND_ACMD41_HCS\n");
            int acmd41_hcs_retries = 0;
            do {
                char acmd41hcs_r1 = sdcard_spi_send_acmd(card, SD_CMD_41, SD_ACMD_41_ARG_HC, 0);
                if (R1_VALID(acmd41hcs_r1) && !R1_ERROR(acmd41hcs_r1) &&
                    !R1_IDLE_BIT_SET(acmd41hcs_r1)) {
                    DEBUG("ACMD41: [OK]\n");
                    return SD_INIT_SEND_CMD58;
                }
                acmd41_hcs_retries++;
            } while (INIT_CMD_RETRY_CNT < 0 || acmd41_hcs_retries <= INIT_CMD_RETRY_CNT);;
            _unselect_card_spi(card);
            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_SEND_ACMD41:
            DEBUG("SD_INIT_SEND_ACMD41\n");
            int acmd41_retries = 0;
            do {
                char acmd41_r1 = sdcard_spi_send_acmd(card, SD_CMD_41, SD_CMD_NO_ARG, 0);
                if (R1_VALID(acmd41_r1) && !R1_ERROR(acmd41_r1) && !R1_IDLE_BIT_SET(acmd41_r1)) {
                    DEBUG("ACMD41: [OK]\n");
                    card->use_block_addr = false;
                    card->card_type = SD_V1;
                    return SD_INIT_SEND_CMD16;
                }
                acmd41_retries++;
            } while (INIT_CMD_RETRY_CNT < 0 || acmd41_retries <= INIT_CMD_RETRY_CNT);

            DEBUG("ACMD41: [ERROR]\n");
            return SD_INIT_SEND_CMD1;

        case SD_INIT_SEND_CMD1:
            DEBUG("SD_INIT_SEND_CMD1\n");
            DEBUG("COULD TRY CMD1 (for MMC-card)-> currently not suported\n");
            _unselect_card_spi(card);
            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_SEND_CMD58:
            DEBUG("SD_INIT_SEND_CMD58\n");
            char cmd58_r1 = sdcard_spi_send_cmd(card, SD_CMD_58, SD_CMD_NO_ARG, INIT_CMD_RETRY_CNT);
            if (R1_VALID(cmd58_r1) && !R1_ERROR(cmd58_r1)) {
                DEBUG("CMD58: [OK]\n");
                card->card_type = SD_V2;

                char r3[4];
                if (_transfer_bytes(card, 0, r3, sizeof(r3)) == sizeof(r3)) {
                    uint32_t ocr = ((uint32_t)r3[0] << (3 * 8)) |
                                   ((uint32_t)r3[1] << (2 * 8)) | (r3[2] << 8) | r3[3];
                    DEBUG("R3 RESPONSE: 0x%02x 0x%02x 0x%02x 0x%02x\n", r3[0], r3[1], r3[2], r3[3]);
                    DEBUG("OCR: 0x%"PRIx32"\n", ocr);

                    if ((ocr & SYSTEM_VOLTAGE) != 0) {
                        DEBUG("OCR: SYS VOLTAGE SUPPORTED\n");

                        if ((ocr & OCR_POWER_UP_STATUS) != 0) { //if power up outine is finished
                            DEBUG("OCR: POWER UP ROUTINE FINISHED\n");
                            if ((ocr & OCR_CCS) != 0) {         //if sd card is sdhc
                                DEBUG("OCR: CARD TYPE IS SDHC (SD_V2 with block adressing)\n");
                                card->use_block_addr = true;
                                _unselect_card_spi(card);
                                return SD_INIT_READ_CID;
                            }

                            DEBUG("OCR: CARD TYPE IS SDSC (SD_v2 with byte adressing)\n");
                            card->use_block_addr = false;
                            return SD_INIT_SEND_CMD16;
                        }

                        DEBUG("OCR: POWER UP ROUTINE NOT FINISHED!\n");
                        /* poll status till power up is finished */
                        return SD_INIT_SEND_CMD58;
                    }

                    DEBUG("OCR: SYS VOLTAGE NOT SUPPORTED!\n");
                }

                DEBUG("CMD58 response: [READ ERROR]\n");
            }

            DEBUG("CMD58: [ERROR]\n");
            _unselect_card_spi(card);
            return SD_INIT_CARD_UNKNOWN;

        case SD_INIT_SEND_CMD16:
            DEBUG("SD_INIT_SEND_CMD16\n");
            char r1_16 = sdcard_spi_send_cmd(card, SD_CMD_16, SD_HC_BLOCK_SIZE, INIT_CMD_RETRY_CNT);
            if (R1_VALID(r1_16) && !R1_ERROR(r1_16)) {
                DEBUG("CARD TYPE IS SDSC (SD_V1 with byte adressing)\n");
                _unselect_card_spi(card);
                return SD_INIT_READ_CID;
            }
            else {
                _unselect_card_spi(card);
                return SD_INIT_CARD_UNKNOWN;
            }

        case SD_INIT_READ_CID:
            DEBUG("SD_INIT_READ_CID\n");
            if (_read_cid(card) == SD_RW_OK) {
                return SD_INIT_READ_CSD;
            }
            else {
                DEBUG("reading cid register failed!\n");
                return SD_INIT_CARD_UNKNOWN;
            }

        case SD_INIT_READ_CSD:
            DEBUG("SD_INIT_READ_CSD\n");
            if (_read_csd(card) == SD_RW_OK) {
                if (card->csd_structure == SD_CSD_V1) {
                    DEBUG("csd_structure is version 1\n");
                }
                else if (card->csd_structure == SD_CSD_V2) {
                    DEBUG("csd_structure is version 2\n");
                }
                return SD_INIT_SET_MAX_SPI_SPEED;
            }
            else {
                DEBUG("reading csd register failed!\n");
                return SD_INIT_CARD_UNKNOWN;
            }

        case SD_INIT_SET_MAX_SPI_SPEED:
            DEBUG("SD_INIT_SET_MAX_SPI_SPEED\n");
            card->spi_clk = SD_CARD_SPI_SPEED_POSTINIT;
            DEBUG("SD_INIT_SET_MAX_SPI_SPEED: [OK]\n");
            return SD_INIT_FINISH;

        default:
            DEBUG("SD-INIT-FSM REACHED INVALID STATE!\n");
            return SD_INIT_CARD_UNKNOWN;

    }
}

static inline bool _wait_for_token(sdcard_spi_t *card, char token, int32_t max_retries)
{
    int tried = 0;

    do {
        char read_byte = 0;
        read_byte = spi_transfer_byte(card->params.spi_dev, GPIO_UNDEF, true,
                                      SD_CARD_DUMMY_BYTE);
        if (read_byte == token) {
            DEBUG("_wait_for_token: [MATCH]\n");
            return true;
        }
        else {
            DEBUG("_wait_for_token: [NO MATCH] (0x%02x)\n", read_byte);
        }

        tried++;
    } while ((max_retries < 0) || (tried <= max_retries));

    return false;
}

static inline void _send_dummy_byte(sdcard_spi_t *card)
{
    char read_byte;

    if (_dyn_spi_rxtx_byte(card, SD_CARD_DUMMY_BYTE, &read_byte) == 1) {
        DEBUG("_send_dummy_byte:echo: 0x%02x\n", read_byte);
    }
    else {
        DEBUG("_send_dummy_byte:_dyn_spi_rxtx_byte: [FAILED]\n");
    }
}

static inline bool _wait_for_not_busy(sdcard_spi_t *card, int32_t max_retries)
{
    char read_byte;
    int tried = 0;

    do {
        if (_dyn_spi_rxtx_byte(card, SD_CARD_DUMMY_BYTE, &read_byte) == 1) {
            if (read_byte == 0xFF) {
                DEBUG("_wait_for_not_busy: [OK]\n");
                return true;
            }
            else {
                DEBUG("_wait_for_not_busy: [BUSY]\n");
            }
        }
        else {
            DEBUG("_wait_for_not_busy:_dyn_spi_rxtx_byte: [FAILED]\n");
            return false;
        }

        tried++;
    } while ((max_retries < 0) || (tried <= max_retries));

    DEBUG("_wait_for_not_busy: [FAILED]\n");
    return false;
}

static char _crc_7(const char *data, int n)
{
    char crc = 0;

    for (int i = 0; i < n; i++) {
        char d = data[i];
        for (int j = 0; j < 8; j++) {
            crc <<= 1;
            if ((d & 0x80) ^ (crc & 0x80)) {
                crc ^= 0x09;
            }
            d <<= 1;
        }
    }
    return (crc << 1) | 1;
}

static uint16_t _crc_16(const char *data, size_t n)
{
    uint16_t crc = 0;

    for (size_t i = 0; i < n; i++) {
        crc = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= data[i];
        crc ^= (uint8_t)(crc & 0xFF) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0xFF) << 5;
    }
    return crc;
}

char sdcard_spi_send_cmd(sdcard_spi_t *card, char sd_cmd_idx, uint32_t argument, int32_t max_retry)
{
    int try_cnt = 0;
    char r1_resu;
    char cmd_data[6];

    cmd_data[0] = SD_CMD_PREFIX_MASK | sd_cmd_idx;
    cmd_data[1] = argument >> (3 * 8);
    cmd_data[2] = (argument >> (2 * 8)) & 0xFF;
    cmd_data[3] = (argument >> 8) & 0xFF;
    cmd_data[4] = argument & 0xFF;
    cmd_data[5] = _crc_7(cmd_data, sizeof(cmd_data) - 1);

    char echo[sizeof(cmd_data)];

    do {
        DEBUG("sdcard_spi_send_cmd: CMD%02d (0x%08lx) (retry %d)\n", sd_cmd_idx, argument, try_cnt);

        if (!_wait_for_not_busy(card, SD_WAIT_FOR_NOT_BUSY_CNT)) {
            DEBUG("sdcard_spi_send_cmd: timeout while waiting for bus to be not busy!\n");
            r1_resu = SD_INVALID_R1_RESPONSE;
            try_cnt++;
            continue;
        }

        if (_transfer_bytes(card, cmd_data, echo, sizeof(cmd_data)) != sizeof(cmd_data)) {
            DEBUG("sdcard_spi_send_cmd: _transfer_bytes: send cmd [%d]: [ERROR]\n", sd_cmd_idx);
            r1_resu = SD_INVALID_R1_RESPONSE;
            try_cnt++;
            continue;
        }

        DEBUG("CMD%02d echo: ", sd_cmd_idx);
        for (int i = 0; i < sizeof(echo); i++) {
            DEBUG("0x%02X ", echo[i]);
        }
        DEBUG("\n");

        /* received byte after cmd12 is a dummy byte and should be ignored */
        if (sd_cmd_idx == SD_CMD_12) {
            _send_dummy_byte(card);
        }

        r1_resu = _wait_for_r1(card, R1_POLLING_RETRY_CNT);

        if (R1_VALID(r1_resu)) {
            break;
        }
        else {
            DEBUG("sdcard_spi_send_cmd: R1_TIMEOUT (0x%02x)\n", r1_resu);
            r1_resu = SD_INVALID_R1_RESPONSE;
        }
        try_cnt++;

    } while ((max_retry < 0) || (try_cnt <= max_retry));

    return r1_resu;
}

char sdcard_spi_send_acmd(sdcard_spi_t *card, char sd_cmd_idx, uint32_t argument, int32_t max_retry)
{
    int err_cnt = 0;
    char r1_resu;

    do {
        DEBUG("sdcard_spi_send_acmd: CMD%02d (0x%08lx)(retry %d)\n", sd_cmd_idx, argument, err_cnt);
        r1_resu = sdcard_spi_send_cmd(card, SD_CMD_55, SD_CMD_NO_ARG, 0);
        if (R1_VALID(r1_resu) && !R1_ERROR(r1_resu)) {
            r1_resu = sdcard_spi_send_cmd(card, sd_cmd_idx, argument, 0);

            if (R1_VALID(r1_resu) && !R1_ERROR(r1_resu)) {
                return r1_resu;
            }
            else {
                DEBUG("ACMD%02d: [ERROR / NO RESPONSE]\n", sd_cmd_idx);
                err_cnt++;
            }
        }
        else {
            DEBUG("CMD55: [ERROR / NO RESPONSE]\n");
            err_cnt++;
        }

    } while ((max_retry < 0) || (err_cnt <= max_retry));

    DEBUG("sdcard_spi_send_acmd: [TIMEOUT]\n");
    return r1_resu;
}

static inline char _wait_for_r1(sdcard_spi_t *card, int32_t max_retries)
{
    int tried = 0;
    char r1;

    do {
        if (_dyn_spi_rxtx_byte(card, SD_CARD_DUMMY_BYTE, &r1) != 1) {
            DEBUG("_wait_for_r1: _dyn_spi_rxtx_byte:[ERROR]\n");
            tried++;
            continue;
        }
        else {
            DEBUG("_wait_for_r1: r1=0x%02x\n", r1);
        }

        if (R1_VALID(r1)) {
            DEBUG("_wait_for_r1: R1_VALID\n");
            return r1;
        }
        tried++;
    } while ((max_retries < 0) || (tried <= max_retries));

    DEBUG("_wait_for_r1: [TIMEOUT]\n");
    return r1;
}

void _select_card_spi(sdcard_spi_t *card)
{
    spi_acquire(card->params.spi_dev, GPIO_UNDEF,
                SD_CARD_SPI_MODE, card->spi_clk);
    gpio_clear(card->params.cs);
}

void _unselect_card_spi(sdcard_spi_t *card)
{
    gpio_set(card->params.cs);
    spi_release(card->params.spi_dev);
}

static inline int _sw_spi_rxtx_byte(sdcard_spi_t *card, char out, char *in){
    char rx = 0;
    int i = 7;
    for(; i >= 0; i--){
        if( ((out >> (i)) & 0x01) == 1){
            gpio_set(card->params.mosi);
        }else{
            gpio_clear(card->params.mosi);
        }
        xtimer_usleep(SD_CARD_PREINIT_CLOCK_PERIOD_US/2);
        gpio_set(card->params.clk);
        rx = (rx | ((gpio_read(card->params.miso) > 0) << i));
        xtimer_usleep(SD_CARD_PREINIT_CLOCK_PERIOD_US/2);
        gpio_clear(card->params.clk);
    }
    *in = rx;
    return 1;
}

static inline int _hw_spi_rxtx_byte(sdcard_spi_t *card, char out, char *in){
    *in = spi_transfer_byte(card->params.spi_dev, GPIO_UNDEF, true, out);
    return 1;
}

static inline int _transfer_bytes(sdcard_spi_t *card, char *out, char *in, unsigned int length){
    int trans_ret;
    unsigned trans_bytes = 0;
    char in_temp;

    for (trans_bytes = 0; trans_bytes < length; trans_bytes++) {
        if (out != NULL) {
            trans_ret = _dyn_spi_rxtx_byte(card, out[trans_bytes], &in_temp);
        }
        else {
            trans_ret = _dyn_spi_rxtx_byte(card, SD_CARD_DUMMY_BYTE, &in_temp);
        }
        if (trans_ret < 0) {
            return trans_ret;
        }
        if (in != NULL) {
            in[trans_bytes] = in_temp;
        }
    }

    return trans_bytes;
}

static sd_rw_response_t _read_data_packet(sdcard_spi_t *card, char token, char *data, int size)
{
    DEBUG("_read_data_packet: size: %d\n", size);
    if (_wait_for_token(card, token, SD_DATA_TOKEN_RETRY_CNT) == true) {
        DEBUG("_read_data_packet: [GOT TOKEN]\n");
    }
    else {
        DEBUG("_read_data_packet: [GOT NO TOKEN]\n");
        return SD_RW_NO_TOKEN;
    }

    if (_transfer_bytes(card, NULL, data, size) == size) {

        DEBUG("_read_data_packet: data: ");
        for (int i = 0; i < size; i++) {
            DEBUG("0x%02X ", data[i]);
        }
        DEBUG("\n");

        char crc_bytes[2];
        if (_transfer_bytes(card, 0, crc_bytes, sizeof(crc_bytes)) == sizeof(crc_bytes)) {
            uint16_t data__crc_16 = (crc_bytes[0] << 8) | crc_bytes[1];

            if (_crc_16(data, size) == data__crc_16) {
                DEBUG("_read_data_packet: [OK]\n");
                return SD_RW_OK;
            }
            else {
                DEBUG("_read_data_packet: [CRC_MISMATCH]\n");
                return SD_RW_CRC_MISMATCH;
            }
        }

        DEBUG("_read_data_packet: _transfer_bytes [RX_TX_ERROR] (while transmitting crc)\n");
        return SD_RW_RX_TX_ERROR;
    }

    DEBUG("_read_data_packet: _transfer_bytes [RX_TX_ERROR] (while transmitting payload)\n");
    return SD_RW_RX_TX_ERROR;
}

static inline int _read_blocks(sdcard_spi_t *card, int cmd_idx, int bladdr, char *data, int blsz,
                               int nbl, sd_rw_response_t *state)
{
    _select_card_spi(card);
    int reads = 0;

    uint32_t addr = card->use_block_addr ? bladdr : (bladdr * SD_HC_BLOCK_SIZE);
    char cmd_r1_resu = sdcard_spi_send_cmd(card, cmd_idx, addr, SD_BLOCK_READ_CMD_RETRIES);

    if (R1_VALID(cmd_r1_resu) && !R1_ERROR(cmd_r1_resu)) {
        DEBUG("_read_blocks: send CMD%d: [OK]\n", cmd_idx);

        for (int i = 0; i < nbl; i++) {
            *state = _read_data_packet(card, SD_DATA_TOKEN_CMD_17_18_24, &(data[i * blsz]), blsz);

            if (*state != SD_RW_OK) {
                DEBUG("_read_blocks: _read_data_packet: [FAILED]\n");
                _unselect_card_spi(card);
                return reads;
            }
            else {
                reads++;
            }
        }

        /* if this was a multi-block read */
        if (cmd_idx == SD_CMD_18) {
            cmd_r1_resu = sdcard_spi_send_cmd(card, SD_CMD_12, 0, 1);

            if (R1_VALID(cmd_r1_resu) && !R1_ERROR(cmd_r1_resu)) {
                DEBUG("_read_blocks: read multi (%d) blocks [OK]\n", nbl);
                *state = SD_RW_OK;
            }
            else {
                DEBUG("_read_blocks: send CMD12: [RX_TX_ERROR]\n");
                *state =  SD_RW_RX_TX_ERROR;
            }
        }
        else {
            DEBUG("_read_blocks: read single block [OK]\n");
            *state = SD_RW_OK;
        }
    }
    else {
        DEBUG("_read_blocks: send CMD%d: [RX_TX_ERROR]\n", cmd_idx);
        *state = SD_RW_RX_TX_ERROR;
    }

    _unselect_card_spi(card);
    return reads;
}

int sdcard_spi_read_blocks(sdcard_spi_t *card, int blockaddr, char *data, int blocksize,
                           int nblocks, sd_rw_response_t *state)
{
    if (nblocks > 1) {
        return _read_blocks(card, SD_CMD_18, blockaddr, data, blocksize, nblocks, state);
    }
    else {
        return _read_blocks(card, SD_CMD_17, blockaddr, data, blocksize, nblocks, state);
    }
}

static sd_rw_response_t _write_data_packet(sdcard_spi_t *card, char token, char *data, int size)
{

    spi_transfer_byte(card->params.spi_dev, GPIO_UNDEF, true, token);

    if (_transfer_bytes(card, data, 0, size) == size) {

        uint16_t data__crc_16 = _crc_16(data, size);
        char crc[sizeof(uint16_t)] = { data__crc_16 >> 8, data__crc_16 & 0xFF };

        if (_transfer_bytes(card, crc, 0, sizeof(crc)) == sizeof(crc)) {

            char data_response;

            data_response = (char)spi_transfer_byte(card->params.spi_dev, GPIO_UNDEF,
                                                    true, SD_CARD_DUMMY_BYTE);

            DEBUG("_write_data_packet: DATA_RESPONSE: 0x%02x\n", data_response);

            if (DATA_RESPONSE_IS_VALID(data_response)) {

                if (DATA_RESPONSE_ACCEPTED(data_response)) {
                    DEBUG("_write_data_packet: DATA_RESPONSE: [OK]\n");
                    return SD_RW_OK;
                }
                else {

                    if (DATA_RESPONSE_WRITE_ERR(data_response)) {
                        DEBUG("_write_data_packet: DATA_RESPONSE: [WRITE_ERROR]\n");
                    }

                    if (DATA_RESPONSE_CRC_ERR(data_response)) {
                        DEBUG("_write_data_packet: DATA_RESPONSE: [CRC_ERROR]\n");
                    }
                    return SD_RW_WRITE_ERROR;
                }

            }
            else {
                DEBUG("_write_data_packet: DATA_RESPONSE invalid\n");
                return SD_RW_RX_TX_ERROR;
            }

        }
        else {
            DEBUG("_write_data_packet: [RX_TX_ERROR] (while transmitting CRC16)\n");
            return SD_RW_RX_TX_ERROR;
        }
    }
    else {
        DEBUG("_write_data_packet: [RX_TX_ERROR] (while transmitting payload)\n");
        return SD_RW_RX_TX_ERROR;
    }
}

static inline int _write_blocks(sdcard_spi_t *card, char cmd_idx, int bladdr, char *data, int blsz,
                                int nbl, sd_rw_response_t *state)
{
    _select_card_spi(card);
    int written = 0;

    uint32_t addr = card->use_block_addr ? bladdr : (bladdr * SD_HC_BLOCK_SIZE);
    char cmd_r1_resu = sdcard_spi_send_cmd(card, cmd_idx, addr, SD_BLOCK_WRITE_CMD_RETRIES);

    if (R1_VALID(cmd_r1_resu) && !R1_ERROR(cmd_r1_resu)) {
        DEBUG("_write_blocks: send CMD%d: [OK]\n", cmd_idx);

        int token;
        if (cmd_idx == SD_CMD_25) {
            token = SD_DATA_TOKEN_CMD_25;
        }
        else {
            token = SD_DATA_TOKEN_CMD_17_18_24;
        }

        for (int i = 0; i < nbl; i++) {
            sd_rw_response_t write_resu = _write_data_packet(card, token, &(data[i * blsz]), blsz);
            if (write_resu != SD_RW_OK) {
                DEBUG("_write_blocks: _write_data_packet: [FAILED]\n");
                _unselect_card_spi(card);
                *state = write_resu;
                return written;
            }
            if (!_wait_for_not_busy(card, SD_WAIT_FOR_NOT_BUSY_CNT)) {
                DEBUG("_write_blocks: _wait_for_not_busy: [FAILED]\n");
                _unselect_card_spi(card);
                *state = SD_RW_TIMEOUT;
                return written;
            }
            written++;
        }

        /* if this is a multi-block write it is needed to issue a stop command*/
        if (cmd_idx == SD_CMD_25) {
            spi_transfer_byte(card->params.spi_dev, GPIO_UNDEF, true,
                              SD_DATA_TOKEN_CMD_25_STOP);
            DEBUG("_write_blocks: write multi (%d) blocks: [OK]\n", nbl);

            _send_dummy_byte(card); //sd card needs dummy byte before we can wait for not-busy state
            if (!_wait_for_not_busy(card, SD_WAIT_FOR_NOT_BUSY_CNT)) {
                _unselect_card_spi(card);
                *state = SD_RW_TIMEOUT;
            }
        }
        else {
            DEBUG("_write_blocks: write single block: [OK]\n");
            *state = SD_RW_OK;
        }

        _unselect_card_spi(card);
        return written;
    }
    else {
        DEBUG("_write_blocks: sdcard_spi_send_cmd: SD_CMD_ERROR_NO_RESP\n");
        _unselect_card_spi(card);
        *state = SD_RW_RX_TX_ERROR;
        return written;
    }
}

int sdcard_spi_write_blocks(sdcard_spi_t *card, int blockaddr, char *data, int blocksize,
                            int nblocks, sd_rw_response_t *state)
{
    if (nblocks > 1) {
        return _write_blocks(card, SD_CMD_25, blockaddr, data, blocksize, nblocks, state);
    }
    else {
        return _write_blocks(card, SD_CMD_24, blockaddr, data, blocksize, nblocks, state);
    }
}

sd_rw_response_t _read_cid(sdcard_spi_t *card)
{
    char cid_raw_data[SD_SIZE_OF_CID_AND_CSD_REG];
    sd_rw_response_t state;
    int nbl = _read_blocks(card, SD_CMD_10, 0, cid_raw_data, SD_SIZE_OF_CID_AND_CSD_REG,
                           SD_BLOCKS_FOR_REG_READ, &state);

    DEBUG("_read_cid: _read_blocks: nbl=%d state=%d\n", nbl, state);
    DEBUG("_read_cid: cid_raw_data: ");
    for (int i = 0; i < sizeof(cid_raw_data); i++) {
        DEBUG("0x%02X ", cid_raw_data[i]);
    }
    DEBUG("\n");

    char crc7 = _crc_7(&(cid_raw_data[0]), SD_SIZE_OF_CID_AND_CSD_REG - 1);
    if (nbl == SD_BLOCKS_FOR_REG_READ) {
        if (crc7 == cid_raw_data[SD_SIZE_OF_CID_AND_CSD_REG - 1]) {
            card->cid.MID = cid_raw_data[0];
            memcpy(&card->cid.OID[0], &cid_raw_data[1], SD_SIZE_OF_OID);
            memcpy(&card->cid.PNM[0], &cid_raw_data[2], SD_SIZE_OF_PNM);
            card->cid.PRV = cid_raw_data[8];
            memcpy((char *)&card->cid.PSN, &cid_raw_data[9], 4);
            card->cid.MDT = (cid_raw_data[13]<<4) | cid_raw_data[14];
            card->cid.CID_CRC = cid_raw_data[15];
            DEBUG("_read_cid: [OK]\n");
            return SD_RW_OK;
        }
        else {
            DEBUG("_read_cid: [SD_RW_CRC_MISMATCH] (data-crc: 0x%02x | calc-crc: 0x%02x)\n",
                  cid_raw_data[SD_SIZE_OF_CID_AND_CSD_REG - 1], crc7);
            return SD_RW_CRC_MISMATCH;
        }
    }
    return state;
}

sd_rw_response_t _read_csd(sdcard_spi_t *card)
{
    char c[SD_SIZE_OF_CID_AND_CSD_REG];
    sd_rw_response_t state;
    int read_resu = _read_blocks(card, SD_CMD_9, 0, c, SD_SIZE_OF_CID_AND_CSD_REG,
                                 SD_BLOCKS_FOR_REG_READ, &state);

    DEBUG("_read_csd: _read_blocks: read_resu=%d state=%d\n", read_resu, state);
    DEBUG("_read_csd: raw data: ");
    for (int i = 0; i < sizeof(c); i++) {
        DEBUG("0x%02X ", c[i]);
    }
    DEBUG("\n");

    if (read_resu == SD_BLOCKS_FOR_REG_READ) {
        if (_crc_7(c, SD_SIZE_OF_CID_AND_CSD_REG - 1) == c[SD_SIZE_OF_CID_AND_CSD_REG - 1]) {
            if (SD_GET_CSD_STRUCTURE(c) == SD_CSD_V1) {
                card->csd.v1.CSD_STRUCTURE = c[0]>>6;
                card->csd.v1.TAAC          = c[1];
                card->csd.v1.NSAC          = c[2];
                card->csd.v1.TRAN_SPEED    = c[3];
                card->csd.v1.CCC           = (c[4]<<4) | ((c[5] & 0xF0)>>4);
                card->csd.v1.READ_BL_LEN   = (c[5] & 0x0F);
                card->csd.v1.READ_BL_PARTIAL    = (c[6] &  (1<<7))>>7;
                card->csd.v1.WRITE_BLK_MISALIGN = (c[6] &  (1<<6))>>6;
                card->csd.v1.READ_BLK_MISALIGN  = (c[6] &  (1<<5))>>5;
                card->csd.v1.DSR_IMP            = (c[6] &  (1<<4))>>4;
                card->csd.v1.C_SIZE         = ((c[6]  & 0x03)<<10) | (c[7]<<2) | (c[8]>>6);
                card->csd.v1.VDD_R_CURR_MIN = (c[8]   & 0x38)>>3;
                card->csd.v1.VDD_R_CURR_MAX = (c[8]   & 0x07);
                card->csd.v1.VDD_W_CURR_MIN = (c[9]   & 0xE0)>>5;
                card->csd.v1.VDD_W_CURR_MAX = (c[9]   & 0x1C)>>2;
                card->csd.v1.C_SIZE_MULT    = ((c[9]  & 0x03)<<1) | (c[10]>>7);
                card->csd.v1.ERASE_BLK_EN   = (c[10]  & (1<<6))>>6;
                card->csd.v1.SECTOR_SIZE    = ((c[10] & 0x3F)<<1) | (c[11]>>7);
                card->csd.v1.WP_GRP_SIZE    = (c[11]  & 0x7F);
                card->csd.v1.WP_GRP_ENABLE  = c[12]>>7;
                card->csd.v1.R2W_FACTOR     = (c[12]   & 0x1C)>>2;
                card->csd.v1.WRITE_BL_LEN   = (c[12]   & 0x03)<<2 | (c[13]>>6);
                card->csd.v1.WRITE_BL_PARTIAL   = (c[13] & (1<<5))>>5;
                card->csd.v1.FILE_FORMAT_GRP    = (c[14] & (1<<7))>>7;
                card->csd.v1.COPY               = (c[14] & (1<<6))>>6;
                card->csd.v1.PERM_WRITE_PROTECT = (c[14] & (1<<5))>>5;
                card->csd.v1.TMP_WRITE_PROTECT  = (c[14] & (1<<4))>>4;
                card->csd.v1.FILE_FORMAT        = (c[14] & 0x0C)>>2;
                card->csd.v1.CSD_CRC                = c[15];
                card->csd_structure = SD_CSD_V1;
                return SD_RW_OK;
            }
            else if (SD_GET_CSD_STRUCTURE(c) == SD_CSD_V2) {
                card->csd.v2.CSD_STRUCTURE = c[0]>>6;
                card->csd.v2.TAAC          = c[1];
                card->csd.v2.NSAC          = c[2];
                card->csd.v2.TRAN_SPEED    = c[3];
                card->csd.v2.CCC           = (c[4]<<4) | ((c[5] & 0xF0)>>4);
                card->csd.v2.READ_BL_LEN   = (c[5] & 0x0F);
                card->csd.v2.READ_BL_PARTIAL    = (c[6] & (1<<7))>>7;
                card->csd.v2.WRITE_BLK_MISALIGN = (c[6] & (1<<6))>>6;
                card->csd.v2.READ_BLK_MISALIGN  = (c[6] & (1<<5))>>5;
                card->csd.v2.DSR_IMP            = (c[6] & (1<<4))>>4;
                card->csd.v2.C_SIZE             = (((uint32_t)c[7] & 0x3F)<<16)
                                                   | (c[8]<<8) | c[9];
                card->csd.v2.ERASE_BLK_EN       = (c[10] & (1<<6))>>6;
                card->csd.v2.SECTOR_SIZE        = (c[10] & 0x3F)<<1 | (c[11]>>7);
                card->csd.v2.WP_GRP_SIZE        = (c[11] & 0x7F);
                card->csd.v2.WP_GRP_ENABLE      = (c[12] & (1<<7))>> 7;
                card->csd.v2.R2W_FACTOR         = (c[12] & 0x1C)>> 2;
                card->csd.v2.WRITE_BL_LEN       = ((c[12] & 0x03)<<2) | (c[13]>>6);
                card->csd.v2.WRITE_BL_PARTIAL   = (c[13] & (1<<5))>>5;
                card->csd.v2.FILE_FORMAT_GRP    = (c[14] & (1<<7))>>7;
                card->csd.v2.COPY               = (c[14] & (1<<6))>>6;
                card->csd.v2.PERM_WRITE_PROTECT = (c[14] & (1<<5))>>5;
                card->csd.v2.TMP_WRITE_PROTECT  = (c[14] & (1<<4))>>4;
                card->csd.v2.FILE_FORMAT        = (c[14] & 0x0C)>>2;
                card->csd.v2.CSD_CRC                = c[15];
                card->csd_structure = SD_CSD_V2;
                return SD_RW_OK;
            }
            else {
                return SD_RW_NOT_SUPPORTED;
            }

        }
        else {
            return SD_RW_CRC_MISMATCH;
        }
    }
    return state;
}

sd_rw_response_t sdcard_spi_read_sds(sdcard_spi_t *card, sd_status_t *sd_status){
    _select_card_spi(card);
    char sds_raw_data[SD_SIZE_OF_SD_STATUS];
    char r1_resu = sdcard_spi_send_cmd(card, SD_CMD_55, SD_CMD_NO_ARG, 0);
    _unselect_card_spi(card);
    if (R1_VALID(r1_resu)) {
        if(!R1_ERROR(r1_resu)){

            sd_rw_response_t state;
            int nbl = _read_blocks(card, SD_CMD_13, 0, sds_raw_data, SD_SIZE_OF_SD_STATUS,
                                   SD_BLOCKS_FOR_REG_READ, &state);

            DEBUG("sdcard_spi_read_sds: _read_blocks: nbl=%d state=%d\n", nbl, state);
            DEBUG("sdcard_spi_read_sds: sds_raw_data: ");
            for (int i = 0; i < sizeof(sds_raw_data); i++) {
                DEBUG("0x%02X ", sds_raw_data[i]);
            }
            DEBUG("\n");

            if (nbl == SD_BLOCKS_FOR_REG_READ) {
                sd_status->DAT_BUS_WIDTH =  sds_raw_data[0] >> 6;
                sd_status->SECURED_MODE  = (sds_raw_data[0] & (1<<5)) >> 5;
                sd_status->SD_CARD_TYPE  = (sds_raw_data[2] << 8) | sds_raw_data[3];
                sd_status->SIZE_OF_PROTECTED_AREA = ((uint32_t)sds_raw_data[4] << (3*8)) |
                                                    ((uint32_t)sds_raw_data[5] << (2*8)) |
                                                    (sds_raw_data[6] << 8    ) |
                                                     sds_raw_data[7];
                sd_status->SPEED_CLASS       = sds_raw_data[8];
                sd_status->PERFORMANCE_MOVE  = sds_raw_data[9];
                sd_status->AU_SIZE           = sds_raw_data[10] >> 4;
                sd_status->ERASE_SIZE        = (sds_raw_data[11] << 8) | sds_raw_data[12];
                sd_status->ERASE_TIMEOUT     = sds_raw_data[13] >> 2;
                sd_status->ERASE_OFFSET      = sds_raw_data[13] & 0x03;
                sd_status->UHS_SPEED_GRADE   = sds_raw_data[14] >> 4;
                sd_status->UHS_AU_SIZE       = sds_raw_data[14] & 0x0F;
                sd_status->VIDEO_SPEED_CLASS = sds_raw_data[15];
                sd_status->VSC_AU_SIZE       = ((sds_raw_data[16] & 0x03) << 8)
                                               | sds_raw_data[17];
                sd_status->SUS_ADDR          = (sds_raw_data[18] << 14) |
                                               (sds_raw_data[19] << 6 ) |
                                               (sds_raw_data[20] >> 2 );
                DEBUG("sdcard_spi_read_sds: [OK]\n");
                return SD_RW_OK;
            }
            return state;
        }
        return SD_RW_RX_TX_ERROR;
    }
    return SD_RW_TIMEOUT;
}

uint64_t sdcard_spi_get_capacity(sdcard_spi_t *card)
{
    if (card->csd_structure == SD_CSD_V1) {
        uint32_t block_len = (1 << card->csd.v1.READ_BL_LEN);
        uint32_t mult = 1 << (card->csd.v1.C_SIZE_MULT + 2);
        uint32_t blocknr = (card->csd.v1.C_SIZE + 1) * mult;
        return blocknr * block_len;
    }
    else if (card->csd_structure == SD_CSD_V2) {
        return (card->csd.v2.C_SIZE + 1) * (uint64_t)(SD_HC_BLOCK_SIZE << 10);
    }
    return 0;
}

uint32_t sdcard_spi_get_sector_count(sdcard_spi_t *card)
{
    return sdcard_spi_get_capacity(card) / SD_HC_BLOCK_SIZE;
}

uint32_t sdcard_spi_get_au_size(sdcard_spi_t *card)
{
    sd_status_t sds;
    if(sdcard_spi_read_sds(card, &sds) == SD_RW_OK) {
        if (sds.AU_SIZE < 0xB) {
            return 1 << (13 + sds.AU_SIZE); /* sds->AU_SIZE = 1 maps to 16KB; 2 to 32KB etc.*/
        }
        else if (sds.AU_SIZE == 0xB) {
            return 12 * SDCARD_SPI_IEC_KIBI * SDCARD_SPI_IEC_KIBI; /* 12 MB */
        }
        else if (sds.AU_SIZE == 0xC) {
            return 1 << (12 + sds.AU_SIZE); /* 16 MB */
        }
        else if (sds.AU_SIZE == 0xD) {
            return 24 * SDCARD_SPI_IEC_KIBI * SDCARD_SPI_IEC_KIBI; /* 24 MB */
        }
        else if (sds.AU_SIZE > 0xD) {
            return 1 << (11 + sds.AU_SIZE); /* 32 MB or 64 MB */
        }
    }
    return 0; /* AU_SIZE is not defined by the card */
}
