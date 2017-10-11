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
 * @brief       The WINC1500 driver (Wi-Fi connectivity only)
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 *
 * @}
 */

/* Required for strnlen in string.h, when building with -std=c99 */
#define _DEFAULT_SOURCE 1
#include <string.h>
/* From pkg/winc1500 */
#include "driver/include/m2m_wifi.h"
#include "driver/source/m2m_hif.h"

#include "winc1500.h"
#include "winc1500_internal.h"
#include "winc1500_netdev.h"

#include "xtimer.h"
#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define START_HANDLER (1)
#define STOP_HANDLER  (2)

winc1500_t winc1500 = { .state = 0 };

char _handler_stack[WINC1500_STACKSIZE];

msg_t _queue[WINC1500_EVENT_QUEUE_LEN];
msg_t _mbox_msgs[WINC1500_EVENT_MBOX_LEN];

char _ssid[WINC1500_MAX_SSID_LEN + 1];
winc1500_ap_t _ap;
uint8_t *_mac_addr;

static int wait_for_event(msg_t *response, uint16_t event, uint16_t error_event);
static winc1500_sec_flags_t sec_module2driver(tenuM2mSecType module_sec);
static tenuM2mSecType sec_driver2module(winc1500_sec_flags_t sec);

/* -------------------------------------------------------------------------- */
/* Private functions                                                          */
/* -------------------------------------------------------------------------- */
#ifndef MODULE_NETDEV_ETH
/*
 * @brief Event handler thread. 
 *        This handler is not used when NETDEV is enabled
 */
static void *event_handler(void *arg)
{
    (void)arg;

    msg_init_queue(_queue, WINC1500_EVENT_QUEUE_LEN);
    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == START_HANDLER) {
            while (1) {
                /* Due to non-deterministic behavior of m2m_wifi_handle_events()
                 * e.g.) Wi-Fi disconnection event before reading RSSI.
                 * we should let this function called more than one time until
                 * it catches a desired event defined in wait_for_event.
                 */
                m2m_wifi_handle_events(NULL);
                if (msg_try_receive(&msg) == 1) {
                    if (msg.type == STOP_HANDLER) {
                        break;
                    }
                }
                xtimer_usleep(1000);
            }
        }
    }
    /* Shouldn't be reached */
    return NULL;
}
#else
#endif

/*
 * @brief   Wait for specific desired event, or return error when an event in
 *          a 'black list'.
 */
static int wait_for_event(msg_t *response, uint16_t event, uint16_t error_event)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_ERR;
    msg_t msg_resp;
    /* Wake up handler */
#ifndef  MODULE_NETDEV_ETH
    msg_t msg_req;
    msg_req.content.value = 0;
    msg_req.type = START_HANDLER;
    msg_send(&msg_req, dev->pid);
#else
    /* When NETDEV enabled it doesn't need to wake it up here. */
#endif
    /* wait for event message */
    while (true) {
        /* TODO: Timeout condition */
        mbox_get(&dev->event_mbox, &msg_resp);
        if (msg_resp.type & event) {
            result = WINC1500_OK;
            goto done;
        }
        else if (msg_resp.type & error_event) {
            result = WINC1500_ERR;
            goto done;
        }
    }
done:
#ifndef  MODULE_NETDEV_ETH
    /* Sleep handler */
    msg_req.content.value = 0;
    msg_req.type = STOP_HANDLER;
    msg_send(&msg_req, dev->pid);
#else
    /* When NETDEV enabled this function doesn't need to sleep handler here. */
#endif
    /* Pass response */
    *response = msg_resp;
    return result;
}

/*
 * @brief   Converts module API's security settings to this driver setting.
 */
static winc1500_sec_flags_t sec_module2driver(tenuM2mSecType module_sec)
{
    winc1500_sec_flags_t sec = WINC1500_SEC_UNKNOWN;
    switch (module_sec) {
        case M2M_WIFI_SEC_INVALID:
            sec = WINC1500_SEC_UNKNOWN;
            break;
        case M2M_WIFI_SEC_OPEN:
            sec |= WINC1500_SEC_FLAGS_OPEN;
            break;
        case M2M_WIFI_SEC_WPA_PSK:
            sec |= WINC1500_SEC_FLAGS_WPA;
            sec |= WINC1500_SEC_FLAGS_WPA2;
            break;
        case M2M_WIFI_SEC_WEP:
            sec |= WINC1500_SEC_FLAGS_WEP;
            break;
        case M2M_WIFI_SEC_802_1X:
            sec |= WINC1500_SEC_FLAGS_WPA;
            sec |= WINC1500_SEC_FLAGS_WPA2;
            sec |= WINC1500_SEC_FLAGS_ENTERPRISE;
            break;
    }
    return sec;
}

/*
 * @brief   Converts this driver's security setting to module API
 */
static tenuM2mSecType sec_driver2module(winc1500_sec_flags_t sec)
{
    tenuM2mSecType module_sec = M2M_WIFI_SEC_INVALID;
    if (sec & WINC1500_SEC_FLAGS_OPEN) {
        module_sec = M2M_WIFI_SEC_OPEN;
    }
    else if ((sec & WINC1500_SEC_FLAGS_WPA) || (sec & WINC1500_SEC_FLAGS_WPA2)) {
        module_sec = M2M_WIFI_SEC_WPA_PSK;
    }
    else if (sec & WINC1500_SEC_FLAGS_WEP) {
        module_sec = M2M_WIFI_SEC_WEP;
    }

    if ((module_sec == M2M_WIFI_SEC_WPA_PSK) && (sec & WINC1500_SEC_FLAGS_ENTERPRISE)) {
        module_sec = M2M_WIFI_SEC_802_1X;
    }
    return module_sec;
}

/* -------------------------------------------------------------------------- */
/* Public functions                                                           */
/* -------------------------------------------------------------------------- */
void winc1500_setup(winc1500_t *dev, const winc1500_params_t *params)
{
#ifdef MODULE_NETDEV_ETH
    dev->netdev.driver = &netdev_driver_winc1500;
#endif
    dev->params = *params;
}

int winc1500_init(const winc1500_params_t *params)
{
#ifdef MODULE_NETDEV_ETH
    /* When NETDEV enabled _init() in @ref winc1500_netdev.c will actually
     * initialize the device. So this function will be a dummy */
    return WINC1500_OK;
#else
    winc1500_t *dev = &winc1500;
    if (dev->state & WINC1500_STATE_INIT) {
        /* Currently multiple instances not allowed. */
        return WINC1500_ERR;
    }
    winc1500_setup(dev, params);

    spi_init(dev->params.spi);
    _lock(dev);

    /* Initialize the BSP. */
    nm_bsp_init();

    tstrWifiInitParam wifi_param;
    memset((uint8_t *)&wifi_param, 0, sizeof(tstrWifiInitParam));

    /* Register a callback. But replaced by the custom callback.
     *  See process_event() in winc1500_callback.c */
    wifi_param.pfAppWifiCb = NULL;
    if (M2M_SUCCESS != m2m_wifi_init(&wifi_param)) {
        return WINC1500_ERR;
    }
    /* Register WINC1500's internal wifi callback instead of the driver's callback
     *  for RIOT GNRC
     */
    hif_register_cb(M2M_REQ_GROUP_WIFI, _wifi_cb);

    /* Initialize winc1500 struct */
    mbox_init(&dev->event_mbox, _mbox_msgs, WINC1500_EVENT_MBOX_LEN);

    /* Create WINC1500 thread */
    dev->pid = thread_create(_handler_stack, sizeof(_handler_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  event_handler, NULL, "winc1500");
    if (dev->pid <= 0) {
        return WINC1500_ERR;
    }

    dev->state = 0 | WINC1500_STATE_INIT;
    dev->state |= WINC1500_STATE_IDLE;

    _unlock(dev);
    return WINC1500_OK;
#endif
}

int winc1500_scan(void)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    msg_t msg_resp;
    if (!(dev->state & WINC1500_STATE_IDLE)) {
        return WINC1500_ERR;
    }
    _lock(dev);
    /* Request scan. */
    result = m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
    if (M2M_SUCCESS != result) {
        result = WINC1500_ERR;
        goto done;
    }

    result = wait_for_event(&msg_resp, WINC1500_EVENT_SCAN_DONE,
                                            WINC1500_EVENT_NOTHING);

done:
    _unlock(dev);
    if (WINC1500_OK == result) {
        return (int)msg_resp.content.value;
    }
    return result;
}

int winc1500_read_ap(winc1500_ap_t *ap_result, uint8_t ap_idx)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    msg_t msg_resp;
    if (!(dev->state & WINC1500_STATE_IDLE)) {
        return WINC1500_ERR;
    }
    _lock(dev);
    /* Request scan. */
    result = m2m_wifi_req_scan_result(ap_idx);
    if (M2M_SUCCESS != result) {
        result = WINC1500_ERR;
        goto done;
    }

    result = wait_for_event(&msg_resp, WINC1500_EVENT_SCAN_RESULT,
                                      WINC1500_EVENT_NOTHING);
done:
    _unlock(dev);
    if (WINC1500_OK == result) {
        winc1500_ap_t *ap_info = (winc1500_ap_t *)msg_resp.content.ptr;
        strncpy(ap_result->ssid, ap_info->ssid, WINC1500_MAX_SSID_LEN);
        ap_result->rssi = ap_info->rssi;
        ap_result->sec = sec_module2driver(ap_info->sec);
    }
    return result;
}

int winc1500_get_mac_addr(uint8_t *addr)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    if (!(dev->state & WINC1500_STATE_INIT)) {
        return WINC1500_ERR;
    }
    _lock(dev);

    /* Get MAC Address. */
    m2m_wifi_get_mac_address(addr);
    DEBUG("MAC Address : ");
    DEBUG("%02X:%02X:%02X:%02X:%02X:%02X\n",
            addr[0], addr[1],
            addr[2], addr[3],
            addr[4], addr[5]);
    _unlock(dev);
    return result;
}

int winc1500_connect(const winc1500_ap_t *ap_info)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    msg_t msg_resp;
    if (!(dev->state & WINC1500_STATE_IDLE)) {
        return WINC1500_ERR;
    }
    _lock(dev);

    /* Request scan. */
    result = m2m_wifi_connect(ap_info->ssid,
                        strnlen(ap_info->ssid, WINC1500_MAX_SSID_LEN),
                        sec_driver2module(ap_info->sec),
                        (void *)ap_info->password, M2M_WIFI_CH_ALL);
    if (M2M_SUCCESS != result) {
        result = WINC1500_ERR;
        goto done;
    }

    result = wait_for_event(&msg_resp, WINC1500_EVENT_CON_STATE_CONNECTED,
                WINC1500_EVENT_NOTHING | WINC1500_EVENT_CON_STATE_DISCONNECTED);
done:
    _unlock(dev);
    return result;
}

int winc1500_connect_list(const winc1500_ap_t ap_info[])
{
    /* TODO: Implement later */
    return -1;
}

int winc1500_connection_info(winc1500_ap_t *ap_result, uint8_t *mac_addr)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    msg_t msg_resp;
    if (!(dev->state & WINC1500_STATE_CONNECTED)) {
        return WINC1500_ERR;
    }
    _lock(dev);

    _mac_addr = mac_addr;
    /* Request scan. */
    result = m2m_wifi_get_connection_info();
    if (M2M_SUCCESS != result) {
        result = WINC1500_ERR;
        goto done;
    }

    result = wait_for_event(&msg_resp, WINC1500_EVENT_CONN_INFO,
                                        WINC1500_EVENT_NOTHING);
done:
    _unlock(dev);
    if (WINC1500_OK == result) {
        winc1500_ap_t *ap_info = (winc1500_ap_t *)msg_resp.content.ptr;
        strncpy(ap_result->ssid, ap_info->ssid, WINC1500_MAX_SSID_LEN);
        ap_result->rssi = ap_info->rssi;
        ap_result->sec = sec_module2driver(ap_info->sec);
    }
    return result;
}

int winc1500_disconnect(void)
{
    winc1500_t *dev = &winc1500;
    int result = WINC1500_OK;
    msg_t msg_resp;
    if (!(dev->state & WINC1500_STATE_CONNECTED)) {
        return WINC1500_ERR;
    }
    _lock(dev);

    /* Request scan. */
    result = m2m_wifi_disconnect();
    if (M2M_SUCCESS != result) {
        result = WINC1500_ERR;
        goto done;
    }

    result = wait_for_event(&msg_resp, WINC1500_EVENT_CON_STATE_DISCONNECTED,
                WINC1500_EVENT_NOTHING);
done:
    _unlock(dev);
    return result;
}
