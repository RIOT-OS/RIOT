/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file        radio.c
 * @brief       Low-level timer driver implementation
 *
 * @author      Christian Kuehling <kuehling@zedat.fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include "nrf51.h"
#include "periph/uart.h"
#include "board.h"
#include "nrf51_bitfields.h"

//#include "radio.h"


#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< payload size in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (1UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (1UL)  //!< payload size in bytes

static uint8_t packet[4];  ///< Packet to transmit
char* receivePacket(void);
void sendPacket(uint8_t addr, char* msg);
void radioConfig(void);
char* msg;
int i = 0;


char* receivePacket(void)
{

	    char* output = "Received Chars:\r\n";
	    char outputchar = output[i++];
		while ( outputchar != '\0')
		{
			uart_write_blocking(0, outputchar);
			outputchar = output[i++];
		}
		i = 0;


	  while(1)
	  {
	    NRF_RADIO->EVENTS_READY = 0U;
	    // Enable radio and wait for ready
	    NRF_RADIO->TASKS_RXEN = 1U;
	    while(NRF_RADIO->EVENTS_READY == 0U)
	    {
	    }
	    NRF_RADIO->EVENTS_END = 0U;
	    // Start listening and wait for address received event
	    NRF_RADIO->TASKS_START = 1U;
	    // Wait for end of packet
	    while(NRF_RADIO->EVENTS_END == 0U)
	    {
	    }
	    // Write received data to LED0 and LED1 on CRC match
	    if (NRF_RADIO->CRCSTATUS == 1U)
	    {
	    	//UART Output
	    	if(packet[0] != 0){
	    		uart_write(0, packet[0]);
	    	}
	    }
	    NRF_RADIO->EVENTS_DISABLED = 0U;
	    // Disable radio
	    NRF_RADIO->TASKS_DISABLE = 1U;
	    while(NRF_RADIO->EVENTS_DISABLED == 0U)
	    {
	    }
	  }
	return msg;

}


void sendPacket(uint8_t addr, char* msg)
{

    char* output = "Enter the Char to Send:\r\n";
    char outputchar = output[i++];
	while ( outputchar != '\0')
	{
		uart_write_blocking(0, outputchar);
		outputchar = output[i++];
	}
	i = 0;

	  // Set payload pointer
	  NRF_RADIO->PACKETPTR = (uint32_t)packet;

	  while(1)
	  {
		char charUart = '0';
	    uart_read_blocking(0, charUart);
	    uart_write(0,charUart);
	    // Place the read character in the payload, enable the radio and
	    // send the packet:
	    packet[0] = charUart;
	    NRF_RADIO->EVENTS_READY = 0U;
	    NRF_RADIO->TASKS_TXEN = 1;
	    while (NRF_RADIO->EVENTS_READY == 0U)
	    {
	    	//wait
	    }
	    NRF_RADIO->TASKS_START = 1U;
	    NRF_RADIO->EVENTS_END = 0U;
	    while(NRF_RADIO->EVENTS_END == 0U)
	    {
	    }
	    NRF_RADIO->EVENTS_DISABLED = 0U;
	    // Disable radio
	    NRF_RADIO->TASKS_DISABLE = 1U;
	    while(NRF_RADIO->EVENTS_DISABLED == 0U)
	    {
	    }
	  }

}


void radioConfig(void)
{
	  // Radio config
	  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
	  NRF_RADIO->FREQUENCY = 7UL;                // Frequency bin 7, 2407MHz
	  NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos);

	  // Radio address config
	  NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
	  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
	  NRF_RADIO->BASE0   = 0xE7E7E7E7UL;  // Base address for prefix 0
	  NRF_RADIO->BASE1   = 0x00C2C2C2UL;  // Base address for prefix 1-7
	  NRF_RADIO->TXADDRESS = 0x00UL;      // Set device address 0 to use when transmitting
	  NRF_RADIO->RXADDRESSES = 0x01UL;    // Enable device address 0 to use which receiving

	  // Packet configuration
	  NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos) |
	                     (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos) |
	                     (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

	  // Packet configuration
	   NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)    |
	                      (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)           |
	                      (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)       |
	                      (PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)           |
	                      (PACKET1_PAYLOAD_SIZE << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

	  // CRC Config
	  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
	  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
	  {
	    NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
	    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
	  }
	  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
	  {
	    NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
	    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
	  }

	  nrf_delay_ms(3);
}
