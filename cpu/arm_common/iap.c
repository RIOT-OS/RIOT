/* iap driver
 * 
 * based on iap driver for LPC2148 Controller made by Andreas Weschenfelder, 2008
 * see:
 * 
 */

#include <iap.h>
#include <lpc2387.h>

#define ENABLE_DEBUG
#include <debug.h>

static unsigned int iap_command[5];			// contains parameters for IAP command
static unsigned int iap_result[2];			// contains results
typedef void (*IAP) (unsigned int[],unsigned int[]);	// typedefinition for IAP entry function
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
uint8_t	iap_write(uint32_t dst, char *src, uint32_t size) {
	char err;
	uint32_t buffer_vic;
	uint8_t sec;
	
	buffer_vic  = VICIntEnable;		// save interrupt enable
	VICIntEnClr = 0xFFFFFFFF;		// clear vic
	
	sec = iap_get_sector(dst);

    /* check sector */
	if(blank_check_sector(sec, sec) == SECTOR_NOT_BLANK) {
		DEBUG("Warning: Sector %i not blank\n", sec);	
	}

    /* prepare sector */
	err = prepare_sectors(iap_get_sector(dst), iap_get_sector(dst));
    if (err) {
		DEBUG("\n-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION: %u", err);
        /* set interrupts back and return */
        VICIntEnable = buffer_vic;	
		return 0;
	}
    /* write flash */
	else {
		err = copy_ram_to_flash(dst, (uint32_t) src, size);
		if(err) {
			DEBUG("ERROR: COPY_RAM_TO_FLASH: %u\n", err);
            /* set interrupts back and return */
            VICIntEnable = buffer_vic;	
			return 0;
		}
        /* check result */
		else {
			err = compare(dst, (uint32_t) src, size);
            if (err) {
				DEBUG("ERROR: COMPARE: %i (at position %u)\n", err, iap_result[1]);
                /* set interrupts back and return */
	            VICIntEnable = buffer_vic;
				return 0;
			}
			else
			{
				DEBUG("Data successfully written!\n");
                /* set interrupts back and return */
				return 1;			
			}
		}
	}
}


uint8_t iap_erase(uint32_t addr) {
    /* check sector */
	if (!blank_check_sector(iap_get_sector(addr), iap_get_sector(addr))) {
		DEBUG("Sector already blank!\n");
		return 1;
	}
	/* prepare sector */
    if (prepare_sectors(iap_get_sector(addr), iap_get_sector(addr))) {
		DEBUG("-- ERROR: PREPARE_SECTOR_FOR_WRITE_OPERATION --\n");
		return 0;
	}
    /* erase sector */
	if (erase_sectors(iap_get_sector(addr), iap_get_sector(addr))) {
		DEBUG("-- ERROR: ERASE SECTOR --\n");
		return 0;
	}
    /* check again */
	if (blank_check_sector(iap_get_sector(addr), iap_get_sector(addr))) {
		DEBUG("-- ERROR: BLANK_CHECK_SECTOR\n");
		return 0;
	}
	DEBUG("Sector successfully erased.\n");
	return 1;
}


/******************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

static uint32_t iap(uint32_t code, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
	iap_command[0] = code;		// set command code
	iap_command[1] = p1;		// set 1st param
	iap_command[2] = p2;		// set 2nd param
	iap_command[3] = p3;		// set 3rd param
	iap_command[4] = p4;		// set 4th param
		
	((void (*)())0x7ffffff1)(iap_command, iap_result);		// IAP entry point
	return *iap_result;
}

/******************************************************************************
 * Function:	blank_check_sector
 * 
 * Description:	This command is used to blank check a sector or multiple sectors 
 * 				of on-chip Flash memory. To blank check a single sector use the
 * 				same "Start" and "End" sector numbers.
 * 				Command: 53
 * 				Param0:	Start Sector Number
 * 				Param1: End Sector Number (should be greater than equal to the start 
 * 						sector number)
 * 
 * Parameters:	long tmp_sect1:		Param0
 * 				long tmp_sect2:		Param1
 * 
 * Return: 		Code 	CMD_SUCCESS |
 * 						BUSY |
 * 						SECTOR_NOT_BLANK |
 * 						INVALID_SECTOR
 * 				Result0: Offset of the first non blank word location if the status code is SECTOR_NOT_BLANK.
 * 				Result1: Contents of non blank wird location.
 *****************************************************************************/
uint32_t blank_check_sector(uint32_t tmp_sect1, uint32_t tmp_sect2) {
	return iap(BLANK_CHECK_SECTOR, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:	copy_ram_to_flash
 * 
 * Description:	This command is used to programm the flash memory. the affected should be 
 * 				prepared first by calling "Prepare Sector for Write Operation" command. the 
 * 				affected sectors are automatically protected again once the copy command is
 * 				successfully executed. the boot sector cannot be written by this command.
 * 				Command: 51
 * 				Param0:	(DST) Destination Flash adress where data bytes are to be written. 
 * 						This address should be a 512 byte boundary.
 * 				Param1: (SRC) Source RAM adress from which data byre are to be read.
 * 				Param2:	Number of bytes to be written. Should be 512 | 1024 | 4096 | 8192.
 * 				Param3: System Clock Frequency (CCLK) in KHz.
 * 
 * Parameters:	long tmp_adr_dst:	Param0
 *  			long tmp_adr_src: 	Param1
 * 				long tmp_size:		Param2
 * 
 * Return: 		Code 	CMD_SUCCESS |
 * 						SRC_ADDR_ERROR (Address not on word boundary) |
 *						DST_ADDR_ERROR (Address not on correct boundary) |
 *						SRC_ADDR_NOT_MAPPED |
 *						DST_ADDR_NOT_MAPPED |
 *						COUNT_ERROR (Byte count is not 512 | 1024 | 4096 | 8192) |
 * 						SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *						BUSY
 *****************************************************************************/
uint32_t copy_ram_to_flash(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size) {
	return iap(COPY_RAM_TO_FLASH, tmp_adr_dst, tmp_adr_src, tmp_size, _XTAL);
}


/******************************************************************************
 * Function:	Prepare_Sector
 * 
 * Description:	This command must be executed before executing "Copy RAM to Flash" or "Erase Sector(s)"
 *				command. Successful execution of the "Copy RAM to Flash" or "Erase Sector(s)" command causes
 *				relevant sectors to be protected again. The boot sector can not be prepared by this command. To
 *				prepare a single sector use the same "Start" and "End" sector numbers..
 * 				Command code: 50
 *				Param0: Start Sector Number
 *				Param1: End Sector Number: Should be greater than or equal to start sector number.
 * 
 * Parameters:	long tmp_sect1: 	Param0
 * 				long tmp_sect2:		Param1
 * 
 * Return: 		Code 	CMD_SUCCESS |
 *						BUSY |
 *						INVALID_SECTOR
 *****************************************************************************/
uint32_t prepare_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2) {
	return iap(PREPARE_SECTOR_FOR_WRITE_OPERATION, tmp_sect1, tmp_sect2, 0 , 0);
}


/******************************************************************************
 * Function:	erase_sectors
 * 
 * Description:	This command is used to erase a sector or multiple sectors of on-chip Flash memory. The boot
 *				sector can not be erased by this command. To erase a single sector use the same "Start" and "End"
 *				sector numbers.
 * 				Command code: 52
 *				Param0: Start Sector Number
 *				Param1: End Sector Number: Should be greater than or equal to start sector number.
 *				Param2: System Clock Frequency (CCLK) in KHz.
 * 
 * Parameters:	long tmp_sect1: 	Param0
 * 				long tmp_sect2:		Param1
 * 
 * Return: 		Code 	CMD_SUCCESS |
 *						BUSY |
 *						SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION |
 *						INVALID_SECTOR
 *****************************************************************************/
uint32_t erase_sectors(uint32_t tmp_sect1, uint32_t tmp_sect2) {
	return iap(ERASE_SECTOR, tmp_sect1, tmp_sect2, _XTAL, 0);
}


/******************************************************************************
 * Function:	compare
 * 
 * Description:	This command is used to compare the memory contents at two locations. compare result may not
 *				be correct when source or destination address contains any of the first 64 bytes starting
 *				from address zero. First 64 bytes can be re-mapped to RAM.
 * 				Command Code: 56
 *				Param0(DST): Starting Flash or RAM address from where data bytes are to be
 *								address should be a word boundary.
 *				Param1(SRC): Starting Flash or RAM address from where data bytes are to be
 *								address should be a word boundary.
 *				Param2: Number of bytes to be compared. Count should be in multiple of 4.
 * 
 * Parameters:	long tmp_adr_dst
 * 				long tmp_adr_src 
 * 				long tmp_size
 * 
 * Return: 		Code 	CMD_SUCCESS |
 *						COMPARE_ERROR |
 * 						COUNT_ERROR (Byte count is not multiple of 4) |
 *						ADDR_ERROR |
 *						ADDR_NOT_MAPPED
 * 				Result0: Offset of the first mismatch if the Status Code is COMPARE_ERROR.
 *****************************************************************************/
uint32_t compare(uint32_t tmp_adr_dst, uint32_t tmp_adr_src, uint32_t tmp_size) {
	return iap(COMPARE, tmp_adr_dst, tmp_adr_src, tmp_size, 0);
}
