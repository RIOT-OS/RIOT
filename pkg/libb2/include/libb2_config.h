#ifndef LIBB2_CONFIG_H
#define LIBB2_CONFIG_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Add no suffix to the functions */
#define SUFFIX

/* Test for a little-endian machine */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NATIVE_LITTLE_ENDIAN
#endif

#ifndef CPU_HAS_UNALIGNED_ACCESS
#define HAVE_ALIGNED_ACCESS_REQUIRED
#endif

#ifdef __cplusplus
}
#endif
#endif /* LIBB2_CONFIG_H */
