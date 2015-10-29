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
    SIMPLELINK_START_ERROR,
    SMARTCONFIG_ERROR
};


#ifdef ERROR_THREAD

#define ERRORS_THREAD_INIT() errors_thread_init()

#define THROW(ERROR)    throw(ERROR)

#else

#define ERRORS_THREAD_INIT()

#define THROW(ERROR)  puts(sl_err_descr[ERROR]); while(1) {}

#endif

void errors_thread_init(void);

void throw(short err_type);


#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* THROW_H_ */
