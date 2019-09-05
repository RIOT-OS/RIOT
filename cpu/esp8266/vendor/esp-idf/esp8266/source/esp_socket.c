// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/errno.h>

#include "esp_socket.h"
#include "net/sockio.h"

#define CRITICAL_DECLARE(t)
#define CRITICAL_ENTER(t)
#define CRITICAL_EXIT(t)

#ifndef ESP_SOCKET_MAX
#define ESP_SOCKET_MAX 2
#endif

#define SET_ERR(err)                    errno = err

#define CHECK_FD(s)                     \
    if (s >= ESP_SOCKET_MAX             \
        || !s_socket[s].info) {         \
        SET_ERR(EINVAL);                \
        return -1;                      \
    }

#define CHECK_METHOD(s, io)             \
    CHECK_FD(s)                         \
    if (!s_socket[s].method             \
        || !s_socket[s].method->io) {   \
        SET_ERR(ESRCH);                 \
        return -1;                      \
    }

#define SOCKET_IO_METHOD(s, io, ...)    \
    s_socket[s].method->io(s_socket[s].index, ##__VA_ARGS__)

/*
 * socket event object
 */
typedef struct esp_socket_event {
    esp_aio_cb_t                cb;
    void                        *arg;
} esp_socket_event_t;

/*
 * socket object
 */
typedef struct esp_socket {
    esp_socket_info_t           *info;

    /*
     * lowlevel socket module index
     */
    void                        *index;

    /*
     * lowlevel socket module method
     */
    const esp_socket_method_t   *method;

    esp_socket_event_t          event[ESP_SOCKET_MAX_EVENT];
} esp_socket_t;

static esp_socket_t s_socket[ESP_SOCKET_MAX];

static inline int event_is_used(int s, int e)
{
    return s_socket[s].event[e].cb != NULL;
}

static inline int alloc_event(int s, int e)
{
    CRITICAL_DECLARE(t);

    if (e >= ESP_SOCKET_MAX_EVENT)
        return -1;

    CRITICAL_ENTER(t);
    if (event_is_used(s, e)) {
        e = ESP_SOCKET_MAX_EVENT;
    }
    CRITICAL_EXIT(t);

    return e < ESP_SOCKET_MAX_EVENT ? e : -1;
}

static inline void free_event(int s, int e)
{
    s_socket[s].event[e].cb = NULL;
}

static inline int alloc_socket(void)
{
    int s;
    CRITICAL_DECLARE(t);

    CRITICAL_ENTER(t);
    for (s = 0; s < ESP_SOCKET_MAX; s++) {
        if (s_socket[s].info == NULL) {
            s_socket[s].info = (void *)1;
            break;
        }
    }
    CRITICAL_EXIT(t);

    return s < ESP_SOCKET_MAX ? s : -1;
}

static inline void free_socket(int s)
{
    int e;

    s_socket[s].info = NULL;
    for (e = 0; e < ESP_SOCKET_MAX_EVENT; e++) {
        free_event(s, e);
    }
}

/*
 * @brief create a socket file description
 */
int esp_socket(int domain, int type, int protocol)
{
    int s;

    s = alloc_socket();
    if (s < 0) {
        SET_ERR(ENOMEM);
        return -1;
    }

    s_socket[s].info = malloc(sizeof(esp_socket_info_t));
    if (!s_socket[s].info) {
        free_socket(s);
        SET_ERR(ENOMEM);
        return -1;
    }

    s_socket[s].info->domain = domain;
    s_socket[s].info->type = type;
    s_socket[s].info->protocol = protocol;

    s_socket[s].index = NULL;
    s_socket[s].method = NULL;

    return s;
}

/*
 * @brief send a block of data asynchronously and receive result by callback function
 */
int esp_aio_sendto(esp_aio_t *aio, const struct sockaddr_ll *to, socklen_t len)
{
    int s = aio->fd;

    CHECK_METHOD(s, aio_sendto);

    return SOCKET_IO_METHOD(s, aio_sendto, aio, to, len);
}

/*
 * @brief register a event and its callback function to target of file description
 */
int esp_aio_event(int fd, unsigned int event, esp_aio_cb_t cb, void *arg)
{
    int e;
    int ret;
    int s = fd;

    CHECK_METHOD(s, aio_event);

    e = alloc_event(s, event);
    if (e < 0) {
        SET_ERR(ENOMEM);
        return -1;
    }

    ret = SOCKET_IO_METHOD(s, aio_event, event, cb, arg);
    if (ret) {
        free_event(s, e);
        return -1;
    }

    s_socket[s].event[e].cb = cb;
    s_socket[s].event[e].arg = arg;

    return 0;
}

/*
 * @brief lowlevel socket module upload event and its data
 */
int esp_upload_event(void *index, esp_socket_info_t *info, unsigned int event, esp_aio_data_t *aio_data)
{
    int ret;
    int s;

    if (event >= ESP_SOCKET_MAX_EVENT)
        return -EINVAL;

    for (s = 0; s < ESP_SOCKET_MAX; s++) {
        if (s_socket[s].index == index && event_is_used(s, event)) {
            esp_aio_t aio;

            aio.fd = s;
            aio.cb = s_socket[s].event[event].cb;
            aio.arg = s_socket[s].event[event].arg;

            aio.pbuf = aio_data->pbuf;
            aio.len = aio_data->len;
            aio.ret = aio_data->status;

            ret = s_socket[s].event[event].cb(&aio);
            if (ret)
                return ret;
        }
    }

    return 0;
}

/*
 * @brief free buffer taken from event callback
 */
int esp_free_pbuf(int fd, void *pbuf)
{
    int ret;
    int s = fd;

    CHECK_METHOD(s, free_pbuf);

    ret = SOCKET_IO_METHOD(s, free_pbuf, pbuf);
    if (ret)
        return -1;

    return 0; 
}

/*
 * @brief map real lowlevel socket object to virtual socket
 */
static int map_socket_ll(int fd, const char *name)
{
    int s = fd;
    const esp_socket_method_t *p;
    extern const esp_socket_method_t __start_ksymatabesp_socket, __stop_ksymatabesp_socket;

    for (p = &__start_ksymatabesp_socket; p != &__stop_ksymatabesp_socket; p++) {
        if (!strcmp(name, p->name))
            break;
    }
    if (p >= &__stop_ksymatabesp_socket) {
        SET_ERR(ENXIO);
        return -1;
    }

    s_socket[s].index = p->open(s_socket[s].info);
    if (!s_socket[s].index)
        return -1;
    s_socket[s].method = p;

    return 0;
}

/*
 * @brief send requset command to target by file description and get the result synchronously
 */
int esp_ioctl(int fd, unsigned int cmd, ...)
{
    int ret;
    va_list va;
    int s = fd;

    va_start(va, cmd);

    switch(cmd) {
#if SIOCGIFINDEX != SIOGIFINDEX
        case SIOGIFINDEX:
#endif
        case SIOCGIFINDEX: {
            const char *name;

            name = va_arg(va, const char *);
            ret = map_socket_ll(fd, name);
            break;
        }
        default: {
            int *arg = ((int *)&cmd) + 1;

            CHECK_METHOD(s, ioctl);
            ret = SOCKET_IO_METHOD(s, ioctl, cmd, arg);
            break;
        }
    }

    va_end(va);

    return ret;
}

/*
 * @brief close target of file description
 */
int esp_close(int fd)
{
    int ret;
    int s = fd;

    CHECK_METHOD(s, close);

    ret = SOCKET_IO_METHOD(s, close);
    if (ret)
        return -1;

    free(s_socket[s].info);
    free_socket(s);

    return 0;
}
