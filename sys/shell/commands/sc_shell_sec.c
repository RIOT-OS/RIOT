
#include <string.h>
#include <stdio.h>

#include "shell_sec.h"
#include "shell_commands.h"

static int _level;
static shell_command_handler_t _login_cb = NULL;
static shell_command_handler_t _passwd_cb = NULL;
static shell_command_handler_t _logout_cb = NULL;

#ifdef MODULE_SHELL_SEC_DEFAULT
#include "shell_sec_params.h"
#include "hashes/md5.h"

static int logged_user = -1;
static int logging_user = -1;
#endif

int _login_handler(int argc, char **argv)
{
    if (_login_cb != NULL) {
        return _login_cb(argc - 1, &argv[1]);
    }
#ifdef MODULE_SHELL_SEC_DEFAULT
    else {
        if (argc >= 2) {
            for (int i = 0; i < (int) SHELL_SEC_USERS_NUMOF; i++) {
                if (strcmp(argv[1], shell_sec_users[i]) == 0) {
                    logging_user = i;
                    return 0;
                }
            }
        }
        logging_user = -1;
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
#ifdef MODULE_SHELL_SEC_DEFAULT
    else {
        if (logging_user < 0) {
            puts("Incorrect user or password");
            return 1;
        }
        if (argc >= 2) {
            uint8_t digest[16];
            md5(digest, argv[1], strlen(argv[1]));
            if (memcmp(digest, shell_sec_passwds[logging_user], 16) == 0) {
                if (logged_user != -1) {
                    printf("Goodbye %s\n", shell_sec_users[logged_user]);
                }
                _level = shell_sec_levels[logging_user];
                logged_user = logging_user;
                logging_user = -1;
                printf("Welcome %s\n", shell_sec_users[logged_user]);
                return 0;
            }
        }
    }
    logging_user = -1;
#endif

    puts("Incorrect user or password");
    return 1;
}

int _logout_handler(int argc, char **argv)
{
    if (_logout_cb != NULL) {
        return _logout_cb(argc - 1, &argv[1]);
    }
#ifdef MODULE_SHELL_SEC_DEFAULT
    else {
        printf("Goodbye %s\n", shell_sec_users[logged_user]);
        logged_user = -1;
        _level = 0;
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
