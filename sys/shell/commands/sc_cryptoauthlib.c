/*
 * Copyright (C) 2020 HAW Hamburg
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
 * @brief       Shell commands for the cryptoauthlib module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdint.h>
#include "cryptoauthlib.h"

void get_bin(char *result, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        result[i] = (((byte << i) & 0x80) ? '1' : '0');
    }
    result[8] = '\0';
}

int _read_config(void)
{
    uint8_t data[ATCA_ECC_CONFIG_SIZE];
    uint8_t data_count = 0;
    char binary[9];

    memset(data, 0, ATCA_ECC_CONFIG_SIZE);

    if (atcab_read_config_zone(data) != ATCA_SUCCESS) {
        printf("Error reading config zone\n");
        return 1;
    }

    printf("Config zone: \n\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN0 SN1 SN2 SN3\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("RN0 RN1 RN2 RN3\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN4 SN5 SN6 SN7\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN8 RSVD I2CE RSVD\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("I2CA RSVD OTPM CM\n");

    for (int i = 0; i < 32; i += 4) {
        static int slotcount = 0;
        printf("%03d:%03d ", data_count, data_count+3);
        for (int j = 0; j < 4; j++) {
            get_bin(binary, data[data_count]);
            printf("%s ", binary);
            data_count++;
        }
        printf("SC%d SC%d ", slotcount, slotcount);
        slotcount++;
        printf("SC%d SC%d\n", slotcount, slotcount);
        slotcount++;
    }

    for (int k = 0; k < 2; k++) {
        static int cnt_no = 0;
        for (int i = 0; i < 8; i += 4) {
            printf("%03d:%03d ", data_count, data_count+3);
            for (int j = 0; j < 4; j++) {
                get_bin(binary, data[data_count]);
                printf("%s ", binary);
                data_count++;
            }
            printf("CNT%d CNT%d CNT%d CNT%d\n", cnt_no, cnt_no, cnt_no, cnt_no);
        }
        cnt_no++;
    }

    for (int i = 0; i < 16; i += 4) {
        printf("%03d:%03d ", data_count, data_count+3);
        for (int j = 0; j < 4; j++) {
            get_bin(binary, data[data_count]);
            printf("%s ", binary);
            data_count++;
        }
        printf("LKU%d LKU%d LKU%d LKU%d\n", i, i+1, i+2, i+3);
    }

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("UE SEL LV LC\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SL0 SL1 RFU0 RFU1\n");

    printf("%03d:%03d ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("X509-0 X509-1 X509-2 X509-3\n");

    for (int i = 0; i < 32; i += 4) {
        static int key_cnt = 0;
        printf("%03d:%03d ", data_count, data_count+3);
        for (int j = 0; j < 4; j++) {
            get_bin(binary, data[data_count]);
            printf("%s ", binary);
            data_count++;
        }
        printf("KC%d KC%d ", key_cnt, key_cnt);
        key_cnt++;
        printf("KC%d KC%d\n", key_cnt, key_cnt);
        key_cnt++;
    }

    return 0;
}

int _check_lock_config(void)
{
    bool is_locked = false;
    atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);

    if (is_locked) {
        printf("Config zone is locked\n");
    }
    else {
        printf("Config zone is not locked\n");
    }

    return 0;
}

int _check_lock_data(void)
{
    bool is_locked = false;
    atcab_is_locked(LOCK_ZONE_DATA, &is_locked);

    if (is_locked) {
        printf("Data zone is locked\n");
    }
    else {
        printf("Data zone is not locked\n");
    }

    return 0;
}

int _lock_config(void)
{
    bool is_locked = false;
    atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);

    if (is_locked) {
        printf("Error: Config zone is already locked\n");
        return 1;
    }

    if (atcab_lock_config_zone() != ATCA_SUCCESS) {
        printf("Error: Locking failed\n");
        return 1;
    }

    printf("Device successfully locked\n");
    return 0;
}

int _cryptoauth(int argc, char **argv)
{
    if (argc > 1) {
        if ((strcmp(argv[1], "read") == 0)) {
            return _read_config();
        }
        else if ((strcmp(argv[1], "lock_c") == 0)) {
            return _lock_config();
        }
        else if ((strcmp(argv[1], "check_lc") == 0)) {
            return _check_lock_config();
        }
        else if ((strcmp(argv[1], "check_ld") == 0)) {
            return _check_lock_data();
        }
    }
    else {
        printf("* read - read Microchip CryptoAuth device's config zone\n");
        printf("* lock_c - PERMANENTLY lock Microchip CryptoAuth device's config \
        zone (cannot be undone!)\n");
        printf("* check_lc - check if Microchip CryptoAuth device's config zone is locked\n");
        printf("* check_ld - check if Microchip CryptoAuth device's data zone is locked\n");
    }
    return 0;
}
