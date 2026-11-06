/*
 * Copyright (C) 2020 Antonio Galea <antonio.galea@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       ATA8510 test
 *
 * @author      Antonio Galea <antonio.galea@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "shell_commands.h"
#include "ps.h"
#include "od.h"

#include "ata8510_params.h"
#include "ata8510.h"

static ata8510_t dev;

#define ENABLE_DEBUG (0)
#include "debug.h"

/* ---------------------------------------------------------------------------- */

int cmd_onoff(int argc, char **argv)
{
    (void)argc;

    if (strcmp(argv[0], "on") == 0) {
        ata8510_poweron(&dev);
    }
    else if (strcmp(argv[0], "off") == 0) {
        ata8510_poweroff(&dev);
    }
    return 0;
}

int cmd_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ata8510_events_t *ev = &dev.events;

    ata8510_get_event_bytes(&dev);
    printf("System: SYS_ERR=%d CMD_RDY=%d SYS_RDY=%d AVCCLOW=%d LOWBATT=%d "
           "SFIFO=%d DFIFO_RX=%d DFIFO_TX=%d\n",
           ev->system.val.sys_err, ev->system.val.cmd_rdy,
           ev->system.val.sys_rdy, ev->system.val.avcclow,
           ev->system.val.lowbatt, ev->system.val.sfifo,
           ev->system.val.dfifo_rx, ev->system.val.dfifo_tx);
    printf("Events: IDCHKA=%d WCOKA=%d SOTA=%d EOTA=%d IDCHKB=%d WCOKB=%d "
           "SOTB=%d EOTB=%d\n",
           ev->events.val.idchka, ev->events.val.wcoka, ev->events.val.sota,
           ev->events.val.eota, ev->events.val.idchkb, ev->events.val.wcokb,
           ev->events.val.sotb, ev->events.val.eotb);
    printf("Power:  PWRON=%d NPWRON6=%d NPWRON5=%d NPWRON4=%d NPWRON3=%d "
           "NPWRON2=%d NPWRON1=%d\n",
           ev->power.val.pwron, ev->power.val.npwron6, ev->power.val.npwron5,
           ev->power.val.npwron4, ev->power.val.npwron3, ev->power.val.npwron2,
           ev->power.val.npwron1);
    printf("Config: PATHB=%d PATHA=%d CH=%d SER=%d\n",
           ev->config.val.pathb, ev->config.val.patha, ev->config.val.ch,
           ev->config.val.ser);

    return 0;
}

int cmd_reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    ata8510_system_reset(&dev);
    return 0;
}

int cmd_mode(int argc, char **argv)
{
    ata8510_system_mode_config_opm_t opm;

    if (argc == 1) {
        char *mode;
        switch (dev.internal_state) {
            case ATA8510_MODE_IDLE:
                mode = "IDLE";
                break;
            case ATA8510_MODE_TX:
                mode = "TX";
                break;
            case ATA8510_MODE_RX:
                mode = "RX";
                break;
            case ATA8510_MODE_RX_POLLING:
                mode = "POLLING";
                break;
            default:
                mode = "UNDEFINED";
                break;
        }
        printf("Mode: %s\n", mode);
    }
    else {
        if (strcmp(argv[1], "idle") == 0) {
            opm = ATA8510_MODE_IDLE;
        }
        else if (strcmp(argv[1], "tx") == 0) {
            opm = ATA8510_MODE_TX;
        }
        else if (strcmp(argv[1], "rx") == 0) {
            opm = ATA8510_MODE_RX;
        }
        else if (strcmp(argv[1], "polling") == 0) {
            opm = ATA8510_MODE_RX_POLLING;
        }
        else {
            printf("usage: %s <idle|tx|rx|polling>\n", argv[0]);
            return -1;
        }
        ata8510_set_mode(&dev, opm);
    }
    return 0;
}

int cmd_send(int argc, char **argv)
{
    uint8_t n;

    if (argc < 2) {
        printf("usage: %s <message>\n", argv[0]);
        return 1;
    }

    n = strlen(argv[1]);
    printf("Sending %d bytes: '%s'\n", n, argv[1]);
    ata8510_send_frame(&dev, (uint8_t *)argv[1], n);
    return 0;
}

int cmd_power(int argc, char **argv)
{
    int power;

    if (argc == 1) {
        power = ata8510_get_radio_power(&dev);
        printf("Power is: %d\n", power);
    }
    else {
        power = atoi(argv[1]);
        printf("Trying to set power to %d\n", power);
        ata8510_set_radio_power(&dev, power);
        power = ata8510_get_radio_power(&dev);
        printf("Power now is: %d\n", power);
    }
    return 0;
}

int cmd_service_channel(int argc, char **argv)
{
    ata8510_service_t service;
    ata8510_channel_t channel;

    ata8510_get_radio_service_channel(&dev, &service, &channel);

    if (argc == 1) {
        if (strcmp(argv[0], "service") == 0) {
            printf("Service is: %d\n", service);
        }
        else if (strcmp(argv[0], "channel") == 0) {
            printf("Channel is: %d\n", channel);
        }
    }
    else {
        int value = atoi(argv[1]);
        if (strcmp(argv[0], "service") == 0) {
            printf("Trying to set service to %d\n", value);
            ata8510_set_radio_service_channel(&dev, value, channel);
            ata8510_get_radio_service_channel(&dev, &service, &channel);
            printf("Service is: %d\n", service);
        }
        else if (strcmp(argv[0], "channel") == 0) {
            printf("Trying to set channel to %d\n", value);
            ata8510_set_radio_service_channel(&dev, service, value);
            ata8510_get_radio_service_channel(&dev, &service, &channel);
            printf("Channel is: %d\n", channel);
        }
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "on", "Turn on the radio", cmd_onoff },
    { "off", "Turn off the radio", cmd_onoff },
    { "status", "Get the radio status", cmd_status },
    { "reset", "Reset the radio", cmd_reset },
    { "mode", "Get/set the radio mode", cmd_mode },
    { "send", "Send a string through radio", cmd_send },
    { "power", "Get/set radio power", cmd_power },
    { "service", "Get/set radio service", cmd_service_channel },
    { "channel", "Get/set radio channel", cmd_service_channel },
    { NULL, NULL, NULL }
};

void recv(uint8_t *buffer, int len, int dbm)
{
    printf("RECEIVED %d bytes, dBm=%d\n", len, dbm);
    od_hex_dump(buffer, len, 0);
    free(buffer);
}

int main(void)
{
    puts("ATA8510 test application\n");

    switch (ata8510_init(&dev, &ata8510_params[0])) {
        case -ATA8510_ERR_MALLOC:
            puts("[Error] Cannot allocate memory.");
            return -1;
        case -ATA8510_ERR_SIGNATURE:
            puts("[Error] Wrong device signature found.");
            return -1;
        case -ATA8510_ERR_SPI:
            puts("[Error] An error occurred when initializing SPI bus.");
            return -1;
        case -ATA8510_ERR_GPIO_INT:
            puts("[Error] An error occurred when initializing interrupt pin.");
            return -1;
        case -ATA8510_ERR_GPIO_POWER:
            puts("[Error] An error occurred when initializing poweron pin.");
            return -1;
        case -ATA8510_ERR_GPIO_RESET:
            puts("[Error] An error occurred when initializing reset- pin.");
            return -1;
    }
    dev.receive_cb = recv;

    /* start the shell */
    puts("Initialization OK, starting the shell now");

    char line_buf[255];
    shell_run(shell_commands, line_buf, 255);

    return 0;
}
