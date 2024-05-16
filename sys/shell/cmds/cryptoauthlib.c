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
#include <string.h>
#include <stdint.h>

#include "atca.h"
#include "atca_params.h"
#include "cryptoauthlib.h"
#include "shell.h"

#define ATCA_CONFIG_READ_ONLY_BYTES (15)
#define ATCA_KEY_SLOT_COUNT         (15)
#define ATCA_X509_KEY_COUNT         (4)

/**
 * @brief   Convert one byte into a binary number
 *
 * @param[out] result   Will contain binary number as a string
 * @param[in]  byte     Byte to convert
 */
void get_bin(char *result, uint8_t byte)
{
    for (int i = 0; i < 8; i++) {
        result[i] = (((byte << i) & 0x80) ? '1' : '0');
    }
    result[8] = '\0';
}

/**
 * @brief   Converts an ATCA device type into a string
 *
 * @param   devtype     ATCADeviceType
 * @return  char*       Device type as string
 */
static char* _convert_atca_devtype(ATCADeviceType devtype)
{
    switch(devtype) {
        case ATSHA204A:
            return "ATSHA204A";
        case ATECC108A:
            return "ATECC108A";
        case ATECC508A:
            return "ATECC508A";
        case ATECC608:
            return "ATECC608";
        case ATSHA206A:
            return "ATSHA206A";
        case ECC204:
            return "ECC204";
        case TA100:
            return "TA100";
        default:
            return "ATCA_DEV_UNKNOWN";
    }
}

/**
 * @brief   Print read-only section of the configuration zone.
 *
 *          Reserved bytes that don't mean anything are omitted.
 *
 * @param[in]   data           Pointer to buffer with input data
 * @param       data_size      Size of input buffer
 * @param[in]   data_count     Current data count
 * @param[out]  communications Contains the device communications mode (0 = Single Wire, 1 = I2C)
 * @return      uint8_t        Return updated @p data_count
 * @return                     -1 if there are no bytes left in data
 */
static uint8_t _print_dev_info_ro(uint8_t* data, size_t data_size, uint8_t data_count, uint8_t* communications)
{
    if ((size_t)data_count >= data_size - ATCA_CONFIG_READ_ONLY_BYTES) {
        return -1;
    }

    printf("Serial No (Pt. 1) | ");
    for (int i = 0; i < 4; i++) {
        printf("0x%02x ", data[data_count]);
        data_count++;
    }
    puts("");

    printf("Revision No.      | ");
    for (int i = 0; i < 4; i++) {
        printf("0x%02x ", data[data_count]);
        data_count++;
    }
    puts("");

    printf("Serial No (Pt. 2) | ");
    for (int i = 0; i < 5; i++) {
        printf("0x%02x ", data[data_count]);
        data_count++;
    }
    puts("");

    /* Skip reserved byte */
    data_count++;
    *communications = data[data_count];
    if (*communications & 0x01) {
        printf("I2C Enabled       | True\n");
        printf("Single Wire       | False\n");
    }
    else {
        printf("I2C Enabled       | False\n");
        printf("Single Wire       | True\n");
    }

    /* Skip reserved byte */
    data_count += 2;
    return data_count;
}

/**
 * @brief   Get authorization mode
 *
 * @param   data    Byte of data to interprete
 * @return  char*   Data interpretation as string
 */
static char* _get_authorization_mode(uint8_t data)
{
    uint8_t auth_mode = data & 0x08;
    return auth_mode ? "Authorization Output Mode" : "Intrustion Detection Mode";
}

/**
 * @brief   Prints the I2C address when I2C is enabled or the
 *          GPIO mode when Single Wire communication is enabled
 *
 * @param   data            Byte of data to interprete
 * @param   communications  Device communications mode (0 = Single Wire, 1 = I2C)
 */
static void _print_i2c_addr_or_gpio_mode(uint8_t data, uint8_t communications)
{
    if (communications) {
        /* I2C enabled */
        printf("I2C Address       | 0x%02x\n", data);
    }
    else {
        /* Single Wire enabled */
        uint8_t gpio_mode = data & 0x03;
        printf("GPIO Mode         | ");
        switch(gpio_mode) {
            case 0x01:
                printf("%s\n", _get_authorization_mode(data));
                break;
            case 0x10:
                puts("Input");
                break;
            case 0x11:
                puts("Output");
                break;
            default:
                puts("Disabled");
        }
        uint8_t gpio_default = data & 0x04;
        if (gpio_default) {
            puts("GPIO Default      | high");
        }
        else {
            puts("GPIO Default      | low");
        }
        if (gpio_mode) {
            if (gpio_mode == 0x01) {
                printf("SignalKey for authorization has ID %d\n", data & 0xF0);
            }
            else {
                printf("SignalKey/KeyID is non-zero and invalid: 0x%02x\n", data & 0xF0);
            }
        }
    }
}

/**
 * @brief   Prints the OTP mode.
 *
 * @param   data Byte of data to interprete
 */
static void _print_otp_mode(uint8_t data)
{
    uint8_t otp_mode = data;
    switch(otp_mode) {
        case 0xAA:
            printf("OTP Mode          | Read-only\n");
            break;
        case 0x55:
            printf("OTP Mode          | Consumption\n");
            break;
        default:
            printf("OTP Mode          | None\n");
    }
}

/**
 * @brief   Prints the chip mode
 *
 * @param   data Byte of data to interprete
 */
static void _print_chip_mode(uint8_t data)
{
    puts("ChipMode:");
    if (data & 0x01) {
        puts("SelectorMode      | Only writeable if zero");
    }
    else {
        puts("SelectorMode      | Always writeable");
    }

    if (data & 0x02) {
        puts("TTLenable         | Input levels VCC referenced");
    }
    else {
        puts("TTLenable         | Fixed input levels");
    }

    if (data & 0x04) {
        puts("Watchdog          | 10 sec (not recommended)");
    }
    else {
        puts("Watchdog          | 1.3 sec (recommended)");
    }
}

/**
 * @brief   Print Key Slot Configurations in hexadecimal and binary format
 *
 * @param[in]   data        Pointer to buffer with input data
 * @param       data_size   Size of input buffer
 * @param[in]   data_count  Current data count
 * @return      uint8_t     Return updated @p data_count
 * @return                  -1 if there are no bytes left in data
 */
static uint8_t _print_slot_config(uint8_t* data, size_t data_size, uint8_t data_count)
{
    size_t atca_slot_config_bytes = ATCA_KEY_SLOT_COUNT*2;
    if ((size_t)data_count >= data_size - atca_slot_config_bytes) {
        return -1;
    }

    char binary[9];
    puts("Slot Config");
    puts("----------------------------------------");
    puts("SlotID  | Hex    | Binary");
    puts("        |        | 7      0 | 15     8");
    puts("--------+--------+----------------------");
    for (int i = 0; i < 32; i += 2) {
        static int slotcount = 0;
        if (slotcount < 10) {
            printf("%d       | 0x%02x%02x | ", slotcount, data[data_count], data[data_count+1]);
        }
        else {
            printf("%d      | 0x%02x%02x | ", slotcount, data[data_count], data[data_count+1]);
        }
        for (int j = 0; j < 2; j++) {
            get_bin(binary, data[data_count]);
            printf("%s | ", binary);
            data_count++;
        }
        puts("");
        slotcount++;
    }
    puts("");

    return data_count;
}

/**
 * @brief       Print lock status of key slots
 *
 * @param[in]   data        Pointer to buffer with input data
 * @param       data_size   Size of input buffer
 * @param[in]   data_count  Current data count
 * @return      uint8_t     Return updated @p data_count
 * @return                  -1 if there are no bytes left in data
 */
static uint8_t _print_slot_lock(uint8_t* data, size_t data_size, uint8_t data_count)
{
    if ((size_t)data_count >= data_size - ATCA_KEY_SLOT_COUNT) {
        return -1;
    }
    puts("\nSlotLocked (X = locked, - = unlocked):");
    puts("Slot   |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15");
    printf("Locked | ");
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 8; i++) {
            if ((data[data_count] >> i) & 0x01) {
                printf(" X ");
            }
            else {
                printf(" - ");
            }
            data_count++;
        }
    }
    puts("");
    return data_count;
}

/**
 * @brief   Print X509 format restrictions
 *
 * @param[in]   data        Pointer to buffer with input data
 * @param       data_size   Size of input buffer
 * @param[in]   data_count  Current data count
 * @return      uint8_t     Return updated @p data_count
 * @return                  -1 if there are no bytes left in data
 */
static uint8_t _print_x509_format(uint8_t* data, size_t data_size, uint8_t data_count)
{
    if ((size_t)data_count >= data_size - ATCA_X509_KEY_COUNT) {
        return -1;
    }

    puts("\nX509 Format:");
    for (int i = 0; i < 4; i++) {
        if (data[data_count+i] == 0x00) {
            printf("PubKey %d         | No restrictions\n", i);
        }
        else {
            printf("PubKey %d:\n", i);
            printf("- PublicPosition  | 0x%02x\n", data[data_count+i] & 0x0F);
            printf("- TemplateLength  | 0x%02x\n", data[data_count+i] & 0xF0);
        }
    }
    return data_count += 4;
}

/**
 * @brief   Print key configurations
 *
 * @param[in]   data        Pointer to buffer with input data
 * @param       data_size   Size of input buffer
 * @param[in]   data_count  Current data count
 * @return      uint8_t     Return updated @p data_count
 * @return                  -1 if there are no bytes left in data
 */
static uint8_t _print_key_config(uint8_t* data, size_t data_size, uint8_t data_count)
{
    size_t atca_key_config_bytes = ATCA_KEY_SLOT_COUNT*2;
    if ((size_t)data_count >= data_size - atca_key_config_bytes) {
        return -1;
    }

    char binary[9];
    puts("\nKey Config");
    puts("----------------------------------------");
    puts("SlotID  | Hex    | Binary");
    puts("        |        | 7      0 | 15     8");
    puts("--------+--------+----------------------");
    for (int i = 0; i < 32; i += 2) {
        static int slotcount = 0;
        if (slotcount < 10) {
            printf("%d       | 0x%02x%02x | ", slotcount, data[data_count], data[data_count+1]);
        }
        else {
            printf("%d      | 0x%02x%02x | ", slotcount, data[data_count], data[data_count+1]);
        }
        for (int j = 0; j < 2; j++) {
            get_bin(binary, data[data_count]);
            printf("%s | ", binary);
            data_count++;
        }
        puts("");
        slotcount++;
    }
    puts("");

    return data_count;
}

static int _read_config(ATCADevice dev)
{
    uint8_t data[ATCA_ECC_CONFIG_SIZE];
    int data_count = 0;
    uint8_t communications;

    memset(data, 0, ATCA_ECC_CONFIG_SIZE);

    int status = calib_read_config_zone(dev, data);
    if (status != ATCA_SUCCESS) {
        printf("Error reading config zone\n");
        return 1;
    }

    printf("Config Zone\n");
    puts("Device Info (Read Only)");
    puts("--------------------------------------------");
    ATCADeviceType devtype = dev->mIface.mIfaceCFG->devtype;
    printf("Device Type       | %s\n", _convert_atca_devtype(devtype));
    if (devtype == ATECC608) {
        puts("\n***************************Note***************************");
        printf("This interpretation is valid for ATECC508A devices.\nIf you have an ATECC608, you'll need to get the datasheet\n(NDA signature required) and interprete it yourself.\nThe config zone is very similar, so you can print it\nin binary format with the 'read_bin' command and\nmatch the corresponding bytes.\n");
        puts("**********************************************************\n");
    }
    data_count = _print_dev_info_ro(data, sizeof(data), data_count, &communications);
    if (data_count == -1) {
        printf("Error formatting read-only config zone\n");
        return 1;
    }
    puts("--------------------------------------------\n");

    puts("Device Info (Writable)");
    puts("---------------------------------------------");
    _print_i2c_addr_or_gpio_mode(data[data_count], communications);
    data_count += 2;

    _print_otp_mode(data[data_count]);
    data_count++;

    _print_chip_mode(data[data_count]);
    data_count++;
    puts("");

    data_count = _print_slot_config(data, sizeof(data), data_count);
    if (data_count == -1) {
        printf("Error formatting read-only config zone\n");
        return 1;
    }

    printf("Counter 0         | 0x%02x 0x%02x 0x%02x 0x%02x\n", data[data_count],
                                                                data[data_count+1],
                                                                data[data_count+2],
                                                                data[data_count+3]);
    data_count += 4;

    printf("Counter 1         | 0x%02x 0x%02x 0x%02x 0x%02x\n", data[data_count],
                                                                data[data_count+1],
                                                                data[data_count+2],
                                                                data[data_count+3]);
    data_count += 4;

    printf("LastKeyUse        | 0x%02x\n", data[data_count]);
    data_count++;

    printf("UserExtra         | 0x%02x\n", data[data_count]);
    data_count++;

    printf("Selector          | 0x%02x\n", data[data_count]);
    data_count++;

    if (data[data_count] == 0x00) {
        puts("LockValue         | Data and OTP Locked");
    }
    else {
        puts("LockValue         | Data and OTP Unlocked");
    }
    data_count++;

    if (data[data_count] == 0x00) {
        puts("LockConfig        | Config Zone Locked");
    }
    else {
        puts("LockConfig        | Config Zone Unlocked");
    }
    data_count++;

    data_count = _print_slot_lock(data, sizeof(data), data_count);
    if (data_count == -1) {
        printf("Error formatting read-only config zone\n");
        return 1;
    }

    data_count = _print_x509_format(data, sizeof(data), data_count);
    if (data_count == -1) {
        printf("Error formatting read-only config zone\n");
        return 1;
    }

    data_count = _print_key_config(data, sizeof(data), data_count);
    if (data_count == -1) {
        printf("Error formatting read-only config zone\n");
        return 1;
    }
    return 0;
}

static int _read_config_bin(ATCADevice dev)
{
    uint8_t data[ATCA_ECC_CONFIG_SIZE];
    int data_count = 0;
    char binary[9];

    memset(data, 0, ATCA_ECC_CONFIG_SIZE);

    int status = calib_read_config_zone(dev, data);
    if (status != ATCA_SUCCESS) {
        printf("Error reading config zone\n");
        return 1;
    }

    puts("ConfigZone Binary Data:");
    puts("---------------------------------------------------");
    puts("Bytes \\ Bits | 7      0 7      0 7      0 7      0");
    puts("-------------+-------------------------------------");
    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN0 SN1 SN2 SN3\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("RN0 RN1 RN2 RN3\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN4 SN5 SN6 SN7\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SN8 RSVD I2CE RSVD\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("I2CA RSVD OTPM CM\n\n");

    puts("Slot Config");
    puts("---------------------------------------------------");
    puts("Bytes \\ Bits | 7      0 15     8 7      0 15     8");
    puts("-------------+-------------------------------------");
    for (int i = 0; i < 32; i += 4) {
        static int slotcount = 0;
        printf("%03d:%03d      | ", data_count, data_count+3);
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
            printf("%03d:%03d      | ", data_count, data_count+3);
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
        printf("%03d:%03d      | ", data_count, data_count+3);
        for (int j = 0; j < 4; j++) {
            get_bin(binary, data[data_count]);
            printf("%s ", binary);
            data_count++;
        }
        printf("LKU%d LKU%d LKU%d LKU%d\n", i, i+1, i+2, i+3);
    }

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("UE SEL LV LC\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("SL0 SL1 RFU0 RFU1\n");

    printf("%03d:%03d      | ", data_count, data_count+3);
    for (int i = 0; i < 4; i++) {
        get_bin(binary, data[data_count]);
        printf("%s ", binary);
        data_count++;
    }
    printf("X509-0 X509-1 X509-2 X509-3\n\n");

    puts("Key Config");
    puts("---------------------------------------------------");
    puts("Bytes \\ Bits | 7      0 15     8 7      0 15     8");
    puts("-------------+-------------------------------------");
    for (int i = 0; i < 32; i += 4) {
        static int key_cnt = 0;
        printf("%03d:%03d      | ", data_count, data_count+3);
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

static int _check_lock_config(ATCADevice dev)
{
    bool is_locked_config = false;
    calib_is_locked(dev, LOCK_ZONE_CONFIG, &is_locked_config);

    if (is_locked_config) {
        printf("Config zone is locked\n");
    }
    else {
        printf("Config zone is not locked\n");
    }

    return 0;
}

static int _check_lock_data(ATCADevice dev)
{
    bool is_locked_data = false;
    calib_is_locked(dev, LOCK_ZONE_DATA, &is_locked_data);

    if (is_locked_data) {
        printf("Data zone is locked\n");
    }
    else {
        printf("Data zone is not locked\n");
    }

    return 0;
}

static int _lock_config(ATCADevice dev)
{
    bool is_locked_config = false;
    calib_is_locked(dev, LOCK_ZONE_CONFIG, &is_locked_config);

    if (is_locked_config) {
        printf("Error: Config zone is already locked\n");
        return 1;
    }

    if (calib_lock_config_zone(dev) != ATCA_SUCCESS) {
        printf("Error: Locking failed\n");
        return 1;
    }

    printf("Config Zone successfully locked\n");
    return 0;
}

static int _lock_data(ATCADevice dev)
{
    bool is_locked_data = false;
    calib_is_locked(dev, LOCK_ZONE_DATA, &is_locked_data);

    if (is_locked_data) {
        printf("Error: Data zone is already locked\n");
        return 1;
    }

    if (calib_lock_data_zone(dev) != ATCA_SUCCESS) {
        printf("Error: Locking failed\n");
        return 1;
    }

    printf("Data Zone successfully locked\n");
    return 0;
}

static int _set_dev(char* id, ATCADevice dev)
{
    int index = atoi(id);
    if (index > (int)ATCA_NUMOF-1) {
        printf("Invalid ID, can be 0 - %d\n", ATCA_NUMOF-1);
        return 1;
    }
    atcab_init_ext(&dev, (ATCAIfaceCfg *)&atca_params[index]);
    return 0;
}

static int _atca(int argc, char **argv)
{
    ATCADevice dev;
    atcab_init_ext(&dev, (ATCAIfaceCfg *)&atca_params[0]);

    if (argc > 1) {
        if ((strcmp(argv[1], "set_dev") == 0)) {
            if (argc < 3) {
                puts("Please enter valid device index number");
                return 1;
            }
            return _set_dev(argv[2], dev);
        }
        if ((strcmp(argv[1], "read") == 0)) {
            return _read_config(dev);
        }
        else if ((strcmp(argv[1], "read_bin") == 0)) {
            return _read_config_bin(dev);
        }
        else if ((strcmp(argv[1], "lock_c") == 0)) {
            return _lock_config(dev);
        }
        else if ((strcmp(argv[1], "lock_d") == 0)) {
            return _lock_data(dev);
        }
        else if ((strcmp(argv[1], "check_lc") == 0)) {
            return _check_lock_config(dev);
        }
        else if ((strcmp(argv[1], "check_ld") == 0)) {
            return _check_lock_data(dev);
        }
    }
    else {
        printf("* set_dev <number> - set and initialize an atca device (defaults to index 0)\n");
        printf("* read - read Microchip CryptoAuth device's config zone\n");
        printf("* read_bin - read config zone and print binary data\n");
        printf("* lock_c - PERMANENTLY lock Microchip CryptoAuth device's config zone (cannot be undone!)\n");
        printf("* lock_d - PERMANENTLY lock Microchip CryptoAuth device's data zone (cannot be undone!)\n");
        printf("* check_lc - check if Microchip CryptoAuth device's config zone is locked\n");
        printf("* check_ld - check if Microchip CryptoAuth device's data zone is locked\n");
    }
    return 0;
}

SHELL_COMMAND(atca, "Commands for Microchip CryptoAuth devices",
        _atca);
