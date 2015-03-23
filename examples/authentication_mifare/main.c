/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file	main.c
 * @brief       Project to make SPI communication works by Udoo and Adafruit PN532 Shield.
 *
 * @author      Simone Pecora <pecora.simone@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../drivers/pn532/pn532.h"

int main(void){
	sleep(10);
	(void) printf("\nWelcome to RIOT!\n");

	/* Initialize SPI communication by pn532_init_master function. Set register MSTR=1 CPOL=0 e NCPHA=0 */

	printf("Initialize pin MOSI, MISO e SCK.\n");
	pn532_init_master();
	printf("End initialize!\n");

	printf("Begin. I'll wake up the PN532\n");
	pn532_begin();
	printf("End begin().\n");

	printf("\nStarting getFirmware version\n");

	uint8_t response, ic, version, rev, support;
	pn532_get_firmware_version( &response, &ic, &version, &rev, &support);

	if( response != 0x03 ){
		printf("Response byte of Get Firmware Version wrong! HALT\n");
		while(1);
	}
	if( ic != 0x32 ){
		printf("Didn't find PN53x board.\n HALT!\n IC version: %X\n", ic);
		while (1); // halt
	}
	if( version != 0x01 ){
		printf("Version received not expected!\n");
	}
	printf("Here it is the frame version:\n| %X | %X | %X | %X | %X |\n", response, ic, version, rev, support);

    // Got ok data, print it out!
	printf("\n\n\n Found Chip PN5");
	printf("%X ", ic);
	printf("firmware version: ");
	printf("%X", version);
	printf(".");
	printf("%X ...", rev);
	printf("ok\n\n\n");

	//Configure board to read RFID tags
	if(! pn532_SAM_config() ){
		printf("Configuration SAM didn't end well! HALT");
		while(1);
	}
	printf("Waiting for an ISO14443A Card ...");


while(1){
	uint8_t success;
	uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
	uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
	success = pn532_read_passive_target_id(PN532_MIFARE_ISO14443A, uid, &uidLength);
	//be careful to choose the block number. Starting from 2 is the best solution if you don't know the use of it
	uint8_t blockNumber = 4;

	if (success) {
	   			// Display some basic information about the card
	       		printf("Found an ISO14443A card");
	       		printf("  UID Length: ");
	       		printf("%d",uidLength);
	       		printf(" bytes\n");
	       		printf("UID Value: ");
	       		pn532_print_hex( uid, uidLength );
	       		printf("\n");

	       		if (uidLength == 4){
	       			// We probably have a Mifare Classic card ...
	        		printf("Seems to be a Mifare Classic card (4 byte UID)\n");

	        		// Now we need to try to authenticate it for read/write access
	        		// Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
	        		printf("Trying to authenticate block 4 with default KEYA value\n");
	        		uint8_t keya[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
//	        		uint8_t keya[6] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};
//	        		uint8_t keya[6] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
//	        		uint8_t keya[6] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5};
//	        		uint8_t keya[6] = {0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD};
//	        		uint8_t keya[6] = {0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A};
//	        		uint8_t keya[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
//	        		uint8_t keya[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	        		uint8_t keya[6] = {0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56};

	        		// Start with block 4 (the first block of sector 1) since sector 0
	        		// contains the manufacturer data and it's probably better just
	        		// to leave it alone unless you know what you're doing
	        		success = pn532_mifareclassic_auth_block(uid, uidLength, blockNumber, 0, keya);
	        		if (success){
	        			printf("Sector 1 (Blocks 4..7) has been authenticated");
	        			uint8_t data[16];

	        			// If you want to write something to block 4 to test with, uncomment
	        			// the following line and this text should be read back in a minute
	        			// data = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
	        			// success = pn532_mifareclassic_WriteDataBlock (4, data);

	        			// Try to read the contents of block 4
	        			success = pn532_mifareclassic_read_data_block( blockNumber, data);
	        			if (success){
	        				// Data seems to have been read ... spit it out
	        				printf("Reading Block 4:");
	        				pn532_print_hex(data, 16);
	        				printf("\n");

	        				// Wait a bit before reading the card again
	        				sleep(1);
	        			}
	        			else{
	        				printf("Ooops ... unable to read the requested block.  Try another key?");
	        			}
	        		}
	        		else{
	        			printf("Ooops ... authentication failed: Try another key?");
	        		}
	        	}
	        	if (uidLength == 7){
	        		// We probably have a Mifare Ultralight card ...
	        		printf("Seems to be a Mifare Ultralight tag (7 byte UID)");
	        		// Try to read the first general-purpose user page (#blockNumber)
	        		printf("Reading page 4");
	        		uint8_t data[32];
	        		success = pn532_mifareultralight_read_page( blockNumber, data);
	        		if (success){
	        			// Data seems to have been read ... spit it out
	        			pn532_print_hex(data, 4);
						// Wait a bit before reading the card again
						sleep(1);
					}
	        		else{
	        			printf("Ooops ... unable to read the requested page!?");
	        		}
	        	}

	}

	return 0;
}
}
