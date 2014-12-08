/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup
 * @{
 *
 * @file        netdev_dummy.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "netdev/base.h"

#include "netdev_dummy.h"

#define _NETDEV_MORE(dev) ((_ut_dev_internal *)(dev->more))

typedef struct {
    char dst[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    size_t dst_len;
    char src[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    size_t src_len;
    char data[UNITTESTS_NETDEV_DUMMY_MAX_PACKET];
    size_t data_len;
} _unittest_test_buffer;

typedef struct {
    unsigned int channel;
    char short_address[UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN];
    unsigned int nid;
    char long_address[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    int tx_power;
    netdev_state_t state;
    netdev_rcv_data_cb_t callbacks[UNITTESTS_NETDEV_DUMMY_MAX_CB];
    _unittest_test_buffer rx_buffer;
    _unittest_test_buffer tx_buffer;
    uint32_t last_event;
} _ut_dev_internal;

netdev_t unittest_netdev_dummy_devs[UNITTESTS_NETDEV_DUMMY_MAX_DEV];
static _ut_dev_internal _netdevs_internal[UNITTESTS_NETDEV_DUMMY_MAX_DEV];

static int _find_dev(netdev_t *dev)
{
    if (dev == NULL || dev < (&(unittest_netdev_dummy_devs[0])) ||
        dev > (&(unittest_netdev_dummy_devs[UNITTESTS_NETDEV_DUMMY_MAX_DEV - 1]))) {
        return -1;
    }

    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_DEV; i++) {
        if (&(unittest_netdev_dummy_devs[i]) == dev) {
            return i;
        }
    }

    return -1;
}

static int _init(netdev_t *dev)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    _NETDEV_MORE(dev)->channel = 0;
    _NETDEV_MORE(dev)->nid = 0;
    _NETDEV_MORE(dev)->state = 0;
    memset(_NETDEV_MORE(dev)->short_address, 0,
           UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN);
    memset(_NETDEV_MORE(dev)->long_address, 0,
           UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN);
    memset(&(_NETDEV_MORE(dev)->rx_buffer), 0, sizeof(_unittest_test_buffer));
    memset(&(_NETDEV_MORE(dev)->tx_buffer), 0, sizeof(_unittest_test_buffer));
    _NETDEV_MORE(dev)->last_event = 0;

    for (int j = 0; j < UNITTESTS_NETDEV_DUMMY_MAX_CB; j++) {
        _NETDEV_MORE(dev)->callbacks[j] = NULL;
    }

    return 0;
}


static int _send_data(netdev_t *dev, void *dest, size_t dest_len,
                      netdev_hlist_t *upper_layer_hdrs, void *data,
                      size_t data_len)
{
    netdev_hlist_t *ptr = upper_layer_hdrs;
    size_t tx_ptr = 0;

    if (dest == NULL || data == NULL) {
        return -EFAULT;
    }

    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if ((data_len + netdev_get_hlist_len(upper_layer_hdrs)) >
        UNITTESTS_NETDEV_DUMMY_MAX_PACKET) {
        return -EMSGSIZE;
    }

    if (dest_len > UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) {
        return -EAFNOSUPPORT;
    }

    memcpy(_NETDEV_MORE(dev)->tx_buffer.dst, dest, dest_len);
    _NETDEV_MORE(dev)->tx_buffer.dst_len = dest_len;
    _NETDEV_MORE(dev)->tx_buffer.data_len = 0;

    if (upper_layer_hdrs) {
        do {
            memcpy(&(_NETDEV_MORE(dev)->tx_buffer.data[tx_ptr]),
                   ptr->header, ptr->header_len);
            _NETDEV_MORE(dev)->tx_buffer.data_len += ptr->header_len;
            tx_ptr += ptr->header_len;
            netdev_hlist_advance(&ptr);
        } while (ptr != upper_layer_hdrs);
    }

    memcpy(&(_NETDEV_MORE(dev)->tx_buffer.data[tx_ptr]), data,
           data_len);
    _NETDEV_MORE(dev)->tx_buffer.data_len += data_len;

    return _NETDEV_MORE(dev)->tx_buffer.data_len;
}

static int _add_receive_data_callback(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    for (int j = 0; j < UNITTESTS_NETDEV_DUMMY_MAX_CB; j++) {
        if (_NETDEV_MORE(dev)->callbacks[j] == cb) {
            return 0;
        }

        if (_NETDEV_MORE(dev)->callbacks[j] == NULL) {
            _NETDEV_MORE(dev)->callbacks[j] = cb;
            return 0;
        }
    }

    return -ENOBUFS;
}

static int _rem_receive_data_callback(netdev_t *dev, netdev_rcv_data_cb_t cb)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    for (int j = 0; j < UNITTESTS_NETDEV_DUMMY_MAX_CB; j++) {
        if (_NETDEV_MORE(dev)->callbacks[j] == cb) {
            _NETDEV_MORE(dev)->callbacks[j] = NULL;
            return 0;
        }
    }

    return 0;
}

static int _get_option(netdev_t *dev, netdev_opt_t opt, void *value,
                       size_t *value_len)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if (value == NULL || value_len == NULL) {
        return -EFAULT;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if (*value_len < sizeof(unsigned int)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(unsigned int)) {
                *value_len = sizeof(unsigned int);
            }

            *((unsigned int *)value) = _NETDEV_MORE(dev)->channel;
            break;

        case NETDEV_OPT_ADDRESS:
            if (*value_len < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) {
                return -EOVERFLOW;
            }

            if (*value_len > UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) {
                *value_len = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN;
            }

            memcpy(value, _NETDEV_MORE(dev)->short_address, *value_len);
            break;

        case NETDEV_OPT_NID:
            if (*value_len < sizeof(unsigned int)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(unsigned int)) {
                *value_len = sizeof(unsigned int);
            }

            *((unsigned int *)value) = _NETDEV_MORE(dev)->nid;
            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (*value_len < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) {
                return -EOVERFLOW;
            }

            if (*value_len > UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) {
                *value_len = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN;
            }

            memcpy(value, _NETDEV_MORE(dev)->long_address, *value_len);
            break;

        case NETDEV_OPT_TX_POWER:
            if (*value_len < sizeof(int)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(int)) {
                *value_len = sizeof(int);
            }

            *((unsigned int *)value) = _NETDEV_MORE(dev)->tx_power;
            break;

        case NETDEV_OPT_MAX_PACKET_SIZE:
#if UNITTESTS_NETDEV_DUMMY_MAX_PACKET < 256
            if (*value_len == 0) {
                return -EOVERFLOW;
            }

            *value_len = sizeof(uint8_t);
            *((uint8_t *)value) = (uint8_t)UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#elif UNITTESTS_NETDEV_DUMMY_MAX_PACKET < (1 << 16)

            if (*value_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            *value_len = sizeof(uint16_t);
            *((uint16_t *)value) = (uint16_t)UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#elif UNITTESTS_NETDEV_DUMMY_MAX_PACKET < (1 << 32)

            if (*value_len < sizeof(uint32_t)) {
                return -EOVERFLOW;
            }

            *value_len = sizeof(uint32_t);
            *((uint32_t *)value) = (uint32_t)UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#else

            if (*value_len < sizeof(uint64_t)) {
                return -EOVERFLOW;
            }

            *value_len = sizeof(uint64_t);
            *((uint64_t *)value) = (uint64_t)UNITTESTS_NETDEV_DUMMY_MAX_PACKET;
#endif

            break;

        case NETDEV_OPT_PROTO:
            if (*value_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }

            if (*value_len > sizeof(netdev_proto_t)) {
                *value_len = sizeof(netdev_proto_t);
            }

            *((netdev_proto_t *)value) = NETDEV_PROTO_UNKNOWN;
            break;

        default:
            return -ENOTSUP;
    }

    return 0;
}

static int _type_pun_up_unsigned(void *value_out, size_t desired_len,
                                 void *value_in, size_t given_len)
{
    if (given_len > desired_len) {
        return -EOVERFLOW;
    }

    /* XXX this is ugly, but bear with me */
    switch (given_len) {
        case 8:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (*((uint64_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint32_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (*((uint32_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint16_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint16_t *)value_in));
                    return 0;

                case 2:
                    *((uint16_t *)value_out) = (*((uint16_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((uint64_t *)value_out) = (uint64_t)(*((uint8_t *)value_in));
                    return 0;

                case 4:
                    *((uint32_t *)value_out) = (uint32_t)(*((uint8_t *)value_in));
                    return 0;

                case 2:
                    *((uint16_t *)value_out) = (uint16_t)(*((uint8_t *)value_in));
                    return 0;

                case 1:
                    *((uint8_t *)value_out) = (*((uint8_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

static int _type_pun_up_signed(void *value_out, size_t desired_len,
                               void *value_in, size_t given_len)
{
    if (given_len > desired_len) {
        return -EOVERFLOW;
    }

    /* XXX this is ugly, but bear with me */
    switch (given_len) {
        case 8:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (*((int64_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 4:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int32_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (*((int32_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 2:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int16_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (int32_t)(*((int16_t *)value_in));
                    return 0;

                case 2:
                    *((int16_t *)value_out) = (*((int16_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        case 1:
            switch (desired_len) {
                case 8:
                    *((int64_t *)value_out) = (int64_t)(*((int8_t *)value_in));
                    return 0;

                case 4:
                    *((int32_t *)value_out) = (int32_t)(*((int8_t *)value_in));
                    return 0;

                case 2:
                    *((int16_t *)value_out) = (int16_t)(*((int8_t *)value_in));
                    return 0;

                case 1:
                    *((int8_t *)value_out) = (*((int8_t *)value_in));
                    return 0;

                default:
                    return -EINVAL;
            }

        default:
            return -EINVAL;
    }
}

static int _set_option(netdev_t *dev, netdev_opt_t opt, void *value,
                       size_t value_len)
{
    uint8_t set_value[UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN];
    int res = 0;

    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if (value == NULL) {
        return -EFAULT;
    }

    switch (opt) {
        case NETDEV_OPT_CHANNEL:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(unsigned int),
                                             value, value_len)) == 0) {
                _NETDEV_MORE(dev)->channel = *((unsigned int *)value);
            }

            break;

        case NETDEV_OPT_ADDRESS:
            if (value_len > UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) {
                return -EOVERFLOW;
            }

            do {
                int start = UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN - value_len;

                if (value_len == 0) {
                    return -EINVAL;
                }

                if (value_len < UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) {
                    memcpy(_NETDEV_MORE(dev)->short_address, value, start);
                }

                memcpy(&(_NETDEV_MORE(dev)->short_address[start]), value,
                       value_len);
            } while (0);

            break;

        case NETDEV_OPT_NID:
            if ((res = _type_pun_up_unsigned(set_value, sizeof(unsigned int),
                                             value, value_len)) == 0) {
                _NETDEV_MORE(dev)->nid = *((unsigned int *)value);
            }

            break;

        case NETDEV_OPT_ADDRESS_LONG:
            if (value_len > UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) {
                return -EOVERFLOW;
            }

            do {
                int start = UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN - value_len;

                if (value_len == 0) {
                    return -EINVAL;
                }

                if (value_len < UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN) {
                    memcpy(_NETDEV_MORE(dev)->long_address, value, start);
                }

                memcpy(&(_NETDEV_MORE(dev)->long_address[start]), value,
                       value_len);
            } while (0);

            break;

        case NETDEV_OPT_TX_POWER:
            if ((res = _type_pun_up_signed(set_value, sizeof(int),
                                           value, value_len)) == 0) {
                _NETDEV_MORE(dev)->tx_power = *((int *)value);
            }

            break;

        default:
            return -ENOTSUP;
    }

    return res;
}

static int _get_state(netdev_t *dev, netdev_state_t *state)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if (state == NULL) {
        return -EFAULT;
    }

    *state = _NETDEV_MORE(dev)->state;

    return 0;
}

static int _set_state(netdev_t *dev, netdev_state_t state)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    switch (state) {
        case NETDEV_STATE_POWER_OFF:
        case NETDEV_STATE_POWER_SLEEP:
        case NETDEV_STATE_POWER_IDLE:
        case NETDEV_STATE_RX_MODE:
        case NETDEV_STATE_PROMISCUOUS_MODE:
            _NETDEV_MORE(dev)->state = state;
            return 0;

        default:
            return -ENOTSUP;
    }
}

static void _event(netdev_t *dev, uint32_t event_type)
{
    if (_find_dev(dev) < 0) {
        return;
    }

    _NETDEV_MORE(dev)->last_event = event_type;
}

const netdev_driver_t unittest_netdev_dummy_driver = {
    _init,
    _send_data,
    _add_receive_data_callback,
    _rem_receive_data_callback,
    _get_option,
    _set_option,
    _get_state,
    _set_state,
    _event,
    NULL,
};

int unittest_netdev_dummy_fire_rcv_event(netdev_t *dev, void *src,
        size_t src_len, void *dest, size_t dest_len, void *data,
        size_t data_len)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if ((src_len != UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN &&
         src_len != UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) ||
        (dest_len != UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN &&
         dest_len != UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN)) {
        return -EAFNOSUPPORT;
    }

    if (data_len > UNITTESTS_NETDEV_DUMMY_MAX_PACKET) {
        return -EMSGSIZE;
    }

    if (src == NULL || dest == NULL || (data == NULL && data_len != 0)) {
        return -EINVAL;
    }

    _NETDEV_MORE(dev)->rx_buffer.src_len = src_len;
    _NETDEV_MORE(dev)->rx_buffer.dst_len = dest_len;
    _NETDEV_MORE(dev)->rx_buffer.data_len = data_len;

    memcpy(_NETDEV_MORE(dev)->rx_buffer.dst, dest, dest_len);
    memcpy(_NETDEV_MORE(dev)->rx_buffer.src, src, src_len);
    memcpy(_NETDEV_MORE(dev)->rx_buffer.data, data, data_len);

    for (int j = 0; j < UNITTESTS_NETDEV_DUMMY_MAX_CB; j++) {
        if (_NETDEV_MORE(dev)->callbacks[j] != 0) {
            int res = _NETDEV_MORE(dev)->callbacks[j](dev,
                      _NETDEV_MORE(dev)->rx_buffer.src,
                      _NETDEV_MORE(dev)->rx_buffer.src_len,
                      _NETDEV_MORE(dev)->rx_buffer.dst,
                      _NETDEV_MORE(dev)->rx_buffer.dst_len,
                      _NETDEV_MORE(dev)->rx_buffer.data,
                      _NETDEV_MORE(dev)->rx_buffer.data_len);

            if (res < 0) {
                return -ECANCELED;
            }
        }
    }

    return 0;
}

int unittest_netdev_dummy_check_transmitted(netdev_t *dev,
        void *expected_dest,
        size_t expected_dest_len,
        void *expected_data,
        size_t expected_data_len)
{
    if (_find_dev(dev) < 0) {
        return -ENODEV;
    }

    if ((expected_dest == NULL && expected_dest_len != 0) ||
        (expected_data == NULL && expected_data_len != 0)) {
        return -EINVAL;
    }

    if (!(expected_dest_len == UNITTESTS_NETDEV_DUMMY_MAX_LONG_ADDR_LEN ||
          expected_dest_len == UNITTESTS_NETDEV_DUMMY_MAX_ADDR_LEN) ||
        expected_dest_len != _NETDEV_MORE(dev)->tx_buffer.dst_len ||
        expected_data_len != _NETDEV_MORE(dev)->tx_buffer.data_len ||
        (memcmp(expected_dest, _NETDEV_MORE(dev)->tx_buffer.dst, expected_dest_len) != 0) ||
        (memcmp(expected_data, _NETDEV_MORE(dev)->tx_buffer.data, expected_data_len) != 0)) {
        return 1;
    }

    return 0;
}

uint32_t unittest_netdev_dummy_get_last_event(netdev_t *dev)
{
    if (_find_dev(dev) < 0) {
        return ENODEV;
    }

    return _NETDEV_MORE(dev)->last_event;
}

void unittest_netdev_dummy_init(void)
{
    for (int i = 0; i < UNITTESTS_NETDEV_DUMMY_MAX_DEV; i++) {
        netdev_t *dev = &(unittest_netdev_dummy_devs[i]);
        dev->type = NETDEV_TYPE_BASE;
        dev->driver = &unittest_netdev_dummy_driver;
        dev->more = &(_netdevs_internal[i]);
    }
}

/**
 * @}
 */
