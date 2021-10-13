/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       SUIT updates over CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "thread.h"
#include "irq.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"

#include "shell.h"

#include "suit/transport/coap.h"
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
#include "riotboot/slot.h"
#endif

#include "suit/storage.h"
#include "suit/storage/ram.h"

#ifdef MODULE_PERIPH_GPIO
#include "periph/gpio.h"
#endif

#define COAP_INBUF_SIZE (256U)

/* Extend stacksize of nanocoap server thread */
static char _nanocoap_server_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#define NANOCOAP_SERVER_QUEUE_SIZE     (8)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static void *_nanocoap_server_thread(void *arg)
{
    (void)arg;

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    return NULL;
}

/* assuming that first button is always BTN0 */
#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void cb(void *arg)
{
    (void) arg;
    printf("Button pressed! Triggering suit update! \n");
    suit_coap_trigger((uint8_t *) SUIT_MANIFEST_RESOURCE, sizeof(SUIT_MANIFEST_RESOURCE));
}
#endif

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
static int cmd_print_riotboot_hdr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int current_slot = riotboot_slot_current();
    if (current_slot != -1) {
        /* Sometimes, udhcp output messes up the following printfs.  That
         * confuses the test script. As a workaround, just disable interrupts
         * for a while.
         */
        unsigned state = irq_disable();
        riotboot_slot_print_hdr(current_slot);
        irq_restore(state);
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }
    return 0;
}

static int cmd_print_current_slot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    /* Sometimes, udhcp output messes up the following printfs.  That
     * confuses the test script. As a workaround, just disable interrupts
     * for a while.
     */
    unsigned state = irq_disable();
    printf("Running from slot %d\n", riotboot_slot_current());
    irq_restore(state);
    return 0;
}
#endif

static int cmd_print_slot_content(int argc, char **argv)
{
    char *slot;
    uint32_t offset;
    size_t len;

    if (argc < 4) {
        printf("usage: %s <storage_id> <addr> <len>\n", argv[0]);
        return -1;
    }

    slot = argv[1];
    offset = atoi(argv[2]);
    len  = atoi(argv[3]);

    suit_storage_t *storage = suit_storage_find_by_id(slot);
    if (!storage) {
        printf("No storage with id \"%s\" present\n", slot);
        return -1;
    }

    suit_storage_set_active_location(storage, slot);

    if (suit_storage_has_readptr(storage)) {
        const uint8_t *buf;
        size_t available;
        suit_storage_read_ptr(storage, &buf, &available);

        size_t to_print = available < offset + len ? available - offset : len;
        for (size_t i = offset; i < to_print; i++) {
            print_byte_hex(buf[i]);
        };
        puts("");
    }

    return 0;
}

static int cmd_lsstorage(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (IS_ACTIVE(MODULE_SUIT_STORAGE_RAM)) {
        for (unsigned i = 0; i < CONFIG_SUIT_STORAGE_RAM_REGIONS; i++) {
            printf("RAM slot %u: \"%s%u\"\n", i,
                    CONFIG_SUIT_STORAGE_RAM_LOCATION_PREFIX, i);
        }
    }
    if (IS_ACTIVE(MODULE_SUIT_STORAGE_FLASHWRITE)) {
        puts("Flashwrite slot 0: \"\"\n");
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    { "current-slot", "Print current slot number", cmd_print_current_slot },
    { "riotboot-hdr", "Print current slot header", cmd_print_riotboot_hdr },
#endif
    { "storage_content", "Print the slot content", cmd_print_slot_content },
    { "lsstorage", "Print the available storage paths", cmd_lsstorage },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT SUIT update example application");

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
    /* initialize a button to manually trigger an update */
    gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING, cb, NULL);
#endif

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    cmd_print_current_slot(0, NULL);
    cmd_print_riotboot_hdr(0, NULL);
#endif

    /* start suit coap updater thread */
    suit_coap_run();

    /* start nanocoap server thread */
    thread_create(_nanocoap_server_stack, sizeof(_nanocoap_server_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _nanocoap_server_thread, NULL, "nanocoap server");

    /* the shell contains commands that receive packets via GNRC and thus
       needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Starting the shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
