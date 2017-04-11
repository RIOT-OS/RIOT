#ifndef SHELL_SEC_PARAMS_H
#define SHELL_SEC_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SHELL_SEC_USE_DEFAULT
#define SHELL_SEC_USE_DEFAULT 0
#endif

#if SHELL_SEC_USE_DEFAULT
#ifndef SHELL_SEC_DEFAULT_USER
#define SHELL_SEC_DEFAULT_USER "admin"
#endif

#ifndef SHELL_SEC_DEFAULT_PASSWD
#define SHELL_SEC_DEFAULT_PASSWD "pass"
#endif
#endif /* SHELL_SEC_USE_DEFAULT */

#ifdef __cplusplus
}
#endif

#endif /* SHELL_SEC_PARAMS_H */
