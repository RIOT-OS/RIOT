/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       shows how to set up and use shell_sec
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 */

#include <stdio.h>
#include <string.h>

#include "shell_commands.h"
#include "shell_sec.h"
#include "shell.h"

#define NB_USER     2
#define USER_LENGTH 10

static char _user[10];

static char user_list[NB_USER][USER_LENGTH] = {
    "user",
    "admin",
};

static char passwd_list[NB_USER][USER_LENGTH] = {
    "passwd1",
    "passwd2",
};

static int login(int argc, char **argv)
{
    if (!argc) {
        puts("No username provided");
        return 1;
    }

    printf("Login for user %s\n", argv[0]);
    strncpy(_user, argv[0], USER_LENGTH - 1);

    return 0;
}

static int passwd(int argc, char **argv)
{
    if (_user[0] == '\0') {
        puts("Please enter a username");
        return 1;
    }
    if (!argc) {
        puts("Invalid password");
        return 1;
    }
    for (int i = 0; i < NB_USER; i++) {
        if (strcmp(user_list[i], _user) == 0) {
            if (strcmp(passwd_list[i], argv[0]) == 0) {
                shell_sec_set_level(i + 1);
                printf("User %s logged in successfully\n", argv[0]);
                return 0;
            }
            else {
                puts("Invalid user or password");
                return 1;
            }
        }
    }

    puts("Invalid user or password");
    return 1;
}

static int logout(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int level = shell_sec_get_level();
    level--;
    if (level < 0) {
        level = 0;
    }
    shell_sec_set_level(level);

    _user[0] = '\0';
    puts("Logged out");

    return 0;
}

static int unsecure_command(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("This is an unsecure command");

    return 0;
}

static int normal_command(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("This is a normal command");

    return 0;
}

static int admin_command(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    puts("This is an admin command");

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "unsecure", "unsecure command example", unsecure_command, 0},
    { "normal", "normal command example", normal_command, 1},
    { "admin", "admin command example", admin_command, 2},
    { NULL, NULL, NULL, 0},
};

int main(void)
{

    printf("test_shell_sec.\n");

    shell_sec_set_cb(login, passwd, logout);
    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
