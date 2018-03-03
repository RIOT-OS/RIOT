/*
 * Copyright (C) 2016-2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_dac
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
#if defined(DAC_COUNT) && DAC_COUNT > 0
#include "em_dac.h"
#endif

int8_t dac_init(dac_t line)
{
    /* check if device is valid */
    if (line >= DAC_NUMOF) {
        return -1;
    }

    uint8_t dev = dac_channel_config[line].dev;

    /* enable clock */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(dac_config[dev].cmu, true);

    /* reset and initialize peripheral */
    DAC_Init_TypeDef init = DAC_INIT_DEFAULT;

    DAC_Reset(dac_config[dev].dev);
    DAC_Init(dac_config[dev].dev, &init);

    /* initialize channel */
    DAC_InitChannel_TypeDef init_channel = DAC_INITCHANNEL_DEFAULT;

    init_channel.enable = true;

    DAC_InitChannel(dac_config[dev].dev,
                    &init_channel,
                    dac_channel_config[line].index);

    return 0;
}

void dac_set(dac_t line, uint16_t value)
{
    uint8_t dev = dac_channel_config[line].dev;

    DAC_ChannelOutputSet(dac_config[dev].dev,
                         dac_channel_config[line].index,
                         value & 0xfff);
}

void dac_poweron(dac_t line)
{
    uint8_t dev = dac_channel_config[line].dev;

    CMU_ClockEnable(dac_config[dev].cmu, true);
}

void dac_poweroff(dac_t line)
{
    uint8_t dev = dac_channel_config[line].dev;

    CMU_ClockEnable(dac_config[dev].cmu, false);
}
