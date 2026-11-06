/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the GNRC shell commands.
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <stdio.h>
#include <stdint.h>

#include "container.h"
#include "fmt.h"
#include "msg.h"
#include "net/gnrc/pktbuf.h"
#include "netdev_mock.h"
#include "shell.h"


static int _pktbuf_add(int argc, char **argv)
{
    gnrc_pktsnip_t *pkt;
    ssize_t data_len;

    if (argc < 2) {
        printf("usage: %s <data hex>\n", argv[0]);
        return 1;
    }

    data_len = strlen(argv[1]) / 2;
    uint8_t data[data_len];

    if ((data_len = scn_buf_hex(data, data_len, argv[1], strlen(argv[1]))) < 0) {
        printf("error: Can not parse \"%s\" as hex string.\n", argv[1]);
        return 1;
    }

    pkt = gnrc_pktbuf_add(NULL, data, data_len, GNRC_NETTYPE_UNDEF);

    if (pkt) {
        printf("success: allocated packet at %p\n", pkt);
    }
    else {
        puts("error: unable to allocate packet");
        return 1;
    }

    return 0;
}

static int _pktbuf_rm(int argc, char **argv)
{
    uintptr_t addr = 0;
    ssize_t addr_len;
    size_t addr_hex_len, orig_hex_len;

    if (argc < 2) {
        printf("usage: %s <addr>\n", argv[0]);
        return 1;
    }

    addr_hex_len = strlen(argv[1]);
    orig_hex_len = addr_hex_len;

    if (addr_hex_len % 2) {
        addr_hex_len += 1;
    }

    addr_len = (size_t)addr_hex_len / 2;

    uint8_t addr_bytes[addr_len];
    char padded[addr_hex_len];

    memset(padded, '0', sizeof(padded));
    memcpy(&padded[addr_hex_len - orig_hex_len], argv[1], orig_hex_len);

    /* don't use `scn_u32_hex()` to be able to check for errors and to
     * support 64-bit addresses */
    if ((addr_len = scn_buf_hex(addr_bytes, addr_len, padded, addr_hex_len)) < 0) {
        printf("error: Can not parse address \"0x%s\".\n", argv[1]);
        return 1;
    }

    if (((size_t)addr_len) > sizeof(uintptr_t)) {
        printf("error: Address too \"0x%s\" large to be a memory address\n", argv[1]);
        return 1;
    }
    for (int i = 0; i < addr_len; i++) {
        addr |= (uint64_t)addr_bytes[i] << ((addr_len - i - 1) * 8);
    }
    gnrc_pktbuf_release((void *)addr);
    printf("success: removed packet allocated at %p\n", (void *)addr);
    return 0;
}

SHELL_COMMAND(pktbuf_add, "Add bytes to packet buffer", _pktbuf_add);
SHELL_COMMAND(pktbuf_remove, "Remove packet snip to packet buffer", _pktbuf_rm);

int main(void)
{
    static msg_t msg_queue[8];
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.gnrc and riotctrl_shell.netif test");

    msg_init_queue(msg_queue, ARRAY_SIZE(msg_queue));

    netdev_mock_init_eth();
    netdev_mock_init_ieee802154();
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
