/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_entropy_source_zero
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include "entropy_source.h"
#include "entropy_source/zero_entropy.h"

entropy_source_tests_rep_t zero_state_rep;
entropy_source_tests_prop_t zero_state_prop;

static int _get_sample(uint8_t *out)
{
    int ret = ENTROPY_SOURCE_OK;
    uint8_t byte = 0;

    *out = byte;

    if (IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ZERO_HEALTH_TEST)) {
        ret = entropy_source_test(&zero_state_rep, &zero_state_prop, byte);
    }

    return ret;
}

int entropy_source_zero_init(void)
{
    if (IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ZERO_HEALTH_TEST)) {
        unsigned int cutoff;
        cutoff = entropy_source_test_rep_cutoff(CONFIG_ENTROPY_SOURCE_ZERO_HMIN);
        entropy_source_test_rep_init(&zero_state_rep, cutoff);
        cutoff = entropy_source_test_prop_cutoff(CONFIG_ENTROPY_SOURCE_ZERO_HMIN);
        entropy_source_test_prop_init(&zero_state_prop, cutoff);
    }

    return ENTROPY_SOURCE_OK;
}

int entropy_source_zero_get(uint8_t *out, size_t len)
{
    assert(out != NULL);

    int ret = ENTROPY_SOURCE_OK;

    if (IS_ACTIVE(CONFIG_ENTROPY_SOURCE_ZERO_COND)) {
        ret = entropy_source_neumann_unbias(_get_sample, out, len);
    }
    else {
        for (unsigned iter = 0; iter < len; iter++) {
            int tmp = _get_sample(&out[iter]);
            /* Remember the worst failure during
             * sampling multiple values to return */
            if (tmp < ret) {
                ret = tmp;
            }
        }
    }
    return ret;
}
