/*
 * Copyright (C) 2021 Inria
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_rbpf
 * @brief       Helpers shared between the host and the virtual machine
 * @experimental
 */

#ifndef RBPF_SHARED_H
#define RBPF_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Macro type to ensure consistent pointer size between host and virtual machine in structs
 *
 * @param type  Type of the pointer
 * @param name  Name of the pointer
 */
#define __bpf_shared_ptr(type, name)    \
    union {                 \
        type name;          \
        uint64_t : 64;          \
    } __attribute__((aligned(8)))


#ifdef __cplusplus
}
#endif
#endif /* RBPF_SHARED_H */
