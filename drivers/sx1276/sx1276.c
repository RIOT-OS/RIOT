/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx1276
 * @{
 * @file
 * @brief       Basic functionality of sx1276 driver
 *
 * @author      Eugene P. [ep@unwds.com]
 * @}
 */
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "xtimer.h"
#include "thread.h"

#include "sx1276.h"
#include "include/sx1276_regs_fsk.h"
#include "include/sx1276_regs_lora.h"

/**
 * Radio registers definition
 */
typedef struct {
    sx1276_radio_modems_t modem;
    uint8_t addr;
    uint8_t value;

} sx1276_radio_registers_t;

/*
 * Private functions prototypes
 */

/**
 * @brief Performs the Rx chain calibration for LF and HF bands
 * Must be called just after the reset so all registers are at their
 *         default values
 */
static void _rx_chain_calibration(sx1276_t *dev);

/**
 * @brief Resets the SX1276
 */
void sx1276_reset(sx1276_t *dev);

/**
 * @brief Writes the buffer contents to the SX1276 FIFO
 *
 * @param [IN] buffer Buffer containing data to be put on the FIFO.
 * @param [IN] size Number of bytes to be written to the FIFO
 */
void sx1276_write_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief Reads the contents of the SX1276 FIFO
 *
 * @param [OUT] buffer Buffer where to copy the FIFO read data.
 * @param [IN] size Number of bytes to be read from the FIFO
 */
void sx1276_read_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size);

/**
 * @brief Sets the SX1276 operating mode
 *
 * @param [IN] op_mode New operating mode
 */
void sx1276_set_op_mode(sx1276_t *dev, uint8_t op_mode);

/*
 * Private global constants
 */

/**
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET_LF                              -164
#define RSSI_OFFSET_HF                              -157

static void sx1276_set_status(sx1276_t *dev, sx1276_radio_state_t state)
{
    dev->settings.state = state;
}

/**
 * @brief SX1276 DIO interrupt handlers initialization
 */

static void sx1276_on_dio0_isr(void *arg);
static void sx1276_on_dio1_isr(void *arg);
static void sx1276_on_dio2_isr(void *arg);
static void sx1276_on_dio3_isr(void *arg);

static void _init_isrs(sx1276_t *dev)
{
    gpio_init_int(dev->dio0_pin, GPIO_IN, GPIO_RISING, sx1276_on_dio0_isr, dev);
    gpio_init_int(dev->dio1_pin, GPIO_IN, GPIO_RISING, sx1276_on_dio1_isr, dev);
    gpio_init_int(dev->dio2_pin, GPIO_IN, GPIO_RISING, sx1276_on_dio2_isr, dev);
    gpio_init_int(dev->dio3_pin, GPIO_IN, GPIO_RISING, sx1276_on_dio3_isr, dev);
}

static inline void send_event(sx1276_t *dev, sx1276_event_type_t event_type)
{
    if (dev->sx1276_event_cb != NULL) {
        dev->sx1276_event_cb(dev, event_type);
    }
}

/**
 * @brief Timeout timers internal routines
 */
static void _on_tx_timeout(void *arg)
{
    sx1276_t *dev = (sx1276_t *) arg;

    /* TX timeout. Send event message to the application's thread */
    send_event(dev, SX1276_TX_TIMEOUT);
}

static void _on_rx_timeout(void *arg)
{
    sx1276_t *dev = (sx1276_t *) arg;

    /* RX timeout. Send event message to the application's thread */
    send_event(dev, SX1276_RX_TIMEOUT);
}

/**
 * @brief Sets timers callbacks and arguments
 */
static void _init_timers(sx1276_t *dev)
{
    dev->_internal.tx_timeout_timer.arg = dev;
    dev->_internal.tx_timeout_timer.callback = _on_tx_timeout;

    dev->_internal.rx_timeout_timer.arg = dev;
    dev->_internal.rx_timeout_timer.callback = _on_rx_timeout;
}

static int _init_peripherals(sx1276_t *dev)
{
    int res;

    /* Setup SPI for SX1276 */
    spi_acquire(dev->spi);
    res = spi_init_master(dev->spi, SPI_CONF_FIRST_RISING, SPI_SPEED_1MHZ);
    spi_release(dev->spi);

    if (res < 0) {
        printf("sx1276: error initializing SPI_%i device (code %i)\n",
        		dev->spi, res);
        return 0;
    }

    res = gpio_init(dev->nss_pin, GPIO_OUT);
    if (res < 0) {
        printf("sx1276: error initializing GPIO_%ld as CS line (code %i)\n",
               (long)dev->nss_pin, res);
        return 0;
    }

    gpio_set(dev->nss_pin);

    return 1;
}

sx1276_init_result_t sx1276_init(sx1276_t *dev)
{
    sx1276_reset(dev);

    /** Do internal initialization routines */
    if (!_init_peripherals(dev)) {
        return SX1276_ERR_SPI;
    }

    _init_isrs(dev);
    _init_timers(dev);

    /* Check presence of SX1276 */
    if (!sx1276_test(dev)) {
        DEBUG("init_radio: test failed");
        return SX1276_ERR_TEST_FAILED;
    }

    _rx_chain_calibration(dev);

    /* Set RegOpMode value to the datasheet's default. Actual default after POR is 0x09 */
    sx1276_reg_write(dev, SX1276_REG_OPMODE, 0x00);

    /* Switch into LoRa mode */
    sx1276_set_modem(dev, SX1276_MODEM_LORA);

    /* Set current frequency */
    sx1276_set_channel(dev, dev->settings.channel);

    /* Create DIO event lines handler */
    kernel_pid_t pid = thread_create((char *) dev->_internal.dio_polling_thread_stack, sizeof(dev->_internal.dio_polling_thread_stack), THREAD_PRIORITY_MAIN,
                                     THREAD_CREATE_STACKTEST, dio_polling_thread, dev,
                                     "sx1276 DIO handler");

    if (pid <= KERNEL_PID_UNDEF) {
        DEBUG("sx1276: creation of DIO handling thread");
        return SX1276_ERR_THREAD;
    }
    dev->_internal.dio_polling_thread_pid = pid;

    return SX1276_INIT_OK;
}

sx1276_radio_state_t sx1276_get_status(sx1276_t *dev)
{
    return dev->settings.state;
}

void sx1276_set_channel(sx1276_t *dev, uint32_t freq)
{
    /* Save current operating mode */
    uint8_t prev_mode = sx1276_reg_read(dev, SX1276_REG_OPMODE);

    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_STANDBY);

    freq = (uint32_t)((double) freq / (double) SX1276_FREQ_STEP);

    /* Write frequency settings into chip */
    sx1276_reg_write(dev, SX1276_REG_FRFMSB, (uint8_t)((freq >> 16) & 0xFF));
    sx1276_reg_write(dev, SX1276_REG_FRFMID, (uint8_t)((freq >> 8) & 0xFF));
    sx1276_reg_write(dev, SX1276_REG_FRFLSB, (uint8_t)(freq & 0xFF));

    /* Restore previous operating mode */
    sx1276_reg_write(dev, SX1276_REG_OPMODE, prev_mode);
}

bool sx1276_test(sx1276_t *dev)
{
    /* Read version number and compare with sx1276 assigned revision */
    uint8_t version = sx1276_reg_read(dev, SX1276_REG_VERSION);

    if (version != VERSION_SX1276 || version == 0x1C) {
        printf("sx1276: test failed, invalid version number: %d\n", version);
        return false;
    }

    return true;
}

bool sx1276_is_channel_free(sx1276_t *dev, uint32_t freq, int16_t rssi_thresh)
{
    int16_t rssi = 0;

    sx1276_set_channel(dev, freq);
    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_RECEIVER);

    xtimer_usleep(1000); /* wait 1 millisecond */

    rssi = sx1276_read_rssi(dev);
    sx1276_set_sleep(dev);

    if (rssi > rssi_thresh) {
        return false;
    }

    return true;
}

void sx1276_set_modem(sx1276_t *dev, sx1276_radio_modems_t modem)
{
    dev->settings.modem = modem;

    switch (dev->settings.modem) {
        case SX1276_MODEM_LORA:
            sx1276_set_op_mode(dev, SX1276_RF_OPMODE_SLEEP);
            sx1276_reg_write(dev,
                             SX1276_REG_OPMODE,
                             (sx1276_reg_read(dev, SX1276_REG_OPMODE)
                              & SX1276_RF_LORA_OPMODE_LONGRANGEMODE_MASK)
                             | SX1276_RF_LORA_OPMODE_LONGRANGEMODE_ON);

            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING1, 0x00);
            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING2, 0x10); /* DIO5=ClkOut */
            break;

        case SX1276_MODEM_FSK:
            sx1276_set_op_mode(dev, SX1276_RF_OPMODE_SLEEP);
            sx1276_reg_write(dev,
                             SX1276_REG_OPMODE,
                             (sx1276_reg_read(dev, SX1276_REG_OPMODE)
                              & SX1276_RF_LORA_OPMODE_LONGRANGEMODE_MASK)
                             | SX1276_RF_LORA_OPMODE_LONGRANGEMODE_OFF);

            sx1276_reg_write(dev, SX1276_REG_DIOMAPPING1, 0x00);
            //sx1276_reg_write(dev, SX1276_REG_DIOMAPPING2, 0x20); /* DIO5=mode_ready */
            break;
        default:
            break;
    }
}

#define RXLORA_RXMODE_RSSI_SX1276_REG_MODEM_CONFIG1 0x0A
#define RXLORA_RXMODE_RSSI_SX1276_REG_MODEM_CONFIG2 0x70

uint32_t sx1276_random(sx1276_t *dev)
{
    uint8_t i;
    uint32_t rnd = 0;

    sx1276_set_modem(dev, SX1276_MODEM_LORA); /* Set LoRa modem ON */

    /* Disable LoRa modem interrupts */
    sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK, SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                     SX1276_RF_LORA_IRQFLAGS_RXDONE |
                     SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                     SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                     SX1276_RF_LORA_IRQFLAGS_TXDONE |
                     SX1276_RF_LORA_IRQFLAGS_CADDONE |
                     SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                     SX1276_RF_LORA_IRQFLAGS_CADDETECTED);

    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_STANDBY);
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, RXLORA_RXMODE_RSSI_SX1276_REG_MODEM_CONFIG1);
    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG1, RXLORA_RXMODE_RSSI_SX1276_REG_MODEM_CONFIG2);

    /* Set radio in continuous reception */
    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_RECEIVER);

    for (i = 0; i < 32; i++) {
        xtimer_usleep(1000); /* wait for the chaos */

        /* Non-filtered RSSI value reading. Only takes the LSB value */
        rnd |= ((uint32_t) sx1276_reg_read(dev, SX1276_REG_LR_RSSIWIDEBAND) & 0x01) << i;
    }

    sx1276_set_sleep(dev);

    return rnd;
}

/**
 * @brief Performs the Rx chain calibration for LF and HF bands
 * @note Must be called just after the reset so all registers are at their
 *         default values
 */
static void _rx_chain_calibration(sx1276_t *dev)
{
    uint8_t reg_pa_config_init_val;
    uint32_t initial_freq;

    /* Save context */
    reg_pa_config_init_val = sx1276_reg_read(dev, SX1276_REG_PACONFIG);
    initial_freq = (double) (((uint32_t) sx1276_reg_read(dev, SX1276_REG_FRFMSB) << 16)
                             | ((uint32_t) sx1276_reg_read(dev, SX1276_REG_FRFMID) << 8)
                             | ((uint32_t) sx1276_reg_read(dev, SX1276_REG_FRFLSB))) * (double) SX1276_FREQ_STEP;

    /* Cut the PA just in case, RFO output, power = -1 dBm */
    sx1276_reg_write(dev, SX1276_REG_PACONFIG, 0x00);

    /* Launch Rx chain calibration for LF band */
    sx1276_reg_write(dev,
                     SX1276_REG_IMAGECAL,
                     (sx1276_reg_read(dev, SX1276_REG_IMAGECAL) & SX1276_RF_IMAGECAL_IMAGECAL_MASK)
                     | SX1276_RF_IMAGECAL_IMAGECAL_START);

    while ((sx1276_reg_read(dev, SX1276_REG_IMAGECAL) & SX1276_RF_IMAGECAL_IMAGECAL_RUNNING)
           == SX1276_RF_IMAGECAL_IMAGECAL_RUNNING) {
    }

    /* Set a frequency in HF band */
    sx1276_set_channel(dev, SX1276_CHANNEL_HF);

    /* Launch Rx chain calibration for HF band */
    sx1276_reg_write(dev,
                     SX1276_REG_IMAGECAL,
                     (sx1276_reg_read(dev, SX1276_REG_IMAGECAL) & SX1276_RF_IMAGECAL_IMAGECAL_MASK)
                     | SX1276_RF_IMAGECAL_IMAGECAL_START);
    while ((sx1276_reg_read(dev, SX1276_REG_IMAGECAL) & SX1276_RF_IMAGECAL_IMAGECAL_RUNNING)
           == SX1276_RF_IMAGECAL_IMAGECAL_RUNNING) {
    }

    /* Restore context */
    sx1276_reg_write(dev, SX1276_REG_PACONFIG, reg_pa_config_init_val);
    sx1276_set_channel(dev, initial_freq);
}

static inline uint8_t sx1276_get_pa_select(uint32_t channel)
{
    if (channel < SX1276_RF_MID_BAND_THRESH) {
        return SX1276_RF_PACONFIG_PASELECT_PABOOST;
    }
    else {
        return SX1276_RF_PACONFIG_PASELECT_RFO;
    }
}

static void setup_power_amplifier(sx1276_t *dev, sx1276_lora_settings_t *settings)
{
    uint8_t pa_config = 0;
    uint8_t pa_dac = 0;

    pa_config = sx1276_reg_read(dev, SX1276_REG_PACONFIG);
    pa_dac = sx1276_reg_read(dev, SX1276_REG_PADAC);

    pa_config = (pa_config & SX1276_RF_PACONFIG_PASELECT_MASK) | sx1276_get_pa_select(dev->settings.channel) << 7;
    pa_config = (pa_config & SX1276_RF_PACONFIG_MAX_POWER_MASK) | (0x05 << 4); // max power is 14dBm

    sx1276_reg_write(dev, SX1276_REG_PARAMP, SX1276_RF_PARAMP_0050_US);

    if ((pa_config & SX1276_RF_PACONFIG_PASELECT_PABOOST)
        == SX1276_RF_PACONFIG_PASELECT_PABOOST) {
        if (dev->settings.lora.power > 17) {
            pa_dac = (pa_dac & SX1276_RF_PADAC_20DBM_MASK) | SX1276_RF_PADAC_20DBM_ON;
        }
        else {
            pa_dac = (pa_dac & SX1276_RF_PADAC_20DBM_MASK) | SX1276_RF_PADAC_20DBM_OFF;
        }
        if ((pa_dac & SX1276_RF_PADAC_20DBM_ON) == SX1276_RF_PADAC_20DBM_ON) {
            if (dev->settings.lora.power < 5) {
                dev->settings.lora.power = 5;
            }
            if (dev->settings.lora.power > 20) {
                dev->settings.lora.power = 20;
            }

            pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                        | (uint8_t)((uint16_t)(dev->settings.lora.power - 5) & 0x0F);
        }
        else {
            if (dev->settings.lora.power < 2) {
                dev->settings.lora.power = 2;
            }
            if (dev->settings.lora.power > 17) {
                dev->settings.lora.power = 17;
            }

            pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                        | (uint8_t)((uint16_t)(dev->settings.lora.power - 2) & 0x0F);
        }
    }
    else {
        if (dev->settings.lora.power < -1) {
            dev->settings.lora.power = -1;
        }
        if (dev->settings.lora.power > 14) {
            dev->settings.lora.power = 14;
        }

        pa_config = (pa_config & SX1276_RF_PACONFIG_OUTPUTPOWER_MASK)
                    | (uint8_t)((uint16_t)(dev->settings.lora.power + 1) & 0x0F);
    }

    sx1276_reg_write(dev, SX1276_REG_PACONFIG, pa_config);
    sx1276_reg_write(dev, SX1276_REG_PADAC, pa_dac);
}

void sx1276_configure_lora(sx1276_t *dev, sx1276_lora_settings_t *settings)
{
    sx1276_set_modem(dev, SX1276_MODEM_LORA);

    /* Copy LoRa configuration into device structure */
    if (settings != NULL) {
        memcpy(&dev->settings.lora, settings, sizeof(sx1276_lora_settings_t));
    }

    if (((dev->settings.lora.bandwidth == SX1276_BW_125_KHZ) && ((dev->settings.lora.datarate == SX1276_SF11) || (dev->settings.lora.datarate == SX1276_SF12)))
        || ((dev->settings.lora.bandwidth == SX1276_BW_250_KHZ) && (dev->settings.lora.datarate == SX1276_SF12))) {
        dev->settings.lora.low_datarate_optimize = 0x01;
    }
    else {
        dev->settings.lora.low_datarate_optimize = 0x00;
    }

    sx1276_reg_write(dev,
                     SX1276_REG_LR_MODEMCONFIG1,
                     (sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG1) &
                      SX1276_RF_LORA_MODEMCONFIG1_BW_MASK &
                      SX1276_RF_LORA_MODEMCONFIG1_CODINGRATE_MASK &
                      SX1276_RF_LORA_MODEMCONFIG1_IMPLICITHEADER_MASK) | (dev->settings.lora.bandwidth << 4)
                     | (dev->settings.lora.coderate << 1) | dev->settings.lora.implicit_header);

    sx1276_reg_write(dev, SX1276_REG_LR_MODEMCONFIG2,
                     (sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG2) &
                      SX1276_RF_LORA_MODEMCONFIG2_SF_MASK &
                      SX1276_RF_LORA_MODEMCONFIG2_RXPAYLOADCRC_MASK &
                      SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK) | (dev->settings.lora.datarate << 4)
                     | (dev->settings.lora.crc_on << 2)
                     | ((dev->settings.lora.rx_timeout >> 8)
                        & ~SX1276_RF_LORA_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK));

    sx1276_reg_write(dev,
                     SX1276_REG_LR_MODEMCONFIG3,
                     (sx1276_reg_read(dev, SX1276_REG_LR_MODEMCONFIG3)
                      & SX1276_RF_LORA_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK)
                     | (dev->settings.lora.low_datarate_optimize << 3));

    sx1276_reg_write(dev, SX1276_REG_LR_SYMBTIMEOUTLSB,
                     (uint8_t)(dev->settings.lora.rx_timeout & 0xFF));

    sx1276_reg_write(dev, SX1276_REG_LR_PREAMBLEMSB,
                     (uint8_t)((dev->settings.lora.preamble_len >> 8) & 0xFF));
    sx1276_reg_write(dev, SX1276_REG_LR_PREAMBLELSB,
                     (uint8_t)(dev->settings.lora.preamble_len & 0xFF));

    if (dev->settings.lora.implicit_header) {
        sx1276_reg_write(dev, SX1276_REG_LR_PAYLOADLENGTH, dev->settings.lora.payload_len);
    }


    if (dev->settings.lora.freq_hop_on) {
        sx1276_reg_write(dev,
                         SX1276_REG_LR_PLLHOP,
                         (sx1276_reg_read(dev, SX1276_REG_LR_PLLHOP)
                          & SX1276_RF_LORA_PLLHOP_FASTHOP_MASK) | SX1276_RF_LORA_PLLHOP_FASTHOP_ON);
        sx1276_reg_write(dev, SX1276_REG_LR_HOPPERIOD,
                         dev->settings.lora.hop_period);
    }

    setup_power_amplifier(dev, settings);

    /* ERRATA sensetivity tweaks */
    if ((dev->settings.lora.bandwidth == SX1276_BW_500_KHZ) && (SX1276_RF_MID_BAND_THRESH)) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x02);
        sx1276_reg_write(dev, SX1276_REG_LR_TEST3A, 0x64);
    }
    else if (dev->settings.lora.bandwidth == SX1276_BW_500_KHZ) {
        /* ERRATA 2.1 - Sensitivity Optimization with a 500 kHz Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x02);
        sx1276_reg_write(dev, SX1276_REG_LR_TEST3A, 0x7F);
    }
    else {
        /* ERRATA 2.1 - Sensitivity Optimization with another Bandwidth */
        sx1276_reg_write(dev, SX1276_REG_LR_TEST36, 0x03);
    }

    sx1276_reg_write(dev, SX1276_REG_LR_DETECTOPTIMIZE, SX1276_RF_LORA_DETECTIONOPTIMIZE_SF7_TO_SF12);
    sx1276_reg_write(dev, SX1276_REG_LR_DETECTIONTHRESHOLD, SX1276_RF_LORA_DETECTIONTHRESH_SF7_TO_SF12);
}

void sx1276_configure_lora_bw(sx1276_t *dev, sx1276_lora_bandwidth_t bw)
{
    dev->settings.lora.bandwidth = bw;
    sx1276_configure_lora(dev, NULL);
}

void sx1276_configure_lora_sf(sx1276_t *dev, sx1276_lora_spreading_factor_t sf)
{
    dev->settings.lora.datarate = sf;
    sx1276_configure_lora(dev, NULL);
}

void sx1276_configure_lora_cr(sx1276_t *dev, sx1276_lora_coding_rate_t cr)
{
    dev->settings.lora.coderate = cr;
    sx1276_configure_lora(dev, NULL);
}

uint32_t sx1276_get_time_on_air(sx1276_t *dev, sx1276_radio_modems_t modem,
                                uint8_t pkt_len)
{
    uint32_t air_time = 0;

    switch (modem) {
        case SX1276_MODEM_FSK:
            break;

        case SX1276_MODEM_LORA:
        {
            double bw = 0.0;

            /* Note: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported. */
            switch (dev->settings.lora.bandwidth) {
                case 7: /* 125 kHz */
                    bw = 125e3;
                    break;
                case 8: /* 250 kHz */
                    bw = 250e3;
                    break;
                case 9: /* 500 kHz */
                    bw = 500e3;
                    break;
            }

            /* Symbol rate : time for one symbol [secs] */
            double rs = bw / (1 << dev->settings.lora.datarate);
            double ts = 1 / rs;

            /* time of preamble */
            double t_preamble = (dev->settings.lora.preamble_len + 4.25) * ts;

            /* Symbol length of payload and time */
            double tmp =
                ceil(
                    (8 * pkt_len - 4 * dev->settings.lora.datarate + 28
                     + 16 * dev->settings.lora.crc_on
                     - (!dev->settings.lora.implicit_header ? 20 : 0))
                    / (double) (4 * dev->settings.lora.datarate
                                - ((dev->settings.lora.low_datarate_optimize
                                    > 0) ? 2 : 0)))
                * (dev->settings.lora.coderate + 4);
            double n_payload = 8 + ((tmp > 0) ? tmp : 0);
            double t_payload = n_payload * ts;

            /* Time on air */
            double t_on_air = t_preamble + t_payload;

            /* return seconds */
            air_time = floor(t_on_air * 1e6 + 0.999);
        }
        break;
    }

    return air_time;
}

void sx1276_send(sx1276_t *dev, uint8_t *buffer, uint8_t size)
{
    switch (dev->settings.modem) {
        case SX1276_MODEM_FSK:
            sx1276_write_fifo(dev, &size, 1);
            sx1276_write_fifo(dev, buffer, size);
            break;

        case SX1276_MODEM_LORA:
        {

            if (dev->settings.lora.iq_inverted) {
                sx1276_reg_write(dev,
                                 SX1276_REG_LR_INVERTIQ,
                                 ((sx1276_reg_read(dev, SX1276_REG_LR_INVERTIQ)
                                   & SX1276_RF_LORA_INVERTIQ_TX_MASK & SX1276_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX1276_RF_LORA_INVERTIQ_RX_OFF | SX1276_RF_LORA_INVERTIQ_TX_ON));
                sx1276_reg_write(dev, SX1276_REG_LR_INVERTIQ2, SX1276_RF_LORA_INVERTIQ2_ON);
            }
            else {
                sx1276_reg_write(dev,
                                 SX1276_REG_LR_INVERTIQ,
                                 ((sx1276_reg_read(dev, SX1276_REG_LR_INVERTIQ)
                                   & SX1276_RF_LORA_INVERTIQ_TX_MASK & SX1276_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX1276_RF_LORA_INVERTIQ_RX_OFF | SX1276_RF_LORA_INVERTIQ_TX_OFF));
                sx1276_reg_write(dev, SX1276_REG_LR_INVERTIQ2, SX1276_RF_LORA_INVERTIQ2_OFF);
            }

            /* Initializes the payload size */
            sx1276_reg_write(dev, SX1276_REG_LR_PAYLOADLENGTH, size);

            /* Full buffer used for Tx */
            sx1276_reg_write(dev, SX1276_REG_LR_FIFOTXBASEADDR, 0x00);
            sx1276_reg_write(dev, SX1276_REG_LR_FIFOADDRPTR, 0x00);

            /* FIFO operations can not take place in Sleep mode
             * So wake up the chip */
            if ((sx1276_reg_read(dev, SX1276_REG_OPMODE) & ~SX1276_RF_OPMODE_MASK)
                == SX1276_RF_OPMODE_SLEEP) {
                sx1276_set_standby(dev);
                xtimer_usleep(SX1276_RADIO_WAKEUP_TIME); /* wait for chip wake up */
            }

            /* Write payload buffer */
            sx1276_write_fifo(dev, buffer, size);
        }
        break;
    }

    /* Enable TXDONE interrupt */
    sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK,
                     SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                     SX1276_RF_LORA_IRQFLAGS_RXDONE |
                     SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                     SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                     //SX1276_RF_LORA_IRQFLAGS_TXDONE |
                     SX1276_RF_LORA_IRQFLAGS_CADDONE |
                     SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                     SX1276_RF_LORA_IRQFLAGS_CADDETECTED);

    /* Set TXDONE interrupt to the DIO0 line */
    sx1276_reg_write(dev,
                     SX1276_REG_DIOMAPPING1,
                     (sx1276_reg_read(dev, SX1276_REG_DIOMAPPING1)
                      & SX1276_RF_LORA_DIOMAPPING1_DIO0_MASK)
                     | SX1276_RF_LORA_DIOMAPPING1_DIO0_01);


    /* Start TX timeout timer */
    xtimer_set(&dev->_internal.tx_timeout_timer, dev->settings.lora.tx_timeout);

    /* Put chip into transfer mode */
    sx1276_set_status(dev, SX1276_RF_TX_RUNNING);
    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_TRANSMITTER);
}

void sx1276_set_sleep(sx1276_t *dev)
{
    /* Disable running timers */
    xtimer_remove(&dev->_internal.tx_timeout_timer);
    xtimer_remove(&dev->_internal.rx_timeout_timer);

    /* Put chip into sleep */
    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_SLEEP);
    sx1276_set_status(dev,  SX1276_RF_IDLE);
}

void sx1276_set_standby(sx1276_t *dev)
{
    /* Disable running timers */
    xtimer_remove(&dev->_internal.tx_timeout_timer);
    xtimer_remove(&dev->_internal.rx_timeout_timer);

    sx1276_set_op_mode(dev, SX1276_RF_OPMODE_STANDBY);
    sx1276_set_status(dev,  SX1276_RF_IDLE);
}

void sx1276_set_rx(sx1276_t *dev, uint32_t timeout)
{
    bool rx_continuous = false;

    switch (dev->settings.modem) {
        case SX1276_MODEM_FSK:
            break;

        case SX1276_MODEM_LORA:
        {
            if (dev->settings.lora.iq_inverted) {
                sx1276_reg_write(dev,
                                 SX1276_REG_LR_INVERTIQ,
                                 ((sx1276_reg_read(dev, SX1276_REG_LR_INVERTIQ)
                                   & SX1276_RF_LORA_INVERTIQ_TX_MASK & SX1276_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX1276_RF_LORA_INVERTIQ_RX_ON | SX1276_RF_LORA_INVERTIQ_TX_OFF));
                sx1276_reg_write(dev, SX1276_REG_LR_INVERTIQ2, SX1276_RF_LORA_INVERTIQ2_ON);
            }
            else {
                sx1276_reg_write(dev,
                                 SX1276_REG_LR_INVERTIQ,
                                 ((sx1276_reg_read(dev, SX1276_REG_LR_INVERTIQ)
                                   & SX1276_RF_LORA_INVERTIQ_TX_MASK & SX1276_RF_LORA_INVERTIQ_RX_MASK)
                                  | SX1276_RF_LORA_INVERTIQ_RX_OFF | SX1276_RF_LORA_INVERTIQ_TX_OFF));
                sx1276_reg_write(dev, SX1276_REG_LR_INVERTIQ2, SX1276_RF_LORA_INVERTIQ2_OFF);
            }

            /* ERRATA 2.3 - Receiver Spurious Reception of a LoRa Signal */
            if (dev->settings.lora.bandwidth < 9) {
                sx1276_reg_write(dev, SX1276_REG_LR_DETECTOPTIMIZE,
                                 sx1276_reg_read(dev, SX1276_REG_LR_DETECTOPTIMIZE) & 0x7F);
                sx1276_reg_write(dev, SX1276_REG_LR_TEST30, 0x00);
                switch (dev->settings.lora.bandwidth) {
                    case SX1276_BW_125_KHZ: // 125 kHz
                        sx1276_reg_write(dev, SX1276_REG_LR_TEST2F, 0x40);
                        break;
                    case SX1276_BW_250_KHZ: // 250 kHz
                        sx1276_reg_write(dev, SX1276_REG_LR_TEST2F, 0x40);
                        break;

                    default:
                        break;
                }
            }
            else {
                sx1276_reg_write(dev, SX1276_REG_LR_DETECTOPTIMIZE,
                                 sx1276_reg_read(dev, SX1276_REG_LR_DETECTOPTIMIZE) | 0x80);
            }

            rx_continuous = dev->settings.lora.rx_continuous;

            /* Setup interrupts */
            if (dev->settings.lora.freq_hop_on) {
                sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK,  //SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                                            //SX1276_RF_LORA_IRQFLAGS_RXDONE |
                                                            //SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                 SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 SX1276_RF_LORA_IRQFLAGS_TXDONE |
                                 SX1276_RF_LORA_IRQFLAGS_CADDONE |
                                 //SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                 SX1276_RF_LORA_IRQFLAGS_CADDETECTED);

                // DIO0=RxDone, DIO2=FhssChangeChannel
                sx1276_reg_write(dev,
                                 SX1276_REG_DIOMAPPING1,
                                 (sx1276_reg_read(dev, SX1276_REG_DIOMAPPING1)
                                  & SX1276_RF_LORA_DIOMAPPING1_DIO0_MASK
                                  & SX1276_RF_LORA_DIOMAPPING1_DIO2_MASK)
                                 | SX1276_RF_LORA_DIOMAPPING1_DIO0_00
                                 | SX1276_RF_LORA_DIOMAPPING1_DIO2_00);
            }
            else {
                sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK,  //SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                                                            //SX1276_RF_LORA_IRQFLAGS_RXDONE |
                                                            //SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                                 SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                                 SX1276_RF_LORA_IRQFLAGS_TXDONE |
                                 SX1276_RF_LORA_IRQFLAGS_CADDONE |
                                 SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                 SX1276_RF_LORA_IRQFLAGS_CADDETECTED);

                // DIO0=RxDone
                sx1276_reg_write(dev,
                                 SX1276_REG_DIOMAPPING1,
                                 (sx1276_reg_read(dev, SX1276_REG_DIOMAPPING1)
                                  & SX1276_RF_LORA_DIOMAPPING1_DIO0_MASK)
                                 | SX1276_RF_LORA_DIOMAPPING1_DIO0_00);
            }

            sx1276_reg_write(dev, SX1276_REG_LR_FIFORXBASEADDR, 0);
            sx1276_reg_write(dev, SX1276_REG_LR_FIFOADDRPTR, 0);
        }
        break;
    }

    sx1276_set_status(dev, SX1276_RF_RX_RUNNING);

    if (rx_continuous) {
        sx1276_set_op_mode(dev, SX1276_RF_LORA_OPMODE_RECEIVER);
    }
    else {
        if (timeout != 0) {
            xtimer_set(&(dev->_internal.rx_timeout_timer), timeout);
        }

        sx1276_set_op_mode(dev, SX1276_RF_LORA_OPMODE_RECEIVER_SINGLE);
    }
}

void sx1276_start_cad(sx1276_t *dev)
{
    switch (dev->settings.modem) {
        case SX1276_MODEM_FSK:
        {

        }
        break;
        case SX1276_MODEM_LORA:
        {
			/* Disable all interrupts except CAD-related */
            sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGSMASK, SX1276_RF_LORA_IRQFLAGS_RXTIMEOUT |
                             SX1276_RF_LORA_IRQFLAGS_RXDONE |
                             SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR |
                             SX1276_RF_LORA_IRQFLAGS_VALIDHEADER |
                             SX1276_RF_LORA_IRQFLAGS_TXDONE |
                                                                /*SX1276_RF_LORA_IRQFLAGS_CADDONE |*/
                             SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL   /* |
                                                                SX1276_RF_LORA_IRQFLAGS_CADDETECTED*/
                             );

            /* DIO3 = CADDone */
            sx1276_reg_write(dev,
                             SX1276_REG_DIOMAPPING1,
                             (sx1276_reg_read(dev, SX1276_REG_DIOMAPPING1)
                              & SX1276_RF_LORA_DIOMAPPING1_DIO3_MASK) | SX1276_RF_LORA_DIOMAPPING1_DIO3_00);

            sx1276_set_status(dev,  SX1276_RF_CAD);
            sx1276_set_op_mode(dev, SX1276_RF_LORA_OPMODE_CAD);
        }
        break;
        default:
            break;
    }
}

int16_t sx1276_read_rssi(sx1276_t *dev)
{
    int16_t rssi = 0;

    switch (dev->settings.modem) {
        case SX1276_MODEM_FSK:
            rssi = -(sx1276_reg_read(dev, SX1276_REG_RSSIVALUE) >> 1);
            break;
        case SX1276_MODEM_LORA:
            if (dev->settings.channel > SX1276_RF_MID_BAND_THRESH) {
                rssi = RSSI_OFFSET_HF + sx1276_reg_read(dev, SX1276_REG_LR_RSSIVALUE);
            }
            else {
                rssi = RSSI_OFFSET_LF + sx1276_reg_read(dev, SX1276_REG_LR_RSSIVALUE);
            }
            break;
        default:
            rssi = -1;
            break;
    }

    return rssi;
}

void sx1276_reset(sx1276_t *dev)
{
    /*
     * This reset scheme is complies with 7.2 chapter of the SX1276 datasheet
     *
     * 1. Set NReset pin to LOW for at least 100 us
     * 2. Set NReset in Hi-Z state
     * 3. Wait at least 5 milliseconds
     */

    gpio_init(dev->reset_pin, GPIO_OUT);

    /* Set reset pin to 0 */
    gpio_clear(dev->reset_pin);

    /* Wait 1 ms */
    xtimer_usleep(1000);

    /* Put reset pin in High-Z */
    gpio_init(dev->reset_pin, GPIO_OD);

    gpio_set(dev->reset_pin);

    /* Wait 10 ms */
    xtimer_usleep(1000 * 10);
}

void sx1276_set_op_mode(sx1276_t *dev, uint8_t op_mode)
{
    static uint8_t op_mode_prev = 0;

    op_mode_prev = sx1276_reg_read(dev, SX1276_REG_OPMODE) & ~SX1276_RF_OPMODE_MASK;

    if (op_mode != op_mode_prev) {
        /* Replace previous mode value and setup new mode value */
        sx1276_reg_write(dev, SX1276_REG_OPMODE, (op_mode_prev & SX1276_RF_OPMODE_MASK) | op_mode);
    }
}

void sx1276_set_max_payload_len(sx1276_t *dev, sx1276_radio_modems_t modem, uint8_t maxlen)
{
    sx1276_set_modem(dev, modem);

    switch (modem) {
        case SX1276_MODEM_FSK:
            break;

        case SX1276_MODEM_LORA:
            sx1276_reg_write(dev, SX1276_REG_LR_PAYLOADMAXLENGTH, maxlen);
            break;
    }
}

/*
 * SPI Register routines
 */

void sx1276_reg_write(sx1276_t *dev, uint8_t addr, uint8_t data)
{
    sx1276_reg_write_burst(dev, addr, &data, 1);
}

uint8_t sx1276_reg_read(sx1276_t *dev, uint8_t addr)
{
    uint8_t data;

    sx1276_reg_read_burst(dev, addr, &data, 1);

    return data;
}

void sx1276_reg_write_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                            uint8_t size)
{
    unsigned int cpsr;

    spi_acquire(dev->spi);
    cpsr = irq_disable();

    gpio_clear(dev->nss_pin);
    spi_transfer_regs(dev->spi, addr | 0x80, (char *) buffer, NULL, size);
    gpio_set(dev->nss_pin);

    irq_restore(cpsr);
    spi_release(dev->spi);
}

void sx1276_reg_read_burst(sx1276_t *dev, uint8_t addr, uint8_t *buffer,
                           uint8_t size)
{
    unsigned int cpsr;

    cpsr = irq_disable();

    spi_acquire(dev->spi);

    gpio_clear(dev->nss_pin);
    spi_transfer_regs(dev->spi, addr & 0x7F, NULL, (char *) buffer, size);
    gpio_set(dev->nss_pin);

    spi_release(dev->spi);

    irq_restore(cpsr);
}

void sx1276_write_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size)
{
    sx1276_reg_write_burst(dev, 0, buffer, size);
}

void sx1276_read_fifo(sx1276_t *dev, uint8_t *buffer, uint8_t size)
{
    sx1276_reg_read_burst(dev, 0, buffer, size);
}

/**
 * IRQ handlers
 */
void sx1276_on_dio0_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 0;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

void sx1276_on_dio1_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 1;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

void sx1276_on_dio2_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 2;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

void sx1276_on_dio3_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 3;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

void sx1276_on_dio4_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 4;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

void sx1276_on_dio5_isr(void *arg)
{
    msg_t msg;

    msg.content.value = 5;
    msg_send_int(&msg, ((sx1276_t *)arg)->_internal.dio_polling_thread_pid);
}

/* Internal event handlers */

void sx1276_on_dio0(void *arg)
{
    sx1276_t *dev = (sx1276_t *) arg;

    volatile uint8_t irq_flags = 0;

    switch (dev->settings.state) {
        case SX1276_RF_RX_RUNNING:
            switch (dev->settings.modem) {
                case SX1276_MODEM_LORA:
                {
                    int8_t snr = 0;

                    /* Clear IRQ */
                    sx1276_reg_write(dev,  SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_RXDONE);

                    irq_flags = sx1276_reg_read(dev,  SX1276_REG_LR_IRQFLAGS);
                    if ((irq_flags & SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR_MASK) == SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR) {
                        sx1276_reg_write(dev,  SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_PAYLOADCRCERROR); /* Clear IRQ */

                        if (!dev->settings.lora.rx_continuous) {
                            sx1276_set_status(dev,  SX1276_RF_IDLE);
                        }

                        xtimer_remove(&dev->_internal.rx_timeout_timer);

                        send_event(dev, SX1276_RX_ERROR_CRC);

                        break;
                    }

                    sx1276_rx_packet_t *packet = &dev->_internal.last_packet;

                    packet->snr_value = sx1276_reg_read(dev,  SX1276_REG_LR_PKTSNRVALUE);
                    if (packet->snr_value & 0x80) { /* The SNR is negative */
                        /* Invert and divide by 4 */
                        snr = ((~packet->snr_value + 1) & 0xFF) >> 2;
                        snr = -snr;
                    }
                    else {
                        /* Divide by 4 */
                        snr = (packet->snr_value & 0xFF) >> 2;
                    }

                    int16_t rssi = sx1276_reg_read(dev, SX1276_REG_LR_PKTRSSIVALUE);
                    if (snr < 0) {
                        if (dev->settings.channel > SX1276_RF_MID_BAND_THRESH) {
                            packet->rssi_value = RSSI_OFFSET_HF + rssi + (rssi >> 4) + snr;
                        }
                        else {
                            packet->rssi_value = RSSI_OFFSET_LF + rssi + (rssi >> 4) + snr;
                        }
                    }
                    else {
                        if (dev->settings.channel > SX1276_RF_MID_BAND_THRESH) {
                            packet->rssi_value = RSSI_OFFSET_HF + rssi + (rssi >> 4);
                        }
                        else {
                            packet->rssi_value = RSSI_OFFSET_LF + rssi + (rssi >> 4);
                        }
                    }

                    packet->size = sx1276_reg_read(dev, SX1276_REG_LR_RXNBBYTES);

                    if (!dev->settings.lora.rx_continuous) {
                        sx1276_set_status(dev,  SX1276_RF_IDLE);
                    }

                    xtimer_remove(&dev->_internal.rx_timeout_timer);

                    /* Read the last packet from FIFO */
                    uint8_t last_rx_addr = sx1276_reg_read(dev, SX1276_REG_LR_FIFORXCURRENTADDR);
                    sx1276_reg_write(dev, SX1276_REG_LR_FIFOADDRPTR, last_rx_addr);
                    sx1276_read_fifo(dev, (uint8_t *) packet->content, packet->size);

                    /* Notify application about new packet received */
                    send_event(dev, SX1276_RX_DONE);
                }
                break;
                default:
                    break;
            }
            break;
        case SX1276_RF_TX_RUNNING:
            xtimer_remove(&dev->_internal.tx_timeout_timer);                /* Clear TX timeout timer */

            sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_TXDONE);   /* Clear IRQ */
            sx1276_set_status(dev,  SX1276_RF_IDLE);

            send_event(dev, SX1276_TX_DONE);
            break;
        default:
            break;
    }
}

void sx1276_on_dio1(void *arg)
{
    /* Get interrupt context */
    sx1276_t *dev = (sx1276_t *) arg;

    switch (dev->settings.state) {
        case SX1276_RF_RX_RUNNING:
            switch (dev->settings.modem) {
                case SX1276_MODEM_LORA:
                    xtimer_remove(&dev->_internal.rx_timeout_timer);

                    sx1276_set_status(dev,  SX1276_RF_IDLE);

                    send_event(dev, SX1276_RX_TIMEOUT);
                    break;
                default:
                    break;
            }
            break;
        case SX1276_RF_TX_RUNNING:
            break;
        default:
            break;
    }
}

void sx1276_on_dio2(void *arg)
{
    /* Get interrupt context */
    sx1276_t *dev = (sx1276_t *) arg;

    switch (dev->settings.state) {
        case SX1276_RF_RX_RUNNING:
            switch (dev->settings.modem) {
                case SX1276_MODEM_LORA:
                    if (dev->settings.lora.freq_hop_on) {
                        /* Clear IRQ */
                        sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL);

                        dev->_internal.last_channel = sx1276_reg_read(dev, SX1276_REG_LR_HOPCHANNEL) & SX1276_RF_LORA_HOPCHANNEL_CHANNEL_MASK;
                        send_event(dev, SX1276_FHSS_CHANGE_CHANNEL);
                    }

                    break;
                default:
                    break;
            }
            break;
        case SX1276_RF_TX_RUNNING:
            switch (dev->settings.modem) {
                case SX1276_MODEM_FSK:
                    break;
                case SX1276_MODEM_LORA:
                    if (dev->settings.lora.freq_hop_on) {
                        /* Clear IRQ */
                        sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_FHSSCHANGEDCHANNEL);

                        dev->_internal.last_channel = sx1276_reg_read(dev, SX1276_REG_LR_HOPCHANNEL) & SX1276_RF_LORA_HOPCHANNEL_CHANNEL_MASK;
                        send_event(dev, SX1276_FHSS_CHANGE_CHANNEL);
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void sx1276_on_dio3(void *arg)
{
    /* Get interrupt context */
    sx1276_t *dev = (sx1276_t *) arg;

    switch (dev->settings.modem) {
        case SX1276_MODEM_FSK:
            break;
        case SX1276_MODEM_LORA:
            /* Clear IRQ */
            sx1276_reg_write(dev, SX1276_REG_LR_IRQFLAGS, SX1276_RF_LORA_IRQFLAGS_CADDETECTED | SX1276_RF_LORA_IRQFLAGS_CADDONE);

            /* Send event message */
            dev->_internal.is_last_cad_success = (sx1276_reg_read(dev, SX1276_REG_LR_IRQFLAGS) & SX1276_RF_LORA_IRQFLAGS_CADDETECTED) == SX1276_RF_LORA_IRQFLAGS_CADDETECTED;
            send_event(dev, SX1276_CAD_DONE);
            break;
        default:
            break;
    }
}

/* Following interrupt lines are not used */
void sx1276_on_dio4(void *arg)
{
    (void) arg;
}

void sx1276_on_dio5(void *arg)
{
    (void) arg;
}

void *dio_polling_thread(void *arg)
{

    DEBUG("sx1276: dio polling thread started");

    sx1276_t *dev = (sx1276_t *) arg;
    msg_t msg_queue[8];
    msg_init_queue(msg_queue, 8);

    msg_t msg;

    while (1) {
        msg_receive(&msg);

        uint32_t v = msg.content.value;
        switch (v) {
            case 0:
                sx1276_on_dio0(dev);
                break;

            case 1:
                sx1276_on_dio1(dev);
                break;

            case 2:
                sx1276_on_dio2(dev);
                break;

            case 3:
                sx1276_on_dio3(dev);
                break;

            case 4:
                sx1276_on_dio4(dev);
                break;

            case 5:
                sx1276_on_dio5(dev);
                break;
        }
    }

    return NULL;
}
