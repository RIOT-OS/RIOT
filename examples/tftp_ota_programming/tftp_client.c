/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Basic example showing how to download a new firmware to
 *              internal flash on slot 2
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "net/gnrc/tftp.h"
#include "periph/flashpage.h"

static const char *_tftp_default_host = "::1";
static uint8_t buf[FLASHPAGE_SIZE];
static uint32_t buf_ptr = 0;
static uint32_t page2 = FW_SLOT_2_PAGE;

static bool tftp_save_to_slot2(uint8_t *data, size_t data_len)
{
    if ((buf_ptr + data_len) <= FLASHPAGE_SIZE) {
        memcpy(buf + buf_ptr, data, data_len);
        buf_ptr += data_len;
    } else {
        uint32_t rest = (buf_ptr + data_len) - FLASHPAGE_SIZE;
        size_t tmp_len = data_len - rest;
        memcpy(buf + buf_ptr, data, tmp_len);
        int err;
        err = flashpage_write_and_verify(page2, buf);
        if (err == FLASHPAGE_OK) {
            printf("Successfully written page %lu at %p\n",
                   page2, flashpage_addr(page2));
            page2++;
            memcpy(buf, data + tmp_len, rest);
            buf_ptr = rest;
        } else {
            printf("Flash program failed with error %d\n", err);
            return false;
        }
    }

    return true;
}

static int write_last_page(uint32_t page, uint32_t slot_page)
{
    int err;

    if (page != slot_page) {
        err = flashpage_write_and_verify(page, buf);
        if (err == FLASHPAGE_OK) {
            printf("Successfully written page %lu at %p\n",
                               page, flashpage_addr(page));
            page2 = slot_page;
            buf_ptr = 0;
            memset(buf, 0xFF, sizeof(buf));
            return true;
        } else {
            printf("Flash program failed with error %d\n", err);
            return false;
        }
    }

    return false;
}

/* default server text which can be received */
static const char _tftp_client_hello[] = "Hello,\n"
                                         "\n"
                                         "Client text would also need to exist to be able to put data.\n"
                                         "\n"
                                         "Enjoy the RIOT-OS\n";

static tftp_action_t _tftp_action;

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
 * @brief called to get or put data, depending on the mode received by `_tftp_start_cb(action, ...)`
 */
static int _tftp_client_data_cb(uint32_t offset, void *data, size_t data_len)
{
    char *c = (char *) data;

    /* if we are reading return the part of the data that is being requested */
    if (_tftp_action == TFTP_WRITE) {
        /* calculate the length of the data block to transfer */
        if (offset + data_len > sizeof(_tftp_client_hello)) {
            data_len -= (offset + data_len) - sizeof(_tftp_client_hello);
        }

        /* copy the block to the output buffer */
        memcpy(data, _tftp_client_hello + offset, data_len);
    }
    else {
        /* we received a data block which we save on slot 2 */
        tftp_save_to_slot2((uint8_t*)c, data_len);
    }

    /* return the length of the data block */
    return data_len;
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
        write_last_page(page2, FW_SLOT_2);
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

static int _tftp_client_cmd(int argc, char * *argv)
{
    ipv6_addr_t ip;
    const char *file_name = argv[2];
    tftp_mode_t mode = TTM_OCTET;
    bool use_options = true;

    ipv6_addr_from_str(&ip, _tftp_default_host);

    if (argc >= 3 && argc <= 6) {
        /* decode the action */
        if (strcmp(argv[1], "get") == 0) {
            _tftp_action = TFTP_READ;
        }
        else if (strcmp(argv[1], "put") == 0) {
            _tftp_action = TFTP_WRITE;
        }
        else {
            return -1;
        }

        /* get the transfer mode */
        if (argc >= 4) {
            if (strcmp(argv[3], "octet") == 0) {
                mode = TTM_OCTET;
            }
            else if (strcmp(argv[3], "ascii") == 0) {
                mode = TTM_ASCII;
            }
            else if (strcmp(argv[3], "mail") == 0) {
                mode = TTM_MAIL;
            }
            else {
                puts("tftp: couldn't parse the TFTP transfer mode");
                return -1;
            }
        }

        /* decode if we must use the TFTP option extension or not */
        if (argc >= 5) {
            if (strcmp(argv[4], "0") == 0) {
                use_options = false;
            }
            else if (strcmp(argv[4], "1") == 0) {
                use_options = true;
            }
            else {
                puts("tftp: invalid options choose 0 or 1");
                return -1;
            }
        }

        /* decode the address */
        if (argc >= 6) {
            if (!ipv6_addr_from_str(&ip, argv[5])) {
                puts("tftp: invalid IP address");
                return -1;
            }
        }
    }
    else {
        return -1;
    }

    if (_tftp_action == TFTP_READ) {
        puts("tftp: starting read request");

        gnrc_tftp_client_read(&ip, file_name, mode, _tftp_client_data_cb,
                              _tftp_client_start_cb, _tftp_client_stop_cb, use_options);
    }
    else if (_tftp_action == TFTP_WRITE) {
        puts("tftp: starting write request");

        gnrc_tftp_client_write(&ip, file_name, mode, _tftp_client_data_cb,
                               sizeof(_tftp_client_hello), _tftp_client_stop_cb, use_options);
    }

    return 0;
}

/**
 * @brief start the TFTP server by creating a thread
 */
int tftp_client_cmd(int argc, char * *argv)
{
    if (_tftp_client_cmd(argc, argv) < 0) {
        printf("usage: %s <action[get|put]> <file_name> <mode[ascii | octet(default) | mail]>\n"
               "\t<use_options[1 (default) | 0]> <addr(default: ::1)>\n", argv[0]);
    }

    return 0;
}
