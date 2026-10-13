/*
 * SPDX-FileCopyrightText: ???
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-tlv.h"

#include <string.h>
#include "errno.h"
#include "tlv.h"

extern void tests_tlv_simple(void);
extern void tests_tlv_ber(void);

void tests_tlv(void)
{
    tests_tlv_simple();
    tests_tlv_ber();
}
