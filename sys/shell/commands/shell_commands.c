/*
 * Copyright (C) 2014  INRIA.
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
 * @brief       Provides prototypes and sets up available shell commands
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include "shell_commands.h"

extern int _reboot_handler(int argc, char **argv);
extern int _version_handler(int argc, char **argv);

#ifdef MODULE_CONFIG
extern int _id_handler(int argc, char **argv);
#endif

#ifdef MODULE_DFPLAYER
extern int _sc_dfplayer(int argc, char **argv);
#endif

#ifdef MODULE_HEAP_CMD
extern int _heap_handler(int argc, char **argv);
#endif

#ifdef MODULE_PERIPH_PM
extern int _pm_handler(int argc, char **argv);
#endif

#ifdef MODULE_PS
extern int _ps_handler(int argc, char **argv);
#endif

#ifdef MODULE_SHT1X
extern int _get_temperature_handler(int argc, char **argv);
extern int _get_humidity_handler(int argc, char **argv);
extern int _get_weather_handler(int argc, char **argv);
extern int _sht_config_handler(int argc, char **argv);
#endif

#ifdef MODULE_AT30TSE75X
extern int _at30tse75x_handler(int argc, char **argv);
#endif

#ifdef MODULE_SAUL_REG
extern int _saul(int argc, char **argv);
#endif

#ifdef MODULE_PERIPH_RTC
extern int _rtc_handler(int argc, char **argv);
#endif

#ifdef MODULE_RTT_CMD
extern int _rtt_handler(int argc, char **argv);
#endif

#ifdef MODULE_MCI
extern int _get_sectorsize(int argc, char **argv);
extern int _get_blocksize(int argc, char **argv);
extern int _get_sectorcount(int argc, char **argv);
extern int _read_sector(int argc, char **argv);
extern int _read_bytes(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_ICMPV6_ECHO
#ifdef MODULE_XTIMER
extern int _gnrc_icmpv6_ping(int argc, char **argv);
#endif
#endif

#ifdef MODULE_RANDOM
extern int _random_init(int argc, char **argv);
extern int _random_get(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_NIB
extern int _gnrc_ipv6_nib(int argc, char **argv);
#endif

#ifdef MODULE_NETSTATS_NEIGHBOR
extern int _netstats_nb(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_NETIF
extern int _gnrc_netif_config(int argc, char **argv);
#ifdef MODULE_GNRC_TXTSND
extern int _gnrc_netif_send(int argc, char **argv);
#endif
#endif

#ifdef MODULE_OPENWSN
extern int _openwsn_ifconfig(int argc, char **argv);
extern int _openwsn_handler(int argc, char **argv);
#endif

#ifdef MODULE_FIB
extern int _fib_route_handler(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_EXT_FRAG_STATS
extern int _gnrc_ipv6_frag_stats(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_WHITELIST
extern int _whitelist(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_BLACKLIST
extern int _blacklist(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_PKTBUF_CMD
extern int _gnrc_pktbuf_cmd(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_RPL
extern int _gnrc_rpl(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_SIXLOWPAN_CTX
extern int _gnrc_6ctx(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
extern int _gnrc_6lo_frag_stats(int argc, char **argv);
#endif

#ifdef MODULE_CCN_LITE_UTILS
extern int _ccnl_open(int argc, char **argv);
extern int _ccnl_content(int argc, char **argv);
extern int _ccnl_interest(int argc, char **argv);
extern int _ccnl_fib(int argc, char **argv);
#endif

#ifdef MODULE_SNTP
extern int _ntpdate(int argc, char **argv);
#endif

#ifdef MODULE_VFS
extern int _vfs_handler(int argc, char **argv);
extern int _ls_handler(int argc, char **argv);
#endif

#ifdef MODULE_CONN_CAN
extern int _can_handler(int argc, char **argv);
#endif

#ifdef MODULE_CORD_EP
extern int _cord_ep_handler(int argc, char **argv);
#endif

#ifdef MODULE_APP_METADATA
extern int _app_metadata_handler(int argc, char **argv);
#endif

#ifdef MODULE_I2C_SCAN
extern int _i2c_scan(int argc, char **argv);
#endif

#ifdef MODULE_SEMTECH_LORAMAC
extern int _loramac_handler(int argc, char **argv);
#endif

#ifdef MODULE_NIMBLE_NETIF
extern int _nimble_netif_handler(int argc, char **argv);
#endif

#ifdef MODULE_NIMBLE_STATCONN
extern int _nimble_statconn_handler(int argc, char **argv);
#endif

#ifdef MODULE_SUIT_TRANSPORT_COAP
extern int _suit_handler(int argc, char **argv);
#endif

#ifdef MODULE_CRYPTOAUTHLIB
extern int _cryptoauth(int argc, char **argv);
#endif

#ifdef MODULE_USB_BOARD_RESET
extern int _bootloader_handler(int argc, char **argv);
#endif

const shell_command_t _shell_command_list[] = {
    {"reboot", "Reboot the node", _reboot_handler},
    {"version", "Prints current RIOT_VERSION", _version_handler},
#ifdef MODULE_USB_BOARD_RESET
    {"bootloader", "Reboot to bootloader", _bootloader_handler},
#endif
#ifdef MODULE_CONFIG
    {"id", "Gets or sets the node's id.", _id_handler},
#endif
#ifdef MODULE_HEAP_CMD
    {"heap", "Prints heap statistics.", _heap_handler},
#endif
#ifdef MODULE_PERIPH_PM
    { "pm", "interact with layered PM subsystem", _pm_handler },
#endif
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
#ifdef MODULE_SHT1X
    {"temp", "Prints measured temperature.", _get_temperature_handler},
    {"hum", "Prints measured humidity.", _get_humidity_handler},
    {"weather", "Prints measured humidity and temperature.", _get_weather_handler},
    {"sht-config", "Get/set SHT10/11/15 sensor configuration.", _sht_config_handler},
#endif
#ifdef MODULE_AT30TSE75X
    {"at30tse75x", "Test AT30TSE75X temperature sensor", _at30tse75x_handler},
#endif
#ifdef MODULE_MCI
    {DISK_READ_SECTOR_CMD, "Reads the specified sector of inserted memory card", _read_sector},
    {DISK_READ_BYTES_CMD, "Reads the specified bytes from inserted memory card", _read_bytes},
    {DISK_GET_SECTOR_SIZE, "Get the sector size of inserted memory card", _get_sectorsize},
    {DISK_GET_SECTOR_COUNT, "Get the sector count of inserted memory card", _get_sectorcount},
    {DISK_GET_BLOCK_SIZE, "Get the block size of inserted memory card", _get_blocksize},
#endif
#ifdef MODULE_GNRC_ICMPV6_ECHO
#ifdef MODULE_XTIMER
    { "ping6", "Ping via ICMPv6", _gnrc_icmpv6_ping },
    { "ping", "Alias for ping6", _gnrc_icmpv6_ping },
#endif
#endif
#ifdef MODULE_RANDOM
    { "random_init", "initializes the PRNG", _random_init },
    { "random_get", "returns 32 bit of pseudo randomness", _random_get },
#endif
#ifdef MODULE_PERIPH_RTC
    {"rtc", "control RTC peripheral interface",  _rtc_handler},
#endif
#ifdef MODULE_RTT_CMD
    {"rtt", "control RTC peripheral interface",  _rtt_handler},
#endif
#ifdef MODULE_GNRC_IPV6_NIB
    {"nib", "Configure neighbor information base", _gnrc_ipv6_nib},
#endif
#ifdef MODULE_NETSTATS_NEIGHBOR
    {"neigh", "Show neighbor statistics", _netstats_nb},
#endif
#ifdef MODULE_GNRC_NETIF
    {"ifconfig", "Configure network interfaces", _gnrc_netif_config},
#ifdef MODULE_GNRC_TXTSND
    {"txtsnd", "Sends a custom string as is over the link layer", _gnrc_netif_send },
#endif
#endif
#ifdef MODULE_OPENWSN
    {"ifconfig", "Shows assigned IPv6 addresses", _openwsn_ifconfig},
    {"openwsn", "OpenWSN commands", _openwsn_handler},
#endif
#ifdef MODULE_FIB
    {"fibroute", "Manipulate the FIB (info: 'fibroute [add|del]')", _fib_route_handler},
#endif
#ifdef MODULE_GNRC_IPV6_EXT_FRAG_STATS
    {"ip6_frag", "IPv6 fragmentation statistics", _gnrc_ipv6_frag_stats },
#endif
#ifdef MODULE_GNRC_IPV6_WHITELIST
    {"whitelist", "whitelists an address for receival ('whitelist [add|del|help]')", _whitelist },
#endif
#ifdef MODULE_GNRC_IPV6_BLACKLIST
    {"blacklist", "blacklists an address for receival ('blacklist [add|del|help]')", _blacklist },
#endif
#ifdef MODULE_GNRC_PKTBUF_CMD
    {"pktbuf", "prints internal stats of the packet buffer", _gnrc_pktbuf_cmd },
#endif
#ifdef MODULE_GNRC_RPL
    {"rpl", "rpl configuration tool ('rpl help' for more information)", _gnrc_rpl },
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_CTX
    {"6ctx", "6LoWPAN context configuration tool", _gnrc_6ctx },
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
    {"6lo_frag", "6LoWPAN fragment statistics", _gnrc_6lo_frag_stats },
#endif
#ifdef MODULE_SAUL_REG
    {"saul", "interact with sensors and actuators using SAUL", _saul },
#endif
#ifdef MODULE_CCN_LITE_UTILS
    { "ccnl_open", "opens an interface or socket", _ccnl_open },
    { "ccnl_int", "sends an interest", _ccnl_interest },
    { "ccnl_cs", "shows CS or creates content and populates it", _ccnl_content },
    { "ccnl_fib", "shows or modifies the CCN-Lite FIB", _ccnl_fib },
#endif
#ifdef MODULE_SNTP
    { "ntpdate", "synchronizes with a remote time server", _ntpdate },
#endif
#ifdef MODULE_VFS
    {"vfs", "virtual file system operations", _vfs_handler},
    {"ls", "list files", _ls_handler},
#endif
#ifdef MODULE_CONN_CAN
    {"can", "CAN commands", _can_handler},
#endif
#ifdef MODULE_CORD_EP
    {"cord_ep", "Resource directory endpoint commands", _cord_ep_handler },
#endif
#ifdef MODULE_APP_METADATA
    {"app_metadata", "Returns application metadata", _app_metadata_handler },
#endif
#ifdef MODULE_I2C_SCAN
    { "i2c_scan", "Performs an I2C bus scan", _i2c_scan },
#endif
#ifdef MODULE_SEMTECH_LORAMAC
    {"loramac", "Control Semtech loramac stack", _loramac_handler},
#endif
#ifdef MODULE_NIMBLE_NETIF
    { "ble", "Manage BLE connections for NimBLE", _nimble_netif_handler },
#endif
#ifdef MODULE_NIMBLE_STATCONN
    { "statconn", "NimBLE netif statconn", _nimble_statconn_handler},
#endif
#ifdef MODULE_SUIT_TRANSPORT_COAP
    { "suit", "Trigger a SUIT firmware update", _suit_handler },
#endif
#ifdef MODULE_CRYPTOAUTHLIB
    { "cryptoauth", "Commands for Microchip CryptoAuth devices", _cryptoauth },
#endif
#ifdef MODULE_DFPLAYER
    {"dfplayer", "Control a DFPlayer Mini MP3 player", _sc_dfplayer},
#endif
    {NULL, NULL, NULL}
};
