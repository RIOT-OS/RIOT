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
#include "periph/gpio.h"

#include "radio.h"


#define PACKET0_S1_SIZE                  (0UL)  //!< S1 size in bits
#define PACKET0_S0_SIZE                  (0UL)  //!< S0 size in bits
#define PACKET0_PAYLOAD_SIZE             (0UL)  //!< payload size in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (1UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (1UL)  //!< payload size in bytes

static uint8_t packet[4];  ///< Packet to transmit

char msg;
int i = 0;


/* These are set to zero as Shockburst packets don't have corresponding fields. */
#define PACKET_S1_FIELD_SIZE             (0UL)  /**< Packet S1 field size in bits. */
#define PACKET_S0_FIELD_SIZE             (0UL)  /**< Packet S0 field size in bits. */
#define PACKET_LENGTH_FIELD_SIZE         (0UL)  /**< Packet length field size in bits. */

/**
 * @brief Function for swapping/mirroring bits in a byte.
 *
 *@verbatim
 * output_bit_7 = input_bit_0
 * output_bit_6 = input_bit_1
 *           :
 * output_bit_0 = input_bit_7
 *@endverbatim
 *
 * @param[in] inp is the input byte to be swapped.
 *
 * @return
 * Returns the swapped/mirrored input byte.
 */
static uint32_t swap_bits(uint32_t inp);

/**
 * @brief Function for swapping bits in a 32 bit word for each byte individually.
 *
 * The bits are swapped as follows:
 * @verbatim
 * output[31:24] = input[24:31]
 * output[23:16] = input[16:23]
 * output[15:8]  = input[8:15]
 * output[7:0]   = input[0:7]
 * @endverbatim
 * @param[in] input is the input word to be swapped.
 *
 * @return
 * Returns the swapped input byte.
 */
static uint32_t bytewise_bitswap(uint32_t inp);

static uint32_t swap_bits(uint32_t inp)
{
    uint32_t i;
    uint32_t retval = 0;

    inp = (inp & 0x000000FFUL);

    for(i = 0; i < 8; i++)
    {
        retval |= ((inp >> i) & 0x01) << (7 - i);
    }

    return retval;
}


static uint32_t bytewise_bitswap(uint32_t inp)
{
      return (swap_bits(inp >> 24) << 24)
           | (swap_bits(inp >> 16) << 16)
           | (swap_bits(inp >> 8) << 8)
           | (swap_bits(inp));
}


char receivePacket(void)
{

	static uint8_t packet[4];
	short boolvar = 0;

	  while(1)
	  {

		NRF_RADIO->PACKETPTR = (uint32_t) packet;
	    NRF_RADIO->EVENTS_READY = 0U;
	    // Enable radio and wait for ready
	    NRF_RADIO->TASKS_RXEN = 1U;
	    gpio_set(GPIO_1);
	    gpio_clear(GPIO_6);
	    while(NRF_RADIO->EVENTS_READY == 0U)
	    {
	    }
	    delay(500000);
	    gpio_clear(GPIO_1);
	    gpio_clear(GPIO_6);

	    NRF_RADIO->EVENTS_END = 0U;
	    // Start listening and wait for address received event
	    NRF_RADIO->TASKS_START = 1U;


	    gpio_clear(GPIO_1);
	    gpio_set(GPIO_6);
	    // Wait for end of packet
	    while(NRF_RADIO->EVENTS_END == 0U)
	    {
	    	gpio_toggle(GPIO_6);
        	delay(1*1000*1000);
	    }
	    delay(500000);
	    gpio_clear(GPIO_1);
	    gpio_clear(GPIO_6);


	    delay(2000000);


	    // Write received data to LED0 and LED1 on CRC match
//	    if (NRF_RADIO->CRCSTATUS == 1U)
//	    {

		    gpio_set(GPIO_1);
		    gpio_set(GPIO_6);
	    	msg = (char)packet[1];
	    	boolvar = 1;
	    	delay(1000000);
//	    }

	    gpio_clear(GPIO_1);
	    gpio_clear(GPIO_6);
	    NRF_RADIO->EVENTS_DISABLED = 0U;
	    // Disable radio
	    NRF_RADIO->TASKS_DISABLE = 1U;
	    while(NRF_RADIO->EVENTS_DISABLED == 0U)
	    {
	    }

	        	delay(1*1000*1000);

	    if(boolvar) break;

	    for( int i = 0; i < 10;i++)  {
			gpio_toggle(GPIO_6);
			delay(500*1000);
	    }
	    for( int i = 0; i < 10;i++)  {
			gpio_toggle(GPIO_1);
			delay(500*1000);
	    }
	  }
	return msg;

}


void sendPacket(uint8_t addr, char msg)
{

	LED_GREEN_OFF;

	  // Set payload pointer
	  NRF_RADIO->PACKETPTR = *packet;
	  //NRF_RADIO->PACKETPTR = (uint32_t)packet;


	    // Place the read character in the payload, enable the radio and
	    // send the packet:

	    packet[0] = 1;
	    packet[1] = 'Q';
	    NRF_RADIO->EVENTS_READY = 0U;

	    NRF_RADIO->TASKS_TXEN = 1;
	    printf("task txen\n");
	    while (NRF_RADIO->EVENTS_READY == 0U)
	    {
	    	delay(5*100*1000);
	    	LED_RED_ON;
	    	delay(5*100*1000);
	    	LED_RED_OFF;
	    }
	    printf("ready\n");

	    NRF_RADIO->EVENTS_END = 0U;
	    NRF_RADIO->TASKS_START = 1U;
	    while(NRF_RADIO->EVENTS_END == 0U)
	    {
	    	delay(5*100*1000);
	    	LED_BLUE_ON;
	    	delay(5*100*1000);
	    	LED_BLUE_OFF;
	    }
	    printf("packet sent\n");
	    // Disable radio

	    NRF_RADIO->EVENTS_DISABLED = 0U;
	    NRF_RADIO->TASKS_DISABLE = 1U;
	    while(NRF_RADIO->EVENTS_DISABLED == 0U)
	    {
	    }
	    printf("disabled\n");

LED_GREEN_ON;
}


void radioConfig(short me)
{

//	 /* Start 16 MHz crystal oscillator */
//	  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
//	  NRF_CLOCK->TASKS_HFCLKSTART = 1;
//
//	  /* Wait for the external oscillator to start up */
//	  while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0)
//	  {
//	  }

	  // Radio config
	  if(me != 1)  {
		  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm << RADIO_TXPOWER_TXPOWER_Pos);
		  //NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk;
	  }
	  else  {
		 // NRF_RADIO->SHORTS = (RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_DISABLE_Msk);
	  }
	  NRF_RADIO->FREQUENCY = 7UL;                // Frequency bin 7, 2407MHz
	  NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);

	  // Radio address config
	  /*NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
	  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
	 */
	  // Radio address config
	      NRF_RADIO->PREFIX0 =
	          ((uint32_t)swap_bits(0xC3) << 24) // Prefix byte of address 3 converted to nRF24L series format
	        | ((uint32_t)swap_bits(0xC2) << 16) // Prefix byte of address 2 converted to nRF24L series format
	        | ((uint32_t)swap_bits(0xC1) << 8)  // Prefix byte of address 1 converted to nRF24L series format
	        | ((uint32_t)swap_bits(0xC0) << 0); // Prefix byte of address 0 converted to nRF24L series format

	      NRF_RADIO->PREFIX1 =
	          ((uint32_t)swap_bits(0xC7) << 24) // Prefix byte of address 7 converted to nRF24L series format
	        | ((uint32_t)swap_bits(0xC6) << 16) // Prefix byte of address 6 converted to nRF24L series format
	        | ((uint32_t)swap_bits(0xC4) << 0); // Prefix byte of address 4 converted to nRF24L series format

	  NRF_RADIO->BASE0       = bytewise_bitswap(0x01234567UL);  // Base address for prefix 0 converted to nRF24L series format
	  NRF_RADIO->BASE1       = bytewise_bitswap(0x89ABCDEFUL);  // Base address for prefix 1-7 converted to nRF24L series format


		  NRF_RADIO->TXADDRESS = 0x01UL;      // Set device address 0 to use when transmitting
		  NRF_RADIO->RXADDRESSES = 0x0UL | (0x1UL) | (0x1UL << 1) | (0x1UL << 2) | (0x1UL << 3) | (0x1UL << 4) | (0x1UL << 5) | (0x1UL << 6) | (0x1UL << 7);    // Enable device address 0 to use which receiving
		  printf("rxaddresses %i\n", NRF_RADIO->RXADDRESSES);

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
//	  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
//	  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos))
//	  {
//	    NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
//	    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
//	  }
//	  else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk) == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos))
//	  {
//	    NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
//	    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
//	  }


}


