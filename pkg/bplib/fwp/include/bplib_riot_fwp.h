/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include "bplib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the FWP callbacks for bplib.
 *
 * Notably provides callbacks for time, using ztimer64 and,
 * if enabled with the `bplib_walltime_available` module,
 * walltime. Also notified the VFS Storage when a contact
 * ends or a channel is closed, to flush the bundle in the
 * cache, so they wont get egressed on a possibly different
 * channel / contact.
 *
 * @return return value of BPLib_FWP_Init()
 */
BPLib_Status_t bplib_riot_fwp_init(void);

#ifdef __cplusplus
}
#endif
