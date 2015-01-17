/*
 * Copyright (C) 2015 Frank Holtz
 * Copyright (C) 2015 Ludwig Ortmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * NRF51 periph/flash.h implementation
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * @ingroup cpu_nrf51822
 * @defgroup cpu_nrf51822_flash
 * @file
 */

#include <sys/types.h>

#include "cpu-conf.h"
#include "periph/flash.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Size of available flash memory */
static size_t _nrf51_flash_size = (FLASH_NUM_PAGES *FLASH_PAGE_SIZE);

/************************************************************************/
/* internal API *********************************************************/
/************************************************************************/

/**
 * @brief Check if given address in valid range
 *
 * @return  Error code FLASH_ERROR_SUCCESS or FLASH_ERROR_ADDR_RANGE
 * TODO: Implement code region 0|1 check and return FLASH_ERROR_FB_CONFIG
 *       NRF_FICR->CLENR0 + PPFC
 */
static uint8_t flash_check_address(void *address)
{
    if (((size_t)address < 0) || ((size_t)address >= _nrf51_flash_size)) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    return FLASH_ERROR_SUCCESS;
}

/************************************************************************/
/* periph/flash.h *******************************************************/
/************************************************************************/

uint8_t flash_init(void)
{
    DEBUG("flash initialized\n");
    return FLASH_ERROR_SUCCESS;
}

flash_page_number_t flash_get_page_number(void *address, flash_page_size_t *page_offset)
{
    flash_page_number_t page_number = (flash_page_number_t)(
                                          (size_t)address / FLASH_PAGE_SIZE);

    if (page_offset != NULL) {
        *page_offset = (flash_page_number_t)(
                           (size_t)address % FLASH_PAGE_SIZE);
    }

    return page_number;
}

void *flash_get_address(flash_page_number_t page)
{
    return ((void *)(page * FLASH_PAGE_SIZE));
}

uint8_t flash_memcpy(void *dest, const void *src, size_t n)
{
    DEBUG("flash_memcpy dest=%d, src=%d, n=%d\n", (size_t)dest, (size_t)src, n);
    
    /* Variables */

    /* temp */
    uint8_t temp;

    /* Flag is switched on verify error */
    bool writeverify = true;

    /* Buffer for unaligned access*/
    flash_data_word_t dstbuffer;

    /* Align dest */
    flash_data_word_t *mydst_start =
        (flash_data_word_t *)((size_t)dest & (FLASH_ERASED_WORD_VALUE - FLASH_WRITE_ALIGN + 1));
    flash_data_word_t *mydst_end =
        (flash_data_word_t *)((size_t)(dest + n) &
                              (FLASH_ERASED_WORD_VALUE - FLASH_WRITE_ALIGN +1));

    /* prepare src to read bytewise */
    uint8_t *mysrc = (uint8_t *)src;

    /* Nothing to do */
    if (n == 0) {
        return (FLASH_ERROR_SUCCESS);
    }

    /* Check destination memory range: start */
    temp = flash_check_address((uint8_t *) mydst_start);

    if (temp > FLASH_ERROR_SUCCESS) {
        DEBUG("attempted to write below first address\n");
        return temp;
    }

    /* Check destination memory range: end */
    temp = flash_check_address((uint8_t *) mydst_end-1);

    if (temp > FLASH_ERROR_SUCCESS) {
        DEBUG("attemted to write beyond last address\n");
        return temp;
    }

    /* Enable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    /* move mysrc down to count of unaligned dst bytes */
    mysrc -= (uint8_t *)dest - (uint8_t *)mydst_start;

    for (flash_data_word_t *i = mydst_start; i <= mydst_end && n > 0; i++) {
        dstbuffer = FLASH_ERASED_WORD_VALUE;

        /* copy data into dstbuffer */
        for (temp = 0; temp < FLASH_WRITE_ALIGN; temp++) {
            /* check address range */
            if ((mysrc >= (uint8_t *)src) && (n > 0)) {
                n--;
                /* for every data word byte (little endian)*/
                switch (temp) {
                    case 0:
                        *((uint8_t *)&dstbuffer) = *mysrc;
                        break;

                    case 1:
                        *((uint8_t *)&dstbuffer + 1) = *mysrc;
                        break;

                    case 2:
                        *((uint8_t *)&dstbuffer + 2) = *mysrc;
                        break;

                    default:
                        *((uint8_t *)&dstbuffer + 3) = *mysrc;
                        break;
                }
            }
            mysrc++;
        }

        /* new data -> write
         * change & to | when erased bit value is 0 */
        if ( *i != (dstbuffer & *i)) {
            /* write data */
            *i = dstbuffer;

            /* wait until write operation is completed */
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

            /* verify data */
            if (*i != dstbuffer) {
                /* set error flag */
                writeverify = false;
            }
        }
    }

    /* Disable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    if (writeverify) {
        return (FLASH_ERROR_SUCCESS);
    }
    else {
        return (FLASH_ERROR_VERIFY);
    }
}

uint8_t flash_memcpy_fast(void *dest, const void *src, size_t n)
{
    flash_data_word_t *mydst = (flash_data_word_t *)dest;
    flash_data_word_t *mysrc = (flash_data_word_t *)src;

    /* Prevent hard fault: Is src and dst aligned ? */
    /* Check alignment */
    if ((n % FLASH_WRITE_ALIGN != 0) ||
        ((size_t)mysrc % FLASH_WRITE_ALIGN != 0) ||
        ((size_t)mydst % FLASH_WRITE_ALIGN != 0)) {
        DEBUG("Unaligned access dest=%d, src=%d, n=%d\n", (size_t)mydst, (size_t)mysrc, n);
        return FLASH_ERROR_ALIGNMENT;
    }

    return (0);

    /* Enable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    /* Write data to flash */
    n = n / FLASH_WRITE_ALIGN;

    while (n--) {
        /* write data */
        *mydst = *mysrc;

        /* increment pointer by 4 on Cortex-M0 */
        mydst++;
        mysrc++;

        /* wait until write operation is completed */
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
    }

    /* Disable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    return FLASH_ERROR_SUCCESS;
}

uint8_t flash_erase_page(flash_page_number_t page)
{
    /* check argument */
    if (page > FLASH_NUM_PAGES) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    /* erase page by address */
    return flash_erase_page_by_address((void *)(page * FLASH_PAGE_SIZE));
}

uint8_t flash_erase_page_by_address(void *address)
{
    /* TODO: check code 0|1 region */
    uint8_t ret = flash_check_address(address);

    if (ret > FLASH_ERROR_SUCCESS) {
        return ret;
    }

    /* Check alignment */
    if ((size_t)address % FLASH_PAGE_SIZE != 0) {
        return FLASH_ERROR_ALIGNMENT;
    }

    /* Enable erasing flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);


    /* Erase page in region 1*/
    NRF_NVMC->ERASEPAGE = (size_t)address;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    /* Disable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    return FLASH_ERROR_SUCCESS;
}

