/******************************************************************************
 * Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * These sources were developed at the Freie Universitaet Berlin, Computer Systems
 * and Telematics group (http://cst.mi.fu-berlin.de).
 * ----------------------------------------------------------------------------
 * This file is part of RIOT.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
*******************************************************************************/

/**
 * @defgroup    sys_shell Shell
 * @ingroup     sys
 * @brief       Simple shell interpreter
 */

#ifndef __SHELL_H
#define __SHELL_H
#include <stdint.h>

typedef struct shell_command_t {
    char *name;
    char *desc;
    void (*handler)(char *);
} shell_command_t;

typedef struct shell_t {
    const shell_command_t *command_list;
    uint16_t shell_buffer_size;
    int (*readchar)(void);
    void (*put_char)(int);
} shell_t;

/**
 * @brief Initialize a shell object
 * @param shell Pointer to preallocated shell object
 * @param shell_commands Pointer to shell command structure. See test_shell application for example.
 * @param shell_buffer_size The size of the shell buffer.
 * @param read_char Pointer to input device read function. Should return exactly one byte or block.
 * @param put_char Pointer to output funtion. currently unused, shell code will use printf.
 */
void shell_init(shell_t *shell,  /*@null@*/ const shell_command_t *shell_commands,
                uint16_t shell_buffer_size, int(*read_char)(void), void (*put_char)(int));

/**
 * @brief Endless loop that waits for command and executes handler.
 */
void shell_run(shell_t *shell);


void shell_auto_init(shell_t *shell);

#endif /* __SHELL_H */
