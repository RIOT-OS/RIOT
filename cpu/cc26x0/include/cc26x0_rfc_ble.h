/*
 * Copyright (C) 2016 Leon George, Florent-Valéry Coen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_rfc_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 RFC BLE definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 * @author          Florent-Valéry Coen <florent.coen@gmail.com>
 */

#ifndef CC26x0_RFC_BLE_H
#define CC26x0_RFC_BLE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup cc2x0_rop_command_ids
 * @{
 */
/* BLE radio operation commands */
#define CMDR_CMDID_BLE_SLAVE      0x1801
#define CMDR_CMDID_BLE_MASTER     0x1802
#define CMDR_CMDID_BLE_ADV        0x1803
#define CMDR_CMDID_BLE_ADV_DIR    0x1804
#define CMDR_CMDID_BLE_ADV_NC     0x1805
#define CMDR_CMDID_BLE_ADV_SCAN   0x1806
#define CMDR_CMDID_BLE_SCANNER    0x1807
#define CMDR_CMDID_BLE_INITIATOR  0x1808
#define CMDR_CMDID_BLE_GENERIC_RX 0x1809
#define CMDR_CMDID_BLE_TX_TEST    0x180A
/* BLE immediate commands */
#define CMDR_CMDID_BLE_ADV_PAYLOAD    0x1001
/**@}*/

/**
 * @addtogroup cc26x0_rop_status_codes
 * @{
 */
/* operation finished normally */
#define R_OP_STATUS_BLE_DONE_OK         0x1400
#define R_OP_STATUS_BLE_DONE_RXTIMEOUT  0x1401
#define R_OP_STATUS_BLE_DONE_NOSYNC     0x1402
#define R_OP_STATUS_BLE_DONE_RXERR      0x1403
#define R_OP_STATUS_BLE_DONE_CONNECT    0x1404
#define R_OP_STATUS_BLE_DONE_MAXNACK    0x1405
#define R_OP_STATUS_BLE_DONE_ENDED      0x1406
#define R_OP_STATUS_BLE_DONE_ABORT      0x1407
#define R_OP_STATUS_BLE_DONE_STOPPED    0x1408
/* operation finished with error */
#define R_OP_STATUS_BLE_ERROR_PAR           0x1800
#define R_OP_STATUS_BLE_ERROR_RXBUF         0x1801
#define R_OP_STATUS_BLE_ERROR_NO_SETUP      0x1802
#define R_OP_STATUS_BLE_ERROR_NO_FS         0x1803
#define R_OP_STATUS_BLE_ERROR_SYNTH_PROG    0x1804
#define R_OP_STATUS_BLE_ERROR_RXOVF         0x1805
#define R_OP_STATUS_BLE_ERROR_TXUNF         0x1806
/**@}*/


typedef struct __attribute__ ((aligned(4))) {
    radio_op_command_t ropCmd;
    uint8_t channel;
    uint8_t whitening;
    uint32_t pParams;
    uint32_t pOutput;
} ble_rop_cmd_t;

typedef struct {
    uint16_t commandNo;
    uint8_t payloadType;
    uint8_t newLen;
    uintptr_t pNewData;
    uintptr_t pParams;// TODO type this properly once type is known (damn datashiet)
} ble_update_adv_payload_t;

#ifdef __cplusplus
}
#endif

#endif /* CC26x0_RFC_BLE_H */

/*@}*/
