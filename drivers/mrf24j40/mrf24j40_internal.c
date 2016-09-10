/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */
#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"

void mrf24j40_init(mrf24j40_t *dev)
{
    uint8_t softrst;
    uint8_t order;
    uint8_t rfstate;

    gpio_clear(dev->params.reset_pin);
    xtimer_usleep(500);         /* Datasheet - Not specified */
    gpio_set(dev->params.reset_pin);
    xtimer_usleep(2000);        /* Datenblatt MRF24J40 ~2ms */

    /* do a soft reset */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, 0x7); // from manual
    do {
        softrst = mrf24j40_reg_read_short(dev, MRF24J40_REG_SOFTRST);
    } while ((softrst & 0x07) != 0);        /* wait until soft-reset has finished */

    /* Check if MRF24J40 is available */
    order = mrf24j40_reg_read_short(dev, MRF24J40_REG_ORDER);
    if (order != 0xFF) {
    }

    /* Here starts init-process as described on MRF24J40 Manual Chap. 3.2 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PACON2, 0x98);           /* Initialize FIFOEN = 1 and TXONTS = 0x6 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXSTBL, 0x95);           /* Initialize RFSTBL = 0x9. */
    /* set default channel */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON1, 0x01);            /* VCO optimization */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON2, 0x80);            /* Bit 7 = PLL Enable */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON6, 0x90);            /* Filter control / clk recovery ctrl. / Battery monitor */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON7, 0x80);            /* Sleep clock selection -> int. 100kHz clk */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON8, 0x10);            /* VCO control */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON1, 0x21);           /* CLKOUT pin enable  / sleep clk divider */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG2, 0x80);           /* CCA Mode 1: energy above threshold / clear CCA bits */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, 0x60);          /* Energy detection threshold -69dBm */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, 0x40);           /* Append RSSI to RXFIFO */

    /* set interrupt pin polarity */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON0, 0x2);        /* IRQ-Pin -> rising edge */
    /* mrf24j40_set_interrupts */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0xf6);       /* enables Transmit- and Receive-IRQ */
    /* set default channel */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON0, 0xf3);        /* Default channel = 26 */
    /* set default TX power - Reset default = Max -> nothing to do */
//	mrf24j40_set_txpower(dev, MRF24J40_DEFAULT_TXPOWER);
    /* reset RF state machine */
    mrf24j40_reset_state_machine(dev);

    xtimer_usleep(200);
    //Delay for 192 us after RF State Machine Reset

    do {
        rfstate = mrf24j40_reg_read_long(dev, MRF24J40_REG_RFSTATE);
    } while ((rfstate & 0xA0) != 0xa0);
    //Wait until the RFSTATE machine indicates RX state
}

void mrf24j40_reg_write_short(mrf24j40_t *dev, const uint8_t addr, const uint8_t value)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi, MRF24J40_SHORT_ADDR_TRANS | (addr << MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_WRITE, value, 0);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

uint8_t mrf24j40_reg_read_short(mrf24j40_t *dev, const uint8_t addr)
{
    char value;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_reg(dev->params.spi, MRF24J40_SHORT_ADDR_TRANS | (addr << MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_READ, 0, &value);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);

    return (uint8_t)value;
}


void mrf24j40_reg_write_long(mrf24j40_t *dev, const uint16_t addr, const uint8_t value)
{
    uint8_t reg1, reg2;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (addr >> 3);
    reg2 = (addr << 5) | MRF24J40_ACCESS_WRITE_LNG;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, 0);
    spi_transfer_byte(dev->params.spi, reg2, 0);
    spi_transfer_byte(dev->params.spi, value, 0);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}


uint8_t mrf24j40_reg_read_long(mrf24j40_t *dev, const uint16_t addr)
{
    uint8_t reg1, reg2;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (addr >> 3);
    reg2 = (addr << 5) | MRF24J40_ACCESS_READ;
    char value;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, 0);
    spi_transfer_byte(dev->params.spi, reg2, &value);
    spi_transfer_byte(dev->params.spi, 0, &value);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);

    return (uint8_t)value;
}

void mrf24j40_tx_normal_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len)
{
    uint8_t reg1, reg2;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (offset >> 3);
    reg2 = (offset << 5) | MRF24J40_ACCESS_READ;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, NULL);
    spi_transfer_byte(dev->params.spi, reg2, (char *)data);
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len - 1);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}


void mrf24j40_tx_normal_fifo_write(mrf24j40_t *dev,
                                   const uint16_t offset,
                                   const uint8_t *data,
                                   const size_t len)
{
    uint16_t addr;
    uint8_t reg1;
    uint8_t reg2;

    addr = offset;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (addr >> 3);
    reg2 = (addr << 5) | MRF24J40_ACCESS_WRITE_LNG;

    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, 0);
    spi_transfer_byte(dev->params.spi, reg2, 0);
    spi_transfer_bytes(dev->params.spi, (char *)data, NULL, len);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void mrf24j40_rx_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len)
{
    uint16_t rx_addr;

    rx_addr = MRF24J40_RX_FIFO + offset;

    uint8_t reg1, reg2;
    reg1 = MRF24J40_LONG_ADDR_TRANS | (rx_addr >> 3);
    reg2 = (rx_addr << 5) | MRF24J40_ACCESS_READ;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, NULL);
    spi_transfer_byte(dev->params.spi, reg2, (char *)data);
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len - 1);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void mrf24j40_rx_fifo_write(mrf24j40_t *dev, const uint16_t offset, const uint8_t *data, const size_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++) {
        mrf24j40_reg_write_long(dev, i, data[i]);
    }
}

uint8_t mrf24j40_get_status(mrf24j40_t *dev)
{
    uint8_t rfstate;
    uint8_t rxmcr;
    uint8_t txstat;
    uint8_t txncon;

    /*
     *  0xe0 = RTSEL2
        0xc0 = RTSEL1
        0xa0 = RX
        0x80 = TX
        0x60 = CALVCO
        0x40 = SLEEP
        0x20 = CALFIL
        0x00 = RESET
     */

    rfstate = mrf24j40_reg_read_long(dev, MRF24J40_REG_RFSTATE);
    rxmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
    txstat = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT);
    txncon = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXNCON);

    if (rfstate == MRF24J40_RFSTATE_SLEEP) {
        return MRF24J40_PSEUDO_STATE_SLEEP;
    }
    else if ((rfstate == MRF24J40_RFSTATE_RX) && ((txncon & MRF24J40_TXNCON_MASK__TXNACKREQ))) {
        return MRF24J40_PSEUDO_STATE_TX_ARET_ON;
    }
    else if ((rfstate == MRF24J40_RFSTATE_RX) && (rxmcr & MRF24J40_RXMCR_MASK__NOACKRSP)) {
        return MRF24J40_PSEUDO_STATE_RX_ON;
    }
    else if ((rfstate == MRF24J40_RFSTATE_RX) && (!(rxmcr & MRF24J40_RXMCR_MASK__NOACKRSP))) {
        return MRF24J40_PSEUDO_STATE_RX_AACK_ON;
    }
    else if ((rfstate == MRF24J40_RFSTATE_RX) && (txstat & MRF24J40_TXSTAT_MASK__TXNSTAT)) {
        return MRF24J40_PSEUDO_STATE_BUSY_TX_ARET;
    }
    else {
        return MRF24J40_PSEUDO_STATE_SLEEP;
    }
}

void mrf24j40_assert_awake(mrf24j40_t *dev)
{
    if (mrf24j40_get_status(dev) == MRF24J40_PSEUDO_STATE_SLEEP) {

        /* set uController pin to high */
        gpio_set(dev->params.sleep_pin);

        /* reset state machine */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x04);    /* RF-State Machine Reset */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);

        /* After wake-up, delay at least 2 ms to allow 20 MHz main
         * oscillator time to stabilize before transmitting or receiving.
         */
        xtimer_usleep(MRF24J40_WAKEUP_DELAY);

        /* enable again IRQs */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0b11110110);     /* enables Transmit- and Receive-IRQ */
    }
}


void mrf24j40_hardware_reset(mrf24j40_t *dev)
{
    /* wake up from sleep in case radio is sleeping */
    mrf24j40_assert_awake(dev);

    /* trigger hardware reset */
    gpio_clear(dev->params.reset_pin);
    xtimer_usleep(MRF24J40_RESET_PULSE_WIDTH);  /* Datasheet - Not specified */
    gpio_set(dev->params.reset_pin);
    xtimer_usleep(MRF24J40_RESET_DELAY);        /* Datenblatt MRF24J40 ~2ms */

}

void mrf24j40_configure_phy(mrf24j40_t *dev)
{
    mrf24j40_set_chan(dev, dev->netdev.chan);
}

