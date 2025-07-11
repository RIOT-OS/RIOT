/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include "shell.h"

#include "suit/transport/coap.h"
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
#include "riotboot/slot.h"
#endif

#include "suit/storage.h"
#include "suit/storage/ram.h"
#ifdef CPU_NATIVE
#include "suit/storage/vfs.h"
#include "xfa.h"
#include "vfs_default.h"
#endif

#ifdef MODULE_PERIPH_GPIO
#include "board.h"
#include "periph/gpio.h"
#endif

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* add handled storages */
#if IS_USED(MODULE_SUIT_STORAGE_VFS)
XFA_USE(char*, suit_storage_files_reg);
#ifdef CPU_NATIVE
XFA(char*, suit_storage_files_reg, 0) _slot0 = VFS_DEFAULT_DATA "/SLOT0.txt";
XFA(char*, suit_storage_files_reg, 1) _slot1 = VFS_DEFAULT_DATA "/SLOT1.txt";
#endif
#endif

/* assuming that first button is always BTN0 */
#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void cb(void *arg)
{
    (void) arg;
    printf("Button pressed! Triggering suit update! \n");
    suit_worker_trigger(SUIT_MANIFEST_RESOURCE, sizeof(SUIT_MANIFEST_RESOURCE));
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

SHELL_COMMAND(riotboot_hdr, "Print current slot header", cmd_print_riotboot_hdr);

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

SHELL_COMMAND(current_slot, "Print current slot number", cmd_print_current_slot);

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

SHELL_COMMAND(storage_content, "Print the slot content", cmd_print_slot_content);

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
#if IS_USED(MODULE_SUIT_STORAGE_VFS)
    for (unsigned i = 0; i < XFA_LEN(char **, suit_storage_files_reg); i++) {
        const char *filepath = (const char *)suit_storage_files_reg[i];
        printf("VFS %u: \"%s\"\n", i, filepath);
    }
#endif

    return 0;
}

SHELL_COMMAND(lsstorage, "Print the available storage paths", cmd_lsstorage);

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
    /* initialize suit storage */
    suit_storage_init_all();

    /* the shell contains commands that receive packets via GNRC and thus
       needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Starting the shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
