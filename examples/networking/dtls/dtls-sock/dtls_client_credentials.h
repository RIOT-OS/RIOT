/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       PSK and RPK client credentials for the dtls-sock example.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de
 *
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pre-Shared Key (PSK) credentials (client and server must have a copy)
 */

/**
 * @brief The identity of the PSK 0
 */
#define CLIENT_PSK_IDENTITY_0 "Identity_0"

/**
 * @brief The PSK 0 itself.
 */
#define CLIENT_PSK_IDENTITY_0_KEY "secretPSK"

/**
 * @brief The identity hint of the PSK 0
 */
#define CLIENT_PSK_IDENTITY_0_HINT "Prefer_Id_0"

/**
 * @brief The identity of the PSK 1
 */
#define CLIENT_PSK_IDENTITY_1 "Identity_1"

/**
 * @brief The PSK 1 itself.
 */
#define CLIENT_PSK_IDENTITY_1_KEY "yetAnotherSecret"

/**
 * @brief The identity hint of the PSK 1
 */
#define CLIENT_PSK_IDENTITY_1_HINT "Prefer_Id_1"

/*
 * Raw Public Key (RPK) credentials
 */
/**
 * @brief The private part of the client key 0.
 */
static const unsigned char client_private_key_0[] = {
    0x41, 0xC1, 0xCB, 0x6B, 0x51, 0x24, 0x7A, 0x14,
    0x43, 0x21, 0x43, 0x5B, 0x7A, 0x80, 0xE7, 0x14,
    0x89, 0x6A, 0x33, 0xBB, 0xAD, 0x72, 0x94, 0xCA,
    0x40, 0x14, 0x55, 0xA1, 0x94, 0xA9, 0x49, 0xFA
};

/**
 * @brief The x coordinate of the public part of the client key 0.
 */
static const unsigned char client_public_key_0_x[] = {
    0x36, 0xDF, 0xE2, 0xC6, 0xF9, 0xF2, 0xED, 0x29,
    0xDA, 0x0A, 0x9A, 0x8F, 0x62, 0x68, 0x4E, 0x91,
    0x63, 0x75, 0xBA, 0x10, 0x30, 0x0C, 0x28, 0xC5,
    0xE4, 0x7C, 0xFB, 0xF2, 0x5F, 0xA5, 0x8F, 0x52
};

/**
 * @brief The y coordinate of the public part of the client key 0.
 */
static const unsigned char client_public_key_0_y[] = {
    0x71, 0xA0, 0xD4, 0xFC, 0xDE, 0x1A, 0xB8, 0x78,
    0x5A, 0x3C, 0x78, 0x69, 0x35, 0xA7, 0xCF, 0xAB,
    0xE9, 0x3F, 0x98, 0x72, 0x09, 0xDA, 0xED, 0x0B,
    0x4F, 0xAB, 0xC3, 0x6F, 0xC7, 0x72, 0xF8, 0x29
};

/**
 * @brief The x coordinate of the public part of the server key 0.
 */
static const unsigned char known_server_public_key_0_x[] = {
    0xef, 0x30, 0x0c, 0x57, 0x0a, 0x65, 0x7b, 0x98,
    0x53, 0x34, 0xfa, 0x52, 0x81, 0xd7, 0x9b, 0x72,
    0xb4, 0xe9, 0x48, 0xf0, 0x56, 0x37, 0xd0, 0x53,
    0xa7, 0x35, 0x61, 0x3c, 0x06, 0xfb, 0x9c, 0xe7,
};

/**
 * @brief The y coordinate of the public part of the server key 0.
 */
static const unsigned char known_server_public_key_0_y[] = {
    0xd1, 0x98, 0xe6, 0x0a, 0x96, 0x41, 0xc7, 0x8a,
    0xac, 0x69, 0x09, 0x47, 0x64, 0x24, 0x30, 0x5f,
    0x1b, 0x70, 0xad, 0x2c, 0xf0, 0xba, 0xa2, 0xd7,
    0xdb, 0x6d, 0x11, 0xe0, 0x36, 0xb9, 0x1e, 0x87
};

/**
 * @brief The x coordinate of the public part of the server key 1.
 */
static const unsigned char known_server_public_key_1_x[] = {
    0x54, 0xe4, 0x3a, 0xa6, 0xe1, 0x19, 0xaf, 0x85,
    0xe4, 0x50, 0x0b, 0x67, 0x89, 0x57, 0x02, 0x82,
    0xad, 0x68, 0x90, 0xe1, 0xfb, 0xd0, 0x2a, 0x2b,
    0xc0, 0xfc, 0xc2, 0xeb, 0xdc, 0x48, 0x7f, 0x92
};

/**
 * @brief The y coordinate of the public part of the server key 1.
 */
static const unsigned char known_server_public_key_1_y[] = {
    0x25, 0x5f, 0x92, 0x94, 0x38, 0x73, 0xb4, 0xfd,
    0x17, 0x9e, 0xe0, 0x7f, 0x12, 0x93, 0xb9, 0xf4,
    0xa5, 0x70, 0x4e, 0xea, 0x09, 0x5f, 0xdf, 0xc1,
    0x8a, 0x66, 0x75, 0x86, 0xc3, 0x34, 0x39, 0x8b
};

#ifdef __cplusplus
}
#endif
