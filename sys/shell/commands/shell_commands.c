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
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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

#ifdef MODULE_ISL29020
extern int _get_isl29020_init_handler(int argc, char **argv);
extern int _get_isl29020_read_handler(int argc, char **argv);
#endif

#ifdef MODULE_LPS331AP
extern int _get_lps331ap_init_handler(int argc, char **argv);
extern int _get_lps331ap_read_handler(int argc, char **argv);
#endif

#ifdef MODULE_L3G4200D
extern int _get_l3g4200d_init_handler(int argc, char **argv);
extern int _get_l3g4200d_read_handler(int argc, char **argv);
#endif

#ifdef MODULE_LSM303DLHC
extern int _get_lsm303dlhc_init_handler(int argc, char **argv);
extern int _get_lsm303dlhc_read_handler(int argc, char **argv);
#endif

#ifdef MODULE_LTC4150
extern int _get_current_handler(int argc, char **argv);
extern int _reset_current_handler(int argc, char **argv);
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
#ifdef MODULE_VTIMER
extern int _icmpv6_ping(int argc, char **argv);
#endif
#endif

#ifdef MODULE_RANDOM
extern int _mersenne_init(int argc, char **argv);
extern int _mersenne_get(int argc, char **argv);
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

#ifdef MODULE_GNRC_ZEP
#ifdef MODULE_IPV6_ADDR
extern int _zep_init(int argc, char **argv);
#endif
#endif

#ifdef MODULE_GNRC_RPL
extern int _gnrc_rpl(int argc, char **argv);
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
#ifdef MODULE_ISL29020
    {"isl29020_init", "Initializes the isl29020 sensor driver.", _get_isl29020_init_handler},
    {"isl29020_read", "Prints data from the isl29020 sensor.", _get_isl29020_read_handler},
#endif
#ifdef MODULE_LPS331AP
    {"lps331ap_init", "Initializes the lps331ap sensor driver.", _get_lps331ap_init_handler},
    {"lps331ap_read", "Prints data from the lps331ap sensor.", _get_lps331ap_read_handler},
#endif
#ifdef MODULE_L3G4200D
    {"l3g4200d_init", "Initializes the l3g4200d sensor driver.", _get_l3g4200d_init_handler},
    {"l3g4200d_read", "Prints data from the l3g4200d sensor.", _get_l3g4200d_read_handler},
#endif
#ifdef MODULE_LSM303DLHC
    {"lsm303dlhc_init", "Initializes the lsm303dlhc sensor driver.", _get_lsm303dlhc_init_handler},
    {"lsm303dlhc_read", "Prints data from the lsm303dlhc sensor.", _get_lsm303dlhc_read_handler},
#endif
#ifdef MODULE_LTC4150
    {"cur", "Prints current and average power consumption.", _get_current_handler},
    {"rstcur", "Resets coulomb counter.", _reset_current_handler},
#endif
#ifdef MODULE_MCI
    {DISK_READ_SECTOR_CMD, "Reads the specified sector of inserted memory card", _read_sector},
    {DISK_READ_BYTES_CMD, "Reads the specified bytes from inserted memory card", _read_bytes},
    {DISK_GET_SECTOR_SIZE, "Get the sector size of inserted memory card", _get_sectorsize},
    {DISK_GET_SECTOR_COUNT, "Get the sector count of inserted memory card", _get_sectorcount},
    {DISK_GET_BLOCK_SIZE, "Get the block size of inserted memory card", _get_blocksize},
#endif
#ifdef MODULE_GNRC_ICMPV6_ECHO
#ifdef MODULE_VTIMER
    { "ping6", "Ping via ICMPv6", _icmpv6_ping },
#endif
#endif
#ifdef MODULE_RANDOM
    { "mersenne_init", "initializes the PRNG", _mersenne_init },
    { "mersenne_get", "returns 32 bit of pseudo randomness", _mersenne_get },
#endif
#if FEATURE_PERIPH_RTC
    {"rtc", "control RTC peripheral interface",  _rtc_handler},
#endif
#ifdef CPU_X86
    {"lspci", "Lists PCI devices", _x86_lspci},
#endif
#ifdef MODULE_GNRC_NETIF
    {"ifconfig", "Configure network interfaces", _netif_config},
    {"txtsnd", "send raw data", _netif_send },
#endif
#ifdef MODULE_FIB
    {"fibroute", "Manipulate the FIB (info: 'fibroute [add|del]')", _fib_route_handler},
#endif
#ifdef MODULE_GNRC_IPV6_NC
    {"ncache", "manage neighbor cache by hand", _ipv6_nc_manage },
    {"routers", "IPv6 default router list", _ipv6_nc_routers },
#endif
#ifdef MODULE_GNRC_ZEP
#ifdef MODULE_IPV6_ADDR
    {"zep_init", "initializes ZEP (Zigbee Encapsulation Protocol)", _zep_init },
#endif
#endif
#ifdef MODULE_GNRC_RPL
    {"rpl", "rpl configuration tool [help|init|rm|root|show]", _gnrc_rpl },
#endif
    {NULL, NULL, NULL}
};
