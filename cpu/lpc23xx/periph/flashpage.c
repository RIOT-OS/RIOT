/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2020 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flash page driver implementation
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include "assert.h"
#include "cpu.h"
#include "iap.h"
#include "irq.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* typedefinition for IAP entry function */
typedef void (*IAP)(unsigned int[], unsigned int[]);
static const IAP IAP_Entry = (IAP)0x7ffffff1;

static uint32_t iap(uint32_t code, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
    /* contains parameters for IAP command */
    static unsigned int iap_command[5];
    /* contains results */
    static unsigned int iap_result[2];

    iap_command[0] = code;      /* set command code */
    iap_command[1] = p1;        /* set 1st param */
    iap_command[2] = p2;        /* set 2nd param */
    iap_command[3] = p3;        /* set 3rd param */
    iap_command[4] = p4;        /* set 4th param */

    IAP_Entry(iap_command, iap_result); /* IAP entry point */
    return *iap_result;
}

/******************************************************************************
 * Function:    check_sector
 *
 * Description: This command is used to blank check a sector or multiple sectors
 *              of on-chip Flash memory. To blank check a single sector use the
 *              same "Start" and "End" sector numbers.
 *
 *              Command code: 53
 *              Param0: Start Sector Number
 *              Param1: End Sector Number (should be greater than equal to the start
 *                      sector number)
 *
 * Parameters:  long sect_start:    Param0
 *              long sect_end:      Param1
 *
 * Return:      Code    CMD_SUCCESS
 *                      BUSY
 *                      SECTOR_NOT_BLANK
 *                      INVALID_SECTOR
 *
 *              Result0: Offset of the first non blank word location if the status
 *              code is SECTOR_NOT_BLANK.
 *              Result1: Contents of non blank wird location.
 *****************************************************************************/
static uint32_t blank_check_sector(uint32_t sect_start, uint32_t sect_end)
{
    return iap(BLANK_CHECK_SECTOR, sect_start, sect_end, 0, 0);
}

/******************************************************************************
 * Function:    copy_ram_to_flash
 *
 * Description: This command is used to program the flash memory. the affected should be
 *              prepared first by calling "Prepare Sector for Write Operation" command. the
 *              affected sectors are automatically protected again once the copy command is
 *              successfully executed. the boot sector cannot be written by this command.
 *
 *              Command code: 51
 *              Param0: (DST) Destination Flash address where data bytes are to be written.
 *                      This address should be a 256 byte boundary.
 *              Param1: (SRC) Source RAM address from which data byre are to be read.
 *              Param2: Number of bytes to be written. Should be 256 | 512 | 1024 | 4096 | 8192.
 *              Param3: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:  long tmp_adr_dst:   Param0
 *              long tmp_adr_src:   Param1
 *              long tmp_size:      Param2
 *
 * Return:      Code    CMD_SUCCESS
 *                      or SRC_ADDR_ERROR (Address not on word boundary)
 *                      or DST_ADDR_ERROR (Address not on correct boundary)
 *                      or SRC_ADDR_NOT_MAPPED
 *                      or DST_ADDR_NOT_MAPPED
 *                      or COUNT_ERROR (Byte count is not 256 | 512 | 1024 | 4096 | 8192)
 *                      or SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
 *                      or BUSY
 *****************************************************************************/
static uint32_t copy_ram_to_flash(void *dst, const void *src, size_t tmp_size)
{
    return iap(COPY_RAM_TO_FLASH, (uintptr_t)dst, (uintptr_t)src, tmp_size, _XTAL);
}

/******************************************************************************
 * Function:    Prepare_Sector
 *
 * Description: This command must be executed before executing "Copy RAM to Flash"
 *              or "Erase Sector(s)" command.
 *              Successful execution of the "Copy RAM to Flash" or "Erase Sector(s)" command causes
 *              relevant sectors to be protected again.
 *              The boot sector can not be prepared by this command.
 *              To prepare a single sector use the same "Start" and "End" sector numbers.
 *
 *              Command code: 50
 *              Param0: Start Sector Number
 *              Param1: End Sector Number: Should be greater than or equal to start sector number.
 *
 * Parameters:  long sect_start:    Param0
 *              long sect_end:      Param1
 *
 * Return:      Code    CMD_SUCCESS
 *                      or BUSY
 *                      or INVALID_SECTOR
 *****************************************************************************/
static uint32_t prepare_sectors(uint32_t sect_start, uint32_t sect_end)
{
    return iap(PREPARE_SECTOR_FOR_WRITE_OPERATION, sect_start, sect_end, 0, 0);
}

/******************************************************************************
 * Function:    erase_sectors
 *
 * Description: This command is used to erase a sector or multiple sectors of on-chip Flash memory.
 *              The boot sector can not be erased by this command.
 *              To erase a single sector use the same "Start" and "End" sector numbers.
 *
 *              Command code: 52
 *              Param0: Start Sector Number
 *              Param1: End Sector Number: Should be greater than or equal to start sector number.
 *              Param2: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:  long sect_start:    Param0
 *              long sect_end:      Param1
 *
 * Return:      Code    CMD_SUCCESS
 *                      or BUSY
 *                      or SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION
 *                      or INVALID_SECTOR
 *****************************************************************************/
static uint32_t erase_sectors(uint32_t sect_start, uint32_t sect_end)
{
    return iap(ERASE_SECTOR, sect_start, sect_end, _XTAL, 0);
}

/******************************************************************************
 * Function:    compare
 *
 * Description: This command is used to compare the memory contents at two locations.
 *              Compare result may not be correct when source or destination address
 *              contains any of the first 64 bytes starting from address zero.
 *              First 64 bytes can be re-mapped to RAM.
 *
 *              Command Code: 56
 *              Param0(DST): Starting Flash or RAM address from where data bytes are to be
 *                              address should be a word boundary.
 *              Param1(SRC): Starting Flash or RAM address from where data bytes are to be
 *                              address should be a word boundary.
 *              Param2: Number of bytes to be compared. Count should be in multiple of 4.
 *
 * Parameters:  long tmp_adr_dst
 *              long tmp_adr_src
 *              long tmp_size
 *
 * Return:      Code    CMD_SUCCESS
 *                      or COMPARE_ERROR
 *                      or COUNT_ERROR (Byte count is not multiple of 4)
 *                      or ADDR_ERROR
 *                      or ADDR_NOT_MAPPED
 *
 *              Result0: Offset of the first mismatch if the Status Code is COMPARE_ERROR.
 *****************************************************************************/
__attribute__((unused))
static uint32_t compare(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size)
{
    return iap(COMPARE, tmp_adr_dst, tmp_adr_src, tmp_size, 0);
}

size_t flashpage_size(unsigned page)
{
    if (page < 8 || page > 21) {
        return 0x1000;
    }

    return 0x8000;
}

void flashpage_erase(unsigned sec)
{
    /* check sector */
    if (!blank_check_sector(sec, sec)) {
        DEBUG("Sector %u already blank\n", sec);
        return;
    }

    /* prepare sector */
    if (prepare_sectors(sec, sec)) {
        DEBUG("ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION\n");
        return;
    }

    unsigned state = irq_disable();

    /* erase sector */
    if (erase_sectors(sec, sec)) {
        DEBUG("ERROR: ERASE SECTOR\n");
    }

    irq_restore(state);
}

void flashpage_write(void *target_addr, const void *data, size_t len)
{
    char err;
    uint8_t sec = flashpage_page(target_addr);

    /* prepare sector */
    err = prepare_sectors(sec, sec);
    if (err) {
        DEBUG("ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION: %u\n", err);
        return;
    }

    /*  write flash */
    unsigned state = irq_disable();
    err = copy_ram_to_flash(target_addr, data, len);
    irq_restore(state);

    if (err) {
        DEBUG("ERROR: COPY_RAM_TO_FLASH: %u\n", err);
    }
}
