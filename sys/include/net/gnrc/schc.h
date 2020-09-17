#ifndef NET_GNRC_SCHC_H
#define NET_GNRC_SCHC_H

#include <stdbool.h>

#include "kernel_types.h"

#include "net/gnrc/schc/config.h"

#ifdef __cplusplus
extern "C" {
#endif

kernel_pid_t gnrc_schc_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_SIXLOWPAN_H */
/** @} */
