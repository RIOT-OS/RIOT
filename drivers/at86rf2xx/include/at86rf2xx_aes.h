/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 *
 * The extended feature set of at86rf2xx transceivers comprises a
 * hardware implementation of AES. There are two supported block
 * cipher modes, ECB and CBC.
 *
 * @{
 *
 * @file
 * @brief       Interface of the at86rf2xx security module (AES)
 *
 * @author      Fabian Hüßler   <fabian.huessler@ovgu.de>
 */
#ifndef AT86RF2XX_AES_H
#define AT86RF2XX_AES_H

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AES key length in bits
 */
#define AT86RF2XX_AES_KEY_BITS                          (128U)
/**
 * @brief   AES key length in bytes
 */
#define AT86RF2XX_AES_KEY_LENGTH                        ((AT86RF2XX_AES_KEY_BITS) / 8)
/**
 * @brief   AES block size in bytes
 */
#define AT86RF2XX_AES_BLOCK_SIZE                        ((AT86RF2XX_AES_KEY_BITS) / 8)
/**
 * @brief   Time to complete the AES algorithm in us
 */
#define AT86RF2XX_AES_DELAY_US                          (24)

/**
 * @name    AES rigister addresses
 * @{
 */
#define AT86RF2XX_REG__AES_STATUS                       (0x82)
#define AT86RF2XX_REG__AES_CTRL                         (0x83)
#define AT86RF2XX_REG__AES_KEY_START                    (0x84)
#define AT86RF2XX_REG__AES_KEY_END                      (0x93)
#define AT86RF2XX_REG__AES_DATA_START                   (0x84)
#define AT86RF2XX_REG__AES_DATA_END                     (0x93)
#define AT86RF2XX_REG__AES_CTRL_MIRROR                  (0x94)
/** @} */

/**
 * @name    Layout of register AES_STATUS
 * @{
 */
#define AT86RF2XX_AES_STATUS_MASK__AES_ER               (0x80)
#define AT86RF2XX_AES_STATUS_MASK__AES_DONE             (0x01)

#define AT86RF2XX_AES_STATUS_AES_ER__NO_ERROR           (0x00)
#define AT86RF2XX_AES_STATUS_AES_ER__ERROR              (0x80)

#define AT86RF2XX_AES_STATUS_AES_DONE__NOT_DONE         (0x00)
#define AT86RF2XX_AES_STATUS_AES_DONE__DONE             (0x01)
/** @} */

/**
 * @name    Layout of register AES_CTRL
 * @{
 */
#define AT86RF2XX_AES_CTRL_MASK__AES_REQUEST            (0x80)
#define AT86RF2XX_AES_CTRL_MASK__AES_MODE               (0x70)
#define AT86RF2XX_AES_CTRL_MASK__AES_DIR                (0x08)

#define AT86RF2XX_AES_CTRL_AES_REQUEST__IDLE            (0x00)
#define AT86RF2XX_AES_CTRL_AES_REQUEST__START           (0x80)

#define AT86RF2XX_AES_CTRL_AES_MODE__ECB                (0x00)
#define AT86RF2XX_AES_CTRL_AES_MODE__KEY                (0x10)
#define AT86RF2XX_AES_CTRL_AES_MODE__CBC                (0x20)

#define AT86RF2XX_AES_CTRL_AES_DIR__ENC                 (0x00)
#define AT86RF2XX_AES_CTRL_AES_DIR__DEC                 (0x08)
/** @} */

/**
 * @name    Layout of register AES_CTRL_MIRROR
 * @{
 */
#define AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_REQUEST     (0x80)
#define AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_MODE        (0x70)
#define AT86RF2XX_AES_CTRL_MIRROR_MASK__AES_DIR         (0x08)

#define AT86RF2XX_AES_CTRL_MIRROR_AES_REQUEST__IDLE     (0x00)
#define AT86RF2XX_AES_CTRL_MIRROR_AES_REQUEST__START    (0x80)

#define AT86RF2XX_AES_CTRL_MIRROR_AES_MODE__ECB         (0x00)
#define AT86RF2XX_AES_CTRL_MIRROR_AES_MODE__KEY         (0x10)
#define AT86RF2XX_AES_CTRL_MIRROR_AES_MODE__CBC         (0x20)

#define AT86RF2XX_AES_CTRL_MIRROR_AES_DIR__ENC          (0x00)
#define AT86RF2XX_AES_CTRL_MIRROR_AES_DIR__DEC          (0x08)
/** @} */

/**
 * @brief   An AES block
 *
 * AES works on blocks of 16 bytes
 */
typedef uint8_t aes_block_t[AT86RF2XX_AES_BLOCK_SIZE];

/**
 * @brief   Read the AES key used for encryption
 *
 * @param[in]       dev     Device
 * @param[out]      key     Buffer to store the key
 */
void at86rf2xx_aes_key_read_encrypt(at86rf2xx_t *dev,
                                    uint8_t key[AT86RF2XX_AES_KEY_LENGTH]);

/**
 * @brief   Write the AES key used for encryption
 *
 * It is important to write the encryption key, before encryption is done
 *
 * @param[in]       dev     Device
 * @param[in]       key     Buffer which stores the key
 */
void at86rf2xx_aes_key_write_encrypt(at86rf2xx_t *dev,
                                     const uint8_t key[AT86RF2XX_AES_KEY_LENGTH]);

/**
 * @brief   Read the AES key used for decryption
 *
 * @param[in]       dev     Device
 * @param[out]      key     Buffer to store the key
 */
void at86rf2xx_aes_key_read_decrypt(at86rf2xx_t *dev,
                                    uint8_t key[AT86RF2XX_AES_KEY_LENGTH]);

/**
 * @brief   Write the AES key used for decryption
 *
 * It is important to write the decryption key, before decryption is done
 *
 * @param[in]       dev     Device
 * @param[in]       key     Buffer which stores the key
 */
void at86rf2xx_aes_key_write_decrypt(at86rf2xx_t *dev,
                                     const uint8_t key[AT86RF2XX_AES_KEY_LENGTH]);

/**
 * @brief   Perform AES algorithm and encrypt data blocks
 *          in @p plain to cipher data blocks, using ECB mode
 *
 * @note    The encryption key must have been written before.
 *
 * @param[in]       dev     Device
 * @param[out]      cipher  If not NULL, cipher data blocks
 * @param[out]      key     If not NULL, last round encryption key is stored
 * @param[in]       plain   Plain data blocks
 * @param[in]       nblocks Number of blocks
 */
void at86rf2xx_aes_ecb_encrypt(at86rf2xx_t *dev,
                               aes_block_t *cipher,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *plain,
                               uint8_t nblocks);

/**
 * @brief   Perform AES algorithm and decrypt data blocks
 *          in @p cipher to plain data blocks, using ECB mode
 *
 * @note    The decryption key must have been written before.
 *
 * @param[in]       dev     Device
 * @param[out]      plain   If not NULL, plain data blocks
 * @param[out]      key     If not NULL, last round decryption key is stored
 * @param[in]       cipher  Cipher data blocks
 * @param[in]       nblocks Number of blocks
 */
void at86rf2xx_aes_ecb_decrypt(at86rf2xx_t *dev,
                               aes_block_t *plain,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *cipher,
                               uint8_t nblocks);

/**
 * @brief   Perform AES algorithm and encrypt data blocks
 *          in @p plain to cipher data blocks, using CBC mode
 *
 * @note    The encryption key must have been written before.
 *
 * @param[in]       dev     Device
 * @param[out]      cipher  If not NULL, cipher blocks
 * @param[out]      key     If not NULL, last round encryption key is stored
 * @param[in,out]   iv      in: initial vector, out: last cipher block if cipher is NULL
 * @param[in]       plain   Plain data blocks
 * @param[in]       nblocks Number of blocks
 */
void at86rf2xx_aes_cbc_encrypt(at86rf2xx_t *dev,
                               aes_block_t *cipher,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               uint8_t iv[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *plain,
                               uint8_t nblocks);

/**
 * @brief   Perform AES algorithm and decrypt data blocks
 *          in @p cipher to plain data blocks, using CBC mode
 *
 * @note    The decryption key must have been written before.
 *
 * @param[in]       dev     Device
 * @param[out]      plain   If not NUll, plain data blocks
 * @param[out]      key     If not NULL, last round decryption key is stored
 * @param[in,out]   iv      in: initial vector, out: last plain block if plain is NULL
 * @param[in]       cipher  Cipher data blocks
 * @param[in]       nblocks Number of blocks
 */
void at86rf2xx_aes_cbc_decrypt(at86rf2xx_t *dev,
                               aes_block_t *plain,
                               uint8_t key[AT86RF2XX_AES_BLOCK_SIZE],
                               uint8_t iv[AT86RF2XX_AES_BLOCK_SIZE],
                               const aes_block_t *cipher,
                               uint8_t nblocks);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AT86RF2XX_AES_H */
