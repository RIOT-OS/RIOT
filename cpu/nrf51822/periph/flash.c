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
    /* Check alignment */
    flash_page_size_t page_offset;
    flash_get_page_number(dest, &page_offset);

    if ((n % FLASH_WRITE_ALIGN != 0) || (page_offset % FLASH_WRITE_ALIGN != 0)) {
        DEBUG("Unaligned access n=%d offset=%d\n", n, page_offset);
        return FLASH_ERROR_ALIGNMENT;
    }

    /* write data to flash */
    uint8_t ret = flash_memcpy_fast(dest, src, n);

    /* verify written data if write was successfull */
    if (ret == FLASH_ERROR_SUCCESS) {
        while (n > 0) {
            if (*((flash_data_word_t *) dest) != *((flash_data_word_t *) src)) {
                return FLASH_ERROR_VERIFY;
            }

            dest += FLASH_WRITE_ALIGN;
            src  += FLASH_WRITE_ALIGN;
            n    -= FLASH_WRITE_ALIGN;
        }
    }

    return (ret);
}

/* TODO: Fix it. Jumps into Hard Fault when src is Pointer to char* */
uint8_t flash_memcpy_fast(void *dest, const void *src, size_t n)
{
    /* Check memory range */
    uint8_t ret = flash_check_address((uint8_t *) dest);

    if (ret > FLASH_ERROR_SUCCESS) {
        DEBUG("attempted to write below first address\n");
        return ret;
    }

    ret = flash_check_address((((uint8_t *) dest) + n));

    if (ret > FLASH_ERROR_SUCCESS) {
        DEBUG("attemted to write beyond last address\n");
        return ret;
    }

    flash_data_word_t *mydst = (flash_data_word_t *)dest;
    flash_data_word_t *mysrc = (flash_data_word_t *)src;

    /* Enable writing to flash */
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

    /* Write data to flash */
    n = n / FLASH_WRITE_ALIGN;

    while (n--) {
        *mydst = *mysrc;
        mydst += FLASH_WRITE_ALIGN;
        mysrc += FLASH_WRITE_ALIGN;

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

