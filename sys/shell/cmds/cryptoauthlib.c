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

#define SERIAL_NO_START_01          (0)
#define REVISION_NO_START           (4)
#define SERIAL_NO_START_02          (8)
#define AES_ENABLE                  (13)
#define I2C_ENABLE                  (14)
#define I2C_ADDRESS                 (16)
#define COUNT_MATCH                 (18)
#define OTP_MODE                    (18)
#define CHIP_MODE                   (19)
#define SLOT_CONFIG_START           (20)
#define COUNTER_01_START            (52)
#define COUNTER_02_START            (60)
#define LAST_KEY_USE_START          (68)
#define USE_LOCK                    (68)
#define VOLATILE_KEY_PERMIT         (69)
#define SECURE_BOOT_START           (70)
#define KDF_IV_LOC                  (72)
#define USER_EXTRA                  (84)
#define USER_EXTRA_ADD              (85)
#define SELECTOR                    (85)
#define LOCK_VALUE                  (86)
#define LOCK_CONFIG                 (87)
#define SLOT_LOCKED_START           (88)
#define CHIP_OPTIONS_START          (90)
#define X509_FORMAT_START           (92)
#define KEY_CONFIG_START            (96)

int device_index = 0;

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
        case ATECC508A:
            return "ATECC508A";
        case ATECC608:
            return "ATECC608";
        default:
            return "Unsupported device";
    }
}

/**
 * @brief   Print read-only section of the configuration zone.
 *
 *          Reserved bytes that don't mean anything are omitted.
 *
 * @param[in]   data           Pointer to buffer with input data
 * @param       devtype        Type of the device currently used
 */
static void _print_dev_info_ro(uint8_t* data, ATCADeviceType devtype)
{
    printf("Serial No (Pt. 1) | ");
    for (int i = 0; i < 4; i++) {
        printf("0x%02x ", data[SERIAL_NO_START_01+i]);
    }
    puts("");

    printf("Revision No.      | ");
    for (int i = 0; i < 4; i++) {
        printf("0x%02x ", data[REVISION_NO_START+i]);
    }
    puts("");

    printf("Serial No (Pt. 2) | ");
    for (int i = 0; i < 5; i++) {
        printf("0x%02x ", data[SERIAL_NO_START_02+i]);
    }
    puts("");

    if (devtype == ATECC608) {
        if (data[AES_ENABLE] & 0x01) {
            printf("AES Enabled       | True\n");
        }
        else {
            printf("AES Enabled       | False\n");
        }
    }

    if (data[I2C_ENABLE] & 0x01) {
        printf("I2C Enabled       | True\n");
        printf("Single Wire       | False\n");
    }
    else {
        printf("I2C Enabled       | False\n");
        printf("Single Wire       | True\n");
    }
}

/**
 * @brief   Get authorization mode
 *
 * @param   data    Byte of data to interpret
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
 * @param   data            Byte of data to interpret
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
 * @brief   Prints the OTP mode or CountMatch, depending on device type.
 *
 * @param   data Byte of data to interpret
 */
static void _print_countmatch_or_otp_mode(uint8_t data, ATCADeviceType devtype)
{
    if (devtype == ATECC608) {
        if (data & 0x01) {
            puts("Counter Match     | Enabled");
            printf("CountMatchKey stored in slot %d\n", data & 0xF0);
        }
        else {
            puts("Counter Match     | Disabled");
        }
    }
    else {
        switch(data) {
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
}

/**
 * @brief   Prints the chip mode
 *
 * @param   data Byte of data to interpret
 */
static void _print_chip_mode(uint8_t data, ATCADeviceType devtype)
{
    puts("ChipMode:");
    if (devtype == ATECC608) {
        if (data & 0x01) {
            puts("I2C Address       | I2C address was set by user and is stored in UserExtraAdd");
        }
        else {
            puts("I2C Address       | Default address");
        }
    }
    else {
        if (data & 0x01) {
            puts("SelectorMode      | Only writeable if zero");
        }
        else {
            puts("SelectorMode      | Always writeable");
        }
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

    if (devtype == ATECC608) {
        printf("Clock Divider     | 0x%02x\n", data & 0xF0);
    }
}

/**
 * @brief   Print Key Slot Configurations in hexadecimal and binary format
 *
 * @param[in]   data        Pointer to buffer with input data
 */
static void _print_slot_config(uint8_t* data)
{
    char binary[9];

    puts("Slot Config");
    puts("----------------------------------------");
    puts("SlotID  | Hex    | Binary");
    puts("        |        | 7      0 | 15     8");
    puts("--------+--------+----------------------");
    for (size_t i = 0; i < ATCA_KEY_SLOT_COUNT*2; i += 2) {
        static int slotcount = 0;
        if (slotcount < 10) {
            printf("%d       | 0x%02x%02x | ", slotcount, data[SLOT_CONFIG_START+i], data[SLOT_CONFIG_START+i+1]);
        }
        else {
            printf("%d      | 0x%02x%02x | ", slotcount, data[SLOT_CONFIG_START+i], data[SLOT_CONFIG_START+i+1]);
        }
        for (int j = 0; j < 2; j++) {
            get_bin(binary, data[i]);
            printf("%s | ", binary);
        }
        puts("");
        slotcount++;
    }
    puts("");
}

/**
 * @brief       Print lock status of key slots
 *
 * @param[in]   data        Pointer to buffer with input data
 */
static void _print_slot_lock(uint8_t* data)
{
    puts("\nSlotLocked (X = locked, - = unlocked):");
    puts("Slot   |  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15");
    printf("Locked | ");
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 8; i++) {
            if ((data[SLOT_LOCKED_START+j] >> i) & 0x01) {
                printf(" - ");
            }
            else {
                printf(" X ");
            }
        }
    }
    puts("");
}

static void _print_chip_options(uint8_t* data)
{
    puts("Chip Options:");
    if (data[CHIP_OPTIONS_START] & 0x01) {
        puts("Power On Self Test| Enabled");
    }
    else {
        puts("Power On Self Test| Disabled");
    }

    if (data[CHIP_OPTIONS_START] & 0x02) {
        puts("IO Prot Key       | Enabled");
    }
    else {
        puts("IO Prot Key       | Disabled");
    }

    if (data[CHIP_OPTIONS_START] & 0x04) {
        puts("AES KDF           | Enabled");
    }
    else {
        puts("AES KDF           | Disabled");
    }

    uint8_t ecdh_prot = data[CHIP_OPTIONS_START+1] & 0x03;
    switch(ecdh_prot) {
        case 0x00:
            puts("ECDH              | Clear Output on Bus OK");
            break;
        case 0x01:
            puts("ECDH              | Encrypted Output on Bus OK");
            break;
        case 0x10:
            puts("ECDH              | Result stored in TempKey or EEPROM Key Slot");
            break;
        default:
            puts("ECDH              | Usage Not permitted");
    }

    uint8_t kdf_prot = data[CHIP_OPTIONS_START+1] & 0x0C;
    switch(kdf_prot) {
        case 0x00:
            puts("KDF               | Clear Output on Bus OK");
            break;
        case 0x01:
            puts("KDF               | Encrypted Output on Bus OK");
            break;
        case 0x10:
            puts("KDF               | Result stored in TempKey or EEPROM Key Slot");
            break;
        default:
            puts("KDF               | Usage Not permitted");
    }

    printf("IO Protection Key stored in slot %d\n", data[CHIP_OPTIONS_START+1] & 0xF0);
}

/**
 * @brief   Print X509 format restrictions
 *
 * @param[in]   data        Pointer to buffer with input data
 */
static void _print_x509_format(uint8_t* data)
{
    puts("\nX509 Format:");
    for (int i = 0; i < 4; i++) {
        if (data[X509_FORMAT_START+i] == 0x00) {
            printf("PubKey %d         | No restrictions\n", i);
        }
        else {
            printf("PubKey %d:\n", i);
            printf("- PublicPosition  | 0x%02x\n", data[X509_FORMAT_START+i] & 0x0F);
            printf("- TemplateLength  | 0x%02x\n", data[X509_FORMAT_START+i] & 0xF0);
        }
    }
}

/**
 * @brief   Print key configurations
 *
 * @param[in]   data        Pointer to buffer with input data
 */
static void _print_key_config(uint8_t* data)
{
    size_t atca_key_config_bytes = ATCA_KEY_SLOT_COUNT*2;

    char binary[9];
    puts("\nKey Config");
    puts("----------------------------------------");
    puts("SlotID  | Hex    | Binary");
    puts("        |        | 7      0 | 15     8");
    puts("--------+--------+----------------------");
    for (size_t i = 0; i < atca_key_config_bytes; i += 2) {
        static int slotcount = 0;
        if (slotcount < 10) {
            printf("%d       | 0x%02x%02x | ", slotcount, data[KEY_CONFIG_START+i], data[KEY_CONFIG_START+i+1]);
        }
        else {
            printf("%d      | 0x%02x%02x | ", slotcount, data[KEY_CONFIG_START+i], data[KEY_CONFIG_START+i+1]);
        }
        for (int j = 0; j < 2; j++) {
            get_bin(binary, data[KEY_CONFIG_START+i]);
            printf("%s | ", binary);
        }
        puts("");
        slotcount++;
    }
    puts("");
}

static void _print_secure_boot(uint8_t* data)
{
    uint8_t secure_boot_mode = data[SECURE_BOOT_START] & 0x03;
    switch(secure_boot_mode) {
        case 0x01:
            puts("SecureBootMode    | Full");
            break;
        case 0x10:
            puts("SecureBootMode    | Stored signature");
            break;
        case 0x11:
            puts("SecureBootMode    | Stored digest");
            break;
        default:
            puts("SecureBootMode    | Disabled");
    }

    if (data[SECURE_BOOT_START] & 0x08) {
        puts("SecureBootPersist | Enabled");
    }
    else {
        puts("SecureBootPersist | Disabled");
    }

    if (data[SECURE_BOOT_START] & 0x10) {
        puts("SecureBootNonce   | Required, must use ATECC608 RNG");
    }
    else {
        puts("SecureBootNonce   | Optional, controlled by SecureBootMode");
    }

    printf("Secure Boot Signature or Digest stored in slot %d\n", data[SECURE_BOOT_START+1] & 0x0F);
    printf("Secure Boot Public Key stored in slot %d\n", data[SECURE_BOOT_START+1] & 0xF0);
}

static int _read_config(ATCADevice dev)
{
    uint8_t data[ATCA_ECC_CONFIG_SIZE];

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

    _print_dev_info_ro(data, devtype);
    puts("--------------------------------------------\n");

    puts("Device Info (Writable)");
    puts("---------------------------------------------");
    _print_i2c_addr_or_gpio_mode(data[I2C_ADDRESS], data[I2C_ENABLE] & 0x01);

    _print_countmatch_or_otp_mode(data[OTP_MODE], devtype);

    _print_chip_mode(data[CHIP_MODE], devtype);
    puts("");

    _print_slot_config(data);

    printf("Counter 0         | 0x%02x 0x%02x 0x%02x 0x%02x\n", data[COUNTER_01_START],
                                                                data[COUNTER_01_START+1],
                                                                data[COUNTER_01_START+2],
                                                                data[COUNTER_01_START+3]);

    printf("Counter 1         | 0x%02x 0x%02x 0x%02x 0x%02x\n", data[COUNTER_02_START],
                                                                data[COUNTER_02_START+1],
                                                                data[COUNTER_02_START+2],
                                                                data[COUNTER_02_START+3]);

    if (devtype == ATECC608) {
        if ((data[USE_LOCK] & 0x0F) == 0x0A) {
            puts("UseLockEnable     | True");
            printf("UseLockKey is stored in slot %d\n", data[USE_LOCK] & 0xF0);
        }
        else {
            puts("UseLockEnable     | False");
        }

        if (data[VOLATILE_KEY_PERMIT] & 0x80) {
            puts("VolatileKeyPermit | Enabled");
            printf("Volatile Key Permit key stored in slot %d\n", data[VOLATILE_KEY_PERMIT] & 0x0F);
        }
        else {
            puts("VolatileKeyPermit | Disabled");
        }

        _print_secure_boot(data);

        printf("KDF IV Loc        | %d\n", data[KDF_IV_LOC]);
    }
    else {
        for (int i = LAST_KEY_USE_START; i < ATCA_KEY_SLOT_COUNT; i++) {
            printf("LastKeyUse %d     | 0x%02x\n", i, data[i]);
        }
    }

    printf("UserExtra         | 0x%02x\n", data[USER_EXTRA]);

    if (devtype == ATECC608) {
        printf("UserExtraAdd (I2C)| 0x%02x\n", data[USER_EXTRA_ADD]);

    }
    else{
        printf("Selector          | 0x%02x\n", data[SELECTOR]);
    }

    if (data[LOCK_VALUE] == 0x00) {
        puts("LockValue         | Data and OTP Locked");
    }
    else {
        puts("LockValue         | Data and OTP Unlocked");
    }

    if (data[LOCK_CONFIG] == 0x00) {
        puts("LockConfig        | Config Zone Locked");
    }
    else {
        puts("LockConfig        | Config Zone Unlocked");
    }

    _print_slot_lock(data);

    if (devtype == ATECC608) {
        _print_chip_options(data);
    }
    _print_x509_format(data);

    _print_key_config(data);

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

static int _set_dev(char* id)
{
    int index = atoi(id);
    if (index > (int)ATCA_NUMOF-1) {
        printf("Invalid ID, can be 0 - %d\n", ATCA_NUMOF-1);
        return 1;
    }
    device_index = index;
    return 0;
}

static int _atca(int argc, char **argv)
{
    if (argc > 1) {
        if ((strcmp(argv[1], "set_dev") == 0)) {
            if (argc < 3) {
                puts("Please enter valid device index number");
                return 1;
            }
            return _set_dev(argv[2]);
        }

        if ((strcmp(argv[1], "read") == 0)) {
            return _read_config(atca_devs_ptr[device_index]);
        }
        else if ((strcmp(argv[1], "read_bin") == 0)) {
            return _read_config_bin(atca_devs_ptr[device_index]);
        }
        else if ((strcmp(argv[1], "lock_c") == 0)) {
            return _lock_config(atca_devs_ptr[device_index]);
        }
        else if ((strcmp(argv[1], "lock_d") == 0)) {
            return _lock_data(atca_devs_ptr[device_index]);
        }
        else if ((strcmp(argv[1], "check_lc") == 0)) {
            return _check_lock_config(atca_devs_ptr[device_index]);
        }
        else if ((strcmp(argv[1], "check_ld") == 0)) {
            return _check_lock_data(atca_devs_ptr[device_index]);
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
