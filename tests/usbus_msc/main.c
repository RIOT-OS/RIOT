/*
 * Copyright (C) 2019-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for demonstrating USBUS MSC implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "mtd.h"
#include "shell.h"
#include "usb/usbus.h"
#include "usb/usbus/msc.h"
#include "ztimer.h"

#ifndef MTD_NUMOF
#define MTD_NUMOF 0
#endif

static char _stack[USBUS_STACKSIZE];
static usbus_t usbus;
static usbus_msc_device_t msc;

static mtd_dev_t *_get_mtd_dev(unsigned idx)
{
    switch (idx) {
#ifdef MTD_0
    case 0: return MTD_0;
#endif
#ifdef MTD_1
    case 1: return MTD_1;
#endif
#ifdef MTD_2
    case 2: return MTD_2;
#endif
#ifdef MTD_3
    case 3: return MTD_3;
#endif
    }

    return NULL;
}

static int _cmd_add_lun(int argc, char **argv)
{
    int dev, ret;
    mtd_dev_t *mtd_dev;

    if (argc < 2) {
        printf("usage: %s <mtd dev>\n", argv[0]);
        puts("\tMTD devices available:");
        for (int i = 0; i < (int)MTD_NUMOF; i++) {
            printf("\t\t%i: MTD_DEV(%i)\n", i, i);
        }
        return -1;
    }

    /* parse the given MTD device */
    dev = atol(argv[1]);
    if (dev < 0 || dev >= (int)MTD_NUMOF) {
        puts("error: invalid MTD device specified");
        return -2;
    }
    mtd_dev = _get_mtd_dev(dev);
    ret = usbus_msc_add_lun(&usbus, mtd_dev);
    if (ret != 0) {
        printf("Cannot add LUN device (error:%d %s)\n", ret, strerror(-ret));
    }
    return 0;
}

static int _cmd_remove_lun(int argc, char **argv)
{
    int dev, ret;
    mtd_dev_t *mtd_dev;

    if (argc < 2) {
        printf("usage: %s <mtd dev>\n", argv[0]);
        puts("\tMTD devices available:");
        for (int i = 0; i < (int)MTD_NUMOF; i++) {
            printf("\t\t%i: MTD_DEV(%i)\n", i, i);
        }
        return -1;
    }

    /* parse the given MTD device */
    dev = atol(argv[1]);
    if (dev < 0 || dev >= (int)MTD_NUMOF) {
        puts("error: invalid MTD device specified");
        return -2;
    }
    mtd_dev = _get_mtd_dev(dev);
    ret = usbus_msc_remove_lun(&usbus, mtd_dev);
    if (ret == -EAGAIN) {
        printf("MTD device was not registered\n");
    }
    return 0;
}

static int _cmd_usb_attach(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const usbopt_enable_t _enable = USBOPT_ENABLE;

    usbdev_set(usbus.dev, USBOPT_ATTACH, &_enable,
               sizeof(usbopt_enable_t));
    return 0;
}

static int _cmd_usb_detach(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static const usbopt_enable_t _enable = USBOPT_DISABLE;

    usbdev_set(usbus.dev, USBOPT_ATTACH, &_enable,
               sizeof(usbopt_enable_t));
    return 0;
}

static int _cmd_usb_reset(int argc, char **argv)
{
    _cmd_usb_detach(argc, argv);
    ztimer_sleep(ZTIMER_MSEC, 100);
    _cmd_usb_attach(argc, argv);
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "add_lun",  "Add a MTD device as new LUN",  _cmd_add_lun },
    { "remove_lun", "Remove existing LUN", _cmd_remove_lun },
    { "usb_attach", "Attach USB to host", _cmd_usb_attach },
    { "usb_detach", "Detach USB from host", _cmd_usb_detach },
    { "usb_reset", "Combine Detach and Attach with a 100ms delay", _cmd_usb_reset },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT USB MSC test application");
    puts("Add one or more MTD devices as USB LUN");
    puts("Then use the attach command to connect");
    puts("your device and start USB operation");

    /* Get driver context */
    usbdev_t *usbdev = usbdev_get_ctx(0);

    assert(usbdev);

    /* Initialize basic usbus struct, don't start the thread yet */
    usbus_init(&usbus, usbdev);

    /* Initialize Mass Storage Class */
    usbus_msc_init(&usbus, &msc);
    /* Create USBUS thread */
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
