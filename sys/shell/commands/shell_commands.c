/**
 * Provides prototypes for available shell commands
 *
 * Copyright (C) 2013  INRIA.
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 *
 *
 *
 * @ingroup shell_commands
 * @{
 * @file    shell_commands.c
 * @brief   sets up the system shell command struct
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @author  Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @note    $Id: shell_commands.c 3855 2013-09-05 12:54:57 kasmi $
 * @}
 */

#include <stdlib.h>
#include "shell_commands.h"

extern void _id_handler(char *id);
extern void _heap_handler(char *unused);

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


/* configure available commands for each transceiver device: */
#ifdef MODULE_TRANSCEIVER
#ifdef DBG_IGNORE
#define _TC_IGN
#endif
#if (defined(MODULE_CC110X_NG) || defined(MODULE_CC2420) || defined(MODULE_NATIVENET))
#define _TC_ADDR
#define _TC_CHAN
#define _TC_MON
#define _TC_SEND
#endif
#if (defined(MODULE_CC2420) || defined(MODULE_NATIVENET))
#define _TC_PAN
#endif
#else /* WITHOUT MODULE_TRANSCEIVER */
#ifdef MODULE_CC110X
extern void _cc110x_get_set_address_handler(char *addr);
extern void _cc110x_get_set_channel_handler(char *addr);
#endif
#endif

#ifdef MODULE_TRANSCEIVER
#ifdef _TC_ADDR
extern void _transceiver_get_set_address_handler(char *addr);
#endif
#ifdef _TC_CHAN
extern void _transceiver_get_set_channel_handler(char *chan);
#endif
#ifdef _TC_SEND
extern void _transceiver_send_handler(char *pkt);
#endif
#ifdef _TC_MON
extern void _transceiver_monitor_handler(char *mode);
#endif
#ifdef _TC_PAN
extern void _transceiver_get_set_pan_handler(char *chan);
#endif
#ifdef _TC_IGN
extern void _transceiver_set_ignore_handler(char *addr);
#endif
#endif

#ifdef MODULE_MCI
extern void _get_sectorsize(char *unused);
extern void _get_blocksize(char *unused);
extern void _get_sectorcount(char *unused);
extern void _read_sector(char *sector);
extern void _read_bytes(char *bytes);
#endif

#ifdef MODULE_RANDOM
extern void _mersenne_init(char *str);
extern void _mersenne_get(char *str);
#endif

const shell_command_t _shell_command_list[] = {
    {"id", "Gets or sets the node's id.", _id_handler},
#ifdef MODULE_LPC_COMMON
    {"heap", "Shows the heap state for the LPC2387 on the command shell.", _heap_handler},
#endif
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


#ifdef MODULE_TRANSCEIVER
#ifdef _TC_ADDR
    {"addr", "Gets or sets the address for the transceiver", _transceiver_get_set_address_handler},
#endif
#ifdef _TC_CHAN
    {"chan", "Gets or sets the channel for the transceiver", _transceiver_get_set_channel_handler},
#endif
#ifdef _TC_SEND
    {"txtsnd", "Sends a text message to a given node via the transceiver", _transceiver_send_handler},
#endif
#ifdef _TC_PAN
    {"pan", "Gets or sets the pan id for the transceiver", _transceiver_get_set_pan_handler},
#endif
#ifdef _TC_MON
    {"monitor", "Enables or disables address checking for the transceiver", _transceiver_monitor_handler},
#endif
#ifdef _TC_IGN
    {"ign", "Ignore the address at the transceiver", _transceiver_set_ignore_handler},
#endif
#else /* WITHOUT MODULE_TRANSCEIVER */
#ifdef MODULE_CC110X
    {"addr", "Gets or sets the address for the CC1100 transceiver", _cc110x_get_set_address_handler},
    {"chan", "Gets or sets the channel for the CC1100 transceiver", _cc110x_get_set_channel_handler},
#endif
#endif


#ifdef MODULE_MCI
    {DISK_READ_SECTOR_CMD, "Reads the specified sector of inserted memory card", _read_sector},
    {DISK_READ_BYTES_CMD, "Reads the specified bytes from inserted memory card", _read_bytes},
    {DISK_GET_SECTOR_SIZE, "Get the sector size of inserted memory card", _get_sectorsize},
    {DISK_GET_SECTOR_COUNT, "Get the sector count of inserted memory card", _get_sectorcount},
    {DISK_GET_BLOCK_SIZE, "Get the block size of inserted memory card", _get_blocksize},
#endif
#ifdef MODULE_RANDOM
    { "mersenne_init", "initializes the PRNG", _mersenne_init },
    { "mersenne_get", "returns 32 bit of pseudo randomness", _mersenne_get },
#endif
    {NULL, NULL, NULL}
};

