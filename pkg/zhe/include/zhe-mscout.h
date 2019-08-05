#ifndef ZHE_MSCOUT_H
#define ZHE_MSCOUT_H

#include <stdint.h>

#define MSCOUT_MAX_SIZE 2
zhe_msgsize_t zhe_make_mscout(uint8_t buf[MSCOUT_MAX_SIZE], size_t bufsz);

#endif
