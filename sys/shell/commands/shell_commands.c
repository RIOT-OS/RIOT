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

#ifdef MODULE_CONFIG
extern int _id_handler(int argc, char **argv);
#endif

#ifdef MODULE_LPC_COMMON
extern int _heap_handler(int argc, char **argv);
#endif

#ifdef MODULE_PS
extern int _ps_handler(int argc, char **argv);
#endif

#ifdef MODULE_SHT11
extern int _get_temperature_handler(int argc, char **argv);
extern int _get_humidity_handler(int argc, char **argv);
extern int _get_weather_handler(int argc, char **argv);
extern int _set_offset_handler(int argc, char **argv);
#endif

#ifdef MODULE_LTC4150
extern int _get_current_handler(int argc, char **argv);
extern int _reset_current_handler(int argc, char **argv);
#endif

#ifdef MODULE_AT30TSE75X
extern int _at30tse75x_handler(int argc, char **argv);
#endif

#ifdef MODULE_SAUL_REG
extern int _saul(int argc, char **argv);
#endif

#if FEATURE_PERIPH_RTC
extern int _rtc_handler(int argc, char **argv);
#endif

#ifdef CPU_X86
extern int _x86_lspci(int argc, char **argv);
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
extern int _icmpv6_ping(int argc, char **argv);
#endif
#endif

#ifdef MODULE_RANDOM
extern int _random_init(int argc, char **argv);
extern int _random_get(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_NETIF
extern int _netif_config(int argc, char **argv);
extern int _netif_send(int argc, char **argv);
#endif

#ifdef MODULE_FIB
extern int _fib_route_handler(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_NC
extern int _ipv6_nc_manage(int argc, char **argv);
extern int _ipv6_nc_routers(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_WHITELIST
extern int _whitelist(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_IPV6_BLACKLIST
extern int _blacklist(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_ZEP
#ifdef MODULE_IPV6_ADDR
extern int _zep_init(int argc, char **argv);
#endif
#endif

#ifdef MODULE_GNRC_RPL
extern int _gnrc_rpl(int argc, char **argv);
#endif

#ifdef MODULE_GNRC_SIXLOWPAN_CTX
#ifdef MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER
extern int _gnrc_6ctx(int argc, char **argv);
#endif
#endif

#ifdef MODULE_CCN_LITE_UTILS
extern int _ccnl_open(int argc, char **argv);
extern int _ccnl_content(int argc, char **argv);
extern int _ccnl_interest(int argc, char **argv);
extern int _ccnl_fib(int argc, char **argv);
#endif

#ifdef MODULE_VFS
extern int _vfs_handler(int argc, char **argv);
extern int _ls_handler(int argc, char **argv);
#endif

const shell_command_t _shell_command_list[] = {
    {"reboot", "Reboot the node", _reboot_handler},
#ifdef MODULE_CONFIG
    {"id", "Gets or sets the node's id.", _id_handler},
#endif
#ifdef MODULE_LPC_COMMON
    {"heap", "Shows the heap state for the LPC2387 on the command shell.", _heap_handler},
#endif
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
#ifdef MODULE_SHT11
    {"temp", "Prints measured temperature.", _get_temperature_handler},
    {"hum", "Prints measured humidity.", _get_humidity_handler},
    {"weather", "Prints measured humidity and temperature.", _get_weather_handler},
    {"offset", "Set temperature offset.", _set_offset_handler},
#endif
#ifdef MODULE_LTC4150
    {"cur", "Prints current and average power consumption.", _get_current_handler},
    {"rstcur", "Resets coulomb counter.", _reset_current_handler},
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
    { "ping6", "Ping via ICMPv6", _icmpv6_ping },
#endif
#endif
#ifdef MODULE_RANDOM
    { "random_init", "initializes the PRNG", _random_init },
    { "random_get", "returns 32 bit of pseudo randomness", _random_get },
#endif
#if FEATURE_PERIPH_RTC
    {"rtc", "control RTC peripheral interface",  _rtc_handler},
#endif
#ifdef CPU_X86
    {"lspci", "Lists PCI devices", _x86_lspci},
#endif
#ifdef MODULE_GNRC_NETIF
    {"ifconfig", "Configure network interfaces", _netif_config},
    {"txtsnd", "Sends a custom string as is over the link layer", _netif_send },
#endif
#ifdef MODULE_FIB
    {"fibroute", "Manipulate the FIB (info: 'fibroute [add|del]')", _fib_route_handler},
#endif
#ifdef MODULE_GNRC_IPV6_NC
    {"ncache", "manage neighbor cache by hand", _ipv6_nc_manage },
    {"routers", "IPv6 default router list", _ipv6_nc_routers },
#endif
#ifdef MODULE_GNRC_IPV6_WHITELIST
    {"whitelist", "whitelists an address for receival ('whitelist [add|del|help]')", _whitelist },
#endif
#ifdef MODULE_GNRC_IPV6_BLACKLIST
    {"blacklist", "blacklists an address for receival ('blacklist [add|del|help]')", _blacklist },
#endif
#ifdef MODULE_GNRC_ZEP
#ifdef MODULE_IPV6_ADDR
    {"zep_init", "initializes ZEP (Zigbee Encapsulation Protocol)", _zep_init },
#endif
#endif
#ifdef MODULE_GNRC_RPL
    {"rpl", "rpl configuration tool ('rpl help' for more information)", _gnrc_rpl },
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_CTX
#ifdef MODULE_GNRC_SIXLOWPAN_ND_BORDER_ROUTER
    {"6ctx", "6LoWPAN context configuration tool", _gnrc_6ctx },
#endif
#endif
#ifdef MODULE_SAUL_REG
    {"saul", "interact with sensors and actuators using SAUL", _saul },
#endif
#ifdef MODULE_CCN_LITE_UTILS
    {"ccnl_open", "opens an interface or socket", _ccnl_open},
    {"ccnl_int", "sends an interest", _ccnl_interest},
    {"ccnl_cont", "create content and populated it", _ccnl_content},
    {"ccnl_fib", "shows or modifies the CCN-Lite FIB", _ccnl_fib},
#endif
#ifdef MODULE_VFS
    {"vfs", "virtual file system operations", _vfs_handler},
    {"ls", "list files", _ls_handler},
#endif
    {NULL, NULL, NULL}
};
