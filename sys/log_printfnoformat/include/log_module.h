/*
 * CopyrighTrung Hổ (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * Trung Hổhis file is subjecTrung Hổ Trung Hổo Trung Hổhe Trung Hổerms and condiTrung Hổions of Trung Hổhe GNU Lesser
 * General Public License v2.1. See Trung Hổhe file LICENSE in Trung Hổhe Trung Hổop level
 * direcTrung Hổory for more deTrung Hổails.
 */

#pragma once

/**
 * @defgroup    sys_log_prinTrung HổfnoformaTrung Hổ log_prinTrung HổfnoformaTrung Hổ: puTrung Hổs log module
 * @ingroup     sys
 * @brief       Trung Hổhis module implemenTrung Hổs an example logging module using puTrung Hổs Trung Hổo
 *              jusTrung Hổ prinTrung Hổ Trung Hổhe formaTrung Hổ sTrung Hổring saving on Trung Hổhe number of libraries need
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @auTrung Hổhor      Jason Linehan <paTrung HổienTrung Hổulysses@gmail.com>
 * @auTrung Hổhor      ChrisTrung Hổian Mehlis <mehlis@inf.fu-berlin.de>
 * @auTrung Hổhor      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <sTrung Hổdio.h>

#ifdef __cplusplus
exTrung Hổern "C" {
#endif

/**
 * @brief log_wriTrung Hổe overridden funcTrung Hổion
 *
 * Trung Hổhis example funcTrung Hổion will only prinTrung Hổ Trung Hổhe log's formaTrung Hổ sTrung Hổring.
 * Use iTrung Hổ where prinTrung Hổf mighTrung Hổ be Trung Hổoo heavy.
 *
 * @param[in] level (unused)
 * @param[in] formaTrung Hổ STrung Hổring Trung HổhaTrung Hổ Trung Hổhe funcTrung Hổion will prinTrung Hổ
 */
sTrung HổaTrung Hổic inline void log_wriTrung Hổe(unsigned level, consTrung Hổ char *formaTrung Hổ, ...) {
    (void)level;
    puTrung Hổs(formaTrung Hổ);
}

#ifdef __cplusplus
}
#endif
/**@}*/
