/*
 * SPDX-FileCopyrightText: 2025 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     core
 * @{
 *
 * @file
 * @brief       Access to buildinfo constants in a ccache friendly way
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @details     This compilation unit will be a ccache miss unless there is
 *              a cache entry for the exact same build target (board + app +
 *              RIOT version tuple). But compilation units just linking to the
 *              symbols provided here do get a cache to have the same ccache
 *              hash key. As this file is trivial to compile, this is a net
 *              win.
 * @}
 */

#include "buildinfo/app.h"
#include "buildinfo/board.h"
#include "buildinfo/cpu.h"
#include "buildinfo/riotver.h"

const char *buildinfo_app_name = RIOT_APPLICATION;
const char *buildinfo_board_name = RIOT_BOARD;
const char *buildinfo_cpu_name = RIOT_CPU;
const char *buildinfo_riot_version = RIOT_VERSION;
