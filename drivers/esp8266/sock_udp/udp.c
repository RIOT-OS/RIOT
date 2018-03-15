/*
 * Copyright (C) 2016 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver
 * @{
 *
 * @file
 * @brief       Implementation of the UDP socket with the esp8266 802.11b/g/n radio module
 *              In the implementation we assume that we are only woking with exactly one ESP8266 module.
 *              The implementation follows the monitor pattern, thereby allow only one udp action at a time.
 *
 * @author      Markus Blechschmidt <markus.blechschmidt@haw-hamburg.de>
 *
 * @}
 */

#include "net/sock/udp.h"
#include "esp8266.h"
#include "xtimer.h"
#include "thread.h"
#include "mutex.h"
#include "sema.h"
#include "msg.h"
#include "net/ipv4/addr.h"
#include "net/af.h"

#include "gnrc_sock_internal.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"

#define INTERVAL_MS (1)

#define TEMP_ID_STR "temp"
#define LOCAL_ID_STR "local"
#define REMOTE_ID_STR "remote"

/* private varibaled fo the driver */
static ESP8266_t esp8266;
static char threadstack[THREAD_STACKSIZE_DEFAULT
  #if(ENABLE_DEBUG == 1)
    + DEBUG_EXTRA_STACKSIZE
  #endif
  ];
static mutex_t esp_sock_mutex;
static mutex_t esp_callback_mutex;
// static kernel_pid_t api_pid;
static void *send_data;
static size_t send_len;
static void *rece_data;
static size_t rece_len;
static sema_t sem_loc_rece_count;
static sema_t sem_loc_rece_space;
static ESP8266_Connection_t *temp_conn;
static ESP8266_Connection_t *local_conn;
static ESP8266_Connection_t *remote_conn;
static sock_udp_ep_t current_ep;

#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
static sock_udp_t *_udp_socks = NULL;
#endif

typedef struct transmission {
    uint8_t *buffer;
    unsigned int length;
    sock_udp_ep_t remote;
} transmission_t;

/* callback functions required by the esp8266 parser drivers/esp8266/esp8266.h */
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("Connection name: %s\r\n", Connection->Name);
    if (0 == strcmp(Connection->Name, TEMP_ID_STR)) {
        temp_conn = Connection;
    }
    if (0 == strcmp(Connection->Name, LOCAL_ID_STR)) {
        local_conn = Connection;
    }
    if (0 == strcmp(Connection->Name, REMOTE_ID_STR)) {
        remote_conn = Connection;
    }
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionError(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionClosed(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer, uint16_t max_buffer_size)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    /* restrain size to avoid memory corruption */
    if (send_len > max_buffer_size) {
        send_len = max_buffer_size;
    }
    memcpy(Buffer, send_data, send_len);
    return send_len;
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t *ESP8266, ESP8266_Connection_t *Connection, char *Buffer)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    /* listening endpoint received something */
    DEBUG("Conection name:  %s\r\n", Connection->Name);
    if (0 == strcmp(Connection->Name, LOCAL_ID_STR)) {
        if (sema_wait_timed(&sem_loc_rece_space, INTERVAL_MS * US_PER_MS) < 0) {
            return;
        }
        DEBUG("local endpoint\r\n");
        /* acutaly, the next line should be
           rece_len = Connection->BytesReceived;
           but the program sometimes gives the wrong value when the message ends with a newline */
        rece_len = Connection->BytesReceived;
        rece_data = malloc(rece_len);
        memcpy(rece_data, Buffer, rece_len);
        memcpy(current_ep.addr.ipv4, Connection->RemoteIP, sizeof(ipv4_addr_t));
        current_ep.family = AF_INET;
        current_ep.port = Connection->RemotePort;

        DEBUG("%d bytes\r\n", rece_len);

        sema_post(&sem_loc_rece_count);
    }
    /* sending endpoint received somethin back */
    else if (Connection == remote_conn) {
        DEBUG("remote endpoint\r\n");
    }
    DEBUG("end %s\r\n", DEBUG_FUNC);
}
void ESP8266_Callback_ConnectedStationsDetected(ESP8266_t *ESP8266, ESP8266_ConnectedStations_t *Stations)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    DEBUG("end %s\r\n", DEBUG_FUNC);
}

/* static functions */
static void *_update_loop(void *args)
{
    ESP8266_t *esp8266_p = (ESP8266_t *) args;
    xtimer_ticks32_t last_wakeup = xtimer_now();

    while (1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL_MS * US_PER_MS);
        ESP8266_Update(esp8266_p);
        ESP8266_TimeUpdate(esp8266_p, INTERVAL_MS);
    }
    return NULL;
}

/* init function */
int auto_init_esp8266(void)
{
    /* create thread for reagular updating and timekeeping of the esp */
    thread_create(threadstack, THREAD_STACKSIZE_DEFAULT,
                  THREAD_PRIORITY_MAIN - 1, 0, _update_loop, &esp8266, "esp8266_update_loop");
    /* initialize IPC data structures */
    mutex_init(&esp_sock_mutex);
    mutex_init(&esp_callback_mutex);
    sema_create(&sem_loc_rece_count, 0);
    sema_create(&sem_loc_rece_space, 1);
    /* call initializing functions */
    mutex_lock(&esp_sock_mutex);
    ESP8266_Init(&esp8266, 115200);
    ESP8266_SetMode(&esp8266, ESP8266_Mode_STA);
    ESP8266_WifiConnect(&esp8266, MY_SSID, MY_PSK);
    /* get an print IP address */
    ESP8266_WaitReady(&esp8266);
    ESP8266_GetSTAIPBlocking(&esp8266);
    DEBUG("The device's IP address is %d.%d.%d.%d\r\n", esp8266.STAIP[0], esp8266.STAIP[1], esp8266.STAIP[2], esp8266.STAIP[3]);
    DEBUG("end %s\r\n", DEBUG_FUNC);
    mutex_unlock(&esp_sock_mutex);
    return 0;
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    char addr_str[IPV4_ADDR_MAX_STR_LEN] = { 0 };
    ESP8266_Result_t res;

    /* check preconditions */
    assert(sock);
    assert(local == NULL || local->port != 0);
    assert(remote == NULL || remote->port != 0);
    DEBUG("%s assertion check succeded\n", DEBUG_FUNC);
    if ((local != NULL) && (remote != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != remote->netif)) {
        return -EINVAL;
    }
    DEBUG("%s value check succeded\n", DEBUG_FUNC);
    /* esp8266 suports IPv4 only 😞 */
    if (((local != NULL) && (local->family != AF_INET)) ||
        ((remote != NULL) && (remote->family != AF_INET))) {
        return -EAFNOSUPPORT;
    }
    DEBUG("%s IPv4 check succeded\n", DEBUG_FUNC);
    memset(&sock->local, 0, sizeof(sock_udp_ep_t));
    if (local != NULL) {
#ifdef MODULE_GNRC_SOCK_CHECK_REUSE
        DEBUG("%s checking for reuse\n", DEBUG_FUNC);
        if (!(flags & SOCK_FLAGS_REUSE_EP)) {
            for (sock_udp_t *ptr = _udp_socks; ptr != NULL;
                 ptr = (sock_udp_t *)ptr->reg.next) {
                if (memcmp(&ptr->local, local, sizeof(sock_udp_ep_t)) == 0) {
                    return -EADDRINUSE;
                }
            }
        }
        DEBUG("%s reuse check succeded\n", DEBUG_FUNC);
        /* prepend to current socks */
        sock->reg.next = (gnrc_sock_reg_t *)_udp_socks;
        _udp_socks = sock;
#endif
        DEBUG("%s checking for support\n", DEBUG_FUNC);
        if (local->family != AF_INET) {
            return -EAFNOSUPPORT;
        }
        DEBUG("%s support check succeded\n", DEBUG_FUNC);
        ipv4_addr_to_str(addr_str, (ipv4_addr_t *) &(local->addr), IPV4_ADDR_MAX_STR_LEN);
        ESP8266_WaitReady(&esp8266);
        res = ESP8266_StartClientConnectionUDP(&esp8266, LOCAL_ID_STR, addr_str, 0, local->port, NULL);
        if (res == ESP_OK) {
            DEBUG("Listening on port %d\r\n", local->port);
        }
        else {
            DEBUG("No local port. Error: %d\r\n", res);
        }
        memcpy(&sock->local, local, sizeof(sock_udp_ep_t));
    }
    memset(&sock->remote, 0, sizeof(sock_udp_ep_t));
    if (remote != NULL) {
        if (remote->family  != AF_INET) {
            return -EAFNOSUPPORT;
        }
        DEBUG("%s support check succeded\n", DEBUG_FUNC);
        if (gnrc_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        memcpy(&sock->remote, remote, sizeof(sock_udp_ep_t));
    }
    if (local != NULL) {
        /* listen only with local given */
        gnrc_sock_create(&sock->reg, GNRC_NETTYPE_UDP,
                         local->port);
    }
    sock->flags = flags;
    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{
    assert(sock != NULL);
    mutex_lock(&esp_sock_mutex);
    // api_pid = thread_getpid();
    ESP8266_WaitReady(&esp8266);
    ESP8266_CloseAllConnections(&esp8266);
    DEBUG("%s\r\n", DEBUG_FUNC);
    mutex_unlock(&esp_sock_mutex);
}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *local)
{
    assert(sock && local);
    if (sock->local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }
    memcpy(local, &sock->local, sizeof(sock_udp_ep_t));
    return 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *remote)
{
    assert(sock && remote);
    if (sock->remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }
    memcpy(remote, &sock->remote, sizeof(sock_udp_ep_t));
    return 0;
}

ssize_t sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote)
{
    DEBUG("start %s\r\n", DEBUG_FUNC);
    assert((sock != NULL) && (data != NULL) && (max_len > 0));
    DEBUG("%s: assertion check succeded\r\n", DEBUG_FUNC);
    if (sock->local.family == AF_UNSPEC) {
        return -EADDRNOTAVAIL;
    }
    DEBUG("%s: family check succeded\r\n", DEBUG_FUNC);

    mutex_lock(&esp_sock_mutex);
    DEBUG("%s: mutex locked\r\n", DEBUG_FUNC);
    int res;
    if ((res = sema_wait_timed(&sem_loc_rece_count, timeout)) < 0) {
        if (res == -ETIMEDOUT) {
            DEBUG("Timeout\r\n");
            free(rece_data);
            mutex_unlock(&esp_sock_mutex);
            return res;
        }
        else {
            DEBUG("Other error\r\n");
            free(rece_data);
            mutex_unlock(&esp_sock_mutex);
            return -1;
        }
    }
    if (rece_len > max_len) {
        free(rece_data);
        mutex_unlock(&esp_sock_mutex);
        return -ENOBUFS;
    }
    /* copy received data to buffer and fill return value sock_udp_ep_t with content*/
    memcpy(data, rece_data, rece_len);
    memcpy(remote, &current_ep, sizeof(current_ep));
    free(rece_data);

    sema_post(&sem_loc_rece_space);
    mutex_unlock(&esp_sock_mutex);
    return rece_len;
}

ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote)
{
    /* lock esp and wait for esp to be ready */
    mutex_lock(&esp_sock_mutex);
    ESP8266_WaitReady(&esp8266);
    /* if another remote endpoint is given, connect to it */
    if (remote != NULL) {
        DEBUG("%s: Using remote given parameter for temporary connection\r\n", DEBUG_FUNC);
        char addr_str[IPV4_ADDR_MAX_STR_LEN] = { 0 };
        ipv4_addr_to_str(addr_str, (ipv4_addr_t *) &(remote->addr), IPV4_ADDR_MAX_STR_LEN);
        DEBUG("%s: remote address: %s\r\n", DEBUG_FUNC, addr_str);
        /* issue esp to establish a client connection */
        ESP8266_WaitReady(&esp8266);
        ESP8266_StartClientConnectionUDP(&esp8266, TEMP_ID_STR, addr_str, remote->port, 0, NULL);
        ESP8266_WaitReady(&esp8266);
    }
    /* else use current remote endpoint */
    else {
        DEBUG("%s: Using remote_conn as connection\r\n", DEBUG_FUNC);
        temp_conn = remote_conn;
    }

    /* put data and length into global variables */
    send_data = (void *)data;
    send_len = len;
    /* request to send them */
    DEBUG("%s: sending data\n", DEBUG_FUNC);
    ESP8266_RequestSendData(&esp8266, temp_conn);
    ESP8266_WaitReady(&esp8266);
    /* close temporaty remote connection if necessary */
    if (remote != NULL) {
        DEBUG("%s: closing temporary connection\r\n", DEBUG_FUNC);
        ESP8266_WaitReady(&esp8266);
        ESP8266_CloseConnection(&esp8266, temp_conn);
        ESP8266_WaitReady(&esp8266);
    }
    DEBUG("end %s\r\n", DEBUG_FUNC);
    mutex_unlock(&esp_sock_mutex);
    /* return number of acutally sent bytes */
    return send_len;
}
