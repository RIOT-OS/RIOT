/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2, CC13x2 RF driver
 */

#ifndef CC26X2_CC13X2_RF_COMMANDS_H
#define CC26X2_CC13X2_RF_COMMANDS_H

#include <ti/drivers/rf/RF.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   RF propietary mode configuration
 */
extern RF_Mode rf_prop_mode;

/**
 * @brief   CMD_PROP_RADIO_DIV_SETUP command
 */
extern rfc_CMD_PROP_RADIO_DIV_SETUP_t rf_cmd_prop_radio_div_setup;

/**
 * @brief   CMD_PROP_RADIO_DIV_SETUP command
 */
extern rfc_CMD_FS_t rf_cmd_prop_fs;

/**
 * @brief   CMD_PROP_TX_ADV command
 */
extern rfc_CMD_PROP_TX_ADV_t rf_cmd_prop_tx_adv;

/**
 * @brief   CMD_PROP_RX_ADV command
 */
extern rfc_CMD_PROP_RX_ADV_t rf_cmd_prop_rx_adv;

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26X2_CC13X2_RF_COMMANDS_H */
