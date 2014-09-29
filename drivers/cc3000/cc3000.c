/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc3000
 * @{
 * @file        cc3000.c
 * @brief       Implementation of the CC3000 SPI API
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "cc3000/cc3000_common.h"
#include "cc3000/socket.h"
#include "cc3000/cc3000_spi.h"
#include "cc3000/hci.h"
#include "cc3000/netapp.h"
#include "cc3000/wlan.h"
#include "hwtimer.h"
#include "board.h"
#include "vtimer.h"

#include "periph/spi.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Global variables */
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];

/* Internal variables */
static cc3000_spi_information_t cc3000_spi_infos;
static unsigned char wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];

/* Internal function prototypes */
static void wlan_irq_handler(void *args);
static void wlan_cb(long event, char *payload, uint8_t length);
static void wlan_write_enable_pin(unsigned char val);
static void wlan_irq_enable(void);
static void wlan_irq_disable(void);
static long wlan_read_irq_pin(void);

/*---------------------------------------------------------------------------*
 *                           CC3000 RIOT API                                 *
 *---------------------------------------------------------------------------*/

void cc3000_init(void)
{
    /* Configure CS pin */
    gpio_init_out(CC3000_CS, GPIO_NOPULL);
    /* Chip select to high (not active) */
    gpio_set(CC3000_CS);

    /* Configure CC3000 WLAN pin */
	gpio_init_out(CC3000_WLAN_EN, GPIO_NOPULL);
    /* Disable CC3000 WLAN for now */
	gpio_clear(CC3000_WLAN_EN);

	/* Initialize CC3000 SPI */
	spi_poweron(CC3000_SPI);
	spi_init_master(CC3000_SPI, SPI_CONF_SECOND_RISING, SPI_SPEED_10MHZ);

	/* Configure interrupt for CC3000 */
	gpio_init_int(CC3000_SPI_IRQ, GPIO_PULLUP, GPIO_FALLING, wlan_irq_handler, 0);
	gpio_irq_disable(CC3000_SPI_IRQ);

	/* Initialize and start CC3000 driver from Texas Instruments */
    wlan_init(wlan_cb, NULL, NULL, NULL, wlan_read_irq_pin,
              wlan_irq_enable, wlan_irq_disable, wlan_write_enable_pin);
    wlan_start(0);
}

void SpiOpen(gcSpiHandleRx pfRxHandler)
{
	cc3000_spi_infos.curr_state = CC3000_STATE_POWERUP;
	cc3000_spi_infos.SPIRxHandler = pfRxHandler;
	cc3000_spi_infos.rx_buffer = (char*) wlan_rx_buffer;
    tSLInformation.WlanInterruptEnable();
}

void SpiClose(void)
{
	tSLInformation.WlanInterruptDisable();
	tSLInformation.WriteWlanPin(WLAN_DISABLE);
}

void SpiResumeSpi(void)
{
    tSLInformation.WlanInterruptEnable();
}

long SpiWrite(unsigned char *data, unsigned short length)
{
    /** Padding **/
    size_t tx_len = (length & 0x01) ? length : length + 1;

	data[0] = CC3000_WRITE_CMD;
	data[1] = ((tx_len & 0xFF00) >> 8);
	data[2] = (tx_len & 0x00FF);
	data[3] = 0;
	data[4] = 0;

	/** Add spi header size **/
	length = tx_len + CC3000_SPI_HEADER_SIZE;
    char output[length];

	/** If CC3000 was just powered up, we need to wait for initialization **/
    if (cc3000_spi_infos.curr_state == CC3000_STATE_POWERUP) {
        while (cc3000_spi_infos.curr_state != CC3000_STATE_INITIALIZED);
    }

    switch (cc3000_spi_infos.curr_state) {
        case CC3000_STATE_INITIALIZED:
            /** First write procedure needed **/
            cc3000_spi_infos.curr_state = CC3000_STATE_FIRST_WRITE;
            gpio_clear(CC3000_CS);
            hwtimer_wait(CC3000_FIRST_WRITE_WAIT);
            spi_transfer_bytes(CC3000_SPI, (char *)data, output, CC3000_FIRST_WRITE_SIZE);
            hwtimer_wait(CC3000_FIRST_WRITE_WAIT);
            spi_transfer_bytes(CC3000_SPI, (char *)data + CC3000_FIRST_WRITE_SIZE, output + CC3000_FIRST_WRITE_SIZE, length - CC3000_FIRST_WRITE_SIZE);
            cc3000_spi_infos.curr_state = CC3000_STATE_IDLE;
            gpio_set(CC3000_CS);
            break;

        default:
            /** Generic write procedure **/
            tSLInformation.WlanInterruptDisable();
            cc3000_spi_infos.curr_state = CC3000_STATE_WRITE;
            gpio_clear(CC3000_CS);
            tSLInformation.WlanInterruptEnable();
            if (!tSLInformation.ReadWlanInterruptPin()) {
                spi_transfer_bytes(CC3000_SPI, (char *)data, 0, length);
                cc3000_spi_infos.curr_state = CC3000_STATE_IDLE;
                gpio_set(CC3000_CS);
            }
            break;
    }

    /** Blocking behavior **/
    while (CC3000_STATE_IDLE != cc3000_spi_infos.curr_state);

	return 0;
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

static void wlan_write_enable_pin(unsigned char val)
{
    if (val) {
        gpio_set(CC3000_WLAN_EN);
    }
    else {
        gpio_clear(CC3000_WLAN_EN);
    }
}

static void wlan_irq_enable(void)
{
    gpio_irq_enable(CC3000_SPI_IRQ);
}

static void wlan_irq_disable(void)
{
    gpio_irq_disable(CC3000_SPI_IRQ);
}

static long wlan_read_irq_pin(void)
{
    return gpio_read(CC3000_SPI_IRQ);
}

static void wlan_irq_handler(void *args)
{
    /* No action if interrupt pin was cleared before */
    if (!tSLInformation.ReadWlanInterruptPin()) {
        switch (cc3000_spi_infos.curr_state) {
            case CC3000_STATE_POWERUP:
                cc3000_spi_infos.curr_state = CC3000_STATE_INITIALIZED;
                break;

            case CC3000_STATE_IDLE:
                cc3000_spi_infos.curr_state = CC3000_STATE_READ_IRQ;
                gpio_clear(CC3000_CS);
                hwtimer_wait(CC3000_FIRST_WRITE_WAIT);
                spi_transfer_byte(CC3000_SPI, CC3000_READ_CMD, cc3000_spi_infos.rx_buffer);
                spi_transfer_bytes(CC3000_SPI, 0, cc3000_spi_infos.rx_buffer + 1, 9);
                uint16_t data_left = ntohs(*((uint16_t *) &cc3000_spi_infos.rx_buffer[3]));
                if (!(data_left & 0x01)) {
                    data_left++;
                }
                data_left -= CC3000_MIN_DATA_LENGTH;

                if (data_left > 0) {
                    spi_transfer_byte(CC3000_SPI, CC3000_READ_CMD, cc3000_spi_infos.rx_buffer + 10);
                    data_left--;
                    if (data_left > 0) {
                        spi_transfer_bytes(CC3000_SPI, 0, cc3000_spi_infos.rx_buffer + 11, data_left);
                    }
                }
                tSLInformation.WlanInterruptDisable();
                gpio_set(CC3000_CS);
                cc3000_spi_infos.curr_state = CC3000_STATE_IDLE;
                cc3000_spi_infos.SPIRxHandler(cc3000_spi_infos.rx_buffer + 5);
                tSLInformation.WlanInterruptEnable();
                break;

            default:
                /* For other states, nothing has to be done here */
                break;
        }
    }
}

static void wlan_cb(long event, char *payload, uint8_t length)
{
    switch (event) {
        case HCI_EVNT_WLAN_BASE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_BASE\n");
            break;

        case HCI_EVNT_WLAN_CONNECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_CONNECT\n");
            break;

        case HCI_EVNT_WLAN_DISCONNECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_DISCONNECT\n");
            break;

        case HCI_EVNT_WLAN_IOCTL_ADD_PROFILE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_IOCTL_ADD_PROFILE\n");
            break;

        case HCI_EVNT_SOCKET:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_SOCKET\n");
            break;

        case HCI_EVNT_BIND:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_BIND\n");
            break;

        case HCI_EVNT_RECV:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_RECV\n");
            break;

        case HCI_EVNT_ACCEPT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_ACCEPT\n");
            break;

        case HCI_EVNT_LISTEN:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_LISTEN\n");
            break;

        case HCI_EVNT_CONNECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_CONNECT\n");
            break;

        case HCI_EVNT_SELECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_SELECT\n");
            break;

        case HCI_EVNT_CLOSE_SOCKET:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_CLOSE_SOCKET\n");
            break;

        case HCI_EVNT_RECVFROM:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_RECVFROM\n");
            break;

        case HCI_EVNT_SETSOCKOPT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_SETSOCKOPT\n");
            break;

        case HCI_EVNT_GETSOCKOPT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_GETSOCKOPT\n");
            break;

        case HCI_EVNT_BSD_GETHOSTBYNAME:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_BSD_GETHOSTBYNAME\n");
            break;

        case HCI_EVNT_SEND:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_SEND\n");
            break;

        case HCI_EVNT_WRITE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WRITE\n");
            break;

        case HCI_EVNT_SENDTO:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_SENDTO\n");
            break;

        case HCI_EVNT_PATCHES_REQ:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_PATCHES_REQ\n");
            break;

        case HCI_EVNT_UNSOL_BASE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_UNSOL_BASE\n");
            break;

        case HCI_EVNT_WLAN_UNSOL_BASE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_UNSOL_BASE\n");
            break;

        case HCI_EVNT_DATA_UNSOL_FREE_BUFF:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_DATA_UNSOL_FREE_BUFF\n");
            break;

        case HCI_EVNT_NVMEM_CREATE_ENTRY:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_NVMEM_CREATE_ENTRY\n");
            break;

        case HCI_EVNT_NVMEM_SWAP_ENTRY:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_NVMEM_SWAP_ENTRY\n");
            break;

        case HCI_EVNT_NVMEM_READ:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_NVMEM_READ\n");
            break;

        case HCI_EVNT_NVMEM_WRITE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_NVMEM_WRITE\n");
            break;

        case HCI_EVNT_READ_SP_VERSION:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_READ_SP_VERSION\n");
            break;

        case HCI_EVNT_INPROGRESS:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_INPROGRESS\n");
            break;

        case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE\n");
            break;

        case HCI_EVNT_WLAN_UNSOL_CONNECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_UNSOL_CONNECT\n");
            break;

        case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_UNSOL_DISCONNECT\n");
            break;

        case HCI_EVNT_WLAN_UNSOL_DHCP:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_UNSOL_DHCP\n");
            break;

        case HCI_EVNT_WLAN_UNSOL_INIT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_UNSOL_INIT\n");
            break;

        case HCI_EVNT_WLAN_TX_COMPLETE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_TX_COMPLETE\n");
            break;

        case HCI_EVNT_WLAN_ASYNC_PING_REPORT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_ASYNC_PING_REPORT\n");
            break;

        case HCI_EVNT_WLAN_KEEPALIVE:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_WLAN_KEEPALIVE\n");
            break;

        case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
            DEBUGF("[DEBUG] Received event: HCI_EVENT_CC3000_CAN_SHUT_DOWN\n");
            break;

        case HCI_EVNT_BSD_TCP_CLOSE_WAIT:
            DEBUGF("[DEBUG] Received event: HCI_EVNT_BSD_TCP_CLOSE_WAIT\n");
            break;

        default:
            DEBUGF("[WARNING] Unknown event received\n");
            break;
    }
}
