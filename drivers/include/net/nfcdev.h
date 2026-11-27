/*
 * SPDX-FileCopyrightText: 2025-2026 Nico Behrens <nico@nico-behrens.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <net/nfc/nfc.h>
#include <net/nfc/nfc_a.h>
#include <net/nfc/nfc_b.h>
#include <net/nfc/nfc_f.h>
#include <net/nfc/nfc_v.h>

typedef enum {
    NFCDEV_STATE_UNINITIALIZED = 0,
    NFCDEV_STATE_DISABLED,
    NFCDEV_STATE_IDLE,
    NFCDEV_STATE_POLLING,
    NFCDEV_STATE_LISTENING,
    NFCDEV_STATE_NFC_DEP_CONNECTED,
} nfcdev_state_t;

typedef enum {
    NFCDEV_MODE_TARGET = 0,
    NFCDEV_MODE_INITIATOR,
} nfcdev_mode_t;

struct nfcdev;

typedef struct {
    nfc_technology_t technology;
    union {
        nfc_a_listener_config_t a;
        nfc_b_listener_config_t b;
        nfc_f_listener_config_t f;
        nfc_v_listener_config_t v;
    } config;
}  nfc_listener_config_t;

typedef struct {
    int (*init)(struct nfcdev *nfcdev, const void *dev_config);

    int (*poll) (struct nfcdev *nfcdev, nfc_listener_config_t *config);

    int (*poll_a)(struct nfcdev *nfcdev,  nfc_a_listener_config_t *config);
    int (*listen_a)(struct nfcdev *nfcdev, const nfc_a_listener_config_t *config);

    int (*poll_b)(struct nfcdev *nfcdev, nfc_b_listener_config_t *config);
    int (*listen_b)(struct nfcdev *nfcdev, const nfc_b_listener_config_t *config);

    int (*poll_f)(struct nfcdev *nfcdev, nfc_f_listener_config_t *config);
    int (*listen_f)(struct nfcdev *nfcdev, const nfc_f_listener_config_t *config);

    int (*poll_v)(struct nfcdev *nfcdev, nfc_v_listener_config_t *config);
    int (*listen_v)(struct nfcdev *nfcdev, const nfc_v_listener_config_t *config);

    int (*poll_dep) (struct nfcdev *nfcdev, nfc_baudrate_t baudrate);
    int (*listen_dep) (struct nfcdev *nfcdev, nfc_baudrate_t baudrate);

    int (*target_send_data)(struct nfcdev *nfcdev, const uint8_t *send, size_t send_len);

    int (*target_receive_data)(struct nfcdev *nfcdev, uint8_t *recv, size_t *recv_len);

    int (*initiator_exchange_data)(struct nfcdev *nfcdev, const uint8_t *send, size_t send_len,
        uint8_t *recv, size_t *recv_len);

    int (*mifare_classic_authenticate) (struct nfcdev *nfcdev, uint8_t block_number,
        const nfc_a_nfcid1_t *nfcid1, bool use_key_a,
        const uint8_t *key);
} nfcdev_ops_t;

typedef struct nfcdev {
    void *dev; /**< Pointer to the device specific structure */
    void *config; /**< Pointer to the device specific configuration structure */
    const nfcdev_ops_t *ops; /**< Pointer to the device operations */
    nfcdev_state_t state; /**< Current state of the NFC device */
} nfcdev_t;


