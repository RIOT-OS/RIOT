/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_ble_nimble
 * @{
 *
 * @file
 * @brief       Implementation of the Bluetooth LE Host Controller Interface
 *
 * ESP32x SoC Bluetooth LE controller uses the uses UART H4 transport protocol.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "log.h"
#include "esp_bt.h"
#include "mutex.h"
#include "od.h"

#include "host/ble_hs.h"
#include "nimble/hci_common.h"
#include "nimble/nimble_port.h"
#include "nimble/transport/hci_h4.h"
#include "sysinit/sysinit.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* The size of an HCI command packet is defined as POOL_CMD_SIZE
 * in `./nimble/transport/src/transport.c.`. Since there is no defined
 * symbol in the header files, we have to define it here. According to
 * the BLE specification, controllers must accept HCI command packets
 * with up to 255 bytes of data excluding the header. */
#define BLE_HCI_CMD_SIZE                (255 + BLE_HCI_CMD_HDR_LEN)

#define BLE_HCI_CMD_HDR_LEN             (3)
#define BLE_HCI_EVT_HDR_LEN             (2)

#define BLE_VHCI_TIMEOUT_MS             2000

/* Definition of UART H4 packet types */
enum {
    BLE_HCI_UART_H4_NONE = 0x00,
    BLE_HCI_UART_H4_CMD  = 0x01,
    BLE_HCI_UART_H4_ACL  = 0x02,
    BLE_HCI_UART_H4_SCO  = 0x03,
    BLE_HCI_UART_H4_EVT  = 0x04
};

static const char *LOG_TAG = "esp_nimble";

static mutex_t _esp_vhci_semaphore = MUTEX_INIT;

static struct hci_h4_sm _esp_h4sm;

static void _ble_vhci_controller_ready_cb(void)
{
    mutex_unlock(&_esp_vhci_semaphore);
}

static int _ble_vhci_packet_received_cb(uint8_t *data, uint16_t len)
{
    /* process the HCI H4 formatted packet and call ble_transport_to_hs_* */
    len = hci_h4_sm_rx(&_esp_h4sm, data, len);
    return 0;
}

static const esp_vhci_host_callback_t vhci_host_cb = {
    .notify_host_send_available = _ble_vhci_controller_ready_cb,
    .notify_host_recv = _ble_vhci_packet_received_cb,
};

static inline int _ble_transport_to_ll(uint8_t *packet, uint16_t len)
{
    uint8_t rc = 0;

    /* check whether the controller is ready to accept packets */
    if (!esp_vhci_host_check_send_available()) {
        LOG_TAG_DEBUG(LOG_TAG, "Controller not ready to accept packets");
    }

    /* take the semaphore with timeout and send the packet to the controller */
    if (ztimer_mutex_lock_timeout(ZTIMER_MSEC, &_esp_vhci_semaphore,
                                  BLE_VHCI_TIMEOUT_MS) == 0) {
        esp_vhci_host_send_packet(packet, len);
    }
    else {
        rc = BLE_HS_ETIMEOUT_HCI;
    }
    return rc;
}

int ble_transport_to_ll_cmd_impl(void *buf)
{
    uint16_t len;
    uint8_t rc = 0;
    uint8_t packet[BLE_HCI_CMD_SIZE + 1];
    uint8_t* cmd = buf;

    assert(cmd != NULL);

    /* Prepare the HCI H4 formatted packet. HCI H4 uses one byte HCI packet
     * indicator in front of the HCI command packet. */

    len = BLE_HCI_CMD_HDR_LEN + cmd[2] + 1;  /* overall length */
    packet[0] = BLE_HCI_UART_H4_CMD;         /* first byte is the packet indicator */
    memcpy(packet + 1, cmd, len - 1);

    if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
        printf("CMD host to ctrl:\n");
        od_hex_dump(packet + 1, len - 1, OD_WIDTH_DEFAULT);
    }

    /* send the packet */
    rc = _ble_transport_to_ll(packet, len);

    /* release the packet buffer */
    ble_transport_free(buf);

    return rc;
}

int ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
    uint16_t len = 0;
    uint8_t rc = 0;
    uint8_t packet[MYNEWT_VAL(BLE_TRANSPORT_ACL_SIZE) + 1];

    assert(om != NULL);

    /* If this packet is zero length, just free it */
    if (OS_MBUF_PKTLEN(om) == 0) {
        os_mbuf_free_chain(om);
        return 0;
    }

    /* Prepare the HCI H4 formatted packet. HCI H4 uses one byte HCI packet
       indicator in front of the HCI command packet. */

    packet[0] = BLE_HCI_UART_H4_ACL;
    len++;

    if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
        printf("ACL host to ctrl:\n");
        od_hex_dump(packet + 1,
                    (om->om_len < 32) ? om->om_len : 32, OD_WIDTH_DEFAULT);
    }

    os_mbuf_copydata(om, 0, OS_MBUF_PKTLEN(om), &packet[1]);
    len += OS_MBUF_PKTLEN(om);

    /* send the packet */
    rc = _ble_transport_to_ll(packet, len);

    /* release the mbuf */
    os_mbuf_free_chain(om);

    return rc;
}

static int _esp_hci_h4_frame_cb(uint8_t pkt_type, void *data)
{
    int rc = 0;

    switch (pkt_type) {
    case HCI_H4_ACL:
        if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
            printf("ACL ctrl to host:\n");
            od_hex_dump((uint8_t *)data, 4, OD_WIDTH_DEFAULT);
        }
        rc = ble_transport_to_hs_acl(data);
        break;
    case HCI_H4_EVT:
        if (ENABLE_DEBUG && IS_USED(MODULE_OD)) {
            printf("EVT ctrl to host:\n");
            od_hex_dump((uint8_t *)data, ((uint8_t *)data)[1] + 2, OD_WIDTH_DEFAULT);
        }
        rc = ble_transport_to_hs_evt(data);
        break;
    default:
        assert(0);
        break;
    }

    return rc;
}

void esp_ble_nimble_init(void)
{
    esp_err_t ret;
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    /* TODO: BLE mode only used, the memory for BT Classic could be released
    if ((ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);) != ESP_OK) {
        LOG_TAG_ERROR(LOG_TAG,
                      "Bluetooth controller release classic bt memory failed: %s",
                      esp_err_to_name(ret));
        assert(0);
    }
    */

    /* init and enable the Bluetooth LE controller */
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        LOG_TAG_ERROR(LOG_TAG, "Bluetooth controller initialize failed: %d", ret);
        assert(0);
    }

    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_BLE)) != ESP_OK) {
        LOG_TAG_ERROR(LOG_TAG, "Bluetooth controller enable failed: %d", ret);
        assert(0);
    }

    /* register callbacks from Bluetooth LE controller */
    if ((ret = esp_vhci_host_register_callback(&vhci_host_cb)) != ESP_OK) {
        assert(0);
    }

    /* init HCI H4 processing */
    hci_h4_sm_init(&_esp_h4sm, &hci_h4_allocs_from_ll, _esp_hci_h4_frame_cb);
}
