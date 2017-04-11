
#include <string.h>
#include <stdio.h>

#include "shell_sec.h"
#include "shell_sec_params.h"
#include "shell_commands.h"

static int _level;
static shell_command_handler_t _login_cb = NULL;
static shell_command_handler_t _passwd_cb = NULL;
static shell_command_handler_t _logout_cb = NULL;

#if SHELL_SEC_USE_DEFAULT
static int user_ok;
#endif

int _login_handler(int argc, char **argv)
{
    if (_login_cb != NULL) {
        return _login_cb(argc - 1, &argv[1]);
    }
#if SHELL_SEC_USE_DEFAULT
    else {
        if (argc >= 2 && strcmp(argv[1], SHELL_SEC_DEFAULT_USER) == 0) {
            user_ok = 1;
            return 0;
        }
        user_ok = 0;
        return 1;
    }
#endif

    return 1;
}

int _passwd_handler(int argc, char **argv)
{
    if (_passwd_cb != NULL) {
        return _passwd_cb(argc - 1, &argv[1]);
    }
#if SHELL_SEC_USE_DEFAULT
    else {
        if (!user_ok) {
            puts("Please enter a username");
            return 1;
        }
        if (argc >= 2 && strcmp(argv[1], SHELL_SEC_DEFAULT_PASSWD) == 0) {
            _level = 1;
            printf("Welcome %s\n", SHELL_SEC_DEFAULT_USER);
            return 0;
        }
    }
#endif

    puts("Incorrect user or password");
    return 1;
}

int _logout_handler(int argc, char **argv)
{
    if (_logout_cb != NULL) {
        return _logout_cb(argc - 1, &argv[1]);
    }
#if SHELL_SEC_USE_DEFAULT
    else {
        user_ok = 0;
        _level = 0;
        printf("Goodbye %s\n", SHELL_SEC_DEFAULT_USER);
    }
#endif
    return 0;
}

void shell_sec_set_cb(shell_command_handler_t login_cb, shell_command_handler_t passwd_cb, shell_command_handler_t logout_cb)
{
    _login_cb = login_cb;
    _passwd_cb = passwd_cb;
    _logout_cb = logout_cb;
}

void shell_sec_set_level(int level)
{
    _level = level;
}

int shell_sec_get_level(void)
{
    return _level;
}
