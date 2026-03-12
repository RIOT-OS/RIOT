/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

/**
 * @defgroup pkg_bplib_fwp Default FWP (Framework Proxy)
 * @ingroup pkg_bplib
 * @brief A default implementation of FWP callbacks which are needed for bplib.
 *
 * # About
 * A FWP provides callbacks for certain events happening in bplib.
 *
 * Many of the callbacks are not used, since the configuration of bplib is
 * different in this port from how bplib is used in NASA's bpnode. What this
 * module does provide is the following:
 *
 * ## Time
 * The bundle protocol essentially knows two operating modes:
 * 1. An accurate DTN time clock is present. All timestamps are absolute times,
 *    and no bundle age block will be inserted.
 *    Thus, if a node thinks it has the correct time, but in reality is 10
 *    years late, the bundle will be deleted by a next hop that knows the true
 *    time.
 * 2. No accurate clock is present. A monotinic timer is still required.
 *    A bundle age block will be used to track the bundle age, since there is
 *    no absolute timestamp.
 *
 * When the pseudomodule `bplib_walltime_available` is selected, it is assumed
 * that the first thing is the case and the walltime is set to the current
 * time. In the case the current time can not reliably be maintained, this
 * module should not be selected, then the second operating mode will be used.
 *
 * ## Optimizations
 *
 * bplib adds AS (Admin Statistics), which includes counters of bundle
 * operations (ingress, forwarding and the like). These are several KB large
 * and thus disabled by default in the port. You can enable them again using
 * the `bplib_include_as` pseudomodule.
 *
 * ## Overriding the default FWP
 *
 * bplib_riot_fwp_init() is called in @ref pkg_bplib_init, so if this module
 * is not enabled with `bplib_default_fwp`, a function with the signature of
 * bplib_riot_fwp_init() has to be provided by other means. The header will
 * always be in the include path of the bplib package.
 *
 *
 * @{
 *
 * @file
 * @brief       Default FWP.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

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
 * cache, so they won't get egressed on a possibly different
 * channel / contact.
 *
 * @return return value of BPLib_FWP_Init()
 */
BPLib_Status_t bplib_riot_fwp_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
