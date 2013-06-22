/**
 * Provides prototypes for available shell commands
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup shell_commands
 * @{
 * @file    shell_commands.c
 * @brief   sets up the system shell command struct
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <shell_commands.h>
#include <stdlib.h>

extern void _id_handler(char *id);

#ifdef MODULE_PS
extern void _ps_handler(char *unused);
#endif

#ifdef MODULE_RTC
extern void _date_handler(char *now);
#endif

#ifdef MODULE_SHT11
extern void _get_temperature_handler(char *unused);
extern void _get_humidity_handler(char *unused);
extern void _get_weather_handler(char *unused);
extern void _set_offset_handler(char *offset);
#endif

#ifdef MODULE_LTC4150
extern void _get_current_handler(char *unused);
extern void _reset_current_handler(char *unused);
#endif

#ifdef MODULE_CC110X
#ifdef MODULE_TRANSCEIVER
extern void _cc1100_get_set_address_handler(char *addr);
extern void _cc1100_get_set_channel_handler(char *chan);
extern void _cc1100_send_handler(char *pkt);
#else
extern void _cc110x_get_set_address_handler(char *addr);
extern void _cc110x_get_set_channel_handler(char *addr);
#endif
#endif

#ifdef MODULE_TRANSCEIVER
#ifdef MODULE_CC110X_NG
extern void _cc110x_ng_get_set_address_handler(char *addr);
extern void _cc110x_ng_get_set_channel_handler(char *chan);
extern void _cc110x_ng_send_handler(char *pkt);
extern void _cc110x_ng_monitor_handler(char *mode);
#endif
#endif

#ifdef MODULE_MCI
extern void _get_sectorsize(char *unused);
extern void _get_blocksize(char *unused);
extern void _get_sectorcount(char *unused);
extern void _read_sector(char *sector);
extern void _read_bytes(char *bytes);
#endif

const shell_command_t _shell_command_list[] = {
    {"id", "Gets or sets the node's id.", _id_handler},
#ifdef MODULE_PS
    {"ps", "Prints information about running threads.", _ps_handler},
#endif
#ifdef MODULE_RTC
    {"date", "Gets or sets current date and time.", _date_handler},
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
#ifdef MODULE_CC110X
#ifdef MODULE_TRANSCEIVER
    {"addr", "Gets or sets the address for the CC1100 transceiver", _cc1100_get_set_address_handler},
    {"chan", "Gets or sets the channel for the CC1100 transceiver", _cc1100_get_set_channel_handler},
    {"txtsnd", "Sends a text message to a given node via the CC1100 transceiver", _cc1100_send_handler},
#else
    {"addr", "Gets or sets the address for the CC1100 transceiver", _cc110x_get_set_address_handler},
    {"chan", "Gets or sets the channel for the CC1100 transceiver", _cc110x_get_set_channel_handler},
#endif
#endif
#ifdef MODULE_TRANSCEIVER
#ifdef MODULE_CC110X_NG
    {"addr", "Gets or sets the address for the CC1100 transceiver", _cc110x_ng_get_set_address_handler},
    {"chan", "Gets or sets the channel for the CC1100 transceiver", _cc110x_ng_get_set_channel_handler},
    {"txtsnd", "Sends a text message to a given node via the CC1100 transceiver", _cc110x_ng_send_handler},
    {"monitor", "Enables or disables address checking for the CC1100 transceiver", _cc110x_ng_monitor_handler},
#endif
#endif
#ifdef MODULE_MCI
    {DISK_READ_SECTOR_CMD, "Reads the specified sector of inserted memory card", _read_sector},
    {DISK_READ_BYTES_CMD, "Reads the specified bytes from inserted memory card", _read_bytes},
    {DISK_GET_SECTOR_SIZE, "Get the sector size of inserted memory card", _get_sectorsize},
    {DISK_GET_SECTOR_COUNT, "Get the sector count of inserted memory card", _get_sectorcount},
    {DISK_GET_BLOCK_SIZE, "Get the block size of inserted memory card", _get_blocksize},
#endif
    {NULL, NULL, NULL}
};

