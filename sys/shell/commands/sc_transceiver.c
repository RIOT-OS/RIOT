/*
 * Copyright (C) 2013 Ludwig Ortmann.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides shell commands to configure the transceiver
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef MODULE_NET_IF
#include "net_if.h"
#endif
#include "transceiver.h"
#include "msg.h"

#if   defined( MODULE_CC110X_LEGACY_CSMA )
#include "cc110x_legacy_csma.h"
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC1100

#elif defined( MODULE_CC110X_LEGACY )
#include "cc110x_legacy.h"
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC1100

#elif defined( MODULE_CC110X )
#include "cc110x.h"
#define TEXT_SIZE           CC1100_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC1100

#elif defined( MODULE_CC2420 )
#include "cc2420.h"
#include "ieee802154_frame.h"
#define TEXT_SIZE           CC2420_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_CC2420

#elif defined( MODULE_AT86RF231 )
#include "at86rf231.h"
#include "ieee802154_frame.h"
#define TEXT_SIZE           AT86RF231_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_AT86RF231

#elif defined( MODULE_NATIVENET )
#include "nativenet.h"
#define TEXT_SIZE           NATIVE_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_NATIVE

#elif defined( MODULE_MC1322X )
#include "mc1322x.h"
#include "maca.h"
#include "maca_packet.h"
#include "ieee802154_frame.h"
#define TEXT_SIZE           MACA_MAX_PAYLOAD_SIZE
#define _TC_TYPE            TRANSCEIVER_MC1322X

#elif defined( MODULE_AT86RF212B )
#include "at86rf212b.h"
#include "ieee802154_frame.h"
#define TEXT_SIZE           AT86RF212B_MAX_DATA_LENGTH
#define _TC_TYPE            TRANSCEIVER_AT86RF212B

#endif

/* checked for type safety */
void _transceiver_get_set_address_handler(int argc, char **argv)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    radio_address_t a;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;

    if (argc > 1) {
        a = atoi(argv[1]);
        printf("[transceiver] trying to set address %" PRIu16 "\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[transceiver] got address: %" PRIu16 "\n", a);
}

#ifndef MODULE_NET_IF
uint8_t hex_to_dec(char c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    else if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 55);
    }
    else if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 87);
    }
    else {
        return 0xff;
    }
}

uint64_t _str_to_eui64(const char *eui64_str)
{
    int i;
    const char *eui64_rev = &eui64_str[strlen(eui64_str) - 1];
    uint64_t eui64 = 0;

    for (i = 7; i >= 0 || eui64_rev >= eui64_str; i--) {
        uint8_t digit;
        eui64 <<= 8;

        while ((digit = hex_to_dec(*eui64_rev)) == 0xFF) {
            if (--eui64_rev < eui64_str) {
                return eui64;
            }
        }

        eui64 = digit;
        eui64_rev--;

        while ((digit = hex_to_dec(*eui64_rev)) == 0xFF) {
            if (--eui64_rev < eui64_str) {
                return eui64;
            }
        }

        eui64 |= digit << 4;
        eui64_rev--;
    }

    return eui64;
}
#endif

/* checked for type safety */
void _transceiver_get_set_long_addr_handler(int argc, char **argv)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    transceiver_eui64_t a;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &a;
    mesg.content.ptr = (char *) &tcmd;

    if (argc > 1) {
#ifdef MODULE_NET_IF
        net_if_eui64_t eui64;
        net_if_hex_to_eui64(&eui64, argv[1]);
        a = eui64.uint64;
#else
        a = _str_to_eui64(argv[1]);
#endif
        printf("[transceiver] trying to set EUI-64 %016"PRIx64"\n", a);
        mesg.type = SET_LONG_ADDR;
    }
    else {
        mesg.type = GET_LONG_ADDR;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("[transceiver] got EUI-64: %016"PRIx64"\n", a);
}


/* checked for type safety */
void _transceiver_get_set_channel_handler(int argc, char **argv)
{
    msg_t mesg;
    transceiver_command_t tcmd;
    int32_t c;

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }

    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &c;
    mesg.content.ptr = (char *) &tcmd;

    if (argc > 1) {
        c = atoi(argv[1]);
        printf("[transceiver] Trying to set channel %" PRIi32 "\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    if (c == -1) {
        puts("[transceiver] Error setting/getting channel");
    }
    else {
        printf("[transceiver] Got channel: %" PRIi32 "\n", c);
    }
}

void _transceiver_send_handler(int argc, char **argv)
{
    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }
    if (argc < 3) {
        printf("Usage:\t%s <ADDR> <MSG> [PAN]\n", argv[0]);
        return;
    }

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X || MODULE_AT86RF212B
    ieee802154_packet_t p;
    uint16_t short_addr;
#else
    radio_packet_t p;
#endif

    transceiver_command_t tcmd;
    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &p;

    char text_msg[TEXT_SIZE];
    memset(text_msg, 0, TEXT_SIZE);
    strcpy(text_msg, argv[2]);

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X || MODULE_AT86RF212B
    memset(&p, 0, sizeof(ieee802154_packet_t));
    p.frame.payload = (uint8_t*) text_msg;
    p.frame.payload_len = strlen(text_msg) + 1;
    p.frame.fcf.frame_type = IEEE_802154_DATA_FRAME;
    p.frame.fcf.dest_addr_m = IEEE_802154_SHORT_ADDR_M;
    p.frame.fcf.src_addr_m = IEEE_802154_SHORT_ADDR_M;
    short_addr = atoi(argv[1]);
    p.frame.dest_addr[1] = (short_addr&0xff);
    p.frame.dest_addr[0] = (short_addr>>8);
    if (argc == 4) {
        p.frame.dest_pan_id = atoi(argv[3]);
    }
    else {
        p.frame.dest_pan_id = IEEE_802154_DEFAULT_PAN_ID;
    }
#else
    p.data = (uint8_t *) text_msg;
    p.length = strlen(text_msg) + 1;
    p.dst = atoi(argv[1]);
#endif

    msg_t mesg;
    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X || MODULE_AT86RF212B
    printf("[transceiver] Sending packet of length %" PRIu16 " to %" PRIu16 ": %s\n", p.frame.payload_len, p.frame.dest_addr[1], (char*) p.frame.payload);
#else
    printf("[transceiver] Sending packet of length %" PRIu16 " to %" PRIu16 ": %s\n", p.length, p.dst, (char*) p.data);
#endif
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    int8_t response = mesg.content.value;
    printf("[transceiver] Packet sent: %" PRIi8 "\n", response);
}

/* checked for type safety */
void _transceiver_monitor_handler(int argc, char **argv)
{
    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }
    else if (argc != 2) {
        printf("Usage:\n%s <MODE>\n", argv[0]);
        return;
    }

    uint8_t m = atoi(argv[1]);
    printf("Setting monitor mode: %" PRIu8 "\n", m);

    transceiver_command_t tcmd;
    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &m;

    msg_t mesg;
    mesg.content.ptr = (char *) &tcmd;
    mesg.type = SET_MONITOR;

    msg_send(&mesg, transceiver_pid);
}

/* checked for type safety */
void _transceiver_get_set_pan_handler(int argc, char **argv)
{
    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }

    int32_t p;

    transceiver_command_t tcmd;
    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &p;

    msg_t mesg;
    mesg.content.ptr = (char*) &tcmd;
    if (argc > 1) {
        p = atoi(argv[1]);
        printf("[transceiver] Trying to set pan %" PRIi32 "\n", p);
        mesg.type = SET_PAN;
    }
    else {
        mesg.type = GET_PAN;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    if (p == -1) {
        puts("[transceiver] Error setting/getting pan");
    }
    else {
        printf("[transceiver] Got pan: %" PRIi32 "\n", p);
    }
}

/* checked for type safety */
#ifdef DBG_IGNORE
void _transceiver_set_ignore_handler(int argc, char **argv)
{

    if (transceiver_pid == KERNEL_PID_UNDEF) {
        puts("Transceiver not initialized");
        return;
    }
    else if (argc < 2) {
        printf("[transceiver] Ignored link layer addresses: ");
        for (uint8_t i = 0; i < TRANSCEIVER_MAX_IGNORED_ADDR; i++) {
            if (transceiver_ignored_addr[i]) {
                printf("%" PRIu16 " ", transceiver_ignored_addr[i]);
            }
        }
        puts("");
        return;
    }
    else if (argc > 2) {
        printf("Usage:\n%s <address>\n", argv[0]);
        return;
    }

    radio_address_t a;

    transceiver_command_t tcmd;
    tcmd.transceivers = _TC_TYPE;
    tcmd.data = &a;

    msg_t mesg;
    mesg.content.ptr = (char*) &tcmd;

    a = atoi(argv[1]);
    printf("[transceiver] trying to add address %" PRIu16 " to the ignore list \n", a);
    mesg.type = DBG_IGN;
    msg_send_receive(&mesg, &mesg, transceiver_pid);

    int16_t response = a;
    if (response == -1) {
        printf("Error: ignore list full\n");
    }
    else {
        printf("Success (added at index %" PRIi16 ").\n", response);
    }
}
#endif
