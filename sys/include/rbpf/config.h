/*
 * Copyright (C) 2021 Inria
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RBPF_CONFIG_H
#define RBPF_CONFIG_H

#ifndef RBPF_ENABLE_ALU32
#define RBPF_ENABLE_ALU32 (1)
#endif

#ifndef RBPF_BRANCHES_ALLOWED
#define RBPF_BRANCHES_ALLOWED 10000
#endif


#ifndef RBPF_EXTERNAL_CALLS
static inline rbpf_call_t rbpf_get_external_call(uint32_t num)
{
    (void)num;
    return NULL;
}
#endif

#endif /* RBPF_CONFIG_H */
