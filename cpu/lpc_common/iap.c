/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup cpu
 * @{
 */

/**
 * @file
 * @brief       lpc iap  driver
 * @note        based on iap driver for LPC2148 Controller made by Andreas Weschenfelder, 2008
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stddef.h>
/* iap driver
 *
 *
 */

#include "irq.h"
#include "flashrom.h"
#include "iap.h"
/* TODO: replace by a lpc generic header */
#include "lpc2387.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* pointer to reserved flash rom section for configuration data */
__attribute((aligned(256))) char configmem[256] __attribute__((section(".configmem")));

static unsigned int iap_command[5];         // contains parameters for IAP command
static unsigned int iap_result[2];          // contains results
typedef void (*IAP)(unsigned int[], unsigned int[]);    // typedefinition for IAP entry function
IAP IAP_Entry;

/* some function prototypes */
static uint32_t blank_check_sector(uint32_t tmp_sect1, uint32_t tmp_sect2);
static uint32_t prepare_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2);
static uint32_t erase_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2);
static uint32_t copy_ram_to_flash(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size);
static uint32_t compare(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size);
static uint32_t iap(uint32_t code, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4);

/******************************************************************************
 * P U B L I C   F U N C T I O N S
 *****************************************************************************/
uint8_t flashrom_write(uint8_t *dst, const uint8_t *src, size_t size)
{
    (void) size; /* unused */

    char err;
    unsigned intstate;
    uint8_t sec;

    sec = iap_get_sector((uint32_t) dst);

    if (sec == INVALID_ADDRESS) {
        DEBUG("Invalid address\n");
        return 0;
    }

    /* check sector */
    if (blank_check_sector(sec, sec) == SECTOR_NOT_BLANK) {
        DEBUG("Warning: Sector %i not blank\n", sec);
    }

    /* prepare sector */
    err = prepare_sectors(sec, sec);

    if (err) {
        DEBUG("\n-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION: %u\n", err);
        return 0;
    }
    /* write flash */
    else {
        intstate = disableIRQ();
        err = copy_ram_to_flash((uint32_t) dst, (uint32_t) src, 256);
        restoreIRQ(intstate);

        if (err) {
            DEBUG("ERROR: COPY_RAM_TO_FLASH: %u\n", err);
            /* set interrupts back and return */
            restoreIRQ(intstate);
            return 0;
        }
        /* check result */
        else {
            err = compare((uint32_t) dst, (uint32_t) src, 256);

            if (err) {
                DEBUG("ERROR: COMPARE: %i (at position %u)\n", err, iap_result[1]);
                return 0;
            }
            else {
                DEBUG("Data successfully written!\n");
                return 1;
            }
        }
    }
}


uint8_t flashrom_erase(uint8_t *addr)
{
    uint8_t sec = iap_get_sector((uint32_t) addr);
    unsigned intstate;

    if (sec == INVALID_ADDRESS) {
        DEBUG("Invalid address\n");
        return 0;
    }

    /* check sector */
    if (!blank_check_sector(sec, sec)) {
        DEBUG("Sector already blank!\n");
        return 1;
    }

    /* prepare sector */
    if (prepare_sectors(sec, sec)) {
        DEBUG("-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION --\n");
        return 0;
    }

    intstate = disableIRQ();

    /* erase sector */
    if (erase_sectors(sec, sec)) {
        DEBUG("-- ERROR: ERASE SECTOR --\n");
        restoreIRQ(intstate);
        return 0;
    }

    restoreIRQ(intstate);

    /* check again */
    if (blank_check_sector(sec, sec)) {
        DEBUG("-- ERROR: BLANK_CHECK_SECTOR\n");
        return 0;
    }

    DEBUG("Sector successfully erased.\n");
    return 1;
}


/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

static uint32_t iap(uint32_t code, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4)
{
    iap_command[0] = code;      // set command code
    iap_command[1] = p1;        // set 1st param
    iap_command[2] = p2;        // set 2nd param
    iap_command[3] = p3;        // set 3rd param
    iap_command[4] = p4;        // set 4th param

    ((void (*)())0x7ffffff1)(iap_command, iap_result);      // IAP entry point
    return *iap_result;
}

/******************************************************************************
 * Function:    blank_check_sector
 *
 * Description: This command is used to blank check a sector or multiple sectors
 *              of on-chip Flash memory. To blank check a single sector use the
 *              same "Start" and "End" sector numbers.
 *              Command: 53
 *              Param0: Start Sector Number
 *              Param1: End Sector Number (should be greater than equal to the start
 *                      sector number)
 *
 * Parameters:  long tmp_sect1:     Param0
 *              long tmp_sect2:     Param1
 *
 * Return:      Code    CMD_SUCCESS |
 *                      BUSY |
 *                      SECTOR_NOT_BLANK |
 *                      INVALID_SECTOR
 *              Result0: Offset of the first non blank word location if the status code is SECTOR_NOT_BLANK.
 *              Result1: Contents of non blank wird location.
 *****************************************************************************/
uint32_t blank_check_sector(uint32_t tmp_sect1, uint32_t tmp_sect2)
{
    return iap(BLANK_CHECK_SECTOR, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:    copy_ram_to_flash
 *
 * Description: This command is used to programm the flash memory. the affected should be
 *              prepared first by calling "Prepare Sector for Write Operation" command. the
 *              affected sectors are automatically protected again once the copy command is
 *              successfully executed. the boot sector cannot be written by this command.
 *              Command: 51
 *              Param0: (DST) Destination Flash adress where data bytes are to be written.
 *                      This address should be a 512 byte boundary.
 *              Param1: (SRC) Source RAM adress from which data byre are to be read.
 *              Param2: Number of bytes to be written. Should be 512 | 1024 | 4096 | 8192.
 *              Param3: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:  long tmp_adr_dst:   Param0
 *              long tmp_adr_src:   Param1
 *              long tmp_size:      Param2
 *
 * Return:      Code    CMD_SUCCESS |
 *                      SRC_ADDR_ERROR (Address not on word boundary) |
 *                      DST_ADDR_ERROR (Address not on correct boundary) |
 *                      SRC_ADDR_NOT_MAPPED |
 *                      DST_ADDR_NOT_MAPPED |
 *                      COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
 *                      SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *                      BUSY
 *****************************************************************************/
uint32_t copy_ram_to_flash(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size)
{
    return iap(COPY_RAM_TO_FLASH, tmp_adr_dst, tmp_adr_src, tmp_size, _XTAL);
}


/******************************************************************************
 * Function:    Prepare_Sector
 *
 * Description: This command must be executed before executing "Copy RAM to Flash" or "Erase Sector(s)"
 *              command. Successful execution of the "Copy RAM to Flash" or "Erase Sector(s)" command causes
 *              relevant sectors to be protected again. The boot sector can not be prepared by this command. To
 *              prepare a single sector use the same "Start" and "End" sector numbers..
 *              Command code: 50
 *              Param0: Start Sector Number
 *              Param1: End Sector Number: Should be greater than or equal to start sector number.
 *
 * Parameters:  long tmp_sect1:     Param0
 *              long tmp_sect2:     Param1
 *
 * Return:      Code    CMD_SUCCESS |
 *                      BUSY |
 *                      INVALID_SECTOR
 *****************************************************************************/
uint32_t prepare_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2)
{
    return iap(PREPARE_SECTOR_FOR_WRITE_OPERATION, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:    erase_sectors
 *
 * Description: This command is used to erase a sector or multiple sectors of on-chip Flash memory. The boot
 *              sector can not be erased by this command. To erase a single sector use the same "Start" and "End"
 *              sector numbers.
 *              Command code: 52
 *              Param0: Start Sector Number
 *              Param1: End Sector Number: Should be greater than or equal to start sector number.
 *              Param2: System Clock Frequency (CCLK) in KHz.
 *
 * Parameters:  long tmp_sect1:     Param0
 *              long tmp_sect2:     Param1
 *
 * Return:      Code    CMD_SUCCESS |
 *                      BUSY |
 *                      SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *                      INVALID_SECTOR
 *****************************************************************************/
uint32_t erase_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2)
{
    return iap(ERASE_SECTOR, tmp_sect1, tmp_sect2, _XTAL, 0);
}


/******************************************************************************
 * Function:    compare
 *
 * Description: This command is used to compare the memory contents at two locations. compare result may not
 *              be correct when source or destination address contains any of the first 64 bytes starting
 *              from address zero. First 64 bytes can be re-mapped to RAM.
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
 * Return:      Code    CMD_SUCCESS |
 *                      COMPARE_ERROR |
 *                      COUNT_ERROR (Byte count is not multiple of 4) |
 *                      ADDR_ERROR |
 *                      ADDR_NOT_MAPPED
 *              Result0: Offset of the first mismatch if the Status Code is COMPARE_ERROR.
 *****************************************************************************/
uint32_t compare(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size)
{
    return iap(COMPARE, tmp_adr_dst, tmp_adr_src, tmp_size, 0);
}
