/*
 * Copyright (C) 2014 Frank Holtz
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Flash interface test
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "periph/flash.h"

uint8_t print_test(char *text, uint32_t return_code, uint32_t error_code)
{
    if ((return_code == error_code) || (error_code == 65535)) {
        printf("%s\tOK (%d)\n", text, (int)return_code);
        return 0;
    }
    else {
        printf("%s\tError (%d)\n", text, (int)return_code);
        return 1;
    }
}

void dump_line(uint8_t *address)
{
    printf("%08x ", (size_t)address);

    for (uint8_t i = 0; i < 16; i++) {
        printf("%02x ", address[i]);
    }

    for (uint8_t i = 0; i < 16; i++) {
        if ((address[i] > 31) && (address[i] < 127)) {
            printf("%c", address[i]);
        }
        else {
            printf(".");
        }
    }

    printf("\n");
}

int main(void)
{
    /* Signature with space for alignment */
    char *testsignature = "RIOT-FLASH-TEST\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    int errors = 0, testsignaturelen = strlen(testsignature), counter = 0;
    uint8_t *flashpage = flash_get_address(FLASH_NUM_PAGES - 1);

    printf("\nFlash interface test...\n"
           "Test\t\t\t\t\tResult (RC)\n"
           "---------------------------------------------------\n");

    errors += print_test("Initializing flash interface\t", flash_init(), FLASH_ERROR_SUCCESS);
    errors += print_test("Compare signature with flash\t", memcmp(flashpage, testsignature,
                         testsignaturelen), 65535);
    errors += print_test("Erase last flash page\t\t", flash_erase_page(FLASH_NUM_PAGES - 1),
                         FLASH_ERROR_SUCCESS);

    for (flash_page_size_t i = 0; i < FLASH_PAGE_SIZE; i++) {
        if (flashpage[i] == (FLASH_ERASED_WORD_VALUE & 0xff)) {
            counter++;
        }
    }

    errors += print_test("Number of erased bytes\t\t", counter, FLASH_PAGE_SIZE);


#if FLASH_NUM_PAGES != 256 && FLASH_NUM_PAGES != 65536
    errors += print_test("Erase flash page outside\t\t", flash_erase_page(FLASH_NUM_PAGES + 1),
                         FLASH_ERROR_ADDR_RANGE);
#endif
#if FLASH_WRITE_ALIGN > 1

    /* align len */
    while (testsignaturelen % FLASH_WRITE_ALIGN > 0) {
        testsignaturelen++;
    }

    errors += print_test("Write test signature unaligned\t", flash_memcpy(flashpage,
                         testsignature, testsignaturelen - 1), FLASH_ERROR_ALIGNMENT);
#endif
    errors += print_test("Write test signature aligned\t", flash_memcpy(flashpage,
                         testsignature, testsignaturelen), FLASH_ERROR_SUCCESS);
    errors += print_test("Compare signature with flash\t", memcmp(flashpage, testsignature,
                         testsignaturelen), 0);

    printf("\nDone with %d errors.\n\n", errors);

    printf("Dump of last flash page:\n");

    for (flash_page_size_t i = 0; i < FLASH_PAGE_SIZE; i += 16) {
        dump_line(flashpage + i);
    }

    return 0;
}
