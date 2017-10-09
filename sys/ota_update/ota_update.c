/**
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "net/gcoap.h"
#include "ota_update.h"
#include "net/gnrc/tftp.h"
#include "periph/flashpage.h"
#include "cpu_conf.h"
#include "firmware.h"
#include "xtimer.h"
#include "periph/pm.h"
#include "sema.h"

static kernel_pid_t ota_update_pid = KERNEL_PID_UNDEF;
static char _msg_stack[OTA_UPDATE_STACK];
static msg_t _ota_update_msg_queue[OTA_UPDATE_MSG_QUEUE_SIZE];

#ifdef MODULE_OTA_UPDATE_TFTP
static tftp_action_t _tftp_action;
#endif
static uint8_t buf[FLASHPAGE_SIZE];
static uint32_t buf_ptr = 0;
static uint32_t page_sum = 0;
static uint32_t slot_addr;
static uint16_t remote_version = 0;
static uint8_t remote_slot = 0;
static uint16_t local_version;
static uint32_t local_appid;
static ota_request_t ota_request;
static char update_name[FIRMWARE_NAME_LENGTH];
/* Counts requests sent by ota_update. */
static uint16_t req_count = 0;
static sema_t sema_fw_version = SEMA_CREATE_LOCKED();
static sema_t sema_fw_name = SEMA_CREATE_LOCKED();

/*
 * Response callback.
 */
static void _resp_handler(unsigned req_state, coap_pkt_t* pdu,
                          sock_udp_ep_t *remote)
{
    (void)remote;       /* not interested in the source currently */

    if (req_state == GCOAP_MEMO_TIMEOUT) {
        printf("[ota_update] timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (req_state == GCOAP_MEMO_ERR) {
        printf("[ota_update] error in response\n");
        return;
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("[ota_update] %s, response code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    if (pdu->payload_len) {
        char remote_version_str[10];
        printf(", %u bytes :%.*s\n", pdu->payload_len, pdu->payload_len,
                                                      (char *)pdu->payload);
        switch (ota_request) {
            case COAP_GET_VERSION:
                strncpy(remote_version_str, (char *)pdu->payload, pdu->payload_len);
                remote_version = strtol(remote_version_str, NULL, 16);
                printf("[ota_update] Got remote version %#x\n", remote_version);
                sema_post(&sema_fw_version);
                break;
            case COAP_GET_NAME:
                strncpy(update_name, (char *)pdu->payload, pdu->payload_len);
                printf("[ota_update] Got remote name %s\n", update_name);
                sema_post(&sema_fw_name);
                break;
        }
    }
    else {
        printf(", empty payload\n");
    }
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    size_t bytes_sent;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("[ota_update] unable to parse destination address");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("[ota_update] unable to parse destination port");
        return 0;
    }

    bytes_sent = gcoap_req_send2(buf, len, &remote, _resp_handler);
    if (bytes_sent > 0) {
        req_count++;
    }
    return bytes_sent;
}

static bool _save_to_page(uint8_t *data, size_t data_len)
{
    if ((buf_ptr + data_len) <= FLASHPAGE_SIZE) {
        memcpy(buf + buf_ptr, data, data_len);
        buf_ptr += data_len;
    }
    else {
        uint32_t rest = (buf_ptr + data_len) - FLASHPAGE_SIZE;
        size_t tmp_len = data_len - rest;
        memcpy(buf + buf_ptr, data, tmp_len);
        int err;
        err = flashpage_write_and_verify(page_sum, buf);
        if (err == FLASHPAGE_OK) {
            printf("[ota_update] Successfully written page %lu at %p\n",
                   page_sum, flashpage_addr(page_sum));
            page_sum++;
            memcpy(buf, data + tmp_len, rest);
            buf_ptr = rest;
        }
        else {
            printf("[ota_update] Flash program failed with error %d\n", err);
            return false;
        }
    }

    return true;
}

static int _write_last_page(uint32_t page, uint32_t slot_page)
{
    if (page != slot_page) {
        int err;
        err = flashpage_write_and_verify(page, buf);
        if (err == FLASHPAGE_OK) {
            printf("[ota_update] Successfully written page %lu at %p\n",
                               page, flashpage_addr(page));
            page_sum = slot_page;
            buf_ptr = 0;
            memset(buf, 0xFF, sizeof(buf));
            return true;
        } else {
            printf("[ota_update] Flash program failed with error %d\n", err);
            return false;
        }
    }

    return false;
}

#ifdef MODULE_OTA_UPDATE_TFTP
/* default server text which can be received */
static const char _tftp_client_hello[] = "Hello,\n"
                                         "\n"
                                         "Client text would also need to exist to be able to put data.\n"
                                         "\n"
                                         "Enjoy the RIOT-OS\n";

static int _tftp_client_data_cb(uint32_t offset, void *data, size_t data_len)
{
    char *c = (char *) data;

    /* we received a data block which we save on the corresponding slot */
    _save_to_page((uint8_t*)c, data_len);

    /* return the length of the data block */
    return data_len;
}

/**
 * @brief called at every transaction start
 */
static bool _tftp_client_start_cb(tftp_action_t action, tftp_mode_t mode,
                                  const char *file_name, size_t *len)
{
    /* translate the mode */
    const char *str_mode = "ascii";

    if (mode == TTM_OCTET) {
        str_mode = "bin";
    }
    else if (mode == TTM_MAIL) {
        str_mode = "mail";
    }

    /* translate the action */
    const char *str_action = "read";
    if (action == TFTP_WRITE) {
        str_action = "write";
    }

    /* display the action being performed */
    printf("tftp_client: %s %s %s:%lu\n", str_mode, str_action, file_name, (unsigned long)*len);

    /* return the length of the text, if this is an read action */
    if (action == TFTP_READ) {
        *len = sizeof(_tftp_client_hello);
    }

    /* remember the action of the current transfer */
    _tftp_action = action;

    /* we accept the transfer to take place so we return true */
    return true;
}

/**
 * @brief the transfer has stopped, see the event argument to determined if it was successful
 *        or not.
 */
static void _tftp_client_stop_cb(tftp_event_t event, const char *msg)
{
    /* decode the stop event received */
    const char *cause = "UNKOWN";

    if (event == TFTP_SUCCESS) {
        cause = "SUCCESS";
        puts("tftp_client: writing last page");
        _write_last_page(page_sum, slot_addr);
    }
    else if (event == TFTP_PEER_ERROR) {
        cause = "ERROR From Client";
    }
    else if (event == TFTP_INTERN_ERROR) {
        cause = "ERROR Internal Server Error";
    }

    /* print the transfer result to the console */
    if (msg != NULL) {
        printf("tftp_client: %s: %s\n", cause, msg);
    }
    else {
        printf("tftp_client: %s\n", cause);
    }
}

static void ota_update_tftp(uint8_t firmware_slot, char *name, char *server_ip_address)
{
    ipv6_addr_t ip;
    tftp_mode_t mode = TTM_OCTET;
    bool use_options = true;
    int ret;

    page_sum = firmware_get_slot_page(firmware_slot);

    puts("[ota_update] RIOT TFTP OTA update starting...");
    printf("[ota_update] Erasing slot %d...\n", firmware_slot);
    firmware_erase_int_image(firmware_slot);


    ipv6_addr_from_str(&ip, server_ip_address);
    _tftp_action = TFTP_READ;

    printf("[ota_update] Sending request for %s to the server %s\n",
           name,
           server_ip_address);

    ret = gnrc_tftp_client_read(&ip, name, mode, _tftp_client_data_cb,
                                _tftp_client_start_cb, _tftp_client_stop_cb,
                                use_options);

    if (ret > 0) {
        puts("[ota_update] Update finished, rebooting in 5 seconds...");
        xtimer_sleep(5);
        pm_reboot();
    } else {
        puts("[ota_update] Update failed!");
        printf("[ota_update] Error: %d\n", ret);
    }
}
#endif /* MODULE_OTA_UPDATE_TFTP */

static void *ota_update_start(void *arg)
{
    (void)arg;
    msg_init_queue(_ota_update_msg_queue, OTA_UPDATE_MSG_QUEUE_SIZE);

    /* Wait a bit to initialise network addresses */
    xtimer_sleep(5);

    coap_pkt_t pdu;
    firmware_metadata_t metadata;
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    char coap_resource[32];
    int local_slot = firmware_current_slot();

    memcpy(&metadata, firmware_get_metadata(local_slot), sizeof(firmware_metadata_t));

    if (firmware_validate_metadata_checksum(&metadata) != -1) {
        local_appid = metadata.appid;
        local_version = metadata.version;
        firmware_metadata_print(&metadata);
    }
    else {
        puts("[ota_update] Error! Cannot retrieve local metadata\n");
        return 0;
    }

    while(1) {
        size_t len;
        ota_request = 0;
        memset(coap_resource, 0, sizeof(coap_resource));
        sprintf(coap_resource, "/0x%lX/version", local_appid);

        printf("[ota_update] Requesting resource %s\n", coap_resource);
        len = gcoap_request(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET,
                            coap_resource);
        printf("[ota_update] sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
               (unsigned) len);
        if (!_send(&buf[0], len, OTA_SERVER_ADDRESS, OTA_SERVER_COAP_PORT)) {
            puts("[ota_update] msg send failed");
        }

        if (sema_wait_timed(&sema_fw_version, OTA_REQ_TIMEOUT)
            == -ETIMEDOUT) {
            printf("[ota_update] Request %u timed out\n", coap_get_id(&pdu));
        }
        else {

            if (local_version >= remote_version) {
                puts("[ota_update] No new firmware available");
            }
            else {
                if (local_slot == 1) {
                    remote_slot = 2;
                }
                else if (local_slot == 2) {
                    remote_slot = 1;
                }
                ota_request = 1;
                memset(coap_resource, 0, sizeof(coap_resource));
                sprintf(coap_resource, "/0x%lX/slot%u/name", local_appid, remote_slot);
                printf("[ota_update] Requesting resource %s\n", coap_resource);
                len = gcoap_request(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET,
                                    coap_resource);
                printf("[ota_update] sending msg ID %u, %u bytes\n", coap_get_id(&pdu),
                       (unsigned) len);
                if (!_send(&buf[0], len, OTA_SERVER_ADDRESS, OTA_SERVER_COAP_PORT)) {
                    puts("[ota_update] msg send failed");
                }

                if (sema_wait_timed(&sema_fw_name, OTA_REQ_TIMEOUT)
                    == -ETIMEDOUT) {
                    printf("[ota_update] Request %u timed out\n", coap_get_id(&pdu));
                }
                else {
#ifdef MODULE_OTA_UPDATE_TFTP
                    ota_update_tftp(remote_slot, update_name, OTA_SERVER_ADDRESS);
#endif
                }
            }
        }

        xtimer_sleep(OTA_PERIODIC_REQ_TIME);
    }

    /* Should not happen */
    return 0;
}

kernel_pid_t ota_update_init(void)
{
    if (ota_update_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }

    ota_update_pid = thread_create(_msg_stack, sizeof(_msg_stack), THREAD_PRIORITY_MAIN - 1,
                                   THREAD_CREATE_STACKTEST, ota_update_start, NULL, "ota_update");

    return ota_update_pid;
}
