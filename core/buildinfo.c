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
 * This compilation unit will be a ccache miss unless there is
 * a cache entry for the exact same build target (board + app +
 * RIOT version tuple). But compilation units just linking to the
 * symbols provided here do get a cache to have the same ccache
 * hash key. As this file is trivial to compile, this is a net win.
 * @}
 */

#include "buildinfo/appinfo.h"
#include "buildinfo/boardinfo.h"
#include "buildinfo/cpuinfo.h"
#include "buildinfo/riotver.h"

const char *buildinfo_app_name = RIOT_APPLICATION_LITERAL;
const char *buildinfo_board_name = RIOT_BOARD_LITERAL;
const char *buildinfo_cpu_name = RIOT_CPU_LITERAL;
const char *buildinfo_riot_version = RIOT_VERSION_LITERAL;
