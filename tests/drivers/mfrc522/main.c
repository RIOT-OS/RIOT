/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @brief   Test application for the MFRC522 controller
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ztimer.h"
#include "shell.h"

#include "mfrc522.h"
#include "mfrc522_params.h"

static mfrc522_t mfrc522_dev;

/* Stores scanned ID read from RFID Module */
static uint8_t read_card[4];

int get_uid(mfrc522_uid_t *uid, bool stop_read)
{
    int rc = 0;

    /* Getting ready for Reading PICCs
     * If a new PICC placed to RFID reader continue
     */
    if (!(rc = mfrc522_picc_is_new_card_present(&mfrc522_dev))) {
        puts("No card detected ...");
        return 0;
    }

    puts("Card detected. Read card serial ...");

    /* Since a PICC placed get Serial and continue */
    rc = mfrc522_picc_read_card_serial(&mfrc522_dev, uid);
    if (rc != 0) {
        return 0;
    }

    printf("Scanned PICC's UID: ");
    for (uint8_t i = 0; i < uid->size; i++) {
        read_card[i] = uid->uid_byte[i];
        printf("%x", read_card[i]);
    }
    printf("\n");

    /* Stop reading */
    if (stop_read) {
        mfrc522_picc_halt_a(&mfrc522_dev);
    }

    return 1;
}

int scan_uid(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_uid_t uid;
    uint8_t success_read;

    int i = 0;
    do {
        if (i >= 1) {
            printf("Repeat scan (%d/20) ...\n", i + 1);
        }
        success_read = get_uid(&uid, true);
        ztimer_sleep(ZTIMER_MSEC, 500);
        i++;
    } while (!success_read && i < 20);

    return 0;
}

int self_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    bool success = mfrc522_pcd_perform_self_test(&mfrc522_dev);

    success ? printf("SUCCESS\n") : printf("FAILED\n");

    /* After self-test the device is unusable => repeat the initialization routine */
    int rc = 0;
    if ((rc = mfrc522_pcd_init(&mfrc522_dev, &mfrc522_params[0])) != 0) {
        printf("Initialization failed! rc = %d", rc);
        return 1;
    }

    return 0;
}

int reset(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_pcd_reset(&mfrc522_dev);

    return 0;
}

int antenna_on(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_pcd_antenna_on(&mfrc522_dev);

    return 0;
}

int antenna_off(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_pcd_antenna_off(&mfrc522_dev);

    return 0;
}

int set_antenna_gain(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc != 2) {
        puts("usage: set_antenna_gain <gain (0-5)>");
        puts("       0: 18db, 1: 23db, 2: 33db, 3: 38db, 4: 43db, 5: 48db");
        return 1;
    }
    else {
        uint8_t input = atoi(argv[1]);
        mfrc522_pcd_rx_gain_t gain;

        switch (input) {
        case 0:
            gain = MFRC522_RXGAIN_18_DB;
            break;
        case 1:
            gain = MFRC522_RXGAIN_23_DB;
            break;
        case 2:
            gain = MFRC522_RXGAIN_33_DB;
            break;
        case 3:
            gain = MFRC522_RXGAIN_38_DB;
            break;
        case 4:
            gain = MFRC522_RXGAIN_43_DB;
            break;
        case 5:
            gain = MFRC522_RXGAIN_48_DB;
            break;
        default:
            puts("gain needs to be one of [0-5]");
            puts("0: 18db, 1: 23db, 2: 33db, 3: 38db, 4: 43db, 5: 48db");
            return 1;
        }

        mfrc522_pcd_set_antenna_gain(&mfrc522_dev, gain);
    }

    return 0;
}

int get_antenna_gain(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_pcd_rx_gain_t gain = mfrc522_pcd_get_antenna_gain(&mfrc522_dev);

    switch (gain) {
    case MFRC522_RXGAIN_18_DB:
    /* fall-through */
    case MFRC522_RXGAIN_18_DB_2:
        printf("gain = MFRC522_RXGAIN_18_DB\n");
        break;
    case MFRC522_RXGAIN_23_DB:
    /* fall-through */
    case MFRC522_RXGAIN_23_DB_2:
        printf("gain = MFRC522_RXGAIN_23_DB\n");
        break;
    case MFRC522_RXGAIN_33_DB:
        printf("gain = MFRC522_RXGAIN_33_DB\n");
        break;
    case MFRC522_RXGAIN_38_DB:
        printf("gain = MFRC522_RXGAIN_38_DB\n");
        break;
    case MFRC522_RXGAIN_43_DB:
        printf("gain = MFRC522_RXGAIN_43_DB\n");
        break;
    case MFRC522_RXGAIN_48_DB:
        printf("gain = MFRC522_RXGAIN_48_DB\n");
        break;
    default:
        printf("gain = UNKNOWN\n");
        break;
    }

    return 0;
}

int firmware_version(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_pcd_dump_version_to_serial(&mfrc522_dev);

    return 0;
}

int picc_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mfrc522_uid_t uid;
    if (get_uid(&uid, false) != 1) {
        return 1;
    }

    mfrc522_picc_dump_to_serial(&mfrc522_dev, &uid);

    return 0;
}

int set_uid(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc != 5) {
        puts("usage: set_uid <byte1> <byte2> <byte3> <byte4>");
        puts("       <byte> as hex value (e.g. 0xA9 or A9)");
        return 1;
    }
    else {
        mfrc522_uid_t uid;
        if (get_uid(&uid, false) != 1) {
            return 1;
        }

        uint8_t byte_1 = (uint8_t)strtol(argv[1], NULL, 16);
        uint8_t byte_2 = (uint8_t)strtol(argv[2], NULL, 16);
        uint8_t byte_3 = (uint8_t)strtol(argv[3], NULL, 16);
        uint8_t byte_4 = (uint8_t)strtol(argv[4], NULL, 16);

        uint8_t uid_buf[4] = { byte_1, byte_2, byte_3, byte_4 };

        int status = mfrc522_mifare_set_uid(&mfrc522_dev, &uid, uid_buf, 4);
        printf("status = %d\n", status);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "reset", "Reset the device.", reset },
    { "antenna_on", "Turn the antenna on.", antenna_on },
    { "antenna_off", "Turn the antenna off.", antenna_off },
    { "set_antenna_gain", "Set antenna gain.", set_antenna_gain },
    { "get_antenna_gain", "Get antenna gain.", get_antenna_gain },
    { "firmware_version", "Print the firmware version.", firmware_version },
    { "picc_dump", "Dumps debug info about a scanned PICC.", picc_dump },
    { "scan_uid", "Scan repeatedly until a PICC's UID is scanned.", scan_uid },
    { "set_uid", "Set the UID of a scanned PICC.", set_uid },
    { "self_test", "Run the self test.", self_test },
    { NULL, NULL, NULL }
};

int main(void)
{
    int rc = 0;

    if ((rc = mfrc522_pcd_init(&mfrc522_dev, &mfrc522_params[0])) != 0) {
        printf("Initialization failed! rc = %d", rc);
        return 1;
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
