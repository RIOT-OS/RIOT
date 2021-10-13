/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       Glue code for running uwb-core for RIOT
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "dpl/dpl.h"
#include "dw1000/dw1000_phy.h"
#include "dw1000/dw1000_hal.h"
#include "uwb_dw1000.h"
#include "uwb_dw1000_config.h"

#include "uwb/uwb.h"
#include "uwb/uwb_mac.h"

#include "utlist.h"
#include "thread.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_NONE
#endif
#include "log.h"

#ifndef DW1000_THREAD_PRIORITY
#define DW1000_THREAD_PRIORITY  MYNEWT_VAL_UWB_DEV_TASK_PRIO
#endif

/* Link list head */
static struct _dw1000_dev_instance_t * dw1000_instance_head;

/* Default instance configuration */
static const dw1000_dev_instance_t dw1000_instance_config_default = {
    .uwb_dev = {
        .idx = 0,
        .role = DW1000_ROLE_DEFAULT,
        .task_prio = DW1000_THREAD_PRIORITY,
        .status = {0},
        .rx_ant_separation = DW1000_RX_ANTSEP_DEFAULT,
        .attrib = {
            .nsfd = DW1000_NSFD_DEFAULT,
            .nsync = DW1000_NSYNC_DEFAULT,
            .nphr = DW1000_NPHR_DEFAULT ,
        },
        .config = {
            .channel = DW1000_CHANNEL_DEFAULT,
            .prf = DW1000_PRF_DEFAULT,
            .dataRate = DW1000_DATARATE_DEFAULT,
            .rx = {
                .pacLength = DW1000_PACLEN_DEFAULT,
                .preambleCodeIndex = DW1000_RX_PREAM_CIDX_DEFAULT,
                .sfdType = DW1000_RX_SFD_TYPE_DEFAULT,
                .phrMode = DW1000_RX_PHR_MODE_DEFAULT,
                .sfdTimeout = DW1000_RX_SFD_TO_DEFAULT,
                .timeToRxStable = DW1000_RX_STABLE_TIME_US ,
                .frameFilter = DW1000_FRAME_FILTER_DEFAULT,
                .xtalTrim = DW1000_XTAL_TRIM_DEFAULT,
            },
            .tx ={
                .preambleCodeIndex = DW1000_TX_PREAM_CIDX_DEAULT,
                .preambleLength = DW1000_TX_PREAM_LEN_DEFAULT,
            },
            .txrf={
                .PGdly = DW1000_CHANNEL_DEFAULT,
                .BOOSTNORM = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP500 = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP250 = dw1000_power_value(DW1000_txrf_config_9db, 2.5),
                .BOOSTP125 = dw1000_power_value(DW1000_txrf_config_9db, 2.5)
            },
            .trxoff_enable = DW1000_TRXOFF_ENABLE,
            .rxdiag_enable = DW1000_RX_DIAGNOSTIC,
            .dblbuffon_enabled = DW1000_DOUBLE_BUFFER_ENABLE,
            .LDE_enable = DW1000_LDE_ENABLE,
            .LDO_enable = DW1000_LDO_ENABLE,
            .sleep_enable =  DW1000_SLEEP_ENABLE,
            .wakeup_rx_enable = DW1000_WAKEUP_RX_ENABLE,
            .rxauto_enable = DW1000_RX_AUTO_ENABLE,
            .cir_enable = 0,             /**< Default behavior for CIR interface */
            .cir_pdoa_slave = 0,         /**< First instance should not act as pdoa slave */
            .blocking_spi_transfers = 1, /**< Nonblocking spi transfers are not supported */
        },
    }
};

void _uwb_dw1000_set_idx(dw1000_dev_instance_t* dev)
{
    int count = 0;
    dw1000_dev_instance_t *elt = NULL;
    LL_COUNT(dw1000_instance_head, elt, count);
    dev->uwb_dev.idx = count++;
    /* prepend to list */
    LL_PREPEND(dw1000_instance_head, dev);
}

void uwb_dw1000_setup(dw1000_dev_instance_t* dev, dw1000_params_t* params)
{
    /* set semaphore */
    dpl_sem_init(params->spi_sem, 0x1);
    /* set default uwb config */
    memcpy(dev, &dw1000_instance_config_default,
           sizeof(dw1000_dev_instance_t));
    /* set uwb_dev idx */
    _uwb_dw1000_set_idx(dev);
    /* this will set the configuration and init uwb_dev which ATM only
       allocates an RX and TX buffer if none is yet available */
    dw1000_dev_init((struct os_dev *) dev, (void *) params);
}

void uwb_dw1000_config_and_start(dw1000_dev_instance_t* dev)
{
    dw1000_dev_config(dev);
}

void uwb_dw1000_set_buffs(dw1000_dev_instance_t* dev, uint8_t* tx_buf,
                          uint8_t* rx_buf)
{
    dev->uwb_dev.rxbuf = rx_buf;
    dev->uwb_dev.txbuf = tx_buf;
}

void uwb_dw1000_init(void)
{
    dw1000_instance_head = NULL;
}

struct uwb_dev* uwb_dev_idx_lookup(int idx)
{
    dw1000_dev_instance_t *current = dw1000_instance_head;

    while (current) {
        if (current->uwb_dev.idx == idx) {
            LOG_DEBUG("uwb_dev: found dev of idx %d\n", idx);
            break;
        }
        current = current->next;
    }

    return (struct uwb_dev*) &current->uwb_dev;
}

/**
 * API to choose DW1000 instances based on parameters.
 *
 * @param idx  Indicates number of instances for the chosen bsp.
 * @return dw1000_dev_instance_t
 */

struct _dw1000_dev_instance_t * hal_dw1000_inst(uint8_t idx)
{
    dw1000_dev_instance_t *current = dw1000_instance_head;

    for (uint8_t i = 0; i < idx; i++) {
        current = current->next;
    }

    return current;
}
