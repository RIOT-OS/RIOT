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
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
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
#define PACKET0_PAYLOAD_SIZE             (8UL)  //!< payload size in bits
#define PACKET1_BASE_ADDRESS_LENGTH      (4UL)  //!< base address length in bytes
#define PACKET1_STATIC_LENGTH            (1UL)  //!< static length in bytes
#define PACKET1_PAYLOAD_SIZE             (1UL)  //!< payload size in bytes
#define MAX_PACKET_LENGTH         255 //Max Packet Length (Payload =-2)

void delay(uint32_t microseconds);

volatile static uint8_t packet[MAX_PACKET_LENGTH];  ///< Packet to transmit

char msg;
int i = 0;

/* These are set to zero as Shockburst packets don't have corresponding fields. */
#define PACKET_S1_FIELD_SIZE             (0UL)  /**< Packet S1 field size in bits. */
#define PACKET_S0_FIELD_SIZE             (0UL)  /**< Packet S0 field size in bits. */
#define PACKET_LENGTH_FIELD_SIZE         (0UL)  /**< Packet length field size in bits. */

/**
 * The following defines and the led function
 * help us to debug this code by pca10000's onboard LED
 **/
#define LED_NONE 0
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3
#define LED_PURPLE 4
#define LED_CYAN 5
#define LED_YELLOW 6
#define LED_WHITE 7

void delay(uint32_t microseconds){
    /* perform busy-waiting for specified number of microseconds  */
    uint32_t cycles = microseconds ;
    for (int i = 0; i < cycles; i++) {
        asm("nop");
    }

}

void led(int color) {
#if ONBOARD_LED
  switch (color) {
  case LED_NONE:
    LED_RED_OFF;
    LED_GREEN_OFF;
    LED_BLUE_OFF;
    break;
  case LED_RED:
    led(LED_NONE);
    LED_RED_ON;
    LED_GREEN_OFF;
    LED_BLUE_OFF;
    break;
  case LED_GREEN:
    led(LED_NONE);
    LED_RED_OFF;
    LED_GREEN_ON;
    LED_BLUE_OFF;
    break;
  case LED_BLUE:
    led(LED_NONE);
    LED_RED_OFF;
    LED_GREEN_OFF;
    LED_BLUE_ON;
    break;
  case LED_PURPLE:
    led(LED_NONE);
    LED_RED_ON;
    LED_GREEN_OFF;
    LED_BLUE_ON;
    break;
  case LED_CYAN:
    led(LED_NONE);
    LED_RED_OFF;
    LED_GREEN_ON;
    LED_BLUE_ON;
    break;
  case LED_YELLOW:
    led(LED_NONE);
    LED_RED_ON;
    LED_GREEN_ON;
    LED_BLUE_OFF;
    break;
  case LED_WHITE:
  default:
    led(LED_NONE);
    LED_RED_ON;
    LED_GREEN_ON;
    LED_BLUE_ON;
    break;
  }
#endif
}

/**
 * @brief Function for generating an 8 bit random number using the internal random generator.
 */
static uint32_t rnd8(void) {
  NRF_RNG->EVENTS_VALRDY = 0;
  while (NRF_RNG->EVENTS_VALRDY == 0) {
    // Do nothing.
  }
  return NRF_RNG->VALUE;
}

char receivePacket(void) {

  //status 0: led 1 led 2 off || LED_RED_ON --> radio disabled
  //status 2: led 1 on led 2 off || LED_PURPLE_ON --> Radio is in RXIDLE state
  //status 3: led 1 off led 2 on || LED_YELLOW_ON --> Radio is in the RX state
  //status 4: led 1 on led 2 on || LED_GREEN_ON --> Radio is in the RXDISABLE state --> msg received

  static uint8_t packet[MAX_PACKET_LENGTH];
  short boolvar = 0;

  while (1) {
    // Set payload pointer
    NRF_RADIO->PACKETPTR = (uint32_t) packet;

    //Radio disabled state 0
    gpio_clear(GPIO_1);
    gpio_clear(GPIO_6);


    led(LED_RED);
    delay(1000000);

    NRF_RADIO->EVENTS_READY = 0U;
    // Enable radio and wait for ready
    NRF_RADIO->TASKS_RXEN = 1U;

    //printf( "radio state: %i\n", (int)(NRF_RADIO->STATE ));
    while (NRF_RADIO->EVENTS_READY == 0U) {
      //printf( "radio state: %i\n", (int)(NRF_RADIO->STATE ));

    }

    //RXIDLE-State 2
    led(LED_PURPLE);
    gpio_set(GPIO_1);
    gpio_clear(GPIO_6);

    delay(5 * 100 * 1000);

    NRF_RADIO->EVENTS_END = 0U;
    // Start listening and wait for address received event
    NRF_RADIO->TASKS_START = 1U;

    //Radio is in RX-State 3
    gpio_clear(GPIO_1);
    gpio_set(GPIO_6);
    led(LED_YELLOW);
    delay(5 * 100 * 1000);
    // Wait for end of packet
    while (NRF_RADIO->EVENTS_END == 0U) {
      //Radio is still in RX-State 3
    }
    printf("Packet rcv!: %s\n", (char *)NRF_RADIO->PACKETPTR);

    msg = (char) packet[1];

    // Write received data to LED0 and LED1 on CRC match
    if (NRF_RADIO->CRCSTATUS == 1U) {
      msg = (char) packet[1];
      boolvar = 1;
    }

    //Radio is in RXDISABLE Mode
    gpio_set(GPIO_1);
    gpio_set(GPIO_6);
    led(LED_GREEN);

    delay(5 * 100 * 1000);

    //printf( "radio state: %i\n", (int)(NRF_RADIO->STATE ));
    NRF_RADIO->EVENTS_DISABLED = 0U;
    // Disable radio
    NRF_RADIO->TASKS_DISABLE = 1U;
    //printf( "radio state: %i\n", (int)(NRF_RADIO->STATE ));
    while (NRF_RADIO->EVENTS_DISABLED == 0U) {
    }

    if (boolvar)
      break;

  }

  printf("PACKET RECEIVED2: %c ------------------------------\n", msg);

  led(LED_NONE);
  gpio_clear(GPIO_1);
  gpio_clear(GPIO_6);

  return msg;

}

void sendPacket(uint8_t addr, char msg) {

  //status 0: led 1 led 2 off || LED_RED_ON --> radio disabled
  //status 10: led 1 on led 2 off || LED_YELLOW_ON --> Radio is in the TXIDLE state
  //status 11: led 1 on led 2 on || LED_GREEN_ON --> Radio is in the TX state

  //START Random Number Generator
  NRF_RNG->TASKS_START = 1;

  packet[0] = MAX_PACKET_LENGTH - 2;    // 254 bytes payload.
  // Fill payload with random data:
  for (i = 0; i < (MAX_PACKET_LENGTH - 2); i++) {
    packet[i + 1] = rnd8();
  }

  // Set payload pointer
  NRF_RADIO->PACKETPTR = (uint32_t) packet;

  led(LED_RED);
  gpio_clear(GPIO_1);
  gpio_clear(GPIO_6);

  delay(5 * 100 * 1000);

  NRF_RADIO->EVENTS_READY = 0U;
  NRF_RADIO->TASKS_TXEN = 1U;

  while (NRF_RADIO->EVENTS_READY == 0U) {
  }

  NRF_RADIO->EVENTS_END = 0U;
  NRF_RADIO->TASKS_START = 1U;

  //RADIO state 10
  gpio_set(GPIO_1);
  gpio_clear(GPIO_6);
  led(LED_YELLOW);

  while (NRF_RADIO->EVENTS_END == 0U) {
    delay(5 * 100 * 1000);
  }

  //RADIO state 11
  gpio_set(GPIO_1);
  gpio_set(GPIO_6);
  led(LED_GREEN);
  delay(5 * 100 * 1000);

  printf("send packet[1]= %lu towards prefix %lu |base %lu| \n",
      (unsigned long) packet[1], (unsigned long) NRF_RADIO->PREFIX0,
      (unsigned long) NRF_RADIO->BASE0);

  NRF_RADIO->EVENTS_DISABLED = 0U;
  NRF_RADIO->TASKS_DISABLE = 1U;
  while (NRF_RADIO->EVENTS_DISABLED == 0U) {
  }

  gpio_clear(GPIO_1);
  gpio_clear(GPIO_6);
  led(LED_NONE);
}

/** @brief Function for disabling the radio.
 */
static void radio_disable(void) {
  NRF_RADIO->SHORTS = 0;
  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->TEST = 0;
  NRF_RADIO->TASKS_DISABLE = 1;
  while (NRF_RADIO->EVENTS_DISABLED == 0) {
    // Do nothing.
  }
  NRF_RADIO->EVENTS_DISABLED = 0;
}

void radioConfig(void) {
  radio_disable();

  // Radio config
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_0dBm
      << RADIO_TXPOWER_TXPOWER_Pos);
  NRF_RADIO->FREQUENCY = 7UL;                // Frequency bin 7, 2407MHz
  NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);

  // Radio address config
  NRF_RADIO->PREFIX0 = 0xC4C3C2E7UL;  // Prefix byte of addresses 3 to 0
  NRF_RADIO->PREFIX1 = 0xC5C6C7C8UL;  // Prefix byte of addresses 7 to 4
  NRF_RADIO->BASE0 = 0xE7E7E7E7UL;  // Base address for prefix 0
  NRF_RADIO->BASE1 = 0x00C2C2C2UL;  // Base address for prefix 1-7
  NRF_RADIO->TXADDRESS = 0x00UL; // Set device address 0 to use when transmitting

  // Enable device address 0 to use which receiving
  NRF_RADIO->RXADDRESSES = 0x01UL;
  printf("RXADDRESSES: %i\n", (int) NRF_RADIO->RXADDRESSES);

  // Packet configuration
  NRF_RADIO->PCNF0 = (PACKET0_S1_SIZE << RADIO_PCNF0_S1LEN_Pos)
      | (PACKET0_S0_SIZE << RADIO_PCNF0_S0LEN_Pos)
      | (PACKET0_PAYLOAD_SIZE << RADIO_PCNF0_LFLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

  // Packet configuration
  NRF_RADIO->PCNF1 = (RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)
      | (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)
      | (PACKET1_BASE_ADDRESS_LENGTH << RADIO_PCNF1_BALEN_Pos)
      | (PACKET1_STATIC_LENGTH << RADIO_PCNF1_STATLEN_Pos)
      | (255UL << RADIO_PCNF1_MAXLEN_Pos); //lint !e845 "The right argument to operator '|' is certain to be 0"

  // CRC Config
  NRF_RADIO->CRCCNF = (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos); // Number of checksum bits
  if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk)
      == (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos)) {
    NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
    NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1
  } else if ((NRF_RADIO->CRCCNF & RADIO_CRCCNF_LEN_Msk)
      == (RADIO_CRCCNF_LEN_One << RADIO_CRCCNF_LEN_Pos)) {
    NRF_RADIO->CRCINIT = 0xFFUL;        // Initial value
    NRF_RADIO->CRCPOLY = 0x107UL;       // CRC poly: x^8+x^2^x^1+1
  }

  delay(1 * 1000 * 1000);
}

