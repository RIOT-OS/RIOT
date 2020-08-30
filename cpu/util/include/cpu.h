/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @defgroup    cpu_util    Dummy/No-op CPU implementation for native utilities
 * @details     The util CPU allows to use the RIOT build system for native
 *              application without attempting to simulate hardware.
 *
 * @{
 *
 * @file
 * @brief       Dummy file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifndef CPU_H
#define CPU_H

#ifdef __cplusplus
extern "C" {
#endif

static inline void cpu_print_last_instruction(void)
{
    /* no-op */
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
