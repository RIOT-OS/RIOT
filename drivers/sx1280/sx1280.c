/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-FileCopyrightText: 2020-2022 Université Grenoble Alpes
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sx1280
 * @{
 * @file
 * @brief       Device driver implementation for the LoRa SX1280 Driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Aymeric Brochier <aymeric.brochier@univ-grenoble-alpes.fr>
 *
 * @}
 */

#include <string.h>
#include "ztimer.h"

#include "sx1280_driver/src/sx1280.h"
#include "sx1280_driver/src/sx1280_regs.h"
#include "smtc_ral/src/ral_sx1280.h"

#include "sx1280.h"
#include "sx1280_constants.h"
#include "sx1280_params.h"
#include "sx1280_netdev.h"

#include "net/lora.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static ral_lora_bw_t _lora_bw_to_ral_bw(int bw)
{
    switch (bw) {
    case LORA_BW_125_KHZ:
        return RAL_LORA_BW_125_KHZ;
    case LORA_BW_200_KHZ:
        return RAL_LORA_BW_200_KHZ;
    case LORA_BW_250_KHZ:
        return RAL_LORA_BW_250_KHZ;
    case LORA_BW_400_KHZ:
        return RAL_LORA_BW_400_KHZ;
    case LORA_BW_500_KHZ:
        return RAL_LORA_BW_500_KHZ;
    case LORA_BW_800_KHZ:
        return RAL_LORA_BW_800_KHZ;
    case LORA_BW_1600_KHZ:
        return RAL_LORA_BW_1600_KHZ;
    default:
        return -1;
    }
}

static ral_lora_sf_t _lora_sf_to_ral_sf(int sf)
{
    return (ral_lora_sf_t)sf;
}

static ral_lora_cr_t _lora_cr_to_ral_cr(int cr)
{
    switch (cr) {
    case LORA_CR_4_5:
        return RAL_LORA_CR_4_5;
    case LORA_CR_4_6:
        return RAL_LORA_CR_4_6;
    case LORA_CR_4_7:
        return RAL_LORA_CR_4_7;
    case LORA_CR_4_8:
        return RAL_LORA_CR_4_8;
    case LORA_CR_LI_4_5:
        return RAL_LORA_CR_LI_4_5;
    case LORA_CR_LI_4_6:
        return RAL_LORA_CR_LI_4_6;
    case LORA_CR_LI_4_8:
        return RAL_LORA_CR_LI_4_8;
    default:
        return -1;
    }
}

static int _ral_lora_cr_to_lora_cr(int cr)
{
    switch (cr) {
    case RAL_LORA_CR_4_5:
        return LORA_CR_4_5;
    case RAL_LORA_CR_4_6:
        return LORA_CR_4_6;
    case RAL_LORA_CR_4_7:
        return LORA_CR_4_7;
    case RAL_LORA_CR_4_8:
        return LORA_CR_4_8;
    case RAL_LORA_CR_LI_4_5:
        return LORA_CR_LI_4_5;
    case RAL_LORA_CR_LI_4_6:
        return LORA_CR_LI_4_6;
    case RAL_LORA_CR_LI_4_8:
        return LORA_CR_LI_4_8;
    default:
        return -1;
    }
}

static uint32_t _get_bw(ral_lora_bw_t lora_bw)
{
    switch (lora_bw) {
    case RAL_LORA_BW_007_KHZ:
        return (uint32_t)7000;
    case RAL_LORA_BW_010_KHZ:
        return (uint32_t)10000;
    case RAL_LORA_BW_015_KHZ:
        return (uint32_t)15000;
    case RAL_LORA_BW_020_KHZ:
        return (uint32_t)20000;
    case RAL_LORA_BW_031_KHZ:
        return (uint32_t)31000;
    case RAL_LORA_BW_041_KHZ:
        return (uint32_t)41000;
    case RAL_LORA_BW_062_KHZ:
        return (uint32_t)62000;
    case RAL_LORA_BW_125_KHZ:
        return (uint32_t)125000;
    case RAL_LORA_BW_200_KHZ:
        return (uint32_t)200000;
    case RAL_LORA_BW_250_KHZ:
        return (uint32_t)250000;
    case RAL_LORA_BW_400_KHZ:
        return (uint32_t)400000;
    case RAL_LORA_BW_500_KHZ:
        return (uint32_t)500000;
    case RAL_LORA_BW_800_KHZ:
        return (uint32_t)800000;
    case RAL_LORA_BW_1600_KHZ:
        return (uint32_t)1600000;
    default:
        return 0;
    }
}

static void _dio1_isr(void *arg)
{
    netdev_trigger_event_isr(arg);
}

static int enable_irq(const sx1280_t *dev)
{
    const uint16_t irq_mask = (
        SX1280_IRQ_TX_DONE |
        SX1280_IRQ_RX_DONE |
        SX1280_IRQ_PBL_DET |
        SX1280_IRQ_HEADER_VALID |
        SX1280_IRQ_HEADER_ERROR |
        SX1280_IRQ_CAD_DONE |
        SX1280_IRQ_CAD_DET |
        SX1280_IRQ_TIMEOUT
        );

    int res = sx1280_set_dio_irq_params(dev, irq_mask, irq_mask, SX1280_IRQ_NONE, SX1280_IRQ_NONE);

    return res;
}

void sx1280_setup(sx1280_t *dev, const sx1280_params_t *params, uint8_t index)
{
    dev->ral.context = dev;
    dev->ral.radio_type = RAL_RADIO_SX1280;
    dev->ral.tcxo_cfg.tcxo_ctrl_mode = RAL_TCXO_NONE;

    netdev_t *netdev = &dev->netdev;

    netdev->driver = &sx1280_driver;
    dev->params = (sx1280_params_t *)params;
    netdev_register(&dev->netdev, NETDEV_SX1280, index);
}

static uint32_t sx1280_init_default_config(sx1280_t *dev)

{
    /* default parameters */
    const ral_params_lora_t params_default = {
        .freq_in_hz = SX1280_CHANNEL_DEFAULT,
        .sf = _lora_sf_to_ral_sf(CONFIG_LORA_SF_DEFAULT),
        .bw = _lora_bw_to_ral_bw(CONFIG_LORA_BW_DEFAULT),
        .cr = _lora_cr_to_ral_cr(CONFIG_LORA_CR_DEFAULT),
        .pbl_len_in_symb = CONFIG_LORA_PREAMBLE_LENGTH_DEFAULT,
        .sync_word = LORA_SYNCWORD_ISM2400_PUBLIC,
        .pld_is_fix = false,
        .crc_is_on = true,
        .invert_iq_is_on = false,
        .symb_nb_timeout = 0,
        .pwr_in_dbm = SX1280_RADIO_TX_POWER,
    };

    /* set current ral_params */
    dev->ral_params_lora = params_default;

    ral_status_t res = ral_sx1280_setup_lora(&dev->ral, &params_default);

    if (res != RAL_STATUS_OK) {
        DEBUG("[sx1280] init: ERROR failed LoRa setup, %d\n", res);
        return -EIO;
    }

    return 0;
}

int sx1280_init(sx1280_t *dev)
{
    /* Setup SPI for SX1280 */
    int res = spi_init_cs(dev->params->spi, dev->params->nss_pin);

    if (res != SPI_OK) {
        DEBUG("[sx1280] error: failed to initialize SPI_%i device (code %i)\n",
              dev->params->spi, res);
        return -1;
    }

    DEBUG("[sx1280] init: SPI_%i initialized with success\n", dev->params->spi);
    /* Initialize Reset */
    if (gpio_init(dev->params->reset_pin, GPIO_OUT) < 0) {
        DEBUG("[sx1280] error: failed to initialize RESET pin\n");
        return -EIO;
    }
    /* Initialize DIO0s */
    if (gpio_init(dev->params->dio0_pin, GPIO_IN) < 0) {
        DEBUG("[sx1280] error: failed to initialize DIO1 pin\n");
        return -EIO;
    }
    /* Initialize DI01s */
    if (gpio_init_int(dev->params->dio1_pin, GPIO_IN, GPIO_RISING, _dio1_isr, dev)) {
        DEBUG("[sx1280] error: failed to initialize DIO1 pin\n");
        return -EIO;
    }
    /* sx1280_reset() and sx1280_set_reg_mode() set in ral_sx1280_init */
    res = ral_sx1280_init(&dev->ral);
    if (res == RAL_STATUS_OK) {
        uint16_t fwid;
        ral_sx1280_read_register(&dev->ral, SX1280_REG_FW_VERSION, (uint8_t *)&fwid, 2);
        if (fwid == 0x5853) {
            DEBUG("[sx1280]: firmware version = %x\n", fwid);
        }
        else {
            DEBUG("[sx1280] error: firmware version = %x\n", fwid);
            return -EIO;
        }
    }

    sx1280_init_default_config(dev);

    res = enable_irq(dev);
    if (res != SX1280_STATUS_OK) {
        DEBUG("[sx1280] init: ERROR failed irq setup, %d\n", res);
        return -EIO;
    }

    sx1280_set_lna_settings(dev, SX1280_LNA_HIGH_SENSITIVITY_MODE);

    return 0;
}

uint32_t sx1280_get_channel(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_channel\n");
    return dev->ral_params_lora.freq_in_hz;
}

void sx1280_set_channel(sx1280_t *dev, uint32_t freq)
{
    DEBUG("[sx1280]: sx1280_set_channel %" PRIu32 "Hz\n", freq);
    dev->ral_params_lora.freq_in_hz = freq;
    sx1280_set_rf_freq(dev, dev->ral_params_lora.freq_in_hz);
}

uint32_t sx1280_get_bandwidth(const sx1280_t *dev)
{
    return _get_bw(dev->ral_params_lora.bw);
}

void sx1280_set_bandwidth(sx1280_t *dev, uint16_t bandwidth)
{
    DEBUG("[sx1280]: sx1280_set_bandwidth (KHz)\n");
    switch (bandwidth) {
    case 200:
        puts("setup: setting 200KHz bandwidth");
        dev->ral_params_lora.bw = RAL_LORA_BW_200_KHZ;
        break;
    case 400:
        puts("setup: setting 400KHz bandwidth");
        dev->ral_params_lora.bw = RAL_LORA_BW_400_KHZ;
        break;
    case 800:
        puts("setup: setting 800KHz bandwidth");
        dev->ral_params_lora.bw = RAL_LORA_BW_800_KHZ;
        break;
    case 1600:
        puts("setup: setting 1600KHz bandwidth");
        dev->ral_params_lora.bw = RAL_LORA_BW_1600_KHZ;
        break;
    /* assume caller function check the validity of the value so should never go here */
    default:
        puts("[Error] setup: invalid bandwidth value given, "
             "only 200, 400, 800 or 1600 allowed.");
        return;
    }
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

uint8_t sx1280_get_spreading_factor(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_spreading_factor\n");
    /* no conversion needed */
    return dev->ral_params_lora.sf;
}

void sx1280_set_spreading_factor(sx1280_t *dev, uint8_t sf)
{
    DEBUG("[sx1280]: sx1280_set_spreading_factor\n");
    dev->ral_params_lora.sf = sf;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

uint8_t sx1280_get_coding_rate(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_coding_rate\n");
    return _ral_lora_cr_to_lora_cr(dev->ral_params_lora.cr);
}

void sx1280_set_coding_rate(sx1280_t *dev, uint8_t cr)
{
    DEBUG("[sx1280]: sx1280_set_coding_rate\n");
    dev->ral_params_lora.cr = cr;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

uint8_t sx1280_get_lora_payload_length(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_lora_payload_length\n");
    sx1280_rx_buffer_status_t rx_buffer_status;

    sx1280_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes;
}
void sx1280_set_lora_payload_length(sx1280_t *dev, uint8_t len)
{
    DEBUG("[sx1280]: sx1280_set_lora_payload_length\n");
    dev->ral_params_lora.pld_len_in_bytes = len;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

bool sx1280_get_lora_crc(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_lora_crc\n");
    return dev->ral_params_lora.crc_is_on;
}

void sx1280_set_lora_crc(sx1280_t *dev, bool crc)
{
    DEBUG("[sx1280]: sx1280_set_lora_crc\n");
    dev->ral_params_lora.crc_is_on = crc;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

bool sx1280_get_lora_implicit_header(const sx1280_t *dev)
{
    (void)dev;
    DEBUG("[sx1280]: sx1280_get_lora_implicit_header not implemented  \n");
    return false;
}

void sx1280_set_lora_implicit_header(sx1280_t *dev, bool mode)
{
    (void)dev;
    DEBUG("[sx1280]: sx1280_set_lora_implicit_header not implemented %d\n", mode);
}

uint16_t sx1280_get_lora_preamble_length(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_lora_preamble_length\n");
    return dev->ral_params_lora.pbl_len_in_symb;
}

void sx1280_set_lora_preamble_length(sx1280_t *dev, uint16_t preamble)
{
    DEBUG("[sx1280]: sx1280_set_lora_preamble_length\n");
    dev->ral_params_lora.pbl_len_in_symb = preamble;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}

bool sx1280_get_lora_iq_invert(const sx1280_t *dev)
{
    DEBUG("[sx1280]: sx1280_get_lora_iq_invert\n");
    return dev->ral_params_lora.invert_iq_is_on;
}

void sx1280_set_lora_iq_invert(sx1280_t *dev, bool iq_invert)
{
    DEBUG("[sx1280]: sx1280_set_lora_iq_invert\n");
    dev->ral_params_lora.invert_iq_is_on = iq_invert;
    ral_sx1280_setup_lora(&dev->ral, &dev->ral_params_lora);
}
