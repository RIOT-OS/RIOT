/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_common
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph_conf.h"
#include "periph/dac.h"

#include "em_cmu.h"

#ifdef _SILICON_LABS_32B_PLATFORM_1
#include "em_dac.h"
#endif

/* guard file in case no DAC device is defined */
#if DAC_NUMOF

typedef struct {
    uint32_t shift;
    uint32_t max_value;
} dac_state_t;

static dac_state_t dac_state[DAC_NUMOF];

int8_t dac_init(dac_t dev, dac_precision_t precision)
{
    /* check if device is valid */
    if (dev >= DAC_NUMOF) {
        return -1;
    }

    /* store precision, because it is used during scan, not in init */
    switch (precision) {
        case DAC_RES_6BIT:
            dac_state[dev].shift = 6;
            dac_state[dev].max_value = (1 << 6) - 1;
            break;
        case DAC_RES_8BIT:
            dac_state[dev].shift = 4;
            dac_state[dev].max_value = (1 << 8) - 1;
            break;
        case DAC_RES_10BIT:
            dac_state[dev].shift = 2;
            dac_state[dev].max_value = (1 << 12) - 1;
            break;
        case DAC_RES_12BIT:
            dac_state[dev].shift = 0;
            dac_state[dev].max_value = (1 << 12) - 1;
            break;
        default:
            /* not supported */
            return -2;
    }

    /* enable clock */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(dac_config[dev].cmu, true);

    /* initialize device */
    DAC_Init_TypeDef init = DAC_INIT_DEFAULT;

    DAC_Reset(dac_config[dev].dev);
    DAC_Init(dac_config[dev].dev, &init);

    /* initialize channels */
    DAC_InitChannel_TypeDef channelInit = DAC_INITCHANNEL_DEFAULT;

    channelInit.enable = true;

    for (int i = 0; i < dac_config[dev].channels; i++) {
        uint8_t index = dac_config[dev].channel_offset + i;

        DAC_InitChannel(dac_config[dev].dev, &channelInit,
                        dac_channel_config[index].index);
    }

    return 0;
}

int8_t dac_write(dac_t dev, uint8_t channel, uint16_t value)
{
    /* check if channel is valid */
    if (channel >= dac_config[dev].channels) {
        return -2;
    }

    /* check if value is in range */
    if (value > dac_state[dev].max_value) {
        return -3;
    }

    /* setup channel */
    uint8_t index = dac_config[dev].channel_offset + channel;

    DAC_ChannelOutputSet(dac_config[dev].dev, dac_channel_config[index].index,
                         value << dac_state[dev].shift);

    return 0;
}

int8_t dac_poweron(dac_t dev)
{
    CMU_ClockEnable(dac_config[dev].cmu, true);

    return 0;
}

int8_t dac_poweroff(dac_t dev)
{
    CMU_ClockEnable(dac_config[dev].cmu, false);

    return 0;
}

uint16_t dac_map(dac_t dev, int value, int min, int max)
{
    return ((value - min) * dac_state[dev].max_value) / (max - min);
}

uint16_t dac_mapf(dac_t dev, float value, float min, float max)
{
    return ((value - min) * dac_state[dev].max_value) / (max - min);
}

#endif /* DAC_NUMOF */
