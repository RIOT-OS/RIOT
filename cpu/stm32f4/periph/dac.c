/*
 * Copyright (C) 2014 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Simon Brummer<simon.brummer@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/dac.h"
#include "periph_conf.h"

/* guard in case that no DAC device is defined */
#if DAC_NUMOF

#define DAC_MAX_12BIT 0x0fff

typedef struct {
    uint8_t shift_mod;
} dac_config_t;

dac_config_t dac_config[DAC_NUMOF];

int8_t dac_init(dac_t dev, dac_precision_t precision)
{
    DAC_TypeDef *dac = 0;
    dac_poweron(dev);

    switch (dev) {
#if DAC_0_EN
        case DAC_0:
            dac = DAC_0_DEV;
            DAC_0_PORT_CLKEN();
            /* Set Mode to analoge out, disable Pullup Pulldown Resistors for both channels */
            DAC_0_PORT->MODER |=  (3 << (DAC_0_CH0_PIN * 2) | 3 << (DAC_0_CH1_PIN * 2));
            DAC_0_PORT->PUPDR &= ~(3 << (DAC_0_CH0_PIN * 2) | 3 << (DAC_0_CH1_PIN * 2));
            break;
#endif
        default:
            /* Unknown Device */
            return -1;
    }

    /* Select Shift value to normalize given Value */
    switch(precision) {
        case DAC_RES_6BIT:
            dac_config[dev].shift_mod = 0x06;       /* 2^6  << 6 = 2^12 */
            break;
        case DAC_RES_8BIT:
            dac_config[dev].shift_mod = 0x04;       /* 2^8  << 4 = 2^12 */
            break;
        case DAC_RES_10BIT:
            dac_config[dev].shift_mod = 0x02;       /* 2^10 << 2 = 2^12 */
            break;
        case DAC_RES_12BIT:
            dac_config[dev].shift_mod = 0x00;       /* 2^12 << 0 = 2^12 */
            break;
        /* Not Supported Resolutions */
        case DAC_RES_14BIT:
        case DAC_RES_16BIT:
        default:
            dac_poweroff(dev);
            return -2;
        break;
    }

    /* Enable Channels, Clear Output */
    dac->CR = 0;
    dac->CR |= (DAC_CR_EN1 | DAC_CR_EN2);
    dac->DHR12R1 = 0;
    dac->DHR12R2 = 0;

    return 0;
}

int8_t dac_write(dac_t dev, uint8_t channel, uint16_t value)
{
    DAC_TypeDef* __attribute__((unused))  dac = 0;
    uint16_t     __attribute__((unused))  val = value << dac_config[dev].shift_mod;

    switch(dev){
#if DAC_0_EN
    case DAC_0:
        dac = DAC_0_DEV;

        if( DAC_MAX_12BIT < val ){
            /* Value out of Range */
            return -3;
        }

        switch(channel){
            case 0:
                dac->DHR12R1 = val;
                break;
            case 1:
                dac->DHR12R2 = val;
                break;
            /* Invalid Channel */
            default:
                return -2;
        }
        break;
#endif
        /* Unknown  Device */
        default:
            return -1;
    }
    return 0;
}

int8_t dac_poweron(dac_t dev)
{
    switch (dev){
#if DAC_0_EN
        case DAC_0:
            DAC_0_CLKEN();
            break;
#endif
        default:
            /* Unknown Device */
            return -1;
    }
    return 0;
}

int8_t dac_poweroff(dac_t dev)
{
    switch (dev) {
#if DAC_0_EN
        case DAC_0:
            DAC_0_CLKDIS();
            break;
#endif
        default:
            /* Unknown Device */
            return -1;
    }
    return 0;
}

uint16_t dac_map(dac_t dev, int value, int min, int max)
{
    return dac_mapf(dev, (int) value, (int) min, (int) max);
}

uint16_t dac_mapf(dac_t dev, float value, float min, float max)
{
    uint16_t val_12_bit = ((value - min) * DAC_MAX_12BIT)/(max-min);
    return val_12_bit >> dac_config[dev].shift_mod;
}

#undef DAC_MAX_12BIT

#endif /* DAC_NUMOF */
