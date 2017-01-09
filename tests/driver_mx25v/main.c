/*
 * Copyright (C) 2016  OTA keys S.A.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the MX25V flash driver
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 *
 * @}
 */
#include <stdio.h>

/**
 * PREREQUISITE: To make it compiled:
 *
 * in Makefile: choose the interface: mci or mtdi or none
 * in board.h: #define SPI_FLASH_CS_PIN    GPIO_PIN(PORT_C, 4)
 * in mx25v_conf.h:
 *    - if not existed, create this file in board folder with mx25v device (readme)
 * in mtdi_conf.h:
 *    - if not existed, create this file in board folder with mtdi device (readme)
 * in this file:
 *    - choose the interface: USE_MCI or USE MTDI or none
 */

// #define USE_MTDI 1
// #define USE_MCI 1

#include <board.h>
#include <periph_conf.h>
#include <thread.h>
#include <periph/spi.h>
#include <periph/gpio.h>

#if USE_MCI
#include "diskio.h"
#elif USE_MTDI
#include <mtdi.h>
#endif

#include <mx25v.h>

#define NB_TEST             (uint8_t)(5)

#define THREAD_PRIORITY_MTDI_TEST        (THREAD_PRIORITY_MAIN - 1)
#define THREAD_NAME_MTDI_TEST            ("MTDI_TEST")
static char mtdi_test_thread_stack[5 * THREAD_STACKSIZE_DEFAULT];
static void *mtdi_test_thread_cb(void *arg);

static uint8_t test_mx25v_run(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_01(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_02(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_03(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_04(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_05(mx25v_t *mx25v_dev);
static uint8_t test_mx25v_read_status(mx25v_t *mx25v_dev, char value);
#if USE_MCI
static uint8_t test_mx25v_mci(void);
#elif USE_MTDI
static uint8_t test_mx25v_mtdi(void);
#endif

mx25v_t mx25_device =  {
    .mtdi_dev = { &mx25v_flash_interface },
    .spi_dev = SPI_0,
    .conf = SPI_CONF_FIRST_RISING,
    .speed = SPI_SPEED_1MHZ,
    .cs_pin = SPI_FLASH_CS_PIN,
    .hold_pin = 0,
    .wp_pin = 0,
};

int main(void)
{
    /* mtdi test thread creation */
    thread_create(mtdi_test_thread_stack, sizeof(mtdi_test_thread_stack),
                  THREAD_PRIORITY_MTDI_TEST,
                  THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  mtdi_test_thread_cb, NULL, THREAD_NAME_MTDI_TEST);

    while (1) { }
}

void *mtdi_test_thread_cb(void *arg)
{
    int ret_init = 0x45;
    uint8_t ret_test = 0x45;

    puts("MX25V flash driver test application\n");
    ret_init = mx25v_init(&mx25_device);

    if (ret_init == 0) {
        ret_test = test_mx25v_run(&mx25_device);
        if (ret_test == 0) {
            puts("success!\n");
        }
        else {
            printf("Test %i failed in part %i!", ((ret_test >> 4) & 0x0F), (ret_test & 0x0F));
        }
     }
    else {
        puts("initialization failed\n");
    }

    while (1);

    return 0;
}


static uint8_t test_mx25v_run(mx25v_t *mx25v_dev)
{
    uint8_t ret_test = 0x45;

    ret_test = test_mx25v_01(mx25v_dev);
    if (ret_test != 0) {
        return (ret_test);
    }

    ret_test = test_mx25v_02(mx25v_dev);
    if (ret_test != 0) {
        return (ret_test);
    }

    ret_test = test_mx25v_03(mx25v_dev);
    if (ret_test != 0) {
        return (ret_test);
    }

    ret_test = test_mx25v_04(mx25v_dev);
    if (ret_test != 0) {
        return (ret_test);
    }

    ret_test = test_mx25v_05(mx25v_dev);
    if (ret_test != 0) {
        return (ret_test);
    }

#if USE_MCI
    ret_test = test_mx25v_mci();
    if (ret_test != 0) {
        return (ret_test);
    }
#elif USE_MTDI
    ret_test = test_mx25v_mtdi();
    if (ret_test != 0) {
        return (ret_test);
    }
#endif

    return 0;
}

/**
 * test 01: read identification
 * - check id
 *
 * 1 iteration = ms
 *
 */
static uint8_t test_mx25v_01(mx25v_t *mx25v_dev)
{
    uint8_t number_of_test = 0x10;
    uint8_t ret = 0x45;
    int res = 0x45;
    mx25v_id_t mx25v_id = {0x45, 0x45, 0x45};

    res = mx25v_read_id(mx25v_dev, &mx25v_id);

    // test result
    if (res == 0) {
        if ((mx25v_id.manufacturer_id == MX25V_MANUFACTURER_ID) &&
            (mx25v_id.memory_id == MX25V_MEMORY_ID) &&
            (mx25v_id.device_id == MX25V_DEVICE_ID) ){
            ret = 0;
        }
        else{
            ret = number_of_test + 2;
        }
    }
    else {
        ret = number_of_test + 1;
    }

    return ret;
} /** end of test 01: read identification */

/**
 * test 02: write status
 * - write status ((BP0 | BP1 | BP2), BP0, BP1, BP2, 0)
 * - read status ((BP0 | BP1 | BP2), BP0, BP1, BP2, 0)
 *
 * 1 iteration = ms
 *
 */
static uint8_t test_mx25v_02(mx25v_t *mx25v_dev)
{
    uint8_t number_of_test = 0x20;
    uint8_t ret_test = 0x45;
    int res = 0x45;
    char check_write_status[NB_TEST] = {(MX25V_BP0 | MX25V_BP1 | MX25V_BP2),
                                         MX25V_BP0, MX25V_BP1, MX25V_BP2, 0};


    for (uint8_t i = 0; i < NB_TEST; i++) {
        res = mx25v_write_status(mx25v_dev, check_write_status[i]);
        if (res != 0) {
            return (number_of_test + (i*1));
        }

        number_of_test = 0x30;
        ret_test = test_mx25v_read_status(mx25v_dev, check_write_status[i]);
        if (ret_test != 0) {
            return (number_of_test + (i*2) + 1);
        }
    }

    return 0;
}

static uint8_t test_mx25v_read_status(mx25v_t *mx25v_dev, char value)
{
    uint8_t ret = 0x45;
    int res = 0x45;
    char temp_value = 0x45;

    res = mx25v_read_status(mx25v_dev, &temp_value);

    // test result
    if (res == 0) {
        if ( (temp_value & MX25V_READ_STATUS) == value) {
            ret = 0;
        }
        else {
            ret = 2;
        }
    }
    else {
        ret = 1;
    }

    return ret;
} /** end of test 02: write status */

/**
 * test 03: [page] erase and write
 * - erase sector
 * - read page: everything should be 255
 * - write page: 0 to 255
 * - read page: 0 to 255
 *
 * 1 iteration = ~ ms
 *
 */
static uint8_t test_mx25v_03(mx25v_t *mx25v_dev)
{
    uint8_t number_of_test = 0x30;
    int res = 0x45;

    /* begin of block 0, first sector of block 0, end of block 0, begin of block 8, end of block 15) */
    uint32_t check_buffer_address[NB_TEST] = {0, 0x00001000, 0x0000F000, 0x00008000, 0x000FF000};
    char check_page_value[MX25V_PAGE_SIZE];

    for (uint8_t i = 0; i < NB_TEST; i++) {
        // erase sector
        res = mx25v_erase_sector(mx25v_dev, check_buffer_address[i]);
        if (res != 0) {
            return (number_of_test + (i*7) + 1);
        }
    }

    for (uint8_t i = 0; i < NB_TEST; i++) {
        // initialize receive value and read
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = 0x45;
        }
        res = mx25v_read_buffer(mx25v_dev, check_buffer_address[i], check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*7) + 2);
        }
        else {
            // check result values to be 255
            for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
                if (check_page_value[j] != 255){
                    return (number_of_test + (i*7) + 3);
                }
            }
        }
    }

    for (uint8_t i = 0; i < NB_TEST; i++) {
        // initialize value to send
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = (uint8_t)i;
        }
        res = mx25v_write_buffer(mx25v_dev, check_buffer_address[i], (const unsigned char*) check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*7) + 4);
        }
    }

    for (uint8_t i = 0; i < NB_TEST; i++) {
        // initialize receive value and check
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = 0x45;
        }
        res = mx25v_read_buffer(mx25v_dev, check_buffer_address[i], check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*7) + 5);
        }
        else
        {
            for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
                if (check_page_value[j] != (uint8_t)i) {
                    return (number_of_test + (i*7) + 6);
                }
            }
        }
    }

    return 0;
} /** end of test 03: [page] erase and write */

/**
 * test 04: [sector] erase and write
 * - erase sector
 * - read sector: everything should be 255
 * - write sector: 0 on page 0,  ..., 15 on page 15
 * - read sector: 0 on page 0,  ..., 15 on page 15
 *
 * 1 iteration = ms
 *
 * note: an overflow can occur if the thread size is not enough.
 */
static uint8_t test_mx25v_04(mx25v_t *mx25v_dev)
{
    uint8_t number_of_test = 0x40;
    int res = 0x45;

    char check_sector_value[MX25V_SECTOR_SIZE];
    uint32_t sector_address = 0x00050000; // sector 1 of block 5 - or sector 80 of chip

    // erase sector
    res = mx25v_erase_sector(mx25v_dev, sector_address);
    if (res != 0) {
        return (number_of_test + 1);
    }

    // initialize receive value and read
    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
        check_sector_value[i] = 0x45;
    }
    res = mx25v_read_buffer(mx25v_dev, sector_address, check_sector_value, MX25V_SECTOR_SIZE);
    if (res != 0) {
        return (number_of_test + 2);
    }
    else {
        for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
            if (check_sector_value[i] != 255) {
                return (number_of_test + 3);
            }
        }
    }

    // initialize value to send
    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
        check_sector_value[i] = (uint8_t)(i/MX25V_PAGE_SIZE);
    }
    res = mx25v_write_buffer(mx25v_dev, sector_address, (const unsigned char*) check_sector_value, MX25V_SECTOR_SIZE);
    if (res != 0) {
        return (number_of_test + 3);
    }

    // initialize receive value and check
    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
        check_sector_value[i] = 0x45;
    }
    res = mx25v_read_buffer(mx25v_dev, sector_address, check_sector_value, MX25V_SECTOR_SIZE);
    if (res != 0) {
        return (number_of_test + 4);
    }
    else {
        for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
            if (check_sector_value[i] != (uint8_t)(i/MX25V_PAGE_SIZE)) {
                return (number_of_test + 5);
            }
        }
    }

    return 0;
}/** end of test 04: [sector] erase and write */

/**
 * test 05: [chip] erase, write and check
 * - erase chip
 * - read at different pages/sector: 255
 * - write at different pages/sector: 0 on sector 0,  ..., 255 on sector 255
 * - read at different pages/sector: 0 on sector 0,  ..., 255 on sector 255
 * - erase chip
 * - read at different pages/sector: 255
 */
static uint8_t test_mx25v_05(mx25v_t *mx25v_dev)
{
    uint8_t number_of_test = 0x50;
    int res = 0x45;

    /* block 0 - sector 15, block 3 - sector 12, block 6 - sector 9, block 9 - sector 6, block 12 - sector 3 */
    uint32_t check_buffer_address[NB_TEST] = {0x0000F000, 0x0003C000, 0x00069000, 0x00096000, 0x000C3000};
    char check_page_value[MX25V_PAGE_SIZE];

    // erase chip
    res = mx25v_erase_chip(mx25v_dev);
    if (res != 0) {
        return (number_of_test + 1);
    }

    // initialize receive value and read at 255
    for (uint8_t i = 0; i < NB_TEST; i++) {
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = 0x45;
        }
        res = mx25v_read_buffer(mx25v_dev, check_buffer_address[i], check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*2) + 2);
        }
        else {
            // check result values to be 255
            for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
                if (check_page_value[j] != 255){
                    return (number_of_test + (i*2) + 3);
                }
            }
        }
    }

    // write
    for (uint8_t i = 0; i < NB_TEST; i++) {
        // initialize value to send
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = (uint8_t)(check_buffer_address[i] >> 12);
        }
        res = mx25v_write_buffer(mx25v_dev, check_buffer_address[i], (const unsigned char*) check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + i + 12);
        }
    }

    // read and check
    for (uint8_t i = 0; i < NB_TEST; i++) {
        // initialize value to send
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = 0x45;
        }
        res = mx25v_read_buffer(mx25v_dev, check_buffer_address[i], check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*2) + 17);
        }
        else {
            // check result values to be 255
            for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
                if (check_page_value[j] != (uint8_t)(check_buffer_address[i] >> 12)){
                    return (number_of_test + (i*2) + 18);
                }
            }
        }
    }

    // erase chip
    res = mx25v_erase_chip(mx25v_dev);
    if (res != 0) {
        return (number_of_test + 25);
    }

    // initialize receive value and read at 255
    for (uint8_t i = 0; i < NB_TEST; i++) {
        for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
            check_page_value[j] = 0x45;
        }
        res = mx25v_read_buffer(mx25v_dev, check_buffer_address[i], check_page_value, MX25V_PAGE_SIZE);
        if (res != 0) {
            return (number_of_test + (i*2) + 26);
        }
        else {
            // check result values to be 255
            for (uint32_t j = 0; j < MX25V_PAGE_SIZE; j++) {
                if (check_page_value[j] != 255){
                    return (number_of_test + (i*2) + 27);
                }
            }
        }
    }

    return 0;
}/** end of test 05: [chip] erase, write and check */

#if USE_MCI
/**
 * test mx25v mci: diskio interface
 *  - initialize
 *  - write on sector 12
 *  - read and check value on sector 12
 *  - ioctl(GET_SECTOR_SIZE) and check value
 *
 * @return          0 on success
 * @return          1 on failed init
 * @return          2 on failed write
 * @return          3 on failed read
 * @return          4 on failed value read
 * @return          5 on failed ioctl
 */
static uint8_t test_mx25v_mci(void)
{
    uint8_t number_of_test = 0x80;
    uint8_t ret_test = 0x45;
    uint8_t buffer[MX25V_SECTOR_SIZE];
    diskio_result_t result;
    diskio_sta_t status;
    uint16_t res = 0;
    uint32_t Value;

    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
        buffer[i] = (uint8_t)(i/MX25V_PAGE_SIZE);
    }

    status = mci_initialize();
    if (status == 0) {
        result = mci_write(buffer, 12, 1);
        if (result == DISKIO_RES_OK) {

            for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
                buffer[i] = 0x45;
            }

            result = mci_read(buffer, 12, 1);
            if (result == DISKIO_RES_OK) {
                for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
                    if (buffer[i] != (uint8_t)(i/MX25V_PAGE_SIZE)) {
                        res++;
                    }
                }

                if (res == 0) {
                    result = mci_ioctl(GET_SECTOR_SIZE, buffer);
                    Value = (buffer[3] << 24) + (buffer[2] << 16) + (buffer[1] << 8) + buffer[0];
                    if (result == DISKIO_RES_OK && Value == MX25V_SECTOR_SIZE) {
                        /* success */
                        ret_test = 0;
                    }
                    else {
                        /* failed ioctl */
                        ret_test = number_of_test + 5;
                    }
                }
                else {
                    /* failed value read */
                    ret_test = number_of_test + 4;
                }
            }
            else {
                /* failed read */
                ret_test = number_of_test + 3;
            }
        }
        else {
            /* failed write */
            ret_test = number_of_test + 2;
        }
    }
    else {
        /* failed init */
        ret_test = number_of_test + 1;
    }

    return ret_test;
}/** end of test mx25v mci: diskio interface */

#elif USE_MTDI

/**
 * test mx25v mtdi: memory technology device interface
 *  - initialize
 *  - erase 1 sector
 *  - write from page 0 to 16 (=1 sector)
 *  - read and check value on sector 0 to 16
 *  - ioctrl(GET_SECTOR_SIZE) and check value
 *
 * @return          0 on success
 * @return          1 on failed init
 * @return          2 on failed erase
 * @return          3 on failed write
 * @return          4 on failed read
 * @return          5 on failed value read
 * @return          6 on failed ioctl
 */
static uint8_t test_mx25v_mtdi(void)
{
    uint8_t number_of_test = 0x80;
    uint8_t ret_test = 0x45;
    uint8_t buffer[MX25V_SECTOR_SIZE];
    mtdi_sta_t status;
    mtdi_result_t result;
    uint16_t res = 0;
    uint32_t Value;

    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
        buffer[i] = (uint8_t)(i/MX25V_PAGE_SIZE);
    }

    status = (mtdi_conf[FLASH_MEMORY_0].api)->mtdi_init(mtdi_conf[FLASH_MEMORY_0].dev);
    if (status == 0) {
        result = (mtdi_conf[FLASH_MEMORY_0].api)->mtdi_erase(mtdi_conf[FLASH_MEMORY_0].dev, 0, 1);
        if (result == MTDI_RES_OK) {
            result = (mtdi_conf[FLASH_MEMORY_0].api)->mtdi_write(mtdi_conf[FLASH_MEMORY_0].dev, buffer, 0, 16);

            if (result == MTDI_RES_OK) {
                for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
                    buffer[i] = 0x45;
                }

                result = (mtdi_conf[FLASH_MEMORY_0].api)->mtdi_read(mtdi_conf[FLASH_MEMORY_0].dev, buffer, 0, 16);
                if (result == MTDI_RES_OK) {
                    for (uint32_t i = 0; i < MX25V_SECTOR_SIZE; i++) {
                        if (buffer[i] != (uint8_t)(i/MX25V_PAGE_SIZE)) {
                            res++;
                        }
                    }

                    if (res == 0) {
                        result = (mtdi_conf[FLASH_MEMORY_0].api)->mtdi_ioctl(mtdi_conf[FLASH_MEMORY_0].dev, MTDI_GET_SECTOR_SIZE, buffer);
                        Value = (buffer[3] << 24) + (buffer[2] << 16) + (buffer[1] << 8) + buffer[0];
                        if (result == MTDI_RES_OK && Value == MX25V_SECTOR_SIZE) {
                            /* success */
                            ret_test = 0;
                        }
                        else {
                            /* failed ioctl */
                            ret_test = number_of_test + 6;
                        }
                    }
                    else {
                        /* failed value read */
                        ret_test = number_of_test + 5;
                    }
                }
                else {
                    /* failed read */
                    ret_test = number_of_test + 4;
                }
            }
            else {
                /* failed write */
                ret_test = number_of_test + 3;
            }
        }
        else {
            /* failed erase */
            ret_test = number_of_test + 2;
        }
    }
    else {
        /* failed init */
        ret_test = number_of_test + 1;
    }

    return ret_test;
}/** end of test mx25v mtdi: memory technology device interface */
#endif

