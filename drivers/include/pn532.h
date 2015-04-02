/**
 *	Copyright (c) 2012, Adafruit Industries
 *	All rights reserved.
 */
/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**************************************************************************/
/**
 * @ingroup     drivers_pn532
 * @{
 * @file        
 * @brief       C Library for the PN532 written for Riot OS.
 * 				SPI Driver for NXP's PN532 NFC/13.56MHz RFID Transceiver
 * 				This is a library to connect a PN532 chip on a Sam3x CPU.
 * 				See the README file for details.
 *
 * 				@TODO RFConfiguration
 *
 * @author      University of Florence
 * @author      Simone Pecora <pecora.simone@gmail.com>
 * @}
 */
/*************************************************************************/

#ifndef PN532_H_
#define PN532_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)
#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST             		(0xD5)
#define PN532_ACK_WAIT_TIME					(10)	// ms, timeout of waiting for ACK
#define PN532_INVALID_ACK					(-1)
#define PN532_TIMEOUT						(-2)
#define PN532_INVALID_FRAME					(-3)
#define PN532_NO_SPACE						(-4)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_WAKEUP                        (0x55)

#define PN532_SPI_STATREAD                  (0x02)
#define PN532_SPI_DATAWRITE                 (0x01)
#define PN532_SPI_DATAREAD                  (0x03)
#define PN532_SPI_READY                     (0x01)

#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_WRITE_ULTRALIGHT         (0xA2)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)
#define PN532_GPIO_P30                      (0)
#define PN532_GPIO_P31                      (1)
#define PN532_GPIO_P32                      (2)
#define PN532_GPIO_P33                      (3)
#define PN532_GPIO_P34                      (4)
#define PN532_GPIO_P35                      (5)


//uint8_t uid[7];  // ISO14443A uid
//uint8_t uidLen;  // uid len
//uint8_t key[6];  // Mifare Classic key

//#define PN532_BUFFSIZE	265
//uint8_t writebuffer[PN532_BUFFSIZE];
//uint8_t readbuffer[PN532_BUFFSIZE];

/**
 * @brief	Initialization PN532 SPI communication.
 * @brief	Setting up SPI device, mode and speed.
 *
 */
void pn532_init_master(void);
/**
 *   @brief  	Low-level SPI write. Send 1 byte to PN532
 *
 *   @param  	p       8-bit command to write to the SPI bus
 *   @return	res		1 if the transfer is without errors
 */
uint8_t pn532_spi_write(uint8_t p);
/**
 * 	@brief  Low-level SPI write. Send an array of bytes to PN532
 *
 * 	@param  *buffer		ptr to the buffer we want to send to PN532
 * 	@param	blen		buffer length
 *
 * 	@returns			number bytes sent
 */
uint8_t pn532_spi_transfer_bytes(uint8_t *buffer, uint8_t blen);
/**
 *  @brief  Low-level SPI read wrapper
 *
 *  @returns The 8-bit value that was read from the SPI bus
 */
uint8_t pn532_spi_read(void);
/*
 *	@brief  Low-level SPI reverse char byte
 *
 *	@returns The 8-bit value reversed
 */
unsigned char reverse(unsigned char b);
/**
 *	@brief  Low-level SPI CS set the given GPIO to LOW/HIGH
 */
void pn532_ss_on(void);
void pn532_ss_off(void);
/**
 *  @brief  Wake up PN532 board
 *  @brief	We also could send a wake up frame but it should just works as it is.
 */
void pn532_begin(void);
/**
 *  @brief		Write a command to the PN532, automatically inserting the
 *  @brief      preamble and required frame details (checksum, len, etc.)
 *
 *  @param		buff		Pointer to the writebuffer
 *  @param  	cmd			Pointer to the command buffer
 *  @param  	cmdlen		Command length in bytes
 *
 *  @returns				Num bytes transferred on the SPI bus
 */
uint8_t pn532_spi_write_command(uint8_t *buff, uint8_t* cmd, uint8_t cmdlen);
/**
 *  @brief  Read a command from the PN532, automatically inserting the
 *  @brief	preamble and required frame details (checksum, len, etc.)
 *
 *  @param  buffer      Pointer to the readbuffer
 *
 *  @returns            Number of bytes read, -2,-3 on error
 */
uint16_t pn532_read_pn( uint8_t *buffer );
/**
 * 	@brief  Write to PN532 Status Read byte and read PN532 Status if is
 * 	@brief	Ready. RDY = 0x01
 *
 * 	@returns	Status of the PN532
 */
uint8_t pn532_is_ready(void);
/**
 *  @brief  	Write to PN532 Status Read byte and read PN532 Status if is
 *   			Ready. RDY = 0x01 Then the host send the Status Read field.
 *   			After that the host receive from PN532 the ACK frame.
 *
 *	@returns	negative on error, 1 on command successfully received
 */
uint8_t pn532_check_ack (void);
/**
 *	@brief  	Read frame from the PN532 and check the ACK received
 *				First of all I send DATAREAD (0x03) field
 *
 *	@returns	0 if the ACK frame is correct
 */
uint8_t pn532_read_ack(void);
/**
 *	@brief  	Write ACK frame from host to PN532.
 *				This command is optional for some functions.
 *
 *	@returns	1 when the ACK frame is sent
 */
void pn532_write_ack(void);

/**
 *	@brief  	Prints a hexadecimal value in plain characters
 *
 *	@param		data      Pointer to the uint8_t data
 *	@param  	numBytes  Data length in bytes
 *
 *	@returns	print	Number of bytes printed
 */
uint8_t pn532_print_hex( uint8_t *data, uint8_t numBytes);
/**
 *	@brief  	Checks the firmware version of the PN5xx chip
 *
 *	@params		pointer to Result, IC, Version, Rev and Support PN5352 fields
 *
 *	@returns	1 on success
 *	@returns	-1 on error
 */
uint32_t pn532_get_firmware_version(uint8_t *res, uint8_t *ic, uint8_t *version, uint8_t *rev, uint8_t *support);
/**
 *	@brief		Get General Status allows the host controller to know at
 *     			a given moment the complete situation of the PN532
 *
 */
void pn532_get_general_status(void);
/*
 *	@brief		This command is used to set internal parameters of the PN532
 *				and then to configure its behavior regarding different cases.
 *
 *	@params		nad		Use of the NAD information in case of initiator configuration
 *	@params		did		DID information in case of initiator configuration
 *	@params		autatr	Automatic generation of the ATR_RES in case of target
 *	@params		autrats	Automatic generation of the RATS in case of ISO/IEC144443-4 PCD mode
 *	@params		piss	Emulation of a ISO/IEC144443-4 PICC is enabled
 *
 *	@returns	1 on success, 0 or negative number on error
 */
uint8_t pn532_set_parameters( uint8_t nad, uint8_t did, uint8_t autatr, uint8_t autrats, uint8_t picc );
/*
 *	@brief		Configures the SAM (Secure Access Module)
 *				This command is used to select the data flow path by
 *				configuring the internal serial data switch.
 *
 *	@returns	True on success, false on error
 */
bool pn532_SAM_config(void);

/**************************DEP Chaining Part*********************************/
/**
 *	@brief		This command is used by a host controller to activate a target
 *   			using either active or passive communication mode.
 *   			Actpass is the communication mode requested by the host controller
 *   			(0x00 = passive; 0x01 = active)
 *   			Br is the baudrate to be used during the activation
 *   			(0x00 = 106 kbps; 0x01 = 212; 0x02 = 424)
 *
 *	@params		buffer	Byte length we want that PN532 as target send to the initiator
 *	@params		blen	Buffer length
 *
 *	@returns	1 on success, 0 or negative number on error
 */
uint16_t pn532_in_jump_for_dep ( uint8_t actpass, uint8_t br, uint8_t next, uint8_t * atrres, uint8_t * alen );
/**
 * @brief		This command is used by a host controller to activate a target
 *				using either active or passive communication mode.
 *				If a target is in the field, it will then be ready for PSL or
 * 				DEP exchanges.
 *
 * @params		buffer	Byte length we want that PN532 as target send to the initiator
 * @params		blen	Buffer length
 *
 */
uint16_t pn532_in_jump_for_psl ( uint8_t actpass, uint8_t br, uint8_t next );
/*
 *  @brief		This command is used to support protocol data exchanges
 *				between the PN532 as initiator and a target.
 *
 *	@params		buffer array data to send to the target
 *	@params		blen length array
 *	@params		tg Byte containing the logical number of the relevant target
 *	@params		mi More Information byte
 *
 *  @returns	1 on success, 0 or negative number on error
 */
uint8_t pn532_in_data_exchange ( uint8_t * buffer, uint8_t blen, uint8_t tg, uint8_t * mi, uint8_t * data, uint8_t * dlen );
/*
 *	@brief		The host controller uses this command to configure the PN532 as target.
 *
 *	@params		mode is a byte indicating which mode the PN532 should respect
 *
 *  @returns	1 on success,	0 or negative number on error
 */
uint8_t pn532_tg_init_as_target( uint8_t mode );
/**
 *	@brief		This command is used in combination with the TgInitAsTarget
 *				command to give the General Bytes.
 *				The PN532 uses them to build the ATR_RES sent to the initiator.
 *
 *	@params		gt is an array containing the general bytes to be used in the ATR_RES (max 47 bytes).
 *	@params		glen is the gt array length
 *
 *	@returns	1 on success, 0 or negative number on error
 */
uint8_t pn532_tg_set_general_bytes( uint8_t * gt, uint8_t glen );
/*
 *  @brief		This command is used in case of the PN532 configured as target
 *  			for Data Exchange Protocol (DEP) or for ISO/IEC14443-4 protocol
 *  			when PN532 is activated in ISO/IEC14443-4 PICC emulated.
 *  			Mi pointer corresponds to More Information byte.
 *
 *
 *	@param		mi		More Information bit
 *
 *  @returns			Byte length read on success, negative number on error
 */
uint16_t pn532_tg_get_data ( uint8_t * mi, uint8_t * data, uint8_t * dlen );
/*
 *  @brief		This command is used in case of the PN532 configured as target
 *  			for Data Exchange Protocol (DEP) or for ISO/IEC14443-4 protocol
 * 			 	when PN532 is activated in ISO/IEC14443-4 PICC emulated (see §4, p:21).
 *  		 	The overall amount of data to be sent can be transmitted in one
 *  		 	frame (262 bytes maximum).
 *
 *	@params		buffer	Byte length we want that PN532 as target send to the initiator
 *	@params		blen	Buffer length
 *
 *  @returns	Byte length on success, negative number on error
 */
uint8_t pn532_tg_set_data ( uint8_t * buffer, uint8_t blen );
/*
 *  @brief		This command is used in case of the PN532 configured as target for Data
 *				Exchange Protocol (DEP) if the overall amount of data to be sent cannot be
 *				transmitted in one frame (more than 262 bytes).
 *
 *	@params		buffer	Byte length we want that PN532 as target send to the initiator
 *	@params		blen	Buffer length
 *
 *  @returns	Byte length on success, negative number on error
 */
bool pn532_tg_set_meta_data ( uint8_t * buffer, uint8_t blen );
/*
 *	@brief		This command is used when the PN532 is configured as target.
 *				This command is used to get a packet of data from an initiator
 *				and to send it back to the host controller.
 *
 *	@params		incommand is an array of raw data (up to 262 bytes) received by
 *				the PN532 Target from the Initiator.
 *	@params		length is the array's length
 *
 */
uint8_t pn532_tg_get_initiator_command ( uint8_t * incommand, uint8_t * length );
/*
 *	@brief		This command is used to send a response packet of data to an initiator.
 *
 *	@params		tgresponse	pointer of the array of raw data to be sent by the PN532 to the Initiator
 *	@params		rlen
 *
 *  @returns	1 on success,	0 or negative number on error
 */
uint8_t pn532_tg_response_to_initiator ( uint8_t * tgresponse, uint8_t rlen );
/**
 * @brief		This command is used by the host controller to know what
 * 				the current state of the PN532 is.
 *
 */
void pn532_tg_get_target_status(void);

/*****************************Mifare Classic Part*********************************/
/**
 *	@brief		Waits for an ISO14443A target to enter the field
 *
 *	@param  	cardBaudRate  Baud rate of the card
 *	@param  	uid           Pointer to the array that will be populated
 *	                          with the card's UID (up to 7 bytes)
 *	@param		uidLength     Pointer to the variable that will hold the
 *	                          length of the card's UID.
 *
 *  @returns true on succes, false on error
 */
bool pn532_read_passive_target_id(uint8_t cardbaudrate, uint8_t *uid, uint8_t * uidlength);
/**
 *	@brief		Tries to authenticate a block of memory on a MIFARE card using the
 *				INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
 *				for more information on sending MIFARE and other commands.
 *
 *	@param		uid           Pointer to a byte array containing the card UID
 *	@param		uidLen        The length (in bytes) of the card's UID (Should
 *                  	      be 4 for MIFARE Classic)
 *  @param		blockNumber   The block number to authenticate.  (0..63 for
 *              	          1KB cards, and 0..255 for 4KB cards).
 *  @param		keyNumber     Which key type to use during authentication
 *                  	      (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
 *  @param		keyData       Pointer to a byte array containing the 6 bytes
 *              	          key value
 *
 *	@returns	1 on success, 0 on error
 */
uint8_t pn532_mifareclassic_auth_block(uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData);
/**
 *	@brief		Tries to read an entire 16-bytes data block at the specified block
 *				address.
 *
 *	@param		blockNumber		The block number to authenticate.  (0..63 for
 *								1KB cards, and 0..255 for 4KB cards).
 *	@param		data			Pointer to the byte array that will hold the
 *								retrieved data (if any)
 *
 *	@returns	1 on success, 0 on error
 */
uint8_t pn532_mifareclassic_read_data_block(uint8_t blockNumber, uint8_t * data);
/**
 *	@brief		Tries to write an entire 16-byte data block at the specified block
 *				address.
 *
 *	@param		blockNumber		The block number to authenticate.  (0..63 for
 *								1KB cards, and 0..255 for 4KB cards).
 *  @param		data			The byte array that contains the data to write.
 *
 *  @returns 	1 on success, 0 on error
 */
uint8_t pn532_mifareclassic_write_data_block( uint8_t blockNumber, uint8_t * data );
/**
 *	@brief		Formats a Mifare Classic card to store NDEF Records
 *
 *	@returns	1 on success, 0 for an error
 */
uint8_t pn532_mifareclassic_format_ndef(void);
/**
 *	@brief		Format a clean Mifare Classic 1K card as an NFC Forum tag
 *				(to store NDEF messages that can be read by any NFC enabled
 *				Android phone, etc.).
 *				We can encode many different kinds of pointers to the card,
 *				from a URL, to an Email address, to a phone number, and many more
 *				check the library header .h file to see the large # of supported
 *				prefixes! It depends from ndefprefix we choose.
 *				See the header for the corrispondent pointer.
 *
 *				Before to use this function you must initialize the board, get the
 *				firmware, SAMConfig and authenticate.
 *
 *	@param		url				byte url
 *	@param		ndefprefix		byte Prefixes for NDEF Records (to identify record type)
 *	@param		blockNumber		block number where we want to authenticate and write in it
 *
 *	@returns		1 on success, 0 on error
 */
uint8_t pn532_format_to_ndef( uint8_t * url, uint8_t ndefprefix, uint8_t blockNumber);
/**
 *	@brief		Writes an NDEF URI Record to the specified sector (1..15)
 *				Note that this function assumes that the Mifare Classic card is
 *				already formatted to work as an "NFC Forum Tag" and uses a MAD1
 *				file system.  You can use the NXP TagWriter app on Android to
 *				properly format cards for this.
 *
 *	@param  	sectorNumber	The sector that the URI record should be written
 *								to (can be 1..15 for a 1K card)
 *	@param		uriIdentifier	The uri identifier code (0 = none, 0x01 = "http://www.", etc.)
 *	@param		url				The uri text to write (max 38 characters).
 *
 *	@returns	1 on success, 0 on error
 */
uint8_t pn532_mifareclassic_write_ndef_uri (uint8_t sectorNumber, uint8_t uriIdentifier, const char * url);
/**
 *	@brief		Tries to read an entire 4-byte page at the specified address.
 *
 *	@param		page		The page number (0..63 in most cases)
 *	@param		buffer      Pointer to the byte array that will hold the
 *							retrieved data (if any).
 *
 *	@returns	1 on success, 0 on error
 */
uint8_t pn532_mifareultralight_read_page (uint8_t page, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* PN532_H_ */
/** @} */
