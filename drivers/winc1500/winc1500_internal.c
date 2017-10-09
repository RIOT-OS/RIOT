/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_winc1500
 * @{
 *
 * @file
 * @brief       The internal callback function the WINC1500 driver
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 *
 * @}
 */
/* From pkg/winc1500 */
#include "driver/include/m2m_wifi.h"
#include "driver/source/m2m_hif.h"

#include "winc1500.h"
#include "winc1500_internal.h"
#include <string.h>

#define ENABLE_DEBUG (1)
#include "debug.h"

static void _process_event(uint8_t msg_type, void *payload);


void _wifi_cb(uint8_t opcode, uint16_t size, uint32_t addr)
{
#ifdef MODULE_GNRC_NETDEV
    winc1500_t *dev = &winc1500;
#endif
    /**
     * The code is originally from m2m_wifi_cb() in m2m_wifi.c. The code is
     * modified for GNRC.
     * This callback basically copy event information and then pass it to
     * _process_event().
     */
    (void) size; /* This one is originally used when CONF_MGMT is set */
    winc1500_event_info_t event_info;
    uint16_t recv_size = 0;
    uint8_t is_done = 0;

    /* Prepare */
    switch (opcode) {
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
            recv_size = sizeof (event_info.state_change);
            break;
        case M2M_WIFI_RESP_GET_SYS_TIME:
            recv_size = sizeof (event_info.sys_time);
            break;
        case M2M_WIFI_RESP_CONN_INFO:
            recv_size = sizeof (event_info.conn_info);
            is_done = 1;
            break;
        case M2M_WIFI_REQ_DHCP_CONF:
            recv_size = sizeof (event_info.ip_config);
            break;
        case M2M_WIFI_REQ_WPS:
            recv_size = sizeof (event_info.wps_info);
            memset((uint8_t *) &event_info, 0, recv_size);
            break;
        case M2M_WIFI_RESP_IP_CONFLICT:
            recv_size = sizeof (event_info.ip_conflicted);
            break;
        case M2M_WIFI_RESP_SCAN_DONE: {
            extern uint8_t gu8scanInProgress;
            gu8scanInProgress = 0;
            recv_size = sizeof (event_info.scan_done);
            break;
        }
        case M2M_WIFI_RESP_SCAN_RESULT:
            recv_size = sizeof (event_info.scan_result);
            break;
        case M2M_WIFI_RESP_CURRENT_RSSI:
            recv_size = 4;
            break;
        case  M2M_WIFI_RESP_CLIENT_INFO:
            recv_size = 4;
            break;
        case M2M_WIFI_RESP_PROVISION_INFO:
            recv_size = sizeof (event_info.prov_info);
            is_done = 1;
            break;
        case M2M_WIFI_RESP_DEFAULT_CONNECT:
            recv_size = sizeof (event_info.default_conn_resp);
            is_done = 1;
            break;
        case M2M_WIFI_RESP_GET_PRNG:
            recv_size = sizeof (event_info.prng_result);
            break;
#ifdef MODULE_GNRC_NETDEV
        case M2M_WIFI_RESP_ETHERNET_RX_PACKET: {
            _unlock_bus(dev);
            dev->rx_addr = addr;
            dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_RX_COMPLETE);
            _lock_bus(dev);
            return;
        }
#endif
        default:
            DEBUG("Unhandled event: %d\n", opcode);
            return;
    }

    /* Get data from the module */
    if (hif_receive(addr, (uint8_t *) &event_info, recv_size,
                        is_done) == M2M_SUCCESS) {
        if (M2M_WIFI_RESP_GET_PRNG != opcode) {
            _process_event(opcode, &event_info);
        }
        else {
            addr += sizeof(event_info.prng_result);
            recv_size = event_info.prng_result.u16PrngSize;
            is_done = 1;
            if (hif_receive(addr, event_info.prng_result.pu8RngBuff,
                            recv_size, is_done) == M2M_SUCCESS) {
                _process_event(opcode, &event_info);
            }
        }
    }
}

/**
 *  @brief  Change the device's state or pass data to the event messaging queue
 *          according to event type and event data(payload)
 */
static void _process_event(uint8_t msg_type, void *payload)
{
    winc1500_t *dev = &winc1500;
    msg_t msg;
    msg.type = WINC1500_EVENT_NOTHING;
    msg.content.value = 0;
    winc1500_event_info_t *event = (winc1500_event_info_t *)payload;
    switch (msg_type) {
        case M2M_WIFI_RESP_CON_STATE_CHANGED: {
            DEBUG("M2M_WIFI_RESP_CON_STATE_CHANGED: ");
            switch (event->state_change.u8CurrState) {
                case M2M_WIFI_CONNECTED:
                    /* WiFi is to connected to AP */
                    msg.type = WINC1500_EVENT_CON_STATE_CONNECTED;
                    DEBUG("Wi-Fi connected\n");
                    dev->state &= ~WINC1500_STATE_IDLE;
                    dev->state |= WINC1500_STATE_STA;
                    dev->state |= WINC1500_STATE_CONNECTED;
                    break;
                case M2M_WIFI_DISCONNECTED:
                    /* WiFi is disconnected from AP */
                    msg.type = WINC1500_EVENT_CON_STATE_DISCONNECTED;
                    DEBUG("Wi-Fi disconnected\n");
                    dev->state |= WINC1500_STATE_IDLE;
                    dev->state &= ~WINC1500_STATE_STA;
                    dev->state &= ~WINC1500_STATE_CONNECTED;
                    dev->state &= ~WINC1500_STATE_IP_OBTAINED;
                    break;
                case M2M_WIFI_UNDEF:
                default:
                    /* Undefined status */
                    break;
            }
#ifdef MODULE_GNRC_NETDEV
            /* Update link state to netdev */
            netdev_event_t event_netdev =
                (event->state_change.u8CurrState == M2M_WIFI_CONNECTED) ?
                    NETDEV_EVENT_LINK_UP:
                    NETDEV_EVENT_LINK_DOWN;

            dev->netdev.event_callback(&dev->netdev, event_netdev);
#endif
            break;
        }
        case M2M_WIFI_RESP_CONN_INFO:
            DEBUG("M2M_WIFI_RESP_CONN_INFO\n");
            /* Copy result */
            _ap.rssi = event->conn_info.s8RSSI;
            _ap.sec = event->conn_info.u8SecType;
            strncpy(_ssid, (char *)event->conn_info.acSSID, WINC1500_MAX_SSID_LEN);
            _ap.ssid = _ssid;
            memcpy(_mac_addr, event->conn_info.au8MACAddress, 6);
            /* set message */
            msg.type = WINC1500_EVENT_CONN_INFO;
            msg.content.ptr = &_ap;
            break;
        case M2M_WIFI_REQ_DHCP_CONF: {
            DEBUG("M2M_WIFI_REQ_DHCP_CONF\n");
            /* Called by m2m_wifi_connect() */
            DEBUG("IP obtained from DHCP server\n");
            DEBUG("IP is %u.%u.%u.%u\n",
                    ((uint8_t *)payload)[0], ((uint8_t *)payload)[1],
                    ((uint8_t *)payload)[2], ((uint8_t *)payload)[3]);
            dev->ip_addr = *(uint32_t *)payload;
            dev->state |= WINC1500_STATE_IP_OBTAINED;
            break;
        }
        case M2M_WIFI_REQ_WPS:
            DEBUG("M2M_WIFI_REQ_WPS\n");
            break;
        case M2M_WIFI_RESP_IP_CONFLICT:
            DEBUG("M2M_WIFI_RESP_IP_CONFLICT\n");
            break;
        case M2M_WIFI_RESP_SCAN_DONE:
            DEBUG("M2M_WIFI_RESP_SCAN_DONE\n");
            msg.type = WINC1500_EVENT_SCAN_DONE;
            msg.content.value = event->scan_done.u8NumofCh;
            break;
        case M2M_WIFI_RESP_SCAN_RESULT:
            /* Called by m2m_wifi_req_scan_result() */
            DEBUG("M2M_WIFI_RESP_SCAN_RESULT\n");
            /* Copy result */
            _ap.rssi = event->scan_result.s8rssi;
            _ap.sec = event->scan_result.u8AuthType;
            strncpy(_ssid, (char *)event->scan_result.au8SSID, WINC1500_MAX_SSID_LEN);
            _ap.ssid = _ssid;
            /* set message */
            msg.type = WINC1500_EVENT_SCAN_RESULT;
            msg.content.ptr = &_ap;

            /* display founded AP. */
            DEBUG("SSID:%s\n", event->scan_result.au8SSID);
            break;
        case M2M_WIFI_RESP_CURRENT_RSSI:
            /* Called by m2m_wifi_req_curr_rssi() */
            DEBUG("M2M_WIFI_RESP_CURRENT_RSSI\n");
            int8_t rssi = *(int8_t *)payload;
            msg.type = WINC1500_EVENT_CURRENT_RSSI;
            DEBUG("RSSI Read: %d\n", rssi);
            msg.content.value = rssi * -1;
            break;
        case M2M_WIFI_RESP_CLIENT_INFO:
            DEBUG("M2M_WIFI_RESP_CLIENT_INFO\n");
            break;
        case M2M_WIFI_RESP_PROVISION_INFO:
            DEBUG("M2M_WIFI_RESP_PROVISION_INFO\n");
            break;
        case M2M_WIFI_RESP_DEFAULT_CONNECT:
            DEBUG("M2M_WIFI_RESP_DEFAULT_CONNECT\n");
            break;
        case M2M_WIFI_RESP_GET_SYS_TIME:
            DEBUG("M2M_WIFI_RESP_GET_SYS_TIME\n");
            /* This event is internally used by module firmware
             * on this event the module will sync time using SNTP */
            msg.type = WINC1500_EVENT_OTHERS;
            DEBUG("SNTP time event called\n");
            break;
        default:
            DEBUG("Unknown WiFi message: %d\n", msg_type);
            break;
    }
    mbox_put(&dev->event_mbox, &msg);
}
