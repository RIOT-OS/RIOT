#ifndef RIOT__NATIVE__SYS_TYPES_H__
#define RIOT__NATIVE__SYS_TYPES_H__

#include "stddef.h"

#include <stdint.h>
#include <time.h>

#ifndef __UINT8_TYPE__
#   define __UINT8_TYPE__ unsigned char
#endif
#ifndef __UINT16_TYPE__
#   define __UINT16_TYPE__ unsigned short
#endif
#ifndef __UINT32_TYPE__
#   define __UINT32_TYPE__ unsigned long
#endif
#ifndef __UINT64_TYPE__
#   define __UINT64_TYPE__ unsigned long long
#endif

typedef __UINT8_TYPE__  u_int8_t;
typedef __UINT16_TYPE__ u_int16_t;
typedef __UINT32_TYPE__ u_int32_t;
typedef __UINT64_TYPE__ u_int64_t;

typedef signed   ssize_t;
typedef ssize_t  off_t;
typedef int      pid_t;

#endif /* ifndef RIOT__NATIVE__SYS_TYPES_H__ */
