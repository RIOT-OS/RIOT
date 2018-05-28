/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 * @file
 * @brief       Basic functionality of cc110x driver
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include "luid.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "xtimer.h"
#include "cpu.h"
#include "log.h"

#include "cc110x.h"
#include "cc110x-defaultsettings.h"
#include "cc110x-defines.h"
#include "cc110x-interface.h"
#include "cc110x-internal.h"
#include "cc110x-spi.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* Internal function prototypes */
#ifndef CC110X_DONT_RESET
static void _reset(cc110x_t *dev);
static void _power_up_reset(cc110x_t *dev);
#endif

int cc110x_setup(cc110x_t *dev, const cc110x_params_t *params)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

#ifdef MODULE_CC110X_HOOKS
    cc110x_hooks_init();
#endif

    dev->params = *params;

    /* Configure chip-select */
    spi_init_cs(dev->params.spi, dev->params.cs);

    /* Configure GDO1 */
    gpio_init(dev->params.gdo1, GPIO_IN);

#ifndef CC110X_DONT_RESET
    /* reset device*/
    _power_up_reset(dev);
#endif

    /* set default state */
    dev->radio_state = RADIO_IDLE;

    /* Write configuration to configuration registers */
    cc110x_writeburst_reg(dev, 0x00, cc110x_default_conf, cc110x_default_conf_size);

    /* Write PATABLE (power settings) */
    cc110x_writeburst_reg(dev, CC110X_PATABLE, CC110X_DEFAULT_PATABLE, 8);

    /* set base frequency */
    cc110x_set_base_freq_raw(dev, CC110X_DEFAULT_FREQ);

    /* Set default channel number */
    cc110x_set_channel(dev, CC110X_DEFAULT_CHANNEL);

    /* set default node id */
    uint8_t addr;
    luid_get(&addr, 1);
    cc110x_set_address(dev, addr);

    LOG_INFO("cc110x: initialized with address=%u and channel=%i\n",
            (unsigned)dev->radio_address,
            dev->radio_channel);

    return 0;
}

uint8_t cc110x_set_address(cc110x_t *dev, uint8_t address)
{
    DEBUG("%s:%s:%u setting address %u\n", RIOT_FILE_RELATIVE, __func__,
            __LINE__, (unsigned)address);
    if (!(address < MIN_UID)) {
        if (dev->radio_state != RADIO_UNKNOWN) {
            cc110x_write_register(dev, CC110X_ADDR, address);
            dev->radio_address = address;
            return address;
        }
    }

    return 0;
}

void cc110x_set_base_freq_raw(cc110x_t *dev, const char* freq_array)
{
#if ENABLE_DEBUG == 1
    uint8_t _tmp[] = { freq_array[2], freq_array[1], freq_array[0], 0x00};
    uint32_t *FREQ = (uint32_t*) _tmp;

    DEBUG("cc110x_set_base_freq_raw(): setting base frequency to %uHz\n",
            (26000000>>16) * (unsigned)(*FREQ));
#endif
    cc110x_writeburst_reg(dev, CC110X_FREQ2, freq_array, 3);
}

void cc110x_set_monitor(cc110x_t *dev, uint8_t mode)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    cc110x_write_register(dev, CC110X_PKTCTRL1, mode ? 0x04 : 0x06);
}

void cc110x_setup_rx_mode(cc110x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    /* Stay in RX mode until end of packet */
    cc110x_write_reg(dev, CC110X_MCSM2, 0x07);
    cc110x_switch_to_rx(dev);
}

void cc110x_switch_to_rx(cc110x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

#ifdef MODULE_CC110X_HOOKS
    cc110x_hook_rx();
#endif

    gpio_irq_disable(dev->params.gdo2);

    /* flush RX fifo */
    cc110x_strobe(dev, CC110X_SIDLE);
    cc110x_strobe(dev, CC110X_SFRX);

    dev->radio_state = RADIO_RX;

    cc110x_write_reg(dev, CC110X_IOCFG2, CC110X_GDO_HIGH_ON_SYNC_WORD);
    cc110x_strobe(dev, CC110X_SRX);

    gpio_irq_enable(dev->params.gdo2);
}

void cc110x_wakeup_from_rx(cc110x_t *dev)
{
    if (dev->radio_state != RADIO_RX) {
        return;
    }

    LOG_DEBUG("cc110x: switching to idle mode\n");

    cc110x_strobe(dev, CC110X_SIDLE);
    dev->radio_state = RADIO_IDLE;
}

void cc110x_switch_to_pwd(cc110x_t *dev)
{
    LOG_DEBUG("cc110x: switching to powerdown mode\n");
    cc110x_wakeup_from_rx(dev);
    cc110x_strobe(dev, CC110X_SPWD);
    dev->radio_state = RADIO_PWD;

#ifdef MODULE_CC110X_HOOKS
     cc110x_hook_off();
#endif
}

#ifndef MODULE_CC110X_HOOKS
int16_t cc110x_set_channel(cc110x_t *dev, uint8_t channr)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    if (channr > MAX_CHANNR) {
        return -1;
    }

    cc110x_write_register(dev, CC110X_CHANNR, channr * 10);
    dev->radio_channel = channr;

    return channr;
}
#endif

#ifndef CC110X_DONT_RESET
static void _reset(cc110x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    cc110x_wakeup_from_rx(dev);
    cc110x_cs(dev);
    cc110x_strobe(dev, CC110X_SRES);
    xtimer_usleep(100);
}

static void _power_up_reset(cc110x_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_set(dev->params.cs);
    gpio_clear(dev->params.cs);
    gpio_set(dev->params.cs);
    xtimer_usleep(RESET_WAIT_TIME);
    _reset(dev);
}
#endif

void cc110x_write_register(cc110x_t *dev, uint8_t r, uint8_t value)
{
    /* Save old radio state */
    uint8_t old_state = dev->radio_state;

    /* Wake up from RX (no effect if in other mode) */
    cc110x_wakeup_from_rx(dev);
    cc110x_write_reg(dev, r, value);

    /* Have to put radio back to RX if old radio state
     * was RX, otherwise no action is necessary */
    if (old_state == RADIO_RX) {
        cc110x_switch_to_rx(dev);
    }
}

int cc110x_rd_set_mode(cc110x_t *dev, int mode)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    int result;

    /* Get current radio mode */
    if ((dev->radio_state == RADIO_UNKNOWN) || (dev->radio_state == RADIO_PWD)) {
        result = RADIO_MODE_OFF;
    }
    else {
        result = RADIO_MODE_ON;
    }

    switch(mode) {
        case RADIO_MODE_ON:
            LOG_DEBUG("cc110x: switching to RX mode\n");
            cc110x_setup_rx_mode(dev);          /* Set chip to desired mode */
            break;

        case RADIO_MODE_OFF:
            gpio_irq_disable(dev->params.gdo2); /* Disable interrupts */
            cc110x_switch_to_pwd(dev);          /* Set chip to power down mode */
            break;

        case RADIO_MODE_GET:
            /* do nothing, just return current mode */
        default:
            /* do nothing */
            break;
    }

    /* Return previous mode */
    return result;
}
