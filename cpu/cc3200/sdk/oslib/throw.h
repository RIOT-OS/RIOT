/*
 * throw.h
 *
 */

#ifndef THROW_H_
#define THROW_H_

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* sl_err_descr[];


enum error_types {
    NO_ERROR = 0,
    GEN_ERROR,
    SIMPLELINK_START_ERROR,
    SMARTCONFIG_ERROR,
    WLAN_SET_POLICY_ERROR,
    SOCKET_OPEN_FAIL,
    SOCKET_CONNECT_FAIL
};


enum {
    SOCKET_CONNECTED = 100,
};

#ifdef ERROR_THREAD

#define ERRORS_THREAD_INIT() errors_thread_init()

#define THROW(ERROR)    throw(ERROR, 0)
#define THROW2(ERROR_ID, ERROR_VALUE)    throw(ERROR_ID, ERROR_VALUE)

#else

#define ERRORS_THREAD_INIT()

#define PANIC(ERROR_ID)  puts(sl_err_descr[ERROR_ID]); while(1) {}

#define THROW(ERROR_ID)  puts(sl_err_descr[ERROR_ID]); while(1) {}
#define THROW2(ERROR_ID, ERROR_VALUE)  printf("%s (%ld)", sl_err_descr[ERROR], ERROR_VALUE); while(1) {}

#endif

void errors_thread_init(void);

void throw(short err_type, long err_value);


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* THROW_H_ */
