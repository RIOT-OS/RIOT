/*
 * Copyright (C) 2016 Unwired Devices <info@unwds.com>
 *               2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Basic functionality of sx127x driver
 *
 * @author      Eugene P. <ep@unwds.com>
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "timex.h"
#include "ztimer.h"
#include "thread.h"
#include "kernel_defines.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "net/lora.h"

#include "sx127x.h"
#include "sx127x_internal.h"
#include "sx127x_registers.h"
#include "sx127x_netdev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* The reset signal must be applied for at least 100 µs to trigger the manual
   reset of the device. To ensure this value is big enough even with an
   inaccurate clock source, an additional 10 % error margin is added. */
#define SX127X_MANUAL_RESET_SIGNAL_LEN_US       (110U)

/* After triggering a manual reset the device needs at least 5 ms to become
   ready before interacting with it. To ensure this value is big enough even
   with an inaccurate clock source, an additional 10 % error margin is added. */
#define SX127X_MANUAL_RESET_WAIT_FOR_READY_US   (5500U)

/* When the device is started by enabling its power supply for the first time
   i.e. on Power-on-Reset (POR), it needs at least 10 ms after the POR cycle is
   done to become ready. To ensure this value is big enough even with an
   inaccurate clock source, an additional 10 % error margin is added. */
#define SX127X_POR_WAIT_FOR_READY_US            (11U * US_PER_MS)

/* Internal functions */
static int _init_spi(sx127x_t *dev);
static int _init_gpios(sx127x_t *dev);
static void _init_timers(sx127x_t *dev);
static void _on_tx_timeout(void *arg);
static void _on_rx_timeout(void *arg);

/* SX127X DIO interrupt handlers initialization */
static void sx127x_on_dio0_isr(void *arg);
static void sx127x_on_dio1_isr(void *arg);
static void sx127x_on_dio2_isr(void *arg);
static void sx127x_on_dio3_isr(void *arg);

void sx127x_setup(sx127x_t *dev, const sx127x_params_t *params, uint8_t index)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &sx127x_driver;
    dev->params = *params;
    netdev_register(&dev->netdev, NETDEV_SX127X, index);
}

int sx127x_reset(const sx127x_t *dev)
{
#ifdef SX127X_USE_TX_SWITCH
    /* tx switch as output, start in rx */
    gpio_init(dev->params.tx_switch_pin, GPIO_OUT);
    gpio_clear(dev->params.tx_switch_pin);
#endif
#ifdef SX127X_USE_RX_SWITCH
    /* rx switch as output, start in rx */
    gpio_init(dev->params.rx_switch_pin, GPIO_OUT);
    gpio_set(dev->params.rx_switch_pin);
#endif

    /*
     * This reset scheme complies with 7.2 chapter of the SX1272/1276 datasheet
     * See http://www.semtech.com/images/datasheet/sx1276.pdf for SX1276
     * See http://www.semtech.com/images/datasheet/sx1272.pdf for SX1272
     *
     * For SX1272:
     * 1. Set Reset pin to HIGH for at least 100 us
     *
     * For SX1276:
     * 1. Set NReset pin to LOW for at least 100 us
     *
     * For both:
     * 2. Set NReset in Hi-Z state
     * 3. Wait at least 5 milliseconds
     */
    if (gpio_is_valid(dev->params.reset_pin)) {
        gpio_init(dev->params.reset_pin, GPIO_OUT);

        /* set reset pin to the state that triggers manual reset */
        gpio_write(dev->params.reset_pin, SX127X_POR_ACTIVE_LOGIC_LEVEL);

        ztimer_sleep(ZTIMER_USEC, SX127X_MANUAL_RESET_SIGNAL_LEN_US);

        /* Put reset pin in High-Z */
        gpio_init(dev->params.reset_pin, GPIO_IN);

        ztimer_sleep(ZTIMER_USEC, SX127X_MANUAL_RESET_WAIT_FOR_READY_US);
    }

    return 0;
}

int sx127x_init(sx127x_t *dev)
{
    /* Do internal initialization routines */
    if (_init_spi(dev) < 0) {
        DEBUG("[sx127x] error: failed to initialize SPI\n");
        return -SX127X_ERR_SPI;
    }

    /* Check presence of SX127X */
    if (sx127x_check_version(dev) < 0) {
        DEBUG("[sx127x] error: no valid device found\n");
        return -SX127X_ERR_NODEV;
    }

    _init_timers(dev);

    if (gpio_is_valid(dev->params.reset_pin)) {
        /* reset pin should be left floating during POR */
        gpio_init(dev->params.reset_pin, GPIO_IN);

        /* wait till device signals end of POR cycle */
        while ((gpio_read(dev->params.reset_pin) > 0) ==
               SX127X_POR_ACTIVE_LOGIC_LEVEL) {}
    }

    /* wait for the device to become ready */
    ztimer_sleep(ZTIMER_USEC, SX127X_POR_WAIT_FOR_READY_US);

    sx127x_reset(dev);

#if defined(MODULE_SX1276)
    sx1276_rx_chain_calibration(dev);
#endif
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_SLEEP);

    if (_init_gpios(dev) < 0) {
        DEBUG("[sx127x] error: failed to initialize GPIOs\n");
        return -SX127X_ERR_GPIOS;
    }

    return SX127X_INIT_OK;
}

void sx127x_init_radio_settings(sx127x_t *dev)
{
    DEBUG("[sx127x] initializing radio settings\n");
    sx127x_set_channel(dev, SX127X_CHANNEL_DEFAULT);
    sx127x_set_modem(dev, SX127X_MODEM_DEFAULT);
    sx127x_set_tx_power(dev, SX127X_RADIO_TX_POWER);
    sx127x_set_bandwidth(dev, CONFIG_LORA_BW_DEFAULT);
    sx127x_set_spreading_factor(dev, CONFIG_LORA_SF_DEFAULT);
    sx127x_set_coding_rate(dev, CONFIG_LORA_CR_DEFAULT);
    sx127x_set_crc(dev, LORA_PAYLOAD_CRC_ON_DEFAULT);
    sx127x_set_freq_hop(dev, IS_ACTIVE(CONFIG_LORA_FREQUENCY_HOPPING_DEFAULT) ? true : false);
    sx127x_set_hop_period(dev, CONFIG_LORA_FREQUENCY_HOPPING_PERIOD_DEFAULT);
    sx127x_set_fixed_header_len_mode(dev, IS_ACTIVE(CONFIG_LORA_FIXED_HEADER_LEN_MODE_DEFAULT) ?
                                     true : false);
    sx127x_set_iq_invert(dev, IS_ACTIVE(CONFIG_LORA_IQ_INVERTED_DEFAULT) ? true : false);
    sx127x_set_payload_length(dev, CONFIG_LORA_PAYLOAD_LENGTH_DEFAULT);
    sx127x_set_preamble_length(dev, CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT);
    sx127x_set_symbol_timeout(dev, CONFIG_LORA_SYMBOL_TIMEOUT_DEFAULT);
    sx127x_set_rx_single(dev, SX127X_RX_SINGLE);
    sx127x_set_tx_timeout(dev, SX127X_TX_TIMEOUT_DEFAULT);
}

uint32_t sx127x_random(sx127x_t *dev)
{
    uint32_t rnd = 0;

    sx127x_set_modem(dev, SX127X_MODEM_LORA); /* Set LoRa modem ON */

    /* Disable LoRa modem interrupts */
    sx127x_reg_write(dev, SX127X_REG_LR_IRQFLAGSMASK, SX127X_RF_LORA_IRQFLAGS_RXTIMEOUT |
                     SX127X_RF_LORA_IRQFLAGS_RXDONE |
                     SX127X_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                     SX127X_RF_LORA_IRQFLAGS_VALIDHEADER |
                     SX127X_RF_LORA_IRQFLAGS_TXDONE |
                     SX127X_RF_LORA_IRQFLAGS_CADDONE |
                     SX127X_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                     SX127X_RF_LORA_IRQFLAGS_CADDETECTED);

    /* Set radio in continuous reception */
    sx127x_set_op_mode(dev, SX127X_RF_OPMODE_RECEIVER);

    for (unsigned i = 0; i < 32; i++) {
        ztimer_sleep(ZTIMER_MSEC, 1);   /* wait one millisecond */

        /* Non-filtered RSSI value reading. Only takes the LSB value */
        rnd |= ((uint32_t)sx127x_reg_read(dev, SX127X_REG_LR_RSSIWIDEBAND) & 0x01) << i;
    }

    sx127x_set_sleep(dev);

    return rnd;
}

/**
 * IRQ handlers
 */
void sx127x_isr(netdev_t *dev)
{
    netdev_trigger_event_isr(dev);
}

static void sx127x_on_dio_isr(sx127x_t *dev, sx127x_flags_t flag)
{
    dev->irq |= flag;
    sx127x_isr((netdev_t *)dev);
}

static void sx127x_on_dio0_isr(void *arg)
{
    sx127x_on_dio_isr((sx127x_t *)arg, SX127X_IRQ_DIO0);
}

static void sx127x_on_dio1_isr(void *arg)
{
    sx127x_on_dio_isr((sx127x_t *)arg, SX127X_IRQ_DIO1);
}

static void sx127x_on_dio2_isr(void *arg)
{
    sx127x_on_dio_isr((sx127x_t *)arg, SX127X_IRQ_DIO2);
}

static void sx127x_on_dio3_isr(void *arg)
{
    sx127x_on_dio_isr((sx127x_t *)arg, SX127X_IRQ_DIO3);
}

/* Internal event handlers */
static int _init_gpios(sx127x_t *dev)
{
    int res;

    /* Check if DIO0 pin is defined */
    if (gpio_is_valid(dev->params.dio0_pin)) {
        res = gpio_init_int(dev->params.dio0_pin, SX127X_DIO_PULL_MODE,
                            GPIO_RISING, sx127x_on_dio0_isr, dev);
        if (res < 0) {
            DEBUG("[sx127x] error: failed to initialize DIO0 pin\n");
            return res;
        }
    }
    else {
        DEBUG("[sx127x] error: no DIO0 pin defined\n");
        DEBUG("[sx127x] error: at least one interrupt should be defined\n");
        return SX127X_ERR_GPIOS;
    }

    /* Check if DIO1 pin is defined */
    if (gpio_is_valid(dev->params.dio1_pin)) {
        res = gpio_init_int(dev->params.dio1_pin, SX127X_DIO_PULL_MODE,
                            GPIO_RISING, sx127x_on_dio1_isr, dev);
        if (res < 0) {
            DEBUG("[sx127x] error: failed to initialize DIO1 pin\n");
            return res;
        }
    }

    /* check if DIO2 pin is defined */
    if (gpio_is_valid(dev->params.dio2_pin)) {
        res = gpio_init_int(dev->params.dio2_pin, SX127X_DIO_PULL_MODE,
                            GPIO_RISING, sx127x_on_dio2_isr, dev);
        if (res < 0) {
            DEBUG("[sx127x] error: failed to initialize DIO2 pin\n");
            return res;
        }
    }
    else {
        /* if frequency hopping is enabled, DIO2 pin must be defined */
        assert(!IS_ACTIVE(CONFIG_LORA_FREQUENCY_HOPPING_DEFAULT));
    }

    /* check if DIO3 pin is defined */
    if (gpio_is_valid(dev->params.dio3_pin)) {
        res = gpio_init_int(dev->params.dio3_pin, SX127X_DIO_PULL_MODE,
                            GPIO_RISING, sx127x_on_dio3_isr, dev);
        if (res < 0) {
            DEBUG("[sx127x] error: failed to initialize DIO3 pin\n");
            return res;
        }
    }

    return res;
}

static void _on_tx_timeout(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;

    dev->event_callback(dev, NETDEV_EVENT_TX_TIMEOUT);
}

static void _on_rx_timeout(void *arg)
{
    netdev_t *dev = (netdev_t *)arg;

    dev->event_callback(dev, NETDEV_EVENT_RX_TIMEOUT);
}

static void _init_timers(sx127x_t *dev)
{
    dev->_internal.tx_timeout_timer.arg = dev;
    dev->_internal.tx_timeout_timer.callback = _on_tx_timeout;

    dev->_internal.rx_timeout_timer.arg = dev;
    dev->_internal.rx_timeout_timer.callback = _on_rx_timeout;
}

static int _init_spi(sx127x_t *dev)
{
    int res;

    /* Setup SPI for SX127X */
    res = spi_init_cs(dev->params.spi, dev->params.nss_pin);

#ifdef MODULE_PERIPH_SPI_GPIO_MODE
    spi_gpio_mode_t gpio_modes = {
        .mosi = (GPIO_OUT | SX127X_DIO_PULL_MODE),
        .miso = (SX127X_DIO_PULL_MODE),
        .sclk = (GPIO_OUT | SX127X_DIO_PULL_MODE),
    };
    res += spi_init_with_gpio_mode(dev->params.spi, gpio_modes);
#endif

    if (res != SPI_OK) {
        DEBUG("[sx127x] error: failed to initialize SPI_%i device (code %i)\n",
              dev->params.spi, res);
        return -1;
    }

    DEBUG("[sx127x] SPI_%i initialized with success\n", dev->params.spi);
    return 0;
}
