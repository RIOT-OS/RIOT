/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_nrf_modem
 * @{
 *
 * @file
 * @brief       Hardware Abstraction Layer which implement some functions
 *              provided by nRF SDK as the modem library rely on them
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "nrf_modem_os.h"
#include "nrf_errno.h"
#include "nrf_modem_platform.h"
#include "nrf_modem_limits.h"

#include <errno.h>

/* RIOT-OS specific headers */
#include "nrf_modem_riot.h"
#include "nrf_modem_hal.h"
#include "cpu.h"

/* nRF Modem relies on Nordic SDK for IPC management, thus emulates it somehow */

typedef struct {
    nrfx_ipc_handler_t handler;
    uint32_t   state;
    void *             p_context;
} ipc_control_block_t;

static ipc_control_block_t m_ipc_cb;

nrfx_err_t nrfx_ipc_init(uint8_t irq_priority, nrfx_ipc_handler_t handler, void * p_context)
{
    m_ipc_cb.handler = handler;
    m_ipc_cb.p_context = p_context;
    /* Prepare & enable IPC interrupt */
    NVIC_SetPriority(IPC_IRQn, irq_priority);
    NVIC_EnableIRQ(IPC_IRQn);
    return NRFX_SUCCESS;
}

void nrfx_ipc_uninit(void)
{
    printf("[nrf_modem_hal]:%s\n", __FUNCTION__);
}

void nrfx_ipc_config_load(const nrfx_ipc_config_t * p_config)
{
    for (unsigned i = 0; i < IPC_CONF_NUM; ++i)
    {
        NRF_IPC_NS->SEND_CNF[i] = p_config->send_task_config[i];
    }

    for (unsigned i = 0; i < IPC_CONF_NUM; ++i)
    {
        NRF_IPC_NS->RECEIVE_CNF[i] = p_config->receive_event_config[i];
    }

    NRF_IPC_NS->INTENSET = p_config->receive_events_enabled;
}

void nrfx_ipc_receive_event_enable(uint8_t event_index)
{
    NRF_IPC_NS->INTENSET = 1 << event_index;
}

void nrfx_ipc_receive_event_disable(uint8_t event_index)
{
    NRF_IPC_NS->INTENCLR = 1 << event_index;
}
void nrfx_ipc_receive_event_group_enable(uint32_t event_bitmask)
{
    NRF_IPC_NS->INTENSET = event_bitmask;
}
void nrfx_ipc_receive_event_group_disable(uint32_t event_bitmask)
{
    NRF_IPC_NS->INTENCLR = event_bitmask;
}

void isr_ipc(void) {
    /* Clear events */
    uint32_t events = NRF_IPC_NS->INTPEND;
    while (NRF_IPC_NS->INTPEND)
    {
        for (uint8_t cpt=0; cpt<8; cpt++) {
            if (events & (1 << cpt)) {
                NRF_IPC_NS->EVENTS_RECEIVE[cpt] = 0;
            }
        }

        m_ipc_cb.handler(events, m_ipc_cb.p_context);
    }
    cortexm_isr_end();

}
