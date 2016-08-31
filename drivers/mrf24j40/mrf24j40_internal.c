/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 * @author      <neo@nenaco.de>
 *
 * @}
 */
#include "periph/spi.h"
#include "periph/gpio.h"
#include "xtimer.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"


void mrf24j40_reg_write_short(const mrf24j40_t *dev, const uint8_t addr, const uint8_t value)
{
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
	spi_transfer_reg(dev->params.spi, MRF24J40_SHORT_ADDR_TRANS | (addr<<MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_WRITE ,value, 0);
	gpio_set(dev->params.cs_pin);
	spi_release(dev->params.spi);
}

uint8_t mrf24j40_reg_read_short(const mrf24j40_t *dev, const uint8_t addr)
{
	char value;
	spi_acquire(dev->params.spi);
	gpio_clear(dev->params.cs_pin);
	spi_transfer_reg(dev->params.spi, MRF24J40_SHORT_ADDR_TRANS | (addr<<MRF24J40_ADDR_OFFSET) | MRF24J40_ACCESS_READ , 0, &value);
	gpio_set(dev->params.cs_pin);
	spi_release(dev->params.spi);

	return (uint8_t)value;
}


void mrf24j40_reg_write_long(const mrf24j40_t *dev, const uint16_t addr, const uint8_t value)
{
	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (addr>>3);
	reg2 = (addr<<5) | MRF24J40_ACCESS_WRITE_LNG;
	spi_acquire(dev->params.spi);
	gpio_clear(dev->params.cs_pin);
	spi_transfer_byte(dev->params.spi, reg1, 0);
	spi_transfer_byte(dev->params.spi, reg2, 0);
	spi_transfer_byte(dev->params.spi, value, 0);
	gpio_set(dev->params.cs_pin);
	spi_release(dev->params.spi);
}


uint8_t mrf24j40_reg_read_long(const mrf24j40_t *dev, const uint16_t addr)
{
	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (addr>>3);
	reg2 = (addr<<5) | MRF24J40_ACCESS_READ;
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

void mrf24j40_tx_normal_fifo_read(const mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len)
{
	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (offset>>3);
	reg2 = (offset<<5) | MRF24J40_ACCESS_READ;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, NULL);
	spi_transfer_byte(dev->params.spi, reg2, (char *)data);
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len-1);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void mrf24j40_tx_normal_fifo_write(const mrf24j40_t *dev,
                          const uint16_t offset,
                          const uint8_t *data,
                          const size_t len)
{
	uint16_t addr;
	uint8_t reg1;
	uint8_t reg2;

	addr = offset;

	reg1 = MRF24J40_LONG_ADDR_TRANS | (addr>>3);
	reg2 = (addr<<5) | MRF24J40_ACCESS_WRITE_LNG;

	spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, 0);
    spi_transfer_byte(dev->params.spi, reg2, 0);
    spi_transfer_bytes(dev->params.spi, (char *)data, NULL, len);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}

void mrf24j40_rx_fifo_read(const mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len)
{
	uint16_t rx_addr;

	/* rx-fifo is at address 0x300 */
	rx_addr = 0x300 + offset;

	uint8_t reg1,reg2;
	reg1 = MRF24J40_LONG_ADDR_TRANS | (rx_addr>>3);
	reg2 = (rx_addr<<5) | MRF24J40_ACCESS_READ;
    spi_acquire(dev->params.spi);
    gpio_clear(dev->params.cs_pin);
    spi_transfer_byte(dev->params.spi, reg1, NULL);
	spi_transfer_byte(dev->params.spi, reg2, (char *)data);
    spi_transfer_bytes(dev->params.spi, NULL, (char *)data, len-1);
    gpio_set(dev->params.cs_pin);
    spi_release(dev->params.spi);
}


void mrf24j40_rx_fifo_write(const mrf24j40_t *dev, const uint16_t offset, const uint8_t *data, const size_t len)
{
	uint16_t i;

	for (i = 0; i < len; i++)
	{
		mrf24j40_reg_write_long(dev, i, data[i]);
	}
}

uint8_t mrf24j40_get_status(const mrf24j40_t *dev)
{
	uint8_t rfstate;
	uint8_t rxmcr;
	uint8_t txstat;

	/* radio chip states:
	 * 	0xe0 = RTSEL2
		0xc0 = RTSEL1
		0xa0 = RX
		0x80 = TX
		0x60 = CALVCO
		0x40 = SLEEP
		0x20 = CALFIL
		0x00 = RESET

		not very exhausting, therefore introducing PSEUDO-STATES
	 */

	rfstate = mrf24j40_reg_read_long(dev, MRF24J40_REG_RFSTATE);
	rxmcr = mrf24j40_reg_read_short(dev, MRF24J40_REG_RXMCR);
	txstat = mrf24j40_reg_read_short(dev, MRF24J40_REG_TXSTAT);

	if ((rfstate == MRF24J40_RFSTATE_SLEEP) && (rxmcr & MRF24J40_RXMCR__NOACKRSP))
	{
		return MRF24J40_PSEUDO_STATE_SLEEP ;
	}

	else if ((rfstate == MRF24J40_RFSTATE_RX) && (rxmcr & MRF24J40_RXMCR__NOACKRSP))
	{
		return MRF24J40_PSEUDO_STATE_RX_AACK_OFF ;
	}
	else if ((rfstate == MRF24J40_RFSTATE_RX) && (txstat & MRF24J40_TXSTAT_MASK__TXNSTAT))
	{
		return MRF24J40_PSEUDO_STATE_BUSY_TX_ARET_OFF;
	}
	else
	{
		return MRF24J40_PSEUDO_STATE_RX_AACK_ON ;
	}
}


void mrf24j40_assert_awake(mrf24j40_t *dev)
{

    if(mrf24j40_get_status(dev) == MRF24J40_PSEUDO_STATE_SLEEP)
    {

    	/* set uController pin to high */
	/* waking up by using registers seems not to work. Use hardware solution (Sleep-Pin) */
        gpio_set(dev->params.sleep_pin);

        /* reset state machine */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x04);	/* RF-State Machine Reset */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_RFCTL, 0x00);

        /* After wake-up, delay at least 2 ms to allow 20 MHz main
         * oscillator time to stabilize before transmitting or receiving.
         */
        xtimer_usleep(MRF24J40_WAKEUP_DELAY);

        /* enable again IRQs (transmit and receive */
        mrf24j40_reg_write_short(dev, MRF24J40_REG_INTCON, 0b11110110);
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
    xtimer_usleep(MRF24J40_RESET_DELAY);      /* Datenblatt MRF24J40 ~2ms */

}

void mrf24j40_configure_phy(mrf24j40_t *dev)
{

//	mrf24j40_set_chan(dev, MRF24J40_DEFAULT_CHANNEL);		/* delay included in mrf24j40_set_chan */
	mrf24j40_set_chan(dev, dev->netdev.chan);
}



