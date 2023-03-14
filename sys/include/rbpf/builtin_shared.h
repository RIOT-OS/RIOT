/*
 * Copyright (C) 2021 Inria
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RBPF_BUILTIN_SHARED_H
#define RBPF_BUILTIN_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /* Key/value store functions */
    BPF_FUNC_BPF_STORE_LOCAL    = 0x10,
    BPF_FUNC_BPF_STORE_GLOBAL   = 0x11,
    BPF_FUNC_BPF_FETCH_LOCAL    = 0x12,
    BPF_FUNC_BPF_FETCH_GLOBAL   = 0x13,
};

#ifdef __cplusplus
}
#endif
#endif /* RBPF_BUILTIN_SHARED_H */
