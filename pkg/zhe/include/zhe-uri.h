#ifndef ZHE_URI_H
#define ZHE_URI_H

#include <stdbool.h>
#include "zhe-config-deriv.h"

bool zhe_urimatch(const uint8_t *a, size_t asz, const uint8_t *b, size_t bsz);
bool zhe_urivalid(const uint8_t *a, size_t asz);

#endif
