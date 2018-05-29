/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_firmware_manifest SUIT manifest driven firmware updates
 * @ingroup     sys_firmware
 * @brief       SUIT manifest driven firmware updates
 *
 * @{
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef FIRMWARE_MANIFEST_H
#define FIRMWARE_MANIFEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "suit.h"
#include "mutex.h"
#include "thread.h"
#include "memarray.h"
#include "cn-cbor/cn-cbor.h"
#include "firmware/flashwrite.h"
#include "net/sock.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include <stdlib.h>
#include <stdint.h>

#define MANIFEST_BUF_SIZE   (512U)
#define COAP_MSG_BUF        (256U)
#define OTA_MANIFEST_BLOCKS         80

#define OTA_MANIFEST_MSG_MANIFEST   0x01
#define OTA_MANIFEST_MSG_START      0x02
#define OTA_MANIFEST_MSG_NEXT       0x03
#define OTA_MANIFEST_MSG_FINAL      0x04

#define OTA_URL_PATH_LEN            SOCK_URLPATH_MAXLEN

typedef struct {
    uint8_t mbuf[MANIFEST_BUF_SIZE];
    char path[OTA_URL_PATH_LEN];
    sock_udp_ep_t remote;
    size_t mbuf_len;
    mutex_t lock;
    kernel_pid_t pid;
    cn_cbor blocks[OTA_MANIFEST_BLOCKS];
    memarray_t storage;
    suit_manifest_t manifest;
    uint8_t coap_buf[COAP_MSG_BUF];
    firmware_flashwrite_t writer;
} firmware_manifest_t;

void firmware_manifest_run(void);
int firmware_manifest_putbytes(uint8_t *buf, size_t len, size_t offset, bool more);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* FIRMWARE_MANIFEST_H */
