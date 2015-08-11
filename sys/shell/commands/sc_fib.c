/*
 * Copyright (C) 2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
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
 * @brief       Provides shell commands to manage and show FIB Entries
 *
 * @author      2015 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "thread.h"
#ifdef MODULE_NG_NETIF
#include "net/ng_netif.h"
#endif
#include "net/fib.h"

#define INFO1_TXT "fibroute add <destination> via <next hop> [dev <device>]"
#define INFO2_TXT " [lifetime <lifetime>]"
#define INFO3_TXT "       <destination> - the destination address\n" \
                  "       <next hop>    - the address of the next-hop towards the <destination>\n" \
                  "       <device>      - the device id of the Interface to use." \
                  " Optional if only one interface is available.\n"
#define INFO4_TXT "       <lifetime>    - optional lifetime in ms when the entry automatically invalidates\n"
#define INFO5_TXT "fibroute del <destination>\n" \
                  "       <destination> - the destination address of the entry to be deleted\n"

/** @brief buffer for the converted destination IP address */
static unsigned char tmp_dst_addr_buf[FIB_DESTINATION_SIZE_SUBSTITUTE];

/** @brief buffer for the converted next-hop IP address */
static unsigned char tmp_nxt_addr_buf[FIB_DESTINATION_SIZE_SUBSTITUTE];

/** @brief the size of an IPv6 address in bytes */
#define FIB_IPV6_ADDRSZ (16)

/** @brief the size of an IPv4 address in bytes */
#define FIB_IPV4_ADDRSZ (4)

/**
* @brief tries to convert a given string to the number representation of an
* either IPv4 or IPv6 address.
* @param [in] addr_str pointer to the address string
* @param [in] addr_str_size the length of the passed address string
* @param [out] out_buf the buffer to store the parsed address
* @param [in, out] out_buf_size the size of the buffer in bytes,
*                  the actau used size is stored here after Success
* @return 0 on succesfull IPv6 address conversion
* @return 1 on succesfull IPv4 address conversion
* @return 2 when no conversion to IPv4 or IPv6 is possible
* @return negative value on errors
*/
static int _fib_custom_pton(const char *addr_str, size_t addr_str_size,
                            unsigned char *out_buf, size_t *out_buf_size)
{
    size_t last_token = 0;
    size_t iter_out = 0;
    char zeros = 0;
    int iter_in = 0;

    char addr_token = ' '; // ':' == ipv6, '.' == ipv4
    for (size_t i = 0; i < addr_str_size; ++i) {

        iter_in = (zeros == 0) ? i : iter_in - 1;

        if ((addr_str[iter_in] == ':') || (addr_str[iter_in] == '.') || (i == addr_str_size - 1)) {
            if (addr_token == ' ') {
                addr_token = addr_str[iter_in];
            }

            if (addr_token != addr_str[iter_in] && !(i == addr_str_size - 1)) {
                return -1;
            }

            if (addr_token == ':' && (iter_in - last_token) == 0) {
                if (*out_buf_size < FIB_IPV6_ADDRSZ) {
                    *out_buf_size = FIB_IPV6_ADDRSZ;
                    return -3;
                }
                zeros = 1;
                iter_in = addr_str_size - 1;
                iter_out = FIB_IPV6_ADDRSZ - 1;
                last_token = iter_in;
                continue;
            }

            char tmp_buf[] = {0, 0, 0, 0};
            unsigned int val = 0;

            if (addr_token == ':') {
                if (zeros == 0) {
                    memcpy(tmp_buf, &addr_str[last_token], (iter_in - last_token));
                    sscanf(tmp_buf, "%x", &val);
                    out_buf[iter_out++] = (unsigned char)((val >> 8) & 0xff);
                    out_buf[iter_out++] = (unsigned char)(val & 0xff);
                }
                else {
                    if ((last_token - iter_in + 1) > 0) {
                        memcpy(tmp_buf, &addr_str[iter_in + 1], (last_token - iter_in + 1));
                        sscanf(tmp_buf, "%x", &val);
                        out_buf[iter_out--] = (unsigned char)(val & 0xff);
                        out_buf[iter_out--] = (unsigned char)((val >> 8) & 0xff);
                    }
                }
            }
            else if (addr_token == '.') {
                memcpy(tmp_buf, &addr_str[last_token], (iter_in - last_token));
                sscanf(tmp_buf, "%d", &val);
                out_buf[iter_out++] = (unsigned char)(val & 0xff);
            }

            last_token = (zeros == 0) ? iter_in + 1 : iter_in;
        }
    }

    if (addr_token != ':' && addr_token != '.') {
        return 2;
    }

    *out_buf_size = (addr_token == ':') ? FIB_IPV6_ADDRSZ : FIB_IPV4_ADDRSZ;
    return (addr_token == ':') ? 0 : 1;
}

static void _fib_usage(int info)
{
    switch (info) {
        case 1: {
            puts("\nbrief: adds a new entry to the FIB.\nusage: "
                 INFO1_TXT "\n" INFO3_TXT);
            break;
        }

        case 2: {
            puts("\nbrief: adds a new entry to the FIB.\nusage: "
                 INFO1_TXT INFO2_TXT "\n" INFO3_TXT INFO4_TXT);
            break;
        }

        case 3: {
            puts("\nbrief: deletes an entry from the FIB.\nusage: "
                 INFO5_TXT);
            break;
        }

        default:
            break;
    };
}

static void _fib_add(const char *dest, const char *next, kernel_pid_t pid, uint32_t lifetime)
{
    unsigned char *dst = (unsigned char *)tmp_dst_addr_buf;
    uint32_t dst_flags = 0xffff;
    unsigned char *nxt = (unsigned char *)tmp_nxt_addr_buf;
    uint32_t nxt_flags = 0xffff;

    size_t buf_dst_size = FIB_DESTINATION_SIZE_SUBSTITUTE;
    size_t buf_nxt_size = FIB_DESTINATION_SIZE_SUBSTITUTE;

    memset(tmp_dst_addr_buf, 0, FIB_DESTINATION_SIZE_SUBSTITUTE);
    memset(tmp_nxt_addr_buf, 0, FIB_DESTINATION_SIZE_SUBSTITUTE);

    int ret = _fib_custom_pton(dest, strlen(dest), tmp_dst_addr_buf, &buf_dst_size);
    if (ret == 0) {
        /* @brief magic number to flag the address as AF_INET6
         *        taken from socket.h:84
        */
        dst_flags = 28;
    }
    else if (ret == 1) {
        /* @brief magic number to flag the address as AF_INET
         *        taken from socket.h:49
        */
        dst_flags = 2;
    }
    else {
        puts("\nunrecognized destination address type. (We use the string as address.)");
        dst = (unsigned char *)dest;
        buf_dst_size = strlen(dest);
    }

    ret = _fib_custom_pton(next, strlen(next), tmp_nxt_addr_buf, &buf_nxt_size);
    if (ret == 0) {
        /* @brief magic number to flag the address as AF_INET6
         *        taken from socket.h:84
        */
        nxt_flags = 28;
    }
    else if (ret == 1) {
        /* @brief magic number to flag the address as AF_INET
         *        taken from socket.h:49
        */
        nxt_flags = 4;
    }
    else {
        puts("\nunrecognized next-hop address type. (We use the string as address.)");
        nxt = (unsigned char *)next;
        buf_dst_size = strlen(next);
    }

    fib_add_entry(pid, dst, buf_dst_size, dst_flags, nxt, buf_nxt_size, nxt_flags, lifetime);
}

int _fib_route_handler(int argc, char **argv)
{
    /* e.g. fibroute right now dont care about the adress/protocol family */
    if (argc == 1) {
        fib_print_routes();
        return 0;
    }

    /* e.g. firoute [add|del] */
    if (argc == 2) {
        if ((strcmp("add", argv[1]) == 0)) {
            _fib_usage(2);
        }
        else if ((strcmp("del", argv[1]) == 0)) {
            _fib_usage(3);
        }
        else {
            puts("\nunrecognized parameter1.\nPlease enter fibroute [add|del] for more information.");
        }

        return 1;
    }

    if (argc > 2 && !((strcmp("add", argv[1]) == 0) || (strcmp("del", argv[1]) == 0))) {
        puts("\nunrecognized parameter2.\nPlease enter fibroute [add|del] for more information.");
        return 1;
    }

    size_t buf_dst_size = FIB_DESTINATION_SIZE_SUBSTITUTE;
    memset(tmp_dst_addr_buf, 0, FIB_DESTINATION_SIZE_SUBSTITUTE);

    /* e.g. fibroute del <destination> */
    if (argc == 3) {
        int ret = _fib_custom_pton(argv[2], strlen(argv[2]), tmp_dst_addr_buf, &buf_dst_size);
        if ((ret == 0) || (ret == 1)) {
            fib_remove_entry(tmp_dst_addr_buf, buf_dst_size);
        }
        else {
            fib_remove_entry((uint8_t *)argv[2], (strlen(argv[2])));
        }

        return 0;
    }

#ifdef MODULE_NG_NETIF
    /* e.g. fibroute add <destination> via <next hop> */
    if ((argc == 5) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)) {
        kernel_pid_t ifs[NG_NETIF_NUMOF];
        size_t ifnum = ng_netif_get(ifs);
        if (ifnum == 1) {
            _fib_add(argv[2], argv[4], ifs[0], FIB_LIFETIME_NO_EXPIRE);
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. fibroute add <destination> via <next hop> lifetime <lifetime> */
    if ((argc == 7) && (strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
        && (strcmp("lifetime", argv[5]) == 0)) {
        kernel_pid_t ifs[NG_NETIF_NUMOF];
        size_t ifnum = ng_netif_get(ifs);
        if (ifnum == 1) {
            _fib_add(argv[2], argv[4], ifs[0], (uint32_t)atoi(argv[6]));
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }
#endif

    /* e.g. fibroute add <destination> via <next hop> dev <device> */
    if (argc == 7) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)) {
            _fib_add(argv[2], argv[4], (kernel_pid_t)atoi(argv[6]), FIB_LIFETIME_NO_EXPIRE);
        }
        else {
            _fib_usage(1);
            return 1;
        }

        return 0;
    }

    /* e.g. fibroute add <destination> via <next hop> dev <device> lifetime <lifetime> */
    if (argc == 9) {
        if ((strcmp("add", argv[1]) == 0) && (strcmp("via", argv[3]) == 0)
            && (strcmp("dev", argv[5]) == 0)
            && (strcmp("lifetime", argv[7]) == 0)) {
            _fib_add(argv[2], argv[4], (kernel_pid_t)atoi(argv[6]), (uint32_t)atoi(argv[8]));
        }
        else {
            _fib_usage(2);
            return 1;
        }

        return 0;
    }

    puts("\nunrecognized parameters.\nPlease enter fibroute [add|del] for more information.");
    return 1;
}
