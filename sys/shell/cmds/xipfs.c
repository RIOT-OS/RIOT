/*
 * Copyright (C) 2024 Université de Lille
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for XIPFS
 *
 * @author      Gregory Guche <gregory.guche@univ-lille.fr>
 *
 * @}
 */

#if defined(MODULE_XIPFS_FS) || defined(MODULE_XIPFS)
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "fs/xipfs_fs.h"
#include "include/xipfs.h"
#include "shell.h"

static char *execute_file_handler_args[XIPFS_EXEC_ARGC_MAX];

static void print_execute_file_usage(void) {
#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT
    printf("Usage : execute [-p] xipfs_executable_filename [arg0] [arg1] ... [arg%d]\n",
           (XIPFS_EXEC_ARGC_MAX - 1));
    printf("OPTION:\n");
    printf("\t-p\n");
    printf("\t\tRun the file with Memory Protection Unit.\n");
#else
    printf("Usage : execute xipfs_executable_filename [arg0] [arg1] ... [arg%d]\n",
           (XIPFS_EXEC_ARGC_MAX - 1));
#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */
}

static int _execute_file_handler(int argc, char **argv) {
    int exe_filename_arg_pos = 1;
    const char *exe_filename;

    if ( (argc == 1) || (argc > XIPFS_EXEC_ARGC_MAX) ) {
        print_execute_file_usage();
        return 1;
    }

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT
    if ((argv[1][0] == '-') && argv[1][1] == 'p') {
        exe_filename_arg_pos = 2;
    }
#endif

    exe_filename = argv[exe_filename_arg_pos];

    memset(execute_file_handler_args, 0, sizeof(execute_file_handler_args));
    for (int i = exe_filename_arg_pos; i < argc; ++i) {
        execute_file_handler_args[i - exe_filename_arg_pos] = argv[i];
    }

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT
    int ret = (exe_filename_arg_pos == 2) ?
            xipfs_extended_driver_safe_execv(exe_filename, execute_file_handler_args)
        :   xipfs_extended_driver_execv(exe_filename, execute_file_handler_args);
#else
        int ret = xipfs_extended_driver_execv(exe_filename, execute_file_handler_args);
#endif

    if (ret < 0) {
        printf("Failed to execute '%s', error=%d (%s)\n",
               argv[exe_filename_arg_pos], ret, strerror(-ret));
        return 2;
    }

    return 0;
}

SHELL_COMMAND(execute, "Execute an XIPFS file", _execute_file_handler);

static void print_create_executable_file_usage(int argc, char **argv) {
    (void)argc;
    printf("Usage %s xipfs_executable_filename xipfs_executable_file_bytesize\n", argv[0]);
    printf("\t- xipfs_executable_filename      : filename of the desired XIPFS file\n");
    printf("\t- xipfs_executable_file_bytesize : bytesize of the desired XIPFS file\n");
}

static int _create_executable_file(int argc, char **argv) {
    if (argc != 3) {
        print_create_executable_file_usage(argc, argv);
        return 1;
    }

    unsigned long file_size = 0;
    if (argv[2][0] == '-') {
        printf("Error : xipfs_executable_file_bytesize must be positive.\n");
        print_create_executable_file_usage(argc, argv);
        return 2;
    }
    file_size = strtoul(argv[2], NULL, 10);
    if (file_size >= (unsigned long)UINT32_MAX) {
        printf("Error : xipfs_executable_file_bytesize must be less than %lu.\n", UINT32_MAX);
        print_create_executable_file_usage(argc, argv);
        return 3;
    }

    int ret = xipfs_extended_driver_new_file(argv[1], (uint32_t)file_size, 1);
    if (ret != 0) {
        printf("Failed to create '%s' as an XIPFS executable file.", argv[1]);
        return 4;
    }

    return 0;
}

SHELL_COMMAND(create_executable, "Create an XIPFS executable file", _create_executable_file);

#endif /* MODULE_XIPFS_FS */
