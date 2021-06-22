/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx126x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the SX1261/2/8 and LLCC68 LoRa radio driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <errno.h>

#include "sx126x_netdev.h"

#include "net/lora.h"
#include "periph/spi.h"

#include "sx126x_driver.h"
#include "sx126x.h"
#include "sx126x_params.h"
#include "sx126x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_SX126X_PKT_TYPE_DEFAULT
#define CONFIG_SX126X_PKT_TYPE_DEFAULT          (SX126X_PKT_TYPE_LORA)
#endif

#ifndef CONFIG_SX126X_CHANNEL_DEFAULT
#define CONFIG_SX126X_CHANNEL_DEFAULT           (868300000UL)   /* in Hz */
#endif

#ifndef CONFIG_SX126X_TX_POWER_DEFAULT
#define CONFIG_SX126X_TX_POWER_DEFAULT          (14U)           /* in dBm */
#endif

#ifndef CONFIG_SX126X_RAMP_TIME_DEFAULT
#define CONFIG_SX126X_RAMP_TIME_DEFAULT         (SX126X_RAMP_10_US)
#endif

const sx126x_pa_cfg_params_t sx1262_pa_cfg = {
    .pa_duty_cycle = 0x02,
    .hp_max = 0x02,
    .device_sel = 0x00,
    .pa_lut = 0x01
};

const sx126x_pa_cfg_params_t sx1268_pa_cfg = {
    .pa_duty_cycle = 0x04,
    .hp_max = 0x06,
    .device_sel = 0x00,
    .pa_lut = 0x01
};

const sx126x_pa_cfg_params_t sx1261_pa_cfg = {
    .pa_duty_cycle = 0x04,
    .hp_max = 0x00,
    .device_sel = 0x01,
    .pa_lut = 0x01
};

void sx126x_setup(sx126x_t *dev, const sx126x_params_t *params, uint8_t index)
{
    netdev_t *netdev = &dev->netdev;

    netdev->driver = &sx126x_driver;
    dev->params = (sx126x_params_t *)params;
    netdev_register(&dev->netdev, NETDEV_SX126X, index);
}

static const uint16_t _bw_khz[3] = {
    [LORA_BW_125_KHZ] = 125,
    [LORA_BW_250_KHZ] = 250,
    [LORA_BW_500_KHZ] = 500,
};

static uint8_t _compute_ldro(sx126x_t *dev)
{
    uint32_t symbol_len = (uint32_t)(1 << dev->mod_params.sf) / _bw_khz[dev->mod_params.bw - SX126X_LORA_BW_125];
    if (symbol_len >= 16) {
        return 0x01;
    }

    return 0x00;
}

static void sx126x_init_default_config(sx126x_t *dev)
{
    /* packet type must be set first */
    sx126x_set_pkt_type(dev, SX126X_PKT_TYPE_LORA);
    sx126x_set_channel(dev, CONFIG_SX126X_CHANNEL_DEFAULT);

    /* Configure PA optimal settings for maximum output power
     * Values used here comes from the datasheet, section 13.1.14 SetPaConfig
     * and are optimal for a TX output power of 14dBm.
     */
    if (sx126x_is_llcc68(dev) || sx126x_is_sx1262(dev)) {
        sx126x_set_pa_cfg(dev, &sx1262_pa_cfg);
    }
    else if (sx126x_is_sx1268(dev)) {
        sx126x_set_pa_cfg(dev, &sx1268_pa_cfg);
    }
    else { /* sx126x_is_sx1261(dev) */
        sx126x_set_pa_cfg(dev, &sx1261_pa_cfg);
    }
    sx126x_set_tx_params(dev, CONFIG_SX126X_TX_POWER_DEFAULT, CONFIG_SX126X_RAMP_TIME_DEFAULT);

    dev->mod_params.bw = (sx126x_lora_bw_t)(CONFIG_LORA_BW_DEFAULT + SX126X_LORA_BW_125);
    dev->mod_params.sf = (sx126x_lora_sf_t)CONFIG_LORA_SF_DEFAULT;
    dev->mod_params.cr = (sx126x_lora_cr_t)CONFIG_LORA_CR_DEFAULT;
    dev->mod_params.ldro = _compute_ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);

    dev->pkt_params.pld_len_in_bytes = 0;
    dev->pkt_params.crc_is_on = LORA_PAYLOAD_CRC_ON_DEFAULT;
    dev->pkt_params.header_type = (
        IS_ACTIVE(CONFIG_LORA_FIXED_HEADER_LEN_MODE_DEFAULT) ? true : false
        );
    dev->pkt_params.preamble_len_in_symb = CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT;
    dev->pkt_params.invert_iq_is_on = (
        IS_ACTIVE(CONFIG_LORA_IQ_INVERTED_DEFAULT) ? true : false
        );
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

static void _dio1_isr(void *arg)
{
    netdev_trigger_event_isr(arg);
}

int sx126x_init(sx126x_t *dev)
{
    /* Setup SPI for SX126X */
    int res = spi_init_cs(dev->params->spi, dev->params->nss_pin);

    if (res != SPI_OK) {
        DEBUG("[sx126x] error: failed to initialize SPI_%i device (code %i)\n",
              dev->params->spi, res);
        return -1;
    }

    DEBUG("[sx126x] init: SPI_%i initialized with success\n", dev->params->spi);

    gpio_init(dev->params->reset_pin, GPIO_OUT);
    gpio_init(dev->params->busy_pin, GPIO_IN_PD);

    /* Initialize DIOs */
    if (gpio_is_valid(dev->params->dio1_pin)) {
        res = gpio_init_int(dev->params->dio1_pin, GPIO_IN, GPIO_RISING, _dio1_isr, dev);
        if (res < 0) {
            DEBUG("[sx126x] error: failed to initialize DIO1 pin\n");
            return res;
        }
    }
    else {
        DEBUG("[sx126x] error: no DIO1 pin defined\n");
        return -EIO;
    }

    /* Reset the device */
    sx126x_reset(dev);

    /* Configure the power regulator mode */
    sx126x_set_reg_mode(dev, dev->params->regulator);

    /* Initialize radio with the default parameters */
    sx126x_init_default_config(dev);

    /* Configure available IRQs */
    const uint16_t irq_mask = (
        SX126X_IRQ_TX_DONE |
        SX126X_IRQ_RX_DONE |
        SX126X_IRQ_PREAMBLE_DETECTED |
        SX126X_IRQ_HEADER_VALID |
        SX126X_IRQ_HEADER_ERROR |
        SX126X_IRQ_CRC_ERROR |
        SX126X_IRQ_CAD_DONE |
        SX126X_IRQ_CAD_DETECTED |
        SX126X_IRQ_TIMEOUT
        );

    sx126x_set_dio_irq_params(dev, irq_mask, irq_mask, 0, 0);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        sx126x_pkt_type_t pkt_type;
        sx126x_get_pkt_type(dev, &pkt_type);
        DEBUG("[sx126x] init radio: pkt type: %d\n", pkt_type);

        sx126x_chip_status_t radio_status;
        sx126x_get_status(dev, &radio_status);
        DEBUG("[sx126x] init: chip mode %d\n", radio_status.chip_mode);
        DEBUG("[sx126x] init: cmd status %d\n", radio_status.cmd_status);
    }

    /* Radio Rx timeout timer stopped on preamble detection */
    sx126x_stop_timer_on_preamble(dev, true);

    return res;
}

uint32_t sx126x_get_channel(const sx126x_t *dev)
{
    return dev->channel;
}

void sx126x_set_channel(sx126x_t *dev, uint32_t freq)
{
    dev->channel = freq;
    sx126x_set_rf_freq(dev, dev->channel);
}

uint8_t sx126x_get_bandwidth(const sx126x_t *dev)
{
    return dev->mod_params.bw - SX126X_LORA_BW_125;
}

void sx126x_set_bandwidth(sx126x_t *dev, uint8_t bandwidth)
{
    dev->mod_params.bw = bandwidth + SX126X_LORA_BW_125;
    dev->mod_params.ldro = _compute_ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_spreading_factor(const sx126x_t *dev)
{
    return dev->mod_params.sf;
}

void sx126x_set_spreading_factor(sx126x_t *dev, uint8_t sf)
{
    dev->mod_params.sf = (sx126x_lora_sf_t)sf;
    dev->mod_params.ldro = _compute_ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_coding_rate(const sx126x_t *dev)
{
    return dev->mod_params.cr;
}

void sx126x_set_coding_rate(sx126x_t *dev, uint8_t cr)
{
    dev->mod_params.cr = (sx126x_lora_cr_t)cr;
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_lora_payload_length(const sx126x_t *dev)
{
    sx126x_rx_buffer_status_t rx_buffer_status;

    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes;
}

void sx126x_set_lora_payload_length(sx126x_t *dev, uint8_t len)
{
    dev->pkt_params.pld_len_in_bytes = len;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_crc(const sx126x_t *dev)
{
    return dev->pkt_params.crc_is_on;
}

void sx126x_set_lora_crc(sx126x_t *dev, bool crc)
{
    dev->pkt_params.crc_is_on = crc;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_implicit_header(const sx126x_t *dev)
{
    return dev->pkt_params.header_type == SX126X_LORA_PKT_IMPLICIT;
}

void sx126x_set_lora_implicit_header(sx126x_t *dev, bool mode)
{
    dev->pkt_params.header_type = (mode ? SX126X_LORA_PKT_IMPLICIT : SX126X_LORA_PKT_EXPLICIT);
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

uint16_t sx126x_get_lora_preamble_length(const sx126x_t *dev)
{
    return dev->pkt_params.preamble_len_in_symb;
}

void sx126x_set_lora_preamble_length(sx126x_t *dev, uint16_t preamble)
{
    dev->pkt_params.preamble_len_in_symb = preamble;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_iq_invert(const sx126x_t *dev)
{
    return dev->pkt_params.invert_iq_is_on;
}

void sx126x_set_lora_iq_invert(sx126x_t *dev, bool iq_invert)
{
    dev->pkt_params.invert_iq_is_on = iq_invert;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}
