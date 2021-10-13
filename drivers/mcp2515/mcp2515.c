/*
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp2515
 * @{
 *
 * @file
 * @brief       mcp2515 can spi driver
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Wouter Symons <wosym@airsantelmo.com>
 * @}
 */

#include <string.h>

#include "xtimer.h"
#include "mcp2515.h"
#include "mcp2515_spi.h"
#include "mcp2515_defines.h"

#include "can/candev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* reset delay should be at least 2 microseconds */
#define RESET_DELAY_US 2

/* size of transmission and reception buffers in mcp2515 */
#define BUFFER_SIZE 13

/* macros for getting the TX and RX control registers */
#define MCP2515_TX_CTRL(mailbox) ((MCP2515_TXB0CTRL) + ((mailbox) << 4))
#define MCP2515_RX_CTRL(mailbox) ((MCP2515_RXB0CTRL) + ((mailbox) << 4))

/* length of the fixed part of a can message: 4 bytes can_id + 1 byte can_dlc */
#define CAN_FIXED_LEN 5

/* oscillator startup time
 * 128 cycles @ clock freq + some extra */
static inline uint32_t _osc_startup(candev_mcp2515_t *dev)
{
    return (128 / (dev->conf->clk / 1000000) + 2);
}

/**
 * @brief enable MCP2515 interrupt @p irq
 *
 * @param[in]  dev          device descriptor
 * @param[in]  irq          interrupt to enable
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
static int _mcp2515_enable_irq(candev_mcp2515_t *dev, uint8_t irq);

/**
 * @brief Fill tx/rx standard buffer instruction from filter identifier @p id
 *
 * The Read RX Buffer instruction provides a means to quickly address a receive
 * buffer for reading. This instruction reduces the SPI overhead by one byte,
 * the address byte. The command byte actually has four possible values that
 * determine the address pointer location.
 *
 * The Load TX Buffer instruction eliminates the eight-bit address required by
 * a normal write command. The eight-bit instruction sets the address pointer
 * to one of six addresses to quickly write to a transmit buffer that points to
 * the “ID” or “data” address of any of the three transmit buffers.
 *
 * @param[in]  id           filter identifier in the MCP2515 mailbox
 * @param|out] bytebuf      buffer instruction
 */
static void _fill_standard_id(uint32_t id, uint8_t *bytebuf);

/**
 * @brief Fill tx/rx extended buffer instruction from filter identifier @p id
 *
 * for more details see _fill_standard_id.
 *
 * @param[in]  id           filter identifier in the MCP2515 mailbox
 * @param|out] bytebuf      buffer instruction
 */
static void _fill_extended_id(uint32_t id, uint8_t *bytebuf);

int mcp2515_init(candev_mcp2515_t *dev, void (*irq_handler_cb)(void *))
{
    int res;

    res = gpio_init_int(dev->conf->int_pin, GPIO_IN_PU, GPIO_FALLING,
                        (gpio_cb_t)irq_handler_cb, (void *)dev);
    if (res != 0) {
        DEBUG("Error setting interrupt pin!\n");
        return -1;
    }
    gpio_init(dev->conf->rst_pin, GPIO_OUT);

    res = mcp2515_spi_init(dev);
    if (res < 0) {
        return -1;
    }

    uint8_t cmd = MCP2515_RXB0CTRL_MODE_RECV_ALL;

    res = mcp2515_spi_write(dev, MCP2515_RXB0CTRL, &cmd, 1);
    if (res < 0) {
        DEBUG("failed to set acceptance mode\n");
        return -1;
    }
    return 0;
}

void mcp2515_reset(candev_mcp2515_t *dev)
{
    gpio_clear(dev->conf->rst_pin);
    xtimer_usleep(RESET_DELAY_US);
    gpio_set(dev->conf->rst_pin);
    xtimer_usleep(_osc_startup(dev));
}

static void _fill_standard_id(uint32_t id, uint8_t *bytebuf)
{
    bytebuf[0] = (uint8_t)((id & 0x000007F8UL) >> 3);   /* T/RXBnSIDH */
    bytebuf[1] = (uint8_t)((id & 0x00000007UL) << 5);   /* T/RXBnSIDL */
    bytebuf[2] = (uint8_t)((id & 0xFF000000UL) >> 24);  /* T/RXBnEID8 */
    bytebuf[3] = (uint8_t)((id & 0x00FF0000UL) >> 16);  /* T/RXBnEID0 */
}

static void _fill_extended_id(uint32_t id, uint8_t *bytebuf)
{
    bytebuf[0] = (uint8_t)((id & 0x1FE00000UL) >> 21);          /* T/RXBnSIDH */
    bytebuf[1] = (uint8_t)((id & 0x001C0000UL) >> 13)
                 | (uint8_t)((id & 0x00030000UL) >> 16) | 0x08; /* T/RXBnSIDL */
    bytebuf[2] = (uint8_t)((id & 0x0000FF00UL) >> 8);           /* T/RXBnEID8 */
    bytebuf[3] = (uint8_t)(id & 0x000000FFUL);                  /* T/RXBnEID0 */
}

int mcp2515_send(candev_mcp2515_t *dev, const struct can_frame *frame,
                 int mailbox)
{
    uint8_t prio = 1;
    uint8_t outbuf[BUFFER_SIZE];
    uint8_t ctrl;

    struct can_frame framebuf;

    if (frame->can_dlc > CAN_MAX_DLEN) {
        return -1;
    }

    framebuf.can_id = frame->can_id;
    framebuf.can_dlc = frame->can_dlc;
    for (int i = 0; i < framebuf.can_dlc; i++) {
        framebuf.data[i] = frame->data[i];
    }

    mcp2515_spi_read(dev, MCP2515_TX_CTRL(mailbox), &ctrl, 1);
    if (ctrl & MCP2515_TXBCTRL_TXREQ) {
        DEBUG("Mailbox in use, TXB%dCTRL: 0x%02x\n", mailbox, ctrl);
        return -1;
    }

    if (framebuf.can_id > CAN_SFF_MASK) {
        framebuf.can_id |= CAN_EFF_FLAG;
    }

    if ((framebuf.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        _fill_extended_id(framebuf.can_id, outbuf);
    }
    else {
        _fill_standard_id(framebuf.can_id, outbuf);
    }

    outbuf[4] = framebuf.can_dlc;
    memcpy(&outbuf[CAN_FIXED_LEN], framebuf.data, framebuf.can_dlc);

    /* set mailbox priority */
    mcp2515_spi_write(dev, MCP2515_TX_CTRL(mailbox), &prio, 1);

    mcp2515_spi_write_txbuf(dev, mailbox, outbuf,
                            CAN_FIXED_LEN + framebuf.can_dlc);
    _mcp2515_enable_irq(dev, MCP2515_CANINTE_TX0IE << mailbox);
    mcp2515_spi_rts(dev, mailbox);

    return mailbox;
}

int mcp2515_receive(candev_mcp2515_t *dev, struct can_frame *frame, int mailbox)
{
    uint8_t inbuf[BUFFER_SIZE];

    mcp2515_spi_read_rxbuf(dev, mailbox, inbuf, BUFFER_SIZE);

    /* extended id */
    if (inbuf[1] & MCP2515_RX_IDE) {
        frame->can_id = ((uint32_t)inbuf[0] << 21) +
                        (((uint32_t)inbuf[1] & 0xE0) << 13) +
                        (((uint32_t)inbuf[1] & 0x03) << 16) +
                        ((uint32_t)inbuf[2] << 8) +
                        inbuf[3];
        frame->can_id |= CAN_EFF_FLAG;
    }

    /* standard id */
    else {
        frame->can_id = ((uint32_t)inbuf[0] << 3) +
                        (((uint32_t)inbuf[1] & 0xE0) >> 5);
    }

    frame->can_dlc = inbuf[4];
    memcpy(frame->data, inbuf + 5, frame->can_dlc);
    return mailbox;
}

int mcp2515_abort(candev_mcp2515_t *dev, int mailbox)
{
    return mcp2515_spi_bitmod(dev, MCP2515_TX_CTRL(mailbox), MCP2515_TXBCTRL_TXREQ, 0);
}

enum mcp2515_mode mcp2515_get_mode(candev_mcp2515_t *dev)
{
    enum mcp2515_mode result = MODE_UNKNOWN;
    uint8_t mode;

    int res = mcp2515_spi_read(dev, MCP2515_CANSTAT, &mode, 1);

    if (res == 0) {
        result = (enum mcp2515_mode)mode & MCP2515_CANSTAT_OPMOD_MASK;
    }

    DEBUG("mcp2515_get_mode: mode=%x\n", result);

    return result;
}

enum mcp2515_mode mcp2515_set_mode(candev_mcp2515_t *dev,
                                   enum mcp2515_mode mode)
{
    DEBUG("mcp2515_set_mode: mode=%x\n", mode);

    if (mode == MODE_UNKNOWN) {
        return -1;
    }

    enum mcp2515_mode cur_mode = mcp2515_get_mode(dev);

    if (cur_mode == mode) {
        return mode;
    }
    if (cur_mode == MODE_SLEEP) {
        mcp2515_wake_up(dev);
    }

    mcp2515_spi_bitmod(dev, MCP2515_CANCTRL, MCP2515_CANCTRL_REQOP_MASK,
                       (uint8_t)mode);
    int cnt = 0;

    /* allow for two retries to set the mode */
    while (cur_mode != mode && cnt++ < 2) {
        cur_mode = mcp2515_get_mode(dev);
    }
    return cur_mode;
}

void mcp2515_wake_up(candev_mcp2515_t *dev)
{
    dev->wakeup_src = MCP2515_WKUP_SRC_INTERNAL;
    mcp2515_spi_bitmod(dev, MCP2515_CANINTF, MCP2515_CANINTF_WAKIF,
                       MCP2515_CANINTF_WAKIF);
    xtimer_usleep(_osc_startup(dev));

    uint8_t flag = mcp2515_get_irq(dev);

    if (flag & INT_WAKEUP) {
        DEBUG("wakeup, irq raised\n");
        dev->wakeup_src = 0;
        mcp2515_clear_irq(dev, INT_WAKEUP);
    }
}

static int _mcp2515_enable_irq(candev_mcp2515_t *dev, uint8_t irq)
{
    return mcp2515_spi_bitmod(dev, MCP2515_CANINTE, irq, irq);
}

int mcp2515_configure_bittiming(candev_mcp2515_t *dev)
{
    struct can_bittiming *tim = &(dev->candev.bittiming);
    int res = 0;
    uint8_t c;
    enum mcp2515_mode mode;

    DEBUG("mcp2515_configure_bittiming: brp=%" PRIu32 ", prop_seg=%" PRIu32
          ", phase_seg1=%" PRIu32 ", phase_seg2=%" PRIu32 "\n", tim->brp,
          tim->prop_seg,
          tim->phase_seg1, tim->phase_seg2);

    mode = mcp2515_get_mode(dev);
    if (mode != MODE_CONFIG) {
        if (mcp2515_set_mode(dev, MODE_CONFIG) != MODE_CONFIG) {
            return -1;
        }
    }

    /* set Synchronization Jump Width Length */
    c = ((tim->brp - 1) & 0x3F) | ((tim->sjw - 1) << 6);
    mcp2515_spi_write(dev, MCP2515_CNF1, &c, 1);

    mcp2515_spi_bitmod(dev, MCP2515_CNF2, MCP2515_CNF2_PRSEG_MASK |
                       MCP2515_CNF2_PHSEG_MASK | MCP2515_CNF2_BTLMODE,
                       MCP2515_CNF2_BTLMODE | (tim->prop_seg - 1) |
                       ((tim->phase_seg1 - 1) << 3));
    mcp2515_spi_bitmod(dev, MCP2515_CNF3,
                       MCP2515_CNF3_PHSEG_MASK | MCP2515_CNF3_WAKFIL,
                       (tim->phase_seg2 - 1) | MCP2515_CNF3_WAKFIL);

    if (mode != MODE_CONFIG) {
        mcp2515_set_mode(dev, mode);
    }

    return res;
}

int mcp2515_init_irqs(candev_mcp2515_t *dev)
{
    return _mcp2515_enable_irq(dev,
                              MCP2515_CANINTE_RX0IE |
                              MCP2515_CANINTE_RX1IE |
                              MCP2515_CANINTE_ERRIE |
                              MCP2515_CANINTE_WAKIE);
}

enum mcp2515_interrupt mcp2515_get_irq(candev_mcp2515_t *dev)
{
    uint8_t flag;

    mcp2515_spi_read(dev, MCP2515_CANINTF, &flag, 1);
    return (enum mcp2515_interrupt)flag;
}

int mcp2515_clear_irq(candev_mcp2515_t *dev, enum mcp2515_interrupt irq)
{
    if (!irq) { /* no irq's to be cleared */
        return 0;
    }
    else {
        return mcp2515_spi_bitmod(dev, MCP2515_CANINTF, irq, 0);
    }
}

int mcp2515_tx_err_occurred(candev_mcp2515_t *dev, int mailbox)
{
    uint8_t ctrl_reg;

    mcp2515_spi_read(dev, MCP2515_TX_CTRL(mailbox), &ctrl_reg, 1);
    if (ctrl_reg & MCP2515_TXBCTRL_TXERR) {
        return 1;
    }
    else {
        return 0;
    }
}

uint8_t mcp2515_get_errors(candev_mcp2515_t *dev)
{
    uint8_t eflg;

    mcp2515_spi_read(dev, MCP2515_EFLG, &eflg, 1);
    return eflg;
}

int mcp2515_set_filter(candev_mcp2515_t *dev, int filter_id, uint32_t filter)
{
    uint8_t buf[4];
    uint8_t reg;

    if ((filter & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        _fill_extended_id(filter, buf);
    }
    else {
        _fill_standard_id(filter, buf);
    }
    if (filter_id < 3) {
        reg = MCP2515_RXF0SIDH + (filter_id << 2);
    }
    else {
        reg = MCP2515_RXF3SIDH + ((filter_id - 3) << 2);
    }
    return mcp2515_spi_write(dev, reg, buf, sizeof(buf));
}

int mcp2515_set_mask(candev_mcp2515_t *dev, int mailbox, uint32_t mask)
{
    uint8_t buf[4];

    if ((mask & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        _fill_extended_id(mask, buf);
    }
    else {
        _fill_standard_id(mask, buf);
    }
    return mcp2515_spi_write(dev, MCP2515_RXM0SIDH + (mailbox << 2), buf, 4);
}
