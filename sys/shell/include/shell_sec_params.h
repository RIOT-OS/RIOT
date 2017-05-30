#ifndef SHELL_SEC_PARAMS_H
#define SHELL_SEC_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef SHELL_SEC_DEFAULT_USER
#define SHELL_SEC_DEFAULT_USER   { "riot", \
                                   "admin" }
#endif

#ifndef SHELL_SEC_DEFAULT_LEVEL
#define SHELL_SEC_DEFAULT_LEVEL  { 1, \
                                   2 }
#endif

#ifndef SHELL_SEC_DEFAULT_PASSWD
#define SHELL_SEC_DEFAULT_PASSWD { {0x2f, 0x53, 0x08, 0x65, 0x55, 0xb7, 0xfb, 0xb9, 0x40, 0xce, 0x78, 0x61, 0x6f, 0xf2, 0x12, 0xe5}, \
                                   {0x21, 0x23, 0x2f, 0x29, 0x7a, 0x57, 0xa5, 0xa7, 0x43, 0x89, 0x4a, 0x0e, 0x4a, 0x80, 0x1f, 0xc3} }
#endif

static const char *shell_sec_users[] = SHELL_SEC_DEFAULT_USER;
static const int shell_sec_levels[] = SHELL_SEC_DEFAULT_LEVEL;

#define SHELL_SEC_USERS_NUMOF (sizeof(shell_sec_users) / sizeof(shell_sec_users[0]))

static const uint8_t shell_sec_passwds[SHELL_SEC_USERS_NUMOF][16] = SHELL_SEC_DEFAULT_PASSWD;

#ifdef __cplusplus
}
#endif

#endif /* SHELL_SEC_PARAMS_H */
