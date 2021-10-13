/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cc110x.h"
#include "cc110x_internal.h"
#include "cc110x_params.h"

#define CC110X_NUM     (sizeof(cc110x_params) / sizeof(cc110x_params[0]))

extern cc110x_t _cc110x_devs[CC110X_NUM];

static const char *pa_settings[] = {
    "-30",
    "-20",
    "-15",
    "-10",
    "0",
    "+5",
    "+7",
    "+10",
};

static const char *state2s(cc110x_state_t state)
{
    switch (state) {
    case CC110X_STATE_IDLE:
        return "IDLE";
    case CC110X_STATE_FRAME_READY:
        return "Frame ready";
    case CC110X_STATE_OFF:
        return "Off";
    case CC110X_STATE_RX_MODE:
        return "RX mode";
    case CC110X_STATE_RECEIVING:
        return "RX mode and currently receiving frame";
    case CC110X_STATE_TX_MODE:
        return "TX";
    case CC110X_STATE_TX_COMPLETING:
        return "TX completing";
    case CC110X_STATE_FSTXON:
        return "Fast TX on";
    case CC110X_STATE_CALIBRATE:
        return "Calibrating";
    case CC110X_STATE_SETTLING:
        return "Settling";
    case CC110X_STATE_RXFIFO_OVERFLOW:
        return "RX FIFO overflow";
    case CC110X_STATE_TXFIFO_UNDERFLOW:
        return "TX FIFO underflow";
    }
    return "Unknown";
}

static const char *gdoconf2s(uint8_t conf)
{
    switch (conf) {
    case CC110X_GDO_ON_RX_DATA:
        return "High when frame received or RX FIFO needs draining";
    case CC110X_GDO_ON_TX_DATA:
        return "Low when TX FIFO needs refilling";
    case CC110X_GDO_ON_TRANSMISSION:
        return "High while frame is incoming / outgoing";
    case CC110X_GDO_ON_CHANNEL_CLEAR:
        return "High when channel is clear";
    case CC110X_GDO_ON_PLL_IN_LOCK:
        return "High when frequency generator is on and PLL is in lock";
    case CC110X_GDO_CONSTANT_LOW:
        return "Constant low";
    case CC110X_GDO_CONSTANT_HIGH:
        return "Constant high";
    case CC110X_GDO0_ANALOG_TEMPERATURE:
        return "Analog output of the temperature sensor (GDO0 only)";
    }

    return "Unknown";
}

static inline const char *b2s(int boolean)
{
    return (boolean) ? "1" : "0";
}

static void print_state(cc110x_t *dev)
{
    uint8_t status;
    uint8_t pktstatus;
    int8_t rssi_raw;
    uint8_t iocfg2;
    uint8_t iocfg0;
    uint8_t txbytes;
    uint8_t rxbytes;
    uint8_t hwaddr;
    uint8_t frend0;
    uint8_t physical_channel;
    uint8_t virtual_channel;

    /* Get all required data and release device */
    cc110x_acquire(dev);

    if (dev->state == CC110X_STATE_OFF) {
        cc110x_release(dev);
        puts("Device in SLEEP mode");
        return;
    }

    /* Reading out the RSSI changes it, as SPI communication seems to generate
     * some noise. Reading the RSSI out first yields up to 20 dBm lower
     * values... (E.g. about -100dBm instead of about -80dBm with no
     * other sources of Sub-GHz RF)
     */
    status = cc110x_read_reliable(dev, CC110X_REG_RSSI, (uint8_t *)&rssi_raw);
    cc110x_read_reliable(dev, CC110X_REG_PKTSTATUS, &pktstatus);
    cc110x_read(dev, CC110X_REG_IOCFG2, &iocfg2);
    cc110x_read(dev, CC110X_REG_IOCFG0, &iocfg0);
    cc110x_read(dev, CC110X_REG_ADDR, &hwaddr);
    cc110x_read(dev, CC110X_REG_FREND0, &frend0);
    cc110x_read(dev, CC110X_REG_CHANNR, &physical_channel);
    virtual_channel = dev->channel;
    cc110x_read_reliable(dev, CC110X_REG_TXBYTES, &txbytes);
    cc110x_read_reliable(dev, CC110X_REG_RXBYTES, &rxbytes);
    cc110x_state_t sw_state = dev->state;
    cc110x_release(dev);

    /* Parse obtained raw data */
    cc110x_state_t hw_state = (status >> 4) & 0x03;
    int ready      = !(status & 0x80);
    int rssi       = ((int)rssi_raw / 2) - (int)dev->rssi_offset;
    int gdo0       = pktstatus & CC110X_PKTSTATUS_GDO0;
    int gdo2       = pktstatus & CC110X_PKTSTATUS_GDO2;
    int recv       = pktstatus & CC110X_PKTSTATUS_RECEIVING;
    int cca        = pktstatus & CC110X_PKTSTATUS_CCA;
    int cs         = pktstatus & CC110X_PKTSTATUS_CS;
    const char *pa = pa_settings[frend0 & 0x07];

    /* Print all information */
    if (!ready) {
        puts("  CRITICAL: Crystal has not stabilized yet!");
    }
    printf("  GDO0: %s (%s)\n", b2s(gdo0), gdoconf2s(iocfg0));
    printf("  GDO2: %s (%s)\n", b2s(gdo2), gdoconf2s(iocfg2));
    printf("  Receiving: %s, CCA: %s, CS: %s\n", b2s(recv), b2s(cca), b2s(cs));
    printf("  RSSI: %i.%sdBm, TX power: %sdBm\n",
           rssi, (rssi_raw & 1) ? "5" : "0", pa);
    printf("  L2 addr: %02x, physical channel: %u, virtual channel: %u\n",
           (int)hwaddr, (unsigned)physical_channel, (unsigned)virtual_channel);
    if (dev->channels->map[virtual_channel] != physical_channel) {
        puts("  WARNING: Physical channel does not match channel map");
    }
    printf("  Driver state: %s, transceiver state: %s\n",
           state2s(sw_state), state2s(hw_state));
    if (hw_state != (sw_state & 0x07)) {
        puts("  WARNING: Transceiver and device state don't match!");
    }
    if (0x80 & txbytes) {
        puts("  TX FIFO: Underflown!");
    }
    else {
        printf("  TX FIFO: %iB\n", (int)txbytes);
    }
    if (0x80 & rxbytes) {
        puts("  RX FIFO: Overflown!");
    }
    else {
        printf("  RX FIFO: %iB\n", (int)rxbytes);
    }
}

int sc_cc110x(int argc, char **argv)
{
    switch (argc) {
    case 1:
        for (unsigned i = 0; i < CC110X_NUM; i++){
            printf("CC110x #%u:\n", i);
            print_state(&_cc110x_devs[i]);
        }
        break;
    case 2:
        if ((!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "--help"))) {
            printf("Usage: %s [NUM]\n"
                   "\n"
                   "Prints the status of the CC1100/CC1101 transceiver "
                   "identified by NUM, or of\n"
                   "all available CC110x transceivers if no argument is "
                   "given\n", argv[0]);
        }
        else {
            unsigned pos = atoi(argv[1]);
            if (pos >= CC110X_NUM) {
                puts("No such transceiver");
                return EXIT_FAILURE;
            }
            printf("CC110x #%u:\n", pos);
            print_state(&_cc110x_devs[pos]);
        }
        break;
    default:
        printf("Usage: %s [NUM]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int sc_cc110x_sleep(int argc, char **argv)
{
    switch (argc) {
    case 1:
        for (unsigned i = 0; i < CC110X_NUM; i++){
            printf("Putting to sleep CC110x #%u:\n", i);
            cc110x_sleep(&_cc110x_devs[i]);
        }
        break;
    case 2:
        if ((!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "--help"))) {
            printf("Usage: %s [NUM]\n"
                    "\n"
                    "Sets into sleep mode the CC1100/CC1101 transceiver "
                    "identified by NUM, or of\n"
                    "all available CC110x transceivers if no argument is "
                    "given\n", argv[0]);
        }
        else {
            unsigned pos = atoi(argv[1]);
            if (pos >= CC110X_NUM) {
                puts("No such transceiver");
                return EXIT_FAILURE;
            }
            printf("Putting to sleep CC110x #%u:\n", pos);
            cc110x_sleep(&_cc110x_devs[pos]);
        }
        break;
    default:
        printf("Usage: %s [NUM]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int sc_cc110x_wakeup(int argc, char **argv)
{
    switch (argc) {
    case 1:
        for (unsigned i = 0; i < CC110X_NUM; i++){
            printf("Waking up CC110x #%u:\n", i);
            cc110x_wakeup(&_cc110x_devs[i]);
        }
        break;
    case 2:
        if ((!strcmp(argv[1], "-h")) || (!strcmp(argv[1], "--help"))) {
            printf("Usage: %s [NUM]\n"
                    "\n"
                    "Wakes up the CC1100/CC1101 transceiver "
                    "identified by NUM, or of\n"
                    "all available CC110x transceivers if no argument is "
                    "given\n", argv[0]);
        }
        else {
            unsigned pos = atoi(argv[1]);
            if (pos >= CC110X_NUM) {
                puts("No such transceiver");
                return EXIT_FAILURE;
            }
            printf("Waking up CC110x #%u:\n", pos);
            cc110x_wakeup(&_cc110x_devs[pos]);
        }
        break;
    default:
        printf("Usage: %s [NUM]\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
