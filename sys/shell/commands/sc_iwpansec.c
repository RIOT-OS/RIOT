/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Manage IEEE 802.15.4 link layer security
 *
 * This is an administrative shell tool, that is used to provide keys, peered
 * devices and their relations. It is similar to the
 * <a href="https://github.com/linux-wpan/wpan-tools/wiki/Guide:-Contiki-%E2%86%94-Linux,-with-llsec#linuxiwpan-configuration">wpan-tools</a>
 * in Linux but only very much limited to the configuration LL security.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/errno.h>

#include "shell.h"
#include "net/netif.h"
#include "net/gnrc/netif.h"
#include "net/netdev/ieee802154.h"
#include "net/ieee802154_security.h"
#include "fmt.h"

#ifndef CSTRLEN
#define CSTRLEN(cs)     (sizeof((const char[]){cs}) - 1)
#endif
#ifndef CSTRCMP
#define CSTRCMP(p, cs)  (memcmp(p, cs, CSTRLEN(cs)))
#endif
#ifndef MIN
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#endif

static inline ieee802154_sec_context_t * _iwpansec_get_sec(netif_t *iface)
{
    return &(((netdev_ieee802154_t *)(((gnrc_netif_t *)iface)->dev))->sec_ctx);
}

static int _iwpansec_enable(netif_t *iface)
{
    netopt_enable_t e = NETOPT_ENABLE;
    return netif_set_opt(iface, NETOPT_ENCRYPTION, 0, &e, sizeof(e));
}

static int _iwpansec_disable(netif_t *iface)
{
    netopt_enable_t e = NETOPT_DISABLE;
    return netif_set_opt(iface, NETOPT_ENCRYPTION, 0, &e, sizeof(e));
}

static void _iwpansec_print_devkeys_implicit(const ieee802154_sec_key_lookup_t *key_lookup,
                                             ieee802154_sec_key_lookup_descriptor_t key)
{
    char s_addr[25] = "                       ";
    if (key_lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
        if (key_lookup->key_lookup.implicit.dev_mode == IEEE802154_SEC_DEV_ADDRMODE_SHORT) {
            l2util_addr_to_str(key_lookup->key_lookup.implicit.dev_addr,
                               IEEE802154_SHORT_ADDRESS_LEN,
                               s_addr + 18);
        }
        else if (key_lookup->key_lookup.implicit.dev_mode == IEEE802154_SEC_DEV_ADDRMODE_LONG) {
            l2util_addr_to_str(key_lookup->key_lookup.implicit.dev_addr,
                               IEEE802154_LONG_ADDRESS_LEN,
                               s_addr);
        }
        printf("%8u | 0x%02x%02x | %s |%5u \n",
                key,
                key_lookup->key_lookup.implicit.dev_pan_id[0],
                key_lookup->key_lookup.implicit.dev_pan_id[1],
                s_addr,
                key_lookup->key);
    }
}


static void _iwpansec_list_devkeys_implicit(netif_t *iface)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    ieee802154_sec_iterator_acquire(ctx);
    puts("<devkey> |    pan |                adddress | <key>");
    puts("---------+--------+-------------------------+------");
    /*         XXX | 0xXXXX | XX:XX:XX:XX:XX:XX:XX:XX |  XXX  */
    ieee802154_sec_key_lookup_t lookup;
    ieee802154_sec_key_lookup_descriptor_t l = IEEE802154_SEC_NO_IDENT;
    while ((l = ieee802154_sec_key_lookup_iterator(ctx, l, &lookup)) != IEEE802154_SEC_NO_IDENT) {
        if (lookup.key_mode == IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
            _iwpansec_print_devkeys_implicit(&lookup, l);
        }
    }
    ieee802154_sec_iterator_release(ctx);
}

static void _iwpansec_print_devkeys_explicit(const ieee802154_sec_key_lookup_t *key_lookup,
                                             ieee802154_sec_key_lookup_descriptor_t key)
{
    char s_source[19] = "                  ";
    if (key_lookup->key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
        if (key_lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_HW_INDEX) {
            memcpy(s_source, "0x", 2);
            fmt_bytes_hex(s_source + 2, key_lookup->key_lookup.explicit.key_source, 8);
        }
        else if (key_lookup->key_mode == IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX) {
            memcpy(s_source, "        0x", 10);
            fmt_bytes_hex(s_source + 10, key_lookup->key_lookup.explicit.key_source, 4);
        }
        printf("%8u |%6u | %s |%5u \n",
                key,
                key_lookup->key_lookup.explicit.key_index,
                s_source,
                key_lookup->key);
    }
}

static void _iwpansec_list_devkeys_explicit(netif_t *iface)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    ieee802154_sec_iterator_acquire(ctx);
    puts("<devkey> | index |             source | <key>");
    puts("---------+-------+--------------------+------");
    /*         XXX |   XXX | 0xXXXXXXXXXXXXXXXX |  XXX  */
    ieee802154_sec_key_lookup_t lookup;
    ieee802154_sec_key_lookup_descriptor_t l = IEEE802154_SEC_NO_IDENT;
    while ((l = ieee802154_sec_key_lookup_iterator(ctx, l, &lookup)) != IEEE802154_SEC_NO_IDENT) {
        if (lookup.key_mode != IEEE802154_SEC_SCF_KEYMODE_IMPLICIT) {
            _iwpansec_print_devkeys_explicit(&lookup, l);
        }
    }
    ieee802154_sec_iterator_release(ctx);
}


static int _iwpansec_devkey(netif_t *iface, int argc, char **argv)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    if (!argc) {
        _iwpansec_list_devkeys_implicit(iface);
        _iwpansec_list_devkeys_explicit(iface);
        return 0;
    }
    else if (!CSTRCMP(argv[0], "add")) {
        if (argc < 3) {
            return -EINVAL;
        }
        long l_k;
        if (!fmt_is_number(argv[1]) ||
            (l_k = strtol(argv[1], NULL, 10)) < 0) {
            return -EINVAL;
        }
        argc -= 3;
        ieee802154_sec_key_lookup_descriptor_t l;
        if (!CSTRCMP(argv[2], "implicit")) {
            if (argc < 1) {
                return -EINVAL;
            }
            if (!CSTRCMP(argv[3], "none")) {
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_implicit(ctx,
                                                                   IEEE802154_SEC_DEV_ADDRMODE_NONE,
                                                                   0, NULL, l_k))
                        ? l : -ECANCELED;
            }
            else if (!CSTRCMP(argv[3], "short")) {
                if (argc < 3) {
                    return -EINVAL;
                }
                uint8_t short_addr[IEEE802154_SHORT_ADDRESS_LEN];
                long l_pan;
                if (strlen(argv[4]) > CSTRLEN("0xFFFF") ||
                    (l_pan = strtol(argv[4], NULL, 16)) < 0 || l_pan > 0xffff) {
                    return -EINVAL;
                }
                if (strlen(argv[5]) != CSTRLEN("xx:xx") ||
                    l2util_addr_from_str(argv[5], short_addr) != sizeof(short_addr)) {
                    return -EINVAL;
                }
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_implicit(ctx,
                                                                   IEEE802154_SEC_DEV_ADDRMODE_SHORT,
                                                                   l_pan, short_addr, l_k))
                        ? l : -ECANCELED;
            }
            else if (!CSTRCMP(argv[3], "long")) {
                if (argc < 3) {
                    return -EINVAL;
                }
                uint8_t long_addr[IEEE802154_LONG_ADDRESS_LEN];
                long l_pan;
                if (strlen(argv[4]) > CSTRLEN("0xFFFF") ||
                    (l_pan = strtol(argv[4], NULL, 16)) < 0 || l_pan > 0xffff) {
                    return -EINVAL;
                }
                if (strlen(argv[5]) != CSTRLEN("xx:xx:xx:xx:xx:xx:xx:xx") ||
                    l2util_addr_from_str(argv[5], long_addr) != sizeof(long_addr)) {
                    return -EINVAL;
                }
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_implicit(ctx, IEEE802154_SEC_DEV_ADDRMODE_LONG,
                                                                   l_pan, long_addr, l_k))
                        ? l : -ECANCELED;
            }
        }
        else if (!CSTRCMP(argv[2], "explicit")) {
            if (argc < 1) {
                return -EINVAL;
            }
            long key_id;
            if (!fmt_is_number(argv[3]) ||
                !(key_id = strtol(argv[3], NULL, 10)) || key_id < 0 || key_id > 255) {
                return -EINVAL;
            }
            if (!--argc) {
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_explicit(ctx, IEEE802154_SEC_SCF_KEYMODE_INDEX,
                                                              key_id, NULL, l_k))
                        ? l : -ECANCELED;
            }
            else if (!CSTRCMP(argv[4], "short")) {
                if (argc < 3) {
                    return -EINVAL;
                }
                struct key_source {
                    uint16_t pan;
                    uint8_t short_addr[2];
                } key_source;
                long l_pan;
                if (strlen(argv[5]) > CSTRLEN("0xFFFF") ||
                    (l_pan = strtol(argv[5], NULL, 16)) < 0 || l_pan > 0xffff) {
                    return -EINVAL;
                }
                if (strlen(argv[6]) != CSTRLEN("xx:xx") ||
                    l2util_addr_from_str(argv[6], key_source.short_addr) != 2) {
                    return -EINVAL;
                }
                key_source.pan = htons(l_pan);
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_explicit(ctx, IEEE802154_SEC_SCF_KEYMODE_SHORT_INDEX,
                                                                   key_id, &key_source, l_k))
                        ? l : -ECANCELED;
            }
            else if (!CSTRCMP(argv[4], "long")) {
                if (argc < 2) {
                    return -EINVAL;
                }
                struct key_source {
                    uint8_t long_addr[8];
                } key_source;
                if (strlen(argv[5]) != CSTRLEN("xx:xx:xx:xx:xx:xx:xx:xx") ||
                    l2util_addr_from_str(argv[5], key_source.long_addr) != 8) {
                    return -EINVAL;
                }
                return IEEE802154_SEC_NO_IDENT !=
                       (l = ieee802154_sec_add_key_lookup_explicit(ctx, IEEE802154_SEC_SCF_KEYMODE_HW_INDEX,
                                                                   key_id, &key_source, l_k))
                        ? l : -ECANCELED;
            }
        }
    }
    else if (!CSTRCMP(argv[0], "rem")) {
        if (argc < 2) {
            return -EINVAL;
        }
        if (!fmt_is_number(argv[1])) {
            return -EINVAL;
        }
        int l = atoi(argv[1]);
        if (l >= 0) {
            ieee802154_sec_remove_key_lookup(ctx, l);
            return l;
        }
    }
    return -EINVAL;
}

static void _iwpansec_print_dev(const ieee802154_sec_dev_lookup_t *dev_lookup,
                                ieee802154_sec_dev_descriptor_t dev)
{
    char s_short[6];
    char s_long[25];
    l2util_addr_to_str(dev_lookup->short_addr, sizeof(dev_lookup->short_addr), s_short);
    l2util_addr_to_str(dev_lookup->long_addr, sizeof(dev_lookup->long_addr), s_long);
    printf("%5u | 0x%02x%02x | %s | %s | %13"PRIu32"\n",
            dev,
            dev_lookup->pan_id[0],
            dev_lookup->pan_id[1],
            s_short,
            s_long,
            dev_lookup->fc
    );
}

static void _iwpansec_list_devs(netif_t *iface)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    ieee802154_sec_iterator_acquire(ctx);
    puts("<dev> |    pan | short |                    long | frame counter");
    puts("------+--------+-------+-------------------------+--------------");
    /*      XXX | 0xXXXX | XX:XX | XX:XX:XX:XX:XX:XX:XX:XX |   XXXXXXXXXX  */

    ieee802154_sec_dev_lookup_t dev;
    ieee802154_sec_dev_descriptor_t d = IEEE802154_SEC_NO_IDENT;
    while ((d = ieee802154_sec_dev_iterator(ctx, d, &dev)) != IEEE802154_SEC_NO_IDENT) {
        _iwpansec_print_dev(&dev, d);
    }
    ieee802154_sec_iterator_release(ctx);
}

static int _iwpansec_dev(netif_t *iface, int argc, char **argv)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    if (!argc) {
        _iwpansec_list_devs(iface);
        return 0;
    }
    else if (!CSTRCMP(argv[0], "add")) {
        if (argc < 4) {
            return -EINVAL;
        }
        uint8_t short_addr[IEEE802154_SHORT_ADDRESS_LEN];
        uint8_t long_addr[IEEE802154_LONG_ADDRESS_LEN];
        long l_pan;
        if (strlen(argv[1]) > CSTRLEN("0xffff") ||
            (l_pan = strtol(argv[1], NULL, 16)) < 0 || l_pan > 0xffff) {
            return -EINVAL;
        }
        if (strlen(argv[2]) != CSTRLEN("xx:xx") ||
            l2util_addr_from_str(argv[2], short_addr) != sizeof(short_addr)) {
            return -EINVAL;
        }
        if (strlen(argv[3]) != CSTRLEN("xx:xx:xx:xx:xx:xx:xx:xx") ||
            l2util_addr_from_str(argv[3], long_addr) != sizeof(long_addr)) {
            return -EINVAL;
        }
        ieee802154_sec_dev_descriptor_t d;
        return IEEE802154_SEC_NO_IDENT !=
               (d = ieee802154_sec_add_dev(ctx, l_pan, short_addr, long_addr))
               ? d : -ECANCELED;
    }
    else if (!CSTRCMP(argv[0], "rem")) {
        if (argc < 2) {
            return -EINVAL;
        }
        if (!fmt_is_number(argv[1])) {
            return -EINVAL;
        }
        int d = atoi(argv[1]);
        if (d >= 0) {
            ieee802154_sec_remove_dev(ctx, d);
            return d;
        }
    }
    else if (!CSTRCMP(argv[0], "peer")) {
        if (argc < 5) {
            return -EINVAL;
        }
        int l = atoi(argv[1]);
        long l_pan;
        uint8_t short_addr[IEEE802154_SHORT_ADDRESS_LEN];
        uint8_t long_addr[IEEE802154_LONG_ADDRESS_LEN];
        if (strlen(argv[2]) > CSTRLEN("0xffff") ||
            (l_pan = strtol(argv[2], NULL, 16)) < 0 || l_pan > 0xffff) {
            return -EINVAL;
        }
        if (strlen(argv[3]) != CSTRLEN("xx:xx") ||
            l2util_addr_from_str(argv[3], short_addr) != sizeof(short_addr)) {
            return -EINVAL;
        }
        if (strlen(argv[4]) != CSTRLEN("xx:xx:xx:xx:xx:xx:xx:xx") ||
            l2util_addr_from_str(argv[4], long_addr) != sizeof(long_addr)) {
            return -EINVAL;
        }
        if (l >= 0) {
            if (IEEE802154_SEC_OK !=
                ieee802154_sec_peer_dev(ctx, l, l_pan, short_addr, long_addr)) {
                return -ECANCELED;
            }
            return 0;
        }
    }
    return -EINVAL;
}

static void _iwpansec_print_key(const ieee802154_sec_key_t *key, ieee802154_sec_key_descriptor_t k)
{
    char s_key[2 * sizeof(key->key) + 3] = "0x";
    fmt_bytes_hex(s_key + 2, key->key, sizeof(key->key));
    s_key[sizeof(s_key) - 1] = '\0';
    printf("%5u | %s | %13"PRIu32"\n",
           k, s_key, key->fc);
}

static void _iwpansec_list_keys(netif_t *iface) {
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    ieee802154_sec_iterator_acquire(ctx);
    puts("<key> |                                key | frame counter");
    puts("------+------------------------------------+--------------");
    /*      XXX | 0xXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX |   XXXXXXXXXX  */

    ieee802154_sec_key_t key;
    ieee802154_sec_key_descriptor_t k = IEEE802154_SEC_NO_IDENT;
    while ((k = ieee802154_sec_key_iterator(ctx, k, &key)) != IEEE802154_SEC_NO_IDENT) {
        _iwpansec_print_key(&key, k);
    }
    ieee802154_sec_iterator_release(ctx);
}

static int _iwpansec_key(netif_t *iface, int argc, char **argv)
{
    ieee802154_sec_context_t *ctx = _iwpansec_get_sec(iface);
    if (!argc) {
        _iwpansec_list_keys(iface);
        return 0;
    }
    else if (!CSTRCMP(argv[0], "add")) {
        uint8_t key[16];
        const char *s_key = argv[1];
        if (!strncmp(s_key, "0x", 2)) {
            s_key += 2;
        }
        if (strlen(s_key) != 2 * sizeof(key)) {
            return -EINVAL;
        }
        for (unsigned i= 0; i < sizeof(key); i++, s_key += 2) {
            if (!(
                (('0' <= s_key[0] && s_key[0] <= '9') ||
                 ('a' <= s_key[0] && s_key[0] <= 'f') ||
                 ('A' <= s_key[0] && s_key[0] <= 'F'))
                &&
                (('0' <= s_key[1] && s_key[1] <= '9') ||
                 ('a' <= s_key[1] && s_key[1] <= 'f') ||
                 ('A' <= s_key[1] && s_key[1] <= 'F')))) {
                return -EINVAL;
            }
            key[i] = scn_u32_hex(s_key, 2);
        }
        ieee802154_sec_dev_descriptor_t k;
        return IEEE802154_SEC_NO_IDENT !=
               (k = ieee802154_sec_add_key(ctx, key))
               ? k : -ECANCELED;
    }
    else if (!CSTRCMP(argv[0], "rem")) {
        int k = atoi(argv[1]);
        if (k >= 0) {
            ieee802154_sec_remove_dev(ctx, k);
            return k;
        }
    }
    return -EINVAL;
}

static int _iwpansec_probe(netif_t *iface)
{
    uint16_t type;
    if (netif_get_opt(iface, NETOPT_DEVICE_TYPE, 0, &type, sizeof(type)) <= 0 ||
        type != NETDEV_TYPE_IEEE802154) {
        return -EINVAL;
    }
    netopt_enable_t sup;
    if (netif_get_opt(iface, NETOPT_ENCRYPTION, 0, &sup, sizeof(sup)) <= 0) {
        return -ENOTSUP;
    }
    return 0;
}

static void _iwpansec_print_help(void)
{
    puts("Usage: iwpansec <interface> [COMMAND]\n"
         "Placeholders in capital letters are user input and placeholders in "
         "small letters are IDs you have to look up, using this program.\n"
         "[enable]\n"
         "[disable]\n"
         "[devkey]\n"
         "  [add <key> <implicit <none | short <PAN> <SHORT> | long <PAN> <LONG>>>]\n"
         "  [add <key> <explicit <ID> [short <PAN> <SHORT_SOURCE> | long <LONG_SOURCE>]>]\n"
         "  [rem <devkey>]\n"
         "[dev]\n"
         "  [add <PAN> <SHORT> <LONG>]\n"
         "  [rem <dev>]\n"
         "  [peer <devkey> <PAN> <SHORT> <LONG>]\n"
         "[key]\n"
         "  [add <KEY>]\n"
         "  [rem <key>]\n"
    );
}

int _sc_iwpansec(int argc, char **argv)
{
    int success = -EINVAL;
    if (argc < 3) {
        goto EXIT_HELP;
    }
    netif_t *iface;
    iface = netif_get_by_name(argv[1]);
    if (!iface) {
        puts("invalid interface given");
        goto EXIT_HELP;
    }
    switch (_iwpansec_probe(iface)) {
        case -EINVAL:
            puts("interface type is not IEEE 802.15.4");
            goto EXIT_HELP;
        case -ENOTSUP:
            puts("interface does not support security");
            goto EXIT_HELP;
    }

    if (!CSTRCMP(argv[2], "enable")) {
        if ((success = _iwpansec_enable(iface)) < 0) {
            goto EXIT_HELP;
        }
    }
    else if (!CSTRCMP(argv[2], "disable")) {
        if ((success = _iwpansec_disable(iface)) < 0) {
            goto EXIT_HELP;
        }
    }
    else if (!CSTRCMP(argv[2], "devkey")) {
        if ((success = _iwpansec_devkey(iface, argc - 3, argv + 3)) < 0) {
            goto EXIT_HELP;
        }
    }
    else if (!CSTRCMP(argv[2], "dev")) {
        if ((success = _iwpansec_dev(iface, argc - 3, argv + 3)) < 0) {
            goto EXIT_HELP;
        }
    }
    else if (!CSTRCMP(argv[2], "key")) {
        if ((success = _iwpansec_key(iface, argc - 3, argv + 3)) < 0) {
            goto EXIT_HELP;
        }
    }
    else {
        goto EXIT_HELP;
    }

    puts("iwpansec: ok");
    return EXIT_SUCCESS;

EXIT_HELP:
    if (success == -EINVAL) {
        _iwpansec_print_help();
    }
    else {
        puts("iwpansec: error");
    }
    return EXIT_FAILURE;
}

SHELL_COMMAND(iwpansec, "Manage IEEE 802.15.4 keys and peer devices", _sc_iwpansec);
