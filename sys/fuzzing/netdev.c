/*
 * Copyright (C) 2019 Sören Tempel <tempel@uni-bremen.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <stddef.h>

#include "mutex.h"
#include "thread.h"
#include "fuzzing.h"

#include "net/netdev_test.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc/netif.h"

/* unlocked once the device is initialized. */
static mutex_t initmtx = MUTEX_INIT_LOCKED;

static int _dev_init(netdev_t *netdev)
{
    (void)netdev;
    mutex_unlock(&initmtx);
    return 0;
}

static int _dev_get_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_SLIP;
    return sizeof(uint16_t);
}

int fuzzing_netdev(gnrc_netif_t *netif) {
    static netdev_test_t dev;
    static char _netif_stack[THREAD_STACKSIZE_DEFAULT];

    netdev_test_setup(&dev, NULL);
    netdev_test_set_init_cb(&dev, _dev_init);
    netdev_test_set_get_cb(&dev, NETOPT_DEVICE_TYPE, _dev_get_device_type);

    return gnrc_netif_raw_create(netif, _netif_stack, THREAD_STACKSIZE_DEFAULT,
                                 GNRC_NETIF_PRIO, "dummy_netif", &dev.netdev.netdev);
}

void fuzzing_netdev_wait(void) {
    mutex_lock(&initmtx);
}
