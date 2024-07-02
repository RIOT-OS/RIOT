/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Example configuration for Microchip CryptoAuth devices
 *
 *              This is an example configuration for an ATECC608A device with
 *              some explanation on what the bits mean (for more details
 *              see complete datasheet).
 *
 * @note        This configuration works with the example/psa_crypto application.
 *
 * @warning     Parts of this config are insecure (e.g. some key slots can be
 *              written to repeatedly), so it should only be used for
 *              testing and development. If you want to use your device for
 *              production, please refer to the datasheet and change the bits
 *              accordingly.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef ATECC608A_CONFIG_H
#define ATECC608A_CONFIG_H

#include <stdint.h>

const uint8_t atecc608a_config[] = {
    0x00, 0x00, 0x00, 0x00, /* Read only serial number */
    0x00, 0x00, 0x00, 0x00, /* Read only revision number */
    0x00, 0x00, 0x00, 0x00, /* Read only serial number */
    0x00, 0x00, 0x00, 0x00, /* Read only reserved, I2C enable, reserved */
    0xC0, 0x00, 0x00, 0x00, /* I2C address, reserved, CountMatch, chip mode*/

    /*  Private keys:
        0x8720 = 1 0 0 0 | 0 1 1 1 | 0 0 1 0 | 0 0 0 0
                 7  -  4 | 3  -  0 | 15 - 12 | 11 -  8
        - External signatures of arbitrary messages enabled
        - Internal signatures of messages by GenDig/GenKey enabled
        - ECDH operations permitted
        - ECDH Master Secret will be output in the clear
        - No write by write command */
    0x87, 0x20, 0x87, 0x20, /* Slot 0, Slot 1 */
    0x87, 0x20, 0x87, 0x20, /* Slot 2, Slot 3 */

    /* Private Keys, write always allowed (use only for testing!!!)*/
    0x87, 0x00, 0x87, 0x00,

    /*  Private key:
        - as above but
        - ECDH Master Secret will be written into slot n + 1 (7)*/
    0x8F, 0x20, 0x87, 0x20, /* Slot 6, Slot 7 */

    /* Data storage and public keys, anything goes */
    0x00, 0x00, 0x00, 0x00, /* Slot 8, Slot 9 */
    0x00, 0x00, 0x00, 0x00, /* Slot 10, Slot 11 */
    0x00, 0x00, 0x00, 0x00, /* Slot 12, Slot 13 */
    0x00, 0x00, 0x00, 0x00, /* Slot 14, Slot 15 */

    0xFF, 0xFF, 0xFF, 0xFF, /* Counter 0 */
    0x00, 0x00, 0x00, 0x00, /* Counter 0 */
    0xFF, 0xFF, 0xFF, 0xFF, /* Counter 1 */
    0x00, 0x00, 0x00, 0x00, /* Counter 1 */
    0x00, 0x00, 0x00, 0x00, /* UseLock, VolatileKeyPermission, Secure Boot */
    0x00, 0x00, 0x00, 0x00, /* KdflvLoc, KdflvStr, KdflcStr, Reserved */
    0x00, 0x00, 0x00, 0x00, /* Reserved */
    0x00, 0x00, 0x00, 0x00, /* Reserved */
    0x00, 0x00, 0x55, 0x55, /* UserExtra, UserExtraAdd, LockValue, LockConfig */
    0xFF, 0xFF, 0x00, 0x00, /* 2x SlotLocked, 2x ChipOptions */
    0x00, 0x00, 0x00, 0x00, /* X509format */

    /*  Private Key, access only with Sign, GenKey, PrivWrite cmds
        Public Version can always be generated
        Slots are individually lockable with Lock command */
    0x13, 0x00, 0x13, 0x00, /* KeyConfig 0, KeyConfig 1 */
    0x13, 0x00, 0x13, 0x00, /* KeyConfig 2, KeyConfig 3 */

    /*  AES Key */
    0x18, 0x00, 0x18, 0x00, /* KeyConfig 4, KeyConfig 5 -> not usable as AES keys! */

    /*  Private Key
        - Used for ECDH
        - Slot 7 will contain corresponding Master Secret */
    0x13, 0x00, 0x1F, 0x00, /* KeyConfig 6, KeyConfig 7 */

    /* SHA Key or other data */
    0x1C, 0x00, /* KeyConfig 8 */

    /* ECC Public Keys */
    0x10, 0x00, /* KeyConfig 9 */
    0x10, 0x00, 0x10, 0x00, /* KeyConfig 10, KeyConfig 11 */
    0x10, 0x00, 0x10, 0x00, /* KeyConfig 12, KeyConfig 13 */

    /* SHA Key or other data */
    0x1C, 0x00, 0x1C, 0x00  /* KeyConfig 14, KeyConfig 15 */
};

#endif /* ATECC608A_CONFIG_H */
