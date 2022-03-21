/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/**
 * @file
 * @brief       Shell for testing the SX1280 as a LoRa 2.4 GHz modem.
 *
 * @author      Nicolas Albarel
 * @author      Didier Donsez
 * @author      Olivier Alphand
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"


#include "shell_commands.h"
#include "shell.h"

#include "sx1280_cmd.h"
#include "sx1280_rangetest_cmd.h"

#if RANGETEST != 1

static const shell_command_t shell_commands[] = {
    { "setup", "Setup the sx1280 parameters for rx/tx", sx1280_setup_cmd },
    { "channel", "Get/Set channel frequency (in Hz)", sx1280_channel_cmd },
    //    {"reset", "Reset the sx1280 device", sx1280_reset_cmd},
    //    {"status", "Get the sx1280 status", sx1280_status_cmd},
    { "preamble", "Get/Set the preamble length", sx1280_preamble_cmd },
    { "syncword", "Get/Set the sync word", sx1280_syncword_cmd },
    { "invert_iq", "Get/Set IQ swapping", sx1280_invertiq_cmd },
    { "crc", "Get/Set CRC on/off", sx1280_crc_cmd },
    { "payload", "Get/Set Payload fix/var", sx1280_payload_cmd },
    { "txpower", "Get/Set the TX power", sx1280_txpower_cmd },
    { "invert_iq", "Get/Set IQ swapping", sx1280_invertiq_cmd },
    //    { "ping", "Ping", sx1280_ping_cmd},
    //    { "pong", "Pong", sx1280_pong_cmd},
    //    { "tx", "Sends a message with sx1280", sx1280_tx_cmd },
    { "send", "Sends a message with sx1280", sx1280_send_cmd },
    { "listen", "Waits for a message with sx1280", sx1280_listen_cmd },
    { "rangetest", "Range Test", sx1280_rangetest_cmd },
    { NULL, NULL, NULL }
};
#endif

int main(void)
{
    sx1280_init_and_reboot_on_failure();

#if RANGETEST == 1
    char *argv[] = { "rangetest", "1000000", "24", "5", "2422000", "2425000", "2479000" };
    // rangetest <number of packet> <packet size> <delay in seconds> <channel1_in_khz> <channel2_in_khz> <channel3_in_khz>
    (void)sx1280_rangetest_cmd(7, argv );
    puts("rebooting ...");
    pm_reboot();
#else
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* define own shell commands */
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
#endif

    return 0;
}
