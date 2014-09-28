#ifndef RIOT__NATIVE__SYS_TYPES_H__
#define RIOT__NATIVE__SYS_TYPES_H__

#include "stddef.h"

#include <stdint.h>
#include <time.h>

typedef __UINT8_TYPE__  u_int8_t;
typedef __UINT16_TYPE__ u_int16_t;
typedef __UINT32_TYPE__ u_int32_t;
typedef __UINT64_TYPE__ u_int64_t;

typedef signed   ssize_t;
typedef ssize_t  off_t;
typedef int      pid_t;

#endif /* ifndef RIOT__NATIVE__SYS_TYPES_H__ */
