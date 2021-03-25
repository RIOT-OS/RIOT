/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Common code for the ETH and PTP driver
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */
#include <string.h>

#include "mutex.h"
#include "net/netdev/eth.h"
#include "periph/gpio.h"
#include "periph/ptp.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void stm32_eth_common_init(void)
{
    /* enable APB2 clock */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    /* select RMII if necessary */
    if (eth_config.mode == RMII) {
        SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
    }

    /* initialize GPIO */
    for (int i = 0; i < (int) eth_config.mode; i++) {
        gpio_init(eth_config.pins[i], GPIO_OUT);
        gpio_init_af(eth_config.pins[i], GPIO_AF11);
    }

    /* enable all clocks */
    RCC->AHB1ENR |= (RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN |
                     RCC_AHB1ENR_ETHMACRXEN | RCC_AHB1ENR_ETHMACPTPEN);

    /* reset the peripheral */
    RCC->AHB1RSTR |= RCC_AHB1RSTR_ETHMACRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;

    /* software reset */
    ETH->DMABMR |= ETH_DMABMR_SR;
    while (ETH->DMABMR & ETH_DMABMR_SR) {}

    if (IS_USED(MODULE_PERIPH_ETH) || IS_USED(MODULE_PERIPH_PTP_TIMER)) {
        NVIC_EnableIRQ(ETH_IRQn);
    }
}

#if IS_USED(MODULE_STM32_ETH) || IS_USED(MODULE_PERIPH_PTP_TIMER)
void isr_eth(void)
{
    DEBUG("[periph_eth_common] isr_eth()\n");

    if (IS_USED(MODULE_PERIPH_PTP_TIMER)) {
        if (ETH->MACSR & ETH_MACSR_TSTS) {
            /* clear interrupt by reading PTPTSSR */
            (void)ETH->PTPTSSR;
            ptp_timer_cb();
        }
    }

    if (IS_USED(MODULE_STM32_ETH)) {
        extern netdev_t *stm32_eth_netdev;
        extern mutex_t stm32_eth_tx_completed;
        unsigned tmp = ETH->DMASR;

        if ((tmp & ETH_DMASR_TS)) {
            ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_TS;
            DEBUG("isr_eth: TX completed\n");
            mutex_unlock(&stm32_eth_tx_completed);
        }

        if ((tmp & ETH_DMASR_RS)) {
            ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_RS;
            DEBUG("isr_eth: RX completed\n");
            if (stm32_eth_netdev) {
                netdev_trigger_event_isr(stm32_eth_netdev);
            }
        }
    }

    cortexm_isr_end();
}
#endif
