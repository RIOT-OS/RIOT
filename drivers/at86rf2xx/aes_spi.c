/* Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of at86rf2xx SPI security module (AES)
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <assert.h>

#include "xtimer.h"
#include "periph/spi.h"
#include "at86rf2xx_aes.h"

#define ENABLE_DEBUG              0
#include "debug.h"

#define AES_DEBUG(...)            DEBUG("[at86rf2xx_aes_spi]: "__VA_ARGS__)

#define AT86RF2XX_CMD_SRAM_READ   (0x00)
#define AT86RF2XX_CMD_SRAM_WRITE  (0x40)

static inline
void at86rf2xx_spi_get_bus(const at86rf2xx_t *dev)
{
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);
}

static inline
void at86rf2xx_spi_release_bus(const at86rf2xx_t *dev)
{
    spi_release(dev->params.spi);
}

static inline
uint8_t _aes_status(at86rf2xx_t *dev)
{
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      AT86RF2XX_CMD_SRAM_READ);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      AT86RF2XX_REG__AES_STATUS);
    return spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, 0);
}

static inline
void _aes_wait_for_result(at86rf2xx_t *dev)
{
    xtimer_usleep(AT86RF2XX_AES_DELAY_US);
    uint8_t status = _aes_status(dev);
    /*
        If this assert fires, there probably is an implementation error.
        The error bit is set before the transceiver has processed a data block.
        There are two cases:
        1. The delay between initiating an AES operation and sending the next cfg
           to AT86RF2XX_REG__AES_CTRL was too short. Meaning the transceiver
           did not have enough time to process the current block.
        2. Less then 16 bytes of data have been sent to the transceiver.

        Both should not occur in the code.
     */
    assert(!(status & AT86RF2XX_AES_STATUS_MASK__AES_ER));
    while (!(status & AT86RF2XX_AES_STATUS_MASK__AES_DONE)) {
        AES_DEBUG("status: %02x\n", status);
        status = _aes_status(dev);
    }
}

static inline
void _aes_open_read(at86rf2xx_t *dev, uint8_t addr)
{
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      AT86RF2XX_CMD_SRAM_READ);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      addr);
}

static inline
void _aes_open_write(at86rf2xx_t *dev, uint8_t addr)
{
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      AT86RF2XX_CMD_SRAM_WRITE);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true,
                      addr);
}

static inline
void _aes_transfer_bytes(at86rf2xx_t *dev, bool cont, const void* out,
                         void* in, size_t len)
{
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, cont, out,
                       in, len);
}

static inline
void _aes_save_key(at86rf2xx_t *dev, uint8_t cfg, uint8_t key[AT86RF2XX_AES_BLOCK_SIZE])
{
    _aes_open_write(dev, AT86RF2XX_REG__AES_CTRL);
    _aes_transfer_bytes(dev, false, &cfg, NULL, sizeof(cfg));
    _aes_open_read(dev, AT86RF2XX_REG__AES_KEY_START);
    _aes_transfer_bytes(dev, false, NULL, key, AT86RF2XX_AES_KEY_LENGTH);
}

static inline
void _aes_transfer_block(at86rf2xx_t *dev, uint8_t cfg, uint8_t mirror,
                         const aes_block_t src, aes_block_t dst)
{
    /*
        cfg:
        value which tells the AES engine what kind of data is coming in
        mirror:
        must be the same value as cfg but depending on whether the bit
        AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST is set, the transceiver
        will process the incoming block, or not
        src:
        current data block of 16 bytes to be sent to the AES engine
        dst:
        if not NULL, dst stores the processed data block of the
        block that has been sent to the AES engine most recently
    */

    /* access SRAM register AES_CTRL for writing */
    _aes_open_write(dev, AT86RF2XX_REG__AES_CTRL);
    /* MOSI: send configuration to the AES_CTRL register */
    _aes_transfer_bytes(dev, true, &cfg, NULL, sizeof(cfg));
    /* MOSI: send first byte of the current block (block_i) */
    _aes_transfer_bytes(dev, true, src, NULL, 1);
    /* MOSI: send the last 15 bytes of block_i */
    /* MISO: get the first 15 bytes of the most recently processed block (block_i-1) */
    _aes_transfer_bytes(dev, true, src + 1, dst, AT86RF2XX_AES_BLOCK_SIZE - 1);
    /* MOSI: send the mirrored cfg value and initiate the processing of block_i (or not) */
    /* MISO: get the last byte of block_i-1 */
    _aes_transfer_bytes(dev, false, &mirror,
                        dst ? dst + AT86RF2XX_AES_BLOCK_SIZE - 1 : NULL, 1);
}

void at86rf2xx_aes_key_read_encrypt(at86rf2xx_t *dev,
                                    uint8_t key[AT86RF2XX_AES_KEY_LENGTH])
{
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
                  AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    at86rf2xx_spi_get_bus(dev);
    _aes_save_key(dev, cfg, key);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_key_write_encrypt(at86rf2xx_t *dev,
                                     const uint8_t key[AT86RF2XX_AES_KEY_LENGTH])
{
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
                  AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    at86rf2xx_spi_get_bus(dev);
    _aes_open_write(dev, AT86RF2XX_REG__AES_CTRL);
    _aes_transfer_bytes(dev, true, &cfg, NULL, sizeof(cfg));
    _aes_transfer_bytes(dev, false, key, NULL, AT86RF2XX_AES_KEY_LENGTH);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_key_read_decrypt(at86rf2xx_t *dev,
                                    uint8_t key[AT86RF2XX_AES_KEY_LENGTH])
{
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
                  AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    at86rf2xx_spi_get_bus(dev);
    _aes_save_key(dev, cfg, key);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_key_write_decrypt(at86rf2xx_t *dev,
                                     const uint8_t key[AT86RF2XX_AES_KEY_LENGTH])
{
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
                  AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    at86rf2xx_spi_get_bus(dev);
    _aes_open_write(dev, AT86RF2XX_REG__AES_CTRL);
    _aes_transfer_bytes(dev, true, &cfg, NULL, sizeof(cfg));
    _aes_transfer_bytes(dev, false, key, NULL, AT86RF2XX_AES_KEY_LENGTH);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_ecb_encrypt(at86rf2xx_t *dev,
                               aes_block_t *cipher,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *plain,
                               uint8_t nblocks)
{
    if (!nblocks) {
        return;
    }
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
                  AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    uint8_t mirror = cfg | AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    at86rf2xx_spi_get_bus(dev);
    _aes_transfer_block(dev, cfg, mirror, plain[0], NULL);
    _aes_wait_for_result(dev);
    if (key) {
        cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
              AT86RF2XX_AES_CTRL_AES_DIR__DEC;
        _aes_save_key(dev, cfg, key);
    }
    cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
          AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    for (unsigned i = 1; i < nblocks; i++) {
        _aes_transfer_block(dev, cfg, mirror, plain[i],
                            cipher ? cipher[i - 1] : NULL);
        _aes_wait_for_result(dev);
    }

    /* send dummy bytes to get the last block of cipher text */
    mirror &= ~AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    _aes_transfer_block(dev, cfg, mirror, plain[0],
                        cipher ? cipher[nblocks - 1] : NULL);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_ecb_decrypt(at86rf2xx_t *dev,
                               aes_block_t *plain,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *cipher,
                               uint8_t nblocks)
{
    if (!nblocks) {
        return;
    }
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
                  AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    uint8_t mirror = cfg | AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    at86rf2xx_spi_get_bus(dev);
    _aes_transfer_block(dev, cfg, mirror, cipher[0], NULL);
    _aes_wait_for_result(dev);
    if (key) {
        cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
              AT86RF2XX_AES_CTRL_AES_DIR__ENC;
        _aes_save_key(dev, cfg, key);
    }
    cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
          AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    for (unsigned i = 1; i < nblocks; i++) {
        _aes_transfer_block(dev, cfg, mirror, cipher[i],
                            plain ? plain[i - 1] : NULL);
        _aes_wait_for_result(dev);
    }

    /* send dummy bytes to get the last block of plain text */
    mirror &= ~AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    _aes_transfer_block(dev, cfg, mirror, cipher[0],
                        plain ? plain[nblocks - 1] : NULL);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_cbc_encrypt(at86rf2xx_t *dev,
                               aes_block_t *cipher,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               uint8_t iv[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *plain,
                               uint8_t nblocks)
{
    if (!nblocks) {
        return;
    }
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
                  AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    uint8_t mirror = cfg | AT86RF2XX_AES_CTRL_MIRROR_AES_REQUEST__START;
    /* The first block has to be ECB encrypted because there is no
        cipher result to be XOR´ed from the last round.
        Instead an "initial vector" is XOR´ed to the first block
        of plain text. */
    uint8_t first[AT86RF2XX_AES_BLOCK_SIZE];
    for (unsigned i = 0; i < AT86RF2XX_AES_BLOCK_SIZE; i++) {
        first[i] = plain[0][i] ^ iv[i];
    }
    at86rf2xx_spi_get_bus(dev);
    _aes_transfer_block(dev, cfg, mirror, first, NULL);
    _aes_wait_for_result(dev);
    if (key) {
        cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
              AT86RF2XX_AES_CTRL_AES_DIR__DEC;
        _aes_save_key(dev, cfg, key);
    }
    cfg = AT86RF2XX_AES_CTRL_AES_MODE__CBC |
          AT86RF2XX_AES_CTRL_AES_DIR__ENC;
    mirror = cfg | AT86RF2XX_AES_CTRL_MIRROR_AES_REQUEST__START;
    for (unsigned i = 1; i < nblocks; i++) {
        _aes_transfer_block(dev, cfg, mirror, plain[i],
                            cipher ? cipher[i - 1] : NULL);
        _aes_wait_for_result(dev);
    }

    /* send dummy bytes to get the last block of cipher text */
    uint8_t *mac = cipher ? cipher[nblocks - 1] : iv;
    mirror &= ~AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    _aes_transfer_block(dev, cfg, mirror, plain[0], mac);
    at86rf2xx_spi_release_bus(dev);
}

void at86rf2xx_aes_cbc_decrypt(at86rf2xx_t *dev,
                               aes_block_t *plain,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               uint8_t iv[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *cipher,
                               uint8_t nblocks)
{
    if (!nblocks) {
        return;
    }
    uint8_t cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
                  AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    uint8_t mirror = cfg | AT86RF2XX_AES_CTRL_MIRROR_AES_REQUEST__START;
    at86rf2xx_spi_get_bus(dev);
    _aes_transfer_block(dev, cfg, mirror, cipher[0], NULL);
    _aes_wait_for_result(dev);
    if (key) {
        cfg = AT86RF2XX_AES_CTRL_AES_MODE__KEY |
              AT86RF2XX_AES_CTRL_AES_DIR__ENC;
        _aes_save_key(dev, cfg, key);
    }
    const uint8_t *xor = iv;
    cfg = AT86RF2XX_AES_CTRL_AES_MODE__ECB |
          AT86RF2XX_AES_CTRL_AES_DIR__DEC;
    for (unsigned i = 1; i < nblocks; i++) {
        _aes_transfer_block(dev, cfg, mirror, cipher[i],
                            plain ? plain[i - 1] : NULL);
        _aes_wait_for_result(dev);
        if (plain) {
            for (unsigned j = 0; j < AT86RF2XX_AES_BLOCK_SIZE; j++) {
                plain[i - 1][j] ^= xor[j];
            }
            xor = cipher[i - 1];
        }
    }

    /* send dummy bytes to get the last block of plain text */
    uint8_t *mac = plain ? plain[nblocks - 1] : iv;
    mirror &= ~AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST;
    _aes_transfer_block(dev, cfg, mirror, cipher[0], mac);
    if (plain) {
        for (unsigned j = 0; j < AT86RF2XX_AES_BLOCK_SIZE; j++) {
            plain[nblocks - 1][j] ^= xor[j];
        }
    }

    at86rf2xx_spi_release_bus(dev);
}
