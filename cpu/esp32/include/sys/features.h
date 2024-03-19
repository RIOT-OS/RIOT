/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Wrapper for sys/features.h
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is a wrapper for sys/features.h to revert default definitions that
 * lead to compilation problems with newer GCC/newlib versions, see below.
 */

#ifndef SYS_FEATURES_H
#define SYS_FEATURES_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include_next <sys/features.h>

/*
 * When using a GCC version with POSIX thread support enabled, as is the
 * case with Espressif's precompiled toolchains, the `_POSIX_THREAD`
 * definition has to be reverted to prevent the inclusion of newlib's
 * POXIS header files in system headers to avoid compilation errors.
 * The reason is that RIOT uses its own `pthread` implementation, but
 * its type declarations are not fully compatible with those in
 * `sys/_pthreadtypes.h`.
 */
#undef _POSIX_THREADS

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SYS_FEATURES_H */
/** @} */
