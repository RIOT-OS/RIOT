/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level NRF51822 RADIO device driver implementation
 *
 * @author      Christian Kuehling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "cpu.h"
#include "nrf_radio.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#define PREPARE_TX(x) do \
    { \
        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos | \
            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos | \
            RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos; \
    } while(0)

#define PREPARE_RX(x) do \
    { \
        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos | \
            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos | \
            RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos; \
    } while(0)

static char nrf_radio_buf[NRF_RADIO_BUFSIZE];


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


int nrf_radio_init(void)
{
    /* power on the radio peripheral device */
    nrf_radio_poweron();

    /* reset radio configuration */
    NRF_RADIO->PCNF0 = 0;
    NRF_RADIO->PCNF1 = 0;
    NRF_RADIO->DACNF = 0;

    /* set default mode */
    nrf_radio_set_mode(NRF_RADIO_DEFAULT_MODE);

    /* set the default channel */
    nrf_radio_set_channel(NRF_RADIO_DEFAULT_CHANNEL);

    /* set default transmit power */
    nrf_radio_set_power(NRF_RADIO_DEFAULT_TXPOWER);

    /* TODO: disable CRC's (for now) */
    NRF_RADIO->CRCCNF = 0;

    /* packet length configuration */
    NRF_RADIO->PCNF0 = 8;           /* set length field to 8 bit -> 1 byte, S0 and S1 to 0 bit */
    uint32_t pcnf0 = NRF_RADIO->PCNF0;
    DEBUG("active pcnf0: 0x%08x\n", (int)pcnf0);

    /* Enable Big Endian */
    NRF_RADIO->PCNF1 =  (RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos);
    /* address configuration: base address configuration */
	NRF_RADIO->PCNF1 |= (NRF_RADIO_DEFAULT_BASEADDR_LENGTH << RADIO_PCNF1_BALEN_Pos);
	/* Maximum length of packet payload configuration*/
	NRF_RADIO->PCNF1 |= NRF_RADIO_MAX_PACKET_SIZE;

    uint32_t pcnf1 = NRF_RADIO->PCNF1;
    DEBUG("active pcnf1: 0x%08x\n", (int)pcnf1);

    /* As you can find in the nRF51 Reference Manual at section 16.1.2, least significant byte is
     * sent first, and least significant bit also is sent first (little endian) when the compiler
     * use store the MSBit first for each byte. So there is a bit swap for each byte needed
     */
    NRF_RADIO->BASE0 = bytewise_bitswap(0x7ee30000UL);
    /* NRF_RADIO->BASE0 = NRF_RADIO_DEFAULT_BASEADDR; */
    NRF_RADIO->PREFIX0= 0xe7UL;

    NRF_RADIO->BASE1 = NRF_RADIO_DEFAULT_BASEADDR;
    //NRF_RADIO->BASE1 = 0x00C2C2C2UL;  // Base address for prefix 1-7

    uint32_t base0 = NRF_RADIO->BASE0;
    DEBUG("active base0: 0x%08x\n", (int)base0);

    uint32_t base1 = NRF_RADIO->BASE1;
    DEBUG("active base1: 0x%08x\n", (int)base1);

    /* address configuration: prefix configuration */
/*     NRF_RADIO->PREFIX0 = NRF_RADIO_DEFAULT_PREFIX;
       NRF_RADIO->PREFIX1 = NRF_RADIO_DEFAULT_PREFIX; */
    /* define TX and RX address */
    NRF_RADIO->TXADDRESS = 0;               /* 1 := BASE0[1] + BASE0[0] + PREFIX0.AP0 */
    NRF_RADIO->RXADDRESSES = 1;
    //NRF_RADIO->RXADDRESSES = (1 << 4);      /* 1 := BASE1[1] + BASE1[0] + PREFIX1.AP4 */
    NRF_RADIO->DACNF = (1 << 4);

    /* Inter Frame Spacing in microseconds */
    NRF_RADIO->TIFS = 150;
    return 0;
}

int nrf_radio_send(uint8_t addr, char *data, int size)
{
    /* drop data if size is too large */
    if (size > NRF_RADIO_BUFSIZE) {
        DEBUG("radio: data to transmit too large to sent\n");
        return -1;
    }

    /* prepare the data to be send */
    nrf_radio_buf[0] = size;
    memcpy((void*)(&nrf_radio_buf[1]), (void*)data, size);

    /* point radio to the prepared packet */
    NRF_RADIO->PACKETPTR = (uint32_t)nrf_radio_buf;


    PREPARE_TX();

    /* set the TX address */
/*    NRF_RADIO->PREFIX0 &= ~(0xff);
    NRF_RADIO->PREFIX0 |= addr; */

    uint32_t prefix0 = NRF_RADIO->PREFIX0;
    DEBUG("radio: PREFIX0 0x%08x\n", (int)prefix0);

    /* put radio into transmit mode */
    DEBUG("radio: TXEN\n");
    NRF_RADIO->EVENTS_READY = 0;
    NRF_RADIO->TASKS_TXEN = 1;
    /* wait until radio is in TXIDLE state */
    while (NRF_RADIO->EVENTS_READY == 0);
    NRF_RADIO->EVENTS_READY = 0;

    /* start packet transmission */
    DEBUG("radio: START\n");
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->TASKS_START = 1;
    /* wait until transmission is done */
    while (NRF_RADIO->EVENTS_END == 0);
    NRF_RADIO->EVENTS_END = 0;

    /* return to DISABLED state */
    DEBUG("radio: DISABLE\n");
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    /* wait until radio was disabled */
    while (NRF_RADIO->EVENTS_DISABLED == 0);
    NRF_RADIO->EVENTS_DISABLED = 0;

    DEBUG("radio: TX-DONE\n");
    return size;
}

int nrf_radio_receive(uint8_t addr, char *data, int maxsize)
{
    if (maxsize < NRF_RADIO_BUFSIZE) {
        DEBUG("radio: receive buffer too small\n");
        return -1;
    }

    /* point RADIO device to receiving data buffer */
    NRF_RADIO->PACKETPTR = (uint32_t)data;

    PREPARE_RX();

    /* set RX address */
/*    NRF_RADIO->PREFIX1 &= ~(0xff);
      NRF_RADIO->PREFIX1 |= addr; */

    uint32_t prefix1 = NRF_RADIO->PREFIX1;
    DEBUG("radio: PREFIX1 0x%08x\n", (int)prefix1);

    /* put radio into receiver mode */
    DEBUG("radio: RXEN\n");
    NRF_RADIO->EVENTS_READY = 0;
    NRF_RADIO->TASKS_RXEN = 1;
    /* wait until radio is in RXIDLE state */
    while (NRF_RADIO->EVENTS_READY != 1);
    NRF_RADIO->EVENTS_READY = 0;

    uint32_t rxadd = NRF_RADIO->RXADDRESSES;
    DEBUG("active rx addresses: 0x%08x\n", (int)rxadd);

    /* start actual listening for packets */
    DEBUG("radio: RX\n");
    NRF_RADIO->EVENTS_END = 0;
    NRF_RADIO->TASKS_START = 1;
    /* wait until a packet with valid preamble was received */
    while (NRF_RADIO->EVENTS_END != 1);
    NRF_RADIO->EVENTS_END = 0;
    DEBUG("radio: HAPPY HAPPY, a packet was received!\n");

    /* turn of the receiving mode */
    DEBUG("radio: DISABLE\n");
    NRF_RADIO->EVENTS_DISABLED = 0;
    NRF_RADIO->TASKS_DISABLE = 1;
    /* wait until radio is in DISABLED state again */
    while (NRF_RADIO->EVENTS_DISABLED != 1);
    NRF_RADIO->EVENTS_DISABLED = 0;

    DEBUG("radio: RX-DONE\n");

    uint32_t rxmatch = NRF_RADIO->RXMATCH;
    DEBUG("radio: RXMATCH 0x%08x\n", (int)rxmatch);

    return (int)data[0];

}

void nrf_radio_set_mode(nrf_radio_mode_t mode)
{
    NRF_RADIO->MODE = (mode & 0x03);
}

void nrf_radio_set_channel(int channel)
{
    NRF_RADIO->FREQUENCY = (channel & 0x3f);
}

void nrf_radio_set_power(nrf_radio_power_t power)
{
    NRF_RADIO->TXPOWER = (power & 0xff);
}

void nrf_radio_poweron(void)
{
    NRF_RADIO->POWER = 1;
}

void nrf_radio_poweroff(void)
{
    NRF_RADIO->POWER = 0;
}
