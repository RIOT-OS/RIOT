/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Shows the use of the RIOT registry for handling runtime
 *              configurations using multiple store options.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include <stdio.h>
#include <string.h>
#include "errno.h"
#include "shell.h"
#include "board.h"
#include "registry/registry.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_REGISTRY_STORE_DUMMY
#include "dummy/include/registry_store_dummy.h"
registry_dummy_t registry_dummy_storage;
#endif /* MODULE_REGISTRY_STORE_DUMMY */

/* Size of the test_bytes configuration parameter */
#ifndef BYTES_LENGTH
#define BYTES_LENGTH    16
#endif

/* These are the 'configuration parameters' */
int64_t test_opt1 = 0;
int8_t test_opt2 = 1;
unsigned char test_bytes[BYTES_LENGTH] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
                                          0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
                                          0xAA, 0xAA, 0xAA, 0xAA};

char *get_handler(int argc, char **argv, char *val, int val_len_max, void *ctx)
{
    (void)ctx;

    if (argc) {
        if (!strcmp("opt1", argv[0])) {
        return registry_str_from_value(REGISTRY_TYPE_INT64, (void *)&test_opt1,
                                       val, val_len_max);
        }
        else if (!strcmp("opt2", argv[0])) {
            return registry_str_from_value(REGISTRY_TYPE_INT8, (void *)&test_opt2,
                                        val, val_len_max);
        }
        else if (!strcmp("bytes", argv[0])) {
            return registry_str_from_bytes((void *)test_bytes, BYTES_LENGTH,
                                           val, val_len_max);

        }
    }
    return NULL;
}

int set_handler(int argc, char **argv, char *val, void *ctx)
{
    (void)ctx;
    const char buf[BYTES_LENGTH];
    int len = sizeof(test_bytes);

    if (argc) {
        DEBUG("[set_handler] Setting %s to %s\n", argv[0], val);
        if (!strcmp("opt1", argv[0])) {
            return registry_value_from_str(val, REGISTRY_TYPE_INT64,
                                           (void *) &test_opt1, 0);
        }
        else if (!strcmp("opt2", argv[0])) {
            return registry_value_from_str(val, REGISTRY_TYPE_INT8,
                                           (void *) &test_opt2, 0);
        }
        else if (!strcmp("bytes", argv[0])) {
            int res = registry_bytes_from_str(val, (void *)buf, &len);
            if (res) {
                DEBUG("[set_handler] Error while parsing base64");
                return res;
            }

            if (len != BYTES_LENGTH) {
                DEBUG("[set_handler] Error less bytes than needed were passed");
                return -1;
            }
            memcpy((void *)test_bytes, (void *)buf, BYTES_LENGTH);
        }
    }
    return -1;
}

int export_handler(int (*export_func)(const char *name, char *val), int argc,
                   char **argv, void *ctx)
{
    (void)ctx;
    char buf[REGISTRY_MAX_VAL_LEN];

    if (!argc) {
        int res = 0;
        // We have to print every parameter
        if (registry_str_from_value(REGISTRY_TYPE_INT64, (void *)&test_opt1,
                                    buf, sizeof(buf))) {
            if ((res = export_func("app/opt1", buf))) {
                return res;
            }
        }

        if (registry_str_from_value(REGISTRY_TYPE_INT8, (void *)&test_opt2, buf,
                                    sizeof(buf))) {
            if ((res = export_func("app/opt2", buf))) {
                return res;
            }
        }

        if (registry_str_from_bytes((void *)test_bytes, BYTES_LENGTH, buf,
                                    sizeof(buf))) {
            if ((res = export_func("app/bytes", buf))) {
                return res;
            }
        }
        return 0;
    }
    else {
        if (!strcmp("opt1", argv[0])) {
            if (registry_str_from_value(REGISTRY_TYPE_INT64, (void *)&test_opt1,
                                        buf, sizeof(buf))) {
                return export_func("app/opt1", buf);
            }
        }
        else if (!strcmp("opt2", argv[0])) {
            if (registry_str_from_value(REGISTRY_TYPE_INT8, (void *)&test_opt2, buf,
                                        sizeof(buf))) {
                return export_func("app/opt2", buf);
            }
        }
        else if (!strcmp("bytes", argv[0])) {
            if (registry_str_from_bytes((void *)test_bytes, BYTES_LENGTH, buf,
                                        sizeof(buf))) {
                return export_func("app/bytes", buf);
            }
        }
    }
    return -EINVAL;
}

/* Helper function to print configuration parameters */
int print_parameter(const char *name, char *val)
{
    printf("%s = %s\n", name, val);
    return 0;
}

/* This is the registry handler for the configuration parameters of the
 * application. It has an unique name and handler functions for the different
 * acctions required. */
registry_handler_t handler = {
    .name = "app",
    .hndlr_get = get_handler,
    .hndlr_set = set_handler,
    .hndlr_export = export_handler
};

int cmd_get(int argc, char **argv)
{
    char buf[REGISTRY_MAX_VAL_LEN];
    int buf_len = REGISTRY_MAX_VAL_LEN;
    char *res;

    if (argc != 2) {
        (void) argv;
        printf("usage: %s <param>\n", argv[0]);
        return 1;
    }

    res = registry_get_value(argv[1], buf, buf_len);

    if (res == NULL) {
        DEBUG("Error: Parameter does not exist\n");
        return 1;
    }
    DEBUG("%s\n", buf);
    return 0;
}

int cmd_list(int argc, char **argv)
{
    if (argc == 1) {
        registry_export(print_parameter, NULL);
    }
    else if (argc == 2) {
        registry_export(print_parameter, argv[1]);
    }
    else {
        printf("usage: %s [module:parameter]\n", argv[0]);
    }
    return 0;
}

int cmd_set(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage: %s <param> <value>\n", argv[0]);
        return 1;
    }
    return registry_set_value(argv[1], argv[2]);
}

int cmd_save(int argc, char **argv)
{
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    return registry_save();
}

int cmd_load(int argc, char **argv)
{
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    return registry_load();
}

static void _dump_cb(char *name, char *val, void *cb_arg)
{
    (void)cb_arg;
    printf("%s \t %s\n", name, val);
}

int cmd_dump(int argc, char **argv)
{
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    DEBUG("Dumping storage...\n");

    /* This is just for debuging pourposes, storage facilities are not meant to
     * be called directly */
#if defined(MODULE_REGISTRY_STORE_DUMMY)
    registry_dummy_storage.store.itf->load(&registry_dummy_storage.store,
                                           _dump_cb, NULL);
#else
    printf("ERROR: No store defined\n");
    return 1;
#endif
    return 0;
}

static int cmd_test_bytes(int argc, char **argv) {
    if (argc != 1) {
        printf("usage: %s\n", argv[0]);
        return 1;
    }
    printf("Sizeof test_bytes: %zu\n", sizeof(test_bytes));
    for (unsigned int i = 0; i < sizeof(test_bytes); i++) {
        printf("%2x ",test_bytes[i]);
    }
    puts("");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "get", "get a parameter value", cmd_get },
    { "set", "set a parameter value", cmd_set },
    { "list", "list parameters", cmd_list },
    { "save", "save all parameters", cmd_save },
    { "load", "load stored configurations", cmd_load },
    { "dump", "dumps everything in storage", cmd_dump },
    { "test_bytes", "dumps the bytes variable in HEX", cmd_test_bytes },
    { NULL, NULL, NULL }
};

int main(void)
{

    /* Initialize the 'registry' module */
    registry_init();

    /* Register the handler for the configuration parameters */
    registry_register(&handler);

    /* Register the correspondant storage facility */
#if defined(MODULE_REGISTRY_STORE_DUMMY)
    DEBUG("Using dummy registry store\n");
    registry_dummy_src(&registry_dummy_storage);
    registry_dummy_dst(&registry_dummy_storage);
#else
#error "You should choose a store for registry"
#endif

    /* Load any existing configurations from the storage */
    registry_load();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
