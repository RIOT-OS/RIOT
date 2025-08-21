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
#include <stdbool.h>

#include "macros/math.h"
#include "macros/utils.h"
#include "net/lora.h"
#include "periph/spi.h"
#include "time_units.h"

#include "sx126x.h"
#include "sx126x_driver.h"
#include "sx126x_driver_regs.h"
#include "sx126x_params.h"
#include "sx126x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const sx126x_pa_cfg_params_t sx1268_pa_cfg = {
    .pa_duty_cycle = 0x04,
    .hp_max = 0x06,
    .device_sel = 0x00,
    .pa_lut = 0x01
};

const sx126x_pa_cfg_params_t lpa_cfg = {
    .pa_duty_cycle = 0x04,
    .hp_max = 0x00,
    .device_sel = 0x01,
    .pa_lut = 0x01
};

const sx126x_pa_cfg_params_t hpa_cfg = {
    .pa_duty_cycle = 0x02,
    .hp_max = 0x02,
    .device_sel = 0x00,
    .pa_lut = 0x01
};

static const uint16_t _bw_khz[11] = {
    [SX126X_LORA_BW_007] = 7,
    [SX126X_LORA_BW_010] = 10,
    [SX126X_LORA_BW_015] = 15,
    [SX126X_LORA_BW_020] = 20,
    [SX126X_LORA_BW_031] = 31,
    [SX126X_LORA_BW_041] = 41,
    [SX126X_LORA_BW_062] = 62,
    [SX126X_LORA_BW_125] = 125,
    [SX126X_LORA_BW_250] = 250,
    [SX126X_LORA_BW_500] = 500,
};

/* 6.1.4 LoRa® Time-on-Air */
static uint16_t _symbol_time_on_air_us(const sx126x_t *dev)
{
    /* 1/KHz -> ms */
    /* US_PER_MS to compensate for truncate of integer division */
    uint32_t toa_us = ((uint32_t)(1 << dev->mod_params.sf) * US_PER_MS)
                      / _bw_khz[dev->mod_params.bw];
    return toa_us;

}

/* 13.4.5 SetModulationParams */
static uint8_t _ldro(const sx126x_t *dev)
{
    /* This parameter is usually set when the LoRa symbol time is equal or above 16.38 ms,
       but can be used if necessary in other situations.*/
    if (_symbol_time_on_air_us(dev) >= 16380) {
        return 0x01;
    }
    return 0x00;
}

/* 6.1.4 LoRa® Time-on-Air */
static uint32_t _symbols_numof(const sx126x_t *dev, uint16_t payload_len)
{
    uint8_t bit_crc = dev->pkt_params.crc_is_on ? 16 : 0;
    /* header is transmitted with 4/8 CR */
    uint8_t sym_header = dev->pkt_params.header_type == SX126X_LORA_PKT_IMPLICIT ? 0 : 20;
    uint8_t sf = dev->mod_params.sf;
    uint8_t cr = dev->mod_params.cr;
    if (sf == SX126X_LORA_SF5 || sf == SX126X_LORA_SF6) {
        /* SF5 and SF6:
           NSYM_preamble + 6.25 + 8 + ceil(max(8 * NBYTE_payload + NBIT_crc - 4 * SF + NSYM_header, 0) / 4 * SF) * (CR * 4)
        */
        int32_t max = MAX(8 * payload_len + bit_crc - 4 * sf + sym_header, 0);
        uint32_t ceil = DIV_ROUND_UP(max, 4 * sf);
        return dev->pkt_params.preamble_len_in_symb + 7 /* 6.25 */ + 8 + ceil * (cr * 4);
    }
    else {
        if (_ldro(dev)) {
            /* all other SF with LDRO:
               NSYM_preamble + 4.25 + 8 + ceil(max(8 * NBYTE_payload + NBIT_crc - 4 * SF + 8 + NSYM_header, 0) / 4 * SF) * (CR * 4)
            */
            int32_t max = MAX(8 * payload_len + bit_crc - 4 * sf + 8 + sym_header, 0);
            uint32_t ceil = DIV_ROUND_UP(max, 4 * sf);
            return dev->pkt_params.preamble_len_in_symb + 5 /* 4.25 */ + 8 + ceil * (cr * 4);
        }
        else {
            /* all other SF without LDRO:
               NSYM_preamble + 4.25 + 8 + ceil(max(8 * NBYTE_payload + NBIT_crc - 4 * SF + 8 + NSYM_header, 0) / 4 * (SF - 2)) * (CR * 4)
            */
            int32_t max = MAX(8 * payload_len + bit_crc - 4 * sf + 8 + sym_header, 0);
            uint32_t ceil = DIV_ROUND_UP(max, 4 * (sf - 2));
            return dev->pkt_params.preamble_len_in_symb + 5 /* 4.25 */ + 8 + ceil * (cr * 4);
        }
    }
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
        sx126x_set_pa_cfg(dev, &hpa_cfg);
    }
    else if (sx126x_is_sx1268(dev)) {
        sx126x_set_pa_cfg(dev, &sx1268_pa_cfg);
    }
    else if (sx126x_is_sx1261(dev)) {
        sx126x_set_pa_cfg(dev, &lpa_cfg);
    }
#if IS_USED(MODULE_SX126X_RF_SWITCH)
    if (dev->params->tx_pa_mode == SX126X_RF_MODE_TX_LPA){
        sx126x_set_pa_cfg(dev, &lpa_cfg);
    } else {
        sx126x_set_pa_cfg(dev, &hpa_cfg);
    }
#endif
    sx126x_set_tx_params(dev, CONFIG_SX126X_TX_POWER_DEFAULT, CONFIG_SX126X_RAMP_TIME_DEFAULT);

    dev->mod_params.bw = _sx126x_lora_bw_from(CONFIG_SX126X_LORA_BW_DEFAULT);
    dev->mod_params.sf = _sx126x_lora_sf_from(CONFIG_SX126X_LORA_SF_DEFAULT);
    dev->mod_params.cr = _sx126x_lora_cr_from(CONFIG_SX126X_LORA_CR_DEFAULT);
    dev->mod_params.ldro = _ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);

    dev->pkt_params.pld_len_in_bytes = CONFIG_SX126X_LORA_FIXED_PAYLOAD_LENGTH_DEFAULT;
    dev->pkt_params.crc_is_on = !IS_ACTIVE(CONFIG_SX126X_LORA_PAYLOAD_CRC_OFF_DEFAULT);
    dev->pkt_params.header_type = IS_ACTIVE(CONFIG_SX126X_LORA_FIXED_HEADER_LEN_MODE_DEFAULT)
                                    ? SX126X_LORA_PKT_IMPLICIT : SX126X_LORA_PKT_EXPLICIT;
    dev->pkt_params.preamble_len_in_symb = CONFIG_SX126X_LORA_PREAMBLE_LENGTH_DEFAULT;
    dev->pkt_params.invert_iq_is_on = IS_ACTIVE(CONFIG_SX126X_LORA_IQ_INVERTED_DEFAULT);
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
    sx126x_cfg_rx_boosted(dev, false);
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

#if IS_ACTIVE(SX126X_SPI)
    gpio_init(dev->params->reset_pin, GPIO_OUT);
    gpio_set(dev->params->reset_pin); /* must be held low for 100us to reset */
    gpio_init(dev->params->busy_pin, GPIO_IN_PD);
    /* Initialize DIOs */
    if (gpio_is_valid(dev->params->dio1_pin) && dev->event_cb) {
        res = gpio_init_int(dev->params->dio1_pin, GPIO_IN, GPIO_RISING,
                            dev->event_cb, dev->event_arg);
        if (res < 0) {
            DEBUG("[sx126x] error: failed to initialize DIO1 pin\n");
            return res;
        }
    }
    else {
        DEBUG("[sx126x] error: no DIO1 pin defined\n");
        return -EIO;
    }
#endif

    /* Reset the device */
    sx126x_reset(dev);

    /* Configure the power regulator mode */
    sx126x_set_reg_mode(dev, dev->params->regulator);

    /* Initialize radio with the default parameters */
    sx126x_init_default_config(dev);

    /* Configure available IRQs */
    uint16_t irq_mask_dio1 = dev->params->dio1_irq_mask;
    uint16_t irq_mask_dio2 = 0;
    uint16_t irq_mask_dio3 = 0;

#if IS_USED(MODULE_SX126X_DIO2)
    if (dev->params->dio2_mode == SX126X_DIO2_IRQ) {
        if (gpio_is_valid(dev->params->u_dio2_arg.dio2_pin)) {
            res = gpio_init_int(dev->params->u_dio2_arg.dio2_pin, GPIO_IN, GPIO_RISING,
                                dev->event_cb, dev->event_arg);
            if (res < 0) {
                DEBUG("[sx126x] error: failed to initialize DIO2 pin\n");
                return res;
            }
            irq_mask_dio2 = dev->params->u_dio2_arg.dio2_irq_mask;
        }
    }
    else if (dev->params->dio2_mode == SX126X_DIO2_RF_SWITCH) {
        if (gpio_is_valid(dev->params->u_dio2_arg.rf_switch_pin)) {
            res = gpio_init(dev->params->u_dio2_arg.rf_switch_pin, GPIO_IN);
            if (res < 0) {
                DEBUG("[sx126x] error: failed to initialize RF switch pin\n");
                return res;
            }
        }
        sx126x_set_dio2_as_rf_sw_ctrl(dev, true);
    }
#endif
#if IS_USED(MODULE_SX126X_DIO3)
    if (dev->params->dio3_mode == SX126X_DIO3_IRQ) {
        if (gpio_is_valid(dev->params->u_dio3_arg.dio3_pin)) {
            res = gpio_init_int(dev->params->u_dio3_arg.dio3_pin, GPIO_IN, GPIO_RISING,
                                dev->event_cb, dev->event_arg);
            if (res < 0) {
                DEBUG("[sx126x] error: failed to initialize DIO3 pin\n");
                return res;
            }
            irq_mask_dio3 = dev->params->u_dio3_arg.dio3_irq_mask;
        }
    }
    else if (dev->params->dio3_mode == SX126X_DIO3_TCX0) {
        sx126x_set_dio3_as_tcxo_ctrl(dev, dev->params->u_dio3_arg.tcxo_volt,
                                     dev->params->u_dio3_arg.tcx0_timeout);

        /* Once the command SetDIO3AsTCXOCtrl(...) is sent to the device,
           the register controlling the internal cap on XTA will be automatically
           changed to 0x2F (33.4 pF) to filter any spurious which could occur
           and be propagated to the PLL.- Verify that. */
        uint8_t trimming_capacitor_values[2] = { 0 };
        sx126x_read_register(dev, SX126X_REG_XTATRIM,
                             trimming_capacitor_values,
                             sizeof(trimming_capacitor_values));
        /* 11.3 pF + x * 0.47pF  = 33.4pF | x = 0x2f*/
        if (trimming_capacitor_values[0] != 0x2f) {
            DEBUG("[sx126x] warning: failed to set TCXO control: SX126X_REG_XTATRIM=%02x\n",
                  trimming_capacitor_values[0]);
        }
        DEBUG("[sx126x] XTA capacitor ~ %upF\n",
              (unsigned)(0.5f + (11.3f + trimming_capacitor_values[0] * 0.47f)));
        DEBUG("[sx126x] XTB capacitor ~ %upF\n",
              (unsigned)(0.5f + (11.3f + trimming_capacitor_values[1] * 0.47f)));

        /* When the 32 MHz clock is coming from a TCXO, the calibration will fail
           and the user should request a complete calibration after calling the function
           SetDIO3AsTcxoCtrl(...). */
        sx126x_cal(dev, SX126X_CAL_ALL);
    }
#endif
    /* The user can specify the use of DC-DC by using the command SetRegulatorMode(...).
       This operation must be carried out in STDBY_RC mode only.*/
    sx126x_set_reg_mode(dev, dev->params->regulator);

    /* Initialize radio with the default parameters */
    sx126x_init_default_config(dev);

    sx126x_set_standby(dev, SX126X_STANDBY_CFG_XOSC);

    sx126x_set_dio_irq_params(dev, SX126X_IRQ_MASK_ALL, irq_mask_dio1, irq_mask_dio2, irq_mask_dio3);

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
    DEBUG("[sx126x]: sx126x_get_radio_status \n");
    return dev->channel;
}

void sx126x_set_channel(sx126x_t *dev, uint32_t freq)
{
    DEBUG("[sx126x]: sx126x_set_channel %" PRIu32 "Hz \n", freq);
    dev->channel = freq;
    sx126x_set_rf_freq(dev, dev->channel);
}

uint8_t sx126x_get_bandwidth(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_bandwidth \n");
    return _sx126x_lora_bw_to(dev->mod_params.bw);
}

void sx126x_set_bandwidth(sx126x_t *dev, uint8_t bandwidth)
{
    DEBUG("[sx126x]: sx126x_set_bandwidth %02x\n", bandwidth);
    dev->mod_params.bw = _sx126x_lora_bw_from(bandwidth);
    dev->mod_params.ldro = _ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_spreading_factor(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_spreading_factor \n");
    return _sx126x_lora_sf_to(dev->mod_params.sf);
}

void sx126x_set_spreading_factor(sx126x_t *dev, uint8_t sf)
{
    DEBUG("[sx126x]: sx126x_set_spreading_factor : %02x\n", sf);
    dev->mod_params.sf = _sx126x_lora_sf_from(sf);
    dev->mod_params.ldro = _ldro(dev);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_coding_rate(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_coding_rate \n");
    return _sx126x_lora_cr_to(dev->mod_params.cr);
}

void sx126x_set_coding_rate(sx126x_t *dev, uint8_t cr)
{
    DEBUG("[sx126x]: sx126x_set_coding_rate %01x\n", cr);
    dev->mod_params.cr = _sx126x_lora_cr_from(cr);
    sx126x_set_lora_mod_params(dev, &dev->mod_params);
}

uint8_t sx126x_get_lora_payload_length(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_lora_payload_length \n");
    sx126x_rx_buffer_status_t rx_buffer_status;

    sx126x_get_rx_buffer_status(dev, &rx_buffer_status);
    return rx_buffer_status.pld_len_in_bytes;
}

void sx126x_set_lora_payload_length(sx126x_t *dev, uint8_t len)
{
    DEBUG("[sx126x]: sx126x_set_lora_payload_length %d\n", len);
    dev->pkt_params.pld_len_in_bytes = len;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_crc(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_lora_crc \n");
    return dev->pkt_params.crc_is_on;
}

void sx126x_set_lora_crc(sx126x_t *dev, bool crc)
{
    DEBUG("[sx126x]: sx126x_set_lora_crc %d\n", crc);
    dev->pkt_params.crc_is_on = crc;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_implicit_header(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_lora_implicit_header \n");
    return dev->pkt_params.header_type == SX126X_LORA_PKT_IMPLICIT;
}

void sx126x_set_lora_implicit_header(sx126x_t *dev, bool mode)
{
    DEBUG("[sx126x]: sx126x_set_lora_implicit_header %d\n", mode);
    dev->pkt_params.header_type = (mode ? SX126X_LORA_PKT_IMPLICIT : SX126X_LORA_PKT_EXPLICIT);
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

uint16_t sx126x_get_lora_preamble_length(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_lora_preamble_length \n");
    return dev->pkt_params.preamble_len_in_symb;
}

void sx126x_set_lora_preamble_length(sx126x_t *dev, uint16_t preamble)
{
    DEBUG("[sx126x]: sx126x_set_lora_preamble_length %" PRIu16 "\n", preamble);
    dev->pkt_params.preamble_len_in_symb = preamble;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

bool sx126x_get_lora_iq_invert(const sx126x_t *dev)
{
    DEBUG("[sx126x]: sx126x_get_lora_iq_invert \n");
    return dev->pkt_params.invert_iq_is_on;
}

void sx126x_set_lora_iq_invert(sx126x_t *dev, bool iq_invert)
{
    DEBUG("[sx126x]: sx126x_set_lora_iq_invert %d\n", iq_invert);
    dev->pkt_params.invert_iq_is_on = iq_invert;
    sx126x_set_lora_pkt_params(dev, &dev->pkt_params);
}

uint32_t sx126x_symbol_time_on_air_us(const sx126x_t *dev)
{
    return _symbol_time_on_air_us(dev);
}

uint32_t sx126x_time_on_air_us(const sx126x_t *dev, uint16_t payload_len)
{
    return _symbols_numof(dev, payload_len) * _symbol_time_on_air_us(dev);
}

#if IS_USED(MODULE_SX126X_STM32WL)
sx126x_t *sx126x_stm32wl = NULL;
void isr_subghz_radio(void)
{
    /* Disable NVIC to avoid ISR conflict in CPU. */
    NVIC_DisableIRQ(SUBGHZ_Radio_IRQn);
    NVIC_ClearPendingIRQ(SUBGHZ_Radio_IRQn);
    if (sx126x_stm32wl && sx126x_stm32wl->event_cb) {
        sx126x_stm32wl->event_cb(sx126x_stm32wl->event_arg);
    }
    cortexm_isr_end();
}
#endif
