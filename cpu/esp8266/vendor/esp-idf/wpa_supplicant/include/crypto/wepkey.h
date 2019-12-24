/*
 * wepkey.h - Generate WEP keys from a passphrase
 *
 * Copyright (C) 2008 by OpenMoko, Inc.
 * Written by Werner Almesberger <werner@openmoko.org>
 * All Rights Reserved
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/*
 * wpkey_64 and wepkey_128 implement the keyphrase hash algorithm found in many
 * (but not all) common access points, including the Linksys WRT54G series.
 */

#ifndef WEPKEY_H
#define WEPKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "c_types.h"


#define WEPKEY_64_BYTES		5
#define WEPKEY_128_BYTES	13


/*
 * "size" is the size of the buffer at "out", in bytes. It has to be at least 5
 * or 13 bytes, respectively. "n" is the key index, in the range 0...3.
 */

size_t wepkey_64(uint8_t *out, size_t size, const char *in, int n);
size_t wepkey_128(uint8_t *out, size_t size, const char *in, int n);

#ifdef __cplusplus
}
#endif

#endif /* !WEPKEY_H */
