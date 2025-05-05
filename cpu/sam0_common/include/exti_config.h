/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_sam0_common
 * @brief           Generic EXTI map for all CPUs of the sam0 family.
 * @{
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author          Derek Hageman <hageman@inthat.cloud>
 */

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[PORT_GROUPS][32] = {
#if PORT_GROUPS >= 1
{
#  ifdef PIN_PA00A_EIC_EXTINT_NUM
    PIN_PA00A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA01A_EIC_EXTINT_NUM
    PIN_PA01A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA02A_EIC_EXTINT_NUM
    PIN_PA02A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA03A_EIC_EXTINT_NUM
    PIN_PA03A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA04A_EIC_EXTINT_NUM
    PIN_PA04A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA05A_EIC_EXTINT_NUM
    PIN_PA05A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA06A_EIC_EXTINT_NUM
    PIN_PA06A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA07A_EIC_EXTINT_NUM
    PIN_PA07A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA08A_EIC_EXTINT_NUM
    PIN_PA08A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA09A_EIC_EXTINT_NUM
    PIN_PA09A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA10A_EIC_EXTINT_NUM
    PIN_PA10A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA11A_EIC_EXTINT_NUM
    PIN_PA11A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA12A_EIC_EXTINT_NUM
    PIN_PA12A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA13A_EIC_EXTINT_NUM
    PIN_PA13A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA14A_EIC_EXTINT_NUM
    PIN_PA14A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA15A_EIC_EXTINT_NUM
    PIN_PA15A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA16A_EIC_EXTINT_NUM
    PIN_PA16A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA17A_EIC_EXTINT_NUM
    PIN_PA17A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA18A_EIC_EXTINT_NUM
    PIN_PA18A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA19A_EIC_EXTINT_NUM
    PIN_PA19A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA20A_EIC_EXTINT_NUM
    PIN_PA20A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA21A_EIC_EXTINT_NUM
    PIN_PA21A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA22A_EIC_EXTINT_NUM
    PIN_PA22A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA23A_EIC_EXTINT_NUM
    PIN_PA23A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA24A_EIC_EXTINT_NUM
    PIN_PA24A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA25A_EIC_EXTINT_NUM
    PIN_PA25A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA26A_EIC_EXTINT_NUM
    PIN_PA26A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA27A_EIC_EXTINT_NUM
    PIN_PA27A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA28A_EIC_EXTINT_NUM
    PIN_PA28A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA29A_EIC_EXTINT_NUM
    PIN_PA29A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA30A_EIC_EXTINT_NUM
    PIN_PA30A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PA31A_EIC_EXTINT_NUM
    PIN_PA31A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
},
#endif

#if PORT_GROUPS >= 2
{
#  ifdef PIN_PB00A_EIC_EXTINT_NUM
    PIN_PB00A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB01A_EIC_EXTINT_NUM
    PIN_PB01A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB02A_EIC_EXTINT_NUM
    PIN_PB02A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB03A_EIC_EXTINT_NUM
    PIN_PB03A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB04A_EIC_EXTINT_NUM
    PIN_PB04A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB05A_EIC_EXTINT_NUM
    PIN_PB05A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB06A_EIC_EXTINT_NUM
    PIN_PB06A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB07A_EIC_EXTINT_NUM
    PIN_PB07A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB08A_EIC_EXTINT_NUM
    PIN_PB08A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB09A_EIC_EXTINT_NUM
    PIN_PB09A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB10A_EIC_EXTINT_NUM
    PIN_PB10A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB11A_EIC_EXTINT_NUM
    PIN_PB11A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB12A_EIC_EXTINT_NUM
    PIN_PB12A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB13A_EIC_EXTINT_NUM
    PIN_PB13A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB14A_EIC_EXTINT_NUM
    PIN_PB14A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB15A_EIC_EXTINT_NUM
    PIN_PB15A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB16A_EIC_EXTINT_NUM
    PIN_PB16A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB17A_EIC_EXTINT_NUM
    PIN_PB17A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB18A_EIC_EXTINT_NUM
    PIN_PB18A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB19A_EIC_EXTINT_NUM
    PIN_PB19A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB20A_EIC_EXTINT_NUM
    PIN_PB20A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB21A_EIC_EXTINT_NUM
    PIN_PB21A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB22A_EIC_EXTINT_NUM
    PIN_PB22A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB23A_EIC_EXTINT_NUM
    PIN_PB23A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB24A_EIC_EXTINT_NUM
    PIN_PB24A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB25A_EIC_EXTINT_NUM
    PIN_PB25A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB26A_EIC_EXTINT_NUM
    PIN_PB26A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB27A_EIC_EXTINT_NUM
    PIN_PB27A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB28A_EIC_EXTINT_NUM
    PIN_PB28A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB29A_EIC_EXTINT_NUM
    PIN_PB29A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB30A_EIC_EXTINT_NUM
    PIN_PB30A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PB31A_EIC_EXTINT_NUM
    PIN_PB31A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
},
#endif

#if PORT_GROUPS >= 3
{
#  ifdef PIN_PC00A_EIC_EXTINT_NUM
    PIN_PC00A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC01A_EIC_EXTINT_NUM
    PIN_PC01A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC02A_EIC_EXTINT_NUM
    PIN_PC02A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC03A_EIC_EXTINT_NUM
    PIN_PC03A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC04A_EIC_EXTINT_NUM
    PIN_PC04A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC05A_EIC_EXTINT_NUM
    PIN_PC05A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC06A_EIC_EXTINT_NUM
    PIN_PC06A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC07A_EIC_EXTINT_NUM
    PIN_PC07A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC08A_EIC_EXTINT_NUM
    PIN_PC08A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC09A_EIC_EXTINT_NUM
    PIN_PC09A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC10A_EIC_EXTINT_NUM
    PIN_PC10A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC11A_EIC_EXTINT_NUM
    PIN_PC11A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC12A_EIC_EXTINT_NUM
    PIN_PC12A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC13A_EIC_EXTINT_NUM
    PIN_PC13A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC14A_EIC_EXTINT_NUM
    PIN_PC14A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC15A_EIC_EXTINT_NUM
    PIN_PC15A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC16A_EIC_EXTINT_NUM
    PIN_PC16A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC17A_EIC_EXTINT_NUM
    PIN_PC17A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC18A_EIC_EXTINT_NUM
    PIN_PC18A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC19A_EIC_EXTINT_NUM
    PIN_PC19A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC20A_EIC_EXTINT_NUM
    PIN_PC20A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC21A_EIC_EXTINT_NUM
    PIN_PC21A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC22A_EIC_EXTINT_NUM
    PIN_PC22A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC23A_EIC_EXTINT_NUM
    PIN_PC23A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC24A_EIC_EXTINT_NUM
    PIN_PC24A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC25A_EIC_EXTINT_NUM
    PIN_PC25A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC26A_EIC_EXTINT_NUM
    PIN_PC26A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC27A_EIC_EXTINT_NUM
    PIN_PC27A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC28A_EIC_EXTINT_NUM
    PIN_PC28A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC29A_EIC_EXTINT_NUM
    PIN_PC29A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC30A_EIC_EXTINT_NUM
    PIN_PC30A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PC31A_EIC_EXTINT_NUM
    PIN_PC31A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
},
#endif

#if PORT_GROUPS >= 4
{
#  ifdef PIN_PD00A_EIC_EXTINT_NUM
    PIN_PD00A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD01A_EIC_EXTINT_NUM
    PIN_PD01A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD02A_EIC_EXTINT_NUM
    PIN_PD02A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD03A_EIC_EXTINT_NUM
    PIN_PD03A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD04A_EIC_EXTINT_NUM
    PIN_PD04A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD05A_EIC_EXTINT_NUM
    PIN_PD05A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD06A_EIC_EXTINT_NUM
    PIN_PD06A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD07A_EIC_EXTINT_NUM
    PIN_PD07A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD08A_EIC_EXTINT_NUM
    PIN_PD08A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD09A_EIC_EXTINT_NUM
    PIN_PD09A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD10A_EIC_EXTINT_NUM
    PIN_PD10A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD11A_EIC_EXTINT_NUM
    PIN_PD11A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD12A_EIC_EXTINT_NUM
    PIN_PD12A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD13A_EIC_EXTINT_NUM
    PIN_PD13A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD14A_EIC_EXTINT_NUM
    PIN_PD14A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD15A_EIC_EXTINT_NUM
    PIN_PD15A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD16A_EIC_EXTINT_NUM
    PIN_PD16A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD17A_EIC_EXTINT_NUM
    PIN_PD17A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD18A_EIC_EXTINT_NUM
    PIN_PD18A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD19A_EIC_EXTINT_NUM
    PIN_PD19A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD20A_EIC_EXTINT_NUM
    PIN_PD20A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD21A_EIC_EXTINT_NUM
    PIN_PD21A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD22A_EIC_EXTINT_NUM
    PIN_PD22A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD23A_EIC_EXTINT_NUM
    PIN_PD23A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD24A_EIC_EXTINT_NUM
    PIN_PD24A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD25A_EIC_EXTINT_NUM
    PIN_PD25A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD26A_EIC_EXTINT_NUM
    PIN_PD26A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD27A_EIC_EXTINT_NUM
    PIN_PD27A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD28A_EIC_EXTINT_NUM
    PIN_PD28A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD29A_EIC_EXTINT_NUM
    PIN_PD29A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD30A_EIC_EXTINT_NUM
    PIN_PD30A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
#  ifdef PIN_PD31A_EIC_EXTINT_NUM
    PIN_PD31A_EIC_EXTINT_NUM,
#  else
    -1,
#  endif
},
#endif
};

#ifdef __cplusplus
}
#endif

/** @} */
