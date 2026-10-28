/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Compatibility definitions for Xtensa special register names
 *
 * Since ESP-IDF v6.0, the Xtensa special register names are defined with the
 * prefix `XT_REG_` in `xtensa/config/xt_specreg.h`, while the names without
 * the prefix in `xtensa/config/specreg.h` are deprecated. The vendor code
 * in this directory is shared by all Xtensa-based ESPs and uses the names
 * without prefix. This header maps the names without prefix to the new
 * names if they are not already defined.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef DOXYGEN

#if defined(XT_REG_CCOMPARE_0) && !defined(CCOMPARE_0)
#  define CCOMPARE_0 XT_REG_CCOMPARE_0
#endif
#if defined(XT_REG_CCOMPARE_1) && !defined(CCOMPARE_1)
#  define CCOMPARE_1 XT_REG_CCOMPARE_1
#endif
#if defined(XT_REG_CCOMPARE_2) && !defined(CCOMPARE_2)
#  define CCOMPARE_2 XT_REG_CCOMPARE_2
#endif
#if defined(XT_REG_CCOUNT) && !defined(CCOUNT)
#  define CCOUNT XT_REG_CCOUNT
#endif
#if defined(XT_REG_CPENABLE) && !defined(CPENABLE)
#  define CPENABLE XT_REG_CPENABLE
#endif
#if defined(XT_REG_DEPC) && !defined(DEPC)
#  define DEPC XT_REG_DEPC
#endif
#if defined(XT_REG_EPC_1) && !defined(EPC_1)
#  define EPC_1 XT_REG_EPC_1
#endif
#if defined(XT_REG_EPC_2) && !defined(EPC_2)
#  define EPC_2 XT_REG_EPC_2
#endif
#if defined(XT_REG_EPC_3) && !defined(EPC_3)
#  define EPC_3 XT_REG_EPC_3
#endif
#if defined(XT_REG_EPC_4) && !defined(EPC_4)
#  define EPC_4 XT_REG_EPC_4
#endif
#if defined(XT_REG_EPC_5) && !defined(EPC_5)
#  define EPC_5 XT_REG_EPC_5
#endif
#if defined(XT_REG_EPC_6) && !defined(EPC_6)
#  define EPC_6 XT_REG_EPC_6
#endif
#if defined(XT_REG_EPC_7) && !defined(EPC_7)
#  define EPC_7 XT_REG_EPC_7
#endif
#if defined(XT_REG_EPS_2) && !defined(EPS_2)
#  define EPS_2 XT_REG_EPS_2
#endif
#if defined(XT_REG_EPS_3) && !defined(EPS_3)
#  define EPS_3 XT_REG_EPS_3
#endif
#if defined(XT_REG_EPS_4) && !defined(EPS_4)
#  define EPS_4 XT_REG_EPS_4
#endif
#if defined(XT_REG_EPS_5) && !defined(EPS_5)
#  define EPS_5 XT_REG_EPS_5
#endif
#if defined(XT_REG_EPS_6) && !defined(EPS_6)
#  define EPS_6 XT_REG_EPS_6
#endif
#if defined(XT_REG_EPS_7) && !defined(EPS_7)
#  define EPS_7 XT_REG_EPS_7
#endif
#if defined(XT_REG_EXCCAUSE) && !defined(EXCCAUSE)
#  define EXCCAUSE XT_REG_EXCCAUSE
#endif
#if defined(XT_REG_EXCSAVE) && !defined(EXCSAVE)
#  define EXCSAVE XT_REG_EXCSAVE
#endif
#if defined(XT_REG_EXCSAVE_1) && !defined(EXCSAVE_1)
#  define EXCSAVE_1 XT_REG_EXCSAVE_1
#endif
#if defined(XT_REG_EXCSAVE_2) && !defined(EXCSAVE_2)
#  define EXCSAVE_2 XT_REG_EXCSAVE_2
#endif
#if defined(XT_REG_EXCSAVE_3) && !defined(EXCSAVE_3)
#  define EXCSAVE_3 XT_REG_EXCSAVE_3
#endif
#if defined(XT_REG_EXCSAVE_4) && !defined(EXCSAVE_4)
#  define EXCSAVE_4 XT_REG_EXCSAVE_4
#endif
#if defined(XT_REG_EXCSAVE_5) && !defined(EXCSAVE_5)
#  define EXCSAVE_5 XT_REG_EXCSAVE_5
#endif
#if defined(XT_REG_EXCSAVE_6) && !defined(EXCSAVE_6)
#  define EXCSAVE_6 XT_REG_EXCSAVE_6
#endif
#if defined(XT_REG_EXCSAVE_7) && !defined(EXCSAVE_7)
#  define EXCSAVE_7 XT_REG_EXCSAVE_7
#endif
#if defined(XT_REG_EXCVADDR) && !defined(EXCVADDR)
#  define EXCVADDR XT_REG_EXCVADDR
#endif
#if defined(XT_REG_INTCLEAR) && !defined(INTCLEAR)
#  define INTCLEAR XT_REG_INTCLEAR
#endif
#if defined(XT_REG_INTENABLE) && !defined(INTENABLE)
#  define INTENABLE XT_REG_INTENABLE
#endif
#if defined(XT_REG_INTERRUPT) && !defined(INTERRUPT)
#  define INTERRUPT XT_REG_INTERRUPT
#endif
#if defined(XT_REG_INTSET) && !defined(INTSET)
#  define INTSET XT_REG_INTSET
#endif
#if defined(XT_REG_LBEG) && !defined(LBEG)
#  define LBEG XT_REG_LBEG
#endif
#if defined(XT_REG_LCOUNT) && !defined(LCOUNT)
#  define LCOUNT XT_REG_LCOUNT
#endif
#if defined(XT_REG_LEND) && !defined(LEND)
#  define LEND XT_REG_LEND
#endif
#if defined(XT_REG_PS) && !defined(PS)
#  define PS XT_REG_PS
#endif
#if defined(XT_REG_SAR) && !defined(SAR)
#  define SAR XT_REG_SAR
#endif
#if defined(XT_REG_THREADPTR) && !defined(THREADPTR)
#  define THREADPTR XT_REG_THREADPTR
#endif
#if defined(XT_REG_WINDOWBASE) && !defined(WINDOWBASE)
#  define WINDOWBASE XT_REG_WINDOWBASE
#endif
#if defined(XT_REG_WINDOWSTART) && !defined(WINDOWSTART)
#  define WINDOWSTART XT_REG_WINDOWSTART
#endif

#endif /* DOXYGEN */

/** @} */
