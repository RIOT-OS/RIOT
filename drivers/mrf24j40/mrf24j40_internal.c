/*
 * SPDX-FileCopyrightText: 2017 Neo Nenaco <neo@nenaco.de>
 * SPDX-FileCopyrightText: 2017 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Neo Nenaco <neo@nenaco.de>
 *
 * @}
 */
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SPIDEV          (dev->params->spi)
#define CSPIN           (dev->params->cs_pin)

static inline void getbus(mrf24j40_t *dev)
{
    spi_acquire(SPIDEV, CSPIN, SPI_MODE_0, dev->params->spi_clk);
}

#if IS_ACTIVE(CONFIG_MRF24J40_USE_EXT_PA_LNA)
static inline void mrf24j40_reg_and_short(mrf24j40_t *dev, const uint8_t addr, uint8_t value)
{
    value &= mrf24j40_reg_read_short(dev, addr);
    mrf24j40_reg_write_short(dev, addr, value);
}

static inline void mrf24j40_reg_or_short(mrf24j40_t *dev, const uint8_t addr, uint8_t value)
{
    value |= mrf24j40_reg_read_short(dev, addr);
    mrf24j40_reg_write_short(dev, addr, value);
}

void mrf24j40_enable_auto_pa_lna(mrf24j40_t *dev)
{
    /* Configure enable pin of the Voltage Regulator for the PA (GPIO3) on MRF24J40MC */
    mrf24j40_reg_or_short(dev, MRF24J40_REG_TRISGPIO, MRF24J40_GPIO_3);

    /* Enable the volate regulator to power the Power Amplifier */
    mrf24j40_reg_or_short(dev, MRF24J40_REG_GPIO, MRF24J40_GPIO_3);

    mrf24j40_reg_write_long(dev, MRF24J40_REG_TESTMODE, (MRF24J40_TESTMODE_RSSIWAIT0 |
                                                         MRF24J40_TESTMODE_TESTMODE2 |
                                                         MRF24J40_TESTMODE_TESTMODE1 |
                                                         MRF24J40_TESTMODE_TESTMODE0));
}

void mrf24j40_disable_auto_pa_lna(mrf24j40_t *dev)
{
    /* Disable automatic switch on PA/LNA */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_TESTMODE, MRF24J40_TESTMODE_RSSIWAIT0);

    /* Configure all GPIOs as Output */
    mrf24j40_reg_or_short(dev, MRF24J40_REG_TRISGPIO, (MRF24J40_GPIO_0 |
                                                       MRF24J40_GPIO_1 |
                                                       MRF24J40_GPIO_2 |
                                                       MRF24J40_GPIO_3));

    /* Disable all GPIO outputs */
    mrf24j40_reg_and_short(dev, MRF24J40_REG_GPIO, ~(MRF24J40_GPIO_0 |
                                                     MRF24J40_GPIO_1 |
                                                     MRF24J40_GPIO_2 |
                                                     MRF24J40_GPIO_3));
}

void mrf24j40_enable_lna(mrf24j40_t *dev)
{
    /* Disable automatic switch on PA/LNA */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_TESTMODE, MRF24J40_TESTMODE_RSSIWAIT0);

    /* Configure all GPIOs as Output */
    mrf24j40_reg_or_short(dev, MRF24J40_REG_TRISGPIO, (MRF24J40_GPIO_0 |
                                                       MRF24J40_GPIO_1 |
                                                       MRF24J40_GPIO_2 |
                                                       MRF24J40_GPIO_3));

    /* Enable LNA, keep PA voltage regulator on */
    mrf24j40_reg_and_short(dev, MRF24J40_REG_GPIO, ~(MRF24J40_GPIO_0 | MRF24J40_GPIO_1));
    mrf24j40_reg_or_short(dev, MRF24J40_REG_GPIO, MRF24J40_GPIO_2 | MRF24J40_GPIO_3);
}
#endif /* CONFIG_MRF24J40_USE_EXT_PA_LNA */

int mrf24j40_init_hw(mrf24j40_t *dev)
{
    if (IS_ACTIVE(CONFIG_MRF24J40_TEST_SPI_CONNECTION)) {
        /* Check if MRF24J40 is available */
        uint8_t txmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
        if ((txmcr == 0xFF) || (txmcr == 0x00)) {
            /* Write default value to TXMCR register */
            mrf24j40_reg_write_short(dev, MRF24J40_REG_TXMCR, MRF24J40_TXMCR_MACMINBE1 |
                                                            MRF24J40_TXMCR_MACMINBE0 |
                                                            MRF24J40_TXMCR_CSMABF2);
            txmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXMCR);
            if (txmcr != (MRF24J40_TXMCR_MACMINBE1 |
                        MRF24J40_TXMCR_MACMINBE0 |
                        MRF24J40_TXMCR_CSMABF2)) {
                DEBUG("[mrf24j40] Initialization failure, SPI interface communication failed\n");
                /* Return to prevents hangup later in the initialization */
                return -ENODEV;
            }
        }
    }

    mrf24j40_hardware_reset(dev);

    /* do a soft reset */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_SOFTRST, MRF24J40_SOFTRST_RSTPWR |
                                                        MRF24J40_SOFTRST_RSTBB  |
                                                        MRF24J40_SOFTRST_RSTMAC );

    /* flush RX FIFO */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXFLUSH, MRF24J40_RXFLUSH_RXFLUSH);

    /* Here starts init-process as described on MRF24J40 Manual Chap. 3.2 */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_PACON2,  (MRF24J40_PACON2_TXONTS2 |
                                                         MRF24J40_PACON2_TXONTS1 |
                                                         MRF24J40_PACON2_FIFOEN));
    mrf24j40_reg_write_short(dev, MRF24J40_REG_TXSTBL, (MRF24J40_TXSTBL_RFSTBL3 |
                                                        MRF24J40_TXSTBL_RFSTBL0 |
                                                        MRF24J40_TXSTBL_MSIFS2  |
                                                        MRF24J40_TXSTBL_MSIFS0));
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON1, MRF24J40_RFCON1_VCOOPT1);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON2, MRF24J40_RFCON2_PLLEN);
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON6, (MRF24J40_RFCON6_TXFIL |
                                                       MRF24J40_RFCON6_20MRECVR));
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON7, MRF24J40_RFCON7_SLPCLKSEL1 );
    mrf24j40_reg_write_long(dev, MRF24J40_REG_RFCON8, MRF24J40_RFCON8_RFVCO );
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON1, (MRF24J40_SLPCON1_CLKOUTEN |
                                                        MRF24J40_SLPCON1_SLPCLKDIV0));
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG2, MRF25J40_BBREG2_CCAMODE1 );
    mrf24j40_reg_write_short(dev, MRF24J40_REG_CCAEDTH, 0x60);
    mrf24j40_reg_write_short(dev, MRF24J40_REG_BBREG6, MRF24J40_BBREG6_RSSIMODE2 );

    mrf24j40_enable_auto_pa_lna(dev);

    /* Enable immediate sleep mode */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_WAKECON, MRF24J40_WAKECON_IMMWAKE);

    /* set interrupt pin polarity, rising edge */
    mrf24j40_reg_write_long(dev, MRF24J40_REG_SLPCON0, MRF24J40_SLPCON0_INTEDGE );
    /* reset RF state machine */
    mrf24j40_reset_state_machine(dev);

    /* clear interrupts */
    mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);

    /* mrf24j40_set_interrupts */
    mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, ~(MRF24J40_INTCON_RXIE | MRF24J40_INTCON_TXNIE));

    return 0;
}

uint8_t mrf24j40_reg_read_short(mrf24j40_t *dev, const uint8_t addr)
{
    char value;

    getbus(dev);
    value = spi_transfer_reg(SPIDEV, CSPIN, MRF24J40_SHORT_ADDR_TRANS |
                                            (addr << MRF24J40_ADDR_OFFSET) |
                                            MRF24J40_ACCESS_READ, 0);
    spi_release(SPIDEV);
    return (uint8_t)value;
}

void mrf24j40_reg_write_short(mrf24j40_t *dev, const uint8_t addr, const uint8_t value)
{
    getbus(dev);
    spi_transfer_reg(SPIDEV, CSPIN , MRF24J40_SHORT_ADDR_TRANS |
                                     (addr << MRF24J40_ADDR_OFFSET) |
                                     MRF24J40_ACCESS_WRITE, value);
    spi_release(SPIDEV);
}

uint8_t mrf24j40_reg_read_long(mrf24j40_t *dev, const uint16_t addr)
{
    uint8_t reg1, reg2;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (addr >> 3);
    reg2 = (addr << 5) | MRF24J40_ACCESS_READ;
    char value;
    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg1);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg2);
    value = spi_transfer_byte(SPIDEV, CSPIN, false, 0);
    spi_release(SPIDEV);

    return (uint8_t)value;
}

void mrf24j40_reg_write_long(mrf24j40_t *dev, const uint16_t addr, const uint8_t value)
{
    uint8_t reg1, reg2;

    reg1 = MRF24J40_LONG_ADDR_TRANS | (addr >> 3);
    reg2 = (addr << 5) | MRF24J40_ACCESS_WRITE_LNG;
    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg1);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg2);
    spi_transfer_byte(SPIDEV, CSPIN, false, value);
    spi_release(SPIDEV);
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

    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg1);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg2);
    spi_transfer_bytes(SPIDEV, CSPIN, false, (char *)data, NULL, len);
    spi_release(SPIDEV);
}

void mrf24j40_rx_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len)
{
    uint16_t rx_addr;

    rx_addr = MRF24J40_RX_FIFO + offset;

    uint8_t reg1, reg2;
    reg1 = MRF24J40_LONG_ADDR_TRANS | (rx_addr >> 3);
    reg2 = (rx_addr << 5) | MRF24J40_ACCESS_READ;
    getbus(dev);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg1);
    spi_transfer_byte(SPIDEV, CSPIN, true, reg2);
    spi_transfer_bytes(SPIDEV, CSPIN, false, NULL, (char *)data, len);
    spi_release(SPIDEV);
}

void mrf24j40_reset_tasks(mrf24j40_t *dev)
{
    dev->pending = MRF24J40_TASK_TX_DONE;
}

void mrf24j40_update_tasks(mrf24j40_t *dev)
{
    uint8_t newpending = 0;
    uint8_t instat = 0;

    instat = mrf24j40_reg_read_short(dev, MRF24J40_REG_INTSTAT);
    /* check if TX done */
    if (instat & MRF24J40_INTSTAT_TXNIF) {
        newpending |= MRF24J40_TASK_TX_DONE | MRF24J40_TASK_TX_READY;
        /* transmit done, returning to configured idle state */
    }
    if (instat & MRF24J40_INTSTAT_RXIF) {
        newpending |= MRF24J40_TASK_RX_READY;
    }
    /* check if RX pending */
    dev->pending |= newpending;
}

void mrf24j40_hardware_reset(mrf24j40_t *dev)
{
    /* trigger hardware reset */
    gpio_clear(dev->params->reset_pin);
    /* Datasheet - Not specified */
    ztimer_sleep(ZTIMER_USEC, MRF24J40_RESET_PULSE_WIDTH);
    gpio_set(dev->params->reset_pin);
    /* Datasheet - MRF24J40 ~2ms */
    ztimer_sleep(ZTIMER_USEC, MRF24J40_RESET_DELAY);
}

void mrf24j40_flush_rx(mrf24j40_t *dev)
{
    mrf24j40_reg_write_short(dev, MRF24J40_REG_RXFLUSH, MRF24J40_RXFLUSH_RXFLUSH);
}
